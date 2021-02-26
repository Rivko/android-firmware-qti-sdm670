/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file see_calibration_accel.cpp
  @brief
  see_calibration_accel implementation.
============================================================================*/
#include "see_calibration_accel.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "sns_accel.pb.h"
#include "sns_registry.pb.h"
#include <pthread.h>
#include "sensors_log.h"
#include <string>
#include <sstream>
#include <iostream>
#define CAL_BIAS_GROUP_NAME "lsm6dsm_0_platform.accel.fac_cal.bias"
#define NUM_AXIS 3
#define G 9.80665
#define FX_ABS(x)            ((x<0)?(-(x)):(x))
using namespace std;

int SAMPLE_COUNT_REQUIRED_FORALGO = 64;
int accel_sample_number = 0;

float bias_thresholds[3] = { 1.47, 1.47, 1.47 + G };
float variance_threshold = 0.000944;

void accel_cal::init_ssc_connectiions()
{

  ssc_suid_cb = [this](const uint8_t* msg , int msgLength)
  { this->handle_ssc_suid_event(msg, msgLength);};

  if (NULL == (ssc_suid_obj = new ssc_connection(ssc_suid_cb))) {
    sns_logd("ssc connection for suid failed");
    return;
  }

  ssc_accel_cal_cb = [this](const uint8_t* msg , int msgLength)
    {this->handle_ssc_accel_cal_event(msg, msgLength);};

  if (NULL == (ssc_accel_cal_obj = new ssc_connection(ssc_accel_cal_cb))) {
    sns_logd("ssc connection failed");
    return;
  }
  ssc_registry_write_cb = [this](const uint8_t* msg , int msgLength)
  { this->handle_ssc_registry_write_event(msg, msgLength);};
  if (NULL
      == (ssc_registry_write_obj = new ssc_connection(ssc_registry_write_cb))) {
    sns_logd("ssc connection failed");
    return;
  }

  sns_logd("ssc connections successful");
}

void accel_cal::calculate_bias(bias_output* output)
{

  sns_logd("calculating bias for 64 samples");

  output->motionState = 0;
  float sampleSum[NUM_AXIS] = { 0 };
  float sampleSqSum[NUM_AXIS] = { 0 };
  float variance[NUM_AXIS] = { 0 };

  for (int j = 0; j < SAMPLE_COUNT_REQUIRED_FORALGO; j++) {

    for (int i = 0; i < NUM_AXIS; i++) {
      sampleSum[i] += samples_for_bias_calculation[j][i];
      sampleSqSum[i] += ((float) (samples_for_bias_calculation[j][i])
          * (float) (samples_for_bias_calculation[j][i]));
    }
  }

  float varT;

  for (int i = 0; i < NUM_AXIS; i++) {
    varT = (float) (sampleSum[i]) * (float) (sampleSum[i]);

    variance[i] = (sampleSqSum[i] - (varT / (float) SAMPLE_COUNT_REQUIRED_FORALGO)) / (float) SAMPLE_COUNT_REQUIRED_FORALGO;

    if (variance[i] > variance_threshold) {
      output->motionState = 0;
      return;
    } else if (0 == variance[i]) {
      output->motionState = 0;
      return;
    } else if ( FX_ABS(sampleSum[i] / SAMPLE_COUNT_REQUIRED_FORALGO) > bias_thresholds[i]) {
      output->motionState = 0;
      return;
    }

  }

  output->motionState = 1;
  output->x = sampleSum[0] / SAMPLE_COUNT_REQUIRED_FORALGO;
  output->y = sampleSum[1] / SAMPLE_COUNT_REQUIRED_FORALGO;
  output->z = sampleSum[2] / SAMPLE_COUNT_REQUIRED_FORALGO;
  sns_logd("bias successfully calculated for 64 samples");

  sns_logd(" variance = %f, %f , %f", variance[0], variance[1], variance[2]);

}

void accel_cal::handle_ssc_registry_write_event(const uint8_t *data,
    size_t size) {
  sns_logd("event received for registry write");
}

void accel_cal::handle_ssc_accel_cal_event(const uint8_t *data, size_t size)
{

  sns_logd("event received for accel config");

  sns_client_event_msg pb_event_msg;
  pb_event_msg.ParseFromArray(data, size);
  int eventSize = pb_event_msg.events_size();
  for (int i = 0; i < eventSize; i++) {
    auto&& pb_event = pb_event_msg.events(i);
    if (pb_event.msg_id() == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT) {
      sns_std_sensor_event pb_sensor_event;
      pb_sensor_event.ParseFromString(pb_event.payload());

      float x = pb_sensor_event.data(0);
      float y = pb_sensor_event.data(1);
      float z = pb_sensor_event.data(2);
      sns_logd("accel data received from event ::: x=%f, y=%f, z=%f", x,y, z);

      sample_cal.x = x;
      sample_cal.y = y;
      sample_cal.z = z;

      sample_calculated_offset.x = 0;
      sample_calculated_offset.y = 0;
      sample_calculated_offset.z = 0;

      calculate_offset_for_registry();
      sns_logd("sample offset, to write in registry = %f,%f,%f",
          sample_calculated_offset.x, sample_calculated_offset.y,
          sample_calculated_offset.z);
      request_registry_write();
      sns_logd("Registry write request completed, out of the function");

      string calString = "X="+ to_string(sample_calculated_offset.x) +"\nY="+ to_string(sample_calculated_offset.y) +"\nZ="+to_string(sample_calculated_offset.z);

      if(ptr_display_accel_cal_cb != NULL )
        (*ptr_display_accel_cal_cb)(calString);

      samples_for_bias_calculation[accel_sample_number][0] = x;
      samples_for_bias_calculation[accel_sample_number][1] = y;
      samples_for_bias_calculation[accel_sample_number][2] = z;

      if (accel_sample_number == SAMPLE_COUNT_REQUIRED_FORALGO - 1) {
        bias_output output;
        calculate_bias(&output);
        if (output.motionState == 0) {
          sns_logd("Motion detected.");
        } else {
          sns_logd("Device at rest");
          sns_logd("calculated cal values: %f, %f, %f", output.x,output.y, output.z);

          curr_cal.x = output.x;
          curr_cal.y = output.y;
          curr_cal.z = output.z;
        }
      }
      accel_sample_number = (accel_sample_number + 1) % 64;
    }
  }
}

void accel_cal::register_calibrated_values_cb_func(display_accel_cal_cb in_func_ptr)
{
  ptr_display_accel_cal_cb = in_func_ptr;
}

void accel_cal::handle_ssc_suid_event(const uint8_t *data, size_t size)
{

  sns_logd(" event received for accel suid");
  /* parse the pb encoded event */
  sns_client_event_msg pb_event_msg;
  pb_event_msg.ParseFromArray(data, size);
  for (int i = 0; i < pb_event_msg.events_size(); i++) {
    sns_logd("suid event iteration %d", i);
    auto& pb_event = pb_event_msg.events(i);
  if (pb_event.msg_id() != SNS_SUID_MSGID_SNS_SUID_EVENT) {
    return;
  }
  sns_suid_event pb_suid_event;
  pb_suid_event.ParseFromString(pb_event.payload());
  const string& datatype = pb_suid_event.data_type();
    for (int j = 0; j < pb_suid_event.suid_size(); j++) {
      sns_logd("suid number %d", j);
      if (datatype == ACCEL) {
        accel_suid.low = pb_suid_event.suid(j).suid_low();
        accel_suid.high = pb_suid_event.suid(j).suid_high();
        stringstream suidLow;
        suidLow << std::hex << accel_suid.low;
        stringstream suidHigh;
        suidHigh << std::hex << accel_suid.high;
        sns_logd("even recieved for accel suid, suid =  %s, %s",
            suidLow.str().c_str(), suidHigh.str().c_str());
      } else if (datatype == REGISTRY) {
        registry_suid.low = pb_suid_event.suid(j).suid_low();
        registry_suid.high = pb_suid_event.suid(j).suid_high();
        stringstream suidLow;
        suidLow << std::hex << registry_suid.low;
        stringstream suidHigh;
        suidHigh << std::hex << registry_suid.high;
        sns_logd("even recieved for registry suid, suid =  %s, %s",
            suidLow.str().c_str(), suidHigh.str().c_str());
      }
    }
  }
  pthread_cond_signal(&condition);
  pthread_mutex_unlock(&cb_mutex);

}

void accel_cal::request_suid(std::string datatype)
{
  sns_client_request_msg pb_req_msg;
  sns_suid_req pb_suid_req;
  string pb_suid_req_encoded;

  const sensor_uid LOOKUP_SUID = { 12370169555311111083ull,
      12370169555311111083ull };
  /* populate SUID request */
  pb_suid_req.set_data_type(datatype);
  pb_suid_req.set_register_updates(true);
  pb_suid_req.SerializeToString(&pb_suid_req_encoded);

  /* populate the client request message */
  pb_req_msg.set_msg_id(SNS_SUID_MSGID_SNS_SUID_REQ);
  pb_req_msg.mutable_request()->set_payload(pb_suid_req_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(LOOKUP_SUID.high);
  pb_req_msg.mutable_suid()->set_suid_low(LOOKUP_SUID.low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(
      SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(
      SNS_STD_CLIENT_PROCESSOR_APSS);
  string pb_req_msg_encoded;
  pb_req_msg.SerializeToString(&pb_req_msg_encoded);
  //DEBUG_LOG(log_instance," sending request to QMI connection for accel suid ");
  ssc_suid_obj->send_request(pb_req_msg_encoded);
}

void accel_cal::request_accel_cal()
{

  float sample_rate = 20;
  sns_client_request_msg pb_req_msg;
  sns_std_sensor_config pb_stream_cfg;
  string pb_stream_cfg_encoded;
  pb_stream_cfg.set_sample_rate(sample_rate);
  pb_stream_cfg.SerializeToString(&pb_stream_cfg_encoded);

  pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
  pb_req_msg.mutable_request()->set_payload(pb_stream_cfg_encoded);


  pb_req_msg.mutable_suid()->set_suid_high(accel_suid.high);
  pb_req_msg.mutable_suid()->set_suid_low(accel_suid.low);

  pb_req_msg.mutable_susp_config()->set_delivery_type(
      SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(
      SNS_STD_CLIENT_PROCESSOR_APSS);

  string pb_req_msg_encoded;

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);
  //setting number of samples recieved to 0, and allocate the input array
  accel_sample_number = 0;
  samples_for_bias_calculation = new float*[SAMPLE_COUNT_REQUIRED_FORALGO];
  if(samples_for_bias_calculation == NULL){
    sns_loge("Memory allocation failed for samples_for_bias_calculation");
    return;
  }

  for (int i = 0; i < SAMPLE_COUNT_REQUIRED_FORALGO; i++) {
    samples_for_bias_calculation[i] = new float[3];
  }

  ssc_accel_cal_obj->send_request(pb_req_msg_encoded);

}

void accel_cal::cal_init()
{
  std::string datatype_accel = ACCEL, datatype_registry = REGISTRY;
  curr_cal.x = 0;
  curr_cal.y = 0;
  curr_cal.z = G;
  sample_calculated_offset.x = 0;
  sample_calculated_offset.y = 0;
  sample_calculated_offset.z = 0;
  pthread_mutex_lock(&cb_mutex);
  request_suid(datatype_accel);
  pthread_cond_wait(&condition, &cb_mutex);
  request_suid(datatype_registry);
  request_accel_cal();

}

accel_cal::~accel_cal()
{
  if (ssc_registry_write_obj != NULL)
  {
    delete ssc_registry_write_obj;
    ssc_registry_write_obj = NULL;
  }

  if(ssc_accel_cal_obj != NULL)
  {
    delete ssc_accel_cal_obj;
    ssc_accel_cal_obj = NULL;
  }

  if(ssc_suid_obj != NULL)
  {
    delete ssc_suid_obj;
    ssc_suid_obj = NULL;
  }
}

void accel_cal::request_registry_write() {

  sns_logd("start of function request_registry_write");
  string name = CAL_BIAS_GROUP_NAME;
  name = name + '\0';
  sns_client_request_msg pb_req_msg;

  sns_registry_write_req pb_stream_registry_data_item;
  string pb_stream_registry_data_encoded = "";
  pb_stream_registry_data_item.set_name(name);

  string item_name[3] = { "x", "y", "z" };
  float item_value[3] = { sample_calculated_offset.x,
      sample_calculated_offset.y, sample_calculated_offset.z };
  sns_registry_data* reg_data_temp =
      pb_stream_registry_data_item.mutable_data();
  for (int i = 0; i < NUM_AXIS; i++) {
    sns_registry_data_item *item_var = reg_data_temp->add_items();
    item_name[i] = item_name[i] + '\0';
    item_var->set_name(item_name[i]);
    item_var->set_flt(item_value[i]);
  }

  sns_logd("items size = %d", reg_data_temp->items_size());
  pb_stream_registry_data_item.SerializeToString(
      &pb_stream_registry_data_encoded);

  pb_req_msg.mutable_suid()->set_suid_high(registry_suid.high);
  pb_req_msg.mutable_suid()->set_suid_low(registry_suid.low);

  pb_req_msg.set_msg_id(SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ);
  pb_req_msg.mutable_request()->set_payload(pb_stream_registry_data_encoded);

  pb_req_msg.mutable_susp_config()->set_delivery_type(
      SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->set_client_proc_type(
      SNS_STD_CLIENT_PROCESSOR_APSS);

  string pb_req_msg_encoded;

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  sns_logd("sending ssn request registry write");

  ssc_registry_write_obj->send_request(pb_req_msg_encoded);

}

void accel_cal::calculate_offset_for_registry() {
  sns_logd("start of function calculate_offset_for_registry");
  sns_logd("sample cal = %f,%f,%f and cumulated_cal is %f,%f,%f", sample_cal.x,
      sample_cal.y, sample_cal.z, curr_cal.x, curr_cal.y, curr_cal.z);
  sample_calculated_offset.x = sample_cal.x - curr_cal.x;
  sample_calculated_offset.y = sample_cal.y - curr_cal.y;
  sample_calculated_offset.z = sample_cal.z - curr_cal.z;
}
