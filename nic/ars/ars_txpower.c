#include "common.h"
#include "wf_debug.h"
#ifdef CONFIG_ARS_SUPPORT


void odm_DynamicTxPowerNIC(void *ars)
{
    #if 0
	ars_st *pars = (ars_st*)ars;
	
	if (!(pars->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;
	
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if(pars->SupportICType == ODM_RTL8192C)	
	{
		odm_DynamicTxPower_92C(pars);
	}
	else if(pars->SupportICType == ODM_RTL8192D)
	{
		odm_DynamicTxPower_92D(pars);
	}
	else if (pars->SupportICType == ODM_RTL8821)
	{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
		PADAPTER		Adapter	 =  pars->Adapter;
		PMGNT_INFO		pMgntInfo = GetDefaultMgntInfo(Adapter);

		if (pMgntInfo->RegRspPwr == 1)
		{
			if(pars->RSSI_Min > 60)
			{
				ODM_SetMACReg(pars, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 1); // Resp TXAGC offset = -3dB

			}
			else if(pars->RSSI_Min < 55)
			{
				ODM_SetMACReg(pars, ODM_REG_RESP_TX_11AC, BIT20|BIT19|BIT18, 0); // Resp TXAGC offset = 0dB
			}
		}
#endif
	}
#endif	
#else
//do nothing
#endif
}

void odm_DynamicTxPower(void *ars)
{
    #if 0
	ars_st *pars = (ars_st*)ars;
	if (!(pars->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;

    dm_DynamicTxPowerNIC(pars);
    #endif

}



void odm_TXPowerTrackingCheckCE(void *ars)
{
    ars_st *pars = ars;
    if (!(pars->SupportAbility & ODM_RF_TX_PWR_TRACK))
        return;

    if(!pars->RFCalibrateInfo.TM_Trigger)        //at least delay 1 sec
    {
      
        hw_write_rf_reg(pars->nic_info, ODM_RF_PATH_A, RF_T_METER_NEW, (BIT(17) | BIT(16)), 0x03);
      
        
        //DBG_871X("Trigger Thermal Meter!!\n");
        
        pars->RFCalibrateInfo.TM_Trigger = 1;
        return;
    }
    else
    {
        //DBG_871X("Schedule TxPowerTracking direct call!!\n");
        pars->RFCalibrateInfo.TM_Trigger = 0;
    }
    
}


#endif

