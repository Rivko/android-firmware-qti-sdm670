/* ===================================================================
** Copyright (c) 2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: ssc_threshold.cpp
** DESC: Android command line application to resample arbitrary sensor
** ================================================================ */
#include <iostream>
#include <sstream>
#include <cinttypes>

#include "sns_client.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_resampler.pb.h"
#include "sns_suid.pb.h"
#include "sns_threshold.pb.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

#include "ssc_connection.h"
#include "sensors_log.h"
#include "ssc_utils.h"

using namespace google::protobuf::io;
using namespace std;

static char *my_exe_name = (char *)"ssc_threshold";
static ssc_connection *connection;
static sensor_uid threshold_suid;
static sensor_uid resampler_suid;
static sensor_uid target_suid;

struct app_config {
    bool  is_valid = false;
    char  target_data_type[ 16];
    char  target_suid[ 36];         // long enough to hold ascii suid
    float batch_period = -1;        // seconds. default omitted.
    float duration = 10.0;          // seconds
    float target_rate = 10.0;       // hs sample_rate

    bool  use_resampler = false;
    sns_resampler_rate rate_type = SNS_RESAMPLER_RATE_MINIMUM;
    bool  filter = false;

    vector<float> thresholds;
    sns_threshold_type threshold_type = SNS_THRESHOLD_TYPE_RELATIVE_VALUE;

    bool debug = false;

    app_config() {}
};

static app_config config;

static int requested = 0;   // num suid lookups requested
static int received = 0; // num attribute responses received
static std::mutex m;
static std::condition_variable cv;

// //////// ///// /////// / ////
// //////// ///// /////// / ////
// //////// ///// /////// / ////

void run()
{
    sensor_uid *feed_suid;
    uint32_t feed_msgid;
    string feed_payload;

    if ( config.use_resampler) {
       // resampler
       sns_logv(">>> using sns_resampler_config <<<");

       sns_resampler_config resampler_config;
       resampler_config.mutable_sensor_uid()->set_suid_high(target_suid.high);
       resampler_config.mutable_sensor_uid()->set_suid_low(target_suid.low);
       resampler_config.set_resampled_rate(config.target_rate); // hz
       resampler_config.set_rate_type(config.rate_type);
       resampler_config.set_filter(config.filter);

       string resampler_config_encoded;
       resampler_config.SerializeToString(&resampler_config_encoded);

       feed_suid = &resampler_suid;
       feed_msgid = SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG;
       feed_payload = resampler_config_encoded;
    }
    else {
       // std_sensor_config
       sns_logv(">>> using sns_std_sensor_config <<<");
       sns_std_sensor_config std_sensor_config;
       std_sensor_config.set_sample_rate(config.target_rate);

       string std_sensor_config_encoded;
       std_sensor_config.SerializeToString(&std_sensor_config_encoded);

       feed_suid = &target_suid;
       feed_msgid = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
       feed_payload = std_sensor_config_encoded;
    }

    // threshold
    sns_threshold_config threshold_config;
    threshold_config.mutable_sensor_uid()->set_suid_high(feed_suid->high);
    threshold_config.mutable_sensor_uid()->set_suid_low(feed_suid->low);
    for (size_t i = 0; i < config.thresholds.size(); i++) {
       threshold_config.add_threshold_val( config.thresholds[i]);
    }
    threshold_config.set_threshold_type(config.threshold_type);
    threshold_config.set_payload_cfg_msg_id(feed_msgid);
    threshold_config.set_payload( feed_payload);

    string threshold_config_encoded;
    threshold_config.SerializeToString(&threshold_config_encoded);

    // client request
    sns_client_request_msg client_request_msg;
    client_request_msg.mutable_suid()->set_suid_high( threshold_suid.high);
    client_request_msg.mutable_suid()->set_suid_low( threshold_suid.low);
    client_request_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    client_request_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    client_request_msg.set_msg_id( SNS_THRESHOLD_MSGID_SNS_THRESHOLD_CONFIG);

    sns_std_request *preq = client_request_msg.mutable_request();
    preq->mutable_batching()->set_batch_period( 0);
    if ( config.batch_period >= 0) { // if supplied convert to microseconds
        uint32_t batch_period_us = ( uint32_t)( config.batch_period * 1000000);
        preq->mutable_batching()->set_batch_period( batch_period_us);
    }
    preq->set_payload( threshold_config_encoded);

    sns_logi( "%s", "start threshold sensor");

    string client_request_msg_encoded;
    client_request_msg.SerializeToString(&client_request_msg_encoded);
    connection->send_request( client_request_msg_encoded);

    useconds_t usec = (int)( config.duration * 1000000);
    cout << "sleep " << config.duration << " seconds" << endl;
    usleep( usec);

    sns_logi( "%s", "stop threshold sensor");
}

/* parse argument 0 | off | 1 | on and return bool */
bool parse_boolean( char *parg)
{
    if ( isdigit( *parg)) {
        return atoi( parg);
    }
    if ( 0 == strncmp( parg, "on",2)) {
        return true;
    }
    return false;
}

void usage( void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<data_type> -sample_rate=<hz>";
    cout << " -duration=<seconds> [-batch_period=<seconds>]" << endl;
    cout << "       [-fixed=<boolean>] [-filter=<boolean>]" << endl;
    cout << "       [-thresholds=<number>,...]";
    cout << " [-thresholds_type=relative,percent,absolute]";
    cout << " [-debug=<boolean>] [ -h | -help]" << endl;
}

static void
parse_arguments (int argc, char *argv[])
{
    config.is_valid = true;

    // command line args:
    char sensor_eq[] = "-sensor=";
    char sample_rate_eq[] = "-sample_rate=";
    char batch_period_eq[] = "-batch_period=";
    char duration_eq[] = "-duration=";
    char filter_eq[] = "-filter=";
    char fixed_eq[] = "-fixed=";
    char thresholds_eq[] = "-thresholds=";
    char thresholds_type_eq[] = "-thresholds_type=";
    char debug_eq[] = "-debug=";

    int len_sensor_eq = sizeof( sensor_eq) - 1;
    int len_sample_rate_eq = sizeof( sample_rate_eq) - 1;
    int len_batch_period_eq = sizeof( batch_period_eq) - 1;
    int len_duration_eq = sizeof( duration_eq) - 1;
    int len_filter_eq = sizeof( filter_eq) - 1;
    int len_fixed_eq = sizeof( fixed_eq) - 1;
    int len_thresholds_eq = sizeof( thresholds_eq) - 1;
    int len_thresholds_type_eq = sizeof( thresholds_type_eq) - 1;
    int len_debug_eq = sizeof( debug_eq) - 1;

    for ( int i = 1; i < argc; i++) {
        if ( 0 == strncmp( argv[i], sensor_eq, len_sensor_eq)) {
            char *psensor = argv[ i] + len_sensor_eq;
            strlcpy( config.target_data_type,
                     psensor,
                     sizeof( config.target_data_type));
        }
        else if ( 0 == strncmp( argv[i], sample_rate_eq, len_sample_rate_eq)) {
            config.target_rate = atof( argv[ i] + len_sample_rate_eq);
        }
        else if ( 0 == strncmp( argv[i], batch_period_eq, len_batch_period_eq)) {
            config.batch_period = atof( argv[ i] + len_batch_period_eq);
        }
        else if ( 0 == strncmp( argv[i], duration_eq, len_duration_eq)) {
            config.duration = atof( argv[ i] + len_duration_eq);
        }
        else if ( 0 == strncmp( argv[i], fixed_eq, len_fixed_eq)) {
            config.use_resampler = true;
            if ( parse_boolean( argv[ i] + len_fixed_eq)) {
               config.rate_type = SNS_RESAMPLER_RATE_FIXED;
            }
            else {
               config.rate_type = SNS_RESAMPLER_RATE_MINIMUM;
            }
        }
        else if ( 0 == strncmp( argv[i], filter_eq, len_filter_eq)) {
            config.use_resampler = true;
            config.filter = parse_boolean( argv[ i] + len_filter_eq);
        }
        else if ( 0 == strncmp( argv[i], thresholds_eq, len_thresholds_eq)) {
            char *thresh_csv  = argv[ i] + len_thresholds_eq;
            stringstream csv( thresh_csv);
            string element;
            while ( getline(csv, element, ',')) {
               config.thresholds.push_back(atof(element.c_str()));
            }
        }
        else if ( 0 == strncmp( argv[i], thresholds_type_eq, len_thresholds_type_eq)) {
            string thresh_type = argv[ i] + len_thresholds_type_eq;
            if ( "relative" == thresh_type) {
               config.threshold_type = SNS_THRESHOLD_TYPE_RELATIVE_VALUE;
            }
            else if ( "percent" == thresh_type) {
               config.threshold_type = SNS_THRESHOLD_TYPE_RELATIVE_PERCENT;
            }
            else if ( "absolute" == thresh_type) {
               config.threshold_type = SNS_THRESHOLD_TYPE_ABSOLUTE;
            }
            else {
               cout << "FAIL - unknown threshold type: " << argv[i] << endl;
               config.is_valid = false;
            }
        }
        else if ( 0 == strncmp( argv[i], debug_eq, len_debug_eq)) {
            config.debug = parse_boolean( argv[ i] + len_debug_eq);
        }
        else if ( 0 == strncmp( argv[i], "-help", 5)
                  ||  0 == strncmp( argv[i], "-h", 2)) {
            usage();
            exit(0);
        }
        else {
            config.is_valid = false;
            cout << "unrecognized arg " << argv[i] << endl;
        }
    }
    if ( !config.is_valid) {
        usage();
        exit(4);
    }
}

/* signal handler for graceful handling of Ctrl-C */
void signal_handler(int signum) {
   sns_loge("SIGINT received. Abort.");
   cout << "FAIL" << endl;
   exit(signum);
}

static void
event_cb(const uint8_t *data, size_t size)
{
  sns_client_event_msg pb_event_msg;

  sns_logv("Received QMI indication with length %zu", size);

  pb_event_msg.ParseFromArray(data, size);

  static int event_count = 0;
  for(int i = 0; i < pb_event_msg.events_size(); i++)
  {
    const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
//    sns_logv("event[%d] msg_id=%d, ts=%llu", i, pb_event.msg_id(),
//             (unsigned long long) pb_event.timestamp());

    auto msg_id = pb_event.msg_id();
    if ( msg_id == SNS_STD_MSGID_SNS_STD_ERROR_EVENT) {
      sns_std_error_event error;
      error.ParseFromString(pb_event.payload());

      sns_loge("Received error event %i", error.error());
    }
    else if ( msg_id == SNS_STD_MSGID_SNS_STD_ATTR_EVENT) {
       sns_logi("Received attribute event");

       unique_lock<mutex> lk(m);   // attribute found
       received++;
       cv.notify_one();
    }
    else if ( msg_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT) {
       sns_std_sensor_event sensor_event;
       sensor_event.ParseFromString( pb_event.payload());
       string s = to_string( event_count++)
                  + " " + config.target_data_type
                  + ": ts = " + to_string(pb_event.timestamp()) + "; ";
       if (sensor_event.data_size() > 2) {
          s += "a = [";
          s += to_string(sensor_event.data(0)) + ", ";
          s += to_string(sensor_event.data(1)) + ", ";
          s += to_string(sensor_event.data(2)) + "]; ";
          s += to_string(sensor_event.status());
       }
       cout << s.c_str() << endl;
    }
    else if ( msg_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT) {
        sns_std_sensor_physical_config_event physical_config_event;
        physical_config_event.ParseFromString(pb_event.payload());

        cout << "physical_config_event:" << endl;
        cout << " sample_rate: ";
        cout << physical_config_event.sample_rate() << endl;

        if ( physical_config_event.has_water_mark()) {
            cout << " watermark: ";
            cout << physical_config_event.water_mark() << endl;
        }
        if ( physical_config_event.range_size()) {
            cout << " range: ";
            for ( int i = 0; i < physical_config_event.range_size(); i++) {
                if ( i) { cout << ", ";}
                cout << physical_config_event.range(i);
            }
            cout << endl;
        }
        if ( physical_config_event.has_resolution()) {
            cout << " resolution: ";
            cout << physical_config_event.resolution() << endl;
        }
        if ( physical_config_event.has_operation_mode()) {
            cout << " operation_mode: ";
            cout << physical_config_event.operation_mode() << endl;
        }
        if ( physical_config_event.has_active_current()) {
            cout << " active_current: ";
            cout << physical_config_event.active_current() << endl;
        }
        if ( physical_config_event.has_stream_is_synchronous()) {
            cout << " stream_is_synchronous: ";
            cout << physical_config_event.stream_is_synchronous() << endl;
        }
        if ( physical_config_event.has_dri_enabled()) {
            cout << " dri_enabled: ";
            cout << physical_config_event.dri_enabled() << endl;
        }
        if ( physical_config_event.has_dae_watermark()) {
            cout << " DAE_watermark: ";
            cout << physical_config_event.dae_watermark() << endl;
        }
    }
    else if ( msg_id == SNS_STD_MSGID_SNS_STD_FLUSH_EVENT) {
       cout << "flush event" << endl;;
    }
    else if ( msg_id == SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT) {
       cout << "resampler_config_event:" << endl;

        sns_resampler_config_event resampler_config_event;
        resampler_config_event.ParseFromString( pb_event.payload());
        string s  = " ";
               s += config.target_data_type;
               s += ": ts = " + to_string(pb_event.timestamp()) + "; ";

        sns_resampler_quality quality = resampler_config_event.quality();
        if (quality == SNS_RESAMPLER_QUALITY_CURRENT_SAMPLE) {
            s += " SAMPLE";
        }
        else if (quality == SNS_RESAMPLER_QUALITY_FILTERED) {
            s += " FILTERED";
        }
        else if (quality == SNS_RESAMPLER_QUALITY_INTERPOLATED_FILTERED) {
            s += " INTERPOLATED_FILTERED";
        }
        else if (quality == SNS_RESAMPLER_QUALITY_INTERPOLATED) {
            s += " INTERPOLATED";
        }
        else {
            s += "quality unknown";
        }

        cout << s.c_str() << endl;
    }
  }
}

static void
attributes_lookup( sensor_uid &suid)
{
    sns_client_request_msg req_message;
    sns_std_attr_req attr_req;
    string attr_req_encoded;
    attr_req.SerializeToString( &attr_req_encoded);

    req_message.set_msg_id( SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    req_message.mutable_request()->set_payload( attr_req_encoded);
    req_message.mutable_suid()->set_suid_high( suid.high);
    req_message.mutable_suid()->set_suid_low( suid.low);
    req_message.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    req_message.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);

    string req_message_encoded;
    req_message.SerializeToString(&req_message_encoded);

    connection->send_request( req_message_encoded);
}

static void
suid_cb(const std::string& datatype, const std::vector<sensor_uid>& suids)
{
  sns_logv("Received SUID event for %s with length %zu",
           datatype.c_str(), suids.size());
  if(suids.size() > 0)
  {
    sensor_uid suid = suids.at(0);

    sns_logv("Received SUID %" PRIx64 "%" PRIx64 " for '%s'",
        suid.high, suid.low, datatype.c_str());

    if ("threshold" == datatype) {
       threshold_suid.high = suid.high;
       threshold_suid.low = suid.low;
    }
    else if ("resampler" == datatype) {
       resampler_suid.high = suid.high;
       resampler_suid.low = suid.low;
    }
    else {
       target_suid.high = suid.high;
       target_suid.low = suid.low;
    }

    sns_logv("attribute_lookup for'%s'",datatype.c_str());
    attributes_lookup( suid);
  }
}

int main(int argc, char* argv[])
{
    int rc = 0;
    string sysout = "PASS";

    cout << my_exe_name << " version 1.3" << endl;
    signal(SIGINT, signal_handler);

    parse_arguments( argc, argv);
    if ( config.debug) {
       sensors_log::set_tag("ssc_threshold");
       sensors_log::set_level(sensors_log::VERBOSE);
       sensors_log::set_stderr_logging(true);
    }

    try {
        connection = new ssc_connection(event_cb);

        suid_lookup lookup(suid_cb);

        requested = 2;  // requesting target and threshold
        if (config.use_resampler) {
           requested++;
           lookup.request_suid("resampler");
        }
        lookup.request_suid(config.target_data_type);
        lookup.request_suid("threshold");

        // wait for attribute lookup to complete
        auto now = std::chrono::system_clock::now();
        auto interval = 1000ms * 5; // 5 second timeout
        auto then = now + interval;

        unique_lock<mutex> lk(m);
        while ( requested > received) {
            if ( cv.wait_until(lk, then) == std::cv_status::timeout) {
               throw runtime_error( "timeout waiting for attribute lookup");
            }
        }

        if ( config.use_resampler &&
             resampler_suid.high == 0 && resampler_suid.low == 0) {
           throw runtime_error( "resampler sensor not found.");
        }
        if ( target_suid.high == 0 && target_suid.low == 0) {
           string target = config.target_data_type;
           throw runtime_error( target + " sensor not found");
        }
        if ( threshold_suid.high == 0 && threshold_suid.low == 0) {
           throw runtime_error( "threshold sensor not found.");
        }

        run();
        connection->~ssc_connection();

    } catch (runtime_error& e) {
        sns_loge("run failed: %s", e.what());
        rc = EXIT_FAILURE;
        sysout = "FAIL";
    }

    cout << sysout << endl;
    return rc;
}

