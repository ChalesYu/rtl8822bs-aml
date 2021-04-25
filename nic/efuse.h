#ifndef __EFUSE_H__
#define __EFUSE_H__

typedef enum WLAN__EEPORM_CODE 
{
	EFUSE_PHYSPACE = 0,
	WLAN_EEPORM_MAC = 1,
	EFUSE_VID = 2,
	EFUSE_PID = 3,
	EFUSE_MANU = 4,
	EFUSE_PRODUCT = 5,
	EFUSE_FREQCAL = 6,
	EFUSE_TEMPCAL = 7,
	EFUSE_CHANNELPLAN = 8,
	EFUSE_POWERCAL = 9,
	EFUSE_HEADERCHECK = 10,
	EFUSE_FIXDVALUE = 11,
	WLAN_EEPORM_BASEVALUE2 = 12,
	EFUSE_PHYCFGCHECK = 13,
	EFUSE_GROUP_0=14,
	EFUSE_GROUP_1=15,
	EFUSE_GROUP_2=16,
	EFUSE_GROUP_3=17,
	EFUSE_GROUP_4=18,
#ifdef CONFIG_FIX_FREQ
	EFUSE_CUSTOMER1=19,
#endif
	WLAN_EEPORM_CODE_MAX
} EUSE_CODE;


char * wf_get_efuse_type(nic_info_st *nic_info);
wf_u32 wf_get_efuse_len(nic_info_st *nic_info);
int wf_get_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len);
int wf_efuse_select(nic_info_st *nic_info);
int wf_get_efuse_load_mode(nic_info_st *nic_info, wf_u32 *bautoload_flag, wf_u32 len);
int wf_mcu_mp_efuse_get(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *outdata, wf_u32 outdata_len);
int wf_mcu_mp_efuse_set(nic_info_st *nic_info, EUSE_CODE efuse_code, wf_u32 *indata, wf_u32 indata_len, wf_u32 *out, wf_u32 out_len);
int  wf_get_phy_efuse_data(nic_info_st *nic_info, wf_u8 *outdata, wf_u32 efuse_len );
wf_u32 wf_get_phy_efuse_len(nic_info_st *nic_info);
#endif