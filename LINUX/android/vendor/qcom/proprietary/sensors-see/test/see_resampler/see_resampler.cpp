/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_resampler.cpp
** DESC: run resampler sensor using command line argumentss
** ================================================================ */
#include <iostream>

#include "see_salt.h"

using namespace std;

char *my_exe_name;

/**
 * @brief parse command line argument of the form keyword=value
 * @param parg[i] - one command line argument
 * @param key[io] - gets string to left of '='
 * @param value[io] - sets string to right of '='
 * @return true - successful parse. false - parse failed.
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

struct app_config {
    bool  is_valid = true;
    char  data_type[ 32];           // type: accel, gyro, mag, ...

    bool  has_name = false;
    char  name[ 32];

    bool  has_batch_period = false;
    float batch_period;             // seconds

    bool  has_flush_period = false;
    float flush_period;             // seconds

    bool  has_flush_only = false;
    bool  flush_only;

    bool  has_max_batch = false;
    bool  max_batch;

    float sample_rate = 10.0;       // hz

    see_resampler_rate  rate_type = SEE_RESAMPLER_RATE_MINIMUM;

    bool  filter = false;

    float duration = 10.0;          // seconds

    app_config() {}

};

void usage(void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<sensor_type>";
    cout << " [-name=<name>]";
    cout << " [-sample_rate=<hz>]" << endl;
    cout << " [-rate_type=<fixed | minimum>]";
    cout << " [-filter=<0 | 1>";
    cout << " [-batch_period=<seconds>] [-flush_period=<seconds>]";
    cout << " [-max_batch=<0 | 1>] [-flush_only=<0 | 1>]";
    cout << " -duration=<seconds>" << endl;
    cout << "           where: <sensor_type> :: accel | gyro | ..." << endl;
}

app_config parse_arguments(int argc, char *argv[])
{
    app_config config;

    // command line args:
    string sensor_eq( "-sensor");
    string name_eq( "-name");
    string sample_rate_eq( "-sample_rate");
    string rate_type_eq( "-rate_type");
    string filter_eq( "-filter");
    string batch_period_eq( "-batch_period");
    string flush_period_eq( "-flush_period");
    string flush_only_eq( "-flush_only");
    string max_batch_eq( "-max_batch");
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
        if ( !get_key_value( argv[i], key, value)) {
           config.is_valid = false;
           cout << "unrecognized arg " << argv[i] << endl;
           continue;
        }
        if ( sensor_eq == key) {
           strncpy( config.data_type,
                    value.c_str(),
                    sizeof( config.data_type));
        }
        else if ( name_eq == key) {
           config.has_name = true;
           strncpy( config.name, value.c_str(), sizeof( config.name));
        }
        else if ( max_batch_eq == key) {
           config.has_max_batch = true;
           config.max_batch = atoi( value.c_str());
        }
        else if ( flush_only_eq == key) {
           config.has_flush_only = true;
           config.flush_only = atoi( value.c_str());
        }
        else if ( sample_rate_eq == key) {
           config.sample_rate = atof( value.c_str());
        }
        else if ( rate_type_eq == key) {
           if ( value == string("fixed")) {
              config.rate_type = SEE_RESAMPLER_RATE_FIXED;
           }
           else if ( value == string("minimum")) {
              config.rate_type = SEE_RESAMPLER_RATE_MINIMUM;
           }
           else {
              config.is_valid = false;
              cout << "bad rate type value: " << value << endl;
           }
        }
        else if ( filter_eq == key) {
           config.filter = atoi( value.c_str());
        }
        else if ( batch_period_eq == key) {
           config.has_batch_period = true;
           config.batch_period = atof( value.c_str());
        }
        else if ( flush_period_eq == key) {
           config.has_flush_period = true;
           config.flush_period = atof( value.c_str());
        }
        else if ( duration_eq == key) {
           config.duration = atof( value.c_str());
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

void init_std_request(see_std_request *std_request, app_config *pconfig)
{
   if ( pconfig->has_batch_period) {
      cout << "batch_period: " << to_string( pconfig->batch_period)
           << " seconds" << endl;
      std_request->set_batch_period( pconfig->batch_period * 1000000); // microsec
   }
   if ( pconfig->has_flush_period) {
      cout << "flush_period: " << to_string( pconfig->flush_period)
           << " seconds" << endl;
      std_request->set_flush_period( pconfig->flush_period * 1000000); // microsec
   }
   if ( pconfig->has_flush_only) {
      cout << "flush_only: " << to_string( pconfig->flush_only) << endl;
      std_request->set_flush_only( pconfig->flush_only);
   }
   if ( pconfig->has_max_batch) {
      cout << "max_batch: " << to_string( pconfig->max_batch) << endl;
      std_request->set_max_batch( pconfig->max_batch);
   }
}

void resample_sensor(see_salt *psalt,
                     sens_uid *resampler_suid,
                     sens_uid *target_suid,
                     app_config *pconfig)
{
   cout << "resample_sensor()" << endl;
   see_std_request std_request;
   init_std_request( &std_request, pconfig);

   see_resampler_config payload( target_suid,
                                 pconfig->sample_rate,
                                 pconfig->rate_type,
                                 pconfig->filter);

   std_request.set_payload( &payload);
   see_client_request_message request( resampler_suid,
                                       MSGID_RESAMPLER_CONFIG,
                                       &std_request);
   psalt->send_request(request);
   cout << "resample_sensor() complete" << endl;
}

void disable_sensor(see_salt *psalt, sens_uid *suid)
{
   cout << "disable_sensor()" << endl;
   see_std_request std_request;
   see_client_request_message request( suid, MSGID_DISABLE_REQ, &std_request);
   psalt->send_request(request);
   cout << "disable_sensor() complete" << endl;
}

int main(int argc, char *argv[])
{
    app_config config = parse_arguments(argc, argv);

    see_salt *psalt = see_salt::get_instance();

    psalt->begin();  // populate sensor attributes

    /*
    * lookup resampler's suid
    */
    sens_uid resampler_suid;
    string resampler = string( "resampler");
    cout << "lookup: " << resampler << endl;

    bool found_resampler = false;
    vector<sens_uid *> sens_uids;
    psalt->get_sensors( resampler, sens_uids);
    if (sens_uids.size()) {
       resampler_suid.low = sens_uids[0]->low;
       resampler_suid.high = sens_uids[0]->high;
    }
    else {
       cout << " not found" << endl;
       cout << "FAIL" << endl;
       return 4;
    }
    cout << endl;

    /*
    * lookup target sensors's suid
    */
    string target_sensor = string( config.data_type);
    cout << "lookup: " << target_sensor << endl;
    psalt->get_sensors( target_sensor, sens_uids);
    for (size_t i = 0; i < sens_uids.size(); i++) {
       sens_uid *suid = sens_uids[i];
       cout << "suid = [high " << hex << suid->high;
       cout << ", low " << hex << suid->low << "]" << endl;
    }

    if ( !sens_uids.size()) {
       cout << "not found" << endl;
       cout << "FAIL" << endl;
       return 4;
    }
    cout << endl;

    sens_uid target_suid;
    target_suid.low = sens_uids[0]->low;
    target_suid.high = sens_uids[0]->high;

    resample_sensor( psalt, &resampler_suid, &target_suid, &config);
    cout << "sleep " << to_string( config.duration) << " seconds" << endl;
    psalt->sleep( config.duration); // seconds
    disable_sensor( psalt, &resampler_suid);

    cout << "PASS" << endl;
    return 0;
}


