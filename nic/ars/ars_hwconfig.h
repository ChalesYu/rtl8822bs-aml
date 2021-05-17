#ifndef __ARS_HWCONFIG_H__
#define __ARS_HWCONFIG_H__

#define ODM_RF_PATH_MAX 2


typedef enum _RF_PATH 
{
    RF_PATH_A = 0,
    RF_PATH_B = 1,
    RF_PATH_C = 2,
    RF_PATH_D = 3,
} RF_PATH, *PRF_PATH;

typedef enum HW_RF_RADIO_PATH
{
    ODM_RF_PATH_A = 0,   //Radio Path A
    ODM_RF_PATH_B = 1,   //Radio Path B
    ODM_RF_PATH_C = 2,   //Radio Path C
    ODM_RF_PATH_D = 3,   //Radio Path D
    ODM_RF_PATH_AB,
    ODM_RF_PATH_AC,
    ODM_RF_PATH_AD,
    ODM_RF_PATH_BC,
    ODM_RF_PATH_BD,
    ODM_RF_PATH_CD,
    ODM_RF_PATH_ABC,
    ODM_RF_PATH_ACD,
    ODM_RF_PATH_BCD,
    ODM_RF_PATH_ABCD,
  //  ODM_RF_PATH_MAX,    //Max RF number 90 support
} HW_RF_RADIO_PATH_E, *PHW_RF_RADIO_PATH_E;

typedef struct _ODM_Per_Pkt_Info_
{
    //u1Byte        Rate;   
    wf_u8       DataRate;
    wf_u8       StationID;
    wf_bool     bPacketMatchBSSID;
    wf_bool     bPacketToSelf;
    wf_bool     bPacketBeacon;
    wf_bool     bToSelf;
}ODM_PACKET_INFO_T;


typedef struct hw_phystatus_st_
{
    wf_u8       RxPWDBAll;  
    wf_u8       SignalQuality;              /* in 0-100 index. */
    wf_s8       RxMIMOSignalQuality[4];     /* per-path's EVM */
    wf_u8       RxMIMOEVMdbm[4];            /* per-path's EVM dbm */
    wf_u8       RxMIMOSignalStrength[4];    /* in 0~100 index */
    wf_s16      Cfo_short[4];               /* per-path's Cfo_short */
    wf_s16      Cfo_tail[4];                    /* per-path's Cfo_tail */
    wf_s8       RxPower;                    /* in dBm Translate from PWdB */
    wf_s8       RecvSignalPower;            /* Real power in dBm for this packet, no beautification and aggregation. Keep this raw info to be used for the other procedures. */
    wf_u8       BTRxRSSIPercentage;
    wf_u8       SignalStrength;             /* in 0-100 index. */
    wf_s8       RxPwr[4];                   /* per-path's pwdb */
    wf_s8       RxSNR[4];                   /* per-path's SNR   */

    wf_u8       BandWidth;
    wf_u8       btCoexPwrAdjust;

}hw_phystatus_st;

typedef struct
{
    recv_agc_st path_agc[2];
    wf_u8 ch_corr[2];
    wf_u8 cck_sig_qual_ofdm_pwdb_all;
    wf_u8 cck_agc_rpt_ofdm_cfosho_a;
    wf_u8 cck_rpt_b_ofdm_cfosho_b;
    wf_u8 rsvd_1;
    wf_u8 noise_power_db_msb;
    wf_s8 path_cfotail[2];
    wf_u8 pcts_mask[2];
    wf_s8 stream_rxevm[2];
    wf_u8 path_rxsnr[2];
    wf_u8 noise_power_db_lsb;
    wf_u8 rsvd_2[3];
    wf_u8 stream_csi[2];
    wf_u8 stream_target_csi[2];
    wf_s8 sig_evm;
    wf_u8 rsvd_3;
    wf_u8 antsel_rx_keep_2:1;
    wf_u8 sgi_en:1;
    wf_u8 rxsc:2;
    wf_u8 idle_long:1;
    wf_u8 r_ant_train_en:1;
    wf_u8 ant_sel_b:1;
    wf_u8 ant_sel:1;

} hw_recv_phy_status_st;

typedef struct _BB_REGISTER_DEFINITION
{
    wf_u32 rfintfs;         // set software control: 
                        //      0x870~0x877[8 bytes]
                            
    wf_u32 rfintfo;             // output data: 
                        //      0x860~0x86f [16 bytes]
                            
    wf_u32 rfintfe;             // output enable: 
                        //      0x860~0x86f [16 bytes]
                            
    wf_u32 rf3wireOffset;   // LSSI data:
                        //      0x840~0x84f [16 bytes]

    wf_u32 rfHSSIPara2;     // wire parameter control2 : 
                        //      0x824~0x827,0x82c~0x82f, 0x834~0x837, 0x83c~0x83f [16 bytes]
                                
    wf_u32 rfLSSIReadBack;  //LSSI RF readback data SI mode
                        //      0x8a0~0x8af [16 bytes]

    wf_u32 rfLSSIReadBackPi;    //LSSI RF readback data PI mode 0x8b8-8bc for Path A and B

}BB_REGISTER_DEFINITION_T, *PBB_REGISTER_DEFINITION_T;


typedef struct hwconfig_info_st_
{
    hw_recv_phy_status_st phy_origin; 
    ODM_PACKET_INFO_T pkt_info; //driver 
    hw_phystatus_st phystatus; // to caculate 

    BB_REGISTER_DEFINITION_T PHYRegDef[MAX_RF_PATH];
}ars_hwconfig_info_st;

wf_s32 wf_ars_query_phystatus(nic_info_st *nic_info, recv_phy_status_st *rps, wf_u8 *pbuf, prx_pkt_t ppt);

wf_u32 hw_read_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask);
wf_s32 hw_write_bb_reg(nic_info_st *pnic_info,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data);
wf_s32 hw_write_rf_reg(    nic_info_st *nic_info,wf_u8 eRFPath,wf_u32 RegAddr,wf_u32 BitMask,wf_u32 Data);
wf_s32 phy_InitBBRFRegisterDefinition(void *ars);

wf_s32 FillH2CCmd(nic_info_st *nic_info, wf_u8 ElementID, wf_u32 CmdLen, wf_u8 *pCmdBuffer);

#endif
