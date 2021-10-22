#include "pcomp.h"
#include "wf_debug.h"


#ifdef CONFIG_RICHV200_FPGA
#ifdef _X86_
#define WF_FW_FILE  L"\\??\\C:\\Program Files\\SCICS\\firmware.bin"
#else
#define WF_FW_FILE  L"\\??\\C:\\Program Files (x86)\\SCICS\\firmware.bin"
#endif
#else
#define WF_FW_FILE  L"\\??\\C:\\ram-fw-908x-old-r1549.bin"
#endif


static char *fw_path = (char *)WF_FW_FILE;


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

	return 0;
}

int nic_read_local_config(void *info)
{
	nic_info_st *pnic_info = info;
	local_info_st *local_info;

	if(pnic_info == NULL) {
		LOG_E("nic info is NULL");
		return -1;
	}

	if(pnic_info->local_info == NULL) {
		LOG_E("local info is NULL");
		return -2;
	}

	local_info = pnic_info->local_info;

	local_info->scan_ch_to = 20;
	local_info->scan_prb_times = 5;

	return 0;
}


#ifdef CONFIG_RICHV200_FPGA

void print_reg_val(PADAPTER pAdapter, wf_u32 addr)
{
	wf_u32 value;

	value = wf_usb_read8(pAdapter, addr);
	LOG_D("reg[%x]=0x%x", addr, value);
}


int side_road_cfg(PADAPTER padapter)
{
    int ret         = 0;
    wf_u8  value8   = 0;

    /************************func configure*****************************/
    
    /* enable reg r/w */
    //LOG_I("old:0xac---0x%x",wf_usb_read8(padapter, 0xac));
    value8 = wf_usb_read8(padapter, 0xac);
    value8 |= 0x02;
    ret = wf_usb_write8(padapter, 0xac, value8);
    //LOG_I("new:0xac---0x%x",wf_usb_read8(padapter, 0xac));

    /* M0 Uart/Fw_type select */
    //LOG_I("old:0xf8---0x%x",wf_usb_read8(padapter, 0xf8));
    value8 = wf_usb_read8(padapter, 0xf8);
    value8 |= 0x10;      // special write for all
    value8 |= 0x40;      // ��M0�Ĵ���

#ifdef CONFIG_FW_ENCRYPT
    value8 |= 0x80;      // ���ܹ̼�
#else
    value8 &= 0x7F;      // �Ǽ��ܹ̼�
#endif
    ret = wf_usb_write8(padapter, 0xf8, value8);
    //LOG_I("new:0xf8---0x%x",wf_usb_read8(padapter, 0xf8));

    //LOG_I("old:0x98---0x%x",wf_usb_read8(padapter, 0x98));
    wf_usb_write8(padapter,0x98,0xff);
   // LOG_I("new:0x98---0x%x",wf_usb_read8(padapter, 0x98));


    /*  For Bluk transport */
    #if 1
    ret=wf_usb_write32(padapter, 0x200, 0x00100000);
    //LOG_I("0x200---0x%x",wf_usb_read32(padapter, 0x200));
    ret=wf_usb_write32(padapter, 0x200, 0x80100000);
    //LOG_I("0x200---0x%x",wf_usb_read32(padapter, 0x200));
    #endif

    LOG_I("[%s] cfg sucess",__func__);

    return 0;
}


int hw_pwr_on_v200(PADAPTER pAdapter)
{
	wf_u8 temp;
	wf_u8 value8;
	wf_u16 value16;
	wf_u32 value32;
	int ret, initSuccess = wf_false;

	value32 = wf_usb_read32(pAdapter, 0x94);
	//LOG_D("reg_94=%x", value32);

	    //set 0x_00AC  bit 4 д0
    value8 = wf_usb_read8(pAdapter, 0xac);
    value8 &= 0xEF;
    ret = wf_usb_write8(pAdapter, 0xac, value8);
    if( WF_RETURN_FAIL == ret) {
        LOG_E("[%s] 0xac failed, check!!!",__func__);
        return ret;
    }
    //set 0x_00AC  bit 0 д0
    value8 &= 0xFE;
    ret = wf_usb_write8(pAdapter, 0xac, value8);
    if( WF_RETURN_FAIL == ret) {
        LOG_E("[%s] 0xac failed, check!!!",__func__);
        return ret;
    }
    //set 0x_00AC  bit 0 д1
    value8 |= 0x01;
    ret = wf_usb_write8(pAdapter, 0xac, value8);
    if( WF_RETURN_FAIL == ret) {
        LOG_E("[%s] 0xac failed, check!!!",__func__);
        return ret;
    }
    wf_msleep(10);
    // waiting for power on
    value16 = 0;
  
    while(1){
        value8 = wf_usb_read8(pAdapter, 0xac);
        if(value8 & 0x10) {
            initSuccess = wf_true;
            break;
        }
        value16++;
        if(value16 > 1000) {
            break;
        }
    }

	LOG_D("power_on value=%x", value8);

    // enable mcu-bus clk
    wf_usb_read32(pAdapter,0x94);       
    ret = wf_usb_write32(pAdapter,0x94,0x6);
    if (WF_RETURN_FAIL == ret) {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }

    if(initSuccess == wf_false) {
        LOG_E("[%s] failed!!!",__func__);
        return WF_RETURN_FAIL;
    }

    LOG_I("[%s] success",__func__);

	ret = side_road_cfg(pAdapter);
	if(ret != WF_RETURN_OK) {
		LOG_E("side road config failed");
		return NDIS_STATUS_FAILURE;
	}

	return WF_RETURN_OK;
}

int hw_pwr_off_v200(PADAPTER pAdapter)
{
	int ret = 0;
	wf_u16 value16 = 0;
	wf_u32 value32 = 0;

	LOG_D("start power off");

	// disable mcu-bus clk
	value32 = wf_usb_read32(pAdapter, 0x94);
	ret = wf_usb_write32(pAdapter, 0x94, 0);
	if (WF_RETURN_OK != ret) {
		LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
		return WF_RETURN_FAIL;
	}

	// 0x00ac  bit 22 write 1, reset dsp
	value32 = wf_usb_read32(pAdapter, 0xac);
	value32 |= WF_BIT(22);
	ret = wf_usb_write32(pAdapter, 0xac, value32);
	if( WF_RETURN_OK != ret) {
		LOG_E("[%s] 0xac failed, check!!!",__func__);
		return ret;
	}

	#if 1
	// clear the power off bit
	value32 &= ~((wf_u32)WF_BIT(11)); 
	ret = wf_usb_write32(pAdapter, 0xac, value32);
	if( WF_RETURN_OK != ret) {
		LOG_E("[%s] 0xac failed, check!!!",__func__);
		return ret;
	}

	// �����µ���ʼ����Ϊ��0x00AC[10] ����0����1��������ʹ��Ӳ���µ�״̬��
	value32 &= ~((wf_u32)WF_BIT(10));  
	ret = wf_usb_write32(pAdapter, 0xac, value32);
	if( WF_RETURN_OK != ret)
	{
		LOG_E("[%s] 0xac failed, check!!!",__func__);
		return ret;
	}
	value32 |= WF_BIT(10);	
	ret = wf_usb_write32(pAdapter, 0xac, value32);
	if( WF_RETURN_OK != ret)
	{
		LOG_E("[%s] 0xac failed, check!!!",__func__);
		return ret;
	}

	wf_msleep(10);
	// waiting for power off
	value16 = 0;
	while(1) {
		value32 = wf_usb_read32(pAdapter, 0xac);
		if(value32 & WF_BIT(11)) {
			break;
		}
		wf_msleep(1);
		value16++;
		if(value16 > 10) {
			break;
		}
	}
	if(value16 > 10) {
		LOG_E("[%s] failed!!!",__func__);
		return WF_RETURN_FAIL;
	}
	#endif
	
	return WF_RETURN_OK;
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

	wf_set_bits_to_le_u32(ptx_desc + 16, 16, 16, checksum);
}  


int hw_write_firmware(PADAPTER pAdapter, UCHAR which, UCHAR *firmware, UINT len)
{
	wf_u16 i;
	wf_u16 checksum;
	wf_u32 align_len;
	wf_u32 send_once;
	wf_u32 send_len;
	wf_u32 send_size;
	wf_u32 remain_size;
	wf_u32 block_num;
	wf_u8 *use_buffer;
	wf_u8 *ptx_desc;
	wf_u8 alloc_buffer[BLOCK_BUF_LEN + 4];
	wf_usb_info_t *usb_info = pAdapter->usb_info;
	LARGE_INTEGER timeout = { 0 };
	wf_u32 *fw_status;

	timeout.QuadPart = DELAY_ONE_MILLISECOND;
	timeout.QuadPart *= 3000;
	
	align_len = ((len + 3) / 4) * 4;

	/* alloc mem for xmit */
	LOG_D("firmware download length is %d", len);

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

	for (i = 0; i<block_num; i++)
	{
		wf_memset(use_buffer, 0, BLOCK_BUF_LEN);
		ptx_desc = use_buffer;
		/* set for fw xmit */
		wf_set_bits_to_le_u32(ptx_desc, 0, 2, TYPE_FW);
		/* set for first packet */
		if (i == 0)
		{
			wf_set_bits_to_le_u32(ptx_desc, 11, 1, 1);
		}
		/* set for last packet */
		if (i == (block_num - 1))
		{
			wf_set_bits_to_le_u32(ptx_desc, 10, 1, 1);
		}
		/* set for which firmware */
		wf_set_bits_to_le_u32(ptx_desc, 12, 1, which);
		/* set for reg HWSEQ_EN */
		wf_set_bits_to_le_u32(ptx_desc, 18, 1, 1);
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
			wf_set_bits_to_le_u32(ptx_desc + send_len, 0, 32, checksum);
			LOG_D("my checksum is 0x%04x,fw_len=%d", checksum, align_len);
			send_len += TXDESC_PACK_LEN;
			send_once += TXDESC_PACK_LEN;
		}
		wf_set_bits_to_le_u32(ptx_desc + 8, 0, 16, send_once);

		/* set for checksum */
		hw_txdesc_chksum(ptx_desc);

		if (wf_usb_write(pAdapter, 2, CMD_QUEUE_INX, ptx_desc, send_len) < 0)//
		{
			LOG_E("bulk download firmware error");
			return -1;
		}
		//LOG_D("bulk download firmware success, idx[%d], all[%d]", i, block_num);
		send_size += send_once;
		remain_size -= send_once;

		//wf_msleep(1);
	}
	
	if(KeWaitForSingleObject(&usb_info->fw_finish, Executive, KernelMode, TRUE, &timeout) != STATUS_SUCCESS) {
		fw_status = &usb_info->fw_buffer[16];
		LOG_E("wait fw download finish timeout! reason=%x", *fw_status);
		return -1;
	}
	
	LOG_D("bulk download firmware success");

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
				value = wf_usb_read8(pAdapter, offset);
				value = value & (~(GET_PWR_CFG_MASK(PwrCfgCmd)));
				value = value | (GET_PWR_CFG_VALUE(PwrCfgCmd) &
					GET_PWR_CFG_MASK(PwrCfgCmd));
				wf_usb_write8(pAdapter, offset, value);

				break;

			case PWR_CMD_POLLING:
				LOG_D("pwr_seq_cmd_prasing: PWR_CMD_POLLING");

				bPollingBit = wf_false;
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
				do {
					value = wf_usb_read8(pAdapter, offset);
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
    USHORT value16;

    wf_usb_write8(pAdapter, 0xac, 0x06);

    wf_usb_write8(pAdapter, 0x98, 0xFF);

	wf_usb_write8(pAdapter, 0x99, 0xFF);
	
	wf_usb_write8(pAdapter, 0x9a, 0x01);
	
     if (WF_RETURN_FAIL == hw_pwr_cmd_parse
         (pAdapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,
         wlan_usb_card_enable_flow))
         return WF_RETURN_FAIL;

	wf_usb_write16(pAdapter, REG_CR_9086X, 0x00);

	value16 = wf_usb_read16(pAdapter, REG_CR_9086X);
     value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
                 | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	wf_usb_write16(pAdapter, REG_CR_9086X, value16);

    // enable mcu-bus clk
    wf_usb_write32(pAdapter, 0x94, 0x6);
	return WF_RETURN_OK;
}



#endif

NDIS_STATUS wf_nic_dev_init(PADAPTER pAdapter)
{
	NDIS_STATUS ret;
	int inner_ret;
	nic_info_st *nic_info = pAdapter->nic_info;
	
	LOG_D("start power on");
#ifndef CONFIG_RICHV200_FPGA
	inner_ret = hw_pwr_on_v100(pAdapter);
#else	
	inner_ret = hw_pwr_on_v200(pAdapter);
#endif
	if(inner_ret != WF_RETURN_OK) {
		LOG_E("power on failed");
		return NDIS_STATUS_FAILURE;
	}

//	inner_ret = side_road_cfg(pAdapter);
//	if(inner_ret != WF_RETURN_OK) {
//		LOG_E("side road config failed");
//		return NDIS_STATUS_FAILURE;
//	}

	inner_ret = hw_read_fw_file(pAdapter);
	if(inner_ret != WF_RETURN_OK) {
		LOG_E("read fw failed");
		return NDIS_STATUS_FAILURE;
	}

	nic_info->hif_node = pAdapter;
	nic_info->ndev = pAdapter;

	nic_info->hif_node_id = 0;

	nic_info->ndev_id = 0;
	nic_info->is_up = 0;
	nic_info->virNic = FALSE;

	nic_info->nic_type = NIC_USB;
	nic_info->dev = NULL;

	nic_info->fwdl_info.fw_rom_type = pAdapter->AdapterFwInfo.fw_usb_rom_type;
	nic_info->fwdl_info.fw0 = (char *)pAdapter->AdapterFwInfo.fw0_usb;
	nic_info->fwdl_info.fw0_size = pAdapter->AdapterFwInfo.fw0_usb_size;
	nic_info->fwdl_info.fw1 = (char *)pAdapter->AdapterFwInfo.fw1_usb;
	nic_info->fwdl_info.fw1_size = pAdapter->AdapterFwInfo.fw1_usb_size;


	nic_info->func_check_flag = 0xAA55BB66;
	nic_info->nic_read = wf_usb_read;
	nic_info->nic_write = wf_usb_write;
	nic_info->nic_tx_queue_insert = wf_usb_xmit_insert;//hif_info->ops->hif_tx_queue_insert;
	nic_info->nic_tx_queue_empty = wf_usb_xmit_empty;//hif_info->ops->hif_tx_queue_empty;
	nic_info->nic_cfg_file_read = nic_read_local_config;
	nic_info->wdn_id_bitmap = &pAdapter->wdn_id_bitmap;

#ifdef CONFIG_RICHV200_FPGA
		nic_info->nic_write_fw = hw_write_firmware;
		//nic_info->nic_write_cmd = hif_write_cmd;
#endif
	
    ret = wf_usb_dev_start(pAdapter);
    if (ret != NDIS_STATUS_SUCCESS) {
        LOG_E("Failed to start usb dev.");
		return ret;
    }

	inner_ret = nic_init(nic_info);
	if(inner_ret != WF_RETURN_OK) {
		LOG_E("nic init failed");
		return NDIS_STATUS_FAILURE;
	}

	// If we want to create more than one device, we should add mlme lock to pAdapter.
	
	/* mlme hardware access hw lock */
    //nic_info->mlme_hw_access_lock = NULL;    
	/* mcu hardware access hw lock */
    nic_info->mcu_hw_access_lock  = &pAdapter->mcu_hw_access_lock;
	nic_info->mcu_hw_access_lock->lock_type = WF_LOCK_TYPE_MUTEX;
	wf_lock_init(&nic_info->mcu_hw_access_lock->lock_mutex, WF_LOCK_TYPE_MUTEX);
	
	ret = wf_xmit_init(pAdapter);
	if (ret != NDIS_STATUS_SUCCESS) {
        LOG_E("xmit init failed");
		return ret;
    }
	
	ret = wf_recv_init(pAdapter);
	if (ret != NDIS_STATUS_SUCCESS) {
        LOG_E("recv init failed");
		return ret;
    }

	inner_ret = nic_enable(nic_info);
	if (inner_ret != WF_RETURN_OK) {
        LOG_E("nic enable failed");
		return NDIS_STATUS_FAILURE;
    }
	
	return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS wf_nic_dev_deinit(PADAPTER pAdapter)
{
	int ret = 0;
	nic_info_st *nic_info = pAdapter->nic_info;

	nic_info->is_surprise_removed = wf_true;
	
	ret += nic_disable(pAdapter->nic_info);
	ret += nic_shutdown(pAdapter->nic_info);
	ret += nic_term(pAdapter->nic_info);

	wf_xmit_deinit(pAdapter);
	wf_recv_deinit(pAdapter);

#ifdef CONFIG_RICHV200
	ret += hw_pwr_off_v200(pAdapter);
#else

#endif
	if(ret != 0) {
		LOG_E("dev deinit failed!!!!");
	}
	
	wf_msleep(100);
	
#ifdef CONFIG_RICHV200
	ret += hw_pwr_on_v200(pAdapter);
#else
	
#endif
	if(ret != 0) {
		LOG_E("dev deinit failed!!!!");
	}

	return NDIS_STATUS_SUCCESS;
}


