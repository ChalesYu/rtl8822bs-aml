#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

void phydm_beamformSetSoundingEnter( void *ars)
{
    ars_st *pars = ars;

    //halComTxbf_EnterWorkItemCallback(pars);
}

void phydm_beamformSetSoundingLeave(    void *ars    )
{
    ars_st *pars = ars;

    //halComTxbf_LeaveWorkItemCallback(pars);

}

void phydm_beamformSetSoundingRate(    void *ars    )
{
    ars_st *pars = ars;

    //halComTxbf_RateWorkItemCallback(pars);
}
void phydm_beamformSetSoundingStatus(   void *ars    )
{
    ars_st *pars = ars;

    //halComTxbf_StatusWorkItemCallback(pars);

}

void phydm_beamformSetSoundingFwNdpa(    void *ars    )
{
    ars_st *pars = ars;

    //halComTxbf_FwNdpaWorkItemCallback(pars);
}
void phydm_beamformSetSoundingClk(    void *ars    )
{
    ars_st *pars = ars;
    //PADAPTER    padapter = pars->Adapter;

    //rtw_run_in_thread_cmd(padapter, halComTxbf_ClkWorkItemCallback, padapter);

}
void phydm_beamformSetResetTxPath(    void *ars    )
{
    ars_st *pars = ars;

    //halComTxbf_ResetTxPathWorkItemCallback(pars);
}

void phydm_beamformSetGetTxRate(
    void *ars
    )
{
    ars_st *pars = ars;

    //halComTxbf_GetTxRateWorkItemCallback(pDM_Odm);
}

wf_bool HalComTxbf_Set(    void *ars,  wf_u8 setType,void *pInBuf)
{
    ars_st *pars = ars;
    wf_u8*          pU1Tmp=(wf_u8*)pInBuf;
    wf_u32*         pU4Tmp=(wf_u32*)pInBuf;
    PHAL_TXBF_INFO  pTxbfInfo = &pars->BeamformingInfo.TxbfInfo;

    ARS_DBG("[%s] setType = 0x%X\n", __func__, setType);
    
    switch(setType)
    {
    case TXBF_SET_SOUNDING_ENTER:
    pTxbfInfo->TXBFIdx = *pU1Tmp;
    phydm_beamformSetSoundingEnter(pars);
    break;

    case TXBF_SET_SOUNDING_LEAVE:
    pTxbfInfo->TXBFIdx = *pU1Tmp;
    phydm_beamformSetSoundingLeave(pars);
    break;

    case TXBF_SET_SOUNDING_RATE:
    pTxbfInfo->BW = pU1Tmp[0];
    pTxbfInfo->Rate = pU1Tmp[1];
    phydm_beamformSetSoundingRate(pars);
    break;

    case TXBF_SET_SOUNDING_STATUS:
    pTxbfInfo->TXBFIdx = *pU1Tmp;
    phydm_beamformSetSoundingStatus(pars);
    break;

    case TXBF_SET_SOUNDING_FW_NDPA:
    pTxbfInfo->NdpaIdx = *pU1Tmp;
    phydm_beamformSetSoundingFwNdpa(pars);
    break;

    case TXBF_SET_SOUNDING_CLK:
    phydm_beamformSetSoundingClk(pars);
    break;
        
    case TXBF_SET_TX_PATH_RESET:
    pTxbfInfo->TXBFIdx = *pU1Tmp;
    phydm_beamformSetResetTxPath(pars);
    break;

    case TXBF_SET_GET_TX_RATE:
    phydm_beamformSetGetTxRate(pars);
    break;
    
    }

    return wf_true;
}
    

void Beamforming_DymPeriod(void *ars,wf_u8         status)
{
    wf_u8                   Idx;
    wf_bool                 bChangePeriod = wf_false;  
    wf_u16                  SoundPeriod_SW, SoundPeriod_FW;
    ars_st *pars        = ars;

    PRT_BEAMFORMEE_ENTRY    pBeamformEntry;
    PRT_BEAMFORMING_INFO    pBeamInfo = &(pars->BeamformingInfo);
    PRT_SOUNDING_INFO       pSoundInfo = &(pBeamInfo->SoundingInfo);

    PRT_BEAMFORMEE_ENTRY    pEntry = &(pBeamInfo->BeamformeeEntry[pBeamInfo->BeamformeeCurIdx]);

    ARS_DBG("[%s] Start!\n", __func__);
    
    //3 TODO  per-client throughput caculation.

    if ((*(pars->pCurrentTxTP) + *(pars->pCurrentRxTP) > 2) && ((pEntry->LogStatusFailCnt <= 20) || status)) 
    {
        SoundPeriod_SW = 40;    /* 40ms */
        SoundPeriod_FW = 40;    /* From  H2C cmd, unit = 10ms */
    } 
    else 
    {
        SoundPeriod_SW = 4000;/* 4s */
        SoundPeriod_FW = 400;
    }
        ARS_DBG("[%s]SoundPeriod_SW=%d, SoundPeriod_FW=%d\n",   __func__, SoundPeriod_SW, SoundPeriod_FW);

    for (Idx = 0; Idx < BEAMFORMEE_ENTRY_NUM; Idx++) 
    {
        pBeamformEntry = pBeamInfo->BeamformeeEntry+Idx;
        
        if (pBeamformEntry->DefaultCSICnt > 20) 
        {
            /*Modified by David*/
            SoundPeriod_SW = 4000;
            SoundPeriod_FW = 400;
        }
        
        ARS_DBG("[%s] Period = %d\n", __func__, SoundPeriod_SW);       
        if (pBeamformEntry->BeamformEntryCap & (BEAMFORMER_CAP_HT_EXPLICIT | BEAMFORMER_CAP_VHT_SU)) 
        {
            if (pSoundInfo->SoundMode == SOUNDING_FW_VHT_TIMER || pSoundInfo->SoundMode == SOUNDING_FW_HT_TIMER) 
            {              
                if (pBeamformEntry->SoundPeriod != SoundPeriod_FW) 
                {
                    pBeamformEntry->SoundPeriod = SoundPeriod_FW;
                    bChangePeriod = wf_true;       /*Only FW sounding need to send H2C packet to change sound period. */
                }
            } 
            else if (pBeamformEntry->SoundPeriod != SoundPeriod_SW) 
            {
                pBeamformEntry->SoundPeriod = SoundPeriod_SW;
            }
        }
    }

    if (bChangePeriod)
    {
        HalComTxbf_Set(pars, TXBF_SET_SOUNDING_FW_NDPA, (wf_u8*)&Idx);
    }
}

/* Used for BeamformingStart_SW and  BeamformingStart_FW */
void phydm_Beamforming_DymNDPARate(void *ars)
{
    wf_u16          NDPARate = ODM_RATE6M, BW;
    ars_st *pars             = ars;

    ARS_DBG("%s Start!\n", __func__);

    if (pars->RSSI_Min > 30)    /*link RSSI > 30%*/
        NDPARate = ODM_RATE24M;
    else
        NDPARate = ODM_RATE6M;

    BW = ODM_BW20M;
    NDPARate = NDPARate << 8 | BW;
    HalComTxbf_Set(pars, TXBF_SET_SOUNDING_RATE, (wf_u8*)&NDPARate);
    ARS_DBG("%s End, NDPA Rate = 0x%X\n", __func__, NDPARate);
}

void phydm_Beamforming_Watchdog(void *ars)
{
    ars_st *pars = ars;
    PRT_BEAMFORMING_INFO        pBeamInfo = &pars->BeamformingInfo;

    ARS_DBG("%s Start!\n", __func__);

    if (pBeamInfo->beamformee_su_cnt == 0)
        return;

    Beamforming_DymPeriod(pars,0);
    phydm_Beamforming_DymNDPARate(pars);

}

#endif

