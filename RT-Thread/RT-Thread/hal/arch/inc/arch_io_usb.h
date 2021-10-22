/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usb.h
** Last modified Date:  2017-01-30
** Last Version:        v1.0
** Description:         usb模块寄存器封装函数实现，适用于STM32F4xx
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2017-01-30
** Version:             v1.0
** Descriptions:        The original version 初始版本
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#ifndef __ARCH_IO_USB_H__
#define __ARCH_IO_USB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
** USB URB状态机状态定义
*********************************************************************************************************/
typedef enum {
  URB_IDLE = 0U,
  URB_DONE,
  URB_NOTREADY,
  URB_NYET,
  URB_ERROR,
  URB_STALL 
}eURBState;

/*********************************************************************************************************
** USB主机通道状态定义
*********************************************************************************************************/
typedef enum {
  HC_IDLE = 0U,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,  
  HC_BBLERR,   
  HC_DATATGLERR
}eUSBHostChannelState;

/*********************************************************************************************************
** STM32 USB设备断点特征结构体定义
*********************************************************************************************************/
typedef struct
{
  uint8_t   num;            /*!< Endpoint number
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15    */
  uint8_t   is_in;          /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */
  uint8_t   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */
  uint8_t   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_EP_Type_                     */
  uint8_t   data_pid_start; /*!< Initial data PID
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */
  uint8_t   even_odd_frame; /*!< IFrame parity
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
  uint16_t  tx_fifo_num;    /*!< Transmission FIFO number
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 15   */
  uint32_t  maxpacket;      /*!< Endpoint Max packet size
                                This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */
  uint8_t   *xfer_buff;     /*!< Pointer to transfer buffer                                               */
  uint32_t  dma_addr;       /*!< 32 bits aligned transfer buffer address                                  */
  uint32_t  xfer_len;       /*!< Current transfer length                                                  */
  uint32_t  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */
}tUSBEPInfo;

/*********************************************************************************************************
** STM32 USB主机通道特征结构体定义
*********************************************************************************************************/
typedef struct
{
  uint8_t   dev_addr ;     /*!< USB device address.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 255    */
  uint8_t   ch_num;        /*!< Host channel number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */
  uint8_t   ep_num;        /*!< Endpoint number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */
  uint8_t   ep_is_in;      /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
  uint8_t   speed;         /*!< USB Host speed.
                                This parameter can be any value of @ref USB_Core_Speed_                    */
  uint8_t   do_ping;       /*!< Enable or disable the use of the PING protocol for HS mode.                */
  uint8_t   process_ping;  /*!< Execute the PING protocol for HS mode.                                     */
  uint8_t   ep_type;       /*!< Endpoint Type.
                                This parameter can be any value of @ref USB_EP_Type_                       */
  uint16_t  max_packet;    /*!< Endpoint Max packet size.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 64KB   */
  uint8_t   data_pid;      /*!< Initial data PID.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
  uint8_t   *xfer_buff;    /*!< Pointer to transfer buffer.                                                */
  uint32_t  xfer_len;      /*!< Current transfer length.                                                   */
  uint32_t  xfer_count;    /*!< Partial transfer length in case of multi packet transfer.                  */
  uint8_t   toggle_in;     /*!< IN transfer current toggle flag.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
  uint8_t   toggle_out;    /*!< OUT transfer current toggle flag
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
  uint32_t  dma_addr;      /*!< 32 bits aligned transfer buffer address.                                   */
  uint32_t  ErrCnt;        /*!< Host channel error count.*/
  uint8_t   urb_state;     /*!< URB state. 
                                           This parameter can be any value of @ref USB_OTG_URBStateTypeDef */
  uint8_t   state;         /*!< Host Channel state. 
                                           This parameter can be any value of @ref USB_OTG_HCStateTypeDef  */
}tUSBHostChannelInfo;

/*********************************************************************************************************
** 函数参数定义
*********************************************************************************************************/
// USB_OTG_CORE VERSION ID
#define USB_OTG_CORE_ID_300A                    0x4F54300AU
#define USB_OTG_CORE_ID_310A                    0x4F54310AU

// USB工作模式定义
#define USB_OTG_MODE_DEVICE                     0U
#define USB_OTG_MODE_HOST                       1U
#define USB_OTG_MODE_DRD                        2U

// USB Core速度参数定义
#define USB_OTG_SPEED_HIGH                      0U
#define USB_OTG_SPEED_HIGH_IN_FULL              1U
#define USB_OTG_SPEED_LOW                       2U
#define USB_OTG_SPEED_FULL                      3U

// USB接口PHY的选择参数
#define USB_OTG_ULPI_PHY                        1U
#define USB_OTG_EMBEDDED_PHY                    2U

// USB_LL_Turnaround_Timeout Turnaround Timeout Value
#ifndef USBD_HS_TRDT_VALUE
#define USBD_HS_TRDT_VALUE                      9U
#endif /* USBD_HS_TRDT_VALUE */
#ifndef USBD_FS_TRDT_VALUE
#define USBD_FS_TRDT_VALUE                      5U
#define USBD_DEFAULT_TRDT_VALUE                 9U
#endif /* USBD_HS_TRDT_VALUE */

// USB Core Max Packet size定义
#define USB_OTG_HS_MAX_PACKET_SIZE              512U
#define USB_OTG_FS_MAX_PACKET_SIZE              64U
#define USB_OTG_MAX_EP0_SIZE                    64U

// USB端点类型定义
#define EP_TYPE_CTRL                            0U
#define EP_TYPE_ISOC                            1U
#define EP_TYPE_BULK                            2U
#define EP_TYPE_INTR                            3U

// 寄存器HCFG的配置参数定义,配置时钟用
#define HCFG_30_60_MHZ                          0U
#define HCFG_48_MHZ                             1U
#define HCFG_6_MHZ                              2U

// 主机通道配置参数
#define HC_PID_DATA0                            0U
#define HC_PID_DATA2                            1U
#define HC_PID_DATA1                            2U
#define HC_PID_SETUP                            3U

// 主机通道传输方式参数
#define HCCHAR_CTRL                             0U
#define HCCHAR_ISOC                             1U
#define HCCHAR_BULK                             2U
#define HCCHAR_INTR                             3U

// 中断状态标志位定义,对应寄存器OTG_HS_GINTSTS的各个位
#define USB_INT_WKUINT                          (1U << 31)
#define USB_INT_SRQINT                          (1U << 30)
#define USB_INT_DISCINT                         (1U << 29)
#define USB_INT_CIDSCHG                         (1U << 28)
#define USB_INT_PTXFE                           (1U << 26)
#define USB_INT_HCINT                           (1U << 25)
#define USB_INT_HPRTINT                         (1U << 24)
#define USB_INT_DATAFSUSP                       (1U << 22)
#define USB_INT_IPXFR_INCOMPISOOUT              (1U << 21)
#define USB_INT_IISOIXFR                        (1U << 20)
#define USB_INT_OEPINT                          (1U << 19)
#define USB_INT_IEPINT                          (1U << 18)
#define USB_INT_EOPF                            (1U << 15)
#define USB_INT_ISOODRP                         (1U << 14)
#define USB_INT_ENUMDNE                         (1U << 13)
#define USB_INT_USBRST                          (1U << 12)
#define USB_INT_USBSUSP                         (1U << 11)
#define USB_INT_ESUSP                           (1U << 10)
#define USB_INT_GONAKEFF                        (1U << 7)
#define USB_INT_GINAKEFF                        (1U << 6)
#define USB_INT_NPTXFE                          (1U << 5)
#define USB_INT_RXFLVL                          (1U << 4)
#define USB_INT_SOF                             (1U << 3)
#define USB_INT_OTGINT                          (1U << 2)
#define USB_INT_MMIS                            (1U << 1)
#define USB_INT_CMOD                            (1U << 0)

// USB主机通道中断状态标志位定义,对应寄存器OTG_HS_HCNT的各个位
#define USB_INT_HC_DTERR                        (1U << 10)
#define USB_INT_HC_FRMOR                        (1U << 9)
#define USB_INT_HC_BBERR                        (1U << 8)
#define USB_INT_HC_TXERR                        (1U << 7)
#define USB_INT_HC_NYET                         (1U << 6)
#define USB_INT_HC_ACK                          (1U << 5)
#define USB_INT_HC_NAK                          (1U << 4)
#define USB_INT_HC_STALL                        (1U << 3)
#define USB_INT_HC_AHBERR                       (1U << 2)
#define USB_INT_HC_CHH                          (1U << 1)
#define USB_INT_HC_XFRC                         (1U << 0)
  
/*********************************************************************************************************
** 外部函数的声明
*********************************************************************************************************/
extern int USBCoreInit(rt_uint32_t u32Base, rt_uint32_t u32PhyInterface, 
                rt_bool_t bExtVbus, rt_bool_t bBatteryChargeEnable, rt_bool_t bDMAEnable);
extern void USBGlobalIntEnable(rt_uint32_t u32Base);
extern void USBGlobalIntDisable(rt_uint32_t u32Base);
extern void USBCurrentModeSet(rt_uint32_t u32Base, rt_uint8_t u8Mode);
extern void USBDeviceInit(rt_uint32_t u32Base, rt_uint32_t u32PhyInterface, 
                   rt_uint32_t u32Speed, rt_uint32_t u32EndpointNum,
                   rt_bool_t bVbusSensing, rt_bool_t bSofEnable, rt_bool_t bDMAEnable);
extern int USBTxFifoFlush(rt_uint32_t u32Base, rt_uint32_t u32Num);
extern int USBRxFifoFlush(rt_uint32_t u32Base);
extern void USBDeviceSpeedSet(rt_uint32_t u32Base,rt_uint8_t u8Speed);
extern rt_uint8_t USBDeviceSpeedGet(rt_uint32_t u32Base);
extern void USBEndpointActivate(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern void USBDedicatedEndpointActivate(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern int USBEndpointDeactivate(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern int USBDedicatedEndpointDeactivate(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern void USBEPXferStart(rt_uint32_t u32Base, tUSBEPInfo *ep, rt_bool_t bUseDMA);
extern void USBEP0XferStart(rt_uint32_t u32Base, tUSBEPInfo *ep, rt_bool_t bUseDMA);
extern void USBPacketWrite(rt_uint32_t u32Base, rt_uint8_t ch_ep_num, 
                    rt_uint8_t *src,rt_uint16_t len, rt_bool_t bUseDMA);
extern void *USBPacketRead(rt_uint32_t u32Base, rt_uint8_t *dest,rt_uint16_t len);
extern void USBEPStallSet(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern void USBEPStallClear(rt_uint32_t u32Base, tUSBEPInfo *ep);
extern void USBDeviceStop(rt_uint32_t u32Base);
extern void USBDeviceAddressSet(rt_uint32_t u32Base, rt_uint8_t address);
extern void USBDeviceConnect(rt_uint32_t u32Base);
extern void USBDeviceDisonnect(rt_uint32_t u32Base);
rt_uint32_t USBInterruptRead(rt_uint32_t u32Base);
rt_uint32_t USBDeviceAllOutInterruptRead(rt_uint32_t u32Base);
rt_uint32_t USBDeviceAllInInterruptRead(rt_uint32_t u32Base);
rt_uint32_t USBDeviceEPOutInterruptRead(rt_uint32_t u32Base, rt_uint8_t epnum);
rt_uint32_t USBDeviceEPInInterruptRead(rt_uint32_t u32Base, rt_uint8_t epnum);
void USBInterruptClear(rt_uint32_t u32Base, rt_uint32_t u32Flag);
rt_uint32_t USBModeGet(rt_uint32_t u32Base);
void USBActivateSetup(rt_uint32_t u32Base);
void USBEP0OutStart(rt_uint32_t u32Base,rt_bool_t bUseDMA, uint8_t *pu8Setup);
void USBHostInit(rt_uint32_t u32Base, rt_uint32_t u32Speed, rt_uint32_t u32Channels, rt_bool_t bDMAEnable);
void USBFSLSPClkSelInit(rt_uint32_t u32Base, rt_uint8_t u8Freq);
void USBPortReset(rt_uint32_t u32Base);
extern void USBVbusDrive(rt_uint32_t u32Base, rt_bool_t bState);
extern uint32_t USBHostSpeedGet(rt_uint32_t u32Base);
extern uint32_t USBCurrentFrameNumberGet(rt_uint32_t u32Base);
extern void USBHostChannelInit(rt_uint32_t u32Base,
                        uint8_t ch_num,uint8_t epnum, uint8_t dev_address,
                        uint8_t speed, uint8_t ep_type, uint16_t mps);
extern void USBHostChannelXferStart(rt_uint32_t u32Base, tUSBHostChannelInfo *hc, rt_bool_t bUseDMA);
extern rt_uint32_t USBHostChannelInterruptRead(rt_uint32_t u32Base);
extern void USBHostChannelHalt(rt_uint32_t u32Base, uint8_t hc_num);
extern void USBDoPingInit(rt_uint32_t u32Base, uint8_t hc_num);
extern void USBHostStop(rt_uint32_t u32Base);
extern void USBRemoteWakeupActivate(rt_uint32_t u32Base);
extern void USBRemoteWakeupDeActivate(rt_uint32_t u32Base);


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_USB_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
