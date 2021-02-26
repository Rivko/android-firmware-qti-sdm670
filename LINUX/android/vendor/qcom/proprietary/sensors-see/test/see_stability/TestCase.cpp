/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: TestCase.cpp
** ================================================================ */

#include <iostream>  // std::cout
#include <map>       // std::map
#include <thread>    // std::thread
#include <chrono>    // std::chrono
#include <vector>    // std::vector
#include <algorithm> // std::find

#include "Utility.h"
#include "StabilityEngine.h"
#include "Template_Impl.hpp"
#include "TestCase.h"
#include "Parser.h"

namespace sensors_stability {

/*========================================================================
 *              Static Variable initialization
 ========================================================================*/
const int TestCase::MICROSECONDS = 1000000;
const int TestCase::MILLISECONDS = 500;

// Client number. Default is 1
int TestCase::client_num  = 1;

// Counter to randomize_min_max
int TestCase::rand_counter = 1;

//----------------------------------------------------------------------------

// Create a singleton object of the class. Parser class will use this object
// to invoke member function get_test_params
TestCase* TestCase::get_test_case_instance() {
    static TestCase test_case_instance;
    return &test_case_instance;
}

// Object of Utility class
Utility utility_obj;

// Object of StabliltyEngine
StabilityEngine* stability_engine_obj;

// Max or min or random rates
const float MAX_RATE = -1.00f;
const float MIN_RATE = -2.00f;
const float RAND_RATE = -3.00f;
//----------------------------------------------------------------------------
// *                    Non-member functions                                 *
//----------------------------------------------------------------------------
    // Print file and line number where exception occured
    void print_file_line_number(const char* file, int line)
    {
        std::cout << "Exception in file: " << file << ", " << "line number: "
                  << line << std::endl;
        std::cout << "FAIL" << std::endl;
    }
//----------------------------------------------------------------------------
    // Return sensor name
     std::string get_sensor_name(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
     {
         std::string sensor_name;
         sensor_name = test_params.find(sensor_type + "$sensor_name")->first;
         return(sensor_name);
     } // end of get_sensor_name()
//----------------------------------------------------------------------------
    // Return sample rate
    float get_sample_rate(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float sample_rate = 0.0f;
        sample_rate = test_params.find(sensor_type + "$sample_rate")->second;
        return(sample_rate);
    } // end of get_sample_rate()
//----------------------------------------------------------------------------
    // Return min sample rate
    float get_min_sample_rate(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        float min_sample_rate = 0.0f;
        min_sample_rate = test_params.find(sensor_type + "$min_sample_rate")->
                                                                        second;
        return(min_sample_rate);
    } // end of get_min_sample_rate()
//----------------------------------------------------------------------------
    // Return max sample rate
    float get_max_sample_rate(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        float max_sample_rate = 0.0f;
        max_sample_rate = test_params.find(sensor_type + "$max_sample_rate")->
                                                                        second;
        return(max_sample_rate);
    } // end of get_max_sample_rate()
//----------------------------------------------------------------------------
    // Return odr rate
    float get_odr_rate(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float odr_rate = 0.0f;
        odr_rate = test_params.find(sensor_type + "$odr_rate")->second;
        return(odr_rate);
    } // end of get_odr_rate()
//----------------------------------------------------------------------------
    // Return stream duration
    float get_stream_duration(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float stream_duration = 0.0f;
        stream_duration = test_params.find(sensor_type + "$stream_duration")->
                                                                        second;
        return(stream_duration);
    } // end of get_stream_duration()
//----------------------------------------------------------------------------
    // Return message id
    int get_msg_id(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        int msg_id = 0;
        msg_id = static_cast<int>(test_params.find(sensor_type +
                                                        "$msg_id")->second);
        return(msg_id);
    } // end of get_msg_id()
//----------------------------------------------------------------------------
    // Return batch period
    float get_batch_period(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float batch_period = 0.0f;
        batch_period = test_params.find(sensor_type + "$batch_period")->second;
        return(batch_period);
    } // end of get_batch_period()
//----------------------------------------------------------------------------
    // Return min batch period
    float get_min_batch_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float min_batch_period = 0.0f;
        min_batch_period = test_params.find(sensor_type + "$min_batch_period")->
                                                                        second;
        return(min_batch_period);
    } // end of get_min_batch_period()
//----------------------------------------------------------------------------
    // Return max batch period
    float get_max_batch_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float max_batch_period = 0.0f;
        max_batch_period = test_params.find(sensor_type + "$max_batch_period")->
                                                                        second;
        return(max_batch_period);
    } // end of get_max_batch_period()
//----------------------------------------------------------------------------
    // Return report period
    float get_report_period(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float report_period = 0.0f;
        report_period = test_params.find(sensor_type + "$report_period")->second;
        return(report_period);
    } // end of get_report_period()
//----------------------------------------------------------------------------
    // Return min report period
    float get_min_report_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float min_report_period = 0.0f;
        min_report_period = test_params.find(sensor_type + "$min_report_period")
                                                                    ->second;
        return(min_report_period);
    } // end of get_min_report_period()
//----------------------------------------------------------------------------
    // Return max report period
    float get_max_report_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float max_report_period = 0.0f;
        max_report_period = test_params.find(sensor_type + "$max_report_period")
                                                                    ->second;
        return(max_report_period);
    } // end of get_max_report_period()
//----------------------------------------------------------------------------
    // Return flush period
    float get_flush_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float flush_period = 0.0f;
        flush_period = test_params.find(sensor_type + "$flush_period")->second;
        return(flush_period);
    } // end of get_flush_period()
//----------------------------------------------------------------------------
    // Return min flush period
    float get_min_flush_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float min_flush_period = 0.0f;
        min_flush_period = test_params.find(sensor_type + "$min_flush_period")->
                                                                        second;
        return(min_flush_period);
    } // end of get_min_flush_period()
//----------------------------------------------------------------------------
    // Return max flush period
    float get_max_flush_period(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float max_flush_period = 0.0f;
        max_flush_period = test_params.find(sensor_type + "$max_flush_period")->
                                                                        second;
        return(max_flush_period);
    } // end of get_max_flush_period()
//----------------------------------------------------------------------------
    // Return flush request interval
    float get_flush_request_interval(
                                        const std::map<std::string,
                                        float>& test_params,
                                        const std::string& sensor_type
                                    )
    {
        float flush_request_interval = 0.0f;
        flush_request_interval = test_params.find(sensor_type +
                                                "$flush_req_interval")->second;
        return(flush_request_interval);
    } // end of get_flush_request_interval()
//----------------------------------------------------------------------------
    // Return flush only
    bool get_flush_only(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        int flush_only = 0;
        flush_only = static_cast<int>(test_params.find(sensor_type +
                                                    "$flush_only")->second);
        return((flush_only == 1) ? true : false);
    } // end of get_flush_only()
//----------------------------------------------------------------------------
    // Return max batch
    bool get_max_batch(
                        const std::map<std::string, float>& test_params,
                        const std::string& sensor_type
                      )
    {
        int max_batch = 0;
        max_batch = static_cast<int>(test_params.find(sensor_type +
                                                   "$max_batch")-> second);
        return((max_batch == 1) ? true : false);
    } // end of get_max_batch()
//----------------------------------------------------------------------------
    // Return db length
    float get_db_length(const std::map<std::string, float>& test_params,
                                  const std::string& sensor_type)
    {
        float db_length = 0.0f;
        db_length = test_params.find(sensor_type + "$db_len")->second;
        return(db_length);
    } // end of get_db length()
//----------------------------------------------------------------------------
    // Return resampler rate type
    int get_resampler_rate_type(const std::map<std::string,
                                        float>& test_params,
                                        const std::string& sensor_type)
    {
        int rate_type = 0;
        rate_type = static_cast<int>(test_params.find(sensor_type
                                           + "$resampler_rate_type")->second);
        return((rate_type == 1) ? 1 : 0);
    } // end of get_resampler_rate_type
//----------------------------------------------------------------------------
    // Return resampler rate
    float get_resampler_rate(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float rate = 0.0f;
        rate = test_params.find(sensor_type + "$resampler_rate")->second;
        return(rate);
    } // end of get_resampler_rate
//----------------------------------------------------------------------------
    // Return min resampler rate
    float get_min_resampler_rate(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        float min_resampler_rate = 0.0f;
        min_resampler_rate = test_params.find(sensor_type +
                                                "$min_resampler_rate")->second;
        return(min_resampler_rate);
    } // end of get_min_resampler_rate()
//----------------------------------------------------------------------------
    // Return max resampler rate
    float get_max_resampler_rate(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        float max_resampler_rate = 0.0f;
        max_resampler_rate = test_params.find(sensor_type +
                                                "$max_resampler_rate")->second;
        return(max_resampler_rate);
    } // end of get_max_resampler_rate()
//----------------------------------------------------------------------------
    // Return resampler filter
    bool get_resampler_filter(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float filter = 0.0f;
        filter = test_params.find(sensor_type + "$resampler_filter")->second;
        return((filter == 1.0) ? true : false);
    } // end of get_resampler_filter
//----------------------------------------------------------------------------
    // Get client number
    int get_client_num(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
        if(test_params.find(sensor_type + "$client") !=
                                            test_params.end()) {
            return(static_cast<int>(test_params.find (sensor_type +
                                                "$client")->second));
        }
        // return 1 as default if not found
        return(1);
    } // end of get_client_num()
//----------------------------------------------------------------------------
    // Get delay
    float get_delay(const std::map<std::string, float>& test_params,
                                            const std::string& sensor_type)
    {
          float delay = 0.0f;
          delay = test_params.find(sensor_type +  "$delay")->second;
          return(delay);
    } // end of get_delay()
//----------------------------------------------------------------------------
    // Return min delay
    float get_min_delay(const std::map<std::string, float>& test_params,
                                        const std::string& sensor_type)
    {
        float min_delay = 0.0f;
        min_delay = test_params.find(sensor_type + "$min_delay")->second;
        return(min_delay);
    } // end of get_min_delay()
//----------------------------------------------------------------------------
    // Return max delay
    float get_max_delay(const std::map<std::string, float>& test_params,
                        const std::string& sensor_type
                       )
    {
        float max_delay = 0.0f;
        max_delay = test_params.find(sensor_type + "$max_delay")->second;
        return(max_delay);
    } // end of get_max_delay()
//----------------------------------------------------------------------------
    // Print random batch period message
    void print_random_batch_period_message(
                                            const std::string& sensor_type,
                                            const float& random_batch_period
                                          )
    {
        long int microseconds = utility_obj.get_microseconds();
        std::cout << "\nRandom batch period of ";
        std::cout << sensor_type << ": ";
        std::cout << random_batch_period * microseconds;
        std::cout << " <microseconds>";
    }
//----------------------------------------------------------------------------
    // Print random report period message
    void print_random_report_period_message(
                                            const std::string& sensor_type,
                                            const float& random_report_period
                                          )
    {
        long int microseconds = utility_obj.get_microseconds();
        std::cout << "\nRandom report period of ";
        std::cout << sensor_type << ": ";
        std::cout << random_report_period * microseconds;
        std::cout << " <microseconds>";
    }
//----------------------------------------------------------------------------
    // Print random flush period message
    void print_random_flush_period_message(
                                            const std::string& sensor_type,
                                            const float& random_flush_period
                                          )
    {
        long int microseconds = utility_obj.get_microseconds();
        std::cout << "\nRandom flush period of ";
        std::cout << sensor_type << ": ";
        std::cout << random_flush_period * microseconds;
        std::cout << " <microseconds>";
    }
//----------------------------------------------------------------------------
    // Print random sample rate message
    void print_random_sample_rate_message(
                                            const std::string& sensor_type,
                                            const float& random_sample_rate
                                          )
    {
        std::cout << "\nRandom sample rate of ";
        std::cout << sensor_type << ": ";
        std::cout << random_sample_rate;
        std::cout << " Hz";
    }
//----------------------------------------------------------------------------
    // Print random resampler rate message
    void print_random_resampler_rate_message(
                                              const std::string& sensor_type,
                                              const float& random_resampler_rate
                                            )
    {
        std::cout << "\nRandom resampler rate of ";
        std::cout << sensor_type << ": ";
        std::cout << random_resampler_rate;
        std::cout << " Hz";
    }
//----------------------------------------------------------------------------
    // Initialize std message request
    void init_std_msg_request(see_std_request& std_request,
                                float batch_period = 0.0f,
                                float flush_period = 0.0f,
                                bool flush_only = false,
                                bool max_batch = false
                              )
    {
        if(batch_period) {
            std_request.set_batch_period(batch_period * TestCase::MICROSECONDS);
            std::cout << "\nbatch period set to "
                      << batch_period * TestCase::MICROSECONDS
                      << " microseconds";
        }
        if(flush_period) {
            std_request.set_flush_period(flush_period * TestCase::MICROSECONDS);
            std::cout << "\nflush period set to "
                      << flush_period * TestCase::MICROSECONDS
                      << " microseconds";
        }
        if(flush_only) {
            std_request.set_flush_only(flush_only);
            std::cout << "\nflush only set to " << std::boolalpha
                      << flush_only;
        }
        if(max_batch) {
            std_request.set_max_batch(max_batch);
            std::cout << "\nmax batch set to " << max_batch << std::boolalpha;
        }
    }
//----------------------------------------------------------------------------
    // Stream Sensor
    salt_rc stream_sensor(see_salt* psalt, sens_uid* target_sensor_suid,
                             const std::string& sensor_type,
                             const float& sample_rate, const float& batch_period,
                             const float& flush_period, const bool& flush_only,
                             const bool& max_batch)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_std_request std_request;
        see_msg_id_e msg_id_config = MSGID_STD_SENSOR_CONFIG;

        // Print std message request
        utility_obj.print_std_message_config(psalt, sensor_type,
                                               target_sensor_suid, sample_rate);
        init_std_msg_request(std_request, batch_period, flush_period,
                                    flush_only, max_batch);
        // Send std request
        rc = utility_obj.std_msg_req(psalt,target_sensor_suid, msg_id_config,
                                                    std_request, sample_rate);
        return(rc);
    } // end of stream_sensor()
//----------------------------------------------------------------------------

    // Resample sensor
    salt_rc resample_sensor(see_salt* psalt, sens_uid* resampler_suid,
                               sens_uid* target_sensor_suid,
                               const int& resampler_rate_type,
                               const std::string& sensor_type,
                               const float& resample_rate,
                               const bool& filter, const float& batch_period,
                               const float& flush_period, const bool& flush_only,
                               const bool& max_batch)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_std_request std_request;
        see_msg_id_e msg_id_config = MSGID_RESAMPLER_CONFIG;
        see_resampler_rate rate_type = SEE_RESAMPLER_RATE_FIXED; // default
        std::string res_rate_type = "fixed";

        if(resampler_rate_type) { //resampler_rate_type == 1
            res_rate_type = "minimum";
            rate_type = SEE_RESAMPLER_RATE_MINIMUM;
        }
        // Print resample message request
        utility_obj.print_resampler_message_config(psalt, sensor_type,
                                                    target_sensor_suid,
                                                    resample_rate, res_rate_type,
                                                    filter
                                                  );
        init_std_msg_request(std_request, batch_period, flush_period,
                                    flush_only, max_batch);
        std::cout << std::endl;
        // Send resample request
        rc = utility_obj.resampler_msg_req(psalt, resampler_suid,
                                            target_sensor_suid,
                                            msg_id_config, std_request,
                                            resample_rate, rate_type, filter
                                          );
        return(rc);
    }// end of resample_sensor
//----------------------------------------------------------------------------
    // On-change sensor
    salt_rc on_change_sensor(see_salt* psalt, sens_uid* target_sensor_suid,
                                sensor* psensor, std::string& sensor_type,
                                const float& batch_period,
                                const float& flush_period,
                                const bool& flush_only, const bool& max_batch)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_std_request std_request;
        utility_obj.print_on_change_message_config(psalt, sensor_type,
                                                     target_sensor_suid
                                                    );
        if("basic_gestures" == psensor->get_type()) {
            see_msg_id_e msg_id_config = MSGID_BASIC_GESTURES_CONFIG;
            init_std_msg_request(std_request, batch_period,
                                                 flush_period, flush_only,
                                                 max_batch);
            std::cout << std::endl;
            // Send basic gestures request
            rc = utility_obj.basic_gestures_req(psalt, target_sensor_suid,
                                                    msg_id_config, std_request);
        }
        else if("psmd" == psensor->get_type()) {
                  see_msg_id_e msg_id_config = MSGID_PSMD_CONFIG;
                  init_std_msg_request(std_request, batch_period, flush_period,
                                            flush_only, max_batch);
                  std::cout << std::endl;
                  // Send psmd request
                  rc = utility_obj.psmd_req(psalt, target_sensor_suid,
                                                msg_id_config, std_request);
        }
        else if("multishake" == psensor->get_type()) {
                  see_msg_id_e msg_id_config = MSGID_MULTISHAKE_CONFIG;
                  init_std_msg_request(std_request, batch_period,
                                              flush_period, flush_only,
                                              max_batch);
                  std::cout << std::endl;
                  // Send multishake request
                  rc = utility_obj.multishake_req(psalt, target_sensor_suid,
                                                         msg_id_config,
                                                         std_request);
        }
        else {
                 see_msg_id_e msg_id_config = MSGID_ON_CHANGE_CONFIG;
                 init_std_msg_request(std_request, batch_period,
                                             flush_period, flush_only,
                                             max_batch);
                 std::cout << std::endl;
                 // Send on-change request
                 rc = utility_obj.on_change_msg_req(psalt, target_sensor_suid,
                                                      msg_id_config, std_request);
        }
        return(rc);
    } // end of on_change_sensor
//----------------------------------------------------------------------------
    // DB sensor
    salt_rc db_sensor(see_salt* psalt, sens_uid* target_sensor_suid,
                        const std::string& sensor_type,
                        const float& db_length, const float& batch_period,
                        const float& flush_period, const bool& flush_only,
                        const bool& max_batch)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_std_request std_request;
        see_msg_id_e msg_id_config = MSGID_SET_DISTANCE_BOUND;
        utility_obj.print_db_message_config(psalt, sensor_type, target_sensor_suid,
                                                db_length);
        init_std_msg_request(std_request, batch_period, flush_period, flush_only,
                                                                    max_batch);
        std::cout << std::endl;
        // Send db request
        rc = utility_obj.distance_bound_req(psalt, target_sensor_suid,
                                             msg_id_config, std_request,
                                             db_length);
        return(rc);
    } // end of db_sensor
//----------------------------------------------------------------------------
    // Stability single client test
    void run_stability(const TestCase::sensor_name_sensor_configs&
                       sensor_config_mapping)
    {
        std::map<std::string, float>test_params;
        // Iterator to test_params
        std::map<std::string, float>::iterator it;

        // Local variables default initialization
        float stream_duration = 0.0f;
        float sample_rate  = 0.0f;
        float min_sample_rate = 0.0f;
        float max_sample_rate = 0.0f;
        float random_sample_rate = 0.0f;
        float resampler_rate = 0.0f;
        float min_resampler_rate = 0.0f;
        float max_resampler_rate = 0.0f;
        float random_resampler_rate = 0.0f;
        float odr_rate = 0.0f;
        float batch_period = 0.0f;
        float min_batch_period = 0.0f;
        float max_batch_period = 0.0f;
        float random_batch_period = 0.0f;
        float report_period = 0.0f;
        float min_report_period = 0.0f;
        float max_report_period = 0.0f;
        float random_report_period = 0.0f;
        float flush_period = 0.0f;
        float min_flush_period = 0.0f;
        float max_flush_period = 0.0f;
        float random_flush_period = 0.0f;
        float flush_request_interval = 0.0f;
        float db_length = 0.0f;

        std::string sensor_name = "";
        std::string req_type = "";

        int resampler_rate_type = 0;
        int req_type_msg_id = 0;

        bool flush_only = false;
        bool filter = false;
        bool max_batch = false;

        salt_rc rc = SALT_RC_FAILED;
        stability_engine_obj->process_test_params(sensor_config_mapping,
                                                 test_params);

        it = test_params.begin();
        // Get usta_log_status
        bool usta_log_status = Parser::get_usta_log_status();
        set_usta_logging(usta_log_status);

        // Get instance
        see_salt* psalt = see_salt::get_instance();
        // Required by David's API
        char* argv[] = {NULL};
        if(psalt == nullptr) {
            print_file_line_number(__FILE__, __LINE__);
            throw "psalt not instantiated.\n";
        }
        psalt->begin(0, argv);
        // Check if sensor config is present
        if(it != test_params.end()) {
            std::string sensor_type = (*it).first;

            if(!sensor_type.empty()) {
                sens_uid* target_sensor_suid = nullptr;
                sens_uid resampler_suid;

                // Get random seed. Used only in case of randomization
                unsigned long long seed = Parser::get_seed();

                // Get sensor name
                if(test_params.find(sensor_type + "$sensor_name") !=
                                           test_params.end()) {
                    sensor_name = get_sensor_name(test_params, sensor_type);
                }

                // Get stream duration
                if(test_params.find(sensor_type + "$stream_duration") !=
                                                test_params.end()) {
                    stream_duration = get_stream_duration(test_params,
                                                            sensor_type);
                }

                // Get message id
                if(test_params.find(sensor_type + "$msg_id") !=
                                                test_params.end()) {
                    req_type_msg_id = get_msg_id(test_params, sensor_type);
                }

                // Get batch period
                if(test_params.find(sensor_type + "$batch_period") !=
                                               test_params.end()) {
                    batch_period = get_batch_period(test_params, sensor_type);
                }

                // Get report period
                if(test_params.find(sensor_type + "$report_period") !=
                                               test_params.end()) {
                    report_period = get_report_period(test_params, sensor_type);
                }

                // Get flush period
                if(test_params.find(sensor_type + "$flush_period") !=
                                              test_params.end()) {
                    flush_period = get_flush_period(test_params, sensor_type);
                }

                // Get flush request interval
                if(test_params.find(sensor_type + "$flush_req_interval") !=
                                            test_params.end()) {
                    flush_request_interval = get_flush_request_interval
                                                    (test_params, sensor_type);
                }

                // Get max batch
                if(test_params.find(sensor_type + "$max_batch") !=
                                            test_params.end()) {
                    max_batch = get_max_batch(test_params, sensor_type);
                }

                // Get db length
                if(test_params.find(sensor_type + "$db_len") !=
                                               test_params.end()) {
                    db_length = get_db_length(test_params, sensor_type);
                }

                // Get resampler rate
                if(test_params.find(sensor_type + "$resampler_rate") !=
                                            test_params.end()) {
                    resampler_rate = get_resampler_rate(test_params, sensor_type);
                }

                // Get resampler rate type
                if(test_params.find(sensor_type + "$resampler_rate_type") !=
                                                       test_params.end()) {
                    resampler_rate_type = get_resampler_rate_type(test_params,
                                                                    sensor_type);
                }

                // Get resampler filter - true or false
                if(test_params.find(sensor_type + "$resampler_filter") !=
                                                test_params.end()) {
                    filter = get_resampler_filter(test_params, sensor_type);
                }

                // Get flush only
                if(test_params.find(sensor_type + "$flush_only") !=
                                                test_params.end()) {
                    flush_only = get_flush_only(test_params, sensor_type);
                }

                // Get min batch period
                if(test_params.find(sensor_type + "$min_batch_period") !=
                                             test_params.end()) {
                    min_batch_period = get_min_batch_period(test_params,
                                                                sensor_type);
                }

                // Get max batch period
                if(test_params.find(sensor_type + "$max_batch_period") !=
                                             test_params.end()) {
                    max_batch_period = get_max_batch_period(test_params,
                                                                sensor_type);
                }

                // Get min report period
                if(test_params.find(sensor_type + "$min_report_period") !=
                                             test_params.end()) {
                    min_report_period = get_min_report_period(test_params,
                                                          sensor_type);
                }

                // Get max report period
                if(test_params.find(sensor_type + "$max_report_period") !=
                                             test_params.end()) {
                    max_report_period = get_max_report_period(test_params,
                                                          sensor_type);
                }

                // Get min flush period
                if(test_params.find(sensor_type + "$min_flush_period") !=
                                             test_params.end()) {
                    min_flush_period = get_min_flush_period(test_params,
                                                                sensor_type);
                }

                // Get max flush period
                if(test_params.find(sensor_type + "$max_flush_period") !=
                                             test_params.end()) {
                    max_flush_period = get_max_flush_period(test_params,
                                                                sensor_type);
                }

                // Get sample rate
                if(test_params.find(sensor_type + "$sample_rate") !=
                                           test_params.end()) {
                    sample_rate = get_sample_rate(test_params, sensor_type);
                }

                // Get odr rate
                if(test_params.find(sensor_type + "$odr_rate") !=
                                                        test_params.end()) {
                    odr_rate = get_odr_rate(test_params, sensor_type);
                }

                // Check request type
                if(req_type_msg_id == static_cast<int>(StabilityEngine::
                                                req_message_ids::
                                                MSGID_STD_SENSOR_CONFIG)) {
                    req_type = "std";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                  StabilityEngine::
                                                  req_message_ids::
                                                  MSGID_RESAMPLER_CONFIG)) {
                        req_type = "res";
                        resampler_suid = utility_obj.get_resampler_suid(psalt);
                }
                else if(req_type_msg_id == static_cast<int>(
                                                StabilityEngine::
                                                req_message_ids::
                                                MSGID_ON_CHANGE_CONFIG)) {
                         req_type = "on_change";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                StabilityEngine::
                                                req_message_ids::
                                                MSGID_SET_DISTANCE_BOUND)) {
                         req_type = "db";
                }
                else {
                        print_file_line_number(__FILE__, __LINE__);
                        throw "Invalid message request type\n";
                }
                // Get target sensor suid
                target_sensor_suid = utility_obj.get_target_sensor_suid(
                                                    psalt,
                                                    target_sensor_suid,
                                                    sensor_type, req_type,
                                                    sensor_name);
                if(target_sensor_suid == nullptr) {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "target_sensor_suid not found\n";
                }
                sensor* psensor = psalt->get_sensor(target_sensor_suid);
                if(psensor == nullptr) {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "psensor is nullptr\n";
                }
                if(sample_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(sample_rate == MAX_RATE && psensor->has_max_rate()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                            sensor_type);
                }
                if(resampler_rate == MIN_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_min_odr_rate(psensor,
                                                                   sensor_type);
                }
                if(resampler_rate == MAX_RATE && psensor->has_max_rate()) {
                    resampler_rate = utility_obj.get_max_odr_rate(psensor,
                                                               sensor_type);
                }
                if(odr_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(odr_rate == MAX_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                                sensor_type);
                }
                // Get min sample rate
                if(test_params.find(sensor_type + "$min_sample_rate") !=
                                            test_params.end()) {
                    min_sample_rate = get_min_sample_rate(test_params,
                                                            sensor_type);
                }

                // Get max sample rate
                if(test_params.find(sensor_type + "$max_sample_rate") !=
                                            test_params.end()) {
                    max_sample_rate = get_max_sample_rate(test_params,
                                                                sensor_type);
                }

                // Get min resampler rate
                if(test_params.find(sensor_type + "$min_resampler_rate") !=
                                            test_params.end()) {
                    min_resampler_rate = get_min_resampler_rate(test_params,
                                                                  sensor_type);
                }

                // Get max resampler rate
                if(test_params.find(sensor_type + "$max_resampler_rate") !=
                                            test_params.end()) {
                    max_resampler_rate = get_max_resampler_rate(test_params,
                                                                  sensor_type);
                }

                // If random odr rate is requested
                if((sample_rate == RAND_RATE || odr_rate == RAND_RATE)
                                                && psensor->has_rates()) {
                    sample_rate = utility_obj.get_random_odr_rate
                                                    (
                                                       psensor,
                                                       sensor_type, seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                if(resampler_rate == RAND_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_random_odr_rate
                                                    (
                                                       psensor,
                                                       sensor_type, seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Get random batch period
                if(min_batch_period && max_batch_period) {
                    random_batch_period = get_random_min_max
                                                    (
                                                       min_batch_period,
                                                       max_batch_period,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Get random report period
                if(min_report_period && max_report_period) {
                    random_report_period = get_random_min_max
                                                    (
                                                       min_report_period,
                                                       max_report_period,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Get random flush period
                if(min_flush_period && max_flush_period){
                    random_flush_period = get_random_min_max
                                                    (
                                                       min_flush_period,
                                                       max_flush_period,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Get random sample rate
                if(min_sample_rate && max_sample_rate) {
                    if(min_sample_rate == MIN_RATE && max_sample_rate ==
                                                                    MAX_RATE) {
                        min_sample_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_sample_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_sample_rate = get_random_min_max
                                                    (
                                                       min_sample_rate,
                                                       max_sample_rate,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Get random resampler rate
                if(min_resampler_rate && max_resampler_rate) {
                    if(min_resampler_rate == MIN_RATE && max_resampler_rate ==
                                                                    MAX_RATE) {
                        min_resampler_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_resampler_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_resampler_rate = get_random_min_max
                                                    (
                                                       min_resampler_rate,
                                                       max_resampler_rate,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                if(report_period) {
                    batch_period = report_period;
                }
                if(random_batch_period) {
                    print_random_batch_period_message(sensor_type,
                                                       random_batch_period);
                    batch_period = random_batch_period;
                }
                if(random_report_period) {
                    print_random_report_period_message(sensor_type,
                                                       random_report_period);
                    batch_period = random_report_period;
                }
                if(random_flush_period) {
                    print_random_flush_period_message(sensor_type,
                                                       random_flush_period);
                    flush_period = random_flush_period;
                }
                if(random_sample_rate) {
                    print_random_sample_rate_message(sensor_type,
                                                      random_sample_rate);
                    sample_rate = random_sample_rate;
                }
                if(random_resampler_rate) {
                    print_random_resampler_rate_message(sensor_type,
                                                          random_resampler_rate);
                    resampler_rate = random_resampler_rate;
                }

                if(req_type == "std") {
                    rc = stream_sensor(psalt, target_sensor_suid, sensor_type,
                                          sample_rate, batch_period,
                                          flush_period, flush_only, max_batch);
                }
                else if(req_type == "res") {
                    rc = resample_sensor(psalt, &resampler_suid,
                                          target_sensor_suid, resampler_rate_type,
                                          sensor_type, resampler_rate, filter,
                                          batch_period, flush_period, flush_only,
                                          max_batch);
                }
                else if(req_type == "on_change") {
                         rc = on_change_sensor(psalt, target_sensor_suid,
                                                    psensor, sensor_type,
                                                    batch_period, flush_period,
                                                    flush_only, max_batch);
                }
                else if(req_type == "db") {
                         rc = db_sensor(psalt, target_sensor_suid, sensor_type,
                                          db_length, batch_period, flush_period,
                                          flush_only, max_batch);
                }
                else {
                       print_file_line_number(__FILE__, __LINE__);
                       throw "Invalid request type\n";
                }
                // Stream duration
                if(rc == SALT_RC_SUCCESS) {
                    utility_obj.stream_duration(stream_duration);
                    // Stop sensor streaming
                    utility_obj.print_disable_sensor_message
                                                        (
                                                            psalt, sensor_type,
                                                            target_sensor_suid
                                                        );
                    if(req_type == "res") {
                        rc = utility_obj.disable_sensor_for_resampler
                                                            (
                                                                psalt,
                                                                &resampler_suid
                                                            );
                    }
                    else {
                           rc = utility_obj.disable_sensor
                                                        (
                                                            psalt,
                                                            target_sensor_suid
                                                        );
                    }
                    if(rc == SALT_RC_SUCCESS) {
                        std::cout << "PASS" << std::endl;
                    }
                    else {
                           std::cout << "rc = " << rc << std::endl;
                           std::cout << "FAIL" << std::endl;
                    }
                }
                else {
                      print_file_line_number(__FILE__, __LINE__);
                      std::cout << sensor_type << " ";
                      throw "failed to stream\n";
                }
            }
        }
        else {
                print_file_line_number(__FILE__, __LINE__);
                throw "No sensor configuration found\n";
        }
    } // end of run_stability
//----------------------------------------------------------------------------
    // Concurrency: single or multi client tests
    void run_concurrency(const TestCase::sensor_name_sensor_configs&
                         sensor_config_mapping)
    {
        std::string sensor_name = "";
        std::string req_type = "";
        const std::string DOLLAR_DELIMITER = "$";
        int req_type_msg_id = 0;

        std::map<std::string, float>test_params;
        stability_engine_obj->process_test_params(sensor_config_mapping,
                                                 test_params);
        // Get test duration
        float test_duration = Parser::get_test_duration();

        // Required by David's API
        char* argv[] = {NULL};

        // Get number of QMI clients for the test
        int num_qmi_clients  = Parser::get_num_qmi_clients();

        // Vector pair of sensor and its suid. Sensors passed from config file
        // are populated to this vector
        std::vector<std::pair<std::string, sens_uid*>> sensor_suid_pair
                                                        [num_qmi_clients];

        // Vector pair of streamed sensor and its suid. Sensors whose requests
        // are streamed successfully are populated to this vector
        std::vector<std::pair<std::string, sens_uid*>> streamed_sens_suid_pair
                                                        [num_qmi_clients];

        // Get random seed. Used only in case of randomization
        unsigned long long seed = Parser::get_seed();

        // Get usta_log_status
        bool usta_log_status = Parser::get_usta_log_status();
        set_usta_logging(usta_log_status);

        // Number of qmi instances
        see_salt* psalt[num_qmi_clients];
        if (num_qmi_clients > 0) {
            for(int i = 0; i < num_qmi_clients; ++i) {
                psalt[i] = nullptr;
            }
        }

        for(int i = 0; i < num_qmi_clients; ++i) {
            // Get see_salt instances
            psalt[i] = see_salt::get_instance();
            if(psalt[i] == nullptr) {
                print_file_line_number(__FILE__, __LINE__);
                throw "psalt not instantiated.\n";
            }
            psalt[i]->begin(0, argv);
        }
        // Iterator to test_params
        std::map<std::string, float>::iterator it;
        // Get sensor configs from multi-map
        sens_uid resampler_suid;
        for(it = test_params.begin(); it != test_params.end(); ++it) {
            sens_uid* target_sensor_suid = nullptr;
            if((*it).first.find_first_of(DOLLAR_DELIMITER) ==
                                         std::string::npos) {
                     std::string sensor_type = (*it).first;

                // Get the actual sensor name passed from text file
                std::string save_sensor_type = sensor_type;

                if(!sensor_type.empty()) {
                    if(test_params.find(sensor_type + "$sensor_name") !=
                                           test_params.end()) {
                            sensor_name = get_sensor_name(test_params,
                                                            sensor_type);
                    }

                    if(test_params.find(sensor_type + "$msg_id") !=
                                                test_params.end()) {
                        req_type_msg_id = get_msg_id(test_params, sensor_type);
                    }

                    // Get client number associated with the sensor
                    TestCase::client_num = get_client_num(test_params,
                                                                sensor_type);

                    // If muliple sensors with same name are present
                    // digit is used to differientiate it (e.g. accel
                    // and accel2, accel3 etc.). Remove digit from the sensor
                    sensor_type.erase(std::remove_if(sensor_type.
                                    begin(), sensor_type.end(), &isdigit),
                                    sensor_type.end());

                    // Check request type
                    if(req_type_msg_id == static_cast<int>(
                                       StabilityEngine::req_message_ids::
                                       MSGID_STD_SENSOR_CONFIG)) {
                        req_type = "std";
                    }
                    else if(req_type_msg_id == static_cast<int>(
                                        StabilityEngine::req_message_ids::
                                        MSGID_ON_CHANGE_CONFIG)) {
                             req_type = "on_change";
                    }
                    else if(req_type_msg_id == static_cast<int>(
                                        StabilityEngine::req_message_ids::
                                        MSGID_SET_DISTANCE_BOUND)) {
                            req_type = "db";
                    }
                    else if(req_type_msg_id == static_cast<int>(
                                                  StabilityEngine::
                                                  req_message_ids::
                                                  MSGID_RESAMPLER_CONFIG)) {
                             req_type = "res";
                            if((num_qmi_clients > 0) && (psalt[TestCase::client_num - 1] == nullptr)) {
                                print_file_line_number(__FILE__, __LINE__);
                                throw "psalt not instantiated\n";
                            }
                             resampler_suid = utility_obj.get_resampler_suid
                                              (
                                                psalt[TestCase::client_num - 1]
                                              );
                    }
                    else {
                            print_file_line_number(__FILE__, __LINE__);
                            throw "Invalid message request\n";
                    }
                    if((num_qmi_clients > 0) && (psalt[TestCase::client_num - 1] == nullptr)) {
                        print_file_line_number(__FILE__, __LINE__);
                        throw "psalt not instantiated\n";
                    }
                    target_sensor_suid = utility_obj.get_target_sensor_suid
                                             (
                                                psalt[TestCase::client_num - 1],
                                                target_sensor_suid,
                                                save_sensor_type, req_type,
                                                sensor_name
                                             );
                    if(target_sensor_suid != nullptr) {
                        //print_file_line_number(__FILE__, __LINE__);
                        //throw "target_sensor_suid not found\n";

                        // Populate target sensor type and its suid to a
                        // vector pair. It is preferred to use this approach
                        // since getting suid's for each sensor takes a few
                        // milliseconds and in concurrency test,
                        // all message requests need to be sent without any
                        // delay between request
                        sensor_suid_pair[TestCase::client_num - 1].push_back(
                                                       std::make_pair(
                                                       save_sensor_type,
                                                       target_sensor_suid));
                        //std::cout << sensor_suid_pair[client_num-1].size()
                        //          << std::endl;
                        target_sensor_suid = nullptr;
                    }
                    else {
                        continue;
                    }
                }
                else {
                      print_file_line_number(__FILE__, __LINE__);
                      throw "No sensor configuration found\n";
                }
            }
        } // end of for loop

        // Get sensor type and target sensor suid from vector pair
        for(int i = 0; i < num_qmi_clients; ++i) {
             std::string sensor_type = "";
             sens_uid* target_sensor_suid = nullptr;

             for(std::vector<std::pair<std::string, sens_uid*>>::
                    const_iterator iter = sensor_suid_pair[i].begin();
                                            iter != sensor_suid_pair[i].
                                            end(); ++iter) {
                sensor_type = iter->first;
                target_sensor_suid = iter->second;

                // Local variables default initialization
                float sample_rate  = 0.0f;
                float min_sample_rate = 0.0f;
                float max_sample_rate = 0.0f;
                float random_sample_rate = 0.0f;
                float resampler_rate = 0.0f;
                float min_resampler_rate = 0.0f;
                float max_resampler_rate = 0.0f;
                float random_resampler_rate = 0.0f;
                float odr_rate = 0.0f;
                float batch_period = 0.0f;
                float min_batch_period = 0.0f;
                float max_batch_period = 0.0f;
                float random_batch_period = 0.0f;
                float report_period = 0.0f;
                float min_report_period = 0.0f;
                float max_report_period = 0.0f;
                float random_report_period = 0.0f;
                float flush_period = 0.0f;
                float min_flush_period = 0.0f;
                float max_flush_period = 0.0f;
                float random_flush_period = 0.0f;
                float flush_request_interval = 0.0f;
                float db_length = 0.0f;
                float delay = 0.0f;

                int  resampler_rate_type = 0;
                bool flush_only = false;
                bool filter = false;
                bool max_batch = false;

                // Get message id
                if(test_params.find(sensor_type + "$msg_id") !=
                                                test_params.end()) {
                    req_type_msg_id = get_msg_id(test_params, sensor_type);
                }

                // Get sample rate
                if(test_params.find(sensor_type + "$sample_rate") !=
                                           test_params.end()) {
                    sample_rate = get_sample_rate(test_params, sensor_type);
                }

                // Get odr rate
                if(test_params.find(sensor_type + "$odr_rate") !=
                                                        test_params.end()) {
                    odr_rate = get_odr_rate(test_params, sensor_type);
                }

                // Get batch period of sensor, if exists
                if(test_params.find(sensor_type + "$batch_period") !=
                                               test_params.end()) {
                    batch_period = get_batch_period(test_params, sensor_type);
                }

                // Get report period of sensor, if exists
                if(test_params.find(sensor_type + "$report_period") !=
                                               test_params.end()) {
                    report_period = get_report_period(test_params, sensor_type);
                }

                // Get flush period
                if(test_params.find(sensor_type + "$flush_period") !=
                                              test_params.end()) {
                    flush_period = get_flush_period(test_params, sensor_type);
                }

                // Get flush request interval
                if(test_params.find(sensor_type + "$flush_req_interval") !=
                                            test_params.end()) {
                    flush_request_interval = get_flush_request_interval
                                                    (test_params, sensor_type);
                }

                // Get max batch
                if(test_params.find(sensor_type + "$max_batch") !=
                                            test_params.end()) {
                    max_batch = get_max_batch(test_params, sensor_type);
                }

                // Get db length
                if(test_params.find(sensor_type + "$db_len") !=
                                               test_params.end()) {
                    db_length = get_db_length(test_params, sensor_type);
                }

                // Get resampler rate
                if(test_params.find(sensor_type + "$resampler_rate") !=
                                            test_params.end()) {
                    resampler_rate = get_resampler_rate(test_params, sensor_type);
                }

                // Get resampler rate type - fixed or minimum
                if(test_params.find(sensor_type + "$resampler_rate_type") !=
                                                       test_params.end()) {
                    resampler_rate_type = get_resampler_rate_type(test_params,
                                                                    sensor_type);
                }

                // Get resampler filter type - true or false
                if(test_params.find(sensor_type + "$resampler_filter") !=
                                                test_params.end()) {
                    filter = get_resampler_filter(test_params, sensor_type);
                }

                // Get flush only
                if(test_params.find(sensor_type + "$flush_only") !=
                                                test_params.end()) {
                    flush_only = get_flush_only(test_params, sensor_type);
                }

                // Get min batch period
                if(test_params.find(sensor_type + "$min_batch_period") !=
                                             test_params.end()) {
                    min_batch_period = get_min_batch_period(test_params,
                                                        sensor_type);
                }

                // Get max batch period
                if(test_params.find(sensor_type + "$max_batch_period") !=
                                             test_params.end()) {
                    max_batch_period = get_max_batch_period(test_params,
                                                         sensor_type);
                }

                // Get min report period
                if(test_params.find(sensor_type + "$min_report_period") !=
                                             test_params.end()) {
                    min_report_period = get_min_report_period(test_params,
                                                          sensor_type);
                }

                // Get max report period
                if(test_params.find(sensor_type + "$max_report_period") !=
                                             test_params.end()) {
                    max_report_period = get_max_report_period(test_params,
                                                           sensor_type);
                }

                // Get min flush period
                if(test_params.find(sensor_type + "$min_flush_period") !=
                                             test_params.end()) {
                    min_flush_period = get_min_flush_period(test_params,
                                                         sensor_type);
                }

                // Get max flush period
                if(test_params.find(sensor_type + "$max_flush_period") !=
                                             test_params.end()) {
                    max_flush_period = get_max_flush_period(test_params,
                                                         sensor_type);
                }

                // Get min sample rate
                if(test_params.find(sensor_type + "$min_sample_rate") !=
                                            test_params.end()) {
                    min_sample_rate = get_min_sample_rate(test_params,
                                                       sensor_type);
                }

                // Get max sample rate
                if(test_params.find(sensor_type + "$max_sample_rate") !=
                                            test_params.end()) {
                    max_sample_rate = get_max_sample_rate(test_params,
                                                       sensor_type);
                }

                // Get min resampler rate
                if(test_params.find(sensor_type + "$min_resampler_rate") !=
                                            test_params.end()) {
                    min_resampler_rate = get_min_resampler_rate(test_params,
                                                                  sensor_type);
                }

                // Get delay
                if(test_params.find(sensor_type + "$delay") !=
                                             test_params.end()) {
                    delay = get_delay(test_params, sensor_type);
                }

                // Get max resampler rate
                if(test_params.find(sensor_type + "$max_resampler_rate") !=
                                            test_params.end()) {
                    max_resampler_rate = get_max_resampler_rate(test_params,
                                                                  sensor_type);
                }
                // Pointer to sensor target
                sensor* psensor = psalt[i]->get_sensor(target_sensor_suid);
                if(psensor == nullptr) {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "psensor is nullptr\n";
                }
                if(sample_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(sample_rate == MAX_RATE && psensor->has_max_rate()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                            sensor_type);
                }
                if(resampler_rate == MIN_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_min_odr_rate(psensor,
                                                                   sensor_type);
                }
                if(resampler_rate == MAX_RATE && psensor->has_max_rate()) {
                    resampler_rate = utility_obj.get_max_odr_rate(psensor,
                                                               sensor_type);
                }
                if(odr_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(odr_rate == MAX_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                                sensor_type);
                }
                if((sample_rate == RAND_RATE || odr_rate == RAND_RATE)
                                                    && psensor->has_rates()) {
                    sample_rate = utility_obj.get_random_odr_rate(
                                                     psensor,
                                                     sensor_type, seed,
                                                     TestCase::rand_counter++
                                                   );
                }
                if(resampler_rate == RAND_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_random_odr_rate
                                                    (
                                                       psensor,
                                                       sensor_type, seed,
                                                       TestCase::rand_counter++
                                                    );
                }

                // Get random batch period
                if(min_batch_period && max_batch_period) {
                     random_batch_period = get_random_min_max(
                                                        min_batch_period,
                                                        max_batch_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random report period
                if(min_report_period && max_report_period) {
                     random_report_period = get_random_min_max(
                                                        min_report_period,
                                                        max_report_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random flush period
                if(min_flush_period && max_flush_period){
                     random_flush_period = get_random_min_max(
                                                        min_flush_period,
                                                        max_flush_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random sample rate
                if(min_sample_rate && max_sample_rate) {
                    if(min_sample_rate == MIN_RATE && max_sample_rate ==
                                                                    MAX_RATE) {
                        min_sample_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_sample_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_sample_rate = get_random_min_max(
                                                        min_sample_rate,
                                                        max_sample_rate,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random resampler rate
                if(min_resampler_rate && max_resampler_rate) {
                    if(min_resampler_rate == MIN_RATE && max_resampler_rate ==
                                                                    MAX_RATE) {
                        min_resampler_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_resampler_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_resampler_rate = get_random_min_max
                                                    (
                                                       min_resampler_rate,
                                                       max_resampler_rate,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
                // Check request type
                if(req_type_msg_id == static_cast<int>(
                                       StabilityEngine::req_message_ids::
                                       MSGID_STD_SENSOR_CONFIG)) {
                     req_type = "std";
                }
                else if(req_type_msg_id == static_cast<int>(
                                       StabilityEngine::req_message_ids::
                                       MSGID_ON_CHANGE_CONFIG)) {
                         req_type = "on_change";
                }
                else if(req_type_msg_id == static_cast<int>(
                                       StabilityEngine::req_message_ids::
                                       MSGID_SET_DISTANCE_BOUND)) {
                         req_type = "db";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                  StabilityEngine::
                                                  req_message_ids::
                                                  MSGID_RESAMPLER_CONFIG)) {
                         req_type = "res";
                     //resampler_suid = utility_obj.get_resampler_suid(psalt[i]);
                }
                else {
                        std::cout << "Invalid message request " << std::endl;
                        std::cout << "FAIL" << std::endl;
                }
                if(report_period) {
                    batch_period = report_period;
                }
                if(random_batch_period) {
                    print_random_batch_period_message(sensor_type,
                                                        random_batch_period);
                    batch_period = random_batch_period;
                }
                if(random_report_period) {
                    print_random_report_period_message(sensor_type,
                                                        random_report_period);
                    batch_period = random_report_period;
                }
                if(random_flush_period) {
                    print_random_flush_period_message(sensor_type,
                                                        random_flush_period);
                    flush_period = random_flush_period;
                }
                if(random_sample_rate) {
                    print_random_sample_rate_message(sensor_type,
                                                        random_sample_rate);
                    sample_rate = random_sample_rate;
                }
                if(random_resampler_rate) {
                    print_random_resampler_rate_message(sensor_type,
                                                          random_resampler_rate);
                    resampler_rate = random_resampler_rate;
                }
                salt_rc rc = SALT_RC_FAILED;
                if(req_type == "std") {
                    rc = stream_sensor(psalt[i], target_sensor_suid, sensor_type,
                                          sample_rate, batch_period,
                                          flush_period, flush_only, max_batch);
                }
                else if(req_type == "res") {
                        rc = resample_sensor(psalt[i], &resampler_suid,
                                                target_sensor_suid,
                                                resampler_rate_type,
                                                sensor_type,
                                                resampler_rate, filter,
                                                batch_period,
                                                flush_period, flush_only,
                                                max_batch
                                            );
                }
                else if(req_type == "on_change") {
                        rc = on_change_sensor(psalt[i], target_sensor_suid,
                                                    psensor, sensor_type,
                                                    batch_period, flush_period,
                                                    flush_only, max_batch);
                }
                else if(req_type == "db") {
                         rc = db_sensor(psalt[i], target_sensor_suid, sensor_type,
                                          db_length, batch_period, flush_period,
                                          flush_only, max_batch);
                }
                else {
                       std::cout << "Invalid request type" << std::endl;
                }
                if(rc == SALT_RC_SUCCESS) {
                    // Add streamed sensor and its suid to vector
                    // only if it is not present for this client
                    const auto tmp = std::make_pair(sensor_type,
                                                        target_sensor_suid);
                    if(!std::any_of(streamed_sens_suid_pair[i].begin(),
                                    streamed_sens_suid_pair[i].end(),
                                    [&target_sensor_suid]
                                    (const std::pair<std::string, sens_uid*>& e)
                                    {return e.second == target_sensor_suid;})) {

                            streamed_sens_suid_pair[i].push_back(std::make_pair
                                                        (
                                                            sensor_type,
                                                            target_sensor_suid)
                                                        );
                    }
                }
                else {
                      std::cout << sensor_type << " failed to stream" << std::endl;
                      std::cout << "FAIL" << std::endl;
                }
                // If same sensors are streamed with same client, delay
                // is required
                if(delay) {
                    std::cout << "Delay before sending in next request: ";
                    std::cout << delay << " second(s)" << std::endl
                              << std::endl;
                    utility_obj.delay_duration(delay);
                    test_duration -= delay;
                }
            } // end of sensor_suid_pair for loop
        } // end of num_qmi_client for loop

        // Test duration
        utility_obj.stream_duration(test_duration);

        // Stop streaming
        salt_rc rc = SALT_RC_FAILED;
        for(int i = 0; i < num_qmi_clients; ++i) {
            for(std::vector<std::pair<std::string, sens_uid*>>::
                const_iterator iter = streamed_sens_suid_pair[i].begin();
                    iter != streamed_sens_suid_pair[i].end(); ++iter) {
                sens_uid* target_sensor_suid = iter->second;

                utility_obj.print_disable_sensor_message
                                                        (
                                                          psalt[i], iter->first,
                                                          target_sensor_suid
                                                        );
                // Check if request type is resampler
                req_type_msg_id = get_msg_id(test_params, iter->first);
                if(req_type_msg_id == static_cast<int>(StabilityEngine::
                                                        req_message_ids::
                                                        MSGID_RESAMPLER_CONFIG)) {
                   rc = utility_obj.disable_sensor_for_resampler(psalt[i],
                                                                 &resampler_suid
                                                                );
                }
                else {
                    rc = utility_obj.disable_sensor(psalt[i], target_sensor_suid);
                }
                if(rc == SALT_RC_SUCCESS) {
                    std::cout << "PASS" << std::endl << std::endl;
                }
                else {
                       std::cout << "rc = " << rc << std::endl;
                       std::cout << " FAIL" << std::endl << std::endl;
                }
            }
        }
    } // end of run_concurrency
//----------------------------------------------------------------------------
    void run_b2b(const TestCase::sensor_name_sensor_configs&
                       sensor_config_mapping)
    {
        std::map<std::string, float>test_params;
        // Iterator to test_params
        std::map<std::string, float>::iterator it;

        // Get total test duration
        float test_duration = Parser::get_test_duration();
        // Local variables default initialization
        float stream_duration = 0.0f;
        float sample_rate  = 0.0f;
        float min_sample_rate = 0.0f;
        float max_sample_rate = 0.0f;
        float random_sample_rate = 0.0f;
        float resampler_rate = 0.0f;
        float min_resampler_rate = 0.0f;
        float max_resampler_rate = 0.0f;
        float random_resampler_rate = 0.0f;
        float odr_rate = 0.0f;
        float batch_period = 0.0f;
        float min_batch_period = 0.0f;
        float max_batch_period = 0.0f;
        float random_batch_period = 0.0f;
        float report_period = 0.0f;
        float min_report_period = 0.0f;
        float max_report_period = 0.0f;
        float random_report_period = 0.0f;
        float flush_period = 0.0f;
        float min_flush_period = 0.0f;
        float max_flush_period = 0.0f;
        float random_flush_period = 0.0f;
        float flush_request_interval = 0.0f;
        float db_length = 0.0f;
        float delay = 0.0f;
        float min_delay = 0.0f;
        float max_delay = 0.0f;
        float random_delay = 0.0f;

        std::string sensor_name = "";
        std::string sensor_type = "";
        std::string req_type = "";

        int resampler_rate_type = 0;
        int req_type_msg_id = 0;

        bool flush_only = false;
        bool filter = false;
        bool max_batch = false;

        salt_rc rc = SALT_RC_FAILED;
        stability_engine_obj->process_test_params(sensor_config_mapping,
                                                        test_params);
        sens_uid* target_sensor_suid = nullptr;
        sens_uid resampler_suid;
        sensor* psensor = nullptr;

        // Get usta_log_status
        bool usta_log_status = Parser::get_usta_log_status();
        set_usta_logging(usta_log_status);

        // Get instance
        see_salt* psalt = see_salt::get_instance();
        // Required by David's API
        char* argv[] = {NULL};
        if(psalt == nullptr) {
            print_file_line_number(__FILE__, __LINE__);
            throw "psalt not instantiated.\n";
        }
        psalt->begin(0, argv);

        it = test_params.begin();
        // Check if sensor config is present
        if(it != test_params.end()) {
            sensor_type = (*it).first;

            if(!sensor_type.empty()) {
                // Get random seed. Used only in case of randomization
                unsigned long long seed = Parser::get_seed();

                // Get sensor name
                if(test_params.find(sensor_type + "$sensor_name") !=
                                           test_params.end()) {
                    sensor_name = get_sensor_name(test_params, sensor_type);
                }

                // Get stream duration
                if(test_params.find(sensor_type + "$stream_duration") !=
                                                test_params.end()) {
                    stream_duration = get_stream_duration(test_params,
                                                            sensor_type);
                }

                // Get message id
                if(test_params.find(sensor_type + "$msg_id") !=
                                                test_params.end()) {
                    req_type_msg_id = get_msg_id(test_params, sensor_type);
                }

                // Get batch period
                if(test_params.find(sensor_type + "$batch_period") !=
                                               test_params.end()) {
                    batch_period = get_batch_period(test_params, sensor_type);
                }

                // Get report period
                if(test_params.find(sensor_type + "$report_period") !=
                                               test_params.end()) {
                    report_period = get_report_period(test_params, sensor_type);
                }

                // Get flush period
                if(test_params.find(sensor_type + "$flush_period") !=
                                              test_params.end()) {
                    flush_period = get_flush_period(test_params, sensor_type);
                }

                // Get flush request interval
                if(test_params.find(sensor_type + "$flush_req_interval") !=
                                            test_params.end()) {
                    flush_request_interval = get_flush_request_interval
                                                    (test_params, sensor_type);
                }

                // Get max batch
                if(test_params.find(sensor_type + "$max_batch") !=
                                            test_params.end()) {
                    max_batch = get_max_batch(test_params, sensor_type);
                }

                // Get db length
                if(test_params.find(sensor_type + "$db_len") !=
                                               test_params.end()) {
                    db_length = get_db_length(test_params, sensor_type);
                }

                // Get sample rate
                if(test_params.find(sensor_type + "$sample_rate") !=
                                           test_params.end()) {
                    sample_rate = get_sample_rate(test_params, sensor_type);
                }

                // Get odr rate
                if(test_params.find(sensor_type + "$odr_rate") !=
                                                        test_params.end()) {
                    odr_rate = get_odr_rate(test_params, sensor_type);
                }

                // Get resampler rate
                if(test_params.find(sensor_type + "$resampler_rate") !=
                                            test_params.end()) {
                    resampler_rate = get_resampler_rate(test_params, sensor_type);
                }

                // Get resampler rate type - fixed or minimum
                if(test_params.find(sensor_type + "$resampler_rate_type") !=
                                                       test_params.end()) {
                    resampler_rate_type = get_resampler_rate_type(test_params,
                                                                   sensor_type);
                }

                // Get resampler filter - true or false
                if(test_params.find(sensor_type + "$resampler_filter") !=
                                                test_params.end()) {
                    filter = get_resampler_filter(test_params, sensor_type);
                }

                // Get flush only
                if(test_params.find(sensor_type + "$flush_only") !=
                                                test_params.end()) {
                    flush_only = get_flush_only(test_params, sensor_type);
                }

                // Get min batch period
                if(test_params.find(sensor_type + "$min_batch_period") !=
                                             test_params.end()) {
                    min_batch_period = get_min_batch_period(test_params,
                                                                sensor_type);
                }

                // Get max batch period
                if(test_params.find(sensor_type + "$max_batch_period") !=
                                             test_params.end()) {
                    max_batch_period = get_max_batch_period(test_params,
                                                                sensor_type);
                }

                // Get min report period
                if(test_params.find(sensor_type + "$min_report_period") !=
                                             test_params.end()) {
                    min_report_period = get_min_report_period(test_params,
                                                          sensor_type);
                }

                // Get max report period
                max_report_period = get_max_report_period(test_params,
                                                          sensor_type);

                // Get min flush period
                if(test_params.find(sensor_type + "$min_flush_period") !=
                                             test_params.end()) {
                    min_flush_period = get_min_flush_period(test_params,
                                                                sensor_type);
                }

                // Get max flush period
                if(test_params.find(sensor_type + "$max_flush_period") !=
                                             test_params.end()) {
                    max_flush_period = get_max_flush_period(test_params,
                                                                sensor_type);
                }

                // Get delay
                if(test_params.find(sensor_type + "$delay") !=
                                             test_params.end()) {
                    delay = get_delay(test_params, sensor_type);
                }

                // Get min delay
                if(test_params.find(sensor_type + "$min_delay") !=
                                             test_params.end()) {
                    min_delay = get_min_delay(test_params, sensor_type);
                }

                // Get max delay
                if(test_params.find(sensor_type + "$max_delay") !=
                                             test_params.end()) {
                    max_delay = get_max_delay(test_params, sensor_type);
                }

                // Get random delay
                // min delay could be 0, hence use max_delay in if block
                if(max_delay) {
                   random_delay = get_random_min_max(min_delay,
                                                     max_delay, seed,
                                                     TestCase::rand_counter++);
                   delay = random_delay;
                }

                // Check request type
                if(req_type_msg_id == static_cast<int>(StabilityEngine::
                                                req_message_ids::
                                                MSGID_STD_SENSOR_CONFIG)) {
                   req_type = "std";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                StabilityEngine::
                                                req_message_ids::
                                                MSGID_ON_CHANGE_CONFIG)) {
                            req_type = "on_change";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                StabilityEngine::
                                                req_message_ids::
                                                MSGID_SET_DISTANCE_BOUND)) {
                            req_type = "db";
                }
                else if(req_type_msg_id == static_cast<int>(
                                                  StabilityEngine::
                                                  req_message_ids::
                                                  MSGID_RESAMPLER_CONFIG)) {
                              req_type = "res";
                              resampler_suid = utility_obj.get_resampler_suid(
                                                                        psalt);
                }
                else {
                          print_file_line_number(__FILE__, __LINE__);
                          throw "Invalid message request type\n";
                }
                // Get target sensor suid
                target_sensor_suid = utility_obj.get_target_sensor_suid(
                                                    psalt,
                                                    target_sensor_suid,
                                                    sensor_type, req_type,
                                                    sensor_name);
                if(target_sensor_suid == nullptr) {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "target_sensor_suid not found\n";
                }
                psensor = psalt->get_sensor(target_sensor_suid);
                if(psensor == nullptr) {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "psensor is nullptr\n";
                }
                if(sample_rate == MAX_RATE && psensor->has_max_rate()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                            sensor_type);
                }
                if(sample_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(resampler_rate == MIN_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_min_odr_rate(psensor,
                                                                   sensor_type);
                }
                if(resampler_rate == MAX_RATE && psensor->has_max_rate()) {
                    resampler_rate = utility_obj.get_max_odr_rate(psensor,
                                                               sensor_type);
                }
                if(odr_rate == MIN_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_min_odr_rate(psensor,
                                                                sensor_type);
                }
                if(odr_rate == MAX_RATE && psensor->has_rates()) {
                    sample_rate = utility_obj.get_max_odr_rate(psensor,
                                                                sensor_type);
                }
                // Get min sample rate
                if(test_params.find(sensor_type + "$min_sample_rate") !=
                                            test_params.end()) {
                    min_sample_rate = get_min_sample_rate(test_params,
                                                            sensor_type);
                }

                // Get max sample rate
                if(test_params.find(sensor_type + "$max_sample_rate") !=
                                            test_params.end()) {
                    max_sample_rate = get_max_sample_rate(test_params,
                                                                sensor_type);
                }

                // If random odr rate is requested
                if((sample_rate == RAND_RATE || odr_rate == RAND_RATE)
                                                    && psensor->has_rates()) {
                    sample_rate = utility_obj.get_random_odr_rate(
                                                        psensor,
                                                        sensor_type, seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                if(resampler_rate == RAND_RATE && psensor->has_rates()) {
                    resampler_rate = utility_obj.get_random_odr_rate
                                                    (
                                                       psensor,
                                                       sensor_type, seed,
                                                       TestCase::rand_counter++
                                                    );
                }

                // Get random batch period
                if(min_batch_period && max_batch_period) {
                    random_batch_period = get_random_min_max(
                                                        min_batch_period,
                                                        max_batch_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random report period
                if(min_report_period && max_report_period) {
                    random_report_period = get_random_min_max(
                                                        min_report_period,
                                                        max_report_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random flush period
                if(min_flush_period && max_flush_period){
                    random_flush_period = get_random_min_max(
                                                        min_flush_period,
                                                        max_flush_period,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random sample rate
                if(min_sample_rate && max_sample_rate) {
                    if(min_sample_rate == MIN_RATE && max_sample_rate ==
                                                                    MAX_RATE) {
                        min_sample_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_sample_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_sample_rate = get_random_min_max(
                                                        min_sample_rate,
                                                        max_sample_rate,
                                                        seed,
                                                        TestCase::rand_counter++
                                                    );
                }
                // Get random resampler rate
                if(min_resampler_rate && max_resampler_rate) {
                    if(min_resampler_rate == MIN_RATE && max_resampler_rate ==
                                                                    MAX_RATE) {
                        min_resampler_rate = utility_obj.get_min_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                        max_resampler_rate = utility_obj.get_max_odr_rate
                                                            (
                                                                psensor,
                                                                sensor_type
                                                            );
                    }
                    random_resampler_rate = get_random_min_max
                                                    (
                                                       min_resampler_rate,
                                                       max_resampler_rate,
                                                       seed,
                                                       TestCase::rand_counter++
                                                    );
                }
            }
            else {
                    print_file_line_number(__FILE__, __LINE__);
                    throw "No sensor configuration found\n";
            }
        }
        if(report_period) {
            batch_period = report_period;
        }
        if(random_batch_period) {
            print_random_batch_period_message(sensor_type, random_batch_period);
            batch_period = random_batch_period;
        }
        if(random_report_period) {
            print_random_report_period_message(sensor_type, random_report_period);
            batch_period = random_report_period;
        }
        if(random_flush_period) {
            print_random_flush_period_message(sensor_type, random_flush_period);
            flush_period = random_flush_period;
        }
        if(random_sample_rate) {
            print_random_sample_rate_message(sensor_type, random_sample_rate);
            sample_rate = random_sample_rate;
        }
        if(random_resampler_rate) {
            print_random_resampler_rate_message(sensor_type,
                                                      random_resampler_rate);
            resampler_rate = random_resampler_rate;
        }
        // Start test time
        auto start_time = std::chrono::system_clock::now();
        auto end_time = start_time + std::chrono::
                                            duration_cast<std::chrono::
                                            system_clock::duration>
                                            (std::chrono::duration<float>
                                            (test_duration));
        while(std::chrono::system_clock::now() < end_time) {
            if(target_sensor_suid == nullptr) {
                print_file_line_number(__FILE__, __LINE__);
                throw "target_sensor_suid not found\n";
            }
            if(req_type == "std") {
                rc = stream_sensor(psalt, target_sensor_suid, sensor_type,
                                          sample_rate, batch_period,
                                          flush_period, flush_only, max_batch);
            }
            else if(req_type == "res") {
                rc = resample_sensor(psalt, &resampler_suid,
                                        target_sensor_suid, resampler_rate_type,
                                        sensor_type, resampler_rate, filter,
                                        batch_period, flush_period, flush_only,
                                        max_batch);
            }
            else if(req_type == "on_change") {
                rc = on_change_sensor(psalt, target_sensor_suid,
                                                    psensor, sensor_type,
                                                    batch_period, flush_period,
                                                    flush_only, max_batch);
            }
            else if(req_type == "db") {
                rc = db_sensor(psalt, target_sensor_suid, sensor_type,
                                          db_length, batch_period, flush_period,
                                          flush_only, max_batch);
            }
            else {
                   print_file_line_number(__FILE__, __LINE__);
                   throw "Invalid request type\n";
            }
            // Stream duration
            if(rc == SALT_RC_SUCCESS) {
                utility_obj.stream_duration(stream_duration);
                // Stop sensor streaming
                utility_obj.print_disable_sensor_message(psalt, sensor_type,
                                                           target_sensor_suid);
                if(req_type == "res") {
                    rc = utility_obj.disable_sensor_for_resampler
                                                            (
                                                                psalt,
                                                                &resampler_suid
                                                            );
                }
                else {
                       rc = utility_obj.disable_sensor(psalt,
                                                        target_sensor_suid);
                }
                if(std::chrono::system_clock::now() < end_time) {
                    if(delay) {
                       std::cout << "Delay before sending in next request: ";
                       std::cout << delay << " second(s)" << std::endl
                                 << std::endl;
                    }
                    if(random_delay) {
                       std::cout << "Random delay before sending in next "
                                 << "request: " << random_delay << " second(s)"
                                 << std::endl;
                    }
                    utility_obj.delay_duration(delay);
                }
            }
            else {
                 std::cout << "rc = " << rc << std::endl;
                 std::cout << "FAIL" << std::endl;
            }
        } // end of while loop
        if(rc == SALT_RC_SUCCESS) {
            std::cout << "PASS" << std::endl;
        }
        else {
               print_file_line_number(__FILE__, __LINE__);
               std::cout << "rc = " << rc << std::endl;
               std::cout << "FAIL" << std::endl;
        }
    } // end of run_b2b
//----------------------------------------------------------------------------

    // Get test params after parsing
    void TestCase::get_test_params(const std::string& test_case_id,
                                   const sensor_name_sensor_configs&
                                   sensor_config_mapping)
    {
        const std::string FIRST_UNDERSCORE = "_";
        std::size_t pos;

        TestCase::test_case_id = test_case_id;
        pos = test_case_id.find_first_of(FIRST_UNDERSCORE);
        std::string test_case_name = test_case_id.substr(pos + 1);

        // Get random seed
        //size_t seed = Parser::get_seed();

        // Test param as key-value pair.
        std::map<std::string, float> test_params;

        // Iterator to test_params
        std::map<std::string, float>::iterator it;

        if(test_case_name == "Stability") {
            run_stability(sensor_config_mapping);
        }
        if(test_case_name == "Concurrency") {
            run_concurrency(sensor_config_mapping);
        }
        if(test_case_name == "Back2Back") {
            run_b2b(sensor_config_mapping);
        }
//-----------------------------------------------------------------------------
} // end TestCase::get_test_params
//-----------------------------------------------------------------------------

} // end namespace sensors_stability
///////////////////////////////////////////////////////////////////////////////
