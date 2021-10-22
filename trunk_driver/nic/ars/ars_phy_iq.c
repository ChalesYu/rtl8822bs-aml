#include "common.h"
#include "wf_debug.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT
#if 1
#define ARS_PHYIQ_DBG(fmt, ...)      LOG_D("ARS_PHYIQ[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_PHYIQ_PRT(fmt, ...)      LOG_D("ARS_PHYIQ-"fmt,##__VA_ARGS__)

#else
#define ARS_PHYIQ_DBG(fmt, ...)
#define ARS_PHYIQ_PRT(fmt, ...) 

#endif
#define ARS_PHYIQ_INFO(fmt, ...)      LOG_I("ARS_PHY_IQ-"fmt,##__VA_ARGS__)
#define ARS_PHYIQ_ERR(fmt, ...)      LOG_E("ARS_PHY_IQ-"fmt,##__VA_ARGS__)

#if defined(CONFIG_RICHV100)
#define RF_LOCK_DIV                        0xC6
#else
#define RF_LOCK_DIV                        0xC8
#endif

wf_s32 phy_lc_calibrate(void      *ars)
{
    wf_s32 ret = -1;
    int err = 0;
    wf_u8 tempReg = 0;
    wf_u32 lc_cal = 0;
    wf_u32 val = 0;
    int cnt = 0;
    wf_u32 chbwB15 = 0;
    wf_u32 lockdivb1 = 0;
    ars_st *pars = ars;    
    
    #if defined(CONFIG_RICHV100)
        ARS_PHYIQ_DBG("%s(): Need add process for rich v100\n",__func__);
        return WF_RETURN_OK;
    #endif
    
    if(!(pars->SupportAbility & ODM_RF_CALIBRATION))
    {   
        ARS_PHYIQ_DBG("%s(): Return: SupportAbility ODM_RF_CALIBRATION is disabled\n",__func__);
        return  WF_RETURN_OK;
    }

    val = hw_read_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_LOCK_DIV, bRFRegOffsetMask);
    if ((val & WF_BIT(1)) != 0)
    {
        ARS_PHYIQ_DBG("%s(): No need lc calibrate",__func__);
        return WF_RETURN_OK;
    }

    pars->RFCalibrateInfo.bLCKInProgress = wf_true;

    /* stop tx in BB */
    tempReg = wf_io_read8(pars->nic_info, 0xd03, &err);
    if ((tempReg & 0x70) != 0)
    {
        wf_io_write8(pars->nic_info, 0xd03, tempReg&0x8F);
    }
    else
    {
        wf_io_write8(pars->nic_info, 0x522, 0xFF);
    }

    lc_cal = hw_read_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);
    hw_write_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, lc_cal|WF_BIT(15));
    wf_msleep(1000);

    for (cnt = 0; cnt < 100; cnt++)
    {
        chbwB15 = hw_read_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_CHNLBW, WF_BIT(15));
        lockdivb1 = hw_read_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_LOCK_DIV, WF_BIT(1));
        
        if ((lockdivb1 == 1)&&(chbwB15 == 0))
        {
            ARS_PHYIQ_DBG("%s(): lock success",__func__);
            ret = WF_RETURN_OK;
            break;
        }

        wf_msleep(5);
    }

    /* Recover tx in BB */
    if ((tempReg & 0x70) != 0)
    {
        wf_io_write8(pars->nic_info, 0xd03, tempReg);
    }
    else
    {
        wf_io_write8(pars->nic_info, 0x522, 0x00);
    }

    pars->RFCalibrateInfo.bLCKInProgress = wf_false;

    return WF_RETURN_FAIL;
}


#endif

