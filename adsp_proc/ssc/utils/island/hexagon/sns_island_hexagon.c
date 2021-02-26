/*============================================================================
  @file sns_island_hexagon.c

  @brief
  Island control for Sensors.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/

#include "sns_island.h"

#include "npa.h"
#include "npa_resource.h"
#include "pb_encode.h"
#include "qurt_atomic_ops.h"
#include "qurt_island.h"
#include "qurt_thread.h"
#include "sns_assert.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_diag_types.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island_service.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_thread_manager.h"
#include "sns_time.h"
#include "sns_types.h"
#include "uSleep.h"

/*============================================================================
  Macros
  ============================================================================*/
#define SNS_ISLAND_CODE __attribute__((section (".text.SNS")))
#define SNS_ISLAND_DATA __attribute__((section (".data.SNS")))
#define SNS_ISLAND_RODATA __attribute__((section (".rodata.SNS")))

#define SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE 5
#define SNS_ISLAND_TRANSITION_LOG_ARR_SIZE 5
#define SNS_ISLAND_MAX_BLOCKED_THREADS 15

/*============================================================================
  Types
  ============================================================================*/

typedef enum
{
  ISLAND_ENTER_CALLBACK = 0,
  ISLAND_EXIT_CALLBACK,
  ISLAND_ENTRY_BLOCKED,
  ISLAND_ENTRY_UNBLOCKED,
  NO_ACTIVE_THREADS,
  ISLAND_EXIT_CALLED
}sns_island_state_transition_triggers;

// System state.
#define SET_IN_ISLAND(state) qurt_atomic_or(&state, 0x04)
#define SET_NOT_IN_ISLAND(state) qurt_atomic_and(&state, 0x03)
#define IS_IN_ISLAND(state) (state & 0x04)

// SEE state
#define SET_ISLAND_UNBLOCKED(state) qurt_atomic_or(&state, 0x02)
#define SET_ISLAND_BLOCKED(state) qurt_atomic_and(&state, 0x05)
#define IS_UNBLOCKED(state)(state & 0x02)

// SEE vote
#define SET_ISLAND_ENABLED(state) qurt_atomic_or(&state, 0x01)
#define SET_ISLAND_DISABLED(state) qurt_atomic_and(&state, 0x06)
#define IS_ENABLED(state) (state & 0x01)

typedef enum
{
  NOT_IN_ISLAND_BLOCKED_DISABLED   = 0x00,    // 0000 0000
  NOT_IN_ISLAND_UNBLOCKED_DISABLED = 0x02,    // 0000 0010
  NOT_IN_ISLAND_UNBLOCKED_ENABLED  = 0x03,    // 0000 0011
  IN_ISLAND_UNBLOCKED_ENABLED      = 0x07,     // 0000 0111
  TRANSITION_FROM_NOT_IN_ISLAND_BLOCKED_DISABLED   = 0x10,    // 0001 0000
  TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_DISABLED = 0x12,    // 0001 0010
  TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_ENABLED  = 0x13,    // 0001 0011
  TRANSITION_FROM_IN_ISLAND_UNBLOCKED_ENABLED      = 0x17     // 0001 0111
}sns_island_valid_island_states;

typedef struct
{
  bool in_use;
  sns_island_exit_cb cb;
  intptr_t args;
} sns_island_exit_signals;

typedef struct
{
  uint32_t thread_id;
  bool block;
}sns_island_thread_blocks;


typedef struct
{
  sns_isafe_list_item list_entry;
  char          client_name[16];
  sns_osa_lock *lock;
}sns_island_aggregator_client;

typedef struct
{
  /* Island service pointer */
  sns_island_service service;

  /* Client handle provided by the NPA */
  npa_client_handle sns_island_handle;

  /* Current island state */
  unsigned int current_state;

  /* Enable/Disable Configuration via Registry */
  bool configure_island_enable;

  /* Mutex protecting current_state
  */
  sns_osa_lock *island_state_lock;

  /* Timestamp of last entry into island mode*/
  sns_time island_entry_timestamp;

  /* Timestamp of last exit out of island mode*/
  sns_time island_exit_timestamp;

  /* Island exit notifications */
  sns_island_exit_signals sns_island_exit_notify[SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE];

  /* Island blocks */
  sns_isafe_list island_blocks;

  /* Last island vote*/
  unsigned int last_vote;

  /* Last enable vote */
  sns_time last_enable;

  /* Previous enable */
  sns_time prvs_enable;

  /* Last disable vote */
  sns_time last_disable;

  /* Previous disable */
  sns_time prvs_disable;

  /* Last entry */
  sns_time last_entry;

  /* Last exit */
  sns_time last_exit;

  /* previous entry */
  sns_time prvs_entry;

  /* previous exit */
  sns_time prvs_exit;

}sns_fw_island_state;

/*============================================================================
  Globals
  ============================================================================*/
static sns_fw_island_state island SNS_ISLAND_DATA;

//External defined in sensors.lcs.template file
#ifndef SNS_NO_ISLAND_PTR_CHECKS
extern uint32_t __sensors_island_start;
extern uint32_t __sensors_island_end;
uintptr_t sns_island_size SNS_ISLAND_DATA = 0;
#endif

//Mappings from the 2018 island states to the legacy island states
const sns_island_state map_to_legacy_states[] SNS_ISLAND_RODATA =
{
  SNS_ISLAND_STATE_ISLAND_DISABLED, // map_to_legacy_states[NOT_IN_ISLAND_BLOCKED_DISABLED]
  SNS_ISLAND_STATE_ISLAND_DISABLED, // Invalid state NOT_IN_ISLAND_BLOCKED_ENABLED
  SNS_ISLAND_STATE_NOT_IN_ISLAND,   // map_to_legacy_states[NOT_IN_ISLAND_UNBLOCKED_DISABLED]
  SNS_ISLAND_STATE_NOT_IN_ISLAND,   // map_to_legacy_states[NOT_IN_ISLAND_UNBLOCKED_ENABLED]
  SNS_ISLAND_STATE_ISLAND_DISABLED, // Invalid state IN_ISLAND_BLOCKED_DISABLED
  SNS_ISLAND_STATE_ISLAND_DISABLED, // Invalid state IN_ISLAND_BLOCKED_ENABLED
  SNS_ISLAND_STATE_ISLAND_DISABLED, // Invalid state IN_ISLAND_UNBLOCKED_DISABLED
  SNS_ISLAND_STATE_IN_ISLAND        // map_to_legacy_states[IN_ISLAND_UNBLOCKED_ENABLED]
};

/*============================================================================
  Static Functions
  ============================================================================*/

static bool
sns_island_disable_silent()
{
  sns_osa_lock_acquire(island.island_state_lock);
  bool ret_val = false;
  sns_time now = sns_get_system_time();
  if (qurt_atomic_compare_and_set(&island.last_vote, SNS_ISLAND_ENABLE, SNS_ISLAND_DISABLE))
  {
      npa_issue_required_request(island.sns_island_handle, SNS_ISLAND_DISABLE);
      island.prvs_disable = island.last_disable;
      island.last_disable = now;
      ret_val = true;
  }
  island.island_entry_timestamp = 0;
  island.island_exit_timestamp = now;
  sns_osa_lock_release(island.island_state_lock);
  return ret_val;
}

static bool
sns_island_enable_silent()
{
  sns_osa_lock_acquire(island.island_state_lock);
  bool ret_val = false;
  sns_time now = sns_get_system_time();
  if (!SNS_DISABLE_ISLAND &&
      island.configure_island_enable &&
      qurt_atomic_compare_and_set(&island.last_vote, SNS_ISLAND_DISABLE, SNS_ISLAND_ENABLE))
  {
      npa_issue_required_request(island.sns_island_handle, SNS_ISLAND_ENABLE);

      island.prvs_enable = island.last_enable;
      island.last_enable = now;
      ret_val = true;
  }
  sns_osa_lock_release(island.island_state_lock);
  return ret_val;
}

//  sns_island_state_transition
//
// Updates the island manager's internal state provided a triggering event. See the
// state machine below for more details.
//
// @param[i] this    pointer to the island manager
// @param[i] trigger Event that should cause a state change
//
// @return  true if the trigger produced a state change
//          false otherwise
//
//                                                      SYSTEM_STATE, SEE_STATE, SEE_VOTE
//
//  sns_island_init()
//      |
//      |         +-------------------------------------------------------------------------------------------------------------------------+
//      |         |                                                                                                                         |
//      |         |    +----------------------------------------------------+ISLAND_EXIT_CALLED                                             |
//      |         |    |                                                    |                                                               |
//      |         |    |                               +--------------------+----------------------+                                        |
//      |         |    |   +---------------------------> Not in Island, Island unblocked,  Enabled +------------------------------+         |
//      |         |    |   |                           +----------------------+--------^-----------+ ISLAND_ENTER_CALLBACK        |         |ISLAND_EXIT_CALLED
//      |         |    |   |                                                  |        |                                          |         |
//      |         |    |   | NO_ACTIVE_THREADS                                |        |                                          |         |
//      |         |    |   |                                                  |        |                       +------------------v---------+---------+
//    +-v---------v----v---+-----------------------+                          |        +-----------------------+ In Island, Island unblocked, Enabled |
//    | Not in Island, Island unblocked,  Disabled +-----------------+        |        ISLAND_EXIT_CALLBACK    +------------------+-------------------+
//    +--------------------^-----------------------+                 |        |                                                   |
//                         |                                         |        |                                                   |
//                         |                    ISLAND_ENTRY_BLOCKED |        |ISLAND_ENTRY_BLOCKED                               |ISLAND_ENTRY_BLOCKED
//                         | ISLAND_ENTRY_UNBLOCKED                  |        |                                                   |
//                         |                           +-------------v--------v------------------+                                |
//                         +---------------------------+ Not in Island, Island blocked, Disabled <--------------------------------+
//                                                     +-----------------------------------------+
//
//
//  Invalid states:
//
//  In Island, Island unblocked, Disabled
//  In Island, Island blocked, Enabled
//  In Island, Island blocked, Disabled
//  Not in Island, Island blocked, Enabled
//
//
SNS_ISLAND_CODE static bool
sns_island_state_transition(sns_fw_island_state *this, sns_island_state_transition_triggers trigger)
{
  unsigned int initial_state = this->current_state;

  switch(trigger)
  {
    case ISLAND_ENTER_CALLBACK:
      qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_ENABLED, IN_ISLAND_UNBLOCKED_ENABLED);
      break;
    case ISLAND_EXIT_CALLBACK:
      qurt_atomic_compare_and_set(&this->current_state, IN_ISLAND_UNBLOCKED_ENABLED, NOT_IN_ISLAND_UNBLOCKED_ENABLED);
      break;
    case ISLAND_ENTRY_BLOCKED:
      if(qurt_atomic_compare_and_set(&this->current_state, IN_ISLAND_UNBLOCKED_ENABLED, TRANSITION_FROM_IN_ISLAND_UNBLOCKED_ENABLED) ||
         qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_ENABLED, TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_ENABLED) ||
         qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_DISABLED, TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_DISABLED))
        {
          if(IS_IN_ISLAND(this->current_state))
          {
            uSleep_exit();
          }
          if(IS_ENABLED(this->current_state))
          {
            sns_island_disable_silent();
          }
          qurt_atomic_set(&this->current_state, NOT_IN_ISLAND_BLOCKED_DISABLED);
        }
        break;
    case ISLAND_ENTRY_UNBLOCKED:
      qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_BLOCKED_DISABLED, NOT_IN_ISLAND_UNBLOCKED_DISABLED);
      break;
    case NO_ACTIVE_THREADS:
      if(qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_DISABLED, TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_DISABLED))
      {
        sns_island_enable_silent();
        qurt_atomic_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_ENABLED);
      }
      break;
    case ISLAND_EXIT_CALLED:
      if(qurt_atomic_compare_and_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_ENABLED, TRANSITION_FROM_NOT_IN_ISLAND_UNBLOCKED_ENABLED) ||
         qurt_atomic_compare_and_set(&this->current_state, IN_ISLAND_UNBLOCKED_ENABLED, TRANSITION_FROM_IN_ISLAND_UNBLOCKED_ENABLED))
      {
        if(IS_IN_ISLAND(this->current_state))
        {
          uSleep_exit();
        }
        sns_island_disable_silent();
        qurt_atomic_set(&this->current_state, NOT_IN_ISLAND_UNBLOCKED_DISABLED);
      }
      else if(IS_IN_ISLAND(this->current_state))
      {
        // Another thread is currently busy pulling the system out of island mode.
        // Do not return till we are certain that we are out of island mode
        uSleep_exit();
      }
      break;
    default:
      SNS_ASSERT(0);
  }

//  if(initial_state != this->current_state)
//  {
//     SNS_PRINTF(LOW, sns_fw_printf, "Island state transition %x ==> %x", initial_state, this->current_state);
//  }
  return (initial_state != this->current_state);
}

SNS_ISLAND_CODE static sns_rc
sns_island_log_transition_encode_cb(void *log, size_t log_size,
                                    size_t encoded_log_size, void *encoded_log,
                                    size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;
  if(NULL == log || 0 == log_size ||
     NULL == encoded_log || 0 == encoded_log_size ||
     NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_island_transition_log_fields, log))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding island transition log : %s",
               PB_GET_ERROR(&stream));
    rc = SNS_RC_FAILED;
  }

  if(SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}

SNS_ISLAND_CODE static void
sns_island_log_transition(sns_island_state current_state, uint64_t timestamp,
                         bool has_user_cookie, uint64_t user_cookie, bool submit_asynchronously)
{
  sns_diag_async_log_item async_log;
  _Static_assert(sizeof(sns_diag_island_transition_log) < sizeof(async_log.payload),
                 "sizeof(sns_diag_island_transition_log)< sizeof(async_log.payload)");
  sns_diag_island_transition_log *island_log = submit_asynchronously ?
    (sns_diag_island_transition_log *)&async_log.payload[0] :
    sns_diag_log_alloc(sizeof(sns_diag_island_transition_log), SNS_LOG_ISLAND_TRANSITION);
  if (NULL != island_log)
  {
    island_log->timestamp = timestamp;
    if (SNS_ISLAND_STATE_IN_ISLAND == current_state)
    {
      island_log->island_state = SNS_DIAG_ISLAND_STATE_IN_ISLAND_MODE;
    }
    else if (SNS_ISLAND_STATE_NOT_IN_ISLAND == current_state)
    {
      island_log->island_state = SNS_DIAG_ISLAND_STATE_NOT_IN_ISLAND_MODE;
    }
    else
    {
      island_log->island_state = SNS_DIAG_ISLAND_STATE_ISLAND_DISABLED;
    }
    island_log->has_cookie = has_user_cookie;
    island_log->cookie = user_cookie;
    if (submit_asynchronously)
    {
      async_log.encoded_size = sns_diag_island_transition_log_size;
      async_log.log_id = SNS_LOG_ISLAND_TRANSITION;
      async_log.payload_size = sizeof(sns_diag_island_transition_log);
      async_log.payload_encode_cb = sns_island_log_transition_encode_cb;
      async_log.timestamp = sns_get_system_time();
      sns_diag_submit_async_log(async_log);
    }
    else
    {
      //First consume all the previously recorded transitions
      sns_diag_consume_async_logs(true);

      //Next, add this log to the diag list
      sns_diag_publish_fw_log(
        SNS_LOG_ISLAND_TRANSITION, sizeof(sns_diag_island_transition_log), island_log,
        sns_diag_island_transition_log_size, sns_island_log_transition_encode_cb);
    }
  }
}

SNS_ISLAND_CODE static void
sns_island_state_notification_cb(uSleep_state_notification state)
{
  SNS_PROFILE(SNS_ISLAND_CALLBACK_START, 1, state);

  //Update internal island state
  if((USLEEP_STATE_ENTER == state))
  {
    island.island_entry_timestamp = sns_get_system_time();
    island.prvs_entry = island.last_entry;
    island.last_entry = island.island_entry_timestamp;
    sns_island_state_transition(&island,ISLAND_ENTER_CALLBACK);
  }
  else
  {
    uint8_t i;

    island.island_exit_timestamp = sns_get_system_time();
    island.prvs_exit = island.last_exit;
    island.last_exit = island.island_exit_timestamp;
    sns_island_state_transition(&island, ISLAND_EXIT_CALLBACK);

    for (i = 0; i < SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE; i++)
    {
      if (island.sns_island_exit_notify[i].in_use)
      {
        island.sns_island_exit_notify[i].cb(island.sns_island_exit_notify[i].args);
      }
    }
    if (0 != island.island_entry_timestamp)
    {
      sns_island_log_transition(SNS_ISLAND_STATE_IN_ISLAND, island.island_entry_timestamp,
        false, 0, true);
    }
    sns_island_log_transition(SNS_ISLAND_STATE_NOT_IN_ISLAND, island.island_exit_timestamp,
        false, 0, true);
  }
  SNS_PROFILE(SNS_ISLAND_CALLBACK_END, 0);
}

/*============================================================================
  Public Functions
  ============================================================================*/

SNS_ISLAND_CODE sns_rc sns_island_init()
{
  sns_time now = sns_get_system_time();
  island.sns_island_handle = npa_create_sync_client(
                    "/core/uSleep",
                    "SNS_ISLAND_uSleep",
                    NPA_CLIENT_REQUIRED );
  island.current_state = NOT_IN_ISLAND_UNBLOCKED_DISABLED;
  island.island_entry_timestamp = 0;
  island.island_exit_timestamp = now;

  sns_island_size = (uintptr_t)&__sensors_island_end - (uintptr_t)&__sensors_island_start;

  sns_memzero(island.sns_island_exit_notify, sizeof(island.sns_island_exit_notify));
  uSleep_notification_cb_handle uSleep_handle;
  uSleep_handle = uSleep_registerNotificationCallback(1000, 0, sns_island_state_notification_cb);
  SNS_ASSERT(uSleep_handle > 0);

  sns_osa_lock_attr attr;
  sns_osa_lock_attr_init(&attr);
  sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  sns_osa_lock_create(&attr, &island.island_state_lock);
  sns_isafe_list_init(&island.island_blocks);

  island.configure_island_enable = true;
  island.last_vote = SNS_ISLAND_DISABLE;
  npa_issue_required_request(island.sns_island_handle, SNS_ISLAND_DISABLE);

  island.prvs_disable = island.last_disable;
  island.last_disable = now;
  island.island_entry_timestamp = 0;
  island.island_exit_timestamp = now;

  return SNS_RC_SUCCESS;
}

SNS_ISLAND_CODE void sns_island_configure_island_enable(bool island_enable)
{
  island.configure_island_enable = island_enable;
}

SNS_ISLAND_CODE sns_rc sns_island_disable()
{
  SNS_ISLAND_EXIT();
  sns_island_disable_silent();
  sns_island_log_transition(SNS_ISLAND_STATE_ISLAND_DISABLED, sns_get_system_time(), false, 0, false);
  sns_pwr_sleep_mgr_set_island_mode_enabled(false);
  return SNS_RC_SUCCESS;
}

SNS_ISLAND_CODE sns_rc sns_island_exit_internal()
{
  bool state_changed = false;
  SNS_PROFILE(SNS_ISLAND_EXIT_START, 0);
  state_changed = sns_island_state_transition(&island, ISLAND_EXIT_CALLED);
  SNS_PROFILE(SNS_ISLAND_EXIT_END, 0);
  return state_changed?SNS_RC_SUCCESS:SNS_RC_NOT_AVAILABLE;
}

SNS_ISLAND_CODE sns_rc sns_island_generate_and_commit_state_log( uint64_t req_id )
{
  //SNS_PRINTF(LOW, sns_fw_printf, "logging island state %x", sns_island_get_island_state());
  sns_island_log_transition(sns_island_get_island_state(),
    sns_get_system_time(), true, req_id, false);

  return SNS_RC_SUCCESS;
}

SNS_ISLAND_CODE sns_rc sns_island_register_island_exit_cb(
  sns_island_exit_cb cb,
  intptr_t args)
{
  uint8_t i;
  for (i = 0; i < SNS_ISLAND_EXIT_NOTIFY_ARR_SIZE; i++)
  {
    if (!island.sns_island_exit_notify[i].in_use)
    {
      island.sns_island_exit_notify[i].cb = cb;
      island.sns_island_exit_notify[i].args = args;
      island.sns_island_exit_notify[i].in_use = true;
      return SNS_RC_SUCCESS;
    }
  }
  return SNS_RC_FAILED;
}

SNS_ISLAND_CODE sns_island_state sns_island_get_island_state()
{
  return map_to_legacy_states[0x07 & island.current_state];
}

SNS_ISLAND_CODE sns_rc sns_island_log_trace(
  uint64_t user_defined_id )
{
  return sns_island_generate_and_commit_state_log( user_defined_id );
}

/*!
  @brief Check if all the data streams for this instance is in island mode

  @param[i] instance        Instance to be checked

  @return
  true                 If all data streams for this instance are in island mode
  false                Otherwise
*/
SNS_ISLAND_CODE bool sns_instance_streams_in_island(sns_sensor_instance *instance)
{
  sns_isafe_list_iter iter;
  sns_fw_data_stream *data_stream;
  bool rv = true;
  sns_fw_sensor_instance *fw_instance;

  fw_instance = (sns_fw_sensor_instance *)instance;

  sns_isafe_list_iter_init(&iter, &fw_instance->data_streams, true);
  for(data_stream = sns_isafe_list_iter_get_curr_data(&iter);
    NULL != data_stream; data_stream = sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
  {
    if(!sns_island_is_island_ptr((intptr_t)data_stream))
    {
      rv = false;
      break;
    }
  }
  return rv;
}

sns_island_client_handle sns_island_aggregator_register_client( const char * client_name)
{
  sns_island_aggregator_client *client = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_island_aggregator_client));
  sns_osa_lock_attr attr;

  SNS_ASSERT(NULL != client);
  sns_osa_lock_attr_init(&attr);
  sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);

  sns_strlcpy(client->client_name, client_name, sizeof(client->client_name));
  sns_isafe_list_item_init(&client->list_entry, client);
  sns_osa_lock_create(&attr, &client->lock);

  return (sns_island_client_handle)client;
}

void sns_island_aggregator_deregister_client(sns_island_client_handle client_handle)
{
  sns_island_aggregator_client *client = (sns_island_aggregator_client *)
    client_handle;
  if(NULL != client)
  {
    sns_island_unblock(client); // Ensure the client is no longer blocking island entry
    sns_osa_lock_delete(client->lock);
    sns_free(client);
  }
}

SNS_ISLAND_CODE sns_rc sns_island_block(sns_island_client_handle client_handle)
{
  sns_island_aggregator_client *client = (sns_island_aggregator_client *)
    client_handle;
  sns_rc ret_val = SNS_RC_INVALID_TYPE;
  sns_time block_timestamp = 0;
  if (NULL != client)
  {
    SNS_ISLAND_EXIT();
    sns_osa_lock_acquire(client->lock);
    sns_osa_lock_acquire(island.island_state_lock);
    if (NULL == client->list_entry.list)
    {
      sns_isafe_list_iter iter;
      sns_isafe_list_iter_init(&iter, &island.island_blocks, true);
      sns_isafe_list_iter_insert(&iter, &client->list_entry, true);
      sns_island_state_transition(&island, ISLAND_ENTRY_BLOCKED);
    }
    block_timestamp = sns_get_system_time();
    sns_osa_lock_release(island.island_state_lock);
    #ifdef SNS_ISLAND_ENABLE_DEBUG
      SNS_SPRINTF(LOW, sns_fw_printf, "Client %s has blocked, Final blocks: %d",
                client->client_name, island.island_blocks.cnt);
    #else
     // SNS_PRINTF(LOW, sns_fw_printf, "Client " SNS_DIAG_PTR " has blocked, Final blocks: %d",
     //           client, island.island_blocks.cnt);
    #endif
    ret_val = SNS_RC_SUCCESS;
    sns_osa_lock_release(client->lock);
  }
  return ret_val;
}

SNS_ISLAND_CODE sns_rc
sns_island_unblock(sns_island_client_handle client_handle)
{
  sns_rc ret_val = SNS_RC_INVALID_TYPE;

  sns_island_aggregator_client *client = (sns_island_aggregator_client *)
    client_handle;
  if (NULL != client)
  {
    sns_osa_lock_acquire(client->lock);
    if(&island.island_blocks == client->list_entry.list)
    {
      bool island_reenabled = false;
      sns_osa_lock_acquire(island.island_state_lock);
      sns_isafe_list_item_remove(&client->list_entry);
      island_reenabled = (0 == island.island_blocks.cnt);
      if(island_reenabled)
      {
        sns_island_state_transition(&island, ISLAND_ENTRY_UNBLOCKED);
      }
      sns_osa_lock_release(island.island_state_lock);
      #ifdef SNS_ISLAND_ENABLE_DEBUG
      SNS_SPRINTF(LOW, sns_fw_printf, "Client %s has unblocked, Final blocks: %d",
                  client->client_name, island.island_blocks.cnt);
      #else
      //SNS_PRINTF(LOW, sns_fw_printf, "Client " SNS_DIAG_PTR " has unblocked, Final blocks: %d",
      //            client, island.island_blocks.cnt);
      #endif
      ret_val = SNS_RC_SUCCESS;
    }
    else if (NULL != client->list_entry.list)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Island aggregator client belongs to unrecognized list");
      ret_val = SNS_RC_SUCCESS;
    }
    sns_osa_lock_release(client->lock);
  }
  return ret_val;
}

SNS_ISLAND_CODE sns_rc sensor_island_exit(sns_island_service *this,
                               struct sns_sensor const* sensor)
{
  UNUSED_VAR(this);
  // TODO add registry support to disable this API for test purpose
  SNS_ISLAND_EXIT();
  SNS_PRINTF(MED, sns_fw_printf, "sensor_island_exit sensor = 0x%X", sensor);

  return SNS_RC_SUCCESS;
}

SNS_ISLAND_CODE sns_rc sensor_instance_island_exit(sns_island_service *this,
                                       struct sns_sensor_instance const* instance)
{
  UNUSED_VAR(this);
  // TODO add registry support to disable this API for test purpose
  SNS_ISLAND_EXIT();
  SNS_PRINTF(MED, sns_fw_printf, "sensor_instance_island_exit instance = 0x%X", instance);

  return SNS_RC_SUCCESS;
}

SNS_ISLAND_CODE sns_rc island_log_trace(sns_island_service *this,
                            uint64_t user_defined_id)
{
  UNUSED_VAR(this);
  // TODO add registry support to disable this API for test purpose
  return sns_island_log_trace(user_defined_id);
}

/**
 * Island Service API.
 */
sns_island_service_api  island_service_api SNS_ISLAND_DATA =
{
  .struct_len = sizeof(sns_island_service_api),
  .sensor_island_exit = &sensor_island_exit,
  .sensor_instance_island_exit = &sensor_instance_island_exit,
  .island_log_trace = &island_log_trace
};

sns_service* sns_island_service_init(void)
{
  /** Island util must be initialized before island service init */
  island.service.api = &island_service_api;
  island.service.service.type = SNS_ISLAND_SERVICE;
  island.service.service.version = 1;
  return (sns_service *)&island;
}

SNS_ISLAND_CODE bool
sns_island_enable()
{
  bool ret_val = false;
  sns_osa_lock_acquire(island.island_state_lock);
  if(0 == island.island_blocks.cnt)
  {
    ret_val = sns_island_state_transition(&island, NO_ACTIVE_THREADS);
  }
  sns_osa_lock_release(island.island_state_lock);
  return ret_val;
}
