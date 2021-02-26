/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_salt_payloads.h
** ================================================================ */
#pragma once
// ----------------------------------
// sns_std_sensor.proto:
// message sns_std_sensor_config {
//    required float sample_rate = 1;
// }
// ----------------------------------
class see_std_sensor_config {
public:
   see_std_sensor_config(float sample_rate) { _sample_rate = sample_rate;}
   float get_sample_rate() { return _sample_rate;}
private:
   float _sample_rate;  // hz
};

// ------------------------------------------------------
// sns_physical_sensor_test.proto:
// message sns_physical_sensor_test_config {
//   required sns_physical_sensor_test_type test_type = 1;
// }
// ------------------------------------------------------
enum see_self_test_type_e {
   SELF_TEST_TYPE_SW = 0,
   SELF_TEST_TYPE_HW = 1,
   SELF_TEST_TYPE_FACTORY = 2,
   SELF_TEST_TYPE_COM = 3
};

class see_self_test_config {
public:
   see_self_test_config(see_self_test_type_e test_type) {
      _test_type = test_type;
   }
   see_self_test_type_e get_test_type() { return _test_type;}
   std::string get_test_type_symbolic( see_self_test_type_e test_type);

private:
   see_self_test_type_e _test_type;
};

// --------------------------------------------
// sns_resampler.proto:
// message sns_resampler_config
// {
//   required sns_std_suid sens_uid = 1;
//   required float resampled_rate = 2;
//   required sns_resampler_rate rate_type = 3;
//   required bool filter = 4;
// }
// --------------------------------------------
enum see_resampler_rate {
   SEE_RESAMPLER_RATE_FIXED = 0,
   SEE_RESAMPLER_RATE_MINIMUM = 1
};

class see_resampler_config {
public:
   see_resampler_config(sens_uid *suid,
                        float resampled_rate,
                        see_resampler_rate rate_type,
                        bool filter) {
      _suid.low = suid->low;
      _suid.high = suid->high;
      _resampled_rate = resampled_rate;
      _rate_type = rate_type;
      _filter = filter;
   }

   sens_uid *get_suid() { return &_suid;}
   float get_resampled_rate() { return _resampled_rate; }
   see_resampler_rate get_rate_type() { return _rate_type;}
   std::string get_rate_type_symbolic( see_resampler_rate rate_type);
   bool get_filter() { return _filter;}

private:
   sens_uid _suid;
   float _resampled_rate;
   see_resampler_rate _rate_type;
   bool _filter;
};

// ----------------------------------------------------
// sns_diag_sensor.proto
// message sns_diag_sensor_log_trigger_req {
//   optional fixed64 cookie = 1;
//   required sns_diag_triggered_log_type log_type = 2;
// }
// ----------------------------------------------------
enum see_diag_trigger_log_type {
   SEE_LOG_TYPE_ISLAND_LOG = 1,
   SEE_LOG_TYPE_MEMORY_USAGE_LOG = 2
};

class see_diag_log_trigger_req {
public:
   see_diag_log_trigger_req( see_diag_trigger_log_type log_type) {
      _log_type = log_type;
      _has_cookie = false;
      _cookie = 0;
   }
   see_diag_log_trigger_req( see_diag_trigger_log_type log_type,
                            uint64_t cookie) {
      _log_type = log_type;
      _has_cookie = true;
      _cookie = cookie;
   }
   see_diag_trigger_log_type get_log_type() { return _log_type;}
   bool has_cookie() { return _has_cookie;}
   uint64_t get_cookie() { return _cookie;}
   std::string get_log_type_symbolic();

private:
   see_diag_trigger_log_type _log_type;
   bool _has_cookie;
   uint64_t _cookie;
};

// ----------------------------------------------------
// sns_distance_bound.proto
// message sns_set_distance_bound {
//     required float distance_bound = 1;
// }
// ----------------------------------------------------
class see_set_distance_bound {
public:
   see_set_distance_bound( float distance_bound) {
      _distance_bound = distance_bound;
   }
   float get_distance_bound() { return _distance_bound; }
private:
   float _distance_bound;
};

// ----------------------------------------------------
// sns_basic_gestures.proto
// note: we're not setting any fields.
// message sns_basic_gestures_config  {
//   optional float sleep           = 1;
//   optional float push_threshold  = 2;
//   optional float pull_threshold  = 3;
//   optional float shake_threshold = 4;
//   optional bytes event_mask    = 5;
// }
// ----------------------------------------------------
class see_basic_gestures_config {
public:
   see_basic_gestures_config() {}
private:
   float _sleep;
   float _push_threshold;
   float _pull_threshold;
   float _shake_threshold;
   uint8_t _event_mask;

};

// ----------------------------------------------------
// sns_multishake.proto
// note: we're setting repeated = 0
// message sns_multishake_config {
//  repeated multishake_axis shake_axis = 1
// }
// ----------------------------------------------------
class see_multishake_config {
public:
     see_multishake_config() {}
private:
   uint8_t _shake_axis;
};

// ----------------------------------------------------
// sns_psmd.proto
// message sns_psmd_config {
//   required sns_psmd_type type = 1;
// }
// ----------------------------------------------------
enum see_psmd_type {
  SEE_PSMD_TYPE_STATIONARY = 0,
  SEE_PSMD_TYPE_MOTION = 1
};

class see_psmd_config {
public:
   see_psmd_config( see_psmd_type type) { _type = type;}
   see_psmd_type get_type() { return _type;}
private:
   see_psmd_type _type;
};

