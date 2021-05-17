/* use linux netdev ioctl framework */
#include "android_priv_cmd.h"
#include "wf_debug.h"
#include "common.h"
#include "wf_cfg80211.h"

#define MIRACAST_DISABLED	0
#define MIRACAST_SOURCE		0x00000001
#define MIRACAST_SINK		0x00000002



#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
#define strnicmp    strncasecmp
#endif

static int android_cmd_null(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    LOG_I("[%s,%d]:Android private command [%s] is null\n",__func__,__LINE__,command);
    return 0;
}

static int android_cmd_getrssi(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    wf_u8 qual,level;
    int bytes_written = 0;
    wf_bool is_connected = wf_false;
    wf_wlan_info_t *pwlan_info = nic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    wf_mlme_get_connect(nic_info, &is_connected);

    if(is_connected){
        wf_wlan_get_signal_and_qual(nic_info,&qual, &level);
        bytes_written +=
        snprintf(&command[bytes_written], total_len, "%s rssi %d",
					 pcur_network->ssid.data, translate_percentage_to_dbm(level));
    }else{
        bytes_written +=snprintf(&command[bytes_written], total_len, "No connection");
    }
    
    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);

    return bytes_written;
}

static int android_cmd_getlinkspeed(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    u16 link_speed = 0;
    int bytes_written = 0;
    wf_bool is_connected = wf_false;
    wf_wlan_info_t *pwlan_info = nic_info->wlan_info;
    wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;

    wf_mlme_get_connect(nic_info, &is_connected);

    if(is_connected){
        wf_wlan_get_max_rate(nic_info,(wf_u8 *)pcur_network->bssid, &link_speed);
        bytes_written += snprintf(command, total_len, "LinkSpeed %d", link_speed / 10);
    }else{
        bytes_written += snprintf(command, total_len, "LinkSpeed 0");
    }
    
    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);

    return bytes_written;
}


static int android_cmd_getp2pdevaddr(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    struct net_device *pndev = nic_info->ndev;

    memcpy(command,pndev->dev_addr,ETH_ALEN);
    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return ETH_ALEN;
}


static int android_cmd_setfwpath(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{

    LOG_I("set fw path realise later");
//    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return 0;
}


static int android_cmd_setband(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    char *value = command + strlen(cmd_handle->cmd_name) + 1;
    u32 band = WIFI_FREQUENCY_BAND_AUTO;
    int ret = -1;
    
    if (sscanf(value, "%u", &band) >= 1){
        nic_info->setband = band;
        LOG_I("Our driver only support 2.4g:%u\n",band);
        return 0;
    }
        
    return ret;
}

static int android_cmd_getband(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;

    bytes_written = snprintf(command, total_len, "%u", nic_info->setband);

    return bytes_written;
}

/* set country code*/
extern int country_code_to_ch_plan (char *pcountry_code, wf_u8 *pch_plan);

static int andriod_set_countrycode(nic_info_st *nic_info, char *country_code)
{
    wf_u8 ch_plan;
    wf_s8 ret = 0;
    hw_info_st *phw_info = (hw_info_st *)nic_info->hw_info;
    
    ret = country_code_to_ch_plan(country_code, &ch_plan);
    if(ret < 0){
        LOG_I("country_code_to_ch_plan failed ret =  :%d\n",ret);
        return ret;
    }else{
        phw_info->channel_plan = ch_plan;
        ret = channel_init(nic_info);
        LOG_I("set channel_plan = %x : ret = %d\n",ch_plan,ret);
    }
    return ret;
}

static int android_cmd_setcountry(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    char *country_code = command + strlen(cmd_handle->cmd_name) + 1;

    if(andriod_set_countrycode(nic_info,country_code) != 0)
        LOG_I("set country code failed !!!\n");
    
    return bytes_written;
}

/* need realise later*/
static int android_cmd_set_mgnt_wpsp2pie(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    int skip = strlen(cmd_handle->cmd_name) + 3;
    
    bytes_written = wf_cfg80211_set_wps_p2pie(nic_info, command + skip, total_len - skip, *(command + skip - 2) - '0');
    LOG_I(" command [%s]  done\n",command);

    return bytes_written;
}

const char *do_query_miracast_str(int mode)
{
	if (mode == MIRACAST_SOURCE)
		return "SOURCE";
	else if (mode == MIRACAST_SINK)
		return "SINK";
	else if (mode == (MIRACAST_SOURCE | MIRACAST_SINK))
		return "SOURCE&SINK";
	else if (mode == MIRACAST_DISABLED)
		return "DISABLED";
	else
		return "INVALID";
}

/* need realise later*/
static int android_cmd_set_miracastmode(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    u8 mode;
    int bytes_written = 0;
 //   wf_wlan_info_t *pwlan_info = nic_info->wlan_info;
//    struct wifi_display_info *pwfd_info = pwlan_info->wfd_info;
    char *cmd = command + strlen(cmd_handle->cmd_name) + 1;

    if(sscanf(cmd, "%hhu", &mode) < 1){
        return -1;
    }

    switch (mode) {
	case 1:
		mode = MIRACAST_SOURCE;
		break;
	case 2:
		mode = MIRACAST_SINK;
		break;
	case 0:
	default:
		mode = MIRACAST_DISABLED;
		break;
	}
/*    pwfd_info->stack_wfd_mode = mode;
    WL_INFO("stack miracast mode: %s\n",
			do_query_miracast_str(wfd_info->stack_wfd_mode));
*/
    LOG_I(" command [%s] need realise\n",command);

    return bytes_written;
}

///////////////////////////////////////////////
#ifdef CONFIG_PNO_SUPPORT
#define	CSCAN_TLV_TYPE_SSID_IE	'S'
#define CIPHER_IE "key_mgmt="
#define CIPHER_NONE "NONE"
#define CIPHER_WPA_PSK "WPA-PSK"
#define CIPHER_WPA_EAP "WPA-EAP IEEE8021X"
int do_aly_ssid_list_tlv(char **list_str, pno_ssid_t * ssid,
							int max, int *bytes_left)
{
	char *str;
	int idx = 0;

	if ((list_str == NULL) || (*list_str == NULL) || (*bytes_left < 0)) {
		WL_INFO("%s error paramters\n", __func__);
		return -1;
	}

	str = *list_str;
	while (*bytes_left > 0) {

		if (str[0] != CSCAN_TLV_TYPE_SSID_IE) {
			*list_str = str;
			WL_INFO("nssid=%d left_parse=%d %d\n", idx, *bytes_left, str[0]);
			return idx;
		}

		*bytes_left -= 1;
		str += 1;

		if (str[0] == 0) {
			ssid[idx].SSID_len = 0;
			memset((char *)ssid[idx].SSID, 0x0, WLAN_SSID_MAXLEN);
			*bytes_left -= 1;
			str += 1;

			WL_INFO("BROADCAST SCAN  left=%d\n", *bytes_left);
		} else if (str[0] <= WLAN_SSID_MAXLEN) {
			ssid[idx].SSID_len = str[0];
			*bytes_left -= 1;
			str += 1;

			if (ssid[idx].SSID_len > *bytes_left) {
				WL_INFO("%s out of memory range len=%d but left=%d\n",
						__func__, ssid[idx].SSID_len, *bytes_left);
				return -1;
			}

			memcpy((char *)ssid[idx].SSID, str, ssid[idx].SSID_len);

			*bytes_left -= ssid[idx].SSID_len;
			str += ssid[idx].SSID_len;

			WL_INFO("%s :size=%d left=%d\n",
					(char *)ssid[idx].SSID, ssid[idx].SSID_len, *bytes_left);
		} else {
			WL_INFO("### SSID size more that %d\n", str[0]);
			return -1;
		}

		if (idx++ > max) {
			WL_INFO("%s number of SSIDs more that %d\n", __func__, idx);
			return -1;
		}
	}

	*list_str = str;
	return idx;
}

static int do_aly_cipher_list(struct pno_nlo_info *nlo_info, char *list_str)
{

	u8 key_len = 0, index = 0;
	char *pch, *pnext, *pend;

	pch = list_str;

	if (nlo_info == NULL || list_str == NULL) {
		WL_INFO("%s error paramters\n", __func__);
		return -1;
	}

	while (strlen(pch) != 0) {
		pnext = strstr(pch, "key_mgmt=");
		if (pnext != NULL) {
			pch = pnext + strlen(CIPHER_IE);
			pend = strstr(pch, "}");
			if (strncmp(pch, CIPHER_NONE, strlen(CIPHER_NONE)) == 0) {
				nlo_info->ssid_cipher_info[index] = 0x00;
			} else if (strncmp(pch, CIPHER_WPA_PSK,
							   strlen(CIPHER_WPA_PSK)) == 0) {
				nlo_info->ssid_cipher_info[index] = 0x66;
			} else if (strncmp(pch, CIPHER_WPA_EAP,
							   strlen(CIPHER_WPA_EAP)) == 0) {
				nlo_info->ssid_cipher_info[index] = 0x01;
			}
			index++;
			pch = pend + 1;
		} else {
			break;
		}
	}
	return 0;
}

static int set_dev_nlo(struct pno_nlo_info *nlo_info,
								pno_ssid_t * ssid, int num, int pno_time,
								int pno_repeat, int pno_freq_expo_max)
{

	int i = 0;
	struct file *fp;
	mm_segment_t fs;
	loff_t pos = 0;
	u8 *source = NULL;
	long len = 0;

	WL_INFO("+%s+\n", __func__);

	nlo_info->fast_scan_period = pno_time;
	nlo_info->ssid_num = num & BIT_LEN_MASK_32(8);
	nlo_info->hidden_ssid_num = num & BIT_LEN_MASK_32(8);
	nlo_info->slow_scan_period = (pno_time * 2);
	nlo_info->fast_scan_iterations = 5;

	if (nlo_info->hidden_ssid_num > 8)
		nlo_info->hidden_ssid_num = 8;

	for (i = 0; i < num; i++) {
		nlo_info->ssid_length[i]
			= ssid[i].SSID_len;
	}

	fp = filp_open("/data/misc/wifi/wpa_supplicant.conf", O_RDONLY, 0644);
	if (IS_ERR(fp)) {
		WL_INFO("Error, wpa_supplicant.conf doesn't exist.\n");
		WL_INFO("Error, cipher array using default value.\n");
		return 0;
	}

	len = i_size_read(fp->f_path.dentry->d_inode);
	if (len < 0 || len > 2048) {
		WL_INFO("Error, file size is bigger than 2048.\n");
		WL_INFO("Error, cipher array using default value.\n");
		return 0;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);

	source = wl_zmalloc(2048);

	if (source != NULL) {
		len = vfs_read(fp, source, len, &pos);
		do_aly_cipher_list(nlo_info, source);
		wl_mfree(source, 2048);
	}

	set_fs(fs);
	filp_close(fp, NULL);

	WL_INFO("-%s-\n", __func__);
	return 0;
}

static int wet_dev_ssid_map(struct pno_ssid_list *pno_ssid_list,
								 pno_ssid_t * ssid, u8 num)
{

	int i = 0;
	if (num > MAX_PNO_LIST_COUNT)
		num = MAX_PNO_LIST_COUNT;

	for (i = 0; i < num; i++) {
		Func_Of_Proc_Pre_Memcpy(&pno_ssid_list->node[i].SSID,
					ssid[i].SSID, ssid[i].SSID_len);
		pno_ssid_list->node[i].SSID_len = ssid[i].SSID_len;
	}
	return 0;
}

static int set_dev_scan_para(_wadptdata * pwadptdata, pno_ssid_t * ssid,
								 unsigned char ch, unsigned char ch_offset,
								 unsigned short bw_mode)
{

	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(pwadptdata);
	struct pno_scan_info *scan_info = pwrctl->pscan_info;
	int i;

	scan_info->channel_num = MAX_SCAN_LIST_COUNT;
	scan_info->orig_ch = ch;
	scan_info->orig_bw = bw_mode;
	scan_info->orig_40_offset = ch_offset;

	for (i = 0; i < scan_info->channel_num; i++) {
		if (i < 11)
			scan_info->ssid_channel_info[i].active = 1;
		else
			scan_info->ssid_channel_info[i].active = 0;

		scan_info->ssid_channel_info[i].timeout = 100;

		scan_info->ssid_channel_info[i].tx_power =
			Func_Chip_Bb_Gettxpowerindex(pwadptdata, 0, 0x02, bw_mode, i + 1);

		scan_info->ssid_channel_info[i].channel = i + 1;
	}

	WL_INFO
		("%s, channel_num: %d, orig_ch: %d, orig_bw: %d orig_40_offset: %d\n",
		 __func__, scan_info->channel_num, scan_info->orig_ch,
		 scan_info->orig_bw, scan_info->orig_40_offset);
	return 0;
}pwr_mode

int set_dev_pno(struct nic_info_st *nic_info, pno_ssid_t * ssid, int num,
					int pno_time, int pno_repeat, int pno_freq_expo_max, u8 tag)
{
/*
	_wadptdata *pwadptdata = (_wadptdata *) wl_netdev_priv(net);
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(pwadptdata);
	struct mlme_ext_priv *pmlmeext = &pwadptdata->mlmeextpriv;
*/
    pwr_info_st *pwr_info_ptr = (pwr_info_st *)nic_info->pwr_info;

	int ret = -1;
	if (tag) {
		if (num == 0) {
			WL_INFO("%s, nssid is zero, no need to setup pno ssid list\n",
					__func__);
			return 0;
		}

		if (pwrctl == NULL) {
			WL_INFO("%s, ERROR: pwrctl is NULL\n", __func__);
			return -1;
		} else {
			pwrctl->pnlo_info =
				(pno_nlo_info_t *) wl_zmalloc(sizeof(pno_nlo_info_t));
			pwrctl->pno_ssid_list =
				(pno_ssid_list_t *) wl_zmalloc(sizeof(pno_ssid_list_t));
			pwrctl->pscan_info =
				(pno_scan_info_t *) wl_zmalloc(sizeof(pno_scan_info_t));
		}

		if (pwrctl->pnlo_info == NULL ||
			pwrctl->pscan_info == NULL || pwrctl->pno_ssid_list == NULL) {
			WL_INFO("%s, ERROR: alloc nlo_info, ssid_list, scan_info fail\n",
					__func__);
			goto failing;
		}

		pwrctl->pno_in_resume = _FALSE;

		pwrctl->pno_inited = _TRUE;
		ret = set_dev_nlo(pwrctl->pnlo_info, ssid, num,
								   pno_time, pno_repeat, pno_freq_expo_max);

		ret = wet_dev_ssid_map(pwrctl->pno_ssid_list, ssid, num);

		ret = set_dev_scan_para(pwadptdata, ssid, pmlmeext->cur_channel,
									pmlmeext->cur_ch_offset,
									pmlmeext->cur_bwmode);

		WL_INFO
			("+%s num: %d, pno_time: %d, pno_repeat:%d, pno_freq_expo_max:%d+\n",
			 __func__, num, pno_time, pno_repeat, pno_freq_expo_max);
	}
	return 0;

failing:
	if (pwrctl->pnlo_info) {
		wl_mfree((u8 *) pwrctl->pnlo_info, sizeof(pno_nlo_info_t));
		pwrctl->pnlo_info = NULL;
	}
	if (pwrctl->pno_ssid_list) {
		wl_mfree((u8 *) pwrctl->pno_ssid_list, sizeof(pno_ssid_list_t));
		pwrctl->pno_ssid_list = NULL;
	}
	if (pwrctl->pscan_info) {
		wl_mfree((u8 *) pwrctl->pscan_info, sizeof(pno_scan_info_t));
		pwrctl->pscan_info = NULL;
	}

	return -1;
}

#ifdef CONFIG_PNO_SET_DEBUG
void dbg_dev_pno(struct net_device *net, u8 tag)
{
	_wadptdata *pwadptdata = (_wadptdata *) wl_netdev_priv(net);
	struct pwrctrl_priv *pwrctl = wadptdata_to_pwrctl(pwadptdata);
	int i = 0, j = 0;
	if (tag) {
		LOG_I("*******NLO_INFO********\n");
		LOG_I("ssid_num: %d\n", pwrctl->pnlo_info->ssid_num);
		LOG_I("fast_scan_iterations: %d\n",
				pwrctl->pnlo_info->fast_scan_iterations);
		LOG_I("fast_scan_period: %d\n", pwrctl->pnlo_info->fast_scan_period);
		LOG_I("slow_scan_period: %d\n", pwrctl->pnlo_info->slow_scan_period);
		LOG_I("ssid_length: ");
		for (i = 0; i < MAX_PNO_LIST_COUNT; i++) {
			LOG_I("%d, ", pwrctl->pnlo_info->ssid_length[i]);
		}
		LOG_I("\n");

		LOG_I("cipher_info: ");
		for (i = 0; i < MAX_PNO_LIST_COUNT; i++) {
			LOG_I("%d, ", pwrctl->pnlo_info->ssid_cipher_info[i]);
		}
		LOG_I("\n");

		LOG_I("channel_info: ");
		for (i = 0; i < MAX_PNO_LIST_COUNT; i++) {
			LOG_I("%d, ", pwrctl->pnlo_info->ssid_channel_info[i]);
		}
		LOG_I("\n");

		LOG_I("******SSID_LISD******\n");
		for (i = 0; i < MAX_PNO_LIST_COUNT; i++) {
			LOG_I("[%d]SSID: %s \n", i, pwrctl->pno_ssid_list->node[i].SSID);
		}

		LOG_I("******SCAN_INFO******\n");
		LOG_I("ch_num: %d\n", pwrctl->pscan_info->channel_num);
		LOG_I("orig_ch: %d\n", pwrctl->pscan_info->orig_ch);
		LOG_I("orig bw: %d\n", pwrctl->pscan_info->orig_bw);
		LOG_I("orig 40 offset: %d\n", pwrctl->pscan_info->orig_40_offset);
		for (i = 0; i < MAX_SCAN_LIST_COUNT; i++) {
			LOG_I("[%02d] avtive:%d, timeout:%d, tx_power:%d, ch:%02d\n",
					i, pwrctl->pscan_info->ssid_channel_info[i].active,
					pwrctl->pscan_info->ssid_channel_info[i].timeout,
					pwrctl->pscan_info->ssid_channel_info[i].tx_power,
					pwrctl->pscan_info->ssid_channel_info[i].channel);
		}
		LOG_I("*****************\n");
	}
}
#endif
#endif


#ifdef CONFIG_PNO_SUPPORT
static int android_cmd_pnosetup(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)

{
	int res = -1;
	int nssid = 0;
	cmd_tlv_t *cmd_tlv_temp;
	char *str_ptr;
	int tlv_size_left;
	int pno_time = 0;
	int pno_repeat = 0;
	int pno_freq_expo_max = 0;
	pno_ssid_t pno_ssids_local[MAX_PNO_LIST_COUNT];
    struct net_device *pndev = nic_info->ndev;
	int cmdlen = strlen(cmd_handle->cmd_name) + 1;

#ifdef CONFIG_PNO_SET_DEBUG
	int i;
	char *p;
	p = pno_in_example;

	total_len = sizeof(pno_in_example);
	str_ptr = p + cmdlen;
#else
	str_ptr = command + cmdlen;
#endif

	if (total_len < (cmdlen + sizeof(cmd_tlv_t))) {
		LOG_I("%s argument=%d less min size\n", __func__, total_len);
		goto exit_proc;
	}

	tlv_size_left = total_len - cmdlen;

	cmd_tlv_temp = (cmd_tlv_t *) str_ptr;
	memset(pno_ssids_local, 0, sizeof(pno_ssids_local));

	if ((cmd_tlv_temp->prefix == PNO_TLV_PREFIX) &&
		(cmd_tlv_temp->version == PNO_TLV_VERSION) &&
		(cmd_tlv_temp->subver == PNO_TLV_SUBVERSION)) {

		str_ptr += sizeof(cmd_tlv_t);
		tlv_size_left -= sizeof(cmd_tlv_t);

		if ((nssid = do_aly_ssid_list_tlv(&str_ptr, pno_ssids_local,
											 MAX_PNO_LIST_COUNT,
											 &tlv_size_left)) <= 0) {
			LOG_I("SSID is not presented or corrupted ret=%d\n", nssid);
			goto exit_proc;
		} else {
			if ((str_ptr[0] != PNO_TLV_TYPE_TIME) || (tlv_size_left <= 1)) {
				LOG_I("%s scan duration corrupted field size %d\n",
						__func__, tlv_size_left);
				goto exit_proc;
			}
			str_ptr++;
			pno_time = simple_strtoul(str_ptr, &str_ptr, 16);
			LOG_I("%s: pno_time=%d\n", __func__, pno_time);

			if (str_ptr[0] != 0) {
				if ((str_ptr[0] != PNO_TLV_FREQ_REPEAT)) {
					LOG_I("%s pno repeat : corrupted field\n", __func__);
					goto exit_proc;
				}
				str_ptr++;
				pno_repeat = simple_strtoul(str_ptr, &str_ptr, 16);
				LOG_I("%s :got pno_repeat=%d\n", __FUNCTION__, pno_repeat);
				if (str_ptr[0] != PNO_TLV_FREQ_EXPO_MAX) {
					LOG_I("%s FREQ_EXPO_MAX corrupted field size\n",
							__func__);
					goto exit_proc;
				}
				str_ptr++;
				pno_freq_expo_max = simple_strtoul(str_ptr, &str_ptr, 16);
				LOG_I("%s: pno_freq_expo_max=%d\n",
						__func__, pno_freq_expo_max);
			}
		}
	} else {
		LOG_I("%s get wrong TLV command\n", __FUNCTION__);
		goto exit_proc;
	}

	res =
		set_dev_pno(nic_info, pno_ssids_local, nssid, pno_time, pno_repeat,
						pno_freq_expo_max, 1);

#ifdef CONFIG_PNO_SET_DEBUG
	dbg_dev_pno(pndev, 1);
#endif
exit_proc:
	return res;
}
#endif
////////////////////////////////////




static int android_cmd_getmacaddr(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    struct net_device *pndev = nic_info->ndev;

    bytes_written =snprintf(command, total_len, "Macaddr = " MAC_FMT, MAC_ARG(pndev->dev_addr));
    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}

static int android_cmd_setblockscan(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    struct wf_widev_priv *pwdev_info = &nic_info->widev_priv;

    char *block_value = command + strlen(cmd_handle->cmd_name) + 1;
#ifdef CONFIG_IOCTL_CFG80211
	pwdev_info->block_scan =(*block_value == '0') ? wf_false : wf_true;
#endif

    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}

static int android_cmd_setblock(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    struct wf_widev_priv *pwdev_info = &nic_info->widev_priv;

    char *block_value = command + strlen(cmd_handle->cmd_name) + 1;
#ifdef CONFIG_IOCTL_CFG80211
	pwdev_info->block =(*block_value == '0') ? wf_false : wf_true;
#endif

    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}

/*need realise*/
static int android_cmd_changedtim(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;

    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}


static int android_cmd_set_macacl(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    //wf_wlan_info_t *pwlan_info = nic_info->wlan_info;
    //wf_wlan_network_t *pcur_network = &pwlan_info->cur_network;


    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}

static int android_cmd_driverversion(nic_info_st *nic_info,char *command,int total_len, android_cmd_handle_st *cmd_handle)
{
    int bytes_written = 0;
    bytes_written = snprintf(command, strlen(DRIVER_VERSION) + 1, DRIVER_VERSION);

    LOG_I("[%s,%d]:Process of android private command [%s] end\n",__func__,__LINE__,command);
    return bytes_written;
}



static android_cmd_handle_st gl_android_cmd[]=
{
    {   "START",    ANDROID_WIFI_CMD_START,                                       android_cmd_null    },
    {    "STOP",    ANDROID_WIFI_CMD_STOP,                                        android_cmd_null    },
    {    "SCAN-ACTIVE", ANDROID_WIFI_CMD_SCAN_ACTIVE,                             android_cmd_null    },
    {    "SCAN-PASSIVE",    ANDROID_WIFI_CMD_SCAN_PASSIVE,                        android_cmd_null    },
    {    "RSSI",    ANDROID_WIFI_CMD_RSSI,                                       android_cmd_getrssi  },
    {    "LINKSPEED",   ANDROID_WIFI_CMD_LINKSPEED,                         android_cmd_getlinkspeed  },
    {    "RXFILTER-START",  ANDROID_WIFI_CMD_RXFILTER_START,                        android_cmd_null  },
    {    "RXFILTER-STOP",   ANDROID_WIFI_CMD_RXFILTER_STOP,                         android_cmd_null  },
    {    "RXFILTER-ADD",    ANDROID_WIFI_CMD_RXFILTER_ADD,                          android_cmd_null  },
    {    "RXFILTER-REMOVE", ANDROID_WIFI_CMD_RXFILTER_REMOVE,                       android_cmd_null  },
    {    "BTCOEXSCAN-START",    ANDROID_WIFI_CMD_BTCOEXSCAN_START,                  android_cmd_null  },
    {    "BTCOEXSCAN-STOP", ANDROID_WIFI_CMD_BTCOEXSCAN_STOP,                       android_cmd_null  },
    {    "BTCOEXMODE",  ANDROID_WIFI_CMD_BTCOEXMODE,                                android_cmd_null  },
    {    "SETSUSPENDOPT",   ANDROID_WIFI_CMD_SETSUSPENDOPT,                         android_cmd_null  },
    {    "P2P_DEV_ADDR",    ANDROID_WIFI_CMD_P2P_DEV_ADDR,                 android_cmd_getp2pdevaddr  },
    {    "SETFWPATH",   ANDROID_WIFI_CMD_SETFWPATH,                            android_cmd_setfwpath  },
    {    "SETBAND", ANDROID_WIFI_CMD_SETBAND,                                    android_cmd_setband  },
    {    "GETBAND", ANDROID_WIFI_CMD_GETBAND,                                    android_cmd_getband  },
    {    "COUNTRY", ANDROID_WIFI_CMD_COUNTRY,                                 android_cmd_setcountry  },
    {    "P2P_SET_NOA", ANDROID_WIFI_CMD_P2P_SET_NOA,                               android_cmd_null  },
    {    "P2P_GET_NOA", ANDROID_WIFI_CMD_P2P_GET_NOA,                               android_cmd_null  },
    {    "P2P_SET_PS",  ANDROID_WIFI_CMD_P2P_SET_PS,                                android_cmd_null  },
    {    "SET_AP_WPS_P2P_IE",   ANDROID_WIFI_CMD_SET_AP_WPS_P2P_IE,    android_cmd_set_mgnt_wpsp2pie  },
    {    "MIRACAST",    ANDROID_WIFI_CMD_MIRACAST,                      android_cmd_set_miracastmode  },
#ifdef CONFIG_PNO_SUPPORT                                       
    {    "PNOSSIDCLR",  ANDROID_WIFI_CMD_PNOSSIDCLR_SET,                            android_cmd_null  },
    {    "PNOSETUP",    ANDROID_WIFI_CMD_PNOSETUP_SET,                          android_cmd_pnosetup  },
    {    "PNOFORCE",    ANDROID_WIFI_CMD_PNOENABLE_SET,                             android_cmd_null  },
    {    "PNODEBUG",    ANDROID_WIFI_CMD_PNODEBUG_SET,                              android_cmd_null  },
#endif                                                                              
    {    "MACADDR", ANDROID_WIFI_CMD_MACADDR,                                 android_cmd_getmacaddr  },
    {    "BLOCK_SCAN",  ANDROID_WIFI_CMD_BLOCK_SCAN,                        android_cmd_setblockscan  },
    {    "BLOCK",   ANDROID_WIFI_CMD_BLOCK,                                     android_cmd_setblock  },
    {    "WFD-ENABLE",  ANDROID_WIFI_CMD_WFD_ENABLE,                                android_cmd_null  },
    {    "WFD-DISABLE", ANDROID_WIFI_CMD_WFD_DISABLE,                           android_cmd_null      },
    {    "WFD-SET-TCPPORT", ANDROID_WIFI_CMD_WFD_SET_TCPPORT,                   android_cmd_null      },
    {    "WFD-SET-MAXTPUT", ANDROID_WIFI_CMD_WFD_SET_MAX_TPUT,                  android_cmd_null      },
    {    "WFD-SET-DEVTYPE", ANDROID_WIFI_CMD_WFD_SET_DEVTYPE,                   android_cmd_null      },
    {    "SET_DTIM",    ANDROID_WIFI_CMD_CHANGE_DTIM,                         android_cmd_changedtim  },
    {    "HOSTAPD_SET_MACADDR_ACL", ANDROID_WIFI_CMD_HOSTAPD_SET_MACADDR_ACL, android_cmd_set_macacl  },
    {    "HOSTAPD_ACL_ADD_STA", ANDROID_WIFI_CMD_HOSTAPD_ACL_ADD_STA,           android_cmd_null      },
    {    "HOSTAPD_ACL_REMOVE_STA",  ANDROID_WIFI_CMD_HOSTAPD_ACL_REMOVE_STA,        android_cmd_null  },
    {    "P2P_DISABLE", ANDROID_WIFI_CMD_P2P_DISABLE,                           android_cmd_null      },
    {    "DRIVER_VERSION",ANDROID_WIFI_CMD_DRIVERVERSION,                  android_cmd_driverversion  },
    {    "SETSUSPENDMODE",ANDROID_WIFI_CMD_SETSUSPENDMODE,                        android_cmd_null    },
};

#define ANDROID_CMD_SIZE (sizeof(gl_android_cmd)/sizeof(gl_android_cmd[0]))

int wf_android_priv_cmd_ioctl(struct net_device *net, struct ifreq *ifr, int cmd)
{
    int ret         = 0;
    int cmd_num;
    char *command = NULL;
    int bytes_written = 0;
#ifdef CONFIG_PNO_SUPPORT
    uint cmdlen = 0;
    uint pno_enable = 0;
#endif
    android_usr_cmd_buff priv_cmd;
    ndev_priv_st *ndev_priv = netdev_priv(net);
    nic_info_st *nic_info = ndev_priv->nic;
    hw_info_st *hw_info = nic_info->hw_info;
    
#ifdef CONFIG_WFD
    //struct wifi_display_info *pwfd_info;
#endif
    
    //Func_Of_Proc_Lock_Suspend();

    if (!ifr->ifr_data) 
    {
        ret = -EINVAL;
        return ret;
    }
    if (hw_info->mp_mode == 1) 
    {
        ret = -EINVAL;
        return ret;
    }
#ifdef CONFIG_COMPAT
    //if (is_compat_task()) 
    if(1)
    {
        compat_android_usr_cmd_buff compat_priv_cmd;

        if (copy_from_user(&compat_priv_cmd, ifr->ifr_data,
             sizeof(compat_android_usr_cmd_buff))) 
        {
            ret = -EFAULT;
            return ret;
        }
        priv_cmd.buf = compat_ptr(compat_priv_cmd.buf);
        priv_cmd.used_len = compat_priv_cmd.used_len;
        priv_cmd.total_len = compat_priv_cmd.total_len;
    } else
#endif
    if (copy_from_user(&priv_cmd, ifr->ifr_data, sizeof(android_usr_cmd_buff))) 
    {
        ret = -EFAULT;
        return ret;
    }
   
    if (!access_ok(VERIFY_READ, priv_cmd.buf, priv_cmd.total_len)) 
    {
        LOG_E("%s: failed to access memory\n", __FUNCTION__);
        ret = -EFAULT;
        return ret;
    }

    command = wf_kzalloc(priv_cmd.total_len);
    if (!command) 
     {
        LOG_E("%s: failed to allocate memory\n", __FUNCTION__);
        ret = -ENOMEM;
        return ret;
    }
    if (copy_from_user(command, (void *)priv_cmd.buf, priv_cmd.total_len)) 
    {
        ret = -EFAULT;
        wf_kfree(command);
        return ret;
    }

    LOG_I("%s: Android private cmd \"%s\" on %s\n", __FUNCTION__, priv_cmd.buf,ifr->ifr_name);

    for (cmd_num = 0; cmd_num < ANDROID_CMD_SIZE; cmd_num++)
    {
        int cmd_name_len = strlen(gl_android_cmd[cmd_num].cmd_name);
        if (0 ==strnicmp(command, gl_android_cmd[cmd_num].cmd_name,cmd_name_len ))
        {      
            bytes_written = gl_android_cmd[cmd_num].func(nic_info,command,priv_cmd.total_len,&gl_android_cmd[cmd_num]);
        }else if(cmd_num == ANDROID_CMD_SIZE){
            LOG_I("[%s]:Unknown PRIVATE command %s - ignored\n",__FUNCTION__,command);
            snprintf(command, 3, "OK");
            bytes_written = strlen("OK");
        }
     }
    
    if (bytes_written >= 0) 
     {
        if ((bytes_written == 0) && (priv_cmd.total_len > 0))
        {
            command[0] = '\0';
        }
        if (bytes_written >= priv_cmd.total_len) 
        {
            LOG_I("%s: bytes_written = %d\n", __FUNCTION__, bytes_written);
            bytes_written = priv_cmd.total_len;
        } 
        else 
        {
            bytes_written++;
        }
        
        priv_cmd.used_len = bytes_written;
        LOG_I("copy date to user: %s\n",command );
        if (copy_to_user((void *)priv_cmd.buf, command, bytes_written)) 
        {
            LOG_I("%s: failed to copy data to user buffer\n", __FUNCTION__);
            ret = -EFAULT;
        }
    }
    else 
    {
        ret = bytes_written;
    }

    wf_kfree(command);
    return ret;
}

