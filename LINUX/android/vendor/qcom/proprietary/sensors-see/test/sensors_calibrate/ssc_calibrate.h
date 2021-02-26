/*============================================================================
  @SSC_Calibrate.h

  Sensor calibration for ssc header file

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

#include <iostream>
#include <unordered_map>
#include <cinttypes>
#include <unistd.h>
#include "ssc_connection.h"
#include "ssc_utils.h"
#include "sensors_log.h"

#include "sns_std_sensor.pb.h"
#include "sns_physical_sensor_test.pb.h"

using namespace std;

/**
 * @brief Class for discovering the sensor_uid for a datatype
 */
class ssc_calibrate
{

public:
    ssc_calibrate();
    ~ssc_calibrate();
    /**
     * @brief do sensor test for a given datatype and test_type
     *
     * @param datatype
     * @return string include sensot name and test result
     */
    std::string calibrate(const std::string& datatype, const uint8_t &test_type);

private:

    struct sensor_uid_hash
    {
        std::size_t operator()(const sensor_uid& suid) const
        {
            std::string data(reinterpret_cast<const char*>(&suid), sizeof(sensor_uid));
            return std::hash<std::string>()(data);
        }
    };

    struct sensor_attributes
    {
        uint8_t num_attributes;
        std::unordered_map<int32_t, sns_std_attr_value> _attr_map;
    };

    /* map of sensor suids for each datatype */
    std::unordered_map<std::string, std::vector<sensor_uid>> _sensor_suid_map;
    std::unordered_map<sensor_uid, std::string, sensor_uid_hash> _sensortest_map;
    std::unordered_map<sensor_uid, sensor_attributes, sensor_uid_hash> _attributes_map;
    std::vector<sensor_uid> avaib_suids;
    sensor_attributes attrs;
    std::condition_variable _cv_attr;
    std::condition_variable _cv_phy_test;
    std::condition_variable _cv_result;
    std::mutex _attr_mutex;
    std::mutex _test_mutex;
    std::mutex _result_mutex;
    uint32_t _pending_attrs;
    uint32_t _pending_test;
    uint8_t _test_type;

    std::string get_sensor_name(sensor_uid suid);
    std::string get_invalid_result(std::string data_type);
    std::string get_test_result(sensor_uid suid);
    std::string test_result_list(std::string data_type);
    const std::string get_string(sensor_uid suid, int32_t attr_id);
    vector<float> get_floats(sensor_uid suid, int32_t attr_id);
    vector<int64_t> get_ints(sensor_uid suid, int32_t attr_id);
    vector<pair<float, float>> get_ranges(sensor_uid suid);
    int64_t get_stream_type(sensor_uid suid);
    std::string suid_to_string(sensor_uid suid);
    std::vector<sensor_uid> get_all_available_suids(std::string data_type, uint8_t test_type);

    /**
     * Send a SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG
     * to sensor driver
     */
    void
    send_selftest_req(ssc_connection *conn, sensor_uid const *suid, const uint8_t &test_type);

    /**
     * Send a SNS_STD_MSGID_SNS_STD_ATTR_REQ
     * to sensor driver
     */
    void send_attr_req(ssc_connection *conn, sensor_uid const *suid);

   /**
    * Physical test event handle function
    */
    void handle_physical_test_event(sensor_uid suid,
                const sns_client_event_msg_sns_client_event& pb_event);

   /**
    * Attribute event handle function
    */
    void handle_attribute_event(sensor_uid suid,
                const sns_client_event_msg_sns_client_event& pb_event);

};
