#pragma once

/*=============================================================================
  @file sns_osa_lock.h

  @brief
  OS Abstraction layer for Sensors.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/internal/sns_osa_lock.h#3 $
  $DateTime: 2018/06/29 12:44:18 $
  $Change: 16539122 $
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_osa_attr.h"
#include "sns_rc.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/
typedef union
{
  char                    __size[__SIZEOF_LOCK];
  long int                __alignment;
} sns_osa_lock  __attribute__((aligned(8))); /**< OS dependent locking mechanism */

typedef union
{
  char                      __size[__SIZEOF_ATTR_LOCK]; /**< OS dependent */
  long int                  __alignment;
} sns_osa_lock_attr;

/*=============================================================================
  Public Functions
  ===========================================================================*/

/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_attr_init
  -----------------------------------------------------------------------------

  @brief Initializes the given lock attribute structure with defaults

  @param [io] attrib        the attribute structure

  @return
  SNS_RC_SUCCESS            attribute structure initialized
  SNS_RC_INVALID_VALUE      input parameter is invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_attr_init(
  sns_osa_lock_attr             *attrib);


/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_attr_set_memory_partition
  -----------------------------------------------------------------------------

  @brief Specifies the memory type where the lock will be located

  @param [io] attrib        the attribute structure
  @param [i]  mem_type      the memory type

  @return
  SNS_RC_SUCCESS            memory partition attribute set
  SNS_RC_NOT_SUPPORTED      memory partition attribute is not supported
  SNS_RC_INVALID_VALUE      one or more input parameters are invalid
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_attr_set_memory_partition(
  sns_osa_lock_attr      *attrib,
  sns_osa_mem_type       mem_type);


/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_create
  -----------------------------------------------------------------------------

  @brief  Allocates memory for a new lock and initializes it

  @param [i] attrib           the initialized attribute structure
  @param [o] lock             destination for the newly created lock

  @return
  SNS_RC_SUCCESS              new lock successfully created
  SNS_RC_NOT_AVAILABLE        no memory
  SNS_RC_INVALID_VALUE        one or more input pointers are NULL
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_create(
  const sns_osa_lock_attr  *attrib,
  sns_osa_lock             **lock);

/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_init
  -----------------------------------------------------------------------------

  @brief  Initializes a new lock only, does not allocate memory for the lock.

  @param [i] attrib           the initialized attribute structure
  @param [o] lock             destination for the newly created lock

  @return
  SNS_RC_SUCCESS              new lock successfully created
  SNS_RC_NOT_AVAILABLE        no memory
  SNS_RC_INVALID_VALUE        one or more input pointers are NULL
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_init(
  const sns_osa_lock_attr  *attrib,
  sns_osa_lock             *lock);


/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_delete
  -----------------------------------------------------------------------------

  @brief  Deinit the given lock and free the memory

  @param [i] lock             the lock

  @return
  SNS_RC_SUCCESS              the given lock deleted
  SNS_RC_INVALID_VALUE        not a valid lock

  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_delete(
  sns_osa_lock             *lock);

/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_deinit
  -----------------------------------------------------------------------------

  @brief  Deinit the given lock

  @param [i] lock             the lock

  @return
  SNS_RC_SUCCESS              the given lock deleted
  SNS_RC_INVALID_VALUE        not a valid lock

  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_deinit(
  sns_osa_lock             *lock);

/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_acquire
  -----------------------------------------------------------------------------

  @brief  Acquires the given lock

  @param [i] lock             the lock

  @return
  SNS_RC_SUCCESS              the given lock acquired
  SNS_RC_INVALID_VALUE        not a valid lock

  @note
  Blocks until lock is acquired
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_acquire(
  sns_osa_lock             *lock);


/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_try_acquire
  -----------------------------------------------------------------------------

  @brief  Acquires the given lock

  @param [i] lock             the lock

  @return
  SNS_RC_SUCCESS              the given lock acquired
  SNS_RC_NOT_AVAILABLE        unable to acquire lock

  @note
  Returns immediately if lock cannot be acquired
  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_try_acquire(
  sns_osa_lock             *lock);


/*!
  -----------------------------------------------------------------------------
  sns_osa_lock_release
  -----------------------------------------------------------------------------

  @brief  Release the given lock

  @param [i] lock             the lock

  @return
  SNS_RC_SUCCESS              the given lock released
  SNS_RC_INVALID_VALUE        not a valid lock

  -----------------------------------------------------------------------------
*/
sns_rc sns_osa_lock_release(
  sns_osa_lock             *lock);
