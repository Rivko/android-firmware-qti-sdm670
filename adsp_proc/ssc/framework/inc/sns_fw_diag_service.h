#pragma once
/*=============================================================================
  @file sns_fw_diag_service.h

  Manages Diagnostic Services for Sensors and Sensor Instances

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_diag.pb.h"
#include "sns_diag_print.h"
#include "sns_diag_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_diag_types.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_log.h"
#include "sns_log_codes.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_time.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/
/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;
struct sns_fw_sensor_instance;
struct sns_fw_request;
struct sns_sensor_uid;
struct sns_fw_diag_service;

/*=============================================================================
  Externs
  ===========================================================================*/
extern sns_diag_datatype_config *default_datatype;

/*=============================================================================
  Type definitions
  ===========================================================================*/

/*=============================================================================
  Public functions
  ===========================================================================*/

size_t
sns_diag_get_preencoded_size(sns_fw_diag_enc_size type);

/**
 * Records the data_type provided by this sensor
 *
 * @param[i] sensor registering with diag
 */
void
sns_diag_register_sensor(struct sns_fw_sensor  *sensor);

/**
 * printf() style function for printing a debug message from
 * sensor context.
 *
 * TODO
 *
 * @param[i] sensor pointer the sns_sensor printing this message
 * @param[i] prio priority (level) of this message
 * @param[i] file file name string
 * @param[i] line file line number of the caller
 * @param[i] format printf() style format string
 *
 */
void
sns_diag_sensor_sprintf(
    const sns_sensor *sensor, sns_message_priority prio,
    const char *file, uint32_t line, const char *format, ...);

/**
 * Query for maximum supported log packet size
 *
 * @param[i] this Diag Service Reference
 *
 * @return Size in bytes
 */
uint32_t
sns_diag_get_max_log_size(sns_diag_service const *this);

/**
 * Publish a framework log packet. Log packet memory is always freed.
 *
 * @param[i] log_id The log packet ID
 * @param[i] payload_size Size of the log packet payload
 * @param[i] payload Pointer to the log packet payload
 * @param[i] payload_encoded_size Size of the encoded payload
 * @param[i] payload_encode_cb Call back function to encode
 *       payload
 *
 * @return sns_rc
 */
sns_rc
sns_diag_publish_fw_log(sns_fw_diag_service_log_id  log_id,
                        uint32_t                    payload_size,
                        void                        *payload,
                        uint32_t                    payload_encoded_size,
                        sns_diag_encode_log_cb      payload_encode_cb);

/**
 * Publish a Sensor log packet. Log packet memory is always freed.
 *
 * Publish a Sensor log packet. Log packet memory is always freed.
 * @param[i] instance_id Unique identifier of the sensor
 *       instance, set to 0 if log is not pertinent to a
 *       specific instance
 * @param[i] sensor_uid UID of the sensor generating the log,
             can be set to NULL in instances when sensor UID is unknown
 * @param[i] data type of the sensor generating the log
 * @param[i] log_id The log packet ID
 * @param[i] payload_size Size of the log packet payload
 * @param[i] payload Pointer to the log packet payload
 * @param[i] payload_encoded_size Size of the encoded payload
 * @param[i] payload_encode_cb Call back function to encode payload
 *
 * @return sns_rc
 */
sns_rc sns_diag_publish_sensor_log(
  uintptr_t                           instance_id,
  struct sns_sensor_uid const        *sensor_uid,
  char const                         *data_type,
  sns_fw_diag_service_log_id         log_id,
  uint32_t                           payload_size,
  void                               *payload,
  uint32_t                           payload_encoded_size,
  sns_diag_encode_log_cb             payload_encode_cb);

/**
 * Allocate memory for a log packet
 *
 * @param[i] log_size Size of the log packet in bytes
 * @param[i] log_id log packet ID
 *
 * @return Pointer to allocated memory
 */
void*
sns_diag_log_alloc(uint32_t log_size,
                   sns_fw_diag_service_log_id log_id);

/**
 * Initializes the diag service
 *
 * @return pointer to the diag service
 */
struct sns_fw_diag_service*
sns_diag_service_init(void);

/**
 * Apply the diag filter for all data types
 *
 * @param[i] 1/0 -enable/disable diag
 */
void
sns_diag_set_filter_all(bool enable);

/**
 * Apply the diag filter for a given data_type
 *
 * @param[i] data_type whose filter to be updated
 * @param[i] 1/0 -enable/disable diag
 */
void
sns_diag_set_filter(const char *data_type, bool enable);

/*
 * Create a inst debug config object and populate it as appropriate.
 *
 * @param[i] instance Sensor Instance which may need a diag_config
 * @param[i] sensor   Sensor reference
 */
sns_rc
sns_diag_sensor_instance_init(struct sns_fw_sensor_instance *instance,
                              struct sns_fw_sensor *sensor);

/**
 * Returns debug configuration for an instance corresponding to
 * a particular SUID.
 *
 * @param[i] fw_instance reference to the Sensor Instance
 * @param[i] sensor_uid Sensor UID for sensor
 *
 * @return sns_diag_src_config *
 */
sns_diag_src_config*
sns_diag_find_diag_config( struct sns_fw_sensor_instance *instance,
  struct sns_sensor_uid const* sensor_uid);

/**
 * Free any state allocated by the DIAG Service within a Senosr Instance.
 *
 * @param[i] instance Instance in which to clear/free DIAG state
 */
void
sns_diag_sensor_instance_deinit(struct sns_fw_sensor_instance *instance);

/**
 * Submit a log to be encoded asynchrounously at a later time.
 * The log data is stored in a circular buffer. Data may be lost
 * in the circular buffer.
 *
 * Use this call to avoid deadlocks with ISLAND_EXIT()
 *
 * @param[i] log   Log to be submitted asynchronusly
 *
 * @return SNS_RC_SUCCESS if successful
 *         SNS_RC_FAILED otherwise
 */
sns_rc
sns_diag_submit_async_log(sns_diag_async_log_item log);

/**
 * Consume all asynchronously submitted logs and add them to
 * the sns_diag_list. Unless force is set to true, this function
 * might refuse to consume logs if there are not enough logs
 *
 * @param[i] force  If true, force consuming all async logs
*/
void
sns_diag_consume_async_logs(bool force);

/**
 * Pre-initialize diag service to enable logging during the boot
 * process
 *
 */
void sns_diag_fw_preinit( void );

/**
 * Deinitialize diag configuration infomration associated with
 * this sensor.
 *
 * @Note: Diag deinit uses attributes. Deinitialize diag before
 *   deinitializing the attribtes
 *
 * @param[i] sensor  Sensor to be deinitialized
 */
void
sns_diag_svc_sensor_deinit(sns_fw_sensor *sensor);

