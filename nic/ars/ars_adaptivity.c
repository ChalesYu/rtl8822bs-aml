#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT

void Phydm_GetNHMCounterStatistics(void *ars)
{
    ars_st* pars = ars;
    wf_u32      value32 = 0;

    value32 = hw_read_bb_reg(pars->nic_info, ODM_REG_NHM_CNT_11N, bMaskDWord);

    pars->NHM_cnt_0 = (wf_u8)(value32 & bMaskByte0);
    pars->NHM_cnt_1 = (wf_u8)((value32 & bMaskByte1) >> 8);

}

void Phydm_NHMCounterStatisticsReset(void *ars)
{
    ars_st* pars = ars;

    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(1), 0);
    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(1), 1);
    
}

void Phydm_NHMCounterStatistics(void *ars)
{
    ars_st*pars = ars;

    if (!(pars->SupportAbility & ODM_BB_NHM_CNT))
    {   
        return;
    }

    /*Get NHM report*/
    Phydm_GetNHMCounterStatistics(pars);

    /*Reset NHM counter*/
    Phydm_NHMCounterStatisticsReset(pars);
}

wf_bool Phydm_CalNHMcnt(void *ars)
{
    ars_st *pars = ars;
    wf_u16  Base = 0;

    Base = pars->NHM_cnt_0 + pars->NHM_cnt_1;

    if (Base != 0) 
    {
        pars->NHM_cnt_0 = ((pars->NHM_cnt_0) << 8) / Base;
        pars->NHM_cnt_1 = ((pars->NHM_cnt_1) << 8) / Base;
    }
    if ((pars->NHM_cnt_0 - pars->NHM_cnt_1) >= 100)
    {   
        return wf_true;         /*clean environment*/
    }
    else
    {   
        return wf_false;        /*noisy environment*/
    }

}


void Phydm_CheckEnvironment(void *ars)
{
    ars_st*pars = ars;
    PADAPTIVITY_STATISTICS  Adaptivity = &pars->Adaptivity;
    wf_bool     isCleanEnvironment = wf_false;

    if (Adaptivity->bFirstLink == wf_true) 
    {
        pars->adaptivity_flag = wf_false;
        Adaptivity->bFirstLink = wf_false;
        return;
    }
    else 
    {
        if (Adaptivity->NHMWait < 3) 
        {       /*Start enter NHM after 4 NHMWait*/
            Adaptivity->NHMWait++;
            Phydm_NHMCounterStatistics(pars);
            return;
        } 
        else 
        {
            Phydm_NHMCounterStatistics(pars);
            isCleanEnvironment = Phydm_CalNHMcnt(pars);
            if (isCleanEnvironment == wf_true) 
            {
                pars->TH_L2H_ini = Adaptivity->TH_L2H_ini_backup;           /*adaptivity mode*/
                pars->TH_EDCCA_HL_diff = Adaptivity->TH_EDCCA_HL_diff_backup;

                pars->Adaptivity_enable = wf_true;

                pars->adaptivity_flag = wf_false;
                
            } 
            else 
            {
                pars->TH_L2H_ini = pars->TH_L2H_ini_mode2;          /*mode2*/
                pars->TH_EDCCA_HL_diff = pars->TH_EDCCA_HL_diff_mode2;

                pars->adaptivity_flag = wf_false;
                pars->Adaptivity_enable = wf_false;
            }
            Adaptivity->NHMWait = 0;
            Adaptivity->bFirstLink = wf_true;
            Adaptivity->bCheck = wf_true;
        }

    }


}

void Phydm_SetEDCCAThreshold(void *ars,wf_s8 H2L,wf_s8 L2H)
{
    ars_st* pars = ars;
    hw_write_bb_reg(pars->nic_info, rOFDM0_ECCAThreshold, bMaskByte2|bMaskByte0, (wf_u32)((wf_u8)L2H|(wf_u8)H2L<<16));
}

void Phydm_NHMCounterStatisticsInit(void *ars)
{
    ars_st*     pars = ars;
    
    /*PHY parameters initialize for n series*/
    wf_io_write16(pars->nic_info, ODM_REG_NHM_TIMER_11N + 2, 0xC350);/*0x894[31:16]=0x0xC350  Time duration for NHM unit: us, 0xc350=200ms*/
    wf_io_write16(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N + 2, 0xffff);/*0x890[31:16]=0xffff     th_9, th_10*/
    wf_io_write32(pars->nic_info, ODM_REG_NHM_TH3_TO_TH0_11N, 0xffffff50);/*0x898=0xffffff52          th_3, th_2, th_1, th_0*/
    wf_io_write32(pars->nic_info, ODM_REG_NHM_TH7_TO_TH4_11N, 0xffffffff);/*0x89c=0xffffffff          th_7, th_6, th_5, th_4*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_FPGA0_IQK_11N, bMaskByte0, 0xff);/*0xe28[7:0]=0xff            th_8*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(10) | BIT(9) | BIT(8), 0x1);/*0x890[10:8]=1           ignoreCCA ignore PHYTXON enable CCX*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTC_11N, BIT(7), 0x1);/*0xc0c[7]=1 max power among all RX ants*/

}


void Phydm_Adaptivity(void *ars,wf_u8 IGI)
{
    ars_st*     pars = ars;
    wf_s8           TH_L2H_dmc, TH_H2L_dmc;
    wf_s8           Diff = 0, IGI_target;
    PADAPTIVITY_STATISTICS  Adaptivity = &pars->Adaptivity;

    if ((pars->EDCCA_enable == wf_false) || (pars->bWIFITest == wf_true)) 
    {
        LOG_I("Disable EDCCA!!!\n");
        return;
    }

    if (!(pars->SupportAbility & ODM_BB_ADAPTIVITY)) 
    {
        LOG_I("adaptivity disable, enable EDCCA mode!!!\n");
        pars->TH_L2H_ini = pars->TH_L2H_ini_mode2;
        pars->TH_EDCCA_HL_diff = pars->TH_EDCCA_HL_diff_mode2;
    }

    LOG_I("odm_Adaptivity() =====>\n");
    LOG_I("IGI_Base=0x%x, TH_L2H_ini = %d, TH_EDCCA_HL_diff = %d\n",
             Adaptivity->IGI_Base, pars->TH_L2H_ini, pars->TH_EDCCA_HL_diff);
    {
        /*fix AC series when enable EDCCA hang issue*/
        hw_write_bb_reg(pars->nic_info, 0x800, BIT(10), 1); /*ADC_mask disable*/
        hw_write_bb_reg(pars->nic_info, 0x800, BIT(10), 0); /*ADC_mask enable*/
    }
    if (*pars->pBandWidth == ODM_BW20M)     /*CHANNEL_WIDTH_20*/
    {   
        IGI_target = Adaptivity->IGI_Base;
    }
    else if (*pars->pBandWidth == ODM_BW40M)
    {   
        IGI_target = Adaptivity->IGI_Base + 2;
    }
    else
    {   
        IGI_target = Adaptivity->IGI_Base;
    }
    Adaptivity->IGI_target = (wf_u8) IGI_target;

    LOG_I("BandWidth=%s, IGI_target=0x%x, DynamicLinkAdaptivity = %d\n",
             (*pars->pBandWidth == ODM_BW80M) ? "80M" : ((*pars->pBandWidth == ODM_BW40M) ? "40M" : "20M"), IGI_target, Adaptivity->DynamicLinkAdaptivity);
    LOG_I("RSSI_min = %d, Adaptivity->AdajustIGILevel= 0x%x, adaptivity_flag = %d, Adaptivity_enable = %d\n",
             pars->RSSI_Min, Adaptivity->AdajustIGILevel, pars->adaptivity_flag, pars->Adaptivity_enable);

    if ((Adaptivity->DynamicLinkAdaptivity == wf_true) && (!pars->bLinked) && (pars->Adaptivity_enable == wf_false)) 
    {
        Phydm_SetEDCCAThreshold(pars, 0x7f, 0x7f);
        LOG_I("In DynamicLink mode(noisy) and No link, Turn off EDCCA!!\n");
        return;
    }

    {
        if ((Adaptivity->AdajustIGILevel > IGI) && (pars->Adaptivity_enable == wf_true)) 
            Diff = Adaptivity->AdajustIGILevel - IGI;
        
        TH_L2H_dmc = pars->TH_L2H_ini - Diff + IGI_target;
        TH_H2L_dmc = TH_L2H_dmc - pars->TH_EDCCA_HL_diff;
    }

    LOG_I("IGI=0x%x, TH_L2H_dmc = %d, TH_H2L_dmc = %d\n", IGI, TH_L2H_dmc, TH_H2L_dmc);
    LOG_I("Adaptivity_IGI_upper=0x%x, H2L_lb = 0x%x, L2H_lb = 0x%x\n", pars->Adaptivity_IGI_upper, Adaptivity->H2L_lb, Adaptivity->L2H_lb);

    Phydm_SetEDCCAThreshold(pars, TH_H2L_dmc, TH_L2H_dmc);
    return;
}


void Phydm_CheckAdaptivity(void *ars)
{
    ars_st*     pars = ars;
    PADAPTIVITY_STATISTICS  Adaptivity = &pars->Adaptivity;
    
    if (pars->SupportAbility & ODM_BB_ADAPTIVITY) 
    {
        if (Adaptivity->DynamicLinkAdaptivity == wf_true) 
        {
            if (pars->bLinked && Adaptivity->bCheck == wf_false) 
            {
                Phydm_NHMCounterStatistics(pars);
                Phydm_CheckEnvironment(pars);
            } 
            else if (!pars->bLinked)
            {
                Adaptivity->bCheck = wf_false;
            }
        } 
        else 
        {
            pars->Adaptivity_enable = wf_true;
            pars->adaptivity_flag = wf_false;

        }
    } 
    else 
    {
        pars->Adaptivity_enable = wf_false;
        pars->adaptivity_flag = wf_false;
    }

    

}

#endif


