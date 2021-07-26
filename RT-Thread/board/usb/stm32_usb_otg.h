/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           stm32_usb_otg.h
** Last modified Date:  2020-05-20
** Last Version:        v1.00
** Description:         USB OTG核基本操作函数实现
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo
** Created date:        2020-05-20
** Version:             v1.00
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Description:         
*********************************************************************************************************/
#ifndef __STM32_USB_OTG_H__
#define __STM32_USB_OTG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************************************
**  一些参数定义
*********************************************************************************************************/
// USB接口PHY的选择参数
#define USB_OTG_ULPI_PHY                        1U
#define USB_OTG_EMBEDDED_FS_PHY                 2U
#define USB_OTG_EMBEDDED_HS_PHY                 3U
  
// USB Core速度参数定义
#define USB_OTG_SPEED_HIGH                      0U
#define USB_OTG_SPEED_HIGH_IN_FULL              1U
#define USB_OTG_SPEED_LOW                       2U
#define USB_OTG_SPEED_FULL                      3U

// USB工作模式定义
#define USB_OTG_MODE_DEVICE                     0U
#define USB_OTG_MODE_HOST                       1U
#define USB_OTG_MODE_DRD                        2U

// USB端点类型定义
#define EP_TYPE_CTRL                            0U
#define EP_TYPE_ISOC                            1U
#define EP_TYPE_BULK                            2U
#define EP_TYPE_INTR                            3U

// 传输数据类型定义
#define HC_PID_DATA0                           0U
#define HC_PID_DATA2                           1U
#define HC_PID_DATA1                           2U
#define HC_PID_SETUP                           3U

/*********************************************************************************************************
  寄存器位值宏定义
*********************************************************************************************************/
// 寄存器: USB_OTG_GRSTCTL
#define USB_OTG_GRSTCTL_AHBIDL          (1UL << 31)
#define USB_OTG_GRSTCTL_CSRST           (1UL << 0)
// 寄存器: USB_OTG_GCCFG
#define USB_OTG_GCCFG_PWRDWN            (1UL << 16)
// 寄存器: USB_OTG_GUSBCFG
#define USB_OTG_GUSBCFG_TSDPS           (1UL << 22)
#define USB_OTG_GUSBCFG_ULPIFSLS        (1UL << 17)
#define USB_OTG_GUSBCFG_PHYSEL          (1UL << 6)
#define USB_OTG_GUSBCFG_ULPIEVBUSD      (1UL << 20)
#define USB_OTG_GUSBCFG_ULPIEVBUSI      (1UL << 21)
// 寄存器:USB_OTG_DSTS
#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     (0U << 1U)
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1U << 1U)
#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2U << 1U)
#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3U << 1U)
// AHBCFG
#define USB_OTG_GAHBCFG_GINT                   (1UL << 0)
#define USB_OTG_GAHBCFG_DMAEN                  (1UL << 5)

// 寄存器HCFG的配置参数定义,配置时钟用
#define HCFG_30_60_MHZ                          0U
#define HCFG_48_MHZ                             1U
#define HCFG_6_MHZ                              2U

// 寄存器HCCHAR
#define USB_OTG_HCCHAR_EPDIR                    (1UL << 15)
#define USB_OTG_HCCHAR_EPTYP                    (3UL << 18)
#define USB_OTG_HCCHAR_ODDFRM                   (1UL << 29)
#define USB_OTG_HCCHAR_CHDIS                    (1UL << 30)
#define USB_OTG_HCCHAR_CHENA                    (1UL << 31)

// 寄存器HCINT
#define USB_OTG_HCINT_XFRC                      (1UL << 0)
#define USB_OTG_HCINT_CHH                       (1UL << 1)
#define USB_OTG_HCINT_AHBERR                    (1UL << 2)
#define USB_OTG_HCINT_STALL                     (1UL << 3)
#define USB_OTG_HCINT_NAK                       (1UL << 4)
#define USB_OTG_HCINT_ACK                       (1UL << 5)
#define USB_OTG_HCINT_NYET                      (1UL << 6)
#define USB_OTG_HCINT_TXERR                     (1UL << 7)
#define USB_OTG_HCINT_BBERR                     (1UL << 8)
#define USB_OTG_HCINT_FRMOR                     (1UL << 9)
#define USB_OTG_HCINT_DTERR                     (1UL << 10)

// 寄存器GINTSTS
#define USB_OTG_GINTSTS_MMIS                    (1UL << 1) 
#define USB_OTG_GINTSTS_SOF                     (1UL << 3)
#define USB_OTG_GINTSTS_RXFLVL                  (1UL << 4)
#define USB_OTG_GINTSTS_NPTXFE                  (1UL << 5) 
#define USB_OTG_GINTSTS_IISOIXFR                (1UL << 20)
#define USB_OTG_GINTSTS_PXFR_INCOMPISOOUT       (1UL << 21)
#define USB_OTG_GINTSTS_HPRTINT                 (1UL << 24)
#define USB_OTG_GINTSTS_HCINT                   (1UL << 25)
#define USB_OTG_GINTSTS_PTXFE                   (1UL << 26)
#define USB_OTG_GINTSTS_DISCINT                 (1UL << 29)

// 寄存器GINTMSK
#define USB_OTG_GINTMSK_NPTXFEM                 (1UL << 5)
#define USB_OTG_GINTMSK_HCIM                    (1UL << 25)  

// 寄存器HCINTMSK
#define USB_OTG_HCINTMSK_XFRCM                  (1UL << 0) 
#define USB_OTG_HCINTMSK_CHHM                   (1UL << 1)
#define USB_OTG_HCINTMSK_AHBERR                 (1UL << 2) 
#define USB_OTG_HCINTMSK_STALLM                 (1UL << 3) 
#define USB_OTG_HCINTMSK_NAKM                   (1UL << 4) 
#define USB_OTG_HCINTMSK_ACKM                   (1UL << 5) 
#define USB_OTG_HCINTMSK_NYET                   (1UL << 6) 
#define USB_OTG_HCINTMSK_TXERRM                 (1UL << 7)
#define USB_OTG_HCINTMSK_BBERRM                 (1UL << 8)
#define USB_OTG_HCINTMSK_FRMORM                 (1UL << 9)
#define USB_OTG_HCINTMSK_DTERRM                 (1UL << 10)
#define USB_OTG_GINTMSK_PTXFEM                  (1UL << 26)

/*********************************************************************************************************
**  实现的外部函数声明
*********************************************************************************************************/
extern void usb_flush_rx(uint32_t u32Base);
extern void usb_flush_tx(uint32_t u32Base, uint32_t num);
extern void usb_core_reset(uint32_t u32Base);
extern void usb_otg_mode_set(uint32_t u32Base, uint8_t u8Mode);
extern int usb_otg_core_init(uint32_t u32Base, uint32_t u32PhyInterface, 
                             rt_bool_t bVbusSensing, rt_bool_t bDMAEnable);
extern int usb_otg_device_init(uint32_t u32Base, uint32_t u32PhyInterface, 
                               uint8_t epnum, rt_bool_t vbus);
extern void usb_otg_vbus_drive (uint32_t u32Base, uint8_t bState);
extern int usb_otg_host_init(uint32_t u32Base, uint32_t u32Speed, uint32_t u32Channels);
extern void usb_otg_data_read(uint32_t u32Base, void* buf, uint32_t len);
extern int usbdevice_speed_get(uint32_t u32Base);

#ifdef __cplusplus
    }
#endif      // __cplusplus
    
#endif      // __STM32_USB_OTG_H__
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
