#ifndef __NIC_H__
#define __NIC_H__


typedef enum
{
    NIC_USB     = 1,
    NIC_SDIO    = 2,
} nic_type_e;

typedef struct
{
    wf_u8  fw_usb_rom_type;
    wf_u32 fw0_usb_size;
    wf_u32 fw1_usb_size;
    char  *fw0_usb;
    char  *fw1_usb;

    wf_u8  fw_sdio_rom_type;
    wf_u32 fw0_sdio_size;
    wf_u32 fw1_sdio_size;
    char  *fw0_sdio;
    char  *fw1_sdio;
} fwdl_info_t;


#ifdef CONFIG_IOCTL_CFG80211
typedef struct wf_widev_invit_info
{
    wf_u8 state;
    wf_u8 peer_mac[ETH_ALEN];
    wf_u8 active;
    wf_u8 token;
    wf_u8 flags;
    wf_u8 status;
    wf_u8 req_op_ch;
    wf_u8 rsp_op_ch;
} wf_widev_invit_info_t;

#define wf_widev_invit_info_init(invit_info) \
    do { \
        (invit_info)->state = 0xff; \
        wf_memset((invit_info)->peer_mac, 0, ETH_ALEN); \
        (invit_info)->active = 0xff; \
        (invit_info)->token = 0; \
        (invit_info)->flags = 0x00; \
        (invit_info)->status = 0xff; \
        (invit_info)->req_op_ch = 0; \
        (invit_info)->rsp_op_ch = 0; \
    } while (0)

typedef struct wf_widev_nego_info
{
    wf_u8 state;
    wf_u8 peer_mac[ETH_ALEN];
    wf_u8 active;
    wf_u8 token;
    wf_u8 status;
    wf_u8 req_intent;
    wf_u8 req_op_ch;
    wf_u8 req_listen_ch;
    wf_u8 rsp_intent;
    wf_u8 rsp_op_ch;
    wf_u8 conf_op_ch;
} wf_widev_nego_info_t;

#define wf_widev_nego_info_init(nego_info) \
    do { \
        (nego_info)->state = 0xff; \
        wf_memset((nego_info)->peer_mac, 0, ETH_ALEN); \
        (nego_info)->active = 0xff; \
        (nego_info)->token = 0; \
        (nego_info)->status = 0xff; \
        (nego_info)->req_intent = 0xff; \
        (nego_info)->req_op_ch = 0; \
        (nego_info)->req_listen_ch = 0; \
        (nego_info)->rsp_intent = 0xff; \
        (nego_info)->rsp_op_ch = 0; \
        (nego_info)->conf_op_ch = 0; \
    } while (0)

typedef struct wf_widev_priv
{
    struct wireless_dev *pwidev;
    void *pnic_info;

    struct cfg80211_scan_request *pscan_request;
    wf_lock_spin scan_req_lock;

    struct net_device *pmon_ndev;
    char ifname_mon[IFNAMSIZ + 1];

    wf_u8 provdisc_req_issued;

    wf_widev_invit_info_t invit_info;
    wf_widev_nego_info_t nego_info;

	wf_bool bandroid_scan;
    wf_bool block;
    wf_bool block_scan;
    wf_bool power_mgmt;

    wf_u16 report_mgmt;

#ifdef CONFIG_CONCURRENT_MODE
	atomic_t ro_ch_to;
	atomic_t switch_ch_to;
#endif
} wf_widev_priv_t;
#endif

typedef struct nic_info
{
    /*hif handle*/
    void *hif_node;
    int   hif_node_id;

    /*device handle*/
    void *dev;

    /*ndev handle*/
    void *ndev;
    wf_u8 ndev_id;
    wf_u8 ndev_num;

#ifdef CONFIG_IOCTL_CFG80211
//    struct wiphy *pwiphy;
//    struct wireless_dev *pwidev;
//    struct wf_widev_priv widev_priv;
    void *pwiphy;
    void *pwidev;
    struct wf_widev_priv widev_priv;
#endif

    /*nic attr*/
    nic_type_e nic_type;
    wf_bool virNic;
	wf_bool is_surprise_removed;
	wf_bool is_driver_stopped;
	wf_bool is_up;
    wf_os_api_sema_t cmd_sema;
    wf_bool cmd_lock_use;
    wf_u32  nic_state;
    fwdl_info_t fwdl_info;
    wf_u32 setband;

    /*wdn*/
    void *wdn;

    /*nic hw*/
    void *hw_info;

    /*nic local cfg*/
    void *local_info;

	/*nic odm message*/
	void *odm;

    /*nic mlme*/
    void *mlme_info;

    /*nic scan*/
    void *scan_info;

    /*nic auth*/
    void *auth_info;

    /*nic sec */
    void *sec_info;

	/*nic pm*/
	void *pwr_info;

    /*nic assoc*/
    void *assoc_info;

    /*wlan info*/
    void *wlan_info;

    /*sta info*/
    void *sta_info;

    /*tx info*/
    void *tx_info;

    /*rx info*/
    void *rx_info;

	/*iw states*/
	void *iwstats;

	/* mp info */
	void *mp_info;

    /*p2p function info*/
    void *p2p;

    /*AdaptiveRateSystem*/
    void *ars;
    
    /* check flag */
    int func_check_flag; //0xAA55BB66

    /*nic read/write reg */
    int (*nic_write)(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*nic_read)(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);

    /*nic write data */
	int (*nic_tx_queue_insert)(void *node,wf_u8 agg_num, char *buff, wf_u32 buff_len, wf_u32 addr,
							int (*tx_callback_func)(void *tx_info, void *param), void *tx_info, void *param);
    int (*nic_tx_queue_empty)(void *node);

    /*nic write firmware */
    int (*nic_write_fw)(void *node, wf_u8 which,  wf_u8 *firmware, wf_u32 len);

    /*nic write cmd */
    int (*nic_write_cmd)(void *node, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);

	/*nic read cfg txt*/
	int (*nic_cfg_file_read)(void *pnic_info);
	wf_u32 nic_num;
#ifdef CONFIG_CONCURRENT_MODE
    void *vir_nic;
#endif
} nic_info_st;

int nic_init(nic_info_st *nic_info);
int nic_term(nic_info_st *nic_info);
int nic_enable(nic_info_st *nic_info);
int nic_disable(nic_info_st *nic_info);
int nic_suspend(nic_info_st *nic_info);
int nic_resume(nic_info_st *nic_info);
int nic_shutdown(nic_info_st *nic_info);
wf_u8 *nic_to_local_addr(nic_info_st *nic_info);

#define wf_is_surprise_removed(nic_info)	((nic_info->is_surprise_removed) == wf_true)
#define wf_is_drv_stopped(nic_info)		    ((nic_info->is_driver_stopped) == wf_true)

#define WF_CANNOT_RUN(nic_info)     (wf_is_surprise_removed(nic_info) || wf_is_drv_stopped(nic_info))


#endif

