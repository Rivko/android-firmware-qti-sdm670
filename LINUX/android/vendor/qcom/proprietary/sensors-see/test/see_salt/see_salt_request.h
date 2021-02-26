/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt_request.h
** DESC: class definitions see_std_request
** ================================================================ */
#pragma once

// ---------------------------------------------------------------------------
// sns_std.proto:
// message sns_std_request {
//   message batch_spec {
//     required uint32 batch_period = 1;
//     optional uint32 flush_period = 2;
//     optional bool flush_only = 3;
//     optional bool max_batch = 4;
//   }
//   optional bytes payload = 3;
// }
// ---------------------------------------------------------------------------
enum see_payload_types_e {
   SEE_PAYLOAD_OMITTED = 0,
   SEE_PAYLOAD_STD_SENSOR,
   SEE_PAYLOAD_SELF_TEST,
   SEE_PAYLOAD_RESAMPLER,
   SEE_PAYLOAD_DIAG_SENSOR_TRIGGER_REQ,
   SEE_PAYLOAD_SET_DISTANCE_BOUND,
   SEE_PAYLOAD_BASIC_GESTURES,
   SEE_PAYLOAD_MULTISHAKE,
   SEE_PAYLOAD_PSMD,
};

class see_std_request {
private:
   bool _has_batch_period;
   int  _batch_period;      // microseconds
   bool _has_flush_period;
   int  _flush_period;      // microseconds

   bool _has_flush_only;
   bool _flush_only;
   bool _has_max_batch;
   bool _max_batch;

   bool _has_payload;
   see_payload_types_e _payload_type;
   union {
      see_std_sensor_config *_std_sensor_config;
      see_self_test_config *_self_test_config;
      see_resampler_config *_resampler_config;
      see_diag_log_trigger_req *_diag_log_trigger_req;
      see_set_distance_bound *_set_distance_bound;
      see_basic_gestures_config * _basic_gestures_config;
      see_multishake_config * _multishake_config;
      see_psmd_config * _psmd_config;
   } _payload;

public:
   see_std_request() {
      _has_batch_period = false;
      _has_flush_period = false;
      _has_flush_only = false;
      _has_max_batch = false;
      _has_payload = false;
      _payload_type = SEE_PAYLOAD_OMITTED;
   }

   void set_batch_spec(int batch_period, int flush_period) {
      set_batch_period(batch_period);
      set_flush_period(flush_period);
   }
   void set_batch_period(int batch_period) {
      _has_batch_period = true;
      _batch_period = batch_period;
   }
   bool has_batch_period() { return _has_batch_period; }
   int  get_batch_period() { return _batch_period; }

   void set_flush_period(int flush_period) {
      _has_flush_period = true;
      _flush_period = flush_period;
   }
   bool has_flush_period() { return _has_flush_period; }
   int  get_flush_period() { return _flush_period; }

   void set_flush_only(bool flush_only) {
      _has_flush_only = true;
      _flush_only = flush_only;
   }
   bool has_flush_only() { return _has_flush_only; }
   bool get_flush_only() { return _flush_only; }

   void set_max_batch(bool max_batch) {
      _has_max_batch = true;
      _max_batch = max_batch;
   }
   bool has_max_batch() { return _has_max_batch; }
   bool get_max_batch() { return _max_batch; }

   bool has_payload() { return _has_payload;}

   int get_payload_type() { return (int) _payload_type;}

   /**
    * @brief see_std_sensor_config = ==== == ======== ===== =======
    */
   void set_payload(see_std_sensor_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_STD_SENSOR;
      _payload._std_sensor_config = payload;
   }
   see_std_sensor_config *get_payload_std_sensor_config() {
      return _payload._std_sensor_config;
   }
   bool is_payload_std_sensor_config() {
      return _payload_type == SEE_PAYLOAD_STD_SENSOR;
   }
   /**
    * @brief see_self_test_config = ==== == ======== ===== =======
    */
   void set_payload(see_self_test_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_SELF_TEST;
      _payload._self_test_config = payload;
   }
   see_self_test_config *get_payload_self_test_config() {
      return _payload._self_test_config;
   }
   bool is_payload_self_test_config() {
      return _payload_type == SEE_PAYLOAD_SELF_TEST;
   }
   /**
    * @brief see_resampler_config = ==== == ============= =======
    */
   void set_payload(see_resampler_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_RESAMPLER;
      _payload._resampler_config = payload;
   }
   see_resampler_config *get_payload_resampler_config() {
      return _payload._resampler_config;
   }
   bool is_payload_resampler_config() {
      return _payload_type == SEE_PAYLOAD_RESAMPLER;
   }
   /**
    * @brief see_diag_log_trigger_req = ==== == ============= =======
    */
   void set_payload(see_diag_log_trigger_req *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_DIAG_SENSOR_TRIGGER_REQ;
      _payload._diag_log_trigger_req = payload;
   }
   see_diag_log_trigger_req *get_payload_diag_log_trigger_req() {
      return _payload._diag_log_trigger_req;
   }
   bool is_payload_diag_log_trigger_req() {
      return _payload_type == SEE_PAYLOAD_DIAG_SENSOR_TRIGGER_REQ;
   }
   /**
    * @brief see_set_distance_bound = ==== == ============= =======
    */
   void set_payload(see_set_distance_bound *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_SET_DISTANCE_BOUND;
      _payload._set_distance_bound = payload;
   }
   see_set_distance_bound *get_payload_set_distance_bound() {
      return _payload._set_distance_bound;
   }
   bool is_payload_set_distance_bound() {
      return _payload_type == SEE_PAYLOAD_SET_DISTANCE_BOUND;
   }
   /**
    * @brief see_basic_gestures_config = ==== == ============= =======
    */
   void set_payload( see_basic_gestures_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_BASIC_GESTURES;
      _payload._basic_gestures_config = payload;
   }
   bool is_payload_basic_gestures_config() {
      return _payload_type == SEE_PAYLOAD_BASIC_GESTURES;
   }
   /**
    * @brief see_multishake_config = ==== == ============= =======
    */
   void set_payload( see_multishake_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_MULTISHAKE;
      _payload._multishake_config = payload;
   }
   bool is_payload_multishake_config() {
      return _payload_type == SEE_PAYLOAD_MULTISHAKE;
   }
   /**
    * @brief see_psmd_config = ==== == ============= =======
    */
   void set_payload(see_psmd_config *payload) {
      _has_payload = true;
      _payload_type = SEE_PAYLOAD_PSMD;
      _payload._psmd_config = payload;
   }
   bool is_payload_psmd_config() {
      return _payload_type == SEE_PAYLOAD_PSMD;
   }
};

enum see_msg_id_e {
   MSGID_ATTR_REQ = 1,
   MSGID_FLUSH_REQ = 2,
   MSGID_DISABLE_REQ = 10, // SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ
   MSGID_RESAMPLER_CONFIG = 512,
   MSGID_GRAVITY_CONFIG = 512,
   MSGID_GYRO_ROT_MATRIX_CONFIG = 512,
   MSGID_BASIC_GESTURES_CONFIG = 512,
   MSGID_MULTISHAKE_CONFIG = 512,
   MSGID_PSMD_CONFIG = 512,
   MSGID_STD_SENSOR_CONFIG = 513, // SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
   MSGID_ON_CHANGE_CONFIG = 514,
   MSGID_SELF_TEST_CONFIG = 515,
   MSGID_SET_DISTANCE_BOUND = 516,
   MSGID_EVENT_GATED_CONFIG = 518,
   MSGID_DIAG_SENSOR_TRIGGER_REQ = 520,
};

enum see_std_client_processor_e {
   SEE_STD_CLIENT_PROCESSOR_SSC = 0,
   SEE_STD_CLIENT_PROCESSOR_APSS = 1,
   SEE_STD_CLIENT_PROCESSOR_ADSP = 2,
   SEE_STD_CLIENT_PROCESSOR_MDSP = 3,
   SEE_STD_CLIENT_PROCESSOR_CDSP = 4,
   SEE_STD_CLIENT_PROCESSOR_COUNT = 5
};

enum see_client_delivery_e {
   SEE_CLIENT_DELIVERY_WAKEUP = 0,
   SEE_CLIENT_DELIVERY_NO_WAKEUP = 1,
};

class see_client_request_message {
public:
   see_client_request_message(sens_uid *suid,
                              see_msg_id_e msg_id,
                              see_std_request* request) {
      _suid.low = suid->low;
      _suid.high = suid->high;
      _msg_id = msg_id;
      _request = request;
   }
   sens_uid *get_suid() { return &_suid;};
   see_msg_id_e get_msg_id() { return _msg_id;}
   see_std_request *get_request() { return _request;}
   std::string get_client_symbolic( see_std_client_processor_e);
   std::string get_wakeup_symbolic( see_client_delivery_e);

   /**
    * @brief suspend_config
    */
   void set_suspend_config(see_std_client_processor_e client,
                           see_client_delivery_e wakeup) {
       _client = client;
       _wakeup = wakeup;
   }
   void get_suspend_config(see_std_client_processor_e &client,
                           see_client_delivery_e &wakeup) {
       client = _client;
       wakeup = _wakeup;
   }

   void set_processor(see_std_client_processor_e client) {
      _client = client;
   }

   void set_wakeup(see_client_delivery_e wakeup){
      _wakeup = wakeup;
   }

private:
   sens_uid _suid;
   see_msg_id_e _msg_id;

   see_std_client_processor_e _client = SEE_STD_CLIENT_PROCESSOR_APSS;
   see_client_delivery_e _wakeup = SEE_CLIENT_DELIVERY_WAKEUP;

   see_std_request* _request;

};
