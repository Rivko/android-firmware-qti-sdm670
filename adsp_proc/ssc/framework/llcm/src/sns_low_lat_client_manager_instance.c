/*=============================================================================
  @file sns_low_lat_client_manager_instance.c

  This module provides low latency sensor data channel by works with few
  other modules.
  This module receive sensor request via fastRPC call and write sensor
  data into shared memory.  Some APIs of this module are exported and
  called by fastRPC remote functions, these exported APIs receive sensor
  requests and this module will generate sensor client requests accordingly
  to sensor framework, then receive sensor events from sensor framework
  and write sensor data into shared memory which accessible to sensor data
  consumer.

  This file handles the sensor instance part of the module implementation.

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_assert.h"
#include "sns_attribute_util.h"
#include "sns_cal.pb.h"
#include "sns_client.pb.h"
#include "sns_data_stream.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_log.h"
#include "sns_log_codes.h"
#include "sns_math_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_low_lat_buffer_android.h"
#include "sns_low_lat_client_manager.h"
#include "sns_low_lat_client_manager_instance.h"

/*=============================================================================
  Macro definitions
  ===========================================================================*/
// Version 3 is for the SNS Low Latency client manager
#define SNS_LOG_LLCM_LOG_VERSION     3
#define SNS_LLCM_SAMPLE_DATA_SIZE    6
/*=============================================================================
  Type definitions
  ===========================================================================*/

/*============================================================================
  Log Packet Definitions
  ===========================================================================*/
/* 
 * LLCM Client Request Log Packet 
 * This message defines the fields used to log information
 * pertaining to client request
 */
typedef SNS_PACK(struct)
{
  sns_log_hdr_type log_hdr;   /* Log header */
  uint32_t version;           /* Version of the log packet */
  uint64_t timestamp;         /* Timestamp this log packet was generated(Qtimer 19.2 MHz) */  
  uint64_t suid_low;          /* Sensor SUID */
  uint64_t suid_high;         /* Sensor SUID */    
  int64_t timestamp_offset_ns;/* Timestamp offset between AP and SSC */
  uint32_t channel_id;        /* Channel being requested */
  int32_t  sensor;            /* Sensor identifier used in Android */
  uint32_t sample_period_us;  /* Sampling period of sensor */
  uint32_t output_buf_most_recent_idx; /* Data index of channel at time of request */
  uint32_t output_buf_max_samples;     /* Maxinum number of channel of channel */
  uint32_t atomic_counter;    /* Monotonically increase counter  */
  uint32_t cur_client_cnt;    /* Number of clients at time of request */
  bool calibrated_stream;     /* Request calibrated sensor stream */
  bool interrupt_enabled;     /* Fire interrupt after sensor data write into buffer */
} sns_llcm_client_req_log;

/* 
 * LLCM Data Log Packet 
 * This message defines the fields used to log data which be 
 * written into shared memory
 */
typedef SNS_PACK(struct)
{
  sns_log_hdr_type log_hdr;   /* Log header */
  uint32_t version;           /* Version of the log packet */
  uint64_t log_timestamp;     /* Timestamp this log packet was generated(Qtimer 19.2 MHz) */
  uint32_t channel_id;        /* Channel_id that this data belongs to */
  int32_t sensor;             /* Sensor identifier used in Android */
  uint32_t sensor_type;       /* Sensor type */
  uint32_t atomic_counter;    /* Monotonically increase counter to indicate event sequence */
  int64_t timestamp;          /* Event Timestamp in nanosecond */
  float data[SNS_LLCM_SAMPLE_DATA_SIZE];  /* Sensor sample*/
} sns_llcm_data_log;

/*=============================================================================
  Static Sensor  Function Definitions
  ===========================================================================*/
extern sns_llcm_channel sns_llcm_channel_list[];
extern sns_osa_lock llcm_stream_op_lock;

/*===========================================================================
  Public Function Definitions
  ===========================================================================*/


/*=============================================================================
  Static Sensor Function Definitions
  ===========================================================================*/
/**
 * Handles the client instance deinit.
 * Clears out all the data streams associated with all the requests.
 * Then calls the disconnect handler which clears all local requests and
 * batch buffers.
 *
 * @param[i] instance   The client instance which is being removed.
 */
static sns_rc sns_llcm_inst_process_deinit(
        sns_sensor_instance *instance)
{
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)instance->state->state;
  sns_isafe_list_iter iter;
  uint32_t      i;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_llcm_sensor_client *sensor_client;
  sns_service_manager *manager =
   instance->cb->get_service_manager(instance);
  sns_stream_service *stream_service =
   (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

  sns_osa_lock_acquire(inst_state->client_list_lock);
  for (i = 0; i < SNS_LLCM_MAX_CHANNELS; i++)
  {
    /* Remove all the streams */
    for (sns_isafe_list_iter_init(&iter,
        &sns_llcm_channel_list[i].sensor_client_list, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sensor_client = (sns_llcm_sensor_client*)
        sns_isafe_list_iter_get_curr_data(&iter);
      if(NULL != sensor_client)
      {
        /* Delete the sensor client from the queue */
        sns_isafe_list_iter_remove(&iter);

        /* Stop data streams */
        sns_osa_lock_acquire(&llcm_stream_op_lock);
        if(NULL != sensor_client->raw_data_stream)
        {
          stream_service->api->remove_stream(stream_service, sensor_client->raw_data_stream);
        }
        if(NULL != sensor_client->resampler_stream)
        {
          stream_service->api->remove_stream(stream_service, sensor_client->resampler_stream);
        }
        sns_osa_lock_release(&llcm_stream_op_lock);
        sns_free(sensor_client);
      }
    }
    /* Clean channel information. */
    sns_memset(&(sns_llcm_channel_list[i]), 0, sizeof(sns_llcm_channel_list[i]));
  }
  sns_osa_lock_release(inst_state->client_list_lock);

  sns_osa_lock_acquire(&llcm_stream_op_lock);
  if (NULL != inst_state->accel_cal_stream)
  {
    stream_service->api->remove_stream(stream_service, inst_state->accel_cal_stream);
  }
  if (NULL != inst_state->mag_cal_stream)
  {
    stream_service->api->remove_stream(stream_service, inst_state->mag_cal_stream);
  }
  if (NULL != inst_state->gyro_cal_stream)
  {
    stream_service->api->remove_stream(stream_service, inst_state->gyro_cal_stream);
  }
  sns_osa_lock_release(&llcm_stream_op_lock);
 
  /* De-attach from the interrupt */
  if (NULL != inst_state->interrupt_handle)
  {
    DAL_DeviceDetach(inst_state->interrupt_handle);
    inst_state->interrupt_handle = NULL;
  }

  return rv;
}
/*=============================================================================
  Static Sensor Function Definitions
  ===========================================================================*/
/*
 * Function to decode the sensor calibration data event.
 */
sns_rc sns_llcm_inst_decode_cal_event(
    sns_sensor_instance *this,
    void                *event,
    uint32_t            event_len,
    sns_time            timestamp,
    float               *cal_bias,
    sns_std_sensor_sample_status *cal_status)
{
  UNUSED_VAR(timestamp);
  sns_rc rc = SNS_RC_SUCCESS;
  float bias[3] = { 0 };
  float scale_factor[3] = { 0 };
  float comp_matrix[3 * 3] = { 0 };

  uint8_t arr_bias_index = 0;
  uint8_t arr_scale_factor_index = 0;
  uint8_t arr_comp_matrix_index = 0;
  uint8_t i;

  sns_cal_event imu_event = sns_cal_event_init_default;

  pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event,
                                               event_len);
  pb_float_arr_arg arg_bias = {
    .arr = bias,
    .arr_len = ARR_SIZE(bias),
    .arr_index = &arr_bias_index
  };

  pb_float_arr_arg arg_scale_factor = {
    .arr = scale_factor,
    .arr_len = ARR_SIZE(scale_factor),
    .arr_index = &arr_scale_factor_index
  };
  pb_float_arr_arg arg_comp_matrix = {
    .arr = comp_matrix,
    .arr_len = ARR_SIZE(comp_matrix),
    .arr_index = &arr_comp_matrix_index
  };

  imu_event.bias = (struct pb_callback_s){
    .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg_bias
  };

  imu_event.scale_factor = (struct pb_callback_s){
    .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg_scale_factor
  };
  imu_event.comp_matrix = (struct pb_callback_s){
    .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg_comp_matrix
  };

  if (!pb_decode(&stream, sns_cal_event_fields, &imu_event))
  {
    SNS_INST_PRINTF(ERROR, this, "pb_decode() failed for calibration event");
    return SNS_RC_FAILED;
  }

  if (arr_bias_index > 0)
  {
    *cal_status = imu_event.status;
  }
  /* Currently only interested in bias */
  for (i = 0; i < arr_bias_index; i++)
  {
    cal_bias[i] = bias[i];
  }

  return rc;
}

/*
 * Function to mark sensor data type from data stream.
 */
static sns_rc sns_llcm_inst_get_datatype(
        sns_sensor_instance *const this,
        sns_data_stream        *data_stream,
        int                    *sensor_type,
        bool                   calibrated_stream)
{
  sns_rc             rc = SNS_RC_SUCCESS;
  int                data_type;
  sns_fw_data_stream *fw_stream = (sns_fw_data_stream *)data_stream;
  sns_fw_sensor      *src_sensor = fw_stream->data_source;
  char               datatype_str[32];

  SNS_ASSERT(sensor_type);
  sns_attr_info_get_data_type(src_sensor->attr_info,
      datatype_str, sizeof(datatype_str));
  if (0 == strncmp(datatype_str, "accel", strlen("accel")))
  {
    if(calibrated_stream)
    {
      data_type = SENSOR_TYPE_ACCELEROMETER;
    }
    else
    {
      data_type = SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED;
    }
  }
  else if (0 == strncmp(datatype_str, "gyro", strlen("gyro")))
  {
    if(calibrated_stream)
    {
      data_type = SENSOR_TYPE_GYROSCOPE;
    }
    else
    {
      data_type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
    }
  }
  else if (0 == strncmp(datatype_str, "mag", strlen("mag")))
  {
    if(calibrated_stream)
    {
      data_type = SENSOR_TYPE_MAGNETIC_FIELD;
    }
    else
    {
      data_type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
    }
  }
  else
  {
    data_type = SENSOR_TYPE_UNKNOWN;
    rc = SNS_RC_NOT_SUPPORTED;
    SNS_INST_PRINTF(ERROR, this, "Unknow sensor data type");
  }
  SNS_INST_PRINTF(HIGH, this, "Sensor data type = %d", data_type);
  *sensor_type = data_type;
  return rc;
}

/*
 * Send configure request to calibration sensor.
 */
static sns_rc sns_llcm_inst_send_config_request(
        sns_sensor_instance *const this,
        sns_data_stream  *data_stream)
{
  sns_rc rc = SNS_RC_SUCCESS;
  size_t encoded_len;
  uint8_t buffer[100];
  sns_std_sensor_config cal_cfg = sns_std_sensor_config_init_default;

  cal_cfg.sample_rate = 0.0;
  encoded_len = pb_encode_request(
        buffer, sizeof(buffer), &cal_cfg,
        sns_std_sensor_config_fields, NULL);
  if (0 < encoded_len)
  {
    sns_request request = (sns_request)
    {
    .message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG,
    .request_len = encoded_len,
    .request = buffer
    };
    rc = data_stream->api->send_request(data_stream, &request);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "failed to encode LLCM config request");
    rc = SNS_RC_FAILED;
  }

  return rc;
}

/*
 * Send request to resampler sensor.
 */
static inline sns_rc sns_llcm_inst_send_resampler_request(
        sns_sensor_instance *const this,
        sns_llcm_sensor_client     *sensor_client,
        float                      sample_rate)
{
  sns_rc rc = SNS_RC_SUCCESS;
  size_t encoded_len;
  uint8_t buffer[100];
  sns_std_request resampler_std_req = sns_std_request_init_default;
  sns_resampler_config resamp_config=sns_resampler_config_init_default;

  sns_memset(buffer, 0, sizeof(buffer));

  sns_memscpy(&resamp_config.sensor_uid,
                 sizeof(resamp_config.sensor_uid),
                 &sensor_client->suid,
                 sizeof(sensor_client->suid));

  resamp_config.resampled_rate = sample_rate;
  resamp_config.rate_type = SNS_RESAMPLER_RATE_MINIMUM;
  resamp_config.filter = false;
  resamp_config.has_event_gated = false;

  resampler_std_req.has_batching = false;
  resampler_std_req.batching.batch_period = 0;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
        &resamp_config, sns_resampler_config_fields, &resampler_std_req);

  if (0 < encoded_len)
  {
    sns_request request = (sns_request){
    .message_id = SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG,
    .request_len = encoded_len, .request = buffer };
    rc = sensor_client->resampler_stream->api->send_request(sensor_client->resampler_stream, &request);
    if (SNS_RC_SUCCESS != rc)
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to send resampler request rc=%d", rc);
    }
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Failed to encode resampler config rc=%d", rc);
  }

  return rc;
}

/*
 * Send request to physical sensor.
 */
static inline sns_rc sns_llcm_inst_send_sensor_request(
        sns_sensor_instance *const this,
        sns_data_stream            *data_stream,
        float                      sample_rate)
{
  sns_rc rc = SNS_RC_SUCCESS;
  size_t encoded_len;
  uint8_t buffer[100];
  sns_std_request std_request = sns_std_request_init_default;
  sns_std_sensor_config sensor_config = sns_std_sensor_config_event_init_default;

  std_request.has_batching = false;
  sensor_config.sample_rate = sample_rate;
  encoded_len = pb_encode_request(buffer, sizeof(buffer), &sensor_config,
                            sns_std_sensor_config_fields, &std_request);
  if (0 < encoded_len)
  {
    sns_request request = (sns_request)
    {
      .message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG,
      .request_len = encoded_len,
      .request = buffer
    };
    rc = data_stream->api->send_request(data_stream, &request);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "failed to encode LLCM event request");
    rc = SNS_RC_FAILED;
  }

  return rc;
}

/*
 * Request sensor data use given configuration.
 */
static sns_rc sns_llcm_inst_request_sensor(
        sns_sensor_instance *const this,
        sns_stream_service         *stream_service,
        sns_llcm_sensor_client     *sensor_client,
        sns_llcm_config_req        *llcm_config_req)
{
  sns_rc  rc = SNS_RC_SUCCESS;
  int     sensor_type;

  /* Create the data stream for the request */
  rc = stream_service->api->create_sensor_instance_stream(stream_service,
    this, *((sns_sensor_uid*)&sensor_client->suid), &sensor_client->raw_data_stream);
  if (NULL == sensor_client->raw_data_stream || SNS_RC_SUCCESS != rc)
  {
    SNS_INST_PRINTF(ERROR, this, "Stream creation failed rc =%d", rc);
    return SNS_RC_FAILED;
  }

  /* Get sensor data type */
  rc = sns_llcm_inst_get_datatype(this,
      sensor_client->raw_data_stream, &sensor_type, llcm_config_req->calibrated_stream);
  if (SNS_RC_SUCCESS != rc)
  {
    return rc;
  }
  sensor_client->sensor_type = sensor_type;

  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;

  if(inst_state->registry_config.qc_vr_mode)
  {
    rc = sns_llcm_inst_send_sensor_request(this,
          sensor_client->raw_data_stream,
          (float)(SNS_LLCM_US_PER_SEC / llcm_config_req->sample_period_us));
    if (SNS_RC_SUCCESS != rc)
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to request sensor data rc=%d", rc);
    }
  }    
  else /* Go though resampler if not QC VR */
  {

    rc = stream_service->api->create_sensor_instance_stream(stream_service,
          this, inst_state->resampler_suid, &sensor_client->resampler_stream);
    if (NULL == sensor_client->resampler_stream || SNS_RC_SUCCESS != rc)
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to create resampler stream rc=%d", rc);
      return SNS_RC_NOT_AVAILABLE;
    }

    rc = sns_llcm_inst_send_resampler_request(this,
          sensor_client,
          (float)(SNS_LLCM_US_PER_SEC / llcm_config_req->sample_period_us));
    if (SNS_RC_SUCCESS != rc)
    {
      SNS_INST_PRINTF(ERROR, this, "Failed to request resampler rc=%d", rc);
    }
  }
  return rc;
}

/*
 * Request calibration sensor for given sensor type, calibration sensor data streams
 * are shared over LLCM instance, do nothing if a data stream is already exist.
 */
static sns_rc sns_llcm_inst_request_calibration(
        sns_sensor_instance *const this,
        sns_stream_service  *stream_service,
        int                 sensor_type)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;

  if (SENSOR_TYPE_GYROSCOPE == sensor_type ||
        SENSOR_TYPE_GYROSCOPE_UNCALIBRATED == sensor_type)
  {
    if (NULL == inst_state->gyro_cal_stream)
    {
      rc = stream_service->api->create_sensor_instance_stream(stream_service,
        this, inst_state->gyro_cal_suid, &inst_state->gyro_cal_stream);
      if (NULL != inst_state->gyro_cal_stream)
      {
        rc = sns_llcm_inst_send_config_request(this, inst_state->gyro_cal_stream);
      }
      if (SNS_RC_SUCCESS != rc)
      {
        if(NULL != inst_state->gyro_cal_stream)
        {
          stream_service->api->remove_stream(stream_service, inst_state->gyro_cal_stream);
        }
        SNS_INST_PRINTF(ERROR, this, "Failed to request gyro_cal, rc=%d", rc);
        return SNS_RC_FAILED;
      }
    }
  }
  else if (SENSOR_TYPE_MAGNETIC_FIELD == sensor_type ||
        SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED == sensor_type)
  {
    /* Mag cal is optional, check suid availability first */
    if (inst_state->mag_cal_available && NULL == inst_state->mag_cal_stream)
    {
      rc = stream_service->api->create_sensor_instance_stream(stream_service,
        this, inst_state->mag_cal_suid, &inst_state->mag_cal_stream);
      if (NULL != inst_state->mag_cal_stream)
      {
        rc = sns_llcm_inst_send_config_request(this, inst_state->mag_cal_stream);
      }
      if (SNS_RC_SUCCESS != rc)
      {
        if(NULL != inst_state->mag_cal_stream)
        {
          stream_service->api->remove_stream(stream_service, inst_state->mag_cal_stream);
        }
        SNS_INST_PRINTF(ERROR, this, "Failed to request mag_cal, rc=%d", rc);
        return SNS_RC_FAILED;
      }
    }
  }
  else if (SENSOR_TYPE_ACCELEROMETER == sensor_type ||
        SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED == sensor_type)
  {
    /* Accel cal is optional, check suid availability first */
    if (inst_state->accel_cal_available && NULL == inst_state->accel_cal_stream)
    {
      rc = stream_service->api->create_sensor_instance_stream(stream_service,
        this, inst_state->accel_cal_suid, &inst_state->accel_cal_stream);
      /* 
       * Continue if accel cal not available
       * This allow the process move on before accel cal sensor implementaion is done.
       */
      if (SNS_RC_NOT_AVAILABLE == rc)
      {
        SNS_INST_PRINTF(ERROR, this, "accel_cal stream not available");
        inst_state->accel_cal_status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
        rc = SNS_RC_SUCCESS;
      }
      else if (NULL != inst_state->accel_cal_stream)
      {
        rc = sns_llcm_inst_send_config_request(this, inst_state->accel_cal_stream);
      }
      if (SNS_RC_SUCCESS != rc)
      {
        if(NULL != inst_state->accel_cal_stream)
        {
          stream_service->api->remove_stream(stream_service, inst_state->accel_cal_stream);
        }
        SNS_INST_PRINTF(ERROR, this, "Failed to request acc_cal, rc=%d", rc);
        return SNS_RC_FAILED;
      }
    }
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Not supported data type %d", sensor_type);
    rc = SNS_RC_NOT_SUPPORTED;
  }

  return rc;
}

/*
 * Update calibration stream use count.
 */
static void sns_llcm_inst_update_cal_use_cnt(
        sns_llcm_inst_state *inst_state,
        int                 sensor_type,
        bool                add_cnt)
{
  if (SENSOR_TYPE_GYROSCOPE == sensor_type ||
            SENSOR_TYPE_GYROSCOPE_UNCALIBRATED == sensor_type)
  {
    add_cnt ? (inst_state->gyro_cal_use_cnt++) : (inst_state->gyro_cal_use_cnt--);
  }
  else if (SENSOR_TYPE_MAGNETIC_FIELD == sensor_type ||
      SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED == sensor_type)
  {
    add_cnt ? (inst_state->mag_cal_use_cnt++) : (inst_state->mag_cal_use_cnt--);
  }
  else if (SENSOR_TYPE_ACCELEROMETER == sensor_type ||
      SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED == sensor_type)
  {
    add_cnt ? (inst_state->accel_cal_use_cnt++) : (inst_state->accel_cal_use_cnt--);
  }
}

/* Only fire interrupt in new platform which include CDSP. */
#ifdef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
/*
 * Attach to interrupt device.
 */
static sns_rc sns_llcm_inst_attach_interrupt(
        sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  DALResult dal_err;
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;

  if (NULL == inst_state->interrupt_handle)
  {
    dal_err = DAL_DeviceAttach(DALDEVICEID_IPCINT, &inst_state->interrupt_handle);
    if (DAL_SUCCESS != dal_err )
    {
      SNS_INST_PRINTF(ERROR, this, "DAL_DeviceAttach err %d", dal_err);
      return SNS_RC_FAILED;
    }
    dal_err = DalIPCInt_IsSupported( inst_state->interrupt_handle,
        DALIPCINT_PROC_CDSP, DALIPCINT_GP_1 );
    if (DAL_SUCCESS != dal_err )
    {
      SNS_INST_PRINTF(ERROR, this, "DalIPCInt_IsSupported err %d", dal_err);
      return SNS_RC_FAILED;
    }
    SNS_INST_PRINTF(MED, this, "Interrupt attached");
  }
  return rc;
}

/*
 * Fire an interrupt.
 */
static void sns_llcm_inst_notify_data_ready(
        sns_llcm_inst_state *inst_state)
{
  /* Send data ready interrupt to CDSP */
  if (NULL != inst_state->interrupt_handle)
  {
    DalIPCInt_Trigger(inst_state->interrupt_handle,
        DALIPCINT_PROC_CDSP,
        DALIPCINT_GP_1);
  }
}
#else

/*
 * Attach to interrupt device (stub functions for platform not support interrupt to CDSP).
 */
static sns_rc sns_llcm_inst_attach_interrupt(sns_sensor_instance *const this)
{
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;

  inst_state->interrupt_handle = NULL;
  SNS_INST_PRINTF(ERROR, this, "This interrupt is not supported in current platform");
  return SNS_RC_NOT_SUPPORTED;
}

/*
 * Fire an interrupt (stub functions for platform not support interrupt to CDSP).
 */
static void sns_llcm_inst_notify_data_ready(
        sns_llcm_inst_state *inst_state)
{
  UNUSED_VAR(inst_state);
  return;
}
#endif /* !SSC_TARGET_HEXAGON_CORE_QDSP6_2_0 */

/*
 * Log request of client config change .
 */
static void sns_llcm_inst_log_client_req(
        sns_sensor_instance *const this,
        sns_llcm_config_req *llcm_config_req,
        sns_llcm_channel    *llcm_channel)
{
  sns_isafe_list_iter iter;
  sns_llcm_client_req_log *log = NULL;
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;
  
  log = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_llcm_client_req_log));
  if(NULL == log)
  {
    SNS_INST_PRINTF(ERROR, this, "Unable to allocate log packet for client request");
  }
  else
  {
    log->version = SNS_LOG_LLCM_LOG_VERSION;
    log->timestamp = sns_get_system_time();
    log->channel_id = llcm_config_req->channel_id;
    log->suid_low = llcm_config_req->sensor_suid->suid_low;
    log->suid_high = llcm_config_req->sensor_suid->suid_high;
    log->sensor = llcm_config_req->sensor_handle;
    log->sample_period_us = llcm_config_req->sample_period_us;
    log->calibrated_stream = llcm_config_req->calibrated_stream;
    log->timestamp_offset_ns = llcm_config_req->timestamp_offset_ns;
    log->output_buf_most_recent_idx = llcm_channel->output_buf_most_recent_idx;
    log->output_buf_max_samples = llcm_channel->output_buf_max_samples;
    log->atomic_counter = llcm_channel->atomic_counter;
    log->interrupt_enabled = inst_state->registry_config.qc_vr_mode;    
    sns_isafe_list_iter_init(&iter, &llcm_channel->sensor_client_list, true);
    log->cur_client_cnt = sns_isafe_list_iter_len(&iter);

    sns_log_set_code(log, LOG_SNS_HDRLL_DATA_CLIENT_C);
    sns_log_set_length(log, sizeof(sns_llcm_client_req_log));
    sns_log_set_timestamp(log);
    sns_log_submit(log);

    sns_free(log);

  }
}

/*
 * Log event sample written into share memory.
 */
static void sns_llcm_inst_log_data(
        sns_sensor_instance *const this,
        sensors_event_t            *circ_buf_sample,
        uint32_t                   channel_id)
{
  sns_llcm_data_log *log = NULL;
  
  log = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_llcm_data_log));
  if(NULL == log)
  {
    SNS_INST_PRINTF(ERROR, this, "Unable to allocate log packet for sample data");
  }
  else
  {
    log->version = SNS_LOG_LLCM_LOG_VERSION;
    log->log_timestamp = sns_get_system_time();
    log->channel_id = channel_id;
    log->sensor = circ_buf_sample->sensor;
    log->sensor_type = circ_buf_sample->type;
    log->atomic_counter = circ_buf_sample->reserved0;
    log->timestamp = circ_buf_sample->timestamp;
    sns_memscpy(log->data,
          sizeof(log->data),
          circ_buf_sample->data,
          sizeof(circ_buf_sample->data));

    sns_log_set_code(log, LOG_SNS_HDRLL_DATA_C);
    sns_log_set_length(log, sizeof(sns_llcm_data_log));
    sns_log_set_timestamp(log);
    sns_log_submit(log);

    sns_free(log);

  }
}
/*
 * Copy sensor data into buffer.
 */
static inline sns_rc sns_llcm_inst_copy_data(
        sns_sensor_instance *const this,
        sns_llcm_inst_state        *inst_state,
        sensors_event_t            *circ_buf_sample,
        sns_llcm_sensor_client     *sensor_client,
        float                      *sample_data,
        uint32_t                   data_len)
{
  sns_rc rc = SNS_RC_SUCCESS;

  circ_buf_sample->type = sensor_client->sensor_type;
  switch (sensor_client->sensor_type)
  {
    case SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      sns_memscpy(circ_buf_sample->uncalibrated_accelerometer.uncalib,
          sizeof(circ_buf_sample->uncalibrated_accelerometer.uncalib),
          sample_data, data_len);
      sns_memscpy(circ_buf_sample->uncalibrated_accelerometer.bias,
          sizeof(circ_buf_sample->uncalibrated_accelerometer.bias),
          inst_state->accel_cal_bias, sizeof(inst_state->accel_cal_bias));
      break;
    }
    case SENSOR_TYPE_ACCELEROMETER:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      if (inst_state->accel_cal_status >= SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW)
      {
        sample_data[0] -= inst_state->accel_cal_bias[0];
        sample_data[1] -= inst_state->accel_cal_bias[1];
        sample_data[2] -= inst_state->accel_cal_bias[2];
      }
      sns_memscpy(circ_buf_sample->acceleration.v,
          sizeof(circ_buf_sample->acceleration.v),
          sample_data, data_len);
      circ_buf_sample->acceleration.status = inst_state->accel_cal_status;
      break;
    }
    case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      sns_memscpy(circ_buf_sample->uncalibrated_gyro.uncalib,
          sizeof(circ_buf_sample->uncalibrated_gyro.uncalib),
          sample_data, data_len);
      sns_memscpy(circ_buf_sample->uncalibrated_gyro.bias,
          sizeof(circ_buf_sample->uncalibrated_gyro.bias),
          inst_state->gyro_cal_bias, sizeof(inst_state->gyro_cal_bias));
      break;
    }
    case SENSOR_TYPE_GYROSCOPE:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      if(inst_state->gyro_cal_status >= SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW)
      {
        sample_data[0] -= inst_state->gyro_cal_bias[0];
        sample_data[1] -= inst_state->gyro_cal_bias[1];
        sample_data[2] -= inst_state->gyro_cal_bias[2];
      }
      sns_memscpy(circ_buf_sample->gyro.v,
          sizeof(circ_buf_sample->gyro.v),
          sample_data, data_len);
      circ_buf_sample->gyro.status = inst_state->gyro_cal_status;
      break;
    }
    case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      sns_memscpy(circ_buf_sample->uncalibrated_magnetic.uncalib,
          sizeof(circ_buf_sample->uncalibrated_magnetic.uncalib),
          sample_data, data_len);
      sns_memscpy(circ_buf_sample->uncalibrated_magnetic.bias,
          sizeof(circ_buf_sample->uncalibrated_magnetic.bias),
          inst_state->mag_cal_bias, sizeof(inst_state->mag_cal_bias));
      break;
    }
    case SENSOR_TYPE_MAGNETIC_FIELD:
    {
      circ_buf_sample->sensor = sensor_client->sensor_handle;
      if (inst_state->mag_cal_status >= SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_LOW)
      {
        sample_data[0] -= inst_state->mag_cal_bias[0];
        sample_data[1] -= inst_state->mag_cal_bias[1];
        sample_data[2] -= inst_state->mag_cal_bias[2];
      }
      sns_memscpy(circ_buf_sample->magnetic.v,
          sizeof(circ_buf_sample->magnetic.v),
          sample_data, data_len);
      circ_buf_sample->magnetic.status = inst_state->mag_cal_status;
      break;
    }
    default:
    {
      circ_buf_sample->type = SENSOR_TYPE_UNKNOWN;
      SNS_INST_PRINTF(ERROR, this, "Unsupported data type %u", sensor_client->sensor_type);
      rc = SNS_RC_INVALID_TYPE;
      break;
    }
  }
  return rc;
}
/*
 * Write new sensor sample into particular low latency channel.
 */
static inline sns_rc sns_llcm_inst_insert_sample(
        sns_sensor_instance *const this,
        sns_llcm_channel           *llcm_channel,
        sns_sensor_event           *event,
        sns_llcm_sensor_client     *sensor_client,
        uint32_t                   channel_id)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;
  sns_low_lat_circ_buf_android* circ_buf = (sns_low_lat_circ_buf_android*)llcm_channel->output_buf;
  sensors_event_t *circ_buf_sample;
  float sample_data[SNS_LLCM_IMU_SENSOR_AXES_NUM] = { 0 };
  uint8_t arr_index = 0;

  SNS_ASSERT(ARR_SIZE(sample_data)==3);

  pb_float_arr_arg arg = {
     .arr = sample_data,
     .arr_len = ARR_SIZE(sample_data),
     .arr_index = &arr_index
  };
  pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
  sns_std_sensor_event sensor_data = sns_std_sensor_event_init_default;

  sensor_data.data = (struct pb_callback_s) {
    .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg
  };
  if (!pb_decode(&stream, sns_std_sensor_event_fields, &sensor_data))
  {
    SNS_INST_PRINTF(ERROR, this, "Error in decoding sensor event");
    return SNS_RC_FAILED;
  }
#if LLCM_DEBUG_MORE
  SNS_INST_PRINTF(LOW, this, "LLCM - insert sample type:%u, sample data(x10000)=%d, %d, %d",
        sensor_client->sensor_type,
        (int32_t)(sample_data[0] * 10000), (int32_t)(sample_data[1] * 10000), (int32_t)(sample_data[2] * 10000));
#endif
  if ((SNS_LLCM_MIN_EIG_P >= fabsf(sample_data[0])) &&
        (SNS_LLCM_MIN_EIG_P >= fabsf(sample_data[1])) &&
        (SNS_LLCM_MIN_EIG_P >= fabsf(sample_data[2])))
  {
    SNS_INST_PRINTF(HIGH, this, "LLCM - Drop zero sample data.");
    return SNS_RC_INVALID_VALUE;
  }
  llcm_channel->output_buf_most_recent_idx++;
  if (llcm_channel->output_buf_most_recent_idx >= llcm_channel->output_buf_max_samples)
  {
    SNS_INST_PRINTF(MED, this,
        "LLCM - insert sample: index rollover most_recent_idx=%u max_samples=%u",
        llcm_channel->output_buf_most_recent_idx, llcm_channel->output_buf_max_samples);
    llcm_channel->output_buf_most_recent_idx = 0;
  }
  circ_buf_sample = &circ_buf->samples[llcm_channel->output_buf_most_recent_idx];

  sns_memset(circ_buf_sample, 0, sizeof(*circ_buf_sample));
  /* Fill out the sample in shared memory */
  rc = sns_llcm_inst_copy_data(this, inst_state, circ_buf_sample, sensor_client, sample_data, sizeof(sample_data));
  if (SNS_RC_SUCCESS != rc)
  {
    SNS_INST_PRINTF(ERROR, this,
        "LLCM - insert sample: fail to copy sensor data");
    /* Move back write index */
    if (0 == llcm_channel->output_buf_most_recent_idx)
    {
      llcm_channel->output_buf_most_recent_idx = llcm_channel->output_buf_max_samples - 1;
    }
    else
    {
      llcm_channel->output_buf_most_recent_idx--;
    }
    return SNS_RC_FAILED;
  }
  circ_buf_sample->version = SNS_LOW_LAT_SENSOR_EVENT_VERSION;
  /* qtimer_ticks_nano = qtimer / 19200000 * 1000000000 
   *  reduction of a fraction, avoid overflow while calculating */
  circ_buf_sample->timestamp = (event->timestamp * 625ULL / 12ULL);
  if(!inst_state->registry_config.qc_vr_mode)
  {
     /* offset = Android time - Qtime in SEE HAL */
    circ_buf_sample->timestamp += inst_state->timestamp_offset_ns;
  }

  /* Flush the cache */
  sns_llcm_cache_clean(circ_buf_sample, sizeof(*circ_buf_sample));

  /* Atomic counter get updated last, increment the atomic counter */
  llcm_channel->atomic_counter++;

  /* If the atomic counter rolls over, then reset it to 1. Zero is an invalid
    * value for the atomic counter.
    */
  if (0 == llcm_channel->atomic_counter)
  {
    SNS_INST_PRINTF(MED, this,
        "LLCM - insert sample: atomic counter rollover");
    llcm_channel->atomic_counter = 1;
  }
  circ_buf_sample->reserved0 = llcm_channel->atomic_counter;
  sns_llcm_cache_clean(&(circ_buf_sample->reserved0), sizeof(circ_buf_sample->reserved0));

  /* Fire off the interrupt, if necessary */
  if(inst_state->registry_config.qc_vr_mode)
  {
    sns_llcm_inst_notify_data_ready(inst_state);
  }

  /* Signal that a new sample is in the buffer */
  qurt_signal2_set(&llcm_channel->signal, SNS_LLCM_SIGNAL_IND);

  SNS_PROFILE(SNS_LLCM_NOTIFY_DATA_READY, 4, sns_osa_thread_get_thread_id(), llcm_channel->output_buf_most_recent_idx, (uint32_t)event->timestamp, llcm_channel->atomic_counter);

  sns_llcm_inst_log_data(this, circ_buf_sample, channel_id);
  
  return rc;
}

/*
 * @See sns_sensor_instance_api::init.
 */
static sns_rc sns_llcm_inst_init(
        sns_sensor_instance *const this,
        sns_sensor_state const *sensor_state)
{
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;
  sns_llcm_state *llcm_sensor = (sns_llcm_state *)sensor_state->state;
  sns_osa_lock_attr lock_attr;
  sns_rc rc;

  SNS_INST_PRINTF(MED, this, "Instance init for client id %d", inst_state->client_id);

  /* All state variables are initialized to zero */
  sns_memset(inst_state, 0, sizeof(*inst_state));

  sns_suid_lookup_get(&llcm_sensor->suid_lookup_data, "resampler", &inst_state->resampler_suid);
  sns_suid_lookup_get(&llcm_sensor->suid_lookup_data, "gyro_cal", &inst_state->gyro_cal_suid);

  inst_state->accel_cal_available =
        sns_suid_lookup_get(&llcm_sensor->opt_suid_lookup_data, "accel_cal", &inst_state->accel_cal_suid);
  inst_state->mag_cal_available =
        sns_suid_lookup_get(&llcm_sensor->opt_suid_lookup_data, "mag_cal", &inst_state->mag_cal_suid);
  SNS_INST_PRINTF(LOW, this, "mag_cal_available %d", inst_state->mag_cal_available);

  /* Copy config */
  sns_memscpy(&inst_state->registry_config, sizeof(inst_state->registry_config),
              &llcm_sensor->registry_config, sizeof(llcm_sensor->registry_config));
  SNS_INST_PRINTF(MED, this, "qc_vr_mode = %d", inst_state->registry_config.qc_vr_mode);

  rc = sns_osa_lock_attr_init(&lock_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_NORMAL);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_create(&lock_attr, &inst_state->client_list_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  return SNS_RC_SUCCESS;
}

/*
 * @See sns_sensor_instance_api::deinit.
 */
static sns_rc sns_llcm_inst_deinit(
    sns_sensor_instance *const this)
{
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;

  sns_llcm_inst_process_deinit(this);
  sns_osa_lock_delete(inst_state->client_list_lock);

  SNS_INST_PRINTF(MED, this, "Instance deinited for client id %d",
      inst_state->client_id);
  return SNS_RC_SUCCESS;
}

/*
 * Function to handle sensor calibration data event.
 */
static void sns_llcm_inst_handle_cal_event(
        sns_sensor_instance *const this,
        sns_data_stream *cal_stream,
        float *cal_bias,
        sns_std_sensor_sample_status *cal_status)
{
  sns_sensor_event	 *event = NULL;
#if LLCM_DEBUG_MORE
  uint32_t		 event_cnt;
#endif

  if (cal_stream)
  {
    event = cal_stream->api->peek_input(cal_stream);
#if LLCM_DEBUG_MORE
    event_cnt = cal_stream->api->get_input_cnt(cal_stream);
    if ( 0 != event_cnt)
    {
      SNS_INST_PRINTF(LOW, this, "cal event cnt:%d, msg_id:%d",
        event_cnt, (NULL == event) ? -1 : event->message_id);
    }
#endif
    while (NULL != event)
    {
       sns_llcm_inst_decode_cal_event(this, event->event,event->event_len,event->timestamp,
                                     cal_bias, cal_status);
       SNS_INST_PRINTF(MED, this, "sensor cal bias(x1000): x:%d y:%d z:%d status:%u",
                       (int32_t)(cal_bias[0] * 1000),
                       (int32_t)(cal_bias[1] * 1000),
                       (int32_t)(cal_bias[2] * 1000),
                       *cal_status);
       event = cal_stream->api->get_next_input(cal_stream);
    }
  }
}

/**
 * Process all pending events for this client.  Form and send indications,
 * and/or batch data as appropriate.
 *
 * @see sns_sensor_instance_api::event_notify
 */
static sns_rc sns_llcm_inst_notify_event(
        sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_llcm_inst_state    *inst_state = (sns_llcm_inst_state*)this->state->state;
  sns_llcm_channel       *llcm_channel;
  sns_data_stream        *data_stream;
  sns_sensor_event       *event = NULL;
  sns_isafe_list_iter    iter;
  uint32_t               channel_id;

  SNS_PROFILE(SNS_LLCM_NOTIFY_EVENT_START, 0);

  sns_osa_lock_acquire(&llcm_stream_op_lock);
  sns_llcm_inst_handle_cal_event(this,
  	inst_state->accel_cal_stream,
  	inst_state->accel_cal_bias,
  	&inst_state->accel_cal_status);

  sns_llcm_inst_handle_cal_event(this,
  	inst_state->gyro_cal_stream,
  	inst_state->gyro_cal_bias,
  	&inst_state->gyro_cal_status);

  sns_llcm_inst_handle_cal_event(this,
  	inst_state->mag_cal_stream,
  	inst_state->mag_cal_bias,
  	&inst_state->mag_cal_status);
  sns_osa_lock_release(&llcm_stream_op_lock);

  /* Loop all sensor data streams */
  for (channel_id = 0; channel_id < SNS_LLCM_MAX_CHANNELS; channel_id++)
  {
    llcm_channel = &(sns_llcm_channel_list[channel_id]);
    if (NULL == llcm_channel->output_buf)
    {
      continue;
    }
    sns_osa_lock_acquire(inst_state->client_list_lock);
    sns_osa_lock_acquire(&llcm_stream_op_lock);
    for(sns_isafe_list_iter_init(&iter, &(llcm_channel->sensor_client_list), true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_llcm_sensor_client *sensor_client = (sns_llcm_sensor_client*)
        sns_isafe_list_iter_get_curr_data(&iter);

        data_stream = (NULL != sensor_client->resampler_stream) ? 
		sensor_client->resampler_stream : sensor_client->raw_data_stream;	
      event = data_stream->api->peek_input(data_stream);

      while (NULL != event)
      {
        if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == event->message_id)
        {
          /* PEND share data stream over channels */
          sns_llcm_inst_insert_sample(this, llcm_channel, event, sensor_client, channel_id);
        }
        else
        {
          SNS_INST_PRINTF(HIGH, this,
              "LLCM - notify event: unexpected message %u", event->message_id);
        }
        /* Already in Android coordinate system */
        event = data_stream->api->get_next_input(data_stream);
      }
    }
    sns_osa_lock_release(&llcm_stream_op_lock);
    sns_osa_lock_release(inst_state->client_list_lock);
  }

  SNS_PROFILE(SNS_LLCM_NOTIFY_EVENT_END, 0);
  return rc;
}

/*
 * @See sns_sensor_instance_api::set_client_config.
 */
static sns_rc sns_llcm_inst_set_client_config(
        sns_sensor_instance *this,
        sns_request const *client_request)
{
  sns_rc              rc = SNS_RC_SUCCESS;
  bool                remove_all_streams = false;
  sns_isafe_list_iter       iter;
  sns_llcm_sensor_client *sensor_client;
  sns_llcm_config_req *llcm_config_req;
  sns_llcm_channel    *llcm_channel;
  sns_llcm_inst_state *inst_state = (sns_llcm_inst_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service  *stream_service =
        (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

  if (SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG != client_request->message_id)
  {
    SNS_INST_PRINTF(ERROR, this,
        "Unexpected message : message_id=%x", client_request->message_id);
    return SNS_RC_NOT_SUPPORTED;
  }

  llcm_config_req = (sns_llcm_config_req *)client_request->request;
  if (NULL == llcm_config_req)
  {
    SNS_INST_PRINTF(ERROR, this, "Client request is NULL.");
    return SNS_RC_INVALID_VALUE;
  }

  /* Update timestamp offset and skip all other operations*/
  if(true == llcm_config_req->offset_update_only)
  {
    inst_state->timestamp_offset_ns = llcm_config_req->timestamp_offset_ns;
#if LLCM_DEBUG_MORE	
    SNS_INST_PRINTF(MED, this, "Update TS offset success HI:0x%x, LO:0x%x",
                SNS_LLCM_FETCH_HI_32(inst_state->timestamp_offset_ns),
                SNS_LLCM_FETCH_LOW_32(inst_state->timestamp_offset_ns));
#endif    
    return SNS_RC_SUCCESS;
  }

  SNS_INST_PRINTF(HIGH, this,
      "Config data: channel_id=%u, sample_period_us=%u cal=%u sensor_handle=%d",
      llcm_config_req->channel_id, llcm_config_req->sample_period_us,
      llcm_config_req->calibrated_stream, llcm_config_req->sensor_handle);

  if (SNS_LLCM_MAX_CHANNELS <= llcm_config_req->channel_id)
  {
    SNS_INST_PRINTF(ERROR, this, "Invalid channel ID: channel_id=%u",
        llcm_config_req->channel_id);
    return SNS_RC_INVALID_VALUE;
  }

  llcm_channel = &sns_llcm_channel_list[llcm_config_req->channel_id];
  if(NULL == llcm_channel)
  {
    SNS_INST_PRINTF(ERROR, this, "Invalid channel pointer: channel_id=%u",
        llcm_config_req->channel_id);
    return SNS_RC_INVALID_VALUE;
  }

  sns_llcm_inst_log_client_req(this, llcm_config_req, llcm_channel);
  
  /* Update timestamp offset if sampling period is not zero and handle is not zero. */
  if (0 != llcm_config_req->sample_period_us &&
    -1 != llcm_config_req->sensor_handle)
  {
    inst_state->timestamp_offset_ns = llcm_config_req->timestamp_offset_ns;
  }

  /* Disable all sensors if handle is -1 */
  if (-1 == llcm_config_req->sensor_handle)
  {
    /* Remove all sensor data streams in the channel  */
    remove_all_streams = true;
    SNS_INST_PRINTF(HIGH, this, "Stop all sensors for channel %d", llcm_config_req->channel_id);
  }
  /* Disable if sampling period is zero */
  if (0 == llcm_config_req->sample_period_us || remove_all_streams)
  {
    sensor_client = NULL;
    sns_osa_lock_acquire(inst_state->client_list_lock);
    /* Find the associated sensor client, remove it */
    for (sns_isafe_list_iter_init(&iter, &llcm_channel->sensor_client_list, true);
        NULL != sns_isafe_list_iter_curr(&iter); )
    {
      sensor_client = (sns_llcm_sensor_client*)
        sns_isafe_list_iter_get_curr_data(&iter);
      if ((NULL != sensor_client) &&
        (remove_all_streams ||
        (sns_sensor_uid_compare((sns_sensor_uid*)llcm_config_req->sensor_suid,
                             (sns_sensor_uid*)&sensor_client->suid))))
      {
        /* Delete the sensor client from the queue */
        sns_isafe_list_iter_remove(&iter);
        SNS_INST_PRINTF(MED, this, "Removeing: sensor_client=0x%x raw_data_stream=0x%x resampler_stream=0x%x",
          sensor_client, sensor_client->raw_data_stream, sensor_client->resampler_stream);
        SNS_INST_PRINTF(MED, this, "Removeing: suid %x%x, %x%x",
          SNS_LLCM_FETCH_HI_32(sensor_client->suid.suid_high),
          SNS_LLCM_FETCH_LOW_32(sensor_client->suid.suid_high),
          SNS_LLCM_FETCH_HI_32(sensor_client->suid.suid_low),
          SNS_LLCM_FETCH_LOW_32(sensor_client->suid.suid_low));

        /* Update calibration stream use count */
        sns_llcm_inst_update_cal_use_cnt(inst_state, sensor_client->sensor_type, false);
#if LLCM_DEBUG_MORE	
        SNS_INST_PRINTF(MED, this, "accel_cal_cnt=%d, gyro_cal_cnt=%d, mag_cal_cnt=%d",
                inst_state->accel_cal_use_cnt,
                inst_state->gyro_cal_use_cnt,
                inst_state->mag_cal_use_cnt);
#endif

        /* Stop data stream */
        sns_osa_lock_acquire(&llcm_stream_op_lock);
        if (NULL != sensor_client->raw_data_stream)
        {
          stream_service->api->remove_stream(stream_service, sensor_client->raw_data_stream);
        }
        if (NULL != sensor_client->resampler_stream)
        {
          stream_service->api->remove_stream(stream_service, sensor_client->resampler_stream);
        }
        sns_osa_lock_release(&llcm_stream_op_lock);
        sns_free(sensor_client);

        /* Stop searching unless need to disable all clients */
        if (!remove_all_streams)
          break;
      }
      else
      {
        sns_isafe_list_iter_advance(&iter);
      }
    }
    sns_osa_lock_release(inst_state->client_list_lock);

    sns_osa_lock_acquire(&llcm_stream_op_lock);
    /* Remove calibration stream if no client need it */
    if((0 >= inst_state->gyro_cal_use_cnt) && (NULL != inst_state->gyro_cal_stream))
    {
      stream_service->api->remove_stream(stream_service, inst_state->gyro_cal_stream);
      inst_state->gyro_cal_stream = NULL;
      inst_state->gyro_cal_use_cnt = 0;
    }
    if((0 >= inst_state->mag_cal_use_cnt) && (NULL != inst_state->mag_cal_stream))
    {
      stream_service->api->remove_stream(stream_service, inst_state->mag_cal_stream);
      inst_state->mag_cal_stream = NULL;
      inst_state->mag_cal_use_cnt = 0;
    }
    if((0 >= inst_state->accel_cal_use_cnt) && (NULL != inst_state->accel_cal_stream))
    {
      stream_service->api->remove_stream(stream_service, inst_state->accel_cal_stream);
      inst_state->accel_cal_stream = NULL;
      inst_state->accel_cal_use_cnt = 0;
    }
    sns_osa_lock_release(&llcm_stream_op_lock);

    if(0 == sns_isafe_list_iter_len(&iter))
    {
      /* Signal to unblock any pending polling when no client in current channel */
      qurt_signal2_set(&llcm_channel->signal, SNS_LLCM_SIGNAL_CANCEL_POLL);
    }
  }
  else
  {
    /* Enable sensor */
    SNS_INST_PRINTF(MED, this,
        "Requesting data stream for suid %x%x, %x%x",
        SNS_LLCM_FETCH_HI_32(llcm_config_req->sensor_suid->suid_high),
        SNS_LLCM_FETCH_LOW_32(llcm_config_req->sensor_suid->suid_high),
        SNS_LLCM_FETCH_HI_32(llcm_config_req->sensor_suid->suid_low),
        SNS_LLCM_FETCH_LOW_32(llcm_config_req->sensor_suid->suid_low));

    /* Create another client and add it to the client queue */
    sensor_client = (sns_llcm_sensor_client*)sns_malloc(SNS_HEAP_MAIN, sizeof(sns_llcm_sensor_client));
    SNS_ASSERT(sensor_client != NULL);

    sns_memset(sensor_client, 0, sizeof(*sensor_client));
    /* Assign the SUID to this client */
    sensor_client->suid.suid_low = llcm_config_req->sensor_suid->suid_low;
    sensor_client->suid.suid_high = llcm_config_req->sensor_suid->suid_high;
    sensor_client->sensor_handle = llcm_config_req->sensor_handle;

    if (inst_state->registry_config.qc_vr_mode)
    {
      /* Create Interrupt Handler if the client is asking for interrupts */
      rc = sns_llcm_inst_attach_interrupt(this);
    }

    sns_osa_lock_acquire(&llcm_stream_op_lock);
    if (SNS_RC_SUCCESS == rc)
    {
      rc = sns_llcm_inst_request_sensor(this, stream_service, sensor_client, llcm_config_req);      
    }
    if (SNS_RC_SUCCESS == rc)
    {
      /* Create calibration stream */
      rc = sns_llcm_inst_request_calibration(this, stream_service, sensor_client->sensor_type);
    }
    sns_osa_lock_release(&llcm_stream_op_lock);

    if (SNS_RC_SUCCESS == rc)
    {
      sns_llcm_inst_update_cal_use_cnt(inst_state, sensor_client->sensor_type, true);
      /* Add client infotmation to list */
      sns_osa_lock_acquire(inst_state->client_list_lock);
      sns_isafe_list_iter_init(&iter,
        &(llcm_channel->sensor_client_list), false);
      sns_isafe_list_item_init(&sensor_client->item, sensor_client);
      sns_isafe_list_iter_insert(&iter, &sensor_client->item, true);
      sns_osa_lock_release(inst_state->client_list_lock);
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Enable sensor failed, rc=%d", rc);
      /* Remove data stream and free client, not release interrupt as interrupt is shared over all channels */
      sns_osa_lock_acquire(&llcm_stream_op_lock);
      if (NULL != sensor_client->raw_data_stream)
      {
        stream_service->api->remove_stream(stream_service, sensor_client->raw_data_stream);
      }
      if (NULL != sensor_client->resampler_stream)
      {
        stream_service->api->remove_stream(stream_service, sensor_client->resampler_stream);
      }
      sns_osa_lock_release(&llcm_stream_op_lock);
      sns_free(sensor_client);
      rc = SNS_RC_FAILED;
    }
  }
  SNS_INST_PRINTF(LOW, this, "LLCM - set config complete, rc=%d", rc);

  return rc;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api llcm_sensor_instance_api SNS_SECTION(".rodata.island") =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_llcm_inst_init,
  .set_client_config = &sns_llcm_inst_set_client_config,
  .notify_event = &sns_llcm_inst_notify_event,
  .deinit = &sns_llcm_inst_deinit
};
