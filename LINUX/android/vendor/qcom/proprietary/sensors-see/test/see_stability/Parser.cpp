/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: Parser.cpp
** =================================================================*/

#include <algorithm>      // std::remove_if
#include <fstream>        // std::ifstream
#include <sstream>        // std::istringstream
#include <iostream>       // std::cout, std::endl
#include <cstdlib>        // EXIT_SUCCESS/EXIT_FAILURE

#include "Parser.h"
#include "TestCase.h"
#include "Utility.h"

namespace sensors_stability {

    namespace Parser {

        unsigned long long random_seed = 0;
        int default_max_cap = 2000;
        int default_tot_report_count = 600;
        int default_test_iteration = 1;
        int default_qmi_clients = 1;
        float default_max_request_duration = 1.0f;
        float default_test_duration = 1.0f;
        float tot_sample_rate = 1.0f;

        std::string default_test_type = "rest";
        std::string default_usb_state = "on";

        bool default_usta_log = 1;  // usta logging enabled

        // Optional args mapping
        optional_args_map optional_args_mapping;

        // Sensor name - Sensor configuration mapping
        sensor_name_sensor_configs sensor_config_mapping;

        // Create a singleton TestCase object
        TestCase* test_case_obj_ptr = TestCase::get_test_case_instance();

        // Create an instance of Utility class
        Utility* utility_parser_obj;

        // Parse command-line arguments
        void command_line_args(int argc, char* argv[])
        {
            std::string input_file(argv[1]);
            parse_required_args(input_file);
        } // end Parser::command_line_args
//-----------------------------------------------------------------------------
        // Print file and line number where exception occured
        void print_file_line_number(const char* file, int line)
        {
            std::cout << "Exception in file: " << file << ", " << "line number: "
                      << line << std::endl;
            std::cout << "FAIL" << std::endl;
        }
//-----------------------------------------------------------------------------
        // Parse required arguments from the input file
        void parse_required_args(const std::string& required_args)
        {
            std::ifstream file(required_args);

            // Iterators for Sensor and sensor configurations
            std::multimap<std::string, sensor_configs> ::iterator
            sensor_name_sensor_configs_it;
            std::multimap<std::string, std::string> ::iterator sensor_configs_it;

            if (!file.is_open()) {
                std::cout << "Input file does not exist" << std::endl;
                std::cout << "FAIL" << std::endl;
            }
            std::size_t pos;
            std::string line;
            std::string test_case_id = "";
            const std::string HYPEN_DELIMITER = "-";
            const std::string WHITE_SPACE_DELIMITER = " ";

            // Read one line at a time from the file
            while (getline(file, line))
            {
                // Skip lines with comments ('#')
                if (line[0] == '#') {
                    continue;
                }
                // Skip CRLF, if input file was created on windows platform
                if (line[0] == '\r') {
                    continue;
                }
                // Skip empty lines
                if (line.empty()) {
                    continue;
                }
                std::cout << "Reading test case arguments from input file"
                          << std::endl;

                // Skip empty lines
                if (line.empty()) {
                    continue;
                }
                // Read test_case_id
                test_case_id = "";
                pos = line.find_first_of(WHITE_SPACE_DELIMITER);

                if (test_case_id.empty()) {
                    test_case_id = line.substr(0, pos);

                    // Remove test_case_id
                    line.erase(0, pos + WHITE_SPACE_DELIMITER.length());
                }
                std::cout << "Test Case Id: => " << test_case_id << std::endl;
                //std::cout << "****Shiva*****: " << test_case_id << std::endl;

                // Find and delete the string "-required""
                pos = line.find_first_of(WHITE_SPACE_DELIMITER);

                // Get the substring after "-required"
                std::string req_args = line.substr(pos + 1);

                // Call function to parse optional arguments
                parse_optional_args(req_args);

                // Delete optional arguments
                if (line.find_last_of(HYPEN_DELIMITER)) {
                    line.erase(line.begin() + line.find_last_of(HYPEN_DELIMITER),
                    line.end());
                }

                // Delete left square bracket
                req_args.erase(std::remove_if(std::begin(req_args),
                                                std::end(req_args),
                                                [](char c) {
                                                return c == '[';
                                                }), std::end(req_args));

                // Local variables
                std::istringstream iss(req_args);
                std::string till_bracket;
                std::string sensor_key;
                std::string tmp;
                std::string sensor_config_key;
                std::string sensor_config_value;

                const std::string RIGHT_BRACKET_DELIMITER = "]";
                const std::string COLON_DELIMITER = ":";
                const std::string COMMA_DELIMITER = ",";
                const std::string EQUAL_DELIMITER = "=";

                float min_rate = 0.0f;
                float max_rate = 0.0f;
                auto found_sensor_name_pos = 0;

                const std::string UNDERSCORE_DELIMITER = "_";
                std::string sensor_supported = "";
                std::string sensor_name = "";

                // Parse tokens of required arguments based on delimiters
                while ((pos = req_args.find_first_of(RIGHT_BRACKET_DELIMITER)) !=
                        std::string::npos) 
                {
                        till_bracket = req_args.substr(0, pos);
                        req_args.erase(0, pos + RIGHT_BRACKET_DELIMITER.length());

                        // Use physical sensor or virtual sensor name as key
                        while ((pos = till_bracket.find_first_of(COLON_DELIMITER))
                                                        != std::string::npos)
                        {
                            sensor_key = till_bracket.substr(0, pos);
                            // If muliple sensors with same name are present
                            // digit is used to differientiate it. Save the
                            // original value to a temp, since the digit has
                            // to be removed from key to check if
                            // sensor is PORed. Check to see if a sensor is
                            // PORed is currently not supported
                            std::string tmp_key = sensor_key;
                            // Remove digit from sensor if present: e.g. accel2
                            sensor_key.erase(std::remove_if(sensor_key.begin(),
                                                           sensor_key.end(),
                                                           &isdigit),
                                                           sensor_key.end());
                            found_sensor_name_pos = sensor_key.
                                                    find(UNDERSCORE_DELIMITER);

                            // Check if sensor name is present
                            if(found_sensor_name_pos > 0) {
                                sensor_name = sensor_key.substr(
                                                     found_sensor_name_pos + 1);
                                sensor_supported = sensor_key;
                                sensor_supported.erase(found_sensor_name_pos);
                             } else {
                                   sensor_supported = sensor_key;
                               }
                            sensor_key = tmp_key;
                            sensor_name_sensor_configs_it =
                            sensor_config_mapping.insert(std::make_pair(
                                                     sensor_key,
                                                     sensor_configs()));
                            till_bracket.erase(0, pos + COLON_DELIMITER.length());

                            while ((pos = till_bracket.find_first_of(
                                                           COMMA_DELIMITER)) !=
                                                           std::string::npos)
                            {
                                tmp = till_bracket.substr(0, pos);
                                till_bracket.erase(0, pos +
                                                    COMMA_DELIMITER.length());

                                // Get sensor_config and its associated value
                                while ((pos = tmp.find_first_of(EQUAL_DELIMITER))
                                                        != std::string::npos)
                                {
                                    sensor_config_key = tmp.substr(0, pos);
                                    sensor_config_value = tmp.substr(pos + 1);
                                    if (sensor_config_key == SAMPLE_RATE ||
                                           sensor_config_key == ODR_RATE ||
                                           sensor_config_key == RESAMPLED_RATE)
                                    {
                                        std::string sample_rate_range =
                                                            sensor_config_value;

                                        // Supported sample rate in min:max range
                                        // TODO: Not yet implemented
                                        /*if((sample_rate_range.
                                                 find_first_of(MIN_RATE) &&
                                                 sample_rate_range.
                                                 find_first_of(MAX_RATE)))
                                        {
                                            std::tie(min_rate, max_rate) =
                                                        utility_parser_obj->
                                              get_supported_min_max_range(
                                                        sample_rate_range);
                                            // Calculate the cumulative
                                            // sample rate of all sensors
                                            // that will be streaming in the
                                            // test. Currently not used
                                              tot_sample_rate += max_rate;

                                        }*/
                                        // Sample rate is in a range: e.g.50:200
                                        if ((sample_rate_range.find_first_of(
                                               COLON_DELIMITER)) !=
                                                        std::string::npos)
                                        {
                                            // Get max rate and add to
                                            // tot_sample_rate
                                            std::tie(min_rate, max_rate) =
                                                   utility_parser_obj->
                                                            get_min_max_range(
                                                            sample_rate_range);
                                            // Currently not used
                                            tot_sample_rate += max_rate;
                                        } else {
                                                // Currently not used
                                                tot_sample_rate +=
                                                std::stof(sensor_config_value);
                                          }
                                    }
                                    sensor_name_sensor_configs_it->second.insert(
                                            std::make_pair(sensor_config_key,
                                            sensor_config_value));
                                            tmp.erase(0, pos +
                                                      EQUAL_DELIMITER.length());
                                }
                            }
                        }
                }
                // Pass testcase id and sensor configuration to TestCase class
                test_case_obj_ptr->get_test_params(test_case_id,
                                              return_sensor_configs());
            }
            // Check if bad bit error flag is set for stream error while reading
            // input file
            if (file.bad()) {
                print_file_line_number(__FILE__, __LINE__);
                file.close();
                throw "Error while reading from input file\n";
            }
            file.close();
        } // end of Parser::parse_required_args
//-----------------------------------------------------------------------------

        // Return sensor configurations to function in TestCase class
        sensor_name_sensor_configs return_sensor_configs()
        {
           return sensor_config_mapping;
        } // end Parser::return_sensor_configs
//-----------------------------------------------------------------------------

        // Removes mandatory arguents and then parses optional arguments from the
        // input file
         void parse_optional_args(std::string args)
         {
            std::string HYPEN_DELIMITER = "-";
            std::string WHITE_SPACE_DELIMITER = " ";
            std::string optional_args;

            // Remove mandatory arguments
            size_t pos = args.find_last_of(WHITE_SPACE_DELIMITER);
            args.erase(0, pos + WHITE_SPACE_DELIMITER.length());

            // Set the values for the respective keys
            set_values_of_optional_args(args);
         } // Parser::parse_optional_args
        //---------------------------------------------------------------------

        // Populates the map after parsing optional arguments
        void set_values_of_optional_args(const std::string& optional_args)
        {
          std::istringstream iss(optional_args);
          std::string s1;
          std::string s2;
          std::string token;
          size_t pos;

          // Function pointer to set values for optional arguments
          typedef void (*opt_args_fptr)(const std::string&);

          typedef std::map <std::string, opt_args_fptr> opt_args_map;
          opt_args_map opt_args_mapping;

          // Insert the name of the variable passed from command-line as key
          // and the address of the function ptr as value
          opt_args_mapping.insert(std::make_pair(SEED, &func_ptr_seed));
          opt_args_mapping.insert(std::make_pair(MAX_CAP,
                                                    &func_ptr_max_cap));
          opt_args_mapping.insert(std::make_pair(TEST_ITERATION,
                                               &func_ptr_test_iteration));
          opt_args_mapping.insert(std::make_pair(TEST_DURATION,
                                               &func_ptr_test_duration));
          opt_args_mapping.insert(std::make_pair(TEST_TYPE,
                                                &func_ptr_test_type));
          opt_args_mapping.insert(std::make_pair(USB_STATE,
                                                &func_ptr_usb_state));
          opt_args_mapping.insert(std::make_pair(NUM_QMI_CLIENTS,
                                               &func_ptr_num_qmi_clients));
          opt_args_mapping.insert(std::make_pair(TOT_REPORT_COUNT,
                                               &func_ptr_tot_report_count));
          opt_args_mapping.insert(std::make_pair(MAX_REQUEST_DURATION,
                                               &func_ptr_max_request_duration));
          opt_args_mapping.insert(std::make_pair(USTA_LOG,
                                                 &func_ptr_usta_log_status));

          // Read till ','
          while (std::getline(iss, token, ',')) {
                pos = token.find('=');
                s1 = token.substr(0, pos);
                s2 = token.substr(pos + 1);

                opt_args_fptr fp = opt_args_mapping[s1];
                // Check if an invalid optional arg is passed
                if (fp != nullptr) {
                    (*fp)(s2);
                }
          }
          // Generate new random seed if no seed is supplied
          if (random_seed == 0) {
              random_seed = get_new_random_seed();
           }
        } // end of Parser::set_values_of_optional_args
//----------------------------------------------------------------------------

        // Set the random seed passed from the input file
        void func_ptr_seed(const std::string& seed)
        {
           // Convert from std::string to unsigned long long
           random_seed = std::stoull(seed);

           std::cout << "Using previously generated seed: ";
           std::cout << random_seed << std::endl << std::endl;
        } // end Parser::func_ptr_seed
//----------------------------------------------------------------------------

        // Override the max capacity of the target from the input file
        void func_ptr_max_cap(const std::string& max_cap)
        {
         optional_args_mapping.insert(std::make_pair(MAX_CAP, max_cap));
         std::cout << "Maximum Capacity " << max_cap << std::endl;
        } // end Parser::func_ptr_max_capacity
//----------------------------------------------------------------------------

        // Override the default test iteration from the input file
        void func_ptr_test_iteration(const std::string& test_iteration)
        {
           optional_args_mapping.insert(std::make_pair(TEST_ITERATION,
                                                               test_iteration));
        } // end Parser::func_ptr_test_iteration
//----------------------------------------------------------------------------

        // Override the default test duration from the input file
        void func_ptr_test_duration(const std::string& test_duration)
        {
           optional_args_mapping.insert(std::make_pair(TEST_DURATION,
                                                                test_duration));
        } // end Parser::func_ptr_test_duration
//----------------------------------------------------------------------------

        // Override the default number of qmi clients from the input file
        void func_ptr_num_qmi_clients(const std::string& num_qmi_clients)
        {
         optional_args_mapping.insert(std::make_pair(NUM_QMI_CLIENTS,
                                                              num_qmi_clients));
        } // end Parser::func_ptr_num_qmi_clients
//----------------------------------------------------------------------------

        // Override the default report count from the input file
        void func_ptr_tot_report_count(const
                                               std::string& tot_report_count)
        {
           optional_args_mapping.insert(std::make_pair(TOT_REPORT_COUNT,
                                                              tot_report_count));
        } // end Parser::func_ptr_tot_report_count
//----------------------------------------------------------------------------

        // Override the default max request duration from the input file
        void func_ptr_max_request_duration(const std::string&
                                                   max_request_duration)
        {
          optional_args_mapping.insert(std::make_pair(
                                                        MAX_REQUEST_DURATION,
                                                        max_request_duration));
        }
//----------------------------------------------------------------------------

        // Override the default test type from the input file
        void func_ptr_test_type(const std::string& test_type)
        {
           optional_args_mapping.insert(std::make_pair(TEST_TYPE, test_type));
        } // end Parser::func_ptr_test_type
//----------------------------------------------------------------------------

        // Override the default usb state from the input file
        void func_ptr_usb_state(const std::string& usb_state)
        {
           optional_args_mapping.insert(std::make_pair(USB_STATE, usb_state));
        } // end Parser::func_ptr_usb_state
//----------------------------------------------------------------------------

        // Override the default usta log status from the input file
        void func_ptr_usta_log_status(const std::string& usta_log_status)
        {
          optional_args_mapping.insert(std::make_pair(USTA_LOG, usta_log_status));
        } // end Parser::func_ptr_usta_log_status
//----------------------------------------------------------------------------

        // Return the supported max capacity of the target
        int get_max_cap()
        {
           // If max_cap is passed from text file
           auto pos = optional_args_mapping.find(MAX_CAP);
           if (pos != optional_args_mapping.end()) {
               return (stoi(optional_args_mapping[MAX_CAP]));
           }
           // Return default value
           return(default_max_cap);
        } // end Parser::get_max_cap
//----------------------------------------------------------------------------

        // Return the number of times the test needs to be run
        int get_test_iteration()
        {
            // If test_iteration is passed from text file
            auto pos = optional_args_mapping.find(TEST_ITERATION);
            if (pos != optional_args_mapping.end()) {
                return (stoi(optional_args_mapping[TEST_ITERATION]));
            }
            // Return default value
            return(default_test_iteration);
        } // end Parser::get_test_iteration
//----------------------------------------------------------------------------

        // Return the test duration
        float get_test_duration()
        {
           // Set test_duration passed from test input file
           auto pos = optional_args_mapping.find(TEST_DURATION);
           if (pos != optional_args_mapping.end()) {
                return (stof(optional_args_mapping[TEST_DURATION]));
           }
            // Return default value
            return(default_test_duration);
        } // Parser::get_test_duration
//----------------------------------------------------------------------------

        // Return the number of qmi clients to be used
        int get_num_qmi_clients()
        {
            auto pos = optional_args_mapping.find(NUM_QMI_CLIENTS);
            if (pos != optional_args_mapping.end()) {
                return (stoi(optional_args_mapping[NUM_QMI_CLIENTS]));
            }
            // Return default value
            return(default_qmi_clients);
        } // Parser::get_num_qmi_clients
//----------------------------------------------------------------------------

        // Return report count
        int get_tot_report_count()
        {
            auto pos = optional_args_mapping.find(TOT_REPORT_COUNT);
            if (pos != optional_args_mapping.end()) {
                return (stoi(optional_args_mapping[TOT_REPORT_COUNT]));
            }
            // Return default value
            return(default_tot_report_count);
        } // Parser::get_tot_report_count
//----------------------------------------------------------------------------

        // Return max request duration
        float get_max_request_duration()
        {
            auto pos = optional_args_mapping.find(MAX_REQUEST_DURATION);
            if (pos != optional_args_mapping.end()) {
                return (stoi(optional_args_mapping[MAX_REQUEST_DURATION]));
            }
            // Return default value
            return(default_max_request_duration);
        } // Parser::get_max_request_duration
//----------------------------------------------------------------------------

        // Return test type
        std::string get_test_type()
        {
            // Set test_state passed from test input file
            auto pos = optional_args_mapping.find(TEST_TYPE);
            if (pos != optional_args_mapping.end()) {
                return (optional_args_mapping[TEST_TYPE]);
            }
            // Return default value
            return (default_test_type);
        } // end Parser::get_test_type
//----------------------------------------------------------------------------

        // Return usb state
        std::string get_usb_state()
        {
            // Set test_state passed from test input file
            auto pos = optional_args_mapping.find(USB_STATE);
            if (pos != optional_args_mapping.end()) {
                return (optional_args_mapping[USB_STATE]);
            }
            // Return default value
            return (default_usb_state);
        } // end Parser::get_usb_state
//----------------------------------------------------------------------------

        // Return usta_log status
        bool get_usta_log_status()
        {
            auto pos = optional_args_mapping.find(USTA_LOG);
            if (pos != optional_args_mapping.end()) {
                  return (stoi(optional_args_mapping[USTA_LOG]));
            }
            return (default_usta_log);
        } // end Parser::get_usta_log_status
//----------------------------------------------------------------------------

        // Return cumulative sample rate of streaming sensors
        // Currently not used
        float get_cumulative_sample_rate()
        {
            return (tot_sample_rate);
        } // end Parser::get_tot_sample_rate
//----------------------------------------------------------------------------

        // Return random seed
        unsigned long long get_seed()
        {
           return (random_seed);

        } // end Parser::get_seed
//----------------------------------------------------------------------------

    } // end namespace Parser
} // end namespace sensors_stability

///////////////////////////////////////////////////////////////////////////////
