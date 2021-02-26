/*=============================================================================
  @file sns_client_test.cpp

  Example client written in C++, using libssc.  Client requests Accel SUID,
  and subsequently sends an enable request to it.  Data will stream for 10
  seconds.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <iostream>
#include <cinttypes>
#include <unistd.h>
#include "ssc_connection.h"
#include "ssc_utils.h"
#include "sensors_log.h"
#include <string>
#include <unordered_map>
#include <vector>

#include "sns_std_sensor.pb.h"
#include "sns_std_type.pb.h"
#include "sns_amd.pb.h"
#include "sns_physical_sensor_test.pb.h"

using namespace std;

/* general useful constants */
static const uint64_t USEC_PER_SEC = 1000000ull;

static int64_t stream_type;
char s_name[20] = "";
static int gSamplerate;


/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/*=============================================================================
  Static Data
  ===========================================================================*/

static ssc_connection *connection;

std::unordered_map<int32_t, sns_std_attr_value> _attr_map;

const string get_string(int32_t attr_id)
{
    const sns_std_attr_value& attr_value =  _attr_map[attr_id];
    if (attr_value.values_size() > 0 && attr_value.values(0).has_str()) {
        return attr_value.values(0).str();
    }
    sns_logd("attribute value is not a string");
    return NULL;
}

vector<float> get_floats(int32_t attr_id)
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
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

vector<int64_t> get_ints(int32_t attr_id)
{
    auto it = _attr_map.find(attr_id);
    if (it == _attr_map.end()) {
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

vector<pair<float, float>> get_ranges()
{
    auto it = _attr_map.find(SNS_STD_SENSOR_ATTRID_RANGES);
    if (it == _attr_map.end()) {
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


int64_t get_stream_type()
{
    auto ints = get_ints(SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
    if (ints.size() == 0) {
        sns_loge("stream_type attribute not available");
    }
    return ints[0];
}


/*print the attributes */
void print_sensor_info()
{
    sns_logv("--------------------- ");
    sns_logv("##### ATTRIBUTES #### ");
    sns_logv("--------------------- ");

    sns_logv("SNS_STD_SENSOR_ATTRID_NAME: %s \n", get_string(SNS_STD_SENSOR_ATTRID_NAME).c_str());
    sns_logv("SNS_STD_SENSOR_ATTRID_VENDOR : %s \n", get_string(SNS_STD_SENSOR_ATTRID_VENDOR).c_str());
    sns_logv("SNS_STD_SENSOR_ATTRID_TYPE :%s \n", get_string(SNS_STD_SENSOR_ATTRID_TYPE).c_str());

    stream_type = get_stream_type();
    switch (stream_type) {
      case SNS_STD_SENSOR_STREAM_TYPE_STREAMING: {
          vector<float> rates = get_floats(SNS_STD_SENSOR_ATTRID_RATES);
          /* streaming sensors must have rates attribute */
          if (rates.size() == 0) {
              sns_logv("rates attribute unavailable for a streaming sensor \n");
          }

          /* ceil() is used to make sure that delay (period) is not
             truncated, as it will cause the calculated rates to be
             more than what is supported by the sensor */
          sns_logv("max_delay: %lld uSec\n", (long long)int64_t(ceil(USEC_PER_SEC / rates[0])));
          sns_logv("min_delay: %lld uSec\n", (long long)int64_t(ceil(USEC_PER_SEC / rates[rates.size()-1])));
          break;
      }
      default:
          sns_logv("invalid stream_type %d", (int)stream_type);
    }

    auto resolutions = get_floats(SNS_STD_SENSOR_ATTRID_RESOLUTIONS);

    sns_logv("-----------------------");
    sns_logv("supported resolutions: ");
    sns_logv("-----------------------");
    for (int i = 0; i < (int)resolutions.size() ; i++) {
        sns_logv("%f", resolutions[i]);
    }
    sns_logv("-----------------------");

    auto ranges = get_ranges();
    sns_logv("supported ranges: ");
    sns_logv("-----------------------");
    for (int i = 0; i < (int)ranges.size() ; i++) {
        sns_logv("%f", ranges[i].second);
    }
    sns_logv("-----------------------");

}


/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Event callback function, as registered with ssc_connection.
 */
static void
event_cb(const uint8_t *data, size_t size)
{
  sns_client_event_msg pb_event_msg;

  sns_logv("Received QMI indication with length %zu", size);

  pb_event_msg.ParseFromArray(data, size);
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
    sns_logv("event[%d] msg_id=%d, ts=%llu", i, pb_event.msg_id(),
             (unsigned long long) pb_event.timestamp());

    if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id())
    {
      sns_std_error_event error;
      error.ParseFromString(pb_event.payload());

      sns_loge("Received error event %i", error.error());
    }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_physical_config_event config;
      config.ParseFromString(pb_event.payload());

      sns_loge("Received config event with sample rate %f", config.sample_rate());
    }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == pb_event.msg_id())
    {
      sns_std_sensor_event event;
      event.ParseFromString(pb_event.payload());

      sns_logv("Received %s sample <%f, %f, %f>",s_name,
          event.data(0), event.data(1), event.data(2));
    }
    else if(SNS_AMD_MSGID_SNS_AMD_EVENT == pb_event.msg_id())
    {
        sns_amd_event event;
        event.ParseFromString(pb_event.payload());
        sns_logv("Received %s sample <%d>",s_name, event.state());
    }
    else if(SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT == pb_event.msg_id())
    {
        sns_physical_sensor_test_event test_event;
        test_event.ParseFromString(pb_event.payload());
        sns_logv("%s test passed 0:fail 1:pass <%d>",s_name, test_event.has_test_passed());

    }
    else if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id())
    {
       sns_std_attr_event attr_event;
       attr_event.ParseFromString(pb_event.payload());
       for (int i = 0; i < attr_event.attributes_size(); i++) {
        sns_loge("attribute ID :%d", attr_event.attributes(i).attr_id());
        _attr_map[attr_event.attributes(i).attr_id()] = attr_event.attributes(i).value();
       }
       print_sensor_info();
    }
    else
    {
      sns_loge("Received unknown message ID %i", pb_event.msg_id());
    }
  }
}


/**
 * Send a SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG to SUID
 */
static void
send_selftest_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  string config_encoded;
  sns_client_request_msg pb_req_msg;
  sns_physical_sensor_test_config config;
  sns_loge("sending self test request");

  if(gSamplerate == -1)
    config.set_test_type(SNS_PHYSICAL_SENSOR_TEST_TYPE_SW);
  else if(gSamplerate == -2)
    config.set_test_type(SNS_PHYSICAL_SENSOR_TEST_TYPE_HW);
  else if(gSamplerate == -3)
    config.set_test_type(SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY);
  else if(gSamplerate == -4)
    config.set_test_type(SNS_PHYSICAL_SENSOR_TEST_TYPE_COM);

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


/**
 * Send a SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG to SUID
 */
static void
send_onchange_config_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  sns_client_request_msg pb_req_msg;
  sns_std_sensor_config config;

  sns_loge("sending on change config request");

  pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG);
  pb_req_msg.mutable_request()->clear_payload();
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
      set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  conn->send_request(pb_req_msg_encoded);
}


/**
 * Send a SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG to SUID
 */
static void
send_config_req(ssc_connection *conn, sensor_uid const *suid)
{
  string pb_req_msg_encoded;
  string config_encoded;
  sns_client_request_msg pb_req_msg;
  sns_std_sensor_config config;

  sns_loge("sending on normal config request and sample rate %d", (int)gSamplerate);

  config.set_sample_rate(gSamplerate);
  config.SerializeToString(&config_encoded);

  pb_req_msg.set_msg_id(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
  pb_req_msg.mutable_request()->set_payload(config_encoded);
  pb_req_msg.mutable_suid()->set_suid_high(suid->high);
  pb_req_msg.mutable_suid()->set_suid_low(suid->low);
  pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
  pb_req_msg.mutable_susp_config()->
      set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

  pb_req_msg.SerializeToString(&pb_req_msg_encoded);

  conn->send_request(pb_req_msg_encoded);
}

void send_attr_request(ssc_connection *conn, sensor_uid const *suid)
{
    sns_client_request_msg pb_req_msg;
    sns_std_attr_req pb_attr_req;
    string pb_attr_req_encoded;
    string pb_req_msg_encoded;

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


/**
 * SUID callback as registered with suid_lookup.
 */
static void
suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids)
{
  sns_logv("Received SUID event with length %zu", suids.size());
  if(suids.size() > 0)
  {
    sensor_uid suid = suids.at(0);
    connection = new ssc_connection(event_cb);

    sns_logv("Received SUID %" PRIx64 "%" PRIx64 " for '%s'",
        suid.high, suid.low, datatype.c_str());

    if(gSamplerate <= -1 && gSamplerate >= -4) {
        sns_logv("self test request");
        send_selftest_req(connection, &suid);
    } else  {
        send_attr_request(connection, &suid);
        sleep(3);
        if(stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING) {
          send_config_req(connection, &suid);
        }
        else if(stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) {
            send_onchange_config_req(connection, &suid);
        }
        else {
          sns_loge("not supported format");
          exit(-1);
        }
    }
  } else {
    sns_loge("FAILED suid not found for given sensor");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{

  if(argc != 3) {
    cout << "usage sns_client_example_cpp <sensor name> samplerate" << endl;
    cout << "(on change sensors do not honor samplerate but needs samplerate argument for testapp compatibiltiy)" << endl;
    //SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG = 515,
    cout << "(samplerate -1 is considered as SNS_PHYSICAL_SENSOR_TEST_TYPE_SW )" << endl;
    cout << "(samplerate -2 is considered as SNS_PHYSICAL_SENSOR_TEST_TYPE_HW )" << endl;
    cout << "(samplerate -3 is considered as SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY )" << endl;
    cout << "(samplerate -4 is considered as SNS_PHYSICAL_SENSOR_TEST_TYPE_COM )" << endl;
    exit(1);
  }

  strlcpy(s_name, argv[1], sizeof(s_name));
  sensors_log::set_tag("sns_client_example");
  sensors_log::set_level(sensors_log::VERBOSE);
  sensors_log::set_stderr_logging(true);

  gSamplerate = atoi(argv[2]);


  cout <<"starting sensors test for press ctrl-C to stop " << s_name << endl;

  suid_lookup lookup(suid_cb);
  lookup.request_suid(s_name);


  sleep(UINT_MAX);
  // to stop sensor
  delete connection;
  //  no sensor samples after this
  sns_logv("Test completed, closing");

  return 0;
}

