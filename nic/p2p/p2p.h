#ifndef __WF_P2P_H__
#define __WF_P2P_H__

#if 0
#define P2P_DBG(fmt, ...)      LOG_D("P2P[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define P2P_DBG(fmt, ...)
#endif


#include "p2p_timer.h"
#include "p2p_proto_mgt.h"
#include "p2p_frame_mgt.h"

#ifdef CONFIG_WFD
#include "p2p_wfd.h"
#endif

#ifdef CONFIG_P2P_WOWLAN
#include "p2p_wowlan.h"
#endif

enum DriverInterface 
{
    DRIVER_WEXT = 1,
    DRIVER_CFG80211 = 2
};
    
#define to_str(x) #x

#define WPSOUI                          0x0050f204
#define WPS_ATTR_VER1                   0x104A
#define WPS_ATTR_SIMPLE_CONF_STATE  0x1044
#define WPS_ATTR_RESP_TYPE          0x103B
#define WPS_ATTR_UUID_E             0x1047
#define WPS_ATTR_MANUFACTURER       0x1021
#define WPS_ATTR_MODEL_NAME         0x1023
#define WPS_ATTR_MODEL_NUMBER       0x1024
#define WPS_ATTR_SERIAL_NUMBER      0x1042
#define WPS_ATTR_PRIMARY_DEV_TYPE   0x1054
#define WPS_ATTR_SEC_DEV_TYPE_LIST  0x1055
#define WPS_ATTR_DEVICE_NAME            0x1011
#define WPS_ATTR_CONF_METHOD            0x1008
#define WPS_ATTR_RF_BANDS               0x103C
#define WPS_ATTR_DEVICE_PWID            0x1012
#define WPS_ATTR_REQUEST_TYPE           0x103A
#define WPS_ATTR_ASSOCIATION_STATE  0x1002
#define WPS_ATTR_CONFIG_ERROR           0x1009
#define WPS_ATTR_VENDOR_EXT         0x1049
#define WPS_ATTR_SELECTED_REGISTRAR 0x1041

#define WPS_MAX_DEVICE_NAME_LEN     32

#define WPS_REQ_TYPE_ENROLLEE_INFO_ONLY         0x00
#define WPS_REQ_TYPE_ENROLLEE_OPEN_8021X        0x01
#define WPS_REQ_TYPE_REGISTRAR                  0x02
#define WPS_REQ_TYPE_WLAN_MANAGER_REGISTRAR 0x03

#define WPS_RESPONSE_TYPE_INFO_ONLY 0x00
#define WPS_RESPONSE_TYPE_8021X     0x01
#define WPS_RESPONSE_TYPE_REGISTRAR 0x02
#define WPS_RESPONSE_TYPE_AP            0x03

#define WPS_WSC_STATE_NOT_CONFIG    0x01
#define WPS_WSC_STATE_CONFIG            0x02

#define WPS_VERSION_1                   0x10

#define WPS_CONFIG_METHOD_FLASH     0x0001
#define WPS_CONFIG_METHOD_ETHERNET  0x0002
#define WPS_CONFIG_METHOD_LABEL     0x0004
#define WPS_CONFIG_METHOD_DISPLAY   0x0008
#define WPS_CONFIG_METHOD_E_NFC     0x0010
#define WPS_CONFIG_METHOD_I_NFC     0x0020
#define WPS_CONFIG_METHOD_NFC       0x0040
#define WPS_CONFIG_METHOD_PBC       0x0080
#define WPS_CONFIG_METHOD_KEYPAD    0x0100
#define WPS_CONFIG_METHOD_VPBC      0x0280
#define WPS_CONFIG_METHOD_PPBC      0x0480
#define WPS_CONFIG_METHOD_VDISPLAY  0x2008
#define WPS_CONFIG_METHOD_PDISPLAY  0x4008

#define WPS_PDT_CID_DISPLAYS            0x0007
#define WPS_PDT_CID_MULIT_MEDIA     0x0008
#define WPS_PDT_CID_WLK_WIDI            WPS_PDT_CID_MULIT_MEDIA

#define WPS_PDT_SCID_MEDIA_SERVER   0x0005
#define WPS_PDT_SCID_WLK_DMP            WPS_PDT_SCID_MEDIA_SERVER

#define WPS_DPID_PIN                    0x0000
#define WPS_DPID_USER_SPEC          0x0001
#define WPS_DPID_MACHINE_SPEC           0x0002
#define WPS_DPID_REKEY                  0x0003
#define WPS_DPID_PBC                    0x0004
#define WPS_DPID_REGISTRAR_SPEC     0x0005

#define WPS_RF_BANDS_2_4_GHZ        0x01
#define WPS_RF_BANDS_5_GHZ      0x02

#define WPS_ASSOC_STATE_NOT_ASSOCIATED          0x00
#define WPS_ASSOC_STATE_CONNECTION_SUCCESS      0x01
#define WPS_ASSOC_STATE_CONFIGURATION_FAILURE   0x02
#define WPS_ASSOC_STATE_ASSOCIATION_FAILURE     0x03
#define WPS_ASSOC_STATE_IP_FAILURE              0x04

#define P2POUI                          0x506F9A09

#define P2P_ATTR_STATUS                 0x00
#define P2P_ATTR_MINOR_REASON_CODE      0x01
#define P2P_ATTR_CAPABILITY             0x02
#define P2P_ATTR_DEVICE_ID              0x03
#define P2P_ATTR_GO_INTENT              0x04
#define P2P_ATTR_CONF_TIMEOUT           0x05
#define P2P_ATTR_LISTEN_CH              0x06
#define P2P_ATTR_GROUP_BSSID                0x07
#define P2P_ATTR_EX_LISTEN_TIMING       0x08
#define P2P_ATTR_INTENTED_IF_ADDR       0x09
#define P2P_ATTR_MANAGEABILITY          0x0A
#define P2P_ATTR_CH_LIST                    0x0B
#define P2P_ATTR_NOA                        0x0C
#define P2P_ATTR_DEVICE_INFO                0x0D
#define P2P_ATTR_GROUP_INFO             0x0E
#define P2P_ATTR_GROUP_ID                   0x0F
#define P2P_ATTR_INTERFACE              0x10
#define P2P_ATTR_OPERATING_CH           0x11
#define P2P_ATTR_INVITATION_FLAGS       0x12

#define P2P_STATUS_SUCCESS                      0x00
#define P2P_STATUS_FAIL_INFO_UNAVAILABLE        0x01
#define P2P_STATUS_FAIL_INCOMPATIBLE_PARAM      0x02
#define P2P_STATUS_FAIL_LIMIT_REACHED           0x03
#define P2P_STATUS_FAIL_INVALID_PARAM           0x04
#define P2P_STATUS_FAIL_REQUEST_UNABLE          0x05
#define P2P_STATUS_FAIL_PREVOUS_PROTO_ERR       0x06
#define P2P_STATUS_FAIL_NO_COMMON_CH            0x07
#define P2P_STATUS_FAIL_UNKNOWN_P2PGROUP        0x08
#define P2P_STATUS_FAIL_BOTH_GOINTENT_15        0x09
#define P2P_STATUS_FAIL_INCOMPATIBLE_PROVSION   0x0A
#define P2P_STATUS_FAIL_USER_REJECT             0x0B

#define P2P_INVITATION_FLAGS_PERSISTENT         BIT(0)

#define DMP_P2P_DEVCAP_SUPPORT  (P2P_DEVCAP_SERVICE_DISCOVERY | \
                                    P2P_DEVCAP_CLIENT_DISCOVERABILITY | \
                                    P2P_DEVCAP_CONCURRENT_OPERATION | \
                                    P2P_DEVCAP_INVITATION_PROC)

#define DMP_P2P_GRPCAP_SUPPORT  (P2P_GRPCAP_INTRABSS)

#define P2P_DEVCAP_SERVICE_DISCOVERY        BIT(0)
#define P2P_DEVCAP_CLIENT_DISCOVERABILITY   BIT(1)
#define P2P_DEVCAP_CONCURRENT_OPERATION     BIT(2)
#define P2P_DEVCAP_INFRA_MANAGED            BIT(3)
#define P2P_DEVCAP_DEVICE_LIMIT             BIT(4)
#define P2P_DEVCAP_INVITATION_PROC          BIT(5)

#define P2P_GRPCAP_GO                           BIT(0)
#define P2P_GRPCAP_PERSISTENT_GROUP         BIT(1)
#define P2P_GRPCAP_GROUP_LIMIT              BIT(2)
#define P2P_GRPCAP_INTRABSS                 BIT(3)
#define P2P_GRPCAP_CROSS_CONN               BIT(4)
#define P2P_GRPCAP_PERSISTENT_RECONN        BIT(5)
#define P2P_GRPCAP_GROUP_FORMATION          BIT(6)

#define P2P_PUB_ACTION_ACTION               0x09

#define P2P_GO_NEGO_REQ                     0
#define P2P_GO_NEGO_RESP                        1
#define P2P_GO_NEGO_CONF                        2
#define P2P_INVIT_REQ                           3
#define P2P_INVIT_RESP                          4
#define P2P_DEVDISC_REQ                     5
#define P2P_DEVDISC_RESP                        6
#define P2P_PROVISION_DISC_REQ              7
#define P2P_PROVISION_DISC_RESP             8

#define P2P_NOTICE_OF_ABSENCE   0
#define P2P_PRESENCE_REQUEST        1
#define P2P_PRESENCE_RESPONSE   2
#define P2P_GO_DISC_REQUEST     3

#define P2P_MAX_PERSISTENT_GROUP_NUM        10

#define P2P_PROVISIONING_SCAN_CNT           3

#define P2P_WILDCARD_SSID_LEN               7

#define P2P_FINDPHASE_EX_NONE               0
#define P2P_FINDPHASE_EX_FULL               1
#define P2P_FINDPHASE_EX_SOCIAL_FIRST       (P2P_FINDPHASE_EX_FULL+1)
#define P2P_FINDPHASE_EX_MAX                    4
#define P2P_FINDPHASE_EX_SOCIAL_LAST        P2P_FINDPHASE_EX_MAX

#define P2P_PROVISION_TIMEOUT               5000
#define P2P_CONCURRENT_PROVISION_TIMEOUT    3000
#define P2P_GO_NEGO_TIMEOUT                 5000
#define P2P_CONCURRENT_GO_NEGO_TIMEOUT      3000
#define P2P_TX_PRESCAN_TIMEOUT              100
#define P2P_INVITE_TIMEOUT                  5000
#define P2P_CONCURRENT_INVITE_TIMEOUT       3000
#define P2P_RESET_SCAN_CH                       25000
#define P2P_MAX_INTENT                      15

#define P2P_MAX_NOA_NUM                     2

#define WPS_CM_NONE                         0x0000
#define WPS_CM_LABEL                            0x0004
#define WPS_CM_DISPLYA                      0x0008
#define WPS_CM_EXTERNAL_NFC_TOKEN           0x0010
#define WPS_CM_INTEGRATED_NFC_TOKEN     0x0020
#define WPS_CM_NFC_INTERFACE                    0x0040
#define WPS_CM_PUSH_BUTTON                  0x0080
#define WPS_CM_KEYPAD                       0x0100
#define WPS_CM_SW_PUHS_BUTTON               0x0280
#define WPS_CM_HW_PUHS_BUTTON               0x0480
#define WPS_CM_SW_DISPLAY_PIN               0x2008
#define WPS_CM_LCD_DISPLAY_PIN              0x4008


typedef enum P2P_ROLE 
{
    P2P_ROLE_DISABLE = 0,
    P2P_ROLE_DEVICE = 1,
    P2P_ROLE_CLIENT = 2,
    P2P_ROLE_GO = 3
}P2P_ROLE;


typedef enum P2P_STATE 
{
    P2P_STATE_NONE = 0,
    P2P_STATE_IDLE = 1,
    P2P_STATE_LISTEN = 2,
    P2P_STATE_SCAN = 3,
    P2P_STATE_FIND_PHASE_LISTEN = 4,
    P2P_STATE_FIND_PHASE_SEARCH = 5,
    P2P_STATE_TX_PROVISION_DIS_REQ = 6,
    P2P_STATE_RX_PROVISION_DIS_RSP = 7,
    P2P_STATE_RX_PROVISION_DIS_REQ = 8,
    P2P_STATE_GONEGO_ING = 9,
    P2P_STATE_GONEGO_OK = 10,
    P2P_STATE_GONEGO_FAIL = 11,
    P2P_STATE_RECV_INVITE_REQ_MATCH = 12,
    P2P_STATE_PROVISIONING_ING = 13,
    P2P_STATE_PROVISIONING_DONE = 14,
    P2P_STATE_TX_INVITE_REQ = 15,
    P2P_STATE_RX_INVITE_RESP_OK = 16,
    P2P_STATE_RECV_INVITE_REQ_DISMATCH = 17,
    P2P_STATE_RECV_INVITE_REQ_GO = 18,
    P2P_STATE_RECV_INVITE_REQ_JOIN = 19,
    P2P_STATE_RX_INVITE_RESP_FAIL = 20,
    P2P_STATE_RX_INFOR_NOREADY = 21,
    P2P_STATE_TX_INFOR_NOREADY = 22,
}P2P_STATE;

typedef enum P2P_WPSINFO 
{
    P2P_NO_WPSINFO = 0,
    P2P_GOT_WPSINFO_PEER_DISPLAY_PIN = 1,
    P2P_GOT_WPSINFO_SELF_DISPLAY_PIN = 2,
    P2P_GOT_WPSINFO_PBC = 3,
}P2P_WPSINFO;

#define P2P_PRIVATE_IOCTL_SET_LEN       64

typedef enum P2P_PROTO_WK_ID 
{
    P2P_FIND_PHASE_WK = 0,
    P2P_RESTORE_STATE_WK = 1,
    P2P_PRE_TX_PROVDISC_PROCESS_WK = 2,
    P2P_PRE_TX_NEGOREQ_PROCESS_WK = 3,
    P2P_PRE_TX_INVITEREQ_PROCESS_WK = 4,
    P2P_AP_P2P_CH_SWITCH_PROCESS_WK = 5,
    P2P_RO_CH_WK = 6,
}P2P_PROTO_WK_ID;

#define WFD_ATTR_DEVICE_INFO            0x00
#define WFD_ATTR_ASSOC_BSSID            0x01
#define WFD_ATTR_COUPLED_SINK_INFO  0x06
#define WFD_ATTR_LOCAL_IP_ADDR      0x08
#define WFD_ATTR_SESSION_INFO       0x09
#define WFD_ATTR_ALTER_MAC          0x0a

#define WFD_DEVINFO_SOURCE                  0x0000
#define WFD_DEVINFO_PSINK                   0x0001
#define WFD_DEVINFO_DUAL                    0x0003

#define WFD_DEVINFO_SESSION_AVAIL           0x0010
#define WFD_DEVINFO_WSD                     0x0040
#define WFD_DEVINFO_PC_TDLS                 0x0080
#define WFD_DEVINFO_HDCP_SUPPORT            0x0100


struct tx_provdisc_req_info 
{
    wf_u16 wps_config_method_request;
    wf_u16 peer_channel_num[2];
    wf_u8 ssidlen;
    wf_u8 ssid[WLAN_SSID_MAXLEN];
    wf_u8 peerDevAddr[WF_ETH_ALEN];
    wf_u8 peerIFAddr[WF_ETH_ALEN];
    wf_u8 benable;
};

struct rx_provdisc_req_info 
{
    wf_u8 peerDevAddr[WF_ETH_ALEN];
    wf_u8 strconfig_method_desc_of_prov_disc_req[4];
};

struct tx_invite_req_info 
{
    wf_u8 token;
    wf_u8 benable;
    wf_u8 go_ssid[WLAN_SSID_MAXLEN];
    wf_u8 ssidlen;
    wf_u8 go_bssid[WF_ETH_ALEN];
    wf_u8 peer_macaddr[WF_ETH_ALEN];
    wf_u8 operating_ch;
    wf_u8 peer_ch;
};

struct profile_info 
{
    wf_u8 ssidlen;
    wf_u8 ssid[WLAN_SSID_MAXLEN];
    wf_u8 peermac[WF_ETH_ALEN];
};

struct tx_invite_resp_info 
{
    wf_u8 token;
};

struct tx_nego_req_info 
{
    wf_u16 peer_channel_num[2];
    wf_u8 peerDevAddr[WF_ETH_ALEN];
    wf_u8 benable;
    wf_u8 peer_ch;
};

struct group_id_info 
{
    wf_u8 go_device_addr[WF_ETH_ALEN];
    wf_u8 ssid[WLAN_SSID_MAXLEN];
};

struct scan_limit_info 
{
    wf_u8 scan_op_ch_only;
#ifndef CONFIG_P2P_OP_CHK_SOCIAL_CH
    wf_u8 operation_ch[2];
#else
    wf_u8 operation_ch[5];
#endif
};



struct wifidirect_info 
{
    
    struct tx_provdisc_req_info tx_prov_disc_info;
    struct rx_provdisc_req_info rx_prov_disc_info;
    struct tx_invite_req_info invitereq_info;
    struct profile_info profileinfo[P2P_MAX_PERSISTENT_GROUP_NUM];
    struct tx_invite_resp_info inviteresp_info;
    struct tx_nego_req_info nego_req_info;
    struct group_id_info groupid_info;
    struct scan_limit_info rx_invitereq_info;
    struct scan_limit_info p2p_info;
#ifdef CONFIG_WFD
    struct wifi_display_info wfd_info;
#endif

#ifdef CONFIG_P2P_WOWLAN
    struct p2p_wowlan_info p2p_wow_info;
#endif

    enum P2P_ROLE role;
    enum P2P_STATE pre_p2p_state;
    enum P2P_STATE p2p_state;
    wf_u8 device_addr[WF_ETH_ALEN];
    wf_u8 interface_addr[WF_ETH_ALEN];
    wf_u8 social_chan[4];
    wf_u8 listen_channel;
    wf_u8 operating_channel;
    wf_u8 listen_dwell;
    wf_u8 p2p_support_rate[8];
    wf_u8 p2p_wildcard_ssid[P2P_WILDCARD_SSID_LEN];
    wf_u8 intent;
    wf_u8 p2p_peer_interface_addr[WF_ETH_ALEN];
    wf_u8 p2p_peer_device_addr[WF_ETH_ALEN];
    wf_u8 peer_intent;
    wf_u8 device_name[WPS_MAX_DEVICE_NAME_LEN];
    wf_u8 device_name_len;
    wf_u8 profileindex;
    wf_u8 peer_operating_ch;
    wf_u8 find_phase_state_exchange_cnt;
    wf_u16 device_password_id_for_nego;
    wf_u8 negotiation_dialog_token;
    wf_u8 nego_ssid[WLAN_SSID_MAXLEN];
    wf_u8 nego_ssidlen;
    wf_u8 p2p_group_ssid[WLAN_SSID_MAXLEN];
    wf_u8 p2p_group_ssid_len;
    wf_u8 persistent_supported;
    wf_u8 session_available;

    enum P2P_WPSINFO ui_got_wps_info;
    wf_u16 supported_wps_cm;
    wf_u8 external_uuid;
    wf_u8 uuid[16];
    wf_u32 channel_list_attr_len;
    wf_u8 channel_list_attr[100];
    wf_u8 driver_interface;

#ifdef CONFIG_CONCURRENT_MODE
    wf_u16 ext_listen_interval;
    wf_u16 ext_listen_period;
#endif

};

typedef struct p2p_info_st_
{
    void *nic_info;
    wf_os_api_timer_t find_phase_timer;
    wf_os_api_timer_t restore_p2p_state_timer;

    wf_os_api_timer_t pre_tx_scan_timer;
    wf_os_api_timer_t reset_ch_sitesurvey;
    wf_os_api_timer_t reset_ch_sitesurvey2;
    wf_os_api_timer_t ap_p2p_switch_timer;

    struct wifidirect_info wdinfo;

    wf_u8 *p2p_beacon_ie;
    wf_u8 *p2p_probe_req_ie;
    wf_u8 *p2p_probe_resp_ie;
    wf_u8 *p2p_go_probe_resp_ie;
    wf_u8 *p2p_assoc_req_ie;
    wf_u8 *p2p_assoc_resp_ie;

    wf_u32 p2p_beacon_ie_len;
    wf_u32 p2p_probe_req_ie_len;
    wf_u32 p2p_probe_resp_ie_len;
    wf_u32 p2p_go_probe_resp_ie_len;
    wf_u32 p2p_assoc_req_ie_len;
    wf_u32 p2p_assoc_resp_ie_len;
    
    wf_u8 p2p_enabled;
    wf_u8 mgnt_tx_rate;
}p2p_info_st;

wf_s32 p2p_info_init(nic_info_st *nic_info);
wf_s32 p2p_info_term(nic_info_st *nic_info);

wf_s32 wf_p2p_enable(nic_info_st *nic_info,P2P_ROLE role);
wf_s32 wf_p2p_disable(nic_info_st *nic_info);


void p2p_set_role(struct wifidirect_info *wdinfo, enum P2P_ROLE role);
void p2p_set_state(struct wifidirect_info *wdinfo, enum P2P_STATE state);
void p2p_set_pre_state(struct wifidirect_info *wdinfo, enum P2P_STATE state);

char *p2p_state_to_str(P2P_STATE state);

#endif
