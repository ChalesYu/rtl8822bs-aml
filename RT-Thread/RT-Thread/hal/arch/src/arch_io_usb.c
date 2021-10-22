/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_usb.c
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
#include <rtthread.h>
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_usb.h"

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


/*********************************************************************************************************
** 函数声明
*********************************************************************************************************/
static void USBCoreReset(rt_uint32_t u32Base);

/*********************************************************************************************************
** Function name:       USBCoreInit
** Descriptions:        Initializes the USB Core
** input parameters:    u32Base:          指定的USB模块
**                      u32PhyInterface:  使用的PHY接口,  可以取下列值：
**                      USB_OTG_ULPI_PHY     // 外部ULPI PHY
**                      USB_OTG_EMBEDDED_PHY // 内部PHY                        
**                      bExtVbus:         Enable or disable the use of the external VBUS.
**                      bDMAEnable        Enable or disable of the USB embedded DMA.
** output parameters:   NONE
** Returned value:      >=0: 成功； <0: 失败
*********************************************************************************************************/
int USBCoreInit(rt_uint32_t u32Base, rt_uint32_t u32PhyInterface, 
                rt_bool_t bExtVbus, rt_bool_t bBatteryChargeEnable, rt_bool_t bDMAEnable)
{
  if (u32PhyInterface == USB_OTG_ULPI_PHY)
  {
    HWREG32(u32Base + USB_OTG_GCCFG) &= ~(USB_OTG_GCCFG_PWRDWN);
    
    /* Init The ULPI Interface */
    HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL); 
    
    /* Select vbus source */
    HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    if(bExtVbus == RT_TRUE)
    {
      HWREG32(u32Base + USB_OTG_GUSBCFG) |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    }
    /* Reset after a PHY select  */
    USBCoreReset(u32Base); 
  }
  else /* FS interface (embedded Phy) */
  {
    /* Select FS Embedded PHY */
    HWREG32(u32Base + USB_OTG_GUSBCFG) |= USB_OTG_GUSBCFG_PHYSEL;
    
    /* Reset after a PHY select and set Host mode */
    USBCoreReset(u32Base);
    
    if(bBatteryChargeEnable == RT_FALSE)
    {
      /* Activate the USB Transceiver */
      HWREG32(u32Base + USB_OTG_GCCFG) |= USB_OTG_GCCFG_PWRDWN;
    }
    else
    {
      /* Deactivate the USB Transceiver */
      HWREG32(u32Base + USB_OTG_GCCFG) &= ~(USB_OTG_GCCFG_PWRDWN);
    }
  }
  
  if(bDMAEnable == RT_TRUE)
  {
    HWREG32(u32Base + USB_OTG_GAHBCFG) |= (3UL << 1);
    HWREG32(u32Base + USB_OTG_GAHBCFG) |= (1UL << 5);
  }  
  
  return 0;
}

/*********************************************************************************************************
** Function name:       USB_SetTurnaroundTime
** Descriptions:         Set the USB turnaround time
** input parameters:    u32Base:          指定的USB模块
**                      hclk:             HCLK的时钟
**                      speed:            指定速度
** output parameters:   NONE
** Returned value:      USB turnaround time In PHY Clocks number
*********************************************************************************************************/
void USB_SetTurnaroundTime(uint32_t u32Base, uint32_t hclk, uint8_t speed)
{
  uint32_t UsbTrd;
  
  /* The USBTRD is configured according to the tables below, depending on AHB frequency
  used by application. In the low AHB frequency range it is used to stretch enough the USB response
  time to IN tokens, the USB turnaround time, so to compensate for the longer AHB read access
  latency to the Data FIFO */
  if (speed == USB_OTG_SPEED_FULL)
  {
    if ((hclk >= 14200000U) && (hclk < 15000000U))
    {
      /* hclk Clock Range between 14.2-15 MHz */
      UsbTrd = 0xFU;
    }
    else if ((hclk >= 15000000U) && (hclk < 16000000U))
    {
      /* hclk Clock Range between 15-16 MHz */
      UsbTrd = 0xEU;
    }
    else if ((hclk >= 16000000U) && (hclk < 17200000U))
    {
      /* hclk Clock Range between 16-17.2 MHz */
      UsbTrd = 0xDU;
    }
    else if ((hclk >= 17200000U) && (hclk < 18500000U))
    {
      /* hclk Clock Range between 17.2-18.5 MHz */
      UsbTrd = 0xCU;
    }
    else if ((hclk >= 18500000U) && (hclk < 20000000U))
    {
      /* hclk Clock Range between 18.5-20 MHz */
      UsbTrd = 0xBU;
    }
    else if ((hclk >= 20000000U) && (hclk < 21800000U))
    {
      /* hclk Clock Range between 20-21.8 MHz */
      UsbTrd = 0xAU;
    }
    else if ((hclk >= 21800000U) && (hclk < 24000000U))
    {
      /* hclk Clock Range between 21.8-24 MHz */
      UsbTrd = 0x9U;
    }
    else if ((hclk >= 24000000U) && (hclk < 27700000U))
    {
      /* hclk Clock Range between 24-27.7 MHz */
      UsbTrd = 0x8U;
    }
    else if ((hclk >= 27700000U) && (hclk < 32000000U))
    {
      /* hclk Clock Range between 27.7-32 MHz */
      UsbTrd = 0x7U;
    }
    else /* if(hclk >= 32000000) */
    {
      /* hclk Clock Range between 32-200 MHz */
      UsbTrd = 0x6U;
    }
  }
  else if (speed == USB_OTG_SPEED_HIGH)
  {
    UsbTrd = USBD_HS_TRDT_VALUE;
  }
  else
  {
    UsbTrd = USBD_DEFAULT_TRDT_VALUE;
  }
  
  HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(0xFUL <<10);
  HWREG32(u32Base + USB_OTG_GUSBCFG) |= (uint32_t)((UsbTrd << 10) & (0xFUL <<10));
}

/*********************************************************************************************************
** Function name:       USBGlobalIntEnable
** Descriptions:        Enables the controller's Global Int in the AHB Config reg
** input parameters:    u32Base:          指定的USB模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBGlobalIntEnable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + USB_OTG_GAHBCFG)  |= (1UL << 0);
}

/*********************************************************************************************************
** Function name:       USBGlobalIntDisable
** Descriptions:        Disable the controller's Global Int in the AHB Config reg
** input parameters:    u32Base:          指定的USB模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBGlobalIntDisable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + USB_OTG_GAHBCFG)  &= ~(1UL << 0);
}

/*********************************************************************************************************
** Function name:       USBCurrentModeSet
** Descriptions:        Set functional mode
** input parameters:    u32Base:          指定的USB模块
**                      u8Mode:  指定工作模式，可以取下列值
**                      USB_OTG_MODE_DEVICE             // 设备模式
**                      USB_OTG_MODE_HOST               // 主机模式
**                      USB_OTG_MODE_DRD                // 双角色模式
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBCurrentModeSet(rt_uint32_t u32Base, rt_uint8_t u8Mode)
{
  HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~((1UL << 30) | (1UL << 29));
  
  if (u8Mode == USB_OTG_MODE_HOST)
  {
    HWREG32(u32Base + USB_OTG_GUSBCFG) |= (1UL << 29); 
  }
  else if ( u8Mode == USB_OTG_MODE_DEVICE)
  {
    HWREG32(u32Base + USB_OTG_GUSBCFG) |= (1UL << 30);  
  }
  rt_thread_mdelay(60U);
}

/*********************************************************************************************************
** Function name:       USBDeviceInit
** Descriptions:        Initializes the USB_OTG controller registers for device mode
** input parameters:    u32Base:          指定的USB模块
**                      u32PhyInterface:  使用的PHY接口,  可以取下列值：
**                      USB_OTG_ULPI_PHY     // 外部ULPI PHY
**                      USB_OTG_EMBEDDED_PHY // 内部PHY 
**                      u32Speed:         PHY速度配置， 可以取下列值：
**                      USB_OTG_SPEED_HIGH           // 高速
**                      USB_OTG_SPEED_HIGH_IN_FULL   // 高速中的全速模式
**                      USB_OTG_SPEED_LOW            // 低速
**                      USB_OTG_SPEED_FULL           // 全速
**                      u32EndpointNum:    Device Endpoints number.
                                           This parameter depends on the used USB core.   
                                           This parameter must be a number between Min_Data = 1 and Max_Data = 15
**                      bVbusSensing:      Enable or disable the VBUS Sensing feature.  
**                      bSofEnable:        Enable or disable the output of the SOF signal
**                      bDMAEnable:        Enable or disable of the USB embedded DMA.             
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceInit(rt_uint32_t u32Base, rt_uint32_t u32PhyInterface, 
                   rt_uint32_t u32Speed, rt_uint32_t u32EndpointNum,
                   rt_bool_t bVbusSensing, rt_bool_t bSofEnable, rt_bool_t bDMAEnable)
{
  rt_uint32_t i = 0U;
  volatile uint32_t *preg;
  
  preg = (uint32_t *)u32Base + USB_OTG_DIEPTXF1;
  for (i = 0U; i < 15U; i++)
  {
    preg[i] = 0U;
  }
  
  /* VBUS Sensing setup */
  if (bVbusSensing == RT_FALSE)
  {
    HWREG32(u32Base + USB_OTG_DCTL) |= (1UL << 1);
    /* Deactivate VBUS Sensing B */
    HWREG32(u32Base + USB_OTG_GCCFG) &= ~(1UL << 21);
    
    /* B-peripheral session valid override enable*/ 
    HWREG32(u32Base + USB_OTG_GOTGCTL) |= (1UL << 6);
    HWREG32(u32Base + USB_OTG_GOTGCTL) |= (1UL << 7);
  } else {
    /* Enable HW VBUS sensing */
    HWREG32(u32Base + USB_OTG_GCCFG) |= (1UL << 21);
  }
  
  /* Restart the Phy Clock */
  HWREG32(u32Base + USB_OTG_PCGCCTL) = 0U;
  
  /* Device mode configuration */
  HWREG32(u32Base + USB_OTG_DCFG) |= (0UL << 11);  // 80% of the frame interval
  
  if(u32PhyInterface  == USB_OTG_ULPI_PHY)
  {
    if(u32Speed == USB_OTG_SPEED_HIGH)
    {      
      /* Set High speed phy */
      USBDeviceSpeedSet(u32Base, USB_OTG_SPEED_HIGH);
    }
    else 
    {
      /* set High speed phy in Full speed mode */
      USBDeviceSpeedSet(u32Base, USB_OTG_SPEED_HIGH_IN_FULL);
    }
  }
  else
  {
    /* Set Full speed phy */
    USBDeviceSpeedSet(u32Base, USB_OTG_SPEED_FULL);
  }
  
  /* Flush the FIFOs */
  USBTxFifoFlush(u32Base, 0x10U); /* all Tx FIFOs */
  USBRxFifoFlush(u32Base);
  
  /* Clear all pending Device Interrupts */
  HWREG32(u32Base + USB_OTG_DIEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DOEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DAINTMSK) = 0U;
  
  for (i = 0U; i <u32EndpointNum; i++)
  {
    if((HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20 * i) & (1UL << 31)) == (1UL << 31)) {
      if(i == 0) {
        HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20 * i) = (1UL << 27);
      } else {
        HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20 * i) = ((1UL << 30) | (1UL << 27));
      }
    } else {
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20 * i) = 0U;
    }
    
    HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20 * i) = 0;
    HWREG32(u32Base + USB_OTG_DIEPINT0 + 0x20 * i) = 0xFB7FU;
  }
  
  for (i = 0U; i <u32EndpointNum; i++)
  {
    if((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) & (1UL << 31)) == (1UL << 31)) {
      if(i == 0) {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = (1UL << 27);
      } else {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = ((1UL << 30) | (1UL << 27));
      }
    } else {
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = 0;
    }
    
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20 * i) = 0;
    HWREG32(u32Base + USB_OTG_DOEPINT0 + 0x20 * i) = 0xFB7FU;
  }
  
  HWREG32(u32Base + USB_OTG_DIEPMSK) &= ~(1UL << 8);
  
  /* Disable all interrupts. */
  HWREG32(u32Base + USB_OTG_GINTMSK) = 0U;
  
  /* Clear any pending interrupts */
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xBFFFFFFFU;
  
  /* Enable the common interrupts */
  if (bDMAEnable == RT_FALSE)
  {
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1U << 4); 
  }
  
  /* Enable interrupts matching to the Device mode ONLY */
  HWREG32(u32Base + USB_OTG_GINTMSK) |= ((1U << 11) | (1U << 12) | (1U << 13) | (1U << 18) |\
                                        (1U << 19)   | (1U << 20)| (1U << 21) | (1U << 31));
  
  if(bSofEnable)
  {
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1U << 3);
  }
  
  if (bVbusSensing == RT_TRUE)
  {
    HWREG32(u32Base + USB_OTG_GINTMSK) |= ((1U << 30) | (1U << 2)); 
  }
}

/*********************************************************************************************************
** Function name:       USBTxFifoFlush
** Descriptions:        Flush a Tx FIFO
** input parameters:    u32Base:        指定的USB模块
**                      u32Num:         FIFO数量             
** output parameters:   NONE
** Returned value:      >=0: 成功； <0: 失败
*********************************************************************************************************/
int USBTxFifoFlush(rt_uint32_t u32Base, rt_uint32_t u32Num)
{
//  rt_uint32_t u32Count = 0;
//  
//  HWREG32(u32Base + USB_OTG_GRSTCTL) = ((1U << 5) | (rt_uint32_t)(u32Num << 6));  
//  
//  do
//  {
//    if (++u32Count > 200000)
//    {
//      return -1;
//    }
//  } while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 5)) == (1U << 5));
  
  HWREG32(u32Base + USB_OTG_GRSTCTL) = ((1U << 5) | (rt_uint32_t)(u32Num << 6)); 
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 5)) == (1U << 5));
  return 0;
}

/*********************************************************************************************************
** Function name:       USBRxFifoFlush
** Descriptions:        Flush a Rx FIFO
** input parameters:    u32Base:        指定的USB模块            
** output parameters:   NONE
** Returned value:      >=0: 成功； <0: 失败
*********************************************************************************************************/
int USBRxFifoFlush(rt_uint32_t u32Base)
{
//  rt_uint32_t u32Count = 0;
//  
//  HWREG32(u32Base + USB_OTG_GRSTCTL) = (1U << 4);  
//  
//  do
//  {
//    if (++u32Count > 200000)
//    {
//      return -1;
//    }
//  } while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 4)) == (1U << 4));
  
  HWREG32(u32Base + USB_OTG_GRSTCTL) = (1U << 4);
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 4)) == (1U << 4));
  
  return 0;
}

/*********************************************************************************************************
** Function name:       USBDeviceSpeedSet
** Descriptions:        Initializes the DevSpd field of DCFG register 
**                      depending the PHY type and the enumeration speed of the device.
** input parameters:    u32Base:        指定的USB模块  
**                      u8Speed:        设置的USB速度，可以取下列值：
**                      USB_OTG_SPEED_HIGH             // 高速
**                      USB_OTG_SPEED_HIGH_IN_FULL     // 高速中的full模式
**                      USB_OTG_SPEED_LOW              // 低速
**                      USB_OTG_SPEED_FULL             // 全速模式           
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceSpeedSet(rt_uint32_t u32Base,rt_uint8_t u8Speed)
{
   HWREG32(u32Base + USB_OTG_DCFG) |= u8Speed;
}

/*********************************************************************************************************
** Function name:       USBDeviceSpeedGet
** Descriptions:        Return the  Dev Speed,与函数USBDeviceSpeedSet对应
** input parameters:    u32Base:        指定的USB模块          
** output parameters:   NONE
** Returned value:      USBD_HS_SPEED      //  高速
**                      USBD_FS_SPEED      //  全速
*********************************************************************************************************/
rt_uint8_t USBDeviceSpeedGet(rt_uint32_t u32Base)
{
  rt_uint8_t u8Speed = 0U;
  uint32_t DevEnumSpeed = HWREG32(u32Base + USB_OTG_DSTS) & (3U << 1);

  if(DevEnumSpeed == DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ)
  {
    u8Speed = USB_OTG_SPEED_HIGH;
  }
  else if ((DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ)||
            (DevEnumSpeed == DSTS_ENUMSPD_FS_PHY_48MHZ))
  {
    u8Speed = USB_OTG_SPEED_FULL;
  }
  else
  {
    u8Speed = 0xFU;
  }
  
  return u8Speed;
}

/*********************************************************************************************************
** Function name:       USBEndpointActivate
** Descriptions:        Activate and configure an endpoint
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构      
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEndpointActivate(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num & 0xFU;
  
  if (ep->is_in == 1U)
  {
    HWREG32(u32Base + USB_OTG_DAINTMSK) |= (0x0000FFFFU & (1UL << epnum));
    
    if ((HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) & (1UL << 15)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((ep->maxpacket & 0x7FFU) |\
                                                                (((rt_uint32_t)ep->type) << 18U) |\
                                                                (((rt_uint32_t)epnum) << 22) |\
                                                                (1UL << 28) |\
                                                                (1UL << 15));
    } 
  }
  else
  {
    HWREG32(u32Base + USB_OTG_DAINTMSK) |= (0xFFFF0000U & ((1UL << epnum) << 16));
    
    if ((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) & (1UL << 15)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= ((ep->maxpacket & 0x7FFU) |\
                                                                (((rt_uint32_t)ep->type) << 18U) |\
                                                                (1UL << 28) |
                                                                (1UL << 15));
    } 
  }
}

/*********************************************************************************************************
** Function name:       USBDedicatedEndpointActivate
** Descriptions:        Activate and configure a dedicated endpoint
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构        
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDedicatedEndpointActivate(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num & 0xFU;
  
  if (ep->is_in == 1U)
  {
    if ((HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) & (1UL << 15)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((ep->maxpacket & 0x7FFU) |\
                                                                (((rt_uint32_t)ep->type) << 18U) |\
                                                                (((rt_uint32_t)epnum) << 22) |\
                                                                (1UL << 28) |\
                                                                (1UL << 15));
    }
    
    HWREG32(u32Base + USB_OTG_DEACHINTMSK) |= (0x0000FFFFU & (1UL << epnum));
  }
  else
  { 
    if ((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) & (1UL << 15)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= ((ep->maxpacket & 0x7FFU) |\
                                                                (((rt_uint32_t)ep->type) << 18U) |\
                                                                (((rt_uint32_t)epnum) << 22) |\
                                                                (1UL << 15));
    }
    
    HWREG32(u32Base + USB_OTG_DEACHINTMSK) |= (0xFFFF0000U & ((1UL << epnum) << 16));
  }
}

/*********************************************************************************************************
** Function name:       USBEndpointDeactivate
** Descriptions:        De-activate and de-initialize an endpoint
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构        
** output parameters:   NONE
** Returned value:      >=0: 成功； <0: 失败
*********************************************************************************************************/
int USBEndpointDeactivate(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num & 0xFU;
  
  /* Disable the IN endpoint */
  if (ep->is_in == 1U)
  {
    /* Disable endpoint interrupts */
    HWREG32(u32Base + USB_OTG_DEACHINTMSK) &= ~(0x0000FFFFU & (1UL << epnum)); 
    HWREG32(u32Base + USB_OTG_DAINTMSK) &= ~(0x0000FFFFU & (1UL << epnum)); 
    HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) &= ~((1UL << 15) |
                                                                (0x7FFUL << 0) |
                                                                (0xFUL << 22) |
                                                                (1UL << 28) |
                                                                (3UL << 18));
  }
  else /* Disable the OUT endpoint */
  {
    /* Disable endpoint interrupts */
    HWREG32(u32Base + USB_OTG_DEACHINTMSK) &= ~(0xFFFF0000U & ((1U << epnum) << 16U));      
    HWREG32(u32Base + USB_OTG_DAINTMSK) &= ~(0xFFFF0000U & ((1U << epnum) << 16U));
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) &= ~((1UL << 15) |
                                                                (0x7FFUL << 0) |
                                                                (1UL << 28) |
                                                                (3UL << 18));  
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       USBDedicatedEndpointDeactivate
** Descriptions:        De-activate and de-initialize a dedicated endpoint
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构          
** output parameters:   NONE
** Returned value:      >=0: 成功； <0: 失败
*********************************************************************************************************/
int USBDedicatedEndpointDeactivate(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num & 0xFU;
  
  /* Disable the IN endpoint */
  if (ep->is_in == 1U)
  {
    HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) &= ~(1UL << 15);
    
    /* Disable endpoint interrupts */
    HWREG32(u32Base + USB_OTG_DAINTMSK) &= ~(0x0000FFFFU & (1UL << epnum));   
  }
  else /* Disable the OUT endpoint */
  {
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) &= ~(1UL << 15); 
    
    /* Disable endpoint interrupts */
    HWREG32(u32Base + USB_OTG_DAINTMSK) &= ~(0xFFFF0000U & ((1U << epnum) << 16U));             
  }
  return 0;
}

/*********************************************************************************************************
** Function name:       USBEPXferStart
** Descriptions:        setup and starts a transfer over an EP
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构
**                      bUseDMA：       是否使用DMA传输
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEPXferStart(rt_uint32_t u32Base, tUSBEPInfo *ep, rt_bool_t bUseDMA)
{
  rt_uint16_t u16PktCnt = 0U;
  uint32_t epnum = ep->num & 0xFU;
  
  /* IN endpoint */
  if (ep->is_in == 1U)
  {
    /* Zero Length Packet? */
    if (ep->xfer_len == 0U)
    {
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x3FFU << 19);
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~((0x3FFU << 19) & (1U << 19));
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x7FFFFU << 0);
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x7FFFFU << 0);
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x3FFU << 19);
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) |= 
        ((0x3FFU << 19) & (((ep->xfer_len + ep->maxpacket - 1U)/ ep->maxpacket) << 19U)) ;
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) |= ((0x7FFFFU << 0) & ep->xfer_len); 
      
      if (ep->type  == EP_TYPE_ISOC)
      {
        HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= (3U << 29);
        HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) |= ((3U << 29) & (1U << 29U));  
      }       
    }
    
    if (bUseDMA == RT_TRUE)
    {
      if((uint32_t)ep->dma_addr != 0) {
        HWREG32(u32Base + USB_OTG_DIEPDMA0 + 0x20UL * epnum) = (uint32_t)(ep->dma_addr);
      }
      if (ep->type == EP_TYPE_ISOC)
      {
        if ((HWREG32(u32Base + USB_OTG_DSTS) & (1U << 8)) == 0U)
        {
          HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 29);
        }
        else
        {
          HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 28);
        }
      }

      /* EP enable, IN data in FIFO */
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((1U<< 26) | (1U << 31));
    }
    else
    {
      /* EP enable, IN data in FIFO */
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((1U<< 26) | (1U << 31));
      
      if (ep->type  != EP_TYPE_ISOC)
      {
        /* Enable the Tx FIFO Empty Interrupt for this EP */
        if (ep->xfer_len > 0U)
        {
          HWREG32(u32Base + USB_OTG_DIEPEMPMSK) |= (1U << epnum);
        }
      } else {
        if ((HWREG32(u32Base + USB_OTG_DSTS) & (1U << 8)) == 0U)
        {
          HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 29);
        }
        else
        {
          HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 28);
        }
        USBPacketWrite(u32Base, ep->num, ep->xfer_buff, ep->xfer_len, bUseDMA);   
      }
    }   
  }
  else /* OUT endpoint */
  {
    /* Program the transfer size and packet count as follows:
    * pktcnt = N
    * xfersize = N * maxpacket
    */  
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) &= ~((0x7FFFFU << 0));
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) &= ~((0x3FFU << 19));
    
    if (ep->xfer_len == 0U)
    {
      HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x7FFFFU << 0) & ep->maxpacket);
      HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x3FFU << 19) & (1U << 19U));
    }
    else
    {
      u16PktCnt = (ep->xfer_len + ep->maxpacket - 1U)/ ep->maxpacket; 
      HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x3FFU << 19) & (u16PktCnt << 19U));
      HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x7FFFFU << 0) & (ep->maxpacket * u16PktCnt));
    }
    
    if (bUseDMA == RT_TRUE)
    {
      if ((uint32_t)ep->xfer_buff != 0U) {
        HWREG32(u32Base + USB_OTG_DOEPDMA0 + 0x20UL * epnum) = (uint32_t)ep->xfer_buff;
      }
    }
    
    if (ep->type == EP_TYPE_ISOC)
    {
      if ((HWREG32(u32Base + USB_OTG_DSTS) & ( 1U << 8U )) == 0U)
      {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= (1U << 29);
      }
      else
      {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= (1U << 28);
      }
    }
    /* EP enable */
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= ((1U << 26) | (1U << 31));
  }
}

/*********************************************************************************************************
** Function name:       USBEP0XferStart
** Descriptions:        setup and starts a transfer over the EP  0
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构
**                      bUseDMA：       是否使用DMA传输
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEP0XferStart(rt_uint32_t u32Base, tUSBEPInfo *ep, rt_bool_t bUseDMA)
{
  uint32_t epnum = ep->num & 0xFU;
  
  /* IN endpoint */
  if (ep->is_in == 1U)
  {
    /* Zero Length Packet? */
    if (ep->xfer_len == 0U)
    {
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x3FFU << 19);
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~((0x3FFU << 19) & (1U << 19));
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x7FFFFU << 0);
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x7FFFFU << 0);
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) &= ~(0x3FFU << 19);

      if(ep->xfer_len > ep->maxpacket)
      {
        ep->xfer_len = ep->maxpacket;
      }
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) |= ((0x3FFU << 19) & (1U << 19U)) ;
      HWREG32(u32Base + USB_OTG_DIEPTSIZ0 + 0x20UL * epnum) |= ((0x7FFFFU << 0) & ep->xfer_len);  
    }
    
    if (bUseDMA == RT_TRUE)
    {
      if ((uint32_t)ep->dma_addr != 0U) {
        HWREG32(u32Base + USB_OTG_DIEPDMA0 + 0x20UL * epnum) = (uint32_t)(ep->dma_addr);
      }
      /* EP enable, IN data in FIFO */
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((1U << 26) | (1U << 31));    
    }
    else
    {
      /* EP enable, IN data in FIFO */
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= ((1U << 26) | (1U << 31));    
    
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (ep->xfer_len > 0U)
      {
        HWREG32(u32Base + USB_OTG_DIEPEMPMSK) |= (1U << ep->num);
      }
    }
  }
  else /* OUT endpoint */
  {
    /* Program the transfer size and packet count as follows:
    * pktcnt = N
    * xfersize = N * maxpacket
    */
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) &= ~((0x7FFFFU << 0));
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) &= ~((0x3FFU << 19));
    
    if (ep->xfer_len > 0U)
    {
      ep->xfer_len = ep->maxpacket;
    }
    
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x3FFU << 19) & (1U << 19U));
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * epnum) |= ((0x7FFFFU << 0) & ep->maxpacket);
    
    if (bUseDMA == RT_TRUE)
    { 
      if ((uint32_t)ep->xfer_buff != 0U) {
        HWREG32(u32Base + USB_OTG_DOEPDMA0 + 0x20UL * epnum) = (uint32_t)ep->xfer_buff;
      }
    }
    
    /* EP enable */
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= ((1U << 26) | (1U << 31));    
  }
}

/*********************************************************************************************************
** Function name:       USBPacketWrite
** Descriptions:        Writes a packet into the Tx FIFO associated with the EP/channel
** input parameters:    u32Base:        指定的USB模块  
**                      ch_ep_num:      endpoint or host channel number
**                      src:            pointer to source buffer
**                      len:            Number of bytes to write
**                      bUseDMA：       是否使用DMA传输
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBPacketWrite(rt_uint32_t u32Base, rt_uint8_t ch_ep_num, 
                    rt_uint8_t *src,rt_uint16_t len, rt_bool_t bUseDMA)
{
  rt_uint32_t u32Count = 0U , i = 0U;
  uint32_t *pSrc = (uint32_t *)src;
  
  if (bUseDMA == RT_FALSE)
  {
    u32Count =  (len + 3U) / 4U;
    for (i = 0U; i < u32Count; i++)
    {
      HWREG32(u32Base + USB_OTG_FIFO_BASE + (ch_ep_num * USB_OTG_FIFO_SIZE)) = *((__packed rt_uint32_t *)pSrc);
      pSrc++;
    }
  }
}

/*********************************************************************************************************
** Function name:       USBPacketRead
** Descriptions:        Read a packet from the Rx FIFO
** input parameters:    u32Base:        指定的USB模块  
**                      dest:           pointer to dest buffer
**                      len:            Number of bytes to read
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void *USBPacketRead(rt_uint32_t u32Base, rt_uint8_t *dest,rt_uint16_t len)
{
  rt_uint32_t i=0U;
  rt_uint32_t u32Count = (len + 3U) / 4U;
  uint32_t *pDest = (uint32_t *)dest;
  
  for (i = 0U; i < u32Count; i++)
  {
    *(__packed uint32_t *)pDest = HWREG32(u32Base + USB_OTG_FIFO_BASE + (0 * USB_OTG_FIFO_SIZE));
    pDest++;
  }
  return ((void *)pDest);
}

/*********************************************************************************************************
** Function name:       USBEPStallSet
** Descriptions:        set a stall condition over an EP
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEPStallSet(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num;
  
  if (ep->is_in == 1U)
  {
    if((((HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum)) & (1UL << 31)) == 0U) && (epnum != 0UL))
    {
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) &= ~(1UL << 30);
    }
    HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 21);
  }
  else
  {
    if((((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum)) & (1UL << 31)) == 0U) && (epnum != 0UL))
    {
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL *epnum) &= ~(1UL << 30); 
    } 
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= (1UL << 21);
  }
}

/*********************************************************************************************************
** Function name:       USBEPStallClear
** Descriptions:        Clear a stall condition over an EP
** input parameters:    u32Base:        指定的USB模块  
**                      ep:             USB断点描述结构
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEPStallClear(rt_uint32_t u32Base, tUSBEPInfo *ep)
{
  uint32_t epnum = ep->num;
  
  if (ep->is_in == 1U)
  {
    HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) &= ~(1UL << 21);
    if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
    {
      HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * epnum) |= (1UL << 28);  /* DATA0 */
    }    
  }
  else
  {
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) &= ~(1UL << 21);
    if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
    {
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * epnum) |= (1UL << 28);  /* DATA0 */
    }    
  }
}

/*********************************************************************************************************
** Function name:       USBDeviceStop
** Descriptions:        Stop the usb device mode
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceStop(rt_uint32_t u32Base)
{
  uint32_t i;
  
  /* Clear Pending interrupt */
  for (i = 0U; i < 15U ; i++)
  {
    HWREG32(u32Base + USB_OTG_DIEPINT0 + 0x20UL * i) = 0xFB7FU;
    HWREG32(u32Base + USB_OTG_DOEPINT0 + 0x20UL * i) = 0xFB7FU;
  }
  
  /* Clear interrupt masks */
  HWREG32(u32Base + USB_OTG_DIEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DOEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DAINTMSK) = 0U;
  
  /* Flush the FIFO */
  USBRxFifoFlush(u32Base);
  USBTxFifoFlush(u32Base, 0x10U);  
}

/*********************************************************************************************************
** Function name:       USBDeviceAddressSet
** Descriptions:        Set the usb device address
** input parameters:    u32Base:        指定的USB模块  
**                      address:        new device address to be assigned
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceAddressSet(rt_uint32_t u32Base, rt_uint8_t address)
{
  HWREG32(u32Base + USB_OTG_DCFG) &= ~(0x7FU << 4);
  HWREG32(u32Base + USB_OTG_DCFG) |= ((address << 4U) & (0x7FU << 4));
}

/*********************************************************************************************************
** Function name:       USBDeviceConnect
** Descriptions:        Connect the USB device by enabling the pull-up/pull-down
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceConnect(rt_uint32_t u32Base)
{
  HWREG32(u32Base + USB_OTG_DCTL) &= ~(1UL << 1);
  rt_thread_mdelay(10U);
}

/*********************************************************************************************************
** Function name:       USBDeviceDisonnect
** Descriptions:        Disconnect the USB device by disabling the pull-up/pull-down
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDeviceDisonnect(rt_uint32_t u32Base)
{
  HWREG32(u32Base + USB_OTG_DCTL) |= (1UL << 1);
  rt_thread_mdelay(10U);
}

/*********************************************************************************************************
** Function name:       USBInterruptRead
** Descriptions:        return the global USB interrupt status
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBInterruptRead(rt_uint32_t u32Base)
{
  uint32_t v;
  
  v = HWREG32(u32Base + USB_OTG_GINTSTS);
  v &= HWREG32(u32Base + USB_OTG_GINTMSK);
  
  return v;  
}

/*********************************************************************************************************
** Function name:       USBDeviceAllOutInterruptRead
** Descriptions:        return the USB device OUT endpoints interrupt status
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBDeviceAllOutInterruptRead(rt_uint32_t u32Base)
{
  uint32_t v;
  
  v  = HWREG32(u32Base + USB_OTG_DAINT);
  v &= HWREG32(u32Base + USB_OTG_DAINTMSK);
  
  return ((v & 0xffff0000U) >> 16U);
}

/*********************************************************************************************************
** Function name:       USBDeviceAllInInterruptRead
** Descriptions:        return the USB device IN endpoints interrupt status
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBDeviceAllInInterruptRead(rt_uint32_t u32Base)
{
  uint32_t v;
  
  v  = HWREG32(u32Base + USB_OTG_DAINT);
  v &= HWREG32(u32Base + USB_OTG_DAINTMSK);
  
  return ((v & 0xFFFFU));
}

/*********************************************************************************************************
** Function name:       USBDeviceEPOutInterruptRead
** Descriptions:        Returns Device OUT EP Interrupt register
** input parameters:    u32Base:        指定的USB模块
**                      epnum:          endpoint number,This parameter can be a value from 0 to 15
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBDeviceEPOutInterruptRead(rt_uint32_t u32Base, rt_uint8_t epnum)
{
  uint32_t v;
  
  v  = HWREG32(u32Base + USB_OTG_DOEPINT0 + 0x20UL * epnum);
  v &= HWREG32(u32Base + USB_OTG_DOEPMSK);
  
  return v;
}

/*********************************************************************************************************
** Function name:       USBDeviceEPInInterruptRead
** Descriptions:        Returns Device IN EP Interrupt register
** input parameters:    u32Base:        指定的USB模块
**                      epnum:          endpoint number,This parameter can be a value from 0 to 15
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBDeviceEPInInterruptRead(rt_uint32_t u32Base, rt_uint8_t epnum)
{
  uint32_t v, msk, emp;
  
  msk = HWREG32(u32Base + USB_OTG_DIEPMSK);
  emp = HWREG32(u32Base + USB_OTG_DIEPEMPMSK);
  msk |= ((emp >> (epnum & 0xFU)) & 0x1U) << 7U;
  v = HWREG32(u32Base + USB_OTG_DIEPINT0 + 0x20UL * epnum) & msk;
  
  return v;
}

/*********************************************************************************************************
** Function name:       USBInterruptClear
** Descriptions:        clear a USB interrupt
** input parameters:    u32Base:        指定的USB模块  
**                      u32Flag:         interrupt flag
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBInterruptClear(rt_uint32_t u32Base, rt_uint32_t u32Flag)
{
  HWREG32(u32Base + USB_OTG_GINTSTS) |= u32Flag;  
}

/*********************************************************************************************************
** Function name:       USBModeGet
** Descriptions:        Returns USB core mode
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      return core mode : Host or Device
**                      This parameter can be one of these values:
**                      0 : Host 
**                      1 : Device
*********************************************************************************************************/
rt_uint32_t USBModeGet(rt_uint32_t u32Base)
{
   
  return (HWREG32(u32Base + USB_OTG_GINTSTS) & 0x1U);
}

/*********************************************************************************************************
** Function name:       USBActivateSetup
** Descriptions:        Activate EP0 for Setup transactions
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBActivateSetup(rt_uint32_t u32Base)
{
  /* Set the MPS of the IN EP based on the enumeration speed */
  HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20UL * 0) &= ~(0x7FFU << 0);
  
  HWREG32(u32Base + USB_OTG_DCTL) |= (1UL << 8);
}

/*********************************************************************************************************
** Function name:       USBEP0OutStart
** Descriptions:        Prepare the EP0 to start the first control setup
** input parameters:    u32Base:        指定的USB模块  
**                      bUseDMA：       是否使用DMA传输
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBEP0OutStart(rt_uint32_t u32Base,rt_bool_t bUseDMA, uint8_t *pu8Setup)
{
  if(HWREG32(u32Base + USB_OTG_GSNPSID) > USB_OTG_CORE_ID_300A) {
    if((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * 0) & (1UL << 31)) == (1UL << 31)) {
      return;
    }
  }
  
  HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * 0) = 0U;
  HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * 0) |= ((0x3FFU << 19) & (1U << 19U));
  HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * 0) |= (3U * 8U);
  HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20UL * 0) |=  (3U << 29); 
  
  if (bUseDMA == RT_TRUE)
  {
    HWREG32(u32Base + USB_OTG_DOEPDMA0 + 0x20UL * 0) = (uint32_t)pu8Setup;
    /* EP enable */
    HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20UL * 0) |= 0x80008000U;
  }
}

/*********************************************************************************************************
** Function name:       USBCoreReset
** Descriptions:        Reset the USB Core (needed after USB clock settings change)
** input parameters:    u32Base:    指定复位的USB模块
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
static void USBCoreReset(rt_uint32_t u32Base)
{
//  rt_uint32_t u32Count;
//
//  /* Wait for AHB master IDLE state. */
//  u32Count = 0;
//  do
//  {
//    if (++u32Count > 200000U)
//    {
//      return;
//    }
//  }
//  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_AHBIDL) == 0U);
//  
//  /* Core Soft Reset */
//  u32Count = 0U;
//  HWREG32(u32Base + USB_OTG_GRSTCTL) |= USB_OTG_GRSTCTL_CSRST;
//
//  do
//  {
//    if (++u32Count > 200000U)
//    {
//      return;
//    }
//  }
//  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);

  /* Wait for AHB master IDLE state. */
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_AHBIDL) == 0U);
  /* Core Soft Reset */
  HWREG32(u32Base + USB_OTG_GRSTCTL) |= USB_OTG_GRSTCTL_CSRST;
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

/*********************************************************************************************************
** Function name:       USBHostInit
** Descriptions:        Initializes the USB_OTG controller registers for host mode
** input parameters:    u32Base:          指定的USB模块
**                      u32Speed:         PHY速度配置， 可以取下列值：
**                              USB_OTG_SPEED_HIGH           // 高速
**                              USB_OTG_SPEED_HIGH_IN_FULL   // 高速中的全速模式
**                              USB_OTG_SPEED_LOW            // 低速
**                              USB_OTG_SPEED_FULL           // 全速
**                      u32Channels:     Host Channels number.
**                                       This parameter Depends on the used USB core.   
**                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15
**                      bDMAEnable:        Enable or disable of the USB embedded DMA.             
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBHostInit(rt_uint32_t u32Base, rt_uint32_t u32Speed, rt_uint32_t u32Channels, rt_bool_t bDMAEnable)
{
  rt_uint32_t i;
  
  /* Restart the Phy Clock */
  HWREG32(u32Base + USB_OTG_PCGCCTL) = 0U;
  /* Disable VBUS sensing */
  HWREG32(u32Base + USB_OTG_GCCFG) &= ~(1UL << 21);
  /* Disable Battery chargin detector */
  HWREG32(u32Base + USB_OTG_GCCFG) &= ~(1UL << 17);
  
  if ((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U)
  {
    if (u32Speed == USB_OTG_SPEED_FULL)
    {
      /* Force Device Enumeration to FS/LS mode only */
      HWREG32(u32Base + USB_OTG_HCFG) |= (1UL << 2);  ;
    }
    else
    {
      /* Set default Max speed support */
      HWREG32(u32Base + USB_OTG_HCFG) &= ~(1UL << 2);
    }
  } else {
    /* Set default Max speed support */
    HWREG32(u32Base + USB_OTG_HCFG) &= ~(1UL << 2);
  }

  /* Make sure the FIFOs are flushed. */
  USBTxFifoFlush(u32Base, 0x10U); /* all Tx FIFOs */
  USBRxFifoFlush(u32Base);

  /* Clear all pending HC Interrupts */
  for (i = 0U; i < u32Channels; i++)
  {
    HWREG32(u32Base + USB_OTG_HCINT0 + 0x20U * i) = 0xFFFFFFFFU;
    HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20U * i) = 0U;
  }
  
  /* Enable VBUS driving */
  USBVbusDrive(u32Base, RT_TRUE);
  
  rt_thread_mdelay(210U);
  
  /* Disable all interrupts. */
  HWREG32(u32Base + USB_OTG_GINTMSK) = 0U; 
  
  /* Clear any pending interrupts */
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xFFFFFFFFU;
  
  if ((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) {
    /* set Rx FIFO size */
    HWREG32(u32Base + USB_OTG_GRXFSIZ) =0x200U; 
    HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) = (uint32_t )(((0x100U << 16U)& (0xFFFFU << 16)) | 0x200U);
    HWREG32(u32Base + USB_OTG_HPTXFSIZ) = (uint32_t )(((0xE0U << 16U)& (0xFFFFU << 16)) | 0x300U);
  } else {
    /* set Rx FIFO size */
    HWREG32(u32Base + USB_OTG_GRXFSIZ) = 0x80U; 
    HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) = (uint32_t )(((0x60U << 16U)& (0xFFFFU << 16)) | 0x80U);
    HWREG32(u32Base + USB_OTG_HPTXFSIZ) = (uint32_t )(((0x40U << 16U)& (0xFFFFU << 16)) | 0xE0U);
  }
  
  /* Enable the common interrupts */
  if (bDMAEnable == RT_FALSE)
  {
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << 4);
  }
  
  /* Enable interrupts matching to the Host mode ONLY */
  HWREG32(u32Base + USB_OTG_GINTMSK) |= ((1UL << 24) | (1UL << 25) | (1UL << 3) |\
                                         (1UL << 29) | (1UL << 21) | (1UL << 31));
}

/*********************************************************************************************************
** Function name:       USBFSLSPClkSelInit
** Descriptions:        Initializes the FSLSPClkSel field of the HCFG register on the PHY type 
**                      and set the right frame interval
** input parameters:    u32Base:          指定的USB模块
**                      u8Freq:           时钟频率，可以选下列值中的一个
**                             HCFG_48_MHZ      // Full Speed 48 MHz Clock 
**                             HCFG_6_MHZ       // Low Speed 6 MHz Clock           
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBFSLSPClkSelInit(rt_uint32_t u32Base, rt_uint8_t u8Freq)
{
  HWREG32(u32Base + USB_OTG_HCFG) &= ~(3U << 0);
  HWREG32(u32Base + USB_OTG_HCFG) |= ((3U << 0) & (rt_uint32_t)(u8Freq));
  
  if (u8Freq ==  HCFG_48_MHZ)
  {
    HWREG32(u32Base + USB_OTG_HFIR) = 48000U;
  }
  else if (u8Freq ==  HCFG_6_MHZ)
  {
     HWREG32(u32Base + USB_OTG_HFIR) = 6000U;
  } 
}

/*********************************************************************************************************
** Function name:       USBPortReset
** Descriptions:        Reset Host Port
** input parameters:    u32Base:          指定的USB模块         
** output parameters:   NONE
** Returned value:      NONE
** Note: The application must wait at least 10 ms before clearing the reset bit.
*********************************************************************************************************/
void USBPortReset(rt_uint32_t u32Base)
{
  volatile uint32_t hprt;
  
  hprt = HWREG32(u32Base + USB_OTG_HPRT);
  hprt &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
  HWREG32(u32Base + USB_OTG_HPRT) = ((1UL << 8) | hprt); 
  rt_thread_mdelay(110);  /* See Note #1 */
  HWREG32(u32Base + USB_OTG_HPRT) = ((~(1UL << 8)) & hprt); 
  rt_thread_mdelay(20); 
}

/*********************************************************************************************************
** Function name:       USBPortReset
** Descriptions:        Reset Host Port
** input parameters:    u32Base:          指定的USB模块         
** output parameters:   NONE
** Returned value:      NONE
** Note: The application must wait at least 10 ms before clearing the reset bit.
*********************************************************************************************************/
void USBVbusDrive(rt_uint32_t u32Base, rt_bool_t bState)
{
  volatile uint32_t hprt;

  hprt = HWREG32(u32Base + USB_OTG_HPRT);
  hprt &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
  
  if (((hprt & (1UL << 12)) == 0U) && (bState == RT_TRUE))
  {
    HWREG32(u32Base + USB_OTG_HPRT) = ((1UL << 12) | hprt); 
  }
  if (((hprt & (1UL << 12)) == (1UL << 12)) && (bState == RT_FALSE))
  {
    HWREG32(u32Base + USB_OTG_HPRT) = ((~(1UL << 12)) & hprt); 
  }
}

/*********************************************************************************************************
** Function name:       USBHostSpeedGet
** Descriptions:        Get Host Core speed
** input parameters:    u32Base:          指定的USB模块         
** output parameters:   NONE
** Returned value:      speed : 主机速度模式，返回值为下列中的一个
**                              USB_OTG_SPEED_HIGH      // High speed mode
**                              USB_OTG_SPEED_FULL      // Full speed mode
**                              USB_OTG_SPEED_LOW       // Low speed mode
*********************************************************************************************************/
uint32_t USBHostSpeedGet(rt_uint32_t u32Base)
{
   volatile uint32_t hprt;
  
  hprt = HWREG32(u32Base + USB_OTG_HPRT);
  return ((hprt & (3UL << 17)) >> 17U);
}

/*********************************************************************************************************
** Function name:       USBCurrentFrameNumberGet
** Descriptions:        Get Host Current Frame number
** input parameters:    u32Base:          指定的USB模块         
** output parameters:   NONE
** Returned value:      Current frame number
*********************************************************************************************************/
uint32_t USBCurrentFrameNumberGet(rt_uint32_t u32Base)
{
  return (HWREG32(u32Base + USB_OTG_HFNUM) & (0xFFFFU));
}

/*********************************************************************************************************
** Function name:       USBHostChannelInit
** Descriptions:        Initialize a host channel
** input parameters:    u32Base:        指定的USB模块 
**                      ch_num:         通道编号，取1-15
**                      epnum：         端点编号，取1-15
**                      dev_address:    当前的设备地址，取0-255
**                      speed:          设备速度，取下列值中的一个
**                              USB_OTG_SPEED_HIGH      // High speed mode
**                              USB_OTG_SPEED_FULL      // Full speed mode
**                              USB_OTG_SPEED_LOW       // Low speed mode
**                      ep_type：        端点类型，取下列值中的一个
**                              EP_TYPE_CTRL            // 控制类型
**                              EP_TYPE_ISOC            // 同步类型
**                              EP_TYPE_BULK            // 批量类型
**                              EP_TYPE_INTR            // 中断类型
**                      mps:    最大包大小，可以是0-32K的数
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBHostChannelInit(rt_uint32_t u32Base,
                        uint8_t ch_num,uint8_t epnum, uint8_t dev_address,
                        uint8_t speed, uint8_t ep_type, uint16_t mps)
{
  uint32_t HCcharEpDir, HCcharLowSpeed;
  
  /* Clear old interrupt conditions for this host channel. */
  HWREG32(u32Base + USB_OTG_HCINT0 + 0x20UL * ch_num) = 0xFFFFFFFFU;
  
  /* Enable channel interrupts required for this transfer. */
  switch (ep_type) 
  {
  case EP_TYPE_CTRL:
  case EP_TYPE_BULK:
    HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) = 
      ((1UL << 0) | (1UL << 3) | (1UL << 7) | (1UL << 10) | (1UL << 2) | (1UL << 4));
 
    if (epnum & 0x80U) 
    {
      HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) |= (1UL << 8);
    } 
    else 
    {
      if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) {
        HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) |= ((1UL << 6) | (1UL << 5));
      }
    }
    break;
    
  case EP_TYPE_INTR:
    HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) = 
      ((1UL << 0) | (1UL << 3) | (1UL << 7) | (1UL << 10) | (1UL << 4) | (1UL << 2) | (1UL << 9));
    
    if (epnum & 0x80U) 
    {
      HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num)|= (1UL << 8);
    }
    
    break;
  case EP_TYPE_ISOC:
    HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) = 
      ((1UL << 0) | (1UL << 5) | (1UL << 2) | (1UL << 9));  
    
    if (epnum & 0x80U) 
    {
      HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * ch_num) |= ((1UL << 7) | (1UL << 8));    
    }
    break;
  default:
      break;
  }
  
  /* Enable the top level host channel interrupt. */
  HWREG32(u32Base + USB_OTG_HAINTMSK) |= (1UL << (ch_num & 0xFU));
  
  /* Make sure host channel interrupts are enabled. */
  HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << 25);
  
  if ((epnum & 0x80U) == 0x80U)
  {
    HCcharEpDir = (0x1U << 15);
  }
  else
  {
    HCcharEpDir = 0U;
  }

  if (speed == USB_OTG_SPEED_LOW)
  {
    HCcharLowSpeed = (0x1U << 17);
  }
  else
  {
    HCcharLowSpeed = 0U;
  }

  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num)  = 
    (((uint32_t)dev_address << 22) & (0x7FU << 22)) |
    ((((uint32_t)epnum & 0x7FU) << 11) & (0x0FU << 11)) |
    (((uint32_t)ep_type << 18) & (3UL << 18)) |
    ((uint32_t)mps & (0x7FFU << 0)) | HCcharEpDir | HCcharLowSpeed;

  if (ep_type == EP_TYPE_INTR)
  {
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * ch_num) |= (1UL << 29);
  }
}

/*********************************************************************************************************
** Function name:       USBCurrentFrameNumberGet
** Descriptions:        Get Host Current Frame number
** input parameters:    u32Base:          指定的USB模块   
**                      hc:               主机通道特征结构体
**                      bUseDMA：        是否使用DMA传输      
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBHostChannelXferStart(rt_uint32_t u32Base, tUSBHostChannelInfo *hc, rt_bool_t bUseDMA)
{
  uint8_t  is_oddframe = 0; 
  uint16_t len_words = 0;   
  uint16_t num_packets = 0;
  uint16_t max_hc_pkt_count = 256;
  volatile uint32_t tmpreg = 0U;
  
  if(((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) && (hc->speed == USB_OTG_SPEED_HIGH))
  {
    if((bUseDMA == RT_FALSE) && (hc->do_ping == 1U))
    {
      USBDoPingInit(u32Base, hc->ch_num);
      return;
    }
    else if(bUseDMA == RT_TRUE)
    {
      HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20UL * hc->ch_num) &= ~((1UL < 6) | (1UL << 5));
      hc->do_ping = 0U;
    }
  }
  
  /* Compute the expected number of packets associated to the transfer */
  if (hc->xfer_len > 0U)
  {
    num_packets = (hc->xfer_len + hc->max_packet - 1U) / hc->max_packet;
    
    if (num_packets > max_hc_pkt_count)
    {
      num_packets = max_hc_pkt_count;
      hc->xfer_len = num_packets * hc->max_packet;
    }
  }
  else
  {
    num_packets = 1;
  }
  if (hc->ep_is_in != 0)
  {
    hc->xfer_len = num_packets * hc->max_packet;
  }
  
  /* Initialize the HCTSIZn register */
  HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * hc->ch_num) = 
    (((hc->xfer_len) & 0x7FFFFU)) | ((num_packets << 19U) & (0x3FFU << 19)) |\
     (((hc->data_pid) << 29U) & (3U << 29));
  
  if (bUseDMA)
  {
    /* xfer_buff MUST be 32-bits aligned */
    HWREG32(u32Base + USB_OTG_HCDMA0 + 0x20UL * hc->ch_num) = (uint32_t)hc->xfer_buff;
  }
  
  is_oddframe = (HWREG32(u32Base + USB_OTG_HFNUM) & 0x01) ? 0 : 1;
  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc->ch_num) &= ~(1UL << 29);
  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc->ch_num) |= (is_oddframe << 29);
  
  /* Set host channel enable */
  tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc->ch_num);
  tmpreg &= ~(1UL << 30);
  if (hc->ep_is_in != 0U)
  {
     tmpreg |= (1UL << 15);
  }
  else
  {
    tmpreg &= ~(1UL << 15);
  }
  tmpreg |= (1UL << 31);
  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc->ch_num) = tmpreg;
  
  if (bUseDMA == RT_FALSE) /* Slave mode */
  {  
    if((hc->ep_is_in == 0U) && (hc->xfer_len > 0U))
    {
      switch(hc->ep_type) 
      {
        /* Non periodic transfer */
      case EP_TYPE_CTRL:
      case EP_TYPE_BULK:
        
        len_words = (hc->xfer_len + 3) / 4;
        
        /* check if there is enough space in FIFO space */
        if(len_words > (HWREG32(u32Base + USB_OTG_GNPTXSTS) & 0xFFFFU))
        {
          /* need to process data in nptxfempty interrupt */
          HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << 5);
        }
        break;
        /* Periodic transfer */
      case EP_TYPE_INTR:
      case EP_TYPE_ISOC:
        len_words = (hc->xfer_len + 3) / 4;
        /* check if there is enough space in FIFO space */
        if(len_words > (HWREG32(u32Base + USB_OTG_HPTXSTS) & 0xFFFFU)) /* split the transfer */
        {
          /* need to process data in ptxfempty interrupt */  
          HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << 26);
        }
        break;
        
      default:
        break;
      }
      
      /* Write packet into the Tx FIFO. */
      USBPacketWrite(u32Base, hc->ch_num, hc->xfer_buff, (uint16_t)hc->xfer_len, 0);
    }
  }
}

/*********************************************************************************************************
** Function name:       USBHostChannelInterruptRead
** Descriptions:        Read all host channel interrupts status
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
rt_uint32_t USBHostChannelInterruptRead(rt_uint32_t u32Base)
{
  return (HWREG32(u32Base + USB_OTG_HAINT) & 0xFFFFU);
}

/*********************************************************************************************************
** Function name:       USBHostChannelHalt
** Descriptions:        Halt a host channel
** input parameters:    u32Base:        指定的USB模块  
**                      hc_num:         Host Channel number, This parameter can be a value from 1 to 15 
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBHostChannelHalt(rt_uint32_t u32Base, uint8_t hc_num)
{
//  uint32_t u32Count = 0U;
  uint32_t HcEpType = (HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) >> 18) & 0x03;
  
  /* Check for space in the request queue to issue the halt. */
  
  if ((HcEpType == EP_TYPE_CTRL) || (HcEpType == EP_TYPE_BULK))
  {
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 30);
    if ((HWREG32(u32Base + USB_OTG_GNPTXSTS) & (0xFFU << 16)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) &= ~(1UL << 31);
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 31);  
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) &= ~(1UL << 15);
//      do 
//      {
//        if (++u32Count > 1000U) 
//        {
//          break;
//        }
//      } 
      while ((HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) & (1UL << 31)) == (1UL << 31));     
    }
    else
    {
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 31); 
    }
  }
  else
  {
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 30);
    
    if ((HWREG32(u32Base + USB_OTG_HPTXSTS) & (0xFFU << 16)) == 0U)
    {
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) &= ~(1UL << 31);
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 31);  
      HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) &= ~(1UL << 15);
//      do 
//      {
//        if (++u32Count > 1000U) 
//        {
//          break;
//        }
//      } 
      while ((HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) & (1UL << 31)) == (1UL << 31));     
    }
    else
    {
       HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) |= (1UL << 31); 
    }
  }
}

/*********************************************************************************************************
** Function name:       USBDoPingInit
** Descriptions:        Initiate Do Ping protocol
** input parameters:    u32Base:        指定的USB模块  
**                      hc_num:         Host Channel number, This parameter can be a value from 1 to 15 
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBDoPingInit(rt_uint32_t u32Base, uint8_t hc_num)
{
  uint8_t  num_packets = 1U;
  uint32_t tmpreg = 0U;

  HWREG32(u32Base + USB_OTG_HCTSIZ0 + 0x20UL * hc_num) = 
    ((num_packets << 19U) & (0x3FFU << 19)) | (1UL << 31);
  
  /* Set host channel enable */
  tmpreg = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num);
  tmpreg &= ~(1UL << 30);
  tmpreg |= (1UL << 31);
  HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * hc_num) = tmpreg;
}

/*********************************************************************************************************
** Function name:       USBDoPingInit
** Descriptions:        Initiate Do Ping protocol
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBHostStop(rt_uint32_t u32Base)
{
  uint8_t i;
//  uint32_t u32Count = 0U;
  uint32_t u32Value;
  
  USBGlobalIntDisable(u32Base);
  
    /* Flush FIFO */
  USBTxFifoFlush(u32Base, 0x10U);
  USBRxFifoFlush(u32Base);
  
  /* Flush out any leftover queued requests. */
  for (i = 0; i <= 15; i++)
  {   

    u32Value = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i);
    u32Value |=  (1UL << 30);
    u32Value &= ~(1UL << 31);  
    u32Value &= ~(1UL << 15);
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) = u32Value;
  }
  
  /* Halt all channels to put them into a known state. */  
  for (i = 0; i <= 15; i++)
  {
    u32Value = HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i);
    
    u32Value |= (1UL << 30);
    u32Value |= (1UL << 31);  
    u32Value &= ~(1UL << 15);
    
    HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) = u32Value;
//    do 
//    {
//      if (++u32Count > 1000U) 
//      {
//        break;
//      }
//    } 
    while ((HWREG32(u32Base + USB_OTG_HCCHAR0 + 0x20UL * i) & (1UL << 31)) == (1UL << 31));
  }

  /* Clear any pending Host interrupts */  
  HWREG32(u32Base + USB_OTG_HAINT) = 0xFFFFFFFFU;
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xFFFFFFFFU;
  USBGlobalIntEnable(u32Base);
}

/*********************************************************************************************************
** Function name:       USBRemoteWakeupActivate
** Descriptions:        active remote wakeup signalling
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBRemoteWakeupActivate(rt_uint32_t u32Base)
{
  if(HWREG32(u32Base + USB_OTG_DSTS) & (1UL << 0)) {
    HWREG32(u32Base + USB_OTG_DCTL) |= (1UL << 0);
  }
}


/*********************************************************************************************************
** Function name:       USBRemoteWakeupDeActivate
** Descriptions:        de-active remote wakeup signalling
** input parameters:    u32Base:        指定的USB模块  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void USBRemoteWakeupDeActivate(rt_uint32_t u32Base)
{
  /* active Remote wakeup signalling */
  HWREG32(u32Base + USB_OTG_DCTL) &= ~(1UL << 0);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

