#ifndef __POWER_ON_H__
#define __POWER_ON_H__

#ifdef CONFIG_RICHV200_FPGA

#define CONFIG_FW_ENCRYPT
#define CMD_PARAM_LENGTH       12
#define TXDESC_OFFSET_NEW      20
#define TXDESC_PACK_LEN        4
#define RXDESC_OFFSET_NEW      16
#define RXDESC_PACK_LEN        4
#define TX_RX_REG_MAX_SIZE     28
#define FIRMWARE_BLOCK_SIZE    (512 -  TXDESC_OFFSET_NEW - TXDESC_PACK_LEN)
#define HIF_BULK_MSG_TIMEOUT    5000

#else

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
#endif

NDIS_STATUS wf_nic_dev_init(PADAPTER pAdapter);
NDIS_STATUS wf_nic_dev_deinit(PADAPTER pAdapter);

#endif
