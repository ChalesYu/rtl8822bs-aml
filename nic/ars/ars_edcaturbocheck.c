#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

#if 0
#define ARS_EDCA_DBG(fmt, ...)      LOG_D("ARS_EDCA[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_EDCA_PRT(fmt, ...)      LOG_D("ARS_EDCA-"fmt,##__VA_ARGS__)

#else
#define ARS_EDCA_DBG(fmt, ...)
#define ARS_EDCA_PRT(fmt, ...) 
#endif
#define ARS_EDCA_INFO(fmt, ...)      LOG_I("ARS_EDCA-"fmt,##__VA_ARGS__)
#define ARS_EDCA_ERR(fmt, ...)      LOG_E("ARS_EDCA-"fmt,##__VA_ARGS__)

#if 0
static wf_u32 edca_setting_UL[HT_IOT_PEER_MAX] = 
// UNKNOWN   REALTEK_90  REALTEK_92SE   BROADCOM    RALINK      ATHEROS     CISCO       MERU        MARVELL     92U_AP      SELF_AP(DownLink/Tx)
{  0x5e4322, 0xa44f,     0x5e4322,      0x5ea32b,   0x5ea422,   0x5ea322,   0x3ea430,   0x5ea42b,   0x5ea44f,   0x5e4322,   0x5e4322};


static wf_u32 edca_setting_DL[HT_IOT_PEER_MAX] = 
// UNKNOWN      REALTEK_90  REALTEK_92SE    BROADCOM    RALINK  ATHEROS CISCO       MERU,       MARVELL     92U_AP      SELF_AP(UpLink/Rx)
{ 0xa44f,       0x5ea44f,   0x5e4322,       0x5ea42b,   0xa44f, 0xa630, 0x5ea630,   0x5ea42b,   0xa44f,     0xa42b,     0xa42b};

static wf_u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] = 
// UNKNOWN      REALTEK_90  REALTEK_92SE    BROADCOM    RALINK      ATHEROS     CISCO   MERU,       MARVELL     92U_AP      SELF_AP
{ 0x4322,       0xa44f,     0x5e4322,       0xa42b,     0x5e4322,   0x4322,     0xa42b, 0x5ea42b,   0xa44f,     0x5e4322,   0x5ea42b};

#endif

wf_s32 odm_EdcaTurboCheckCE(void *ars)
{
    #if 0
    ars_st *        pars = ars;
    u32 EDCA_BE_UL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_UL[pMgntInfo->IOTPeer];
    u32 EDCA_BE_DL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_DL[pMgntInfo->IOTPeer];
    u32 ICType=pars->SupportICType;
    u32 IOTPeer=0;
    u8  WirelessMode=0xFF;                   //invalid value
    u32     trafficIndex;
    u32 edca_param;
    u64 cur_tx_bytes = 0;
    u64 cur_rx_bytes = 0;
    u8  bbtchange = _FALSE;
    u8  bBiasOnRx = _FALSE;
    HAL_DATA_TYPE       *pHalData = GET_HAL_DATA(Adapter);
    struct dvobj_priv       *pdvobjpriv = adapter_to_dvobj(Adapter);
    struct xmit_priv        *pxmitpriv = &(Adapter->xmitpriv);
    struct recv_priv        *precvpriv = &(Adapter->recvpriv);
    struct registry_priv    *pregpriv = &Adapter->registrypriv;
    struct mlme_ext_priv    *pmlmeext = &(Adapter->mlmeextpriv);
    struct mlme_ext_info    *pmlmeinfo = &(pmlmeext->mlmext_info);

    if(pars->bLinked != _TRUE)
    {
        precvpriv->bIsAnyNonBEPkts = _FALSE;
        return;
    }

    if ((pregpriv->wifi_spec == 1) )//|| (pmlmeinfo->HT_enable == 0))
    {
        precvpriv->bIsAnyNonBEPkts = _FALSE;
        return;
    }

    if(pars->pWirelessMode!=NULL)
        WirelessMode=*(pars->pWirelessMode);

    IOTPeer = pmlmeinfo->assoc_AP_vendor;

    if (IOTPeer >=  HT_IOT_PEER_MAX)
    {
        precvpriv->bIsAnyNonBEPkts = _FALSE;
        return;
    }
    
    // Check if the status needs to be changed.
    if((bbtchange) || (!precvpriv->bIsAnyNonBEPkts) )
    {
        cur_tx_bytes = pdvobjpriv->traffic_stat.cur_tx_bytes;
        cur_rx_bytes = pdvobjpriv->traffic_stat.cur_rx_bytes;

        //traffic, TX or RX
        if(bBiasOnRx)
        {
            if (cur_tx_bytes > (cur_rx_bytes << 2))
            { // Uplink TP is present.
                trafficIndex = UP_LINK; 
            }
            else
            { // Balance TP is present.
                trafficIndex = DOWN_LINK;
            }
        }
        else
        {
            if (cur_rx_bytes > (cur_tx_bytes << 2))
            { // Downlink TP is present.
                trafficIndex = DOWN_LINK;
            }
            else
            { // Balance TP is present.
                trafficIndex = UP_LINK;
            }
        }

        //if ((pars->DM_EDCA_Table.prv_traffic_idx != trafficIndex) || (!pars->DM_EDCA_Table.bCurrentTurboEDCA))
        {
        
            //92D txop can't be set to 0x3e for cisco1250
            if((ICType!=ODM_RTL8192D) && (IOTPeer== HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
            {
                EDCA_BE_DL = edca_setting_DL[IOTPeer];
                EDCA_BE_UL = edca_setting_UL[IOTPeer];
            }
            //merge from 92s_92c_merge temp brunch v2445    20120215 
            else if((IOTPeer == HT_IOT_PEER_CISCO) &&((WirelessMode==ODM_WM_G)||(WirelessMode==(ODM_WM_B|ODM_WM_G))||(WirelessMode==ODM_WM_A)||(WirelessMode==ODM_WM_B)))
            {
                EDCA_BE_DL = edca_setting_DL_GMode[IOTPeer];
            }
            else if((IOTPeer== HT_IOT_PEER_AIRGO )&& ((WirelessMode==ODM_WM_G)||(WirelessMode==ODM_WM_A)))
            {
                EDCA_BE_DL = 0xa630;
            }
            else if(IOTPeer == HT_IOT_PEER_MARVELL)
            {
                EDCA_BE_DL = edca_setting_DL[IOTPeer];
                EDCA_BE_UL = edca_setting_UL[IOTPeer];
            }
            else if(IOTPeer == HT_IOT_PEER_ATHEROS)
            {
                // Set DL EDCA for Atheros peer to 0x3ea42b. Suggested by SD3 Wilson for ASUS TP issue. 
                EDCA_BE_DL = edca_setting_DL[IOTPeer];
            }
           

            if (trafficIndex == DOWN_LINK)
                edca_param = EDCA_BE_DL;
            else
                edca_param = EDCA_BE_UL;

            rtw_write32(Adapter, REG_EDCA_BE_PARAM, edca_param);

            pars->DM_EDCA_Table.prv_traffic_idx = trafficIndex;
        }
        
        pars->DM_EDCA_Table.bCurrentTurboEDCA = _TRUE;
    }
    else
    {
        //
        // Turn Off EDCA turbo here.
        // Restore original EDCA according to the declaration of AP.
        //
         if(pars->DM_EDCA_Table.bCurrentTurboEDCA)
        {
            rtw_write32(Adapter, REG_EDCA_BE_PARAM, pHalData->AcParam_BE);
            pars->DM_EDCA_Table.bCurrentTurboEDCA = _FALSE;
        }
    }
    #else
    //to do
    #endif

    return WF_RETURN_OK;

}

wf_s32 odm_EdcaTurboCheck(void *ars)
{
    // 
    // For AP/ADSL use prtl8192cd_priv
    // For CE/NIC use PADAPTER
    //

    //
    // 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
    // at the same time. In the stage2/3, we need to prive universal interface and merge all
    // HW dynamic mechanism.
    //
    ars_st *pars = (ars_st *)ars;
   
    odm_EdcaTurboCheckCE(pars);

    return WF_RETURN_OK;
}   // odm_CheckEdcaTurbo



wf_s32 ODM_EdcaTurboInit(void *ars)
{
    ars_st *pars    = NULL;
    wf_s32 err      = 0;
    wf_u32 value_vo = 0;
    wf_u32 value_vi = 0;
    wf_u32 value_be = 0;
    wf_u32 value_bk = 0;
    nic_info_st *nic_info = NULL;
    
    if(NULL == ars)
    {
        ARS_EDCA_ERR("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_EDCA_INFO("start");
    
    pars = ars;
    nic_info = pars->nic_info;
    
    pars->edcaturbo.DM_EDCA_Table.bCurrentTurboEDCA = wf_false;
    pars->edcaturbo.DM_EDCA_Table.bIsCurRDLState = wf_false;
    pars->edcaturbo.bIsAnyNonBEPkts =wf_false;

    ars_io_lock_try(pars);
    value_vo = wf_io_read32(pars->nic_info,ODM_EDCA_VO_PARAM,&err);
    if(err)
    {
        ARS_EDCA_ERR("[%s] ODM_EDCA_VO_PARAM failed",__func__);
        ars_io_unlock_try(pars);
        return err;
    }

    value_vi = wf_io_read32(pars->nic_info,ODM_EDCA_VI_PARAM,&err);
    if(err)
    {
        ARS_EDCA_ERR("[%s] ODM_EDCA_VO_PARAM failed",__func__);
        ars_io_unlock_try(pars);
        return err;
    }
    value_be = wf_io_read32(pars->nic_info,ODM_EDCA_BE_PARAM,&err);
    if(err)
    {
        ARS_EDCA_ERR("[%s] ODM_EDCA_VO_PARAM failed",__func__);
        ars_io_unlock_try(pars);
        return err;
    }
    value_bk = wf_io_read32(pars->nic_info,ODM_EDCA_BK_PARAM,&err);
    if(err)
    {
        ARS_EDCA_ERR("[%s] ODM_EDCA_VO_PARAM failed",__func__);
        ars_io_unlock_try(pars);
        return err;
    }
    ars_io_unlock_try(pars);
    
    LOG_I("Orginial VO PARAM: 0x%x\n",value_vo);
    LOG_I("Orginial VI PARAM: 0x%x\n",value_vi);
    LOG_I("Orginial BE PARAM: 0x%x\n",value_be);
    LOG_I("Orginial BK PARAM: 0x%x\n",value_bk);

    return WF_RETURN_OK;
}


#endif

