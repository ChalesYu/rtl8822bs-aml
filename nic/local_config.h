#ifndef __LOCAL_CFG_H__
#define __LOCAL_CFG_H__


typedef enum 
{
	WF_AUTO_MODE,		/* Let the driver decides */
	WF_ADHOC_MODE,		/* Single cell network */
	WF_INFRA_MODE,		/* Multi cell network, roaming, ... */
	WF_MASTER_MODE,		/* Synchronisation master or Access Point */
	WF_REPEAT_MODE,		/* Wireless Repeater (forwarder) */
	WF_SECOND_MODES,	/* Secondary master/repeater (backup) */
	WF_MONITOR_MODE,	/* Passive monitor (listen only) */
	WF_MESH_MODE,		/* Mesh (IEEE 802.11s) network */
}sys_work_mode_e;


typedef struct  
{
    sys_work_mode_e    	work_mode;
    wf_u8         		channel;
    CHANNEL_WIDTH  		bw;
    wf_bool         	adhoc_master;
    wf_u8               ssid[32];
    wf_u8               channel_plan;
    wf_u8               ba_enable;
#ifdef CONFIG_ARS_SUPPORT
    wf_u8               adaptivity_en;
    wf_u8               antenna_diversity;
    wf_u8               beamforming_support;
    wf_u8               force_igi;
#endif
}local_info_st;

#define NIC_INFO_2_WORK_MODE(nic) ((local_info_st *)nic->local_info)->work_mode

int wf_local_cfg_init(nic_info_st *nic_info);
int wf_local_cfg_term(nic_info_st *nic_info);
int wf_local_cfg_set_default(nic_info_st *nic_info);
int wf_local_cfg_get_default(nic_info_st *nic_info);
sys_work_mode_e wf_local_cfg_get_work_mode (nic_info_st *pnic_info);
void wf_local_cfg_set_work_mode (nic_info_st *pnic_info, sys_work_mode_e mode);


#endif
