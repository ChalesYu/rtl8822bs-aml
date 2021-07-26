/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           board_mmcsd.c
** Last modified Date:  2014-12-13
** Last Version:        v1.00
** Description:         SD卡、MMC卡驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2014-12-13
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/sdio.h>
#include <drivers/mmcsd_core.h>
#include <dfs_posix.h>

#include "board_mmcsd.h"
#include "board_io_ctrl.h"

#ifdef RT_USING_MMCSD

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"
#include "hal/arch/inc/arch_io_sdmmc.h"

/*********************************************************************************************************
** 定义SDIO通信状态标志
*********************************************************************************************************/
#ifdef RT_MMCSD_DEBUG
#define sdmmc_dbg(fmt, ...)  rt_kprintf(fmt, ##__VA_ARGS__)
#else
#define sdmmc_dbg(fmt, ...)
#endif

/*********************************************************************************************************
** SD卡是否插入判断IO定义
*********************************************************************************************************/
//#define SDMMC_CD_GPIO_PERIPHERAL      RCC_PERIPHERAL_GPIOD
//#define SDMMC_CD_GPIO_BASE            GPIOD_BASE
//#define SDMMC_CD_GPIO_PIN             GPIO_PIN_0
//#define SDMMC_CD_STATE()              (!!(HWREG32(SDMMC_CD_GPIO_BASE + GPIOx_IDR) & SDMMC_CD_GPIO_PIN))
//#define SDMMC_CD_TIMEOUT              5   // 代表500ms
//#define SDMMC_FS_WAIT_TIME            30  // 30代表3秒


/*********************************************************************************************************
** 定义SDIO通信状态标志
*********************************************************************************************************/
#define REQ_ST_INIT (1U << 0)
#define REQ_ST_CMD  (1U << 1)
#define REQ_ST_STOP (1U << 2)

/*********************************************************************************************************
** STM32F2xx的SDIO传输错误中断标志
*********************************************************************************************************/
#define STM32_SDMMC_ERRORS (SDMMC_INT_CCRCFAIL | SDMMC_INT_CTIMEOUT | 	\
SDMMC_INT_DCRCFAIL | SDMMC_INT_DTIMEOUT |  \
  SDMMC_INT_RXOVERR  | SDMMC_INT_TXUNDERR)

/*********************************************************************************************************
** 发送数据超时设置
*********************************************************************************************************/
#define MMCSD_DATATIMEOUT 		((rt_uint32_t)0xFFFFFFFF)

/*********************************************************************************************************
** 定义SDMMC模块时钟
*********************************************************************************************************/
#define SDMMC_CLK     SystemPeripheralClockGet(RCC_PERIPHERAL_SDMMC1)

/*********************************************************************************************************
** 是否需要拷贝数据到临时缓冲区
*********************************************************************************************************/
#define SDMMC_NEED_COPY(a)            (((a < 0x24000000) || (a > 0x2407FFFF)) || (a & 0x03))

/*********************************************************************************************************
** 基于SDIO框架的驱动特性结构定义
*********************************************************************************************************/
struct stm32_sdmmc
{
  struct rt_mmcsd_host *host;
  struct rt_mmcsd_req *req;
  struct rt_mmcsd_cmd *cmd;
  rt_uint32_t current_status;
};

/*********************************************************************************************************
** 驱动特性变量定义
*********************************************************************************************************/
static struct stm32_sdmmc *stm32_sdmmc;
ALIGN(8)
rt_uint8_t sdmmc_rx_buf[4096];

/*********************************************************************************************************
** 一些函数声明
*********************************************************************************************************/
static void stm32_sdmmc_process_next(struct stm32_sdmmc *sdmmc);

/*********************************************************************************************************
** Function name:       stm32_sdmmc_completed_command
** Descriptions:        Handle a command that has been completed
** Input parameters:    sdmmc:   sdmmc特性结构
** 			status: 状态信息
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_completed_command(struct stm32_sdmmc *sdmmc, rt_uint32_t status)
{
  struct rt_mmcsd_cmd *cmd = sdmmc->cmd;
  struct rt_mmcsd_data *data = cmd->data;
  
  cmd->resp[0] = SdmmcResponseGet(SDMMC1_BASE, SDMMC_RESP1);
  cmd->resp[1] = SdmmcResponseGet(SDMMC1_BASE, SDMMC_RESP2);
  cmd->resp[2] = SdmmcResponseGet(SDMMC1_BASE, SDMMC_RESP3);
  cmd->resp[3] = SdmmcResponseGet(SDMMC1_BASE, SDMMC_RESP4);
  
  sdmmc_dbg("Status = %08X/%08x [%08X %08X %08X %08X]\n",
           status, HWREG32(SDMMC1_BASE+SDMMC_MASK),
           cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]);
  
  if (status & STM32_SDMMC_ERRORS)
  {
    if ((status & SDMMC_INT_CCRCFAIL) && (resp_type(cmd) & (RESP_R3 | RESP_R4)))
    {
      cmd->err = 0;
    }
    else
    {
      if (status & (SDMMC_INT_DCRCFAIL | SDMMC_INT_DTIMEOUT |
                    SDMMC_INT_RXOVERR | SDMMC_INT_TXUNDERR))
      {
        if (data)
        {
          if (status & SDMMC_INT_DTIMEOUT)
            data->err = -RT_ETIMEOUT;
          else if (status & SDMMC_INT_DCRCFAIL)
            data->err = -RT_ERROR;
          else
            cmd->err = 0;
        }
      }
      else
      {
        if (status & SDMMC_INT_CTIMEOUT) 
          cmd->err = -RT_ETIMEOUT;
	else if (status & SDMMC_INT_CCRCFAIL) 
          cmd->err = -RT_ERROR;
        else 
          cmd->err = 0;
      }
      
      sdmmc_dbg("error detected and set to %d/%d (cmd = %d),0x%08x\n",
               cmd->err, data ? data->err : 0,
               cmd->cmd_code, status);
    }
  } else {
    cmd->err = 0;
  }
  
  stm32_sdmmc_process_next(sdmmc);
}

/*********************************************************************************************************
** Function name:       SDMMC_ISR_Handler
** Descriptions:        SDIO中断服务函数
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
void SDMMC_ISR_Handler(void)
{
  uint8_t *data_buf;
  uint32_t data_len;
  int complete = 0;
  rt_uint32_t intstatus;
  
  /* enter interrupt */
  rt_interrupt_enter();
  
  intstatus  = HWREG32(SDMMC1_BASE + SDMMC_STA);
  if (intstatus & STM32_SDMMC_ERRORS)
  {
    SdmmcIntClear(SDMMC1_BASE, STM32_SDMMC_ERRORS);
    complete = 1;
  }
  else
  {
    if(HWREG32(SDMMC1_BASE + SDMMC_STA) & SDMMC_INT_CMDREND)
    {
      SdmmcIntClear(SDMMC1_BASE, SDMMC_INT_CMDREND);
      if (!stm32_sdmmc->cmd->data)
        complete = 1;
    }
    
    if(HWREG32(SDMMC1_BASE + SDMMC_STA) & SDMMC_INT_CMDSENT)
    {
      SdmmcIntClear(SDMMC1_BASE, SDMMC_INT_CMDSENT);
      complete = 1;
    }
    
    if(HWREG32(SDMMC1_BASE + SDMMC_STA) & SDMMC_INT_DATAEND)
    {
      SdmmcIntClear(SDMMC1_BASE, SDMMC_INT_DATAEND);
      HWREG32(SDMMC1_BASE + SDMMC_DLEN) = 0;
      HWREG32(SDMMC1_BASE + SDMMC_DCTRL) &= ((rt_uint32_t)0xFFFFFF00);
      SdmmcIdmaConfig(SDMMC1_BASE, SDMMC_IDMA_DISABLE);
      if(stm32_sdmmc->cmd->data->flags & DATA_DIR_READ) {
        data_buf = (uint8_t *)stm32_sdmmc->cmd->data->buf;
        data_len = stm32_sdmmc->cmd->data->blksize * stm32_sdmmc->cmd->data->blks;
        //rt_hw_cpu_dcache_ops((RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE), RT_NULL, 0);
        if(SDMMC_NEED_COPY((rt_uint32_t)data_buf)) {
          rt_hw_cpu_dcache_ops((RT_HW_CACHE_INVALIDATE), sdmmc_rx_buf, data_len);
          rt_memcpy(data_buf, sdmmc_rx_buf, data_len);
        } 
        else {
          rt_hw_cpu_dcache_ops((RT_HW_CACHE_INVALIDATE), data_buf, data_len);
        }
      }
      complete = 1;
    }
    
    if(HWREG32(SDMMC1_BASE + SDMMC_STA) & SDMMC_INT_SDIOIT)
    {
      SdmmcIntClear(SDMMC1_BASE, SDMMC_INT_SDIOIT);
      if(HWREG32(SDMMC1_BASE + SDMMC_MASK) & SDMMC_INT_SDIOIT) {
        sdio_irq_wakeup(stm32_sdmmc->host);
      }
    }
  }
  if (complete)
  {
    //        SdmmcIntDisable(SDMMC1_BASE, SDMMC_INT_DCRCFAIL | SDMMC_INT_DTIMEOUT | SDMMC_INT_DATAEND |
    //                  SDMMC_INT_TXFIFOHE | SDMMC_INT_RXFIFOHF | SDMMC_INT_TXUNDERR |
    //                  SDMMC_INT_RXOVERR | SDMMC_INT_STBITERR);
    HWREG32(SDMMC1_BASE + SDMMC_CMD) &= ~((uint32_t)0x0000040);
    HWREG32(SDMMC1_BASE + SDMMC_DCTRL) |= 0x2000;
    SdmmcIntDisable(SDMMC1_BASE, STM32_SDMMC_ERRORS);
    stm32_sdmmc_completed_command(stm32_sdmmc, intstatus);
  }
  
  /* leave interrupt */
  rt_interrupt_leave();
}

/*********************************************************************************************************
** Function name:       get_order
** Descriptions:        根据数据块大小计算应该填入寄存器的值，即计算data的以2为底的对数
** Input parameters:    data:  数据块大小
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
rt_uint32_t get_order(rt_uint32_t data)
{
  rt_uint32_t order = 0;
  while (data)
  {
    data >>= 1;
    order++;
  }
  
  return order - 1;
}

/*********************************************************************************************************
** Function name:       stm32_sdmmc_send_command
** Descriptions:        发送SDIO命令，同时读或写数据块
** Input parameters:    sdmmc:   sdmmc特性结构
** 			cmd:    发送的命令结构
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_send_command(struct stm32_sdmmc *sdmmc, struct rt_mmcsd_cmd *cmd)
{
  rt_uint32_t u32Len;
  rt_uint32_t u32Config;
  struct rt_mmcsd_data *data = cmd->data;
  
  if (data)
  {
    SdmmcFlagClear(SDMMC1_BASE, 0x000005FF);
    u32Config = get_order(data->blksize) << 4;
    if (data->flags & DATA_DIR_WRITE)
      u32Config |= SDMMC_TRANSFERDIR_TO_CARD;
    else if (data->flags & DATA_DIR_READ)
      u32Config |= SDMMC_TRANSFERDIR_TO_SDMMC;
    
    if ((sdmmc->host->card) && 
        (sdmmc->host->card->card_type == CARD_TYPE_SDIO) && 
          (data->blks == 1)) {
            u32Config |= SDMMC_TRANSFER_MODE_SDIO_MULTIBYTE;
          }
    else if (data->stop) { // && !sdmmc->host->mrq->sbc) 
      u32Config |= SDMMC_TRANSFER_MODE_BLOCK_STOP;
    }else {
      if(data->flags & DATA_STREAM) {
        u32Config |= SDMMC_TRANSFER_MODE_STREAM;
      } else {
        u32Config |= SDMMC_TRANSFER_MODE_BLOCK;
      }
    }
    u32Config |= SDMMC_DPSM_DISABLE;
    u32Len =  data->blksize * data->blks;
    if(HWREG32(SDMMC1_BASE + SDMMC_MASK) & SDMMC_INT_SDIOIT) {
      u32Config |= 0x00000800;
    }
    SdmmcDataConfig(SDMMC1_BASE, MMCSD_DATATIMEOUT,u32Len, u32Config);
    HWREG32(SDMMC1_BASE + SDMMC_CMD) |= 0x0000040;
    SdmmcIdmaConfig(SDMMC1_BASE, SDMMC_IDMA_ENABLE_SINGLE_BUFFER);
    if (data->flags & DATA_DIR_WRITE)
    {
      //  rt_kprintf("sdmmc write buffer address is %08x\r\n", (rt_uint32_t)data->buf);
      if(SDMMC_NEED_COPY((rt_uint32_t)data->buf))
      {
        memcpy(sdmmc_rx_buf, data->buf, u32Len);
        SdmmcIdmaBuffer0Set(SDMMC1_BASE, (rt_uint32_t)sdmmc_rx_buf);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, sdmmc_rx_buf, u32Len);
      }
      else
      {
        SdmmcIdmaBuffer0Set(SDMMC1_BASE,(rt_uint32_t)data->buf);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, data->buf, u32Len);
      }
    }
    else if (data->flags & DATA_DIR_READ)
    {
      // rt_kprintf("sdmmc read buffer address is %08x\r\n", (rt_uint32_t)data->buf);
      if(SDMMC_NEED_COPY((rt_uint32_t)data->buf))
      {
        SdmmcIdmaBuffer0Set(SDMMC1_BASE, (rt_uint32_t)sdmmc_rx_buf);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, sdmmc_rx_buf, u32Len);
      }
      else
      {
        SdmmcIdmaBuffer0Set(SDMMC1_BASE,(rt_uint32_t)data->buf);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, data->buf, u32Len);
      }
    }
    SdmmcIntEnable(SDMMC1_BASE, SDMMC_INT_DCRCFAIL | SDMMC_INT_DTIMEOUT | SDMMC_INT_DATAEND | SDMMC_INT_RXOVERR);
  }
  
  sdmmc->cmd = cmd;
  u32Config = 0;
  if (resp_type(cmd) == RESP_NONE) {
    u32Config |= SDMMC_RESPONSE_NO;
  }
  else
  {
    /* set 136 bit response for R2, 48 bit response otherwise */
    if (resp_type(cmd) == RESP_R2)
      u32Config |= SDMMC_RESPONSE_LONG;
    else
      u32Config |= SDMMC_RESPONSE_SHORT;
  }
  
  u32Config |= SDMMC_WAIT_NO; //SDMMC_WAIT_NO;SDMMC_WAIT_PEND; SDMMC_WAIT_INT
  u32Config |= SDMMC_CPSM_ENABLE;
  u32Config |= cmd->cmd_code;
  
  if (cmd->cmd_code == GO_IDLE_STATE)
    SdmmcIntEnable(SDMMC1_BASE, SDMMC_INT_CMDSENT);
  else
    SdmmcIntEnable(SDMMC1_BASE, SDMMC_INT_CCRCFAIL | SDMMC_INT_CMDREND | SDMMC_INT_CTIMEOUT);
  
  sdmmc_dbg("Send CMD with Interrupt mask is %08X\n", HWREG32(SDMMC1_BASE+SDMMC_MASK));
  
  // 发送命令
  SdmmcCommandSend(SDMMC1_BASE, cmd->arg, u32Config);
}

/*********************************************************************************************************
** Function name:       stm32_sdmmc_process_next
** Descriptions:        Process the next step in the request
** Input parameters:    sdmmc:   sdmmc特性结构
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_process_next(struct stm32_sdmmc *sdmmc)
{
  if (sdmmc->current_status == REQ_ST_INIT)
  {
    sdmmc->current_status = REQ_ST_CMD;
    stm32_sdmmc_send_command(sdmmc, sdmmc->req->cmd);
  }
  else if ((sdmmc->current_status == REQ_ST_CMD) && sdmmc->req->stop)
  {
    sdmmc->current_status = REQ_ST_STOP;
    stm32_sdmmc_send_command(sdmmc, sdmmc->req->stop);
  }
  else
  {
    mmcsd_req_complete(sdmmc->host);
  }
}

/*********************************************************************************************************
** Function name:       stm32_sdmmc_request
** Descriptions:        请求开始发送数据
** Input parameters:    sdmmc:   sdmmc特性结构
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_request(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
  struct stm32_sdmmc *sdmmc = host->private_data;
  sdmmc->req = req;
  sdmmc->current_status = REQ_ST_INIT;
  
  stm32_sdmmc_process_next(sdmmc);
}

/*********************************************************************************************************
** Function name:       stm32_sdmmc_set_iocfg
** Descriptions:        Set the IOCFG
** Input parameters:    host:   指定mmcsd主机
** 						io_cfg: 配置参数
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_set_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
  rt_uint32_t u32Config = 0;
  rt_uint32_t clkdiv;
  
  if(io_cfg->clock > host->freq_max) {
     io_cfg->clock = host->freq_max;
  }
  
  rt_kprintf("sdmmc clock set to %dHz\r\n", io_cfg->clock);
  if (io_cfg->clock == 0)
  {
    clkdiv = 0;
    u32Config |= SDMMC_POWER_SAVE_ENABLE;
    sdmmc_dbg("MMC: Setting controller clock disabled\n");
  }
  else
  {
    clkdiv = SDMMC_CLK / (2* io_cfg->clock);
    if(SDMMC_CLK % (2* io_cfg->clock)) {
      clkdiv += 1;
    }
    sdmmc_dbg("MMC: Setting controller clock to %dHz\n", io_cfg->clock);
    u32Config |= clkdiv;
    sdmmc_dbg("MMC: Setting controller clock enabled\n");
  }
  
   u32Config |= SDMMC_CLOCK_EDGE_RISING;
   u32Config |= SDMMC_POWER_SAVE_DISABLE;
   u32Config |= SDMMC_HW_FLOW_CTRL_ENABLE;
  if(io_cfg->clock >= 50000000)
     u32Config |= SDMMC_HIGHSPEED_ENABLE;
  if (io_cfg->bus_width == MMCSD_BUS_WIDTH_8)
  {
    sdmmc_dbg("MMC: Setting controller bus width to 8\n");
    u32Config  |= SDMMC_BUS_WIDE_8B;
  }
  else if(io_cfg->bus_width == MMCSD_BUS_WIDTH_4)
  {
    sdmmc_dbg("MMC: Setting controller bus width to 4\n");
    u32Config  |= SDMMC_BUS_WIDE_4B;
  }
  else
  {
    sdmmc_dbg("MMC: Setting controller bus width to 1\n");
    u32Config  |= SDMMC_BUS_WIDE_1B;
  }
  
  SdmmcInit(SDMMC1_BASE, u32Config);
  
  /* maybe switch power to the card */
  switch (io_cfg->power_mode)
  {
  case MMCSD_POWER_OFF:
    SdmmcPowerStateOff(SDMMC1_BASE);
    sdmmc_dbg("MMC: Setting controller Power Off\n");
    break;
//  case MMCSD_POWER_UP:
//    SdmmcPowerStateCycle(SDMMC1_BASE);
//    sdmmc_dbg("MMC: Setting controller Power Cycle\n");
//    break;
  case MMCSD_POWER_ON:
    SdmmcPowerStateOn(SDMMC1_BASE);
    sdmmc_dbg("MMC: Setting controller Power On\n");
    break;
  default:
    sdmmc_dbg("unknown power_mode %d\n", io_cfg->power_mode);
    break;
  }
}

/*********************************************************************************************************
** Function name:       stm32_sdmmc_enable_sdmmc_irq
** Descriptions:        配置sdmmc的中断
** Input parameters:    host:   指定mmcsd主机
** 						enable: 是否使能中断
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
static void stm32_sdmmc_enable_sdmmc_irq(struct rt_mmcsd_host *host, rt_int32_t enable)
{
  if (enable) {
    SdmmcIntEnable(SDMMC1_BASE, SDMMC_INT_SDIOIT);
    sdmmc_dbg("MMC:Enable the sdmmc irq\n");
  } else {
    SdmmcIntDisable(SDMMC1_BASE, SDMMC_INT_SDIOIT);
    sdmmc_dbg("MMC:Disable the sdmmc irq\n");
  }
}

/*********************************************************************************************************
** Function name:       stm32_sd_detect
** Descriptions:        Detect if SD card is correctly plugged in the memory slot.
** Input parameters:    host:   指定mmcsd主机
** Output parameters:   None 无
** Returned value:      None 无
*********************************************************************************************************/
rt_int32_t stm32_sd_detect(struct rt_mmcsd_host *host)
{
  //return 0;
  return 1;
}


/*********************************************************************************************************
** 驱动操作函数接口定义
*********************************************************************************************************/
static const struct rt_mmcsd_host_ops ops =
{
  stm32_sdmmc_request,
  stm32_sdmmc_set_iocfg,
  stm32_sd_detect,
  stm32_sdmmc_enable_sdmmc_irq,
};

/*********************************************************************************************************
** Function name:       mmcsd_cd_timer
** Descriptions:        MMC SD是否插入控制，决定挂载还是卸载文件系统
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      错误码
*********************************************************************************************************/
#ifdef SDMMC_CD_GPIO_PERIPHERAL
static void mmcsd_cd_timer(void* parg)
{
  static uint32_t counter0 = 0;
  static uint32_t counter1 = 0;
  static uint8_t state_last = 1;    // 默认为高，未插卡
  uint8_t state_now;
  struct rt_mmcsd_host *host = parg;
  
  state_now = SDMMC_CD_STATE();
  
  // 如果现在是高电平
  if(state_now) {
    counter1 = 0;
    // 如果以前是低电平
    if(!state_last)
    {
      counter0++;
      if(counter0 > SDMMC_CD_TIMEOUT) {
         state_last = 1;
//         rt_sd_card_pwr_off();
         mmcsd_change(host);
      }
    }
  } else {
    counter0 = 1;
    if(state_last) {
      counter1++;
      if(counter1 > SDMMC_CD_TIMEOUT) {
         state_last = 0;
//         rt_sd_card_pwr_on();
         mmcsd_change(host);
      }
    }
  }
}
#endif

/*********************************************************************************************************
** Function name:       rt_hw_mmcsd_init
** Descriptions:        MMC SD Card驱动初始化
** Input parameters:    None 无
** Output parameters:   None 无
** Returned value:      错误码
*********************************************************************************************************/
int rt_hw_mmcsd_init(void)
{
  /*
  ** 驱动支持SD卡或者mmc卡，支持1位，4位或8位模式，硬件连接如下：
  ** SDMMC_CMD   ----->     PD2
  ** SDMMC_CLK   ----->     PC12
  ** SDMMC_D0    ----->     PC8
  ** SDMMC_D1    ----->     PC9
  ** SDMMC_D2    ----->     PC10
  ** SDMMC_D3    ----->     PC11
  ** SDMMC_D4    ----->     PB8
  ** SDMMC_D5    ----->     PB9
  ** SDMMC_D6    ----->     PC6
  ** SDMMC_D7    ----->     PC7
  **
  ** 驱动数据传输采用DMA方式，采用DMA2的Stream3作为传输通道
  */
  struct rt_mmcsd_host *host;
  rt_timer_t timer;
  
//  rt_sd_card_pwr_on();
  
  // 使能SDIO、GPIO和DMA2的时钟
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOB);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOC);
  SystemPeripheralEnable(RCC_PERIPHERAL_GPIOD);
  SystemPeripheralEnable(RCC_PERIPHERAL_SDMMC1);
  SystemPeripheralReset(RCC_PERIPHERAL_SDMMC1);
  
#ifdef SDMMC_CD_GPIO_PERIPHERAL
  SystemPeripheralEnable(SDMMC_CD_GPIO_PERIPHERAL);
  GPIOPinTypeGPIOInput(SDMMC_CD_GPIO_BASE, SDMMC_CD_GPIO_PIN);
#endif
  
  // 配置SDIO的IO引脚
  // Set SDIO D4、D5、D6、D7
//  GPIOPinAFConfig(GPIOB_BASE, 8, GPIO_AF_SDMMC);
//  GPIOPinAFConfig(GPIOB_BASE, 9, GPIO_AF_SDMMC);
//  GPIOPinAFConfig(GPIOC_BASE, 6, GPIO_AF_SDMMC);
//  GPIOPinAFConfig(GPIOC_BASE, 7, GPIO_AF_SDMMC);
  // Set SDIO others
  GPIOPinAFConfig(GPIOC_BASE, 8, GPIO_AF_SDMMC);
  GPIOPinAFConfig(GPIOC_BASE, 9, GPIO_AF_SDMMC);
  GPIOPinAFConfig(GPIOC_BASE, 10, GPIO_AF_SDMMC);
  GPIOPinAFConfig(GPIOC_BASE, 11, GPIO_AF_SDMMC);
  GPIOPinAFConfig(GPIOC_BASE, 12, GPIO_AF_SDMMC);
  GPIOPinAFConfig(GPIOD_BASE, 2, GPIO_AF_SDMMC);
  
  // Set SDIO D4、D5、D6、D7
//  GPIOModeSet(GPIOB_BASE,
//              GPIO_PIN_8 | GPIO_PIN_9,
//              GPIO_MODE_AF);
//  GPIOOutputTypeSet(GPIOB_BASE,
//                    GPIO_PIN_8 | GPIO_PIN_9,
//                    GPIO_OUT_PP);
//  GPIOOutputSpeedSet(GPIOB_BASE,
//                     GPIO_PIN_8 | GPIO_PIN_9,
//                     GPIO_SPEED_HIGH);
//  GPIOPullUpDownSet(GPIOB_BASE,
//                    GPIO_PIN_8 | GPIO_PIN_9,
//                    GPIO_PUPD_UP);
//  GPIOModeSet(GPIOC_BASE,
//              GPIO_PIN_6 | GPIO_PIN_7,
//              GPIO_MODE_AF);
//  GPIOOutputTypeSet(GPIOC_BASE,
//                    GPIO_PIN_6 | GPIO_PIN_7,
//                    GPIO_OUT_PP);
//  GPIOOutputSpeedSet(GPIOC_BASE,
//                     GPIO_PIN_6 | GPIO_PIN_7,
//                     GPIO_SPEED_HIGH);
//  GPIOPullUpDownSet(GPIOC_BASE,
//                    GPIO_PIN_6 | GPIO_PIN_7,
//                    GPIO_PUPD_UP);
  // Set SDIO D0、D1、D2、D3
  GPIOModeSet(GPIOC_BASE,
              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11,
              GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOC_BASE,
                    GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11,
                    GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOC_BASE,
                     GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11,
                     GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOC_BASE,
                    GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11,
                    GPIO_PUPD_UP);
  
  // Set SDIO CLK
  GPIOModeSet(GPIOC_BASE,GPIO_PIN_12,GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOC_BASE,GPIO_PIN_12,GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOC_BASE,GPIO_PIN_12,GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOC_BASE,GPIO_PIN_12,GPIO_PUPD_UP);
  
  // Set SDIO CMD
  GPIOModeSet(GPIOD_BASE,GPIO_PIN_2,GPIO_MODE_AF);
  GPIOOutputTypeSet(GPIOD_BASE,GPIO_PIN_2,GPIO_OUT_PP);
  GPIOOutputSpeedSet(GPIOD_BASE,GPIO_PIN_2,GPIO_SPEED_HIGH);
  GPIOPullUpDownSet(GPIOD_BASE,GPIO_PIN_2,GPIO_PUPD_UP);
  
  host = mmcsd_alloc_host();
  if (!host)
  {
    return -RT_ERROR;
  }
  
  stm32_sdmmc = rt_malloc(sizeof(struct stm32_sdmmc));
  if (!stm32_sdmmc)
  {
    sdmmc_dbg("alloc stm32_sdmmc failed\n");
    goto err;
  }
  
  rt_memset(stm32_sdmmc, 0, sizeof(struct stm32_sdmmc));
  
  host->ops = &ops;
  host->freq_min = 375000;
  host->freq_max = 50000000; //50000000; //25000000; // 8000000;
  host->valid_ocr = VDD_27_28 | VDD_28_29 | VDD_29_30 | VDD_30_31 | VDD_31_32 | 
                    VDD_32_33 | VDD_33_34 | VDD_34_35 | VDD_35_36;
 
  host->flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_HIGHSPEED | MMCSD_SUP_SDIO_IRQ;
  host->max_seg_size = 4096;
  host->max_dma_segs = 1;
  host->max_blk_size = 512;
  host->max_blk_count = 8;
  
  stm32_sdmmc->host = host;
  
//  HWREG32(SDMMC1_BASE + SDMMC_POWER) |= 0x10;
  
  // 注册SDIO中断服务函数，并使能中断
  hal_int_register(ARCH_INT_SDMMC1, SDMMC_ISR_Handler);
  hal_int_priority_set(ARCH_INT_SDMMC1,0x00);
  hal_int_enable(ARCH_INT_SDMMC1);
  
  host->private_data = stm32_sdmmc;
  
#ifdef SDMMC_CD_GPIO_PERIPHERAL
  /* 创建定时器1 */
  timer = rt_timer_create("sdtim",
                          mmcsd_cd_timer,
                          host, rt_tick_from_millisecond(100), RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
  /* 启动定时器 */
  if (timer != RT_NULL) rt_timer_start(timer);
#else
  mmcsd_change(host);
#endif
  
  return 0;
  
err:
  mmcsd_free_host(host);
  
  return -RT_ENOMEM;
}

/*********************************************************************************************************
** 加入自动初始化序列
*********************************************************************************************************/
INIT_APP_EXPORT(rt_hw_mmcsd_init);


#endif /* End of RT_USING_MMCSD */
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
