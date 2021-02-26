#ifndef SLIM_SERVICE_01_H
#define SLIM_SERVICE_01_H
/**
  @file sensor_location_interface_manager_v01.h

  @brief This is the public header file which defines the slim service Data structures.

  This header file defines the types and structures that were defined in
  slim. It contains the constant values defined, enums, structures,
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
  Copyright (c) 2014-2017 Qualcomm Atheros, Inc.
  All rights reserved.
  Qualcomm Atheros Proprietary and Confidential.


  $Header$
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.2
   It was generated on: Tue Jun 13 2017 (Spin 0)
   From IDL File: sensor_location_interface_manager_v01.idl */

/** @defgroup slim_qmi_consts Constant values defined in the IDL */
/** @defgroup slim_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup slim_qmi_enums Enumerated types used in QMI messages */
/** @defgroup slim_qmi_messages Structures sent as QMI messages */
/** @defgroup slim_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup slim_qmi_accessor Accessor for QMI service object */
/** @defgroup slim_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup slim_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define SLIM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define SLIM_V01_IDL_MINOR_VERS 0x02
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define SLIM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define SLIM_V01_MAX_MESSAGE_ID 0x002F
/**
    @}
  */


/** @addtogroup slim_qmi_consts
    @{
  */
#define QMI_SLIM_SERVICE_INSTANCE_TEST_V01 0
#define QMI_SLIM_SERVICE_INSTANCE_AP_V01 1
#define QMI_SLIM_SERVICE_INSTANCE_MP_V01 2
#define QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01 50
#define QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01 65
#define QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01 65
#define QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01 50
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMERRORENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_SUCCESS_V01 = 0, /**<  No error; success \n  */
  eQMI_SLIM_ERROR_CLIENT_HANDLE_INVALID_V01 = 1, /**<  Invalid client handle \n  */
  eQMI_SLIM_ERROR_UNSUPPORTED_V01 = 2, /**<  Operation is not supported \n  */
  eQMI_SLIM_ERROR_IPC_COMMUNICATION_V01 = 3, /**<  Failure with IPC communication \n  */
  eQMI_SLIM_ERROR_INVALID_CALLBACK_V01 = 4, /**<  Client callback is invalid \n  */
  eQMI_SLIM_ERROR_INTERNAL_V01 = 5, /**<  Internal SLIM error \n  */
  eQMI_SLIM_ERROR_MAX_TXN_COUNT_REACHED_V01 = 6, /**<  Maximum transaction count was exceeded \n  */
  eQMI_SLIM_ERROR_BAD_ARGUMENT_V01 = 7, /**<  Client provided a bad argument \n  */
  eQMI_SLIM_ERROR_QMI_INTERFACE_V01 = 8, /**<  Error occurred in the QMI communication \n  */
  eQMI_SLIM_ERROR_ALLOCATION_V01 = 9, /**<  Error occurred during memory allocation \n  */
  eQMI_SLIM_ERROR_SERVICE_RESERVED_V01 = 10, /**<  Service connection is already reserved \n  */
  eQMI_SLIM_ERROR_SERVICE_NOT_ENABLED_V01 = 11, /**<  Service is not enabled \n  */
  eQMI_SLIM_ERROR_MAX_CLIENT_COUNT_REACHED_V01 = 12, /**<  Maximum client count was exceeded \n  */
  eQMI_SLIM_ERROR_OTHER_V01 = 13, /**<  Unmappable error code  */
  QMISLIMERRORENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimErrorEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMCLIENTENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_NODE_TEST_V01 = 0, /**<  Test client \n  */
  eQMI_SLIM_NODE_AP_V01 = 1, /**<  Application processor \n  */
  eQMI_SLIM_NODE_MP_V01 = 2, /**<  Modem processor  */
  QMISLIMCLIENTENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimClientEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMSERVICEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_SERVICE_SENSOR_ACCEL_V01 = 0, /**<  Accelerometer sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP_V01 = 1, /**<   Accelerometer temperature sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_GYRO_V01 = 2, /**<  Gyro sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP_V01 = 3, /**<  Gyro temperature sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB_V01 = 4, /**<   Calibrated magnetometer sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB_V01 = 5, /**<  Uncalibrated magnetometer sensor data service  */
  eQMI_SLIM_SERVICE_SENSOR_BARO_V01 = 6, /**<  Barometer service  */
  eQMI_SLIM_SERVICE_AMD_V01 = 7, /**<   Absolute motion data service  */
  eQMI_SLIM_SERVICE_RMD_V01 = 8, /**<    Relative motion data service  */
  eQMI_SLIM_SERVICE_SMD_V01 = 9, /**<   Significant motion data service  */
  eQMI_SLIM_SERVICE_MOTION_DATA_V01 = 10, /**<  Motion data service  */
  eQMI_SLIM_SERVICE_PEDOMETER_V01 = 11, /**<   Pedometer service  */
  eQMI_SLIM_SERVICE_DISTANCE_BOUND_V01 = 12, /**<   Distance bound (CMC-based) data service   */
  eQMI_SLIM_SERVICE_VEHICLE_ACCEL_V01 = 13, /**<   Vehicle accelerometer data service  */
  eQMI_SLIM_SERVICE_VEHICLE_GYRO_V01 = 14, /**<  Vehicle angular rate data service  */
  eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY_V01 = 15, /**<  Vehicle odometry data service  */
  eQMI_SLIM_SERVICE_PED_ALIGNMENT_V01 = 16, /**<  Pedestrian alignment service  */
  eQMI_SLIM_SERVICE_VEHICLE_SPEED_V01 = 17, /**<   Vehicle speed data service  */
  eQMI_SLIM_SERVICE_VEHICLE_DWS_V01 = 18, /**<  Vehicle differential wheel speed data service  */
  eQMI_SLIM_SERVICE_VEHICLE_GEAR_V01 = 19, /**<  Vehicle gear data service  */
  QMISLIMSERVICEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimServiceEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMTIMESOURCEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_TIME_SOURCE_UNSPECIFIED_V01 = 0, /**<  Sensor time source is unspecified;
       can be converted to common with timesync  */
  eQMI_SLIM_TIME_SOURCE_COMMON_V01 = 1, /**<  Time source is common between the sensors and
       the SLIM engine: monotonous millisecond time;
       this does not include leap seconds  */
  eQMI_SLIM_TIME_SOURCE_GPS_V01 = 2, /**<  GPS time, in milliseconds since GPS epoch 00:00:00 January 6th 1980;
       this does not include leap seconds  */
  eQMI_SLIM_TIME_SOURCE_UTC_V01 = 3, /**<  UTC time, in milliseconds since UNIX epoch 00:00:00 January 1st 1970;
       like UNIX time, this includes leap seconds  */
  eQMI_SLIM_TIME_SOURCE_LEGACY_V01 = 4, /**<  Legacy sensor timestamp; used only when playing back old logs that
       require sensor-to-GPS timestamp conversion using legacy timesync  */
  QMISLIMTIMESOURCEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimTimeSourceEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMSERVICEPROVIDERENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_SERVICE_PROVIDER_OTHER_V01 = 0, /**<  Unknown service provider type; this value is used in service data
       reports when the service provider cannot be mapped to one of the known
       types
   */
  eQMI_SLIM_SERVICE_PROVIDER_SSC_V01 = 1, /**<  SSC provider   */
  eQMI_SLIM_SERVICE_PROVIDER_SAMLITE_V01 = 2, /**<   SAMLite provider  */
  eQMI_SLIM_SERVICE_PROVIDER_NATIVE_V01 = 3, /**<   Native provider (NDK/Sensor1, and so forth).  */
  QMISLIMSERVICEPROVIDERENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimServiceProviderEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMQMDMOTIONSTATEENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_QMD_MOTION_STATE_UNSPECIFIED_V01 = 0, /**<  Motion state is unspecified  */
  eQMI_SLIM_QMD_MOTION_STATE_REST_V01 = 1, /**<  Rest motion state  */
  eQMI_SLIM_QMD_MOTION_STATE_MOVE_V01 = 2, /**<  Move motion state  */
  QMISLIMQMDMOTIONSTATEENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimQmdMotionStateEnumT_v01;
/**
    @}
  */

/** @addtogroup slim_qmi_enums
    @{
  */
typedef enum {
  QMISLIMINJECTPEDOMETERREPORTENUMT_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_SLIM_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT_V01 = 0, /**<  Report is injected on detection of a step event  */
  eQMI_SLIM_INJECT_PEDOMETER_REPORT_PERIODICALLY_V01 = 1, /**<  Report is injected periodically  */
  QMISLIMINJECTPEDOMETERREPORTENUMT_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiSlimInjectPedometerReportEnumT_v01;
/**
    @}
  */

typedef uint64_t qmiSlimServiceProviderMaskT_v01;
#define QMI_SLIM_SERVICE_PROVIDER_MASK_SSC_V01 ((qmiSlimServiceProviderMaskT_v01)0x0000000000000001ull) /**<  SSC provider can be used  */
#define QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE_V01 ((qmiSlimServiceProviderMaskT_v01)0x0000000000000002ull) /**<  SAMLite provider can be used  */
#define QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE_V01 ((qmiSlimServiceProviderMaskT_v01)0x0000000000000004ull) /**<  Native provider can be used  */
typedef uint64_t qmiSlimFlagsMaskT_v01;
#define QMI_SLIM_FLAGS_MASK_TIME_JUMP_V01 ((qmiSlimFlagsMaskT_v01)0x0000000000000001ull) /**<  Time has jumped so that the timestamp is not related to the
       previous timestamp; occurs when SLIM timesync is reset  */
#define QMI_SLIM_FLAGS_MASK_TIME_UNC_HIGH_V01 ((qmiSlimFlagsMaskT_v01)0x0000000000000002ull) /**<  Timestamp uncertainty is high, and must not be trusted; occurs
       if there is too much noise in the timesync measurements,
       or if timesync has not yet converged to a low-uncertainty estimate  */
typedef uint64_t qmiSlimVehicleOdometryMaskT_v01;
#define QMI_SLIM_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT_V01 ((qmiSlimVehicleOdometryMaskT_v01)0x0000000000000001ull) /**<  Odometry data includes at least some data where
       the vehicle might have been moving in the reverse direction. If
       the odometry data might be in reverse, this bit must be set.
       If the odometry data is all in the forward direction, this bit
       must not be set  */
#define QMI_SLIM_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS_V01 ((qmiSlimVehicleOdometryMaskT_v01)0x0000000000000002ull) /**<  Odometry data includes at least some data affected
       by a major error source affecting distance-traveled accuracy,
       such as wheel slippage because of skidding, gravel, snow, or ice as
       detected by the vehicle, for example, via an ABS or other system  */
#define QMI_SLIM_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT_V01 ((qmiSlimVehicleOdometryMaskT_v01)0x0000000000000004ull) /**<  Odometry data is an absolute amount since the vehicle
       began service, and is the same vehicle that is regularly used with
       this device (so that the offset of this value, since the last time
       this measurement was used by the location engine, can be safely used
       as a likely correct estimate of the distance traveled since last
       use)  */
/** @addtogroup slim_qmi_aggregates
    @{
  */
typedef struct {

  qmiSlimServiceProviderEnumT_v01 provider;
  /**<   Provider type. Values: \n
      - eQMI_SLIM_SERVICE_PROVIDER_OTHER (0) --  Unknown service provider type; this value is used in service data
       reports when the service provider cannot be mapped to one of the known
       types

      - eQMI_SLIM_SERVICE_PROVIDER_SSC (1) --  SSC provider
      - eQMI_SLIM_SERVICE_PROVIDER_SAMLITE (2) --   SAMLite provider
      - eQMI_SLIM_SERVICE_PROVIDER_NATIVE (3) --   Native provider (NDK/Sensor1, and so forth).
 */

  qmiSlimServiceEnumT_v01 slimService;
  /**<
 SLIM service type; a message-specific subset of applicable services.
 Values: \n
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL (0) --  Accelerometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP (1) --   Accelerometer temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO (2) --  Gyro sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP (3) --  Gyro temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB (4) --   Calibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB (5) --  Uncalibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_BARO (6) --  Barometer service
      - eQMI_SLIM_SERVICE_AMD (7) --   Absolute motion data service
      - eQMI_SLIM_SERVICE_RMD (8) --    Relative motion data service
      - eQMI_SLIM_SERVICE_SMD (9) --   Significant motion data service
      - eQMI_SLIM_SERVICE_MOTION_DATA (10) --  Motion data service
      - eQMI_SLIM_SERVICE_PEDOMETER (11) --   Pedometer service
      - eQMI_SLIM_SERVICE_DISTANCE_BOUND (12) --   Distance bound (CMC-based) data service
      - eQMI_SLIM_SERVICE_VEHICLE_ACCEL (13) --   Vehicle accelerometer data service
      - eQMI_SLIM_SERVICE_VEHICLE_GYRO (14) --  Vehicle angular rate data service
      - eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY (15) --  Vehicle odometry data service
      - eQMI_SLIM_SERVICE_PED_ALIGNMENT (16) --  Pedestrian alignment service
      - eQMI_SLIM_SERVICE_VEHICLE_SPEED (17) --   Vehicle speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_DWS (18) --  Vehicle differential wheel speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_GEAR (19) --  Vehicle gear data service
 */

  qmiSlimTimeSourceEnumT_v01 timeSource;
  /**<
 Time source. Values: \n
      - eQMI_SLIM_TIME_SOURCE_UNSPECIFIED (0) --  Sensor time source is unspecified;
       can be converted to common with timesync
      - eQMI_SLIM_TIME_SOURCE_COMMON (1) --  Time source is common between the sensors and
       the SLIM engine: monotonous millisecond time;
       this does not include leap seconds
      - eQMI_SLIM_TIME_SOURCE_GPS (2) --  GPS time, in milliseconds since GPS epoch 00:00:00 January 6th 1980;
       this does not include leap seconds
      - eQMI_SLIM_TIME_SOURCE_UTC (3) --  UTC time, in milliseconds since UNIX epoch 00:00:00 January 1st 1970;
       like UNIX time, this includes leap seconds
      - eQMI_SLIM_TIME_SOURCE_LEGACY (4) --  Legacy sensor timestamp; used only when playing back old logs that
       require sensor-to-GPS timestamp conversion using legacy timesync
 */

  uint64_t time;
  /**<   Notification timestamp or base time, in milliseconds;
       add sample time offsets to this base time to get full
       sample timestamps.
   */

  qmiSlimFlagsMaskT_v01 flags;
  /**<
 SLIM flags. Values: \n
      - QMI_SLIM_FLAGS_MASK_TIME_JUMP (0x0000000000000001) --  Time has jumped so that the timestamp is not related to the
       previous timestamp; occurs when SLIM timesync is reset
      - QMI_SLIM_FLAGS_MASK_TIME_UNC_HIGH (0x0000000000000002) --  Timestamp uncertainty is high, and must not be trusted; occurs
       if there is too much noise in the timesync measurements,
       or if timesync has not yet converged to a low-uncertainty estimate
 */
}qmiSlimServiceDataHeaderT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Genericresponsemessage Message; Queries the fields supported for a single command as implemented
               by the currently running software. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SLIM-Specific Error Code */
  uint8_t slimError_valid;  /**< Must be set to true if slimError is being passed */
  qmiSlimErrorEnumT_v01 slimError;
  /**<
 SLIM-specific error code. Values: \n
      - eQMI_SLIM_SUCCESS (0) --  No error; success \n
      - eQMI_SLIM_ERROR_CLIENT_HANDLE_INVALID (1) --  Invalid client handle \n
      - eQMI_SLIM_ERROR_UNSUPPORTED (2) --  Operation is not supported \n
      - eQMI_SLIM_ERROR_IPC_COMMUNICATION (3) --  Failure with IPC communication \n
      - eQMI_SLIM_ERROR_INVALID_CALLBACK (4) --  Client callback is invalid \n
      - eQMI_SLIM_ERROR_INTERNAL (5) --  Internal SLIM error \n
      - eQMI_SLIM_ERROR_MAX_TXN_COUNT_REACHED (6) --  Maximum transaction count was exceeded \n
      - eQMI_SLIM_ERROR_BAD_ARGUMENT (7) --  Client provided a bad argument \n
      - eQMI_SLIM_ERROR_QMI_INTERFACE (8) --  Error occurred in the QMI communication \n
      - eQMI_SLIM_ERROR_ALLOCATION (9) --  Error occurred during memory allocation \n
      - eQMI_SLIM_ERROR_SERVICE_RESERVED (10) --  Service connection is already reserved \n
      - eQMI_SLIM_ERROR_SERVICE_NOT_ENABLED (11) --  Service is not enabled \n
      - eQMI_SLIM_ERROR_MAX_CLIENT_COUNT_REACHED (12) --  Maximum client count was exceeded \n
      - eQMI_SLIM_ERROR_OTHER (13) --  Unmappable error code
 */
}qmiSlimGenRespMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Indicates the service error. */
typedef struct {

  /* Mandatory */
  /*  Service */
  qmiSlimServiceEnumT_v01 slimService;
  /**<
 Service type. Values:\n
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL (0) --  Accelerometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP (1) --   Accelerometer temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO (2) --  Gyro sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP (3) --  Gyro temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB (4) --   Calibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB (5) --  Uncalibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_BARO (6) --  Barometer service
      - eQMI_SLIM_SERVICE_AMD (7) --   Absolute motion data service
      - eQMI_SLIM_SERVICE_RMD (8) --    Relative motion data service
      - eQMI_SLIM_SERVICE_SMD (9) --   Significant motion data service
      - eQMI_SLIM_SERVICE_MOTION_DATA (10) --  Motion data service
      - eQMI_SLIM_SERVICE_PEDOMETER (11) --   Pedometer service
      - eQMI_SLIM_SERVICE_DISTANCE_BOUND (12) --   Distance bound (CMC-based) data service
      - eQMI_SLIM_SERVICE_VEHICLE_ACCEL (13) --   Vehicle accelerometer data service
      - eQMI_SLIM_SERVICE_VEHICLE_GYRO (14) --  Vehicle angular rate data service
      - eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY (15) --  Vehicle odometry data service
      - eQMI_SLIM_SERVICE_PED_ALIGNMENT (16) --  Pedestrian alignment service
      - eQMI_SLIM_SERVICE_VEHICLE_SPEED (17) --   Vehicle speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_DWS (18) --  Vehicle differential wheel speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_GEAR (19) --  Vehicle gear data service
 */

  /* Mandatory */
  /*  Error Code */
  qmiSlimErrorEnumT_v01 slimError;
  /**<
 SLIM-specific error code. Values: \n
      - eQMI_SLIM_SUCCESS (0) --  No error; success \n
      - eQMI_SLIM_ERROR_CLIENT_HANDLE_INVALID (1) --  Invalid client handle \n
      - eQMI_SLIM_ERROR_UNSUPPORTED (2) --  Operation is not supported \n
      - eQMI_SLIM_ERROR_IPC_COMMUNICATION (3) --  Failure with IPC communication \n
      - eQMI_SLIM_ERROR_INVALID_CALLBACK (4) --  Client callback is invalid \n
      - eQMI_SLIM_ERROR_INTERNAL (5) --  Internal SLIM error \n
      - eQMI_SLIM_ERROR_MAX_TXN_COUNT_REACHED (6) --  Maximum transaction count was exceeded \n
      - eQMI_SLIM_ERROR_BAD_ARGUMENT (7) --  Client provided a bad argument \n
      - eQMI_SLIM_ERROR_QMI_INTERFACE (8) --  Error occurred in the QMI communication \n
      - eQMI_SLIM_ERROR_ALLOCATION (9) --  Error occurred during memory allocation \n
      - eQMI_SLIM_ERROR_SERVICE_RESERVED (10) --  Service connection is already reserved \n
      - eQMI_SLIM_ERROR_SERVICE_NOT_ENABLED (11) --  Service is not enabled \n
      - eQMI_SLIM_ERROR_MAX_CLIENT_COUNT_REACHED (12) --  Maximum client count was exceeded \n
      - eQMI_SLIM_ERROR_OTHER (13) --  Unmappable error code
 */
}qmiSlimServiceErrorIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Disables the SLIM service. */
typedef struct {

  /* Mandatory */
  /*  Service to Disable */
  qmiSlimServiceEnumT_v01 slimService;
  /**<
 Service to disable. Values: \n
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL (0) --  Accelerometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP (1) --   Accelerometer temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO (2) --  Gyro sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP (3) --  Gyro temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB (4) --   Calibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB (5) --  Uncalibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_BARO (6) --  Barometer service
      - eQMI_SLIM_SERVICE_AMD (7) --   Absolute motion data service
      - eQMI_SLIM_SERVICE_RMD (8) --    Relative motion data service
      - eQMI_SLIM_SERVICE_SMD (9) --   Significant motion data service
      - eQMI_SLIM_SERVICE_MOTION_DATA (10) --  Motion data service
      - eQMI_SLIM_SERVICE_PEDOMETER (11) --   Pedometer service
      - eQMI_SLIM_SERVICE_DISTANCE_BOUND (12) --   Distance bound (CMC-based) data service
      - eQMI_SLIM_SERVICE_VEHICLE_ACCEL (13) --   Vehicle accelerometer data service
      - eQMI_SLIM_SERVICE_VEHICLE_GYRO (14) --  Vehicle angular rate data service
      - eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY (15) --  Vehicle odometry data service
      - eQMI_SLIM_SERVICE_PED_ALIGNMENT (16) --  Pedestrian alignment service
      - eQMI_SLIM_SERVICE_VEHICLE_SPEED (17) --   Vehicle speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_DWS (18) --  Vehicle differential wheel speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_GEAR (19) --  Vehicle gear data service
 */
}qmiSlimDisableServiceReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Registers a client for a SLIM Service Provider connection. */
typedef struct {

  /* Mandatory */
  /*  Client Type */
  qmiSlimClientEnumT_v01 clientType;
  /**<   Client type. Values: \n
      - eQMI_SLIM_NODE_TEST (0) --  Test client \n
      - eQMI_SLIM_NODE_AP (1) --  Application processor \n
      - eQMI_SLIM_NODE_MP (2) --  Modem processor
 */
}qmiSlimRegisterReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Unregisters a client. */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmiSlimUnregisterReqMsgT_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Notifies that a service update occurred. */
typedef struct {

  /* Mandatory */
  /*  Updated Service */
  qmiSlimServiceEnumT_v01 slimService;
  /**<   Service type. Values: \n
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL (0) --  Accelerometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP (1) --   Accelerometer temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO (2) --  Gyro sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP (3) --  Gyro temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB (4) --   Calibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB (5) --  Uncalibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_BARO (6) --  Barometer service
      - eQMI_SLIM_SERVICE_AMD (7) --   Absolute motion data service
      - eQMI_SLIM_SERVICE_RMD (8) --    Relative motion data service
      - eQMI_SLIM_SERVICE_SMD (9) --   Significant motion data service
      - eQMI_SLIM_SERVICE_MOTION_DATA (10) --  Motion data service
      - eQMI_SLIM_SERVICE_PEDOMETER (11) --   Pedometer service
      - eQMI_SLIM_SERVICE_DISTANCE_BOUND (12) --   Distance bound (CMC-based) data service
      - eQMI_SLIM_SERVICE_VEHICLE_ACCEL (13) --   Vehicle accelerometer data service
      - eQMI_SLIM_SERVICE_VEHICLE_GYRO (14) --  Vehicle angular rate data service
      - eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY (15) --  Vehicle odometry data service
      - eQMI_SLIM_SERVICE_PED_ALIGNMENT (16) --  Pedestrian alignment service
      - eQMI_SLIM_SERVICE_VEHICLE_SPEED (17) --   Vehicle speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_DWS (18) --  Vehicle differential wheel speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_GEAR (19) --  Vehicle gear data service
 */

  /* Mandatory */
  /*  Set of Available Providers */
  qmiSlimServiceProviderMaskT_v01 providerFlags;
  /**<
 Set of available providers. Values: \n
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SSC (0x0000000000000001) --  SSC provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE (0x0000000000000002) --  SAMLite provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE (0x0000000000000004) --  Native provider can be used
 */
}qmiSlimServiceUpdateIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Requests a time update. */
typedef struct {

  /* Mandatory */
  /*  Reference Time */
  uint64_t time;
  /**<
     Reference time, in milliseconds.
    */
}qmiSlimGetTimeUpdateReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Response Message; Requests a time update. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;

  /* Optional */
  /*  SLIM-Specific Error Code */
  uint8_t slimError_valid;  /**< Must be set to true if slimError is being passed */
  qmiSlimErrorEnumT_v01 slimError;
  /**<
 SLIM-specific error code. Values: \n
      - eQMI_SLIM_SUCCESS (0) --  No error; success \n
      - eQMI_SLIM_ERROR_CLIENT_HANDLE_INVALID (1) --  Invalid client handle \n
      - eQMI_SLIM_ERROR_UNSUPPORTED (2) --  Operation is not supported \n
      - eQMI_SLIM_ERROR_IPC_COMMUNICATION (3) --  Failure with IPC communication \n
      - eQMI_SLIM_ERROR_INVALID_CALLBACK (4) --  Client callback is invalid \n
      - eQMI_SLIM_ERROR_INTERNAL (5) --  Internal SLIM error \n
      - eQMI_SLIM_ERROR_MAX_TXN_COUNT_REACHED (6) --  Maximum transaction count was exceeded \n
      - eQMI_SLIM_ERROR_BAD_ARGUMENT (7) --  Client provided a bad argument \n
      - eQMI_SLIM_ERROR_QMI_INTERFACE (8) --  Error occurred in the QMI communication \n
      - eQMI_SLIM_ERROR_ALLOCATION (9) --  Error occurred during memory allocation \n
      - eQMI_SLIM_ERROR_SERVICE_RESERVED (10) --  Service connection is already reserved \n
      - eQMI_SLIM_ERROR_SERVICE_NOT_ENABLED (11) --  Service is not enabled \n
      - eQMI_SLIM_ERROR_MAX_CLIENT_COUNT_REACHED (12) --  Maximum client count was exceeded \n
      - eQMI_SLIM_ERROR_OTHER (13) --  Unmappable error code
 */

  /* Optional */
  /*  Request Receive Time */
  uint8_t rxTime_valid;  /**< Must be set to true if rxTime is being passed */
  uint64_t rxTime;
  /**<
    Remote request receive time, in milliseconds.
  */

  /* Optional */
  /*  Response Dispatch Time */
  uint8_t txTime_valid;  /**< Must be set to true if txTime is being passed */
  uint64_t txTime;
  /**<
    Remote response dispatch time, in milliseconds.
  */
}qmiSlimGetTimeUpdateRespMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_aggregates
    @{
  */
/**

 */
typedef struct {

  /*  Aligned Batch Start Time */
  uint64_t batchStartTime;
  /**<   Batch Start Time, in milli second. */

  /*  Time Source of Batch Start Time */
  qmiSlimTimeSourceEnumT_v01 timeSource;
  /**<
 Time source. Values: \n
      - eQMI_SLIM_TIME_SOURCE_UNSPECIFIED (0) --  Sensor time source is unspecified;
       can be converted to common with timesync
      - eQMI_SLIM_TIME_SOURCE_COMMON (1) --  Time source is common between the sensors and
       the SLIM engine: monotonous millisecond time;
       this does not include leap seconds
      - eQMI_SLIM_TIME_SOURCE_GPS (2) --  GPS time, in milliseconds since GPS epoch 00:00:00 January 6th 1980;
       this does not include leap seconds
      - eQMI_SLIM_TIME_SOURCE_UTC (3) --  UTC time, in milliseconds since UNIX epoch 00:00:00 January 1st 1970;
       like UNIX time, this includes leap seconds
      - eQMI_SLIM_TIME_SOURCE_LEGACY (4) --  Legacy sensor timestamp; used only when playing back old logs that
       require sensor-to-GPS timestamp conversion using legacy timesync
 */
}qmiSlimSensorDataBatchAlignReqStructT_v01;  /* Type */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Enables reporting for a specified sensor. */
typedef struct {

  /* Mandatory */
  /*  SLIM Service to Enable */
  qmiSlimServiceEnumT_v01 slimService;
  /**<
 SLIM service to enable. Values: \n
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL (0) --  Accelerometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_ACCEL_TEMP (1) --   Accelerometer temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO (2) --  Gyro sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_GYRO_TEMP (3) --  Gyro temperature sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_BARO (6) --  Barometer service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_CALIB (4) --   Calibrated magnetometer sensor data service
      - eQMI_SLIM_SERVICE_SENSOR_MAG_UNCALIB (5) --  Uncalibrated magnetometer sensor data service
 */

  /* Optional */
  /*  Service Provider Selection Mask */
  uint8_t providerFlags_valid;  /**< Must be set to true if providerFlags is being passed */
  qmiSlimServiceProviderMaskT_v01 providerFlags;
  /**<
 Service provider selection mask. Valid masks: \n
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SSC (0x0000000000000001) --  SSC provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE (0x0000000000000002) --  SAMLite provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE (0x0000000000000004) --  Native provider can be used
 */

  /* Optional */
  /*  Samples per Batch */
  uint8_t sampleCount_valid;  /**< Must be set to true if sampleCount is being passed */
  uint16_t sampleCount;
  /**<   Sample count. \n
     Note: Maximum sample count per batch is QMI_SLIM_SENSOR_MAX_ SAMPLE_SETS.
    */

  /* Optional */
  /*  Batches per Second */
  uint8_t reportRate_valid;  /**< Must be set to true if reportRate is being passed */
  uint16_t reportRate;
  /**<   Report rate, in batches per second. */

  /* Optional */
  /*  Aligned Batch Start Time struct */
  uint8_t batchAlignReq_valid;  /**< Must be set to true if batchAlignReq is being passed */
  qmiSlimSensorDataBatchAlignReqStructT_v01 batchAlignReq;
  /**<   Batch Start Time and Time Source. */
}qmiSlimEnableSensorDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Enables pedometer data. */
typedef struct {

  /* Optional */
  /*  Service Provider Selection Mask */
  uint8_t providerFlags_valid;  /**< Must be set to true if providerFlags is being passed */
  qmiSlimServiceProviderMaskT_v01 providerFlags;
  /**<
 Service provider selection mask. Valid bitmasks: \n
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SSC (0x0000000000000001) --  SSC provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE (0x0000000000000002) --  SAMLite provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE (0x0000000000000004) --  Native provider can be used
 */

  /* Optional */
  /*  Periodic Reporting Period */
  uint8_t reportPeriod_valid;  /**< Must be set to true if reportPeriod is being passed */
  uint32_t reportPeriod;
  /**<   Specifies the interval for the reporting period, in seconds, in Q16
       number format. \n
        P = 0 for asynchronous reporting.\n
        P > 0 for periodic reporting. Maximum reporting period is 3600 seconds.
    */

  /* Optional */
  /*  Reset Step Count */
  uint8_t resetStepCount_valid;  /**< Must be set to true if resetStepCount is being passed */
  uint8_t resetStepCount;
  /**<   Indicates whether the service must reset the step count.
        This value is only used if reporting is already enabled.
    */

  /* Optional */
  /*  Step Count Threshold */
  uint8_t stepCountThreshold_valid;  /**< Must be set to true if stepCountThreshold is being passed */
  uint32_t stepCountThreshold;
  /**<   Specifies the number of steps to be sampled in a pedometer report.
        If the threshold is set to 0, every step event is reported.
    */

  /* Optional */
  /*  Sample Rate */
  uint8_t sampleRate_valid;  /**< Must be set to true if sampleRate is being passed */
  uint32_t sampleRate;
  /**<   Sampling rate, in Hz, in Q16 number format. Default value is 20 Hz. Range:
     5 to 20 Hz.
    */

  /* Optional */
  /*  Duty Cycle Percent */
  uint8_t dutyCycleOnPercent_valid;  /**< Must be set to true if dutyCycleOnPercent is being passed */
  uint8_t dutyCycleOnPercent;
  /**<   Percentage of the time that the duty cycle is on for pedometer service when it is
       duty-cycled. The sensor stream to the pedometer service is only active
       during the duty cycle ON period. Range: 0 to 100.
    */
}qmiSlimEnablePedometerReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Enables reports of AMD or RMD service data. */
typedef struct {

  /* Mandatory */
  /*  SLIM Service */
  qmiSlimServiceEnumT_v01 slimService;
  /**<
 Service type. Values: \n
      - eQMI_SLIM_SERVICE_AMD (7) --   Absolute motion data service
      - eQMI_SLIM_SERVICE_RMD (8) --    Relative motion data service
 */

  /* Optional */
  /*  Service Provider Selection Mask */
  uint8_t providerFlags_valid;  /**< Must be set to true if providerFlags is being passed */
  qmiSlimServiceProviderMaskT_v01 providerFlags;
  /**<
 Service provider selection mask. Valid masks: \n
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SSC (0x0000000000000001) --  SSC provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE (0x0000000000000002) --  SAMLite provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE (0x0000000000000004) --  Native provider can be used
 */

  /* Optional */
  /*  Report Period */
  uint8_t reportPeriod_valid;  /**< Must be set to true if reportPeriod is being passed */
  uint32_t reportPeriod;
  /**<   Report period, in seconds, in Q16 number format; a value of 0 means
       only reporting new events.
    */

  /* Optional */
  /*  Acceleration Variance Threshold */
  uint8_t varThreshold_valid;  /**< Must be set to true if varThreshold is being passed */
  uint32_t varThreshold;
  /**<   Acceleration variance threshold in Q16 number format, in
        m\textsuperscript{2}/s\textsuperscript{4}; declare motion if the sum
        of variances on all axes exceed this value.
    */

  /* Optional */
  /*  Time Period */
  uint8_t varWinLen_valid;  /**< Must be set to true if varWinLen is being passed */
  uint32_t varWinLen;
  /**<   Time period over which the variance is computed, in seconds, in Q16
       number format.
    */
}qmiSlimEnableQmdDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Request Message; Enables vehicle sensor, speed, differential wheel speed, gear or odometry data. */
typedef struct {

  /* Mandatory */
  /*  Vehicle Service */
  qmiSlimServiceEnumT_v01 slimService;
  /**<   Vehicle service type. Values: \n
      - eQMI_SLIM_SERVICE_VEHICLE_ACCEL (13) --   Vehicle accelerometer data service
      - eQMI_SLIM_SERVICE_VEHICLE_GYRO (14) --  Vehicle angular rate data service
      - eQMI_SLIM_SERVICE_VEHICLE_ODOMETRY (15) --  Vehicle odometry data service
      - eQMI_SLIM_SERVICE_VEHICLE_SPEED (17) --   Vehicle speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_DWS (18) --  Vehicle differential wheel speed data service
      - eQMI_SLIM_SERVICE_VEHICLE_GEAR (19) --  Vehicle gear data service
 */

  /* Optional */
  /*  Service Provider Selection Mask */
  uint8_t providerFlags_valid;  /**< Must be set to true if providerFlags is being passed */
  qmiSlimServiceProviderMaskT_v01 providerFlags;
  /**<   Service provider selection mask. Valid bitmasks: \n
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SSC (0x0000000000000001) --  SSC provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_SAMLITE (0x0000000000000002) --  SAMLite provider can be used
      - QMI_SLIM_SERVICE_PROVIDER_MASK_NATIVE (0x0000000000000004) --  Native provider can be used
 */
}qmiSlimEnableVehicleDataReqMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Reports the sensor service data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Sample Time Offsets */
  uint32_t sampleTimeOffsets_len;  /**< Must be set to # of elements in sampleTimeOffsets */
  uint32_t sampleTimeOffsets[QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
     Variable length array to specify sensor sample time offsets; maximum
     length of the array is QMI_SLIM_SENSOR_ MAX_SAMPLE_SETS.

     Units: 1/32768 seconds (one oscillator clock cycle)
    */

  /* Optional */
  /*  Samples for X-Axis */
  uint8_t samplesX_valid;  /**< Must be set to true if samplesX is being passed */
  uint32_t samplesX_len;  /**< Must be set to # of elements in samplesX */
  float samplesX[QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
     Variable length array to specify sensor sample data;
     every sample must have 1 to 3 data items, and the size of the
     array is equal to the sampleTimeOffsets length.

     Maximum length is QMI_SLIM_ SENSOR_MAX_SAMPLE_SETS.
    */

  /* Optional */
  /*  Samples for Y-Axis */
  uint8_t samplesY_valid;  /**< Must be set to true if samplesY is being passed */
  uint32_t samplesY_len;  /**< Must be set to # of elements in samplesY */
  float samplesY[QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
     Variable length array to specify sensor sample data;
     every sample must have 1 to 3 data items, and the size of the
     array is equal to the sampleTimeOffsets length.

     Maximum length is QMI_SLIM_ SENSOR_MAX_SAMPLE_SETS.
    */

  /* Optional */
  /*  Samples for Z-Axis */
  uint8_t samplesZ_valid;  /**< Must be set to true if samplesZ is being passed */
  uint32_t samplesZ_len;  /**< Must be set to # of elements in samplesZ */
  float samplesZ[QMI_SLIM_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
     Variable length array to specify sensor sample data;
     every sample must have 1 to 3 data items, and the size of the
     array is equal to the sampleTimeOffsets length.

     Maximum length is QMI_SLIM_ SENSOR_MAX_SAMPLE_SETS.
    */
}qmiSlimSensorDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Indicates the pedometer data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Time Interval */
  float timeInterval;
  /**<
     Time interval, in milliseconds, during which the step count was calculated. Subtracting
     timeInterval from the timestamp field yields the time when
     the step detection for the first step in this report started.
    */

  /* Mandatory */
  /*  Step Count */
  uint32_t stepCount;
  /**<
     Number of steps counted during the time interval.
    */

  /* Optional */
  /*  Step Confidence */
  uint8_t stepConfidence_valid;  /**< Must be set to true if stepConfidence is being passed */
  uint8_t stepConfidence;
  /**<
     Confidence associated with the step. This field is only applicable for a
     single step report, that is, if the step count is one. Range: 0 to 100.

     Note: The report is ignored if confidence is 0.
    */

  /* Optional */
  /*  Step Count Uncertainty */
  uint8_t stepCountUncertainty_valid;  /**< Must be set to true if stepCountUncertainty is being passed */
  float stepCountUncertainty;
  /**<   Uncertainty (in steps) associated with the step count. */

  /* Optional */
  /*  Current Rate of Steps Estimate */
  uint8_t stepRate_valid;  /**< Must be set to true if stepRate is being passed */
  float stepRate;
  /**<
     Current estimate for the rate of steps per second.
     Range: @latexonly $\geq$ 0.0. @endlatexonly
    */

  /* Optional */
  /*  Current Stride Length Estimate */
  uint8_t strideLength_valid;  /**< Must be set to true if strideLength is being passed */
  float strideLength;
  /**<   Current stride length estimate in meters per stride.
       Range: @latexonly $\geq$ 0.@endlatexonly
    */

  /* Optional */
  /*  Stride Length Uncertainty */
  uint8_t strideLengthUncertainty_valid;  /**< Must be set to true if strideLengthUncertainty is being passed */
  float strideLengthUncertainty;
  /**<   Uncertainty associated with the stride length, in meters per stride,
      provided at a 1-sigma (68 percent) confidence. Range: @latexonly $\geq$ 0. @endlatexonly
    */

  /* Optional */
  /*  Fractional Portion of the Number of Steps Counted */
  uint8_t stepCountFractional_valid;  /**< Must be set to true if stepCountFractional is being passed */
  float stepCountFractional;
  /**<   Sub-integer portion of the number of steps counted during the time
       interval. If the report is generated on event, this is 0.
       Range: 0.0 to < 1.0.
    */

  /* Optional */
  /*  Type Of Report Generated */
  uint8_t reportType_valid;  /**< Must be set to true if reportType is being passed */
  qmiSlimInjectPedometerReportEnumT_v01 reportType;
  /**<   Type of Report. Values: \n
      - eQMI_SLIM_INJECT_PEDOMETER_REPORT_ON_STEP_EVENT (0) --  Report is injected on detection of a step event
      - eQMI_SLIM_INJECT_PEDOMETER_REPORT_PERIODICALLY (1) --  Report is injected periodically
 */
}qmiSlimPedometerDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Reports AMD or RMD service data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Motion State Output of QMD Algorithm Instance */
  qmiSlimQmdMotionStateEnumT_v01 motionState;
  /**<
 Motion state output of QMD algorithm instance. Values:\n
      - eQMI_SLIM_QMD_MOTION_STATE_UNSPECIFIED (0) --  Motion state is unspecified
      - eQMI_SLIM_QMD_MOTION_STATE_REST (1) --  Rest motion state
      - eQMI_SLIM_QMD_MOTION_STATE_MOVE (2) --  Move motion state
 */
}qmiSlimQmdDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Reports the vehicle sensor data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Sample Time Offsets */
  uint32_t sampleTimeOffset_len;  /**< Must be set to # of elements in sampleTimeOffset */
  uint32_t sampleTimeOffset[QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
    Variable length array to specify the on-vehicle sensor samples.
    This time offset, in microseconds, must be relative to the sensor time
    of the first sample.

    Maximum length is QMI_SLIM_ VEHICLE_SENSOR_MAX_SAMPLE_ SETS.
  */

  /* Optional */
  /*  Front Axis Samples */
  uint8_t sampleX_valid;  /**< Must be set to true if sampleX is being passed */
  uint32_t sampleX_len;  /**< Must be set to # of elements in sampleX */
  float sampleX[QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle sensor samples.

    Units: \n
    - Accel: m/s\textsuperscript{2} \n
    - Gyro: rad/s
  */

  /* Optional */
  /*  Right Axis Samples */
  uint8_t sampleY_valid;  /**< Must be set to true if sampleY is being passed */
  uint32_t sampleY_len;  /**< Must be set to # of elements in sampleY */
  float sampleY[QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle sensor samples.

    Units:\n
    - Accel: m/s\textsuperscript{2} \n
    - Gyro: rad/s
  */

  /* Optional */
  /*  Down Axis Samples */
  uint8_t sampleZ_valid;  /**< Must be set to true if sampleZ is being passed */
  uint32_t sampleZ_len;  /**< Must be set to # of elements in sampleZ */
  float sampleZ[QMI_SLIM_VEHICLE_SENSOR_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle sensor samples.

    Units:\n
    - Accel: m/s\textsuperscript{2}\n
    - Gyro: rad/s
  */
}qmiSlimVehicleSensorDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Reports the vehicle odometry data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Deviation Flags */
  qmiSlimVehicleOdometryMaskT_v01 odometryFlags;
  /**<
 Flags to indicate any deviation from the default measurement assumptions.
 Values: \n
      - QMI_SLIM_MASK_VEHICLE_ODOMETRY_REVERSE_MOVEMENT (0x0000000000000001) --  Odometry data includes at least some data where
       the vehicle might have been moving in the reverse direction. If
       the odometry data might be in reverse, this bit must be set.
       If the odometry data is all in the forward direction, this bit
       must not be set
      - QMI_SLIM_MASK_VEHICLE_ODOMETRY_AFFECTED_BY_ERRORS (0x0000000000000002) --  Odometry data includes at least some data affected
       by a major error source affecting distance-traveled accuracy,
       such as wheel slippage because of skidding, gravel, snow, or ice as
       detected by the vehicle, for example, via an ABS or other system
      - QMI_SLIM_MASK_VEHICLE_ODOMETRY_ABSOLUTE_MEASUREMENT (0x0000000000000004) --  Odometry data is an absolute amount since the vehicle
       began service, and is the same vehicle that is regularly used with
       this device (so that the offset of this value, since the last time
       this measurement was used by the location engine, can be safely used
       as a likely correct estimate of the distance traveled since last
       use)
 */

  /* Mandatory */
  /*  Distance Traveled Base */
  uint32_t distanceTravelledBase;
  /**<
    Units of accumulated distance, in meters. Range: 0 to 4,000,000 km.
  */

  /* Mandatory */
  /*  Sample Time Offset */
  uint32_t sampleTimeOffset_len;  /**< Must be set to # of elements in sampleTimeOffset */
  uint32_t sampleTimeOffset[QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01];
  /**<
     Time offset, in microseconds, relative to the sensor time of the first sample.
  */

  /* Optional */
  /*  Average of Left and Right Traveled Distance */
  uint8_t distanceTravelledAvg_valid;  /**< Must be set to true if distanceTravelledAvg is being passed */
  uint32_t distanceTravelledAvg_len;  /**< Must be set to # of elements in distanceTravelledAvg */
  uint32_t distanceTravelledAvg[QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01];
  /**<
    This measurement (in millimeters) is added to
    the Distance Traveled Base TLV measurement (in meters) to
    get the total distance traveled sample value.

    Range: 0 to 4000 km.
  */

  /* Optional */
  /*  Left Traveled Distance */
  uint8_t distanceTravelledLeft_valid;  /**< Must be set to true if distanceTravelledLeft is being passed */
  uint32_t distanceTravelledLeft_len;  /**< Must be set to # of elements in distanceTravelledLeft */
  uint32_t distanceTravelledLeft[QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01];
  /**<
    This measurement (in  millimeters) is added to
    the Distance Traveled Base TLV measurement (in meters) to
    get the total distance traveled sample value.

    Range: 0 to 4000 km.
  */

  /* Optional */
  /*  Right Traveled Distance */
  uint8_t distanceTravelledRight_valid;  /**< Must be set to true if distanceTravelledRight is being passed */
  uint32_t distanceTravelledRight_len;  /**< Must be set to # of elements in distanceTravelledRight */
  uint32_t distanceTravelledRight[QMI_SLIM_VEHICLE_ODOMETRY_MAX_SAMPLE_SETS_V01];
  /**<
    This measurement (in millimeters) is added to
    the Distance Traveled Base TLV measurement (in meters) to
    get the total distance traveled sample value.

    Range: 0 to 4000 km.
  */
}qmiSlimVehicleOdometryDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/** @addtogroup slim_qmi_messages
    @{
  */
/** Indication Message; Reports the vehicle motion data like speed, differential wheel speed or gear data. */
typedef struct {

  /* Mandatory */
  /*  Service Data Header */
  qmiSlimServiceDataHeaderT_v01 header;

  /* Mandatory */
  /*  Sample Time Offsets */
  uint32_t sampleTimeOffset_len;  /**< Must be set to # of elements in sampleTimeOffset */
  uint32_t sampleTimeOffset[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Variable length array to specify the on-vehicle sensor samples.
    This time offset, in microseconds, must be relative to the sensor time
    of the first sample.

    Maximum length is QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS.
  */

  /* Optional */
  /*  On-Vehicle Speed Samples */
  uint8_t speed_valid;  /**< Must be set to true if speed is being passed */
  uint32_t speed_len;  /**< Must be set to # of elements in speed */
  float speed[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle speed samples in m/s.
  */

  /* Optional */
  /*  On-Vehicle Differential Front Left Wheel speed samples */
  uint8_t dwsFL_valid;  /**< Must be set to true if dwsFL is being passed */
  uint32_t dwsFL_len;  /**< Must be set to # of elements in dwsFL */
  float dwsFL[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle differential front left
    wheel speed samples in m/s.
  */

  /* Optional */
  /*  On-vehicle Differential Front Right Wheel speed samples */
  uint8_t dwsFR_valid;  /**< Must be set to true if dwsFR is being passed */
  uint32_t dwsFR_len;  /**< Must be set to # of elements in dwsFR */
  float dwsFR[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle differential front right
    wheel speed samples in  m/s.
  */

  /* Optional */
  /*  On-vehicle Differential Rear Left Wheel speed samples */
  uint8_t dwsRL_valid;  /**< Must be set to true if dwsRL is being passed */
  uint32_t dwsRL_len;  /**< Must be set to # of elements in dwsRL */
  float dwsRL[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle differential rear left
    wheel speed samples in m/s.
  */

  /* Optional */
  /*  On-vehicle Differential Rear Right Wheel speed samples */
  uint8_t dwsRR_valid;  /**< Must be set to true if dwsRR is being passed */
  uint32_t dwsRR_len;  /**< Must be set to # of elements in dwsRR */
  float dwsRR[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle differential rear right
    wheel speed samples in m/s.
  */

  /* Optional */
  /*  On-vehicle Gear data Samples */
  uint8_t gear_valid;  /**< Must be set to true if gear is being passed */
  uint32_t gear_len;  /**< Must be set to # of elements in gear */
  int8_t gear[QMI_SLIM_VEHICLE_MOTION_DATA_MAX_SAMPLE_SETS_V01];
  /**<
    Optional variable length array to specify the on-vehicle gear samples.
    Gear data is interpreted as 0 – Parked, 1 – Reverse, 2 – Neutral, 3 - Forward.
  */
}qmiSlimVehicleMotionDataIndMsgT_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */
//#define REMOVE_QMI_SLIM_DISABLE_SERVICE_V01
//#define REMOVE_QMI_SLIM_ENABLE_PEDOMETER_V01
//#define REMOVE_QMI_SLIM_ENABLE_QMD_DATA_V01
//#define REMOVE_QMI_SLIM_ENABLE_SENSOR_DATA_V01
//#define REMOVE_QMI_SLIM_ENABLE_VEHICLE_DATA_V01
//#define REMOVE_QMI_SLIM_GET_SUPPORTED_FIELDS_V01
//#define REMOVE_QMI_SLIM_GET_SUPPORTED_MSGS_V01
//#define REMOVE_QMI_SLIM_GET_TIME_UPDATE_V01
//#define REMOVE_QMI_SLIM_PEDOMETER_DATA_V01
//#define REMOVE_QMI_SLIM_QMD_DATA_V01
//#define REMOVE_QMI_SLIM_REGISTER_V01
//#define REMOVE_QMI_SLIM_SENSOR_DATA_V01
//#define REMOVE_QMI_SLIM_SERVICE_ERROR_V01
//#define REMOVE_QMI_SLIM_SERVICE_UPDATE_V01
//#define REMOVE_QMI_SLIM_UNREGISTER_V01
//#define REMOVE_QMI_SLIM_VEHICLE_MOTION_DATA_V01
//#define REMOVE_QMI_SLIM_VEHICLE_ODOMETRY_DATA_V01
//#define REMOVE_QMI_SLIM_VEHICLE_SENSOR_DATA_V01

/*Service Message Definition*/
/** @addtogroup slim_qmi_msg_ids
    @{
  */
#define QMI_SLIM_GET_SUPPORTED_MSGS_REQ_V01 0x001E
#define QMI_SLIM_GET_SUPPORTED_MSGS_RESP_V01 0x001E
#define QMI_SLIM_GET_SUPPORTED_FIELDS_REQ_V01 0x001F
#define QMI_SLIM_GET_SUPPORTED_FIELDS_RESP_V01 0x001F
#define QMI_SLIM_REGISTER_REQ_V01 0x0020
#define QMI_SLIM_REGISTER_RESP_V01 0x0020
#define QMI_SLIM_UNREGISTER_REQ_V01 0x0021
#define QMI_SLIM_UNREGISTER_RESP_V01 0x0021
#define QMI_SLIM_SERVICE_UPDATE_IND_V01 0x0022
#define QMI_SLIM_SERVICE_ERROR_IND_V01 0x0023
#define QMI_SLIM_GET_TIME_UPDATE_REQ_V01 0x0024
#define QMI_SLIM_GET_TIME_UPDATE_RESP_V01 0x0024
#define QMI_SLIM_DISABLE_SERVICE_REQ_V01 0x0025
#define QMI_SLIM_DISABLE_SERVICE_RESP_V01 0x0025
#define QMI_SLIM_ENABLE_SENSOR_DATA_REQ_V01 0x0026
#define QMI_SLIM_ENABLE_SENSOR_DATA_RESP_V01 0x0026
#define QMI_SLIM_SENSOR_DATA_IND_V01 0x0027
#define QMI_SLIM_ENABLE_PEDOMETER_REQ_V01 0x0028
#define QMI_SLIM_ENABLE_PEDOMETER_RESP_V01 0x0028
#define QMI_SLIM_PEDOMETER_DATA_IND_V01 0x0029
#define QMI_SLIM_ENABLE_QMD_DATA_REQ_V01 0x002A
#define QMI_SLIM_ENABLE_QMD_DATA_RESP_V01 0x002A
#define QMI_SLIM_QMD_DATA_IND_V01 0x002B
#define QMI_SLIM_ENABLE_VEHICLE_DATA_REQ_V01 0x002C
#define QMI_SLIM_ENABLE_VEHICLE_DATA_RESP_V01 0x002C
#define QMI_SLIM_VEHICLE_ODOMETRY_DATA_IND_V01 0x002D
#define QMI_SLIM_VEHICLE_SENSOR_DATA_IND_V01 0x002E
#define QMI_SLIM_VEHICLE_MOTION_DATA_IND_V01 0x002F
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro slim_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type slim_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define slim_get_service_object_v01( ) \
          slim_get_service_object_internal_v01( \
            SLIM_V01_IDL_MAJOR_VERS, SLIM_V01_IDL_MINOR_VERS, \
            SLIM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

