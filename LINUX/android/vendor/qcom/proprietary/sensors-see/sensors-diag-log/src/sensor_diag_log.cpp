/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file sensor_diag_log.cpp
  @brief
  sensor diag library implementation.
============================================================================*/



#include "log.h"
#include <android/log.h>
#include <time.h>
#include "../inc/sensor_diag_log.h"
#include "diag_lsm.h"
#include  "comdef.h"
#include "sensors_log.h"
#include "sensors_timeutil.h"
#include <cutils/properties.h>

/* limit log packet size to 12k */
#define SNS_LOG_PKT_UINT64_BUFF_SIZE (1500)

#ifndef LOG_SNS_HLOS_REQUEST_C
#define LOG_SNS_HLOS_REQUEST_C 0x19D9
#endif

#ifndef LOG_SNS_HLOS_EVENT_C
#define LOG_SNS_HLOS_EVENT_C 0x19DA
#endif

#ifndef LOG_SNS_HLOS_RESPONSE_C
#define LOG_SNS_HLOS_RESPONSE_C 0x19FF
#endif

#define CLIENT_MANAGER_SUID_LOW 0x1628E8697D485ECD
#define CLIENT_MANAGER_SUID_HIGH 0xB44E6DDA82EF9B2B
const char SENSORS_HAL_DIAG_SUPPPORT[] = "persist.debug.sensors.hal_diag_logging";

typedef PACK(struct)
{
  uint16_t len;     /* Specifies the length, in bytes of the
           entry, including this header. */

  uint16_t code;      /* Specifies the log code for the entry as
           enumerated above. Note: This is
           specified as word to guarantee size. */

  uint32_t ts[2];     /* The system timestamp for the log entry. The
           upper 48 bits represent elapsed time since
           6 Jan 1980 00:00:00 in 1.25 ms units. The
           low order 16 bits represent elapsed time
           since the last 1.25 ms tick in 1/32 chip
           units (this 16 bit counter wraps at the
           value 49152). */
}sensors_diag_log_hdr_type;

typedef PACK(struct)
{
  sensors_diag_log_hdr_type log_hdr;
  uint64_t buffer[SNS_LOG_PKT_UINT64_BUFF_SIZE];
}sensors_diag_log_pkt;

sensors_diag_log::sensors_diag_log(uint64_t in_client_id)
{
  client_id = in_client_id;
  diag_flag = Diag_LSM_Init(NULL);
  if(diag_flag == false)
    sns_logv("diag_init_flag is failed ");
  diag_flag = support_diag_logging();
  if(diag_flag == false)
    sns_logv("diag_logging is disabled with system config ");
  pthread_mutex_init(&_mutex, NULL);
}
sensors_diag_log::~sensors_diag_log()
{
  Diag_LSM_DeInit();
  pthread_mutex_destroy(&_mutex);
}
bool sensors_diag_log::support_diag_logging()
{
    char diag_logging[PROPERTY_VALUE_MAX];
    property_get(SENSORS_HAL_DIAG_SUPPPORT, diag_logging, "true");
    sns_logd("diag_logging: %s",diag_logging);
    if (!strncmp("false", diag_logging,5)) {
        sns_logi("support_diag_logging : %s",diag_logging);
        return false;
    }
    return true;
}

sensors_diag_log_return_code sensors_diag_log::log_request_msg(
    const string &request_msg,
    const string &src_sensor_data_type,
    char *logging_module_type)
{
  sensors_diag_log_return_code return_code = SENSORS_DIAG_LOG_SUCCESS;
  if(diag_flag == false)
  {
    sns_logv("diag_flag is false");
    return SENSORS_DIAG_LOG_DIAG_INIT_FAIL;
  }
  if(logging_module_type == NULL)
  {
    sns_logv("logging_module_type is not valid");
    return SENSORS_DIAG_LOG_INVALID_INPUT;
  }
  if(!log_status(sensors_diag_log_get_diag_log_id(SENSORS_DIAG_LOG_REQUEST)))
  {
    sns_logv("diag_log_id is not enabled");
    return SENSORS_DIAG_LOG_PKT_ALLOC_FAILED;
  }
  pthread_mutex_lock(&_mutex);
  sns_diag_client_api_log client_api_log_req_msg;
  sns_diag_sensor_log sensor_log_req_msg;

  sns_std_suid suid;
  sns_client_request_msg client_req_msg ;
  client_req_msg.ParseFromString(request_msg);
  suid.set_suid_low((uint64_t)CLIENT_MANAGER_SUID_LOW);
  suid.set_suid_high((uint64_t)CLIENT_MANAGER_SUID_HIGH);

  client_api_log_req_msg.set_client_id(client_id);
  client_api_log_req_msg.set_src_sensor_type(src_sensor_data_type.c_str());
  client_api_log_req_msg.set_allocated_request_payload(&client_req_msg);

  string pb_encoded_sns_diag_sensor_log;
  uint64_t time_stamp = sensors_timeutil::get_instance().qtimer_get_ticks();
  std::string sensor_data_type_str(logging_module_type);

  sensor_log_req_msg.set_log_id(SENSORS_DIAG_LOG_REQUEST);
  sensor_log_req_msg.set_timestamp(time_stamp);
  sensor_log_req_msg.set_allocated_suid(&suid);
  sensor_log_req_msg.set_allocated_sensor_type(&sensor_data_type_str);
  sensor_log_req_msg.set_instance_id((uint64_t)this);
  sensor_log_req_msg.set_allocated_client_api_payload(&client_api_log_req_msg);
  sensor_log_req_msg.SerializeToString(&pb_encoded_sns_diag_sensor_log);

  sensor_log_req_msg.release_suid();
  sensor_log_req_msg.release_sensor_type();
  client_api_log_req_msg.release_request_payload();

  sensor_log_req_msg.release_client_api_payload();

  return_code = submit_log_packet(SENSORS_DIAG_LOG_REQUEST,
      pb_encoded_sns_diag_sensor_log);

  pthread_mutex_unlock(&_mutex);
  sns_logv("%s Ended" , __func__);
  return return_code;
}
sensors_diag_log_return_code sensors_diag_log::log_event_msg(
    const string &event_msg,
    const string &src_sensor_data_type,
    char *logging_module_type)
{
  sensors_diag_log_return_code return_code = SENSORS_DIAG_LOG_SUCCESS;
  if(diag_flag == false)
  {
    sns_logv("diag_flag is false");
    return SENSORS_DIAG_LOG_DIAG_INIT_FAIL;
  }
  if(logging_module_type == NULL)
  {
    sns_logv("logging_module_type is not valid");
    return SENSORS_DIAG_LOG_INVALID_INPUT;
  }
  if(!log_status(sensors_diag_log_get_diag_log_id(SENSORS_DIAG_LOG_EVENT)))
  {
    sns_logv("diag_log_id is not enabled");
    return SENSORS_DIAG_LOG_PKT_ALLOC_FAILED;
  }
  pthread_mutex_lock(&_mutex);
  sns_diag_client_api_log client_api_log_event_msg;
  sns_diag_sensor_log sensor_log_event_msg;

  sns_std_suid suid;
  sns_client_event_msg client_event_msg;
  client_event_msg.ParseFromString(event_msg);
  suid.set_suid_low((uint64_t)CLIENT_MANAGER_SUID_LOW);
  suid.set_suid_high((uint64_t)CLIENT_MANAGER_SUID_HIGH);

  client_api_log_event_msg.set_client_id(client_id);
  client_api_log_event_msg.set_src_sensor_type(src_sensor_data_type.c_str());
  client_api_log_event_msg.set_allocated_event_payload(&client_event_msg);

  string pb_encoded_sns_diag_sensor_log;

  uint64_t time_stamp = sensors_timeutil::get_instance().qtimer_get_ticks();
  std::string sensor_data_type_str(logging_module_type);

  sensor_log_event_msg.set_log_id(SENSORS_DIAG_LOG_EVENT);
  sensor_log_event_msg.set_timestamp(time_stamp);
  sensor_log_event_msg.set_allocated_suid(&suid);
  sensor_log_event_msg.set_allocated_sensor_type(&sensor_data_type_str);
  sensor_log_event_msg.set_instance_id((uint64_t)this);
  sensor_log_event_msg.set_allocated_client_api_payload(&client_api_log_event_msg);
  sensor_log_event_msg.SerializeToString(&pb_encoded_sns_diag_sensor_log);

  sensor_log_event_msg.release_suid();
  sensor_log_event_msg.release_sensor_type();
  client_api_log_event_msg.release_event_payload();

  sensor_log_event_msg.release_client_api_payload();

  return_code = submit_log_packet(SENSORS_DIAG_LOG_EVENT,
      pb_encoded_sns_diag_sensor_log);
  pthread_mutex_unlock(&_mutex);
  return return_code;
}

sensors_diag_log_return_code sensors_diag_log::log_response_msg(
    const uint32_t resp_msg_vale,
    const string &src_sensor_data_type,
    char *logging_module_type)
{
  sensors_diag_log_return_code return_code = SENSORS_DIAG_LOG_SUCCESS;
  if(diag_flag == false)
  {
    sns_logv("diag_flag is false");
    return SENSORS_DIAG_LOG_DIAG_INIT_FAIL;
  }
  if(logging_module_type == NULL)
  {
    sns_logv("logging_module_type is not valid");
    return SENSORS_DIAG_LOG_INVALID_INPUT;
  }
  if(!log_status(sensors_diag_log_get_diag_log_id(SENSORS_DIAG_LOG_RESPONSE)))
  {
    sns_logv("diag_log_id is not enabled");
    return SENSORS_DIAG_LOG_PKT_ALLOC_FAILED;
  }
  pthread_mutex_lock(&_mutex);
  sns_diag_client_api_log client_api_log_req_msg;
  sns_diag_sensor_log sensor_log_req_msg;

  sns_std_suid suid;
  sns_diag_client_resp_msg client_resp_msg ;
  client_resp_msg.set_error((sns_std_error)resp_msg_vale);
  suid.set_suid_low((uint64_t)CLIENT_MANAGER_SUID_LOW);
  suid.set_suid_high((uint64_t)CLIENT_MANAGER_SUID_HIGH);

  client_api_log_req_msg.set_client_id(client_id);
  client_api_log_req_msg.set_src_sensor_type(src_sensor_data_type.c_str());
  client_api_log_req_msg.set_allocated_resp_payload(&client_resp_msg);

  string pb_encoded_sns_diag_sensor_log;
  uint64_t time_stamp = sensors_timeutil::get_instance().qtimer_get_ticks();
  std::string sensor_data_type_str(logging_module_type);

  sensor_log_req_msg.set_log_id(SENSORS_DIAG_LOG_RESPONSE);
  sensor_log_req_msg.set_timestamp(time_stamp);
  sensor_log_req_msg.set_allocated_suid(&suid);
  sensor_log_req_msg.set_allocated_sensor_type(&sensor_data_type_str);
  sensor_log_req_msg.set_instance_id((uint64_t)this);
  sensor_log_req_msg.set_allocated_client_api_payload(&client_api_log_req_msg);
  sensor_log_req_msg.SerializeToString(&pb_encoded_sns_diag_sensor_log);

  sensor_log_req_msg.release_suid();
  sensor_log_req_msg.release_sensor_type();
  client_api_log_req_msg.release_resp_payload();

  sensor_log_req_msg.release_client_api_payload();

  return_code = submit_log_packet(SENSORS_DIAG_LOG_RESPONSE,
      pb_encoded_sns_diag_sensor_log);
  pthread_mutex_unlock(&_mutex);
  sns_logv("%s Ended" , __func__);
  return return_code;
}
sensors_diag_log_return_code sensors_diag_log::submit_log_packet(
    sensors_diag_logid internal_log_id,
    string &pb_encoded_sns_diag_sensor_log)
{
  sensors_diag_log_return_code return_code = SENSORS_DIAG_LOG_SUCCESS;

  /*Get diag_log_id from log_id*/
  uint32_t diag_log_id = sensors_diag_log_get_diag_log_id(internal_log_id);
  if(log_status(diag_log_id))
  {
    sns_logv("%d is enabled for logging" , diag_log_id);
  }
  else
  {
    sns_logv("%d is disabled for logging" , diag_log_id);
  }
  /*Alloc diag memory fir diag_log_id */
  uint32_t bytes_written;
  uint64_t max_buffer_size = sizeof(uint64_t)*SNS_LOG_PKT_UINT64_BUFF_SIZE;
  if(pb_encoded_sns_diag_sensor_log.size() < max_buffer_size)
  {
    bytes_written = pb_encoded_sns_diag_sensor_log.size();
  }
  else
  {
    bytes_written = max_buffer_size;
  }
  sns_logv("max_buffer_size %ld , bytes_written %d",
      (long int)max_buffer_size , (bytes_written));

  sensors_diag_log_pkt *log_pkt = NULL;
  uint32_t log_size = bytes_written + sizeof(sensors_diag_log_hdr_type);
  log_pkt = (sensors_diag_log_pkt *)log_alloc(diag_log_id,log_size);
  if(NULL == log_pkt)
  {
    sns_logv("failed to alloc log pck for diag");
    return_code = SENSORS_DIAG_LOG_PKT_ALLOC_FAILED;
  }
  else
  {
    memcpy(log_pkt->buffer, pb_encoded_sns_diag_sensor_log.c_str(),bytes_written);
    log_commit(log_pkt);
    /*log pkt has been submitted to Diag */
    sns_logv("Log pkt submitted to diag");
  }

  return return_code;
}

uint32_t sensors_diag_log::sensors_diag_log_get_diag_log_id(
    sensors_diag_logid log_id)
{
  if( log_id == SENSORS_DIAG_LOG_REQUEST)
    return LOG_SNS_HLOS_REQUEST_C; //0x19D9

  else if( log_id == SENSORS_DIAG_LOG_EVENT)
    return LOG_SNS_HLOS_EVENT_C; //0x19DA

  else if( log_id == SENSORS_DIAG_LOG_RESPONSE)
    return LOG_SNS_HLOS_RESPONSE_C;

  else
    return 0;
}
