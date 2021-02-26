/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: Utility.cpp
** ================================================================ */

#include <iostream>
#include <random>
#include <chrono>
#include <thread>     // std::this_thread
#include <iomanip>    // std::setprecision
#include <sstream>    // std::ostringstream
#include <string>     // std::string

#include "Template_Impl.hpp"
#include "Utility.h"
#include "Parser.h"

namespace sensors_stability {

    const int Utility::MICROSECONDS = 1000000;
    const int Utility::MILLISECONDS = 1000;

// ***************************************************************************
//                       Non-member functions                                *
// ***************************************************************************
    // Print log timestamp
    void print_log_time_stamp()
    {
        //using namespace std::chrono;
        std::chrono::system_clock::time_point start_time;
        const auto end_time = std::chrono::system_clock::now();
        auto delta_time = end_time - start_time;
        const auto hrs = std::chrono::duration_cast<std::chrono::hours>
                                                                (delta_time);
        delta_time -= hrs;
        const auto mts = std::chrono::duration_cast<std::chrono::minutes>
                                                                (delta_time);
        delta_time -= mts;
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>
                                                                (delta_time);
        delta_time -= secs;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>
                                                                (delta_time);
        std::cout << '\n';
        std::ostringstream stream;
        stream << hrs.count() % 24 << ':' << std::setfill('0') << std::setw(2)
               << mts.count() << ':' << std::setfill('0') << std::setw(2)
               << secs.count() << '.' << std::setfill('0') << std::setw(3)
               << ms.count();
        std::string result = stream.str();
        std::cout << result << " ";
    } // end print_log_time_stamp
// ***************************************************************************
//                       Member/Friend functions                             *
// ***************************************************************************
    // Generate a new random seed
    unsigned long long generate_random_seed()
    {
        std::mt19937 engine{(unsigned int) time(NULL)};
        std::cout << "New random seed: ";

        unsigned long long seed = engine();
        std::cout << seed << std::endl << std::endl;
        return(seed);
    }
//----------------------------------------------------------------------------
    // Return random seed as unsigned long long by value
    unsigned long long Parser::get_new_random_seed()
    {
        return(generate_random_seed());
    } // end Parser::get_new_random_seed
//----------------------------------------------------------------------------
    // Set decimal precision in the constructor
    Utility::Utility()
    {
        // Fixed floating-point notation
        std::cout << std::fixed;

        //Decimal precision to be set
        std::cout << std::setprecision(2);
    } // end Utility::Utility
//----------------------------------------------------------------------------
    // Return a value within min:max range
    std::pair<float, float>Utility::get_min_max_range(std::string range)
    {
        // Position of string pattern
        size_t pos;
        std::string colon_delimiter = ":";
        float min = 0.0f;
        float max = 0.0f;

        // Parse string on ":" delimiter
        while((pos = range.find_first_of(colon_delimiter)) != std::string::npos) {
               min = stof(range.substr(0, pos));
               max = stof(range.substr(pos + 1));
               range.erase(0, pos + colon_delimiter.length());
        }
        return(std::make_pair(min, max));
    } // end Utility::get_min_max_range
//----------------------------------------------------------------------------
    // Return a value within supported min:max of the sensor
    // TODO: Not yet implemented
    std::pair<float, float>Utility::get_supported_min_max_range(std::string range)
    {
        // Position of string pattern
        size_t pos;
        std::string colon_delimiter = ":";
        float min = 0.0f;
        float max = 0.0f;

        // Parse string on ":" delimiter
        while((pos = range.find_first_of(colon_delimiter)) !=
                                                            std::string::npos) {
              min = 10.0f;  // TODO: get the supported min value from attributes
              max = 400.0f; // TODO: get the supported max value from attributes
        }
        return(std::make_pair(min, max));

    } // end Utility::get_min_max_range
//----------------------------------------------------------------------------
    // Stream duration or delay
    void Utility::stream_duration(const float& str_dur)
    {
        std::cout << "Streaming enabled sensor(s) for duration: " << str_dur
                  << " secs" << std::endl << std::endl;
        delay_duration(str_dur);
    } // end Utility::stream_duration
//----------------------------------------------------------------------------
    // Delay duration
    void Utility::delay_duration(const float& delay_dur)
    {
        std::this_thread::sleep_for(std::chrono::duration<float>(delay_dur));
    } // end Utility::stream_duration
//----------------------------------------------------------------------------
    // Return units of time in microseconds
     long int Utility::get_microseconds()
     {
         return(Utility::MICROSECONDS);
     } // end Utility::get_microseconds
//----------------------------------------------------------------------------
    // Return units of time in milliseconds
     short Utility::get_milliseconds()
     {
         return(Utility::MILLISECONDS);
     } // end Utility::get_milliseconds
//----------------------------------------------------------------------------
    // Adjust the sample rate of sensors if total sample rate > max cap of target
    float Utility::adjust_sample_rate(float sample_rate)
    {
        float tot_sample_rate = Parser::get_cumulative_sample_rate();
        if(tot_sample_rate < 0) {
            tot_sample_rate = sample_rate;
        }
        int max_cap = Parser::get_max_cap();
        return((max_cap / tot_sample_rate) * sample_rate);
    } // end adjust_sample_rate
//----------------------------------------------------------------------------
    // Print standard message request to stdout
    void Utility::print_std_message_config(const see_salt* psalt,
                                             const std::string& sensor_type,
                                             const sens_uid* suid,
                                             const float& sample_rate)
    {
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Using psalt instance " << psalt;
        std::cout << " to enable " << sensor_type << " having SUID ";
        std::cout << "[low " << std::hex << suid->low;
        std::cout << ", high " << std::hex << suid->high << "], ";
        std::cout << "streaming @ " << sample_rate << " Hz";
    } // end of print_std_message_config
//----------------------------------------------------------------------------
    // Print resample message request to stdout
    void Utility::print_resampler_message_config(see_salt* psalt,
                                                    const std::string&
                                                    sensor_type, sens_uid* suid,
                                                    const float& sample_rate,
                                                    std::string rate_type,
                                                    bool filter
                                                )
    {
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Using psalt instance " << psalt;
        std::cout << " to enable " << sensor_type << " having SUID ";
        std::cout << "[low " << std::hex << suid->low;
        std::cout << ", high " << std::hex << suid->high << "], ";
        std::cout << "streaming at resample rate of " << sample_rate
                  << " Hz, " << std::endl;
        std::cout << "with rate type " << rate_type;
        std::cout << ", filter set to " << std::boolalpha << filter << " ";
    } // end of print_resampler_message_config
//----------------------------------------------------------------------------
    // Print on-change message request to stdout
    void Utility::print_on_change_message_config(see_salt* psalt,
                                                    const std::string&
                                                    sensor_type, sens_uid* suid)
    {
         std::cout << '\n';
         print_log_time_stamp();
         std::cout << "Using psalt instance " << psalt;
         std::cout << " to enable " << sensor_type;
         std::cout << " having SUID ";
         std::cout << "[low " << std::hex;
         std::cout << suid->low;
         std::cout << ", high " << std::hex;
         std::cout << suid->high << "] for on-change request " << std::endl;
     } // end Utility::print_on_change_message
//----------------------------------------------------------------------------
    // Print db message request to stdout
    void Utility::print_db_message_config(see_salt* psalt,
                                            const std::string& sensor_type,
                                            sens_uid* suid,
                                            const float& db_length)
    {
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Using psalt instance =  " << psalt;
        std::cout << " to enable " << sensor_type;
        std::cout << " having SUID ";
        std::cout << "[low " << std::hex;
        std::cout << suid->low;
        std::cout << ", high " << std::hex;
        std::cout << suid->high << "]";
        std::cout << "\nwith db length, set to " << db_length;
        std::cout << " meters";
    } // end Utility::print_db_message
//----------------------------------------------------------------------------
    // Print disable sensor message request to stdout
    void Utility::print_disable_sensor_message(see_salt* psalt,
                                                const std::string& sensor_type,
                                                sens_uid* suid)
    {
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Using psalt instance " << psalt;
        std::cout << " to disable " << sensor_type;
        std::cout << " streaming having ";
        std::cout << "suid [low " << std::hex;
        std::cout << suid->low;
        std::cout << ", high " << std::hex;
        std::cout << suid->high << "]" << std::endl;
    } // end Utility::print_disable_sensor_message
//----------------------------------------------------------------------------
    // Get resampler suid
    sens_uid Utility::get_resampler_suid(see_salt* psalt)
    {
        sens_uid resampler_suid;
        std::string resampler = "resampler";
        std::vector<sens_uid *> sens_uids;
        psalt->get_sensors(resampler, sens_uids);

        if(sens_uids.size()) {
            resampler_suid.low = sens_uids[0]->low;
            resampler_suid.high = sens_uids[0]->high;
        } else {
                std::cout << "Resampler suid not found" << std::endl;
                std::cout << "FAIL" << std::endl;
          }
        return(resampler_suid);
    } // end of get_resampler_suid
//----------------------------------------------------------------------------
    // Get target sensor suid
    sens_uid* Utility::get_target_sensor_suid(see_salt* psalt, sens_uid* suid,
                                              const std::string& sensor_type,
                                              const std::string& req_type,
                                              const std::string& sensor_name)
    {
        // Required to print to stdout
        std::string save_sensor_type = sensor_type;

        // sensor_type_with_digit is passed as const reference
        // hence cannot be modified
        std::string tmp_sensor_type = sensor_type;

        // If muliple sensors with same name are present
        // digit is used to differientiate it (e.g. accel
        // and accel2). Remove digit from the sensor
        tmp_sensor_type.erase(std::remove_if(tmp_sensor_type.begin(),
                                             tmp_sensor_type.end(),
                                             &isdigit), tmp_sensor_type.end());

        std::vector<sens_uid *> sensor_suids;
        // Get a vector of suids for the target sensor
        psalt->get_sensors(tmp_sensor_type, sensor_suids);

        if(0 == sensor_suids.size()) {
            std::cout << "Sensor " << tmp_sensor_type << " not found or ";
            std::cout << "SUID for " << tmp_sensor_type << " not found. ";
            std::cout << "No stream request will be sent for " << tmp_sensor_type
                      << "\nFAIL" << std::endl << std::endl;
        }
        else {
            bool std_req = false;
            bool on_change_req = false;
            bool db_req = false;

            if(req_type == "std" || req_type == "res") {
                std_req = true;
            }
            if (req_type == "on_change") {
                on_change_req = true;
            }
            if (req_type == "db") {
                db_req = true;
            }
            for(size_t i = 0; i < sensor_suids.size(); ++i) {
                sens_uid* target_sensor_suid = sensor_suids[i];
                sensor* psensor = psalt->get_sensor(target_sensor_suid);

                if(psensor != nullptr) {
                   if(!sensor_name.empty()) {
                      std::string name = psensor->get_name();
                      std::string type = psensor->get_type();
                      if(tmp_sensor_type == type && sensor_name == name) {
                         if (psensor->has_stream_type()) {
                             if(on_change_req &&
                                (psensor->is_stream_type_on_change() ||
                                    psensor->is_stream_type_single_output())) {
                                 suid = target_sensor_suid;
                                 break;
                             }
                             else if(std_req &&
                                      psensor->is_stream_type_streaming()) {
                                      suid = target_sensor_suid;
                                      break;
                             }
                             else if(db_req && "distance_bound" ==
                                                         psensor->get_type()) {
                                      suid = target_sensor_suid;
                                      break;
                             }
                             else {
                                    std::cout << "Sensor ";
                                    std::cout << save_sensor_type;
                                    std::cout << " not found or ";
                                    std::cout << "SUID for ";
                                    std::cout << save_sensor_type;
                                    std::cout << " not found. ";
                                    std::cout << "No stream request will be ";
                                    std::cout << "sent for " << save_sensor_type
                                              << std::endl << std::endl;
                                    std::cout << "FAIL" << std::endl;
                                    return(nullptr);
                             }
                            }
                        }
                    }
                    if (psensor->has_stream_type()) {
                       if(on_change_req && (psensor->is_stream_type_on_change()
                                || psensor->is_stream_type_single_output())) {
                          suid = target_sensor_suid;
                          break;
                        }
                        else if(std_req && psensor->is_stream_type_streaming()) {
                                  suid = target_sensor_suid;
                                  break;
                        }
                        else if(db_req && "distance_bound" ==
                                                        psensor->get_type()) {
                                 suid = target_sensor_suid;
                                 break;
                        }
                    }
                    else {
                           std::cout << "Sensor " << save_sensor_type;
                           std::cout << " not found or ";
                           std::cout << "SUID for " << save_sensor_type;
                           std::cout << " not found. ";
                           std::cout << "No stream request will be ";
                           std::cout << "sent for " << save_sensor_type
                                     << std::endl << std::endl;
                           std::cout << "FAIL" << std::endl;
                           return(nullptr);
                    }
                }
                else {
                        std::cout << "Not a valid sensor.... " << std::endl;
                        std::cout << "FAIL" << std::endl;
                        return(nullptr);
                }
            }
        }
        return(suid);
    } // end of get_target_sensor_suid
//----------------------------------------------------------------------------
    // Get adjusted rate, if max capacity has exceeded
    float Utility::max_cap_exceed(float& rate, int& max_cap, std::string&
                                  sensor_type)
    {
        float cumulative_rate = Parser::get_cumulative_sample_rate();
        float adjusted_rate = 0.0f;

        // If sensor has random odr rate (-3) or
        // has max rate (-1) or min rate (-2)
        if(cumulative_rate < 0) {
            if(rate > max_cap) {
                adjusted_rate = adjust_sample_rate(rate);
                std::cout << "Adjusted rate of " << sensor_type << " is ";
                std::cout << adjusted_rate << " Hz" << std::endl << std::endl;
            }
            else {
                adjusted_rate = rate;
            }
        }
        else {
            float total_rate = cumulative_rate + rate;
            if(total_rate > max_cap) {
                adjusted_rate = adjust_sample_rate(total_rate);
                std::cout << "Adjusted rate of " << sensor_type << " is ";
                std::cout << adjusted_rate << " Hz" << std::endl << std::endl;
            }
            else {
                adjusted_rate = total_rate;
            }
        }
        return(adjusted_rate);
    } // end of max_cap_exceed
//----------------------------------------------------------------------------
    // Get random odr rate
    float Utility::get_random_odr_rate(sensor* psensor, const std::string&
                                       sensor_type, const size_t& seed,
                                       const int& rand_cnt)
    {
        std::vector<float> rates;
        psensor->get_rates(rates);
        //int vec_size = rates.size();

        float random_odr_rate = 0.0f;
        if(!rates.empty()) {
            //int max_cap = Parser::get_max_cap();

            random_odr_rate = get_random_sensor_or_odr(rates, seed, rand_cnt);
            std::cout << "Random ODR rate of " << sensor_type << ": ";
            std::cout << random_odr_rate << " Hz";
            std::cout << std::endl;

            // Check if capacity of the target has exceeded
            //rand_odr_rate = max_cap_exceed(random_odr_rate, max_cap,
            //                                                    sensor_type);
        }
        else {
             std::cout << "Missing odr attribute" << std::endl;
             std::cout << "FAIL" << std::endl;
        }
        return(random_odr_rate);
    } // end of get_random_odr_rate
//----------------------------------------------------------------------------
    // Get min odr rate
    float Utility::get_min_odr_rate(sensor* psensor, std::string& sensor_type)
    {
         // call default constructor to initialize empty vector
        std::vector<float> rates;
        psensor->get_rates(rates);

        float min_odr_rate = 0.0f;

        if(!rates.empty()) {
           int max_cap = Parser::get_max_cap();
           min_odr_rate = rates.front();

           std::cout << "Min sample/odr rate of " << sensor_type << ": ";
           std::cout << min_odr_rate << " Hz";
           std::cout << std::endl;

           // Check if capaity of the target has exceeded
           //min_odr_rate = max_cap_exceed(min_odr_rate, max_cap,
           //                                    sensor_type);

        } else {
            std::cout << "Missing min odr attribute" << std::endl;
            std::cout << "FAIL" << std::endl;
          }
        return(min_odr_rate);
    } // end of get_min_odr_rate
//----------------------------------------------------------------------------
    // Get max odr rate
    float Utility::get_max_odr_rate(sensor* psensor, std::string& sensor_type)
    {
        std::vector<float> rates;
        psensor->get_rates(rates);

        float max_odr_rate = 0.0f;

        if(!rates.empty()) {
           int max_cap = Parser::get_max_cap();
           max_odr_rate = rates.back();

           std::cout << "Max sample/odr rate of " << sensor_type << ": ";
           std::cout << max_odr_rate << " Hz";
           std::cout << std::endl;

           // Check if capaity of the target has exceeded
           //max_odr_rate = max_cap_exceed(max_odr_rate, max_cap,
           //                                    sensor_type);

        } else {
             std::cout << "Missing max odr attribute" << std::endl;
             std::cout << "FAIL" << std::endl;
          }
        return(max_odr_rate);
    } // end of get_max_odr_rate
//----------------------------------------------------------------------------
   // Standard message request
   salt_rc Utility::std_msg_req(see_salt* psalt, sens_uid* suid,
                                   const see_msg_id_e& msg_id_config,
                                   see_std_request& std_request,
                                   const float& rate)
    {
      salt_rc rc = SALT_RC_FAILED;
      see_std_sensor_config sample_rate(rate);
      std_request.set_payload(&sample_rate);

      see_client_request_message request(suid, msg_id_config, &std_request);
      rc = psalt->send_request(request);
      std::cout << '\n';
      print_log_time_stamp();
      std::cout << "Request return code: " << rc << std::endl;
      return(rc);
    } // end Message::std_msg_req
//----------------------------------------------------------------------------
    // Resampler message request
    salt_rc Utility::resampler_msg_req(see_salt* psalt, sens_uid* resampler_suid,
                                          sens_uid* suid,
                                          const see_msg_id_e& msg_id_config,
                                          see_std_request& std_request,
                                          const float& resampler_rate,
                                          see_resampler_rate& rate_type,
                                          bool filter)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_resampler_config payload(suid, resampler_rate, rate_type, filter);
        std_request.set_payload(&payload);

        see_client_request_message request(resampler_suid, msg_id_config,
                                                    &std_request);
        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return(rc);
    } // end res_msg_req
//----------------------------------------------------------------------------
    // On-change message request
    salt_rc Utility::on_change_msg_req(see_salt* psalt, sens_uid* suid,
                                        const see_msg_id_e& msg_id_config,
                                        see_std_request& std_request)
    {
        salt_rc rc = SALT_RC_FAILED;
        see_client_request_message request(suid, msg_id_config, &std_request);
        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return rc;
    } // end on_change_msg_req
//----------------------------------------------------------------------------
    // Distance bound request
    salt_rc Utility::distance_bound_req(see_salt* psalt, sens_uid* suid,
                                         const see_msg_id_e &msg_id_config,
                                         see_std_request& std_request,
                                         const float &db_length)
    {
        salt_rc rc = SALT_RC_FAILED;
        // Payload
        see_set_distance_bound distance_bound(db_length);
        std_request.set_payload(&distance_bound);

        see_client_request_message request(suid, msg_id_config, &std_request);
        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return(rc);
    } // end distance_bound_req
//----------------------------------------------------------------------------
    // Basic gestures request
    salt_rc Utility::basic_gestures_req(see_salt* psalt, sens_uid* suid,
                                         const see_msg_id_e& msg_id_config,
                                         see_std_request& std_request)
    {
        salt_rc rc = SALT_RC_FAILED;
        // Payload
        see_basic_gestures_config config;
        std_request.set_payload(&config);
        see_client_request_message request(suid, msg_id_config, &std_request);
        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return(rc);
    } // end basic_gestures_req
//----------------------------------------------------------------------------
    // Psmd request
    salt_rc Utility::psmd_req(see_salt* psalt, sens_uid* suid,
                                const see_msg_id_e& msg_id_config,
                                see_std_request& std_request)
    {
        salt_rc rc = SALT_RC_FAILED;
        //Payload
        see_psmd_config config(SEE_PSMD_TYPE_STATIONARY);
        std_request.set_payload(&config);
        see_client_request_message request(suid, msg_id_config, &std_request);

        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return(rc);
    } // end psmd_req
//----------------------------------------------------------------------------
    // Multishake request
    salt_rc Utility::multishake_req(see_salt* psalt, sens_uid* suid,
                                        const see_msg_id_e& msg_id_config,
                                        see_std_request& std_request)
    {
        salt_rc rc = SALT_RC_FAILED;
        //Payload
        see_multishake_config config;
        std_request.set_payload(&config);
        see_client_request_message request(suid, msg_id_config, &std_request);

        rc = psalt->send_request(request);
        std::cout << '\n';
        print_log_time_stamp();
        std::cout << "Request return code: " << rc << std::endl;
        return(rc);
    } // end psmd_req
//----------------------------------------------------------------------------
    // Disable sensor request
    salt_rc Utility::disable_sensor(see_salt* psalt, sens_uid* suid)
    {
        see_std_request std_request;
        see_client_request_message request(suid, MSGID_DISABLE_REQ, &std_request);

        salt_rc rc = psalt->send_request(request);
        print_log_time_stamp();
        std::cout << "Disable request return code: " << rc << std::endl;
        return(rc);
    } // end Utility::disable_sensor
//----------------------------------------------------------------------------
    // Disable resampler request
    salt_rc Utility::disable_sensor_for_resampler(see_salt* psalt,
                                                  sens_uid* resampler_suid)
    {
        see_std_request std_request;
        see_client_request_message request(resampler_suid, MSGID_DISABLE_REQ,
                                                                   &std_request);
        salt_rc rc = psalt->send_request(request);
        print_log_time_stamp();
        std::cout << "Resampler disable request return code: " << rc << std::endl;
        return(rc);
    } // end Utility::disable_sensor_for_resampler
//----------------------------------------------------------------------------
} // end sensors_stability namespace
///////////////////////////////////////////////////////////////////////////////
