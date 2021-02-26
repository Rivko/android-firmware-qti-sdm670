/*===========================================================================
                              vectors.c

DESCRIPTION:

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <stdint.h>
#include <stdlib.h>
#include "cortex-m3.h"
#include "rex.h"
#include "rex_target.h"
#include "CoreVerify.h"
#include "coredump.h"
#include "aop_interrupt_table.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
extern void rex_entry(void);
extern void rex_sv_sched(void);

rex_isr_func_type* usr_irq_handlers = NULL;

static void nmi_fault(void)
{
  abort ();
}

static void hard_fault(void)
{
  abort ();
}

static void mem_manage_fault(void)
{
  unsigned int cfs = SCS.ConfigurableFaultStatus;
  unsigned int mmar = SCS.MemManageAddress;
  abort ();
}

static void bus_fault(void)
{
  unsigned int cfs = SCS.ConfigurableFaultStatus;
  unsigned int bfar = SCS.BusFaultAddress;
  abort ();
}

static void usage_fault(void)
{
  unsigned int cfs = SCS.ConfigurableFaultStatus;
  abort ();
}

static void svc_call(void)
{
	// TODO
    for(;;){};
}

static void sys_tick(void)
{
	// TODO
    for(;;){};
}

#if defined __ARMCC_VERSION__
static __asm  void abort_isr(void)
{
    IMPORT abort
    b abort             // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
}

#elif defined __clang__
static __asm  void abort_isr(void)
{
    asm("b abort"); // Use b to not overwrite EXEC_RETURN value to make rpm_parse_faults.cmm work
}

#endif /* __clang__ */

extern void Reset_Handler(void);

/* Place table in separate section */
#pragma arm section rodata="exceptions_area"
rex_interrupt_func rex_vector_table[REX_TARGET_NUM_SYS_VECTORS + REX_TARGET_NUM_IRQS] __attribute__((at(0xE0000))) = {
	(rex_interrupt_func)0x0, // not used
	Reset_Handler,  
	nmi_fault,
	hard_fault,
	mem_manage_fault,
	bus_fault,
	usage_fault,
	NULL, /* reserved (7) */
	NULL, /* reserved (8) */
	NULL, /* reserved (9) */
	NULL, /* reserved (10) */
	svc_call,
	NULL, /* reserved (12) */
	NULL, /* reserved (13) */
	rex_sv_sched,
	sys_tick,
	// AOP's NVIC interrupts start here.
	abort_isr,                         //  0 
	abort_isr,                         //  1 
	abort_isr,                         //  2
	abort_isr,                         //  3
	abort_isr,                         //  4
	abort_isr,                         //  5
	abort_isr,                         //  6
	abort_isr,                         //  7
	abort_isr,                         //  8
	abort_isr,                         //  9 
	abort_isr,                         // 10
	abort_isr,                         // 11
	abort_isr,                         // 12 
	abort_isr,                         // 13
	abort_isr,                         // 14
	abort_isr,                         // 15
	abort_isr,                         // 16
	abort_isr,                         // 17
	abort_isr,                         // 18
	abort_isr,                         // 19
	abort_isr,                         // 20 
	abort_isr,                         // 21
	abort_isr,                         // 22 
	abort_isr,                         // 23
	abort_isr,                         // 24
	abort_isr,                         // 25
	abort_isr,                         // 26
	abort_isr,                         // 27
	abort_isr,                         // 28
	abort_isr,                         // 29
	abort_isr,                         // 30
	abort_isr,                         // 31
	abort_isr,                         // 32
	abort_isr,                         // 33
	abort_isr,                         // 34
	abort_isr,                         // 35
	abort_isr,                         // 36
	abort_isr,                         // 37
	abort_isr,                         // 38
	abort_isr,                         // 39
	abort_isr,                         // 40
	abort_isr,                         // 41
	abort_isr,                         // 42
	abort_isr,                         // 43
	abort_isr,                         // 44
	abort_isr,                         // 45
	abort_isr,                         // 46
	abort_isr,                         // 47
	abort_isr,                         // 48
	abort_isr,                         // 49
	abort_isr,                         // 50
	abort_isr,                         // 51
	abort_isr,                         // 52
	abort_isr,                         // 53
	abort_isr,                         // 54
	abort_isr,                         // 55
	abort_isr,                         // 56
	abort_isr,                         // 57
	abort_isr,                         // 58
	abort_isr,                         // 59
	abort_isr,                         // 60
	abort_isr,                         // 61
	abort_isr,                         // 62
	abort_isr,                         // 63
	abort_isr,                         // 64
	abort_isr,                         // 65 
	abort_isr,                         // 66 
	abort_isr,                         // 67 
	abort_isr,                         // 68 
	abort_isr,                         // 69 
	abort_isr,                         // 70 
	abort_isr,                         // 71 
	abort_isr,                         // 72 
	abort_isr,                         // 73 
	abort_isr,                         // 74
	abort_isr,                         // 75 
	abort_isr,                         // 76 
	abort_isr,                         // 77 
	abort_isr,                         // 78 
	abort_isr,                         // 79 
	abort_isr,                         // 80 
	abort_isr,                         // 81 
	abort_isr,                         // 82 
	abort_isr,                         // 83 
	abort_isr,                         // 84 
	abort_isr,                         // 85 
	abort_isr,                         // 86 
	abort_isr,                         // 87
	abort_isr,                         // 88 
	abort_isr,                         // 89 
	abort_isr,                         // 90 
	abort_isr,                         // 91 
	abort_isr,                         // 92 
	abort_isr,                         // 93 
	abort_isr,                         // 94 
	abort_isr,                         // 95 
	abort_isr,                         // 96 
	abort_isr,                         // 97 
	abort_isr,                         // 98 
	abort_isr,                         // 99 
	abort_isr,                         // 100
	abort_isr,                         // 101
	abort_isr,                         // 102
	abort_isr,                         // 103
	abort_isr,                         // 104
	abort_isr,                         // 105
	abort_isr,                         // 106
	abort_isr,                         // 107
	abort_isr,                         // 108
	abort_isr,                         // 109
	abort_isr,                         // 110
	abort_isr,                         // 111
	abort_isr,                         // 112
	abort_isr,                         // 113
	abort_isr,                         // 114
	abort_isr,                         // 115
	abort_isr,                         // 116
	abort_isr,                         // 117
	abort_isr,                         // 118
	abort_isr,                         // 119
	abort_isr,                         // 120
	abort_isr,                         // 121
	abort_isr,                         // 122 
	abort_isr,                         // 123
	abort_isr,                         // 124
	abort_isr,                         // 125
	abort_isr,                         // 126
	abort_isr,                         // 127
};
#pragma arm section


/**
 * <!-- rex_is_in_irq_mode -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
boolean rex_is_in_irq_mode( void )
{
    return (__builtin_arm_rsr("IPSR") & 0xFF) != 0;
}


/**
 * <!-- rex_ints_are_locked -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int rex_ints_are_locked(void)
{
    return __builtin_arm_rsr("BASEPRI") <= REX_TARGET_MIN_PRI;
}

/**
 * <!-- rex_int_lock -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
dword rex_int_lock(void)
{
    dword old = rex_ints_are_locked();

    __builtin_arm_wsr("BASEPRI_MAX", REX_TARGET_MIN_PRI);
    return old;
}

/**
 * <!-- rex_int_free -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
dword rex_int_free(void)
{
    dword old = rex_ints_are_locked();

    /*TODO: what if a realtime isr has disabled interrupts */
    __builtin_arm_wsr("BASEPRI", 0);

    return old;
}

/**
 * <!-- rex_enable_interrupt -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_enable_interrupt(rex_vect_type v)
{
    unsigned idx = v / 32, mask = (1 << (v % 32));

    SCS.NVIC.Enable[idx] = mask;
}

/**
 * <!-- rex_disable_interrupt -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_disable_interrupt(rex_vect_type v)
{
    unsigned idx = v / 32, mask = (1 << (v % 32));

    SCS.NVIC.Disable[idx] = mask;
}

/**
 * <!-- rex_clear_interrupt -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_clear_interrupt(rex_vect_type v)
{
    unsigned idx = v / 32, mask = (1 << (v % 32));
    SCS.NVIC.Clear[idx] = mask;
}

/**
 * <!-- rex_set_interrupt_vector -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_set_interrupt_vector(rex_vect_type v, rex_isr_func_type fnc_ptr)
{
    if(!usr_irq_handlers)
    {
        usr_irq_handlers = malloc(REX_TARGET_NUM_IRQS*sizeof(rex_isr_func_type));
    }
    CORE_VERIFY(usr_irq_handlers != NULL);
    CORE_VERIFY(fnc_ptr != NULL);
    
    usr_irq_handlers[v] = fnc_ptr;
}

/**
 * <!-- rex_interrupt_soft_trigger -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_interrupt_soft_trigger(rex_vect_type v)
{
    CORE_VERIFY(v <128);

    STIR.SoftwareTriggerInterrupt = v;
    
}

/**
 * <!-- rex_configure_interrupt -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_configure_interrupt(rex_vect_type v, interrupt_config config)
{
     CORE_VERIFY(v < NVIC_INT_MAX);
     unsigned ind = v/32;
     volatile unsigned  *AOP_INTERRUPT_POLARITY = (unsigned int *)polarity_registers[ind];
     volatile unsigned  *AOP_INTERRUPT_EDGE = (unsigned int *)edge_registers[ind];

    switch(config)
    {
        case RISING_EDGE:
            *AOP_INTERRUPT_POLARITY |=  (1 << (v % 32));
            *AOP_INTERRUPT_EDGE     |=  (1 << (v % 32));
            break;

        case FALLING_EDGE:
            *AOP_INTERRUPT_POLARITY &= ~(1 << (v % 32));
            *AOP_INTERRUPT_EDGE     |=  (1 << (v % 32));
            break;

        case LEVEL_HIGH:
            *AOP_INTERRUPT_POLARITY |=  (1 << (v % 32));
            *AOP_INTERRUPT_EDGE     &= ~(1 << (v % 32));
            break;

        case LEVEL_LOW:
            *AOP_INTERRUPT_POLARITY &= ~(1 << (v % 32));
            *AOP_INTERRUPT_EDGE     &= ~(1 << (v % 32));
            break;
    }

  // Ensure interrupt configuration takes effect before the next interrupt fires
  asm ("dsb");
  asm ("isb");
}

/**
 * <!-- rex_get_interrupt_polarity -->
 *
 * @brief returns configured polarity for an interrupt
 *
 * @param rex_vect_type interrupt vector
 * @param 
 *
 * @return interrupt_config
 */
interrupt_config rex_get_interrupt_polarity(rex_vect_type v)
{
  unsigned ind = v/32;
  unsigned  *AOP_INTERRUPT_POLARITY = (unsigned int *)polarity_registers[ind];
  unsigned  *AOP_INTERRUPT_EDGE = (unsigned int *)edge_registers[ind];
  bool pol = *AOP_INTERRUPT_POLARITY & (unsigned)(1 << (v%32));
  bool edge = *AOP_INTERRUPT_EDGE & (unsigned)(1 << (v%32));

  if (pol == 1 && edge == 1)
  {
    return RISING_EDGE;
  }
  else if (pol == 0 && edge == 1)
  {
    return FALLING_EDGE;
  }
  else if (pol == 1 && edge == 0)
  {
    return LEVEL_HIGH;
  }
  else if (pol == 0 && edge == 0 )
  {
    return LEVEL_LOW;
  }
  else
  {
    return -1;
  }
}


/**
 * <!-- rex_set_interrupt_priority -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_set_interrupt_priority(rex_vect_type v, unsigned priority)
{
    unsigned priority_word = SCS.NVIC.Priority[v/4];
    unsigned priority_offset = 8 * (v % 4);
    unsigned priority_mask = ~(0xff << priority_offset);

    SCS.NVIC.Priority[v/4] = (priority_word & priority_mask) | ((0xff & priority) << priority_offset);
}

/**
 * <!-- rex_is_interrupt_pending -->
 *
 * @brief Determines if there's a pending (and enabled interrupt) at NVIC
 *
 * @param v - the interrupt number
 *
 * @return true - interrupt pending
 *         false - interrupt not pending
 */
bool rex_is_interrupt_pending (rex_vect_type v)
{
  CORE_VERIFY (v < NVIC_INT_MAX);
  unsigned i = v/32;
  unsigned m = (1 << (v % 32));

  return ((SCS.NVIC.Enable[i] & m) && (SCS.NVIC.Set[i] & m));
}
