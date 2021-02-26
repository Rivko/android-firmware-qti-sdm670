/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_diag_log.h
  @brief
  sensor diag library interface File.
============================================================================*/

#pragma once

#include <unistd.h>
#include <inttypes.h>

#include "sns_diag.pb.h"

using namespace std;

/*
* @brief sensor diag log packet library return codes
*
*/
typedef enum sensors_diag_log_return_code
{
  SENSORS_DIAG_LOG_SUCCESS = 0,
  SENSORS_DIAG_LOG_INVALID_INPUT,
  SENSORS_DIAG_LOG_PKT_ALLOC_FAILED,
  SENSORS_DIAG_LOG_DIAG_INIT_FAIL
} sensors_diag_log_return_code;

/**
* @brief Internal enum values for logging type. This enum can be either request
*        , event or response.
*/
typedef enum
{
  SENSORS_DIAG_LOG_REQUEST  = 100,
  SENSORS_DIAG_LOG_EVENT    = 102,
  SENSORS_DIAG_LOG_RESPONSE = 101
} sensors_diag_logid;

/**
* @brief Class which implements the HLOS diag library
*/
class sensors_diag_log
{
public:
  /**
  * @param[i] in_client_id, Unique client ID sent by client.
  *         This client_id is mapped to client_id in
  *         message sns_diag_client_api_log
  *
  */
  sensors_diag_log(uint64_t in_client_id);
  ~sensors_diag_log();
  /**
   * @brief log_event_msg, logs event message provided by the client to diag/QXDM.
   *
   * @param[i] event_msg. This is mapped to message sns_client_event_msg,
   *        in pb encoded format.
   *
   *
   * @param[i] src_sensor_data_type, is data type of the sensor
   *
   * @param[i] logging_module_type, Name of the logging module.
   *
   * @param[o] sensors_diag_log_return_code. Can be any one of
   *  SENSORS_DIAG_LOG_SUCCESS,
   *  SENSORS_DIAG_LOG_INVALID_INPUT,
   *  SENSORS_DIAG_LOG_PKT_ALLOC_FAILED,
   *  SENSORS_DIAG_LOG_DIAG_INIT_FAIL
   */
  sensors_diag_log_return_code log_event_msg(
      const string &event_msg,
      const string &src_sensor_data_type,
      char *logging_module_type);


  /**
   * @brief log_request_msg, log request messages provided by client
   *        on to diag/QXDM.
   *
   * @param[i] request_msg, This is mapped to message sns_client_request_msg,
   *        in pb encoded format.
   *
   * @param[i] src_sensor_data_type, is data type of the sensor
   *
   * @param[i] logging_module_type, Name of the logging module.
   *
   * @param[o] sensors_diag_log_return_code. Can be any one of
   *  SENSORS_DIAG_LOG_SUCCESS,
   *  SENSORS_DIAG_LOG_INVALID_INPUT,
   *  SENSORS_DIAG_LOG_PKT_ALLOC_FAILED,
   *  SENSORS_DIAG_LOG_DIAG_INIT_FAIL
   */
  sensors_diag_log_return_code log_request_msg(
      const string &request_msg,
      const string &src_sensor_data_type,
      char *logging_module_type);

  /**
   * @brief log_response_msg, log response messages provided by client
   *        on to diag/QXDM.
   *
   * @param[i] response_msg, This is mapped to Client API Response message
   *        as defined in sns_diag.proto, in pb encoded format.
   *
   * @param[i] src_sensor_data_type, is data type of the sensor
   *
   * @param[i] logging_module_type, Name of the logging module.
   *
   * @param[o] sensors_diag_log_return_code. Can be any one of
   *  SENSORS_DIAG_LOG_SUCCESS,
   *  SENSORS_DIAG_LOG_INVALID_INPUT,
   *  SENSORS_DIAG_LOG_PKT_ALLOC_FAILED,
   *  SENSORS_DIAG_LOG_DIAG_INIT_FAIL
   */
  sensors_diag_log_return_code log_response_msg(
      const uint32_t resp_msg_vale,
      const string &src_sensor_data_type,
      char *logging_module_type);

private:

  /**
   * @brief submit_log_packet, Submit the encoded buffer to Diag.
   *
   * @param[i] internal_log_id is the internal enum value which can be any one of
   *        SENSORS_DIAG_LOG_REQUEST
   *        SENSORS_DIAG_LOG_EVENT
   *        SENSORS_DIAG_LOG_RESPONSE
   *
   * @param[i] pb_encoded_sns_diag_sensor_log, Encoded bufffer in string format
   *
   * @param[o] sensors_diag_log_return_code. Can be any one of
   *  SENSORS_DIAG_LOG_SUCCESS,
   *  SENSORS_DIAG_LOG_INVALID_INPUT,
   *  SENSORS_DIAG_LOG_PKT_ALLOC_FAILED,
   *  SENSORS_DIAG_LOG_DIAG_INIT_FAIL
   */

  sensors_diag_log_return_code submit_log_packet(
      sensors_diag_logid internal_log_id,
      string &pb_encoded_sns_diag_sensor_log);



  /**
   * @brief sensors_diag_log_get_diag_log_id is used map internal log code to diag code
   *
   * @param[i] log_id is the internal enum value which can be any one of
   *        SENSORS_DIAG_LOG_REQUEST
   *        SENSORS_DIAG_LOG_EVENT
   *        SENSORS_DIAG_LOG_RESPONSE
   *
   * @return[o] diag log id
   */
  uint32_t sensors_diag_log_get_diag_log_id(
      sensors_diag_logid log_id);

  bool support_diag_logging();

  /* diag_init_flag is used to check Diag_LSM_Init is success or failure. */
  bool diag_flag ;

  /* Unique client ID sent by client.
  *         This client_id is mapped to client_id in
  *         message sns_diag_client_api_log
  */
  uint64_t client_id;

  pthread_mutex_t     _mutex;

};

