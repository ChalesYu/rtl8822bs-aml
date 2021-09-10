#ifndef _ARS_PATHDIV_H__
#define _ARS_PATHDIV_H__
typedef struct pathdiv_info_st_
{
	wf_u8 reserved;
}ars_pathdiv_info_st;

wf_s32 odm_PathDiversity(void *ars);
wf_s32 odm_PathDiversityInit(void *ars);

#endif
