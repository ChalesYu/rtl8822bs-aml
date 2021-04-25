#include "common.h"
#include "wf_debug.h"

#define MCU_LINKED           (1)
#define MCU_UNLINKED         (0)

#define _HW_STATE_NOLINK_       0x00
#define _HW_STATE_ADHOC_        0x01
#define _HW_STATE_STATION_      0x02
#define _HW_STATE_AP_           0x03
#define _HW_STATE_MONITOR_      0x04
#define _HW_STATE_NO_EXIST_     0xAA

#ifndef BIT
#define BIT(x)  ( 1 << (x))
#endif

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000
#define BIT32   0x0100000000
#define BIT33   0x0200000000
#define BIT34   0x0400000000
#define BIT35   0x0800000000
#define BIT36   0x1000000000

#define REG_BSSID0                      0x0618
#define REG_BSSID1                      0x0708

#define RND4(x) (((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)




int wf_mcu_cmd_get_status(nic_info_st *nic_info)
{
    int ret;
    /*test base on hisilicon platform, it would need 25000*/
    wf_u32 timeout = 25000; //2000->8000->15000(for GK7202 , get efuse, dw fw, )
    wf_u32 data = 0;
    wf_u32 tryCnt = 0;

    // set mailbox int finish
    ret = wf_io_write32(nic_info, WF_MAILBOX_INT_FINISH, 0x12345678);
    if ( WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_MAILBOX_INT_FINISH failed, check!!!", __func__);
        return ret;
    }

    // set mailbox triger int
    ret = wf_io_write8(nic_info, WF_MAILBOX_REG_INT, 1);
    if ( WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_MAILBOX_REG_INT failed, check!!!", __func__);
        return ret;
    }

    while (timeout--)
    {
        int err = 0;
        if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
            return WF_RETURN_FAIL;

        tryCnt++;
        data = wf_io_read32(nic_info, WF_MAILBOX_INT_FINISH,&err);
        if(err)
        {
            LOG_E("[%s] read failed,err:%d",__func__,err);
            break;
        }
        if ( NIC_USB == nic_info->nic_type && 0x55 == data)
        {
            //LOG_D("MCU Feedback [tryCnt:%d]",tryCnt);
            return WF_RETURN_OK;

        }
        else if (NIC_SDIO == nic_info->nic_type && 0x000000aa == data)
        {

            //LOG_D("MCU Feedback [tryCnt:%d]",tryCnt);
            return WF_RETURN_OK;

        }

        //wf_msleep(1);
    }

    LOG_I("timeout !!!  data:0x%x", data);
    return WF_RETURN_FAIL;
}


/*************************************************
 * Function     : wf_mcu_get_chip_version
 * Description  : To get the version of this chip
 * Input        : nic_info
 * Output       : version
 * Return       : 1. WF_RETURN_FAIL
                  2. WF_RETURN_OK
 *************************************************/

int wf_mcu_get_chip_version(nic_info_st *nic_info, wf_u32 *version)
{

    int ret   = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_READ_VERSION, NULL, 0, version, 1);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_READ_VERSION, NULL, 0, version, 1);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] READ_VERSION failed,check!!!", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


/*************************************************
* Function     : wf_mcu_set_concurrent_mode
* Description  :
* Input        : nic_info
* Output       :
* Return       : 1. WF_RETURN_FAIL, function work fail
                 2. WF_RETURN_OK, function work well
*************************************************/
int wf_mcu_set_concurrent_mode(nic_info_st *nic_info, wf_bool concur_mode)
{
    int ret = 0;

    int mode    = 0;

    if (concur_mode == wf_true)
    {
        mode = 1;
    }

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE, &mode, 1, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_CONFIG_CONCURRENT_MODE, &mode, 1, NULL, 0);

    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE error");
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

/*************************************************
* Function     : wf_mcu_cmd_term
* Description  :
* Input        : nic_info
* Output       :
* Return       : 1. WF_RETURN_FAIL, function work fail
                 2. WF_RETURN_OK, function work well
*************************************************/
int wf_mcu_cmd_term(nic_info_st *nic_info)
{
    int ret                 = 0;
    LOG_I("[%s] %p", __func__, nic_info);
    if ( NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_DEINIT, NULL, 0, NULL, 0);
    }
    else
    {
        //ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_DEINIT,NULL,0,NULL,0);
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("UMSG_OPS_HAL_DEINIT error");
        return WF_RETURN_FAIL;
    }
    return WF_RETURN_OK;
}


/*************************************************
* Function     : wf_mcu_init_hardware1
* Description  :
* Input        : nic_info
* Output       :
* Return       : 1. WF_RETURN_FAIL, function work fail
                 2. WF_RETURN_OK, function work well
*************************************************/
int wf_mcu_init_hardware1(nic_info_st *nic_info)
{
    int ret = 0;

    wf_u32 is_dw    = 1;

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_INIT_STEP0, &is_dw, 1, NULL, 0);
    }
    else
    {
        wf_u32 u4Tmp[5] = { 0 };
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_GET_BCN_PAR, &is_dw, 1, u4Tmp, 5);
        LOG_I("[%s] BcnCtrlVal:%d ", __func__,u4Tmp[0]);
        LOG_I("[%s] TxPause:%d ", __func__,u4Tmp[1]);
        LOG_I("[%s] FwHwTxQCtrl:%d ", __func__,u4Tmp[2]);
        LOG_I("[%s] TbttR:%d ", __func__,u4Tmp[3]);
        LOG_I("[%s] CR_1:%d ", __func__,u4Tmp[4]);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}


/*************************************************
* Function     : wf_mcu_burst_pktlen_init
* Description  :
* Input        : 1. nic_info
                 2. pktdata,this can be true or false
                 3. len, 'pktdata' length, here is 2
* Output       :
* Return       : 1. WF_RETURN_FAIL, function work fail
                 2. WF_RETURN_OK, function work well
*************************************************/
int wf_mcu_burst_pktlen_init(nic_info_st *nic_info)
{
    int ret = 0;

    wf_u32 u4Tmp[2] = { 0 };


    if (NIC_USB == nic_info->nic_type)
    {
        //ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_INIT_STEP0,&is_dw,1,NULL,0);
    }
    else
    {
        u4Tmp[0] = 0;
        u4Tmp[1] = 1;//1. normal chip
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_INIT_PKT_LEN, u4Tmp, 2, NULL, 0);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}


int wf_mcu_ant_sel_init(nic_info_st *nic_info)
{
    int ret = 0;
    wf_u32  tmp = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        //ret = mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_INIT_STEP0,&is_dw,1,NULL,0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_INIT_ANT_SEL, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }

    return WF_RETURN_OK;
}


int wf_mcu_update_tx_fifo(nic_info_st *nic_info)
{
    int ret = 0;
    if (NIC_USB == nic_info->nic_type)
    {
        ret =   mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_UPDATE_TX_FIFO, NULL, 0, NULL, 0);
    }
    else
    {
        ret =   mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_GENXIN_TX_FIFO, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_UPDATE_TX_FIFO failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

int wf_mcu_update_thermal(nic_info_st *nic_info)
{
    int ret = 0;
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_UPDATE_THERMAL, NULL, 0, NULL, 0);
    }
    else
    {
        // todo
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_UPDATE_THERMAL failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

#ifdef CONFIG_CONCURRENT_MODE
int wf_mcu_get_partner_fwstate(nic_info_st *pnic_info)
{
	nic_info_st *pvir_nic = pnic_info->vir_nic;
	if(pnic_info == NULL)
	{
		return WIFI_FW_NO_EXIST;
	}
	if(pvir_nic == NULL)
	{
		return WIFI_FW_NO_EXIST;
	}

	return pvir_nic->nic_state;
}
#endif

#ifdef CONFIG_LPS
void wf_mcu_lps_config(nic_info_st * pnic_info) 
{
    wf_u32 arg[2];
    pwr_info_st *pwr_info = pnic_info->pwr_info;
    arg[0] = pwr_info->smart_lps;
    arg[1] = pwr_info->pwr_mgnt;
    mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_LPS_CONFIG, arg, 2, NULL, 0);
}

void wf_mcu_set_fw_lps_config(nic_info_st *pnic_info)
{
    wf_u32 aid = 0;
    wf_wlan_info_t *wlan_info = (wf_wlan_info_t *)pnic_info->wlan_info;
    wf_wlan_network_t *cur_network = &(wlan_info->cur_network);

    aid = (wf_u32)cur_network->aid;

    mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_LPS_SET, &aid, 1, NULL, 0);
}

#endif

int wf_mcu_set_op_mode(nic_info_st *nic_info, wf_u32 mode)
{
    int ret = 0;

    wf_u8 val8;
    wf_u32 tmp[5] = { 0 };
    wf_u32 hw_mode;
    mlme_state_e state;
    wf_bool bConnect;
    wf_u32 mlmeState;
    wf_u32 fwState;

    if ((nic_info->is_driver_stopped == wf_true) || (nic_info->is_surprise_removed == wf_true))
    {
        return WF_RETURN_FAIL;
    }

    wf_mlme_get_connect(nic_info, &bConnect);
    if (bConnect == wf_true)
    {
        mlmeState = _HW_STATE_STATION_;
        fwState = WIFI_STATION_STATE | WIFI_ASOC_STATE;
    }
    else
    {
        mlmeState = _HW_STATE_NO_EXIST_;
        fwState =  WIFI_FW_NO_EXIST;
    }

    switch (mode)
    {
        /* STA mode */
        case WF_AUTO_MODE:
        case WF_INFRA_MODE:
            tmp[0] = _HW_STATE_STATION_;
            break;

        /* AdHoc mode */
        case WF_ADHOC_MODE:
            tmp[0] = _HW_STATE_ADHOC_;
            break;

        /* AP mode */
        case WF_MASTER_MODE:
            tmp[0] = _HW_STATE_AP_;
            break;

        /* Sniffer mode */
        case WF_MONITOR_MODE:
            tmp[0] = _HW_STATE_MONITOR_;
            tmp[4]  = BIT0|BIT1|BIT2|BIT3|BIT5|BIT8|BIT11|BIT12|BIT13;
            break;

        case WF_REPEAT_MODE:
        case WF_SECOND_MODES:
        case WF_MESH_MODE:
        default:
        {
            LOG_E("Unsupport Mode!!");
            return WF_RETURN_FAIL;
        }
        break;
    }
#ifdef CONFIG_CONCURRENT_MODE
    tmp[1] = nic_info->nic_num; //iface: 0 or 1
#else
    tmp[1] = 0; //iface: 0 or 1
#endif
    tmp[2] = mlmeState; //get mlme state
#ifdef CONFIG_CONCURRENT_MODE
    tmp[3] = wf_mcu_get_partner_fwstate(nic_info);
#else
    tmp[3] = fwState;    //get fw stste
#endif
    tmp[4] = WF_BIT(0) | WF_BIT(2) | WF_BIT(31);
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_OP_MODE, tmp, 5, NULL, 0);
    }
    else
    {
        tmp[4] = WF_BIT(0) | WF_BIT(2) | WF_BIT(31);
        LOG_I("[%s] 0x%x,0x%x,0x%x,0x%x,0x%x",__func__,tmp[0],tmp[1],tmp[2],tmp[3],tmp[4]);
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HW_SET_OP_MODE, tmp, 5, NULL, 0);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("===>%s,fail!\n", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;

}


int wf_mcu_set_ch_bw(nic_info_st *nic_info, wf_u32 *args, wf_u32 arg_len)
{
    int ret = 0;
    wf_u32  tmp = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_CHNLBW_MODE, args, arg_len, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_CHNLBW_MODE, args, arg_len, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return ret;
    }

    /*some kind of register operation*/
    //LOG_I("To do");

    return WF_RETURN_OK;


}


int  wf_mcu_set_hw_reg(nic_info_st *nic_info, wf_u32 *value, wf_u32 len)
{
    int  ret = WF_RETURN_OK;
    if (len > MAILBOX_MAX_TXLEN)
    {
        LOG_E("%s len = %d is bigger than MAILBOX_MAX_TXLEN(%d), check!!!! ", __func__,  len, MAILBOX_MAX_TXLEN);
        return WF_RETURN_FAIL;
    }


    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_SET_HWREG, value, len, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_SET_HWREG, value, len, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_SET_HWREG[%d] failed, check!!!!", __func__, value[0]);
        return WF_RETURN_FAIL;
    }

    return ret;
}


int wf_mcu_set_hw_invalid_all(nic_info_st *nic_info)
{
    int ret = 0;
    wf_u32 arg[2] = {0};
    hw_info_st *hw_info = (hw_info_st *)nic_info->hw_info;
    arg[0] = HW_VAR_CAM_INVALID_ALL;
    arg[1] = 0;
    hw_info->hw_reg.cam_invalid = arg[1];

    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}


int wf_mcu_set_config_xmit(nic_info_st *nic_info, int event, wf_u32 val)
{
    wf_u32 temp;
    int ret = 0;
    temp = wf_io_read32(nic_info, WF_XMIT_CTL,NULL);

    if (event & WF_XMIT_AGG_MAXNUMS)
    {
        temp = temp & 0x07FFFFFF;
        val = val & 0x1F;
        temp = temp | (val << 27);
    }

    if (event & WF_XMIT_AMPDU_DENSITY)
    {
        temp = temp & 0xFFFF1FFF;
        val = val & 0x07;
        temp = temp | (val << 13);

    }

    if (event & WF_XMIT_OFFSET)
    {
        temp = temp & 0xFFFFFF00;
        val = val & 0xFF;
        temp = temp | (val << 0);
    }

    if (event & WF_XMIT_PKT_OFFSET)
    {
        temp = temp & 0xFFFFE0FF;
        val = val & 0x1F;
        temp = temp | (val << 8);
    }

    ret  = wf_io_write32(nic_info, WF_XMIT_CTL, temp);

    return WF_RETURN_OK;
}



int  wf_mcu_set_no_filter(nic_info_st *nic_info)
{
    int ret = 0;
    int var;

    if (NIC_USB == nic_info->nic_type)
    {
        var = 1;
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_NOTCH_FILTER, &var, 1, NULL, 0);
    }
    else
    {
        //todo
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HAL_PHY_IQ_CALIBRATE failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

// use for control ODM
int  wf_mcu_set_user_info(nic_info_st *nic_info, wf_bool state)
{
    int ret = 0;
    int var;

    #ifdef CONFIG_RICHV200_FPGA
    if(state) {
        var = 0x000000001;
    } else {
        var = 0x00000000;
    }

    if ((wf_mlme_check_mode(nic_info, WF_INFRA_MODE) == wf_true)) {
        var |= 0x000000008;
    }
    else if ((wf_mlme_check_mode(nic_info, WF_MASTER_MODE) == wf_true)) {
        var |= 0x000000010;
    }
    else if (wf_mlme_check_mode(nic_info, WF_ADHOC_MODE) == wf_true)
    {
        var |= 0x000000020;
    } else {
        LOG_E("[%s]:not support work mode", __func__);
        return WF_RETURN_FAIL;
    }

    ret = mcu_cmd_communicate(nic_info, UMSG_OPS_MP_USER_INFO, &var, 1, NULL, 0);

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_MP_USER_INFO failed", __func__);
        return WF_RETURN_FAIL;
    }
    #else

    if (state == wf_false)
    {
        // ODM disable
        wf_set_odm_dig(nic_info,wf_true,0x26);
        wf_odm_set_ability(nic_info, ODM_FUNC_BACKUP, 0);
    }
    else
    {
        //ODM enable
        wf_odm_set_ability(nic_info, ODM_FUNC_RESTORE, 0);
        wf_set_odm_dig(nic_info,wf_true,0xff);
    }

    #endif

    return WF_RETURN_OK;
}

int wf_mcu_enable_xmit(nic_info_st *nic_info)
{
    return WF_RETURN_OK;
}


int wf_mcu_disable_xmit(nic_info_st *nic_info)
{
    return WF_RETURN_OK;
}


int wf_mcu_init_hardware2(nic_info_st *nic_info, hw_param_st *param)
{
    int ret = 0;   

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_FW_INIT, param->send_msg, 1, param->recv_msg, 9);
    }
    else
    {
        hw_info_st *hwinfo = (hw_info_st *)nic_info->hw_info;
        int i = 0;
        LOG_D("arg[0]:0x%x\n", param->send_msg[0]);
        for (i = 0; i < WF_ETH_ALEN; i++)
        {
            param->send_msg[i + 1] = hwinfo->macAddr[i];
            LOG_D("mac[%d]= 0x%x\n", i + 1, param->send_msg[i + 1]);
        }

        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_FW_INIT, param->send_msg, 7, param->recv_msg, 9);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("hal fw init failed");
        return ret;
    }

    return WF_RETURN_OK;
}


int wf_mcu_set_mlme_scan(nic_info_st *nic_info, wf_bool enable)
{
    int ret = 0;
    wf_u32 arg[7] = {0};
    wf_bool bConnect;
    wf_u8 linkState;
    wf_u32 fwState;
    sys_work_mode_e work_mode = get_sys_work_mode(nic_info);

    if (work_mode == WF_ADHOC_MODE)
    {
        fwState = WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE;
    }
    else if (work_mode == WF_INFRA_MODE)
    {
        fwState = WIFI_STATION_STATE;
    }
    else if (work_mode == WF_MONITOR_MODE)
    {
        fwState = WIFI_SITE_MONITOR;
    }
    
    wf_mlme_get_connect(nic_info, &bConnect);
    if (bConnect == wf_true)
    {
        nic_info->nic_state = WIFI_ASOC_STATE | fwState;
        linkState = MCU_LINKED;
    }
    else
    {
        nic_info->nic_state = fwState;
        linkState = MCU_UNLINKED;
    }

    arg[0] = enable;
#ifdef CONFIG_CONCURRENT_MODE
    arg[1] = nic_info->nic_num;
	arg[2] = 1;
#else
    arg[1] = 0;
	arg[2] = 0;
#endif
    arg[3] = nic_info->nic_state;
    arg[4] = linkState;
#ifndef CONFIG_CONCURRENT_MODE
    arg[5] = WIFI_FW_NO_EXIST;
#else
	arg[5] = wf_mcu_get_partner_fwstate(nic_info);
#endif
    arg[6] = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_MLME_SITE, arg, 7, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HW_SET_MLME_SITE, arg, 7, NULL, 0);
    }
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HW_SET_MLME_SITE failed");
        return -1;
    }

    if (enable == wf_true)
    {
        //LOG_I("disable bssid filter");
    }
    else
    {
        //LOG_I("enable bssid filter");
    }

    return 0;
}




int wf_mcu_set_mlme_join(nic_info_st *nic_info, wf_u8 type)
{
    wf_u32 param[5] = { 0 };
    int ret = 0;
    wf_u32 mlmeState;
    wf_u32 fwState;
    sys_work_mode_e work_mode = wf_local_cfg_get_work_mode(nic_info);
    

    if (work_mode == WF_ADHOC_MODE)
    {
        mlmeState = _HW_STATE_ADHOC_;
        fwState = WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE;
    }
    else if (work_mode == WF_INFRA_MODE)
    {
        mlmeState = _HW_STATE_STATION_;
        fwState = WIFI_STATION_STATE;
    }
    
    param[0] = type;

#ifdef CONFIG_CONCURRENT_MODE
    param[1] = nic_info->nic_num;  //iface0 or iface1
#else
    param[1] = 0;  //iface0 or iface1
#endif

    param[2] = fwState;
    param[3] = mlmeState;
    
#ifdef CONFIG_CONCURRENT_MODE
    param[4] = wf_mcu_get_partner_fwstate(nic_info);
#else
	param[4] = fwState;
#endif

    ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_MLME_JOIN, param, 5, NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HW_SET_MLME_JOIN failed");
        return -1;
    }

    return 0;
}

void wf_mcu_hw_var_set_macaddr(nic_info_st *nic_info, wf_u8 * val)
{
    wf_u8 idx = 0;
    wf_u32 reg_macid;
    wf_u32 ret = 0;
    wf_u32 var[7] = { 0 };

#ifdef CONFIG_CONCURRENT_MODE
    var[0] = nic_info->nic_num;
#else
    var[0] = 0;
#endif
    for (idx = 0; idx < 6; idx++)
        var[idx + 1] = val[idx];

    ret =
        mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_SET_MAC, var, 7,NULL,0);
        if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HAL_SET_BSSID failed");

    }
}



int wf_mcu_set_bssid(nic_info_st *nic_info, wf_u8 *bssid)
{
    wf_u8 idx = 0;
    wf_u32 reg_bssid;
    wf_u32 var[7] = { 0 };
    int ret = 0;

#ifdef CONFIG_CONCURRENT_MODE
    if(nic_info->nic_num == 1){
        var[0] = REG_BSSID1;
    }else{
        var[0] = REG_BSSID0;  //iface0 or iface1
    }
#else
    var[0] = REG_BSSID0;  //iface0 or iface1
#endif

    if (bssid != NULL)
    {
        for (idx = 0; idx < 6; idx++)
            var[idx + 1] = bssid[idx];
    }
    else
    {
        for (idx = 0; idx < 6; idx++)
            var[idx + 1] = 0;
    }

    ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_SET_BSSID, var, 7, NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HAL_SET_BSSID failed");
        return -1;
    }

    return 0;
}



int wf_mcu_set_sifs(nic_info_st *nic_info, wf_u32 value)
{
    int i = 0;
    wf_u32 arg[5] = {0};

    arg[0] = HW_VAR_RESP_SIFS;
    //LOG_I("[%s] arg[0]:0x%x",__func__,arg[0]);
    for(i=0; i<4; i++)
    {
        arg[i] = ((wf_u8*)&value)[i];
        //LOG_I("[%s] arg[%d]:0x%x",__func__,i,arg[i]);
    }
    return wf_mcu_set_hw_reg(nic_info, arg, 5);
}

int wf_mcu_set_macid_wakeup(nic_info_st *nic_info, wf_u32 wdn_id)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_MACID_WAKEUP;
    arg[1] = wdn_id;

    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}

int wf_mcu_set_basic_rate (nic_info_st *nic_info, wf_u16 br_cfg)
{
    int ret = 0;
    wf_u32 BrateCfg;
    wf_u16 rrsr_2g_force_mask = WF_RATE_1M | WF_RATE_2M | WF_RATE_5_5M | WF_RATE_11M;
    wf_u16 rrsr_2g_allow_mask = WF_RATE_24M | WF_RATE_12M | WF_RATE_6M | rrsr_2g_force_mask;

    BrateCfg = rrsr_2g_force_mask | br_cfg;
    BrateCfg &= rrsr_2g_allow_mask;

    ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_BASIC_RATE, &BrateCfg, 1, NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HW_SET_BASIC_RATE failed");
        return -1;
    }

    return WF_RETURN_OK;
}


int wf_mcu_set_preamble(nic_info_st *nic_info, wf_u8 short_val)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_ACK_PREAMBLE;
    arg[1] = short_val;

    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}

int wf_mcu_set_wmm_para_disable(nic_info_st *nic_info, wdn_net_info_st *wdn_info)
{
    wf_u8 ACI, ACM, AIFS, ECWMin, ECWMax, aSifsTime;
    wf_u32 acParm;
    wf_u16 TXOP;
    int ret = 0;
    wf_u32 arg[2] = {0};

    if(wdn_info->network_type & WIRELESS_11_24N)
    {
        aSifsTime = 16;
    }
    else
    {
        aSifsTime = 10;
    }
    AIFS = aSifsTime + (2 * SHORT_SLOT_TIME);
    ECWMax = 10;
    ECWMin = 4;
    TXOP = 0;
    acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);

    arg[0] = HW_VAR_AC_PARAM_BE;
    arg[1] = acParm;
    if (wf_mcu_set_hw_reg(nic_info, arg, 4) != 0)
    {
        return -1;
    }

    arg[0] = HW_VAR_AC_PARAM_BK;
    if (wf_mcu_set_hw_reg(nic_info, arg, 4) != 0)
    {
        return -1;
    }

    arg[0] = HW_VAR_AC_PARAM_VI;
    if (wf_mcu_set_hw_reg(nic_info, arg, 4) != 0)
    {
        return -1;
    }

    TXOP = 0x2f;
    ECWMax = 3;
    ECWMin = 2;
    acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
    arg[0] = HW_VAR_AC_PARAM_VO;
    arg[1] = acParm;
    if (wf_mcu_set_hw_reg(nic_info, arg, 4) != 0)
    {
        return -1;
    }
    return 0;

}

int wf_mcu_set_wmm_para_enable(nic_info_st *nic_info, wdn_net_info_st *wdn_info)
{
    wf_u8 ACI, ACM, AIFS, ECWMin, ECWMax, aSifsTime;
    wf_u16 TXOP;
    wf_u32 acParm;
    int i = 0;
    wf_wmm_para_st *wmm_info = &wdn_info->wmm_info;
    wf_u32 arg[2] = {0};

    wdn_info->acm_mask = 0;
    if(wdn_info->network_type & WIRELESS_11_24N)
    {
        aSifsTime = 16;
    }
    else
    {
        aSifsTime = 10;
    }
    for(i = 0 ; i < 4; i++)
    {
        ACI = (wmm_info->ac[i].ACI >> 5) & 0x03;
        ACM = (wmm_info->ac[i].ACI >> 4) & 0x01;
        AIFS = (wmm_info->ac[i].ACI & 0x0f)* SHORT_SLOT_TIME + aSifsTime;

#if 0
        LOG_D("ACI:0x%x", ACI);
        LOG_D("ACM:0x%x", ACM);
        LOG_D("AIFS:0x%x", AIFS);
#endif

        ECWMin = (wmm_info->ac[i].ECW & 0x0f);
        ECWMax = (wmm_info->ac[i].ECW & 0xf0)>>4;
        TXOP = le16_to_cpu(wmm_info->ac[i].TXOP_limit);

        aSifsTime = 16;
        acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
        switch (ACI)
        {
            case 0x00:
                arg[0] = HW_VAR_AC_PARAM_BE;
                wdn_info->acm_mask |= (ACM ? BIT(1) : 0);
                break;
            case 0x01:
                arg[0] = HW_VAR_AC_PARAM_BK;
                break;
            case 0x02:
                arg[0] = HW_VAR_AC_PARAM_VI;
                wdn_info->acm_mask |= (ACM ? BIT(2) : 0);
                break;
            case 0x03:
                arg[0] = HW_VAR_AC_PARAM_VO;
                wdn_info->acm_mask |= (ACM ? BIT(3) : 0);
                break;
        }

        arg[1] = acParm;

        wf_mcu_set_hw_reg(nic_info,arg,4);

        LOG_D("acParm:0x%x   acm_mask:0x%x", acParm, wdn_info->acm_mask);
    }

    return 0;
}


int wf_mcu_set_bcn_intv (nic_info_st *nic_info, wf_u16 val)
{
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_BEACON_INTERVAL;
    arg[1] = val;
    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}


int wf_mcu_set_slot_time(nic_info_st *nic_info, wf_u32 slotTime)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_SLOT_TIME;
    arg[1] = slotTime;

    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}


int wf_mcu_set_correct_tsf(nic_info_st *nic_info, wf_u64 tsf)
{
    wf_wlan_info_t *pwlan_info;
    wf_u32 arg[6] = {0};
    wf_u32 ret;

    arg[0] = HW_VAR_CORRECT_TSF;
    arg[1] = (wf_u32)tsf;
    arg[2] = tsf >> 32;
#ifdef CONFIG_CONCURRENT_MODE
    if(nic_info->nic_num == 1)
    {
        arg[3] = 1;
    }
    else
    {
        arg[3] = 0;
    }
#else
    arg[3] = 0;
#endif

    arg[4] = nic_info->nic_state;
    arg[5] = WIFI_FW_NO_EXIST;

    LOG_D("[wf_mcu_set_correct_tsf]TSF:%x   %x", arg[1], arg[0]);

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_CORRECT_TSF, arg, 6, NULL, 0);
    }
    else
    {
//        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_SET_HWREG, value, len, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret )
    {
        LOG_E("[%s] UMSG_OPS_HW_SET_CORRECT_TSF failed, check!!!!", __func__);
        return WF_RETURN_FAIL;
    }
    return 0;
}


int wf_mcu_set_media_status(nic_info_st *nic_info, wf_u32 status)
{
    int ret = 0;
    wf_u32 arg[2] = {0};
#ifdef CONFIG_CONCURRENT_MODE
    if(nic_info->nic_num == 1)
    {
        arg[0] = HW_VAR_MEDIA_STATUS1;
    }
    else
    {
        arg[0] = HW_VAR_MEDIA_STATUS;
    }
#else
    arg[0] = HW_VAR_MEDIA_STATUS;
#endif
    arg[1] = status;

    return wf_mcu_set_hw_reg(nic_info, arg, 2);
}


int wf_mcu_set_phy_config(nic_info_st *nic_info, phy_config_t *cfg)
{
    int ret = 0;

    ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_CONFIG_MSG, (wf_u32 *)cfg, sizeof(phy_config_t) / 4, NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("UMSG_OPS_HAL_CONFIG_MSG failed");
        return -1;
    }

    return 0;
}

int wf_mcu_mp_bb_rf_gain_offset(nic_info_st *nic_info)
{
    int ret = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        //do nothing
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_MP_BB_RF_GAIN_OFFSET, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WLAN_OPS_DXX0_MP_BB_RF_GAIN_OFFSET failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

int wf_mcu_mbox1_cmd(nic_info_st *nic_info, wf_u8 *cmd, wf_u32 cmd_len, wf_u8 ElementID )
{
    int ret     = 0;
    wf_u32 *tmp     = NULL;
    wf_u32 slen  = 0;
    wf_u32 status = 0;
    wf_u32 i = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        //do nothing
    }
    else
    {
        slen = (cmd_len + 2) * 4;
        tmp = wf_kzalloc(slen);
        if (NULL == tmp)
        {
            LOG_E("[%s] wf_kzalloc failed,check!!!", __func__);
            return WF_RETURN_FAIL;
        }
        tmp[0] = ElementID;
        tmp[1] = cmd_len;
        LOG_I("arg[0]:0x%x, arg[1]:0x%x", tmp[0], tmp[1]);
        for (i = 0; i < cmd_len; i++)
        {
            tmp[i + 2] = cmd[i];
            LOG_I("arg[%d]:0x%x", i + 2, tmp[i + 2]);
        }
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_wMBOX1_CMD, tmp, cmd_len + 2, &status, 1);
        wf_kfree(tmp);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] wf_mcu_mbox1_cmd failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}

int wf_mcu_cca_config(nic_info_st *nic_info)
{
    int ret = 0;

    if (NIC_USB == nic_info->nic_type)
    {
        //do nothing
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_CCA_CONFIG, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WLAN_OPS_DXX0_HAL_CCA_CONFIG failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}


int wf_mcu_watchdog(nic_info_st *nic_info)
{
    int ret = 0;
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_MSG_WDG, NULL, 0, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_MSG_WDG, NULL, 0, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;

}

int wf_mcu_set_on_rcr_am (nic_info_st *nic_info, wf_bool on_off)
{
    wf_u32 buf[2];

    buf[0] = on_off ? HW_VAR_ON_RCR_AM : HW_VAR_OFF_RCR_AM;
    buf[1] = 0;
    wf_mcu_set_hw_reg(nic_info, buf, 2);

    return WF_RETURN_OK;
}

int wf_mcu_set_dk_cfg (nic_info_st *nic_info, wf_u32 auth_algrthm, wf_bool dk_en)
{
    wf_u32 buf[2];

    buf[0] = dk_en;
    buf[1] = auth_algrthm;
    mcu_cmd_communicate(nic_info, UMSG_OPS_HW_SET_DK_CFG, buf, 2, NULL, 0);

    return WF_RETURN_OK;
}

int wf_mcu_set_sec_cfg (nic_info_st *nic_info, wf_u8 val)
{
    wf_u32 buf[5];

    buf[0] = HW_VAR_SEC_CFG;
    buf[1] = wf_false;
    buf[2] = wf_false;
    buf[3] = val;
    buf[4] = wf_true;
    wf_mcu_set_hw_reg(nic_info, buf, 5);

    return WF_RETURN_OK;
}

int wf_mcu_set_sec_cam (nic_info_st *nic_info,
                        wf_u8 cam_id, wf_u16 ctrl, wf_u8 *mac, wf_u8 *key)
{
    int ret = 0;
    int i   = 0;
    wf_u32 buff[WF_SECURITY_CAM_SIZE] = {0};

    buff[0] = cam_id;
    buff[1] = ctrl;

    for (i = 0; i < WF_ETH_ALEN; i++)
    {
        buff[i + 2] = mac[i];
    }

    for (i = 0; i < WF_SECURITY_KEY_SIZE; i++)
    {
        buff[i + 2 + WF_ETH_ALEN] = key[i];
    }

    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_SEC_WRITE_CAM,
                                  buff, WF_SECURITY_CAM_SIZE, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_SEC_WRITE_CAM,
                                  buff, WF_SECURITY_CAM_SIZE, NULL, 0);
    }

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    return WF_RETURN_OK;
}





/**/
int wf_mcu_fill_mbox1_fw(nic_info_st *nic_info, wf_u8 element_id, wf_u8 *cmd, wf_u32 cmd_len)
{
    int  ret = 0;
    wf_u32 *buf = NULL;
    int i    = 0;

    buf = (wf_u32 *) wf_kzalloc((cmd_len + 2) * 4);
    if (!buf)
    {
        LOG_E("[%s] failed, check", __func__);
        return WF_RETURN_FAIL;
    }

    buf[0] = element_id;
    buf[1] = cmd_len;
    //LOG_I("[%s] element_id:0x%x, cmd_len:%d", __func__, element_id, cmd_len);
    for (i = 0; i < cmd_len; i++)
    {
        buf[i + 2] = cmd[i];
        //LOG_D("[%s] 0x%x", __func__, buf[i + 2]);
    }
    if (NIC_USB == nic_info->nic_type)
    {
        ret = mcu_cmd_communicate(nic_info, UMSG_OPS_HAL_wMBOX1_CMD, buf, cmd_len + 2, NULL, 0);
    }
    else
    {
        ret = mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_wMBOX1_CMD, buf, cmd_len + 2, NULL, 0);
    }
    wf_kfree(buf);

    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed", __func__);
        return WF_RETURN_FAIL;
    }

    buf = NULL;
    return WF_RETURN_OK;
}

int wf_mcu_set_max_ampdu_len(nic_info_st *pnic_info, wf_u8 max_ampdu_len)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_AMPDU_FACTOR;
    arg[1] = max_ampdu_len;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_min_ampdu_space(nic_info_st *pnic_info, wf_u8 min_space)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    arg[0] = HW_VAR_AMPDU_MIN_SPACE;
    arg[1] = min_space;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_usb_agg_normal(nic_info_st *nic_info, wf_u8 cur_wireless_mode)
{
    int ret = WF_RETURN_FAIL;
    wf_u32 mbox1[1] = { 0 };

    if(NIC_USB != nic_info->nic_type)
    {
        return ret;
    }
#if 0
    if (cur_wireless_mode < WIRELESS_11_24N && cur_wireless_mode > 0)
    {
        if (0x6 != pHalData->RegAcUsbDmaSize || 0x10 != pHalData->RegAcUsbDmaTime)
        {
            pHalData->RegAcUsbDmaSize = 0x6;
            pHalData->RegAcUsbDmaTime = 0x10;

            mbox1[0] = cur_wireless_mode;
            ret =mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_SET_USB_AGG_NORMAL,mbox1, 1,NULL, 0);
            if (!ret)
            {
                LOG_E("[%s] failed \n", __func__);
                return;
            }
        }

    }
    else if (cur_wireless_mode >= WIRELESS_11_24N && cur_wireless_mode <= WIRELESS_MODE_MAX)
    {
        if (0x5 != pHalData->RegAcUsbDmaSize || 0x20 != pHalData->RegAcUsbDmaTime)
        {
            pHalData->RegAcUsbDmaSize = 0x5;
            pHalData->RegAcUsbDmaTime = 0x20;

            mbox2[0] = cur_wireless_mode;

            ret =mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_SET_USB_AGG_NORMAL,mbox2,1, NULL, 0);
            if (!ret)
            {
                LOG_E("[%s] failed", __func__);
                return ret;
            }

        }

    }
    else
    {
    }

    return ret;
#else
    mbox1[0] = cur_wireless_mode;
    ret =mcu_cmd_communicate(nic_info,UMSG_OPS_HAL_SET_USB_AGG_NORMAL,mbox1, 1,NULL, 0);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] failed \n", __func__);
        return ret;
    }
    return WF_RETURN_OK;
#endif
}

int wf_mcu_check_tx_buff(nic_info_st *nic_info)
{
    wf_u32 arg[1] = {0};

    arg[0] = WLAN_HAL_VALUE_CHECK_TXBUF;
    return wf_mcu_set_hw_reg(nic_info, arg, 1);
}

int wf_mcu_check_rx_fifo(nic_info_st *nic_info)
{
    wf_u32 arg[2] = { 0 };

    if(NIC_SDIO == nic_info->nic_type)
    {
        return mcu_cmd_communicate(nic_info, WLAN_OPS_DXX0_HAL_CHECK_RXFIFO_FULL, NULL,0,arg, 2);
    }
    else
    {
        return 0;
    }
}

int wf_mcu_reset_chip(nic_info_st *nic_info)
{

    if(NIC_USB == nic_info->nic_type)
    {
        return mcu_cmd_communicate(nic_info, UMSG_OPS_RESET_CHIP, NULL,0,NULL, 0);
    }
    else
    {
        return 0;
    }

}

int wf_mcu_set_ac_vo(nic_info_st *pnic_info)
{
    int ret = 0;
    wf_u32 acparm;
    wf_u32 arg[2] = {0};

#ifdef CONFIG_CONCURRENT_MODE
    acparm = 0x005ea42b;
#else
    acparm = 0x002F3217;
#endif

    arg[0] = HW_VAR_AC_PARAM_VO;
    arg[1] = acparm;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_ac_vi(nic_info_st *pnic_info)
{
    int ret = 0;
    wf_u32 acparm;
    wf_u32 arg[2] = {0};

    acparm = 0x005E4317;
    arg[0] = HW_VAR_AC_PARAM_VI;
    arg[1] = acparm;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_ac_be(nic_info_st *pnic_info)
{
    int ret = 0;
    wf_u32 acparm;
    wf_u32 arg[2] = {0};

    acparm = 0x005ea42b;
    arg[0] = HW_VAR_AC_PARAM_BE;
    arg[1] = acparm;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_ac_bk(nic_info_st *pnic_info)
{
    int ret = 0;
    wf_u32 acparm;
    wf_u32 arg[2] = {0};

    acparm = 0x0000A444;
    arg[0] = HW_VAR_AC_PARAM_BK;
    arg[1] = acparm;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_bcn_valid(nic_info_st *pnic_info)
{
    wf_u32 arg[2] = {0};
    int ret = 0;
#ifdef CONFIG_CONCURRENT_MODE
    if(pnic_info->nic_num == 1){
        arg[0] = HW_VAR_BCN_VALID1;
    }else{
        arg[0] = HW_VAR_BCN_VALID;
    }
#else
    arg[0] = HW_VAR_BCN_VALID;
#endif
    arg[1] = 0;

    ret = wf_mcu_set_hw_reg(pnic_info,arg,2);
    return ret;
}

int wf_mcu_get_bcn_valid(nic_info_st *pnic_info,wf_u32 *val32)
{
    wf_u32 recv_buf;
    int ret = 0;

    wf_u32 arg[1] = {0};

#ifdef CONFIG_CONCURRENT_MODE
		if(pnic_info->nic_num == 1){
			arg[0] = HW_VAR_BCN_VALID1;
		}else{
			arg[0] = HW_VAR_BCN_VALID;
		}
#else
		arg[0] = HW_VAR_BCN_VALID;
#endif


    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_GET_HWREG, arg, 1, val32, 1);
    if (ret == WF_RETURN_OK)
    {
        return 0;
    }
    else
    {
        LOG_D("get bcn valid fail");
        return -1;
    }
}


int wf_mcu_set_bcn_sel(nic_info_st *pnic_info)
{
    wf_u32 arg[2] = {0};
    int ret = 0;

    arg[0] = HW_VAR_DL_BCN_SEL;
    arg[1] = 0;

    ret = wf_mcu_set_hw_reg(pnic_info,arg,2);
    return ret;
}

#ifdef CFG_ENABLE_AP_MODE

int wf_mcu_set_ap_mode(nic_info_st *pnic_info)
{
    int ret = 0;
    wf_u32 arg[2] = {0};

    LOG_D("[set AP role] %s", __func__);

#ifdef CONFIG_CONCURRENT_MODE
        if(pnic_info->nic_num == 1){
            arg[0] = HW_VAR_MEDIA_STATUS1;
        }else{
            arg[0] = HW_VAR_MEDIA_STATUS;
        }
#else
        arg[0] = HW_VAR_MEDIA_STATUS;
#endif

    arg[1] = WIFI_FW_AP_STATE;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_set_sec(nic_info_st *pnic_info)
{
    sec_info_st *psec_info = pnic_info->sec_info;
    int ret = 0;
    wf_u8 val8;
    wf_u32 arg[2] = {0};
    val8 = (psec_info->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) ? 0xcc : 0xcf;

    arg[0] = HW_VAR_SEC_CFG;
    arg[1] = val8;

    return wf_mcu_set_hw_reg(pnic_info, arg, 2);
}

int wf_mcu_disable_rx_agg(nic_info_st * nic_info)
{
    wf_u32 data;
    LOG_I("0x10c:0x%02x",wf_io_read32(nic_info,0x10c,NULL));
	data = wf_io_read32(nic_info,0x10c,NULL);
	data = data & (~BIT(2));
	wf_io_write32(nic_info,0x10c,data);
	LOG_I("0x10c:0x%02x",wf_io_read32(nic_info,0x10c,NULL));
    return 0;

}

#endif

#ifdef CFG_ADHOC_MODE
int wf_mcu_set_bcn_reg(nic_info_st *pnic_info)
{
    wf_u32 par[6] = {0};
    int ret = 0;

    par[0] = 0x40;
    par[1] = 0x64;
    par[2] = wf_true;
    par[3] = wf_false;
    par[4] = wf_false;
    par[5] = 0x0550;

    ret = mcu_cmd_communicate(pnic_info, UMSG_OPS_HAL_SET_BCN_REG, par, 6, NULL, 0);
    if (ret == WF_RETURN_OK)
    {
        return 0;
    }
    else
    {
        LOG_D("set bcn reg fail");
        return -1;
    }
}
#endif
