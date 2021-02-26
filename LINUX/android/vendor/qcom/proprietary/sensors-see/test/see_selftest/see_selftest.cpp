/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_selftest.cpp
** DESC: physical sensor self test
** ================================================================ */
#include <iostream>
#include "see_salt.h"

using namespace std;

char *my_exe_name;

void self_test_sensor(see_salt *psalt,
                      sens_uid *suid,
                      see_self_test_type_e testtype)
{
   see_self_test_config payload( testtype);
   cout << "self_test_sensor() testtype " << to_string( testtype);
   cout << " - " << payload.get_test_type_symbolic( testtype) << endl;

   see_std_request std_request;
   std_request.set_payload( &payload);
   see_client_request_message request( suid,
                                       MSGID_SELF_TEST_CONFIG,
                                       &std_request);
   psalt->send_request(request);
   cout << "self_test_sensor()" << endl;
}

struct app_config {
    bool  is_valid = true;
    char  data_type[ 32];           // type: accel, byro, mag, ...

    bool  has_name = false;
    char  name[ 32];

    bool  has_on_change = false;
    bool  on_change;

    bool  has_testtype = false;
    int   testtype = SELF_TEST_TYPE_FACTORY;

    float duration = 5.0;          // seconds

    app_config() {}

};

void usage(void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<sensor_type>";
    cout << " -testtype=<number>";
    cout << " [-name=<name> [-on_change=<0 | 1>]";
    cout << " -duration=<seconds>" << endl;
    cout << "           where: <sensor_type> :: accel | gyro | ..." << endl;
}

/**
 * @brief parse command line argument of the form keyword=value
 * @param parg[i] - command line argument
 * @param key[io] - gets string to left of '='
 * @param value[io] - sets string to right of '='
 */
bool get_key_value(char *parg, string &key, string &value)
{
   char *pkey = parg;

   while ( char c = *parg) {
      if ( c == '=') {
         key = string( pkey, parg - pkey);
         value = string( ++parg);
         return true;
      }
      parg++;
   }
   return false;
}

app_config parse_arguments(int argc, char *argv[])
{
    app_config config;

    // command line args:
    string sensor_eq( "-sensor");
    string name_eq( "-name");
    string on_change_eq( "-on_change");
    string testtype_eq( "-testtype");
    string duration_eq( "-duration");

    my_exe_name = argv[0];
    for ( int i = 1; i < argc; i++) {
        if (( 0 == strcmp( argv[i], "-h"))
              || ( 0 == strcmp( argv[i], "-help"))) {
           config.is_valid = false;
           break;
        }
        string key;
        string value;
        if ( get_key_value( argv[i], key, value)) {
           if ( sensor_eq == key) {
              strncpy( config.data_type,
                       value.c_str(),
                       sizeof( config.data_type));
           }
           else if ( name_eq == key) {
              config.has_name = true;
              strncpy( config.name,
                       value.c_str(),
                       sizeof( config.name));
           }
           else if ( on_change_eq == key) {
              config.has_on_change = true;
              config.on_change = atoi( value.c_str());
           }
           else if ( testtype_eq == key) {
              config.has_testtype = true;
              config.testtype = atoi( value.c_str());
           }
           else if ( duration_eq == key) {
              config.duration = atof( value.c_str());
           }
           else {
               config.is_valid = false;
               cout << "unrecognized arg " << argv[i] << endl;
           }
        }
        else {
            config.is_valid = false;
            cout << "unrecognized arg " << argv[i] << endl;
        }
    }
    if ( !config.is_valid) {
       usage();
       exit( 4);
    }

    return config;
}

int main(int argc, char *argv[])
{
    app_config config = parse_arguments(argc, argv);

    see_salt *psalt = see_salt::get_instance();
    psalt->begin();

    /* get vector of all suids for target sensor */
    vector<sens_uid *> sens_uids;
    string target_sensor = string( config.data_type);
    cout << "lookup: " << target_sensor << endl;
    psalt->get_sensors( target_sensor, sens_uids);
    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *suid = sens_uids[i];
       cout << "suid = [high " << hex << suid->high;
       cout << ", low " << hex << suid->low << "]" << endl;
    }

    if ( 0 == sens_uids.size()) {
       cout << "not found" << endl;
       cout << "FAIL" << endl;
       return 4;
    }
    cout << endl;

    /* choose target_suid based on type, name, on_change */
    sens_uid *suid = NULL;
    sensor *psensor = NULL;

    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *candidate_suid = sens_uids[i];
       psensor = psalt->get_sensor( candidate_suid);
       if ( config.has_name) {
          string name = psensor->get_name();
          if ( 0 != strcmp( config.name, name.c_str())) {
             continue;
          }
       }
       if ( config.has_on_change) {
          if ( !psensor->has_stream_type()) {
             continue;
          }
          if ( config.on_change != psensor->is_stream_type_on_change()) {
             continue;
          }
       }
       suid = candidate_suid;    // found target
       break;
    }

    cout << endl;
    cout << "-sensor=" << config.data_type << " ";
    if ( config.has_name) {
       cout << "-name=" << config.name << " ";
    }
    if ( config.has_on_change) {
       cout << "-on_change=" << to_string(config.on_change) << " ";
    }
    if ( suid == NULL) {
       cout << "not found" << endl;
       cout << "FAIL" << endl;
       return 4;
    }

    cout << "found" << endl;
    cout << "suid = [high " << hex << suid->high;
    cout << ", low " << hex << suid->low << "]" << endl;

    if ( !config.has_testtype) {
       config.testtype = SELF_TEST_TYPE_FACTORY;
    }

    self_test_sensor( psalt, suid,
                      (see_self_test_type_e)config.testtype);
    cout << "sleep " << config.duration << " seconds" << endl;
    psalt->sleep( config.duration); // seconds

    cout << "PASS" << endl;
    return 0;
}
