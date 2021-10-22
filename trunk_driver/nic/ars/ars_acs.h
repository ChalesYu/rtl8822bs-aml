#ifndef __ARS_ACS_H__
#define __ARS_ACS_H__

typedef struct acs_info_st_
{
	wf_u8 reserved;
}ars_acs_info_st;

wf_s32 odm_AutoChannelSelectInit(void *ars);

#endif
