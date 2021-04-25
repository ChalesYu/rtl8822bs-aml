#include <time.h>
#include <Ntifs.h>

#include "wf_os_api.h"
#include "wf_debug.h"
#include "wf_list.h"
#include "wf_timer.h"

#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)

typedef struct wf_api_test_s{
	wf_thread_t thread[2];
	char name[20];
	wf_os_api_timer_t timer[2];
	wf_lock_spin spinlock;
	wf_lock_mutex mutexlock;
	char data_buffer[10];
	wf_list_t list_header;
	wf_sema semphor;
}wf_api_test_t;

typedef struct wf_api_param_s{
	int id;
	char sub_type;
	char name[20];
}wf_api_param_t;

typedef struct wf_api_list_node_s{
	int id;
	int index;
	int type;
	wf_list_t list;
}wf_api_list_node_t;

wf_api_test_t test;

wf_api_param_t test_param_thread1 = {
	0x53,
	0x01,
	"thread1"
};

wf_api_param_t test_param_thread2 = {
	0x53,
	0x02,
	"thread2"
};
	
wf_api_param_t test_param_timer1 = {
	0x53,
	0x03,
	"timer1"
};

wf_api_param_t test_param_timer2 = {
	0x53,
	0x04,
	"timer2"
};

#define EXEC_INFO(param) LOG_D("#################ID[%x] SUB[%x] NAME[%s]\n",param->id, param->sub_type, param->name)


void KSleep(LONG MilliSecond)
{
    LARGE_INTEGER Interval = {0};
    Interval.QuadPart = DELAY_ONE_MILLISECOND;
    Interval.QuadPart *= MilliSecond;
    KeDelayExecutionThread(KernelMode, 0, &Interval);
}

static void api_test_malloc_free(void)
{
	wf_u32 *ptr = NULL, i;

	ptr = wf_malloc(10*sizeof(wf_u32));
	if(ptr == NULL) {
		LOG_E("####Malloc buffer failed!\n");
	} else {
		LOG_D("####Malloc buffer sucess!\n");
		LOG_N("####dump info:\n");
		for(i=0; i<10; i++) {
			ptr[i] = i;
			LOG_N("%d ", i);
		}
		LOG_N("\n");

		wf_free(ptr);
		LOG_D("####malloc test end!\n");
	}
}

#if 0
static void api_test_thread1(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[0]) == wf_false)
    {
		EXEC_INFO(param);
		KSleep(1000);
    }

	wf_thread_exit(&test.thread[0]);
}

static void api_test_thread2(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[1]) == wf_false)
    {
    	EXEC_INFO(param);
		KSleep(1000);
    }

	wf_thread_exit(&test.thread[1]);
}

static void api_test_spinlock1(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[0]) == wf_false)
    {
    	LOG_D("thread1 spinlock\n");
		wf_lock_spin_lock(&test.spinlock);
		EXEC_INFO(param);
        test.data_buffer[0] = 0x3c;
		KSleep(100);
		LOG_D("thread1 execed! data=%x\n", test.data_buffer[0]);
		wf_lock_spin_unlock(&test.spinlock);
		KSleep(837);
    }

	wf_thread_exit(&test.thread[0]);
}

static void api_test_spinlock2(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[1]) == wf_false)
    {
    	LOG_D("thread1 spinlock\n");
        wf_lock_spin_lock(&test.spinlock);
		EXEC_INFO(param);
        test.data_buffer[0] = 0x57;
		KSleep(100);
		LOG_D("thread2 execed! data=%x\n", test.data_buffer[0]);
		wf_lock_spin_unlock(&test.spinlock);
		KSleep(968);
    }

	wf_thread_exit(&test.thread[1]);
}

static void api_test_mutexlock1(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[0]) == wf_false)
    {
    	LOG_D("thread1 mutexlock\n");
		wf_lock_mutex_lock(&test.mutexlock);
		EXEC_INFO(param);
        test.data_buffer[0] = 0x3c;
		KSleep(100);
		LOG_D("thread1 execed! data=%x\n", test.data_buffer[0]);
		wf_lock_mutex_unlock(&test.mutexlock);
		KSleep(837);
    }

	wf_thread_exit(&test.thread[0]);
}

static void api_test_mutexlock2(wf_api_param_t *param)
{
    /* poll ap thread according to each wdn_info */
    while (wf_thread_wait_stop(&test.thread[1]) == wf_false)
    {
    	LOG_D("thread2 mutexlock\n");
        wf_lock_mutex_lock(&test.mutexlock);
		EXEC_INFO(param);
        test.data_buffer[0] = 0x57;
		KSleep(100);
		LOG_D("thread2 execed! data=%x\n", test.data_buffer[0]);
		wf_lock_mutex_unlock(&test.mutexlock);
		KSleep(968);
    }

	wf_thread_exit(&test.thread[1]);
}

#endif
static void api_test_timer1(void *dpc, wf_os_api_timer_t *timer)
{
	wf_api_param_t *pparam = timer->pdata;
    EXEC_INFO(pparam);
	wf_timer_set(timer, 2000);
}

static void api_test_timer2(void *dpc, wf_os_api_timer_t *timer)
{
	wf_api_param_t *pparam = timer->pdata;
    EXEC_INFO(pparam);
	wf_timer_set(timer, 2000);
}

static void api_test_list_gen_proc(wf_api_test_t *test_info)
{
	wf_u32 i;
	wf_api_list_node_t *pnode, *ptemp;
	wf_list_t *plist;

	LOG_D("list gen:\n");
#if 0

	if(wf_list_is_empty(&test_info->list_header) == wf_false) {
		DbgPrint("####list has node\n");
	} else {
		DbgPrint("####list is null\n");
	}


	pnode = wf_malloc(sizeof(wf_api_list_node_t));
	if(pnode == NULL) {
		DbgPrint("malloc list node failed!\n");
		return;
	}

	pnode->index = 0;
	pnode->id = 5;
	pnode->type = 1;
	plist = &pnode->list;

	ptemp = LIST_CONTAINOR(plist, wf_api_list_node_t, list);
	DbgPrint("####pnode: [%x] [%x]\n", pnode, ptemp);
	wf_list_insert_head(&(pnode->list), &test_info->list_header);
	DbgPrint("####pnode insert sucess [%x] [%x-%x]\n", 
		pnode, test_info->list_header.next, test_info->list_header.prev);
	if(wf_list_is_empty(&test_info->list_header) == wf_false) {
		DbgPrint("####list has node\n");
	} else {
		DbgPrint("####list is null\n");
	}
	
	ptemp = wf_list_next(&test_info->list_header);
	DbgPrint("####get pnode: [%x] [%x]\n", pnode, ptemp);
	wf_list_delete(ptemp);
	DbgPrint("####del pnode: [%x] [%x-%x]\n", pnode, test_info->list_header.next, test_info->list_header.prev);
	wf_free(pnode);
#endif
#if 1	
	for(i=0; i<5; i++) {
		pnode = wf_malloc(sizeof(wf_api_list_node_t));
		if(pnode == NULL) {
			LOG_E("malloc list node failed!\n");
			return;
		}

		pnode->index = i;
		pnode->id = 5;
		pnode->type = 1;
		wf_list_insert_head(&(pnode->list), &test_info->list_header);
	}

	for(i=0; i<5; i++) {
		pnode = wf_malloc(sizeof(wf_api_list_node_t));
		if(pnode == NULL) {
			LOG_E("malloc list node failed!\n");
			return;
		}

		pnode->index = i;
		pnode->id = 5;
		pnode->type = 2;
		wf_list_insert_tail(&(pnode->list), &test_info->list_header);
	}
#endif
	LOG_D("list gen finish!\n");
}

static void api_test_list_destory(wf_api_test_t *test_info)
{
	wf_api_list_node_t *pnode;
    wf_list_t *plist;

	LOG_D("list destory:\n");
	while (wf_list_is_empty(&test_info->list_header) == wf_false)
    {
        plist = wf_list_next(&test_info->list_header);
        pnode = WF_CONTAINER_OF(plist, wf_api_list_node_t, list);
		LOG_D("####delete node[%d]:id=%d type=%d\n", pnode->index, pnode->id, pnode->type);
        wf_list_delete(plist);
        wf_free(pnode);
    }
}

void api_test_list(wf_api_test_t *test_info)
{
	wf_list_init(&test_info->list_header);
	api_test_list_gen_proc(test_info);
	api_test_list_destory(test_info);
}

static void api_test_semphor1(wf_api_param_t *param)
{
	LOG_D("[%s] start\n", __func__);
    /* poll ap thread according to each wdn_info */
    while (wf_os_api_thread_wait_stop(&test.thread[0]) == wf_false)
    {
		EXEC_INFO(param);
		wf_os_api_sema_post(&test.semphor);
		KSleep(1000);
    }

	wf_os_api_thread_exit(&test.thread[0]);
	LOG_D("[%s] end\n", __func__);
}

static void api_test_semphor2(wf_api_param_t *param)
{
	LOG_D("[%s] start\n", __func__);
    /* poll ap thread according to each wdn_info */
    while (wf_os_api_thread_wait_stop(&test.thread[1]) == wf_false)
    {
    #if 0
    	wf_sema_wait(&test.semphor);
    	EXEC_INFO(param);
		KSleep(200);
	#else
		if(!wf_os_api_sema_try(&test.semphor)) {
			LOG_D("thread2 receive semphor\n");
			KSleep(500);
		} else {
			EXEC_INFO(param);
			KSleep(200);
		}
	#endif
    }

	wf_os_api_thread_exit(&test.thread[1]);
	LOG_D("[%s] end\n", __func__);
}

static void api_test_timing(void)
{
	LARGE_INTEGER time_before = {0}, time_after = {0};
	
	KeQueryTickCount(&time_before);
	KSleep(1);
	KeQueryTickCount(&time_after);
	LOG_D("####[KeQueryTickCount] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);

	time_before.QuadPart = KeQueryTimeIncrement();
	KSleep(1);
	time_after.QuadPart = KeQueryTimeIncrement();
	LOG_D("####[KeQueryTimeIncrement] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);

	KeQuerySystemTime(&time_before);
	KSleep(1);
	KeQuerySystemTime(&time_after);
	LOG_D("####[KeQuerySystemTime] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);

	KeQueryTickCount(&time_before);
	KSleep(30);
	KeQueryTickCount(&time_after);
	LOG_D("####[KeQueryTickCount] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);

	time_before.QuadPart = KeQueryTimeIncrement();
	KSleep(30);
	time_after.QuadPart = KeQueryTimeIncrement();
	LOG_D("####[KeQueryTimeIncrement] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);

	KeQuerySystemTime(&time_before);
	KSleep(30);
	KeQuerySystemTime(&time_after);
	LOG_D("####[KeQuerySystemTime] before:%lld after:%lld\n", 
		time_before.QuadPart, time_after.QuadPart);
}

static void api_test_file(void)
{
	wf_file *file;
	char *path = L"\\??\\C:\\ram-fw-9188-new-r1455.bin";//can't use network path
	wf_u8 buffer[128] = {0};
	wf_u32 i;
	LOG_D("src file:%s\n", path);

	file = wf_os_api_file_open(path);
	if(file == NULL) {
		LOG_E("open file faile! [%s]", path);
		return;
	}
	
	wf_os_api_file_read(file, 128, buffer, 128);
	
	DbgPrint("file data:\n");
	for(i=0; i<128; i++) {
		if(i && i%16==0)
			DbgPrint("\n");
		DbgPrint("%02x ", buffer[i]);
	}
	DbgPrint("\n");

	
	wf_os_api_file_read(file, 640, buffer, 128);

	LOG_D("file data:\n");
	for(i=0; i<128; i++) {
		if(i && i%16==0)
			LOG_N("\n");
		LOG_N("%02x ", buffer[i]);
	}
	LOG_N("\n");

	wf_os_api_file_close(file);
}

void workqueue_handle(wf_work_struct *work)
{
	LOG_D("[%s] \n", __func__);
}

void api_test_workqueue(PDEVICE_OBJECT DeviceObject)
{
	wf_workqueue_mgnt_st arg;
    static wf_workqueue_func_param_st wq_test_param={"wq_test1",workqueue_handle};
	int i = 0;

	LOG_D("[%s,%d]\n", __func__, __LINE__);
	wf_os_api_workqueue_register(&arg, &wq_test_param);
	LOG_D("[%s,%d]\n", __func__, __LINE__);
	//test1
	for (i = 0; i < 5; i++)
	{
        arg.ops->workqueue_work(&arg);
	}
	LOG_D("[%s,%d]\n", __func__, __LINE__);
	//test2
	for (i = 0; i < 5; i++)
	{
		arg.ops->workqueue_work(&arg);
		wf_mdelay(1000);
	}
	LOG_D("[%s,%d]\n", __func__, __LINE__);
	//term
	arg.ops->workqueue_term(&arg);
	LOG_D("[%s,%d]\n", __func__, __LINE__);
}
void os_api_test(PDRIVER_OBJECT  DriverObject)
{
#if 0
	//api_test_file();

	api_test_timing();

	api_test_malloc_free();

	api_test_list(&test);
	LOG_D("[%s] %d\n", __func__, __LINE__);
	//wf_lock_spin_init(&test.spinlock);

	//wf_lock_mutex_init(&test.mutexlock);

	wf_os_api_sema_init(&test.semphor, 0);
	LOG_D("[%s] %d\n", __func__, __LINE__);
	if(NULL == wf_os_api_thread_create(&test.thread[0], test.name, api_test_semphor1, &test_param_thread1)) {
		LOG_E("####thread1 create failed!\n");
	}
	LOG_D("[%s] %d\n", __func__, __LINE__);
	if(NULL == wf_os_api_thread_create(&test.thread[1], test.name, api_test_semphor2, &test_param_thread2)) {
		LOG_E("####thread2 create failed!\n");
	}
	LOG_D("[%s] create thread success\n",__func__);
	wf_os_api_timer_init(&test.timer[0], api_test_timer1, &test_param_timer1);
	LOG_D("[%s] %d\n", __func__, __LINE__);
	wf_timer_set(&test.timer[0], 2000);
	LOG_D("[%s] %d\n", __func__, __LINE__);
	wf_os_api_timer_init(&test.timer[1], api_test_timer2, &test_param_timer2);
	LOG_D("[%s] %d\n", __func__, __LINE__);
	wf_timer_set(&test.timer[1], 2000);
#endif
	{

		api_test_workqueue(DriverObject);
	}
	
	LOG_D("[%s] end\n", __func__);
}

void os_api_test_exit(void)
{
	wf_os_api_thread_destory(&test.thread[1]);
	wf_os_api_timer_unreg(&test.timer[1]);

	wf_os_api_thread_destory(&test.thread[0]);
	wf_os_api_timer_unreg(&test.timer[0]);
	//wf_lock_spin_free(&test.spinlock);
}

