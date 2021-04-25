#ifndef __WF_OS_API_WORKQUEUE_H__

typedef struct wf_workqueue_s {
    PIO_WORKITEM *work_item;
    void *driver_obj;
    void(*function)(void *);
    WORK_QUEUE_TYPE que_type;
    void *param;
}wf_workqueue_t;


typedef wf_workqueue_t wf_work_struct;
typedef wf_workqueue_t wf_workqueue_struct;

typedef void(*work_func)(wf_work_struct *work);
typedef struct wf_workqueue_mgnt_st_ wf_workqueue_mgnt_st;

typedef struct
{
    char *workqueue_name;
    work_func func;
    PDRIVER_OBJECT  DriverObject;
}wf_workqueue_func_param_st;

typedef struct wf_workqueue_ops_st_
{
    int (*workqueue_init)(wf_workqueue_mgnt_st *wq, void *param);
    int (*workqueue_term)(wf_workqueue_mgnt_st *wq);
    int (*workqueue_work)(wf_workqueue_mgnt_st *wq);
}wf_workqueue_ops_st;

struct wf_workqueue_mgnt_st_
{
    wf_work_struct work;
    wf_workqueue_struct *workqueue;
    wf_workqueue_func_param_st *param;
    wf_workqueue_ops_st *ops;
};


void wf_os_api_workqueue_register(wf_workqueue_mgnt_st *wq, void *param);

#endif // !__WF_OS_API_WORKQUEUE_H__

