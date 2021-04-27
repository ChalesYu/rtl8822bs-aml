#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT
static wf_u32 phy_CalculateBitShift( wf_u32 BitMask )
{
    wf_u32 i;

    for (i = 0; i <= 31; i++) 
    {
        if (((BitMask >> i) &  0x1) == 1)
        {      
            break;
        }
    }

    return (i);
}

void hw_write_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data)
{
    wf_s32 err;
    wf_u32          OriginalValue, BitShift;


    LOG_I("--->%s(): RegAddr(%#x), BitMask(%#x), Data(%#x)\n", __func__,RegAddr, BitMask, Data); 

    if (BitMask != bMaskDWord) 
    { 
        /*if not "double word" write */
        OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
        if(err)
        {
            LOG_E("[%s,%d] wf_io_read32 failed",__func__,__LINE__);
            return;
        }
        BitShift = phy_CalculateBitShift(BitMask);
        Data = ((OriginalValue & (~BitMask)) | ((Data << BitShift) & BitMask));
    }

    wf_io_write32(pnic_info, RegAddr, Data);
}

wf_u32 hw_read_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask)
{
    wf_u32  ReturnValue = 0, OriginalValue, BitShift;
    wf_u16  BBWaitCounter = 0;
    wf_s32  err;

    OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
    if(err)
    {
        LOG_E("[%s] wf_io_read32 failed",__func__);
        return 0;
    }
    
    BitShift = phy_CalculateBitShift(BitMask);
    ReturnValue = (OriginalValue & BitMask) >> BitShift;

    return (ReturnValue);

}

void phy_InitBBRFRegisterDefinition(void *ars)
{
    ars_st *pars = ars;
    ars_hwconfig_info_st *hw = &pars->hwconfig;

    /* RF Interface Sowrtware Control */
    hw->PHYRegDef[ODM_RF_PATH_A].rfintfs = rFPGA0_XAB_RFInterfaceSW; /* 16 LSBs if read 32-bit from 0x870 */
    hw->PHYRegDef[ODM_RF_PATH_B].rfintfs = rFPGA0_XAB_RFInterfaceSW; /* 16 MSBs if read 32-bit from 0x870 (16-bit for 0x872) */

    /* RF Interface Output (and Enable) */
    hw->PHYRegDef[ODM_RF_PATH_A].rfintfo = rFPGA0_XA_RFInterfaceOE; /* 16 LSBs if read 32-bit from 0x860 */
    hw->PHYRegDef[ODM_RF_PATH_B].rfintfo = rFPGA0_XB_RFInterfaceOE; /* 16 LSBs if read 32-bit from 0x864 */

    /* RF Interface (Output and)  Enable */
    hw->PHYRegDef[ODM_RF_PATH_A].rfintfe = rFPGA0_XA_RFInterfaceOE; /* 16 MSBs if read 32-bit from 0x860 (16-bit for 0x862) */
    hw->PHYRegDef[ODM_RF_PATH_B].rfintfe = rFPGA0_XB_RFInterfaceOE; /* 16 MSBs if read 32-bit from 0x864 (16-bit for 0x866) */

    hw->PHYRegDef[ODM_RF_PATH_A].rf3wireOffset = rFPGA0_XA_LSSIParameter; /*LSSI Parameter */
    hw->PHYRegDef[ODM_RF_PATH_B].rf3wireOffset = rFPGA0_XB_LSSIParameter;

    hw->PHYRegDef[ODM_RF_PATH_A].rfHSSIPara2 = rFPGA0_XA_HSSIParameter2;  /*wire control parameter2 */
    hw->PHYRegDef[ODM_RF_PATH_B].rfHSSIPara2 = rFPGA0_XB_HSSIParameter2;  /*wire control parameter2 */

    /* Tranceiver Readback LSSI/HSPI mode */
    hw->PHYRegDef[ODM_RF_PATH_A].rfLSSIReadBack = rFPGA0_XA_LSSIReadBack;
    hw->PHYRegDef[ODM_RF_PATH_B].rfLSSIReadBack = rFPGA0_XB_LSSIReadBack;
    hw->PHYRegDef[ODM_RF_PATH_A].rfLSSIReadBackPi = TransceiverA_HSPI_Readback;
    hw->PHYRegDef[ODM_RF_PATH_B].rfLSSIReadBackPi = TransceiverB_HSPI_Readback;

}

static wf_u32 phy_RFSerialRead_8188F(nic_info_st *nic_info, RF_PATH eRFPath,wf_u32 Offset)
{
    ars_st *pars = nic_info->ars;
    wf_u32                      retValue = 0;
    ars_hwconfig_info_st *hwcfg = &pars->hwconfig;
    BB_REGISTER_DEFINITION_T    *pPhyReg = &hwcfg->PHYRegDef[eRFPath];
    wf_u32                      NewOffset;
    wf_u32                      tmplong, tmplong2;
    wf_u8                   RfPiEnable = 0;
    wf_u32                      MaskforPhySet = 0;
    int i = 0;

    /* */
    /* Make sure RF register offset is correct */
    /* */
    Offset &= 0xff;

    NewOffset = Offset;

    if (eRFPath == RF_PATH_A) {
        tmplong2 = hw_read_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord);
        tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;    /*T65 RF */
        hw_write_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
    } else {
        tmplong2 = hw_read_bb_reg(nic_info, rFPGA0_XB_HSSIParameter2 | MaskforPhySet, bMaskDWord);
        tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;    /*T65 RF */
        hw_write_bb_reg(nic_info, rFPGA0_XB_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
    }

    tmplong2 = hw_read_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord);
    hw_write_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
    hw_write_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 | bLSSIReadEdge);

    #if 0
    rtw_udelay_os(10);

    for (i = 0; i < 2; i++)
        rtw_udelay_os(MAX_STALL_TIME);
    rtw_udelay_os(10);
    #else
    wf_mdelay(11);
    #endif

    if (eRFPath == RF_PATH_A)
        RfPiEnable = (wf_u8)hw_read_bb_reg(nic_info, rFPGA0_XA_HSSIParameter1 | MaskforPhySet, BIT(8));
    else if (eRFPath == RF_PATH_B)
        RfPiEnable = (wf_u8)hw_read_bb_reg(nic_info, rFPGA0_XB_HSSIParameter1 | MaskforPhySet, BIT(8));

    if (RfPiEnable) 
    {
        /* Read from BBreg8b8, 12 bits for 8190, 20bits for T65 RF */
        retValue = hw_read_bb_reg(nic_info, pPhyReg->rfLSSIReadBackPi | MaskforPhySet, bLSSIReadBackData);

        /*RT_DISP(FINIT, INIT_RF, ("Readback from RF-PI : 0x%x\n", retValue)); */
    } 
    else 
    {
        /*Read from BBreg8a0, 12 bits for 8190, 20 bits for T65 RF */
        retValue = hw_read_bb_reg(nic_info, pPhyReg->rfLSSIReadBack | MaskforPhySet, bLSSIReadBackData);
    }
    return retValue;

}

static void phy_RFSerialWrite_8188F(nic_info_st *nic_info,RF_PATH eRFPath,wf_u32 Offset,wf_u32 Data)
{
    ars_st *pars = nic_info->ars;
    ars_hwconfig_info_st *hwcfg = &pars->hwconfig;
    wf_u32  DataAndAddr = 0;
    BB_REGISTER_DEFINITION_T    *pPhyReg = &hwcfg->PHYRegDef[eRFPath];
    wf_u32  NewOffset;

    Offset &= 0xff;

    /* */
    /* Shadow Update */
    /* */
    /*PHY_RFShadowWrite(Adapter, eRFPath, Offset, Data); */

    /* */
    /* Switch page for 8256 RF IC */
    /* */
    NewOffset = Offset;

    /* */
    /* Put write addr in [5:0]  and write data in [31:16] */
    /* */
    /*DataAndAddr = (Data<<16) | (NewOffset&0x3f); */
    DataAndAddr = ((NewOffset << 20) | (Data & 0x000fffff)) & 0x0fffffff;   /* T65 RF */

    /* */
    /* Write Operation */
    /* */
    hw_write_bb_reg(nic_info, pPhyReg->rf3wireOffset, bMaskDWord, DataAndAddr);
    /*RTPRINT(FPHY, PHY_RFW, ("RFW-%d Addr[0x%lx]=0x%lx\n", eRFPath, pPhyReg->rf3wireOffset, DataAndAddr)); */

}

void hw_write_rf_reg(    nic_info_st *nic_info,wf_u8 eRFPath,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data)
{
    wf_u32      Original_Value, BitShift;

    /* RF data is 12 bits only */
    if (BitMask != bRFRegOffsetMask) 
    {
        Original_Value = phy_RFSerialRead_8188F(nic_info, eRFPath, RegAddr);
        BitShift =  phy_CalculateBitShift(BitMask);
        Data = ((Original_Value & (~BitMask)) | (Data << BitShift));
    }

    phy_RFSerialWrite_8188F(nic_info, eRFPath, RegAddr, Data);
}


wf_s8 hw_CCKRSSI(wf_u8 LNA_idx,wf_u8 VGA_idx)
{
    wf_s8 rx_pwr_all    =0x00;
    
    switch(LNA_idx)
    {
        case 7:
            if (VGA_idx <= 27)
            {
                rx_pwr_all = -100 + 2 * (27 - VGA_idx);
            }
            else
            {
                rx_pwr_all = -100;
            }
            break;

        case 5:
            rx_pwr_all = -74 + 2 * (21 - VGA_idx);
            break;

        case 3:
            rx_pwr_all = -60 + 2 * (20 - VGA_idx);
            break;

        case 1:
            rx_pwr_all = -44 + 2 * (19 - VGA_idx);
            break;

        default:
        break;
    }
    return  rx_pwr_all;
}

wf_u8 odm_QueryRxPwrPercentage(wf_s8 AntPower)
{
    if ((AntPower <= -100) || (AntPower >= 20))
    {
        return  0;
    }
    else if (AntPower >= 0)
    {
        return  100;
    }
    else
    {
        return  (100+AntPower);
    }
    
}

static wf_u8 odm_EVMdbToPercentage(wf_s8 Value)
{
    //
    // -33dB~0dB to 0%~99%
    //
    wf_s8 ret_val;
    
    ret_val = Value;
    ret_val /= 2;

    if (ret_val >= 0)
    {   
        ret_val = 0;
    }

    if (ret_val <= -33)
    {   
        ret_val = -33;
    }

    ret_val = 0 - ret_val;
    ret_val *= 3;

    if (ret_val == 99)
    {   
        ret_val = 100;
    }

    return (wf_u8)ret_val;
}
    

static void hw_query_phystatus(nic_info_st *pnic_info, hw_recv_phy_status_st *prx_phyinfo)
{
    ars_st *pars = NULL;
    SWAT_T  *pDM_SWAT_Table = NULL;
    wf_u8   i = 0;
    wf_u8   Max_spatial_stream = 0;
    wf_s8   rx_pwr[4];
    wf_s8               rx_pwr_all  =0;
    wf_u8   EVM         = 0;
    wf_u8   PWDB_ALL    = 0;
    wf_u8   PWDB_ALL_BT = 0;
    wf_u8   RSSI, total_rssi=   0;
    wf_bool isCCKrate   =wf_false;  
    wf_u8   rf_rx_num   = 0;
    wf_u8   cck_highpwr = 0;
    wf_u8   LNA_idx     = 0;
    wf_u8   VGA_idx     = 0;
    
    if(NULL == pnic_info || NULL == prx_phyinfo)
    {
        ARS_DBG("input param is null");
        return ;
    }
    pars = pnic_info->ars;
    if(NULL == pars)
    {
        ARS_DBG("pars is null");
        return;
    }
    pDM_SWAT_Table = &pars->DM_SWAT_Table;
    
    isCCKrate = (pars->hwconfig.pkt_info.DataRate <= ODM_RATE11M) ? wf_true : wf_false;
    if(isCCKrate)
    {
        wf_u8 report;
        wf_u8 cck_agc_rpt;

        pars->dbg.dbg_info.NumQryPhyStatusCCK++;
        // 
        // (1)Hardware does not provide RSSI for CCK
        // (2)PWDB, Average PWDB cacluated by hardware (for rate adaptive)
        //

        //if(pHalData->eRFPowerState == eRfOn)
        cck_highpwr = pars->bCckHighPower;
        //else
        //  cck_highpwr = FALSE;

        cck_agc_rpt =  prx_phyinfo->cck_agc_rpt_ofdm_cfosho_a ;


        LNA_idx = ((cck_agc_rpt & 0xE0) >>5);
        VGA_idx = (cck_agc_rpt & 0x1F); 

        rx_pwr_all = hw_CCKRSSI(LNA_idx, VGA_idx);
        PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);
        if (PWDB_ALL > 100)
        {         
            PWDB_ALL = 100;
        }

        pars->cck_lna_idx = LNA_idx;
        pars->cck_vga_idx = VGA_idx;
        pars->hwconfig.phystatus.RxPWDBAll = PWDB_ALL;
        pars->hwconfig.phystatus.BTRxRSSIPercentage = PWDB_ALL;
        pars->hwconfig.phystatus.RecvSignalPower = rx_pwr_all;
        //
        // (3) Get Signal Quality (EVM)
        //
        //if(pPktinfo->bPacketMatchBSSID)
        {
            wf_u8   SQ,SQ_rpt;
            
            if(pars->hwconfig.phystatus.RxPWDBAll > 40 && !pars->bInHctTest)
            {
                SQ = 100;
            }
            else
            {
                SQ_rpt = prx_phyinfo->cck_sig_qual_ofdm_pwdb_all;
                    
                if(SQ_rpt > 64)
                {
                    SQ = 0;
                }
                else if (SQ_rpt < 20)
                {
                    SQ = 100;
                }
                else
                {
                    SQ = ((64-SQ_rpt) * 100) / 44;
                }
            
            }
            
            pars->hwconfig.phystatus.SignalQuality = SQ;
            pars->hwconfig.phystatus.RxMIMOSignalQuality[ODM_RF_PATH_A] = SQ;
            pars->hwconfig.phystatus.RxMIMOSignalQuality[ODM_RF_PATH_B] = -1;
        }

        for (i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++) 
        {
            if (i == 0)
            {         
                pars->hwconfig.phystatus.RxMIMOSignalStrength[0] = PWDB_ALL;
            }
            else
            {         
                pars->hwconfig.phystatus.RxMIMOSignalStrength[1] = 0;
            }
        }
    }
    else //OFDM
    {
        pars->dbg.dbg_info.NumQryPhyStatusOFDM++;

        // 
        // (1)Get RSSI for HT rate
        //
        
         for(i = ODM_RF_PATH_A; i < ODM_RF_PATH_MAX; i++)   
        {
            // 2008/01/30 MH we will judge RF RX path now.
            if (pars->RFPathRxEnable & BIT(i))
            {         
                rf_rx_num++;
             }

            rx_pwr[i] = ((prx_phyinfo->path_agc[i].gain& 0x3F)*2) - 110;
            pars->ofdm_agc_idx[i] = (prx_phyinfo->path_agc[i].gain & 0x3F);

            pars->hwconfig.phystatus.RxPwr[i] = rx_pwr[i];

            /* Translate DBM to percentage. */
            RSSI = odm_QueryRxPwrPercentage(rx_pwr[i]);
            total_rssi += RSSI;
           
            pars->hwconfig.phystatus.RxMIMOSignalStrength[i] =(wf_u8) RSSI;

            //Get Rx snr value in DB        
            pars->hwconfig.phystatus.RxSNR[i] = pars->dbg.dbg_info.RxSNRdB[i] = (wf_s32)(prx_phyinfo->path_rxsnr[i]/2);
        
        }
        
        
        //
        // (2)PWDB, Average PWDB cacluated by hardware (for rate adaptive)
        //
        rx_pwr_all = (((prx_phyinfo->cck_sig_qual_ofdm_pwdb_all) >> 1 )& 0x7f) -110;        
        
        PWDB_ALL_BT = PWDB_ALL = odm_QueryRxPwrPercentage(rx_pwr_all);  
    
    
        pars->hwconfig.phystatus.RxPWDBAll = PWDB_ALL;
        pars->hwconfig.phystatus.BTRxRSSIPercentage = PWDB_ALL_BT;
        pars->hwconfig.phystatus.RxPower = rx_pwr_all;
        pars->hwconfig.phystatus.RecvSignalPower = rx_pwr_all;
        
        
        
        //
        // (3)EVM of HT rate
        //
        if(pars->hwconfig.pkt_info.DataRate >=ODM_RATEMCS8 && pars->hwconfig.pkt_info.DataRate <=ODM_RATEMCS15)
        {      
            Max_spatial_stream = 2; //both spatial stream make sense
        }
        else
        {      
            Max_spatial_stream = 1; //only spatial stream 1 makes sense
        }

        for(i=0; i<Max_spatial_stream; i++)
        {
            // Do not use shift operation like "rx_evmX >>= 1" because the compilor of free build environment
            // fill most significant bit to "zero" when doing shifting operation which may change a negative 
            // value to positive one, then the dbm value (which is supposed to be negative)  is not correct anymore.            
            EVM = odm_EVMdbToPercentage( (prx_phyinfo->stream_rxevm[i] ));  //dbm

            //GET_RX_STATUS_DESC_RX_MCS(pDesc), pDrvInfo->rxevm[i], "%", EVM));
            
            //if(pPktinfo->bPacketMatchBSSID)
            {
                if(i==ODM_RF_PATH_A) // Fill value in RFD, Get the first spatial stream only
                {                       
                    pars->hwconfig.phystatus.SignalQuality = (wf_u8)(EVM & 0xff);
                }                   
                pars->hwconfig.phystatus.RxMIMOSignalQuality[i] = (wf_u8)(EVM & 0xff);
            }
        }
        

        ODM_ParsingCFO(pars, &pars->hwconfig.pkt_info, prx_phyinfo->path_cfotail);
        
    }


    //UI BSS List signal strength(in percentage), make it good looking, from 0~100.
    //It is assigned to the BSS List in GetValueFromBeaconOrProbeRsp().
    if(isCCKrate)
    {       
        pars->hwconfig.phystatus.SignalStrength = (wf_u8)PWDB_ALL;
    }
    else
    {   
        if (rf_rx_num != 0)
        {           

            total_rssi/=rf_rx_num;
            pars->hwconfig.phystatus.SignalStrength = (wf_u8)total_rssi;
        }
    }

}
void wf_ars_query_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf, prx_pkt_t ppt)
{
    ars_st *pars = NULL;
    wf_bool bPacketMatchBSSID     = wf_false;
    wf_bool bPacketBeacon         = wf_false;
    hw_info_st *hw_info           = NULL;
    wdn_net_info_st *wdn_net_info = NULL;
    if(NULL == nic_info || NULL == rps || NULL == pbuf || NULL == ppt)
    {
        ARS_DBG("input param is null");
        return;
    }
    pars = nic_info->ars;
    if(NULL == pars)
    {
        ARS_DBG("pars is null");
        return;
    }

    hw_info = nic_info->hw_info;
    if(NULL == hw_info)
    {
        ARS_DBG("hw_info is null");
        return;
    }
    
    bPacketMatchBSSID  = (!IsFrameTypeCtrl(pbuf)) && (!ppt->pkt_info.icv_err) && (!ppt->pkt_info.crc_err) && (0 == wf_memcmp(get_hdr_bssid(pbuf), wdn_net_info->mac,WF_ETH_ALEN));
    if(bPacketMatchBSSID && (GetFrameSubType(pbuf) == WIFI_BEACON))
    {
        bPacketBeacon = wf_true;
        pars->timectl.bcn_cnt++;
    }
    else
    {
        bPacketBeacon = wf_false;
    }

    /* DataRate */
    pars->hwconfig.pkt_info.DataRate  = ppt->pkt_info.rx_rate;

    ARS_DBG();
    /* StationID */
    wdn_net_info  = wf_wdn_find_info(nic_info,get_hdr_bssid(pbuf));
    if (wdn_net_info == NULL)
    {
        pars->hwconfig.pkt_info.StationID = wdn_net_info->wdn_id;
    }
    else
    {
        pars->hwconfig.pkt_info.StationID = 0xff;
    }
    ARS_DBG();
    /* bPacketMatchBSSID */
    pars->hwconfig.pkt_info.bPacketMatchBSSID   = bPacketMatchBSSID;

    /* bPacketToSelf */
    if(bPacketMatchBSSID && (0 == wf_memcmp(hw_info->macAddr,get_ra(pbuf),WF_ETH_ALEN)))
    {
        pars->hwconfig.pkt_info.bPacketToSelf = wf_true;
    }
    else
    {
        pars->hwconfig.pkt_info.bPacketToSelf = wf_false;
    }
    ARS_DBG();
    /* bPacketBeacon */
    pars->hwconfig.pkt_info.bPacketBeacon       = bPacketBeacon;

    /* bToSelf */
    if(0 == wf_memcmp(hw_info->macAddr,get_ra(pbuf),WF_ETH_ALEN))
    {
        pars->hwconfig.pkt_info.bToSelf  = wf_true;
    }
    else
    {
        pars->hwconfig.pkt_info.bToSelf  = wf_false;
    }
    ARS_DBG();
    hw_query_phystatus(nic_info,(hw_recv_phy_status_st*)rps);

}

#endif
