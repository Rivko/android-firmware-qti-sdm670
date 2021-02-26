#ifndef ICB_NOTIFY_H
#define ICB_NOTIFY_H
/*=============================================================================

FILE:      icb_notify.h

DESCRIPTION: This defines the NPA ICB Arbiter Node interface

  Copyright (c) 2017 Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2009 - 2010, Apple Inc. All rights reserved.
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
-------------------------------------------------------------------------------

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdrivers/icb_notify.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  $Change: 14402555 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 when         who     what, where, why
 ----------   ---     -----------------------------------------------------------
 2017/02/01   op     Initial version

=============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*============================================================================
                          DEFINITIONS
============================================================================*/
/* The number of simultaneously monitored state values for a single rule. */
#define ICB_NOTIFY_MAX_STATES 2

/*============================================================================
                          DATA TYPES
============================================================================*/
typedef struct icb_notify_type * icb_notify_handle_type;

typedef enum
{
  ICB_NOTIFY_MASTER_NODE,
  ICB_NOTIFY_SLAVE_NODE,
  ICB_NOTIFY_NUM_NODE_TYPES,
  ICB_NOTIFY_NODE_TYPE_PLACEHOLDER = 0x7FFFFFFF
} icb_node_type;

typedef enum
{
  ICB_NOTIFY_AB,
  ICB_NOTIFY_IB,
  ICB_NOTIFY_TOTAL,
  ICB_NOTIFY_IB_AB, /**< IB and AB simultaneously. IB in threshold[0], AB in threshold[1]. */
  ICB_NOTIFY_NUM_THRESHOLD_TYPES,
  ICB_NOTIFY_THRESHOLD_TYPE_PLACEHOLDER = 0x7FFFFFFF
} icb_notify_threshold_type;

typedef enum
{
  ICB_NOTIFY_CROSSING_DEFAULT, /**< Up through high threshold, or down through low threshold. */
  ICB_NOTIFY_CROSSING_BOTH,    /**< Both directions through each threshold. */
  ICB_NOTIFY_CROSSING_NUM_TYPES,
  ICB_NOTIFY_CROSSING_TYPE_PLACEHOLDER = 0x7FFFFFFF
} icb_notify_crossing_type;

typedef struct
{
  icb_node_type             node_type;
  uint32_t                  id;
  icb_notify_threshold_type threshold_type;
  icb_notify_crossing_type  crossing_type;
  uint64_t                  high_threshold[ICB_NOTIFY_MAX_STATES];
  uint64_t                  low_threshold[ICB_NOTIFY_MAX_STATES];
} icb_notify_definition_type;

typedef struct
{
  bool     high_threshold;
  bool     low_threshold;
  uint64_t ib;
  uint64_t ab;
  uint64_t total;
} icb_notify_status_type;

/* ICB notification callback type.
 * IMPORTANT:
 *   The status pointer is ONLY valid during the callback.
 *   Please copy structure at the pointer. */
typedef void (*icb_notify_cb_type)(icb_notify_handle_type handle, icb_notify_status_type *status, void *cb_data);

/*============================================================================
                          DOCUMENTATION
==============================================================================
Expected usage and callflow:
* Registering for a notification:
** Prepare and initialize callback function variables
** Call icb_notify_register() with the desired notification type
** Handle initial status callback issued by icb_notify_register()

When a callback fires:
* Save the handle and COPY the status information
* Return. Do NOT process your state machine/threshold events in the callback context.
* Process your state machine/threshold events from a separate thread signalled
  from callback context.

Processing a callback event:
* Fetch current node status with icb_notify_get_notification_status()
* Take whatever action required based on the event.
* Update thresholds as necessary with icb_notify_update()
* If not updating, clear threshold crossing flags with
  icb_notify_reset_notification_status() if desired.

Threshold crossings:
* The high threshold value itself is considered part of the region above
  the high threshold.
** This means that a node transitioning from below the threshold to the
   threshold exactly will trigger a callback with the high threshold event set.
** If a callback desired on a crossing in both directions, a callback will only
   be triggered if the state is lower than the threshold value when transitioning
   from above the high threshold.
* Similarly for the low threshold, it's value is considered part of the region
  below the low threshold.
** This means that a node transitioning from above the threshold to the
   threshold exactly will trigger a callback with the low threshold event set.
** If a callback desired on a crossing in both directions, a callback will only
   be triggered if the state is higher than the threshold value when transitioning
   from below the low threshold.
============================================================================*/

/*============================================================================
                          FUNCTIONS
============================================================================*/

/**=============================================================================
   @brief Creates a notification handle for the specified node and threshold conditions.

   @param definition:    A pointer to notification specification
   @param callback:      A pointer to notification callback to use
   @param callback_data: A pointer to notification callback data to send

   @return null if an error occurred, non-null otherwise.

   @remark IMPORTANT: This call will trigger a callback from the same context as
           the caller. It will happen before the function returns.
==============================================================================*/
icb_notify_handle_type icb_notify_register
(
  icb_notify_definition_type *definition,
  icb_notify_cb_type          callback,
  void                       *callback_data
);

/**=============================================================================
   @brief Update an existing notification handle with a new rule definition

   @param handle:     The notification handle to deregister
   @param definition: A pointer to notification specification

   @return true if successful, false otherwise

   @remark IMPORTANT: This call will trigger a callback from the same context as
           the caller. It will happen before the function returns.
==============================================================================*/
bool icb_notify_update
(
  icb_notify_handle_type      handle,
  icb_notify_definition_type *definition
);

/**=============================================================================
   @brief Deregister the given notification handle

   Deregister the passed handle, removing any future notifications and freeing
   any memory associated with the handle. The handle cannot be used again
   afterward.

   @param handle: The notification handle to deregister

   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_deregister
(
  icb_notify_handle_type handle
);

/**=============================================================================
   @brief Get the notification status for a given notification handle

   @param handle: The notification handle to check
   @param handle: A pointer to the buffer status to fill

   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_get_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
);

/**=============================================================================
   @brief Reset the notification status on a given notification handle

   @param handle: The notification handle
   @param status: The notification status to reset

   @return true if successful, false otherwise

==============================================================================*/
bool icb_notify_reset_notification_status
(
  icb_notify_handle_type  handle,
  icb_notify_status_type *status
);

#endif /* ICB_NOTIFY_H */
