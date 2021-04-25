#ifndef __ARS_ENTRY_H__
#define __ARS_ENTRY_H__

#ifdef CONFIG_ARS_SUPPORT
#if 1
#define ARS_DBG(fmt, ...)      LOG_D("ARS[%s,%d]"fmt, __func__, __LINE__,##__VA_ARGS__)
#else
#define ARS_DBG(fmt, ...)
#endif

#include "ars_acs.h"
#include "ars_noisemonitor.h"
#include "ars_pathdiv.h"
#include "ars_rainfo.h"
#include "ars_antdect.h"
#include "ars_antdiv.h"
#include "ars_bbps.h"
#include "ars_beamforming.h"
#include "ars_cfo.h"
#include "ars_edcaturbocheck.h"
#include "ars_txpower.h"
#include "ars_dig.h"
#include "ars_hwconfig.h"
#include "ars_timer.h"

typedef enum _BASEBAND_CONFIG_PHY_REG_PG_VALUE_TYPE
{
    PHY_REG_PG_RELATIVE_VALUE = 0,
    PHY_REG_PG_EXACT_VALUE = 1
} PHY_REG_PG_TYPE;


typedef struct _ODM_Phy_Dbg_Info_
{
    //ODM Write,debug info
    wf_s8       RxSNRdB[4];
    wf_u32      NumQryPhyStatus;
    wf_u32      NumQryPhyStatusCCK;
    wf_u32      NumQryPhyStatusOFDM;
    wf_u8       NumQryBeaconPkt;
    //Others
    wf_s32      RxEVM[4];   
    
}ODM_PHY_DBG_INFO_T;

typedef  struct adaptive_rate_system_st
{
    void *nic_info;
    acs_info_st acs;
    nm_info_st nm;
    pathdiv_info_st pathdiv;
    ra_info_st ra;
    txpower_info_st txpower;
    antdect_info_st antdect;
    antdiv_info_st antdiv;
    bbps_info_st bbps;
    beamform_info_st beamform;
    cfo_info_st cfo;
    dig_info_st dig;
    edcaturbo_info_st edcaturbo;
    hwconfig_info_st  hwconfig;
    timer_ctl_info_st timectl;


    // from here copy from onkey version
    //will rewrite after function is ok
    PHY_REG_PG_TYPE     PhyRegPgValueType;
    wf_u8               PhyRegPgVersion;


    wf_u64          DebugComponents;
    wf_u32          DebugLevel;

    wf_u32          NumQryPhyStatusAll;     //CCK + OFDM
    wf_u32          LastNumQryPhyStatusAll; 
    wf_u32          RxPWDBAve;
    wf_bool         MPDIG_2G;       //off MPDIG
    wf_u8           Times_2G;

    //------ ODM HANDLE, DRIVER NEEDS NOT TO HOOK------//
    wf_bool         bCckHighPower; 
    wf_u8           RFPathRxEnable;     // ODM_CMNINFO_RFPATH_ENABLE
    wf_u8           ControlChannel;

    ODM_PHY_DBG_INFO_T PhyDbgInfo;

    wf_u8           cck_lna_idx;
    wf_u8           cck_vga_idx;
    wf_u8           ofdm_agc_idx[4];
    wf_bool         bInHctTest;

    wf_u32          SupportAbility;
    SWAT_T          DM_SWAT_Table;
    
}ars_st;

wf_s32 ars_init(nic_info_st *pnic_info);
wf_s32 ars_term(nic_info_st *pnic_info);

#endif
#endif
