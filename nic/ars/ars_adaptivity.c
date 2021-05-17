#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT
#if 0
#define ARS_ADT_DBG(fmt, ...)      LOG_D("ARS_ADT[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_ADT_PRT(fmt, ...)      LOG_D("ARS_ADT-"fmt,##__VA_ARGS__)

#else
#define ARS_ADT_DBG(fmt, ...)
#define ARS_ADT_PRT(fmt, ...) 

#endif

#define ARS_ADT_INFO(fmt, ...)      LOG_I("ARS_TADT-"fmt,##__VA_ARGS__)
#define ARS_ADT_ERR(fmt, ...)      LOG_E("ARS_ADT-"fmt,##__VA_ARGS__)


wf_s32 Phydm_MACEDCCAState(void *ars,PhyDM_MACEDCCA_Type      State)
{
    #if ARS_INIT_WRITE_REG_EN
    ars_st *pars = ars;
    
    if (State == PhyDM_IGNORE_EDCCA) 
    {
        hw_write_bb_reg(pars->nic_info, REG_TX_PTCL_CTRL, BIT(15), 1);    /*ignore EDCCA  reg520[15]=1*/
        hw_write_bb_reg(pars->nic_info, REG_RD_CTRL, BIT(11), 0);         /*reg524[11]=0*/
    } 
    else 
    {   /*don't set MAC ignore EDCCA signal*/
        hw_write_bb_reg(pars->nic_info, REG_TX_PTCL_CTRL, BIT(15), 0);    /*don't ignore EDCCA     reg520[15]=0*/
        hw_write_bb_reg(pars->nic_info, REG_RD_CTRL, BIT(11), 1);         /*reg524[11]=1  */
    }
    #endif
    ARS_ADT_DBG("EDCCA enable State = %d\n", State);
    return WF_RETURN_OK;
}

wf_s32 Phydm_GetNHMCounterStatistics(void *ars)
{
    ars_st* pars = ars;
    wf_u32      value32 = 0;

    value32 = hw_read_bb_reg(pars->nic_info, ODM_REG_NHM_CNT_11N, bMaskDWord);

    pars->NHM_cnt_0 = (wf_u8)(value32 & bMaskByte0);
    pars->NHM_cnt_1 = (wf_u8)((value32 & bMaskByte1) >> 8);

    return WF_RETURN_OK;
}

wf_s32 Phydm_NHMCounterStatisticsReset(void *ars)
{
    ars_st* pars = ars;

    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(1), 0);
    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(1), 1);

    return WF_RETURN_OK;
}

wf_s32 Phydm_NHMCounterStatistics(void *ars)
{
    ars_st*pars = ars;

    if (!(pars->SupportAbility & ODM_BB_NHM_CNT))
    {   
        return WF_RETURN_OK;
    }

    /*Get NHM report*/
    Phydm_GetNHMCounterStatistics(pars);

    /*Reset NHM counter*/
    Phydm_NHMCounterStatisticsReset(pars);

    return WF_RETURN_OK;
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


wf_s32 Phydm_CheckEnvironment(void *ars)
{
    ars_st*pars = ars;
    PADAPTIVITY_STATISTICS  Adaptivity = &pars->Adaptivity;
    wf_bool     isCleanEnvironment = wf_false;

    if (Adaptivity->bFirstLink == wf_true) 
    {
        pars->adaptivity_flag = wf_false;
        Adaptivity->bFirstLink = wf_false;
        return WF_RETURN_OK;
    }
    else 
    {
        if (Adaptivity->NHMWait < 3) 
        {       /*Start enter NHM after 4 NHMWait*/
            Adaptivity->NHMWait++;
            Phydm_NHMCounterStatistics(pars);
            return WF_RETURN_OK;
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

    return WF_RETURN_OK;
}

wf_s32 Phydm_SetEDCCAThreshold(void *ars,wf_s8 H2L,wf_s8 L2H)
{
    ars_st* pars = ars;
    hw_write_bb_reg(pars->nic_info, rOFDM0_ECCAThreshold, bMaskByte2|bMaskByte0, (wf_u32)((wf_u8)L2H|(wf_u8)H2L<<16));

    return WF_RETURN_OK;
}

wf_s32 Phydm_NHMCounterStatisticsInit(void *ars)
{
    #if ARS_INIT_WRITE_REG_EN
    ars_st*     pars = ars;
    /*PHY parameters initialize for n series*/
    
    wf_io_write16(pars->nic_info, ODM_REG_NHM_TIMER_11N + 2, 0xC350);/*0x894[31:16]=0x0xC350  Time duration for NHM unit: us, 0xc350=200ms*/
    wf_io_write16(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N + 2, 0xffff);/*0x890[31:16]=0xffff     th_9, th_10*/
    wf_io_write32(pars->nic_info, ODM_REG_NHM_TH3_TO_TH0_11N, 0xffffff50);/*0x898=0xffffff52          th_3, th_2, th_1, th_0*/
    wf_io_write32(pars->nic_info, ODM_REG_NHM_TH7_TO_TH4_11N, 0xffffffff);/*0x89c=0xffffffff          th_7, th_6, th_5, th_4*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_FPGA0_IQK_11N, bMaskByte0, 0xff);/*0xe28[7:0]=0xff            th_8*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_NHM_TH9_TH10_11N, BIT(10) | BIT(9) | BIT(8), 0x1);/*0x890[10:8]=1           ignoreCCA ignore PHYTXON enable CCX*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_OFDM_FA_RSTC_11N, BIT(7), 0x1);/*0xc0c[7]=1 max power among all RX ants*/
   #else
   #endif

   return WF_RETURN_OK;
}


wf_s32 Phydm_Adaptivity(void *ars,wf_u8 IGI)
{
    ars_st*     pars = ars;
    wf_s8           TH_L2H_dmc, TH_H2L_dmc;
    wf_s8           Diff = 0, IGI_target;
    PADAPTIVITY_STATISTICS  Adaptivity = &pars->Adaptivity;

    if ((pars->EDCCA_enable == wf_false) || (pars->bWIFITest == wf_true)) 
    {
        ARS_ADT_DBG("Disable EDCCA!!!\n");
        return WF_RETURN_OK;
    }

    if (!(pars->SupportAbility & ODM_BB_ADAPTIVITY)) 
    {
        ARS_ADT_DBG("adaptivity disable, enable EDCCA mode!!!\n");
        pars->TH_L2H_ini = pars->TH_L2H_ini_mode2;
        pars->TH_EDCCA_HL_diff = pars->TH_EDCCA_HL_diff_mode2;
    }

    ARS_ADT_DBG("odm_Adaptivity() =====>\n");
    ARS_ADT_DBG("IGI_Base=0x%x, TH_L2H_ini = %d, TH_EDCCA_HL_diff = %d\n",
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

    ARS_ADT_DBG("BandWidth=%s, IGI_target=0x%x, DynamicLinkAdaptivity = %d\n",
             (*pars->pBandWidth == ODM_BW80M) ? "80M" : ((*pars->pBandWidth == ODM_BW40M) ? "40M" : "20M"), IGI_target, Adaptivity->DynamicLinkAdaptivity);
    ARS_ADT_DBG("RSSI_min = %d, Adaptivity->AdajustIGILevel= 0x%x, adaptivity_flag = %d, Adaptivity_enable = %d\n",
             pars->RSSI_Min, Adaptivity->AdajustIGILevel, pars->adaptivity_flag, pars->Adaptivity_enable);

    if ((Adaptivity->DynamicLinkAdaptivity == wf_true) && (!pars->bLinked) && (pars->Adaptivity_enable == wf_false)) 
    {
        Phydm_SetEDCCAThreshold(pars, 0x7f, 0x7f);
        ARS_ADT_DBG("In DynamicLink mode(noisy) and No link, Turn off EDCCA!!\n");
        return WF_RETURN_OK;
    }

        if ((Adaptivity->AdajustIGILevel > IGI) && (pars->Adaptivity_enable == wf_true))
        {
            Diff = Adaptivity->AdajustIGILevel - IGI;
        }
        
        TH_L2H_dmc = pars->TH_L2H_ini - Diff + IGI_target;
        TH_H2L_dmc = TH_L2H_dmc - pars->TH_EDCCA_HL_diff;

    ARS_ADT_DBG("IGI=0x%x, TH_L2H_dmc = %d, TH_H2L_dmc = %d\n", IGI, TH_L2H_dmc, TH_H2L_dmc);
    ARS_ADT_DBG("Adaptivity_IGI_upper=0x%x, H2L_lb = 0x%x, L2H_lb = 0x%x\n", pars->Adaptivity_IGI_upper, Adaptivity->H2L_lb, Adaptivity->L2H_lb);

    Phydm_SetEDCCAThreshold(pars, TH_H2L_dmc, TH_L2H_dmc);
    return WF_RETURN_OK;
}


wf_s32 Phydm_CheckAdaptivity(void *ars)
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

    return WF_RETURN_OK;

}


wf_s32 Phydm_AdaptivityInit(void *ars)
{
    ars_st *pars            = NULL;
    nic_info_st *nic_info   = NULL;
    local_info_st *local    = NULL;
    PADAPTIVITY_STATISTICS  Adaptivity = NULL;
    wf_s8   IGItarget = 0x32;
    
    if(NULL == ars)
    {
        ARS_ADT_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;
    Adaptivity = &pars->Adaptivity;
    nic_info = pars->nic_info;
    local = nic_info->local_info;

    pars->Carrier_Sense_enable = (local->adaptivity_mode != 0) ? wf_true : wf_false;
    pars->DCbackoff = local->adaptivity_dc_backoff;
    Adaptivity->DynamicLinkAdaptivity = (local->adaptivity_dml != 0) ? wf_true : wf_false;


    if (pars->Carrier_Sense_enable == wf_false) 
    {

        if (local->adaptivity_th_l2h_ini != 0)
        {      
            pars->TH_L2H_ini = local->adaptivity_th_l2h_ini;
        }
        else
        {      
            pars->TH_L2H_ini = 0xf5;
        }
    } 
    else
    {
        pars->TH_L2H_ini = 0xa;
    }


    if (local->adaptivity_th_edcca_hl_diff != 0)
    {   
        pars->TH_EDCCA_HL_diff = local->adaptivity_th_edcca_hl_diff;
    }
    else
    {   
        pars->TH_EDCCA_HL_diff = 7;
    }

    Adaptivity->TH_L2H_ini_backup = pars->TH_L2H_ini;
    Adaptivity->TH_EDCCA_HL_diff_backup = pars->TH_EDCCA_HL_diff;


    pars->Adaptivity_IGI_upper = 0;
    pars->Adaptivity_enable = wf_false; /*use this flag to decide enable or disable*/

    pars->EDCCA_enable = wf_true;       /*even no adaptivity, we still enable EDCCA*/

    pars->TH_L2H_ini_mode2 = 20;
    pars->TH_EDCCA_HL_diff_mode2 = 8;
    
    Adaptivity->IGI_Base = 0x32;
    Adaptivity->IGI_target = 0x1c;
    Adaptivity->H2L_lb = 0;
    Adaptivity->L2H_lb = 0;
    Adaptivity->NHMWait = 0;
    Adaptivity->bCheck = wf_false;
    Adaptivity->bFirstLink = wf_true;
    Adaptivity->AdajustIGILevel = 0;

    Phydm_MACEDCCAState(pars, PhyDM_DONT_IGNORE_EDCCA);

    /*Search pwdB lower bound*/
    #if ARS_INIT_WRITE_REG_EN
    hw_write_bb_reg(pars->nic_info, ODM_REG_DBG_RPT_11N, bMaskDWord, 0x208);

    /*hw_write_bb_reg(pars, ODM_REG_EDCCA_DOWN_OPT_11N, BIT12 | BIT11 | BIT10, 0x7);*/      /*interfernce need > 2^x us, and then EDCCA will be 1*/
    hw_write_bb_reg(pars->nic_info, ODM_REG_EDCCA_DCNF_11N, BIT(21) | BIT(20), 0x1);      /*0:rx_dfir, 1: dcnf_out, 2 :rx_iq, 3: rx_nbi_nf_out*/
    #endif

    /*we need to consider PwdB upper bound for 8814 later IC*/
    Adaptivity->AdajustIGILevel = (wf_u8)((pars->TH_L2H_ini + IGItarget) - PwdBUpperBound + DFIRloss);  /*IGI = L2H - PwdB - DFIRloss*/

    ARS_ADT_DBG("TH_L2H_ini = 0x%x, TH_EDCCA_HL_diff = 0x%x, Adaptivity->AdajustIGILevel = 0x%x\n", pars->TH_L2H_ini, pars->TH_EDCCA_HL_diff, Adaptivity->AdajustIGILevel);

    return WF_RETURN_OK;
}


#endif


