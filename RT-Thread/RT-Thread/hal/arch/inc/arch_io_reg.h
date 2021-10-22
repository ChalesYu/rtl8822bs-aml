/****************************************Copyright (c)****************************************************
**                             成 都 世 纪 华 宁 科 技 有 限 公 司
**                                http://www.huaning-iot.com
**                                http://hichard.taobao.com
**
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:           arch_io_reg.h
** Last modified Date:  2019-03-15
** Last Version:        v1.0
** Description:         IO外设寄存器定义
**
**--------------------------------------------------------------------------------------------------------
** Created By:          Renhaibo任海波
** Created date:        2019-03-15
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

#ifndef __ARCH_IO_REG_H__
#define __ARCH_IO_REG_H__

#ifdef __cplusplus
extern "C" {
#endif
  
/*********************************************************************************************************
** 设备信息相关寄存器
*********************************************************************************************************/
#define UID0               0x0000
#define UID1               0x0004
#define UID2               0x0008
#define FLASH_SIZE         0x1FF1E880
  
/*********************************************************************************************************
** RCC模块寄存器定义
*********************************************************************************************************/
#define RCC_CR              0x0000
#define RCC_ICSCR           0x0004
#define RCC_CRRCR           0x0008
#define RCC_CFGR            0x0010
#define RCC_D1CFGR          0x0018
#define RCC_D2CFGR          0x001C
#define RCC_D3CFGR          0x0020
#define RCC_PLLCKSELR       0x0028
#define RCC_PLLCFGR         0x002C
#define RCC_PLL1DIVR        0x0030
#define RCC_PLL1FRACR       0x0034
#define RCC_PLL2DIVR        0x0038
#define RCC_PLL2FRACR       0x003C
#define RCC_PLL3DIVR        0x0040
#define RCC_PLL3FRACR       0x0044
#define RCC_D1CCIPR         0x004C
#define RCC_D2CCIP1R        0x0050
#define RCC_D2CCIP2R        0x0054
#define RCC_D3CCIPR         0x0058
#define RCC_CIER            0x0060
#define RCC_CIFR            0x0064
#define RCC_CICR            0x0068
#define RCC_BDCR            0x0070
#define RCC_CSR             0x0074
#define RCC_AHB3RSTR        0x007C
#define RCC_AHB1RSTR        0x0080
#define RCC_AHB2RSTR        0x0084
#define RCC_AHB4RSTR        0x0088
#define RCC_APB3RSTR        0x008C
#define RCC_APB1LRSTR       0x0090
#define RCC_APB1HRSTR       0x0094
#define RCC_APB2RSTR        0x0098
#define RCC_APB4RSTR        0x009C
#define RCC_GCR             0x00A0
#define RCC_D3AMR           0x00A8
#define RCC_RSR             0x00D0
#define RCC_AHB3ENR         0x00D4
#define RCC_AHB1ENR         0x00D8
#define RCC_AHB2ENR         0x00DC
#define RCC_AHB4ENR         0x00E0
#define RCC_APB3ENR         0x00E4
#define RCC_APB1LENR        0x00E8
#define RCC_APB1HENR        0x00EC
#define RCC_APB2ENR         0x00F0
#define RCC_APB4ENR         0x00F4
#define RCC_AHB3LPENR       0x00FC
#define RCC_AHB1LPENR       0x0100
#define RCC_AHB2LPENR       0x0104
#define RCC_AHB4LPENR       0x0108
#define RCC_APB3LPENR       0x010C
#define RCC_APB1LLPENR      0x0110
#define RCC_APB1HLPENR      0x0114
#define RCC_APB2LPENR       0x0118
#define RCC_APB4LPENR       0x011C
#define RCC_C1_AHB3ENR         0x0134
#define RCC_C1_AHB1ENR         0x0138
#define RCC_C1_AHB2ENR         0x013C
#define RCC_C1_AHB4ENR         0x0140
#define RCC_C1_APB3ENR         0x0144
#define RCC_C1_APB1LENR        0x0148
#define RCC_C1_APB1HENR        0x014C
#define RCC_C1_APB2ENR         0x0150
#define RCC_C1_APB4ENR         0x0154
#define RCC_C1_AHB3LPENR       0x015C
#define RCC_C1_AHB1LPENR       0x0160
#define RCC_C1_AHB2LPENR       0x0164
#define RCC_C1_AHB4LPENR       0x0168
#define RCC_C1_APB3LPENR       0x016C
#define RCC_C1_APB1LLPENR      0x0170
#define RCC_C1_APB1HLPENR      0x0174
#define RCC_C1_APB2LPENR       0x0178
#define RCC_C1_APB4LPENR       0x017C
  
/*********************************************************************************************************
** 独立看门狗模块的寄存器定义
*********************************************************************************************************/
#define IWDG_KR             0x0000
#define IWDG_PR             0x0004
#define IWDG_RLR            0x0008
#define IWDG_SR             0x000C
#define IWDG_WINR           0x0010
  
/*********************************************************************************************************
** PWR模块寄存器定义
*********************************************************************************************************/
#define PWR_CR1             0x0000
#define PWR_CSR1            0x0004
#define PWR_CR2             0x0008
#define PWR_CR3             0x000C
#define PWR_CPUCR           0x0010
#define PWR_D3CR            0x0018
#define PWR_WKUPCR          0x0020
#define PWR_WKUPFR          0x0024
#define PWR_WKUPEPR         0x0028
  
/*********************************************************************************************************
** CRS模块寄存器定义
*********************************************************************************************************/
#define CRS_CR              0x0000
#define CRS_CFGR            0x0004
#define CRS_ISR             0x0008
#define CRS_ICR             0x000C
  
/*********************************************************************************************************
** GPIO模块寄存器定义
*********************************************************************************************************/
#define GPIOx_MODER         0x0000
#define GPIOx_OTYPER        0x0004
#define GPIOx_OSPEEDR       0x0008
#define GPIOx_PUPDR         0x000C
#define GPIOx_IDR           0x0010
#define GPIOx_ODR           0x0014
#define GPIOx_BSRR          0x0018
#define GPIOx_LCKR          0x001C
#define GPIOx_AFRL          0x0020
#define GPIOx_AFRH          0x0024
// 一下寄存器是将BSRR寄存器拆分为高低16位，用于分开访问
#define GPIOx_BSRR_SET      0x0018
#define GPIOx_BSRR_RESET    0x001A

/*********************************************************************************************************
** Flash控制模块寄存器定义
*********************************************************************************************************/
#define FLASH_ACR1          0x0000
#define FLASH_KEYR1         0x0004
#define FLASH_OPTKEYR1      0x0008
#define FLASH_CR1           0x000C
#define FLASH_SR1           0x0010
#define FLASH_CCR1          0x0014
#define FLASH_OPTCR1        0x0018
#define FLASH_OPTSR_CUR     0x001C
#define FLASH_OPTSRPRG      0x0020
#define FLASH_OPTCCR        0x0024
#define FLASH_PRAR_CUR1     0x0028
#define FLASH_PRAG_PRG1     0x002C
#define FLASH_SCAR_CUR1     0x0030
#define FLASH_SCAR_PRG1     0x0034
#define FLASH_WPSN_CUR1R    0x0038
#define FLASH_WPSN_PRG1R    0x003C
#define FLASH_BOOT_CURR     0x0040
#define FLASH_BOOT_PRGR     0x0044
#define FLASH_CRCCR1        0x0050
#define FLASH_CRCSADD1R     0x0054
#define FLASH_CRCEADD1R     0x0058
#define FLASH_CRCDATAR      0x005C
#define FLASH_ECCFA1R       0x0060

#define FLASH_ACR2          0x0100
#define FLASH_KEYR2         0x0104
#define FLASH_OPTKEYR2      0x0108
#define FLASH_CR2           0x010C
#define FLASH_SR2           0x0110
#define FLASH_CCR2          0x0114
#define FLASH_OPTCR2        0x0118
#define FLASH_OPTSR_CUR2    0x011C
#define FLASH_OPTSRPRG2     0x0120
#define FLASH_OPTCCR2       0x0124
#define FLASH_PRAR_CUR2     0x0128
#define FLASH_PRAG_PRG2     0x012C
#define FLASH_SCAR_CUR2     0x0130
#define FLASH_SCAR_PRG2     0x0134
#define FLASH_WPSN_CUR2R    0x0138
#define FLASH_WPSN_PRG2R    0x013C
#define FLASH_BOOT_CURR2    0x0140
#define FLASH_BOOT_PRGR2    0x0144
#define FLASH_CRCCR2        0x0150
#define FLASH_CRCSADD2R     0x0154
#define FLASH_CRCEADD2R     0x0158
#define FLASH_CRCDATAR2     0x015C
#define FLASH_ECCFA2R       0x0160

/*********************************************************************************************************
** SYSCFG模块寄存器定义
*********************************************************************************************************/
#define SYSCFG_PMCR         0x0004
#define SYSCFG_EXTICR1      0x0008
#define SYSCFG_EXTICR2      0x000C
#define SYSCFG_EXTICR3      0x0010
#define SYSCFG_EXTICR4      0x0014
#define SYSCFG_CFGR         0x0018
#define SYSCFG_CCCSR        0x0020
#define SYSCFG_CCVR         0x0024
#define SYSCFG_CCCR         0x0028
#define SYSCFG_PKGR         0x0124
#define SYSCFG_UR0          0x0300
#define SYSCFG_UR2          0x0308
#define SYSCFG_UR3          0x030C
#define SYSCFG_UR4          0x0310
#define SYSCFG_UR5          0x0314
#define SYSCFG_UR6          0x0318
#define SYSCFG_UR7          0x031C
#define SYSCFG_UR8          0x0320
#define SYSCFG_UR9          0x0324
#define SYSCFG_UR10         0x0328
#define SYSCFG_UR11         0x032C
#define SYSCFG_UR12         0x0330
#define SYSCFG_UR13         0x0334
#define SYSCFG_UR14         0x0338
#define SYSCFG_UR15         0x033C
#define SYSCFG_UR16         0x0340
#define SYSCFG_UR17         0x0344
  
/*********************************************************************************************************
** USART模块寄存器定义
*********************************************************************************************************/
#define USART_CR1           0x0000
#define USART_CR2           0x0004
#define USART_CR3           0x0008 
#define USART_BRR           0x000C
#define USART_GTPR          0x0010
#define USART_RTOR          0x0014
#define USART_RQR           0x0018
#define USART_ISR           0x001C
#define USART_ICR           0x0020
#define USART_RDR           0x0024
#define USART_TDR           0x0028
#define USART_PRESC         0x002C
  
///*********************************************************************************************************
//** LPUART模块寄存器定义
//*********************************************************************************************************/
//#define LPUARTx_CR1         0x0000
//#define LPUARTx_CR2         0x0004
//#define LPUARTx_CR3         0x0008
//#define LPUARTx_BRR         0x000C
//#define LPUARTx_RQR         0x0018
//#define LPUARTx_ISR         0x001C
//#define LPUARTx_ICR         0x0020
//#define LPUARTx_RDR         0x0024
//#define LPUARTx_TDR         0x0028

///*********************************************************************************************************
//** LP Timer模块寄存器定义
//*********************************************************************************************************/
//#define LPTIMx_ISR         0x0000
//#define LPTIMx_ICR         0x0004
//#define LPTIMx_IER         0x0008
//#define LPTIMx_CFGR        0x000C
//#define LPTIMx_CR          0x0010
//#define LPTIMx_CMP         0x0014
//#define LPTIMx_ARR         0x0018
//#define LPTIMx_CNT         0x001C
//#define LPTIMx_OR          0x0020
//#define LPTIMx_CFGR2       0x0024
//#define LPTIMx_RCR         0x0028
  
/*********************************************************************************************************
** SPI及I2S模块寄存器定义
*********************************************************************************************************/
#define SPI2S_CR1           0x0000
#define SPI_CR2             0x0004
#define SPI_CFG1            0x0008
#define SPI_CFG2            0x000C
#define SPI2S_IER           0x0010 
#define SPI2S_SR            0x0014
#define SPI2S_IFCR          0x0018 
#define SPI2S_TXDR          0x0020
#define SPI2S_RXDR          0x0030
#define SPI_CRCPOLY         0x0040 
#define SPI_TXCRC           0x0044
#define SPI_RXCRC           0x0048
#define SPI_UDRDR           0x004C
#define SPI_I2SCFGR         0x0050
#define SPI_I2SPR           0x0052

/*********************************************************************************************************
** QUADSPI模块寄存器定义
*********************************************************************************************************/
#define QUADSPI_CR           0x0000
#define QUADSPI_DCR          0x0004
#define QUADSPI_SR           0x0008
#define QUADSPI_FCR          0x000C
#define QUADSPI_DLR          0x0010
#define QUADSPI_CCR          0x0014
#define QUADSPI_AR           0x0018
#define QUADSPI_ABR          0x001C
#define QUADSPI_DR           0x0020
#define QUADSPI_PSMKR        0x0024
#define QUADSPI_PSMAR        0x0028
#define QUADSPI_PIR          0x002C
#define QUADSPI_LPTR         0x0030
  
/*********************************************************************************************************
** RTC模块寄存器定义
*********************************************************************************************************/
#define RTC_TR             0x0000
#define RTC_DR             0x0004
#define RTC_CR             0x0008
#define RTC_ISR            0x000C
#define RTC_PRER           0x0010
#define RTC_WUTR           0x0014
#define RTC_ALRMAR         0x001C
#define RTC_ALRMBR         0x0020
#define RTC_WPR            0x0024
#define RTC_SSR            0x0028
#define RTC_SHIFTR         0x002C
#define RTC_TSTR           0x0030
#define RTC_TSDR           0x0034
#define RTC_TSSSR          0x0038
#define RTC_CALR           0x003C
#define RTC_TAMPCR         0x0040
#define RTC_ALRMASSR       0x0044
#define RTC_ALRMBSSR       0x0048
#define RTC_OR             0x004C
#define RTC_BKP_BASE       0x0050   // 备份寄存器，一共32个32位寄存器

/*********************************************************************************************************
** EXTI模块寄存器定义
*********************************************************************************************************/
#define EXTI_RTSR1          0x0000
#define EXTI_FTSR1          0x0004
#define EXTI_SWIER1         0x0008
#define EXTI_D3PMR1         0x000C
#define EXTI_D3PCR1L        0x0010
#define EXTI_D3PCR1H        0x0014
  
#define EXTI_RTSR2          0x0020
#define EXTI_FTSR2          0x0024
#define EXTI_SWIER2         0x0028
#define EXTI_D3PMR2         0x002C
#define EXTI_D3PCR2L        0x0030
#define EXTI_D3PCR2H        0x0034
  
#define EXTI_RTSR3          0x0040
#define EXTI_FTSR3          0x0044
#define EXTI_SWIER3         0x0048
#define EXTI_D3PMR3         0x004C
#define EXTI_D3PCR3L        0x0050
#define EXTI_D3PCR3H        0x0054 
  
#define EXTI_CPUIMR1        0x0080
#define EXTI_CPUEMR1        0x0084
#define EXTI_CPUPR1         0x0088
#define EXTI_CPUIMR2        0x0090
#define EXTI_CPUEMR2        0x0094
#define EXTI_CPUPR2         0x0098
#define EXTI_CPUIMR3        0x00A0
#define EXTI_CPUEMR3        0x00A4
#define EXTI_CPUPR3         0x00A8

///*********************************************************************************************************
//** SAI模块的寄存器定义
//*********************************************************************************************************/
//#define SAI_GCR             0x0000
//#define SAI_xCR1            0x0004
//#define SAI_xCR2            0x0008
//#define SAI_xFRCR           0x000C
//#define SAI_xSLOTR          0x0010
//#define SAI_xIM             0x0014
//#define SAI_xSR             0x0018
//#define SAI_xCLRFR          0x001C
//#define SAI_xDR             0x0020

/*********************************************************************************************************
** DMAMUX模块寄存器定义
*********************************************************************************************************/
//  控制器寄存器对于DMAMUX1有16个，对于DMAMUX2有8个，这里给出基地址
#define DMAMUX_C0CR           0x0000
#define DMAMUX_CSR            0x0080
#define DMAMUX_CFR            0x0084
// 该寄存器有8个，这里给出基地址
#define DMAMUX_RG0CR          0x0100
#define DMAMUX_RGSR           0x0140
#define DMAMUX_RGCFR          0x0144
  
/*********************************************************************************************************
** DMA模块寄存器定义
*********************************************************************************************************/
#define BDMA_ISR             0x0000    //  中断状态寄存器
#define BDMA_IFCR            0x0004    //  中断清除寄存器
#define BDMA_CCR1            0x0008    //  DMA通道x配置寄存器，这里为通道1，其它通道地址为该值+(0x14*(通道号))
#define BDMA_CNDTR1          0x000C    //  DMA通道x传输数量寄存器，这里为通道1，其它通道地址为该值+(0x14*(通道号))
#define BDMA_CPAR1           0x0010    //  DMA通道x外设地址寄存器，这里为通道1，其它通道地址为该值+(0x14*(通道号)
#define BDMA_CM0AR1          0x0014    //  DMA通道x存储器地址寄存器，这里为通道1，其它通道地址为该值+(0x14*(通道号))
#define BDMA_CM1AR1          0x0018    //  DMA通道x存储器地址寄存器，这里为通道1，其它通道地址为该值+(0x14*(通道号))
  
/*********************************************************************************************************
** DMA模块寄存器定义
*********************************************************************************************************/
#define DMA_LISR            0x0000
#define DMA_HISR            0x0004
#define DMA_LIFCR           0x0008
#define DMA_HIFCR           0x000C
#define DMA_SxCR            0x0010   // 其它Stream 0x18 * x(0 <= x <= 7)
#define DMA_SxNDTR          0x0014   // 其它Stream 0x18 * x(0 <= x <= 7)
#define DMA_SxPAR           0x0018   // 其它Stream 0x18 * x(0 <= x <= 7)
#define DMA_SxMOAR          0x001C   // 其它Stream 0x18 * x(0 <= x <= 7)
#define DMA_SxM1AR          0x0020   // 其它Stream 0x18 * x(0 <= x <= 7)
#define DMA_SxFCR           0x0024   // 其它Stream 0x18 * x(0 <= x <= 7)

/*********************************************************************************************************
** Ethernet模块寄存器定义
*********************************************************************************************************/
/*  下面是以太网MAC和MMC寄存器的定义  */
#define ETH_MACCR           0x0000
#define ETH_MACECR          0x0004
#define ETH_MACPFR          0x0008
#define ETH_MACWTR          0x000C
#define ETH_MACHT0R         0x0010
#define ETH_MACHT1R         0x0014

#define ETH_MACVTR          0x0050
#define ETH_MACVHTR         0x0058
#define ETH_MACVIR          0x0060
#define ETH_MACIVIR         0x0064
#define ETH_MACQTXFCR       0x0070
#define ETH_MACRXFCR        0x0090
  
#define ETH_MACISR          0x00B0
#define ETH_MACIER          0x00B4
#define ETH_MACRXTXSR       0x00B8
  
#define ETH_MACPCSR         0x00C0
#define ETH_MACRWKPFR       0x00C4
  
#define ETH_MACLCSR         0x00D0
#define ETH_MACLTCR         0x00D4
#define ETH_MACLETR         0x00D8
#define ETH_MAC1USTCR       0x00DC
  
#define ETH_MACVR           0x0110 
#define ETH_MACDR           0x0114
#define ETH_MACHWF1R        0x0120
#define ETH_MACHWF2R        0x0124
  
#define ETH_MACMDIOAR       0x0200
#define ETH_MACMDIODR       0x0204
#define ETH_MACARPAR        0x0AE0
  
#define ETH_MACA0HR         0x0300
#define ETH_MACA0LR         0x0304
#define ETH_MACA1HR         0x0308
#define ETH_MACA1LR         0x030C
#define ETH_MACA2HR         0x0310
#define ETH_MACA2LR         0x0314
#define ETH_MACA3HR         0x0318
#define ETH_MACA3LR         0x031C

#define ETH_MMC_CONTROL                 0x0700
#define ETH_MMC_RX_INTERRUPT            0x0704
#define ETH_MMC_TX_INTERRUPT            0x0708
#define ETH_MMC_RX_INTERRUPT_MASK       0x070C
#define ETH_MMC_TX_INTERRUPT_MASK       0x0710
  
#define ETH_TX_SINGLE_COLLISION_GOOD_PACKETS    0x074C  
#define ETH_TX_MULTIPLE_COLLISION_GOOD_PACKETS  0x075C
#define ETH_TX_PACKET_COUNT_GOOD                0x0768
#define ETH_RX_CRC_ERROR_PACKETS                0x0794
#define ETH_RX_ALIGNMENT_ERROR_PACKETS          0x0798
#define ETH_RX_UNICAST_PACKETS_GOOD             0x07C4
#define ETH_TX_LPI_USEC_CNTR                    0x07EC
#define ETH_TX_LPI_TRAN_CNTR                    0x07F0
#define ETH_RX_LPI_USEC_CNTR                    0x07F4
#define ETH_RX_LPI_TRAN_CNTR                    0x07F8
  
#define ETH_MACL3L4C0R                  0x0900
#define ETH_MACL4A0R                    0x0904
#define ETH_MACL3A00R                   0x0910
#define ETH_MACL3A10R                   0x0914
#define ETH_MACL3A20R                   0x0918
#define ETH_MACL3A30R                   0x091C
  
#define ETH_MACL3L4C1R                  0x0930
#define ETH_MACL4A1R                    0x0934
#define ETH_MACL3A01R                   0x0940
#define ETH_MACL3A11R                   0x0944
#define ETH_MACL3A21R                   0x0948
#define ETH_MACL3A31R                   0x094C
  
#define ETH_MACTSCR             0x0B00
#define ETH_MACSSIR             0x0B04 
#define ETH_MACSTSR             0x0B08 
#define ETH_MACSTNR             0x0B0C 
#define ETH_MACSTSUR            0x0B10
#define ETH_MACSTNUR            0x0B14 
#define ETH_MACTSAR             0x0B18 

#define ETH_MACTSSR             0x0B20
#define ETH_MACTXTSSNR          0x0B30
#define ETH_MACTXTSSSR          0x0B34
#define ETH_MACACR              0x0B40
#define ETH_MACATSNR            0x0B48
#define ETH_MACATSSR            0x0B4C
#define ETH_MACTSIACR           0x0B50
#define ETH_MACTSEACR           0x0B54
#define ETH_MACTSICNR           0x0B58
#define ETH_MACTSECNR           0x0B5C
  
#define ETH_MACPPSCR            0x0B70
#define ETH_MACPPSTTSR          0x0B80
#define ETH_MACPPSTTNR          0x0B84
#define ETH_MACPPSIR            0x0B88
#define ETH_MACPPSWR            0x0B8C
#define ETH_MACPOCR             0x0BC0
#define ETH_MACSPI0R            0x0BC4
#define ETH_MACSPI1R            0x0BC8
#define ETH_MACSPI2R            0x0BCC
#define ETH_MACLMIR             0x0BD0
  
/*  下面是以太网MTL寄存器的定义  */
#define ETH_MTLOMR              0x0C00
#define ETH_MTLISR              0x0C20
#define ETH_MTLTXQOMR           0x0D00
#define ETH_MTLTXQUR            0x0D04
#define ETH_MTLTXQDR            0x0D08
#define ETH_MTLQICSR            0x0D2C
#define ETH_MTLRXQOMR           0x0D30
#define ETH_MTLRXQMPOCR         0x0D34
#define ETH_MTLRXQDR            0x0D38
  

/*  下面是以太网DMA寄存器的定义  */
#define ETH_DMAMR               0x1000
#define ETH_DMASBMR             0x1004
#define ETH_DMAAISR             0x1008
#define ETH_DMADSR              0x100C

#define ETH_DMACCR              0x1100
#define ETH_DMACTXCR            0x1104
#define ETH_DMACRXCR            0x1108
#define ETH_DMACTXDLAR          0x1114
#define ETH_DMACRXDLAR          0x111C
#define ETH_DMACTXDTPR          0x1120
#define ETH_DMACRXDTPR          0x1128
#define ETH_DMACTXDRLR          0x112C
#define ETH_DMACRXDRLR          0x1130
#define ETH_DMACIER             0x1134
#define ETH_DMACRXIWTR          0x1138
#define ETH_DMACCATXDR          0x1144
#define ETH_DMACCARXDR          0x114C
#define ETH_DMACCATXBR          0x1154
#define ETH_DMACCARXBR          0x115C
#define ETH_DMACSR              0x1160
#define ETH_DMACMFCR            0x116C

/*********************************************************************************************************
** FMC模块寄存器定义
*********************************************************************************************************/
#define FMC_BCR1                0x0000
#define FMC_BCR2                0x0008
#define FMC_BCR3                0x0010
#define FMC_BCR4                0x0018
#define FMC_BTR1                0x0004
#define FMC_BTR2                0x000C
#define FMC_BTR3                0x0014
#define FMC_BTR4                0x001C
#define FMC_BWTR1               0x0104
#define FMC_BWTR2               0x010C
#define FMC_BWTR3               0x0114
#define FMC_BWTR4               0x011C
#define FMC_PCR2                0x0060
#define FMC_PCR3                0x0080
#define FMC_PCR4                0x00A0
#define FMC_SR2                 0x0064
#define FMC_SR3                 0x0084
#define FMC_SR4                 0x00A4
#define FMC_PMEM2               0x0068
#define FMC_PMEM3               0x0088
#define FMC_PMEM4               0x00A8
#define FMC_PATT2               0x006C
#define FMC_PATT3               0x008C
#define FMC_PATT4               0x00AC
#define FMC_PIO4                0x00B0
#define FMC_ECCR2               0x0074
#define FMC_ECCR3               0x0094

#define FMC_SDCR1               0x0140
#define FMC_SDCR2               0x0144
#define FMC_SDTR1               0x0148
#define FMC_SDTR2               0x014C
#define FMC_SDCMR               0x0150
#define FMC_SDRTR               0x0154
#define FMC_SDSR                0x0158
  
/*********************************************************************************************************
**   ADC模块寄存器定义
**   Offset             Register
**   0x0000 - 0x00D0    Master ADC1 or Master ADC3
**   0x00D4 - 0x00FC    Reserved
**   0x0100 - 0x01D0    Slave ADC2
**   0x01D4 - 0x02FC    Reserved
**   0x0300 - 0x0310    Common registers
*********************************************************************************************************/
// ADC register map and reset values for each ADC (offset=0x000 for master ADC, 0x100 for slave ADC)
#define ADC_ISR           0x0000
#define ADC_IER           0x0004 
#define ADC_CR            0x0008
#define ADC_CFGR          0x000C
#define ADC_CFGR2         0x0010 
#define ADC_SMPR1         0x0014 
#define ADC_SMPR2         0x0018
#define ADC_PCSEL         0x001C
#define ADC_LTR1          0x0020
#define ADC_HTR1          0x0024
#define ADC_SQR1          0x0030
#define ADC_SQR2          0x0034
#define ADC_SQR3          0x0038
#define ADC_SQR4          0x003C 
#define ADC_DR            0x0040
#define ADC_JSQR          0x004C
#define ADC_OFR1          0x0060
#define ADC_OFR2          0x0064 
#define ADC_OFR3          0x0068
#define ADC_OFR4          0x006C
#define ADC_JDR1          0x0080
#define ADC_JDR2          0x0084 
#define ADC_JDR3          0x0088
#define ADC_JDR4          0x008C
#define ADC_AWD2CR        0x00A0
#define ADC_AWD3CR        0x00A4 
#define ADC_LTR2          0x00B0
#define ADC_HTR2          0x00B4
#define ADC_LTR3          0x00B8
#define ADC_HTR3          0x00BC
#define ADC_DIFSEL        0x00C0
#define ADC_CALFACT       0x00C4
#define ADC_CALFACT1      0x00C8
  
// ADC register map and reset values (master and slave ADC common registers) offset =0x300)
#define ADC_CSR           0x0000
#define ADC_CCR           0x0008
#define ADC_CDR           0x000C
#define ADC_CDR2          0x0010
  
/*********************************************************************************************************
** VREF模块的寄存器定义
*********************************************************************************************************/
#define VREFBUF_CSR      0x0000
#define VREFBUF_CCR      0x0004
  
/*********************************************************************************************************
** TIM模块的寄存器定义
*********************************************************************************************************/
#define TIMx_CR1            0x0000
#define TIMx_CR2            0x0004
#define TIMx_SMCR           0x0008
#define TIMx_DIER           0x000C
#define TIMx_SR             0x0010
#define TIMx_EGR            0x0014
#define TIMx_CCMR1          0x0018
#define TIMx_CCMR2          0x001C
#define TIMx_CCER           0x0020
#define TIMx_CNT            0x0024
#define TIMx_PSC            0x0028
#define TIMx_ARR            0x002C
#define TIMx_CCR1           0x0034
#define TIMx_CCR2           0x0038
#define TIMx_CCR3           0x003C
#define TIMx_CCR4           0x0040
#define TIMx_DCR            0x0048
#define TIMx_DMAR           0x004C

/*********************************************************************************************************
** SDIO模块寄存器定义
*********************************************************************************************************/
#define SDMMC_POWER          0x0000
#define SDMMC_CLKCR          0x0004
#define SDMMC_ARG            0x0008
#define SDMMC_CMD            0x000C
#define SDMMC_RESPCMD        0x0010

#define SDMMC_RESP_BASE      0x0014
#define SDMMC_RESP1          0x0000
#define SDMMC_RESP2          0x0004
#define SDMMC_RESP3          0x0008
#define SDMMC_RESP4          0x000C

#define SDMMC_DTIMER         0x0024
#define SDMMC_DLEN           0x0028
#define SDMMC_DCTRL          0x002C
#define SDMMC_DCOUNT         0x0030
#define SDMMC_STA            0x0034
#define SDMMC_ICR            0x0038
#define SDMMC_MASK           0x003C
#define SDMMC_ACKTIME        0x0040

#define SDMMC_IDMACTRL       0x0050
#define SDMMC_IDMABSIZE      0x0054
#define SDMMC_IDMABASE0      0x0058
#define SDMMC_IDMABASE1      0x005C
  
#define SDMMC_FIFO           0x0080

/*********************************************************************************************************
** USB FS与HS模块寄存器定义，HS USB比FS USB寄存器多一些，大多向下兼容
*********************************************************************************************************/
// Core global control and status registers (CSRs)
#define USB_OTG_GOTGCTL         0x0000
#define USB_OTG_GOTGINT         0x0004
#define USB_OTG_GAHBCFG         0x0008
#define USB_OTG_GUSBCFG         0x000C
#define USB_OTG_GRSTCTL         0x0010
#define USB_OTG_GINTSTS         0x0014
#define USB_OTG_GINTMSK         0x0018
#define USB_OTG_GRXSTSR         0x001C  
#define USB_OTG_GRXSTSP         0x0020
#define USB_OTG_GRXFSIZ         0x0024
#define USB_OTG_GNPTXFSIZ_TX0FSIZ       0x0028
#define USB_OTG_GNPTXSTS        0x002C
#define USB_OTG_GCCFG           0x0038
#define USB_OTG_CID             0x003C

#define USB_OTG_GSNPSID         0x0040
#define USB_OTG_GHWCFG1         0x0044
#define USB_OTG_GHWCFG2         0x0048
#define USB_OTG_GHWCFG3         0x004C
#define USB_OTG_GLPMCFG         0x0054
#define USB_OTG_GPWRDN          0x0058
#define USB_OTG_GDFIFOCFG       0x005C
#define USB_OTG_GADPCTL         0x0060

#define USB_OTG_HPTXFSIZ        0x0100

// 下面寄存器共8个，地址0x0104 + 0x04 * (x-1), x取1..8
#define USB_OTG_DIEPTXF1        0x0104
#define USB_OTG_DIEPTXF2        0x0108
#define USB_OTG_DIEPTXF3        0x010C
#define USB_OTG_DIEPTXF4        0x0110
#define USB_OTG_DIEPTXF5        0x0114
#define USB_OTG_DIEPTXF6        0x0118
#define USB_OTG_DIEPTXF7        0x011C 
#define USB_OTG_DIEPTXF8        0x0120 

// Host-mode control and status registers (CSRs)
#define USB_OTG_HCFG            0x0400
#define USB_OTG_HFIR            0x0404
#define USB_OTG_HFNUM           0x0408
#define USB_OTG_HPTXSTS         0x0410
#define USB_OTG_HAINT           0x0414
#define USB_OTG_HAINTMSK        0x0418
#define USB_OTG_HPRT            0x0440

// 下面寄存器共16个，地址0x0500 + 0x20 * x, x取0..15
#define USB_OTG_HCCHAR0         0x0500
#define USB_OTG_HCCHAR1         0x0520
#define USB_OTG_HCCHAR2         0x0540
#define USB_OTG_HCCHAR3         0x0560
#define USB_OTG_HCCHAR4         0x0580
#define USB_OTG_HCCHAR5         0x05A0
#define USB_OTG_HCCHAR6         0x05C0
#define USB_OTG_HCCHAR7         0x05E0
#define USB_OTG_HCCHAR8         0x0600
#define USB_OTG_HCCHAR9         0x0620
#define USB_OTG_HCCHAR10        0x0640
#define USB_OTG_HCCHAR11        0x0660
#define USB_OTG_HCCHAR12        0x0680
#define USB_OTG_HCCHAR13        0x06A0
#define USB_OTG_HCCHAR14        0x06C0
#define USB_OTG_HCCHAR15        0x06E0

// 下面寄存器共16个，地址0x0504 + 0x20 * x, x取0..15
#define USB_OTG_HCSPLT0         0x0504
#define USB_OTG_HCSPLT1         0x0524
#define USB_OTG_HCSPLT2         0x0544
#define USB_OTG_HCSPLT3         0x0564
#define USB_OTG_HCSPLT4         0x0584
#define USB_OTG_HCSPLT5         0x05A4
#define USB_OTG_HCSPLT6         0x05C4
#define USB_OTG_HCSPLT7         0x05E4
#define USB_OTG_HCSPLT8         0x0604
#define USB_OTG_HCSPLT9         0x0624
#define USB_OTG_HCSPLT10        0x0644
#define USB_OTG_HCSPLT11        0x0664
#define USB_OTG_HCSPLT12        0x0684
#define USB_OTG_HCSPLT13        0x06A4
#define USB_OTG_HCSPLT14        0x06C4
#define USB_OTG_HCSPLT15        0x06E4

// 下面寄存器共16个，地址0x0508 + 0x20 * x, x取0..15
#define USB_OTG_HCINT0          0x0508
#define USB_OTG_HCINT1          0x0528  
#define USB_OTG_HCINT2          0x0548
#define USB_OTG_HCINT3          0x0568
#define USB_OTG_HCINT4          0x0588
#define USB_OTG_HCINT5          0x05A8
#define USB_OTG_HCINT6          0x05C8
#define USB_OTG_HCINT7          0x05E8
#define USB_OTG_HCINT8          0x0608
#define USB_OTG_HCINT9          0x0628
#define USB_OTG_HCINT10         0x0648
#define USB_OTG_HCINT11         0x0668
#define USB_OTG_HCINT12         0x0688
#define USB_OTG_HCINT13         0x06A8
#define USB_OTG_HCINT14         0x06C8
#define USB_OTG_HCINT15         0x06E8
  
// 下面寄存器共16个，地址0x050C + 0x20 * x, x取0..15
#define USB_OTG_HCINTMSK0       0x050C
#define USB_OTG_HCINTMSK1       0x052C
#define USB_OTG_HCINTMSK2       0x054C
#define USB_OTG_HCINTMSK3       0x056C
#define USB_OTG_HCINTMSK4       0x058C
#define USB_OTG_HCINTMSK5       0x05AC
#define USB_OTG_HCINTMSK6       0x05CC
#define USB_OTG_HCINTMSK7       0x05EC
#define USB_OTG_HCINTMSK8       0x060C
#define USB_OTG_HCINTMSK9       0x062C
#define USB_OTG_HCINTMSK10      0x064C
#define USB_OTG_HCINTMSK11      0x066C
#define USB_OTG_HCINTMSK12      0x068C
#define USB_OTG_HCINTMSK13      0x06AC
#define USB_OTG_HCINTMSK14      0x06CC
#define USB_OTG_HCINTMSK15      0x06EC

// 下面寄存器共116个，地址0x0510 + 0x20 * x, x取0..15
#define USB_OTG_HCTSIZ0         0x0510
#define USB_OTG_HCTSIZ1         0x0530
#define USB_OTG_HCTSIZ2         0x0550
#define USB_OTG_HCTSIZ3         0x0570
#define USB_OTG_HCTSIZ4         0x0590
#define USB_OTG_HCTSIZ5         0x05B0
#define USB_OTG_HCTSIZ6         0x05D0
#define USB_OTG_HCTSIZ7         0x05F0
#define USB_OTG_HCTSIZ8         0x0610
#define USB_OTG_HCTSIZ9         0x0630
#define USB_OTG_HCTSIZ10        0x0650
#define USB_OTG_HCTSIZ11        0x0670
#define USB_OTG_HCTSIZ12        0x0690
#define USB_OTG_HCTSIZ13        0x06B0
#define USB_OTG_HCTSIZ14        0x06D0
#define USB_OTG_HCTSIZ15        0x06F0

// 下面寄存器共16个，地址0x0514 + 0x20 * x, x取0..16
#define USB_OTG_HCDMA0          0x0514
#define USB_OTG_HCDMA1          0x0534
#define USB_OTG_HCDMA2          0x0554
#define USB_OTG_HCDMA3          0x0574
#define USB_OTG_HCDMA4          0x0594
#define USB_OTG_HCDMA5          0x05B4
#define USB_OTG_HCDMA6          0x05D4
#define USB_OTG_HCDMA7          0x05F4
#define USB_OTG_HCDMA8          0x0614
#define USB_OTG_HCDMA9          0x0634
#define USB_OTG_HCDMA10         0x0654
#define USB_OTG_HCDMA11         0x0674

// Device-mode control and status registers
#define USB_OTG_DCFG            0x0800
#define USB_OTG_DCTL            0x0804
#define USB_OTG_DSTS            0x0808
#define USB_OTG_DIEPMSK         0x0810
#define USB_OTG_DOEPMSK         0x0814
#define USB_OTG_DAINT           0x0818
#define USB_OTG_DAINTMSK        0x081C
#define USB_OTG_DVBUSDIS        0x0828
#define USB_OTG_DVBUSPULSE      0x082C
#define USB_OTG_DTHRCTL         0x0830
#define USB_OTG_DIEPEMPMSK      0x0834
#define USB_OTG_DEACHINT        0x0838
#define USB_OTG_DEACHINTMSK     0x083C
#define USB_OTG_DIEPEACHMSK1    0x0844
#define USB_OTG_DOEPEACHMSK1    0x0884

// 下面寄存器共8个，地址0x0900 + 0x20 * x, x取0..7
#define USB_OTG_DIEPCTL0        0x0900
#define USB_OTG_DIEPCTL1        0x0920  
#define USB_OTG_DIEPCTL2        0x0940
#define USB_OTG_DIEPCTL3        0x0960
#define USB_OTG_DIEPCTL4        0x0980
#define USB_OTG_DIEPCTL5        0x09A0
#define USB_OTG_DIEPCTL6        0x09C0
#define USB_OTG_DIEPCTL7        0x09E0 
  
#define USB_OTG_DOEPCTL0        0x0B00  
// 下面寄存器共3个，地址0x0B00 + 0x20 * x, x取1..3
#define USB_OTG_DOEPCTL1        0x0B20
#define USB_OTG_DOEPCTL2        0x0B40
#define USB_OTG_DOEPCTL3        0x0B60  
  
// 下面寄存器共8个，地址0x0908 + 0x20 * x, x取0..7
#define USB_OTG_DIEPINT0        0x0908
#define USB_OTG_DIEPINT1        0x0928
#define USB_OTG_DIEPINT2        0x0948
#define USB_OTG_DIEPINT3        0x0968
#define USB_OTG_DIEPINT4        0x0988
#define USB_OTG_DIEPINT5        0x09A8
#define USB_OTG_DIEPINT6        0x09C8
#define USB_OTG_DIEPINT7        0x09E8
  
// 下面寄存器共8个，地址0x0B08 + 0x20 * x, x取0..7
#define USB_OTG_DOEPINT0        0x0B08
#define USB_OTG_DOEPINT1        0x0B28
#define USB_OTG_DOEPINT2        0x0B48
#define USB_OTG_DOEPINT3        0x0B68
#define USB_OTG_DOEPINT4        0x0B88
#define USB_OTG_DOEPINT5        0x0BA8
#define USB_OTG_DOEPINT6        0x0BC8
#define USB_OTG_DOEPINT7        0x0BE8
  
  
#define USB_OTG_DIEPTSIZ0       0x0910
// 下面寄存器共3个，地址0x0910+ 0x20 * x, x取1..3
#define USB_OTG_DIEPTSIZ1       0x0930
#define USB_OTG_DIEPTSIZ2       0x0950  
#define USB_OTG_DIEPTSIZ3       0x0970 
  
// 下面寄存器共6个，地址0x0918 + 0x20 * x, x取0..5
#define USB_OTG_DTXFSTS0        0x0918
#define USB_OTG_DTXFSTS1        0x0938
#define USB_OTG_DTXFSTS2        0x0958
#define USB_OTG_DTXFSTS3        0x0978
#define USB_OTG_DTXFSTS4        0x0998
#define USB_OTG_DTXFSTS5        0x09B8

#define USB_OTG_DOEPTSIZ0       0x0B10  
// 下面寄存器共5个，地址0x0B10+ 0x20 * x, x取1..5
#define USB_OTG_DOEPTSIZ1       0x0B30
#define USB_OTG_DOEPTSIZ2       0x0B50    
#define USB_OTG_DOEPTSIZ3       0x0B70  
#define USB_OTG_DOEPTSIZ4       0x0B90  
#define USB_OTG_DOEPTSIZ5       0x0BB0   
  
// 下面寄存器共6个，地址0x0914+ 0x20 * x, x取0..5
#define USB_OTG_DIEPDMA0        0x0914
#define USB_OTG_DIEPDMA1        0x0934
#define USB_OTG_DIEPDMA2        0x0954  
#define USB_OTG_DIEPDMA3        0x0974  
#define USB_OTG_DIEPDMA4        0x0994   
#define USB_OTG_DIEPDMA5        0x09B4 

// 下面寄存器共6个，地址0x0b14+ 0x20 * x, x取0..5
#define USB_OTG_DOEPDMA0        0x0B14
#define USB_OTG_DOEPDMA1        0x0B34
#define USB_OTG_DOEPDMA2        0x0B54
#define USB_OTG_DOEPDMA3        0x0B74
#define USB_OTG_DOEPDMA4        0x0B94
#define USB_OTG_DOEPDMA5        0x0BB4
  
#define USB_OTG_PCGCCTL         0x0E00 

#define USB_OTG_FIFO_BASE       0x1000U
#define USB_OTG_FIFO_SIZE       0x1000U


#ifdef __cplusplus
    }
#endif      // __cplusplus

#endif // endif of __ARCH_IO_REG_H__
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
