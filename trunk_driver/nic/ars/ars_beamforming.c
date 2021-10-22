
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT

#define ARS_BF_DBG(fmt, ...)      LOG_D("ARS_BF[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_BF_PRT(fmt, ...)      LOG_D("ARS_BF-"fmt,##__VA_ARGS__)
#define ARS_BF_INFO(fmt, ...)      LOG_I("ARS_BF-"fmt,##__VA_ARGS__)
#define ARS_BF_ERR(fmt, ...)      LOG_E("ARS_BF-"fmt,##__VA_ARGS__)

wf_s32 phydm_beamformSetSoundingEnter( void *ars)
{
    ars_st *pars = NULL;
    pars = ars;

    //halComTxbf_EnterWorkItemCallback(pars);
    return WF_RETURN_OK;
}

wf_s32 phydm_beamformSetSoundingLeave(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;

    //halComTxbf_LeaveWorkItemCallback(pars);
    return WF_RETURN_OK;
}

wf_s32 phydm_beamformSetSoundingRate(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;
    //halComTxbf_RateWorkItemCallback(pars);
    return WF_RETURN_OK;
}
wf_s32 phydm_beamformSetSoundingStatus(   void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;
    //halComTxbf_StatusWorkItemCallback(pars);

    return WF_RETURN_OK;
}

wf_s32 phydm_beamformSetSoundingFwNdpa(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;
    //halComTxbf_FwNdpaWorkItemCallback(pars);
    return WF_RETURN_OK;
}
wf_s32 phydm_beamformSetSoundingClk(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;

    //rtw_run_in_thread_cmd(padapter, halComTxbf_ClkWorkItemCallback, padapter);
    return WF_RETURN_OK;
}
wf_s32 phydm_beamformSetResetTxPath(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;
    //halComTxbf_ResetTxPathWorkItemCallback(pars);
    return WF_RETURN_OK;
}

wf_s32 phydm_beamformSetGetTxRate(    void *ars    )
{
    ars_st *pars = NULL;

    pars = ars;

    //halComTxbf_GetTxRateWorkItemCallback(pDM_Odm);
    return WF_RETURN_OK;
}

wf_bool HalComTxbf_Set(    void *ars,  wf_u8 setType,void *pInBuf)
{
    ars_st *pars = ars;
    wf_u8*          pU1Tmp=(wf_u8*)pInBuf;
    PHAL_TXBF_INFO  pTxbfInfo = &pars->BeamformingInfo.TxbfInfo;

    ARS_BF_DBG("[%s] setType = 0x%X\n", __func__, setType);

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


wf_s32 Beamforming_DymPeriod(void *ars,wf_u8         status)
{
    wf_u8                   Idx;
    wf_bool                 bChangePeriod = wf_false;
    wf_u16                  SoundPeriod_SW, SoundPeriod_FW;
    ars_st *pars        = ars;

    PRT_BEAMFORMEE_ENTRY    pBeamformEntry;
    PRT_BEAMFORMING_INFO    pBeamInfo = &(pars->BeamformingInfo);
    PRT_SOUNDING_INFO       pSoundInfo = &(pBeamInfo->SoundingInfo);

    PRT_BEAMFORMEE_ENTRY    pEntry = &(pBeamInfo->BeamformeeEntry[pBeamInfo->BeamformeeCurIdx]);

    ARS_BF_DBG("[%s] Start!\n", __func__);

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
    ARS_BF_DBG("[%s]SoundPeriod_SW=%d, SoundPeriod_FW=%d\n",   __func__, SoundPeriod_SW, SoundPeriod_FW);

    for (Idx = 0; Idx < BEAMFORMEE_ENTRY_NUM; Idx++)
    {
        pBeamformEntry = pBeamInfo->BeamformeeEntry+Idx;

        if (pBeamformEntry->DefaultCSICnt > 20)
        {
            /*Modified by David*/
            SoundPeriod_SW = 4000;
            SoundPeriod_FW = 400;
        }

        ARS_BF_DBG("[%s] Period = %d\n", __func__, SoundPeriod_SW);
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
    return WF_RETURN_OK;
}

/* Used for BeamformingStart_SW and  BeamformingStart_FW */
wf_s32 phydm_Beamforming_DymNDPARate(void *ars)
{
    wf_u16          NDPARate = ODM_RATE6M, BW;
    ars_st *pars             = ars;

    ARS_BF_DBG("%s Start!\n", __func__);

    if (pars->RSSI_Min > 30)    /*link RSSI > 30%*/
        NDPARate = ODM_RATE24M;
    else
        NDPARate = ODM_RATE6M;

    BW = ODM_BW20M;
    NDPARate = NDPARate << 8 | BW;
    HalComTxbf_Set(pars, TXBF_SET_SOUNDING_RATE, (wf_u8*)&NDPARate);
    ARS_BF_DBG("%s End, NDPA Rate = 0x%X\n", __func__, NDPARate);

    return WF_RETURN_OK;
}

wf_s32 phydm_Beamforming_Watchdog(void *ars)
{
    ars_st *pars = ars;
    PRT_BEAMFORMING_INFO        pBeamInfo = &pars->BeamformingInfo;

    ARS_BF_DBG("%s Start!\n", __func__);

    if (pBeamInfo->beamformee_su_cnt == 0)
    {
        return WF_RETURN_OK;
    }

    Beamforming_DymPeriod(pars,0);
    phydm_Beamforming_DymNDPARate(pars);

    return WF_RETURN_OK;
}

wf_s32 phydm_Beamforming_Init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_BF_ERR("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_BF_INFO("start");
    pars = ars;

    return WF_RETURN_OK;
}

#endif

