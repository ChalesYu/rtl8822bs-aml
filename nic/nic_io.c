#include "common.h"
#include "wf_debug.h"

wf_u8 wf_io_read8(const nic_info_st *nic_info, wf_u32 addr,int *err)
{
    wf_u8 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

wf_u16 wf_io_read16(const nic_info_st *nic_info, wf_u32 addr, int *err)
{
    wf_u16 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

wf_u32 wf_io_read32(const nic_info_st *nic_info, wf_u32 addr, int *err)
{
    wf_u32 value;
    int ret = 0;
    WF_ASSERT(nic_info != NULL);

    ret = nic_info->nic_read(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
    if(err)
    {
        *err = ret;
    }

    return value;
}

int wf_io_write8(const nic_info_st *nic_info, wf_u32 addr, wf_u8 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}

int wf_io_write16(const nic_info_st *nic_info, wf_u32 addr, wf_u16 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}

int wf_io_write32(const nic_info_st *nic_info, wf_u32 addr, wf_u32 value)
{
    WF_ASSERT(nic_info != NULL);

    return nic_info->nic_write(nic_info->hif_node, 0, addr, (char *)&value, sizeof(value));
}


int wf_io_write_data(const nic_info_st *nic_info, wf_u8 agg_num, char *pbuf, wf_u32 len, wf_u32 addr,
                            int (*callback_func)(void*tx_info, void *param), void *tx_info, void *param)
{
    int ret = 0;

    WF_ASSERT(nic_info != NULL);

    if (nic_info->nic_tx_queue_insert == NULL)
    {
        LOG_E("nic_tx_queue_insert is not register, please check!!");
        return -1;
    }

    ret = nic_info->nic_tx_queue_insert(nic_info->hif_node, agg_num, pbuf, len, addr,
                            callback_func, tx_info, param);

    return ret;
}


int wf_io_write_data_queue_check(const nic_info_st *nic_info)
{
    int ret = 0;
    WF_ASSERT(nic_info != NULL);
    if (nic_info->nic_tx_queue_empty == NULL)
    {
        LOG_E("nic_tx_queue_empty is not register, please check!!");
        return -1;
    }

    return nic_info->nic_tx_queue_empty(nic_info->hif_node);
}


int wf_io_write_firmware(const nic_info_st *nic_info, wf_u8 which,  wf_u8 *firmware, wf_u32 len)
{
    WF_ASSERT(nic_info != NULL);
    WF_ASSERT(firmware != NULL);

    if (nic_info->nic_write_fw == NULL)
    {
        LOG_E("nic_write_fw is not register, please check!!");
        return -1;
    }

    return nic_info->nic_write_fw(nic_info->hif_node, which, firmware, len);
}


static char* cmd_to_str(int cmd)
{
    #define c2s(x) #x
    switch(cmd)
    {
        case FUNC_REPLY                                     :
           return c2s(FUNC_REPLY);
        case UMSG_OPS_READ_VERSION                          :
            return c2s(UMSG_OPS_READ_VERSION);
        case UMSG_OPS_HAL_GETESIZE                          :
            return c2s(UMSG_OPS_HAL_GETESIZE);
        case UMSG_OPS_HAL_EFSUSESEL                         :
            return c2s(UMSG_OPS_HAL_EFSUSESEL);
        case UMSG_OPS_HAL_EFUSEMAP                          :
            return c2s(UMSG_OPS_HAL_EFUSEMAP);
        case UMSG_OPS_HAL_EFUSEMAP_LEN                      :
            return c2s(UMSG_OPS_HAL_EFUSEMAP_LEN);
        case UMSG_OPS_HAL_EFUSETYPE                         :
            return c2s(UMSG_OPS_HAL_EFUSETYPE);
        case UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG              :
            return c2s(UMSG_OPS_HAL_EFUSE_BAUTOLOAD_FLAG);
        case UMSG_OPS_HAL_PWRDWMODE                         :
            return c2s(UMSG_OPS_HAL_PWRDWMODE);
        case UMSG_OPS_HAL_RFCHNLVAL                         :
            return c2s(UMSG_OPS_HAL_RFCHNLVAL);
        case UMSG_OPS_HAL_INIT_STEP0                        :
            return c2s(UMSG_OPS_HAL_INIT_STEP0);
        case UMSG_OPS_HAL_INIT_STEP1                        :
            return c2s(UMSG_OPS_HAL_INIT_STEP1);
        case UMSG_OPS_HAL_INIT_ANT_SEL                      :
            return c2s(UMSG_OPS_HAL_INIT_ANT_SEL);
        case UMSG_OPS_HAL_INIT_DM                           :
            return c2s(UMSG_OPS_HAL_INIT_DM);
        case UMSG_OPS_HAL_RF_CHNLBW                         :
            return c2s(UMSG_OPS_HAL_RF_CHNLBW);
        case UMSG_OPS_HAL_SET_BW_MODE                       :
            return c2s(UMSG_OPS_HAL_SET_BW_MODE);
        case UMSG_OPS_SET_TX_PW_LEVEL                       :
            return c2s(UMSG_OPS_SET_TX_PW_LEVEL);
        case UMSG_OPS_HAL_MAC_TXEN_RXEN                     :
            return c2s(UMSG_OPS_HAL_MAC_TXEN_RXEN);
        case UMSG_OPS_HAL_FWHW_TXQ_CTRL                     :
            return c2s(UMSG_OPS_HAL_FWHW_TXQ_CTRL);
        case UMSG_OPS_HAL_INIT_STEP2                        :
            return c2s(UMSG_OPS_HAL_INIT_STEP2);
        case UMSG_PHY_LCC_RATE                              :
            return c2s(UMSG_PHY_LCC_RATE);
        case UMSG_OPS_HAL_SET_HWREG                         :
            return c2s(UMSG_OPS_HAL_SET_HWREG);
        case UMSG_OPS_HAL_GET_HWREG                         :
            return c2s(UMSG_OPS_HAL_GET_HWREG);
        case UMSG_OPS_HAL_CONFIG_MSG                        :
            return c2s(UMSG_OPS_HAL_CONFIG_MSG);
        case UMSG_OPS_HAL_INIT_MSG                          :
            return c2s(UMSG_OPS_HAL_INIT_MSG);
        case UMSG_OPS_HAL_MSG_WDG                           :
            return c2s(UMSG_OPS_HAL_MSG_WDG);
        case UMSG_OPS_HAL_WRITEVAR_MSG                      :
            return c2s(UMSG_OPS_HAL_WRITEVAR_MSG);
        case UMSG_OPS_HAL_READVAR_MSG                       :
            return c2s(UMSG_OPS_HAL_READVAR_MSG);
        case UMSG_OPS_MSG_UPDATEIG                          :
            return c2s(UMSG_OPS_MSG_UPDATEIG);
        case UMSG_OPS_MSG_PAUSEIG                           :
            return c2s(UMSG_OPS_MSG_PAUSEIG);
        case UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS             :
            return c2s(UMSG_OPS_PHYDM_wMBOX0_CONTENT_PARS);
        case UMSG_0PS_MSG_GET_RATE_BITMAP                   :
            return c2s(UMSG_0PS_MSG_GET_RATE_BITMAP);
        case UMSG_OPS_MSG_RF_SAVE                           :
            return c2s(UMSG_OPS_MSG_RF_SAVE);
        case UMSG_OPS_MSG_RHY_STATUS                        :
            return c2s(UMSG_OPS_MSG_RHY_STATUS);
        case UMSG_OPS_MSG_EMB_MAC_IMG                       :
            return c2s(UMSG_OPS_MSG_EMB_MAC_IMG);
        case UMSG_OPS_MSG_EMB_RF_IMG                        :
            return c2s(UMSG_OPS_MSG_EMB_RF_IMG);
        case UMSG_OPS_INIT_BB_PHY_REG                       :
            return c2s(UMSG_OPS_INIT_BB_PHY_REG);
        case UMSG_OPS_MSG_EMB_TXPWRTRACK_IMG                :
            return c2s(UMSG_OPS_MSG_EMB_TXPWRTRACK_IMG);
        case UMSG_OPS_HAL_GET_MSG_STA_INFO                  :
            return c2s(UMSG_OPS_HAL_GET_MSG_STA_INFO);
        case UMSG_OPS_HAL_SYNC_MSG_STA_INFO                 :
            return c2s(UMSG_OPS_HAL_SYNC_MSG_STA_INFO);
        case UMSG_OPS_MSG_TXPWR_TRACKING_CHECK              :
            return c2s(UMSG_OPS_MSG_TXPWR_TRACKING_CHECK);
        case UMSG_OPS_HAL_MSG_ADAPTIVITY_PARM_SET           :
            return c2s(UMSG_OPS_HAL_MSG_ADAPTIVITY_PARM_SET);
        case UMSG_OPS_HAL_MSG_SET_PWR_TRACK_CTR             :
            return c2s(UMSG_OPS_HAL_MSG_SET_PWR_TRACK_CTR);
        case UMSG_OPS_HAL_MSG_GET_PWR_TRACK_CTR             :
            return c2s(UMSG_OPS_HAL_MSG_GET_PWR_TRACK_CTR);
        case UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VER            :
            return c2s(UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VER);
        case UMSG_OPS_HAL_MSG_ABILITY_OPS                   :
            return c2s(UMSG_OPS_HAL_MSG_ABILITY_OPS);
        case UMSG_OPS_HAL_MSG_GET_TX_PWR_TRACKING_OFFSET    :
            return c2s(UMSG_OPS_HAL_MSG_GET_TX_PWR_TRACKING_OFFSET);
        case UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VAL_TYPE       :
            return c2s(UMSG_OPS_HAL_MSG_GET_PHY_REG_PG_VAL_TYPE);
        case UMSG_OPS_HAL_SET_REG_CR_9086X                  :
            return c2s(UMSG_OPS_HAL_SET_REG_CR_9086X);
        case UMSG_OPS_HAL_MSG_GET_DISABLE_PWR_TRAINING      :
            return c2s(UMSG_OPS_HAL_MSG_GET_DISABLE_PWR_TRAINING);
        case UMSG_OPS_HAL_MSG_SET_REGA24                    :
            return c2s(UMSG_OPS_HAL_MSG_SET_REGA24);
        case UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VERISON        :
            return c2s(UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VERISON);
        case UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VAL_TYPE       :
            return c2s(UMSG_OPS_HAL_MSG_SET_PHY_REG_PG_VAL_TYPE);
        case UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE            :
            return c2s(UMSG_OPS_HAL_MSG_INIT_DEFAULT_VALUE);
        case UMSG_OPS_HAL_MSG_SET_APK_THERMAL_METER_IGNORE  :
            return c2s(UMSG_OPS_HAL_MSG_SET_APK_THERMAL_METER_IGNORE);
        case UMSG_OPS_HAL_SET_REG_CCK_CHECK_9086X           :
            return c2s(UMSG_OPS_HAL_SET_REG_CCK_CHECK_9086X);
        case UMSG_OPS_HAL_SET_PLL_REF_CLK_SEL               :
            return c2s(UMSG_OPS_HAL_SET_PLL_REF_CLK_SEL);
        case UMSG_OPS_HAL_SET_REG_AMPDU_MAX_LENGTH_9086X    :
            return c2s(UMSG_OPS_HAL_SET_REG_AMPDU_MAX_LENGTH_9086X);
        case UMSG_OPS_HAL_SET_REG_DWBCN1_CTRL_9086X         :
            return c2s(UMSG_OPS_HAL_SET_REG_DWBCN1_CTRL_9086X);
        case UMSG_OPS_HAL_INIT_MSG_VAR                      :
            return c2s(UMSG_OPS_HAL_INIT_MSG_VAR);
        case UMSG_OPS_HAL_CALI_LLC                          :
            return c2s(UMSG_OPS_HAL_CALI_LLC);
        case UMSG_OPS_HAL_PHY_IQ_CALIBRATE                  :
            return c2s(UMSG_OPS_HAL_PHY_IQ_CALIBRATE);
        case UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE            :
            return c2s(UMSG_OPS_HAL_CONFIG_CONCURRENT_MODE);
        case UMSG_OPS_HAL_CHNLBW_MODE                       :
            return c2s(UMSG_OPS_HAL_CHNLBW_MODE);
        case UMSG_OPS_HAL_DW_FW                             :
            return c2s(UMSG_OPS_HAL_DW_FW);
        case UMSG_OPS_HAL_INIT_MAC_PHY_RF                   :
            return c2s(UMSG_OPS_HAL_INIT_MAC_PHY_RF);
        case UMSG_OPS_HAL_FW_INIT                           :
            return c2s(UMSG_OPS_HAL_FW_INIT);
        case UMSG_OPS_HAL_UPDATE_THERMAL                    :
            return c2s(UMSG_OPS_HAL_UPDATE_THERMAL);
        case UMSG_OPS_HAL_UPDATE_TX_FIFO                    :
            return c2s(UMSG_OPS_HAL_UPDATE_TX_FIFO);
        case UMSG_OPS_HAL_RESUME_TXBCN                      :
            return c2s(UMSG_OPS_HAL_RESUME_TXBCN);
        case UMSG_OPS_HAL_STOP_TXBCN                        :
            return c2s(UMSG_OPS_HAL_STOP_TXBCN);
        case UMSG_OPS_HAL_BCN_FUNC_ENABLE                   :
            return c2s(UMSG_OPS_HAL_BCN_FUNC_ENABLE);
        case UMSG_OPS_HAL_SET_BCN_REG                       :
            return c2s(UMSG_OPS_HAL_SET_BCN_REG);
        case UMSG_OPS_HAL_NOTCH_FILTER                      :
            return c2s(UMSG_OPS_HAL_NOTCH_FILTER);
        case UMSG_OPS_HW_VAR_SET_MONITOR                    :
            return c2s(UMSG_OPS_HW_VAR_SET_MONITOR);
        case UMSG_OPS_HAL_SET_MAC                           :
            return c2s(UMSG_OPS_HAL_SET_MAC);
        case UMSG_OPS_HAL_SET_BSSID                         :
            return c2s(UMSG_OPS_HAL_SET_BSSID);
        case UMSG_OPS_HAL_SET_BCN                           :
            return c2s(UMSG_OPS_HAL_SET_BCN);
        case UMSG_OPS_HW_SET_BASIC_RATE                     :
            return c2s(UMSG_OPS_HW_SET_BASIC_RATE);
        case UMSG_OPS_HW_SET_OP_MODE                        :
            return c2s(UMSG_OPS_HW_SET_OP_MODE);
        case UMSG_OPS_HW_SET_CORRECT_TSF                    :
            return c2s(UMSG_OPS_HW_SET_CORRECT_TSF);
        case UMSG_OPS_HW_SET_MLME_DISCONNECT                :
            return c2s(UMSG_OPS_HW_SET_MLME_DISCONNECT);
        case UMSG_OPS_HW_SET_MLME_SITE                      :
            return c2s(UMSG_OPS_HW_SET_MLME_SITE);
        case UMSG_OPS_HW_SET_MLME_JOIN                      :
            return c2s(UMSG_OPS_HW_SET_MLME_JOIN);
        case UMSG_OPS_HW_FIFO_CLEARN_UP                     :
            return c2s(UMSG_OPS_HW_FIFO_CLEARN_UP);
        case UMSG_OPS_HW_UPDATE_TSF                         :
            return c2s(UMSG_OPS_HW_UPDATE_TSF);
        case UMSG_OPS_HW_SET_DK_CFG                         :
            return c2s(UMSG_OPS_HW_SET_DK_CFG);
        case UMSG_OPS_HW_FWLPS_RF_ON                        :
            return c2s(UMSG_OPS_HW_FWLPS_RF_ON);
        case UMSG_OPS_HAL_STATES                            :
            return c2s(UMSG_OPS_HAL_STATES);
        case UMSG_OPS_HAL_SEC_READ_CAM                      :
            return c2s(UMSG_OPS_HAL_SEC_READ_CAM);
        case UMSG_OPS_HAL_SEC_WRITE_CAM                     :
            return c2s(UMSG_OPS_HAL_SEC_WRITE_CAM);
        case UMSG_OPS_HAL_wMBOX1_CMD                        :
            return c2s(UMSG_OPS_HAL_wMBOX1_CMD);
        case UMSG_Ops_HAL_ISMONITOR_RST                     :
            return c2s(UMSG_Ops_HAL_ISMONITOR_RST);
        case UMSG_OPS_HAL_CHECK_RXFIFO_FULL                 :
            return c2s(UMSG_OPS_HAL_CHECK_RXFIFO_FULL);
        case UMSG_OPS_HAL_TXDMA_STATUS                      :
            return c2s(UMSG_OPS_HAL_TXDMA_STATUS);
        case UMSG_OPS_HAL_MSG_IO                            :
            return c2s(UMSG_OPS_HAL_MSG_IO);
        case UMSG_OPS_HAL_CKIPSSTUTAS                       :
            return c2s(UMSG_OPS_HAL_CKIPSSTUTAS);
        case UMSG_OPS_HAL_TEST_LDO                          :
            return c2s(UMSG_OPS_HAL_TEST_LDO);
        case UMSG_OPS_HAL_LPS_OPT                           :
            return c2s(UMSG_OPS_HAL_LPS_OPT);
        case UMSG_OPS_HAL_LPS_CONFIG                        :
            return c2s(UMSG_OPS_HAL_LPS_CONFIG);
        case UMSG_OPS_HAL_LPS_SET                           :
            return c2s(UMSG_OPS_HAL_LPS_SET);
        case UMSG_OPS_HAL_LPS_GET                           :
            return c2s(UMSG_OPS_HAL_LPS_GET);
        case UMSG_OPS_HAL_READ_WKFM_CAM                     :
            return c2s(UMSG_OPS_HAL_READ_WKFM_CAM);
        case UMSG_OPS_HAL_WRITE_WKFM_CAM                    :
            return c2s(UMSG_OPS_HAL_WRITE_WKFM_CAM);
        case UMSG_OPS_HAL_SWITCH_GPIO_WL                    :
            return c2s(UMSG_OPS_HAL_SWITCH_GPIO_WL);
        case UMSG_OPS_HAL_SET_OUTPUT_GPIO                   :
            return c2s(UMSG_OPS_HAL_SET_OUTPUT_GPIO);
        case UMSG_OPS_HAL_ENABLE_RXDMA                      :
            return c2s(UMSG_OPS_HAL_ENABLE_RXDMA);
        case UMSG_OPS_HAL_DISABLE_TX_REPORT                 :
            return c2s(UMSG_OPS_HAL_DISABLE_TX_REPORT);
        case UMSG_OPS_HAL_ENABLE_TX_REPORT                  :
            return c2s(UMSG_OPS_HAL_ENABLE_TX_REPORT);
        case UMSG_OPS_HAL_RELEASE_RX_DMA                    :
            return c2s(UMSG_OPS_HAL_RELEASE_RX_DMA);
        case UMSG_OPS_HAL_CHECK_WOW_CTL                     :
            return c2s(UMSG_OPS_HAL_CHECK_WOW_CTL);
        case UMSG_OPS_HAL_UPDATE_TX_IV                      :
            return c2s(UMSG_OPS_HAL_UPDATE_TX_IV);
        case UMSG_OPS_HAL_GATE_BB                           :
            return c2s(UMSG_OPS_HAL_GATE_BB);
        case UMSG_OPS_HAL_SET_RXFF_BOUNDARY                 :
            return c2s(UMSG_OPS_HAL_SET_RXFF_BOUNDARY);
        case UMSG_OPS_HAL_REG_SWITCH                        :
            return c2s(UMSG_OPS_HAL_REG_SWITCH);
        case UMSG_OPS_HAL_SET_USB_AGG_NORMAL                :
            return c2s(UMSG_OPS_HAL_SET_USB_AGG_NORMAL);
        case UMSG_OPS_HAL_SET_USB_AGG_CUSTOMER              :
            return c2s(UMSG_OPS_HAL_SET_USB_AGG_CUSTOMER);
        case UMSG_OPS_HAL_DM_DYNAMIC_TX_AGG                 :
            return c2s(UMSG_OPS_HAL_DM_DYNAMIC_TX_AGG);
        case UMSG_OPS_HAL_SET_RPWM                          :
            return c2s(UMSG_OPS_HAL_SET_RPWM);
        case UMSG_OPS_REPROBE_USB3                          :
            return c2s(UMSG_OPS_REPROBE_USB3);
        case UMSG_OPS_EFUSE_1BYTE                           :
            return c2s(UMSG_OPS_EFUSE_1BYTE);
        case UMSG_OPS_HAL_DEINIT                            :
            return c2s(UMSG_OPS_HAL_DEINIT);
        case UMSG_OPS_MAC_HIDDEN                            :
            return c2s(UMSG_OPS_MAC_HIDDEN);
        case UMSG_OPS_HAL_PAUSE_RXDMA                       :
            return c2s(UMSG_OPS_HAL_PAUSE_RXDMA);
        case MO_OPS_HAL_UMSG_CLOSE                          :
            return c2s(MO_OPS_HAL_UMSG_CLOSE);
        case UMSG_OPS_MP_TEST                               :
            return c2s(UMSG_OPS_MP_TEST);
        case UMSG_OPS_MP_CMD_HDL                            :
            return c2s(UMSG_OPS_MP_CMD_HDL);
        case UMSG_OPS_MP_SET_ANT_TX                         :
            return c2s(UMSG_OPS_MP_SET_ANT_TX);
        case UMSG_OPS_MP_SET_ANT_RX                         :
            return c2s(UMSG_OPS_MP_SET_ANT_RX);
        case UMSG_OPS_MP_PROSET_TXPWR_1                     :
            return c2s(UMSG_OPS_MP_PROSET_TXPWR_1);
        case UMSG_OPS_MP_PROSET_TXPWR_2                     :
            return c2s(UMSG_OPS_MP_PROSET_TXPWR_2);
        case UMSG_OPS_MP_INIT                               :
            return c2s(UMSG_OPS_MP_INIT);
        case UMSG_OPS_MP_ARX                                :
            return c2s(UMSG_OPS_MP_ARX);
        case UMSG_OPS_MP_GET_PSDATA                         :
            return c2s(UMSG_OPS_MP_GET_PSDATA);
        case UMSG_OPS_MP_SET_PRX                            :
            return c2s(UMSG_OPS_MP_SET_PRX);
        case UMSG_OPS_MP_JOIN                               :
            return c2s(UMSG_OPS_MP_JOIN);
        case UMSG_OPS_MP_DIS_DM                             :
            return c2s(UMSG_OPS_MP_DIS_DM);
        case UMSG_OPS_MP_SET_TXPOWERINDEX                   :
            return c2s(UMSG_OPS_MP_SET_TXPOWERINDEX);
        case UMSG_OPS_MP_SET_CCKCTX                         :
            return c2s(UMSG_OPS_MP_SET_CCKCTX);
        case UMSG_OPS_MP_SET_OFDMCTX                        :
            return c2s(UMSG_OPS_MP_SET_OFDMCTX);
        case UMSG_OPS_MP_SET_SINGLECARRTX                   :
            return c2s(UMSG_OPS_MP_SET_SINGLECARRTX);
        case UMSG_OPS_MP_SET_SINGLETONETX                   :
            return c2s(UMSG_OPS_MP_SET_SINGLETONETX);
        case UMSG_OPS_MP_SET_CARRSUPPTX                     :
            return c2s(UMSG_OPS_MP_SET_CARRSUPPTX);
        case UMSG_OPS_MP_SET_MACTXEDCA                      :
            return c2s(UMSG_OPS_MP_SET_MACTXEDCA);
        case UMSG_OPS_MSG_WRITE_DIG                         :
            return c2s(UMSG_OPS_MSG_WRITE_DIG);
        case UMSG_OPS_MP_EFUSE_READ                         :
            return c2s(UMSG_OPS_MP_EFUSE_READ);
        case UMSG_OPS_MP_EFUSE_WRITE                        :
            return c2s(UMSG_OPS_MP_EFUSE_WRITE);
        case UMSG_OPS_MP_EFUSE_ACCESS                       :
            return c2s(UMSG_OPS_MP_EFUSE_ACCESS);
        case UMSG_OPS_MP_EFUSE_GSize                        :
            return c2s(UMSG_OPS_MP_EFUSE_GSize);
        case UMSG_OPS_MP_EFUSE_GET                          :
            return c2s(UMSG_OPS_MP_EFUSE_GET);
        case UMSG_OPS_MP_EFUSE_SET                          :
            return c2s(UMSG_OPS_MP_EFUSE_SET);
        case UMSG_OPS_MP_MACRXCOUNT                         :
            return c2s(UMSG_OPS_MP_MACRXCOUNT);
        case UMSG_OPS_MP_PHYRXCOUNT                         :
            return c2s(UMSG_OPS_MP_PHYRXCOUNT);
        case UMSG_OPS_MP_RESETCOUNT                         :
            return c2s(UMSG_OPS_MP_RESETCOUNT);
        case UMSG_OPS_MP_PHYTXOK                            :
            return c2s(UMSG_OPS_MP_PHYTXOK);
        case UMSG_OPS_MP_CTXRATE                            :
            return c2s(UMSG_OPS_MP_CTXRATE);
        case FUNC_CMD_RESERVED                              :
            return c2s(FUNC_CMD_RESERVED);
        case UMSG_OPS_MP_USER_INFO                          :
            return c2s(UMSG_OPS_MP_USER_INFO);
        case UMSG_OPS_CMD_TEST                              :
            return c2s(UMSG_OPS_CMD_TEST);
        case UMSG_OPS_RESET_CHIP                            :
            return c2s(UMSG_OPS_RESET_CHIP);
        case M0_OPS_MP_BB_REG_GET                           :
            return c2s(M0_OPS_MP_BB_REG_GET);
        case M0_OPS_MP_BB_REG_SET                           :
            return c2s(M0_OPS_MP_BB_REG_SET);
        case M0_OPS_MP_RF_REG_GET                           :
            return c2s(M0_OPS_MP_RF_REG_GET);
        case M0_OPS_MP_RF_REG_SET                           :
            return c2s(M0_OPS_MP_RF_REG_SET);
        case M0_OPS_HAL_DBGLOG_CONFIG                       :
            return c2s(M0_OPS_HAL_DBGLOG_CONFIG);
        default:
            LOG_E("Unknown cmd:0x%x",cmd);
            return "Unknown cmd";
    }
}

int wf_io_write_cmd_special(nic_info_st *nic_info, wf_u32 func_code, wf_u32 *recv,  int len, int offs)
{
    int ret = 0;
    wf_u32 mailbox_reg_addr = MAILBOX_ARG_START;
    int revclen;
    int i;

    //LOG_I("[%s] func_code:0x%08x",__func__, func_code);
    LOG_I("[%s] func_code:0x%08x,%s",__func__, func_code,cmd_to_str(func_code));
    if (len > MAILBOX_MAX_RDLEN)
    {
        LOG_E("discard func %d because rd len = %d too big\r\n", func_code,len);
        return WF_RETURN_FAIL;
    }

    revclen = len;

    ret = wf_io_write32(nic_info, MAILBOX_REG_START, func_code);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }

    ret = wf_io_write32(nic_info, mailbox_reg_addr, len);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }

    mailbox_reg_addr += MAILBOX_WORD_LEN;
    ret = wf_io_write32(nic_info, mailbox_reg_addr, offs);

    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
        return ret;
    }
    mailbox_reg_addr += MAILBOX_WORD_LEN;

    ret = wf_mcu_cmd_get_status(nic_info);
    if(WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s,%d] wf_mcu_cmd_get_status failed",__func__,__LINE__);
        return ret;
    }

    mailbox_reg_addr = MAILBOX_ARG_START;

    while (len--)
    {

        *recv++ = wf_io_read32(nic_info, mailbox_reg_addr,NULL);
        mailbox_reg_addr += 4;
    }


    LOG_I("func_code:0x%x, sendlen:%d, recvlen:%d\n",func_code, 0, revclen );
    return ret;
}

int wf_io_write_cmd_by_mailbox(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    int ret;
    wf_u32 mailbox_reg_addr = MAILBOX_ARG_START;
    wf_os_api_sema_t *pCmdSema = &nic_info->cmd_sema;
#ifdef CONFIG_CONCURRENT_MODE
	nic_info_st *pvir_nic = nic_info->vir_nic;
	wf_os_api_sema_t *pvir_CmdSema = &pvir_nic->cmd_sema;
#endif

    if (nic_info->cmd_lock_use)
    {
        //LOG_I(">>>>> func_code:0x%08x,%s",cmd,cmd_to_str(cmd));
        wf_os_api_sema_wait(pCmdSema);
#ifdef CONFIG_CONCURRENT_MODE
		if(pvir_nic->cmd_lock_use)
		{
			wf_os_api_sema_wait(pvir_CmdSema);
		}
#endif
    }
    #if 0
    {
        int i=0;
        if( 0x28 != cmd && 0x22 != cmd && 0x21 != cmd && 0x20 != cmd && 0x6b != cmd)
        {
            LOG_I("[%s] func_code:0x%08x,%s",__func__,cmd,cmd_to_str(cmd));
            for(i=0;i<send_len;i++)
            {
                LOG_I("[%d]:0x%x",i,send_buf[i]);
            }
        }
    }
    #endif
    ret = wf_io_write32(nic_info,MAILBOX_REG_START,cmd);
   	if(WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s,%d] wf_io_write32 failed cmd:0x%x",__func__,__LINE__,cmd);
		goto exit;

	}
    ret = wf_io_write32(nic_info, mailbox_reg_addr, send_len);
	if(WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
		goto exit;
	}

    mailbox_reg_addr += MAILBOX_WORD_LEN;
    ret = wf_io_write32(nic_info, mailbox_reg_addr, recv_len);
	if(WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
		goto exit;
	}

    mailbox_reg_addr += MAILBOX_WORD_LEN;

    while ((send_len--) && send_buf)
    {
        ret = wf_io_write32(nic_info, mailbox_reg_addr, *send_buf++);
		if(WF_RETURN_FAIL == ret)
		{
			LOG_E("[%s,%d] wf_io_write32 failed",__func__,__LINE__);
			goto exit;
		}
        mailbox_reg_addr += MAILBOX_WORD_LEN;
    }

    ret = wf_mcu_cmd_get_status(nic_info);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("mcu_cmd_get_status failed, check mcu feedback");
		goto exit;
    }

    mailbox_reg_addr = MAILBOX_ARG_START;
    if (ret == 0)
    {
        while ((recv_len--) && recv_buf)
        {
            *recv_buf++ = wf_io_read32(nic_info, mailbox_reg_addr,NULL);
            mailbox_reg_addr += MAILBOX_WORD_LEN;
        }
    }

	ret = WF_RETURN_OK;

exit:
    if (nic_info->cmd_lock_use)
    {
        //LOG_I("<<<<<< func_code:0x%08x,%s",cmd,cmd_to_str(cmd));
        wf_os_api_sema_post(pCmdSema);
#ifdef CONFIG_CONCURRENT_MODE
		if(pvir_nic->cmd_lock_use)
		{
			wf_os_api_sema_post(pvir_CmdSema);
		}

#endif
	}

    return ret;
}


int wf_io_write_cmd_by_txd(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
    WF_ASSERT(nic_info != NULL);

    if (nic_info->nic_write_cmd == NULL)
    {
        LOG_E("nic_write_cmd is not register, please check!!");
        return -1;
    }

	return nic_info->nic_write_cmd(nic_info->hif_node, cmd, send_buf, send_len, recv_buf, recv_len);
}


