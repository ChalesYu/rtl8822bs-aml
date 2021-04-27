#include "pcomp.h"
#include "common.h"
#include "wf_oids_adapt.h"
#include "wf_debug.h"
#include "tx_windows.h"
#include "hw_init.h"

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


wlan_pwr_cfg wlan_usb_card_enable_flow[WLAN_TRANS_ACT_TO_CARDEMU_STEPS +
	WLAN_TRANS_CARDEMU_TO_PDN_STEPS + WLAN_TRANS_END_STEPS] = {
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, WF_BIT(0), 0 }, /*Set SDIO suspend local register*/	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, WF_BIT(1), WF_BIT(1)}, /*wait power state to suspend*/\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(3) | WF_BIT(4), 0}, 	\
	{0x00C4, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, WF_BIT(4), 0}, \
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(2), 0},	\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, WF_BIT(1), WF_BIT(1)},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(7), 0},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, (WF_BIT(3)), 0},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(0), WF_BIT(0)},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, WF_BIT(0), 0},	 \
	{0x0027, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xff, 0x35}, \
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,0,PWR_CMD_END, 0, 0},
};


static char *fw_path = WF_FW_FILE;


//static char *fw_path = WF_FW_9188new;
//static char *fw_path = WF_FW_9188old;
static int hw_read_fw_file(PADAPTER                pAdapter)
{
    wf_file *file = NULL;
    fw_file_header_t fw_file_head;
    fw_header_t fw_head;
    loff_t pos;
    int i;
#ifndef CONFIG_RICHV200_FPGA
    DbgPrint("parse richv100 firmware!\n");
#else
	DbgPrint("parse richv200 firmware!\n");
#endif
    //file = wf_os_api_file_open(WF_FW_FILE);
    file = wf_os_api_file_open(fw_path);
    
    if (file == NULL)
    {
        DbgPrint("usb firmware open failed\n");
        return -1;
    }
    pos = 0;
    wf_os_api_file_read(file, pos, (unsigned char *)&fw_file_head, sizeof(fw_file_head));
#ifndef CONFIG_RICHV200_FPGA
	if ((fw_file_head.magic_number != 0xaffa) || (fw_file_head.interface_type != 0x9083))
	{
	    DbgPrint("usb firmware format error, magic:0x%x, type:0x%x\n",
	        fw_file_head.magic_number, fw_file_head.interface_type);
	    wf_os_api_file_close(file);
	    return -1;
	}
#else
#endif
    pAdapter->AdapterFwInfo.fw_usb_rom_type = fw_file_head.rom_type;
    pos += sizeof(fw_file_head);
    for (i = 0; i<fw_file_head.firmware_num; i++)
    {
        wf_memset(&fw_head, 0, sizeof(fw_head));
        wf_os_api_file_read(file, pos, (unsigned char *)&fw_head, sizeof(fw_head));
        if (fw_head.type == 0)
        {
            LOG_D("USB FW0 Ver: %d.%d.%d.%d, size:%dBytes\n",
                fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
                (fw_head.version >> 16) & 0xFF, (fw_head.version >> 24) & 0xFF,
                fw_head.length);
            pAdapter->AdapterFwInfo.fw0_usb_size = fw_head.length;
            // TODO: Free this memory when unloading.
            pAdapter->AdapterFwInfo.fw0_usb = wf_kzalloc(fw_head.length);
			
            if (NULL == pAdapter->AdapterFwInfo.fw0_usb)
            {
                DbgPrint("firmware0 usb kzalloc failed\n");
                wf_os_api_file_close(file);
                return -1;
            }
            wf_os_api_file_read(file, fw_head.offset, (unsigned char *)pAdapter->AdapterFwInfo.fw0_usb, fw_head.length);
        }
		else
		{
			LOG_D("USB FW1 Ver: %d.%d.%d.%d, size:%dBytes",
				fw_head.version & 0xFF, (fw_head.version >> 8) & 0xFF,
				(fw_head.version >> 16) & 0xFF, (fw_head.version >> 24) & 0xFF,
				fw_head.length);
			fw_head.length -= 32;
			pAdapter->AdapterFwInfo.fw1_usb_size = fw_head.length;
			pAdapter->AdapterFwInfo.fw1_usb = wf_kzalloc(fw_head.length);
			if (NULL == pAdapter->AdapterFwInfo.fw1_usb)
			{
				LOG_E("firmware1 usb kzalloc failed");
				wf_os_api_file_close(file);
				return -1;
			}
			wf_os_api_file_read(file, fw_head.offset + 32, (unsigned char *)pAdapter->AdapterFwInfo.fw1_usb, fw_head.length);
		}
        pos += sizeof(fw_head);
    }
	LOG_D("Read FW Finish\n");
    wf_os_api_file_close(file);
}


#ifdef CONFIG_RICHV200_FPGA

void print_reg_val(PADAPTER pAdapter, wf_u32 addr)
{
	wf_u32 value;

	value = HwPlatformIORead1Byte(pAdapter, addr, NULL);
	LOG_D("reg[%x]=0x%x", addr, value);
}


int hw_pwr_on_v200(PADAPTER pAdapter)
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


wf_u16 hw_firmware_chksum(wf_u8 *firmware, wf_u32 len)
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

void hw_txdesc_chksum(wf_u8 *ptx_desc)
{
	wf_u16 *usPtr = (wf_u16 *)ptx_desc;
	wf_u32 index;
	wf_u16 checksum = 0;

	for (index = 0; index < 9; index++)
		checksum ^= le16_to_cpu(*(usPtr + index));

	SET_BITS_TO_LE_4BYTE(ptx_desc + 16, 16, 16, checksum);
}


int hw_write_firmware(PADAPTER pAdapter, UCHAR which, UCHAR *firmware, UINT len)
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
			checksum = hw_firmware_chksum(firmware, align_len);
			LOG_I("cal checksum=%d", checksum);
			SET_BITS_TO_LE_4BYTE(ptx_desc + send_len, 0, 32, checksum);
			LOG_D("my checksum is 0x%04x,fw_len=%d", checksum, align_len);
			send_len += TXDESC_PACK_LEN;
			send_once += TXDESC_PACK_LEN;
		}
		SET_BITS_TO_LE_4BYTE(ptx_desc + 8, 0, 16, send_once);

		/* set for checksum */
		hw_txdesc_chksum(ptx_desc);

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

#else

INT32 hw_pwr_cmd_parse(PADAPTER pAdapter,
	wf_u8 CutVersion,
	wf_u8 FabVersion,
	wf_u8 InterfaceType, wlan_pwr_cfg PwrSeqCmd[])
{
	wlan_pwr_cfg PwrCfgCmd = { 0 };
	wf_u8  bPollingBit = wf_false;
	wf_u32 AryIdx = 0;
	wf_u8  value = 0;
	wf_u32 offset = 0;
	wf_u32 pollingCount = 0;
	wf_u32 maxPollingCnt = 5000;

	do {
		PwrCfgCmd = PwrSeqCmd[AryIdx];

		if ((GET_PWR_CFG_FAB_MASK(PwrCfgCmd) & FabVersion) &&
			(GET_PWR_CFG_CUT_MASK(PwrCfgCmd) & CutVersion) &&
			(GET_PWR_CFG_INTF_MASK(PwrCfgCmd) & InterfaceType)) {
			switch (GET_PWR_CFG_CMD(PwrCfgCmd)) {
			case PWR_CMD_READ:

				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_READ");
				break;

			case PWR_CMD_WRITE:
				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_WRITE");
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
				value = HwPlatformIORead1Byte(pAdapter, offset, NULL);
				value = value & (~(GET_PWR_CFG_MASK(PwrCfgCmd)));
				value = value | (GET_PWR_CFG_VALUE(PwrCfgCmd) &
					GET_PWR_CFG_MASK(PwrCfgCmd));
				HwPlatformIOWrite1Byte(pAdapter, offset, value);

				break;

			case PWR_CMD_POLLING:
				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_POLLING");

				bPollingBit = wf_false;
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
				do {
					value = HwPlatformIORead1Byte(pAdapter, offset, NULL);
					value = value & GET_PWR_CFG_MASK(PwrCfgCmd);
					if (value ==
						(GET_PWR_CFG_VALUE(PwrCfgCmd) &
							GET_PWR_CFG_MASK(PwrCfgCmd)))
						bPollingBit = wf_true;
					else
						wf_mdelay(10);

					if (pollingCount++ > maxPollingCnt)
					{

						LOG_E("pwr_seq_cmd_prasing: Fail to polling Offset[%#x]=%02x",
							offset, value);
						return WF_RETURN_FAIL;
					}
				} while (!bPollingBit);

				break;

			case PWR_CMD_DELAY:
				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_DELAY");
				if (GET_PWR_CFG_VALUE(PwrCfgCmd) == PWRSEQ_DELAY_US)
					wf_mdelay(GET_PWR_CFG_OFFSET(PwrCfgCmd));
				else
					wf_mdelay(GET_PWR_CFG_OFFSET(PwrCfgCmd) * 1000);
				break;

			case PWR_CMD_END:
				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_END");
				return WF_RETURN_OK;

			default:
				LOG_E("pwr_seq_cmd_prasing: Unknown CMD!!");
				break;
			}
		}

		AryIdx++;
	} while (1);

	return WF_RETURN_OK;
}

int hw_pwr_on_v100(PADAPTER pAdapter)
{
	UCHAR  value8;
    USHORT value16;
	UINT32 value32;

    HwPlatformIOWrite1Byte(pAdapter, 0xac, 0x06);

    HwPlatformIOWrite1Byte(pAdapter, 0x98, 0xFF);

	HwPlatformIOWrite1Byte(pAdapter, 0x99, 0xFF);
	
	HwPlatformIOWrite1Byte(pAdapter, 0x9a, 0x01);
	
     if (WF_RETURN_FAIL == hw_pwr_cmd_parse
         (pAdapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,
         wlan_usb_card_enable_flow))
         return WF_RETURN_FAIL;

	HwPlatformIOWrite2Byte(pAdapter, REG_CR_9086X, 0x00);

	value16 = HwPlatformIORead2Byte(pAdapter, REG_CR_9086X,NULL);
     value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
                 | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	HwPlatformIOWrite2Byte(pAdapter, REG_CR_9086X, value16);

    // enable mcu-bus clk
    HwPlatformIOWrite4Byte(pAdapter,0x94,0x6);
	return WF_RETURN_OK;
}



#endif

NDIS_STATUS hw_init(PADAPTER pAdapter)
{
	UINT status = 0;
#ifndef CONFIG_RICHV200_FPGA
	hw_pwr_on_v100(pAdapter);
#else	
	hw_pwr_on_v200(pAdapter);
#endif
	nic_info_st *nic_info = wf_kzalloc(sizeof(nic_info_st));
	if (nic_info == NULL) {
		LOG_E("malloc nic info failed!\n");
		return NDIS_STATUS_FAILURE;
	}
	memset(nic_info, 0, sizeof(nic_info_st));

	pAdapter->nic_info = nic_info;

	hw_read_fw_file(pAdapter);

	nic_info->hif_node = pAdapter;
	nic_info->ndev = pAdapter;

	nic_info->hif_node_id = 0;

	nic_info->ndev_id = 0;
	nic_info->is_up = 0;
	nic_info->virNic = FALSE;

	nic_info->nic_type = NIC_USB;
	nic_info->dev = NULL;

	nic_info->fwdl_info.fw_usb_rom_type = pAdapter->AdapterFwInfo.fw_usb_rom_type;
	nic_info->fwdl_info.fw0_usb = pAdapter->AdapterFwInfo.fw0_usb;
	nic_info->fwdl_info.fw0_usb_size = pAdapter->AdapterFwInfo.fw0_usb_size;
	nic_info->fwdl_info.fw1_usb = pAdapter->AdapterFwInfo.fw1_usb;
	nic_info->fwdl_info.fw1_usb_size = pAdapter->AdapterFwInfo.fw1_usb_size;


	nic_info->func_check_flag = 0xAA55BB66;
	nic_info->nic_read = wf_usb_io_read;
	nic_info->nic_write = wf_usb_io_write;
	nic_info->nic_tx_queue_insert = wf_xmit_list_insert;//hif_info->ops->hif_tx_queue_insert;
	nic_info->nic_tx_queue_empty = wf_xmit_list_empty;//hif_info->ops->hif_tx_queue_empty;



#ifdef CONFIG_RICHV200_FPGA
		nic_info->nic_write_fw = hw_write_firmware;
		//nic_info->nic_write_cmd = hif_write_cmd;
#endif

	if(HwBusStartDevice(pAdapter) != NDIS_STATUS_SUCCESS) {
		LOG_E("hw bus start failed!");
		return NDIS_STATUS_FAILURE;
	}

	status = nic_init(nic_info);
	
	if (status == 0) {
		nic_enable(nic_info);
		return NDIS_STATUS_SUCCESS;
	}
	return NDIS_STATUS_FAILURE;
}


