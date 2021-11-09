#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL  (~WF_DEBUG_MASK)
#include "common.h"
#include "wf_80211.h"

#include "utils/os.h"
#include "sec/utils/common.h"
#include "common/defs.h"
#include "common/wpa_common.h"
#include "crypto/sha1.h"
#include "wifi_inf.h"
#include "cfg80211.h"
#include "sec/wpa.h"


int wf_sta_hw_set_unicast_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = 4 + pwdn_info->wdn_id;
    ctrl = BIT(15) | (pwdn_info->dot118021XPrivacy << 2);
    ret = wf_mcu_set_sec_cam(pnic_info,
                             cam_id, ctrl,
                             pwdn_info->mac, pwdn_info->dot118021x_UncstKey.skey);

    return ret;
}

int wf_sta_hw_set_group_key(nic_info_st *pnic_info, wdn_net_info_st *pwdn_info)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wf_u8 cam_id;
    wf_u16 ctrl;
    int ret;

    cam_id = psec_info->dot118021XGrpKeyid & 0x03; /* cam_id0~3 8021x group key */
    ctrl = BIT(15) | BIT(6) |
           (psec_info->dot118021XGrpPrivacy << 2) |
           psec_info->dot118021XGrpKeyid;
    ret = wf_mcu_set_sec_cam(pnic_info, cam_id, ctrl,
                             pwdn_info->bssid,
                             psec_info->dot118021XGrpKey[psec_info->dot118021XGrpKeyid].skey);
    wf_mcu_set_on_rcr_am(pnic_info, wf_true);

    return ret;
}

int wf_sta_set_encryption(nic_info_st *pnic_info,
                          ieee_param *param, wf_u32 param_len)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    wdn_net_info_st *pwdn_info;
    int res = 0;


    param->u.crypt.err = 0;
    param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

    if (param_len !=
        WF_OFFSETOF(ieee_param, u.crypt.key) + param->u.crypt.key_len)
    {
        res = -22;
        goto exit;
    }

    if (is_bcast_addr(param->sta_addr))
    {
        if (param->u.crypt.idx >= WEP_KEYS)
        {
            res = -22;
            goto exit;
        }
    }
    else
    {
        res = -22;
        goto exit;
    }
    wf_mcu_set_sec_cfg(pnic_info,
                       psec_info->dot11AuthAlgrthm ==
                       dot11AuthAlgrthm_8021X ? 0xcf : 0xcc);

    if (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) /* 802_1x */
    {
        local_info_st *plocal_info = pnic_info->local_info;
        if (plocal_info->work_mode == WF_INFRA_MODE) /* sta mode */
        {
            pwdn_info = wf_wdn_find_info(pnic_info,
                                         wf_wlan_get_cur_bssid(pnic_info));
            if (pwdn_info == NULL)
            {
                goto exit;
            }

            if (os_strcmp((const char *)param->u.crypt.alg, "none") != 0)
            {
                pwdn_info->ieee8021x_blocked = wf_false;
            }

            if (psec_info->ndisencryptstatus == wf_ndis802_11Encryption2Enabled ||
                psec_info->ndisencryptstatus == wf_ndis802_11Encryption3Enabled)
            {
                pwdn_info->dot118021XPrivacy = psec_info->dot11PrivacyAlgrthm;
            }

            /* PTK: param->u.crypt.key */
            if (param->u.crypt.set_tx == 1) /* pairwise key */
            {
                /* KCK PTK0~127 */
                wf_memcpy(pwdn_info->dot118021x_UncstKey.skey, param->u.crypt.key,
                          min(param->u.crypt.key_len, 16));

                if (os_strcmp((const char *)param->u.crypt.alg, "TKIP") == 0) /* set mic key */
                {
                    /* KEK PTK128~255 */
                    wf_memcpy(pwdn_info->dot11tkiptxmickey.skey,
                              &(param->u.crypt.key[16]), 8); /* PTK128~191 */
                    wf_memcpy(pwdn_info->dot11tkiprxmickey.skey,
                              &(param->u.crypt.key[24]), 8); /* PTK192~255 */
                    psec_info->busetkipkey = wf_true;
                }
                if (pwdn_info->dot118021XPrivacy == _AES_)
                {

                    wf_sta_hw_set_unicast_key(pnic_info, pwdn_info);
                }
            }
            else /* group key */
            {
                wf_memcpy(psec_info->dot118021XGrpKey[param->u.crypt.idx].skey,
                          param->u.crypt.key,
                          min( param->u.crypt.key_len, 16));
                wf_memcpy(psec_info->dot118021XGrptxmickey[param->u.crypt.idx].skey,
                          &param->u.crypt.key[16], 8);
                wf_memcpy(psec_info->dot118021XGrprxmickey[param->u.crypt.idx].skey,
                          &param->u.crypt.key[24], 8);
                psec_info->binstallGrpkey = TRUE;
                psec_info->dot118021XGrpKeyid = param->u.crypt.idx;
                if (psec_info->dot118021XGrpPrivacy == _AES_)
                {
                    wf_sta_hw_set_group_key(pnic_info, pwdn_info);
                }

            }
        }
    }

exit:
    return res;
}


int wf_key(nic_info_st *pnic_info,
           wf_u8 key_index, const wf_u8 * mac_addr,
           struct key_params *params)
{

    char *alg_name;
    wf_u32 param_len;
    ieee_param *param = NULL;
    int res = 0;
    printf("%s\n",__func__);
    param_len = WF_OFFSETOF(ieee_param, u.crypt.key) + params->key_len;
    param = (ieee_param *)wf_vmalloc(param_len);
    if (param == NULL)
    {
        res = -1;
        goto exit;
    }
    memset(param, 0, param_len);

    memset(param->sta_addr, 0xff, ETH_ALEN);

    switch (params->cipher)
    {
        case IW_AUTH_CIPHER_NONE:
            alg_name = "none";
            break;

        case WLAN_CIPHER_SUITE_WEP40:
        case WLAN_CIPHER_SUITE_WEP104:
            alg_name = "WEP";
            break;

        case WLAN_CIPHER_SUITE_TKIP:
            alg_name = "TKIP";
            break;

        case WLAN_CIPHER_SUITE_CCMP:
            alg_name = "CCMP";
            break;

        default:
            res = -1;
            goto exit;
    }

    strncpy((char *)param->u.crypt.alg, alg_name, IEEE_CRYPT_ALG_NAME_LEN);

    if (mac_addr == NULL || is_bcast_addr(mac_addr))
    {
        param->u.crypt.set_tx = 0;
    }
    else
    {
        param->u.crypt.set_tx = 1;
    }

    param->u.crypt.idx = key_index;

    if (params->seq_len && params->seq)
    {
        wf_memcpy(param->u.crypt.seq, (wf_u8 *) params->seq, params->seq_len);
    }

    if (params->key_len && params->key)
    {
        param->u.crypt.key_len = params->key_len;
        wf_memcpy(param->u.crypt.key, (wf_u8 *) params->key, params->key_len);
    }
    if (wf_local_cfg_get_work_mode(pnic_info) == WF_INFRA_MODE)
    {
        res = wf_sta_set_encryption(pnic_info, param, param_len);
    }

    else
    {
    }

exit :
    if (param)
    {
        wf_vfree((wf_u8 *)param);
    }

    return res;

}

