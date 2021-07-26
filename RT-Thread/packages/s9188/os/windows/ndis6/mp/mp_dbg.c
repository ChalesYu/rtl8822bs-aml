
#include "pcomp.h"
#include "wf_debug.h"

#define COUNTER_UNIT (1024*1024)
#define COUNTER_LOW_TH (5*COUNTER_UNIT)
#define COUNTER_HIGH_TH (60*COUNTER_UNIT)
//#define 

//the length parameter not include mac header len
void wf_dbg_counter_add(void *info, wf_u32 length, wf_u8 dir)
{
	wf_dbg_info_t *dbg_info = info;
	wf_u32 local_len = length<<3;//avoid calc in lock
	KIRQL irq = 0;

	KeAcquireSpinLock(&dbg_info->lock, &irq);
	if(dir == DBG_DIR_TX) {
		dbg_info->tx_cnt += (local_len);
	} else if(dir == DBG_DIR_RX) {
		dbg_info->rx_cnt += (local_len);
	}
	KeReleaseSpinLock(&dbg_info->lock, irq);
}

wf_u8 wf_dbg_start_ctrl(wf_dbg_info_t *dbg_info)
{
	if(dbg_info->start_falg == 0) {
		if(dbg_info->tx_counter > COUNTER_LOW_TH || dbg_info->rx_counter > COUNTER_LOW_TH) {
			dbg_info->start_falg = 1;
			dbg_info->stop_cnt = 0;
		}
	} else {
		//if continue 10s the tx and rx speed less 5Mbits/s, we stop debug
		if(dbg_info->tx_counter < COUNTER_LOW_TH && dbg_info->rx_counter < COUNTER_LOW_TH) {
			dbg_info->stop_cnt++;
		} else {
			dbg_info->stop_cnt = 0;
		}
		if(dbg_info->stop_cnt > 10) {
			dbg_info->start_falg = 0;
			dbg_info->stop_cnt = 0;
		}
	}

	return dbg_info->start_falg;
}

void wf_dbg_counter_handle(WDFTIMER WdfTimer)
{
	KIRQL irq = 0;
		
	wf_timer_ctx_t *timer_ctx = wf_get_timer_ctx(WdfTimer);
	PADAPTER padapter = timer_ctx->padapter;
	wf_dbg_info_t *dbg_info ;
	wf_recv_info_t *recv_info;
	wf_xmit_info_t *xmit_info;
	wf_usb_info_t *usb_info;
	nic_info_st *nic_info;
	wf_u64 tx_cnt;
	wf_u64 rx_cnt;

	if(padapter == NULL) {
		return;
	}

	dbg_info = padapter->dbg_info;
	recv_info = padapter->recv_info;
	xmit_info = padapter->xmit_info;
	usb_info = padapter->usb_info;
	nic_info = padapter->nic_info;

	if(WF_CANNOT_RUN(nic_info)) {
        return;
    }

	if(dbg_info == NULL || recv_info == NULL || xmit_info == NULL || usb_info == NULL) {
		return;
	}

//	KeAcquireSpinLock(&dbg_info->lock, &irq);
//	dbg_info->tx_counter = dbg_info->tx_cnt;
//	dbg_info->rx_counter = dbg_info->rx_cnt;
//	dbg_info->tx_cnt = 0;
//	dbg_info->rx_cnt = 0;
//	KeReleaseSpinLock(&dbg_info->lock, irq);
	WdfTimerStart(dbg_info->counter_timer, WDF_REL_TIMEOUT_IN_MS(2000));
	//DbgPrint("tx_pend=%d, rx_pend=%d, usb_pend=%d\n", xmit_info->proc_cnt, recv_info->proc_cnt, usb_info->proc_cnt);
	//LOG_D("drop_que=%d, drop_crc=%d, drop_type=%d, pkt_pend=%d", dbg_info->drop_by_que, dbg_info->drop_by_crc, dbg_info->drop_by_type, dbg_info->pkt_pending);
	//LOG_D("tx=%dK rx=%dK", dbg_info->tx_counter>>10, dbg_info->rx_counter>>10);
#if 0
	//we have get the counter value, then we can output info no lock
	if(wf_dbg_start_ctrl(dbg_info)) {
		//if(dbg_info->tx_counter < COUNTER_LOW_TH && dbg_info->rx_counter < COUNTER_LOW_TH) {
			DbgPrint("u:mf=%2d df=%2d mp=%2d dp=%2d pr=%2d  t:df=%2d dp=%2d pr=%2d  r:cf=%2d mp=%2d dp=%2d pr=%2d\n",
				usb_info->mgmt_free.cnt,
				usb_info->data_free.cnt,
				usb_info->mgmt_pend.cnt,
				usb_info->data_pend.cnt,
				usb_info->proc_cnt,
				
				xmit_info->data_free.cnt,
				xmit_info->data_pend.cnt,
				xmit_info->proc_cnt,
				
				recv_info->comm_free.cnt,
				recv_info->mgmt_pend.cnt,
				recv_info->data_pend.cnt,
				recv_info->proc_cnt);
		//}
	}
#endif

}


NDIS_STATUS wf_dbg_counter_timer_create(void *adapter)
{
	PADAPTER 			padapter = adapter;
	WDF_TIMER_CONFIG    config;
	NTSTATUS        	ntStatus;
	NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
	WDF_OBJECT_ATTRIBUTES  attributes;
	wf_timer_ctx_t *timer_ctx;
	wf_dbg_info_t *dbg_info = padapter->dbg_info;

	WDF_TIMER_CONFIG_INIT(&config, wf_dbg_counter_handle);
	//config.Period = 1;
	
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
                                   &attributes,
                                   wf_timer_ctx_t
                                   );
    attributes.ParentObject = padapter->WdfDevice;

    ntStatus = WdfTimerCreate(
                             &config,
                             &attributes,
                             &(dbg_info->counter_timer)     // Output handle
                             );
	
	if (!NT_SUCCESS(ntStatus)) {
        NT_STATUS_TO_NDIS_STATUS(ntStatus, &ndisStatus);
		LOG_D("create failed");
        return ndisStatus;
    }

	LOG_D("create success");

	timer_ctx = wf_get_timer_ctx(dbg_info->counter_timer);
    timer_ctx->padapter = padapter;

	WdfTimerStart(dbg_info->counter_timer, WDF_REL_TIMEOUT_IN_MS(1000));

	return ndisStatus;
}


NDIS_STATUS wf_dbg_init(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_dbg_info_t *dbg_info = NULL;

	LOG_D("start init dbg!\n");

	dbg_info = wf_malloc(sizeof(wf_dbg_info_t));
	if(dbg_info == NULL) {
		LOG_E("malloc recv info failed!\n");
		return NDIS_STATUS_FAILURE;
	}

	wf_memset(dbg_info, 0, sizeof(wf_dbg_info_t));

	padapter->dbg_info = dbg_info;
	dbg_info->adapter = padapter;

	wf_dbg_counter_timer_create(padapter);

	return NDIS_STATUS_SUCCESS;
}

void wf_dbg_deinit(void *adapter)
{
	PADAPTER padapter = adapter;
	wf_dbg_info_t *dbg_info = padapter->dbg_info;

	LOG_D("start deinit dbg!\n");

	if(dbg_info == NULL) {
		return;
	}

	wf_free(dbg_info);
}


