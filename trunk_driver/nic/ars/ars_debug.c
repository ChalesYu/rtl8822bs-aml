
#undef WF_DEBUG_LEVEL
#define WF_DEBUG_LEVEL (~WF_DEBUG_DEBUG)
#include "common.h"

#ifdef CONFIG_ARS_DRIVER_SUPPORT

#define ARS_DBG(fmt, ...)      LOG_D("ARS_DBG[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#define ARS_PRT(fmt, ...)      LOG_D("ARS_DBG-"fmt,##__VA_ARGS__)
#define DBG 0

wf_s32 ars_debug_init(void *ars)
{
    ars_st *pars = NULL;
    ars_dbg_info_st *dbg = NULL;
    if(NULL == ars)
    {
        ARS_DBG("input param is null");
        return WF_RETURN_FAIL;
    }

    pars = ars;
    dbg = &pars->dbg;
    dbg->DebugLevel = ODM_DBG_TRACE;

    dbg->DebugComponents            = 0;
#if DBG
//BB Functions
    dbg->DebugComponents            |= ODM_COMP_DIG;
    dbg->DebugComponents            |= ODM_COMP_RA_MASK;
    dbg->DebugComponents            |= ODM_COMP_DYNAMIC_TXPWR;
    dbg->DebugComponents            |= ODM_COMP_FA_CNT;
    dbg->DebugComponents            |= ODM_COMP_RSSI_MONITOR;
    dbg->DebugComponents            |= ODM_COMP_CCK_PD;
    dbg->DebugComponents            |= ODM_COMP_ANT_DIV;
    dbg->DebugComponents            |= ODM_COMP_PWR_SAVE;
    dbg->DebugComponents            |= ODM_COMP_PWR_TRAIN;
    dbg->DebugComponents            |= ODM_COMP_RATE_ADAPTIVE;
    dbg->DebugComponents            |= ODM_COMP_PATH_DIV;
    dbg->DebugComponents            |= ODM_COMP_DYNAMIC_PRICCA;
    dbg->DebugComponents            |= ODM_COMP_RXHP ;
    dbg->DebugComponents            |= ODM_COMP_MP;
    dbg->DebugComponents            |= ODM_COMP_CFO_TRACKING;
    dbg->DebugComponents            |= ODM_COMP_ACS;
    dbg->DebugComponents            |= PHYDM_COMP_ADAPTIVITY;
    dbg->DebugComponents            |= PHYDM_COMP_RA_DBG;
    dbg->DebugComponents            |= PHYDM_COMP_TXBF;
//MAC Functions
    dbg->DebugComponents            |= ODM_COMP_EDCA_TURBO;
    dbg->DebugComponents            |= ODM_COMP_EARLY_MODE;
    dbg->DebugComponents            |= ODM_FW_DEBUG_TRACE;
//RF Functions
    dbg->DebugComponents            |= ODM_COMP_TX_PWR_TRACK;
    dbg->DebugComponents            |= ODM_COMP_RX_GAIN_TRACK;
    dbg->DebugComponents            |= ODM_COMP_CALIBRATION ;
//Common
    dbg->DebugComponents            |= ODM_PHY_CONFIG ;
    dbg->DebugComponents            |= ODM_COMP_COMMON ;
    dbg->DebugComponents            |= ODM_COMP_INIT ;
    dbg->DebugComponents            |= ODM_COMP_PSD;
    dbg->DebugComponents            |= ODM_COMP_NOISY_DETECT;
#endif

    dbg->fw_buff_is_enpty = wf_true;
    dbg->pre_c2h_seq = 0;

    return WF_RETURN_OK;
}

#endif

