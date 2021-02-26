#pragma once

/*=============================================================================
  @file sns_osa_thread.h

  @brief
  OS Abstraction layer for Sensors.

  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_osa_thread.h#1 $
  $DateTime: 2018/02/01 03:15:18 $
  $Change: 15367748 $
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "sns_osa_attr.h"
#include "sns_rc.h"

/*=============================================================================
  Constants
  ===========================================================================*/

#define SNS_OSA_THREAD_NAME_MAX_LEN   15  /**< excluding terminating NULL */

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef uint32_t                sns_osa_thread_sigs;  /**< 32-bit signal */
typedef struct sns_osa_thread   sns_osa_thread;     /**< OS dependent thread */
typedef union
{
  char                      __size[__SIZEOF_ATTR_THREAD]; /**< OS dependent */
  long int                  __alignment;
} sns_osa_thread_attr;

typedef void  *sns_osa_thread_func_arg;
typedef void (*sns_osa_thread_func)(sns_osa_thread_func_arg);

/*=============================================================================
  Public Functions
  ===========================================================================*/

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_init
  -----------------------------------------------------------------------------

  @brief Creates a thread attribute structure initialized with defaults

  @param [io] attrib        the attribute structure

  @return
  SNS_RC_SUCCESS            attribute structure initialized
  SNS_RC_OUTSIDE_RANGE      input parameter is invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_init(
  sns_osa_thread_attr*      attrib);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_set_stack
  -----------------------------------------------------------------------------

  @brief Sets the thread stack

  @param [io] attrib        the attribute structure
  @param [i] stack_start    start of the preallocated stack;
                              if NULL, stack will be allocated from heap
  @param [i] stack_size     stack size in bytes

  @return
  SNS_RC_SUCCESS            stack attribute set
  SNS_RC_RESOURCE_UNAVAIL   no memory available for stack allocation
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_set_stack(
  sns_osa_thread_attr      *attrib,
  uintptr_t                 stack_start,
  size_t                    stack_size);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_set_priority
  -----------------------------------------------------------------------------

  @brief Sets the thread priority

  @param [io] attrib        the attribute structure
  @param [i] priority       thread priority

  @return
  SNS_RC_SUCCESS            priority attribute set
  SNS_RC_NOT_SUPPORTED      priority attribute is not supported
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_set_priority(
  sns_osa_thread_attr      *attrib,
  uint8_t                   priority);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_set_name
  -----------------------------------------------------------------------------

  @brief Sets the thread stack

  @param [io] attrib        the attribute structure
  @param [i] name           name

  @return
  SNS_RC_SUCCESS            name attribute set
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_set_name(
  sns_osa_thread_attr      *attrib,
  const char               *name);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_set_memory_partition
  -----------------------------------------------------------------------------

  @brief Specifies the memory type where the thread will be located

  @param [io] attrib        the attribute structure
  @param [i]  mem_type      the memory type

  @return
  SNS_RC_SUCCESS            memory partition attribute set
  SNS_RC_NOT_SUPPORTED      memory partition attribute is not supported
  SNS_RC_OUTSIDE_RANGE      one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_set_memory_partition(
  sns_osa_thread_attr      *attrib,
  sns_osa_mem_type          mem_type);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_attr_set_user_info
  -----------------------------------------------------------------------------

  @brief Assigns user defined information to be stored with a thread

  @param [io] attrib        the attribute structure
  @param [i]  info     The user defined info associated with thread

  @return
  SNS_RC_SUCCESS            User data was set
  SNS_RC_NOT_SUPPORTED      User data attribute is not supported
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_attr_set_user_info(
  sns_osa_thread_attr    *attrib,
  void                   *info);

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_create
  -----------------------------------------------------------------------------

  @brief  Creates a new thread

  @param [i] thread_func      thread main function
  @param [i] thread_func_arg  input parameter to thread_func
  @param [i] attrib           the initialized attribute structure
  @param [o] thread           destination for the newly created thread

  @return
  SNS_RC_SUCCESS              new thread successfully created
  SNS_RC_RESOURCE_UNAVAIL     no memory
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_create(
  sns_osa_thread_func         thread_func,
  sns_osa_thread_func_arg     thread_func_arg,
  const sns_osa_thread_attr  *attrib,
  sns_osa_thread            **thread);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_delete
  -----------------------------------------------------------------------------

  @brief Deletes the given thread

  @param [i] thread           the thread to delete

  @return
  SNS_RC_SUCCESS              given thread deleted
  SNS_RC_OUTSIDE_RANGE        given thread not found
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_delete(
  sns_osa_thread             *thread);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_sigs_wait
  -----------------------------------------------------------------------------

  @brief Waits for the given signals in the current thread

  @note  The received signals are consumed

  @param [i] sigs_mask        the signals for which to wait
  @param [o] sigs_rcvd        destination for signals set when thread woke up

  @return
  SNS_RC_SUCCESS              one or more signals received
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_sigs_wait(
  sns_osa_thread_sigs         sigs_mask,
  sns_osa_thread_sigs        *sigs_rcvd);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_sigs_set
  -----------------------------------------------------------------------------

  @brief Sets the given signals to wake up the given thread

  @param [i] thread           the thread
  @param [i] sigs             the signals to set

  @return
  SNS_RC_SUCCESS              signals set
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_sigs_set(
  sns_osa_thread             *thread,
  sns_osa_thread_sigs         sigs);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_sigs_check
  -----------------------------------------------------------------------------

  @brief Checks the current thread for any pending signals

  @param [i] sigs_mask        the signals for which to check
  @param [i] consume          whether to consume/clear the signals
  @param [o] sigs_rcvd        destination for pending signals

  @return
  SNS_RC_SUCCESS              one or more signals received
  SNS_RC_OUTSIDE_RANGE        one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_thread_sigs_check(
  sns_osa_thread_sigs         sigs_mask,
  bool                        consume,
  sns_osa_thread_sigs        *sigs_rcvd);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_get_priority
  -----------------------------------------------------------------------------

  @brief Returns the current thread's priority

  @param none

  @return
  the current thread's priority
  -----------------------------------------------------------------------------
*/
uint8_t sns_osa_thread_get_priority(void);


/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_set_priority
  -----------------------------------------------------------------------------

  @brief Sets the current thread's priority to the given value

  @param [i] priority         the priority to set

  @return
  the priority before it was changed
  -----------------------------------------------------------------------------
*/
uint8_t sns_osa_thread_set_priority(
  uint8_t                   priority);

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_get_user_info
  -----------------------------------------------------------------------------

  @brief Gets the user defined information that was stored with current thread

  @return
  Pointer to user defined information if user information was set
  NULL otherwise
  -----------------------------------------------------------------------------
*/
void *sns_osa_thread_get_user_info(void);

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_get_thread_id()
  -----------------------------------------------------------------------------

  @brief Gets the thread id

  @return
  Thread id
  -----------------------------------------------------------------------------
*/
int sns_osa_thread_get_thread_id(void);


/*!
  -----------------------------------------------------------------------------
  sns_osa_init
  -----------------------------------------------------------------------------

  @brief Initializes all OSA global state.

  -----------------------------------------------------------------------------
*/
void sns_osa_init(void);

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_active
  -----------------------------------------------------------------------------

  @brief Mark a thread as running

  -----------------------------------------------------------------------------
  */
void sns_osa_thread_active(void);

/*!
  -----------------------------------------------------------------------------
  sns_osa_thread_idle
  -----------------------------------------------------------------------------

  @brief Mark a thread as idle

  -----------------------------------------------------------------------------
  */
void sns_osa_thread_idle(void);
