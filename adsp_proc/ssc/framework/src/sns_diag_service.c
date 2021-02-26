/*=============================================================================
  @file sns_diag_service.c

  Manages Diagnostic Services for Sensors and Sensor Instances

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_attribute_service.h"
#include "sns_cstruct_extn.h"
#include "sns_diag.pb.h"
#include "sns_diag_print.h"
#include "sns_diag_service.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_diag_types.h"
#include "sns_fw_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_log.h"
#include "sns_log_codes.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_sem.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_rc.h"
#include "sns_sensor_uid.h"
#include "sns_service.h"
#include "sns_thread_prio.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_fw_log.h"
#include "sns_fw_request.h"

#define SNS_DIAG_LIST_SIZE_LIM          50
#define SNS_DIAG_LIST_LEN_UPPER_THRESH  200
#define SNS_DIAG_LIST_LEN_LOWER_THRESH  5

#define SNS_DIAG_MAX_ISLAND_ALLOC_SIZE 30*1024 // bytes
#define SNS_DIAG_ISLAND_REENTRY_ALLOC_SIZE ((size_t)(0.5 * SNS_DIAG_MAX_ISLAND_ALLOC_SIZE))

#define SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE 12
#define SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_MIN_EVENTS_TO_DRAIN 10

/* Drop log packets in DIAG Service when remaining free memory is below this */
#define SNS_DIAG_SERVICE_BUFFER_LOW_MEM (SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_CB_THRESH + (100 * 1024))

/*  
* Diag allows maximum 244 bytes including diag log header in island.
* Diag log header packet length is 12 bytes and diag service additional fields length is ~53.
* The maximum permitted log size that sensors - algos / drivers are allowed to
* allocate 178 bytes in island and 1600 bytes in non-island.
*/
#define SNS_DIAG_IN_ISLAND_MAX_SENSOR_LOG_SIZE (178) // bytes

#define SNS_DIAG_NON_ISLAND_MAX_SENSOR_LOG_SIZE (1600) // bytes

/* The SUID used by the dummy sensor used by diag to refer to all framework operations */
// TODO: Remove this when we have a real framework sensor
#define FW_SUID {  \
    .sensor_uid =  \
      {  \
        0x1 ,0x2 ,0x3 ,0x4 ,0x5 ,0x6 ,0x7 ,0x8 ,  \
        0x9 ,0xa ,0xb ,0xc ,0xd ,0xe ,0xf ,0x0  \
      }  \
  }

// Uncomment to enable debug messages for Diag
//#define SNS_DIAG_ENABLE_DEBUG

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Private state used by the Framework for the Diag manager.
 */
typedef struct sns_fw_diag_service
{
  sns_diag_service service;

  /* List of type sns_diag_datatype_config of discovered sensor
     data types and whether they are enabled or disabled */
  sns_list datatypes;
  /* List of datatypes that have some updates to be processed */
  sns_list config_move_queue;
  /* Lock that protects datatypes and config_move_queue */
  sns_osa_lock *data_type_list_lock;

  size_t encoded_sensor_api_hdr_size;
  size_t encoded_client_api_hdr_size;
  size_t encoded_island_transition;

  // total outstanding diag allocation in heap in bytes
  size_t island_alloc_size;
  size_t main_alloc_size;

  sns_island_client_handle island_client;

  // Client for SCPM voting
  sns_scpm_mgr_client *pwr_client;

  // Circular buffer
  sns_diag_async_log_item circ_buffer[SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE];
  sns_osa_lock *circ_buffer_idx_lock;
  uint8_t circ_buffer_read_idx;
  uint8_t circ_buffer_write_idx;
  uint8_t circ_buffer_pending_cnt;

  // Number of dropped log packets due to inability to malloc
  uint64_t num_dropped_logs;

  // Number of log packets dropped at diag layer
  uint32_t num_diag_dropped_logs;
} sns_fw_diag_service;

typedef struct
{
  sns_diag_datatype_config *old;
  sns_diag_datatype_config *new;
}sns_diag_config_move;

typedef struct
{
  size_t array_len;
  sns_diag_config_move *array;
}sns_diag_config_move_arg;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/
// Diag Service API
static sns_diag_service_api diag_service_api SNS_SECTION(".data.sns");
// Diag Service
static sns_fw_diag_service diag_service SNS_SECTION(".data.sns");

// Diag Thread
static sns_osa_thread *sns_diag_thread SNS_SECTION(".data.sns");

// List that stores log packets to be submitted in diag thread
static sns_isafe_list sns_diag_list SNS_SECTION(".data.sns");

// Lock on sns_diag_list
static sns_osa_lock *sns_diag_lock SNS_SECTION(".data.sns");

// Semaphor used by diag thread
static sns_osa_sem *sns_diag_sem SNS_SECTION(".data.sns");

// SUID used by the framework sensor (dummy sensor used by diag)
static const sns_sensor_uid fw_suid SNS_SECTION(".rodata.sns") = FW_SUID;

// Default datatype config for uninitialized sensors
sns_diag_datatype_config *default_datatype SNS_SECTION(".data.sns") = NULL;

// PEND: Need to make this an actual Sensor
static sns_fw_sensor fw_printf_sensor SNS_SECTION(".data.sns");

/*=============================================================================
  Global Data Definitions
  ===========================================================================*/
sns_sensor *sns_fw_printf SNS_SECTION(".data.sns") = (sns_sensor *)&fw_printf_sensor;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Find a datatype
 *
 * @param[i] datatype    Data type to be found
 *
 * @return sns_diag_datatype_config*
 */
static sns_diag_datatype_config *sns_diag_find_datatype(const char *datatype)
{
  sns_list_iter iter;
  sns_list_item *diag_config_item;
  sns_diag_datatype_config *ret_val = NULL;
  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  for(diag_config_item = sns_list_iter_init(&iter, &diag_service.datatypes, true);
      NULL != diag_config_item;
      diag_config_item = sns_list_iter_advance(&iter))
  {
    sns_diag_datatype_config *curr = (sns_diag_datatype_config *)sns_list_item_get_data(diag_config_item);
    if(0 == strcmp(datatype, curr->datatype))
    {
      ret_val = curr;
      break;
    }
  }
  sns_osa_lock_release(diag_service.data_type_list_lock);
  #ifdef SNS_DIAG_ENABLE_DEBUG
    SNS_SPRINTF(HIGH, sns_fw_printf, "Found %s at 0x" SNS_DIAG_PTR " ",
                datatype, ret_val);
  #endif
  return ret_val;
}

/**
 * Add a new datatype
 *
 * @param[i] datatype  The datatype to be recorded
 * @param[i] island    If the datatype is to be permanently
 *                       stored in island mode
 *
 * @return sns_diag_datatype_config*
 */
static sns_diag_datatype_config *sns_diag_add_datatype(const char *datatype, bool island)
{
  sns_list_iter iter;
  sns_mem_heap_id heap = island ? SNS_HEAP_ISLAND : SNS_HEAP_MAIN;
  sns_diag_datatype_config *config = sns_diag_find_datatype(datatype);

  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  if(NULL == config)
  {
    config = sns_malloc(heap, sizeof(sns_diag_datatype_config));
    SNS_ASSERT(NULL != config);

    sns_strlcpy(config->datatype, datatype, SNS_SENSOR_DATATYPE_NAME_LEN);
    sns_list_item_init(&config->datatypes_list_entry, config);
    sns_list_item_init(&config->config_move_entry, config);

    config->island_cnt = 0;
    config->enabled = true;
    config->disable_moving = island;
    config->vprintf_fptr = sns_diag_vprintf_v2;

    sns_list_iter_init(&iter, &diag_service.datatypes, true);
    sns_list_iter_insert(&iter, &config->datatypes_list_entry, false);
  }
  sns_osa_lock_release(diag_service.data_type_list_lock);

  return config;
}
/**
 * Return proper diag log size based on island/non-island mode
 */

SNS_SECTION(".text.sns")static uint32_t
sns_diag_get_sensor_max_log_size(void)
{
  uint32_t log_size = 0;
  if(SNS_ISLAND_STATE_IN_ISLAND == sns_island_get_island_state())
  {
    log_size = SNS_DIAG_IN_ISLAND_MAX_SENSOR_LOG_SIZE;
  }
  else
  {
    log_size = SNS_DIAG_NON_ISLAND_MAX_SENSOR_LOG_SIZE;
  }
  return log_size;
}

/**
 * Find and return the Qualcomm diag log id used to commit the log
 */
SNS_SECTION(".text.sns") static uint32_t
sns_diag_get_diag_log_id(sns_fw_diag_service_log_id log_id)
{
  if(log_id == SNS_LOG_CLIENT_API_REQUEST)
  {
    #ifdef LOG_SNS_CLIENT_API_REQUEST
      return LOG_SNS_CLIENT_API_REQUEST;
    #else
    return LOG_SNS_SMR_REQUEST_C; //0x14F3
    #endif
  }
  else if(log_id == SNS_LOG_CLIENT_API_RESPONSE)
  {
    #ifdef LOG_SNS_CLIENT_API_RESPONSE
      return LOG_SNS_CLIENT_API_RESPONSE;
    #else
    return LOG_SNS_SMR_RESPONSE_C; //0x14F4
    #endif
  }
  else if(log_id == SNS_LOG_CLIENT_API_EVENT)
  {
    #ifdef LOG_SNS_CLIENT_API_EVENT
      return LOG_SNS_CLIENT_API_EVENT;
    #else
    return LOG_SNS_SMR_INDICATION_C; //0x14F5
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_API_REQUEST)
  {
    #ifdef LOG_SNS_SENSOR_API_REQUEST
      return LOG_SNS_SENSOR_API_REQUEST;
    #else
    return LOG_SNS_SENSOR1_REQUEST_C; //0x14F0
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_API_EVENT)
  {
    #ifdef LOG_SNS_SENSOR_API_EVENT
      return LOG_SNS_SENSOR_API_EVENT;
    #else
    return LOG_SNS_SENSOR1_INDICATION_C; //0x14F2
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_RAW_INT)
  {
    #ifdef LOG_SNS_SENSOR_STATE_INT
      return LOG_SNS_SENSOR_STATE_INT;
    #else
    return LOG_SNS_QMD_RESULT_C; //0x151E
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_INTERNAL_INT ||
          log_id == SNS_LOG_QSOCKET_CLIENT_API)
  {
    #ifdef LOG_SNS_SENSOR_STATE_INT
      return LOG_SNS_SENSOR_STATE_INT;
    #else
    return LOG_SNS_QMD_RESULT_C; //0x151E
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_RAW_EXT)
  {
    #ifdef LOG_SNS_SENSOR_STATE_EXT
      return LOG_SNS_SENSOR_STATE_EXT;
    #else
    return LOG_SNS_SENSOR1_RESPONSE_C; //0x14F1
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_INTERNAL_EXT)
  {
    #ifdef LOG_SNS_SENSOR_STATE_EXT
      return LOG_SNS_SENSOR_STATE_EXT;
    #else
    return LOG_SNS_SENSOR1_RESPONSE_C; //0x14F1
    #endif
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_INTERRUPT)
  {
    #ifdef LOG_SNS_HW_INTERRUPT
      return LOG_SNS_HW_INTERRUPT;
    #else
    return LOG_SNS_DRV_MD_IRQ_C; //0x1559
    #endif
  }
  else if(log_id == SNS_LOG_INSTANCE_MAP)
  {
    #ifdef LOG_SNS_INSTANCE_MAP
      return LOG_SNS_INSTANCE_MAP;
    #else
    return LOG_SENSORS_SDP_INTERNAL; //0x151A
    #endif
  }
  else if(log_id == SNS_LOG_ISLAND_TRANSITION)
  {
    #ifdef LOG_ISLAND_TRANSITION_C
      return LOG_ISLAND_TRANSITION_C;
    #else
      return LOG_SENSORS_SDP_INTERNAL; //0x151A
    #endif
  }
  else if(log_id == SNS_LOG_ISLAND_EXIT_VOTE)
  {
    #ifdef LOG_ISLAND_EXIT_VOTE_C
      return LOG_ISLAND_EXIT_VOTE_C;
    #else
      return LOG_SENSORS_SDP_INTERNAL; //0x151A
    #endif
  }
  else if(log_id == SNS_LOG_MEMORY_UTILIZATION)
  {
    #ifdef LOG_MEMORY_UTILIZATION_C
      return LOG_MEMORY_UTILIZATION_C;
    #else
      return LOG_SENSORS_SDP_INTERNAL; //0x151A
    #endif
  }
  else
  {
     return 0;
  }
}

/**
 * Translate sns_diag_sensor_state_log to sns_fw_diag_service_log_id
 */
SNS_SECTION(".text.sns")static sns_fw_diag_service_log_id
sns_diag_get_sensor_state_log_id(sns_diag_sensor_state_log log_type,
                                 uint32_t debug_ssid)
{
  sns_fw_diag_service_log_id log_id = SNS_DIAG_SENSOR_STATE_LOG_NUM;

  if(SNS_DIAG_SENSOR_STATE_LOG_INTERNAL == log_type)
  {
    log_id = (MSG_SSID_SNS == debug_ssid ||
              MSG_SSID_SNS_FRAMEWORK == debug_ssid ||
              MSG_SSID_SNS_PLATFORM == debug_ssid ||
              MSG_SSID_SNS_SENSOR_INT == debug_ssid) ?
       SNS_LOG_SENSOR_STATE_INTERNAL_INT : SNS_LOG_SENSOR_STATE_INTERNAL_EXT;
  }
  else if(SNS_DIAG_SENSOR_STATE_LOG_RAW == log_type)
  {
    log_id = (MSG_SSID_SNS == debug_ssid ||
              MSG_SSID_SNS_FRAMEWORK == debug_ssid ||
              MSG_SSID_SNS_PLATFORM == debug_ssid ||
              MSG_SSID_SNS_SENSOR_INT == debug_ssid) ?
      SNS_LOG_SENSOR_STATE_RAW_INT : SNS_LOG_SENSOR_STATE_RAW_EXT;
  }
  else if(SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT == log_type)
  {
    log_id = SNS_LOG_SENSOR_STATE_INTERRUPT;
  }
  else
  {
    log_id = SNS_LOG_MAX_ID;
  }

  return log_id;
}

SNS_SECTION(".text.sns") sns_diag_log_info*
sns_diag_get_log_info(void *payload)
{
  sns_diag_log_info *rv = (sns_diag_log_info*)((uintptr_t)payload -
      sizeof(rv->info));

  return rv;
}

/**
 * Disables island mode
 *
 * Note: sns_diag_lock must be held while calling this function
 */
SNS_SECTION(".text.sns") static void
sns_diag_disable_island(sns_fw_diag_service *this)
{
  sns_island_block(this->island_client);
}

/**
 * Attempt to reenable island mode
 *
 * Note: sns_diag_lock must be held while calling this function
 */
SNS_SECTION(".text.sns") static void
sns_diag_attempt_reenable_island(sns_fw_diag_service *this)
{
  bool unblock = (this->island_alloc_size <= SNS_DIAG_ISLAND_REENTRY_ALLOC_SIZE &&
      this->main_alloc_size == 0);
  if(unblock)
  {
    sns_island_unblock(this->island_client);
  }
}

SNS_SECTION(".text.sns") void*
sns_diag_log_alloc(uint32_t log_size,
                   sns_fw_diag_service_log_id log_id)
{
  sns_diag_log_info *log_info = NULL;
  void *payload = NULL;
  uint32_t diag_log_id = sns_diag_get_diag_log_id(log_id);

  if(sns_log_status(diag_log_id) && log_size <= sns_diag_get_sensor_max_log_size())
  {
    size_t log_info_size = log_size + sizeof(sns_diag_log_info);

    sns_osa_lock_acquire(sns_diag_lock);
    if((diag_service.island_alloc_size + log_info_size
        <= SNS_DIAG_MAX_ISLAND_ALLOC_SIZE) &&
        (SNS_ISLAND_STATE_IN_ISLAND == sns_island_get_island_state()))
    {
      log_info = sns_malloc(SNS_HEAP_ISLAND, log_info_size);

      if(NULL != log_info)
      {
        diag_service.island_alloc_size += log_info_size;
      }
    }

    if(NULL == log_info)
    {
      sns_diag_disable_island(&diag_service);
      log_info = sns_malloc(SNS_HEAP_MAIN, log_info_size);

      if(NULL != log_info)
      {
        diag_service.main_alloc_size += log_info_size;
      }
      else
      {
        SNS_PRINTF(ERROR, sns_fw_printf,
            "Unable to allocate space for log size:%u in main heap, dropping",
            log_info_size);
        diag_service.num_dropped_logs++;
      }
    }
    sns_osa_lock_release(sns_diag_lock);

    if(NULL != log_info)
    {
      log_info->info.payload_alloc_size = log_size;
      log_info->info.sanity = 0xABCDABCD;
      payload = log_info->payload;
    }
  }

  return payload;
}

SNS_SECTION(".text.sns") void
sns_diag_log_free(sns_diag_log_info *log_info)
{
  size_t log_info_size = log_info->info.payload_alloc_size +
    sizeof(sns_diag_log_info);

  sns_osa_lock_acquire(sns_diag_lock);

  if(sns_island_is_island_ptr((intptr_t)log_info))
  {
    if(diag_service.island_alloc_size >= log_info_size)
    {
      diag_service.island_alloc_size -= log_info_size;
    }
    else
    {
      SNS_SPRINTF(HIGH, sns_fw_printf,
                 "Diag allocated island heap size %u mismatch with log size being freed %u",
                 diag_service.island_alloc_size, log_info_size);

      diag_service.island_alloc_size = 0;
    }
  }
  else
  {
    if(diag_service.main_alloc_size >= log_info_size)
    {
      diag_service.main_alloc_size -= log_info_size;
    }
    else
    {
      SNS_SPRINTF(ERROR, sns_fw_printf,
                 "Diag allocated main heap size %u mismatch with log size being freed %u",
                 diag_service.main_alloc_size, log_info_size);

      diag_service.main_alloc_size = 0;
    }
  }
  sns_free(log_info);
  sns_diag_attempt_reenable_island(&diag_service);
  sns_osa_lock_release(sns_diag_lock);
}

/**
 * Given a log ID, determine the appropriate field number.
 */
SNS_SECTION(".text.sns") static uint32_t
get_field_number(uint32_t log_id)
{
  if(log_id == SNS_LOG_CLIENT_API_REQUEST ||
     log_id == SNS_LOG_CLIENT_API_RESPONSE ||
     log_id == SNS_LOG_CLIENT_API_EVENT)
  {
    return sns_diag_sensor_log_client_api_payload_tag;
  }
  else if(log_id == SNS_LOG_SENSOR_API_REQUEST ||
          log_id == SNS_LOG_SENSOR_API_EVENT)
  {
    return sns_diag_sensor_log_sensor_api_payload_tag;
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_INTERNAL_INT ||
          log_id == SNS_LOG_SENSOR_STATE_INTERNAL_EXT ||
          log_id == SNS_LOG_QSOCKET_CLIENT_API)
  {
    return sns_diag_sensor_log_opaque_payload_tag;
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_RAW_INT ||
          log_id == SNS_LOG_SENSOR_STATE_RAW_EXT)
  {
    return sns_diag_sensor_log_sensor_state_raw_payload_tag;
  }
  else if(log_id == SNS_LOG_SENSOR_STATE_INTERRUPT)
  {
    return sns_diag_sensor_log_sensor_state_interrupt_payload_tag;
  }

  return 0;
}

SNS_SECTION(".text.sns") static sns_rc
sns_diag_create_sensor_log(sns_diag_log *log,
                           sns_diag_log_info *log_info,
                           uint32_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  size_t island_alloc_size = 0, payload_bytes_written = 0;
  uint32_t data_type_len = strlen(log_info->info.data_type);
  pb_buffer_arg sensor_type_args = (pb_buffer_arg)
    { .buf = log_info->info.data_type, .buf_len = data_type_len};
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)log->buffer,
                                               sizeof(log->buffer));
  sns_diag_sensor_log header = sns_diag_sensor_log_init_default;
  void *encoded_payload = NULL;

  sns_osa_lock_acquire(sns_diag_lock);
  island_alloc_size = diag_service.island_alloc_size;
  sns_osa_lock_release(sns_diag_lock);

  if(island_alloc_size + log_info->info.payload_encoded_size <= SNS_DIAG_MAX_ISLAND_ALLOC_SIZE)
  {
    encoded_payload =
      sns_malloc(SNS_HEAP_ISLAND, log_info->info.payload_encoded_size);
  }

  if(NULL == encoded_payload)
  {
    SNS_ISLAND_EXIT();
    encoded_payload =
      sns_malloc(SNS_HEAP_MAIN, log_info->info.payload_encoded_size);
  }

  if(NULL != encoded_payload)
  {
    if(NULL != log_info->info.payload_encode_cb)
    {
      if(!sns_island_is_island_ptr((intptr_t)log_info->info.payload_encode_cb))
      {
        SNS_ISLAND_EXIT();
      }
      rc = log_info->info.payload_encode_cb(
        log_info->payload, log_info->info.payload_size,
        log_info->info.payload_encoded_size, encoded_payload,
        &payload_bytes_written);
    }

    if(SNS_RC_SUCCESS == rc)
    {
      header.log_id = log_info->info.log_id;
      header.instance_id = log_info->info.instance_id;
      sns_memscpy(&header.suid,sizeof(header.suid),
                  &log_info->info.suid,
                  sizeof(log_info->info.suid));
      header.timestamp = log_info->info.log_time_stamp;
      header.sensor_type.funcs.encode = &pb_encode_string_cb;
      header.sensor_type.arg = &sensor_type_args;

      if(!pb_encode(&stream, sns_diag_sensor_log_fields, &header))
      {
        rc = SNS_RC_FAILED;
        SNS_SPRINTF(ERROR, sns_fw_printf,
            "Error encoding header for sensor log id:%u due to:%s",
            header.log_id, PB_GET_ERROR(&stream));
      }
      else
      {
        uint32_t field_number = get_field_number(header.log_id);

        if(!pb_encode_tag(&stream, PB_WT_STRING, field_number))
        {
          rc = SNS_RC_FAILED;
          SNS_SPRINTF(ERROR, sns_fw_printf,
              "Error encoding payload tag for sensor log id:%u due to %s",
              header.log_id, PB_GET_ERROR(&stream));
        }
        else
        {
          if (sns_diag_sensor_log_opaque_payload_tag == field_number)
          { //tag opaque log payload field properly
            uint64_t tag_size = sizeof(uint8_t);
            uint64_t payload_len = payload_bytes_written;
            uint8_t length_size = 0;
            do
            {
              payload_len >>= 7;
              length_size++;
            } while(payload_len);

            if(!pb_encode_varint(&stream, (payload_bytes_written + tag_size + length_size)))
            {
              rc = SNS_RC_FAILED;
              SNS_SPRINTF(ERROR, sns_fw_printf,
                  "Error encoding opaque_payload for sensor log id:%u due to %s",
                  header.log_id, PB_GET_ERROR(&stream));
            }
            else if(!pb_encode_tag(&stream, PB_WT_STRING,sns_diag_opaque_payload_payload_tag))
            {
              rc = SNS_RC_FAILED;
              SNS_SPRINTF(ERROR, sns_fw_printf,
                  "Error tagging opaque_payload_payload for sensor log id:%u due to %s",
                  header.log_id, PB_GET_ERROR(&stream));
            }
          }

          if(!pb_encode_string(&stream,(pb_byte_t*)encoded_payload,
                                    payload_bytes_written))
          {
            SNS_SPRINTF(ERROR, sns_fw_printf,
                "Error encoding payload for sensor log id:%u due to :%s",
                header.log_id, PB_GET_ERROR(&stream));
            rc = SNS_RC_FAILED;
          }
          else
          {
            *bytes_written = stream.bytes_written;
          }
        }
      }
    }
  }
  else
  {
    rc = SNS_RC_FAILED;
  }

  if(NULL != encoded_payload)
  {
    sns_free(encoded_payload);
  }

  return rc;
}

SNS_SECTION(".text.sns") static sns_rc
sns_diag_create_fw_log(sns_diag_log *log,
                       sns_diag_log_info *log_info,
                       uint32_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  size_t island_alloc_size = 0, payload_bytes_written = 0;
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)log->buffer,
                                               sizeof(log->buffer));
  sns_diag_fw_log header = sns_diag_fw_log_init_default;
  void *encoded_payload = NULL;

  sns_osa_lock_acquire(sns_diag_lock);
  island_alloc_size = diag_service.island_alloc_size;
  sns_osa_lock_release(sns_diag_lock);

  if(island_alloc_size + log_info->info.payload_encoded_size <= SNS_DIAG_MAX_ISLAND_ALLOC_SIZE)
  {
    encoded_payload = sns_malloc(SNS_HEAP_ISLAND,
                                 log_info->info.payload_encoded_size);
  }

  if(NULL == encoded_payload)
  {
    SNS_ISLAND_EXIT();
    encoded_payload = sns_malloc(SNS_HEAP_MAIN,
                                 log_info->info.payload_encoded_size);
  }

  if(NULL != encoded_payload)
  {
    if(NULL != log_info->info.payload_encode_cb)
    {
      if(!sns_island_is_island_ptr((intptr_t)log_info->info.payload_encode_cb))
      {
        SNS_ISLAND_EXIT();
      }

      rc = log_info->info.payload_encode_cb(
        log_info->payload, log_info->info.payload_size,
        log_info->info.payload_encoded_size, encoded_payload, &payload_bytes_written);
    }

    if(SNS_RC_SUCCESS == rc)
    {
      header.log_id = log_info->info.log_id;
      header.timestamp = log_info->info.log_time_stamp;

      if(!pb_encode(&stream, sns_diag_fw_log_fields, &header))
      {
        rc = SNS_RC_FAILED;
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding header for fw log id:%u due to:%s",
                   header.log_id, PB_GET_ERROR(&stream));
      }
      else
      {
        uint32_t field_number = 0;

        if(header.log_id == SNS_LOG_INSTANCE_MAP)
        {
          field_number =
            sns_diag_fw_log_instance_map_payload_tag;
        }
        else if(header.log_id == SNS_LOG_ISLAND_TRANSITION)
        {
          field_number =
            sns_diag_fw_log_island_mode_payload_tag;
        }
        else if(header.log_id == SNS_LOG_ISLAND_EXIT_VOTE)
        {
          field_number =
            sns_diag_fw_log_island_exit_vote_payload_tag;
        }
        else if(header.log_id == SNS_LOG_MEMORY_UTILIZATION)
        {
          field_number =
            sns_diag_fw_log_memory_utilization_payload_tag;
        }

        if(!pb_encode_tag(&stream, PB_WT_STRING, field_number))
        {
          SNS_SPRINTF(ERROR, sns_fw_printf,
                     "Error encoding fw log id:%u payload field tag due to %s",
                     header.log_id, PB_GET_ERROR(&stream));
          rc = SNS_RC_FAILED;
        }
        else if(!pb_encode_string(&stream,(pb_byte_t*)encoded_payload,
                                  payload_bytes_written))
        {
          SNS_SPRINTF(ERROR, sns_fw_printf,
                     "Error encoding fw log id:%u payload string due to %s",
                     header.log_id, PB_GET_ERROR(&stream));
          rc = SNS_RC_FAILED;
        }
        else
        {
          *bytes_written = stream.bytes_written;
        }
      }
    }
  }
  else
  {
    rc = SNS_RC_FAILED;
  }

  if(NULL != encoded_payload)
  {
    sns_free(encoded_payload);
  }

  return rc;
}


SNS_SECTION(".text.sns") static sns_rc
process_async_log(sns_fw_diag_service *this, sns_diag_async_log_item *log)
{
  if(NULL != log && NULL != log->payload_encode_cb && 0 < log->payload_size &&
     sns_diag_get_max_log_size(&this->service) > log->payload_size)
  {
    void *payload = sns_diag_log_alloc(log->payload_size, log->log_id);
    if(NULL != payload)
    {
      sns_memscpy(payload, log->payload_size, log->payload, sizeof(log->payload));
      return sns_diag_publish_fw_log(log->log_id,
                                     log->payload_size,
                                     payload,
                                     log->encoded_size,
                                     log->payload_encode_cb);
    }
  }
  return SNS_RC_FAILED;
}

/**
 * Upon notification of low memory, drop the oldest unprocessed events from
 * all data streams.
 */
static void
handle_low_mem(intptr_t arg)
{
  sns_isafe_list_iter iter;
  sns_fw_diag_service *this = (sns_fw_diag_service*)arg;
  UNUSED_VAR(this);

  SNS_PRINTF(HIGH, sns_fw_printf, "Main buffer low memory, dropping data");

  sns_osa_lock_acquire(sns_diag_lock);
  sns_isafe_list_iter_init(&iter, &sns_diag_list, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_diag_log_info *log_info = (sns_diag_log_info*)
      sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&iter));
    sns_free(log_info);
  }
  sns_osa_lock_release(sns_diag_lock);

  SNS_PRINTF(LOW, sns_fw_printf, "handle_low_mem complete");
}

/**
 *  Makes a copy of the datatype config in the specified heap
 *  and initializes it. Note: diag_service.data_type_list_lock
 *  must be held prior to calling this function
 *
 * @param[i] config    The datatype config that must be copied
 * @param[i] heap      The heap to copy the datatype config into
 *
 * @return sns_diag_datatype_config*
 */
static sns_diag_datatype_config *sns_diag_move_datatype_config(sns_diag_datatype_config *config, sns_mem_heap_id heap)
{
  sns_diag_datatype_config *new_config = (sns_diag_datatype_config *)sns_malloc(heap, sizeof(sns_diag_datatype_config));
  if(NULL != new_config)
  {
    sns_list_iter datatypes_iter;
    sns_strlcpy(new_config->datatype, config->datatype, SNS_SENSOR_DATATYPE_NAME_LEN);
    new_config->enabled = config->enabled;
    new_config->vprintf_fptr = config->vprintf_fptr;
    new_config->island_cnt = config->island_cnt;

    sns_list_item_init(&new_config->datatypes_list_entry, new_config);
    sns_list_item_init(&new_config->config_move_entry, new_config);

    sns_list_item_remove(&config->datatypes_list_entry);
    sns_list_iter_init(&datatypes_iter, &diag_service.datatypes, true);
    sns_list_iter_insert(&datatypes_iter, &new_config->datatypes_list_entry, false);
  }
  #ifdef SNS_DIAG_ENABLE_DEBUG
    SNS_SPRINTF(HIGH, sns_fw_printf, "Moved config %s from 0x"SNS_DIAG_PTR" to 0x"SNS_DIAG_PTR" ",
                config->datatype, config, new_config);
  #endif
  return new_config;
}

/**
 * Updates a sensor, all sibling sensors of the same library and
 * all instances spawned by these sensors to point to the new
 * datatype config.
 *
 * @param[i] sensor      A sensor that is known to point to the
 *                         old_config
 * @param[i] arg        The old and new datatype config
 */
static bool sns_diag_move_sensor(sns_fw_sensor *sensor, void *arg)
{
  sns_isafe_list_iter instances_iter;
  sns_isafe_list_item *instance_item;
  sns_diag_config_move_arg *move_arg = (sns_diag_config_move_arg *)arg;
  for(size_t i = 0; i < move_arg->array_len; i++)
  {
    sns_diag_datatype_config *old_config = move_arg->array[i].old;
    sns_diag_datatype_config *new_config = move_arg->array[i].new;

    // Update the sensor datatype config if it is pointing to old_config
    if(sensor->diag_config.config == old_config)
    {
      sensor->diag_config.config = new_config;
    }

    // Update all instances
    for(instance_item = sns_isafe_list_iter_init(&instances_iter, &sensor->sensor_instances, true);
        NULL != instance_item;
        instance_item = sns_isafe_list_iter_advance(&instances_iter))
    {
      sns_fw_sensor_instance *instance = (sns_fw_sensor_instance *)sns_isafe_list_item_get_data(instance_item);
      sns_diag_src_config *config = (sns_diag_src_config *)
        sns_cstruct_extn_get_buffer(&instance->extn, instance->diag_header.cstruct_enxtn_idx);

      for(uint8_t i = 0; i < instance->diag_header.datatypes_cnt; i++)
      {
        if(config[i].config == old_config)
        {
          config[i].config = new_config;
          #ifdef SNS_DIAG_ENABLE_DEBUG
            SNS_SPRINTF(HIGH, sns_fw_printf, "Moved Instance config %s  @ 0x"SNS_DIAG_PTR" from 0x"SNS_DIAG_PTR" to 0x"SNS_DIAG_PTR" ",
              config[i].config->datatype, &config[i], old_config, new_config);
          #endif
        }
      }
    }
  }
  return true;
}

/**
 *  Moves all datatype configs that have been queued in the
 *  diag_service.config_move_queue into or out of island
 *  memory.
 *
 */
SNS_SECTION(".text.sns") static void
sns_diag_handle_pending_datatype_updates()
{
  sns_list_iter iter;
  size_t count = 0;
  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  count = diag_service.config_move_queue.cnt;
  sns_osa_lock_release(diag_service.data_type_list_lock);

  if(0 < count)
  {
    sns_diag_config_move moves[count];
    sns_diag_config_move_arg arg;
    size_t moves_len = 0;

    // Step 1: Exit Island
    SNS_ISLAND_EXIT();

    // Step 2: Figure out all the config moves
    sns_osa_lock_acquire(diag_service.data_type_list_lock);
    for(sns_list_iter_init(&iter, &diag_service.config_move_queue, true);
        NULL != sns_list_iter_curr(&iter) && moves_len < count;
        sns_list_iter_advance(&iter))
    {
      sns_diag_datatype_config *old_config = (sns_diag_datatype_config *)sns_list_iter_get_curr_data(&iter);
      if(default_datatype != old_config)
      {
        sns_diag_datatype_config *new_config = sns_diag_move_datatype_config(old_config,
          (old_config->island_cnt)? SNS_HEAP_ISLAND: SNS_HEAP_MAIN);
        if(NULL != new_config)
        {
          moves[moves_len].old = old_config;
          moves[moves_len].new = new_config;
          moves_len += 1;
        }
      }
    }
    sns_osa_lock_release(diag_service.data_type_list_lock);

    // Step 3: Update all sensors
    arg.array_len = moves_len;
    arg.array = moves;
    if(0< moves_len)
    {
      sns_sensor_foreach(sns_diag_move_sensor, &arg);
    }

    // Step 4: Free up old configs
    sns_osa_lock_acquire(diag_service.data_type_list_lock);
    for(size_t i = 0; i < moves_len; i++)
    {
      sns_list_item_remove(&moves[i].old->config_move_entry);
      sns_free(moves[i].old);
    }
    sns_osa_lock_release(diag_service.data_type_list_lock);
  }
}

/**
 * Thread function for the Diag thread.
 *
 */
SNS_SECTION(".text.sns") static void
sns_diag_thread_func(sns_osa_thread_func_arg arg)
{
  UNUSED_VAR(arg);
  sns_osa_thread_active();
  while(1)
  {
    sns_isafe_list_iter iter;
    sns_diag_log_info *log_info;
    uint32_t list_size;

    sns_osa_lock_acquire(sns_diag_lock);
    list_size = sns_diag_list.cnt;
    sns_osa_lock_release(sns_diag_lock);

    if(list_size < SNS_DIAG_LIST_LEN_LOWER_THRESH)
    {
      sns_pwr_scpm_mgr_mcps_bump_down(diag_service.pwr_client);
    }
    sns_osa_thread_idle();
    sns_osa_sem_wait(sns_diag_sem);
    sns_osa_thread_active();

    // First consume all the async logs
    sns_diag_consume_async_logs(false);

    // Next, process the sns_diag_list
    sns_osa_lock_acquire(sns_diag_lock);
    sns_isafe_list_iter_init(&iter, &sns_diag_list, true);
    log_info = (sns_diag_log_info*)sns_isafe_list_iter_get_curr_data(&iter);
    if(NULL != log_info)
    {
      size_t island_alloc_size = 0;

      sns_isafe_list_iter_remove(&iter);
      island_alloc_size = diag_service.island_alloc_size;

      sns_rc rc = SNS_RC_SUCCESS;
      uint32_t bytes_written = 0;
      sns_diag_log *log = NULL;

      if(island_alloc_size + sizeof(sns_diag_log) <= SNS_DIAG_MAX_ISLAND_ALLOC_SIZE)
      {
        log = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_diag_log));
      }

      if(NULL == log)
      {
        SNS_ISLAND_EXIT();
        log = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_diag_log));
      }

      if(NULL == log)
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Unable to allocate log packet");
        diag_service.num_dropped_logs++;
      }
      sns_osa_lock_release(sns_diag_lock);

      if(NULL != log)
      {
        if(SNS_DIAG_LOG_TYPE_SENSOR == log_info->info.log_type)
        {
          rc = sns_diag_create_sensor_log(log, log_info, &bytes_written);
        }
        else
        {
          rc = sns_diag_create_fw_log(log, log_info, &bytes_written);
        }

        if(SNS_RC_SUCCESS == rc)
        {
          uint32_t length = sizeof(sns_log_hdr_type) + bytes_written;
          sns_log_set_code(log, log_info->info.diag_log_code);
          sns_log_set_length(log, length);
          sns_log_set_timestamp(log);
          if(!sns_log_submit(log))
          {
            diag_service.num_diag_dropped_logs++;
            SNS_PRINTF(HIGH, sns_fw_printf,"Diag packet code:%x Length:%d submission failed."
              "Number of packets diag layer dropped:%d",diag_service.num_diag_dropped_logs);
          }
        }

        sns_free(log);
      }

      sns_diag_log_free(log_info);
    }
    else
    {
      sns_osa_lock_release(sns_diag_lock);
    }
    sns_diag_handle_pending_datatype_updates();
  }
}

/**
 * Queue the log packet information for submission at a later
 * time on the diag thread.
 */
// Uncomment the variable (and its references) below to easily access the log that caused the ASSERT
//sns_diag_log_info *bad_log SNS_SECTION(".data.sns");
SNS_SECTION(".text.sns") static void
sns_diag_queue_log_info(sns_diag_log_info *log_info)
{
  sns_isafe_list_iter iter;
  uint32_t list_size;

  //bad_log = log_info;
  SNS_ASSERT(0xABCDABCD == log_info->info.sanity);
  SNS_ASSERT(SNS_HEAP_MAX > sns_memmgr_get_heap_id(log_info));

  log_info->info.log_time_stamp = sns_get_system_time();

  sns_osa_lock_acquire(sns_diag_lock);
  sns_isafe_list_iter_init(&iter, &sns_diag_list, false);
  sns_isafe_list_item_init(&log_info->info.list_entry, log_info);
  sns_isafe_list_iter_insert(&iter, &log_info->info.list_entry, true);
  list_size = sns_isafe_list_iter_len(&iter);
  sns_osa_lock_release(sns_diag_lock);

  if(list_size > SNS_DIAG_LIST_LEN_UPPER_THRESH)
  {
    sns_pwr_scpm_mgr_mcps_bump_up(diag_service.pwr_client);
  }
  sns_osa_sem_post(sns_diag_sem);
}


/*=============================================================================
  Public functions
  ===========================================================================*/

SNS_SECTION(".text.sns") size_t
sns_diag_get_preencoded_size(sns_fw_diag_enc_size type)
{
  size_t enc_size = 0;

  if(SNS_DIAG_ENC_SIZE_SENSOR_API_HDR == type)
  {
    enc_size = diag_service.encoded_sensor_api_hdr_size;
  }
  else if(SNS_DIAG_ENC_SIZE_CLIENT_API_HDR == type)
  {
    enc_size = diag_service.encoded_client_api_hdr_size;
  }
  else if(SNS_DIAG_ENC_SIZE_ISLAND_TRANSITION == type)
  {
    enc_size = diag_service.encoded_island_transition;
  }

  return enc_size;
}

SNS_SECTION(".text.sns") uint32_t
sns_diag_get_debug_ssid(char const *vendor)
{
  //#define MSG_SSID_SNS     53
  //#define MSG_SSID_SNS_FRAMEWORK  122
  //#define MSG_SSID_SNS_PLATFORM   123
  //#define MSG_SSID_SNS_SENSOR_INT 124
  //#define MSG_SSID_SNS_SENSOR_EXT 125

  //default ssid; assumed to be non-qualcomm
  uint32_t debug_ssid = MSG_SSID_SNS_SENSOR_EXT;

  if(0 == strcmp(vendor, "qualcomm") ||
     0 == strcmp(vendor, "qti_qualcomm"))
  {
    debug_ssid = MSG_SSID_SNS;
  }

  return debug_ssid;
}

void
sns_diag_register_sensor(sns_fw_sensor *sensor)
{
  char data_type[64];
  char vendor[32];

  sns_attr_info_get_data_type(sensor->attr_info,
      data_type, sizeof(data_type));
  sns_attr_info_get_vendor(sensor->attr_info,
      vendor, sizeof(vendor));

  SNS_SPRINTF(MED, sns_fw_printf,
      "Sensor %s Vendor:%s SSID:%u registered with diag",
      data_type, vendor, sns_diag_get_debug_ssid(vendor));

  sns_diag_datatype_config *config = sns_diag_find_datatype(data_type);
  if(NULL == config)
  {
    config = sns_diag_add_datatype(data_type, false);
  }
  SNS_ASSERT(NULL != config);

  sensor->diag_config.debug_ssid = sns_diag_get_debug_ssid(vendor);
  sensor->diag_config.suid = sensor->sensor.sensor_api->get_sensor_uid(&sensor->sensor);
  sensor->diag_config.config = config;
}

SNS_SECTION(".text.sns") uint32_t
sns_diag_get_max_log_size(sns_diag_service const *this)
{
  UNUSED_VAR(this);
  return sns_diag_get_sensor_max_log_size();
}

SNS_SECTION(".text.sns") sns_rc
sns_diag_publish_fw_log(sns_fw_diag_service_log_id  log_id,
                        uint32_t                    payload_size,
                        void                        *payload,
                        uint32_t                    payload_encoded_size,
                        sns_diag_encode_log_cb      payload_encode_cb)
{
  sns_rc rc = SNS_RC_FAILED;
  uint32_t diag_log_id = sns_diag_get_diag_log_id(log_id);
  sns_diag_log_info *log_info = (NULL == payload) ?
    NULL : sns_diag_get_log_info(payload);

  if(!sns_log_status(diag_log_id) ||
     payload_size > sns_diag_get_sensor_max_log_size() || NULL == payload)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    log_info->info.diag_log_code = diag_log_id;
    log_info->info.log_type = SNS_DIAG_LOG_TYPE_FW;
    log_info->info.payload_size = payload_size;
    log_info->info.payload_encoded_size = payload_encoded_size;
    log_info->info.payload_encode_cb = payload_encode_cb;

    log_info->info.log_id = log_id;

    sns_diag_queue_log_info(log_info);
    rc = SNS_RC_SUCCESS;
  }

  if(SNS_RC_SUCCESS != rc && NULL != log_info)
  {
    sns_diag_log_free(log_info);
  }

  return rc;
}

SNS_SECTION(".text.sns") sns_rc
sns_diag_publish_sensor_log(uintptr_t                    instance_id,
                            sns_sensor_uid const        *sensor_uid,
                            char const                  *data_type,
                            sns_fw_diag_service_log_id  log_id,
                            uint32_t                    payload_size,
                            void                        *payload,
                            uint32_t                    payload_encoded_size,
                            sns_diag_encode_log_cb      payload_encode_cb)
{
  sns_rc rc = SNS_RC_FAILED;
  uint32_t diag_log_id = sns_diag_get_diag_log_id(log_id);
  sns_diag_log_info *log_info = (NULL == payload) ?
    NULL : sns_diag_get_log_info(payload);

  if(NULL == sensor_uid)
  {
    sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance_id;
    sensor_uid = fw_instance->sensor->sensor.sensor_api->get_sensor_uid(&(fw_instance->sensor->sensor));
  }
  if(!sns_log_status(diag_log_id) ||
     NULL == sensor_uid || NULL == data_type ||
     payload_size > sns_diag_get_sensor_max_log_size() || NULL == payload)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    uint32_t data_type_len = strlen(data_type) + 1;

    log_info->info.diag_log_code = diag_log_id;
    log_info->info.log_type = SNS_DIAG_LOG_TYPE_SENSOR;
    sns_strlcpy(log_info->info.data_type, data_type, data_type_len);

    log_info->info.payload_size = payload_size;
    log_info->info.payload_encoded_size = payload_encoded_size;
    log_info->info.payload_encode_cb = payload_encode_cb;

    log_info->info.log_id = log_id;
    sns_memscpy(&log_info->info.suid,sizeof(log_info->info.suid),
                  sensor_uid,sizeof(sns_sensor_uid));
    log_info->info.instance_id = instance_id;
    sns_diag_queue_log_info(log_info);
    rc = SNS_RC_SUCCESS;
  }

  if(SNS_RC_SUCCESS != rc && NULL != log_info)
  {
    sns_diag_log_free(log_info);
  }

  return rc;
}

SNS_SECTION(".text.sns") sns_rc
sns_diag_submit_log(sns_diag_service            *this,
                    sns_sensor_instance         *instance,
                    sns_sensor_uid const        *sensor_uid,
                    uint32_t                    log_size,
                    void                        *log,
                    sns_diag_sensor_state_log   log_type,
                    uint32_t                    encoded_log_size,
                    sns_diag_encode_log_cb      encode_log_cb)
{
  UNUSED_VAR(this);
  sns_rc rc = SNS_RC_FAILED;
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_diag_src_config *diag_config =
    sns_diag_find_diag_config(fw_instance, sensor_uid);

  if(NULL == diag_config->config)
  {
    SNS_PRINTF(ERROR, &((sns_fw_sensor_instance*)instance)->sensor->sensor,
        "sns_diag_submit_log: Invalid client");
  }
  else if(NULL == log || log_type >= SNS_DIAG_SENSOR_STATE_LOG_NUM ||
     sns_diag_get_log_info(log) != diag_config->alloc_log[log_type])
  {
    SNS_PRINTF(ERROR, &((sns_fw_sensor_instance*)instance)->sensor->sensor,
        "sns_diag_submit_log: Invalid log packet");
  }
  else
  {
    if(!sns_island_is_island_ptr((intptr_t)diag_config->config))
    {
      SNS_ISLAND_EXIT();
    }
    char const *data_type = diag_config->config->datatype;
    sns_fw_diag_service_log_id log_id = sns_diag_get_sensor_state_log_id(
        log_type, diag_config->debug_ssid);

    rc = sns_diag_publish_sensor_log((intptr_t)fw_instance, sensor_uid,
        data_type, log_id, log_size, diag_config->alloc_log[log_type]->payload,
        encoded_log_size, encode_log_cb);

    diag_config->alloc_log[log_type] = NULL;
  }

  return rc;
}

SNS_SECTION(".text.sns") void*
sns_diag_alloc_log_for_instance(sns_diag_service          *this,
                              sns_sensor_instance       *instance,
                              sns_sensor_uid const      *sensor_uid,
                              uint32_t                  log_size,
                              sns_diag_sensor_state_log log_type)
{
  UNUSED_VAR(this);
  sns_diag_src_config *diag_config =
    sns_diag_find_diag_config((sns_fw_sensor_instance*)instance, sensor_uid);

  if(!sns_island_is_island_ptr((intptr_t)diag_config->config))
  {
    SNS_ISLAND_EXIT();
  }

  bool diag_check = diag_config->config->enabled;

  sns_fw_diag_service_log_id log_id = SNS_LOG_MAX_ID;
  void *log_ptr = NULL;

  if(diag_check && log_type < SNS_DIAG_SENSOR_STATE_LOG_NUM)
  {
    log_id = sns_diag_get_sensor_state_log_id(log_type,
        diag_config->debug_ssid);

    if(NULL != diag_config->alloc_log[log_type])
    {
      if(!sns_island_is_island_ptr((intptr_t)diag_config->config))
      {
        SNS_ISLAND_EXIT();
      }

      SNS_SPRINTF(ERROR, &((sns_fw_sensor_instance *)instance)->sensor->sensor,
        "Sensor instance for %s attempted to re-allocate a diag log of type %d",
        diag_config->config->datatype, log_type);
      sns_diag_log_free(diag_config->alloc_log[log_type]);
      diag_config->alloc_log[log_type] = NULL;
    }

    if(!sns_log_status(sns_diag_get_diag_log_id(log_id)))
    {
      return NULL;
    }

    log_ptr = sns_diag_log_alloc(log_size, log_id);
    if(NULL != log_ptr)
    {
      diag_config->alloc_log[log_type] = sns_diag_get_log_info(log_ptr);
    }
  }
  return log_ptr;
}


SNS_SECTION(".text.sns") void
sns_diag_sensor_sprintf(const sns_sensor *sensor,
                        sns_message_priority prio,
                        const char *file, uint32_t line,
                        const char *format, ...)
{
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)sensor;

  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state()
     && NULL != sensor)
  {
    sns_diag_vprintf_fptr vprintf_fptr =
      fw_sensor->diag_config.config->vprintf_fptr;

    if((sns_diag_vprintf_fptr)&sns_diag_vprintf_v2 == vprintf_fptr ||
       ((sns_diag_vprintf_fptr)&sns_diag_vprintf_v2_err_fatal_only == vprintf_fptr
        && SNS_ERROR == prio && SNS_FATAL == prio))
    {
      char buf[SNS_PRINT_MESSAGE_MAX_LEN];
      va_list args;
      va_start(args, format);
      vsnprintf(buf, SNS_PRINT_MESSAGE_MAX_LEN, format, args);
      va_end(args);
      sns_diag_print_buf(fw_sensor->diag_config.debug_ssid, prio, file, line, buf);
    }
  }
}

SNS_SECTION(".text.sns") void
sns_diag_sensor_printf_v2(sns_diag_service *this,
                          sns_sensor *sensor,
                          struct sns_msg_v2_const_type const *const_ptr,
                          uint32_t nargs,
                          ...)
{
  UNUSED_VAR(this);
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)sensor;

  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state()
     && sensor != NULL)
  {
    sns_diag_vprintf_fptr vprintf_fptr;
    va_list args;
    vprintf_fptr = fw_sensor->diag_config.config->vprintf_fptr;
    va_start(args, nargs);
    vprintf_fptr(sensor, NULL, const_ptr, nargs, args);
    va_end(args);
  }
}

SNS_SECTION(".text.sns") static void
sns_diag_sensor_printf(sns_diag_service *this,
                       sns_sensor *sensor,
                       sns_message_priority prio,
                       const char *file, uint32_t line, uint32_t nargs,
                       const char *format, ...)
{
  UNUSED_VAR(this);
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)sensor;

  if(NULL != fw_sensor &&
     NULL != fw_sensor->attr_info &&
     SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    bool diag_check = fw_sensor->diag_config.config->enabled ||
        !sns_attr_info_get_available(fw_sensor->attr_info);
    if(diag_check || prio == SNS_ERROR || prio == SNS_FATAL)
    {
      va_list args;
      va_start(args, format);
      sns_diag_vprintf(fw_sensor->diag_config.debug_ssid, prio, file, line,
        nargs, format, args);
      va_end(args);
    }
  }
}

SNS_SECTION(".text.sns") static void
sns_diag_sensor_inst_printf(sns_diag_service *this,
                            sns_sensor_instance *instance,
                            sns_message_priority prio,
                            const char *file, uint32_t line, uint32_t nargs,
                            const char *format, ...)
{
  UNUSED_VAR(this);
  bool diag_check = false;
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance *)instance;
  sns_diag_src_config *config = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&fw_instance->extn, fw_instance->diag_header.cstruct_enxtn_idx);

  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    // Enable PRINTF if any parent sensor has enabled diag
    for(uint8_t i = 0; i < fw_instance->diag_header.datatypes_cnt; i++)
    {
      if(config[i].config->enabled)
      {
        diag_check = true;
        break;
      }
    }
    if(diag_check || prio == SNS_ERROR || prio == SNS_FATAL)
    {
      va_list args;
      va_start(args, format);
      sns_diag_vprintf(config->debug_ssid, prio, file, line,
                              nargs, format, args);
      va_end(args);
    }
  }
}

SNS_SECTION(".text.sns") void
sns_diag_sensor_inst_printf_v2(sns_diag_service *this,
                               sns_sensor_instance *instance,
                               struct sns_msg_v2_const_type const *const_ptr,
                               uint32_t nargs,
                               ...)
{
  UNUSED_VAR(this);
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_diag_src_config *config = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&fw_instance->extn, fw_instance->diag_header.cstruct_enxtn_idx);

  if(SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    sns_diag_vprintf_fptr vprintf_fptr;
    va_list args;
    vprintf_fptr = config->config->vprintf_fptr;
    va_start(args, nargs);
    vprintf_fptr(NULL, instance, const_ptr, nargs, args);
    va_end(args);
  }
}

void
sns_diag_set_filter_all(bool enable)
{
  sns_list_iter iter;
  sns_list_item *diag_config_item;

  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  for(diag_config_item = sns_list_iter_init(&iter, &diag_service.datatypes, true);
      NULL != diag_config_item;
      diag_config_item = sns_list_iter_advance(&iter))
  {
    sns_diag_datatype_config *curr = (sns_diag_datatype_config *)sns_list_item_get_data(diag_config_item);
    curr->enabled = enable;
  }
  sns_osa_lock_release(diag_service.data_type_list_lock);
}

void
sns_diag_set_filter(const char* data_type,
                    bool enable)
{
  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  sns_diag_datatype_config *config = sns_diag_find_datatype(data_type);
  if(NULL == config)
  {
    config = sns_diag_add_datatype(data_type, false);
  }
  config->enabled = enable;
  config->vprintf_fptr = enable ? sns_diag_vprintf_v2 :
    sns_diag_vprintf_v2_err_fatal_only;

  sns_osa_lock_release(diag_service.data_type_list_lock);
}

SNS_SECTION(".text.sns") static void
sns_diag_mark_island_datatype_for_instance(sns_fw_sensor_instance *instance)
{
  sns_diag_src_config *config = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&instance->extn, instance->diag_header.cstruct_enxtn_idx);
  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  for(uint8_t i = 0; i < instance->diag_header.datatypes_cnt; i++)
  {
    if(!sns_island_is_island_ptr((intptr_t)config[i].config))
    {
      SNS_ISLAND_EXIT();
    }
    config[i].config->island_cnt += 1;
    if(!sns_island_is_island_ptr((intptr_t)config[i].config) &&
       !config[i].config->disable_moving &&
       !sns_list_item_present(&config[i].config->config_move_entry))
    {
      sns_list_iter iter;
      sns_list_iter_init(&iter, &diag_service.config_move_queue, true);
      sns_list_iter_insert(&iter, &config[i].config->config_move_entry, false);
      sns_osa_sem_post(sns_diag_sem);
    }
  }
  sns_osa_lock_release(diag_service.data_type_list_lock);
}

SNS_SECTION(".text.sns") static void
sns_diag_unmark_island_datatype_for_instance(sns_fw_sensor_instance *instance)
{
  sns_diag_src_config *config = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&instance->extn, instance->diag_header.cstruct_enxtn_idx);
  sns_osa_lock_acquire(diag_service.data_type_list_lock);
  for(uint8_t i = 0; i < instance->diag_header.datatypes_cnt; i++)
  {
    if(!sns_island_is_island_ptr((intptr_t)config[i].config))
    {
      SNS_ISLAND_EXIT();
    }
    if(config[i].config->island_cnt > 0)
    {
      config[i].config->island_cnt -= 1;
      if(0 == config[i].config->island_cnt &&
         !config[i].config->disable_moving &&
         sns_island_is_island_ptr((intptr_t)config[i].config) &&
         !sns_list_item_present(&config[i].config->config_move_entry))
      {
        sns_list_iter iter;
        sns_list_iter_init(&iter, &diag_service.config_move_queue, true);
        sns_list_iter_insert(&iter, &config[i].config->config_move_entry, false);
        sns_osa_sem_post(sns_diag_sem);
      }
    }
  }
  sns_osa_lock_release(diag_service.data_type_list_lock);
}

static SNS_SECTION(".text.sns") void
sns_diag_service_island_exit_cb(intptr_t args)
{
  UNUSED_VAR(args);
}

sns_fw_diag_service*
sns_diag_service_init(void)
{
  sns_rc rc;
  sns_osa_thread_attr attr;
  sns_osa_lock_attr lock_attr;
  sns_osa_sem_attr sem_attr;

  rc = sns_osa_lock_attr_init(&lock_attr);
  rc |= sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_lock_create(&lock_attr, &sns_diag_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_sem_attr_init(&sem_attr);
  rc |= sns_osa_sem_attr_set_memory_partition(&sem_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_sem_create(&sem_attr, &sns_diag_sem);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_thread_attr_init(&attr);
  rc |= sns_osa_thread_attr_set_name(&attr, "SNS_DIAG");
  rc |= sns_osa_thread_attr_set_stack(&attr, (uintptr_t)NULL, 4096);
  rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_thread_attr_set_priority(&attr, SNS_DIAG_THREAD_PRIO);

  if(SNS_RC_SUCCESS == rc)
  {
    rc = sns_osa_thread_create(&sns_diag_thread_func, NULL, &attr, &sns_diag_thread);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
    sns_island_register_island_exit_cb(sns_diag_service_island_exit_cb, (intptr_t )sns_diag_thread);
  }

  sns_isafe_list_init(&sns_diag_list);
  diag_service_api.struct_len = sizeof(diag_service_api);
  diag_service_api.get_max_log_size = &sns_diag_get_max_log_size;
  diag_service_api.alloc_log = &sns_diag_alloc_log_for_instance;
  diag_service_api.submit_log = &sns_diag_submit_log;
  diag_service_api.sensor_printf = &sns_diag_sensor_printf;
  diag_service_api.sensor_inst_printf = &sns_diag_sensor_inst_printf;
  diag_service_api.sensor_printf_v2 = &sns_diag_sensor_printf_v2;
  diag_service_api.sensor_inst_printf_v2 = &sns_diag_sensor_inst_printf_v2;

  diag_service.service.service.version = SNS_DIAG_SERVICE_VERSION;
  diag_service.service.service.type = SNS_DIAG_SERVICE;
  diag_service.service.api = &diag_service_api;

  diag_service.num_dropped_logs = 0;
  diag_service.num_diag_dropped_logs = 0;

  sns_memzero(&diag_service.circ_buffer, sizeof(sns_diag_async_log_item)*SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE);
  diag_service.circ_buffer_read_idx = 0;
  diag_service.circ_buffer_write_idx = 0;
  diag_service.circ_buffer_pending_cnt = 0;
  rc = sns_osa_lock_attr_init(&lock_attr);
  rc |= sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_lock_create(&lock_attr, &diag_service.circ_buffer_idx_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_diag_sensor_api_log sensor_api_hdr =
    sns_diag_sensor_api_log_init_default;
  pb_get_encoded_size(&diag_service.encoded_sensor_api_hdr_size,
                      sns_diag_sensor_api_log_fields,
                      &sensor_api_hdr);

  // PEND: Estimate max size correctly
  diag_service.encoded_sensor_api_hdr_size += 100;

  sns_diag_client_api_log client_api_hdr =
    sns_diag_client_api_log_init_default;
  pb_get_encoded_size(&diag_service.encoded_client_api_hdr_size,
                      sns_diag_client_api_log_fields,
                      &client_api_hdr);

  // PEND: Estimate max size correctly
  diag_service.encoded_client_api_hdr_size += 100;

  sns_diag_island_transition_log island_transition =
    sns_diag_island_transition_log_init_default;
  pb_get_encoded_size(&diag_service.encoded_island_transition,
                      sns_diag_island_transition_log_fields,
                      &island_transition);

  diag_service.island_client =
    sns_island_aggregator_register_client("diag service");

  sns_mem_heap_register_lowmem_cb(SNS_HEAP_MAIN, SNS_DIAG_SERVICE_BUFFER_LOW_MEM,
    handle_low_mem, (intptr_t)&diag_service);

  diag_service.pwr_client = sns_pwr_scpm_mgr_create_client();
  return &diag_service;
}

SNS_SECTION(".text.sns") sns_rc
sns_diag_sensor_instance_init(sns_fw_sensor_instance *instance,
                              struct sns_fw_sensor *this)
{
  sns_sensor* sensor = (sns_sensor*)this;
  sns_diag_src_config *config = (sns_diag_src_config *)sns_cstruct_extn_get_buffer(&instance->extn,
    instance->diag_header.cstruct_enxtn_idx);
  sns_fw_sensor* fw_sensor;

  uint8_t i = 0;
  for(fw_sensor = (sns_fw_sensor*)sensor->cb->get_library_sensor(sensor, true);
      fw_sensor != NULL && i < instance->diag_header.datatypes_cnt;
      fw_sensor = (sns_fw_sensor*)sensor->cb->get_library_sensor(sensor, false))
  {
    sns_memscpy(&config[i], sizeof(*config), &fw_sensor->diag_config, sizeof(sns_diag_src_config));
    for (uint8_t j = 0; j < SNS_DIAG_SENSOR_STATE_LOG_NUM; j++)
    {
      config[i].alloc_log[j] = NULL;
    }
    i += 1;
  }

  if(SNS_ISLAND_STATE_NOT_IN_ISLAND != instance->island_operation)
  {
    sns_diag_mark_island_datatype_for_instance(instance);
  }

  return SNS_RC_SUCCESS;
}

/* Find the debug configuration corresponding to a specific sensor UID
   for the instance*/
SNS_SECTION(".text.sns") sns_diag_src_config*
sns_diag_find_diag_config(
  sns_fw_sensor_instance *instance, sns_sensor_uid const* sensor_uid)
{
  uint8_t i = 0;
  sns_diag_src_config *ret_val = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&instance->extn, instance->diag_header.cstruct_enxtn_idx);
  SNS_ASSERT(NULL != ret_val);
  if(NULL != sensor_uid)
  {
    for(i = 0; i < instance->diag_header.datatypes_cnt; i++)
    {
       if(sns_sensor_uid_compare(sensor_uid, ret_val[i].suid))
      {
         break;
      }
    }
  }
  return &ret_val[i];
}

SNS_SECTION(".text.sns") void
sns_diag_sensor_instance_deinit(sns_fw_sensor_instance *instance)
{
  sns_diag_src_config *config = (sns_diag_src_config *)
    sns_cstruct_extn_get_buffer(&instance->extn, instance->diag_header.cstruct_enxtn_idx);

  for(uint8_t i = 0; i < instance->diag_header.datatypes_cnt; i++)
  {
    for(int8_t j = 0; j < SNS_DIAG_SENSOR_STATE_LOG_NUM; j++)
    {
      if(NULL != config[i].alloc_log[j])
      {
        sns_diag_log_free(config[i].alloc_log[j]);
      }
    }
  }

    if(SNS_ISLAND_STATE_NOT_IN_ISLAND != instance->island_operation)
  {
    sns_diag_unmark_island_datatype_for_instance(instance);
  }

}

sns_rc
sns_diag_submit_async_log(sns_diag_async_log_item log)
{
  sns_fw_diag_service *this = &diag_service;
  sns_osa_lock_acquire(this->circ_buffer_idx_lock);
  sns_memscpy(&this->circ_buffer[this->circ_buffer_write_idx],
              sizeof(sns_diag_async_log_item),
              &log,
              sizeof(sns_diag_async_log_item));
  this->circ_buffer_pending_cnt += 1;
  this->circ_buffer_write_idx = (this->circ_buffer_write_idx + 1)
    % SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE;
  if(this->circ_buffer_read_idx == this->circ_buffer_write_idx)
  {
    this->circ_buffer_read_idx = (this->circ_buffer_read_idx + 1)
      % SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE;
  }
  sns_osa_lock_release(this->circ_buffer_idx_lock);
  sns_osa_sem_post(sns_diag_sem);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") void
sns_diag_consume_async_logs(bool force)
{
  sns_fw_diag_service *this = &diag_service;
  bool done = false;

  sns_osa_lock_acquire(this->circ_buffer_idx_lock);
  bool consume_logs = force ||
    (this->circ_buffer_pending_cnt >= SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_MIN_EVENTS_TO_DRAIN );
  sns_osa_lock_release(this->circ_buffer_idx_lock);

  sns_diag_async_log_item local_copy;
  do
  {
    sns_osa_lock_acquire(this->circ_buffer_idx_lock);
    bool process_log = false;
    if(this->circ_buffer_read_idx != this->circ_buffer_write_idx && consume_logs)
    {
      sns_memscpy(&local_copy,
                  sizeof(sns_diag_async_log_item),
                  &this->circ_buffer[this->circ_buffer_read_idx],
                  sizeof(sns_diag_async_log_item));
      this->circ_buffer_read_idx = (this->circ_buffer_read_idx + 1) % SNS_DIAG_ASYNC_LOG_CIRC_BUFFER_SIZE;
      this->circ_buffer_pending_cnt -= 1;
      process_log =  true;
      done = false;
    }
    else
    {
      done = true;
    }
    sns_osa_lock_release(this->circ_buffer_idx_lock);
    if(process_log)
    {
      process_async_log(this, &local_copy);
    }
  }
  while(!done);
}

void sns_diag_fw_preinit()
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)sns_fw_printf;
  sns_rc rc = SNS_RC_SUCCESS;

  sns_osa_lock_attr lock_attr;
  sns_list_init(&diag_service.datatypes);
  sns_list_init(&diag_service.config_move_queue);

  rc = sns_osa_lock_attr_init(&lock_attr);
  rc |= sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_lock_create(&lock_attr, &diag_service.data_type_list_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  // Add island-mode platform sensors into island mode

  // Framework Sensor
  sns_diag_datatype_config *config = sns_diag_add_datatype("framework", true);
  sensor->diag_config.config = config;
  sensor->diag_config.debug_ssid = MSG_SSID_SNS;
  sensor->diag_config.suid = &fw_suid;

  // Timer sensor
  config = sns_diag_add_datatype("timer", true);
  config->enabled = false;

  // Interrupt sensor
  config = sns_diag_add_datatype("interrupt", true);
  config->enabled = false;

  // ASCP sensor
  config = sns_diag_add_datatype("async_com_port", true);
  config->enabled = false;

  // Default sensor
  config = sns_diag_add_datatype("unknown", true);
  config->enabled = true;
  default_datatype = config;
}

void
sns_diag_svc_sensor_deinit(sns_fw_sensor *sensor)
{
  UNUSED_VAR(sensor);
}


