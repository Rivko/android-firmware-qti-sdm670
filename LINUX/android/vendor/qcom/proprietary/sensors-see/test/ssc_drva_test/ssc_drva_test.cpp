/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: ssc_drva_test.cpp
** DESC: Android command line application for SEE driver acceptance testing
** USAGE: -sensor=<datatype> -sample_rate=<hz value>-duration=<seconds>
**         -batch_period=<seconds> -testcase=<name>
**   where <datatype>         :: accel | gyro | mag | ... | suid
**         <hz value>         :: int or float
**         <seconds>         :: int or float
** TODO -wakeup=
**
** Revision History
** 01-27-2017   df  birth
** 03-13-2017   df  add support for event "RECALC_TIMEOUT seconds"
** 03-27-2017   df  fixups for build with sensors-see.lnx.1.0-dev
** 04-14-2017   df  code cleanup. add -debug command line arg
** 04-20-2017   df  add some comments
** 05-03-2017   df  display non ( pass, fail, recalc) events to stdout
**                  + changed to sensors_log::VERBOSE
** 05-17-2017   df  client_api_change
** ================================================================ */
#include <iostream>
#include <fstream>
#include <cinttypes>
#include <sys/file.h>

#include "sns_client.pb.h"
#include "sns_diag_sensor.pb.h"
#include "sns_da_test.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "google/protobuf/io/zero_copy_stream_impl_lite.h"

#include "sensors_log.h"
#include "ssc_connection.h"
#include "ssc_utils.h"

using namespace google::protobuf::io;
using namespace std;

static char *my_exe_name = (char *)"";
static char *newline = (char *)"\n";

/*
** result_fn file get test results message,
** either "PASS" or "FAIL <description"
*/
static char *results_fn = (char *)"/data/drva_test_result";

static const char lit_pass[] = "PASS";
static const char lit_fail[] = "FAIL";
static const char lit_recalc_timeout[] = "RECALC_TIMEOUT";
static float recalc_duration = 0;    // set by event "RECALC_TIMEOUT number"

static const int MAX_RESULT_DESCRIPTION_LEN = 128;
static char result_description[ MAX_RESULT_DESCRIPTION_LEN] = "";

static const int MAX_CMD_LINE_ARGS_LEN = 2048;
char args[ MAX_CMD_LINE_ARGS_LEN];


/**
 * @brief Class for discovering the sensor_uid for a datatype
 */
class locate
{
public:
    /**
     * @brief lookup the first sensor_uid for a given datatype
     *
     * @param datatype
     * @return sensor_uid
     */
    sensor_uid lookup( const string& datatype);

private:
    void on_suids_available(const std::string& datatype,
                            const std::vector<sensor_uid>& suids);
    std::string _datatype = "";
    sensor_uid _suid;

    bool _lookup_done = false;
    std::mutex _m;
    std::condition_variable _cv;

};
void locate::on_suids_available(const string& datatype,
                                const vector<sensor_uid>& suids)
{
    sns_logi("%u sensor(s) available for datatype %s",
             (unsigned int) suids.size(), datatype.c_str());

    _datatype = datatype;
    for ( size_t i = 0; i < suids.size(); i++) {
        _suid.low = suids[i].low;
        _suid.high = suids[i].high;

        sns_logi("datatype %s suid = [%" PRIx64 " %" PRIx64 "]",
                 datatype.c_str(), _suid.high, _suid.low);
        break;
    }

    unique_lock<mutex> lk(_m);
    _lookup_done = true;
    _cv.notify_one();

}

sensor_uid locate::lookup(const string& datatype)
{
   if ( _lookup_done) {
      return _suid;
   }

    suid_lookup lookup(
        [this](const auto& datatype, const auto& suids)
        {
            on_suids_available(datatype, suids);
        });

    lookup.request_suid( datatype);

    sns_logi("waiting for suid lookup");
    // TODO remove timeout once suid_lookup request_suid() is fixed
    auto now = std::chrono::system_clock::now();
    auto interval = 1000ms * 5; // 5 second timeout
    auto then = now + interval;

    unique_lock<mutex> lk(_m);
    while ( !_lookup_done) {
        if ( _cv.wait_until(lk, then) == std::cv_status::timeout) {
            cout << "suid not found for " << datatype << endl;
            break;
        }
    }

    sns_logi( "%s", "end suid lookup");

    return _suid;
}

/**
 * @brief Class for sending a sns_da_test_req message to the
 *        da_test sensor
 */
class da_test_runner
{
public:
    /**
     * @brief create the sns_client_request_msg carrying the
     * sns_da_test_req payload
     *
     * @param cmd_line_args character array of cmd line arguments
     * @param sensor_uid for the da_test sensor
     * @return sns_client_request_msg
     */
    sns_client_request_msg create_da_test_request(const char *cmd_line_args,
                                                  sensor_uid suid,
                                                  int32_t msg_id);
    /**
     * @brief run the da_test sensor
     *
     * @param cmd_line_args character array of cmd line arguments
     * @param test duration in seconds
     * @param sensor_uid for the da_test sensor
     */
    void runner(const char *cmd_line_args,
                double duration,
                sensor_uid suid,
                int32_t msg_id);

    /**
     * @brief create the sns_client_request_msg carrying a
     *        sns_diag_log_trigger_req
     * @param cookie - uint64_t memory log cookie
     * @param suid - sensor_uid for the diag_sensor
     */
    sns_client_request_msg create_memory_log_request( uint64_t cookie,
                                                      sensor_uid suid);

   /**
    * @brief send memory log request with cookie to diag_sensor
    * @param cookie - uint64_t memory log cookie
    * @param suid - sensor_uid for the diag_sensor
    */
    void send_memory_log_req(uint64_t cookie, sensor_uid suid);

private:
    void handle_event(const uint8_t *data, size_t size);
    std::mutex m;
    std::condition_variable cv;
    bool drva_test_done = false;
};

sns_client_request_msg
da_test_runner::create_memory_log_request( uint64_t cookie, sensor_uid suid)
{
    sns_client_request_msg req_message;
    sns_diag_log_trigger_req trigger_req;
    string trigger_req_encoded;

    /* populate trigger message */
    trigger_req.set_log_type( SNS_DIAG_TRIGGERED_LOG_TYPE_MEMORY_USAGE_LOG);
    trigger_req.set_cookie( cookie);
    trigger_req.SerializeToString(&trigger_req_encoded);

    /* populate client request message */
    req_message.mutable_suid()->set_suid_high(suid.high);
    req_message.mutable_suid()->set_suid_low(suid.low);
    req_message.set_msg_id( SNS_DIAG_SENSOR_MSGID_SNS_DIAG_LOG_TRIGGER_REQ);
    req_message.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    req_message.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    req_message.mutable_request()->set_payload(trigger_req_encoded);
    return req_message;
}

sns_client_request_msg
da_test_runner::create_da_test_request(const char *cmd_line_args,
                                       sensor_uid suid,
                                       int32_t msg_id)
{
    sns_client_request_msg req_message;
    sns_da_test_req da_test_req;
    string pb_da_test_encoded;

    /* populate driver acceptance request message */
    da_test_req.set_test_args(cmd_line_args);
    da_test_req.SerializeToString(&pb_da_test_encoded);

    /* populate client request message */
    req_message.mutable_suid()->set_suid_high(suid.high);
    req_message.mutable_suid()->set_suid_low(suid.low);
    req_message.set_msg_id( SNS_DA_TEST_MSGID_SNS_DA_TEST_REQ);
    req_message.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    req_message.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    req_message.mutable_request()->set_payload(pb_da_test_encoded);
    return req_message;
}

void da_test_runner::handle_event(const uint8_t *data, size_t size)
{
    sns_client_event_msg event_msg;
    event_msg.ParseFromArray(data, size);

    if (event_msg.events_size() < 1) {
        sns_logi("no events in message");
        return;
    }

    cout << "handle_event " << endl;

    for ( int i = 0; i < event_msg.events_size(); i++) {

       const sns_client_event_msg_sns_client_event& pb_event =
           event_msg.events(i);

       auto msg_id = pb_event.msg_id();
       if ( msg_id == SNS_DA_TEST_MSGID_SNS_DA_TEST_EVENT) {

          sns_da_test_event da_test_event;
          da_test_event.ParseFromString(pb_event.payload());

          sns_logi( "DebugString %s",da_test_event.DebugString().c_str());

          /* set result_description from da_test_event message*/
          string r = da_test_event.test_event();

          sns_logi("received event: %s", r.c_str());
          cout << "received event: " << r.c_str() << endl;

          int  len_recalc_timeout = sizeof( lit_recalc_timeout) - 1;
          if ( 0 == strncmp( r.c_str(), lit_pass, 4)
               || 0 == strncmp( r.c_str(), lit_fail, 4)
               || 0 == strncmp( r.c_str(), lit_recalc_timeout,
                                len_recalc_timeout)) {
             /*
             ** if PASS, FAIL, or RECALC_TIMEOUT
             */
             strlcpy( result_description, r.c_str(), sizeof( result_description));

             unique_lock<mutex> lk(m);
             if ( 0 == strncmp( result_description,
                                lit_recalc_timeout,
                                len_recalc_timeout)) {
                 // get number of addtional seconds from "RECALC_DURATION number\0"
                 recalc_duration = atof( result_description + len_recalc_timeout);
                 sns_logi("recalc_timeout additional seconds: %f", recalc_duration);
             }
             else {
                 recalc_duration = 0.0;
                 drva_test_done = true;
             }
             cv.notify_one();                      // PASS, FAIL, or RECALC_TIMEOUT
          }
          else {
             sns_logi("ignored event: %s", r.c_str());
             cout << "ignored event: " << r.c_str() << endl;
          }
       }
       else if ( msg_id == SNS_DA_TEST_MSGID_SNS_DA_TEST_LOG) {
          sns_da_test_log da_test_log;
          da_test_log.ParseFromString(pb_event.payload());
          cout << "DA_TEST_LOG DebugString: " << da_test_log.DebugString().c_str() << endl;
       }
       else {
          cout << "handle event ignored msg_id: " << msg_id << endl;
       }
    }
}

// send memory_log request with cookie to diag_sensor
void da_test_runner::send_memory_log_req(uint64_t cookie, sensor_uid suid)
{
    sns_logi("%s %lu", "enter send_memory_log_req", cookie);
    cout << "enter send_memory_log_req cookie: " << to_string(cookie) << endl;

    sns_client_request_msg req_message;
    req_message = create_memory_log_request(cookie, suid);

    ssc_connection ssc_conn([this](const uint8_t *data, size_t size)
    {
        handle_event(data, size);
    });

    string req_message_encoded;
    req_message.SerializeToString(&req_message_encoded);
    ssc_conn.send_request( req_message_encoded);

    sns_logi("%s", "exit send_memory_log_req");
    cout << "exit send_memory_log_req" << endl;
}

void da_test_runner::runner(const char *cmd_line_args,
                            double duration,
                            sensor_uid suid,
                            int32_t msg_id)
{
    sns_logi("%s", "enter runner");
    cout << "enter da_test runner" << endl;
    cout << cmd_line_args << endl;

    sns_client_request_msg req_message;
    req_message = create_da_test_request(cmd_line_args, suid, msg_id);

    ssc_connection ssc_conn([this](const uint8_t *data, size_t size)
    {
        handle_event(data, size);
    });

    string req_message_encoded;
    req_message.SerializeToString(&req_message_encoded);
    ssc_conn.send_request( req_message_encoded);

    /* compute when to timeout as now + test duration + cushion */
    auto cushion = 3000ms;        // android/test startup/shudown cushion
    auto now = std::chrono::system_clock::now();
    auto interval = 1000ms * duration;
    auto then = now + interval + cushion;

    sns_logi("waiting for da_test event");
    unique_lock<mutex> lk(m);
    while ( !drva_test_done) {
        if ( cv.wait_until(lk, then) == std::cv_status::timeout) {
            snprintf( result_description,
                      sizeof( result_description),
                      "FAIL %s timeout", my_exe_name);
            sns_logi("%s", result_description);
            break;
        }
        if ( recalc_duration != 0.0) {
            now = std::chrono::system_clock::now();
            interval = 1000ms * recalc_duration;
            then = now + interval + cushion;
            recalc_duration = 0.0;
        }
    }
    sns_logi("%s", "exit  runner");
}

void app_run(char *cmd_line_args, double duration)
{
    string diag_sensor_type = "diag_sensor";
    locate diagsensor;
    sensor_uid diag_sensor_suid = diagsensor.lookup( diag_sensor_type);
    if (diag_sensor_suid.high == 0 && diag_sensor_suid.low == 0) {
        throw runtime_error("diag_sensor suid not found");
    }

    string data_type = "da_test";
    locate runner;
    sensor_uid da_test_suid = runner.lookup( data_type);

    if (da_test_suid.high == 0 && da_test_suid.low == 0) {
        throw runtime_error("da_test suid not found");
    }

    struct timeval tv;
    gettimeofday( &tv, NULL);

    uint64_t cookie = (uint64_t)tv.tv_usec;

    {
       da_test_runner memory_logger;
       memory_logger.send_memory_log_req( cookie, diag_sensor_suid);
    }
    {
       da_test_runner command_line;
       command_line.runner( cmd_line_args, duration, da_test_suid,
                            SNS_DA_TEST_MSGID_SNS_DA_TEST_REQ);
    }
    {
       usleep( 2000000);   // 2 seconds : microseconds
       da_test_runner memory_logger;
       memory_logger.send_memory_log_req( cookie, diag_sensor_suid);
    }
    cout << result_description << endl;
}

/*
** append result_description to output file
** supports multiple concurrent ssc_drva_tests
*/
void write_result_description( void)
{
   int fd = open( results_fn, O_WRONLY | O_CREAT | O_APPEND, 0666);
   if ( fd >= 0) {
      if ( flock(fd, LOCK_EX) == 0) {
         size_t count = strlen( result_description);
         ssize_t len = write( fd, (const void *)result_description, count);
         write( fd, (const void *)": ", 2);
         write( fd, (const void *)args, strlen(args)); // append used command line
         write( fd, (const void *)newline, strlen(newline));
      }
      else {
         cout <<  "FAIL flock failed." << endl;
      }
      close(fd);
   }
   else {
      cout <<  "FAIL open failed for " << results_fn << endl;
   }
}

/* signal handler for graceful handling of Ctrl-C */
void signal_handler(int signum)
{
    snprintf( result_description,
             sizeof( result_description),
             "FAIL %s SIGINT received. Aborted.", my_exe_name);
    sns_logi( "%s", result_description);
    write_result_description();
    cout << result_description << endl;

    exit(signum);
}

/* pass the command line arguments to the ssc driver acceptance module */
int main(int argc, char* argv[])
{
    const char *lit_duration_eq = "-duration=";
    double duration = 0.0;            // seconds
    *args = '\0';

    signal(SIGINT, signal_handler);

    /* parse/latch command line args */
    my_exe_name = argv[0];
    cout << my_exe_name << " version 1.4" << endl;

    int lde_len = strlen( lit_duration_eq);
    for ( int i = 1; i < argc; i++) {
        if ( 0 == strcmp( argv[ i], "-debug")) {
            sensors_log::set_level(sensors_log::VERBOSE);
            sensors_log::set_stderr_logging( true);
            continue;
        }
        if ( 0 == strncmp( argv[ i], lit_duration_eq, lde_len)) {
            duration = atof( argv[ i] + lde_len);
        }
        /* concatenate argv[] to reconstruct command line */
        if ( strlcat( args, argv[ i], sizeof( args)) >= sizeof( args)
             || strlcat( args, " ", sizeof( args)) >= sizeof( args)) {
            snprintf( result_description,
                      sizeof( result_description),
                      "FAIL %s command line args too long. Limit %d chars.",
                      my_exe_name,
                      MAX_CMD_LINE_ARGS_LEN);

            sns_logi("%s", result_description);
            write_result_description();
            exit( EXIT_FAILURE);
        }
    }
    cout << my_exe_name << " " << args << endl;

    int rc = 0;
    try {
        size_t len = strlen( args);
        if ( len >=  1) {
           args[len - 1] = '\0';
        }
        app_run( args, duration);
    } catch ( exception& e) {
        snprintf( result_description,
                  sizeof( result_description),
                  "FAIL %s caught exception: %s", my_exe_name, e.what());
        sns_loge( "%s", result_description);
        rc = EXIT_FAILURE;
    }

    write_result_description();
    return rc;
}
