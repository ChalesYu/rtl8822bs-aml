#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_DRIVER_SUPPORT
#if 0
#define ARS_HW_DBG(fmt, ...)      LOG_D("ARS_HW[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_HW_PRT(fmt, ...)      LOG_D("ARS_HW-"fmt,##__VA_ARGS__)

#else
#define ARS_HW_DBG(fmt, ...)
#define ARS_HW_PRT(fmt, ...)
#endif

#define ARS_HW_INFO(fmt, ...)      LOG_I("ARS_DIG-"fmt,##__VA_ARGS__)
#define ARS_HW_ERR(fmt, ...)      LOG_E("ARS_DIG-"fmt,##__VA_ARGS__)

// Rx smooth factor
#define	Rx_Smooth_Factor (20)

static wf_u8 _is_fw_read_cmd_down(nic_info_st *nic_info, wf_u8 msgbox_num)
{
    wf_u8  read_down = wf_false;
    int retry_cnts = 100;
    wf_u8 valid;

    do
    {
        valid = wf_io_read8(nic_info, REG_HMETFR,NULL) & BIT(msgbox_num);
        if (0 == valid)
        {
            read_down = wf_true;
        }
        else
        {
            wf_mdelay(1);
         }
    } while ((!read_down) && (retry_cnts--));

    return read_down;

}


/*****************************************
* H2C Msg format :
*| 31 - 8       |7-5    | 4 - 0 |
*| h2c_msg  |Class  |CMD_ID |
*| 31-0                     |
*| Ext msg                  |
*
******************************************/
wf_s32 FillH2CCmd(nic_info_st *nic_info, wf_u8 ElementID, wf_u32 CmdLen, wf_u8 *pCmdBuffer)
{
    wf_u8  h2c_box_num;
    wf_u32 msgbox_addr;
    wf_u32 msgbox_ex_addr = 0;
    wf_u32 h2c_cmd = 0;
    wf_u32 h2c_cmd_ex = 0;
    wf_s32 ret = WF_RETURN_FAIL;
    ars_st *pars = nic_info->ars;
    if (!pCmdBuffer)
    {
        LOG_E("[%s] pCmdBuffer is null",__func__);
        return ret;
    }
    if (CmdLen > RTL8188F_MAX_CMD_LEN)
    {
        LOG_E("[%s] CmdLen[%d] > RTL8188F_MAX_CMD_LEN[%d]",__func__,CmdLen,RTL8188F_MAX_CMD_LEN);
        return ret;
    }
    if (nic_info->is_driver_stopped || nic_info->is_surprise_removed)
    {
        return WF_RETURN_FAIL;
    }

    ars_io_lock_try(pars);
    /*pay attention to if  race condition happened in  H2C cmd setting. */
    h2c_box_num = pars->LastHMEBoxNum;

    if (!_is_fw_read_cmd_down(nic_info, h2c_box_num))
    {
        LOG_I(" fw read cmd failed...,h2c_box_num:%d\n",h2c_box_num);
        LOG_I("[%s] MAC_1C0=%08x, MAC_1C4=%08x, MAC_1C8=%08x, MAC_1CC=%08x\n",
            __func__,
            wf_io_read32(nic_info, 0x1c0,NULL),
            wf_io_read32(nic_info, 0x1c4,NULL),
            wf_io_read32(nic_info, 0x1c8,NULL),
            wf_io_read32(nic_info, 0x1cc,NULL));
        LOG_I("[%s] 0x1c0: 0x%8x,0x1c4: 0x%8x\n",
            __func__,
            wf_io_read32(nic_info, 0x1c0,NULL),
            wf_io_read32(nic_info, 0x1c4,NULL));
        return ret;
    }

    if (CmdLen <= 3)
    {
        wf_memcpy((wf_u8 *)(&h2c_cmd) + 1, pCmdBuffer, CmdLen);
    }
    else
    {
        wf_memcpy((wf_u8 *)(&h2c_cmd) + 1, pCmdBuffer, 3);
        wf_memcpy((wf_u8 *)(&h2c_cmd_ex), pCmdBuffer + 3, CmdLen - 3);
        /**(wf_u8*)(&h2c_cmd) |= BIT(7); */
    }

    *(wf_u8 *)(&h2c_cmd) |= ElementID;

    if (CmdLen > 3)
    {
        msgbox_ex_addr = REG_HMEBOX_EXT0_8188F + (h2c_box_num * RTL8188F_EX_MESSAGE_BOX_SIZE);
        h2c_cmd_ex = wf_le32_to_cpu(h2c_cmd_ex);
        wf_io_write32(nic_info, msgbox_ex_addr, h2c_cmd_ex);
    }

    msgbox_addr = REG_HMEBOX_0 + (h2c_box_num * MESSAGE_BOX_SIZE);
    h2c_cmd = wf_le32_to_cpu(h2c_cmd);
    wf_io_write32(nic_info, msgbox_addr, h2c_cmd);

    LOG_I("[%s] MSG_BOX:%d, CmdLen(%d), CmdID(0x%x), reg:0x%x =>h2c_cmd:0x%.8x, reg:0x%x =>h2c_cmd_ex:0x%.8x\n"
        ,__func__,pars->LastHMEBoxNum , CmdLen, ElementID, msgbox_addr, h2c_cmd, msgbox_ex_addr, h2c_cmd_ex);

    pars->LastHMEBoxNum = (h2c_box_num + 1) % MAX_H2C_BOX_NUMS;
    ars_io_unlock_try(pars);

    return WF_RETURN_OK;
}

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

wf_s32 hw_write_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data)
{
    wf_s32 err;
    wf_u32          OriginalValue, BitShift;


    //LOG_I("--->%s(): RegAddr(%#x), BitMask(%#x), Data(%#x)\n", __func__,RegAddr, BitMask, Data); 

    if (BitMask != bMaskDWord) 
    { 
        /*if not "double word" write */
        OriginalValue = wf_io_read32(pnic_info, RegAddr,&err);
        if(err)
        {
            LOG_E("[%s,%d] wf_io_read32 failed",__func__,__LINE__);
            return WF_RETURN_FAIL;
        }
        BitShift = phy_CalculateBitShift(BitMask);
        Data = ((OriginalValue & (~BitMask)) | ((Data << BitShift) & BitMask));
    }

    wf_io_write32(pnic_info, RegAddr, Data);

    return WF_RETURN_OK;
}

wf_u32 hw_read_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask)
{
    wf_u32  ReturnValue = 0;
    wf_u32 OriginalValue = 0;
    wf_u32 BitShift = 0;
    wf_s32  err= 0;

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

wf_s32 phy_InitBBRFRegisterDefinition(void *ars)
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

    return WF_RETURN_OK;
}

static wf_u32 phy_RFSerialRead_8188F(nic_info_st *nic_info, RF_PATH eRFPath,wf_u32 Offset)
{
    ars_st *pars                        = nic_info->ars;
    wf_u32 retValue                     = 0;
    ars_hwconfig_info_st *hwcfg         = &pars->hwconfig;
    BB_REGISTER_DEFINITION_T *pPhyReg   = &hwcfg->PHYRegDef[eRFPath];
    wf_u32 NewOffset                    = 0;
    //wf_u32 tmplong                      = 0;
    wf_u32 tmplong2                     = 0;
    wf_u8 RfPiEnable                    = 0;
    wf_u32 MaskforPhySet                = 0;

    /* */
    /* Make sure RF register offset is correct */
    /* */
    Offset &= 0xff;

    NewOffset = Offset;

    if (eRFPath == RF_PATH_A) 
    {
        tmplong2 = hw_read_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord);
        tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;    /*T65 RF */
        hw_write_bb_reg(nic_info, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
    } 
    else 
    {
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

static wf_s32 phy_RFSerialWrite_8188F(nic_info_st *nic_info,RF_PATH eRFPath,wf_u32 Offset,wf_u32 Data)
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

    return WF_RETURN_OK;
}

wf_s32 hw_write_rf_reg(    nic_info_st *nic_info,wf_u8 eRFPath,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data)
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

    return WF_RETURN_OK;
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
    
wf_s32 odm_Process_RSSIForDM(void *ars)
{
    ars_st *pars = ars;
    wf_s32 UndecoratedSmoothedPWDB = 0;
    wf_s32 UndecoratedSmoothedCCK = 0;
    wf_s32 UndecoratedSmoothedOFDM = 0;
    wf_s32 RSSI_Ave= 0;
    wf_u8  i = 0;
    wf_u8 isCCKrate=0; 
    wf_u8 RSSI_max = 0;
    wf_u8 RSSI_min = 0;
    wf_u32          Weighting=0;
    wdn_net_info_st *pwdn = NULL;
    RSSI_STA *rssi_sta    = NULL;

    if (pars->hwconfig.pkt_info.StationID >= ODM_ASSOCIATE_ENTRY_NUM)
    {
        return WF_RETURN_OK;
    }

    rssi_sta = &pars->rssi_sta[pars->hwconfig.pkt_info.StationID];
    #ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
    odm_S0S1_SwAntDivByCtrlFrame_ProcessRSSI(pars, pars->hwconfig.phystatus, &pars->hwconfig.pkt_info);
    #endif

    //
    // 2012/05/30 MH/Luke.Lee Add some description 
    // In windows driver: AP/IBSS mode STA
    //
    //if (pars->SupportPlatform == ODM_WIN)
    //{
    //  pEntry = pars->pODM_StaInfo[pars->pAidMap[pPktinfo->StationID-1]];            
    //}
    //else
    pwdn = wf_wdn_find_info_by_id(pars->nic_info, pars->hwconfig.pkt_info.StationID);
    if(NULL == pwdn)
    {
        LOG_W("[%s,%d] can't find StationID(%d) for wdn",__func__,__LINE__,pars->hwconfig.pkt_info.StationID);
        return WF_RETURN_FAIL;
    }

    if ((!pars->hwconfig.pkt_info.bPacketMatchBSSID))/*data frame only*/
    {
        return WF_RETURN_OK;
    }

    if(pars->hwconfig.pkt_info.bPacketBeacon)
    {
        pars->dbg.dbg_info.NumQryBeaconPkt++;
    }
    
    isCCKrate = (pars->hwconfig.pkt_info.DataRate <= ODM_RATE11M )?wf_true :wf_false;
    pars->RxRate = pars->hwconfig.pkt_info.DataRate;

    //--------------Statistic for antenna/path diversity------------------
    if(pars->SupportAbility & ODM_BB_ANT_DIV)
    {
        #if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
            ODM_Process_RSSIForAntDiv(pars,&pars->hwconfig.phystatus,&pars->hwconfig.pkt_info);
        #endif
    }
    #if(defined(CONFIG_PATH_DIVERSITY))
    else if(pars->SupportAbility & ODM_BB_PATH_DIV)
    {
        phydm_process_rssi_for_path_div(pars,&pars->hwconfig.phystatus,&pars->hwconfig.pkt_info);
    }
    #endif
    //-----------------Smart Antenna Debug Message------------------//
    
    UndecoratedSmoothedCCK =  rssi_sta->UndecoratedSmoothedCCK;
    UndecoratedSmoothedOFDM = rssi_sta->UndecoratedSmoothedOFDM;
    UndecoratedSmoothedPWDB = rssi_sta->UndecoratedSmoothedPWDB;
    
    if(pars->hwconfig.pkt_info.bPacketToSelf || pars->hwconfig.pkt_info.bPacketBeacon)
    {

        if(!isCCKrate)//ofdm rate
        {
            {
                if (pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_B] == 0) 
                {
                    RSSI_Ave = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A];
                    pars->RSSI_A = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A];
                    pars->RSSI_B = 0;
                } 
                else 
                {
                    /*DbgPrint("pRfd->Status.RxMIMOSignalStrength[0] = %d, pRfd->Status.RxMIMOSignalStrength[1] = %d\n",*/ 
                        /*pRfd->Status.RxMIMOSignalStrength[0], pRfd->Status.RxMIMOSignalStrength[1]);*/
                    pars->RSSI_A =  pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A];
                    pars->RSSI_B = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_B];
                    
                    if (pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A] > pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_B]) 
                    {
                        RSSI_max = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A];
                        RSSI_min = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_B];
                    } 
                    else 
                    {
                        RSSI_max = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_B];
                        RSSI_min = pars->hwconfig.phystatus.RxMIMOSignalStrength[ODM_RF_PATH_A];
                    }
                    if ((RSSI_max - RSSI_min) < 3)
                    {
                        RSSI_Ave = RSSI_max;
                    }
                    else if ((RSSI_max - RSSI_min) < 6)
                    {
                        RSSI_Ave = RSSI_max - 1;
                    }
                    else if ((RSSI_max - RSSI_min) < 10)
                    {
                        RSSI_Ave = RSSI_max - 2;
                    }
                    else
                    {
                        RSSI_Ave = RSSI_max - 3;
                    }
                }
            }
                    
            //1 Process OFDM RSSI
            if(UndecoratedSmoothedOFDM <= 0)    // initialize
            {
                UndecoratedSmoothedOFDM = pars->hwconfig.phystatus.RxPWDBAll;
            }
            else
            {
                if(pars->hwconfig.phystatus.RxPWDBAll > (wf_u32)UndecoratedSmoothedOFDM)
                {
                    UndecoratedSmoothedOFDM =   
                            ( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
                            (RSSI_Ave)) /(Rx_Smooth_Factor);
                    UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM + 1;
                }
                else
                {
                    UndecoratedSmoothedOFDM =   
                            ( ((UndecoratedSmoothedOFDM)*(Rx_Smooth_Factor-1)) + 
                            (RSSI_Ave)) /(Rx_Smooth_Factor);
                }
            }               
            if (rssi_sta->OFDM_pkt != 64) 
            {
                i = 63;
                rssi_sta->OFDM_pkt -= (wf_u32)(((rssi_sta->PacketMap>>i)&BIT(0))-1);
            }
            rssi_sta->PacketMap = (rssi_sta->PacketMap<<1) | BIT(0);
            
                                        
        }
        else
        {
            RSSI_Ave = pars->hwconfig.phystatus.RxPWDBAll;
            pars->RSSI_A = (wf_u8) pars->hwconfig.phystatus.RxPWDBAll;
            pars->RSSI_B = 0xFF;
            pars->RSSI_C = 0xFF;
            pars->RSSI_D = 0xFF;

            //1 Process CCK RSSI
            if(UndecoratedSmoothedCCK <= 0) // initialize
            {
                UndecoratedSmoothedCCK = pars->hwconfig.phystatus.RxPWDBAll;
            }
            else
            {
                if(pars->hwconfig.phystatus.RxPWDBAll > (wf_u32)UndecoratedSmoothedCCK)
                {
                    UndecoratedSmoothedCCK =    
                            ( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
                            (pars->hwconfig.phystatus.RxPWDBAll)) /(Rx_Smooth_Factor);
                    UndecoratedSmoothedCCK = UndecoratedSmoothedCCK + 1;
                }
                else
                {
                    UndecoratedSmoothedCCK =    
                            ( ((UndecoratedSmoothedCCK)*(Rx_Smooth_Factor-1)) + 
                            (pars->hwconfig.phystatus.RxPWDBAll)) /(Rx_Smooth_Factor);
                }
            }
            i = 63;
            rssi_sta->OFDM_pkt -= (wf_u32)((rssi_sta->PacketMap>>i)&BIT(0));          
            rssi_sta->PacketMap = rssi_sta->PacketMap<<1;           
        }

        
        //2011.07.28 LukeLee: modified to prevent unstable CCK RSSI
        if (rssi_sta->OFDM_pkt == 64) 
        { 
            /* speed up when all packets are OFDM*/
            UndecoratedSmoothedPWDB = UndecoratedSmoothedOFDM;
        } 
        else 
        {
            if (rssi_sta->ValidBit < 64)
            {
                rssi_sta->ValidBit++;
            }

            if (rssi_sta->ValidBit == 64) 
            {
                Weighting = ((rssi_sta->OFDM_pkt<<4) > 64)?64:(rssi_sta->OFDM_pkt<<4);
                UndecoratedSmoothedPWDB = (Weighting*UndecoratedSmoothedOFDM+(64-Weighting)*UndecoratedSmoothedCCK)>>6;
            } 
            else 
            {
                if (rssi_sta->ValidBit != 0)
                {
                    UndecoratedSmoothedPWDB = (rssi_sta->OFDM_pkt*UndecoratedSmoothedOFDM+(rssi_sta->ValidBit-rssi_sta->OFDM_pkt)*UndecoratedSmoothedCCK)/rssi_sta->ValidBit;
                }
                else
                {
                    UndecoratedSmoothedPWDB = 0;
                }
            }
        }
        
        //if (rssi_sta->UndecoratedSmoothedPWDB == 0 && wf_false == pars->ra.ra_rpt_linked)
        if (rssi_sta->UndecoratedSmoothedPWDB == 0)
        {
            //phydm_ra_rssi_rpt_wk(pars);
            phydm_rssi_report(pars,pwdn);
        }
        rssi_sta->UndecoratedSmoothedCCK  = UndecoratedSmoothedCCK;
        rssi_sta->UndecoratedSmoothedOFDM = UndecoratedSmoothedOFDM;
        rssi_sta->UndecoratedSmoothedPWDB = UndecoratedSmoothedPWDB;

        ARS_HW_PRT("UndecoratedSmoothedPWDB:%d",UndecoratedSmoothedPWDB);
    }

    return WF_RETURN_OK;
}

static wf_s32 hw_query_phystatus(nic_info_st *pnic_info, hw_recv_phy_status_st *prx_phyinfo)
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
        ARS_HW_DBG("input param is null");
        return WF_RETURN_FAIL;
    }
    pars = pnic_info->ars;
    if(NULL == pars)
    {
        ARS_HW_DBG("pars is null");
        return WF_RETURN_FAIL;
    }
    pDM_SWAT_Table = &pars->DM_SWAT_Table;
    
    isCCKrate = (pars->hwconfig.pkt_info.DataRate <= ODM_RATE11M) ? wf_true : wf_false;
    if(isCCKrate)
    {
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
        if(pars->hwconfig.pkt_info.bPacketMatchBSSID)
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
        ARS_HW_PRT("cck_lna_idx:%d", pars->cck_lna_idx);
        ARS_HW_PRT("cck_vga_idx:%d", pars->cck_vga_idx);
        ARS_HW_PRT("RxPWDBAll:%d",pars->hwconfig.phystatus.RxPWDBAll );
        ARS_HW_PRT("RecvSignalPower:%d",pars->hwconfig.phystatus.RecvSignalPower);
        ARS_HW_PRT("RxMIMOSignalQuality:%d",pars->hwconfig.phystatus.SignalQuality);
        ARS_HW_PRT("RxMIMOSignalStrength:%d",pars->hwconfig.phystatus.RxMIMOSignalStrength[0]);
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
            
            //if(pars->hwconfig.pkt_info->bPacketMatchBSSID)
            {
                if(i==ODM_RF_PATH_A) // Fill value in RFD, Get the first spatial stream only
                {                       
                    pars->hwconfig.phystatus.SignalQuality = (wf_u8)(EVM & 0xff);
                }                   
                pars->hwconfig.phystatus.RxMIMOSignalQuality[i] = (wf_u8)(EVM & 0xff);
            }
        }
        

        ODM_ParsingCFO(pars, &pars->hwconfig.pkt_info, prx_phyinfo->path_cfotail);
        ARS_HW_PRT("ofdm_agc_idx:%d", pars->ofdm_agc_idx[0]);
        ARS_HW_PRT("RxPwr:%d", pars->hwconfig.phystatus.RxPwr[0]);
        ARS_HW_PRT("RSSI:%d",pars->hwconfig.phystatus.RxMIMOSignalStrength[0] );
        ARS_HW_PRT("RxSNR:%d",pars->hwconfig.phystatus.RxSNR[0]);
        ARS_HW_PRT("RxPWDBAll:%d",pars->hwconfig.phystatus.RxPWDBAll);
        ARS_HW_PRT("RxPower:%d",pars->hwconfig.phystatus.RxPower);
        ARS_HW_PRT("RecvSignalPower:%d",pars->hwconfig.phystatus.RecvSignalPower);
        ARS_HW_PRT("RxMIMOSignalQuality:%d",pars->hwconfig.phystatus.RxMIMOSignalQuality[0]);

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

    ARS_HW_PRT("SignalStrength:%d",pars->hwconfig.phystatus.SignalStrength);

    return WF_RETURN_OK;
}
wf_s32 wf_ars_query_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf, prx_pkt_t ppt)
{
    ars_st *pars = NULL;
    wf_bool bPacketMatchBSSID     = wf_false;
    wf_bool bPacketBeacon         = wf_false;
    hw_info_st *hw_info           = NULL;
    wdn_net_info_st *wdn_net_info = NULL;

    
    if(NULL == nic_info || NULL == rps || NULL == pbuf || NULL == ppt)
    {
        ARS_HW_DBG("input param is null");
        return WF_RETURN_FAIL;
    }
    pars = nic_info->ars;
    if(NULL == pars)
    {
        ARS_HW_DBG("pars is null");
        return WF_RETURN_FAIL;
    }

    hw_info = nic_info->hw_info;
    if(NULL == hw_info)
    {
        ARS_HW_DBG("hw_info is null");
        return WF_RETURN_FAIL;
    }

    /* DataRate */
    pars->hwconfig.pkt_info.DataRate  = ppt->pkt_info.rx_rate;
    
    /* StationID */
    wdn_net_info  = wf_wdn_find_info(nic_info,get_hdr_bssid(pbuf));
    if (NULL != wdn_net_info)
    {
        bPacketMatchBSSID  = (!IsFrameTypeCtrl(pbuf)) && (!ppt->pkt_info.icv_err) && (!ppt->pkt_info.crc_err)&& (0 == wf_memcmp(get_hdr_bssid(pbuf), wdn_net_info->mac,WF_ETH_ALEN)) ;
        if(bPacketMatchBSSID && (GetFrameSubType(pbuf) == WIFI_BEACON))
        {
            bPacketBeacon = wf_true;
            pars->ars_thread.bcn_cnt++;
        }

        pars->hwconfig.pkt_info.StationID = wdn_net_info->wdn_id;
    }
    else
    {
        #ifdef CONFIG_MP_MODE
        pars->hwconfig.pkt_info.StationID = 0;
        #else
        pars->hwconfig.pkt_info.StationID = 0xff;
        return WF_RETURN_OK;
        #endif
    }

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
    ARS_HW_PRT("------------------------------------------------------");
    ARS_HW_PRT("DataRate:%x",pars->hwconfig.pkt_info.DataRate);
    ARS_HW_PRT("StationID:%x",pars->hwconfig.pkt_info.StationID);
    ARS_HW_PRT("bPacketMatchBSSID:%x",pars->hwconfig.pkt_info.bPacketMatchBSSID);
    ARS_HW_PRT("bPacketToSelf:%x",pars->hwconfig.pkt_info.bPacketToSelf);
    ARS_HW_PRT("bPacketBeacon:%x",pars->hwconfig.pkt_info.bPacketBeacon);
    ARS_HW_PRT("bToSelf:%x",pars->hwconfig.pkt_info.bToSelf);
    
    hw_query_phystatus(nic_info,(hw_recv_phy_status_st*)rps);

    odm_Process_RSSIForDM(pars);

    return WF_RETURN_OK;
}

#endif
