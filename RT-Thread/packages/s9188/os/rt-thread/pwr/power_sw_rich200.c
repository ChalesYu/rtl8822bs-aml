#include "common.h"
#include "hif/hif.h"
#include "hif/wf_sdio.h"
#include "wf_debug.h"

#define PWR_CMD_READ			0x00
#define PWR_CMD_WRITE			0x01
#define PWR_CMD_POLLING			0x02
#define PWR_CMD_DELAY			0x03
#define PWR_CMD_END				0x04

#define PWR_BASEADDR_MAC		0x00
#define PWR_BASEADDR_USB		0x01
#define PWR_BASEADDR_PCIE		0x02
#define PWR_BASEADDR_SDIO		0x03

#define	PWR_INTF_SDIO_MSK		WF_BIT(0)
#define	PWR_INTF_USB_MSK		WF_BIT(1)
#define	PWR_INTF_PCI_MSK		WF_BIT(2)
#define	PWR_INTF_ALL_MSK		(WF_BIT(0)|WF_BIT(1)|WF_BIT(2)|WF_BIT(3))

#define	PWR_FAB_TSMC_MSK		WF_BIT(0)
#define	PWR_FAB_UMC_MSK			WF_BIT(1)
#define	PWR_FAB_ALL_MSK			(WF_BIT(0)|WF_BIT(1)|WF_BIT(2)|WF_BIT(3))

#define	PWR_CUT_TESTCHIP_MSK	WF_BIT(0)
#define	PWR_CUT_A_MSK			WF_BIT(1)
#define	PWR_CUT_B_MSK			WF_BIT(2)
#define	PWR_CUT_C_MSK			WF_BIT(3)
#define	PWR_CUT_D_MSK			WF_BIT(4)
#define	PWR_CUT_E_MSK			WF_BIT(5)
#define	PWR_CUT_F_MSK			WF_BIT(6)
#define	PWR_CUT_G_MSK			WF_BIT(7)
#define	PWR_CUT_ALL_MSK			0xFF

typedef enum __pwrseq_cmd_delay_unit__ {
	PWRSEQ_DELAY_US,
	PWRSEQ_DELAY_MS,
} pwrseq_cmd_delay_unit;

typedef struct __wlan_pwr_cfg__ {
	wf_u16 offset;
	wf_u8 cut_msk;
	wf_u8 fab_msk:4;
	wf_u8 interface_msk:4;
	wf_u8 base:4;
	wf_u8 cmd:4;
	wf_u8 msk;
	wf_u8 value;
} wlan_pwr_cfg, *pwlan_pwr_cfg;

#define GET_PWR_CFG_OFFSET(__PWR_CMD)		__PWR_CMD.offset
#define GET_PWR_CFG_CUT_MASK(__PWR_CMD)		__PWR_CMD.cut_msk
#define GET_PWR_CFG_FAB_MASK(__PWR_CMD)		__PWR_CMD.fab_msk
#define GET_PWR_CFG_INTF_MASK(__PWR_CMD)	__PWR_CMD.interface_msk
#define GET_PWR_CFG_BASE(__PWR_CMD)			__PWR_CMD.base
#define GET_PWR_CFG_CMD(__PWR_CMD)			__PWR_CMD.cmd
#define GET_PWR_CFG_MASK(__PWR_CMD)			__PWR_CMD.msk
#define GET_PWR_CFG_VALUE(__PWR_CMD)		__PWR_CMD.value
#define	WLAN_TRANS_CARDEMU_TO_ACT_STEPS	    13
#define	WLAN_TRANS_ACT_TO_CARDEMU_STEPS	    15
#define	WLAN_TRANS_CARDEMU_TO_SUS_STEPS	    14
#define	WLAN_TRANS_SUS_TO_CARDEMU_STEPS	    15
#define	WLAN_TRANS_CARDEMU_TO_PDN_STEPS	    15
#define	WLAN_TRANS_PDN_TO_CARDEMU_STEPS	    15
#define	WLAN_TRANS_ACT_TO_LPS_STEPS		    11
#define	WLAN_TRANS_LPS_TO_ACT_STEPS		    13
#define	WLAN_TRANS_ACT_TO_SWLPS_STEPS		21
#define	WLAN_TRANS_SWLPS_TO_ACT_STEPS		14
#define	WLAN_TRANS_END_STEPS		        1

wlan_pwr_cfg wlan_usb_card_enable_flow[WLAN_TRANS_ACT_TO_CARDEMU_STEPS +
                                       WLAN_TRANS_CARDEMU_TO_PDN_STEPS +
									   WLAN_TRANS_END_STEPS] = {
    {0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, WF_BIT(0), 0}, /*Set SDIO suspend local register*/	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, WF_BIT(1), WF_BIT(1)}, /*wait power state to suspend*/\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(3)|WF_BIT(4), 0}, 	\
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


wlan_pwr_cfg wlan_usb_card_disable_flow[WLAN_TRANS_ACT_TO_CARDEMU_STEPS +
										WLAN_TRANS_CARDEMU_TO_PDN_STEPS +
										WLAN_TRANS_END_STEPS] = {
    {0x001F, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0},
	{0x004E, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(7), 0},
	{0x0027, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xff, 0x34},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, WF_BIT(1), WF_BIT(1)},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, WF_BIT(1), 0},
	{0x0007, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x00},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(3)|WF_BIT(4), WF_BIT(3)},
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, WF_BIT(0), WF_BIT(0)},
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, WF_BIT(1), 0},
	{0x00C4, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, WF_BIT(4), WF_BIT(4)},
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,0,PWR_CMD_END, 0, 0},
};


wlan_pwr_cfg wlan_usb_enter_lps_flow[WLAN_TRANS_ACT_TO_LPS_STEPS +
									 WLAN_TRANS_END_STEPS] = {
    {0x0139, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(0), WF_BIT(0)},
    {0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0xFF},
    {0x05F8, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},
    {0x05F9, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},
    {0x05FA, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, 0xFF, 0},
    {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(0), 0},
    {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_DELAY, 0, PWRSEQ_DELAY_US},
    {0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(1), 0},
    {0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, 0xFF, 0x3F},
    {0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(1), 0},
    {0x0553, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, WF_BIT(5), WF_BIT(5)},
    {0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,0,PWR_CMD_END, 0, 0},
};




wlan_pwr_cfg wlan_sdio_card_enable_flow[WLAN_TRANS_ACT_TO_CARDEMU_STEPS +
                                        WLAN_TRANS_CARDEMU_TO_PDN_STEPS +
										WLAN_TRANS_END_STEPS] = {
    {0x903a, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_WRITE, WF_BIT(0), 0},
    {0x903a, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,PWR_BASEADDR_SDIO,PWR_CMD_POLLING, WF_BIT(1), WF_BIT(1)},
    {0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(3)|WF_BIT(4), 0},
	{0x00C4, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, WF_BIT(4), 0},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(2), 0},
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, WF_BIT(1), WF_BIT(1)},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(7), 0},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(3), 0},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_WRITE, WF_BIT(0), WF_BIT(0)},
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_POLLING, WF_BIT(0), 0},
	{0x0027, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK, PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xff, 0x35},
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,PWR_BASEADDR_MAC,PWR_CMD_END, 0, 0},
};

wlan_pwr_cfg wlan_sdio_card_disable_flow[WLAN_TRANS_ACT_TO_CARDEMU_STEPS +
                                         WLAN_TRANS_CARDEMU_TO_PDN_STEPS +
										 WLAN_TRANS_END_STEPS] = {
	{0x001F, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,  0x00,  PWR_CMD_WRITE, 0xFF, 0},
	{0x004E, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,  0x00,  PWR_CMD_WRITE, WF_BIT(7), 0},
	{0x0027, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,  0x00,  PWR_CMD_WRITE, 0xff, 0x34},
	{0x0005, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,  0x00,  PWR_CMD_WRITE, WF_BIT(2), WF_BIT(2)},
	{0x0007, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, 0x00,  PWR_CMD_WRITE, 0xFF, 0x00},
	{0xFFFF, 0xFF, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,  0,	  PWR_CMD_END, 	0, 	  0},
};




int hal_pwr_seq_cmd_prasing(nic_info_st *nic_info,
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
	int ret = 0;

	do {
		PwrCfgCmd = PwrSeqCmd[AryIdx];

			if ((GET_PWR_CFG_FAB_MASK(PwrCfgCmd) & FabVersion) &&
			(GET_PWR_CFG_CUT_MASK(PwrCfgCmd) & CutVersion) &&
			(GET_PWR_CFG_INTF_MASK(PwrCfgCmd) & InterfaceType)) {
			switch (GET_PWR_CFG_CMD(PwrCfgCmd)) {
			case PWR_CMD_READ:

				LOG_D("hal_pwr_seq_cmd_prasing: PWR_CMD_READ");
				break;

			case PWR_CMD_WRITE:
				LOG_D("hal_pwr_seq_cmd_prasing: PWR_CMD_WRITE\n");
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
	            value = wf_io_read8(nic_info, offset,NULL);
				value = value & (~(GET_PWR_CFG_MASK(PwrCfgCmd)));
				value = value | (GET_PWR_CFG_VALUE(PwrCfgCmd) &
								GET_PWR_CFG_MASK(PwrCfgCmd));
				ret  = wf_io_write8(nic_info, offset, value);
				if( WF_RETURN_FAIL == ret)
				{
					LOG_E("[%s] failed, check!!!",__func__);
					return ret;
				}
				break;

			case PWR_CMD_POLLING:
				LOG_D("hal_pwr_seq_cmd_prasing: PWR_CMD_POLLING\n");

				bPollingBit = wf_false;
				offset = GET_PWR_CFG_OFFSET(PwrCfgCmd);
				do {
                    value = wf_io_read8(nic_info, offset,NULL);
					value = value & GET_PWR_CFG_MASK(PwrCfgCmd);
					if (value ==
						(GET_PWR_CFG_VALUE(PwrCfgCmd) &
						 GET_PWR_CFG_MASK(PwrCfgCmd)))
						bPollingBit = wf_true;
					else
						wf_mdelay(10);

					if (pollingCount++ > maxPollingCnt)
					{

						LOG_E("hal_pwr_seq_cmd_prasing: Fail to polling Offset[%#x]=%02x\n",
									   offset, value);
						return WF_RETURN_FAIL;
					}
				} while (!bPollingBit);

				break;

			case PWR_CMD_DELAY:
				LOG_D("hal_pwr_seq_cmd_prasing: PWR_CMD_DELAY\n");
				if (GET_PWR_CFG_VALUE(PwrCfgCmd) == PWRSEQ_DELAY_US)
					wf_mdelay(GET_PWR_CFG_OFFSET(PwrCfgCmd));
				else
					wf_mdelay(GET_PWR_CFG_OFFSET(PwrCfgCmd) * 1000);
				break;

			case PWR_CMD_END:
				LOG_D("hal_pwr_seq_cmd_prasing: PWR_CMD_END\n");
				return WF_RETURN_OK;

			default:
				LOG_D("hal_pwr_seq_cmd_prasing: Unknown CMD!!\n");
				break;
			}
		}

		AryIdx++;
	} while (1);

	return WF_RETURN_OK;
}

#define REG_CR_9086X			0x0100
//#define WF_REG_HCTL		      0x903A
#define WF_REG_CL		        0x001C
#define DXX0_EN_ADDR     		0x00E4
#define DXX0_START_ADDR 		0x00E8
#define DXX0_CLOCK_EN   		0x0094

#define WF_REG_MCU_DL			0x0080
#define WF_REG_SYS_ENABLE		0x0002
#define WF_REG_1CC				0x01CC

#define HCI_TXDMA_EN			WF_BIT(0)
#define HCI_RXDMA_EN			WF_BIT(1)
#define TXDMA_EN				WF_BIT(2)
#define RXDMA_EN				WF_BIT(3)
#define PROTOCOL_EN				WF_BIT(4)
#define SCHEDULE_EN				WF_BIT(5)
#define MACTXEN					WF_BIT(6)
#define MACRXEN					WF_BIT(7)
#define ENSWBCN					WF_BIT(8)
#define ENSEC					WF_BIT(9)
#define CALTMR_EN				WF_BIT(10)

int wf_sdio_tmp(nic_info_st *nic_info)
{
	wf_u8  value8;
	wf_u16 value16;
	wf_u32 value32;
	int ret;

	value32 = wf_io_read32(nic_info, 0x228,NULL);
	value32 &= 0xFFFFFF00;
	value32 |= ((((wf_u32)(0x1F)) << 0));
	ret = wf_io_write32(nic_info, 0x228, value32);
	if( WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s] 0x228 failed, check!!!",__func__);
		return ret;
	}

	if (WF_RETURN_FAIL == hal_pwr_seq_cmd_prasing
    (nic_info, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,
    wlan_sdio_card_enable_flow))
    return WF_RETURN_FAIL;

    ret = wf_io_write16(nic_info, REG_CR_9086X, 0x00);
	if( WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s] REG_CR_9086X failed, check!!!",__func__);
		return ret;
	}

#if 1
	value16 = wf_io_read16(nic_info, REG_CR_9086X,NULL);
    value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
                | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	ret = wf_io_write16(nic_info, REG_CR_9086X, value16);
#else
	value16 = wf_io_read16(nic_info, REG_CR_9086X,NULL);
    value16 |= 0xff;
	ret = wf_io_write16(nic_info, REG_CR_9086X, value16);
#endif

	if( WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s] REG_CR_9086X failed, check!!!",__func__);
		return ret;
	}

	value32 = wf_io_read32(nic_info, 0x214,NULL);
	value32 &= 0x0000FF00;
	value32 |= ((((wf_u32)(0x10)) << 16) | (((wf_u32)(0x15)) << 0));
	ret = wf_io_write32(nic_info, 0x214, value32);
	if( WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s] 0x214 failed, check!!!",__func__);
		return ret;
	}

	value32 = wf_io_read32(nic_info, 0x200,NULL);
	value32 &= 0xFF000000;
	value32 |= ((((wf_u32)(0x99)) << 16) | (((wf_u32)(0x15)) << 8) | (((wf_u32)(0x20)) << 0));
	value32 |= 0x80000000;
	LOG_I("Write 0x200: 0x%08X", value32);
	ret = wf_io_write32(nic_info, 0x200, value32);
	if( WF_RETURN_FAIL == ret)
	{
		LOG_E("[%s] 0x200 failed, check!!!",__func__);
		return ret;
	}
	LOG_I("Read 0x200: 0x%08X", wf_io_read32(nic_info, 0x200,NULL));
	
	/* check power on */
    {
		wf_u8 check_index = 0;
		while(check_index < 100)
		{
			ret = wf_io_write32(nic_info, 0x1B8, 0x12345678);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0x1B8 failed, check!!!",__func__);
				return ret;
			}
            value32 = wf_io_read32(nic_info, 0x1B8,NULL);
			LOG_D("value32(0x1B8):0x%x",value32);
			if (0x12345678 == value32)  {
				break;
			} else {
				check_index ++;
			}
		}

		if(100 == check_index)
		{
			LOG_E("[%s] power check failed, check_index:%d",__func__, check_index);
			return WF_RETURN_FAIL;
		}
		else
		{
			LOG_I("power check ok, check_index:%d",check_index);
		}
	}

    // mac回环模式测试
    //LOG_D("enabl")
//	 value32 = wf_io_read32(nic_info, REG_CR_9086X,NULL);
//	 value32 |= 0x0B000000;
//	 wf_io_write32(nic_info, REG_CR_9086X, value32);
//	 LOG_D("addr[0x0100]: 0x%08x", wf_io_read32(nic_info, REG_CR_9086X,NULL));
			 
	return ret;
}


int wf_power_on(nic_info_st *nic_info)
{
	int ret = 0;
    switch(nic_info->nic_type)
    {
        case NIC_USB:
        {
			wf_u8 temp;
            wf_u16 value16;
			wf_u32 value32;

			// 配置 0x_00AC  bit 4 写0
			temp = wf_io_read8(nic_info, 0xac,NULL);
			temp &= 0xEF;
			ret = wf_io_write8(nic_info, 0xac, temp);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0xac failed, check!!!",__func__);
				return ret;
			}
			// 配置 0x_00AC  bit 0 写0
			temp &= 0xFE;
			ret = wf_io_write8(nic_info, 0xac, temp);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0xac failed, check!!!",__func__);
				return ret;
			}
			// 配置 0x_00AC  bit 0 写1
			temp |= 0x01;
			ret = wf_io_write8(nic_info, 0xac, temp);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0xac failed, check!!!",__func__);
				return ret;
			}

            // waiting for power on
			value16 = 0;
            while(1) {
				temp = wf_io_read8(nic_info, 0xac,NULL);
				if(temp & 0x10) {
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

			// 配置 0x_00AC  bit 1 写1
			temp = wf_io_read8(nic_info, 0xac,NULL);
			temp |= 0x02;
			ret = wf_io_write8(nic_info, 0xac, temp);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0xac failed, check!!!",__func__);
				return ret;
			}

			// 写0x_00F8[4]  bit 4 写1 
			temp = wf_io_read8(nic_info, 0xf8,NULL);
			temp |= 0x10;      
			temp |= 0x40;      // 开M0的串口

		#ifdef CONFIG_FW_ENCRYPT
			temp |= 0x80;      // 加密固件
		#else
			temp &= 0x7F;      // 非加密固件
		#endif
		
			ret = wf_io_write8(nic_info, 0xf8, temp);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0xf8 failed, check!!!",__func__);
				return ret;
			}
			
			ret = wf_io_write8(nic_info, 0x98, 0xFF);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0x98 failed, check!!!",__func__);
				return ret;
			}

			ret = wf_io_write8(nic_info, 0x94, 0x1E);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0x94 failed, check!!!",__func__);
				return ret;
			}

			ret = wf_io_write8(nic_info, 0x200, 0x20);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0x200 failed, check!!!",__func__);
				return ret;
			}
			ret = wf_io_write16(nic_info, 0x202, 0x8099);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0202 failed, check!!!",__func__);
				return ret;
			}

            return WF_RETURN_OK;
        }

        case NIC_SDIO:
		{
			wf_u8 value8;
			wf_u16 value16;
			wf_u32 value32;

			wf_u16 addr;
			struct hif_node_ * hif_node = nic_info->hif_node;
			
            ret = wf_io_write8(nic_info, 0xac, (0x02 | wf_io_read8(nic_info, 0xac,NULL)));

			value8 = wf_io_read8(nic_info, 0xf8,NULL);
			
		#ifdef CONFIG_NORMAL_FPGA
			LOG_D("*************NORMAL MODE*********");
			value8 |= 0x01; 	 // set bit0  //bit0:: 1:normal ; 0:new tx/rx 
		#else
			LOG_D("new txd/rxd mode...");
			value8 &= 0xFE;
		#endif
		
			value8 |= 0x10; 	 // set bit4

		#ifdef CONFIG_FW_ENCRYPT
			value8 |= 0x80;      // 加密固件
		#else
			value8 &= 0x7F;      // 非加密固件
		#endif
		
			LOG_I("write 0x%02x -> 0xf8",value8);
		
			wf_io_write8(nic_info, 0xf8, value8);
			
			wf_io_write8(nic_info, 0x98, 0xFF);
			wf_io_write8(nic_info, 0x99, 0xFF);
			wf_io_write8(nic_info, 0x9a, 0x01);


			value8 = wf_io_read8(nic_info, 0x05,NULL);  //2020.11.6
			value8 |= 0x1;

			ret = wf_io_write8(nic_info, 0x05, value8);
			if( WF_RETURN_FAIL == ret){
				LOG_E("[%s] 0x04 failed, check!!!",__func__);
				return ret;
			}

			while(1){
				LOG_I("read8 addr[0x03]");
				value8 = wf_io_read8(nic_info, 0x03,NULL);
				LOG_I("read8 addr[0x03]=0x%08x",value8);
				if(value8 & 0x04){
					LOG_D("addr[0x00] . bit26 ==1");
					break;
				}
				wf_msleep(1000);
			}

			value8 = wf_io_read8(nic_info, 0x100,NULL);
			value8 |= 0x0f;
			ret = wf_io_write8(nic_info, 0x100, value8);

			value8 = wf_io_read8(nic_info, 0x94,NULL);
			value8 |= 0x02;
			ret = wf_io_write8(nic_info, 0x94, value8);

			value8 = wf_io_read8(nic_info, 0x94,NULL);
			value8 |= 0x5;
			ret = wf_io_write8(nic_info, 0x94, value8);

			ret = wf_io_write8(nic_info, 0x9002,0x04);
			ret = wf_io_write8(nic_info, 0x9003,0x00);
			ret = wf_io_write8(nic_info, 0x9034,0xcf);
			ret = wf_io_write8(nic_info, 0x9058,0x1);
			ret = wf_io_write8(nic_info, 0x9060,0x3);
			ret = wf_io_write8(nic_info, 0x9048,0x8);
			
			value8 = wf_io_read8(nic_info,0x9034,NULL);

			value8 = wf_io_read8(nic_info,0x9058,NULL);
			value8 = wf_io_read8(nic_info,0x9060,NULL);
			value8 = wf_io_read8(nic_info,0x9048,NULL);


			ret = wf_io_write8(nic_info, 0x904c,0xff);
			ret = wf_io_write8(nic_info, 0x904d,0xff);
			ret = wf_io_write8(nic_info, 0x904e,0xff);
			ret = wf_io_write8(nic_info, 0x904f,0xff);

			wf_sdio_tmp(nic_info);

			return WF_RETURN_OK;
		}

        default:
            LOG_E("Error Nic type");
            return WF_RETURN_FAIL;
    }

    // enable mcu-bus clk
    wf_io_read32(nic_info,0x94,NULL);       
    ret = wf_io_write32(nic_info,0x94,0x6);
    if (WF_RETURN_FAIL == ret)
    {
        LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
}


#define REG_SYS_FUNC_EN_9086X			0x0002
#define REG_MCUFWDL_9086X				0x0080
#define REG_TX_RPT_CTRL					0x04EC


int wf_power_off(nic_info_st *nic_info)
{
	int ret = 0;

    // disable mcu-bus clk
    wf_io_read32(nic_info,0x94,NULL);
	ret = wf_io_write32(nic_info,0x94,0);
    if (WF_RETURN_FAIL == ret)
    {
    	LOG_E("[%s] WF_CLK_ADDR failed,check!!!",__func__);
        return WF_RETURN_FAIL;
    }
    
    
	switch(nic_info->nic_type)
    {
        case NIC_USB:
		{
            #if 0 //power off have some issue, need check
			wf_u8  value8;
			wf_u16 value16;
            wf_bool bRet;

            // power off 
            ret = wf_io_write32(nic_info, 0x24, 0x35001fff);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] 0x24 failed, check!!!",__func__);
				return ret;
			}

            /*Stop Tx Report Timer. 0x4EC[Bit1]=b'0 */
            value8 = wf_io_read8(nic_info, REG_TX_RPT_CTRL,NULL);
	        ret = wf_io_write8(nic_info, REG_TX_RPT_CTRL, value8 & (~WF_BIT(1)));
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] REG_TX_RPT_CTRL failed, check!!!",__func__);
				return ret;
			}

            /* stop rx */
	        ret = wf_io_write8(nic_info, REG_CR_9086X, 0x0);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] REG_CR_9086X failed, check!!!",__func__);
				return ret;
			}

	        bRet=hal_pwr_seq_cmd_prasing(nic_info, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,
                                    wlan_usb_enter_lps_flow);
            if (bRet == WF_RETURN_FAIL)
                return WF_RETURN_FAIL;

            /* Reset DSP */
	        value8 = wf_io_read8(nic_info, REG_SYS_FUNC_EN_9086X + 1,NULL);
	        ret = wf_io_write8(nic_info, REG_SYS_FUNC_EN_9086X + 1, (value8 & (~WF_BIT(2))));
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] REG_SYS_FUNC_EN_9086X failed, check!!!",__func__);
				return ret;
			}

            /* reset DSP ready status */
	        ret = wf_io_write8(nic_info, REG_MCUFWDL_9086X, 0x00);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] REG_MCUFWDL_9086X failed, check!!!",__func__);
				return ret;
			}

	        bRet=hal_pwr_seq_cmd_prasing(nic_info, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,
                                    wlan_usb_card_disable_flow);
            if (bRet == WF_RETURN_FAIL)
                return WF_RETURN_FAIL;
            #endif

			return 0;
		}
		case NIC_SDIO:
		{
            wf_u8  value8;
			wf_u16 value16;

			value8 = wf_io_read8(nic_info, WF_REG_SYS_ENABLE+1,NULL);
			value8 &= ~WF_BIT(2);
			ret = wf_io_write8(nic_info, WF_REG_SYS_ENABLE+1, value8);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] WF_REG_SYS_ENABLE failed, check!!!",__func__);
				return ret;
			}
			ret = wf_io_write8(nic_info, WF_REG_MCU_DL, 0);
			if( WF_RETURN_FAIL == ret)
			{
				LOG_E("[%s] WF_REG_MCU_DL failed, check!!!",__func__);
				return ret;
			}
			if (WF_RETURN_FAIL == hal_pwr_seq_cmd_prasing
                (nic_info, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,
                wlan_sdio_card_disable_flow))
                return WF_RETURN_FAIL;

			return WF_RETURN_OK;
		}
		default:
		{
            LOG_E("Error Nic type");
            return WF_RETURN_FAIL;
		}
	}
}




