/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: Parser.h
** DESC: Parses data from the input file and populates the parsed
*        data to multi-map
** ================================================================ */

#ifndef PARSER_H
#define PARSER_H

#include <map>          // std::map
#include <random>       // std::mt19937
#include <iostream>     // std::cout

namespace sensors_stability {

    namespace Parser {

        // Required arguments initialized in .cpp file
        // Mersenne twister engine random number generator
        extern std::mt19937 mt; 
        extern unsigned long long random_seed;

        // Required arguments initialized to string values, passed from input
        // file
        const std::string SAMPLE_RATE = "sample_rate";
        const std::string ODR_RATE = "odr_rate";
        const std::string RESAMPLED_RATE = "resampled_rate";

        // Optional arguments initialized to string values, passed from input
        // file
        const std::string SEED = "seed";
        const std::string MAX_CAP = "max_cap";
        const std::string TEST_TYPE = "test_type";
        const std::string USB_STATE = "usb_state";
        const std::string TEST_ITERATION = "test_iteration";
        const std::string TEST_DURATION = "test_duration";
        const std::string NUM_QMI_CLIENTS = "num_qmi_clients";
        const std::string TOT_REPORT_COUNT = "tot_report_count";
        const std::string MAX_REQUEST_DURATION = "max_request_duration";
        const std::string MIN_RATE = "min_rate";
        const std::string MAX_RATE = "max_rate";
        const std::string USTA_LOG = "usta_log";

        // Primitive types must be Initialized in .cpp file
        // Initialized to 2000. Req is from Marko's Moast test spec
        extern int default_max_cap;
        // Initialized to 600
        extern int default_tot_report_count;
        // Initialized to 1
        extern int default_test_iteration;
        // Initialized to 1
        extern int default_qmi_clients;
        // Initialized to 1.0f
        extern float default_max_request_duration;
        // Initialized to 1.0f
        extern float default_test_duration;

        // Initialized to "rest"
        extern std::string default_test_type;
        // Initialized to "on"
        extern std::string default_usb_state;

        // Initialized to 1 i.e. true
        extern bool default_usta_log;

/*========================================================================
 *               Function declarations
 ========================================================================*/

     /**
     * Parses data passed from the input file. Parsed data is then populated to
     * a multi-map
     *
     * @param required args passed as const std::string reference
     * @return void
     */
      void parse_required_args(const std::string& required_args);
//-----------------------------------------------------------------------------

     /**
     * Remove required arguments from the input file and then parse optional
     * arguments
     *
     * @param args passed as std::string, pass-by-value
     * @return void
     */
      void parse_optional_args(std::string args);
//-----------------------------------------------------------------------------

     /**
     * Populate the parsed values from the optional arguments as key-value
     * pairs to opt_args_mapping
     *
     * @param optional_args passed as const string reference.
     * @return void
     */
      void set_values_of_optional_args(const std::string& optional_args);
//-----------------------------------------------------------------------------

     /**
     * Set the seed passed from the input file
     *
     * @param seed passed as const std::string reference
     * @return void
     */
      void func_ptr_seed(const std::string& seed);
//-----------------------------------------------------------------------------

     /**
     * Override the default max capacity of the target by passing from input file
     * Default is 2000
     *
     * @param max_cap passed as const std::string reference
     * @return void
     */
      void func_ptr_max_cap(const std::string& max_cap);
//-----------------------------------------------------------------------------

     /**
     * Override the default test duration for the test by passing from
     * the input file. Default is 1
     *
     * @param test_duration passed as const std::string reference
     * @return void
     */
      void func_ptr_test_duration(const std::string& test_duration);
//-----------------------------------------------------------------------------

     /**
     * Override the default test iteration for the test by passing from the input
     * file. Default is 1
     *
     * @param test_iteration passed as const std::string reference
     * @return void
     */
      void func_ptr_test_iteration(const std::string& test_iteration);
//-----------------------------------------------------------------------------

     /**
     * Override the default test type for the test by passing from the input file
     * Default is rest
     *
     * @param test_type passed as const std::string reference
     * @return void
     */
      void func_ptr_test_type(const std::string& test_type);
//-----------------------------------------------------------------------------

     /**
     * Override the default USB state for the test by passing from the input file
     * Default is on
     *
     * @param usb_state passed as const std::string reference
     * @return void
     */
      void func_ptr_usb_state(const std::string& test_state);
//-----------------------------------------------------------------------------

    /**
     * Override the default usta log status for the test by passing from the
     * input file. Default is usta logging off
     *
     * @param usta_log_status passed as const std::string reference
     * @return void
     */
      void func_ptr_usta_log_status(const std::string& usta_log_status);
//-----------------------------------------------------------------------------

     /**
     * Override the default number of QMI clients for the test by passing from the
     * input file. Default is 1
     *
     * @param num_qmi_clients passed as const std::string reference
     * @return void
     */
      void func_ptr_num_qmi_clients(const std::string& num_qmi_clients);
//-----------------------------------------------------------------------------

     /**
     * Override the default total report count for the test by passing from the
     * input file. Default is 600
     *
     * @param tot_report_count passed as const std::string reference
     * @return void
     */
      void func_ptr_tot_report_count(const std::string&
                                                    total_report_count_string);
//-----------------------------------------------------------------------------

     /**
     * Override the default max request duration for the test by passing from the
     * input file. Default is 1. Currently not used
     *
     * @param max_duration count passed as const std::string reference
     * @return void
     */
      void func_ptr_max_request_duration(const std::string&
                                                  max_request_duration_string);
//-----------------------------------------------------------------------------

       // Mapping of required arguments
       typedef std::multimap<std::string, std::string> sensor_configs;
       typedef std::multimap<std::string, sensor_configs>
                                                sensor_name_sensor_configs;
       extern sensor_name_sensor_configs sensor_config_mapping;

       // Mapping of optional arguments
       typedef std::map<std::string, std::string> optional_args_map;
       extern optional_args_map optional_args_mapping;

       // Total sample rates of streaming sensors
        extern float tot_sample_rate;

/*========================================================================
 *               Function declarations
 ========================================================================*/

     /**
     * Takes input file containing test case id, required and optional
     * arguments and invokes another function to parse the required and
     * optional arguments
     *
     * @param  argc passed as integer pass-by-value
     * @param  argv passed as array of pointer to char pass-by-value
     * @return void
     */
      void command_line_args(int argc, char* argv[]);
//-----------------------------------------------------------------------------

     /**
      * Generate a new random seed, if seed is not passed from input file
      *
      * @param void
      * @return unsigned long long randsom seed as value
      *
      */
      unsigned long long get_new_random_seed();
//-----------------------------------------------------------------------------

     /**
     * Return max capacity of the target
     *
     * @param void
     * @return max capacity of the target as integer value. Default is 2000
     */
     int get_max_cap();
//-----------------------------------------------------------------------------

     /**
     * Return cumulative sample rate of streaming sensors.
     *
     * @param void
     * @return tot_sample_rate as float value
     */
     float get_cumulative_sample_rate();  // Currently not used
//-----------------------------------------------------------------------------

     /**
     * Return test iteration
     *
     * @param void
     * @return test_iteration as integer value. Default is 1
     */
      int get_test_iteration();
//-----------------------------------------------------------------------------

     /**
     * Return test duration
     *
     * @param void
     * @return test_duration as float value. Default is 1.0f
     */
     float get_test_duration();
//-----------------------------------------------------------------------------

     /**
     * Return test type
     *
     * @param void
     * @return test_state as std::string value. Default is 'rest'
     */
      std::string get_test_type();
//-----------------------------------------------------------------------------

     /**
     * Return usb state
     *
     * @param void
     * @return usb_state as std::string value. Default is 'on' i.e. USB is
     * connected
     */
     std::string get_usb_state();
//-----------------------------------------------------------------------------

    /**
     * Return ussta log status
     *
     * @param void
     * @return usta_log status as bool value. Default is 0 i.e. usta logging is
     * off
     */
     bool get_usta_log_status();
//-----------------------------------------------------------------------------

     /**
     * Return sensor configurations to function in TestCase class
     *
     * @param
     * @return Sensor configurations mapping by value
     */
      sensor_name_sensor_configs return_sensor_configs();
//-----------------------------------------------------------------------------

     /**
     * Return random seed to function in TestCase class
     *
     * @param void
     * @return random_seed as unsigned long long value
     */
      unsigned long long get_seed();
//-----------------------------------------------------------------------------

     /**
     * Return number of qmi clients
     *
     * @param void
     * @return num_qmi_clients as integer value. Default is 1
     */
      int get_num_qmi_clients();
//-----------------------------------------------------------------------------

     /**
     * Return total report count
     *
     * @param void
     * @return total report count as integer value. Default is 600
     */
      int get_tot_report_count();
//-----------------------------------------------------------------------------

     /**
      * Return max request duration.
      *
      * @param void
      * @return max request duration as float value. Default is 1.0f
      */
       float get_max_request_duration(); // Currently not used
//-----------------------------------------------------------------------------

      /**
       * Override the default max capacity of the target by passing from input
       * file.
       *
       * @param max_cap as integer value
       * @return void
       */
       void set_max_cap(int max_cap);
//-----------------------------------------------------------------------------
    } // end namespace Parser

} // end namespace sensors_stability
///////////////////////////////////////////////////////////////////////////////

#endif /* PARSER_H */
