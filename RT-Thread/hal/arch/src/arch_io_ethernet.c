/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_ethernet.c
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         ethernetģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2013-04-04
** Version:             v1.0
** Descriptions:        The original version ��ʼ�汾
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Description:
**
*********************************************************************************************************/
#include "hal/arch/inc/arch_io_memmap.h"
#include "hal/arch/inc/arch_io_reg.h"
#include "hal/arch/inc/arch_io_ethernet.h"


/*********************************************************************************************************
�ֲ���������
*********************************************************************************************************/

/*********************************************************************************************************
** Function name:       EthernetInit
** Descriptions:        Ethernetģ������
** input parameters:    u32Base:         ethernetģ�����ַ
**                      u32Clock:       �ṩ����̫��ģ���ʱ�ӣ���AHB��ʱ��Ƶ��
**                      u32RecvLength:  �������ݰ�����
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetInit(rt_uint32_t u32Base, rt_uint32_t u32Clock, rt_uint32_t u32RecvLength)
{
  rt_uint32_t u32Temp;
  rt_uint32_t u32PhyRegValue;
  rt_uint32_t u32AutoValue;
  
  // ����u32Clock��ֵ��ȷ��MDCģ���ʱ��
  u32Temp = HWREG32(u32Base + ETH_MACMDIOAR);
  u32Temp &= 0xFFFFF0FF;
  if((u32Clock >= 20000000) && (u32Clock < 35000000)) {
    u32Temp |= (2UL << 8);
  } else if((u32Clock >= 35000000) && (u32Clock < 60000000)) {
    u32Temp |= (3UL << 8);
  } else if((u32Clock >= 60000000) && (u32Clock < 100000000)) {
    u32Temp |= (0UL << 8);
  } else if((u32Clock >= 100000000) && (u32Clock < 150000000)) {
    u32Temp |= (1UL << 8);
  } else if((u32Clock >= 150000000) && (u32Clock < 250000000)) {
    u32Temp |= (4UL << 8);
  } 
  else {  // ��������ǣ�ʱ�Ӿ���250M-300M
    u32Temp |= (5UL << 8);
  }
  HWREG32(u32Base + ETH_MACMDIOAR) = u32Temp;
  
  // MAC LPI 1US Tic Counter Configuration
  HWREG32(u32Base + ETH_MAC1USTCR) = ((u32Clock / 1000000UL) - 1);
  
  // MAC, MTL and DMA default Configuration
  // MACCR Configuration
  u32Temp = HWREG32(u32Base + ETH_MACCR);
  u32Temp &= ~((rt_uint32_t)0xFFFB7F7CU);
  u32Temp |= (rt_uint32_t)((0UL << 24) |    // IFG between Packets during transmission is 96Bit
                           (3UL << 28) |    // Source Address Insertion or Replacement Control
                             (1UL << 27) |    // Checksum Offload
                               (0UL << 23) |    // Giant Packet Size Limit Control
                                 (0UL << 22) |    // IEEE 802.3as Support for 2K Packets
                                   (1UL << 21) |    // CRC stripping for Type packets
                                     (1UL << 20) |    // Automatic Pad or CRC Stripping
                                       (0UL << 19) |    // Watchdog Disable
                                         (0UL << 17) |    // Jabber Disable
                                           (0UL << 16) |    // Jumbo Packet Enable
                                             (1UL << 14) |    // MAC Speed-->100M
                                               (1UL << 13) |    // Duplex Mode
                                                 (0UL << 12) |    // Loopback Mode
                                                   (0UL << 11) |    // Enable Carrier Sense Before Transmission in Full-Duplex Mode
                                                     (0UL << 10) |    // Disable Receive Own
                                                       (0UL << 9) |     // Disable Carrier Sense During Transmission
                                                         (0UL << 8) |     // Disable Retry
                                                           (0UL << 5) |     // Back-Off Limit
                                                             (0UL << 4) |     // Deferral Check
                                                               (0UL << 2)       // Preamble Length for Transmit packets
                                                                 );
  HWREG32(u32Base + ETH_MACCR) = u32Temp;
  
  // MACECR Configuration
  u32Temp = HWREG32(u32Base + ETH_MACECR);
  u32Temp &= ~((rt_uint32_t)0x3F077FFFU);
  u32Temp |= (rt_uint32_t)((0UL << 25) |    // Extended Inter-Packet Gap
                           (0UL << 24) |    // Extended Inter-Packet Gap Enable
                             (0UL << 18) |    // Unicast Slow Protocol Packet Detect
                               (0UL << 17) |    // Slow Protocol Detection Enable
                                 (0UL << 16) |    // Disable CRC Checking for Received Packets
                                   (0x618UL << 0)   //  Giant Packet Size Limit
                                     );
  HWREG32(u32Base + ETH_MACECR) = u32Temp;
  
  // MACWTR Configuration
  u32Temp = HWREG32(u32Base + ETH_MACWTR);
  u32Temp &= ~((rt_uint32_t)0x0000010FU);
  u32Temp |= (rt_uint32_t)((0UL << 8) |    // Programmable Watchdog Enable
                           (0UL << 0)      // Watchdog Timeout-->2 Kbytes
                             );
  HWREG32(u32Base + ETH_MACWTR) = u32Temp;
  
  // MACQTXFCR Configuration
  u32Temp = HWREG32(u32Base + ETH_MACQTXFCR);
  u32Temp &= ~((rt_uint32_t)0xFFFF00F2U);
  u32Temp |= (rt_uint32_t)((0UL << 1) |    // Transmit Flow Control Enable
                           (0UL << 4) |    // Pause time minus 4 slot times
                             (0UL << 7) |    // Disable Zero-Quanta Pause
                               (0UL << 16)     // Pause Time
                                 );
  HWREG32(u32Base + ETH_MACQTXFCR) = u32Temp;
  
  // MACRXFCR Configuration
  u32Temp = HWREG32(u32Base + ETH_MACRXFCR);
  u32Temp &= ~((rt_uint32_t)0x00000003U);
  u32Temp |= (rt_uint32_t)((0UL << 0) |    // Receive Flow Control Enable
                           (0UL << 1)      // Unicast Pause Packet Detect
                             );
  HWREG32(u32Base + ETH_MACRXFCR) = u32Temp;
  
  // ETH_MTLTXQOMR Configuration
  u32Temp = HWREG32(u32Base + ETH_MTLTXQOMR);
  u32Temp &= ~((rt_uint32_t)0x00000072U);
  u32Temp |= (rt_uint32_t)((1UL << 1));  // Transmit Store and Forward
  HWREG32(u32Base + ETH_MTLTXQOMR) = u32Temp;
  
  // MTLRXQOMR Configuration
  u32Temp = HWREG32(u32Base + ETH_MTLRXQOMR);
  u32Temp &= ~((rt_uint32_t)0x0000007BU);
  u32Temp |= (rt_uint32_t)((1UL << 5) |    // Receive Queue Store and Forward
                           (0UL << 6) |    // Disable Dropping of TCP/IP Checksum Error Packets
                             (0UL << 4) |    // Forward Error Packets
                               (0UL << 3)      // Forward Undersized Good Packets
                                 );
  HWREG32(u32Base + ETH_MTLRXQOMR) = u32Temp;
  
  // DMAMR Configuration
  u32Temp = HWREG32(u32Base + ETH_DMAMR);
  u32Temp &= ~((rt_uint32_t)0x00007802U);
  u32Temp |= (rt_uint32_t)((0UL << 12));  // The priority ratio is 1:1 
  HWREG32(u32Base + ETH_DMAMR) = u32Temp;
  
  // DMASBMR Configuration
  u32Temp = HWREG32(u32Base + ETH_DMASBMR);
  u32Temp &= ~((rt_uint32_t)0x0000D001U);
  u32Temp |= (rt_uint32_t)((1UL << 12) |    //  Address-Aligned Beats
                           (1UL << 0)  |    //  Fixed Burst Length
                             (0UL << 15)      //  Rebuild INCRx Burst
                               );
  HWREG32(u32Base + ETH_DMASBMR) = u32Temp;
  
  // DMACCR Configuration
  u32Temp = HWREG32(u32Base + ETH_DMACCR);
  u32Temp &= ~((rt_uint32_t)0x00013FFFU);
  u32Temp |= (rt_uint32_t)((0UL << 16) |    //  8xPBL mode
                           (536UL << 0)     //   Maximum Segment Size
                             );
  HWREG32(u32Base + ETH_DMACCR) = u32Temp;
  
  // DMACTXCR Configuration
  u32Temp = HWREG32(u32Base + ETH_DMACTXCR);
  u32Temp &= ~((rt_uint32_t)0x003F1010U);
  u32Temp |= (rt_uint32_t)((32UL << 16) |   //  Transmit Programmable Burst Length
                           (0UL << 4)  |    //  Operate on Second Packet
                             (0UL << 12)      //  TCP Segmentation Enabled
                               );
  HWREG32(u32Base + ETH_DMACTXCR) = u32Temp;
  
  // DMACRXCR Configuration
  u32Temp = HWREG32(u32Base + ETH_DMACRXCR);
  u32Temp &= ~((rt_uint32_t)0x803F0000U);
  u32Temp |= (rt_uint32_t)((0UL << 31) |     //  DMA Rx Channel Packet Flush
                           (32UL << 16)      //  Receive Programmable Burst Length
                             );
  HWREG32(u32Base + ETH_DMACRXCR) = u32Temp;
  
  /* SET DSL to 64 bit */
  u32Temp = HWREG32(u32Base + ETH_DMACCR);
  u32Temp &= ~((rt_uint32_t)(7UL << 18));
  u32Temp |= (rt_uint32_t)((2UL << 18));   // ��32bitΪ��λ��2��ʾ64bit
  HWREG32(u32Base + ETH_DMACCR) = u32Temp;	
  
  /* Set Receive Buffers Length (must be a multiple of 4) */
  if ((u32RecvLength % 4) != 0) {
    return;
  } else {
    HWREG32(u32Base + ETH_DMACRXCR) &= ~(((rt_uint32_t)0x3FFFU) << 1);
    HWREG32(u32Base + ETH_DMACRXCR) |= (u32RecvLength << 1);
  }
  //  /*------------------ DMA Tx Descriptors Configuration ----------------------*/
  //  ETH_DMATxDescListInit(heth);
  //  
  //  /*------------------ DMA Rx Descriptors Configuration ----------------------*/
  //  ETH_DMARxDescListInit(heth);
}

/*********************************************************************************************************
** Function name:       EthernetPHYRead
** Descriptions:        ��ȡָ����ַ��PHY�Ĵ���
** input parameters:    u32Base:        ��̫��ģ�����ַ
**                      u8PHYDevAddr:   PHY�����ĵ�ַ
**                      u8PHYRegAddr:   PHY�Ĵ����ĵ�ַ
** output parameters:   NONE
** Returned value:      ��ȡ��PHY�Ĵ���������
*********************************************************************************************************/
rt_uint32_t EthernetPHYRead(rt_uint32_t u32Base, rt_uint8_t u8PHYDevAddr, rt_uint8_t u8PHYRegAddr)
{
  rt_uint32_t u32Temp;
  
  //  �ȴ�MAC MDIO��æ
  while(HWREG32(u32Base + ETH_MACMDIOAR) & 0x00000001);
  
  /* Get the  MACMDIOAR value */
  u32Temp = HWREG32(u32Base + ETH_MACMDIOAR);
  
  /* Prepare the MDIO Address Register value 
  - Set the PHY device address 
  - Set the PHY register address
  - Set the read mode 
  - Set the MII Busy bit */
  u32Temp &=  ~((rt_uint32_t)0x03FF000CU);
  u32Temp |= (((rt_uint32_t)u8PHYDevAddr) << 21); //  дPHY������ַ 
  u32Temp |= (((rt_uint32_t)u8PHYRegAddr) << 16); //  дPHY������ַ 
  u32Temp |= (((rt_uint32_t)3UL) << 2);           //  Set the read mode 
  u32Temp |= 0x00000001UL;
  //��д��PHY���ƼĴ���
  HWREG32(u32Base + ETH_MACMDIOAR) =  u32Temp;
  
  //  �ȶ�PHY�Ĵ����������
  while(HWREG32(u32Base + ETH_MACMDIOAR) & 0x00000001);
  
  return (HWREG32(u32Base + ETH_MACMDIODR) & 0xFFFF);
}

/*********************************************************************************************************
** Function name:       EthernetPHYRead
** Descriptions:        ��ȡָ����ַ��PHY�Ĵ���
** input parameters:    u32Base:        ��̫��ģ�����ַ
**                      u8PHYDevAddr:   PHY�����ĵ�ַ
**                      u8PHYRegAddr:   PHY�Ĵ����ĵ�ַ
**                      u32Data:        д��ָ��PHY�Ĵ���������
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetPHYWrite(rt_uint32_t u32Base, rt_uint8_t u8PHYDevAddr, rt_uint8_t u8PHYRegAddr, rt_uint32_t u32Data)
{
  rt_uint32_t u32Temp;
  
  //  �ȴ�MAC MDIO��æ
  while(HWREG32(u32Base + ETH_MACMDIOAR) & 0x00000001);
  
  /* Get the  MACMDIOAR value */
  u32Temp = HWREG32(u32Base + ETH_MACMDIOAR);
  /* Prepare the MDIO Address Register value 
     - Set the PHY device address 
     - Set the PHY register address
     - Set the write mode 
     - Set the MII Busy bit */
  u32Temp &=  ~((rt_uint32_t)0x03FF000CU);
  u32Temp |= (((rt_uint32_t)u8PHYDevAddr) << 21); //  дPHY������ַ 
  u32Temp |= (((rt_uint32_t)u8PHYRegAddr) << 16); //  дPHY������ַ 
  u32Temp |= (((rt_uint32_t)1UL) << 2);           //  Set the write mode 
  u32Temp |= 0x00000001UL;
  
  /* Give the value to the MII data register */
  HWREG32(u32Base + ETH_MACMDIODR) = (rt_uint16_t)u32Data;
  //��д��PHY���ƼĴ���
  HWREG32(u32Base + ETH_MACMDIOAR) =  u32Temp;
  
  //  �ȶ�PHY�Ĵ����������
  while(HWREG32(u32Base + ETH_MACMDIOAR) & 0x00000001);
}

/*********************************************************************************************************
** Function name:       EthernetEnable
** Descriptions:        ʹ����̫�����������������ͺͽ��ܵ�
** input parameters:    u32Base      ��̫��ģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetEnable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ETH_MACCR) |= (1UL << 1);     //  Enable the MAC transmission
  HWREG32(u32Base + ETH_MACCR) |= (1UL << 0);     //  Enable the MAC reception
  HWREG32(u32Base + ETH_MTLTXQOMR) |= (1UL << 0); //  Set the Flush Transmit FIFO bit
  HWREG32(u32Base + ETH_DMACTXCR) |= (1UL << 0);  //  Enable the DMA transmission
  HWREG32(u32Base + ETH_DMACRXCR) |= (1UL << 0);  //  Enable the DMA reception
  //  Clear Tx and Rx process stopped flags
  HWREG32(u32Base + ETH_DMACSR) |= ((1UL << 1) | (1UL << 8));
}


/*********************************************************************************************************
** Function name:       EthernetDisable
** Descriptions:        ��ֹ��̫�����������������ͺͽ��ܵ�
** input parameters:    u32Base      ��̫��ģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetDisable(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ETH_DMACTXCR) |= ~((rt_uint32_t)(1UL << 0));  //  Disable the DMA transmission
  HWREG32(u32Base + ETH_DMACRXCR) |= ~((rt_uint32_t)(1UL << 0));  //  Disable the DMA reception
  HWREG32(u32Base + ETH_MACCR) |= ~((rt_uint32_t)(1UL << 0));     //  Disable the MAC reception
  HWREG32(u32Base + ETH_MTLTXQOMR) |= (1UL << 0);                  //  Set the Flush Transmit FIFO bit
  HWREG32(u32Base + ETH_MACCR) |= ~((rt_uint32_t)(1UL << 1));     //  Disable the MAC transmission
}

/*********************************************************************************************************
** Function name:       EthernetMACAddrSet
** Descriptions:        ������̫����MAC��ַ���������ʹ��MAC0��MAC1-MAC3��ʹ��
** input parameters:    u32Base      ��̫��ģ�����ַ
**                      pu8MacAddr  ��Ҫ�趨��MAC��ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetMACAddrSet(rt_uint32_t u32Base, rt_uint8_t *pu8MacAddr)
{
  rt_uint32_t u32Temp;
  
  u32Temp = (((rt_uint32_t)pu8MacAddr[5] << 8) | (rt_uint32_t)pu8MacAddr[4]);
  HWREG32(u32Base + ETH_MACA0HR) = u32Temp;
  
  u32Temp = (((rt_uint32_t)pu8MacAddr[3] << 24) | ((rt_uint32_t)pu8MacAddr[2] << 16) |
             ((rt_uint32_t)pu8MacAddr[1] << 8) | (rt_uint32_t)pu8MacAddr[0]);
  
  HWREG32(u32Base + ETH_MACA0LR) = u32Temp;
}

/*********************************************************************************************************
** Function name:       EthernetMACAddrGet
** Descriptions:        ��ȡ��̫����MAC��ַ���������ʹ��MAC0��MAC1-MAC3��ʹ��
** input parameters:    u32Base      ��̫��ģ�����ַ
**                      pu8MacAddr  ��ȡ��MAC��ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetMACAddrGet(rt_uint32_t u32Base, rt_uint8_t *pu8MacAddr)
{
  rt_uint32_t u32Temp;
  
  u32Temp = HWREG32(u32Base + ETH_MACA0HR);
  pu8MacAddr[5] = ((u32Temp >> 8) & (rt_uint8_t)0xFF);
  pu8MacAddr[4] = (u32Temp & (rt_uint8_t)0xFF);
  
  u32Temp = HWREG32(u32Base + ETH_MACA0LR);
  pu8MacAddr[3] = ((u32Temp >> 24) & (rt_uint8_t)0xFF);
  pu8MacAddr[2] = ((u32Temp >> 16) & (rt_uint8_t)0xFF);
  pu8MacAddr[1] = ((u32Temp >> 8 ) & (rt_uint8_t)0xFF);
  pu8MacAddr[0] = (u32Temp & (rt_uint8_t)0xFF);
}

/*********************************************************************************************************
** Function name:       EthernetMACAddrExtSet
** Descriptions:        ������̫����MAC��ַ�����������MAC1-MAC3������
** input parameters:    u32Base      ��̫��ģ�����ַ
**                      u8Number��   ���õ�MAC��ַ��ţ�����ȡ1��2��3
**                      pu8MacAddr  ��Ҫ�趨��MAC��ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetMACAddrExtSet(rt_uint32_t u32Base, rt_uint8_t u8Number, rt_uint8_t *pu8MacAddr)
{
  rt_uint32_t u32Temp;
  
  if((u8Number < 1) || (u8Number > 3))
  {
    return;
  }
  
  u32Temp = (((rt_uint32_t)pu8MacAddr[5] << 8) | (rt_uint32_t)pu8MacAddr[4]);
  HWREG32(u32Base + ETH_MACA0HR + u8Number * 8) = u32Temp;
  
  u32Temp = (((rt_uint32_t)pu8MacAddr[3] << 24) | ((rt_uint32_t)pu8MacAddr[2] << 16) |
             ((rt_uint32_t)pu8MacAddr[1] << 8) | (rt_uint32_t)pu8MacAddr[0]);
  
  HWREG32(u32Base + ETH_MACA0LR + u8Number * 8) = u32Temp;
  
  HWREG32(u32Base + ETH_MACA0HR + u8Number * 8) |= ((1UL << 30) | (1UL << 31));
}

/*********************************************************************************************************
** Function name:       EthernetReset
** Descriptions:        �����λ��̫��������
** input parameters:    u32Base          ��̫��ģ�����ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetReset(rt_uint32_t u32Base)
{
  HWREG32(u32Base + ETH_DMAMR) |= 0x00000001;
}
/*********************************************************************************************************
** Function name:       EthernetResetStatusGet
** Descriptions:        �ж���̫���������Ƿ��ڸ�λ״̬
** input parameters:    u32Base          ��̫��ģ�����ַ
** output parameters:   NONE
** Returned value:      RT_TRUE:  ��̫�����������ڸ�λ״̬
**                      RT_FALSE����̫����������λ���
*********************************************************************************************************/
rt_bool_t EthernetResetStatusGet(rt_uint32_t u32Base)
{
  if((HWREG32(u32Base + ETH_DMAMR)) & 0x00000001)
    return RT_TRUE;
  else
    return RT_FALSE;
}
/*********************************************************************************************************
** Function name:       EthernetDMAIntEnable
** Descriptions:        ʹ����̫����������DMA�ж�
** input parameters:    u32Base          ��̫��ģ�����ַ
**                      u32Config        ʹ�ܵ��ж�Դ������ȡ����ֵ������ֵ�Ļ�
**                      DMA_INTRUPT_NSE         //  �����жϻ���ʹ��
**                      DMA_INTRUPT_ASE         //  �쳣�жϻ���ʹ��
**                      DMA_INTRUPT_CDEE        //  �����������ж�ʹ��
**                      DMA_INTRUPT_FBEE        //  �������������ж�ʹ��
**                      DMA_INTRUPT_ERIE        //  ���ڵĽ����ж�ʹ��
**                      DMA_INTRUPT_ETIE        //  ���ڵķ����ж�ʹ��
**                      DMA_INTRUPT_RWTE        //  ���տ��Ź���ʱ�ж�
**                      DMA_INTRUPT_RSE         //  ����ֹͣ�ж�ʹ��
**                      DMA_INTRUPT_RBUE        //  ���ջ������������ж�ʹ��
**                      DMA_INTRUPT_RIE         //  �����ж�ʹ��
**                      DMA_INTRUPT_TBUE        //  ���ͻ������������ж�ʹ��
**                      DMA_INTRUPT_TXSE        //  �����ж�ʹ��
**                      DMA_INTRUPT_TIE         //  �����ж�ʹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetDMAIntEnable(rt_uint32_t u32Base, rt_uint32_t u32Config)
{
  HWREG32(u32Base + ETH_DMACIER) |= u32Config;
}

/*********************************************************************************************************
** Function name:       EthernetDMAIntDisable
** Descriptions:        ������̫����������DMA�ж�
** input parameters:    u32Base          ��̫��ģ�����ַ
**                      u32Config        ���ܵ��ж�Դ������ȡ����ֵ������ֵ�Ļ�
**                      DMA_INTRUPT_NSE         //  �����жϻ���ʹ��
**                      DMA_INTRUPT_ASE         //  �쳣�жϻ���ʹ��
**                      DMA_INTRUPT_CDEE        //  �����������ж�ʹ��
**                      DMA_INTRUPT_FBEE        //  �������������ж�ʹ��
**                      DMA_INTRUPT_ERIE        //  ���ڵĽ����ж�ʹ��
**                      DMA_INTRUPT_ETIE        //  ���ڵķ����ж�ʹ��
**                      DMA_INTRUPT_RWTE        //  ���տ��Ź���ʱ�ж�
**                      DMA_INTRUPT_RSE         //  ����ֹͣ�ж�ʹ��
**                      DMA_INTRUPT_RBUE        //  ���ջ������������ж�ʹ��
**                      DMA_INTRUPT_RIE         //  �����ж�ʹ��
**                      DMA_INTRUPT_TBUE        //  ���ͻ������������ж�ʹ��
**                      DMA_INTRUPT_TXSE        //  �����ж�ʹ��
**                      DMA_INTRUPT_TIE         //  �����ж�ʹ��
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void EthernetDMAIntDisable(rt_uint32_t u32Base, rt_uint32_t u32Config)
{
  HWREG32(u32Base + ETH_DMACIER) &= ~u32Config;
}
/*********************************************************************************************************
END FILE
*********************************************************************************************************/
