/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_qspi.c
** Last modified Date:  2019-03-22
** Last Version:        v1.0
** Description:         quad spiģ��Ĵ�����װ����ʵ������
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo�κ���
** Created date:        2019-03-22
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
#include "hal/arch/inc/arch_io_rcc.h"
#include "hal/arch/inc/arch_io_qspi.h"


/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPIģ�鹤��ģʽ����
** input parameters:    u32Base      QSPIģ��Ļ���ַ
**                      u32Config:   �������ò���������ȡ���м������ֵĻ�
**                      
**                      // QSPI�����ʷ�Ƶ��ѡ��
**                      QSPI_CLOCK_PRESCALER(x)         //  ����ʱ�ӷ�Ƶ��ȡ1-256
**                      // Polling match mode����
**                      QSPI_AND_MATCH_MODE             // Polling match mode����ķ�ʽ
**                      QSPI_OR_MATCH_MODE              // Polling match mode����ķ�ʽ
**                      // Pool mode stopֹͣ��������
**                      QSPI_POLL_STOP_BY_DISABLE_QSPI  // Pool mode stop��ͨ����ֹQSPI
**                      QSPI_POLL_STOP_BY_MATCH         // Pool mode stop��ͨ��Poll Match
**                      // FIFO threshold level����
**                      QSPI_FIFO_THRESHOLD(x)          // FIFO������ã�ȡ1-32
**                      // Flash Memoryѡ�񣬵�ʹ��˫ͨ��Flashʱ�õ�
**                      QSPI_SELECTED_FLASH1            // ѡ��Flash1
**                      QSPI_SELECTED_FLASH2            // ѡ��Flash2
**                      //  ˫ͨ��Flash�Ƿ�ʹ������
**                      QSPI_DUAL_FLASH_DISABLE         // ˫ͨ��Flash��ֹ
**                      QSPI_DUAL_FLASH_ENABLE          // ˫ͨ��Flashʹ��
**                      // ������λ���ã� DDRģʽ��������ΪNO SHIFT
**                      QSPI_SAMPLE_NO_SHIFT            // ��������λ
**                      QSPI_SAMPLE_HALF_SHIFT          // ������λ���ʱ�ӣ� DDRģʽ������
**                      // ��ʱ������ʹ������
**                      QSPI_TIMEOUT_DISABLE            // Timeout counter��ֹ
**                      QSPI_TIMEOUT_ENABLE             // Timeout counterʹ��
**                      
** output parameters:   NONE
** Returned value:      NONE
** ��ʼ��ʱ����1�Σ����ú�QSPI�ǽ�ֹ��
*********************************************************************************************************/
void QSPIInit(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // ���������
    HWREG32(u32Base + QUADSPI_CR) = 0;
    HWREG32(u32Base + QUADSPI_CR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPIģ�鹤��ģʽ����
** input parameters:    u32Base      QSPIģ��Ļ���ַ
**                      u32Config:   �������ò���������ȡ���м������ֵĻ�
**                      
**                      // Flash�洢����������
**                      QSPI_FLASH_MEMORY_SIZE(x)       // �洢��������ȡ1-32����2���ֽڵ�4G�ֽڵ�����
**                      // Ƭѡ�źŸߵ�ƽʵ������
**                      QSPI_CS_HIGH_TIME(x)            // Ƭѡ�ߵ�ƽά��ʱ�䣬ȡ1-8
**                      // QSPI����ģʽ����
**                      QSPI_CKMODE_0                   // ����ģʽ0
**                      QSPI_CKMODE_3                   // ����ģʽ3
**                      
** output parameters:   NONE
** Returned value:      NONE
** �����ڽ�ֹQSPI�����µ���
*********************************************************************************************************/
void QSPIDeviceConfig(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // ���������
    HWREG32(u32Base + QUADSPI_DCR) = 0;
    HWREG32(u32Base + QUADSPI_DCR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIInit
** Descriptions:        QSPIģ�鹤��ģʽ����
** input parameters:    u32Base      QSPIģ��Ļ���ַ
**                      u32Config:   �������ò���������ȡ���м������ֵĻ�
**                      
**                      // DDRʹ�ܿ���
**                      QSPI_DDR_MODE_DISABLE            // ��ֹDDRģʽ
**                      QSPI_DDR_MODE_ENABLE             // ʹ��DDRģʽ
**                      // DDR Hold����
**                      QSPI_DDR_HOLD_BY_ANALOG_DELAY   // ��ģ�ⷽʽ��ʱDDR Hold
**                      QSPI_DDR_HOLD_BY_CLOCK          // ��ʱ���ź���ʱDDR Hold
**                      // Runing Mode����
**                      QSPI_NORMAL_CLOCK_MODE          // ����ģʽ
**                      QSPI_FREE_CLOCK_MODE            // Free running clock mode
**                      // ����ָ�����
**                      QSPI_SEND_INSTRUCTION_EVERY     // ÿ�δ��䶼����ָ��
**                      QSPI_SEND_INSTRUCTION_FIRST     // ֻ�е�һ��ͨ�Ųŷ���ָ��
**                      //  Functional mode����
**                      QSPI_INDIRECT_WRITE_MODE        // ���дģʽ 
**                      QSPI_INDIRECT_READ_MODE         // ��Ӷ�ģʽ
**                      QSPI_AUTO_POLLING_MODE          // �Զ���ѵģʽ
**                      QSPI_MEMMORY_MAP_MODE           // �洢��ӳ��ģʽ
**                      // ���ݸ�ʽ����
**                      QSPI_DATA_MODE_NO               // �����ݴ���
**                      QSPI_DATA_MODE_1LINE            // ���ݴ�����one line
**                      QSPI_DATA_MODE_2LINE            // ���ݴ�����double line
**                      QSPI_DATA_MODE_4LINE            // ���ݴ�����Four line
**                      // Dummy��������
**                      QSPI_DUMMY_CYCLES(x)            // Dummy���ڣ�ȡ0-31
**                      //  Alternate bytes size����
**                      QSPI_ALTERNATE_BYTES_8BIT       // Alternate bytes 8Bit
**                      QSPI_ALTERNATE_BYTES_16BIT      // Alternate bytes 16Bit
**                      QSPI_ALTERNATE_BYTES_24BIT      // Alternate bytes 24Bit
**                      QSPI_ALTERNATE_BYTES_32BIT      // Alternate bytes 32Bit
**                      // Alternate Mode����
**                      QSPI_ALTERNATE_MODE_NO          // ��Alternate Bytes����
**                      QSPI_ALTERNATE_MODE_1LINE       // Alternate Bytes������one line
**                      QSPI_ALTERNATE_MODE_2LINE       // Alternate Bytes������double line
**                      QSPI_ALTERNATE_MODE_4LINE       // Alternate Bytes������Four line
**                      // Address bytes size����
**                      QSPI_ADDRESS_BYTES_8BIT         // ��ַ bytes 8Bit
**                      QSPI_ADDRESS_BYTES_16BIT        // ��ַ bytes 8Bit
**                      QSPI_ADDRESS_BYTES_24BIT        // ��ַ bytes 8Bit
**                      QSPI_ADDRESS_BYTES_32BIT        // ��ַ bytes 8Bit
**                      // Address Mode����
**                      QSPI_ADDRESS_MODE_NO            // ��Address Bytes����
**                      QSPI_ADDRESS_MODE_1LINE         // Address Bytes������one line
**                      QSPI_ADDRESS_MODE_2LINE         // Address Bytes������double line
**                      QSPI_ADDRESS_MODE_4LINE         // Address Bytes������Four line
**                      // Instruction Mode����
**                      QSPI_INSTRUCTION_MODE_NO        // ��Instruction Bytes����
**                      QSPI_INSTRUCTION_MODE_1LINE     // Instruction Bytes������one line
**                      QSPI_INSTRUCTION_MODE_2LINE     // Instruction Bytes������double line
**                      QSPI_INSTRUCTION_MODE_4LINE     // Instruction Bytes������Four line
**                      // ���͵�Instruction
**                      QSPI_INSTRUCTION(x)             // ���͵�ָ�����ο�Memory���ֲ�
**                      
** output parameters:   NONE
** Returned value:      NONE
** �����ڽ�ֹQSPI�����µ���
*********************************************************************************************************/
void QSPICommunicationConfig(rt_uint32_t u32Base,rt_uint32_t u32Config)
{ 
    // ���������
    HWREG32(u32Base + QUADSPI_CCR) = 0;
    HWREG32(u32Base + QUADSPI_CCR) = u32Config;
}

/*********************************************************************************************************
** Function name:       QSPIEnable
** Descriptions:        QSPI����ʹ��
** input parameters:    u32Base      QSPIģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIEnable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) |= 0x00000001;
}

/*********************************************************************************************************
** Function name:       QSPIDisable
** Descriptions:        QSPI��������
** input parameters:    u32Base      QSPIģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIDisable(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) &= ~((rt_uint32_t)(0x00000001));
}

/*********************************************************************************************************
** Function name:       QSPIAbortRequest
** Descriptions:        QSPI��ֹ����
** input parameters:    u32Base      QSPIģ��Ļ���ַ
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPIAbortRequest(rt_uint32_t u32Base)
{
    HWREG32(u32Base + QUADSPI_CR) |= 0x00000002;
    HWREG32(u32Base + QUADSPI_CR) &= ~((rt_uint32_t)(0x00000002));
}

/*********************************************************************************************************
** Function name:       QSPITransfer
** Descriptions:        QSPIһ�δ���
** input parameters:    u32Base      QSPIģ��Ļ���ַ
**                      u32Config:   ��������
**                      u32Address:  �����flash��ַ
**                      size��       �������ݴ�С
**                      data:        ���������
** output parameters:   NONE
** Returned value:      NONE
*********************************************************************************************************/
void QSPITransfer(rt_uint32_t u32Base, rt_uint32_t u32Config, 
                  rt_uint32_t u32Address, rt_uint32_t size, void * data)
{
  while(0x00000020 & HWREG32(u32Base + QUADSPI_SR));
  /*set transfer size*/
  HWREG32(u32Base + QUADSPI_DLR) = size - 1;
  /*alternate byte set. Currently not supported*/
  HWREG32(u32Base + QUADSPI_ABR) = 0xFFFFFFFF;
  /*set transfer this will start the transfer if the IMODE != IMODE_NO*/
  HWREG32(u32Base + QUADSPI_CCR) = u32Config;
  /*Set address*/
  HWREG32(u32Base + QUADSPI_AR) = u32Address;
  
  /*Read or write data. Only if Size > 0*/
  for(uint32_t i = 0;i < size;++i)
  { /*Wait fifo threshold*/
    while(!(0x00000004 & HWREG32(u32Base + QUADSPI_SR)));
    if(0x00000000 == (u32Config & 0x0C000000))
    {/*Write */
      HWREG8(u32Base + QUADSPI_DR) = ((rt_uint8_t *)data)[i];
    }    
    else
    {/*Read*/
      ((rt_uint8_t *)data)[i] = HWREG8(u32Base + QUADSPI_DR);
    }
  }
  while(!(0x00000002 & HWREG32(u32Base + QUADSPI_SR)));
  HWREG32(u32Base + QUADSPI_FCR) |= 0x00000002;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
