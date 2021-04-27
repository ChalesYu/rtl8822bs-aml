#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

void odm_RSSIMonitorCheckCE(void *ars)
{
    #if 0
    ars_st*     pars = ars;
    PSTA_INFO_T           pEntry;
    int i = 0;
    int tmpEntryMaxPWDB = 0, tmpEntryMinPWDB = 0xff;
    u8  sta_cnt = 0;
    
    if (pars->bLinked != wf_true)
        return; 

    for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) 
    {
        pEntry = pars->pODM_StaInfo[i];
        if (IS_STA_VALID(pEntry)) {
            if (IS_MCAST(pEntry->hwaddr))  /*if(psta->mac_id ==1)*/
                continue;

            if (pEntry->rssi_stat.UndecoratedSmoothedPWDB == (-1))
                continue;

            if (pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
                tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

            if (pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
                tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;

            if (phydm_rssi_report(pars, i))
                sta_cnt++;
        }
    }
    /*DBG_871X("%s==> sta_cnt(%d)\n", __func__, sta_cnt);*/

    if (tmpEntryMaxPWDB != 0)   // If associated entry is found
        pHalData->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
    else
        pHalData->EntryMaxUndecoratedSmoothedPWDB = 0;

    if (tmpEntryMinPWDB != 0xff) // If associated entry is found
        pHalData->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
    else
        pHalData->EntryMinUndecoratedSmoothedPWDB = 0;

    FindMinimumRSSI(Adapter);//get pdmpriv->MinUndecoratedPWDBForDM

    pars->RSSI_Min = pHalData->MinUndecoratedPWDBForDM;
    //ODM_CmnInfoUpdate(&pHalData->odmpriv ,ODM_CMNINFO_RSSI_MIN, pdmpriv->MinUndecoratedPWDBForDM);
    #else
    //to do 
    #endif
}


void odm_RSSIMonitorCheck(void *ars)
{
    //
    // For AP/ADSL use prtl8192cd_priv
    // For CE/NIC use PADAPTER
    //
    ars_st*     pars = ars;
    if (!(pars->SupportAbility & ODM_BB_RSSI_MONITOR))
        return;

    //
    // 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
    // at the same time. In the stage2/3, we need to prive universal interface and merge all
    // HW dynamic mechanism.
    //
    
    odm_RSSIMonitorCheckCE(pars);

}   // odm_RSSIMonitorCheck



void phydm_ra_dynamic_retry_count(void *ars)
{
    ars_st* pars = ars;
    ars_ra_info_st*         pRA_Table = &pars->ra;
    wf_u8   i, retry_offset;
    wf_u32  ma_rx_tp;
    LOG_I("pars->pre_b_noisy = %d\n", pars->pre_b_noisy );
    if (pars->pre_b_noisy != pars->NoisyDecision) 
    {

        if (pars->NoisyDecision) 
        {
            LOG_I("->Noisy Env. RA fallback value\n");
            hw_write_bb_reg(pars->nic_info, 0x430, bMaskDWord, 0x0);
            hw_write_bb_reg(pars->nic_info, 0x434, bMaskDWord, 0x04030201);       
        } 
        else 
        {
            LOG_I("->Clean Env. RA fallback value\n");
            hw_write_bb_reg(pars->nic_info, 0x430, bMaskDWord, 0x02010000);
            hw_write_bb_reg(pars->nic_info, 0x434, bMaskDWord, 0x06050403);
        }
        pars->pre_b_noisy = pars->NoisyDecision;
    }
}


void odm_RefreshRateAdaptiveMaskCE(void *ars)
{
    ars_st * pars = ars;
    wf_u8  i;

    if (!pars->ra.bUseRAMask) 
    {
        LOG_I("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n");
        return;
    }

    //printk("==> %s \n",__FUNCTION__);

    #if 0
    for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) 
    {
        PSTA_INFO_T pstat = pars->pODM_StaInfo[i];
        if (IS_STA_VALID(pstat)) 
        {
            if (IS_MCAST(pstat->hwaddr))  //if(psta->mac_id ==1)
                continue;

            if (TRUE == ODM_RAStateCheck(pars, pstat->rssi_stat.UndecoratedSmoothedPWDB, FALSE , &pstat->rssi_level)) 
            {
                LOG_I("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level));
                //printk("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level);
                rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
            } 
            else if (pars->bChangeState) 
            {
                LOG_I("Change Power Training State, bDisablePowerTraining = %d\n", pars->bDisablePowerTraining));
                rtw_hal_update_ra_mask(pstat, pstat->rssi_level);
            }

        }
    }
    #else
    // to do
    #endif

}

void odm_RefreshRateAdaptiveMask(void *ars)
{
    ars_st*      pars = ars;
    LOG_I("odm_RefreshRateAdaptiveMask()---------->\n");
    if (!(pars->SupportAbility & ODM_BB_RA_MASK)) 
    {
        LOG_I("odm_RefreshRateAdaptiveMask(): Return cos not supported\n");
        return;
    }
    //
    // 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
    // at the same time. In the stage2/3, we need to prive universal interface and merge all
    // HW dynamic mechanism.
    //
    
    odm_RefreshRateAdaptiveMaskCE(pars);

}



#if (defined(CONFIG_RA_DYNAMIC_RTY_LIMIT))

void phydm_ra_dynamic_retry_limit(void *ars)
{
    ars_st *pars = ars;
    ars_ra_info_st*         pRA_Table = &pars->ra;
    PSTA_INFO_T     pEntry;
    wf_u8   i, retry_offset;
    wf_u32  ma_rx_tp;


    if (pars->pre_number_active_client == pars->number_active_client) 
    {
        
        LOG_I" pre_number_active_client ==  number_active_client\n");
        return;
        
    } 
    else 
    {
        if (pars->number_active_client == 1) 
        {
            phydm_reset_retry_limit_table(pars);
            LOG_I"one client only->reset to default value\n");
        } 
        else 
        {

            retry_offset = pars->number_active_client * pRA_Table->retry_descend_num;
            
            for (i = 0; i < ODM_NUM_RATE_IDX; i++) 
            {

                phydm_retry_limit_table_bound(pars, &(pRA_Table->per_rate_retrylimit_20M[i]), retry_offset);
                phydm_retry_limit_table_bound(pars, &(pRA_Table->per_rate_retrylimit_40M[i]), retry_offset);    
            }               
        }
    }
}
#else
void phydm_ra_dynamic_retry_limit(void *pars)
{
}

#endif

#endif

