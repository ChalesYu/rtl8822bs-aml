/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-10-21     Bernard      the first version.
 * 2011-10-27     aozima       update for cortex-M4 FPU.
 * 2011-12-31     aozima       fixed stack align issues.
 * 2012-01-01     aozima       support context switch load/store FPU register.
 * 2012-12-11     lgnq         fixed the coding style.
 * 2012-12-23     aozima       stack addr align to 8byte.
 * 2012-12-29     Bernard      Add exception hook.
 * 2013-06-23     aozima       support lazy stack optimized.
 * 2018-07-24     aozima       enhancement hard fault exception handler.
 * 2019-07-03     yangjie      add __rt_ffs() for armclang.
 */

#include <rthw.h>
#include <rtthread.h>

#if               /* ARMCC */ (  (defined ( __CC_ARM ) && defined ( __TARGET_FPU_VFP ))    \
                  /* Clang */ || (defined ( __CLANG_ARM ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) \
                  /* IAR */   || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ ))        \
                  /* GNU */   || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
#define USE_FPU   1
#else
#define USE_FPU   0
#endif

/* exception and interrupt handler table */
rt_uint32_t rt_interrupt_from_thread;
rt_uint32_t rt_interrupt_to_thread;
rt_uint32_t rt_thread_switch_interrupt_flag;
/* exception hook */
static rt_err_t (*rt_exception_hook)(void *context) = RT_NULL;

struct exception_stack_frame
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;
};

struct stack_frame
{
#if USE_FPU
    rt_uint32_t flag;
#endif /* USE_FPU */

    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

    struct exception_stack_frame exception_stack_frame;
};

struct exception_stack_frame_fpu
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r12;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t psr;

#if USE_FPU
    /* FPU register */
    rt_uint32_t S0;
    rt_uint32_t S1;
    rt_uint32_t S2;
    rt_uint32_t S3;
    rt_uint32_t S4;
    rt_uint32_t S5;
    rt_uint32_t S6;
    rt_uint32_t S7;
    rt_uint32_t S8;
    rt_uint32_t S9;
    rt_uint32_t S10;
    rt_uint32_t S11;
    rt_uint32_t S12;
    rt_uint32_t S13;
    rt_uint32_t S14;
    rt_uint32_t S15;
    rt_uint32_t FPSCR;
    rt_uint32_t NO_NAME;
#endif
};

struct stack_frame_fpu
{
    rt_uint32_t flag;

    /* r4 ~ r11 register */
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t r11;

#if USE_FPU
    /* FPU register s16 ~ s31 */
    rt_uint32_t s16;
    rt_uint32_t s17;
    rt_uint32_t s18;
    rt_uint32_t s19;
    rt_uint32_t s20;
    rt_uint32_t s21;
    rt_uint32_t s22;
    rt_uint32_t s23;
    rt_uint32_t s24;
    rt_uint32_t s25;
    rt_uint32_t s26;
    rt_uint32_t s27;
    rt_uint32_t s28;
    rt_uint32_t s29;
    rt_uint32_t s30;
    rt_uint32_t s31;
#endif

    struct exception_stack_frame_fpu exception_stack_frame;
};

rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    struct stack_frame *stack_frame;
    rt_uint8_t         *stk;
    unsigned long       i;

    stk  = stack_addr + sizeof(rt_uint32_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk, 8);
    stk -= sizeof(struct stack_frame);

    stack_frame = (struct stack_frame *)stk;

    /* init all register */
    for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i ++)
    {
        ((rt_uint32_t *)stack_frame)[i] = 0xdeadbeef;
    }

    stack_frame->exception_stack_frame.r0  = (unsigned long)parameter; /* r0 : argument */
    stack_frame->exception_stack_frame.r1  = 0;                        /* r1 */
    stack_frame->exception_stack_frame.r2  = 0;                        /* r2 */
    stack_frame->exception_stack_frame.r3  = 0;                        /* r3 */
    stack_frame->exception_stack_frame.r12 = 0;                        /* r12 */
    stack_frame->exception_stack_frame.lr  = (unsigned long)texit;     /* lr */
    stack_frame->exception_stack_frame.pc  = (unsigned long)tentry;    /* entry point, pc */
    stack_frame->exception_stack_frame.psr = 0x01000000L;              /* PSR */

#if USE_FPU
    stack_frame->flag = 0;
#endif /* USE_FPU */

    /* return task's current stack address */
    return stk;
}

/**
 * This function set the hook, which is invoked on fault exception handling.
 *
 * @param exception_handle the exception handling hook function.
 */
void rt_hw_exception_install(rt_err_t (*exception_handle)(void *context))
{
    rt_exception_hook = exception_handle;
}

#define SCB_CFSR        (*(volatile const unsigned *)0xE000ED28) /* Configurable Fault Status Register */
#define SCB_HFSR        (*(volatile const unsigned *)0xE000ED2C) /* HardFault Status Register */
#define SCB_MMAR        (*(volatile const unsigned *)0xE000ED34) /* MemManage Fault Address register */
#define SCB_BFAR        (*(volatile const unsigned *)0xE000ED38) /* Bus Fault Address Register */
#define SCB_AIRCR       (*(volatile unsigned long *)0xE000ED0C)  /* Reset control Address Register */
#define SCB_RESET_VALUE 0x05FA0004                               /* Reset value, write to SCB_AIRCR can reset cpu */

#define SCB_CFSR_MFSR   (*(volatile const unsigned char*)0xE000ED28)  /* Memory-management Fault Status Register */
#define SCB_CFSR_BFSR   (*(volatile const unsigned char*)0xE000ED29)  /* Bus Fault Status Register */
#define SCB_CFSR_UFSR   (*(volatile const unsigned short*)0xE000ED2A) /* Usage Fault Status Register */

#ifdef RT_USING_FINSH
static void usage_fault_track(void)
{
    rt_kprintf("usage fault:\n");
    rt_kprintf("SCB_CFSR_UFSR:0x%02X ", SCB_CFSR_UFSR);

    if(SCB_CFSR_UFSR & (1<<0))
    {
        /* [0]:UNDEFINSTR */
        rt_kprintf("UNDEFINSTR ");
    }

    if(SCB_CFSR_UFSR & (1<<1))
    {
        /* [1]:INVSTATE */
        rt_kprintf("INVSTATE ");
    }

    if(SCB_CFSR_UFSR & (1<<2))
    {
        /* [2]:INVPC */
        rt_kprintf("INVPC ");
    }

    if(SCB_CFSR_UFSR & (1<<3))
    {
        /* [3]:NOCP */
        rt_kprintf("NOCP ");
    }

    if(SCB_CFSR_UFSR & (1<<8))
    {
        /* [8]:UNALIGNED */
        rt_kprintf("UNALIGNED ");
    }

    if(SCB_CFSR_UFSR & (1<<9))
    {
        /* [9]:DIVBYZERO */
        rt_kprintf("DIVBYZERO ");
    }

    rt_kprintf("\n");
}

static void bus_fault_track(void)
{
    rt_kprintf("bus fault:\n");
    rt_kprintf("SCB_CFSR_BFSR:0x%02X ", SCB_CFSR_BFSR);

    if(SCB_CFSR_BFSR & (1<<0))
    {
        /* [0]:IBUSERR */
        rt_kprintf("IBUSERR ");
    }

    if(SCB_CFSR_BFSR & (1<<1))
    {
        /* [1]:PRECISERR */
        rt_kprintf("PRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<2))
    {
        /* [2]:IMPRECISERR */
        rt_kprintf("IMPRECISERR ");
    }

    if(SCB_CFSR_BFSR & (1<<3))
    {
        /* [3]:UNSTKERR */
        rt_kprintf("UNSTKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<4))
    {
        /* [4]:STKERR */
        rt_kprintf("STKERR ");
    }

    if(SCB_CFSR_BFSR & (1<<7))
    {
        rt_kprintf("SCB->BFAR:%08X\n", SCB_BFAR);
    }
    else
    {
        rt_kprintf("\n");
    }
}

static void mem_manage_fault_track(void)
{
    rt_kprintf("mem manage fault:\n");
    rt_kprintf("SCB_CFSR_MFSR:0x%02X ", SCB_CFSR_MFSR);

    if(SCB_CFSR_MFSR & (1<<0))
    {
        /* [0]:IACCVIOL */
        rt_kprintf("IACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<1))
    {
        /* [1]:DACCVIOL */
        rt_kprintf("DACCVIOL ");
    }

    if(SCB_CFSR_MFSR & (1<<3))
    {
        /* [3]:MUNSTKERR */
        rt_kprintf("MUNSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<4))
    {
        /* [4]:MSTKERR */
        rt_kprintf("MSTKERR ");
    }

    if(SCB_CFSR_MFSR & (1<<7))
    {
        /* [7]:MMARVALID */
        rt_kprintf("SCB->MMAR:%08X\n", SCB_MMAR);
    }
    else
    {
        rt_kprintf("\n");
    }
}

static void hard_fault_track(void)
{
    if(SCB_HFSR & (1UL<<1))
    {
        /* [1]:VECTBL, Indicates hard fault is caused by failed vector fetch. */
        rt_kprintf("failed vector fetch\n");
    }

    if(SCB_HFSR & (1UL<<30))
    {
        /* [30]:FORCED, Indicates hard fault is taken because of bus fault,
                        memory management fault, or usage fault. */
        if(SCB_CFSR_BFSR)
        {
            bus_fault_track();
        }

        if(SCB_CFSR_MFSR)
        {
            mem_manage_fault_track();
        }

        if(SCB_CFSR_UFSR)
        {
            usage_fault_track();
        }
    }

    if(SCB_HFSR & (1UL<<31))
    {
        /* [31]:DEBUGEVT, Indicates hard fault is triggered by debug event. */
        rt_kprintf("debug event\n");
    }
}
#endif /* RT_USING_FINSH */

struct exception_info
{
    rt_uint32_t exc_return;
    struct stack_frame stack_frame;
};

void rt_hw_hard_fault_exception(struct exception_info *exception_info)
{
    extern long list_thread(void);
    struct exception_stack_frame *exception_stack = &exception_info->stack_frame.exception_stack_frame;
    struct stack_frame *context = &exception_info->stack_frame;

    if (rt_exception_hook != RT_NULL)
    {
        rt_err_t result;

        result = rt_exception_hook(exception_stack);
        if (result == RT_EOK) return;
    }

    rt_kprintf("psr: 0x%08x\n", context->exception_stack_frame.psr);

    rt_kprintf("r00: 0x%08x\n", context->exception_stack_frame.r0);
    rt_kprintf("r01: 0x%08x\n", context->exception_stack_frame.r1);
    rt_kprintf("r02: 0x%08x\n", context->exception_stack_frame.r2);
    rt_kprintf("r03: 0x%08x\n", context->exception_stack_frame.r3);
    rt_kprintf("r04: 0x%08x\n", context->r4);
    rt_kprintf("r05: 0x%08x\n", context->r5);
    rt_kprintf("r06: 0x%08x\n", context->r6);
    rt_kprintf("r07: 0x%08x\n", context->r7);
    rt_kprintf("r08: 0x%08x\n", context->r8);
    rt_kprintf("r09: 0x%08x\n", context->r9);
    rt_kprintf("r10: 0x%08x\n", context->r10);
    rt_kprintf("r11: 0x%08x\n", context->r11);
    rt_kprintf("r12: 0x%08x\n", context->exception_stack_frame.r12);
    rt_kprintf(" lr: 0x%08x\n", context->exception_stack_frame.lr);
    rt_kprintf(" pc: 0x%08x\n", context->exception_stack_frame.pc);

    if (exception_info->exc_return & (1 << 2))
    {
        rt_kprintf("hard fault on thread: %s\r\n\r\n", rt_thread_self()->name);

#ifdef RT_USING_FINSH
        list_thread();
#endif
    }
    else
    {
        rt_kprintf("hard fault on handler\r\n\r\n");
    }

    if ( (exception_info->exc_return & 0x10) == 0)
    {
        rt_kprintf("FPU active!\r\n");
    }

#ifdef RT_USING_FINSH
    hard_fault_track();
#endif /* RT_USING_FINSH */

    while (1);
}

/**
 * shutdown CPU
 */
void rt_hw_cpu_shutdown(void)
{
    rt_kprintf("shutdown...\n");

    RT_ASSERT(0);
}

/**
 * reset CPU
 */
RT_WEAK void rt_hw_cpu_reset(void)
{
    SCB_AIRCR = SCB_RESET_VALUE;
}

#define SCB_CCR        (*(volatile unsigned long *)0xE000ED14)  /* Configuration Control Register */
#define SCB_CCSIDR     (*(volatile unsigned long *)0xE000ED80)  /* Cache Size ID Register */
#define SCB_CSSELR     (*(volatile unsigned long *)0xE000ED84)  /* Cache Size Selection Register */

#define SCB_ICIALLU    (*(volatile unsigned long *)0xE000EF50)  /* I-Cache Invalidate All to PoU */
#define SCB_ICIMVAU    (*(volatile unsigned long *)0xE000EF58)  /* I-Cache invalidate by address to the PoUa */
#define SCB_DCIMVAC    (*(volatile unsigned long *)0xE000EF5C)  /* D-Cache Invalidate by MVA to PoC */
#define SCB_DCISW      (*(volatile unsigned long *)0xE000EF60)  /* D-Cache Invalidate by Set-way */
#define SCB_DCCMVAU    (*(volatile unsigned long *)0xE000EF64)  /* Data cache by address to PoU */
#define SCB_DCCMVAC    (*(volatile unsigned long *)0xE000EF68)  /* D-Cache Clean by MVA to PoC */
#define SCB_DCCSW      (*(volatile unsigned long *)0xE000EF6C)  /* Data cache clean by set/way */
#define SCB_DCCIMVAC   (*(volatile unsigned long *)0xE000EF70)  /* D-Cache Clean and Invalidate by MVA to PoC */
#define SCB_DCCISW     (*(volatile unsigned long *)0xE000EF74)  /* D-Cache Clean and Invalidate by Set-way */

#define SCB_CCR_L1_INSTRUCTION_CACHE_BIT   ((rt_uint32_t)(1UL<<17))/*  L1 instruction cache control bit */
#define SCB_CCR_L1_DATA_CACHE_BIT          ((rt_uint32_t)(1UL<<16))/*  L1 Daata cache control bit */
#define SCB_CCSIDR_NUMSETS_POS          13U                      /*  SCB CCSIDR: NumSets Position */
#define SCB_CCSIDR_NUMSETS_MSK          (0x7FFFUL << SCB_CCSIDR_NUMSETS_POS) /* SCB CCSIDR: NumSets Mask */
#define SCB_CCSIDR_ASSOCIATIVITY_POS   3U                                   /* SCB CCSIDR: Associativity Position */
#define SCB_CCSIDR_ASSOCIATIVITY_MSK   (0x3FFUL << SCB_CCSIDR_ASSOCIATIVITY_POS)/* SCB CCSIDR: Associativity Mask */
#define SCB_DCISW_WAY_POS               30U                                     /* SCB DCISW: Way Position */
#define SCB_DCISW_WAY_MSK               (3UL << SCB_DCISW_WAY_POS)              /* SCB DCISW: Way Mask */
#define SCB_DCISW_SET_POS               5U                                      /* SCB DCISW: Set Position */
#define SCB_DCISW_SET_MSK               (0x1FFUL << SCB_DCISW_SET_POS)          /* SCB DCISW: Set Mask */
#define SCB_DCCSW_WAY_POS               30U                                     /* SCB DCCSW: Way Position */
#define SCB_DCCSW_WAY_MSK               (3UL << SCB_DCCSW_WAY_POS)              /* SCB DCCSW: Way Mask */
#define SCB_DCCSW_SET_POS               5U                                      /* SCB DCCSW: Set Position */
#define SCB_DCCSW_SET_MSK               (0x1FFUL << SCB_DCCSW_SET_POS)          /* SCB DCCSW: Set Mask */
#define SCB_DCCISW_WAY_POS              30U                                     /* SCB DCCISW: Way Position */
#define SCB_DCCISW_WAY_MSK              (3UL << SCB_DCCISW_WAY_POS)             /* SCB DCCISW: Way Mask */
#define SCB_DCCISW_SET_POS              5U                                      /* SCB DCCISW: Set Position */
#define SCB_DCCISW_SET_MSK              (0x1FFUL << SCB_DCCISW_SET_POS)         /* SCB DCCISW: Set Mask */

/* The L1-caches on all Cortex?-M7s are divided into lines of 32 bytes. */
#define L1CACHE_LINESIZE_BYTE          (32U)

/* Cache Size ID Register Macros */
#define CCSIDR_WAYS(x)         (((x) & SCB_CCSIDR_ASSOCIATIVITY_MSK) >> SCB_CCSIDR_ASSOCIATIVITY_POS)
#define CCSIDR_SETS(x)         (((x) & SCB_CCSIDR_NUMSETS_MSK      ) >> SCB_CCSIDR_NUMSETS_POS      )

#if defined(__CC_ARM) || defined(__CLANG_ARM)
#define __DMB()        __asm("dsm 0xF")
#define __DSB()        __asm("dsb 0xF")
#define __ISB()        __asm("isb 0xF")
#elif defined(__IAR_SYSTEMS_ICC__)
#include <iccarm_builtin.h>
#define __DMB()         __iar_builtin_DMB()   //__asm("DMB")
#define __DSB()         __iar_builtin_DSB()   //__asm("DSB")
#define __ISB()         __iar_builtin_ISB()   //__asm("ISB")
#elif defined(__GNUC__)
__STATIC_FORCEINLINE void __DMB(void)
{
  __ASM volatile ("dmb 0xF":::"memory");
}
__STATIC_FORCEINLINE void __DSB(void)
{
  __ASM volatile ("dsb 0xF":::"memory");
}
__STATIC_FORCEINLINE void __ISB(void)
{
  __ASM volatile ("isb 0xF":::"memory");
}
#endif

/**
 * enable icache
 */
RT_WEAK void rt_hw_cpu_icache_enable(void)
{
  __DSB();
  __ISB();
  SCB_ICIALLU = 0;
  __DSB();
  __ISB();
  SCB_CCR |= SCB_CCR_L1_INSTRUCTION_CACHE_BIT;
  __DSB();
  __ISB();
}

/**
 * disable icache
 */
RT_WEAK void rt_hw_cpu_icache_disable(void)
{
  __DSB();
  __ISB();
  SCB_CCR &= ~((rt_uint32_t)SCB_CCR_L1_INSTRUCTION_CACHE_BIT);
  SCB_ICIALLU = 0;
  __DSB();
  __ISB();
}

/**
 * get icache status
 */
RT_WEAK rt_base_t rt_hw_cpu_icache_status(void)
{
  return 0;
}

/**
 * set icache ops
 */
RT_WEAK void rt_hw_cpu_icache_ops(int ops, void *addr, int size)
{
  if (ops & RT_HW_CACHE_INVALIDATE) {
    if(addr == RT_NULL) {
      __DSB();
      __ISB();
      SCB_ICIALLU = 0;
      __DSB();
      __ISB();
    } else {
      rt_uint32_t address = ((rt_uint32_t)addr) & ((rt_uint32_t)(~(L1CACHE_LINESIZE_BYTE - 1)));
      rt_int32_t size_byte = size + (rt_uint32_t)addr - address;
      rt_uint32_t linesize = L1CACHE_LINESIZE_BYTE;
      __DSB();
      while (size_byte > 0)
      {
        SCB_ICIMVAU = address;
        address += linesize;
        size_byte -= linesize;
      }
      __DSB();
      __ISB();
    }
  }
}

/**
 * enable dcache
 */
RT_WEAK void rt_hw_cpu_dcache_enable(void)
{
  rt_uint32_t ccsidr;
  rt_uint32_t sets;
  rt_uint32_t ways;
  
  SCB_CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
  __DSB();
  
  ccsidr = SCB_CCSIDR;
  
  /* invalidate D-Cache */
  sets = (uint32_t)(CCSIDR_SETS(ccsidr));
  do {
    ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
    do {
      SCB_DCISW = (((sets << SCB_DCISW_SET_POS) & SCB_DCISW_SET_MSK) |
                    ((ways << SCB_DCISW_WAY_POS) & SCB_DCISW_WAY_MSK) );
    } while (ways-- != 0U);
  } while(sets-- != 0U);
  __DSB();
  
  SCB_CCR |=  SCB_CCR_L1_DATA_CACHE_BIT;  /* enable D-Cache */
  
  __DSB();
  __ISB();
}

/**
 * disable dcache
 */
RT_WEAK void rt_hw_cpu_dcache_disable(void)
{
  register rt_uint32_t ccsidr;
  register rt_uint32_t sets;
  register rt_uint32_t ways;
  
  SCB_CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
  __DSB();
  
  SCB_CCR &= ~SCB_CCR_L1_DATA_CACHE_BIT;  /* disable D-Cache */
  __DSB();
  
  ccsidr = SCB_CCSIDR;
  
  /* clean & invalidate D-Cache */
  sets = (rt_uint32_t)(CCSIDR_SETS(ccsidr));
  do {
    ways = (rt_uint32_t)(CCSIDR_WAYS(ccsidr));
    do {
      SCB_DCCISW = (((sets << SCB_DCCISW_SET_POS) & SCB_DCCISW_SET_MSK) |
                     ((ways << SCB_DCCISW_WAY_POS) & SCB_DCCISW_WAY_MSK) );
    } while (ways-- != 0U);
  } while(sets-- != 0U);
  
  __DSB();
  __ISB();
}

/**
 * get dcache status
 */
RT_WEAK rt_base_t rt_hw_cpu_dcache_status(void)
{
  return 0;
}

/**
 * set dcache ops
 */
RT_WEAK void rt_hw_cpu_dcache_ops(int ops, void *addr, int size)
{
  rt_uint32_t op_addr =  ((rt_uint32_t)addr) & ((rt_uint32_t)(~(L1CACHE_LINESIZE_BYTE - 1)));
  rt_int32_t op_size = size + (rt_uint32_t)addr - op_addr;
  rt_int32_t linesize = L1CACHE_LINESIZE_BYTE;                /* in Cortex-M7 size of cache line is fixed to 8 words (32 bytes) */
  
  if((ops & RT_HW_CACHE_FLUSH) && (ops & RT_HW_CACHE_INVALIDATE))
  {
    if(addr == RT_NULL) {
      rt_uint32_t ccsidr;
      rt_uint32_t sets;
      rt_uint32_t ways;
      
      SCB_CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
      __DSB();
      
      ccsidr = SCB_CCSIDR;
      
      /* clean & invalidate D-Cache */
      sets = (rt_uint32_t)(CCSIDR_SETS(ccsidr));
      do {
        ways = (rt_uint32_t)(CCSIDR_WAYS(ccsidr));
        do {
          SCB_DCCISW = (((sets << SCB_DCCISW_SET_POS) & SCB_DCCISW_SET_MSK) |
                         ((ways << SCB_DCCISW_WAY_POS) & SCB_DCCISW_WAY_MSK) );
        } while (ways-- != 0U);
      } while(sets-- != 0U);
      
      __DSB();
      __ISB();
    } else { 
      __DSB();
      
      while (op_size > 0) {
        SCB_DCCIMVAC = op_addr;
        op_addr += (rt_uint32_t)linesize;
        op_size -=           linesize;
      }
      
      __DSB();
      __ISB();
    }
  }
  else if (ops & RT_HW_CACHE_FLUSH)
  {
    if(addr == RT_NULL) {
      rt_uint32_t ccsidr;
      rt_uint32_t sets;
      rt_uint32_t ways;
      
      SCB_CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
      __DSB();
      
      ccsidr = SCB_CCSIDR;
      
      /* clean D-Cache */
      sets = (uint32_t)(CCSIDR_SETS(ccsidr));
      do {
        ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
        do {
          SCB_DCCSW = (((sets << SCB_DCCSW_SET_POS) & SCB_DCCSW_SET_MSK) |
                         ((ways << SCB_DCCSW_WAY_POS) & SCB_DCCSW_WAY_MSK) );
        } while (ways-- != 0U);
      } while(sets-- != 0U);
      
      __DSB();
      __ISB();
    } else {
      __DSB();
      
      while (op_size > 0) {
        SCB_DCCMVAC = op_addr;
        op_addr += (uint32_t)linesize;
        op_size -=           linesize;
      }
      
      __DSB();
      __ISB();
    }
  }
  else if (ops & RT_HW_CACHE_INVALIDATE)
  {
    if(addr == RT_NULL) {
      rt_uint32_t ccsidr;
      rt_uint32_t sets;
      rt_uint32_t ways;
      
      SCB_CSSELR = 0U; /*(0U << 1U) | 0U;*/  /* Level 1 data cache */
      __DSB();
      
      ccsidr = SCB_CCSIDR;
      
      /* clean D-Cache */
      sets = (uint32_t)(CCSIDR_SETS(ccsidr));
      do {
        ways = (uint32_t)(CCSIDR_WAYS(ccsidr));
        do {
          SCB_DCISW = (((sets << SCB_DCISW_SET_POS) & SCB_DCISW_SET_MSK) |
                        ((ways << SCB_DCISW_WAY_POS) & SCB_DCISW_WAY_MSK) );
        } while (ways-- != 0U);
      } while(sets-- != 0U);
      
      __DSB();
      __ISB();
    } else {
      __DSB();
      
      while (op_size > 0) {
        SCB_DCIMVAC = op_addr;
        op_addr += (uint32_t)linesize;
        op_size -=           linesize;
      }
      
      __DSB();
      __ISB();
    }
  }
}

#ifdef RT_USING_CPU_FFS
/**
 * This function finds the first bit set (beginning with the least significant bit)
 * in value and return the index of that bit.
 *
 * Bits are numbered starting at 1 (the least significant bit).  A return value of
 * zero from any of these functions means that the argument was zero.
 *
 * @return return the index of the first bit set. If value is 0, then this function
 * shall return 0.
 */
#if defined(__CC_ARM)
__asm int __rt_ffs(int value)
{
    CMP     r0, #0x00
    BEQ     exit

    RBIT    r0, r0
    CLZ     r0, r0
    ADDS    r0, r0, #0x01

exit
    BX      lr
}
#elif defined(__CLANG_ARM)
int __rt_ffs(int value)
{
    __asm volatile(
        "CMP     r0, #0x00            \n"
        "BEQ     1f                   \n"

        "RBIT    r0, r0               \n"
        "CLZ     r0, r0               \n"
        "ADDS    r0, r0, #0x01        \n"

        "1:                           \n"

        : "=r"(value)
        : "r"(value)
    );
    return value;
}
#elif defined(__IAR_SYSTEMS_ICC__)
int __rt_ffs(int value)
{
    if (value == 0) return value;

    asm("RBIT %0, %1" : "=r"(value) : "r"(value));
    asm("CLZ  %0, %1" : "=r"(value) : "r"(value));
    asm("ADDS %0, %1, #0x01" : "=r"(value) : "r"(value));

    return value;
}
#elif defined(__GNUC__)
int __rt_ffs(int value)
{
    return __builtin_ffs(value);
}
#endif

#endif
