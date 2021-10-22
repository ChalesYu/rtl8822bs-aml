/*
 * hif_io.c
 *
 * used for hif io read or write.
 *
 * Author: hichard
 *
 * Copyright (c) 2020 SmartChip Integrated Circuits(SuZhou ZhongKe) Co.,Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */
#include "wf_os_api.h"
#include "hif.h"
#include "wf_debug.h"

int hif_io_write(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen)
{
  struct hif_node_ *hif_node = node;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  return hif_node->ops->hif_write(node, flag, addr,data,datalen);
}


int hif_io_read(void *node,  unsigned char flag, unsigned int addr, char *data, int datalen)
{
  struct hif_node_ *hif_node = node;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_read != NULL);
  
  return hif_node->ops->hif_read(node, flag, addr,data,datalen);
}

unsigned char hif_io_read8(void *node, unsigned int addr, int *err)
{
  struct hif_node_ *hif_node = node;
  int ret = 0;
  wf_u8 value;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 1);
  if(err)
  {
    *err = ret;
  }
  return value;
}

unsigned long hif_io_read16(void *node, unsigned int addr,int *err)
{
  struct hif_node_ *hif_node = node;
  wf_u16 value;
  int ret = 0;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 2);
  if(err)
  {
    *err = ret;
  }
  return value;
}

unsigned int hif_io_read32(void *node, unsigned int addr, int *err)
{
  struct hif_node_ *hif_node = node;
  wf_u32 value;
  int ret = 0;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  ret = hif_node->ops->hif_read(node, 0, addr, (char *)&value, 4);
  if(err)
  {
    *err = ret;
  }
  
  return value;
}

int hif_io_write8(void *node, unsigned int addr, unsigned char value)
{
  struct hif_node_ *hif_node = node;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 1);
}

int hif_io_write16(void *node, unsigned int addr, unsigned long value)
{
  struct hif_node_ *hif_node = node;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 2);
}

int hif_io_write32(void *node, unsigned int addr, unsigned int value)
{
  struct hif_node_ *hif_node = node;
  
  WF_ASSERT(hif_node != NULL);
  WF_ASSERT(hif_node->ops != NULL);
  WF_ASSERT(hif_node->ops->hif_write != NULL);
  
  return hif_node->ops->hif_write(node, 0, addr, (char *)&value, 4);
}


#ifdef MCU_CMD_TXD

void txdesc_new_chksum(wf_u8 *ptx_desc)
{
  wf_u16 *usPtr = (wf_u16 *) ptx_desc;
  wf_u32 index;
  wf_u16 checksum = 0;
  
  for (index = 0; index < 9; index++)
    checksum ^= le16_to_cpu(*(usPtr + index));
  
  wf_set_bits_to_le_u32(ptx_desc + 16, 16, 16, checksum);
}

static wf_inline wf_u32 wf_io_bulk_read_one_reg(void *hif, struct xmit_buf *pxmit_buf, wf_u16 addr)
{
  wf_u8	u8Value;
  wf_u16 u16Value;
  wf_u32 u32Value;
  wf_u8 *ptx_desc;
  wf_u8 *prx_desc;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  ptx_desc = pxmit_buf->pbuf;
  wf_memset(pxmit_buf->pbuf, 0, 28 + 20);

  wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_REG);
  wf_set_bits_to_le_u32(ptx_desc, 14, 1, 0);
  wf_set_bits_to_le_u32(ptx_desc, 15, 3, 1);
  wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
  wf_set_bits_to_le_u32(ptx_desc + 4, 0, 16, addr);
  txdesc_new_chksum(ptx_desc);
  pxmit_buf->pkt_len = 20;
  
  wf_lock_mutex_lock(&hif_node->reg_mutex);
  wf_hif_bulk_reg_init(hif_node);
  if(nic_info->nic_tx_queue_insert(nic_info->hif_node, 1, ptx_desc, pxmit_buf->pkt_len,
                                   CMD_QUEUE_INX, NULL, NULL, NULL) != 0) {
                                     LOG_E("bulk access reg error by send addr");
                                     wf_lock_mutex_unlock(&hif_node->reg_mutex);
                                     return -1;
                                   }
  
  if(wf_hif_bulk_reg_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0) {
    LOG_E("bulk access reg read timeout");
    wf_lock_mutex_unlock(&hif_node->reg_mutex);
    return -1;
  }
  
  prx_desc = pxmit_buf->pbuf;
  pxmit_buf->pkt_len = 16 + 4 + 4;
  if(hif_node->reg_size != pxmit_buf->pkt_len) {
    LOG_E("bulk access reg read length error");
    wf_lock_mutex_unlock(&hif_node->reg_mutex);
    return -1;
  }
  wf_memcpy(prx_desc, hif_node->reg_buffer, hif_node->reg_size);
  
  wf_lock_mutex_unlock(&hif_node->reg_mutex);
  
  u8Value = wf_le_u8_read(prx_desc);
  if((u8Value & 0x03) != TYPE_REG) {
    LOG_E("bulk access reg read error");
    return -1;
  }
  u16Value = wf_le_u16_read(prx_desc + 4);
  u16Value &= 0x3FFF;
  if(u16Value != (4 + 4)) {
    LOG_E("bulk access reg read length error, value is %d", u16Value);
    return -1; 
  }
  
  u8Value = wf_le_u8_read(prx_desc + 20);
  if(u8Value != 0x5a) {
    LOG_E("bulk access reg status error");
    if(u8Value == 0x01) {
      LOG_E("bulk access reg txd checksum error");
    } else if(u8Value ==0xFF) {
      LOG_E("bulk access reg timeout");
    }
    return -1;	
  }
  
  u32Value = wf_le_u32_read(prx_desc + 16);
  
  return u32Value;
}

static wf_inline int wf_io_bulk_write_one_reg(void *hif, struct xmit_buf *pxmit_buf, wf_u16 addr, wf_u32 value)
{
  wf_u8	u8Value;
  wf_u16 u16Value;
  wf_u8 *ptx_desc;
  wf_u8 *prx_desc;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  
  wf_memset(pxmit_buf->pbuf, 0, 28 + 20);
  ptx_desc = pxmit_buf->pbuf;
  wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_REG);
  wf_set_bits_to_le_u32(ptx_desc, 14, 1, 1);
  wf_set_bits_to_le_u32(ptx_desc, 15, 3, 1);
  wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
  wf_set_bits_to_le_u32(ptx_desc + 4, 0, 16, addr);
  
  txdesc_new_chksum(ptx_desc);
  
  wf_set_bits_to_le_u32(ptx_desc + 20, 0, 32, value);
  
  pxmit_buf->pkt_len = TXDESC_OFFSET_NEW + 4;
  
  wf_lock_mutex_lock(&hif_node->reg_mutex);
  wf_hif_bulk_reg_init(hif_node);
  if(nic_info->nic_tx_queue_insert(nic_info->hif_node, 1, ptx_desc, pxmit_buf->pkt_len,
                                   CMD_QUEUE_INX, NULL, NULL, NULL) != 0) {
                                     LOG_E("bulk access reg error by send addr");
                                     wf_lock_mutex_unlock(&hif_node->reg_mutex);
                                     return -1;
                                   }
  
  if(wf_hif_bulk_reg_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0) {
    LOG_E("bulk access reg read timeout");
    wf_lock_mutex_unlock(&hif_node->reg_mutex);
    return -1;
  }
  
  prx_desc = pxmit_buf->pbuf;
  pxmit_buf->pkt_len = 16 + 4;
  if(hif_node->reg_size != pxmit_buf->pkt_len) {
    LOG_E("bulk access reg read length error");
    wf_lock_mutex_unlock(&hif_node->reg_mutex);
    return -1;
  }
  wf_memcpy(prx_desc, hif_node->reg_buffer, hif_node->reg_size);
  
  wf_lock_mutex_unlock(&hif_node->reg_mutex);
  
  u8Value = wf_le_u8_read(prx_desc);
  if((u8Value & 0x03) != TYPE_REG) {
    LOG_E("bulk access reg read error");
    return -1;
  }
  u16Value = wf_le_u16_read(prx_desc + 4);
  u16Value &= 0x3FFF;
  if(u16Value != 4) {
    LOG_E("bulk access reg read length error, value is %d", u16Value);
    return -1; 
  }
  
  u8Value = wf_le_u8_read(prx_desc + 16);
  if(u8Value != 0x5a) {
    LOG_E("bulk access reg status error");
    if(u8Value == 0x01) {
      LOG_E("bulk access reg txd checksum error");
    } else if(u8Value ==0xFF) {
      LOG_E("bulk access reg timeout");
    }
    return -1;	
  }
  
  return sizeof(value);
} 

wf_u8 wf_io_bulk_read8(void *hif, wf_u16 addr)
{
  
  wf_u32 u32Value;
  wf_u16 align_addr;
  wf_u16 offset_addr;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  align_addr = addr & 0xFFFC;
  offset_addr = addr & 0x03;
  u32Value = wf_io_bulk_read_one_reg(hif, pxmit_buf, align_addr);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return ((u32Value >> (offset_addr * 8)) & 0xFF);
}

wf_u16 wf_io_bulk_read16(void *hif, wf_u16 addr)
{
  wf_u32 u32Value;
  wf_u16 align_addr;
  wf_u16 offset_addr;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if(addr & 0x0001) {
    return -1;
  }
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  align_addr = addr & 0xFFFC;
  offset_addr = addr & 0x02;
  u32Value = wf_io_bulk_read_one_reg(hif, pxmit_buf, align_addr);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return ((u32Value >> (offset_addr * 8)) & 0xFFFF);
}


wf_u32 wf_io_bulk_read32(void *hif, wf_u16 addr)
{
  wf_u32 u32Value;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if(addr & 0x0003) {
    return -1;
  }
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  u32Value = wf_io_bulk_read_one_reg(hif, pxmit_buf, addr);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return u32Value;
}

int wf_io_bulk_write8(void *hif, wf_u16 addr, wf_u8 value)
{
  wf_u32 u32Value;
  wf_u16 align_addr;
  wf_u16 offset_addr;
  int ret;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  align_addr = addr & 0xFFFC;
  offset_addr = addr & 0x03;
  u32Value = wf_io_bulk_read_one_reg(hif, pxmit_buf, align_addr);
  u32Value &= ~((wf_u32)(0xFFUL << (offset_addr * 8)));
  u32Value |= ((wf_u32)((wf_u32)value << (offset_addr * 8)));
  
  ret = wf_io_bulk_write_one_reg(hif, pxmit_buf, align_addr, u32Value);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return ret;
}

int wf_io_bulk_write16(void *hif, wf_u16 addr, wf_u16 value)
{
  wf_u32 u32Value;
  wf_u16 align_addr;
  wf_u16 offset_addr;
  int ret;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if(addr & 0x0001) {
    return -1;
  }
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  align_addr = addr & 0xFFFC;
  offset_addr = addr & 0x02;
  u32Value = wf_io_bulk_read_one_reg(hif, pxmit_buf, align_addr);
  u32Value &= ~((wf_u32)(0xFFFFUL << (offset_addr * 8)));
  u32Value |= ((wf_u32)((wf_u32)value << (offset_addr * 8)));
  
  ret = wf_io_bulk_write_one_reg(hif, pxmit_buf, align_addr, u32Value);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return ret;
}

int wf_io_bulk_write32(void *hif, wf_u16 addr, wf_u32 value)
{
  int ret;
  struct xmit_buf *pxmit_buf;
  hif_node_st *hif_node = hif;
  nic_info_st *nic_info = hif_node->nic_info[0];
  WF_ASSERT(nic_info != NULL); 
  
  if(addr & 0x0003) {
    return -1;
  }
  
  if (nic_info->func_check_flag != 0xAA55BB66)
  {
    LOG_E("func_check_flag is not 0xAA55BB66, check the func ok?");
    LOG_E("nic_info:%p  func_check_flag:%x  nic_write:%p	nic_read:%p x",
          nic_info,nic_info->func_check_flag,nic_info->nic_write,nic_info->nic_read);
    return -1;
  }
  
  pxmit_buf = wf_xmit_extbuf_new(nic_info->tx_info);
  if(pxmit_buf == NULL){
    return -1;
  }
  
  ret = wf_io_bulk_write_one_reg(hif, pxmit_buf, addr, value);
  
  wf_xmit_extbuf_delete(nic_info->tx_info, pxmit_buf);
  
  return ret;
}

#endif

