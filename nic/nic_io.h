#ifndef __NIC_IO_H__
#define __NIC_IO_H__

typedef enum
{
    TYPE_CMD     = 0,
    TYPE_FW      = 1,
    TYPE_REG     = 2,
    TYPE_DATA    = 3
} tx_rx_desc_type_e;

typedef enum
{
    FIRMWARE_M0    = 0,
    FIRMWARE_DSP   = 1
} firmware_type_e;


/* read reg */
wf_u8  wf_io_read8(const nic_info_st *nic_info, wf_u32 addr,int *err);
wf_u16 wf_io_read16(const nic_info_st *nic_info, wf_u32 addr,int *err);
wf_u32 wf_io_read32(const nic_info_st *nic_info, wf_u32 addr,int *err);

/* write reg */
int wf_io_write8(const nic_info_st *nic_info, wf_u32 addr, wf_u8 value);
int wf_io_write16(const nic_info_st *nic_info, wf_u32 addr, wf_u16 value);
int wf_io_write32(const nic_info_st *nic_info, wf_u32 addr, wf_u32 value);

/* send special cmd */
int wf_io_write_cmd_special(nic_info_st *nic_info, wf_u32 func_code, wf_u32 *recv,  int len, int offs);

/* send cmd */
int wf_io_write_cmd_by_mailbox(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);
int wf_io_write_cmd_by_txd(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);

/* send data */
int wf_io_write_data(const nic_info_st *nic_info, wf_u8 agg_num, char *pbuf, wf_u32 len, wf_u32 addr,
							int (*callback_func)(void*tx_info, void *param), void *tx_info, void *param);
int wf_io_write_data_queue_check(const nic_info_st *nic_info);


/* send firmware */
int wf_io_write_firmware(const nic_info_st *nic_info, wf_u8 which,  wf_u8 *firmware, wf_u32 len);


#endif

