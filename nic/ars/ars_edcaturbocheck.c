#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

typedef struct _EDCA_TURBO_
{
    wf_bool bCurrentTurboEDCA;
    wf_bool bIsCurRDLState;
    wf_u32  prv_traffic_idx; // edca turbo

}EDCA_T,*pEDCA_T;
static wf_u32 edca_setting_UL[HT_IOT_PEER_MAX] = 
// UNKNOWN   REALTEK_90  REALTEK_92SE   BROADCOM    RALINK      ATHEROS     CISCO       MERU        MARVELL     92U_AP      SELF_AP(DownLink/Tx)
{  0x5e4322, 0xa44f,     0x5e4322,      0x5ea32b,   0x5ea422,   0x5ea322,   0x3ea430,   0x5ea42b,   0x5ea44f,   0x5e4322,   0x5e4322};


static wf_u32 edca_setting_DL[HT_IOT_PEER_MAX] = 
// UNKNOWN      REALTEK_90  REALTEK_92SE    BROADCOM    RALINK  ATHEROS CISCO       MERU,       MARVELL     92U_AP      SELF_AP(UpLink/Rx)
{ 0xa44f,       0x5ea44f,   0x5e4322,       0x5ea42b,   0xa44f, 0xa630, 0x5ea630,   0x5ea42b,   0xa44f,     0xa42b,     0xa42b};

static wf_u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] = 
// UNKNOWN      REALTEK_90  REALTEK_92SE    BROADCOM    RALINK      ATHEROS     CISCO   MERU,       MARVELL     92U_AP      SELF_AP
{ 0x4322,       0xa44f,     0x5e4322,       0xa42b,     0x5e4322,   0x4322,     0xa42b, 0x5ea42b,   0xa44f,     0x5e4322,   0x5ea42b};



void odm_EdcaTurboCheckCE(void *ars)
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

}

void odm_EdcaTurboCheck(void *ars)
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

}   // odm_CheckEdcaTurbo

#endif

