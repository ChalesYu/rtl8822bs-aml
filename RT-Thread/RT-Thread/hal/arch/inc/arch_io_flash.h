/****************************************Copyright (c)****************************************************
**                             �� �� �� �� �� �� �� �� �� �� �� ˾
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_flash.h
** Last modified Date:  2013-04-04
** Last Version:        v1.0
** Description:         flashģ��Ĵ�����װ����ʵ������
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
#ifndef __ARCH_IO_FLASH_H__
#define __ARCH_IO_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rthw.h>

/*********************************************************************************************************
**  Flash��������
*********************************************************************************************************/
/*
** Flash��ʼ��ַ��0x08000000
** ����0�� 0x0800 0000 - 0x0801FFFF   ��128K
** ����1�� 0x0802 0000 - 0x0803FFFF   ��128K
** ����2�� 0x0804 0000 - 0x0805FFFF   ��128K
** ����3�� 0x0806 0000 - 0x0807FFFF   ��128K
** ����4�� 0x0808 0000 - 0x0809FFFF   ��128K
** ����5�� 0x080A 0000 - 0x080BFFFF   ��128K
** ����6�� 0x080C 0000 - 0x080DFFFF   ��128K
** ����7�� 0x080E 0000 - 0x080FFFFF   ��128K
 */


/*********************************************************************************************************
**  ��Ҫ�ĺ궨��
*********************************************************************************************************/
// Flash�ȴ����ڶ���
#define FLASH_Latency_0                0
#define FLASH_Latency_1                1
#define FLASH_Latency_2                2
#define FLASH_Latency_3                3
#define FLASH_Latency_4                4
#define FLASH_Latency_5                5
#define FLASH_Latency_6                6
#define FLASH_Latency_7                7

// Flash�����Ŷ���
#define FLASH_Sector_0                  ((rt_uint32_t)0)        /*!< Sector Number 0 */
#define FLASH_Sector_1                  ((rt_uint32_t)1)        /*!< Sector Number 1 */
#define FLASH_Sector_2                  ((rt_uint32_t)2)        /*!< Sector Number 2 */
#define FLASH_Sector_3                  ((rt_uint32_t)3)        /*!< Sector Number 3 */
#define FLASH_Sector_4                  ((rt_uint32_t)4)        /*!< Sector Number 4 */
#define FLASH_Sector_5                  ((rt_uint32_t)5)        /*!< Sector Number 5 */
#define FLASH_Sector_6                  ((rt_uint32_t)6)        /*!< Sector Number 6 */
#define FLASH_Sector_7                  ((rt_uint32_t)7)        /*!< Sector Number 7 */
   
//  Flash  BANK����
#define FLASH_BANK_1                    ((rt_uint32_t)0x01U)                          /*!< Bank 1   */
#define FLASH_BANK_2                    ((rt_uint32_t)0x02U)                          /*!< Bank 2   */
#define FLASH_BANK_BOTH                 ((rt_uint32_t)(FLASH_BANK_1 | FLASH_BANK_2)) /*!< Bank1 and Bank2  */

/*********************************************************************************************************
** �ⲿ����������
*********************************************************************************************************/
extern void FlashSetLatency(rt_uint8_t u8Latency);
extern void FlashUnlock(void);
extern void FlashLock(void);
extern rt_bool_t FlashEraseSector(rt_uint32_t u32Bank, rt_uint32_t u32Sector);
extern rt_bool_t FlashProgram256Bit(rt_uint32_t Address, rt_uint32_t DataAddress);
extern rt_bool_t FlashProgram(rt_uint32_t u32Addr, rt_uint32_t DataAddress, int i32Size);


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_FLASH_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
