
#ifndef __HIF_H__
#define __HIF_H__

#include "hif_queue.h"
#if defined(CONFIG_USB_FLAG)
#include "usb.h"
#elif defined(CONFIG_SDIO_FLAG)
#include "sdio.h"
#else
#include "usb.h"
#include "sdio.h"
#endif

typedef enum hif_enum_
{
    HIF_USB     = 1,
    HIF_SDIO    = 2,
} hif_enum;

#define MAX_NIC 5

struct hif_node;
/*node operation*/
typedef struct
{
    int (*hif_write)(struct hif_node *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*hif_read)(struct hif_node *node, unsigned char flag, unsigned int addr, char *data, int datalen);
    int (*hif_show)(struct hif_node *node);
    int (*hif_init)(struct hif_node *node);
    int (*hif_exit)(struct hif_node *node);
    int (*hif_insert_netdev)(struct hif_node *node);
    int (*hif_remove_netdev)(struct hif_node *node);
    int (*hif_tx_queue_insert)(void *hif_info,OS_U8 agg_num, char *buff, OS_U32 buff_len, OS_U32 addr,
                               int (*tx_callback_func)(void*tx_info, void *param), void *tx_info, void *param);
    int (*hif_tx_queue_empty)(void *hif_info);
} hif_node_ops_t;

typedef struct hif_node
{
    OS_U8 node_id;
    os_list_t next;
    hif_enum hif_type;//can be HIF_USB, HIF_SDIO
    union
    {
#if defined(CONFIG_SDIO_FLAG)
        hif_sdio_t      sdio;
#elif defined(CONFIG_USB_FLAG)
        hif_usb_mngt    usb;
#else
        hif_sdio_t      sdio;
        hif_usb_mngt    usb;
#endif
    } u;

    /*common part */
    hif_node_ops_t *ops;
    nic_info_st *nic_info[MAX_NIC];
    data_queue_mngt_t trx_pipe;
    OS_BOOL hif_tr_ctrl;
    OS_BOOL dev_removed;
    OS_U32 nic_number;

    void *odm;
    void *ars;
    /*proc debug system*/
    void *proc_info;

#ifdef CONFIG_RICHV200
    /* usb or sdio rx handle info */
#define HIF_BULK_MSG_TIMEOUT    5000
    wf_lock_t reg_mutex;
    osCompletion_t reg_completion;
    osCompletion_t fw_completion;
    osCompletion_t cmd_completion;
    OS_U8 *reg_buffer;
    OS_U8 *fw_buffer;
    OS_U8 *cmd_snd_buffer;
    OS_U8 *cmd_rcv_buffer;
    OS_U16 reg_size;
    OS_U16 fw_size;
    OS_U16 cmd_size;
    OS_U8 cmd_completion_flag;
    OS_U8 cmd_remove;
    OS_BOOL bulk_enable;
#endif

    wf_lock_t mlme_hw_access_lock;
    wf_lock_t mcu_hw_access_lock;
} hif_node_t;

typedef struct
{
    char *mem;
    unsigned int mem_sz; //mem size
    char is_free;
    os_list_t mm_next;
} hif_mm_t;

typedef struct
{
    OS_U8 usb_num; //usb number in hif_usb_sdio
    OS_U8 sdio_num; //sdio number in hif_usb_sdio
    OS_U8 hif_num; //all usb and sido number node in hif_usb_sdio, so hif_num = usb_num+sdio_num

    wf_u64 id_bitmap;
    wf_u64 usb_id_bitmap;
    wf_u64 sdio_id_bitmap;

    OS_U32  cfg_size;
    char   *cfg_content;

    OS_U8  fw_rom_type;
    OS_U32 fw0_size;
    OS_U32 fw1_size;
    char  *fw0;
    char  *fw1;

    os_list_t hif_usb_sdio;
    wf_lock_t lock_mutex;

    /*common part*/
    os_list_t mm_list;

} hif_mngent_t;

/*hm: hif management*/
typedef enum
{
    HM_ADD = 1,//usb_num,sdio_num and hif_num do add operation
    HM_SUB = 2 //usb_num,sdio_num and hif_num do subtraction operation
} HM_OPERATION;

OS_BOOL hm_get_mod_removed(void);

os_list_t *hm_get_list_head(void);
OS_U8 hm_new_id(int *err);
OS_U8 hm_new_usb_id(int *err);
OS_U8 hm_new_sdio_id(int *err);
OS_U8 hm_del_id(OS_U8 id);
OS_U8 hm_del_usb_id(OS_U8 id);
OS_U8 hm_del_sdio_id(OS_U8 id);
wf_lock_t *hm_get_lock(void);

OS_U8 hm_set_num(HM_OPERATION op);
OS_U8 hm_set_usb_num(HM_OPERATION op);
OS_U8 hm_set_sdio_num(HM_OPERATION op);
hif_mngent_t * hif_mngent_get(void);


void hif_node_register(hif_node_t **node, hif_enum type, hif_node_ops_t *ops);
void hif_node_unregister(hif_node_t *node);


int  hif_dev_insert(hif_node_t *hif_info);
void hif_dev_removed(hif_node_t *hif_info);


int hif_io_write(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);
int hif_io_read(void *node, unsigned char flag, unsigned int addr, char *data, int datalen);
int hif_io_enable(void *node);
int hif_io_disable(void *node);
unsigned char hif_io_read8(void *node, unsigned int addr,int *err);
unsigned long hif_io_read16(void *node, unsigned int addr,int *err);
unsigned int hif_io_read32(void *node, unsigned int addr,int *err);
int hif_io_write8(void *node, unsigned int addr, unsigned char value);
int hif_io_write16(void *node, unsigned int addr, unsigned long value);
int hif_io_write32(void *node, unsigned int addr, unsigned int value);




int wf_hif_queue_enable(hif_node_t *hif_node);
int wf_hif_queue_disable(hif_node_t *hif_node);

#ifdef CONFIG_RICHV200
int wf_hif_bulk_enable(hif_node_t *hif_node);
int wf_hif_bulk_disable(hif_node_t *hif_node);

void wf_hif_bulk_reg_init(hif_node_t *hif_node);
int wf_hif_bulk_reg_wait(hif_node_t *hif_node, OS_U32 timeout);
void wf_hif_bulk_reg_post(hif_node_t *hif_node, OS_U8 *buff, OS_U16 len);
void wf_hif_bulk_fw_init(hif_node_t *hif_node);
int wf_hif_bulk_fw_wait(hif_node_t *hif_node, OS_U32 timeout);
void wf_hif_bulk_fw_post(hif_node_t *hif_node, OS_U8 *buff, OS_U16 len);
void wf_hif_bulk_cmd_init(hif_node_t *hif_node);
int wf_hif_bulk_cmd_wait(hif_node_t *hif_node, OS_U32 timeout);
void wf_hif_bulk_cmd_post(hif_node_t *hif_node, OS_U8 *buff, OS_U16 len);
void wf_hif_bulk_cmd_post_exit(hif_node_t *hif_node);

int wf_hif_bulk_rxd_type(OS_U8 *prx_desc);

int hif_write_firmware(void *node, OS_U8 which,  OS_U8 *firmware, OS_U32 len);
int hif_write_cmd(void *node, OS_U32 cmd, OS_U32 *send_buf, OS_U32 send_len, OS_U32 *recv_buf, OS_U32 recv_len);

#endif

int hif_tasklet_rx_handle(hif_node_t *hif_info);
int hif_tasklet_tx_handle(hif_node_t *hif_info);

OS_RET hif_init(void);
void hif_exit(void);

#endif

