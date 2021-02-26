/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: TestCase.h
** DESC: Defines various test scenarios
** ================================================================ */

#ifndef TESTCASE_H
#define TESTCASE_H

#include <random>         // std::mt19937

namespace sensors_stability {

class TestCase {

private:

// Mersenne twister engine random number generator
static std::mt19937 mt;

// Holds generated random seed
//static unsigned long long random_seed;

public:
    TestCase() {}
    ~TestCase() {}

    static const int MICROSECONDS;
    static const int MILLISECONDS;
    static int client_num;
    static int rand_counter;
    std::string test_case_id;

    typedef std::multimap<std::string, std::string> sensor_configs;
    typedef std::multimap<std::string, sensor_configs> sensor_name_sensor_configs;
    static sensor_name_sensor_configs sensor_config_mapping;

    // Create a single instance of the class
    static TestCase* get_test_case_instance();

    // Prevent construction of new object by copy constructor
    TestCase(TestCase const&) = delete;

    // Prevent construction of new object when assignment operator is used
    void operator=(TestCase const&) = delete;
//----------------------------------------------------------------------------
    /**
     * Get testcase id and sensor_configs after the parsing
     * @param const reference to std::string and sensor configurations
     * multi-map passed by value
     * @return void
     */
    void get_test_params(const std::string& testcase_id,
                         const sensor_name_sensor_configs& sensor_config_mapping);
//----------------------------------------------------------------------------
    /**
     * Return microseconds
     *
     * @param void
     * @return int as value
     */
    static int get_microseconds();
//----------------------------------------------------------------------------
    /**
     * Return milliseconds
     *
     * @param void
     * @return int as value
     */
    static int get_milliseconds();
//----------------------------------------------------------------------------
};

} // namespace sensors_stability

///////////////////////////////////////////////////////////////////////////////

#endif /* TESTCASE_H */
