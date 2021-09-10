#ifndef __ARS_TXPOWER_H__
#define __ARS_TXPOWER_H__
typedef struct txpower_info_st_
{
	wf_u8 reserved;
}ars_txpower_info_st;

wf_s32 odm_DynamicTxPower(void *ars);
wf_s32 odm_TXPowerTrackingCheckCE(void *ars);
wf_s32 odm_DynamicTxPowerInit(void *ars);

#endif
