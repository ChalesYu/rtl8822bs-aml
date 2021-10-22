
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT

#define ARS_PT_DBG(fmt, ...)      LOG_D("ARS_PT[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_PT_PRT(fmt, ...)      LOG_D("ARS_PT-"fmt,##__VA_ARGS__)
#define ARS_PT_INFO(fmt, ...)      LOG_I("ARS_PT-"fmt,##__VA_ARGS__)
#define ARS_PT_ERR(fmt, ...)      LOG_E("ARS_PT-"fmt,##__VA_ARGS__)

wf_u32 OFDMSwingTable_New[OFDM_TABLE_SIZE] =
{
    0x0b40002d, // 0,  -15.0dB
    0x0c000030, // 1,  -14.5dB
    0x0cc00033, // 2,  -14.0dB
    0x0d800036, // 3,  -13.5dB
    0x0e400039, // 4,  -13.0dB
    0x0f00003c, // 5,  -12.5dB
    0x10000040, // 6,  -12.0dB
    0x11000044, // 7,  -11.5dB
    0x12000048, // 8,  -11.0dB
    0x1300004c, // 9,  -10.5dB
    0x14400051, // 10, -10.0dB
    0x15800056, // 11, -9.5dB
    0x16c0005b, // 12, -9.0dB
    0x18000060, // 13, -8.5dB
    0x19800066, // 14, -8.0dB
    0x1b00006c, // 15, -7.5dB
    0x1c800072, // 16, -7.0dB
    0x1e400079, // 17, -6.5dB
    0x20000080, // 18, -6.0dB
    0x22000088, // 19, -5.5dB
    0x24000090, // 20, -5.0dB
    0x26000098, // 21, -4.5dB
    0x288000a2, // 22, -4.0dB
    0x2ac000ab, // 23, -3.5dB
    0x2d4000b5, // 24, -3.0dB
    0x300000c0, // 25, -2.5dB
    0x32c000cb, // 26, -2.0dB
    0x35c000d7, // 27, -1.5dB
    0x390000e4, // 28, -1.0dB
    0x3c8000f2, // 29, -0.5dB
    0x40000100, // 30, +0dB
    0x43c0010f, // 31, +0.5dB
    0x47c0011f, // 32, +1.0dB
    0x4c000130, // 33, +1.5dB
    0x50800142, // 34, +2.0dB
    0x55400155, // 35, +2.5dB
    0x5a400169, // 36, +3.0dB
    0x5fc0017f, // 37, +3.5dB
    0x65400195, // 38, +4.0dB
    0x6b8001ae, // 39, +4.5dB
    0x71c001c7, // 40, +5.0dB
    0x788001e2, // 41, +5.5dB
    0x7f8001fe  // 42, +6.0dB
};

wf_u8 getSwingIndex(void *ars)
{
    ars_st* pars = ars;
    wf_u8           i = 0;
    wf_u32          bbSwing = 0;
    wf_u32          swingTableSize = 0;
    wf_u32 *            pSwingTable = NULL;


    bbSwing = hw_read_bb_reg(pars->nic_info, rOFDM0_XATxIQImbalance, 0xFFC00000);

    pSwingTable = OFDMSwingTable_New;
    swingTableSize = OFDM_TABLE_SIZE;


    for (i = 0; i < swingTableSize; ++i)
    {
        wf_u32 tableValue = pSwingTable[i];

        if (tableValue >= 0x100000 )
        {
            tableValue >>= 22;
        }
        if (bbSwing == tableValue)
        {
            break;
        }
    }
    return i;
}


wf_s32 odm_TXPowerTrackingThermalMeterInit(void *ars)
{
    ars_st*     pars = ars;
    wf_u8 defaultSwingIndex = 0;
    wf_u8           p = 0;
    PODM_RF_CAL_T   pRFCalibrateInfo = &(pars->RFCalibrateInfo);

    defaultSwingIndex = getSwingIndex(pars);
    pRFCalibrateInfo->bTXPowerTracking = wf_true;
    pRFCalibrateInfo->TXPowercount = 0;
    pRFCalibrateInfo->bTXPowerTrackingInit = wf_false;

    if(pars->mp_mode == wf_false)
    {
        pRFCalibrateInfo->TxPowerTrackControl = wf_true;
    }
    else
    {
        pRFCalibrateInfo->TxPowerTrackControl = wf_false;
    }

    if(pars->mp_mode == wf_false)
    {
        pRFCalibrateInfo->TxPowerTrackControl = wf_true;
    }


    LOG_I("pars TxPowerTrackControl = %d\n", pRFCalibrateInfo->TxPowerTrackControl);



    //pars->RFCalibrateInfo.TxPowerTrackControl = TRUE;
    pRFCalibrateInfo->ThermalValue      =    pars->eeprom_data.EEPROMThermalMeter;
    pRFCalibrateInfo->ThermalValue_IQK  = pars->eeprom_data.EEPROMThermalMeter;
    pRFCalibrateInfo->ThermalValue_LCK  = pars->eeprom_data.EEPROMThermalMeter;


    pRFCalibrateInfo->DefaultOfdmIndex =28;                         //OFDM: -1dB
    pRFCalibrateInfo->DefaultCckIndex =20;                          //CCK:-6dB

    pRFCalibrateInfo->BbSwingIdxCckBase = pRFCalibrateInfo->DefaultCckIndex;
    pRFCalibrateInfo->CCK_index = pRFCalibrateInfo->DefaultCckIndex;

    for (p = ODM_RF_PATH_A; p < MAX_RF_PATH; ++p)
    {
        pRFCalibrateInfo->BbSwingIdxOfdmBase[p] = pRFCalibrateInfo->DefaultOfdmIndex;
        pRFCalibrateInfo->OFDM_index[p] = pRFCalibrateInfo->DefaultOfdmIndex;
        pRFCalibrateInfo->DeltaPowerIndex[p] = 0;
        pRFCalibrateInfo->DeltaPowerIndexLast[p] = 0;
        pRFCalibrateInfo->PowerIndexOffset[p] = 0;
    }
    pRFCalibrateInfo->Modify_TxAGC_Value_OFDM=0;            //add by Mingzhi.Guo
    pRFCalibrateInfo->Modify_TxAGC_Value_CCK=0;         //add by Mingzhi.Guo

    return WF_RETURN_OK;
}

//======================================================================
// <20121113, Kordan> This function should be called when TxAGC changed.
// Otherwise the previous compensation is gone, because we record the
// delta of temperature between two TxPowerTracking watch dogs.
//
// NOTE: If Tx BB swing or Tx scaling is varified during run-time, still
//       need to call this function.
//======================================================================
wf_s32 ODM_ClearTxPowerTrackingState(void *ars    )
{
    ars_st*       pars = ars;
    wf_u8          p = 0;
    PODM_RF_CAL_T   pRFCalibrateInfo = &(pars->RFCalibrateInfo);

    pRFCalibrateInfo->BbSwingIdxCckBase = pRFCalibrateInfo->DefaultCckIndex;
    pRFCalibrateInfo->BbSwingIdxCck = pRFCalibrateInfo->DefaultCckIndex;
    pars->RFCalibrateInfo.CCK_index = 0;

    for (p = ODM_RF_PATH_A; p < MAX_RF_PATH; ++p)
    {
        pRFCalibrateInfo->BbSwingIdxOfdmBase[p] = pRFCalibrateInfo->DefaultOfdmIndex;
        pRFCalibrateInfo->BbSwingIdxOfdm[p] = pRFCalibrateInfo->DefaultOfdmIndex;
        pRFCalibrateInfo->OFDM_index[p] = pRFCalibrateInfo->DefaultOfdmIndex;

        pRFCalibrateInfo->PowerIndexOffset[p] = 0;
        pRFCalibrateInfo->DeltaPowerIndex[p] = 0;
        pRFCalibrateInfo->DeltaPowerIndexLast[p] = 0;
        pRFCalibrateInfo->PowerIndexOffset[p] = 0;

        pRFCalibrateInfo->Absolute_OFDMSwingIdx[p] = 0;    /* Initial Mix mode power tracking*/
        pRFCalibrateInfo->Remnant_OFDMSwingIdx[p] = 0;
        pRFCalibrateInfo->KfreeOffset[p] = 0;
    }

    pRFCalibrateInfo->Modify_TxAGC_Flag_PathA = wf_false;       /*Initial at Modify Tx Scaling Mode*/
    pRFCalibrateInfo->Modify_TxAGC_Flag_PathB = wf_false;       /*Initial at Modify Tx Scaling Mode*/
    pRFCalibrateInfo->Modify_TxAGC_Flag_PathC = wf_false;       /*Initial at Modify Tx Scaling Mode*/
    pRFCalibrateInfo->Modify_TxAGC_Flag_PathD = wf_false;       /*Initial at Modify Tx Scaling Mode*/
    pRFCalibrateInfo->Remnant_CCKSwingIdx = 0;
    pRFCalibrateInfo->ThermalValue = pars->eeprom_data.EEPROMThermalMeter;

    pRFCalibrateInfo->Modify_TxAGC_Value_CCK=0;         //modify by Mingzhi.Guo
    pRFCalibrateInfo->Modify_TxAGC_Value_OFDM=0;        //modify by Mingzhi.Guo

    return WF_RETURN_OK;
}

wf_s32 phydm_rf_init(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_PT_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    ARS_PT_INFO("start");
    pars = ars;

    odm_TXPowerTrackingInit(pars);
    ODM_ClearTxPowerTrackingState(pars);

    return WF_RETURN_OK;
}
wf_s32 odm_TXPowerTrackingInit(void *ars)
{
    ars_st *pars = NULL;

    if(NULL == ars)
    {
        ARS_PT_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;

    odm_TXPowerTrackingThermalMeterInit(pars);

    return WF_RETURN_OK;
}

#endif

