#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_ARS_SUPPORT

void ODM_ParsingCFO(void *pars, void *pkt_info,wf_s8 *pcfotail )
{
    ars_st *ars                         = NULL;
    ODM_PACKET_INFO_T *pktinfo          = NULL;
    CFO_TRACKING            *pCfoTrack  = NULL;
    wf_u8 i = 0;

    if(NULL == pars || NULL == pkt_info)
    {
        ARS_DBG("input param is null");
        return;
    }
    
    ars = pars;
    pktinfo = pkt_info;
    pCfoTrack = &ars->cfo.cfo_tr;
    if(!(ars->SupportAbility & ODM_BB_CFO_TRACKING))
        return;

    if(pktinfo->bPacketMatchBSSID)
    {               
        //3 Update CFO report for path-A & path-B
        // Only paht-A and path-B have CFO tail and short CFO
        for(i = ODM_RF_PATH_A; i <= ODM_RF_PATH_B; i++)   
        {
            pCfoTrack->CFO_tail[i] = (int)pcfotail[i];
        }

        //3 Update packet counter
        if(pCfoTrack->packetCount == 0xffffffff)
            pCfoTrack->packetCount = 0;
        else
            pCfoTrack->packetCount++;
    }
}

wf_u8 odm_GetDefaultCrytaltalCap(void *ars)
{
    #if 0
    PDM_ODM_T                   pars = (PDM_ODM_T)pDM_VOID;
    wf_u8                       CrystalCap = 0x20;

    PADAPTER                    Adapter = pars->Adapter;
    HAL_DATA_TYPE               *pHalData = GET_HAL_DATA(Adapter);

    CrystalCap = pHalData->CrystalCap;

    CrystalCap = CrystalCap & 0x3f;

    return CrystalCap;
    #else
    return 0;
    #endif
}

void odm_SetCrystalCap(void *ars,wf_u8 CrystalCap)
{
    ars_st *    pars = ars;
    CFO_TRACKING                *pCfoTrack = &pars->cfo.cfo_tr;

    if(pCfoTrack->CrystalCap == CrystalCap)
        return;

    pCfoTrack->CrystalCap = CrystalCap;

    {
#if 0
        /* write 0x24[22:17] = 0x24[16:11] = CrystalCap */
        CrystalCap = CrystalCap & 0x3F;
        hw_write_bb_reg(pars, REG_AFE_XTAL_CTRL, 0x007ff800, (CrystalCap|(CrystalCap << 6)));
#endif
    } 
    LOG_I("%s(): CrystalCap = 0x%x\n", __func__,CrystalCap);
}

void odm_SetATCStatus(void *ars,wf_bool ATCStatus)
{
    ars_st*                 pars = ars;
    CFO_TRACKING           *pCfoTrack = &pars->cfo.cfo_tr;

    if(pCfoTrack->bATCStatus == ATCStatus)
        return;
    
    hw_write_bb_reg(pars->nic_info, ODM_REG_BB_ATC_11N, ODM_BIT_BB_ATC_11N, ATCStatus);
    pCfoTrack->bATCStatus = ATCStatus;
}

void ODM_CfoTrackingReset(void *ars)
{
    ars_st* pars = ars;
    CFO_TRACKING*   pCfoTrack = &pars->cfo.cfo_tr;

    pCfoTrack->DefXCap = odm_GetDefaultCrytaltalCap(pars);
    pCfoTrack->bAdjust = wf_true;

    if(pCfoTrack->CrystalCap > pCfoTrack->DefXCap)
    {
        odm_SetCrystalCap(pars, pCfoTrack->CrystalCap - 1);
        LOG_I("ODM_CfoTrackingReset(): approch default value (0x%x)\n", pCfoTrack->CrystalCap);
    } 
    else if (pCfoTrack->CrystalCap < pCfoTrack->DefXCap)
    {
        odm_SetCrystalCap(pars, pCfoTrack->CrystalCap + 1);
        LOG_I("ODM_CfoTrackingReset(): approch default value (0x%x)\n", pCfoTrack->CrystalCap);
    }

    odm_SetATCStatus(pars, wf_true);
}


void ODM_CfoTracking(void *ars)
{
    ars_st *    pars = ars;
    CFO_TRACKING *pCfoTrack = &pars->cfo.cfo_tr;
    wf_s32 CFO_kHz_A    = 0;
    wf_s32 CFO_kHz_B    = 0;
    wf_s32 CFO_ave      = 0;
    wf_s32 CFO_ave_diff = 0;
    wf_s32 CrystalCap   = (int)pCfoTrack->CrystalCap;
    wf_u8 Adjust_Xtal   = 1;

    //4 Support ability
    if(!(pars->SupportAbility & ODM_BB_CFO_TRACKING))
    {
        LOG_I("%s(): Return: SupportAbility ODM_BB_CFO_TRACKING is disabled\n",__func__);
        return;
    }

    LOG_I("ODM_CfoTracking()=========> \n");

    if(!pars->bLinked || !pars->bOneEntryOnly)
    {   
        //4 No link or more than one entry
        ODM_CfoTrackingReset(pars);
        LOG_I("%s(): Reset: bLinked = %d, bOneEntryOnly = %d\n", 
            __func__,pars->bLinked, pars->bOneEntryOnly);
    }
    else
    {
        //3 1. CFO Tracking
        //4 1.1 No new packet
        if(pCfoTrack->packetCount == pCfoTrack->packetCount_pre)
        {
            LOG_I("ODM_CfoTracking(): packet counter doesn't change\n");
            return;
        }
        pCfoTrack->packetCount_pre = pCfoTrack->packetCount;
    
        //4 1.2 Calculate CFO
        CFO_kHz_A =  (int)((pCfoTrack->CFO_tail[0] * 3125)  / 10)>>7; /* CFO_tail[1:0] is S(8,7),    (num_subcarrier>>7) x 312.5K = CFO value(K Hz)   */
        CFO_kHz_B =  (int)((pCfoTrack->CFO_tail[1] * 3125)  / 10)>>7;
        
        if(pars->RFType < ODM_2T2R)
        {      
            CFO_ave = CFO_kHz_A;
        }
        else
        {      
            CFO_ave = (int)(CFO_kHz_A + CFO_kHz_B) >> 1;
        }
        LOG_I("ODM_CfoTracking(): CFO_kHz_A = %dkHz, CFO_kHz_B = %dkHz, CFO_ave = %dkHz\n", 
                        CFO_kHz_A, CFO_kHz_B, CFO_ave);

        //4 1.3 Avoid abnormal large CFO
        CFO_ave_diff = (pCfoTrack->CFO_ave_pre >= CFO_ave)?(pCfoTrack->CFO_ave_pre - CFO_ave):(CFO_ave - pCfoTrack->CFO_ave_pre);
        if(CFO_ave_diff > 20 && pCfoTrack->largeCFOHit == 0 && !pCfoTrack->bAdjust)
        {
            LOG_I("ODM_CfoTracking(): first large CFO hit\n");
            pCfoTrack->largeCFOHit = 1;
            return;
        }
        else
        {
            pCfoTrack->largeCFOHit = 0;
        }
        pCfoTrack->CFO_ave_pre = CFO_ave;

        //4 1.4 Dynamic Xtal threshold
        if(pCfoTrack->bAdjust == wf_false)
        {
            if(CFO_ave > CFO_TH_XTAL_HIGH || CFO_ave < (-CFO_TH_XTAL_HIGH))
                pCfoTrack->bAdjust = wf_true;
        }
        else
        {
            if(CFO_ave < CFO_TH_XTAL_LOW && CFO_ave > (-CFO_TH_XTAL_LOW))
                pCfoTrack->bAdjust = wf_false;
        }
        
        //4 1.7 Adjust Crystal Cap.
        if(pCfoTrack->bAdjust)
        {
            if(CFO_ave > CFO_TH_XTAL_LOW)
                CrystalCap = CrystalCap + Adjust_Xtal;
            else if(CFO_ave < (-CFO_TH_XTAL_LOW))
                CrystalCap = CrystalCap - Adjust_Xtal;

            if(CrystalCap > 0x3f)
                CrystalCap = 0x3f;
            else if (CrystalCap < 0)
                CrystalCap = 0;

            odm_SetCrystalCap(pars, (wf_u8)CrystalCap);
        }
        LOG_I("ODM_CfoTracking(): Crystal cap = 0x%x, Default Crystal cap = 0x%x\n", 
            pCfoTrack->CrystalCap, pCfoTrack->DefXCap);

        //3 2. Dynamic ATC switch
        if(CFO_ave < CFO_TH_ATC && CFO_ave > -CFO_TH_ATC)
        {
            odm_SetATCStatus(pars, wf_false);
            LOG_I("ODM_CfoTracking(): Disable ATC!!\n");
        }
        else
        {
            odm_SetATCStatus(pars, wf_true);
            LOG_I("ODM_CfoTracking(): Enable ATC!!\n");
        }
    }
}


#endif

