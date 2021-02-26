/*=============================================================================
  @file ssc_calibrate.cpp

  Sensorcal module: using libssc function to send requst SUIDS for given data
  type and do a test for given test type.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "ssc_calibrate.h"

using namespace google::protobuf::io;

/*=============================================================================
  Static Data
  ===========================================================================*/

static const auto SENSOR_GET_TIMEOUT = 3000ms;
static const auto WAIT_RESULT_TIMEOUT = 5s;
/*=============================================================================
  Function Definitions
  ===========================================================================*/
ssc_calibrate::ssc_calibrate()
{
    sensors_log::set_tag("ssc_calibrate");
    sensors_log::set_level(sensors_log::VERBOSE);
    sensors_log::set_stderr_logging(true);
    _pending_attrs = 0;
    _pending_test = 0;
}

ssc_calibrate::~ssc_calibrate() {

}

const std::string ssc_calibrate::get_string(sensor_uid suid, int32_t attr_id)
{
    string empty_string = "";
    const sensor_attributes& attrs_t = _attributes_map.at(suid);
    const sns_std_attr_value& attr_value = attrs_t._attr_map.at(attr_id);
    if (attr_value.values_size() > 0 && attr_value.values(0).has_str()) {
        return attr_value.values(0).str();
    }
    sns_logd("attribute value is not a string");
    return empty_string;
}

vector<float> ssc_calibrate::get_floats(sensor_uid suid, int32_t attr_id)
{
    const sensor_attributes& attrs_t = _attributes_map.at(suid);
    auto it = attrs_t._attr_map.find(attr_id);
    if (it == attrs_t._attr_map.end()) {
        return vector<float>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<float> floats(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (!attr_value.values(i).has_flt()) {
            sns_loge("attribute :%d and element %d is not a float", attr_id, i);
        }
        floats[i] = attr_value.values(i).flt();
    }
    return floats;
}

vector<int64_t> ssc_calibrate::get_ints(sensor_uid suid, int32_t attr_id)
{
    const sensor_attributes& attrs_t = _attributes_map.at(suid);
    auto it = attrs_t._attr_map.find(attr_id);
    if (it == attrs_t._attr_map.end()) {
        return vector<int64_t>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<int64_t> ints(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (!attr_value.values(i).has_sint()) {
            sns_loge("attribute :%d and element %d is not an int", attr_id, i);
        }
        ints[i] = attr_value.values(i).sint();
    }
    return ints;
}

vector<pair<float, float>> ssc_calibrate::get_ranges(sensor_uid suid)
{
    const sensor_attributes& attrs_t = _attributes_map.at(suid);
    auto it = attrs_t._attr_map.find(SNS_STD_SENSOR_ATTRID_RANGES);
    if (it == attrs_t._attr_map.end()) {
        return vector<pair<float, float>>();
    }
    const sns_std_attr_value& attr_value = it->second;
    vector<pair<float, float>> ranges(attr_value.values_size());
    for (int i=0; i < attr_value.values_size(); i++) {
        if (attr_value.values(i).has_subtype() &&
            attr_value.values(i).subtype().values_size() > 1 &&
            attr_value.values(i).subtype().values(0).has_flt() &&
            attr_value.values(i).subtype().values(1).has_flt())
        {
            ranges[i].first = attr_value.values(i).subtype().values(0).flt();
            ranges[i].second = attr_value.values(i).subtype().values(1).flt();
        } else {
            sns_loge("malformed value for SNS_STD_SENSOR_ATTRID_RANGES");
        }
    }
    return ranges;
}


int64_t ssc_calibrate::get_stream_type(sensor_uid suid)
{
    auto ints = get_ints(suid, SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
    if (ints.size() == 0) {
        sns_loge("stream_type attribute not available");
        return -1;
    }
    return ints[0];
}

std::string ssc_calibrate::suid_to_string(sensor_uid suid)
{
   std::string suid_str;
   std::string str = to_string((unsigned long)suid.high);
   suid_str.append(str, 0, 4);
   return suid_str;
}


std::vector<sensor_uid> ssc_calibrate::get_all_available_suids(std::string data_type, uint8_t test_type)
{
    std::vector<sensor_uid> avalib_suids;
    std::vector<sensor_uid> sensor_uids = _sensor_suid_map.at(data_type);

    if((data_type == "pressure" || data_type == "humidity" || data_type == "mag")
        && test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY) {
           return vector<sensor_uid>();
    } else {

        if(sensor_uids.size() == 1)
            return sensor_uids;

        if(sensor_uids.size() == 2)
        {
            std::vector<sensor_uid>::iterator suid_1 = sensor_uids.begin();
            std::vector<sensor_uid>::iterator suid_2 = sensor_uids.end()-1;
            if(get_sensor_name(*suid_1) == get_sensor_name(*suid_2) &&
               get_stream_type(*suid_1) != get_stream_type(*suid_2)) {

                avalib_suids.push_back(*suid_1);
                return avalib_suids;
            }

            avalib_suids.push_back(*suid_1);
            avalib_suids.push_back(*suid_2);
            return avalib_suids;
        }
        return vector<sensor_uid>();
    }

}


std::string ssc_calibrate::get_sensor_name(sensor_uid suid)
{
    std::string sensor_name;
    std::string tmp;

    tmp = get_string(suid, SNS_STD_SENSOR_ATTRID_NAME);
    if (tmp.empty()) {
          sns_loge("name attribute not available");
          return "error";
    }
    sensor_name.append(tmp, 0, tmp.size()-1);
    return sensor_name;
}

std::string ssc_calibrate::get_invalid_result(std::string data_type)
{
    std::vector<sensor_uid> sensor_uids = _sensor_suid_map.at(data_type);
    std::string sensor_name;
    std::string sensor_name1;
    std::string sensor_name2;

    if(sensor_uids.size() == 1) {
        sensor_name = get_sensor_name(*sensor_uids.begin());
        return sensor_name + ":" + "-1" + ";";
    }
    /* suppose only support at most two suids for given data_type */
    if(sensor_uids.size() == 2) {
        std::vector<sensor_uid>::iterator suid_1 = sensor_uids.begin();
        std::vector<sensor_uid>::iterator suid_2 = sensor_uids.end()-1;
        sensor_name1 = get_sensor_name(*suid_1);
        sensor_name2 = get_sensor_name(*suid_2);
        if(sensor_name1 != sensor_name2) {
            return sensor_name1 + ":" + "-1" + ";" + sensor_name2 + ":" + "-1" + ";";
        }
        return sensor_name1 + ":" + "-1" + ";";
    }

    return "not support more than two suids;";
}

std::string ssc_calibrate::get_test_result(sensor_uid suid)
{
    auto it = _sensortest_map.find(suid);
    if (it != _sensortest_map.end()) {
        return it->second;
    } else {
        return "-1";
    }
}


std::string ssc_calibrate::test_result_list(std::string data_type)
{
    std::string result, tmp;
    std::string sensor_name;

    int num = 0;
    for (const sensor_uid& suid : avaib_suids) {
        sns_logd("test result for %s-%d", data_type.c_str(), num);
        num++;
        sensor_name = get_sensor_name(suid);
        /* distinguish between two sensors that have same sensor name */
        tmp = suid_to_string(suid);
        result.append(sensor_name, 0, sensor_name.length());
        const std::string& result_tmp = get_test_result(suid);
        result = result + "-" + tmp + ":" + result_tmp + ";";

    }
    return result;
}



/**
 * Event callback function, as registered with ssc_connection.
 */
void ssc_calibrate::handle_physical_test_event(sensor_uid suid,
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_physical_sensor_test_event test_event;
    test_event.ParseFromString(pb_event.payload());

    std::string sensor_name = get_sensor_name(suid);
    std::string test_status = std::to_string(test_event.test_passed());
    sns_logd("sensor name is %s,test result is %d, test type is %d",
          sensor_name.c_str(), test_event.test_passed(), test_event.test_type());
    _sensortest_map[suid] = test_status;

    /* notify the thread waiting for test result */
    unique_lock<mutex> test_lock(_test_mutex);
    _pending_test--;
    if (_pending_test == 0) {
        _cv_phy_test.notify_one();
    }

}


void ssc_calibrate::handle_attribute_event(sensor_uid suid,
    const sns_client_event_msg_sns_client_event& pb_event)
{
    sns_std_attr_event pb_attr_event;
    sns_std_attr_value attr_value;
    pb_attr_event.ParseFromString(pb_event.payload());

    attrs.num_attributes = pb_attr_event.attributes_size();
    for (int i=0; i<pb_attr_event.attributes_size(); i++) {
           sns_logv("attr_id is %d", pb_attr_event.attributes(i).attr_id());
           attrs._attr_map[pb_attr_event.attributes(i).attr_id()] = pb_attr_event.attributes(i).value();
    }
    _attributes_map[suid] = attrs;

    /* notify the thread waiting for attributes */
    unique_lock<mutex> lock(_attr_mutex);
    _pending_attrs--;
    if (_pending_attrs == 0) {
        _cv_attr.notify_one();
    }
}


/**
 * Send a SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG
 * to sensor driver
 */
void ssc_calibrate::
send_selftest_req(ssc_connection *conn, sensor_uid const *suid, const uint8_t &test_type)
{
    string pb_req_msg_encoded;
    string config_encoded;
    sns_client_request_msg pb_req_msg;
    sns_physical_sensor_test_config config;

    config.set_test_type((sns_physical_sensor_test_type)test_type);
    config.SerializeToString(&config_encoded);

    pb_req_msg.set_msg_id(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG);
    pb_req_msg.mutable_request()->set_payload(config_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(suid->high);
    pb_req_msg.mutable_suid()->set_suid_low(suid->low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->
        set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(pb_req_msg_encoded);
}

void ssc_calibrate::
send_attr_req(ssc_connection *conn, sensor_uid const *suid)
{
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;
    sns_std_attr_req pb_attr_req;
    string pb_attr_req_encoded;
    /* populate the pb_attr_req message */
    pb_attr_req.set_register_updates(false);
    pb_attr_req.SerializeToString(&pb_attr_req_encoded);
    /* populate the client request message */
    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    pb_req_msg.mutable_request()->set_payload(pb_attr_req_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(suid->high);
    pb_req_msg.mutable_suid()->set_suid_low(suid->low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    conn->send_request(pb_req_msg_encoded);
}


std::string ssc_calibrate::
calibrate(const string& datatype, const uint8_t &test_type)
{
    std::string test;
    _test_type = test_type;

    suid_lookup lookup(
     [this](const auto& datatype, const auto& suids)
     {
        sns_logv("Received SUID event with length %zu", suids.size());
        if(suids.size() > 0)
        {
            _sensor_suid_map[datatype] = suids;
            ssc_connection ssc_conn([this](const uint8_t* data, size_t size)
            {
                sns_logd("event callback start:\n");
                sns_client_event_msg pb_event_msg;
                pb_event_msg.ParseFromArray(data, size);
                sensor_uid suid (pb_event_msg.suid().suid_low(),
                    pb_event_msg.suid().suid_high());
                for (int i=0; i < pb_event_msg.events_size(); i++) {
                    auto&& pb_event = pb_event_msg.events(i);
                    sns_logv("event[%d] msg_id=%d", i, pb_event.msg_id());
                    if (pb_event.msg_id() ==  SNS_STD_MSGID_SNS_STD_ATTR_EVENT) {
                        handle_attribute_event(suid, pb_event);
                    }
                    if (pb_event.msg_id() ==
                            SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT)
                        handle_physical_test_event(suid, pb_event);
                }
            });

            int count = 0;
            for (size_t i = 0; i < suids.size(); i++) {
                sns_logv("Received SUID %" PRIx64 "%" PRIx64 " for '%s'-%d",
                    suids[i].high, suids[i].low, datatype.c_str(), count);
                count++;
                _pending_attrs++;
                send_attr_req(&ssc_conn, &suids[i]);
            }

            sns_logd("waiting for attributes...");
            /* wait until attributes are received */
            unique_lock<mutex> lock(_attr_mutex);
            while( _pending_attrs) {
                _cv_attr.wait(lock);
            }
            sns_logd("attributes received");

            avaib_suids = get_all_available_suids(datatype, _test_type);
            if(avaib_suids.size()) {
                count = 0;
                for (size_t i = 0; i < avaib_suids.size(); i++) {
                    sns_logv("Received SUID %" PRIx64 "%" PRIx64 " for '%s'-%d",
                        avaib_suids[i].high, avaib_suids[i].low, datatype.c_str(), count);
                    count++;
                    _pending_test++;
                    send_selftest_req(&ssc_conn, &avaib_suids[i], _test_type);
                }

                /* wait until test result are received */
                unique_lock<mutex> test_lock(_test_mutex);
                bool timeout = !_cv_phy_test.wait_for(test_lock, SENSOR_GET_TIMEOUT,
                            [this] { return (_pending_test == 0); });
                if (timeout) {
                    sns_loge("timeout while waiting for test result, pending = %d",
                    _pending_test);
                    return;
                }
                sns_logd("test result received");

                /* notify the thread waiting for test result */
                unique_lock<mutex> result_lock(_result_mutex);
                _cv_result.notify_one();
           }
       } else {
            sns_loge("not found suids for given datatype");
            exit(1);
      }
    });

    lookup.request_suid(datatype);

    /* wait avaib_suids */
    usleep(100000);
    if(avaib_suids.size()) {
        /* wait until test result are received */
        unique_lock<mutex> result_lock(_result_mutex);
        if(_cv_result.wait_for(result_lock, std::chrono::seconds(WAIT_RESULT_TIMEOUT))
           == std::cv_status::timeout) {
            test = test_result_list(datatype);
            sns_logd("test result: %s", test.c_str());
            return test;
        }
        test = test_result_list(datatype);
        sns_logd("test result: %s", test.c_str());
        return test;
    }

   test = get_invalid_result(datatype);
   sns_logd("test result: %s", test.c_str());
   return test;
}

