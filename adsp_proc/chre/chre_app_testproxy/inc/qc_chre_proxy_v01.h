#ifndef QCP_SVC_SERVICE_01_H
#define QCP_SVC_SERVICE_01_H
/**
  @file qc_chre_proxy_v01.h

  @brief This is the public header file which defines the QCP_SVC service Data structures.

  This header file defines the types and structures that were defined in
  QCP_SVC. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/chre.dsp/1.2.0/chre_app_testproxy/inc/qc_chre_proxy_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Mar  2 2017 (Spin 0)
   From IDL File: qc_chre_proxy_v01.idl */

/** @defgroup QCP_SVC_qmi_consts Constant values defined in the IDL */
/** @defgroup QCP_SVC_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup QCP_SVC_qmi_enums Enumerated types used in QMI messages */
/** @defgroup QCP_SVC_qmi_messages Structures sent as QMI messages */
/** @defgroup QCP_SVC_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup QCP_SVC_qmi_accessor Accessor for QMI service object */
/** @defgroup QCP_SVC_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup QCP_SVC_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define QCP_SVC_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define QCP_SVC_V01_IDL_MINOR_VERS 0x01
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define QCP_SVC_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define QCP_SVC_V01_MAX_MESSAGE_ID 0x000D
/**
    @}
  */


/** @addtogroup QCP_SVC_qmi_consts
    @{
  */

/**  Max number of Hubs on the device  */
#define QCP_MAX_HUBS_V01 32

/**  Max number of nano apps on the Hub :ta */
#define QCP_MAX_NANOAPPS_V01 64

/**  Max number of nano apps on the Hub  */
#define QCP_MAX_MEMORY_RANGES_V01 16

/**  Max length of log file name  */
#define QCP_MAX_FILENAME_LEN_V01 256

/**  Max length of text sent to log file  */
#define QCP_LOG_TEXT_MAX_LEN_V01 128

/**  Max length of sensor type name  */
#define QCP_SENSOR_TYPE_NAME_MAX_LEN_V01 32

/**  Max length of sensor name  */
#define QCP_SENSOR_NAME_MAX_LEN_V01 64

/**  Max number of ODRs  */
#define QCP_MAX_ODRS_V01 32

/**  Same as SMGR buffering report max samples  */
#define QCP_MAX_SAMPLES_PER_EVENT_V01 100

/**  Max length of sensor's vendor name  */
#define QCP_SENSOR_VENDOR_NAME_MAX_LEN_V01 64

/**  Max length of Hub name  */
#define QCP_HUB_NAME_MAX_LEN_V01 64

/**  The size of the opaque req, resp and event payload size  */
#define QCP_OPAQUE_PAYLOAD_SIZE_V01 2048

/**  Max length of Hub name  */
#define QCP_HUB_MSG_MAX_LEN_V01 65000

/**  Max length of Hub's vendor name  */
#define QCP_HUB_VENDOR_NAME_MAX_LEN_V01 64

/**  Max length of Tool chain used to compile binaries  */
#define QCP_HUB_TOOLCHAIN_NAME_MAX_LEN_V01 64
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE_V01 0x10000

/**  from context_hub.h 

 * Other memory types (likely not writeable, informational only)
  */
#define QCP_HUB_MEM_TYPE_BOOTLOADER_V01 0xFFFFFFFFul
#define QCP_HUB_MEM_TYPE_OS_V01 0xFFFFFFFEul
#define QCP_HUB_MEM_TYPE_EEDATA_V01 0xFFFFFFFDul
#define QCP_HUB_MEM_TYPE_RAM_V01 0xFFFFFFFCul

/** 
 * Types of memory blocks on the context hub
 * 

 Memory can be read from  */
#define QCP_MEM_FLAG_READ_V01 0x1

/**  Memory can be written to  */
#define QCP_MEM_FLAG_WRITE_V01 0x2

/**  Memory can be executed from  */
#define QCP_MEM_FLAG_EXEC_V01 0x4
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_RC_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCP_RC_SUCCESS_V01 = 0, /**<  success  */
  QCP_RC_FAILED_V01 = 1, /**<  generic error  */
  QCP_RC_FIRST_FOPEN_V01 = 2, /**<  error opening first instance of log file  */
  QCP_RC_SECONDARY_FOPEN_V01 = 3, /**<  only one open log file is allowed at any instance. Return if there is
      a request to open secondary file instance  */
  QCP_RC_EMEMORY_V01 = 4, /**<  memory allocation failed  */
  QCP_RC_TIMEOUT_V01 = 5, /**<  time out waiting for a response from context hub  */
  QCP_RC_INVALID_MSG_ID_V01 = 6, 
  QCP_RC_INVALID_MSG_TYPE_V01 = 7, 
  QCP_RC_INVALID_HUB_ID_V01 = 8, 
  QCP_RC_INVALID_APP_ID_V01 = 9, 
  QCP_RC_INVALID_SENSOR_ID_V01 = 10, 
  QCP_RC_INVALID_NAPP_ARGS_V01 = 11, 
  QCP_RC_PAYLOAD_TRUCATED_V01 = 12, 
  QCP_RC_PAYLOAD_TOO_LONG_V01 = 13, 
  QCP_RC_DIAG_ERROR_V01 = 0x100, /**<  Codes used by qcp diag  */
  QCP_RC_DIAG_HEADER_VERSION_V01 = 0x101, /**<  Invalid qcp diag header version  */
  QCP_RC_DIAG_HEADER_MSG_ID_V01 = 0x102, /**<  Invalid msg id  */
  QCP_RC_DIAG_HEADER_MSG_TYPE_V01 = 0x103, /**<  Invalid qcp diag header version  */
  QCP_RC_DIAG_HEADER_PAYLOAD_LEN_V01 = 0x104, /**<  Invalid qcp diag payload len  */
  QCP_RC_DIAG_QMI_ENCODE_V01 = 0x105, /**<  Error doing QMI encode of response message  */
  QCP_RC_DIAG_QMI_DECODE_V01 = 0x106, /**<  Error doing QMI decode of request diag packet  */
  QCP_RC_DIAG_EMEMORY_V01 = 0x107, 
  QCP_RC_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_rc_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_CONTEXT_SENSOR_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CONTEXT_SENSOR_RESERVED_V01 = 0, 
  CONTEXT_SENSOR_ACCELEROMETER_V01 = 1, 
  CONTEXT_SENSOR_GYROSCOPE_V01 = 2, 
  CONTEXT_SENSOR_MAGNETOMETER_V01 = 3, 
  CONTEXT_SENSOR_BAROMETER_V01 = 4, 
  CONTEXT_SENSOR_PROXIMITY_SENSOR_V01 = 5, 
  CONTEXT_SENSOR_AMBIENT_LIGHT_SENSOR_V01 = 6, 
  CONTEXT_SENSOR_GPS_V01 = 0x100, /**<  Reserving this space for variants on GPS  */
  CONTEXT_SENSOR_WIFI_V01 = 0x200, /**<  Reserving this space for variants on WIFI  */
  CONTEXT_SENSOR_AUDIO_V01 = 0x300, /**<  Reserving this space for variants on Audio  */
  CONTEXT_SENSOR_CAMERA_V01 = 0x400, /**<  Reserving this space for variants on Camera  */
  CONTEXT_SENSOR_BLE_V01 = 0x500, 
  CONTEXT_SENSOR_MAX_V01 = 0x0fffffff, /**<  make sure enum size is set  */
  QCP_CONTEXT_SENSOR_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_context_sensor_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_APPS_POWER_COLLAPSE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QCP_APPS_POWER_COLLAPSE_DISABLE_V01 = 1, /**<  Do not allow application processor to power collapse (keep continuously wake lock)  */
  QCP_APPS_POWER_COLLAPSE_ENABLE_V01 = 2, /**<  Allow application processor to power collapse  */
  QCP_APPS_POWER_COLLAPSE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_apps_power_collapse_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_HUB_MESSAGES_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  CONTEXT_HUB_APPS_ENABLE_V01 = 1, /**<  Enables loaded nano-app(s)  */
  CONTEXT_HUB_APPS_DISABLE_V01 = 2, /**<  Disables loaded nano-app(s)  */
  CONTEXT_HUB_LOAD_APP_V01 = 3, /**<  Load a supplied app  */
  CONTEXT_HUB_UNLOAD_APP_V01 = 4, /**<  Unload a specified app. Before the unload call APPS_DISABLE method  */
  CONTEXT_HUB_QUERY_APPS_V01 = 5, /**<  Query for app(s) info on hub  */
  CONTEXT_HUB_QUERY_MEMORY_V01 = 6, /**<  Query for memory info  */
  QCP_HUB_MESSAGES_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_hub_messages_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  NAPP_CMD_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  NAPP_CMD_RESET_TESTAPP_V01 = 0x10001, 
  NAPP_CMD_GET_TESTAPP_INFO_V01 = 0x10002, 
  NAPP_CMD_EVENT_ENABLE_V01 = 0x10003, 
  NAPP_CMD_EVENT_DISABLE_V01 = 0x10004, 
  NAPP_CMD_SENS_FIND_V01 = 0x10005, 
  NAPP_CMD_SENS_REQUEST_V01 = 0x10006, 
  NAPP_CMD_SENS_EVENT_V01 = 0x10007, 
  NAPP_CMD_SENS_REQ_RATE_CHANGE_V01 = 0x10008, 
  NAPP_CMD_SENS_GET_CUR_RATE_V01 = 0x10009, 
  NAPP_CMD_SENS_RELEASE_V01 = 0x1000A, 
  NAPP_CMD_SENS_TRIG_ON_DEMAND_V01 = 0x1000B, 
  NAPP_CMD_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}napp_cmd_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_SENSTYPE_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  SENS_TYPE_INVALID_V01 = 0, 
  SENS_TYPE_ACCEL_V01 = 1, 
  SENS_TYPE_ANY_MOTION_V01 = 2, 
  SENS_TYPE_NO_MOTION_V01 = 3, 
  SENS_TYPE_SIG_MOTION_V01 = 4, 
  SENS_TYPE_FLAT_V01 = 5, 
  SENS_TYPE_GYRO_V01 = 6, 
  SENS_TYPE_GYRO_UNCAL_V01 = 7, 
  SENS_TYPE_MAG_V01 = 8, 
  SENS_TYPE_MAG_UNCAL_V01 = 9, 
  SENS_TYPE_BARO_V01 = 10, 
  SENS_TYPE_TEMP_V01 = 11, 
  SENS_TYPE_ALS_V01 = 12, 
  SENS_TYPE_PROX_V01 = 13, 
  SENS_TYPE_ORIENTATION_V01 = 14, 
  SENS_TYPE_HEARTRATE_ECG_V01 = 15, 
  SENS_TYPE_HEARTRATE_PPG_V01 = 16, 
  SENS_TYPE_GRAVITY_V01 = 17, 
  SENS_TYPE_LINEAR_ACCEL_V01 = 18, 
  SENS_TYPE_ROTATION_VECTOR_V01 = 19, 
  SENS_TYPE_GEO_MAG_ROT_VEC_V01 = 20, 
  SENS_TYPE_GAME_ROT_VECTOR_V01 = 21, 
  SENS_TYPE_STEP_COUNT_V01 = 22, 
  SENS_TYPE_STEP_DETECT_V01 = 23, 
  SENS_TYPE_GESTURE_V01 = 24, 
  SENS_TYPE_TILT_V01 = 25, 
  SENS_TYPE_DOUBLE_TWIST_V01 = 26, 
  SENS_TYPE_DOUBLE_TAP_V01 = 27, 
  SENS_TYPE_WIN_ORIENTATION_V01 = 28, 
  SENS_TYPE_HALL_V01 = 29, 
  SENS_TYPE_ACTIVITY_V01 = 30, 
  SENS_TYPE_VSYNC_V01 = 31, 
  SENS_TYPE_ACCEL_RAW_V01 = 32, 
  SENS_TYPE_WIFI_SCAN_V01 = 33, 
  SENS_TYPE_BLE_SCAN_V01 = 34, 
  SENS_TYPE_GPS_LOCATION_V01 = 35, 
  SENS_TYPE_GNSS_DATA_V01 = 36, 
  SENS_TYPE_CELL_INFO_V01 = 37, 
  SENS_TYPE_WRIST_TILT_V01 = 39, 
  SENS_TYPE_FIRST_USER_V01 = 64, 
  SENS_TYPE_LAST_USER_V01 = 128, 
  QCP_SENSTYPE_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_senstype_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_enums
    @{
  */
typedef enum {
  QCP_NUMAXIS_E_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  NUM_AXIS_EMBEDDED_V01 = 0, 
  NUM_AXIS_ONE_V01 = 1, 
  NUM_AXIS_THREE_V01 = 3, 
  NUM_AXIS_WIFI_V01 = 251, 
  NUM_AXIS_BLE_V01 = 252, 
  NUM_AXIS_CELL_INFO_V01 = 253, 
  NUM_AXIS_GPS_LOC_V01 = 254, 
  NUM_AXIS_GNSS_DATA_V01 = 255, 
  QCP_NUMAXIS_E_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qcp_numaxis_e_v01;
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/**  Identify the return code from proxy application CHRE.
  
 TODO: add more return codes 

 * Following enum defines the types of sensors that a hub may declare support
 * for. Declaration for support would mean that the hub can access and process
 * data from that particular sensor type.
 
 The power states used to control Apps power collapse
 

 * All communication between the context hubs and the Context Hub Service is in
 * the form of messages. Some message types are distinguished and their
 * Semantics shall be well defined.
 * Custom message types should be defined starting above
 * CONTEXT_HUB_PRIVATE_MSG_BASE
 

 * The private messages types sent to nanoapp


 * The sensorType
 * From sensType.h

 */
typedef struct {

  /*  This structure shall be the first element of every response message  */
  qcp_rc_e_v01 qcp_rc;
  /**<   return code from proxy */

  int64_t chre_rc;
  /**<   return code from chre */
}qcp_common_resp_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The nanoapp is identified by the id
 
 */
typedef struct {

  uint64_t id;
  /**<   This unique number identify the nanoapp */
}qcp_hub_app_name_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 TODO: Define the message formats structure associated with qcp_hub_messages_e
 
 */
typedef struct {

  uint8_t numSamples;

  uint8_t numFlushes;

  uint8_t biasBits;

  uint8_t interrupt;
}qcp_SensorFirstSample_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t deltaTime;

  float x;

  float y;

  float z;
}qcp_sens_sample_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The following structure defines each memory block in detail
 
 */
typedef struct {

  uint32_t total_bytes;

  uint32_t free_bytes;

  uint32_t type;

  uint32_t mem_flags;
}qcp_mem_range_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
typedef struct {

  qcp_hub_app_name_s_v01 app_name;

  uint32_t app_version;

  uint32_t num_mem_ranges;

  qcp_mem_range_s_v01 mem_usage[2];
}qcp_hub_app_info_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The message to/from the nanoapp.
 
 */
typedef struct {

  qcp_hub_app_name_s_v01 app_name;
  /**<   The unique ID for the app this message is sent */

  uint32_t message_type;
  /**<   Message type */

  uint32_t message_len;
  /**<   Message length */

  uint32_t message_payload_len;  /**< Must be set to # of elements in message_payload */
  char message_payload[QCP_HUB_MSG_MAX_LEN_V01];
  /**<   Message payload */
}qcp_hub_message_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The SensorInfo
 
 */
typedef struct {

  uint8_t sensorName_len;

  char sensorName[QCP_SENSOR_NAME_MAX_LEN_V01];
  /**<   sensors.c code does not use this */

  uint8_t num_supportedRates;

  uint32_t supportedRates[QCP_MAX_ODRS_V01];
  /**<   The list of ODRs supported by sensor */

  qcp_senstype_e_v01 sensorType;

  /*  < senstype.h  */
  qcp_numaxis_e_v01 numAxis;
  /**<   enum NumAxis */

  uint8_t interrupt;

  uint8_t flags1;
  /**<   enum SensorInfoFlags1 */

  uint16_t minSamples;
  /**<   minimum host fifo size (in # of samples) */

  uint8_t biasType;

  uint8_t rawType;

  uint16_t pad;

  float rawScale;
}qcp_sensorInfo_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The command to nanoapp, used as payload for send_message
 
 */
typedef struct {

  uint64_t latency;
  /**<   The report period */

  uint32_t rate;
  /**<   sampling rate */

  qcp_senstype_e_v01 sensorType;

  uint32_t idx;

  uint32_t sensorHandle;

  uint32_t cmd;

  uint32_t flags;

  uint32_t mode;
}qcp_napp_cmd_msg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * The response event from nanoapp to proxy
 
 */
typedef struct {

  int32_t rc;

  uint32_t napp_version;

  uint32_t sensorHandle;

  qcp_sensorInfo_v01 si;

  uint32_t cur_rate;

  uint64_t interval;

  uint64_t latency;

  uint32_t enabled;
}qcp_napp_evt_msg_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_aggregates
    @{
  */
/** 
 * nanoapp binary description used as a payload for CONTEXT_HUB_LOAD_APP.
 
 */
typedef struct {

  uint32_t header_version;

  uint32_t magic;

  qcp_hub_app_name_s_v01 app_id;

  uint32_t app_version;

  uint32_t flags;

  uint64_t hw_hub_type;
}qcp_app_binary_s_v01;  /* Type */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Reset the chre proxy application.
  This must be the first message to chre proxy application. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_reset_proxy_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Reset the chre proxy application.
  This must be the first message to chre proxy application. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */
}qcp_reset_proxy_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Get the chre proxy application information. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_get_proxy_info_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Get the chre proxy application information. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint32_t app_version;
  /**<   Defines the proxy application version.
  */
}qcp_get_proxy_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Start logging at chre proxy application for all messages sent and
  received to from CHRE HAL. */
typedef struct {

  /* Mandatory */
  uint32_t log_filename_len;  /**< Must be set to # of elements in log_filename */
  char log_filename[QCP_MAX_FILENAME_LEN_V01];
  /**<   The log file name provided by the test pc */
}qcp_start_log_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Start logging at chre proxy application for all messages sent and
  received to from CHRE HAL. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */
}qcp_start_log_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Stop logging at chre proxy application. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_stop_log_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Stop logging at chre proxy application. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint32_t log_filename_len;  /**< Must be set to # of elements in log_filename */
  char log_filename[QCP_MAX_FILENAME_LEN_V01];
  /**<   Return the absolute path + file name */
}qcp_stop_log_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; The message is use to control app's power collapse.

  For power testing purposes application processor shall be forced to
  power collapse (and not to hold any wake-locks). */
typedef struct {

  /* Mandatory */
  qcp_apps_power_collapse_e_v01 power_collapse;
  /**<   Apps power collapse state requested */
}qcp_ctrl_apps_power_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; The message is use to control app's power collapse.

  For power testing purposes application processor shall be forced to
  power collapse (and not to hold any wake-locks). */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  qcp_apps_power_collapse_e_v01 power_collapse;
  /**<   Identify the new Apps power collapse state. */
}qcp_ctrl_apps_power_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Get the number of context hubs present. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_fetch_hub_ids_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Get the number of context hubs present. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint32_t hub_ids_len;  /**< Must be set to # of elements in hub_ids */
  uint32_t hub_ids[QCP_MAX_HUBS_V01];
  /**<   Return the number of hubs available on the device.
  */
}qcp_fetch_hub_ids_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Get the hub information for the specified hub id. */
typedef struct {

  /* Mandatory */
  uint32_t hub_id;
  /**<   Get the hub info for this hub Id */
}qcp_fetch_hub_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Get the hub information for the specified hub id. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint32_t hub_name_len;  /**< Must be set to # of elements in hub_name */
  char hub_name[QCP_HUB_NAME_MAX_LEN_V01];
  /**<   descriptive name eg: "Awesome Hub #1" */

  /* Mandatory */
  uint32_t vendor_name_len;  /**< Must be set to # of elements in vendor_name */
  char vendor_name[QCP_HUB_VENDOR_NAME_MAX_LEN_V01];
  /**<   hub hardware vendor eg: "Qualcomm" */

  /* Mandatory */
  uint32_t toolchain_name_len;  /**< Must be set to # of elements in toolchain_name */
  char toolchain_name[QCP_HUB_TOOLCHAIN_NAME_MAX_LEN_V01];
  /**<   toolchain to make binaries eg:"gcc ARM" */

  /* Mandatory */
  uint32_t platform_version;
  /**<   Version of the hardware : eg 0x20 */

  /* Mandatory */
  uint32_t toolchain_version;
  /**<   Version of the toolchain : eg: 0x484 */

  /* Mandatory */
  uint32_t hub_id;
  /**<   a device unique id for this hub */

  /* Mandatory */
  float peak_mips;
  /**<   Peak MIPS platform can deliver */

  /* Mandatory */
  float stopped_power_draw_mw;
  /**<   if stopped, retention power, milliwatts */

  /* Mandatory */
  float sleep_power_draw_mw;
  /**<   if sleeping, retention power, milliwatts */

  /* Mandatory */
  float peak_power_draw_mw;
  /**<   for a busy CPUm power in milliwatts */

  /* Mandatory */
  uint32_t num_connected_sensors;
  /**<   number of connected sensors */

  /* Mandatory */
  qcp_hub_app_name_s_v01 os_app_name;
  /**<   send msgs here for OS functions */

  /* Mandatory */
  uint32_t max_supported_msg_len;
  /**<   This is the maximum size of the message that can
    be sent to the hub in one chunk (in bytes) */
}qcp_fetch_hub_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Get the connected sensors info from the hub id. */
typedef struct {

  /* Mandatory */
  uint32_t hub_id;
  /**<   Get connected sensors info from this hub */

  /* Mandatory */
  uint32_t sensor_idx;
  /**<   Get info from for this sensor */
}qcp_fetch_conn_sens_info_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Get the connected sensors info from the hub id. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. 
 Cloned below items from connected_sensor_t of context_hub.h */

  /* Mandatory */
  uint32_t sensor_id;
  /**<   identifier for this sensor 
 Cloned below items from physical_sensor_description_t of context_hub.h */

  /* Mandatory */
  uint32_t sensor_type;
  /**<   From the definitions above eg: 100 */

  /* Mandatory */
  uint32_t type_string_len;  /**< Must be set to # of elements in type_string */
  char type_string[QCP_SENSOR_TYPE_NAME_MAX_LEN_V01];
  /**<   Type as a string. eg: "GPS" */

  /* Mandatory */
  uint32_t sensor_name_len;  /**< Must be set to # of elements in sensor_name */
  char sensor_name[QCP_SENSOR_NAME_MAX_LEN_V01];
  /**<   Identifier eg: "Bosch BMI160" */

  /* Mandatory */
  uint32_t sensor_vendor_len;  /**< Must be set to # of elements in sensor_vendor */
  char sensor_vendor[QCP_SENSOR_VENDOR_NAME_MAX_LEN_V01];
  /**<   Vendor : eg "STM" */

  /* Mandatory */
  uint32_t sensor_version;
  /**<   Version : eg 0x1001 */

  /* Mandatory */
  uint32_t fifo_reserved_count;
  /**<   Batching possible in hardware. Please note that here hardware does not include
   the context hub itself. Thus, this definition may be different from say the
   number advertised in the sensors HAL which allows for batching in a hub.
  */

  /* Mandatory */
  uint32_t fifo_max_count;
  /**<   maximum number of batchable events. */

  /* Mandatory */
  uint64_t min_delay_ms;
  /**<   in milliseconds, corresponding to highest sampling freq */

  /* Mandatory */
  uint64_t max_delay_ms;
  /**<   in milliseconds, corresponds to minimum sampling frequency */

  /* Mandatory */
  float peak_power_mw;
  /**<   At max frequency & no batching, power in milliwatts */
}qcp_fetch_conn_sens_info_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Get the CHRE hub information. */
typedef struct {

  /* Mandatory */
  uint32_t hub_id;
  /**<   The unique id for the hub where message is sent */

  /* Mandatory */
  uint64_t app_id;
  /**<   The unique id of destination nanoapp to which message is sent */

  /* Mandatory */
  uint32_t message_type;
  /**<   The message type */

  /* Optional */
  uint8_t app_name_valid;  /**< Must be set to true if app_name is being passed */
  uint64_t app_name;
  /**<   The payload for CONTEXT_HUB ENABLE, DISABLE, UNLOAD, QUERY_APPS */

  /* Optional */
  uint8_t app_binary_valid;  /**< Must be set to true if app_binary is being passed */
  qcp_app_binary_s_v01 app_binary;
  /**<   payload for CONTEXT_HUB_LOAD_APP */

  /* Optional */
  uint8_t app_filename_valid;  /**< Must be set to true if app_filename is being passed */
  uint32_t app_filename_len;  /**< Must be set to # of elements in app_filename */
  char app_filename[QCP_MAX_FILENAME_LEN_V01];
  /**<   payload for CONTEXT_HUB_LOAD_APP, shall supply with app_binary */

  /* Optional */
  uint8_t sensorType_valid;  /**< Must be set to true if sensorType is being passed */
  qcp_senstype_e_v01 sensorType;
  /**<   SensorType, see senstype.h */

  /* Optional */
  uint8_t idx_valid;  /**< Must be set to true if idx is being passed */
  uint32_t idx;
  /**<   id field in connected_sensor_t */

  /* Optional */
  uint8_t sensorHandle_valid;  /**< Must be set to true if sensorHandle is being passed */
  uint32_t sensorHandle;
  /**<   sensorHandle, used to communicate with sensor going forward */

  /* Optional */
  uint8_t rate_valid;  /**< Must be set to true if rate is being passed */
  uint32_t rate;
  /**<   sampling rate in Hz */

  /* Optional */
  uint8_t latency_valid;  /**< Must be set to true if latency is being passed */
  uint64_t latency;
  /**<   Latency in ns. The request to SMGR will go as report rate in Hz */

  /* Optional */
  uint8_t mode_valid;  /**< Must be set to true if mode is being passed */
  uint32_t mode;
  /**<   Operating mode from chreSensorConfigureMode */

  /* Optional */
  uint8_t opaque_payload_valid;  /**< Must be set to true if opaque_payload is being passed */
  uint32_t opaque_payload_len;  /**< Must be set to # of elements in opaque_payload */
  uint8_t opaque_payload[QCP_OPAQUE_PAYLOAD_SIZE_V01];
  /**<   the opaque payload provided by test script sent to app_name */
}qcp_send_message_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Get the CHRE hub information. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint32_t hub_id;
  /**<   The unique id for the hub where message is sent */

  /* Mandatory */
  uint64_t app_id;
  /**<   The unique id of nano test app to which message is sent */

  /* Mandatory */
  uint32_t message_type;
  /**<   The message type */

  /* Optional */
  uint8_t app_info_valid;  /**< Must be set to true if app_info is being passed */
  uint32_t app_info_len;  /**< Must be set to # of elements in app_info */
  qcp_hub_app_info_s_v01 app_info[QCP_MAX_NANOAPPS_V01];
  /**<   Response to CONTEXT_HUB_QUERY_APPS */

  /* Optional */
  uint8_t mem_ranges_valid;  /**< Must be set to true if mem_ranges is being passed */
  uint32_t mem_ranges_len;  /**< Must be set to # of elements in mem_ranges */
  qcp_mem_range_s_v01 mem_ranges[QCP_MAX_MEMORY_RANGES_V01];
  /**<   Response to CONTEXT_HUB_QUERY_MEMORY */

  /* Optional */
  uint8_t napp_version_valid;  /**< Must be set to true if napp_version is being passed */
  int32_t napp_version;
  /**<   nanoapp version returned by NAPP_CMD_GET_TESTAPP_INFO */

  /* Optional */
  uint8_t sensorHandle_valid;  /**< Must be set to true if sensorHandle is being passed */
  uint32_t sensorHandle;
  /**<   sensorHandle returned by NAPP_CMD_SENS_FIND */

  /* Optional */
  uint8_t si_valid;  /**< Must be set to true if si is being passed */
  qcp_sensorInfo_v01 si;
  /**<   SensorInfo structure returned by returned by NAPP_CMD_SENS_FIND */

  /* Optional */
  uint8_t cur_rate_valid;  /**< Must be set to true if cur_rate is being passed */
  uint32_t cur_rate;
  /**<   current sampling rate, returned by NAPP_CMD_SENS_GET_CUR_RATE */

  /* Optional */
  uint8_t interval_valid;  /**< Must be set to true if interval is being passed */
  uint64_t interval;
  /**<   The interval, in nanoseconds, at which the sensor is now sampling */

  /* Optional */
  uint8_t latency_valid;  /**< Must be set to true if latency is being passed */
  uint64_t latency;
  /**<   The latency, in nanoseconds, at which the senor is now reporting */

  /* Optional */
  uint8_t enabled_valid;  /**< Must be set to true if enabled is being passed */
  uint32_t enabled;
  /**<   True if the sensor is actively powered and sampling; false otherwise */

  /* Optional */
  uint8_t opaque_payload_valid;  /**< Must be set to true if opaque_payload is being passed */
  uint32_t opaque_payload_len;  /**< Must be set to # of elements in opaque_payload */
  uint8_t opaque_payload[QCP_OPAQUE_PAYLOAD_SIZE_V01];
  /**<   the opaque payload from app_name */
}qcp_send_message_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Indication Message; This is sensor event received from the hub, AKA sensor indications */
typedef struct {

  /* Mandatory */
  uint32_t hub_id;
  /**<   Get the hub info for this hub Id */

  /* Mandatory */
  uint64_t app_id;
  /**<   The unique id of destination nanoapp to which message is sent */

  /* Mandatory */
  qcp_senstype_e_v01 sensorType;
  /**<   SensorType, see senstype.h */

  /* Mandatory */
  uint8_t numAxis;
  /**<   number of axis sensor supports */

  /* Mandatory */
  qcp_SensorFirstSample_s_v01 samplesInfo;

  /* Mandatory */
  uint64_t firstSampleTs;
  /**<   first sample SLPI timestamp in nano seconds */

  /* Mandatory */
  uint32_t samples_len;  /**< Must be set to # of elements in samples */
  qcp_sens_sample_s_v01 samples[QCP_MAX_SAMPLES_PER_EVENT_V01];
  /**<   array of samples */
}qcp_sensor_event_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Indication Message; This is generic event received from the hub */
typedef struct {

  /* Mandatory */
  uint32_t hub_id;
  /**<   Get the hub info for this hub Id */

  /* Mandatory */
  uint64_t app_id;
  /**<   The unique id of destination nanoapp to which message is sent */

  /* Mandatory */
  uint32_t event_type;
  /**<   generic event_type */

  /* Mandatory */
  uint32_t opaque_payload_len;  /**< Must be set to # of elements in opaque_payload */
  uint8_t opaque_payload[QCP_OPAQUE_PAYLOAD_SIZE_V01];
  /**<   the opaque payload from app_name */
}qcp_generic_event_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Stop the nanohub hal. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_stop_hub_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Stop the nanohub hal. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */
}qcp_stop_hub_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; This command gets current time of day on the Apps and SSC. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qcp_get_times_req_msg_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; This command gets current time of day on the Apps and SSC. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */

  /* Mandatory */
  uint64_t timestamp_dsps;
  /**<   Timestamp from the DSPS in nano seconds */

  /* Mandatory */
  uint64_t timestamp_apps;
  /**<   Timestamp from the apps processor in nanoseconds since last epoch */
}qcp_get_times_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Request Message; Write the string to log file as 0x1FFC packet. */
typedef struct {

  /* Mandatory */
  uint32_t str_payload_len;  /**< Must be set to # of elements in str_payload */
  char str_payload[QCP_LOG_TEXT_MAX_LEN_V01];
  /**<   String writren into log file */
}qcp_text_out_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup QCP_SVC_qmi_messages
    @{
  */
/** Response Message; Write the string to log file as 0x1FFC packet. */
typedef struct {

  /* Mandatory */
  qmi_response_type_v01 qmi_rc;
  /**<   Standard response type used by QMI */

  /* Mandatory */
  qcp_common_resp_s_v01 resp;
  /**<   Common message response. */
}qcp_text_out_resp_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QCP_CTRL_APPS_POWER_V01 
//#define REMOVE_QCP_FETCH_CONN_SENS_INFO_V01 
//#define REMOVE_QCP_FETCH_HUB_IDS_V01 
//#define REMOVE_QCP_FETCH_HUB_INFO_V01 
//#define REMOVE_QCP_GENERIC_EVENT_V01 
//#define REMOVE_QCP_GET_PROXY_INFO_V01 
//#define REMOVE_QCP_GET_TIMES_V01 
//#define REMOVE_QCP_RESET_PROXY_V01 
//#define REMOVE_QCP_SEND_MESSAGE_V01 
//#define REMOVE_QCP_SENSOR_EVENT_V01 
//#define REMOVE_QCP_START_LOG_V01 
//#define REMOVE_QCP_STOP_HUB_V01 
//#define REMOVE_QCP_STOP_LOG_V01 
//#define REMOVE_QCP_TEXT_OUT_V01 

/*Service Message Definition*/
/** @addtogroup QCP_SVC_qmi_msg_ids
    @{
  */
#define QCP_RESET_PROXY_REQ_V01 0x0000
#define QCP_RESET_PROXY_RESP_V01 0x0000
#define QCP_GET_PROXY_INFO_REQ_V01 0x0001
#define QCP_GET_PROXY_INFO_RESP_V01 0x0001
#define QCP_START_LOG_REQ_V01 0x0002
#define QCP_START_LOG_RESP_V01 0x0002
#define QCP_STOP_LOG_REQ_V01 0x0003
#define QCP_STOP_LOG_RESP_V01 0x0003
#define QCP_CTRL_APPS_POWER_REQ_V01 0x0004
#define QCP_CTRL_APPS_POWER_RESP_V01 0x0004
#define QCP_FETCH_HUB_IDS_REQ_V01 0x0005
#define QCP_FETCH_HUB_IDS_RESP_V01 0x0005
#define QCP_FETCH_HUB_INFO_REQ_V01 0x0006
#define QCP_FETCH_HUB_INFO_RESP_V01 0x0006
#define QCP_FETCH_CONN_SENS_INFO_REQ_V01 0x0007
#define QCP_FETCH_CONN_SENS_INFO_RESP_V01 0x0007
#define QCP_SEND_MESSAGE_REQ_V01 0x0008
#define QCP_SEND_MESSAGE_RESP_V01 0x0008
#define QCP_SENSOR_EVENT_V01 0x0009
#define QCP_STOP_HUB_REQ_V01 0x000A
#define QCP_STOP_HUB_RESP_V01 0x000A
#define QCP_GET_TIMES_REQ_V01 0x000B
#define QCP_GET_TIMES_RESP_V01 0x000B
#define QCP_TEXT_OUT_REQ_V01 0x000C
#define QCP_TEXT_OUT_RESP_V01 0x000C
#define QCP_GENERIC_EVENT_V01 0x000D
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro QCP_SVC_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type QCP_SVC_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define QCP_SVC_get_service_object_v01( ) \
          QCP_SVC_get_service_object_internal_v01( \
            QCP_SVC_V01_IDL_MAJOR_VERS, QCP_SVC_V01_IDL_MINOR_VERS, \
            QCP_SVC_V01_IDL_TOOL_VERS )
/**
    @}
  */

void snsHeapFree(uint16_t eventType, void *eventData);

#ifdef __cplusplus
}
#endif
#endif

