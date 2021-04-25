#include "pcomp.h"
#include "wf_debug.h"
#include "wf_mix.h"
#include "wf_typedef.h"

//#include "wf_os_api.h"

// TODO: Add wf_mdelay after OS API is finished. 2021/03/02
// Temporarily do nothing with function wf_mdelay().

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

typedef enum __pwrseq_cmd_delay_unit__ {
	PWRSEQ_DELAY_US,
	PWRSEQ_DELAY_MS,
} pwrseq_cmd_delay_unit;

typedef struct __wlan_pwr_cfg__ {
	wf_u16 offset;
	wf_u8 cut_msk;
	wf_u8 fab_msk : 4;
	wf_u8 interface_msk : 4;
	wf_u8 base : 4;
	wf_u8 cmd : 4;
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


INT32 pwr_seq_cmd_prasing(PADAPTER pAdapter,
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

int power_on(PADAPTER pAdapter)
{
	UCHAR  value8;
    USHORT value16;
	UINT32 value32;

    HwPlatformIOWrite1Byte(pAdapter, 0xac, 0x06);

    HwPlatformIOWrite1Byte(pAdapter, 0x98, 0xFF);

	HwPlatformIOWrite1Byte(pAdapter, 0x99, 0xFF);
	
	HwPlatformIOWrite1Byte(pAdapter, 0x9a, 0x01);
	
     if (WF_RETURN_FAIL == pwr_seq_cmd_prasing
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

