#ifndef __VRM_OS_H__
#define __VRM_OS_H__

/*! \file vrm_os.h
*
*  \brief This header file contains internal VRM driver OS specific function definitions.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/utils/os/aop/vrm_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm.h"
#include "vrm_config.h"
#include "rex.h"
#include <string.h>

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

typedef rex_isr_func_type vrm_isr_func;
typedef rex_crit_sect_type vrm_os_lock_type;

//******************************************************************************
// Internal API Functions
//******************************************************************************

/**
 * @brief Initializes critical section lock for given VRM lock
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_lock_init(vrm_os_lock_type *lock_ptr);

/**
 * @brief Enters critical section
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_lock(vrm_os_lock_type *lock_ptr);

/**
 * @brief Leaves critical section
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_unlock(vrm_os_lock_type *lock_ptr);

/**
 * @brief malloc api to allocate memory
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_malloc(uint32 size, void** mem_ptr);

/**
 * @brief memset api to set the given memory with given value
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_memset(void* mem_ptr, uint32 value, uint32 size);

/**
 * @brief memcmp api to compare two memory address
 *
 * @return  returns 1 when memory is different and 0 when same
 */
uint32 vrm_os_memcmp(void* mem_ptr_1, void* mem_ptr_2, uint32 size);

/**
 * @brief api to return time stamp
 *
 * @return  returns current time tick value
 */
uint64 vrm_os_timestamp(void);

/**
 * @brief registers given isr to all the VRM irqs and enables 
 *        the VRM irqs in HW.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_register_isr(vrm_isr_func isr);

/**
 * @brief OS specific ISR to disable VRM interrupts and signal 
 *        the VRM REX task to run the VRM IRQ handler.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_isr(int irq);

/**
 * @brief VRM function call to create a task which handles VRM 
 *        IRQ. This task will be signalled to execute from VRM
 *        ISR when there is a VRM interrupt triggered.
 *
 * @return  VRM_SUCCESS on success, error code on error
 */
vrm_result vrm_os_create_task(void);

#endif
