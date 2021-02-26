#pragma once
/*=============================================================================
  @file sns_fw_event_service.h

  Handles the storage of all Sensor events.  The ThreadManager is notified
  when new events are available for processing.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_atomic.h"
#include "sns_event_service.h"
#include "sns_isafe_list.h"
#include "sns_island_util.h"
#include "sns_mem_def.h"
#include "sns_osa_lock.h"
#include "sns_thread_manager.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_fw_sensor_instance;
struct sns_fw_sensor;
struct sns_fw_data_stream;
struct sns_fw_sensor_event;
struct sns_osa_thread;
struct sns_thread_mgr_task;

/*=============================================================================
  Preprocessor Definitions
  ===========================================================================*/

/* PEND: Use compiler alignas option for memory alignment */
#define SNS_FW_EVENT_SERVICE_BUFFER_ISLAND (SNS_FW_EVENT_SERVICE_BUFFER_ALLOC*1024)

/* When the main buffer has less than 300K remaining free space, drop data */
#define SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_CB_THRESH (300 * 1024)

/* When the SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_UPPER_THRESH point is reached,
   retain this much data and drop everything else */
#define SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_DROP_TILL_THRESH (SNS_FW_EVENT_SERVICE_BUFFER_ISLAND / 2)

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Private state used by the Framework for the event manager.
 */
typedef struct sns_fw_event_service
{
  sns_event_service service;

  uint64_t *island_buffer;
  uint64_t *buffer; /* PEND: Implement DDR circular buffer and logic */

  /* The beginning and end of the active buffer being used. */
  uintptr_t active_buffer_start;
  uintptr_t active_buffer_end;

  /* Points into buffer, where the next allocated event will be placed */
  uintptr_t buffer_head;
  /* Pointer to the oldest pending event;
   * will always be strictly less than active_buffer_end */
  uintptr_t buffer_tail;

  /* Location in buffer at which tail pointer should roll over and start
     at active_buffer_start */
  uintptr_t rollover_here;

  /* Current and max numbers of allocations; protected by buffer_lock */
  int32_t  num_alloc;
  int32_t  max_alloc;

  /* Current and max numbers of event buffer bytes allocated; protected by buffer_lock */
  int32_t  bytes_allocated;
  int32_t  max_bytes_allocated;

  /* Protects the buffer, island_buffer, and associated meta data.
   * Must be held when writing to the buffer, or the head/tail pointers.
   * Does not need to be held when reading an event from the buffer. */
  sns_osa_lock *buffer_lock;

  /* Event service requires disabling island mode */
  bool disable_island;

  /* Whether the low memory thread is actively dropping Event Service samples;
   * Field is not mutex-protected */
  _Atomic unsigned int low_mem_active;

  /* Island client for event service */
  sns_island_client_handle island_client;
} sns_fw_event_service;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize the event service; to be used only by the Service Manager.
 */
sns_fw_event_service* sns_event_service_init(void);

/* See sns_event_service_api::publish_error */
sns_rc sns_event_service_publish_error(
    struct sns_fw_sensor_instance *instance, sns_rc reason);

/**
 * Mark an event as consumed by one of its clients.  If this event was located
 * on the buffer tail, and it has no other pending clients, free the event, and
 * any other clientless events on the buffer tail.
 *
 * @param[i] fw_event Event just processed; to be removed if at the tail
 */
void sns_event_service_consume(struct sns_fw_sensor_event *fw_event);

/**
 * Allocate and send an event to a single client.  This function may be used by
 * the Framework when an event is only applicable to a single client, and that
 * client may not have (yet) an instance to satisfy it.  Examples include
 * errors during set_client_request, or handling Attribute Requests.
 *
 * @param[i] stream Stream on which to send the event
 * @param[i] message_id
 * @param[i] event_len Length of the event buffer
 * @param[i] event Pointer to a PB encoded event message
 *
 * @return
 * SNS_RC_NOT_AVAILABLE - Closing stream is unavailable
 * SNS_RC_SUCCESS
 */
sns_rc sns_event_service_send_event(struct sns_fw_data_stream *stream,
    uint32_t message_id, uint32_t event_len, uint8_t *event);

/**
 * Publish a generic error event with appropriate error reason.
 * @note This is a helper function; an error event could also be
 * generated and published using the alloc_event/publish_event API.
 *
 * @param[i] stream Stream on which to send the event
 * @param[i] reason,
 * SNS_RC_FAILED - A catastrophic failure has occurred, restart the Framework
 * SNS_RC_NOT_AVAILABLE - No data is available for processing
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_event_service_send_error_event(struct sns_fw_data_stream *stream,
    sns_rc reason);

/**
 * Unlike other structures in the API, the start of the sns_fw_sensor_event
 * and sns_sensor_event structures are not equivalent.  Therefore some pointer
 * arithmatic must be performed.
 *
 * @param[i] fw_event Valid sensor event
 *
 * @return Framework event corresponding to input
 */
struct sns_fw_sensor_event* sns_event_service_get_fw_event(
    struct sns_sensor_event *fw_event);

/**
 * Generates log packets that contain statistics on the event buffer usage
 *
 * @param[i] cookie Cookie that is used to populate the user_defiend field
 *                  in the log packet
 */
void sns_event_service_log_buffer_use(uint64_t cookie);

/**
 * Remove an unpublished event allocated by a sensor instance that is
 * about to be deinitialized.
 *
 * @param[i] instance Instance that is about to be deinitialized
 */
void sns_event_service_deinit_instance(struct sns_fw_sensor_instance *instance);

/**
 * @return True if the low memory thread is actively dropping events.
 */
bool sns_event_service_low_mem_active(void);

/**
 * Add a datastream to list of pending events
 *
 * @param[i] stream Data stream that has pending events
 */
void sns_event_service_add_pending_event(struct sns_fw_data_stream *stream);
