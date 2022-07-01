/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include "mp_precomp.h"
#include "../../phydm_precomp.h"



/*---------------------------Define Local Constant---------------------------*/
/* 2010/04/25 MH Define the max tx power tracking tx agc power.*/
#define	ODM_TXPWRTRACK_MAX_IDX8723B	6

/* MACRO definition for cali_info->tx_iqc_8723b[0]*/
#define 	PATH_S0 						1 /* RF_PATH_B*/
#define	idx_0xc94						0
#define	idx_0xc80						1
#define	idx_0xc4c						2
#define	idx_0xc14						0
#define	idx_0xca0						1
#define	KEY							0
#define	VAL							1

/* MACRO definition for cali_info->tx_iqc_8723b[1]*/
#define 	PATH_S1 						0 /* RF_PATH_A*/
#define	idx_0xc9c						0
#define	idx_0xc88						1
#define	idx_0xc4c						2
#define	idx_0xc1c						0
#define	idx_0xc78						1


/*---------------------------Define Local Constant---------------------------*/


/*============================================================*/
/* Tx Power Tracking*/
/*============================================================*/

void halrf_rf_lna_setting_8723b(
	struct dm_struct	*dm,
	enum halrf_lna_set type
)
{
		/*phydm_disable_lna*/
		if (type == HALRF_LNA_DISABLE) {
			/*S0*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x1);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x30, 0xfffff, 0x18000);	/*select Rx mode*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x31, 0xfffff, 0x0001f);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x32, 0xfffff, 0xe6137);	/*disable LNA*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x0);
			/*S1*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x00020, 0x1);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, 0xfffff, 0x3008d);	/*select Rx mode and disable LNA*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x00020, 0x0);
		} else if (type == HALRF_LNA_ENABLE) {
			/*phydm_enable_lna*/
			/*S0*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x1);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x30, 0xfffff, 0x18000);	/*select Rx mode*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x31, 0xfffff, 0x0001f);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x32, 0xfffff, 0xe6177);	/*disable LNA*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, 0x80000, 0x0);
			/*S1*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x00020, 0x1);
			odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, 0xfffff, 0x300bd);	/*select Rx mode and disable LNA*/
			odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x00020, 0x0);
		}
}

void set_iqk_matrix_8723b(
	struct dm_struct	*dm,
	u8		OFDM_index,
	u8		rf_path,
	s32		iqk_result_x,
	s32		iqk_result_y
)
{
	s32			ele_A = 0, ele_D, ele_C = 0, value32;

	if (OFDM_index >= OFDM_TABLE_SIZE)
		OFDM_index = OFDM_TABLE_SIZE - 1;

	ele_D = (ofdm_swing_table_new[OFDM_index] & 0xFFC00000) >> 22;

	/*new element A = element D x X*/
	if ((iqk_result_x != 0) && (*(dm->band_type) == ODM_BAND_2_4G)) {
		if ((iqk_result_x & 0x00000200) != 0)	/*consider minus*/
			iqk_result_x = iqk_result_x | 0xFFFFFC00;
		ele_A = ((iqk_result_x * ele_D) >> 8) & 0x000003FF;

		/*new element C = element D x Y*/
		if ((iqk_result_y & 0x00000200) != 0)
			iqk_result_y = iqk_result_y | 0xFFFFFC00;
		ele_C = ((iqk_result_y * ele_D) >> 8) & 0x000003FF;

		/*if (rf_path == RF_PATH_A)*/
		switch (rf_path) {
		case RF_PATH_A:
			/*wirte new elements A, C, D to regC80 and regC94, element B is always 0*/
			value32 = (ele_D << 22) | ((ele_C & 0x3F) << 16) | ele_A;
			odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD, value32);

			value32 = (ele_C & 0x000003C0) >> 6;
			odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, MASKH4BITS, value32);

			value32 = ((iqk_result_x * ele_D) >> 7) & 0x01;
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(24), value32);
			break;
		case RF_PATH_B:
			/*wirte new elements A, C, D to regC88 and regC9C, element B is always 0*/
			value32 = (ele_D << 22) | ((ele_C & 0x3F) << 16) | ele_A;
			odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD, value32);

			value32 = (ele_C & 0x000003C0) >> 6;
			odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKH4BITS, value32);

			value32 = ((iqk_result_x * ele_D) >> 7) & 0x01;
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(28), value32);

			break;
		default:
			break;
		}
	} else {
		switch (rf_path)	{
		case RF_PATH_A:
			odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD, ofdm_swing_table_new[OFDM_index]);
			odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, MASKH4BITS, 0x00);
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(24), 0x00);
			break;

		case RF_PATH_B:
			odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD, ofdm_swing_table_new[OFDM_index]);
			odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKH4BITS, 0x00);
			odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(28), 0x00);
			break;

		default:
			break;
		}
	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "TxPwrTracking path B: X = 0x%x, Y = 0x%x ele_A = 0x%x ele_C = 0x%x ele_D = 0x%x 0xeb4 = 0x%x 0xebc = 0x%x\n",
		(u32)iqk_result_x, (u32)iqk_result_y, (u32)ele_A, (u32)ele_C, (u32)ele_D, (u32)iqk_result_x, (u32)iqk_result_y);
}


void
set_cck_filter_coefficient(
	struct dm_struct	*dm,
	u8		cck_swing_index
)
{
	if (*dm->channel != 14) {
		odm_write_1byte(dm, 0xa22, cck_swing_table_ch1_ch13_new[cck_swing_index][0]);
		odm_write_1byte(dm, 0xa23, cck_swing_table_ch1_ch13_new[cck_swing_index][1]);
		odm_write_1byte(dm, 0xa24, cck_swing_table_ch1_ch13_new[cck_swing_index][2]);
		odm_write_1byte(dm, 0xa25, cck_swing_table_ch1_ch13_new[cck_swing_index][3]);
		odm_write_1byte(dm, 0xa26, cck_swing_table_ch1_ch13_new[cck_swing_index][4]);
		odm_write_1byte(dm, 0xa27, cck_swing_table_ch1_ch13_new[cck_swing_index][5]);
		odm_write_1byte(dm, 0xa28, cck_swing_table_ch1_ch13_new[cck_swing_index][6]);
		odm_write_1byte(dm, 0xa29, cck_swing_table_ch1_ch13_new[cck_swing_index][7]);
	} else {
		odm_write_1byte(dm, 0xa22, cck_swing_table_ch14_new[cck_swing_index][0]);
		odm_write_1byte(dm, 0xa23, cck_swing_table_ch14_new[cck_swing_index][1]);
		odm_write_1byte(dm, 0xa24, cck_swing_table_ch14_new[cck_swing_index][2]);
		odm_write_1byte(dm, 0xa25, cck_swing_table_ch14_new[cck_swing_index][3]);
		odm_write_1byte(dm, 0xa26, cck_swing_table_ch14_new[cck_swing_index][4]);
		odm_write_1byte(dm, 0xa27, cck_swing_table_ch14_new[cck_swing_index][5]);
		odm_write_1byte(dm, 0xa28, cck_swing_table_ch14_new[cck_swing_index][6]);
		odm_write_1byte(dm, 0xa29, cck_swing_table_ch14_new[cck_swing_index][7]);
	}
}

void do_iqk_8723b(
	void		*dm_void,
	u8		delta_thermal_index,
	u8		thermal_value,
	u8		threshold
)
{
#if 0 /* mark by Lucas@SD4 20140128, suggested by Allen@SD3*/
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	void		*adapter = dm->adapter;
	HAL_DATA_TYPE	*hal_data = GET_HAL_DATA(adapter);
#endif

	odm_reset_iqk_result(dm);

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
#if USE_WORKITEM
	platform_acquire_mutex(&hal_data->mx_chnl_bw_control);
#else
	platform_acquire_spin_lock(adapter, RT_CHANNEL_AND_BANDWIDTH_SPINLOCK);
#endif
#elif ((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	platform_acquire_mutex(&hal_data->mx_chnl_bw_control);
#endif
#endif


	dm->rf_calibrate_info.thermal_value_iqk = thermal_value;
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	phy_iq_calibrate_8723b(dm, false, false);
#else
	phy_iq_calibrate_8723b(adapter, false, false);
#endif

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
#if USE_WORKITEM
	platform_release_mutex(&hal_data->mx_chnl_bw_control);
#else
	platform_release_spin_lock(adapter, RT_CHANNEL_AND_BANDWIDTH_SPINLOCK);
#endif
#elif ((DEV_BUS_TYPE == RT_USB_INTERFACE) || (DEV_BUS_TYPE == RT_SDIO_INTERFACE))
	platform_release_mutex(&hal_data->mx_chnl_bw_control);
#endif
#endif
#endif /* #if 0 */
}

/*-----------------------------------------------------------------------------
 * Function:	odm_TxPwrTrackSetPwr88E()
 *
 * Overview:	88E change all channel tx power accordign to flag.
 *				OFDM & CCK are all different.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who	Remark
 *	04/23/2012	MHC	Create version 0.
 *
 *---------------------------------------------------------------------------*/
void
odm_tx_pwr_track_set_pwr_8723b(
	void		*dm_void,
	enum pwrtrack_method	method,
	u8				rf_path,
	u8				channel_mapped_index
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	void	*adapter = dm->adapter;
	u8		pwr_tracking_limit_ofdm = 34; /*+0dB*/
	u8		pwr_tracking_limit_cck = 28;	/*-2dB*/
	u8		tx_rate = 0xFF;
	u8		final_ofdm_swing_index = 0;
	u8		final_cck_swing_index = 0;
	u8		i = 0;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);
	if (*(dm->mp_mode) == true) {
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
#if (MP_DRIVER == 1)
		PMPT_CONTEXT p_mpt_ctx = &(adapter->MptCtx);

		tx_rate = MptToMgntRate(p_mpt_ctx->MptRateIndex);
#endif
#elif (DM_ODM_SUPPORT_TYPE & (ODM_CE))
#ifdef CONFIG_MP_INCLUDED
		PMPT_CONTEXT	p_mpt_ctx = &(adapter->mppriv.mpt_ctx);

		tx_rate = mpt_to_mgnt_rate(p_mpt_ctx->mpt_rate_index);
#endif
#endif
#endif
	} else {
		u16	rate	 = *(dm->forced_data_rate);

		if (!rate) {	/*auto rate*/
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
			tx_rate = adapter->HalFunc.GetHwRateFromMRateHandler(dm->tx_rate);
#elif (DM_ODM_SUPPORT_TYPE & (ODM_CE))
			tx_rate = hw_rate_to_m_rate(dm->tx_rate);
#endif
		} else /*force rate*/
			tx_rate = (u8)rate;

	}

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "===>ODM_TxPwrTrackSetPwr8723B\n");

	if (tx_rate != 0xFF) {

#if (DM_ODM_SUPPORT_TYPE == (ODM_WIN|ODM_CE))
		/*2 CCK*/
		if ((tx_rate >= MGN_1M) && (tx_rate <= MGN_11M))
			pwr_tracking_limit_cck = 28;	/*-2dB*/
		/*2 OFDM*/
		else if ((tx_rate >= MGN_6M) && (tx_rate <= MGN_48M))
			pwr_tracking_limit_ofdm = 36; /*+3dB*/
		else if (tx_rate == MGN_54M)
			pwr_tracking_limit_ofdm = 34; /*+2dB*/

		/*2 HT*/
		else if ((tx_rate >= MGN_MCS0) && (tx_rate <= MGN_MCS2)) /*QPSK/BPSK*/
			pwr_tracking_limit_ofdm = 38; /*+4dB*/
		else if ((tx_rate >= MGN_MCS3) && (tx_rate <= MGN_MCS4)) /*16QAM*/
			pwr_tracking_limit_ofdm = 36; /*+3dB*/
		else if ((tx_rate >= MGN_MCS5) && (tx_rate <= MGN_MCS7)) /*64QAM*/
			pwr_tracking_limit_ofdm = 34; /*+2dB*/

		else
#endif
			pwr_tracking_limit_ofdm =  cali_info->default_ofdm_index;   /*Default OFDM index = 30*/
	}
	RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "tx_rate=0x%x, pwr_tracking_limit=%d\n", tx_rate, pwr_tracking_limit_ofdm);

	if (method == TXAGC) {
		u8	rf = 0;
		u32	pwr = 0, tx_agc = 0;
		void *adapter = dm->adapter;

		RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "odm_TxPwrTrackSetPwr8723B CH=%d\n", *(dm->channel));

		cali_info->remnant_ofdm_swing_idx[rf_path] = cali_info->absolute_ofdm_swing_idx[rf_path];

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
		if (*(dm->mp_mode) == true) {
			pwr = phy_query_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
			pwr += dm->rf_calibrate_info.power_index_offset[rf_path];
			phy_set_bb_reg(adapter, REG_TX_AGC_A_CCK_1_MCS32, MASKBYTE1, pwr);
			tx_agc = (pwr << 16) | (pwr << 8) | (pwr);
			phy_set_bb_reg(adapter, REG_TX_AGC_B_CCK_11_A_CCK_2_11, MASKH3BYTES, tx_agc);
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "ODM_TxPwrTrackSetPwr8723B: CCK Tx-rf(A) Power = 0x%x\n", tx_agc);

			pwr = phy_query_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, 0xFF);
			pwr += (cali_info->bb_swing_idx_ofdm[rf_path] - cali_info->bb_swing_idx_ofdm_base[rf_path]);
			tx_agc |= ((pwr << 24) | (pwr << 16) | (pwr << 8) | pwr);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_RATE18_06, MASKDWORD, tx_agc);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_RATE54_24, MASKDWORD, tx_agc);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS03_MCS00, MASKDWORD, tx_agc);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS07_MCS04, MASKDWORD, tx_agc);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS11_MCS08, MASKDWORD, tx_agc);
			phy_set_bb_reg(adapter, REG_TX_AGC_A_MCS15_MCS12, MASKDWORD, tx_agc);
			RF_DBG(dm, DBG_RF_TX_PWR_TRACK, "ODM_TxPwrTrackSetPwr8723B: OFDM Tx-rf(A) Power = 0x%x\n", tx_agc);
		} else {
			cali_info->modify_tx_agc_flag_path_a = true;
			cali_info->modify_tx_agc_flag_path_a_cck = true;

		}
#endif
	} else if (method == BBSWING) {
		final_ofdm_swing_index = cali_info->default_ofdm_index + cali_info->absolute_ofdm_swing_idx[rf_path];
		final_cck_swing_index = cali_info->default_cck_index + cali_info->absolute_ofdm_swing_idx[rf_path];

		/* Adjust BB swing by OFDM IQ matrix*/
		if (final_ofdm_swing_index >= pwr_tracking_limit_ofdm)
			final_ofdm_swing_index = pwr_tracking_limit_ofdm;
		else if (final_ofdm_swing_index <= 0)
			final_ofdm_swing_index = 0;

		if (final_cck_swing_index >= CCK_TABLE_SIZE)
			final_cck_swing_index = CCK_TABLE_SIZE - 1;
		else if (cali_info->bb_swing_idx_cck <= 0)
			final_cck_swing_index = 0;

		set_iqk_matrix_8723b(dm, final_ofdm_swing_index, rf_path,
			dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
			dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

		set_cck_filter_coefficient(dm, final_cck_swing_index);

	} else if (method == MIX_MODE) {
		RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"cali_info->default_ofdm_index=%d,  dm->DefaultCCKIndex=%d, cali_info->absolute_ofdm_swing_idx[rf_path]=%d, rf_path = %d\n",
			cali_info->default_ofdm_index, cali_info->default_cck_index, cali_info->absolute_ofdm_swing_idx[rf_path], rf_path);

		final_ofdm_swing_index = cali_info->default_ofdm_index + cali_info->absolute_ofdm_swing_idx[rf_path];
		final_cck_swing_index = cali_info->default_cck_index + cali_info->absolute_ofdm_swing_idx[rf_path];

		if (final_ofdm_swing_index > pwr_tracking_limit_ofdm) {
			/*BBSwing higher then Limit*/
			cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index - pwr_tracking_limit_ofdm;

			set_iqk_matrix_8723b(dm, pwr_tracking_limit_ofdm, rf_path,
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

			cali_info->modify_tx_agc_flag_path_a = true;

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A Over BBSwing Limit, pwr_tracking_limit = %d, Remnant tx_agc value = %d\n",
				pwr_tracking_limit_ofdm, cali_info->remnant_ofdm_swing_idx[rf_path]);
		} else if (final_ofdm_swing_index <= 0) {
			cali_info->remnant_ofdm_swing_idx[rf_path] = final_ofdm_swing_index;

			set_iqk_matrix_8723b(dm, 0, rf_path,
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

			cali_info->modify_tx_agc_flag_path_a = true;

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A Lower then BBSwing lower bound  0, Remnant tx_agc value = %d\n",
				cali_info->remnant_ofdm_swing_idx[rf_path]);
		} else {
			set_iqk_matrix_8723b(dm, final_ofdm_swing_index, rf_path,
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][0],
				dm->rf_calibrate_info.iqk_matrix_reg_setting[channel_mapped_index].value[0][1]);

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A Compensate with BBSwing, final_ofdm_swing_index = %d\n", final_ofdm_swing_index);

			if (cali_info->modify_tx_agc_flag_path_a) {
				/*If tx_agc has changed, reset tx_agc again*/
				cali_info->remnant_ofdm_swing_idx[rf_path] = 0;
				cali_info->modify_tx_agc_flag_path_a = false;

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A dm->Modify_TxAGC_Flag = false\n");
			}
		}

		if (final_cck_swing_index > pwr_tracking_limit_cck) {
			cali_info->remnant_cck_swing_idx = final_cck_swing_index - pwr_tracking_limit_cck;
			set_cck_filter_coefficient(dm, pwr_tracking_limit_cck);
			cali_info->modify_tx_agc_flag_path_a_cck = true;

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A CCK Over Limit, pwr_tracking_limit_cck = %d, cali_info->remnant_cck_swing_idx  = %d\n", pwr_tracking_limit_cck, cali_info->remnant_cck_swing_idx);
		} else if (final_cck_swing_index <= 0) {
			/* Lowest CCK index = 0*/
			cali_info->remnant_cck_swing_idx = final_cck_swing_index;
			set_cck_filter_coefficient(dm, 0);
			cali_info->modify_tx_agc_flag_path_a_cck = true;

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A CCK Under Limit, pwr_tracking_limit_cck = %d, cali_info->remnant_cck_swing_idx  = %d\n", 0, cali_info->remnant_cck_swing_idx);
		} else {
			set_cck_filter_coefficient(dm, final_cck_swing_index);

			RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A CCK Compensate with BBSwing, final_cck_swing_index = %d\n", final_cck_swing_index);

			if (cali_info->modify_tx_agc_flag_path_a_cck) {
				/*If tx_agc has changed, reset tx_agc again*/
				cali_info->remnant_cck_swing_idx = 0;
				cali_info->modify_tx_agc_flag_path_a_cck = false;

				RF_DBG(dm, DBG_RF_TX_PWR_TRACK,"******Path_A dm->Modify_TxAGC_Flag_CCK = false\n");
			}
		}
	}	else	{
		return; /* This method is not supported.*/
	}
}

void
get_delta_swing_table_8723b(
	void		*dm_void,
	u8 **temperature_up_a,
	u8 **temperature_down_a,
	u8 **temperature_up_b,
	u8 **temperature_down_b
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
#if (DM_ODM_SUPPORT_TYPE & ODM_AP)
	struct rtl8192cd_priv	*priv = dm->priv;
	u8			channel  = priv->pmib->dot11RFEntry.dot11channel;
#else
	void		*adapter		 = dm->adapter;
	HAL_DATA_TYPE	*hal_data		 = GET_HAL_DATA(adapter);
	u8			channel		 = *dm->channel;
#endif
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);
	u16			rate			 = *(dm->forced_data_rate);

	if (1 <= channel && channel <= 14) {
		if (IS_CCK_RATE(rate)) {
			*temperature_up_a   = cali_info->delta_swing_table_idx_2g_cck_a_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2g_cck_a_n;
			*temperature_up_b   = cali_info->delta_swing_table_idx_2g_cck_b_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2g_cck_b_n;
		} else {
			*temperature_up_a   = cali_info->delta_swing_table_idx_2ga_p;
			*temperature_down_a = cali_info->delta_swing_table_idx_2ga_n;
			*temperature_up_b   = cali_info->delta_swing_table_idx_2gb_p;
			*temperature_down_b = cali_info->delta_swing_table_idx_2gb_n;
		}
	}
	/*else if ( 36 <= channel && channel <= 64) {
		*temperature_up_a   = cali_info->delta_swing_table_idx_5ga_p[0];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[0];
		*temperature_up_b   = cali_info->delta_swing_table_idx_5gb_p[0];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[0];
	} else if ( 100 <= channel && channel <= 140) {
		*temperature_up_a   = cali_info->delta_swing_table_idx_5ga_p[1];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[1];
		*temperature_up_b   = cali_info->delta_swing_table_idx_5gb_p[1];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[1];
	} else if ( 149 <= channel && channel <= 173) {
		*temperature_up_a   = cali_info->delta_swing_table_idx_5ga_p[2];
		*temperature_down_a = cali_info->delta_swing_table_idx_5ga_n[2];
		*temperature_up_b   = cali_info->delta_swing_table_idx_5gb_p[2];
		*temperature_down_b = cali_info->delta_swing_table_idx_5gb_n[2];
	}*/else {
		*temperature_up_a   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_a = (u8 *)delta_swing_table_idx_2ga_n_8188e;
		*temperature_up_b   = (u8 *)delta_swing_table_idx_2ga_p_8188e;
		*temperature_down_b = (u8 *)delta_swing_table_idx_2ga_n_8188e;
	}

}


void configure_txpower_track_8723b(
	struct txpwrtrack_cfg *config
)
{
	config->swing_table_size_cck = CCK_TABLE_SIZE;
	config->swing_table_size_ofdm = OFDM_TABLE_SIZE;
	config->threshold_iqk = IQK_THRESHOLD;
	config->average_thermal_num = AVG_THERMAL_NUM_8723B;
	config->rf_path_count = MAX_PATH_NUM_8723B;
	config->thermal_reg_addr = RF_T_METER_8723B;

	config->odm_tx_pwr_track_set_pwr = odm_tx_pwr_track_set_pwr_8723b;
	config->do_iqk = do_iqk_8723b;
	config->phy_lc_calibrate = halrf_lck_trigger;
	config->get_delta_swing_table = get_delta_swing_table_8723b;
}

/*1 7.	IQK*/
#define MAX_TOLERANCE		5
#define IQK_DELAY_TIME		1

u8			/*bit0 = 1 => Tx OK, bit1 = 1 => Rx OK*/
phy_path_a_iqk_8723b(
	struct dm_struct		*dm,
	boolean		config_path_b,
	u8		rf_path
)
{
	u32 reg_eac, reg_e94, reg_e9c, tmp, path_sel_bb /*, reg_ea4*/;
	u8 result = 0x00;

	/* Save RF path */
	path_sel_bb = odm_get_bb_reg(dm, R_0x948, MASKDWORD);

	RF_DBG(dm, DBG_RF_IQK, "path A IQK!\n");

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/*	enable path A PA in TXIQK mode*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x20000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0003f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xc7f87);
	/*	disable path B PA in TXIQK mode*/
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, RFREGOFFSETMASK, 0x00020 );
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x40ec1 );*/

	/*1 Tx IQK*/
	/*IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);
	/*path-A IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x8214010a);*/
	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x821403ea);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x28110000);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x00462911);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*ant switch*/
	if (config_path_b || (rf_path == 0))
		/* wifi switch to S1*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000000);
	else
		/* wifi switch to S0*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*GNT_BT = 0*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path A LOK & IQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);


	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));


	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;

	return result;

}

u8			/*bit0 = 1 => Tx OK, bit1 = 1 => Rx OK*/
phy_path_a_rx_iqk_8723b(
	struct dm_struct		*dm,
	boolean		config_path_b,
	u8		rf_path
)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4, u4tmp, tmp, path_sel_bb;
	u8 result = 0x00;

	/* Save RF path */
	path_sel_bb = odm_get_bb_reg(dm, R_0x948, MASKDWORD);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	RF_DBG(dm, DBG_RF_IQK, "path A RX IQK:Get TXIMR setting\n");
	/*1 Get TXIMR setting*/
	/*modify RXIQK mode table*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	/*LNA2 off, PA on for Dcut*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7fb7);
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x0);*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/*path-A IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160c1f);*/
	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160ff0);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x28110000);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*ant switch*/
	if (config_path_b || (rf_path == 0))
		/* wifi switch to S1*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000000);
	else
		/* wifi switch to S0*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*GNT_BT = 0*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path A LOK & IQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))

		result |= 0x01;
	else							/*if Tx not OK, ignore Rx*/
		return result;


	u4tmp = 0x80007C00 | (reg_e94 & 0x3FF0000)  | ((reg_e9c & 0x3FF0000) >> 16);
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, u4tmp);
	RF_DBG(dm, DBG_RF_IQK, "0xe40 = 0x%x u4tmp = 0x%x\n", odm_get_bb_reg(dm, REG_TX_IQK, MASKDWORD), u4tmp);


	/*1 RX IQK*/
	RF_DBG(dm, DBG_RF_IQK, "path A RX IQK\n");

	/*modify RXIQK mode table*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	/*LAN2 on, PA off for Dcut*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7d77);
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x0);	*/

	/*PA, PAD setting*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0xf80);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x55, RFREGOFFSETMASK, 0x4021f);


	/*IQK setting*/
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/*path-A IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82110000);
	/*	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x281604c2);*/
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x2816001f);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a8d1);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*ant switch*/
	if (config_path_b || (rf_path == 0))
		/* wifi switch to S1*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000000);
	else
		/* wifi switch to S0*/
		odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*GNT_BT = 0*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path A LOK & IQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_ea4 = odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xea4 = 0x%x, 0xeac = 0x%x\n", reg_ea4, reg_eac);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xea0(before IQK)= 0x%x, 0xea8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xea0, MASKDWORD), odm_get_bb_reg(dm, R_0xea8, MASKDWORD));

	/*	PA/PAD controlled by 0x0*/
	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0x780);

	/* Allen 20141201 */
	tmp = (reg_eac & 0x03FF0000) >> 16;
	if ((tmp & 0x200) > 0)
		tmp = 0x400 - tmp;

	/*if Tx is OK, check whether Rx is OK*/
	if (!(reg_eac & BIT(27)) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) < 0x11a) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) > 0xe6) &&
	    (tmp < 0x1a))
		result |= 0x02;
	else							/*if Tx not OK, ignore Rx*/
		RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK fail!!\n");


	return result;


}

u8				/*bit0 = 1 => Tx OK, bit1 = 1 => Rx OK*/
phy_path_b_iqk_8723b(
	struct dm_struct		*dm
)
{
	u32 reg_eac, reg_e94, reg_e9c, tmp, path_sel_bb/*, reg_ec4, reg_ecc, path_sel_bb*/;
	u8	result = 0x00;

	RF_DBG(dm, DBG_RF_IQK, "path B IQK!\n");

	/* Save RF path*/
	path_sel_bb = odm_get_bb_reg(dm, R_0x948, MASKDWORD);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/*	in TXIQK mode*/
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1 );
		odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x20000 );
		odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0003f );
		odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xc7f87 );*/
	/*	enable path B PA in TXIQK mode*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x20, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x40fc1);



	/*1 Tx IQK*/
	/*IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);
	/*path-A IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82140114);*/
	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x821403ea);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x28110000);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x00462911);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*switch to path B*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);
#if 1
	/* odm_set_rf_reg(dm, RF_PATH_A, RF_0xb0, RFREGOFFSETMASK, 0xeffe0); */

	/*GNT_BT = 0
	#endif
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path B LOK & IQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;

	return result;

}



u8			/*bit0 = 1 => Tx OK, bit1 = 1 => Rx OK*/
phy_path_b_rx_iqk_8723b(
	struct dm_struct		*dm,
	boolean		config_path_b
)
{
	u32 reg_e94, reg_e9c, reg_ea4, reg_eac, u4tmp, tmp, path_sel_bb;
	u8 result = 0x00;

	/* Save RF path*/
	path_sel_bb = odm_get_bb_reg(dm, R_0x948, MASKDWORD);
	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/*switch to path B*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*1 Get TXIMR setting*/
	RF_DBG(dm, DBG_RF_IQK, "path B RX IQK:Get TXIMR setting!\n");
	/*modify RXIQK mode table*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7fb7);
	/*open PA S1 & SMIXER*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x20, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x60fcd);


	/*IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, 0x01007c00);
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);


	/*path-B IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	/*	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160c1f );*/
	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82160ff0);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x28110000);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a911);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*switch to path B*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*GNT_BT = 0*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path B TXIQK @ RXIQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);


	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_e94 = odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD);
	reg_e9c = odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD);
	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xe94 = 0x%x, 0xe9c = 0x%x\n", reg_e94, reg_e9c);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xe90(before IQK)= 0x%x, 0xe98(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xe90, MASKDWORD), odm_get_bb_reg(dm, R_0xe98, MASKDWORD));


	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))

		result |= 0x01;
	else							/*if Tx not OK, ignore Rx*/
		return result;



	u4tmp = 0x80007C00 | (reg_e94 & 0x3FF0000)  | ((reg_e9c & 0x3FF0000) >> 16);
	odm_set_bb_reg(dm, REG_TX_IQK, MASKDWORD, u4tmp);
	RF_DBG(dm, DBG_RF_IQK, "0xe40 = 0x%x u4tmp = 0x%x\n", odm_get_bb_reg(dm, REG_TX_IQK, MASKDWORD), u4tmp);


	/*1 RX IQK*/
	RF_DBG(dm, DBG_RF_IQK, "path B RX IQK\n");
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7d77);
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x0);	 */

	/*open PA S1 & close SMIXER*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x20, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x60ebd);

	/*PA, PAD setting*/
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_0xdf, RFREGOFFSETMASK, 0xf80);*/
	/*	odm_set_rf_reg(dm, RF_PATH_A, RF_0x56, RFREGOFFSETMASK, 0x51000); */



	/*IQK setting*/
	odm_set_bb_reg(dm, REG_RX_IQK, MASKDWORD, 0x01004800);

	/*path-B IQK setting*/
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x18008c1c);
	odm_set_bb_reg(dm, REG_TX_IQK_TONE_B, MASKDWORD, 0x38008c1c);
	odm_set_bb_reg(dm, REG_RX_IQK_TONE_B, MASKDWORD, 0x38008c1c);

	odm_set_bb_reg(dm, REG_TX_IQK_PI_A, MASKDWORD, 0x82110000);
	/*	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x281604c2);*/
	odm_set_bb_reg(dm, REG_RX_IQK_PI_A, MASKDWORD, 0x2816001f);
	odm_set_bb_reg(dm, REG_TX_IQK_PI_B, MASKDWORD, 0x82110000);
	odm_set_bb_reg(dm, REG_RX_IQK_PI_B, MASKDWORD, 0x28110000);

	/*LO calibration setting*/
	odm_set_bb_reg(dm, REG_IQK_AGC_RSP, MASKDWORD, 0x0046a8d1);

	/*enter IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);

	/*switch to path B*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, 0x00000280);

	/*GNT_BT = 0*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00000800);

	/*One shot, path B LOK & IQK*/
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf9000000);
	odm_set_bb_reg(dm, REG_IQK_AGC_PTS, MASKDWORD, 0xf8000000);

	ODM_delay_ms(IQK_DELAY_TIME_8723B);

	/*restore ant path*/
	odm_set_bb_reg(dm, R_0x948, MASKDWORD, path_sel_bb);
	/*GNT_BT = 1*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, 0x00001800);

	/*leave IQK mode*/
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);

	/* Check failed*/
	reg_eac = odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD);
	reg_ea4 = odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD);

	RF_DBG(dm, DBG_RF_IQK, "0xeac = 0x%x\n", reg_eac);
	RF_DBG(dm, DBG_RF_IQK, "0xea4 = 0x%x, 0xeac = 0x%x\n", reg_ea4, reg_eac);
	/*monitor image power before & after IQK*/
	RF_DBG(dm, DBG_RF_IQK, "0xea0(before IQK)= 0x%x, 0xea8(afer IQK) = 0x%x\n",
		odm_get_bb_reg(dm, R_0xea0, MASKDWORD), odm_get_bb_reg(dm, R_0xea8, MASKDWORD));

	/* Allen 20141201 */
	tmp = (reg_eac & 0x03FF0000) >> 16;
	if ((tmp & 0x200) > 0)
		tmp = 0x400 - tmp;

	if (!(reg_eac & BIT(27)) &&		/*if Tx is OK, check whether Rx is OK*/
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) < 0x11a) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) > 0xe6) &&
	    (tmp < 0x1a))

		result |= 0x02;
	else
		RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK fail!!\n");


	return result;


}


void
_phy_path_a_fill_iqk_matrix8723b(
	struct dm_struct		*dm,
	boolean		is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only
)
{
	u32	oldval_0, X, TX0_A, reg;
	s32	Y, TX0_C;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);

	RF_DBG(dm, DBG_RF_IQK, "path A IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed");

	if (final_candidate == 0xFF)
		return;
	else if (is_iqk_ok) {
		oldval_0 = (odm_get_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][0];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;
		TX0_A = (X * oldval_0) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "X = 0x%x, TX0_A = 0x%x, oldval_0 0x%x\n", X, TX0_A, oldval_0);
		odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x3FF, TX0_A);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(31), ((X * oldval_0 >> 7) & 0x1));

		Y = result[final_candidate][1];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;

		/*2 Tx IQC*/
		TX0_C = (Y * oldval_0) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "Y = 0x%x, TX = 0x%x\n", Y, TX0_C);
		odm_set_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, 0xF0000000, ((TX0_C & 0x3C0) >> 6));
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc94][KEY] = REG_OFDM_0_XC_TX_AFE;
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc94][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XC_TX_AFE, MASKDWORD);

		odm_set_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, 0x003F0000, (TX0_C & 0x3F));
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc80][KEY] = REG_OFDM_0_XA_TX_IQ_IMBALANCE;
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc80][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XA_TX_IQ_IMBALANCE, MASKDWORD);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(29), ((Y * oldval_0 >> 7) & 0x1));
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc4c][KEY] = REG_OFDM_0_ECCA_THRESHOLD;
		cali_info->tx_iqc_8723b[PATH_S1][idx_0xc4c][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, MASKDWORD);

		if (is_tx_only) {
			RF_DBG(dm, DBG_RF_IQK, "_phy_path_a_fill_iqk_matrix8723b only Tx OK\n");

			/* <20130226, Kordan> Saving RxIQC, otherwise not initialized.*/
			cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][KEY] = REG_OFDM_0_RX_IQ_EXT_ANTA;
			cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][VAL] = 0xfffffff & odm_get_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, MASKDWORD);
			cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][KEY] = REG_OFDM_0_XA_RX_IQ_IMBALANCE;
			/*			cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, MASKDWORD);*/
			cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][VAL] = 0x40000100;
			return;
		}

		reg = result[final_candidate][2];
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		if (RTL_ABS(reg, 0x100) >= 16)
			reg = 0x100;
#endif

		/*2 Rx IQC*/
		odm_set_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0x3FF, reg);
		reg = result[final_candidate][3] & 0x3F;
		odm_set_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, 0xFC00, reg);
		cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][KEY] = REG_OFDM_0_XA_RX_IQ_IMBALANCE;
		cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, MASKDWORD);

		reg = (result[final_candidate][3] >> 6) & 0xF;
		odm_set_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, 0xF0000000, reg);
		cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][KEY] = REG_OFDM_0_RX_IQ_EXT_ANTA;
		cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, MASKDWORD);

	}
}

void
_phy_path_b_fill_iqk_matrix8723b(
	struct dm_struct		*dm,
	boolean		is_iqk_ok,
	s32		result[][8],
	u8		final_candidate,
	boolean		is_tx_only
)
{
	u32	oldval_1, X, TX1_A, reg;
	s32	Y, TX1_C;
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);

	RF_DBG(dm, DBG_RF_IQK, "path B IQ Calibration %s !\n", (is_iqk_ok) ? "Success" : "Failed");

	if (final_candidate == 0xFF)
		return;
	else if (is_iqk_ok) {
		oldval_1 = (odm_get_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD) >> 22) & 0x3FF;

		X = result[final_candidate][4];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;

		TX1_A = (X * oldval_1) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "X = 0x%x, TX1_A = 0x%x\n", X, TX1_A);

		odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x3FF, TX1_A);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(27), ((X * oldval_1 >> 7) & 0x1));

		Y = result[final_candidate][5];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;

		TX1_C = (Y * oldval_1) >> 8;
		RF_DBG(dm, DBG_RF_IQK, "Y = 0x%x, TX1_C = 0x%x\n", Y, TX1_C);

		/*2 Tx IQC*/
		odm_set_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, 0xF0000000, ((TX1_C & 0x3C0) >> 6));
		/*		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc9c][KEY] = REG_OFDM_0_XD_TX_AFE;*/
		/*		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc9c][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKDWORD);*/
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc94][KEY] = REG_OFDM_0_XC_TX_AFE;
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc94][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XD_TX_AFE, MASKDWORD);

		odm_set_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, 0x003F0000, (TX1_C & 0x3F));
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc80][KEY] = REG_OFDM_0_XA_TX_IQ_IMBALANCE;
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc80][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XB_TX_IQ_IMBALANCE, MASKDWORD);

		odm_set_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, BIT(25), ((Y * oldval_1 >> 7) & 0x1));
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc4c][KEY] = REG_OFDM_0_ECCA_THRESHOLD;
		cali_info->tx_iqc_8723b[PATH_S0][idx_0xc4c][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_ECCA_THRESHOLD, MASKDWORD);

		if (is_tx_only) {
			RF_DBG(dm, DBG_RF_IQK, "_phy_path_b_fill_iqk_matrix8723b only Tx OK\n");

			cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][KEY] = REG_OFDM_0_XA_RX_IQ_IMBALANCE;
			/*			cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XA_RX_IQ_IMBALANCE, MASKDWORD); */
			cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][VAL] = 0x40000100;
			cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][KEY] = REG_OFDM_0_RX_IQ_EXT_ANTA;
			cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][VAL] = 0x0fffffff & odm_get_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, MASKDWORD);
			return;
		}

		/*2 Rx IQC*/
		reg = result[final_candidate][6];
		odm_set_bb_reg(dm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0x3FF, reg);
		reg = result[final_candidate][7] & 0x3F;
		odm_set_bb_reg(dm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, 0xFC00, reg);
		cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][KEY] = REG_OFDM_0_XA_RX_IQ_IMBALANCE;
		cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][VAL] = odm_get_bb_reg(dm, REG_OFDM_0_XB_RX_IQ_IMBALANCE, MASKDWORD);

		reg = (result[final_candidate][7] >> 6) & 0xF;
		/*		odm_set_bb_reg(dm, REG_OFDM_0_AGC_RSSI_TABLE, 0x0000F000, reg); */
		cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][KEY] = REG_OFDM_0_RX_IQ_EXT_ANTA;
		cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][VAL] = (reg << 28) | (odm_get_bb_reg(dm, REG_OFDM_0_RX_IQ_EXT_ANTA, MASKDWORD) & 0x0fffffff);
	}
}

void
odm_set_iqc_by_rfpath(
	struct dm_struct		*dm,
	u32	rf_path
)
{
	struct dm_rf_calibration_struct	*cali_info = &(dm->rf_calibrate_info);

	if ((cali_info->tx_iqc_8723b[PATH_S0][idx_0xc80][VAL] != 0x0) &&
	    (cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][VAL] != 0x0) &&
	    (cali_info->tx_iqc_8723b[PATH_S1][idx_0xc80][VAL] != 0x0) &&
	    (cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][VAL] != 0x0)) {

		if (rf_path) {

			/*S1: rf_path = 0, S0:rf_path = 1*/
			/*S0 TX IQC*/
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc94][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc94][VAL]);
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc80][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc80][VAL]);
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc4c][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S0][idx_0xc4c][VAL]);
			/*S0 RX IQC*/
			odm_set_bb_reg(dm, cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][KEY], MASKDWORD, cali_info->rx_iqc_8723b[PATH_S0][idx_0xc14][VAL]);
			odm_set_bb_reg(dm, cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][KEY], MASKDWORD, cali_info->rx_iqc_8723b[PATH_S0][idx_0xca0][VAL]);
		} else {
			/*S1 TX IQC*/
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc94][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc94][VAL]);
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc80][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc80][VAL]);
			odm_set_bb_reg(dm, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc4c][KEY], MASKDWORD, cali_info->tx_iqc_8723b[PATH_S1][idx_0xc4c][VAL]);
			/*S1 RX IQC*/
			odm_set_bb_reg(dm, cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][KEY], MASKDWORD, cali_info->rx_iqc_8723b[PATH_S1][idx_0xc14][VAL]);
			odm_set_bb_reg(dm, cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][KEY], MASKDWORD, cali_info->rx_iqc_8723b[PATH_S1][idx_0xca0][VAL]);
		}
	}
}


void
_phy_save_adda_registers8723b(
	struct dm_struct		*dm,
	u32		*adda_reg,
	u32		*adda_backup,
	u32		register_num
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Save ADDA parameters.\n");
	for (i = 0; i < register_num; i++)
		adda_backup[i] = odm_get_bb_reg(dm, adda_reg[i], MASKDWORD);

}


void
_phy_save_mac_registers8723b(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Save MAC parameters.\n");
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		mac_backup[i] = odm_read_1byte(dm, mac_reg[i]);
	mac_backup[i] = odm_read_4byte(dm, mac_reg[i]);

}


void
_phy_reload_adda_registers8723b(
	struct dm_struct		*dm,
	u32		*adda_reg,
	u32		*adda_backup,
	u32		regiester_num
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Reload ADDA power saving parameters !\n");
	for (i = 0 ; i < regiester_num; i++)
		odm_set_bb_reg(dm, adda_reg[i], MASKDWORD, adda_backup[i]);

}


void
_phy_reload_mac_registers8723b(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "Reload MAC parameters !\n");
	for (i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++)
		odm_write_1byte(dm, mac_reg[i], (u8)mac_backup[i]);

	odm_write_4byte(dm, mac_reg[i], mac_backup[i]);
}


void
_phy_path_adda_on8723b(
	struct dm_struct		*dm,
	u32		*adda_reg,
	boolean		is_path_a_on,
	boolean		is2T
)
{
	u32	path_on;
	u32	i;

	RF_DBG(dm, DBG_RF_IQK, "ADDA ON.\n");

	path_on = is_path_a_on ? 0x01c00014 : 0x01c00014;
	if (false == is2T) {
		path_on = 0x01c00014;
		odm_set_bb_reg(dm, adda_reg[0], MASKDWORD, 0x01c00014);
	} else
		odm_set_bb_reg(dm, adda_reg[0], MASKDWORD, path_on);

	for (i = 1; i < IQK_ADDA_REG_NUM; i++)
		odm_set_bb_reg(dm, adda_reg[i], MASKDWORD, path_on);
}


void
_phy_mac_setting_calibration8723b(
	struct dm_struct		*dm,
	u32		*mac_reg,
	u32		*mac_backup
)
{
	u32	i = 0;

	RF_DBG(dm, DBG_RF_IQK, "MAC settings for Calibration.\n");
	odm_write_1byte(dm, mac_reg[i], 0x3F);
	for (i = 1 ; i < (IQK_MAC_REG_NUM - 1); i++)
		odm_write_1byte(dm, mac_reg[i], (u8)(mac_backup[i] & (~BIT(3))));
	odm_write_1byte(dm, mac_reg[i], (u8)(mac_backup[i] & (~BIT(5))));
}


void
_phy_path_a_stand_by8723b(
	struct dm_struct		*dm
)
{
	RF_DBG(dm, DBG_RF_IQK, "path-A standby mode!\n");
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x0);
	odm_set_rf_reg(dm, RF_PATH_A, RF_AC, MASKDWORD, 0x10000);
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x808000);
}

void
_phy_pi_mode_switch8723b(
	struct dm_struct		*dm,
	boolean		pi_mode
)
{
	u32	mode;

	RF_DBG(dm, DBG_RF_IQK, "BB Switch to %s mode!\n", (pi_mode ? "PI" : "SI"));

	mode = pi_mode ? 0x01000100 : 0x01000000;
	odm_set_bb_reg(dm, REG_FPGA0_XA_HSSI_PARAMETER1, MASKDWORD, mode);
	odm_set_bb_reg(dm, REG_FPGA0_XB_HSSI_PARAMETER1, MASKDWORD, mode);
}

boolean
phy_simularity_compare_8723b(
	struct dm_struct		*dm,
	s32		result[][8],
	u8		 c1,
	u8		 c2
)
{
	u32		i, j, diff, simularity_bit_map, bound = 0;
	u8		final_candidate[2] = {0xFF, 0xFF};
	boolean		is_result = true;
	boolean		is2T = true;

	s32 tmp1 = 0, tmp2 = 0;

	if (is2T)
		bound = 8;
	else
		bound = 4;

	RF_DBG(dm, DBG_RF_IQK, "===> IQK:phy_simularity_compare_8192e c1 %d c2 %d!!!\n", c1, c2);


	simularity_bit_map = 0;

	for (i = 0; i < bound; i++) {

		if ((i == 1) || (i == 3) || (i == 5) || (i == 7)) {
			if ((result[c1][i] & 0x00000200) != 0)
				tmp1 = result[c1][i] | 0xFFFFFC00;
			else
				tmp1 = result[c1][i];

			if ((result[c2][i] & 0x00000200) != 0)
				tmp2 = result[c2][i] | 0xFFFFFC00;
			else
				tmp2 = result[c2][i];
		} else {
			tmp1 = result[c1][i];
			tmp2 = result[c2][i];
		}

		diff = (tmp1 > tmp2) ? (tmp1 - tmp2) : (tmp2 - tmp1);

		if (diff > MAX_TOLERANCE) {
			RF_DBG(dm, DBG_RF_IQK, "IQK:differnece overflow %d index %d compare1 0x%x compare2 0x%x!!!\n",	diff, i, result[c1][i], result[c2][i]);

			if ((i == 2 || i == 6) && !simularity_bit_map) {
				if (result[c1][i] + result[c1][i + 1] == 0)
					final_candidate[(i / 4)] = c2;
				else if (result[c2][i] + result[c2][i + 1] == 0)
					final_candidate[(i / 4)] = c1;
				else
					simularity_bit_map = simularity_bit_map | (1 << i);
			} else
				simularity_bit_map = simularity_bit_map | (1 << i);
		}
	}

	RF_DBG(dm, DBG_RF_IQK, "IQK:phy_simularity_compare_8192e simularity_bit_map   %x !!!\n", simularity_bit_map);

	if (simularity_bit_map == 0) {
		for (i = 0; i < (bound / 4); i++) {
			if (final_candidate[i] != 0xFF) {
				for (j = i * 4; j < (i + 1) * 4 - 2; j++)
					result[3][j] = result[final_candidate[i]][j];
				is_result = false;
			}
		}
		return is_result;
	} else {

		if (!(simularity_bit_map & 0x03)) {
			/*path A TX OK*/
			for (i = 0; i < 2; i++)
				result[3][i] = result[c1][i];
		}

		if (!(simularity_bit_map & 0x0c)) {
			/*path A RX OK*/
			for (i = 2; i < 4; i++)
				result[3][i] = result[c1][i];
		}

		if (!(simularity_bit_map & 0x30)) {
			/*path B TX OK*/
			for (i = 4; i < 6; i++)
				result[3][i] = result[c1][i];
		}

		if (!(simularity_bit_map & 0xc0)) {
			/*path B RX OK*/
			for (i = 6; i < 8; i++)
				result[3][i] = result[c1][i];
		}
		return false;
	}
}

void
_phy_check_coex_status_8723b(
	struct dm_struct	*dm,
	boolean		beforek
)
{
	u8		u1b_tmp;
	u16		count = 0;
	u8		h2c_parameter;

#if MP_DRIVER != 1
	if (beforek) {
		/* Set H2C cmd to inform FW (enable). */
		h2c_parameter = 1;
		odm_fill_h2c_cmd(dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);
		/* Check 0x1e6 or 100ms timeout*/
		count = 0;
		u1b_tmp = odm_read_1byte(dm, 0x1e6);
		while (u1b_tmp != 0x1 && count < 5000) {
			ODM_delay_us(20);
			u1b_tmp = odm_read_1byte(dm, 0x1e6);
			count++;
		}

		if (count >= 5000)
			RF_DBG(dm, DBG_RF_INIT, "[IQK]Polling 0x1e6 to 1 for WiFi calibration H2C cmd FAIL! count(%d)", count);

		/* Wait BT IQK finished. */
		/* polling 0x1e7[0]=1 or 600ms timeout */
		count = 0;
		u1b_tmp = odm_read_1byte(dm, 0x1e7);
		while ((!(u1b_tmp & BIT(0))) && count < 30000) {
			ODM_delay_us(20);
			u1b_tmp = odm_read_1byte(dm, 0x1e7);
			count++;
		}

		if (count >= 30000)
			RF_DBG(dm, DBG_RF_INIT, "[IQK]Waiting BT IQK finish time out! count(%d)", count);

	} else {
		/* Set H2C cmd to inform FW (disable). */
		h2c_parameter = 0;
		odm_fill_h2c_cmd(dm, ODM_H2C_WIFI_CALIBRATION, 1, &h2c_parameter);
		/* Check 0x1e6 or 100ms timeout*/
		count = 0;
		u1b_tmp = odm_read_1byte(dm, 0x1e6);
		while (u1b_tmp != 0 && count < 5000) {
			ODM_delay_us(20);
			u1b_tmp = odm_read_1byte(dm, 0x1e6);
			count++;
		}

		if (count >= 5000)
			RF_DBG(dm, DBG_RF_INIT, "[IQK]Polling 0x1e6 to 0 for WiFi calibration H2C cmd FAIL! count(%d)", count);
		}
#endif
}

void
_phy_iq_calibrate_8723b(
	struct dm_struct		*dm,
	s32		result[][8],
	u8		t,
	boolean		is2T,
	u8		rf_path
)
{
	u32			i;
	u8			path_aok, path_bok;
	u8			tmp0xc50 = (u8)odm_get_bb_reg(dm, R_0xc50, MASKBYTE0);
	u8			tmp0xc58 = (u8)odm_get_bb_reg(dm, R_0xc58, MASKBYTE0);
	u32			ADDA_REG[IQK_ADDA_REG_NUM] = {
		REG_FPGA0_XCD_SWITCH_CONTROL,	REG_BLUE_TOOTH,
		REG_RX_WAIT_CCA,		REG_TX_CCK_RFON,
		REG_TX_CCK_BBON,	REG_TX_OFDM_RFON,
		REG_TX_OFDM_BBON,	REG_TX_TO_RX,
		REG_TX_TO_TX,		REG_RX_CCK,
		REG_RX_OFDM,		REG_RX_WAIT_RIFS,
		REG_RX_TO_RX,		REG_STANDBY,
		REG_SLEEP,			REG_PMPD_ANAEN
	};
	u32			IQK_MAC_REG[IQK_MAC_REG_NUM] = {
		REG_TXPAUSE,		REG_BCN_CTRL,
		REG_BCN_CTRL_1, REG_GPIO_MUXCFG
	};

	u32	IQK_BB_REG_92C[IQK_BB_REG_NUM] = {
		REG_OFDM_0_TRX_PATH_ENABLE,		REG_OFDM_0_TR_MUX_PAR,
		REG_FPGA0_XCD_RF_INTERFACE_SW,	REG_CONFIG_ANT_A,	REG_CONFIG_ANT_B,
		REG_FPGA0_XAB_RF_INTERFACE_SW,	REG_FPGA0_XA_RF_INTERFACE_OE,
		REG_FPGA0_XB_RF_INTERFACE_OE, REG_CCK_0_AFE_SETTING
	};

	u32 path_sel_bb;
	u32	retry_count = 2;

	/* Note: IQ calibration must be performed after loading
	PHY_REG.txt , and radio_a, radio_b.txt */
#ifdef MP_TEST
	if (*(dm->mp_mode))
		retry_count = 9;
#endif


	if (t == 0) {
		RF_DBG(dm, DBG_RF_IQK, "IQ Calibration for %s for %d times\n", (is2T ? "2T2R" : "1T1R"), t);
		_phy_save_adda_registers8723b(dm, ADDA_REG, dm->rf_calibrate_info.ADDA_backup, IQK_ADDA_REG_NUM);
		_phy_save_mac_registers8723b(dm, IQK_MAC_REG, dm->rf_calibrate_info.IQK_MAC_backup);
		_phy_save_adda_registers8723b(dm, IQK_BB_REG_92C, dm->rf_calibrate_info.IQK_BB_backup, IQK_BB_REG_NUM);
	}
	RF_DBG(dm, DBG_RF_IQK, "IQ Calibration for %s for %d times\n", (is2T ? "2T2R" : "1T1R"), t);

	_phy_path_adda_on8723b(dm, ADDA_REG, true, is2T);
	_phy_mac_setting_calibration8723b(dm, IQK_MAC_REG, dm->rf_calibrate_info.IQK_MAC_backup);

	/*odm_set_bb_reg(dm, REG_FPGA0_RFMOD, BIT24, 0x00); */
	odm_set_bb_reg(dm, REG_CCK_0_AFE_SETTING, 0x0f000000, 0xf);
	odm_set_bb_reg(dm, REG_OFDM_0_TRX_PATH_ENABLE, MASKDWORD, 0x03a05600);
	odm_set_bb_reg(dm, REG_OFDM_0_TR_MUX_PAR, MASKDWORD, 0x000800e4);
	odm_set_bb_reg(dm, REG_FPGA0_XCD_RF_INTERFACE_SW, MASKDWORD, 0x22204000);

	/*RX IQ calibration setting for 8723B D cut large current issue when leaving IPS*/

	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x30000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xf7fb7);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x20, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x60fbd);

	/*path A TX IQK*/
	for (i = 0 ; i < retry_count ; i++) {
		path_aok = phy_path_a_iqk_8723b(dm, is2T, rf_path);
		if (path_aok == 0x01) {
			odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
			dm->rf_calibrate_info.tx_lok[RF_PATH_A] = odm_get_rf_reg(dm, RF_PATH_A, RF_0x8, RFREGOFFSETMASK);

			RF_DBG(dm, DBG_RF_IQK, "path A Tx IQK Success!!\n");
			result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
			result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
			break;
		}
	}
	/*path A RXIQK*/
	for (i = 0 ; i < retry_count ; i++) {
		path_aok = phy_path_a_rx_iqk_8723b(dm, is2T, rf_path);
		if (path_aok == 0x03) {
			RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK Success!!\n");
			/*				result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD)&0x3FF0000)>>16;*/
			/*				result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD)&0x3FF0000)>>16;*/
			result[t][2] = (odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
			result[t][3] = (odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
			break;
		} else
			RF_DBG(dm, DBG_RF_IQK, "path A Rx IQK Fail!!\n");

	}

	if (0x00 == path_aok)
		RF_DBG(dm, DBG_RF_IQK, "path A IQK failed!!\n");
	/*path B IQK*/
	if (is2T) {
		/*path B TX IQK*/
		for (i = 0 ; i < retry_count ; i++) {
			path_bok = phy_path_b_iqk_8723b(dm);
			if (path_bok == 0x01) {
				odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0x000000);
				dm->rf_calibrate_info.tx_lok[RF_PATH_B] = odm_get_rf_reg(dm, RF_PATH_B, RF_0x8, RFREGOFFSETMASK);

				RF_DBG(dm, DBG_RF_IQK, "path B Tx IQK Success!!\n");
				result[t][4] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
				result[t][5] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD) & 0x3FF0000) >> 16;
				break;
			}
		}


		/*path B RX IQK*/
		for (i = 0 ; i < retry_count ; i++) {
			path_bok = phy_path_b_rx_iqk_8723b(dm, is2T);
			if (path_bok == 0x03) {
				RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK Success!!\n");
				/*				result[t][0] = (odm_get_bb_reg(dm, REG_TX_POWER_BEFORE_IQK_A, MASKDWORD)&0x3FF0000)>>16;*/
				/*				result[t][1] = (odm_get_bb_reg(dm, REG_TX_POWER_AFTER_IQK_A, MASKDWORD)&0x3FF0000)>>16; */
				result[t][6] = (odm_get_bb_reg(dm, REG_RX_POWER_BEFORE_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
				result[t][7] = (odm_get_bb_reg(dm, REG_RX_POWER_AFTER_IQK_A_2, MASKDWORD) & 0x3FF0000) >> 16;
				break;
			} else
				RF_DBG(dm, DBG_RF_IQK, "path B Rx IQK Fail!!\n");
		}

		if (0x00 == path_bok)
			RF_DBG(dm, DBG_RF_IQK, "path B IQK failed!!\n");
	}

	/*Back to BB mode, load original value*/
	RF_DBG(dm, DBG_RF_IQK, "IQK:Back to BB mode, load original value!\n");
	odm_set_bb_reg(dm, REG_FPGA0_IQK, MASKH3BYTES, 0);

	if (t != 0) {
		/*Reload ADDA power saving parameters*/
		_phy_reload_adda_registers8723b(dm, ADDA_REG, dm->rf_calibrate_info.ADDA_backup, IQK_ADDA_REG_NUM);
		/*Reload MAC parameters*/
		_phy_reload_mac_registers8723b(dm, IQK_MAC_REG, dm->rf_calibrate_info.IQK_MAC_backup);
		_phy_reload_adda_registers8723b(dm, IQK_BB_REG_92C, dm->rf_calibrate_info.IQK_BB_backup, IQK_BB_REG_NUM);

		/*Allen initial gain 0xc50,  Restore RX initial gain*/
		odm_set_bb_reg(dm, R_0xc50, MASKBYTE0, 0x50);
		odm_set_bb_reg(dm, R_0xc50, MASKBYTE0, tmp0xc50);
		if (is2T) {
			odm_set_bb_reg(dm, R_0xc58, MASKBYTE0, 0x50);
			odm_set_bb_reg(dm, R_0xc58, MASKBYTE0, tmp0xc58);
		}
		/*load 0xe30 IQC default value*/
		odm_set_bb_reg(dm, REG_TX_IQK_TONE_A, MASKDWORD, 0x01008c00);
		odm_set_bb_reg(dm, REG_RX_IQK_TONE_A, MASKDWORD, 0x01008c00);
	}
	RF_DBG(dm, DBG_RF_IQK, "_phy_iq_calibrate_8723b() <==\n");
}


void
_phy_lc_calibrate_8723b(
	struct dm_struct		*dm,
	boolean		is2T
)
{
	u8	tmp_reg;
	u32	rf_amode = 0, rf_bmode = 0, lc_cal;

	/*Check continuous TX and Packet TX*/
	tmp_reg = odm_read_1byte(dm, 0xd03);

	if ((tmp_reg & 0x70) != 0)			/*Deal with contisuous TX case*/
		odm_write_1byte(dm, 0xd03, tmp_reg & 0x8F);	/*disable all continuous TX*/
	else							/* Deal with Packet TX case*/
		odm_write_1byte(dm, REG_TXPAUSE, 0xFF);			/* block all queues*/

	if ((tmp_reg & 0x70) != 0)	{
		/*1. Read original RF mode*/
		rf_amode = odm_get_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS);
		if (is2T)
			rf_bmode = odm_get_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS);
		/*2. Set RF mode = standby mode*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS, (rf_amode & 0x8FFFF) | 0x10000);
		if (is2T)
			odm_set_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS, (rf_bmode & 0x8FFFF) | 0x10000);
	}

	/*3. Read RF reg18*/
	lc_cal = odm_get_rf_reg(dm, RF_PATH_A, RF_CHNLBW, MASK12BITS);

	/*4. Set LC calibration begin	bit15*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xb0, RFREGOFFSETMASK, 0xDFBE0); /* LDO ON*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_CHNLBW, MASK12BITS, lc_cal | 0x08000);

	ODM_delay_ms(100);

	odm_set_rf_reg(dm, RF_PATH_A, RF_0xb0, RFREGOFFSETMASK, 0xDFFE0); /* LDO OFF*/

	/* channel 10 LC calibration issue for 8723bs with 26M xtal */
	if (dm->support_interface == ODM_ITRF_SDIO && dm->package_type >= 0x2)
		odm_set_rf_reg(dm, RF_PATH_A, RF_CHNLBW, MASK12BITS, lc_cal);

	/*Restore original situation*/
	if ((tmp_reg & 0x70) != 0) {	/*Deal with contisuous TX case */
		/*path-A*/
		odm_write_1byte(dm, 0xd03, tmp_reg);
		odm_set_rf_reg(dm, RF_PATH_A, RF_AC, MASK12BITS, rf_amode);

		/*path-B*/
		if (is2T)
			odm_set_rf_reg(dm, RF_PATH_B, RF_AC, MASK12BITS, rf_bmode);
	} else
		odm_write_1byte(dm, REG_TXPAUSE, 0x00);

}


/*IQK version:0x1e    20171109*/
/*IQK is controlled by is_2ant, RF path*/
void
phy_iq_calibrate_8723b(
	void		*dm_void,
	boolean		is_recovery
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
	struct _hal_rf_				*rf = &(dm->rf_table);
	struct dm_rf_calibration_struct *cali_info = &(dm->rf_calibrate_info);
	s32			result[4][8];
	u8			i, final_candidate, indexforchannel;
	boolean			is_patha_ok, is_pathb_ok;
	s32			rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc, reg_tmp = 0;
	boolean			is12simular, is13simular, is23simular;
	boolean			is_start_cont_tx = false, is_single_tone = false, is_carrier_suppression = false;
	u32			IQK_BB_REG_92C[IQK_BB_REG_NUM] = {
		REG_OFDM_0_XA_RX_IQ_IMBALANCE,	REG_OFDM_0_XB_RX_IQ_IMBALANCE,
		REG_OFDM_0_ECCA_THRESHOLD,	REG_OFDM_0_AGC_RSSI_TABLE,
		REG_OFDM_0_XA_TX_IQ_IMBALANCE,	REG_OFDM_0_XB_TX_IQ_IMBALANCE,
		REG_OFDM_0_XC_TX_AFE,	REG_OFDM_0_XD_TX_AFE,
		REG_OFDM_0_RX_IQ_EXT_ANTA
	};
	u32			GNT_BT_default;
	u32			start_time;
	s32			progressing_time;
	u32			path_sel_bb;
	u32 offset, data;
	u8 path, is_result = SUCCESS;
	

	/* Save RF path */
	path_sel_bb = odm_get_bb_reg(dm, R_0x948, MASKDWORD);
	path  = (((path_sel_bb == 0x0) || (path_sel_bb == 0x200)) ? RF_PATH_A : RF_PATH_B);

	if (is_recovery) {
		/*Restore TX IQK*/
		for (i = 0; i < 3; ++i) {
			offset = cali_info->tx_iqc_8723b[path][i][0];
			data = cali_info->tx_iqc_8723b[path][i][1];
			if ((offset == 0) || (i == 1 && data == 0)) {	/* 0xc80, 0xc88 ==> index=1 */
				is_result = FAIL;
				break;
			}
			/*RT_TRACE(_module_mp_, _drv_notice_,("Switch to S1 TxIQC(offset, data) = (0x%X, 0x%X)\n", offset, data));*/
			odm_set_bb_reg(dm, offset, MASKDWORD, data);
		}

		/* Restore RX IQK*/
		for (i = 0; i < 2; ++i) {
			offset = cali_info->rx_iqc_8723b[path][i][0];
			data = cali_info->rx_iqc_8723b[path][i][1];
			if ((offset == 0) || (i == 0 && data == 0)) {	/* 0xc14, 0xc1c ==> index=0 */
				is_result = FAIL;
				break;
			}
			/*RT_TRACE(_module_mp_, _drv_notice_,("Switch to S1 RxIQC (offset, data) = (0x%X, 0x%X)\n", offset, data));*/
			odm_set_bb_reg(dm, offset, MASKDWORD, data);
		}

		if (dm->rf_calibrate_info.tx_lok[RF_PATH_A] == 0)
			is_result = FAIL;
		else {
			odm_set_rf_reg(dm, RF_PATH_A, RF_TXM_IDAC, RFREGOFFSETMASK, dm->rf_calibrate_info.tx_lok[RF_PATH_A]);
			odm_set_rf_reg(dm, RF_PATH_B, RF_TXM_IDAC, RFREGOFFSETMASK, dm->rf_calibrate_info.tx_lok[RF_PATH_B]);
		}

		if (is_result == SUCCESS)
			goto out;
	}

#if 0
	if (is_recovery) {
		RF_DBG(dm, DBG_RF_INIT, "phy_iq_calibrate_8723b: Return due to is_recovery!\n");
		_phy_reload_adda_registers8723b(dm, IQK_BB_REG_92C, dm->rf_calibrate_info.IQK_BB_backup_recover, 9);
		goto out;
	}
#endif

	/* Check & wait if BT is doing IQK */
	if (*(dm->mp_mode) == false)
		_phy_check_coex_status_8723b(dm, true);

	RF_DBG(dm, DBG_RF_IQK, "IQK:Start!!!\n");
	;

	/*save default GNT_BT*/
	GNT_BT_default = odm_get_bb_reg(dm, R_0x764, MASKDWORD);

	for (i = 0; i < 8; i++) {
		result[0][i] = 0;
		result[1][i] = 0;
		result[2][i] = 0;
		result[3][i] = 0;
	}
	final_candidate = 0xff;
	is_patha_ok = false;
	is_pathb_ok = false;
	is12simular = false;
	is23simular = false;
	is13simular = false;


	for (i = 0; i < 3; i++) {
		_phy_iq_calibrate_8723b(dm, result, i, ~(*dm->is_1_antenna), *dm->rf_default_path);
		if (i == 1) {
			is12simular = phy_simularity_compare_8723b(dm, result, 0, 1);
			if (is12simular) {
				final_candidate = 0;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is12simular final_candidate is %x\n", final_candidate);
				break;
			}
		}

		if (i == 2) {
			is13simular = phy_simularity_compare_8723b(dm, result, 0, 2);
			if (is13simular) {
				final_candidate = 0;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is13simular final_candidate is %x\n", final_candidate);
				break;
			}
			is23simular = phy_simularity_compare_8723b(dm, result, 1, 2);
			if (is23simular) {
				final_candidate = 1;
				RF_DBG(dm, DBG_RF_IQK, "IQK: is23simular final_candidate is %x\n", final_candidate);
			} else {
				for (i = 0; i < 8; i++)
					reg_tmp += result[3][i];

				if (reg_tmp != 0)
					final_candidate = 3;
				else
					final_candidate = 0xFF;
			}
		}
	}

	if ((result[final_candidate][0] | result[final_candidate][2] | result[final_candidate][4] | result[final_candidate][6]) == 0) {
		for (i = 0; i < 8; i++) {
			if ((i == 0) || (i == 2) || (i == 4)  || (i == 6))
				result[final_candidate][i] = 0x100;
			else
				result[final_candidate][i] = 0;
		}
	}

	for (i = 0; i < 4; i++)	{
		rege94 = result[i][0];
		rege9c = result[i][1];
		regea4 = result[i][2];
		regeac = result[i][3];
		regeb4 = result[i][4];
		regebc = result[i][5];
		regec4 = result[i][6];
		regecc = result[i][7];
		RF_DBG(dm, DBG_RF_IQK, "IQK: rege94=%x rege9c=%x regea4=%x regeac=%x regeb4=%x regebc=%x regec4=%x regecc=%x\n ", rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc);
	}

	if (final_candidate != 0xff) {
		dm->rf_calibrate_info.rege94 = rege94 = result[final_candidate][0];
		dm->rf_calibrate_info.rege9c = rege9c = result[final_candidate][1];
		regea4 = result[final_candidate][2];
		regeac = result[final_candidate][3];
		dm->rf_calibrate_info.regeb4 = regeb4 = result[final_candidate][4];
		dm->rf_calibrate_info.regebc = regebc = result[final_candidate][5];
		regec4 = result[final_candidate][6];
		regecc = result[final_candidate][7];
		RF_DBG(dm, DBG_RF_IQK, "IQK: final_candidate is %x\n", final_candidate);
		RF_DBG(dm, DBG_RF_IQK, "IQK: rege94=%x rege9c=%x regea4=%x regeac=%x regeb4=%x regebc=%x regec4=%x regecc=%x\n ", rege94, rege9c, regea4, regeac, regeb4, regebc, regec4, regecc);
		is_patha_ok = is_pathb_ok = true;
	} else {
		RF_DBG(dm, DBG_RF_IQK, "IQK: FAIL use default value\n");

		dm->rf_calibrate_info.rege94 = dm->rf_calibrate_info.regeb4 = 0x100;
		dm->rf_calibrate_info.rege9c = dm->rf_calibrate_info.regebc = 0x0;
		panic_printk("Load IQK default value");
	}

	if (rege94 != 0)
		_phy_path_a_fill_iqk_matrix8723b(dm, is_patha_ok, result, final_candidate, (regea4 == 0));
	if (regeb4 != 0)
		_phy_path_b_fill_iqk_matrix8723b(dm, is_pathb_ok, result, final_candidate, (regec4 == 0));

	indexforchannel = 0;


	/*To Fix BSOD when final_candidate is 0xff
	by sherry 20120321 */
	if (final_candidate < 4) {
		for (i = 0; i < iqk_matrix_reg_num; i++)
			dm->rf_calibrate_info.iqk_matrix_reg_setting[indexforchannel].value[0][i] = result[final_candidate][i];
		dm->rf_calibrate_info.iqk_matrix_reg_setting[indexforchannel].is_iqk_done = true;
	}
	RF_DBG(dm, DBG_RF_IQK, "\nIQK OK indexforchannel %d.\n", indexforchannel);
	_phy_save_adda_registers8723b(dm, IQK_BB_REG_92C, dm->rf_calibrate_info.IQK_BB_backup_recover, IQK_BB_REG_NUM);

	/*restore GNT_BT*/
	odm_set_bb_reg(dm, R_0x764, MASKDWORD, GNT_BT_default);

	/*Resotr RX mode table parameter*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_WE_LUT, 0x80000, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_RCK_OS, RFREGOFFSETMASK, 0x18000);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G1, RFREGOFFSETMASK, 0x0001f);
	odm_set_rf_reg(dm, RF_PATH_A, RF_TXPA_G2, RFREGOFFSETMASK, 0xe6177);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0xed, 0x20, 0x1);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x43, RFREGOFFSETMASK, 0x300bd);

	/*set GNT_BT= HW control*/
	/*	odm_set_bb_reg(dm, R_0x764, BIT12, 0x0);
		odm_set_bb_reg(dm, R_0x764, BIT(11), 0x0); */

	if(*(dm->mp_mode)) {
		if ((path_sel_bb == 0x0) || (path_sel_bb == 0x200))  /* S1 */
			odm_set_iqc_by_rfpath(dm, 0);
		else
			odm_set_iqc_by_rfpath(dm, 1);
	} else {
		if (*dm->rf_default_path == 0x0)  /* S1 */
			odm_set_iqc_by_rfpath(dm, 0);
		else
			odm_set_iqc_by_rfpath(dm, 1);
	}
	if (*(dm->mp_mode) == false)
		_phy_check_coex_status_8723b(dm, false);
	RF_DBG(dm, DBG_RF_IQK, "IQK finished\n");
out:
	return;
}

void
phy_lc_calibrate_8723b(
	void		*dm_void
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;

	_phy_lc_calibrate_8723b(dm, false);
}


void _phy_set_rf_path_switch_8723b(
	struct dm_struct		*dm,
	boolean		is_main,
	boolean		is2T
)
{
	if (is_main)   /* Left antenna */
		odm_set_bb_reg(dm, R_0x92c, MASKDWORD, 0x1);
	else
		odm_set_bb_reg(dm, R_0x92c, MASKDWORD, 0x2);
}


void phy_set_rf_path_switch_8723b(
	struct dm_struct	*dm,
	boolean		is_main
)
{

#ifdef DISABLE_BB_RF
	return;
#endif

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
	_phy_set_rf_path_switch_8723b(adapter, is_main, true);
#endif

}


/*return value true => Main; false => Aux */
boolean _phy_query_rf_path_switch_8723b(
	struct dm_struct	*dm,
	boolean		is2T
)
{
	if (odm_get_bb_reg(dm, R_0x92c, MASKDWORD) == 0x01)
		return true;
	else
		return false;
}


/*return value true => Main; false => Aux*/
boolean phy_query_rf_path_switch_8723b(
	struct dm_struct		*dm
)
{
#ifdef DISABLE_BB_RF
	return true;
#endif

	return _phy_query_rf_path_switch_8723b(dm, false);
}
