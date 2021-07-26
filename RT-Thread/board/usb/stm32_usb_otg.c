/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           stm32_usb_otg.c
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
#include <stdint.h>
#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "hal/cortexm/hal_cortexm_api.h"

#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_ints.h"
#include "hal/arch/inc/arch_io_dma.h"
#include "hal/arch/inc/arch_io_gpio.h"

#include "stm32_usb_otg.h"

/*********************************************************************************************************
** 函数声明
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       usb_flush_rx
** Descriptions:        flush rx fifo
** input parameters:    u32Base:    USB在CPU总线的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_flush_rx(uint32_t u32Base)
{  
  HWREG32(u32Base + USB_OTG_GRSTCTL) = (1U << 4);
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 4)) == (1U << 4));
}

/*********************************************************************************************************
** Function name:       usb_flush_tx
** Descriptions:        flush tx fifo
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      num:     fifo数量，和主机channel数一致  
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_flush_tx(uint32_t u32Base, uint32_t num)
{
  HWREG32(u32Base + USB_OTG_GRSTCTL) = ((1U << 5) | (uint32_t)(num << 6)); 
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & (1U << 5)) == (1U << 5));
}

/*********************************************************************************************************
** Function name:       usb_core_reset
** Descriptions:        复位USB核
** input parameters:    u32Base:    USB在CPU总线的基地址
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_core_reset(uint32_t u32Base)
{
  /* Wait for AHB master IDLE state. */
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_AHBIDL) == 0U);
  /* Core Soft Reset */
  HWREG32(u32Base + USB_OTG_GRSTCTL) |= USB_OTG_GRSTCTL_CSRST;
  while ((HWREG32(u32Base + USB_OTG_GRSTCTL) & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

/*********************************************************************************************************
** Function name:       usb_otg_mode_set
** Descriptions:        Set functional mode
** input parameters:    u32Base:          指定的USB模块
**                      u8Mode:  指定工作模式，可以取下列值
**                      USB_OTG_MODE_DEVICE             // 设备模式
**                      USB_OTG_MODE_HOST               // 主机模式
**                      USB_OTG_MODE_DRD                // 双角色模式
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_otg_mode_set(uint32_t u32Base, uint8_t u8Mode)
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
** Function name:       usb_otg_core_init
** Descriptions:        init the IO and OTG core
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      u32PhyInterface: 使用的PHY接口,  可以取下列值：
**                      USB_OTG_ULPI_PHY         // 外部ULPI PHY
**                      USB_OTG_EMBEDDED_FS_PHY  // 内部全速PHY 
**                      USB_OTG_EMBEDDED_HS_PHY  // 内部高速PHY
**                      bVbusSensing:      Enable or disable the VBUS Sensing feature.  
**                      bDMAEnable:        Enable or disable of the USB embedded DMA.             
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
int usb_otg_core_init(uint32_t u32Base, uint32_t u32PhyInterface, 
                             rt_bool_t bVbusSensing, rt_bool_t bDMAEnable)
{
  if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) == 0U) {
    /* Select FS Embedded PHY */
    HWREG32(u32Base + USB_OTG_GUSBCFG) |= USB_OTG_GUSBCFG_PHYSEL;
    /* Reset after a PHY select and set Host mode */
    usb_core_reset(u32Base);
    /* Deactivate the power down*/
    HWREG32(u32Base + USB_OTG_GCCFG) = (USB_OTG_GCCFG_PWRDWN);
  } else if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) {
    if(USB_OTG_EMBEDDED_FS_PHY == u32PhyInterface) {
      /* Select FS Embedded PHY */
      HWREG32(u32Base + USB_OTG_GUSBCFG) |= USB_OTG_GUSBCFG_PHYSEL;
      /* Reset after a PHY select and set Host mode */
      usb_core_reset(u32Base);
      /* Deactivate the power down*/
      HWREG32(u32Base + USB_OTG_GCCFG) |= (USB_OTG_GCCFG_PWRDWN);
    } else if(0) {//(USB_OTG_EMBEDDED_HS_PHY == u32PhyInterface) 
      //      // init embedded phy
      //      HWREG32(u32Base + USB_OTG_GCCFG) &= ~(USB_OTG_GCCFG_PWRDWN);
      //    /* Init The UTMI Interface */
      //    HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
      //    /* Select vbus source */
      //    HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
      //    /* Select UTMI Interace */
      //    HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~ USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
      //    HWREG32(u32Base + USB_OTG_GCCFG) |= USB_OTG_GCCFG_PHYHSEN;
      //    /* Enables control of a High Speed USB PHY */
      //    USB_HS_PHYCInit(USBx);
    } else if(USB_OTG_ULPI_PHY == u32PhyInterface) {
      HWREG32(u32Base + USB_OTG_GCCFG) &= ~(USB_OTG_GCCFG_PWRDWN);
      /* Init The ULPI Interface */
      HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
      /* Select vbus source */
      HWREG32(u32Base + USB_OTG_GUSBCFG) &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    } else {
      return -1;
    }
    
    if(bVbusSensing)
      HWREG32(u32Base + USB_OTG_GUSBCFG) |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    
    /* Reset after a PHY select  */
    usb_core_reset(u32Base);
  } else {
    return -1;
  }
  
  if(bDMAEnable) {
    if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) {
      // only HS core has DMA feature
      HWREG32(u32Base + USB_OTG_GAHBCFG) |= (3UL << 1);
      HWREG32(u32Base + USB_OTG_GAHBCFG) |= USB_OTG_GAHBCFG_DMAEN;
    }
  }
  
  return 0;
}

/*********************************************************************************************************
** Function name:       usb_otg_device_init
** Descriptions:        init usb otg as usb device
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      u32PhyInterface: 使用的PHY接口,  可以取下列值：
**                      USB_OTG_ULPI_PHY         // 外部ULPI PHY
**                      USB_OTG_EMBEDDED_FS_PHY  // 内部全速PHY 
**                      USB_OTG_EMBEDDED_HS_PHY  // 内部高速PHY
**                      epnum:      usb设备的端点数量
**                      vbus：      是否使能VBUS探测            
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
int usb_otg_device_init(uint32_t u32Base, uint32_t u32PhyInterface, 
                               uint8_t epnum, rt_bool_t vbus)
{
  uint32_t i;

  if(vbus)
    HWREG32(u32Base + USB_OTG_GCCFG) |= (1UL << 21);
  else {
    HWREG32(u32Base + USB_OTG_GCCFG) &= ~(1UL << 21);
     /* B-peripheral session valid override enable*/ 
    HWREG32(u32Base + USB_OTG_GOTGCTL) |= (1UL << 6);
    HWREG32(u32Base + USB_OTG_GOTGCTL) |= (1UL << 7);
  }
  
//#if defined(USB_OTG_GCCFG_VBUSBSEN)
//#if defined(ENABLE_VBUS_DETECT)
//    USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
//#else
//#if defined(USB_OTG_GCCFG_NOVBUSSENS)
//    /* Enable VBUS */
//    USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
//#endif
//#endif
//#endif
//  
//#if defined(STM32F1)
//    USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
//#endif

   /* Restart the Phy Clock */
  HWREG32(u32Base + USB_OTG_PCGCCTL) = 0U;
  /* Device mode configuration */
  HWREG32(u32Base + USB_OTG_DCFG) |= (0UL << 11);  // 80% of the frame interval
  
  if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) == 0U) {
    HWREG32(u32Base + USB_OTG_DCFG) |= USB_OTG_SPEED_FULL;
  } else {
    if(USB_OTG_EMBEDDED_FS_PHY == u32PhyInterface) {
      HWREG32(u32Base + USB_OTG_DCFG) |= USB_OTG_SPEED_FULL;
    } else {
      HWREG32(u32Base + USB_OTG_DCFG) |= USB_OTG_SPEED_HIGH;
    }
  }

  usb_flush_tx(u32Base, 0x10);
  usb_flush_rx(u32Base);
  
  /* Clear all pending Device Interrupts */
  HWREG32(u32Base + USB_OTG_DIEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DOEPMSK) = 0U;
  HWREG32(u32Base + USB_OTG_DAINT) = 0xFFFFFFFF;
  HWREG32(u32Base + USB_OTG_DAINTMSK) = 0U;
  
  // ep will init in the reset handler
  for (i = 0U; i < epnum; i++){
    if((HWREG32(u32Base + USB_OTG_DIEPCTL0 + 0x20 * i) & (1UL << 31)) == (1UL << 31)) {
      if (i == 0U) {
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
  
  for (i = 0U; i < epnum; i++){
    if((HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) & (1UL << 31)) == (1UL << 31)) {
      if(i == 0) {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = (1UL << 27);
      } else {
        HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = ((1UL << 30) | (1UL << 27));
      }
    }else{
      HWREG32(u32Base + USB_OTG_DOEPCTL0 + 0x20 * i) = 0;
    }
    HWREG32(u32Base + USB_OTG_DOEPTSIZ0 + 0x20 * i) = 0;
    HWREG32(u32Base + USB_OTG_DOEPINT0 + 0x20 * i) = 0xFB7FU;
  }
  
  HWREG32(u32Base + USB_OTG_DIEPMSK) &= ~(1UL << 8);
  // disable global int
  HWREG32(u32Base + USB_OTG_GAHBCFG) &= ~(1U << 0);
  HWREG32(u32Base + USB_OTG_GINTMSK) = 0U;
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xBFFFFFFFU;
  
  //TODO: support LPM, BCD feature
  if( HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN){
    // If DMA enabled, setup the threshold value
    volatile uint32_t temp;
    HWREG32(u32Base + USB_OTG_DTHRCTL) = ((0x040UL << 2) | (0x040UL << 17));
    HWREG32(u32Base + USB_OTG_DTHRCTL) |= ((1UL << 16) | (1UL << 1) | (1UL<< 0));
    temp = HWREG32(u32Base + USB_OTG_DTHRCTL);
    (void)temp;
  }else{
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1U << 4);
  }

  /* Enable interrupts matching to the Device mode ONLY */
  if(vbus) {
      HWREG32(u32Base + USB_OTG_GINTMSK)|= ((1U << 11) | (1U << 12) | (1U << 13) | (1U << 18) |
                    (1U << 19)   | (1U << 20)| (1U << 21)  | (1U << 31) | (1U << 30) | (1U << 2));
  } else {
     HWREG32(u32Base + USB_OTG_GINTMSK) |= ((1U << 11) | (1U << 12) |(1U << 13) | (1U << 18) |
                    (1U << 19) | (1U << 20)| (1U << 21)  | (1U << 31));
  }
  
  HWREG32(u32Base + USB_OTG_DCTL) |= (1U << 1);
  rt_thread_mdelay(30);
  HWREG32(u32Base + USB_OTG_DCTL)&= ~(1U << 1);
  rt_thread_mdelay(10);
  HWREG32(u32Base + USB_OTG_GAHBCFG) |= USB_OTG_GAHBCFG_GINT;
  return 0;
}

/*********************************************************************************************************
** Function name:       usb_otg_vbus_drive
** Descriptions:        drive the vbus
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      bState：     enable or disable the vbus   
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
void usb_otg_vbus_drive (uint32_t u32Base, uint8_t bState)
{
  volatile uint32_t hprt0 = 0U;
  
  hprt0 = HWREG32(u32Base + USB_OTG_HPRT);;
  hprt0 &= ~((1UL << 2) | (1UL << 1) | (1UL << 3) | (1UL << 5));
  
  if (((hprt0 & (1UL << 12)) == 0U) && (bState == RT_TRUE))
  {
    HWREG32(u32Base + USB_OTG_HPRT) = ((1UL << 12) | hprt0); 
  }
  if (((hprt0 & (1UL << 12)) == (1UL << 12)) && (bState == RT_FALSE))
  {
    HWREG32(u32Base + USB_OTG_HPRT) = ((~(1UL << 12)) & hprt0); 
  }
}

/*********************************************************************************************************
** Function name:       usb_otg_host_init
** Descriptions:        init use otg as usb host
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      u32Speed:         PHY速度配置， 可以取下列值：
**                              USB_OTG_SPEED_HIGH           // 高速
**                              USB_OTG_SPEED_LOW            // 低速
**                              USB_OTG_SPEED_FULL           // 全速
**                      u32Channels:     Host Channels number.
**                                       This parameter Depends on the used USB core.   
**                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15
**                      vbus：      是否使能VBUS探测            
** output parameters:   NONE
** Returned value:      -1: 初始化失败； 0：初始化成功
*********************************************************************************************************/
int usb_otg_host_init(uint32_t u32Base, uint32_t u32Speed, uint32_t u32Channels)
{
  uint32_t i;
  
  /* Restart the Phy Clock */
  HWREG32(u32Base + USB_OTG_PCGCCTL) = 0U;
  
  /* Disable VBUS sensing */
  HWREG32(u32Base + USB_OTG_GCCFG) &= ~(1UL << 21);
  if(u32Speed == USB_OTG_SPEED_HIGH) {
     HWREG32(u32Base + USB_OTG_HCFG) &= ~(1UL << 2);
  } else {
    HWREG32(u32Base + USB_OTG_HCFG) |= (1UL << 2);
  }

  /* Make sure the FIFOs are flushed. */
  usb_flush_tx(u32Base, 0x10U); /* all Tx FIFOs */
  usb_flush_rx(u32Base);

  /* Clear all pending HC Interrupts */
  for (i = 0U; i < u32Channels; i++){
    HWREG32(u32Base + USB_OTG_HCINT0 + 0x20U * i) = 0xFFFFFFFFU;
    HWREG32(u32Base + USB_OTG_HCINTMSK0 + 0x20U * i) = 0U;
  }

  /* Enable VBUS driving */
  usb_otg_vbus_drive(u32Base, 1U);
  
  rt_thread_mdelay(200);
  
  /* Disable all interrupts. */
  HWREG32(u32Base + USB_OTG_GINTMSK) = 0U;
  
  /* Clear any pending interrupts */
  HWREG32(u32Base + USB_OTG_GINTSTS) = 0xFFFFFFFFU;

  // The FIFO will not set success if remove the delay function before
  // How to wait is not mentioned
  // FIFO did not allocate all memory, the remain memory maybe used for the request queue
  // How request queue work is not describe in the RM
  if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) == 0U) {
    /* set Rx/Tx FIFO size */
    HWREG32(u32Base + USB_OTG_GRXFSIZ)  = 0x80U;
    do{
      HWREG32(u32Base + USB_OTG_HPTXFSIZ) = (uint32_t )(((0x40U << 16)& (0xFFFFU << 16)) | 0xE0U);
    }while( HWREG32(u32Base + USB_OTG_HPTXFSIZ) != (uint32_t )(((0x40U << 16)& (0xFFFFU << 16)) | 0xE0U) );
    do{
      HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) = (uint32_t )(((0x60U << 16) & (0xFFFFU << 16)) | 0x80U);
    }while( HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) != (uint32_t )(((0x60U << 16) & (0xFFFFU << 16)) | 0x80U) );
  }else{
    /* set Rx/Tx FIFO size */
    HWREG32(u32Base + USB_OTG_GRXFSIZ) = 0x200U;
    do{
      HWREG32(u32Base + USB_OTG_HPTXFSIZ) = (uint32_t )(((0xE0U << 16) & (0xFFFFU << 16)) | 0x300U);
    }while(HWREG32(u32Base + USB_OTG_HPTXFSIZ) != (uint32_t )(((0xE0U << 16) & (0xFFFFU << 16)) | 0x300U) );
    do{
      HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) = (uint32_t )(((0x100U << 16) & (0xFFFFU << 16)) | 0x200U);
    }while( HWREG32(u32Base + USB_OTG_GNPTXFSIZ_TX0FSIZ) != (uint32_t )(((0x100U << 16) & (0xFFFFU << 16)) | 0x200U) );
  }
  
  if((HWREG32(u32Base + USB_OTG_GAHBCFG) & USB_OTG_GAHBCFG_DMAEN) == 0){
    // If DMA not enabled
    // copy data in fifo non-empty interrupt
    HWREG32(u32Base + USB_OTG_GINTMSK) |= (1UL << 4);
  }

  /* Enable interrupts matching to the Host mode ONLY */
  HWREG32(u32Base + USB_OTG_GINTMSK) |= ((1UL << 24) | (1UL << 25) |(1UL << 3) | 
                                         (1UL << 29)|(1UL << 21)  | (1UL << 31));
  
  HWREG32(u32Base + USB_OTG_GAHBCFG) |= USB_OTG_GAHBCFG_GINT;
  
  /* Enable VBUS driving */
  usb_otg_vbus_drive(u32Base, 1U);
  
  return 0;
}

/*********************************************************************************************************
** Function name:       usb_otg_data_read
** Descriptions:        get the usb device speed
** input parameters:    u32Base:    USB在CPU总线的基地址
**                      buf:        读取数据存放缓冲区
**                      len:        读取数据长度       
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void usb_otg_data_read(uint32_t u32Base, void* buf, uint32_t len)
{
  uint32_t i;
  uint32_t *pDest = (uint32_t *)buf;
  uint32_t u32Count = (len + 3U) / 4U;
  uint32_t u32Temp;
  
  if(buf != NULL) {
    for (i = 0U; i < u32Count; i++)
    {
      *(__packed uint32_t *)pDest = HWREG32(u32Base + USB_OTG_FIFO_BASE + (0 * USB_OTG_FIFO_SIZE));
      pDest++;
    }
  } else {
    for (i = 0U; i < u32Count; i++)
    {
      u32Temp = HWREG32(u32Base + USB_OTG_FIFO_BASE + (0 * USB_OTG_FIFO_SIZE));
    }
  }
}

/*********************************************************************************************************
** Function name:       usbdevice_speed_get
** Descriptions:        get the usb device speed
** input parameters:    u32Base:    USB在CPU总线的基地址           
** output parameters:   NONE
** Returned value:      获取到的速度
*********************************************************************************************************/
int usbdevice_speed_get(uint32_t u32Base)
{
    if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) == 0U) {
       return USB_OTG_SPEED_FULL;
    }

    if((HWREG32(u32Base + USB_OTG_CID) & (0x1U << 8)) != 0U) {
        if( (HWREG32(u32Base + USB_OTG_DCFG) & (3UL << 0)) == 0){
            return USB_OTG_SPEED_HIGH;
        }
    }
    return USB_OTG_SPEED_FULL;
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
