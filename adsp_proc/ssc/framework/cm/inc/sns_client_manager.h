#pragma once
/*=============================================================================
  @file sns_client_manager.h

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  The module
  will translate incoming QMI request messages into Client Request objects,
  and similarly will translate Sensor Events into QMI indications.  This
  translation will be trivial, and this module will only concern itself with
  the requested batch period and the client processor.  The configuration
  request will be passed along to the appropriate Sensor, and this module
  will batch all data received (as appropriate).

  This file only handles the sensor api part of the module implementation.

  Copyright (c) 2016-2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include "qmi_csi.h"
#include "sns_client.pb.h"
#include "sns_client_api_v01.h"
#include "sns_data_stream.h"
#include "sns_fw_sensor.h"
#include "sns_list.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_registry.pb.h"
#include "sns_registry_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_std.pb.h"
#include "sns_std_type.pb.h"
#include "sns_suid_util.h"
#include "sns_timer.h"
#include "sns_types.h"

/*=============================================================================
  Defines
  ===========================================================================*/
/* Max framework reserved request message ID as commented in sns_client.proto */
#define SNS_CM_MAX_FRAMEWORK_RESERVED_REQ_ID 127

#define SNS_CM_THREAD_QURT_SIGNAL           0x01
#define SNS_CM_THREAD_TIMER_SIGNAL          0x02
#define SNS_CM_THREAD_FLUSH_TO_DDR_SIGNAL   0x04
#define SNS_CM_THREAD_LOW_MEM_THRSH1_SIGNAL 0x08
#define SNS_CM_THREAD_LOW_MEM_THRSH2_SIGNAL 0x10

// When island heap < 2k, flush to DDR
#define SNS_CM_MAX_ISLAND_HEAP_USAGE      2*1024UL

#define SNS_CM_MAX_BATCH_HEAP_USAGE_LOW   (SNS_BATCH_HEAP_ALLOC*1024)/3
#define SNS_CM_MAX_BATCH_HEAP_USAGE_HIGH  50*1024

//Flag to enable debug prints.
#define ENABLE_DEBUG_PRINT 0
//Flag to simulate the behavior ,
//that all processors other than APPS to be in suspend state.
#define ENABLE_CM_SUSPEND_SIM 0

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

//Macros to split 64 bit value to low and hi 32 bits
#define SNS_CM_FETCH_LOW_32(x) (uint32_t)(x>>32)
#define SNS_CM_FETCH_HI_32(y) (uint32_t)(y)

#define SNS_CM_SENSOR_NAME "cm"
#define SNS_CM_VENDOR_NAME "qualcomm"
#define SNS_CM_PROTO_NAME "sns_client.proto"

// The default minimum batch period threshold in milliseconds.
// This value is used as the default value if the registry read fails.
#define SNS_CM_DEFAULT_MIN_BATCH_PERIOD_THRESHOLD 10

//The sleep vote latency
#define SNS_CM_APPS_AWAKE_LATENCY_USEC 5000

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/*
 * Maintains the timer details for a batch request.
 */
typedef struct timer_info
{
 /* In ticks. Batch period for the request. */
 sns_time batch_period;
} sns_timer_info;


typedef enum sns_cm_flush_type
{
  SNS_CM_NO_FLUSH=0,
  /*Used to identify a flush requested by client. */
  SNS_CM_CLIENT_FLUSH = 1,
  /*Used to identify a flush requested by batch
   * period expiry. */
  SNS_CM_BATCH_FLUSH = 2
} sns_cm_flush_type;

typedef struct sns_cm_request
{
 /* Original client request message associated
  * with this cm request. */
 sns_client_request_msg req_msg;

 /* Array of message ids maintained
  */
 uint32_t *nowk_msg_ids;
 uint32_t nowk_msg_id_cnt;

 sns_isafe_list_item item;
 /* List to batch data in. */
 sns_isafe_list batch_buff;

 /* List to track last sent time for data events. */
 sns_isafe_list data_evnt_msg_lst;

 /* Maintains the details of the batch timer */
 sns_timer_info timer;

 /* Ts of first sample added to batch buffer */
 sns_time first_batch_ts;

 /* Ts of last sample added to batch buffer
  * The first and last ts is an optimization to
  * avoid looking through batch buffer list */
 sns_time last_batch_ts;

 /*Tracks when the data was last sent for this request */
 sns_time last_sent_time;

 /* Time delta used to batch data for a request */
 sns_time compare_period;

 /* Data stream created with the sensor for this request. */
 sns_data_stream *data_stream;

 /* Stores the byte string encoded sensor config payload */
 void *sensor_payload;

 /* The sensor payload length */
 uint32_t sensor_payload_len;
 /* counts the number of client flushes initiated on this request */
 uint32_t active_flush_req_count;

 /* Flag which indicates if a flush is active */
 sns_cm_flush_type flush_type;

 /* The attribute value for stream type for the suid */
 sns_std_sensor_stream_type stream_attr_value;

 /* True value means processor is in suspend state */
 bool proc_susp_state;

 /* Is the request for a streaming sensor */
 bool is_streaming_sensor;

} sns_cm_request;

/*
 * Structure used to register a timer.
 */
typedef struct cm_eval_timer
{
  sns_time         time_out;
  sns_time         reg_time;
  sns_sensor       *sensor;
}cm_eval_timer;

typedef struct cm_proc_info
{
  /* The processor type for the client */
  sns_std_client_processor proc_type;
  /* Is the processor awake or suspended */
  bool is_suspended;
  /* Whether the batch time period must be recalculated after the timer expires */
  bool reval_batch_period;
  /* Whether the timer being registered for the processor is of
   * type wakeup or non wakeup.
   */
  sns_client_delivery delivery_type;
  /* Structure to maintain the details about the current registered timeout */
  cm_eval_timer time;
  /* The system timer object associated with the processor */
  sns_timer *system_timer;
  /* This structure type is added to proc_type_list maintained in cm
   * sensor state */
  sns_list_item item;
  uint32_t non_wk_cnt;
  /* Island client for this processror*/
  sns_island_client_handle island_client;
  /* Current batching period */
  sns_time current_timeout;
}cm_proc_info;
/*
 * Structure used to handle a timer event from timer thread.
 */
typedef struct cm_timer_event
{
  bool                     active;
  sns_std_client_processor proc_type;
  sns_client_delivery      delivery_type;
  /* Time stamp when timer was fired*/
  sns_time                 time_now;
  /* Time stamp when timer was registered*/
  sns_time                 time_reg;
}cm_timer_event;

/*
 * Structure which stores all registry related configuration.
 */
typedef struct cm_reg_config
{
  uint32_t ddr_heap_size;
  /* Enable power test mode which disables MAX batching */
  bool max_batch_disabled;
  /* Sets the minimum batch period threshold. Any batch period less than this
   * is treated as a streaming request.
   */
  uint32_t min_batch_period_thrshld_ms;
}cm_reg_config;

/**
 * Singular state for the Client Manager.  Represented to the Framework
 * as a Sensor.
 */

typedef struct sns_cm_state
{
  /* QMI handle identifying this service. */
  qmi_csi_service_handle service_provider;
  /* Signalling data used by QMI */
  qmi_csi_os_params os_params;
  /* Thread which will process all QMI callbacks */
  sns_osa_thread *thread;

  //Client manager required streams
  /* The stream associated with remote proc sensor */
  sns_data_stream *remote_proc_stream;
  /* The stream associated with suid sensor */
  sns_data_stream *suid_stream;
  /* The stream associated with registry sensor */
  sns_data_stream *registry_stream;
  /* smp2p, registry */
  SNS_SUID_LOOKUP_DATA(2) suid_lookup_data;

  /* Structure which stores all registry configuration for CM*/
  cm_reg_config registry_config;

  //List to maintain all processor types currently connected to SEE.
  sns_list proc_type_list;

  /*Flag to indicate if the config request to remote
   * proc sensor has been done. */
  bool remote_proc_state_configure;
  /* List used to handle timer call backs from the timer thread. */
  /* Timer events */
  cm_timer_event expired_timer_events[_sns_std_client_processor_MAX];
  /* Mutex lock to protect expired_timer_events */
  sns_osa_lock *expired_timer_events_lock;
  /* Island client handle */
  sns_island_client_handle island_client;
  /* SCPM manager client handle */
  sns_scpm_mgr_client *scpm_client;
  /* Tracks whether a bump up is active due to flush.
   */
  bool bump_up;

  /* Flag when set to true indicates there is no sensor data in island 
     which can be flughed to DDR. */
  bool can_flush_to_ddr;
} sns_cm_state;

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * This function re-evaluates and re-registers CM timers for relevant processors,
 * whenever there is a connect, disconnect on QMI connection.
 *
 * @param[i] this       The CM sensor pointer.
 * @param[i] proc_type  The processor type for timer evaluation.
 * @param[i] cm_req     The request which is being added/removed.
 */
void sns_cm_reval_and_register_timers(
    sns_sensor *const this,
	sns_std_client_processor proc_type,
	sns_cm_request *cm_req);

/**
 * This is the callback function for the registry item parsing.
 *
 * @param[i] reg_item     The registry data item.
 * @param[i] item_name    The item name of the registry item.
 * @param[i] ite_str_val
 * @param[i] parsed_buffer The buffer containing the registry item.
 *
 */
bool cm_parse_reg_config(
    sns_registry_data_item *reg_item,
    pb_buffer_arg  *item_name,
    pb_buffer_arg  *item_str_val,
    void *parsed_buffer);

/**
 * Gets the processor info structure for a processor type
 *
 * @param[i] this        The CM Sensor.
 * @param[i] proc_type   The processor type.
 */
cm_proc_info* sns_cm_get_proc_info(
    sns_sensor *this,
    sns_std_client_processor proc_type);

/**
 * Creates the processor info structure for a processor type and adds
 * it to the processor type list.
 *
 * @param[i] this               The CM Sensor.
 * @param[i] client_proc_type   The processor type.
 */
cm_proc_info* sns_cm_create_proc_info(
    sns_sensor *this,
    sns_std_client_processor client_proc_type);

/**
 * Decides island block and unblock state.
 *
 * @param[i] this              The CM sensor instance.
 * @param[i] proc_info         The processor
 *
 * @return   True              If island is being blocked
 *           False             If island is being unblocked.
 */
bool sns_cm_is_island_blocked(
    sns_sensor *this,
    sns_std_client_processor proc_type);

/**
 * Get CM suid.
 *
 * @param[i] this  The CM sensor instance.
 * return CM sensor suid
 */
sns_sensor_uid const *cm_get_sensor_uid(
    sns_sensor const *const this);

/*
 * In the event of low batch memory, this function sends out all
 * accumulated data.
 *
 * @param[i] cm_sensor The framework sensor pointer for CM sensor.
 */
void cm_handle_low_batch_memory(
    sns_fw_sensor *cm_sensor,
    bool drop_events);

/**
 * Check if island is blocked and set/release latency vote
 *
 * @param[i] this              The CM sensor instance.
 * @param[i] proc_info         The processor
 *
 */
void sns_cm_set_latency_vote(
    sns_sensor *this,
    sns_std_client_processor proc_type);

/**
 * Bump down MCPS once flush is complete
 *
 * @param[i] instance          The CM sensor instance.
 */
void sns_cm_bump_down_on_flush_compl (
   sns_sensor_instance *instance);

/**
 * Bump up MCPS once flush is initiated
 *
 * @param[i] instance          The CM sensor instance.
 */
void sns_cm_bump_up_on_flush_initiate (
   sns_sensor_instance *instance);
