/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file see_calibration_accel.h
  @brief
  see_calibration_accel header file
============================================================================*/
#include "ssc_connection.h"
#include "pthread.h"
#include "sensors_log.h"
#include <string>
#include <vector>
#define ACCEL "accel"
#define REGISTRY "registry"

using namespace std;
typedef struct sensor_uid {
  uint64_t low;
  uint64_t high;
} sensor_uid;

typedef void (*display_accel_cal_cb)(string display_accel_cal);

using accel_suid_event_function =
    std::function<void(const std::string& datatype,
        const std::vector<sensor_uid>& suids)>;
typedef void (*registry_write_cb)();

using registry_write_event_function =
std::function<void(const std::string& datatype,
    const std::vector<sensor_uid>& suids)>;
typedef struct {
  float x;
  float y;
  float z;
  int motionState;
} bias_output;
typedef struct {
  float x;
  float y;
  float z;
} coordinates;

class accel_cal {
public:
  accel_cal() {

    sns_logd("accel_cal constructor, creating ssc connections");
    init_ssc_connectiions();
    pthread_mutex_init(&cb_mutex, NULL);
    pthread_cond_init(&condition, NULL);
  }
  ~accel_cal();
  void cal_init();
  void register_calibrated_values_cb_func(display_accel_cal_cb in_func_ptr);
private:

  sensor_uid accel_suid;
  sensor_uid registry_suid;
  ssc_connection* ssc_suid_obj;
  ssc_connection* ssc_accel_cal_obj;
  ssc_connection* ssc_registry_write_obj;
  ssc_event_cb ssc_accel_cal_cb;
  ssc_event_cb ssc_registry_write_cb;
  ssc_event_cb ssc_suid_cb;
  //usta_logging_util *log_instance;
  pthread_mutex_t cb_mutex;
  pthread_cond_t condition;
  float** samples_for_bias_calculation;
  void handle_ssc_suid_event(const uint8_t *encoded_data_string,
      size_t encoded_string_size);
  void handle_ssc_accel_cal_event(const uint8_t *encoded_data_string,
      size_t encoded_string_size);
  void handle_ssc_registry_write_event(const uint8_t *encoded_data_string,
      size_t encoded_string_size);
  void init_ssc_connectiions();
  void request_suid(std::string datatype);
  void request_accel_cal();
  void request_registry_write();
  void calculate_bias(bias_output* output);
  void calculate_offset_for_registry();
  display_accel_cal_cb ptr_display_accel_cal_cb;
  registry_write_cb ptr_display_registry_write_cb;
  coordinates curr_cal;
  coordinates sample_cal;
  coordinates sample_calculated_offset;

};

