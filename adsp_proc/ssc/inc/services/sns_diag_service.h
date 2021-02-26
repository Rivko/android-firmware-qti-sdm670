#pragma once
/*=============================================================================
  @file sns_diag_service.h

  Manages Diagnostic Services for Sensors and Sensor Instances

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stddef.h>
#include <stdint.h>

#include "sns_rc.h"
#include "sns_service.h"

#if !defined(__FILENAME__)
/* Find the base filename using __FILE__ macro */
#if defined (__GNUC__)
/* Ff using gcc, use the builtin strrchr function to compute at compile time */
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? \
                      __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#else
/* Fall back to generic run-time computation for other compilers */
#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? \
                      strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#endif

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_instance;
struct sns_sensor;
struct sns_sensor_uid;
struct sns_msg_v2_const_type;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * The Diagnostics Manager. Will be obtained from sns_service_manager::get_service.
 */
typedef struct sns_diag_service
{
  /* Service information */
  sns_service service;

  /* Public api provided by the framework to be used by the sensor. */
  struct sns_diag_service_api *api;

} sns_diag_service;

/**
 * Types of sensor state logs
 */
typedef enum sns_diag_sensor_state_log
{
  SNS_DIAG_SENSOR_STATE_LOG_INTERNAL = 0,
  SNS_DIAG_SENSOR_STATE_LOG_RAW = 1,
  SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT = 2,
} sns_diag_sensor_state_log;

/**
 * Priority levels for debug messages
 */
typedef enum sns_message_priority
{
  SNS_LOW     = 0,
  SNS_MED     = 1,
  SNS_HIGH    = 2,
  SNS_ERROR   = 3,
  SNS_FATAL   = 4,
} sns_message_priority;

/**
 * Callback Function to PB Encode Log
 *
 * @param[i] log Pointer to log packet information. Data stored here could be
 *    either the unencoded log packet message or any suitable data from which
 *    the encoded log packet can be generated.
 * @param[i] log_size Size of the log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of the log
 * @param[o] encoded_log Location where encoded log should be generated
 * @param[o] bytes_written Actual bytes written as part of encode
 *
 * @return sns_rc
 */
typedef sns_rc(*sns_diag_encode_log_cb)(void *log,
                                        size_t log_size,
                                        size_t encoded_log_size,
                                        void *encoded_log,
                                        size_t *bytes_written);

typedef struct sns_diag_service_api
{
  uint32_t struct_len;

  /**
   * @param[i] this Diag Service reference
   * @return Maximum size of a log packet supported by the Framework (bytes)
   */
  uint32_t (*get_max_log_size)(
    sns_diag_service const *this);

  /**
   * Allocate memory for a log packet
   *
   * Memory allocated by this function is used to store log packet information
   * and is returned as part of sns_diag_encode_log_cb for encoding. Only a
   * single outstanding allocation for log packet memory is allowed per log
   * type as defined in sns_diag_sensor_state_log
   *
   * @param[i] this Diag Service
   * @param[i] instance Instance pointer of the sensor
   * @param[i] sensor_uid UID of sensor for which log packet is being created
   * @param[i] log_size Requested size in bytes to store log packet information
   * @param[i] log_type Type of log packet
   *
   * @return Allocated memory; NULL if OOM or logging disabled for this sensor
   */
  void* (*alloc_log)(
    sns_diag_service                 *this,
    struct sns_sensor_instance       *instance,
    struct sns_sensor_uid const      *sensor_uid,
    uint32_t                         log_size,
    sns_diag_sensor_state_log        log_type);

  /**
   * Submit a log packet
   *
   * @param[i] this This diag instance
   * @param[i] instance Instance pointer of the sensor submitting the log
   * @param[i] sensor_uid UID of the sensor submitting the log
   * @param[i] log_size Size of the log packet information in bytes
   * @param[i] log Pointer to the log packet information
   * @param[i] log_type Type of log
   * @param[i] encoded_log_size Size of the encoded log in bytes
   * @param[i] encode_log_cb Function used to encode log
   *
   * @return
   *  SNS_RC_NOT_AVAILABLE if the log packet is currently disabled
   *  SNS_RC_INVALID_TYPE if the suid is not recognized
   *  SNS_RC_SUCCESS log packet was successfully submitted
   */
  sns_rc (*submit_log)(
    sns_diag_service                 *this,
    struct sns_sensor_instance       *instance,
    struct sns_sensor_uid const      *sensor_uid,
    uint32_t                         log_size,
    void                             *log,
    sns_diag_sensor_state_log        log_type,
    uint32_t                         encoded_log_size,
    sns_diag_encode_log_cb           encode_log_cb);

  /**
   * printf() style function for printing a debug message from sensor
   *
   * Limited printf formatting support (only 32-bit int argument supported)
   *
   * @param[i] this This diag service instance
   * @param[i] sensor pointer the sns_sensor printing this message
   * @param[i] prio priority (level) of this message
   * @param[i] file file name string
   * @param[i] line file line number of the caller
   * @param[i] number of arguments in "..."
   * @param[i] format printf() style format string
   */
  void (*sensor_printf)(
    sns_diag_service          *this,
    struct sns_sensor         *sensor,
    sns_message_priority       prio,
    const char                *file,
    uint32_t                   line,
    uint32_t                   nargs,
    const char                *format, ...);

  /**
   * printf()-like function for printing a debug message from sensor instance
   *
   * limited printf formatting support (only 32-bit int argument supported)
   *
   * @param[i] this This diag service instance
   * @param[i] sensor instance pointer to the sensor instance printing this message
   * @param[i] prio priority (level) of this message
   * @param[i] file file name of the caller
   * @param[i] line file line number of the caller
   * @param[i] number of arguments in "..."
   * @param[i] format printf-style format string
   */
  void (*sensor_inst_printf)(
    sns_diag_service                  *this,
    struct sns_sensor_instance        *inst,
    sns_message_priority               prio,
    const char                        *file,
    uint32_t                           line,
    uint32_t                           nargs,
    const char                        *format, ...);

  /**
   * printf() style function for printing a debug message from sensor
   *
   * Limited printf formatting support (only 32-bit int argument supported)
   *
   * @param[i] this This diag service instance
   * @param[i] sensor pointer the sns_sensor printing this message
   * @param[i] diag-defined static const msg_v2_const_type for the message
   * @param[i] number of arguments included in "..."
   */
  void (*sensor_printf_v2)(
    sns_diag_service                   *this,
    struct sns_sensor                  *sensor,
    struct sns_msg_v2_const_type const *msg_v2_struct,
    uint32_t                            nargs,
    ...);

  /**
   * printf()-like function for printing a debug message from sensor instance
   *
   * limited printf formatting support (only 32-bit int argument supported)
   *
   * @param[i] this This diag service instance
   * @param[i] sensor instance pointer to the sensor instance printing this message
   * @param[i] diag-defined static const msg_v2_const_type for the message
   * @param[i] number of arguments included in "..."
   */
  void (*sensor_inst_printf_v2)(
    sns_diag_service                   *this,
    struct sns_sensor_instance         *instance,
    struct sns_msg_v2_const_type const *msg_v2_struct,
    uint32_t                            nargs,
    ...);
} sns_diag_service_api;
