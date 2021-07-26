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

#define WF_N_BYTE_ALIGMENT(value, alignment)  ((alignment == 1) ? (value) : (((value + alignment - 1) / alignment) * alignment))

#define WF_U32_BIT_LEN_MASK(bit_len)    	((wf_u32)(0xFFFFFFFF >> (32 - (bit_len))))
#define WF_U16_BIT_LEN_MASK(bit_len)    	((wf_u16)(0xFFFF >> (16 - (bit_len))))
#define WF_U8_BIT_LEN_MASK(bit_len)     	((wf_u8)(0xFF >> (8 - (bit_len))))

#define WF_U32_BIT_OFFSET_LEN_MASK(bit_offset, bit_len) 	((wf_u32)(WF_U32_BIT_LEN_MASK(bit_len) << (bit_offset)))
#define WF_U16_BIT_OFFSET_LEN_MASK(bit_offset, bit_len) 	((wf_u16)(WF_U16_BIT_LEN_MASK(bit_len) << (bit_offset)))
#define WF_U8_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)  	((wf_u8)(WF_U8_BIT_LEN_MASK(bit_len) << (bit_offset)))

#define wf_le_u32_read(ptr)           		wf_le32_to_cpu(*((wf_u32 *)(ptr)))
#define wf_le_u16_read(ptr)           		wf_le16_to_cpu(*((wf_u16 *)(ptr)))
#define wf_le_u8_read(ptr)            		(*((wf_u8 *)(ptr)))

#define wf_be_u32_read(ptr)           		wf_be32_to_cpu(*((wf_u32 *)(ptr)))
#define wf_be_u16_read(ptr)           		wf_be16_to_cpu(*((wf_u16 *)(ptr)))
#define wf_be_u8_read(ptr)            		(*((wf_u8 *)(ptr)))

#define wf_le_u32_write(ptr, val)    		(*((wf_u32 *)(ptr))) = wf_cpu_to_le32(val)
#define wf_le_u16_write(ptr, val)    		(*((wf_u16 *)(ptr))) = wf_cpu_to_le16(val)
#define wf_le_u8_write(ptr, val)     		(*((wf_u8 *)(ptr))) = ((wf_u8)(val))

#define wf_be_u32_write(ptr, val)    		(*((wf_u32 *)(ptr))) = wf_cpu_to_be32(val)
#define wf_be_u16_write(ptr, val)    		(*((wf_u16 *)(ptr))) = wf_cpu_to_be16(val)
#define wf_be_u8_write(ptr, val)     		(*((wf_u8 *)(ptr))) = ((wf_u8)(val))

#define wf_le_u32_to_host_u32(pstart)  		(wf_le32_to_cpu(*((wf_u32 *)(pstart))))
#define wf_le_u16_to_host_u16(pstart)  		(wf_le16_to_cpu(*((wf_u16 *)(pstart))))
#define wf_le_u8_to_host_u8(pstart)    		((*((wf_u8 *)(pstart))))

#define wf_be_u32_to_host_u32(pstart)  		(wf_be32_to_cpu(*((wf_u32 *)(pstart))))
#define wf_be_u16_to_host_u16(pstart)  		(wf_be16_to_cpu(*((wf_u16 *)(pstart))))
#define wf_be_u8_to_host_u8(pstart)    		((*((wf_u8 *)(pstart))))

#define wf_le_bits_to_u32(pstart, bit_offset, bit_len) \
    ((wf_le_u32_to_host_u32(pstart) >> (bit_offset)) & WF_U32_BIT_LEN_MASK(bit_len))

#define wf_le_bits_to_u16(pstart, bit_offset, bit_len) \
    ((wf_le_u16_to_host_u16(pstart) >> (bit_offset)) & WF_U16_BIT_LEN_MASK(bit_len))

#define wf_le_bits_to_u8(pstart, bit_offset, bit_len) \
    ((wf_le_u8_to_host_u8(pstart) >> (bit_offset)) & WF_U8_BIT_LEN_MASK(bit_len))

#define wf_be_bits_to_u32(pstart, bit_offset, bit_len) \
    ((wf_be_u32_to_host_u32(pstart) >> (bit_offset)) & WF_U32_BIT_LEN_MASK(bit_len))

#define wf_be_bits_to_u16(pstart, bit_offset, bit_len) \
    ((wf_be_u16_to_host_u16(pstart) >> (bit_offset)) & WF_U16_BIT_LEN_MASK(bit_len))

#define wf_be_bits_to_u8(pstart, bit_offset, bit_len) \
    ((wf_be_u8_to_host_u8(pstart) >> (bit_offset)) & WF_U8_BIT_LEN_MASK(bit_len))

#define wf_le_bits_clear_to_u32(pstart, bit_offset, bit_len) \
    (wf_le_u32_to_host_u32(pstart) & (~WF_U32_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)))

#define wf_le_bits_clear_to_u16(pstart, bit_offset, bit_len) \
    (wf_le_u16_to_host_u16(pstart) & (~WF_U16_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)))

#define wf_le_bits_clear_to_u8(pstart, bit_offset, bit_len) \
    (wf_le_u8_to_host_u8(pstart) & ((wf_u8)(~WF_U8_BIT_OFFSET_LEN_MASK(bit_offset, bit_len))))

#define wf_be_bits_clear_to_u32(pstart, bit_offset, bit_len) \
    (wf_be_u32_to_host_u32(pstart) & (~WF_U32_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)))

#define wf_be_bits_clear_to_u16(pstart, bit_offset, bit_len) \
    (wf_be_u16_to_host_u16(pstart) & (~WF_U16_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)))

#define wf_be_bits_clear_to_u8(pstart, bit_offset, bit_len) \
    (wf_be_u8_to_host_u8(pstart) & (~WF_U8_BIT_OFFSET_LEN_MASK(bit_offset, bit_len)))

#define wf_set_bits_to_le_u32(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 32) \
            wf_le_u32_write(pstart, value); \
        else { \
            wf_le_u32_write(pstart, \
                wf_le_bits_clear_to_u32(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u32)value) & WF_U32_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

#define wf_set_bits_to_le_u16(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 16) \
            wf_le_u16_write(pstart, value); \
        else { \
            wf_le_u16_write(pstart, \
                wf_le_bits_clear_to_u16(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u16)value) & WF_U16_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

#define wf_set_bits_to_le_u8(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 8) \
            wf_le_u8_write(pstart, value); \
        else { \
            wf_le_u8_write(pstart, \
                wf_le_bits_clear_to_u8(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u8)value) & WF_U8_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

#define wf_set_bits_to_be_u32(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 32) \
            wf_be_u32_write(pstart, value); \
        else { \
            wf_be_u32_write(pstart, \
                wf_be_bits_clear_to_u32(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u32)value) & WF_U32_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

#define wf_set_bits_to_be_u16(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 16) \
            wf_be_u16_write(pstart, value); \
        else { \
            wf_be_u16_write(pstart, \
                wf_be_bits_clear_to_u16(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u16)value) & WF_U16_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

#define wf_set_bits_to_be_u8(pstart, bit_offset, bit_len, value) \
    do { \
        if (bit_offset == 0 && bit_len == 8) \
            wf_be_u8_write(pstart, value); \
        else { \
            wf_be_u8_write(pstart, \
                wf_be_bits_clear_to_u8(pstart, bit_offset, bit_len) \
                | \
                ((((wf_u8)value) & WF_U8_BIT_LEN_MASK(bit_len)) << (bit_offset)) \
            ); \
        } \
    } while (0)

/* read reg */
wf_u8  wf_io_read8(const nic_info_st *nic_info, wf_u32 addr,int *err);
wf_u16 wf_io_read16(const nic_info_st *nic_info, wf_u32 addr,int *err);
wf_u32 wf_io_read32(const nic_info_st *nic_info, wf_u32 addr,int *err);

/* write reg */
int wf_io_write8(const nic_info_st *nic_info, wf_u32 addr, wf_u8 value);
int wf_io_write16(const nic_info_st *nic_info, wf_u32 addr, wf_u16 value);
int wf_io_write32(const nic_info_st *nic_info, wf_u32 addr, wf_u32 value);

/* send special cmd */

/* send cmd */
int wf_io_write_cmd_by_mailbox(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);
int wf_io_write_cmd_by_mailbox_try(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);
int wf_io_write_cmd_by_txd(nic_info_st *nic_info, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len);

/* send data */
int wf_io_write_data(const nic_info_st *nic_info, wf_u8 agg_num, char *pbuf, wf_u32 len, wf_u32 addr,
							int (*callback_func)(void*tx_info, void *param), void *tx_info, void *param);
int wf_io_write_data_queue_check(const nic_info_st *nic_info);
int wf_io_tx_xmit_wake(const nic_info_st *nic_info);

/* send firmware */
int wf_io_write_firmware(const nic_info_st *nic_info, wf_u8 which,  wf_u8 *firmware, wf_u32 len);


#endif

