/*! \file vrm_os.c
*
*  \brief Implementation file for VRM OS specific functions.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/utils/os/aop/vrm_os.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_os.h"
#include "vrm_driver.h"
#include "time_service.h"
#include "dog.h"
#include <stdlib.h>

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_TASK_SIGNAL_MASK 0x1
#define VRM_TASK_PRIORITY 110
#define VRM_TASK_STACK_SIZE 1024

typedef struct
{
   vrm_irq_num last_irq;
}vrm_os_info_type;

//******************************************************************************
// Global Data
//******************************************************************************
rex_tcb_type vrm_task_tcb;
REX_DECLARE_STACK(vrm_task_stack, VRM_TASK_STACK_SIZE);

static vrm_os_info_type vrm_os_info = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void vrm_os_task(dword param)
{
  vrm_irq_num irq_index;

  while (1)
  {
    // wait for signal
    rex_wait (VRM_TASK_SIGNAL_MASK);
    rex_clr_sigs (rex_self(), VRM_TASK_SIGNAL_MASK);

    vrm_irq_handler(vrm_os_info.last_irq);

    // enable all the VRM interrupts back after handling them
    for (irq_index = VRM_IRQ_FIRST; irq_index < VRM_IRQ_LAST; irq_index++)
    {
       rex_enable_interrupt((rex_vect_type)irq_index);
    }
  }
}

//******************************************************************************
// API Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_os_lock_init(vrm_os_lock_type *lock_ptr)
{
    vrm_result result = VRM_SUCCESS;

    rex_init_crit_sect(lock_ptr);

    return result;
}

vrm_result vrm_os_lock(vrm_os_lock_type *lock_ptr)
{
    vrm_result result = VRM_SUCCESS;

    rex_enter_crit_sect(lock_ptr);

    return result;
}

vrm_result vrm_os_unlock(vrm_os_lock_type *lock_ptr)
{
    vrm_result result = VRM_SUCCESS;

    rex_leave_crit_sect(lock_ptr);

    return result;
}

vrm_result vrm_os_malloc(uint32 size, void** mem_ptr)
{
    vrm_result result = VRM_SUCCESS;

    *mem_ptr = malloc(size);

    if (NULL == *mem_ptr)
    {
       result = VRM_NULL_PTR_ERROR;
    }
    else
    {
       result = vrm_os_memset(*mem_ptr, 0, size);
    }

    return result;
}

vrm_result vrm_os_memset(void* mem_ptr, uint32 value, uint32 size)
{
    vrm_result result = VRM_SUCCESS;

    if (NULL == mem_ptr)
    {
       result = VRM_NULL_PTR_ERROR;
    }
    else
    {
       memset(mem_ptr, value, size);
    }

    return result;
}

uint32 vrm_os_memcmp(void* mem_ptr_1, void* mem_ptr_2, uint32 size)
{
    return memcmp(mem_ptr_1, mem_ptr_2, size);
}

uint64 vrm_os_timestamp(void)
{
    return time_service_now();
}

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_os_register_isr(vrm_isr_func isr)
{
   vrm_result result = VRM_SUCCESS;
   vrm_irq_num irq_index;

   if(isr == NULL) 
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      // register all VRM irqs to the same ISR
      for (irq_index = VRM_IRQ_FIRST; irq_index < VRM_IRQ_LAST; irq_index++)
      {
         rex_set_interrupt_vector((rex_vect_type)irq_index, isr);

         // configure and enable the VRM IRQs in HW
         rex_configure_interrupt((rex_vect_type)irq_index, LEVEL_HIGH);
         rex_clear_interrupt((rex_vect_type)irq_index);
         rex_enable_interrupt((rex_vect_type)irq_index);
      }

      //result = vrm_os_create_task();
   }

   return result;
}

vrm_result vrm_os_isr(int irq)
{
   vrm_result result = VRM_SUCCESS;
   vrm_irq_num irq_index;

   // disable all the VRM interrupts till this interrupt is handled
   for (irq_index = VRM_IRQ_FIRST; irq_index < VRM_IRQ_LAST; irq_index++)
   {
      rex_disable_interrupt((rex_vect_type)irq_index);
   }

   vrm_os_info.last_irq = (vrm_irq_num)irq;

   // signal vrm task to start handling interrupt
   rex_set_sigs (&vrm_task_tcb, VRM_TASK_SIGNAL_MASK);

   rex_clear_interrupt((rex_vect_type)irq);

   return result;
}

vrm_result vrm_os_create_task()
{
   vrm_result result = VRM_SUCCESS;

   static boolean task_created_flag = FALSE;

   if (task_created_flag == FALSE)
   {
      rex_stack_size_type stack_size = REX_STACK_SIZE(vrm_task_stack);

      rex_def_task(&vrm_task_tcb, vrm_task_stack, stack_size, VRM_TASK_PRIORITY, vrm_os_task, 0);

      task_created_flag = TRUE;
   }

   return result;
}
