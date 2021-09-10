#ifndef __SDIO_H__
#define __SDIO_H__


#ifdef CONFIG_RICHV200
#define MIN_RXD_SIZE      16
#else
#define MIN_RXD_SIZE      24
#endif

#define SDIO_BASE                   0x10250000

#define WL_REG_HIMR                 0x9004
#define WL_REG_HISR                 0x9008
#define WL_REG_SZ_RX_REQ            0x9010
#define WL_REG_HCTL                 0x903A
#define WL_REG_PUB_FREEPG           0x901C
#define WL_REG_HIG_FREEPG           0x9020
#define WL_REG_MID_FREEPG           0x9024
#define WL_REG_LOW_FREEPG           0x9028
#define WL_REG_EXT_FREEPG           0x902C

#define WL_REG_AC0_FREEPG           0x9014

#define WL_REG_TXCTL                0x9000
#define WL_REG_AC_OQT_FREEPG        0x9030

#define WL_LPS_CTL1                 0x9058
#define WL_LPS_CTL2                 0x905C

#define WL_REG_QUE_PRI_SEL          0x906C

#define TX_SIDEROAD_FLOW_CTRL       0x9096

#define SDIO_RD                     1
#define SDIO_WD                     0

#define SDIO_FIFO_TYP_NUM 6

#define TX_FLOW_WF_DATA            3
#define TX_RESERVED_PG_NUM         65

enum WF_SDIO_OPERATION_FLAG
{
    WF_SDIO_NORMAL_FLAG     = 0,
    WF_SDIO_TRX_QUEUE_FLAG  = 1,
};

typedef struct hif_sdio_management_
{
    OS_U8 sdio_id;
    sdio_func_t *func;
    OS_U32 sdio_himr;
    OS_U32 sdio_hisr;
    OS_U8 sdio_hisr_en;
    OS_U32 SysIntrMask;
    int sdio_irq_start;
    OS_U32 block_transfer_len;
    OS_U16 SdioRxFIFOSize;
    OS_U8 SdioRxFIFOCnt;
    OS_U8 SdioTxOQTFreeSpace;
    OS_U8 SdioTxFIFOFreePage[SDIO_FIFO_TYP_NUM];
    OS_U8 SdioTxOQTMaxFreeSpace;
    OS_U8 tx_no_low_queue;
    OS_U8 tx_fifo_ppg_num;
    OS_U8 tx_fifo_lpg_num;
    OS_U8 tx_fifo_epg_num;
    OS_U8 tx_fifo_mpg_num;
    OS_U8 tx_fifo_hpg_num;
    OS_U16 tx_fifo_all_use_num;
    OS_U8 last_tx_ppg_num;
    osWork_t irq_work;
    osWorkqueue_t *irq_wq;
    OS_U64 irq_cnt;
    int int_flag;
    OS_BOOL isr_triggered;
} hif_sdio_t;

int sdio_init(void);
int sdio_exit(void);




int wf_sdioh_interrupt_disable(void *hif_info);
int wf_sdioh_interrupt_enable(void *hif_info);
int wf_sdioh_config(void *hif_info);



#endif
