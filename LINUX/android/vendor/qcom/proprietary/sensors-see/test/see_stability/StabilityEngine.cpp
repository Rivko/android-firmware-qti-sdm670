/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: StabilityEngine.cpp
** ================================================================ */

#include <iostream>           // std::cout
#include <map>                // std::map

#include "StabilityEngine.h"
#include "Utility.h"
#include "Parser.h"

namespace sensors_stability {

// Static Variable initialization
// TODO: Not implemented - Random Sensor
const std::string StabilityEngine::RANDOM_SENSOR = "random_sensor";
const std::string StabilityEngine::REQ_TYPE      = "req_type";
const std::string StabilityEngine::SAMPLE_RATE   = "sample_rate";
const std::string StabilityEngine::ODR_RATE      = "odr_rate";

const std::string StabilityEngine::BATCH_PERIOD  = "batch_period";
const std::string StabilityEngine::FLUSH_PERIOD  = "flush_period";
const std::string StabilityEngine::FLUSH_ONLY    = "flush_only";
const std::string StabilityEngine::FLUSH_REQUEST_INTERVAL =
                                                    "flush_request_interval";
const std::string StabilityEngine::REPORT_PERIOD = "report_period";

const std::string StabilityEngine::STD_REQ           = "std";
const std::string StabilityEngine::RES_REQ           = "res";
const std::string StabilityEngine::CAL_REQ           = "cal";
const std::string StabilityEngine::ON_CHG_REQ        = "on_change";
const std::string StabilityEngine::DIST_BOUND_REQ    = "db";

const std::string StabilityEngine::STREAM_DURATION    = "stream_duration";
const std::string StabilityEngine::DELAY              = "delay";

const std::string StabilityEngine::RESAMPLER_RATE      = "resampler_rate";
const std::string StabilityEngine::RESAMPLER_RATE_TYPE = "resampler_rate_type";
const std::string StabilityEngine::RESAMPLER_FILTER    = "resampler_filter";

const std::string StabilityEngine::WAKEUP          = "wakeup";
const std::string StabilityEngine::NO_WAKEUP       = "no_wakeup";

const std::string StabilityEngine::CLIENT    = "client";
const std::string StabilityEngine::DB_LENGTH = "db_length";

// Keys to populate test_param map
// '$' is used as a delimiter
const std::string StabilityEngine::MSG_ID            = "$msg_id";

const std::string StabilityEngine::MIN_SAMP_RATE     = "$min_sample_rate";
const std::string StabilityEngine::MAX_SAMP_RATE     = "$max_sample_rate";
const std::string StabilityEngine::SAMP_RATE         = "$sample_rate";

const std::string StabilityEngine::MIN_ODR_RT        = "$min_odr_rate";
const std::string StabilityEngine::MAX_ODR_RT        = "$max_odr_rate";
const std::string StabilityEngine::ODR_RT            = "$odr_rate";

const std::string StabilityEngine::MIN_RPT_PERIOD    = "$min_report_period";
const std::string StabilityEngine::MAX_RPT_PERIOD    = "$max_report_period";
const std::string StabilityEngine::RPT_PERIOD        = "$report_period";

const std::string StabilityEngine::MIN_BTCH_PERIOD   = "$min_batch_period";
const std::string StabilityEngine::MAX_BTCH_PERIOD   = "$max_batch_period";
const std::string StabilityEngine::BTCH_PERIOD       = "$batch_period";

const std::string StabilityEngine::MIN_FLSH_PERIOD   = "$min_flush_period";
const std::string StabilityEngine::MAX_FLSH_PERIOD   = "$max_flush_period";
const std::string StabilityEngine::FLSH_PERIOD       = "$flush_period";
const std::string StabilityEngine::FLSH_ONLY         = "$flush_only";
const std::string StabilityEngine::FLSH_REQ_INTERVAL = "$flush_req_interval";

const std::string StabilityEngine::MIN_STREAM_DUR    = "$min_stream_dur";
const std::string StabilityEngine::MAX_STREAM_DUR    = "$max_stream_dur";
const std::string StabilityEngine::STREAM_DUR        = "$stream_duration";

const std::string StabilityEngine::MIN_DEL           = "$min_delay";
const std::string StabilityEngine::MAX_DEL           = "$max_delay";
const std::string StabilityEngine::DEL               = "$delay";

const std::string StabilityEngine::MIN_RES_RATE      = "$min_resampler_rate";
const std::string StabilityEngine::MAX_RES_RATE      = "$max_resampler_rate";
const std::string StabilityEngine::RES_RATE          = "$resampler_rate";

const std::string StabilityEngine::RES_RATE_TYPE     = "$resampler_rate_type";
const std::string StabilityEngine::RES_FILTER        = "$resampler_filter";

const std::string StabilityEngine::WK_UP             = "$wakeup";
const std::string StabilityEngine::NO_WK_UP          = "$no_wakeup";

const std::string StabilityEngine::CLNT              = "$client";
const std::string StabilityEngine::DB_LEN            = "$db_len";

// Create an instance of Utility class
Utility utility_stability_obj;

// Create a singleton object of the class. TestCase class will use this object
// to invoke the member functions
StabilityEngine* StabilityEngine::get_stability_engine_instance() {
    static StabilityEngine stability_engine_instance;
    return &stability_engine_instance;
}

// Enum class message ids
StabilityEngine::req_message_ids message_id;

// Enum class resampler rate type
StabilityEngine::resampler_rate_type res_rate_type;

// Enum class std_delivery type
StabilityEngine::std_delivery std_delivery_type;

//----------------------------------------------------------------------------

// Process test parameters
void StabilityEngine::process_test_params(
                                            const sensor_name_sensor_configs&
                                            sensor_configs,
                                            std::map<std::string, float>&
                                            test_params
                                         )
{
    std::map<std::string, float>::iterator it = test_params.begin();

    // Local variable declarations
    const std::string COLON_DELIMITER = ":";
    std::string sensor_type;

    // Get the max capacity of the target
    float max_cap = Parser::get_max_cap();

    // Get cumulative sample rates of all sensors streaming
    float cumulative_sample_rate = Parser::get_cumulative_sample_rate();

    // For max efficiency
    auto sensor_it = sensor_configs.cbegin();

    for(;sensor_it != sensor_configs.cend(); ++sensor_it) {
        sensor_type = sensor_it->first;
        // Insert a dummy value of 0.0f for the the sensor_key(i.e. accel => 0,
        // gyro =>0, mag => 0 etc. since the value is not used anywhere.
        // Reason for this work around is to avoid creating a generic
        // std::map<T1, T2> which is more complex.
        test_params.insert(it, std::pair<std::string, float>(
                                                sensor_type, 0.0f));

        // Reset values before reading the configs for the next sensor
        float resampler_rate = 0.0f;
        float min_samp_rate = 0.0f;
        float max_samp_rate = 0.0f;
        float samp_rate = 0.0f;

        float min_odr = 0.0f;
        float max_odr = 0.0f;
        float odr = 0.0f;

        float min_rpt_period = 0.0f;
        float max_rpt_period = 0.0f;
        float rpt_period = 0.0f;

        float batch_period = 0.0f;
        float min_batch_period = 0.0f;
        float max_batch_period = 0.0f;

        float flush_period = 0.0f;
        float min_flush_period = 0.0f;
        float max_flush_period = 0.0f;
        float flush_only = 0.0f;
        float flush_request_interval = 0.0f;

        float min_str_dur = 0.0f;
        float max_str_dur = 0.0f;
        float str_dur = 0.0f;

        float min_delay = 0.0f;
        float max_delay = 0.0f;
        float delay = 0.0f;

        float rate_type_fixed = 0.0f;
        float rate_type_minimum = 0.0f;
        float filter = 0.0f;

        float wake_up = 0.0f;
        float no_wake_up = 0.0f;

        float msg_id = 0.0f;
        float clnt = 0.0f;
        float db_len = 0.0f;

        for(auto config_it = sensor_it->second.cbegin(); config_it !=
                                    sensor_it->second.cend(); ++config_it) {

            // Type of request
            if(config_it->first == REQ_TYPE) {
                if(config_it->second == RES_REQ) {
                    message_id = StabilityEngine::req_message_ids::
                                                MSGID_RESAMPLER_CONFIG;
                    msg_id = static_cast<float>(message_id);
                    test_params.insert(it,std::pair<std::string, float>(
                                           sensor_type + MSG_ID, msg_id));
                }
                if(config_it->second == CAL_REQ) {
                   message_id = StabilityEngine::req_message_ids::
                                                    MSGID_ON_CHANGE_CONFIG;
                   msg_id = static_cast<float>(message_id);
                   test_params.insert(it,std::pair<std::string, float>(
                                           sensor_type + MSG_ID, msg_id));
                }
                if(config_it->second == STD_REQ) {
                   message_id = StabilityEngine::req_message_ids::
                                                    MSGID_STD_SENSOR_CONFIG;
                   msg_id = static_cast<float>(message_id);
                   test_params.insert(it,std::pair<std::string, float>(
                                           sensor_type + MSG_ID, msg_id));
                }
                if(config_it->second == ON_CHG_REQ) {
                   message_id = StabilityEngine::req_message_ids::
                                                    MSGID_ON_CHANGE_CONFIG;
                   msg_id = static_cast<float>(message_id);
                   test_params.insert(it,std::pair<std::string, float>(
                                           sensor_type + MSG_ID, msg_id));
                }
                if(config_it->second == DIST_BOUND_REQ) {
                   message_id = StabilityEngine::req_message_ids::
                                                MSGID_SET_DISTANCE_BOUND;
                   msg_id = static_cast<float>(message_id);
                   test_params.insert(it,std::pair<std::string, float>(
                                           sensor_type + MSG_ID, msg_id));
                }

            } // end Req type
//----------------------------------------------------------------------------
            // Process resampler rate type
            if(config_it->first == RESAMPLER_RATE_TYPE) {
                if(config_it->second == "fixed") {
                   res_rate_type = StabilityEngine::resampler_rate_type::
                                                    SEE_RESAMPLER_RATE_FIXED;
                   rate_type_fixed = static_cast<float>(res_rate_type);
                   test_params.insert(it, std::pair<std::string,
                                      float>(sensor_type +
                                      RES_RATE_TYPE, rate_type_fixed));
                } else { // minimum
                        res_rate_type = StabilityEngine::resampler_rate_type::
                                                     SEE_RESAMPLER_RATE_MINIMUM;
                        rate_type_minimum = static_cast<float>(res_rate_type);
                        test_params.insert(it, std::pair<std::string,
                                            float>(sensor_type +
                                            RES_RATE_TYPE, rate_type_minimum));
                 }
            } // end Resampler rate type
//----------------------------------------------------------------------------
            // Process resampler filter
            if(config_it->first == RESAMPLER_FILTER) {
                if(config_it->second == "true") {
                   filter = 1.0f;
                   test_params.insert(it, std::pair<std::string, float>(
                                      sensor_type + RES_FILTER, filter));
                } else { // false
                      filter = 0.0f;
                      test_params.insert(it, std::pair<std::string, float>(
                                         sensor_type + RES_FILTER, filter));
                  }

            } // end Resampler filter
//-----------------------------------------------------------------------------
            // Process client
            if(config_it->first == CLIENT) {
                clnt = stof(config_it->second);
                test_params.insert(it,std::pair<std::string, float>(
                                   sensor_type + CLNT, clnt));

            } // end client
//-----------------------------------------------------------------------------
            // Process distance bound
            if(config_it->first == DB_LENGTH) {
                db_len = stof(config_it->second);
                test_params.insert(it,std::pair<std::string, float>(
                                   sensor_type + DB_LEN, db_len));

            } // end distance bound
//-----------------------------------------------------------------------------
            // Process delivery types - wake-up, no_wake-up etc.
            // Note: Test cases that uses these flags are responsible
            //       for casting the value from float to integer
            if(config_it->first == WAKEUP) {
               if(config_it->second == "true") {
                 std_delivery_type = StabilityEngine::std_delivery::WAKEUP;
                 wake_up = static_cast<float>(std_delivery_type);
                 test_params.insert(it, std::pair<std::string, float>(
                                    sensor_type + WK_UP, wake_up));
              }
            }
            if(config_it->first == NO_WAKEUP) {
                if(config_it->second == "true") {
                  std_delivery_type = StabilityEngine::std_delivery::NO_WAKEUP;
                  no_wake_up = static_cast<float>(std_delivery_type);
                  test_params.insert(it, std::pair<std::string, float>(
                                     sensor_type + NO_WK_UP, no_wake_up));
               }
            } // end wake-up/non-wake-up
//----------------------------------------------------------------------------
            // Process sample rate
            if(config_it->first == SAMPLE_RATE) {
                // Sample rate is a range i.e. sample rate = 50:200
                if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                 std::string::npos) {

                    // Get min and max range. e.g. sample_rate = 50.0:200.0
                    std::tie(min_samp_rate, max_samp_rate) =
                             utility_stability_obj.get_min_max_range(
                                                        config_it->second);

                    // Insert into map:e.g. accel_min_sample_rate=>50
                    test_params.insert(it, std::pair<std::string, float>(
                                       sensor_type + MIN_SAMP_RATE,
                                       min_samp_rate));

                    // Insert into map:e.g. accel_max_sample_rate=>200
                    test_params.insert(it, std::pair<std::string,
                                           float>(sensor_type + MAX_SAMP_RATE,
                                                  max_samp_rate));
                }
                // If sample rate is fixed, e.g. sample_rate = 100
                else {
                      samp_rate = stof(config_it->second);
                      // Insert into map:e.g. accel_sample_rate=>100
                      test_params.insert(it, std::pair<std::string, float>(
                                         sensor_type + SAMP_RATE, samp_rate));
                }
            } // end sample_rate
//----------------------------------------------------------------------------
            // Process resample rate
            if(config_it->first == RESAMPLER_RATE) {
                // Resample rate is in a range i.e. resample rate = 50:200
                if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                 std::string::npos) {
                    // Get min and max range. e.g. resample_rate = 50.0:200.0
                    std::tie(min_samp_rate, max_samp_rate) =
                             utility_stability_obj.get_min_max_range(
                                                        config_it->second);
                    // Insert into map:e.g. accel_min_resample_rater=>50
                    test_params.insert(it, std::pair<std::string, float>(
                                        sensor_type + MIN_RES_RATE,
                                        min_samp_rate));
                    // Insert into map:e.g. accel_min_resample_rater=>50:200
                    test_params.insert(it, std::pair<std::string,
                                           float>(sensor_type + MAX_RES_RATE,
                                                  max_samp_rate));
                }
                else {
                       samp_rate = stof(config_it->second);
                        test_params.insert(it, std::pair<std::string, float>(
                                                       sensor_type + RES_RATE,
                                                       samp_rate));
                }
            } // end resampler_rate
//----------------------------------------------------------------------------
            // Process ODR rate
            if(config_it->first == ODR_RATE) {
                odr = stof(config_it->second);
                // Insert into map:e.g. accel_odr_rate=>25
                test_params.insert(it, std::pair<std::string, float>(
                                           sensor_type + ODR_RT, odr));
            } // end odr_rate
//----------------------------------------------------------------------------
            // Process report period
            if(config_it->first == REPORT_PERIOD) {
               // Report period is a range i.e. report_period = 0.2:0.9
               if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                  std::string::npos) {
                  // Get min and max
                  std::tie(min_rpt_period, max_rpt_period) =
                               utility_stability_obj.get_min_max_range(
                                                       config_it->second);

                  // Insert into map:e.g. accel_min_rpt_period=>0.5
                  test_params.insert(it, std::pair<std::string, float>(
                                     sensor_type + MIN_RPT_PERIOD,
                                                   min_rpt_period));

                  test_params.insert(it, std::pair<std::string, float>(
                                    sensor_type + MAX_RPT_PERIOD,
                                                  max_rpt_period));
                }
                else { // report_period is fixed like 0.5
                    rpt_period = stof(config_it->second);

                    // Insert into map:e.g. accel_rpt_period=>0.5
                    test_params.insert(it, std::pair<std::string, float>(
                                       sensor_type + RPT_PERIOD,
                                                     rpt_period));
                }
            } // end report_period
//----------------------------------------------------------------------------
            // Process batch_Period
           if(config_it->first == BATCH_PERIOD) {
               // Batch period is a range i.e. batch_period = 0.2:0.9
               if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                  std::string::npos) {

                  // Get min and max
                  std::tie(min_batch_period, max_batch_period) =
                           utility_stability_obj.get_min_max_range(
                                                     config_it->second);
                  // Insert into map:e.g. accel_min_batch_period=> 1.0
                  test_params.insert(it, std::pair<std::string, float>(
                                              sensor_type + MIN_BTCH_PERIOD,
                                                            min_batch_period));

                  test_params.insert(it, std::pair<std::string, float>(
                                              sensor_type + MAX_BTCH_PERIOD,
                                                            max_batch_period));
                }
                else { // batch_period is fixed like 0.5
                        batch_period = stof(config_it->second);

                        // Insert into map:e.g. accel_batch_period=> 0.5
                        test_params.insert(it, std::pair<std::string, float>(
                                               sensor_type + BTCH_PERIOD,
                                                             batch_period));
                }
           } // end batch_period
//----------------------------------------------------------------------------
            // Process flush_Period
            if(config_it->first == FLUSH_PERIOD) {
               // Flush period is a range i.e. flush_period = 0.2:0.9
               if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                   std::string::npos) {

                  // Get min and max
                  std::tie(min_flush_period, max_flush_period) =
                           utility_stability_obj.get_min_max_range(
                                                       config_it->second);

                  // Insert into map:e.g. accel_min_flush_period=> 1.0
                  test_params.insert(it, std::pair<std::string, float>(
                                              sensor_type + MIN_FLSH_PERIOD,
                                                            min_flush_period));

                  test_params.insert(it, std::pair<std::string, float>(
                                              sensor_type + MAX_FLSH_PERIOD,
                                                            max_flush_period));

                   }
                   else { // flush_period is fixed like 0.5
                         flush_period = stof(config_it->second);
                        // Insert into map:e.g. accel_flush_period=> 0.5
                        test_params.insert(it, std::pair<std::string, float>(
                                                   sensor_type + FLSH_PERIOD,
                                                                 flush_period));
                     }
            } // end flush_period
//----------------------------------------------------------------------------
            // Process flush only
            if(config_it->first == FLUSH_ONLY) {
                if(config_it->second == "false") {
                   flush_only = 0.0f;
                   test_params.insert(it, std::pair<std::string, float>(
                                      sensor_type + FLSH_ONLY, flush_only));
                }
                else { // true
                      flush_only = 1.0f;
                      test_params.insert(it, std::pair<std::string, float>(
                                         sensor_type + FLSH_ONLY, flush_only));
                }
             } // end flush_only
//----------------------------------------------------------------------------
            // Process flush request interval
            if(config_it->first == FLUSH_REQUEST_INTERVAL) {
                flush_request_interval = stof(config_it->second);
                test_params.insert(it, std::pair<std::string, float>(
                                            sensor_type + FLSH_REQ_INTERVAL,
                                                        flush_request_interval));
            } // end flush_request_interval
//----------------------------------------------------------------------------
            // Process stream duration
            if(config_it->first == STREAM_DURATION) {
               // Stream duration is a range i.e. stream_duration = 0.2:0.9
               if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                   std::string::npos) {
                   // Get min and max
                   std::tie(min_str_dur, max_str_dur) =
                            utility_stability_obj.get_min_max_range(
                                                       config_it->second);

                   // Insert into map:e.g. accel_min_stream_duration=>0.5
                   test_params.insert(it, std::pair<std::string, float>(
                                               sensor_type + MIN_STREAM_DUR,
                                                             min_str_dur));

                   test_params.insert(it, std::pair<std::string, float>(
                                               sensor_type + MAX_STREAM_DUR,
                                                             max_str_dur));
                }
                else { // stream duration is fixed like 0.5
                       str_dur = stof(config_it->second);

                       // Insert into map:e.g. accel_stream_duration=>0.5
                       test_params.insert(it, std::pair<std::string, float>(
                                                   sensor_type + STREAM_DUR,
                                                                 str_dur));
                }
            } // end stream_duration
//----------------------------------------------------------------------------
            // Process delay
            if(config_it->first == DELAY) {
               // Delay is a range i.e. delay = 0.2:0.9
               if((config_it->second.find_first_of(COLON_DELIMITER)) !=
                                                  std::string::npos) {
                  // Get min and max
                  std::tie(min_delay, max_delay) =
                                utility_stability_obj.get_min_max_range(
                                                            config_it->second);

                  // Insert into map:e.g. accel_min_delay=>0.5
                  test_params.insert(it, std::pair<std::string, float>(
                                            sensor_type + MIN_DEL, min_delay));

                  test_params.insert(it, std::pair<std::string, float>(
                                            sensor_type + MAX_DEL, max_delay));
                }
                else { // delay is fixed like 0.5
                    delay = stof(config_it->second);

                    // Insert into map:e.g. accel_delay=>0.5
                    test_params.insert(it, std::pair<std::string, float>(
                                                sensor_type + DEL, delay));
                }
            } // end delay
//----------------------------------------------------------------------------
        } // end inner for loop
    } // end outer for loop
 } // end StabilityEngine::process_sensor_configs
} // namespace sensors_stability
///////////////////////////////////////////////////////////////////////////////
