//#include "hif.h"
#include "pcomp.h"
#include "common.h"
#include "wf_debug.h"
#include "wf_mix.h"
#include "wf_typedef.h"
#define CONFIG_FW_ENCRYPT
extern VOID
FreeWriteRequestsWorker(
	PADAPTER adapter,
	PUSB_WRITE_RESOURCES    WriteResources
);
extern BOOL
AllocateWriteRequestsForSync(
	PADAPTER adapter,
	BYTE    MaxOutstanding
);

extern void wf_Msleep(wf_u32 MilliSecond);
#define CMD_PARAM_LENGTH       12
#define TXDESC_OFFSET_NEW      20
#define TXDESC_PACK_LEN        4
#define RXDESC_OFFSET_NEW      16
#define RXDESC_PACK_LEN        4
#define TX_RX_REG_MAX_SIZE     28
#define FIRMWARE_BLOCK_SIZE    (512 -  TXDESC_OFFSET_NEW - TXDESC_PACK_LEN)
#define HIF_BULK_MSG_TIMEOUT    5000

void print_reg_val(PADAPTER pAdapter, wf_u32 addr)
{
	wf_u32 value;

	value = HwPlatformIORead1Byte(pAdapter, addr, NULL);
	LOG_D("reg[%x]=0x%x", addr, value);
}


int power_on_v200(PADAPTER pAdapter)
{

	int ret = 0;
	wf_u8 temp;
	wf_u16 value16;
	wf_u32 value8;
	wf_bool initSuccess = wf_false;
#if 1
	temp = wf_usb_io_read8(pAdapter, 0xac, NULL);
	temp &= 0xEF;
	wf_usb_io_write8(pAdapter, 0xac, temp);
	print_reg_val(pAdapter, 0xac);
	temp &= 0xFE;
	wf_usb_io_write8(pAdapter, 0xac, temp);
	print_reg_val(pAdapter, 0xac);
	// ���� 0x_00AC  bit 0 д1
	temp |= 0x01;
	wf_usb_io_write8(pAdapter, 0xac, temp);
	print_reg_val(pAdapter, 0xac);
	// waiting for power on
	value16 = 0;
	while (1) {
		temp = wf_usb_io_read8(pAdapter, 0xac, NULL);
		if (temp & 0x10) {
			break;
		}
		wf_msleep(1);
		value16++;
		if (value16 > 10) {
			break;
		}
	}
	if (value16 > 10) {
		LOG_E("[%s] failed!!!", __func__);
		return WF_RETURN_FAIL;
	}

	// ���� 0x_00AC  bit 1 д1
	temp = wf_usb_io_read8(pAdapter, 0xac, NULL);
	temp |= 0x02;
	wf_usb_io_write8(pAdapter, 0xac, temp);
	print_reg_val(pAdapter, 0xac);
	// д0x_00F8[4]  bit 4 д1 
	temp = wf_usb_io_read8(pAdapter, 0xf8, NULL);
	temp |= 0x10;
	temp |= 0x40;      // 

#ifdef CONFIG_FW_ENCRYPT
	temp |= 0x80;      // 
#else
	temp &= 0x7F;      // 
#endif
	wf_usb_io_write8(pAdapter, 0xf8, temp);
	print_reg_val(pAdapter, 0xf8);
	wf_usb_io_write8(pAdapter, 0x98, 0xFF);
	print_reg_val(pAdapter, 0x98);
	wf_usb_io_write8(pAdapter, 0x94, 0x1E);
	print_reg_val(pAdapter, 0x94);
	wf_usb_io_write8(pAdapter, 0x200, 0x20);
	print_reg_val(pAdapter, 0x200);
	wf_usb_io_write16(pAdapter, 0x202, 0x8099);
	print_reg_val(pAdapter, 0x202);

	// enable mcu-bus clk
	wf_usb_io_read32(pAdapter, 0x94, NULL);
	wf_usb_io_write32(pAdapter, 0x94, 0x6);

	LOG_I("[%s] success", __func__);
#else

	//set 0x_00AC  bit 4 д0
	value8 = wf_usb_io_read8(pAdapter, 0xac, NULL);
	value8 &= 0xEF;
	wf_usb_io_write8(pAdapter, 0xac, value8);
	print_reg_val(pAdapter, 0xac);
	//set 0x_00AC  bit 0 д0
	value8 &= 0xFE;
	wf_usb_io_write8(pAdapter, 0xac, value8);
	print_reg_val(pAdapter, 0xac);
	//set 0x_00AC  bit 0 д1
	value8 |= 0x01;
	wf_usb_io_write8(pAdapter, 0xac, value8);
	print_reg_val(pAdapter, 0xac);
	wf_msleep(10);
	// waiting for power on
	value16 = 0;

	while (1) {
		value8 = wf_usb_io_read8(pAdapter, 0xac, NULL);
		if (value8 & 0x10) {
			initSuccess = wf_true;
			break;
		}
		value16++;
		if (value16 > 1000) {
			break;
		}
	}

	// enable mcu-bus clk
	wf_usb_io_read32(pAdapter, 0x94, NULL);
	wf_usb_io_write32(pAdapter, 0x94, 0x6);
	if (initSuccess == wf_false)
	{
		LOG_E("[%s] failed!!!", __func__);
		return WF_RETURN_FAIL;
	}

	LOG_I("[%s] success", __func__);

	return WF_RETURN_OK;
#endif
}
wf_u16 io_firmware_chksum(wf_u8 *firmware, wf_u32 len)
{
	wf_u32 loop;
	wf_u16 *u16Ptr = (wf_u16 *)firmware;
	wf_u32 index;
	wf_u16 checksum = 0;

	loop = len / 2;
	for (index = 0; index < loop; index++)
		checksum ^= le16_to_cpu(*(u16Ptr + index));

	return checksum;
}
void io_txdesc_chksum(wf_u8 *ptx_desc)
{
	wf_u16 *usPtr = (wf_u16 *)ptx_desc;
	wf_u32 index;
	wf_u16 checksum = 0;

	for (index = 0; index < 9; index++)
		checksum ^= le16_to_cpu(*(usPtr + index));

	SET_BITS_TO_LE_4BYTE(ptx_desc + 16, 16, 16, checksum);
}

int hif_write_firmware(PADAPTER pAdapter, UCHAR which, UCHAR *firmware, UINT len)
{
	wf_u8  u8Value;
	wf_u16 i;
	wf_u16 checksum;
	wf_u16 u16Value;
	wf_u32 align_len;
	wf_u32 buffer_len;
	wf_u32 back_len;
	wf_u32 send_once;
	wf_u32 send_len;
	wf_u32 send_size;
	wf_u32 remain_size;
	wf_u32 block_num;
	wf_u8 *alloc_buffer;
	wf_u8 *use_buffer;
	wf_u8 *ptx_desc;
	wf_u8 *prx_desc;
	wf_u32 register_addr;
	wf_u16 rx_len;
	PUSB_DEVICE_CONTEXT     pUsbDeviceContext;
	align_len = ((len + 3) / 4) * 4;

	/* alloc mem for xmit */
	buffer_len = TXDESC_OFFSET_NEW + TXDESC_PACK_LEN + FIRMWARE_BLOCK_SIZE;
	LOG_D("firmware download length is %d", len);
	LOG_D("firmware download buffer size is %d", buffer_len);
	alloc_buffer = wf_kzalloc(buffer_len + 4);
	if (alloc_buffer == NULL)
	{
		LOG_E("can't kzalloc memmory for download firmware");
		return -1;
	}
	use_buffer = (wf_u8 *)WF_N_BYTE_ALIGMENT((SIZE_PTR)(alloc_buffer), 4);

	block_num = align_len / FIRMWARE_BLOCK_SIZE;
	if (align_len % FIRMWARE_BLOCK_SIZE)
	{
		block_num += 1;
	}
	else
	{
		align_len += 4;
		block_num += 1;
	}
	remain_size = align_len;
	send_size = 0;

	LOG_I("fwdownload block number is %d", block_num);
	AllocateWriteRequestsForSync(pAdapter, 88);
	WDF_REQUEST_SEND_OPTIONS_INIT(
		&syncRequset,
		WDF_REQUEST_SEND_OPTION_SYNCHRONOUS
	);
	WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(
		&syncRequset,
		WDF_REL_TIMEOUT_IN_SEC(30)
	);
	for (i = 0; i<block_num; i++)
	{
		wf_memset(use_buffer, 0, buffer_len);
		ptx_desc = use_buffer;
		/* set for fw xmit */
		SET_BITS_TO_LE_4BYTE(ptx_desc, 0, 2, TYPE_FW);
		/* set for first packet */
		if (i == 0)
		{
			SET_BITS_TO_LE_4BYTE(ptx_desc, 11, 1, 1);
		}
		/* set for last packet */
		if (i == (block_num - 1))
		{
			SET_BITS_TO_LE_4BYTE(ptx_desc, 10, 1, 1);
		}
		/* set for which firmware */
		SET_BITS_TO_LE_4BYTE(ptx_desc, 12, 1, which);
		/* set for reg HWSEQ_EN */
		SET_BITS_TO_LE_4BYTE(ptx_desc, 18, 1, 1);
		/* set for pkt_len */
		if (remain_size > FIRMWARE_BLOCK_SIZE)
		{
			send_once = FIRMWARE_BLOCK_SIZE;
		}
		else
		{
			send_once = remain_size;
		}

		wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW, firmware + send_size, send_once);

		send_len = TXDESC_OFFSET_NEW + send_once;
		/* set for  firmware checksum */
		if (i == (block_num - 1))
		{
			checksum = io_firmware_chksum(firmware, align_len);
			LOG_I("cal checksum=%d", checksum);
			SET_BITS_TO_LE_4BYTE(ptx_desc + send_len, 0, 32, checksum);
			LOG_D("my checksum is 0x%04x,fw_len=%d", checksum, align_len);
			send_len += TXDESC_PACK_LEN;
			send_once += TXDESC_PACK_LEN;
		}
		SET_BITS_TO_LE_4BYTE(ptx_desc + 8, 0, 16, send_once);

		/* set for checksum */
		io_txdesc_chksum(ptx_desc);

		if (wf_usb_io_write(pAdapter, 2, CMD_QUEUE_INX, ptx_desc, send_len) < 0)//
		{
			LOG_E("bulk download firmware error");
			wf_kfree(alloc_buffer);
			return -1;
		}
		//LOG_D("bulk download firmware success, idx[%d], all[%d]", i, block_num);
		send_size += send_once;
		remain_size -= send_once;
	}
	pUsbDeviceContext = GetUsbDeviceContext(pAdapter->UsbDevice);
	FreeWriteRequestsWorker(pAdapter, &pUsbDeviceContext->SyncWrites);
	LOG_D("bulk download firmware success")
#if 0
		prx_desc = use_buffer;
	back_len = RXDESC_OFFSET_NEW + RXDESC_PACK_LEN;
	wf_u8 *temp;
	temp = wf_kzalloc(512);
	//temp = ;
	wf_memcpy(prx_desc, temp, back_len);

	u8Value = ReadLE1Byte(prx_desc);
	if ((u8Value & 0x03) != TYPE_FW)
	{
		LOG_E("bulk download firmware type error by read back");
		wf_kfree(alloc_buffer);
		return -1;
	}
	u16Value = ReadLE2Byte(prx_desc + 4);
	u16Value &= 0x3FFF;
	if (u16Value != RXDESC_PACK_LEN)
	{
		LOG_E("bulk download firmware length error, value: %d", u16Value);
		wf_kfree(alloc_buffer);
		return -1;
	}

	u8Value = ReadLE1Byte(prx_desc + 16);
	if (u8Value != 0x00)
	{
		LOG_E("bulk download firmware status error");
		u16Value = ReadLE2Byte(prx_desc + 18);
		LOG_D("Read checksum is 0x%04x", u16Value);
		if (u8Value == 0x01)
		{
			LOG_E("bulk download firmware txd checksum error");
		}
		else if (u8Value == 0x02)
		{
			LOG_E("bulk download firmware fw checksum error");
		}
		else if (u8Value == 0x03)
		{
			LOG_E("bulk download firmware fw & txd checksum error");
		}
		wf_kfree(alloc_buffer);
		return -1;
	}
#endif
	wf_kfree(alloc_buffer);

	if (which == FIRMWARE_M0)
	{
		LOG_I("bulk download m0 firmware ok");
	}
	else if (which == FIRMWARE_DSP)
	{
		LOG_I("bulk download dsp firmware ok");
	}

	return 0;
}

#if 0
int hif_write_cmd(PADAPTER pAdapter, wf_u32 cmd, wf_u32 *send_buf, wf_u32 send_len, wf_u32 *recv_buf, wf_u32 recv_len)
{
	int ret = 0;
	wf_u8  u8Value;
	wf_u16 u16Value;
	wf_u8 *ptx_desc;
	wf_u8 *prx_desc;
	wf_u32 register_addr;
	wf_u16 snd_pktLen = 0;
	wf_u16 rcv_pktLen = 0;

	ptx_desc = wf_kzalloc(512);
	wf_memset(ptx_desc, 0, TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH);

	/* set for reg xmit */
	SET_BITS_TO_LE_4BYTE(ptx_desc, 0, 2, TYPE_CMD);
	/* set for cmd index */
	SET_BITS_TO_LE_4BYTE(ptx_desc, 2, 8, 0);
	/* set for reg HWSEQ_EN */
	SET_BITS_TO_LE_4BYTE(ptx_desc, 18, 1, 1);
	/* set SEQ  for test*/
	//SET_BITS_TO_LE_4BYTE(ptx_desc, 24, 8, __gmcu_cmd_count & 0xFF);
	/* set for pkt_len */
	SET_BITS_TO_LE_4BYTE(ptx_desc + 8, 0, 16, CMD_PARAM_LENGTH + send_len * 4);
	/* set for checksum */
	io_txdesc_chksum(ptx_desc);

	/* set for  func_code */
	SET_BITS_TO_LE_4BYTE(ptx_desc + TXDESC_OFFSET_NEW, 0, 32, cmd);
	/* set for  len */
	SET_BITS_TO_LE_4BYTE(ptx_desc + TXDESC_OFFSET_NEW + 4, 0, 32, send_len);
	/* set for  offs */
	SET_BITS_TO_LE_4BYTE(ptx_desc + TXDESC_OFFSET_NEW + 8, 0, 32, recv_len);

	/* set for send content */
	if (send_len != 0)
	{
		wf_memcpy(ptx_desc + TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH, send_buf, send_len * 4);
	}

	snd_pktLen = TXDESC_OFFSET_NEW + CMD_PARAM_LENGTH + send_len * 4;

	//wf_hif_bulk_cmd_init(hif_node);
	ret = wf_tx_queue_insert(hif_node, 1, ptx_desc, snd_pktLen, wf_quary_addr(CMD_QUEUE_INX), NULL, NULL, NULL);
	if (ret != 0)
	{
		LOG_E("bulk access cmd error by send");
		ret = -1;
		goto mcu_cmd_communicate_exit;
	}

	if (wf_hif_bulk_cmd_wait(hif_node, HIF_BULK_MSG_TIMEOUT) == 0)
	{
		LOG_E("bulk access cmd read timeout");
		ret = -1;
		goto mcu_cmd_communicate_exit;
	}

	prx_desc = hif_node->cmd_rcv_buffer;
	rcv_pktLen = RXDESC_OFFSET_NEW + recv_len * 4 + CMD_PARAM_LENGTH;
	if (hif_node->cmd_size != rcv_pktLen)
	{
		LOG_E("mcu cmd: 0x%08X", cmd);
		LOG_E("bulk access cmd read length error, recv cmd size is %d, but need pkt_len is %d", hif_node->cmd_size, rcv_pktLen);
		ret = -1;
		goto mcu_cmd_communicate_exit;
	}

	prx_desc = hif_node->cmd_rcv_buffer;
	u8Value = ReadLE1Byte(prx_desc);
	if ((u8Value & 0x03) != TYPE_CMD)
	{
		LOG_E("bulk access cmd read error");
		ret = -1;
		goto mcu_cmd_communicate_exit;
	}
	u16Value = ReadLE2Byte(prx_desc + 4);
	u16Value &= 0x3FFF;
	if (u16Value != (recv_len * 4 + CMD_PARAM_LENGTH))
	{
		LOG_E("bulk access cmd read length error, value is %d, send cmd is 0x%x, cmd is 0x%x",
			u16Value, cmd, *((wf_u32 *)prx_desc + RXDESC_OFFSET_NEW));

		ret = -1;
		goto mcu_cmd_communicate_exit;
	}

	if (recv_len != 0)
	{
		wf_memcpy(recv_buf, prx_desc + RXDESC_OFFSET_NEW + CMD_PARAM_LENGTH, recv_len * 4);
	}

mcu_cmd_communicate_exit:

	return ret;
}
#endif

void log_array(void *ptr, wf_u16 len)
{
	wf_u16 i = 0;
	wf_u16 num;
	wf_u8 *pdata = ptr;

#define NUM_PER_LINE    8
	//	for (i = 0, num = len / NUM_PER_LINE; i < num;
	//         i++, pdata = &pdata[i * NUM_PER_LINE]) {
	for (i = 0, num = len / NUM_PER_LINE; i < num;
		i++, pdata += 8) {
		LOG_D("Efuse:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
			pdata[0], pdata[1], pdata[2], pdata[3],
			pdata[4], pdata[5], pdata[6], pdata[7]);
	}
	num = len % NUM_PER_LINE;
	if (num) {
		for (i = 0; i < num; i++) {
			LOG_D("Efuse:%02X", pdata[i]);
		}
	}
}
