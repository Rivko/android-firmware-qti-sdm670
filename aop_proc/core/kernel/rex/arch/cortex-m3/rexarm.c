/*===========================================================================
                              rexarm.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2016, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Proprietary and Confidential - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rex.h"
#include "rex_target.h"
#include "cortex-m3.h"
#include "customer.h"
#include "CoreVerify.h"
#include <string.h>

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define BAD_REG 0xdecafbad;
#define PENDSV_SET (1 << 28)
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
/* extra stuff we need to track */
struct extra_stackframe
{
  unsigned int r4;
  unsigned int r5;
  unsigned int r6;
  unsigned int r7;
  unsigned int r8;
  unsigned int r9;
  unsigned int r10;
  unsigned int r11;
};

/* stack frame from the NVIC */
struct nvic_stackframe
{
  unsigned int r0;
  unsigned int r1;
  unsigned int r2;
  unsigned int r3;
  unsigned int r12;
  unsigned int lr;
  unsigned int pc;
  unsigned int psr;
};

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
extern void rex_task_preamble(void (*func_ptr)( dword arg ), dword arg);
/* Defined in vectors.c */
extern rex_isr_func_type* usr_irq_handlers;
extern rex_interrupt_func rex_vector_table[REX_TARGET_NUM_SYS_VECTORS + REX_TARGET_NUM_IRQS];
extern void abort_isr(void);

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================


/*===========================================================================
FUNCTION: rex_init_stack

DESCRIPTION: Initializes the TCB stack

RETURN VALUE: The top of the stack
===========================================================================*/
byte *
rex_init_stack(byte *stack_ptr, rex_task_func_type p_task, rex_task_func_param_type p_param)
{
  struct nvic_stackframe *nvic;
  struct extra_stackframe *extra;
  byte *orig_stack = stack_ptr;

  /* stack pointer points to the top full location */
  stack_ptr -= (sizeof(struct nvic_stackframe) - sizeof(unsigned int));
  nvic = (struct nvic_stackframe *)stack_ptr;
  //memset(nvic, 0, sizeof(struct nvic_stackframe));
  nvic->r0 = (unsigned int)p_task;
  nvic->r1 = p_param;
  nvic->r2 = 0x22222222;
  nvic->r3 = 0x33333333;
  nvic->r12 = 0x12121212;
  nvic->lr = 0xfffffffd; /* return to thread mode with PSP */
  nvic->pc = (unsigned int)rex_task_preamble;
  /* start with zero PSR except the thumb bit must be on*/
  nvic->psr = (1<<24);

  stack_ptr -= sizeof(struct extra_stackframe);
  extra = (struct extra_stackframe *)stack_ptr;
  //memset(extra, 0, sizeof(struct extra_stackframe));
  extra->r4 = 0x44444444;
  extra->r5 = 0x55555555;
  extra->r6 = 0x66666666;
  extra->r7 = 0x77777777;
  extra->r8 = 0x88888888;
  extra->r9 = 0x99999999;
  extra->r10 = (unsigned int)orig_stack; //TODO: why?
  extra->r11 = 0xbbbbbbbb;
  
  return stack_ptr;
}

/*===========================================================================
FUNCTION: rex_sched_internal

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rex_sched_internal(void)
{
  SCS.IRQControlState |= PENDSV_SET;
}

/*===========================================================================
FUNCTION: rex_start_task

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rex_start_task(rex_tcb_type *p_tcb)
{
  unsigned int scratch = 0;

  aop_mpu_enable_psp_stack_guard(TRUE, (unsigned)p_tcb->stack_limit - AOP_MPU_STACK_GUARD_SIZE);

  /* set process sp to the task's sp */
  asm("msr PSP, %[sp]" :: [sp] "r"(p_tcb->sp));
  /* switch to psp, privledged mode */
  asm("mrs %[scratch], CONTROL" :: [scratch] "r"(scratch));
  asm("orrs %[scratch], %[scratch], #2" :: [scratch] "r"(scratch));
  asm("msr CONTROL, %[scratch]" :: [scratch] "r"(scratch));
  /* instruction barrier requried after switching stacks */
  asm("isb");
  
  SCS.IRQControlState |= PENDSV_SET;
  unlock_ints ();
  CORE_VERIFY (0);
}

/*===========================================================================
FUNCTION: rex_sv_sched

DESCRIPTION: This the main scheduler function. This function is invoked in
the context of a PENDSV interrupt. 

RETURN VALUE: none
===========================================================================*/
__asm void rex_sv_sched(void)
{
  asm("ldr    r1, =rex_first_task_scheduled");
  asm("ldr    r0, [r1]");
  asm("cmp    r0, #0");
  asm("beq    first_task");
  /*
   * Is task swap required even though rex_int_nest_level == 0?
   * Not required if curr task is same as best task.
   *
   */
  asm("ldr    r0, =rex_curr_task");
  asm("ldr    r0, [r0]");
  asm("ldr    r1, =rex_best_task");
  asm("ldr    r1, [r1]");
  asm("cmp    r0, r1");
  asm("beq    skip");
  /*
   * If a TASKLOCK is in effect, then we do not need to even consider
   * swapping tasks.
   *
   */
  asm("ldr    r2, =rex_sched_allow");
  asm("ldr    r2, [r2]");
  asm("cmp    r2, #0");
  asm("beq    skip");

  /* skip save if we are scheduling the first task and haven't set up psp */
  asm("mrs    r1, psp");
  asm("cmp    r1, #0");
  asm("beq    restore");

  /* Save context of outgoing task, excluding what NVIC already saved. */
  asm("stmfd  r1!, {r4-r11}");

  asm("msr    psp, r1");

  /* save sp of outgoing task to tcb */
  asm("ldr    r0, =rex_curr_task");
  asm("ldr    r2, [r0]");
  /*TODO: next line should be str    r1, [r2, #__cpp(offsetof(rex_tcb_type, sp))] */
  asm("str    r1, [r2, #0]");
  /* TODO: slices++ */

asm("restore:");
  asm("ldr    r0, =rex_curr_task");
  asm("ldr    r1, =rex_best_task");
  asm("ldr    r1, [r1]");
  asm("str    r1, [r0]"); /* curr_task <- best_task */

  asm("mov r2, #0");
  asm("ldr r3, [r1, #28]"); /* intlock_nest_level */
  asm("cmp r3, #0");
  asm("beq set_basepri");
  asm("mov r2, 0x10");
  asm("set_basepri:");
  asm("msr basepri, r2");

  /* load new sp from tcb*/
  /*TODO: next line should be ldr     r1, [r1, #__cpp(offsetof(rex_tcb_type, sp))] */
  asm("ldr    r1, [r1, #0]");

  /* restore registers */
  asm("ldmfd  r1!, {r4-r11}");
  /* restore special processor state */
  asm("msr    psp, r1");

  asm("dsb");
  asm("isb");
  
asm("skip:");
  asm("bx     lr");

asm("first_task:");
  asm("str    r1, [r1, #1]"); /* rex_first_task_scheduled = 1 */
  asm("mrs    r1, psp");
  asm("add    r1, r1, %[extra_stackfram_size]" :: [extra_stackfram_size] "J"(sizeof(struct extra_stackframe)));
  asm("msr    psp, r1");
  asm("b      restore");
  asm(".p2align  4");
}

/*===========================================================================
FUNCTION: rex_arch_handle_irq

DESCRIPTION:
   Function for handling generic interrupts.
   Will invoke interrupt controller provided function and provide IRQ number

RETURN VALUE:
===========================================================================*/
static void rex_arch_handle_irq(void)
{
  int irq;
  rex_isr_func_type usr_irq_handler;

  /* Get IRQ NUM and invoke user handler */
  irq = (__builtin_arm_rsr("IPSR") - 0x10);
  usr_irq_handler = usr_irq_handlers[irq];    

  CORE_VERIFY(usr_irq_handler != NULL);

  usr_irq_handler(irq);
}

/*===========================================================================
FUNCTION: rex_arch_init

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rex_arch_init(void)
{
  int i, num_ints;

  // Disable interrupts globally.
  __disable_irq();

  // Figure out how many interrupts are supported on this platform.
  num_ints = ((SCS.IntCtrlType & 0x1F) + 1) << 5;

  // Disable all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Disable[i] = 0xFFFFFFFF;

  // Clear all interrupts.
  for(i = 0; i < num_ints / 32; ++i)
    SCS.NVIC.Clear[i] = 0xFFFFFFFF;

  // Set all interrupt levels as pre-empting.
  SCS.AIRC = 0x05FA0000;

  // based on the size of vector_table and method of vector_table indexing (i (upto < num_ints) + 16)
  CORE_VERIFY( num_ints <= REX_TARGET_NUM_IRQS );

  // Set all interrupts to lowest priority.
  for(i = 0; i < num_ints / 4; ++i)
    SCS.NVIC.Priority[i] = 0xFFFFFFFF;

  // Configure the exception vector table.
  SCS.ExceptionTableOffset = (unsigned)&rex_vector_table;

  // Enable traps on divides by 0 and double-word stack alignment.
  SCS.ConfigCtrl |= 0x210;

  // Memory/Bus/Usage faults get highest user priority.
  SCS.SystemPriority[0] = 0;

  // Other system exceptions get middling priority.
  SCS.SystemPriority[1] = 0xC0000000; // SVC = 0xC0
  // SCS.SystemPriority[2] = 0x05050080; // SysTick and PendSV = 0x5, Debug = 0x80
  SCS.SystemPriority[2] = 0xFFFF0080; // SysTick and PendSV = 0xFF, Debug = 0x80

  // Enable the usage, bus, and memory management fault handlers.
  SCS.SystemHandlerCtrlAndState = 0x70000;

  // Now that we're all set, go through and re-enable interrupts that have handlers.
  for(i = 0; i < num_ints; ++i)
  {
    // Check for weakly linked handler that doesn't exist yet.
    // Note the +REX_TARGET_NUM_SYS_VECS that skips system handlers.
    rex_vector_table[i + REX_TARGET_NUM_SYS_VECTORS] = rex_arch_handle_irq;
  }

  //TBD - Need to check on Wdog interrrupt number
#if 0
  // Give the watchdog bark IRQ the same priority as a fault.
  interrupt_set_priority(WATCHDOG_IRQ_NUM, 0);
#endif

  // Enable fault handling.
  set_basepri(0);
  // REX_INTLOCK();
  __enable_irq();
}

/*===========================================================================
  FUNCTION: rex_get_cpsr

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
unsigned long rex_get_cpsr(void)
{
  return 0; /*TODO: this is for logging only ? */
}

/**
 * <!-- rex_task_intlock -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_task_intlock (void)
{
  lock_ints ();
  rex_self ()->intlock_nest_level ++;
}

/**
 * <!-- rex_task_intunlock -->
 *
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
void rex_task_intunlock (void)
{
  CORE_VERIFY (rex_self ()->intlock_nest_level > 0);
  if (-- rex_self ()->intlock_nest_level == 0)
  {
    unlock_ints ();
  }
}
