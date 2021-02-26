#pragma once
/*=============================================================================
  @file sns_thread_manager.h

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_rc.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_library;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Handle a task.
 *
 * @param[i] arg Task to perform, as provided by the service
 *
 * @return
 * SNS_RC_FAILED - A catastrophic failure has occurred, restart the Framework
 * SNS_RC_NOT_AVAILABLE - This task is not ready for processing, try another
 * SNS_RC_SUCCESS
 */
typedef sns_rc (*sns_thread_mgr_task_func)(void *arg);

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Add a new pending task to the thread manager queue.
 *
 * @param[i] priority Whether the pending task is high-priority
 */
void sns_thread_manager_add(struct sns_sensor_library *library,
    sns_thread_mgr_task_func cb_func, void *cb_arg, bool priority);

/**
 * Initialize the Thread Manager, and begin all worker threads.
 */
sns_rc sns_thread_manager_init(void);

/**
 * Deinit the Thread Manager
 */
void sns_thread_manager_deinit(void);

/**
 * Disable worker threads from handling any further tasks.  Active tasks may
 * be continued until their completion.
 *
 * @param[i] status True to disable; false to re-enable
 */
void sns_thread_manager_disable(bool status);

/**
 * Remove pending events from task queue of each thread pool
 * @param[i] stream to be removed
 */

void sns_thread_manager_remove_pending_events(void *cb_arg);
