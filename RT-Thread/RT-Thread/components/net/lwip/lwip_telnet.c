/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           lwip_telnet.c
** Last modified Date:  2015-06-23
** Last Version:        v1.00
** Description:         lwip的telnet服务器实现，采用telnet用作finish的shell接口
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2015-06-23
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <rtthread.h>
#include <lwip/sockets.h>

#include <finsh.h>
#include <shell.h>

#define TELNET_PORT         23
#define TELNET_RX_BUFFER    256
#define TELNET_TX_BUFFER    4096

#define ISO_nl              0x0a
#define ISO_cr              0x0d

#define STATE_NORMAL        0
#define STATE_IAC           1
#define STATE_WILL          2
#define STATE_WONT          3
#define STATE_DO            4
#define STATE_DONT          5
#define STATE_CLOSE         6

#define TELNET_IAC          255
#define TELNET_WILL         251
#define TELNET_WONT         252
#define TELNET_DO           253
#define TELNET_DONT         254

struct rb
{
    rt_uint16_t read_index, write_index;
    rt_uint8_t *buffer_ptr;
    rt_uint16_t buffer_size;
};

struct telnet_session
{
    struct rb rx_ringbuffer;
    struct rb tx_ringbuffer;

    rt_sem_t rx_ringbuffer_lock;
    rt_sem_t tx_ringbuffer_lock;

    struct rt_device device;

    int  socket;

    /* telnet protocol */
    rt_uint8_t state;
    rt_uint8_t echo_mode;

};
struct telnet_session* telnet;

/* 一个环形buffer的实现 */
/* 初始化环形buffer，size指的是buffer的大小。注：这里并没对数据地址对齐做处理 */
static void rb_init(struct rb* rb, rt_uint8_t *pool, rt_uint16_t size)
{
    RT_ASSERT(rb != RT_NULL);

    /* 对读写指针清零*/
    rb->read_index = rb->write_index = 0;

    /* 环形buffer的内存数据块 */
    rb->buffer_ptr = pool;
    rb->buffer_size = size;
}

/* 向环形buffer中写入数据 */
static rt_size_t rb_put(struct rb* rb, const rt_uint8_t *ptr, rt_uint16_t length)
{
    rt_size_t size;

    /* 判断是否有足够的剩余空间 */
    if (rb->read_index > rb->write_index)
        size = rb->read_index - rb->write_index;
    else
        size = rb->buffer_size - rb->write_index + rb->read_index;

    /* 没有多余的空间 */
    if (size == 0) return 0;

    /* 数据不够放置完整的数据，截断放入 */
    if (size < length) length = size;

    if (rb->read_index > rb->write_index)
    {
        /* read_index - write_index 即为总的空余空间 */
        memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
        rb->write_index += length;
    }
    else
    {
        if (rb->buffer_size - rb->write_index > length)
        {
            /* write_index 后面剩余的空间有足够的长度 */
            memcpy(&rb->buffer_ptr[rb->write_index], ptr, length);
            rb->write_index += length;
        }
        else
        {
            /*
             * write_index 后面剩余的空间不存在足够的长度，需要把部分数据复制到
             * 前面的剩余空间中
             */
            memcpy(&rb->buffer_ptr[rb->write_index], ptr,
                   rb->buffer_size - rb->write_index);
            memcpy(&rb->buffer_ptr[0], &ptr[rb->buffer_size - rb->write_index],
                   length - (rb->buffer_size - rb->write_index));
            rb->write_index = length - (rb->buffer_size - rb->write_index);
        }
    }

    return length;
}

/* 向环形buffer中写入一个字符 */
static rt_size_t rb_putchar(struct rb* rb, const rt_uint8_t ch)
{
    rt_uint16_t next;

    /* 判断是否有多余的空间 */
    next = rb->write_index + 1;
    if (next >= rb->buffer_size) next = 0;

    if (next == rb->read_index) return 0;

    /* 放入字符 */
    rb->buffer_ptr[rb->write_index] = ch;
    rb->write_index = next;

    return 1;
}

/* 从环形buffer中读出数据 */
static rt_size_t rb_get(struct rb* rb, rt_uint8_t *ptr, rt_uint16_t length)
{
    rt_size_t size;

    /* 判断是否有足够的数据 */
    if (rb->read_index > rb->write_index)
        size = rb->buffer_size - rb->read_index + rb->write_index;
    else
        size = rb->write_index - rb->read_index;

    /* 没有足够的数据 */
    if (size == 0) return 0;

    /* 数据不够指定的长度，取环形buffer中实际的长度 */
    if (size < length) length = size;

    if (rb->read_index > rb->write_index)
    {
        if (rb->buffer_size - rb->read_index > length)
        {
            /* read_index的数据足够多，直接复制 */
            memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
            rb->read_index += length;
        }
        else
        {
            /* read_index的数据不够，需要分段复制 */
            memcpy(ptr, &rb->buffer_ptr[rb->read_index],
                   rb->buffer_size - rb->read_index);
            memcpy(&ptr[rb->buffer_size - rb->read_index], &rb->buffer_ptr[0],
                   length - rb->buffer_size + rb->read_index);
            rb->read_index = length - rb->buffer_size + rb->read_index;
        }
    }
    else
    {
        /*
         * read_index要比write_index小，总的数据量够（前面已经有总数据量的判
         * 断），直接复制出数据。
         */
        memcpy(ptr, &rb->buffer_ptr[rb->read_index], length);
        rb->read_index += length;
    }

    return length;
}

static rt_size_t rb_available(struct rb* rb)
{
    rt_size_t size;

    if (rb->read_index > rb->write_index)
        size = rb->buffer_size - rb->read_index + rb->write_index;
    else
        size = rb->write_index - rb->read_index;

    /* 返回ringbuffer中存在的数据大小 */
    return size;
}

/* RT-Thread Device Driver Interface */
static rt_err_t telnet_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t telnet_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t telnet_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t telnet_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_size_t result;

    /* read from rx ring buffer */
    rt_sem_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
    result = rb_get(&(telnet->rx_ringbuffer), buffer, size);
    rt_sem_release(telnet->rx_ringbuffer_lock);

    return result;
}

static rt_size_t telnet_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    const rt_uint8_t *ptr;

    ptr = (rt_uint8_t*)buffer;

    rt_sem_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
    while (size)
    {
        if (*ptr == '\n')
            rb_putchar(&telnet->tx_ringbuffer, '\r');

        if (rb_putchar(&telnet->tx_ringbuffer, *ptr) == 0)  /* overflow */
            break;
        ptr ++; size --;
    }
    rt_sem_release(telnet->tx_ringbuffer_lock);
    
    return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

static rt_err_t telnet_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    return RT_EOK;
}

/* send telnet option to remote */
static void telnet_send_option(struct telnet_session* telnet, rt_uint8_t option, rt_uint8_t value)
{
    rt_uint8_t optbuf[4];

    optbuf[0] = TELNET_IAC;
    optbuf[1] = option;
    optbuf[2] = value;
    optbuf[3] = 0;

    rt_sem_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
    rb_put(&telnet->tx_ringbuffer, optbuf, 3);
    rt_sem_release(telnet->tx_ringbuffer_lock);
}

/* process tx data */
void telnet_process_tx(struct telnet_session* telnet, int socket)
{
    rt_size_t length;
    rt_uint8_t tx_buffer[32];

    while (1)
    {
        rt_memset(tx_buffer, 0, sizeof(tx_buffer));
        rt_sem_take(telnet->tx_ringbuffer_lock, RT_WAITING_FOREVER);
        /* get buffer from ringbuffer */
        length = rb_get(&(telnet->tx_ringbuffer), tx_buffer, sizeof(tx_buffer));
        rt_sem_release(telnet->tx_ringbuffer_lock);

        /* do a tx procedure */
        if (length > 0)
        {
            send(socket,tx_buffer, length, 0);
        }
        else break;
    }
}

/* process rx data */
void telnet_process_rx(struct telnet_session* telnet, rt_uint8_t *data, rt_size_t length)
{
    rt_size_t rx_length, index;

    for (index = 0; index < length; index ++)
    {
        switch(telnet->state)
        {
        case STATE_IAC:
            if (*data == TELNET_IAC)
            {
                /* take semaphore */
                rt_sem_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
                /* put buffer to ringbuffer */
                rb_putchar(&(telnet->rx_ringbuffer), *data);
                /* release semaphore */
                rt_sem_release(telnet->rx_ringbuffer_lock);

                telnet->state = STATE_NORMAL;
            }
            else
            {
                /* set telnet state according to received package */
                switch (*data)
                {
                case TELNET_WILL: telnet->state = STATE_WILL; break;
                case TELNET_WONT: telnet->state = STATE_WONT; break;
                case TELNET_DO:   telnet->state = STATE_DO; break;
                case TELNET_DONT: telnet->state = STATE_DONT; break;
                default: telnet->state = STATE_NORMAL; break;
                }
            }
            break;
        
        /* don't option */
        case STATE_WILL:
        case STATE_WONT:
            telnet_send_option(telnet, TELNET_DONT, *data);
            telnet->state = STATE_NORMAL;
            break;

        /* won't option */
        case STATE_DO:
        case STATE_DONT:
            telnet_send_option(telnet, TELNET_WONT, *data);
            telnet->state = STATE_NORMAL;
            break;

        case STATE_NORMAL:
            if (*data == TELNET_IAC) telnet->state = STATE_IAC;
            else if (*data != '\r') /* ignore '\r' */
            {
                rt_sem_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
                /* put buffer to ringbuffer */
                rb_putchar(&(telnet->rx_ringbuffer), *data);
                rt_sem_release(telnet->rx_ringbuffer_lock);
            }
            break;
        }

        data ++;
    }

    rt_sem_take(telnet->rx_ringbuffer_lock, RT_WAITING_FOREVER);
    /* get total size */
    rx_length = rb_available(&telnet->rx_ringbuffer);
    rt_sem_release(telnet->rx_ringbuffer_lock);

    /* indicate there are reception data */
    if ((rx_length > 0) && (telnet->device.rx_indicate != RT_NULL))
        telnet->device.rx_indicate(&telnet->device, 
            rx_length);

    return;
}

/* process socket close */
void telnet_process_close(struct telnet_session* telnet, int socket)
{
    /* set console */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
    /* set finsh device */
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);

    /* close connection */
    closesocket(socket);

    /* restore shell option */
    finsh_set_echo(telnet->echo_mode);

    rt_kprintf("resume console to %s\n", RT_CONSOLE_DEVICE_NAME);
}

/* telnet server thread entry */
void telnet_thread(void* parameter)
{
    static rt_uint8_t recv_buf[256];
    rt_uint32_t iCliLen;
    int sAccept, sListen;
    int len;
    int iMode = 1;
    struct sockaddr_in SerAddr,cliAddr;

    // 设置本地IP地址及端口号
    SerAddr.sin_family = AF_INET;
    SerAddr.sin_port = htons(TELNET_PORT);
    SerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Create a new socket */
    sListen = socket(PF_INET,SOCK_STREAM,0);

    /* Bind connection to well known port number 7. */
    bind(sListen,(const struct sockaddr *)&SerAddr,sizeof(SerAddr));

    /* Tell connection to go into listening mode. */
    listen(sListen,1);
    
    /* register telnet device */
    telnet->device.type     = RT_Device_Class_Char;
    telnet->device.init     = telnet_init;
    telnet->device.open     = telnet_open;
    telnet->device.close    = telnet_close;
    telnet->device.read     = telnet_read;
    telnet->device.write    = telnet_write;
    telnet->device.control  = telnet_control;

    /* no private */
    telnet->device.user_data = RT_NULL;

    /* register telnet device */
    rt_device_register(&telnet->device, "telnet",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM);

    while (1)
    {
        rt_kprintf("telnet server waiting for connection\n");

        /* Grab new connection. */
        iCliLen = sizeof(cliAddr);
        sAccept = accept(sListen,(struct sockaddr *)&cliAddr,&iCliLen);

        if(sAccept < 0)
        {
          rt_thread_delay( RT_TICK_PER_SECOND);
          continue;
        }
        
        rt_kprintf("new telnet connection, switch console to telnet...\n");
        
        //  关闭 nagle 算法. 提升效率.
        setsockopt(sAccept, IPPROTO_TCP, TCP_NODELAY, (const void *)&iMode, sizeof(iMode));
        //  服务器保鲜处理
        {
          int     iKeepIdle     = 5;                                  /*  空闲时间 5 秒               */
          int     iKeepInterval = 5;                                  /*  两次探测间的时间间隔 5 秒   */
          int     iKeepCount    = 3;                                  /*  探测 3 次失败认为是掉线     */
          
          setsockopt(sAccept, SOL_SOCKET, SO_KEEPALIVE, (const void *)&iMode, sizeof(iMode));
          /*  启用保鲜定时器              */
          // 设置保鲜定时器参数
          setsockopt(sAccept,IPPROTO_TCP,TCP_KEEPIDLE, (const void *)&iKeepIdle,     sizeof(int));
          setsockopt(sAccept,IPPROTO_TCP,TCP_KEEPINTVL,(const void *)&iKeepInterval, sizeof(int));
          setsockopt(sAccept,IPPROTO_TCP,TCP_KEEPCNT,  (const void *)&iKeepCount,    sizeof(int));
        }

        //ioctlsocket(sAccept,FIONBIO,&iMode);            //非阻塞设置
        
        /* Process the new connection. */
        /* set console */
//        rt_console_set_device("telnet");
//        /* set finsh device */
//        finsh_set_device("telnet");
//
//        /* set init state */
//        telnet->state = STATE_NORMAL;
//
//        telnet->echo_mode = finsh_get_echo();
//        /* disable echo mode */
//        finsh_set_echo(0);

        while (1)
        {
            /* try to send all data in tx ringbuffer */
            //telnet_process_tx(telnet, sAccept);
            
            len = recv(sAccept, recv_buf, 256,0);
            if(0 == len)
            {
              break;
            }
            if(-1 == len)
            {
              if(!(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
              {
                break;
              } else {
                continue;
              }
            }
            send(sAccept, recv_buf, len, 0);
            //telnet_process_rx(telnet, recv_buf, len);
            
            //rt_thread_delay(1);
        }
         closesocket(sAccept);
        //telnet_process_close(telnet, sAccept);
        rt_thread_delay(RT_TICK_PER_SECOND * 5);
    }
}

/* telnet server */
void telnet_srv()
{
    rt_thread_t tid;

    if (telnet == RT_NULL)
    {
        rt_uint8_t *ptr;

        telnet = rt_malloc (sizeof(struct telnet_session));
        if (telnet == RT_NULL)
        {
            rt_kprintf("telnet: no memory\n");
            return;
        }

        /* init ringbuffer */
        ptr = rt_malloc (TELNET_RX_BUFFER);
        rb_init(&telnet->rx_ringbuffer, ptr, TELNET_RX_BUFFER);
        /* create rx ringbuffer lock */
        telnet->rx_ringbuffer_lock = rt_sem_create("rxrb", 1, RT_IPC_FLAG_FIFO);
        ptr = rt_malloc (TELNET_TX_BUFFER);
        rb_init(&telnet->tx_ringbuffer, ptr, TELNET_TX_BUFFER);
        /* create tx ringbuffer lock */
        telnet->tx_ringbuffer_lock = rt_sem_create("txrb", 1, RT_IPC_FLAG_FIFO);
    }

    tid = rt_thread_create("telnet", telnet_thread, RT_NULL,
                           2048, 25, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(telnet_srv, startup telnet server);
#endif
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
