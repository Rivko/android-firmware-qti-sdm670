#pragma once
/*=============================================================================
  @file sns_fw_diag_types.h

  Type definitions for diag service

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "sns_cstruct_extn.h"
#include "sns_diag.pb.h"
#include "sns_diag_print.h"
#include "sns_isafe_list.h"
#include "sns_list.h"
#include "sns_log.h"
#include "sns_time.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/
#define SNS_SENSOR_DATATYPE_NAME_LEN 32

// Maximum number of supported sensor state log packets
#define SNS_DIAG_SENSOR_STATE_LOG_NUM 3

// Length of a uint64_t buffer allocated for the log packet payload.
#define SNS_LOG_PKT_UINT64_BUFF_SIZE (200)

// Length of a buffer used for a log in sns_diag_async_log_item
#define SNS_DIAG_ASYNC_CIRC_BUFFER_SIZE 128

#define SNS_DIAG_SERVICE_VERSION 2

/*=============================================================================
  Type definitions
  ===========================================================================*/

struct sns_sensor_uid;

/**
 * Supported Log Packet Types in the diag service
 */
typedef enum
{
  SNS_LOG_CLIENT_API_REQUEST = 0,
  SNS_LOG_CLIENT_API_RESPONSE = 1,
  SNS_LOG_CLIENT_API_EVENT = 2,
  SNS_LOG_SENSOR_API_REQUEST = 3,
  SNS_LOG_SENSOR_API_EVENT = 4,
  SNS_LOG_SENSOR_STATE_RAW_INT = 5,
  SNS_LOG_SENSOR_STATE_INTERNAL_INT = 6,
  SNS_LOG_SENSOR_STATE_RAW_EXT = 7,
  SNS_LOG_SENSOR_STATE_INTERNAL_EXT = 8,
  SNS_LOG_SENSOR_STATE_INTERRUPT = 9,
  SNS_LOG_INSTANCE_MAP = 10,
  SNS_LOG_ISLAND_TRANSITION = 11,
  SNS_LOG_ISLAND_EXIT_VOTE = 12,
  SNS_LOG_MEMORY_UTILIZATION = 13,
  SNS_LOG_QSOCKET_CLIENT_API = 14,
  SNS_LOG_MAX_ID
} sns_fw_diag_service_log_id;

typedef enum
{
  SNS_DIAG_ENC_SIZE_SENSOR_API_HDR = 0,
  SNS_DIAG_ENC_SIZE_CLIENT_API_HDR = 1,
  SNS_DIAG_ENC_SIZE_ISLAND_TRANSITION = 2,
} sns_fw_diag_enc_size;


typedef SNS_PACK(struct)
{
  sns_log_hdr_type log_hdr;
  uint64_t buffer[SNS_LOG_PKT_UINT64_BUFF_SIZE];
} sns_diag_log;


typedef enum
{
  SNS_DIAG_LOG_TYPE_SENSOR = 0,
  SNS_DIAG_LOG_TYPE_FW = 1
} sns_diag_log_type;

/* Unencoded diag log header, stores all the information
   necessary to create the final encoded log packet
   before submission to Core diag*/
typedef struct sns_diag_log_info
{
  struct
  {
    sns_std_suid suid;
    uint64_t log_time_stamp;
    uint64_t instance_id;
    char data_type[SNS_SENSOR_DATATYPE_NAME_LEN];
    uint32_t diag_log_code;
    uint32_t log_id;
    uint32_t payload_alloc_size; // allocated payload size
    uint32_t payload_size; // actual payload size
    uint32_t payload_encoded_size;
    uint32_t sanity;
    sns_isafe_list_item list_entry; // sns_diag_list 
    sns_diag_encode_log_cb payload_encode_cb;
    sns_diag_log_type log_type;
    // PEND: Add another field that functions as a log packet counter. It'll
    //       help us determine if log packets are getting dropped and how many
    // uint64_t counter;
  } info;
  uint8_t payload[]; // unencoded payload
}sns_diag_log_info;

typedef struct
{
 /* Log data. Data stored here could be either the unencoded log packet message
  *  or any suitable data from which the encoded log packet can be generated.
  */
  uint8_t payload[SNS_DIAG_ASYNC_CIRC_BUFFER_SIZE];
  /* size of payload
  */
  size_t payload_size;
  /* Size of the encoded payload
   */
  size_t encoded_size;
  /* Callback to encode the payload
   */
  sns_diag_encode_log_cb payload_encode_cb;
  /* Timestamp of the log packet
   */
  sns_time timestamp;
  /* Log ID
   */
  sns_fw_diag_service_log_id log_id;
} sns_diag_async_log_item;

/* Diag filter settings for a datatype */
typedef struct
{
  // Data type
  char datatype[SNS_SENSOR_DATATYPE_NAME_LEN];

  /* Entry into the list of known data types */
  sns_list_item datatypes_list_entry;

  /* Entry into the list of configs to be moved */
  sns_list_item config_move_entry;

  /* Print function used for this data type */
  sns_diag_vprintf_fptr vprintf_fptr;

  /* Number of Island instances that support this datatype */
  uint8_t island_cnt;

  /* If diag features enabled for this datatype */
  bool enabled;

  /* If this datatype be moved */
  bool disable_moving;
}sns_diag_datatype_config;

/* Diag filter settings for a source of diag logs and F3 messages */
typedef struct
{
  /* SUID of this source */
  const struct sns_sensor_uid *suid;

  /* Datatype config that maps to the above SUID */
  sns_diag_datatype_config *config;

  /* Debug SSID used by thsi source */
  uint32_t debug_ssid;

  /* Log Packets allocated for an instance */
  struct sns_diag_log_info *alloc_log[SNS_DIAG_SENSOR_STATE_LOG_NUM];
}sns_diag_src_config;

/* Diag meta information stored in a sensor instance */
typedef struct
{
  /* Number of datatypes supported by instance */
  uint8_t datatypes_cnt;
  /* Index used to obtain the array of source configs */
  uint8_t cstruct_enxtn_idx;
}sns_diag_src_instance;

