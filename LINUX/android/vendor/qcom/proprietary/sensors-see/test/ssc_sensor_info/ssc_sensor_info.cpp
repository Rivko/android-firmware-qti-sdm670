/* ===================================================================
** Copyright (c) 2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: ssc_sensor_info.cpp
** DESC: Android command line application to list available suids
**       and attributes per suid
** ================================================================ */
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <cinttypes>

#include "sns_client.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"

#include "google/protobuf/io/zero_copy_stream_impl_lite.h"
#include "ssc_connection.h"
#include "sensors_log.h"
#include "ssc_utils.h"

using namespace google::protobuf::io;
using namespace std;

/*
** result_fn file get test results message,
** either "PASS" or "FAIL <description"
*/
string results_fn = "/data/sensors/drva_test_result";

const int MAX_SENSOR_NAME_LEN = 20;
const int MAX_RESULT_DESCRIPTION_LEN = 128;
char result_description[ MAX_RESULT_DESCRIPTION_LEN];

typedef struct {
    string      data_type;
    sensor_uid  suid;
} dtuid_pair;

std::vector<dtuid_pair> pairs;

// //////// ///// /////// / ////
// //////// ///// /////// / ////
// //////// ///// /////// / ////
class locate
{
public:
    sensor_uid lookup( const string& datatype);
private:
    void on_suids_available(const std::string& datatype,
                            const std::vector<sensor_uid>& suids);
//    std::string _datatype;
//    std::vector<sensor_uid>& _suids;

    sensor_uid _suid;   //

    bool _lookup_done;
    std::mutex _m;
    std::condition_variable _cv;

};
void locate::on_suids_available(const string& datatype,
                                const vector<sensor_uid>& suids)
{
    sns_logi("%u sensor(s) available for datatype %s",
             (unsigned int) suids.size(), datatype.c_str());

    for ( size_t i = 0; i < suids.size(); i++) {

        dtuid_pair element;
        element.data_type = datatype;
        element.suid.high = suids[i].high;
        element.suid.low = suids[i].low;
        pairs.push_back( element);

        sns_logi("datatype %s suid = [%" PRIx64 " %" PRIx64 "]",
                 datatype.c_str(), element.suid.high, element.suid.low);
    }

    unique_lock<mutex> lk(_m);
    _lookup_done = true;
    _cv.notify_one();

}

sensor_uid locate::lookup(const string& datatype)
{

    _lookup_done = false;

    suid_lookup lookup(
        [this](const auto& datatype, const auto& suids)
        {
            on_suids_available(datatype, suids);
        });

    lookup.request_suid( datatype);

    sns_logi("waiting for suid lookup");
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

// //////// ///// /////// / ////
// //////// ///// /////// / ////
// //////// ///// /////// / ////

class sensor_attributes {
public:
    const sns_std_attr_value& get(int32_t attr_id) {
        return _attr_map.at(attr_id);
    }

    void put( int32_t attr_id, const sns_std_attr_value& attr_value) {
        _attr_map[ attr_id] = attr_value;
    }

    string attr_to_string( int32_t attr_id,
                           const sns_std_attr_value& attr_value);

    const string& get_name( int32_t attr_id) {
       return _attr_id_to_name.at( attr_id);
    }
    const string& get_id( string name);

    void display();

sensor_attributes() {
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_NAME          ] = "NAME          ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_VENDOR        ] = "VENDOR        ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_TYPE          ] = "TYPE          ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_AVAILABLE     ] = "AVAILABLE     ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_VERSION       ] = "VERSION       ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_API           ] = "API           ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_RATES         ] = "RATES         ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_RESOLUTIONS   ] = "RESOLUTIONS   ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_FIFO_SIZE     ] = "FIFO_SIZE     ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT] = "ACTIVE_CURRENT";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT ] = "SLEEP_CURRENT ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_RANGES        ] = "RANGES        ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_OP_MODES      ] = "OP_MODES      ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_DRI           ] = "DRI           ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_STREAM_SYNC   ] = "STREAM_SYNC   ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_EVENT_SIZE    ] = "EVENT_SIZE    ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_STREAM_TYPE   ] = "STREAM_TYPE   ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_DYNAMIC       ] = "DYNAMIC       ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_HW_ID         ] = "HW_ID         ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_RIGID_BODY    ] = "RIGID_BODY    ";
    _attr_id_to_name[SNS_STD_SENSOR_ATTRID_PLACEMENT     ] = "PLACEMENT     ";

    _display_order.push_back( SNS_STD_SENSOR_ATTRID_NAME);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_VENDOR);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_TYPE);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_AVAILABLE);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_VERSION);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_API);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_RATES);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_RESOLUTIONS);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_RANGES);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_DRI);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_FIFO_SIZE);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_STREAM_TYPE);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_STREAM_SYNC);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_DYNAMIC);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_EVENT_SIZE);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_OP_MODES);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_HW_ID);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_RIGID_BODY);
    _display_order.push_back( SNS_STD_SENSOR_ATTRID_PLACEMENT);
}

private:
    std::map<int32_t, string> _attr_id_to_name;
    std::unordered_map<int32_t, sns_std_attr_value> _attr_map;
    std::vector<int32_t> _display_order;
};

/* remove leading and trailing whitespace */
std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto str_begin = str.find_first_not_of(whitespace);
    if (str_begin == std::string::npos)
        return ""; // all blanks

    const auto str_end = str.find_last_not_of(whitespace);
    const auto str_len = str_end - str_begin;

    return str.substr(str_begin, str_len);
}

/* returns string representation of attr_value */
string sensor_attributes::attr_to_string( int32_t attr_id,
                                          const sns_std_attr_value& attr_value)
{
    auto count = attr_value.values_size();

    string s;
    try {
       s = get_name( attr_id);   // get attribute name
    }
    catch ( std::out_of_range) {
       s = "unknown id(" + to_string(attr_id) + ")"; // attribute name not known
    }
    s += " = ";

    if ( count) {
        if ( count > 1) { s += "["; }

        for (int i=0; i < count; i++) {
            if ( i) { s += ", ";}
            if (attr_value.values(i).has_str()) {
                s += trim(attr_value.values(i).str());;
            }
            else if (attr_value.values(i).has_sint()) {
                int v = attr_value.values(i).sint();
                if ( attr_id == SNS_STD_SENSOR_ATTRID_AVAILABLE
                     || attr_id == SNS_STD_SENSOR_ATTRID_DYNAMIC) {
                    s += (v) ? "true" : "false";
                }
                else if ( attr_id == SNS_STD_SENSOR_ATTRID_VERSION) {
                    uint16_t minor = v & 0xff;
                    uint16_t major = ( v & 0xff00) >> 8;
                    s += to_string( major) + "." + to_string( minor);
                }
                else if ( attr_id == SNS_STD_SENSOR_ATTRID_STREAM_TYPE) {
                    if ( v == SNS_STD_SENSOR_STREAM_TYPE_STREAMING) {
                        s += "streaming";
                    }
                    else if ( v == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) {
                        s += "on_change";
                    }
                    else if ( v == SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT) {
                        s += "single_output";
                    }
                    else {
                        s += "unknown";
                    }
                }
                else if ( attr_id == SNS_STD_SENSOR_ATTRID_RIGID_BODY) {
                    if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY) {
                        s += "display";
                    }
                    else if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_KEYBOARD) {
                        s += "keyboard";
                    }
                    else if ( v == SNS_STD_SENSOR_RIGID_BODY_TYPE_EXTERNAL) {
                        s += "external";
                    }
                    else {
                        s += "unknown";
                    }
                }
                else {
                    s += to_string( v);
                }
            }
            else if (attr_value.values(i).has_flt()) {
                s += to_string( attr_value.values(i).flt());
            }
            else if (attr_value.values(i).has_boolean()) {
                int v = attr_value.values(i).boolean();
                s += (v) ? "true" : "false";
            }
            else if (attr_value.values(i).has_subtype()) {
                // assert subtype used only for ranges of float.
                sns_std_attr_value subtype = attr_value.values(i).subtype();
                s += "[";
                for ( int j=0; j < subtype.values_size(); j++) {
                    if ( j) { s += ","; }
                    if (subtype.values(j).has_flt()) {
                        s += to_string( subtype.values(j).flt());
                    }
                    else {
                        s += "!flt";
                    }
                }
                s += "]";
            }
            else {
                s += "!!! unknown value type !!!";
            }
        }
        if ( count > 1) { s += "]"; }
    }

    return s;
}

/* display attributes using determinstic order */
void sensor_attributes::display()
{
    for ( size_t i=0; i < _display_order.size(); i++) {
        int32_t attr_id = _display_order[ i];

        auto it = _attr_map.find( attr_id);
        if ( it != _attr_map.end()) {
            const sns_std_attr_value& attr_value = get( attr_id);
            string sout = attr_to_string( attr_id, attr_value);
            cout << sout << endl;
        }
    }
    // handle new unknown attributes
    int32_t attr_id = _display_order.size();
    int32_t done_id = _attr_map.size();
    while ( attr_id < done_id) {
       const sns_std_attr_value& attr_value = get( attr_id);
       string sout = attr_to_string( attr_id, attr_value);
       cout << sout << endl;
       attr_id++;
    }
}

// //////// ///// /////// / ////

class do_attrib_lookup
{
public:
    void lookup_attrib( string data_type, sensor_uid &suid);
private:
    void handle_attrib_event(const uint8_t *data, size_t size);
    std::mutex _m;
    std::condition_variable _cv;
    std::string _data_type;
    sensor_uid _suid;
    bool _lookup_done = false;
    sensor_attributes _attributes;
};

void do_attrib_lookup::lookup_attrib( string data_type, sensor_uid &suid)
{
    sns_logi( "%s", "begin lookup_attrib");

    _data_type = data_type;
    _suid.high = suid.high;
    _suid.low = suid.low;

    ssc_connection client([this](const uint8_t *data, size_t size)
    {
        handle_attrib_event(data, size);
    });

    sns_logi("lookup sensor = %s suid = [%" PRIx64 " %" PRIx64 "]",
             data_type.c_str(), suid.high, suid.low);

    sns_client_request_msg req_message;
    sns_std_attr_req attr_req;
    string attr_req_encoded;
    attr_req.SerializeToString( &attr_req_encoded);

    /* populate the client request message */
    req_message.set_msg_id( SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    req_message.mutable_request()->set_payload( attr_req_encoded);
    req_message.mutable_suid()->set_suid_high( suid.high);
    req_message.mutable_suid()->set_suid_low( suid.low);
    req_message.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    req_message.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);

    _lookup_done = false;
    string req_message_encoded;
    req_message.SerializeToString(&req_message_encoded);
    client.send_request( req_message_encoded);

    sns_logi( "waiting for attr lookup");
    unique_lock<mutex> lk(_m);
    auto now = std::chrono::system_clock::now();
    auto then = now + 100ms;    // timeout after 100ms
    while (! _lookup_done) {
        if ( _cv.wait_until(lk, then) == std::cv_status::timeout) {
            sns_logi( "%s", "attr lookup timed out");
            break;
        }
    }
    sns_logi( "%s", "end lookup_attrib\n");
}

// //////// ///// /////// / ////
// //////// ///// /////// / ////
// //////// ///// /////// / ////

void do_attrib_lookup::handle_attrib_event(const uint8_t *data, size_t size)
{
    /* sns_logi( "cb: %s",  event_msg.DebugString().c_str()); */

    sns_client_event_msg event_msg;
    event_msg.ParseFromArray(data, size);

    if (event_msg.events_size() < 1) {
        sns_logi("no events in message");
        return;
    }
    const sns_client_event_msg_sns_client_event& pb_event =
        event_msg.events(0);

    sns_std_attr_event pb_attr_event;
    pb_attr_event.ParseFromString(pb_event.payload());
    /*
    sns_logi( "datatype=%s attributes = %s", _data_type.c_str(),
        pb_attr_event.DebugString().c_str());
    */

    for (int i=0; i < pb_attr_event.attributes_size(); i++) {
        int32_t attr_id = pb_attr_event.attributes(i).attr_id();
        const sns_std_attr_value& attr_value = pb_attr_event.attributes(i).value();

        _attributes.put( attr_id, attr_value);
    }

    cout << endl;   // blank line separator
    _attributes.display();

    unique_lock<mutex> lk(_m);
    _lookup_done = true;
    _cv.notify_one();
}

void get_sensor_info( char *target_sensor)
{
    string target = target_sensor;

    locate sensors;
    sensors.lookup( target);

    for ( size_t i = 0; i < pairs.size(); i++) {
        sns_logi( "get_sensor_info %d", (int)i);
        do_attrib_lookup attribes;
        attribes.lookup_attrib( pairs[ i].data_type, pairs[ i].suid);
    }
}

/* write result_description to output file*/
void write_result_description( void)
{
    ofstream out_file;
    out_file.open( results_fn);
    out_file << result_description;
    out_file.close();
}

/* signal handler for graceful handling of Ctrl-C */
void signal_handler(int signum)
{
    strncpy( result_description,
             "fail SIGINT received. Aborted",
             sizeof( result_description));
    sns_logi( "%s", result_description);
    write_result_description();
    exit(signum);
}
void usage( void){
    cout << "usage: ssc_sensor_info [ -sensor=name] [-help]" << endl;
    cout << "       where name: accel | gyro | mag | pressure | ..." << endl;
}

/* get/display attributes for one or all sensors */
int main(int argc, char* argv[])
{
    char target_sensor[ MAX_SENSOR_NAME_LEN];
    target_sensor[ 0] = '\0';

    const char lit_sensor_eq[] = "-sensor=";
    int lseq_len = strlen( lit_sensor_eq);

    signal(SIGINT, signal_handler);


    /* parse command line arg */
    for ( int i = 1; i < argc; i++) {
        if ( 0 == strncmp( argv[ i], lit_sensor_eq, lseq_len)) {
            strlcpy( target_sensor,
                     argv[ i] + lseq_len,
                     sizeof( target_sensor));
        }
        else if (( 0 == strcmp( argv[i], "-h"))
                 || ( 0 == strcmp( argv[i], "-help"))) {
            usage();
            exit( 0);
        }
        else if ( 0 == strcmp( argv[i], "-debug")) {
            sensors_log::set_level(sensors_log::VERBOSE);
            sensors_log::set_stderr_logging( true);
        }
        else {
            cout << "unrecognized arg: " << argv[i] << endl;
            usage();
            exit( EXIT_FAILURE);
        }
    }

    int rc = 0;
    try {
        get_sensor_info( target_sensor);
    } catch (runtime_error& e) {
        snprintf( result_description,
                  sizeof( result_description),
                  "fail caught runtime_error %s", e.what());
        sns_logi( "%s", result_description);
        rc = EXIT_FAILURE;
    }

    write_result_description();
    return rc;
}
