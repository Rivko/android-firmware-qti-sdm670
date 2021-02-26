/* ===================================================================
** Copyright (c) 2017 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_conc_horse.cpp
** DESC: stream a single sensor concurrently
** ================================================================ */
#include <iostream>
#include "see_salt.h"

using namespace std;

char *my_exe_name;

salt_rc resample_sensor(see_salt *psalt,
                        sens_uid *resampler_suid,
                        sens_uid *target_suid,
                        float resample_rate)
{
   cout << "resample_sensor() @ " << to_string(resample_rate) << " hz" << endl;
   see_std_request std_request;

   see_resampler_config payload( target_suid,
                                 resample_rate,
                                 SEE_RESAMPLER_RATE_FIXED,
                                 true);

   std_request.set_payload( &payload);
   see_client_request_message request( resampler_suid,
                                       MSGID_RESAMPLER_CONFIG,
                                       &std_request);
   salt_rc rc = psalt->send_request(request);
   cout << "resample_sensor() complete rc " << to_string( rc) << endl;
   return rc;
}

salt_rc stream_sensor( see_salt *psalt,
                       sens_uid *suid,
                       float rate)
{
   cout << "stream_sensor() @ " << to_string( rate) << " hz"  << endl;
   see_std_sensor_config sample_rate( rate); // hz
   see_std_request std_request;
   std_request.set_payload( &sample_rate);
   see_client_request_message request( suid,
                                       MSGID_STD_SENSOR_CONFIG,
                                       &std_request);
   salt_rc rc = psalt->send_request(request);
   cout << "stream sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc disable_sensor(see_salt *psalt, sens_uid *suid)
{
   cout << "disable_sensor()" << endl;
   see_std_request std_request;
   see_client_request_message request( suid,
                                       MSGID_DISABLE_REQ,
                                       &std_request);
   salt_rc rc = psalt->send_request(request);
   cout << "disable_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

struct app_config {
    bool  is_valid = true;
    char  data_type[ 32];           // type: accel, byro, mag, ...

    bool has_sample_rates = false;
    string sample_rates;

    bool  has_resample = false;
    bool  resample;

    bool  has_debug = false;
    bool  debug;

    float duration = 10.0;          // seconds

    app_config() {}

};


void usage( void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<sensor_type>";
    cout << " [-sample_rates=<hz,...>]";
    cout << " [-resample=<0 | 1>]";
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


bool get_next_csv(const char *parg, string &next, string &tail)
{
   const char *pkey = parg;

   while ( char c = *parg) {
      if ( c == ',') {
         next = string( pkey, parg - pkey);
         tail = string( ++parg);
         return true;
      }
      parg++;
   }
   if ( pkey != parg) {
      next = string( pkey, parg - pkey);
      tail = "";
      return true;
   }
   return false;
}


app_config parse_arguments(int argc, char *argv[])
{
    app_config config;

    // command line args:
    string sensor_eq( "-sensor");
    string sample_rates_eq( "-sample_rates");
    string resample_eq( "-resample");
    string duration_eq( "-duration");
    string debug_eq( "-debug");

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
              strncpy( config.data_type, value.c_str(),
                       sizeof( config.data_type));
           }
           else if ( sample_rates_eq == key) {
              config.has_sample_rates = true;
              config.sample_rates = value;

              const char *rate_spec = value.c_str();
              while ( char c = *rate_spec) {
                 rate_spec++;
                 if ( isdigit(c) || c == ',') {
                    continue;
                 }
                 cout << "invalid csv sample_rates " << value << endl;
                 config.is_valid = false;
                 break;
              }
           }
           else if ( duration_eq == key) {
              config.duration = atof( value.c_str());
           }
           else if ( resample_eq == key) {
              config.has_resample = true;
              config.resample = atoi( value.c_str());
           }
           else if ( debug_eq == key) {
              config.has_debug = true;
              config.debug = atoi( value.c_str());
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

int main( int argc, char *argv[])
{
    app_config config = parse_arguments(argc, argv);
    cout << my_exe_name << " version 1.1" << endl;

    see_salt *psalt = see_salt::get_instance();
    if ( config.has_debug) {
       psalt->set_debugging( config.debug);
    }
    psalt->begin();

    /*
    * lookup resampler's suid
    */
    sens_uid resampler_suid;
    bool found_resampler = false;
    string resampler = string( "resampler");
    if ( config.has_resample && config.resample) {
       cout << "lookup: " << resampler << endl;

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
    }
    cout << endl;

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

    /* use the first suid for this target_sensor */
    sens_uid *suid = sens_uids[0];
    sensor *psensor = psalt->get_sensor( suid);
    /*
    * set resample rates from -sample_rates=...
    * or from psensor->get_rates()
    */
    std::vector<float> rates;
    if ( config.has_sample_rates) {
       const char *prate_spec = config.sample_rates.c_str();
       string next, tail;
       while ( get_next_csv( prate_spec, next, tail)) {
          rates.push_back( atof( next.c_str()));
          prate_spec = (const char *)tail.c_str();
       }
    }
    else {
       if ( !psensor->has_rates()) {
          cout << " missing rates attribute" << endl;
          cout << "FAIL" << endl;
          return 4;
       }
       psensor->get_rates( rates);     // use ODR rates
    }

    if ( rates.size()) {
       vector< see_salt *> clients;
       clients.push_back( psalt);

       /* startup additional clients as necessary: one client for each rate */
       for ( size_t i = 1; i < rates.size(); i++) {
          see_salt *pclient = see_salt::get_instance();
          if ( config.has_debug) {
             pclient->set_debugging( config.debug);
          }

          cout << " = ==== == ======== ===== =======" << endl;
          cout << " client: " << to_string(pclient->get_instance_num());
          cout << " for " << to_string( rates[ i]) << " hz" << endl;
          pclient->begin();
          clients.push_back( pclient);
       }
       // startup streams
       cout << " - ---- -- -------- ----- -------" << endl;
       salt_rc rc = SALT_RC_SUCCESS;
       for ( size_t i = 0; i < clients.size(); i++) {
          see_salt *pclient = clients[ i];
          float sample_rate = rates[ i];
          cout << "start client: " << to_string(pclient->get_instance_num()) << " ";
          cout << " at " << to_string( sample_rate) << " hz";

          if ( config.has_resample && config.resample) {
             cout << " resample" << endl;
             rc = resample_sensor( pclient, &resampler_suid, suid, sample_rate);
          }
          else {
             cout << endl;
             rc = stream_sensor( pclient, suid, sample_rate);
          }
          if ( rc) {
             break;
          }
       }
       cout << " launch complete rc " << to_string( rc) << endl;
       if ( rc == SALT_RC_SUCCESS) {
          psalt->sleep( config.duration); // seconds
          cout << " - ---- -- -------- ----- -------" << endl;
          for ( size_t i = 0; i < clients.size(); i++) {
             see_salt *pclient = clients[ i];
             if ( config.has_resample && config.resample) {
                cout << "stop resampler client: " << to_string(pclient->get_instance_num()) << " ";
                rc = disable_sensor( pclient, &resampler_suid); // stop resampler
             }
             else {
                cout << "stop client: " << to_string(pclient->get_instance_num()) << " ";
                rc = disable_sensor( pclient, suid); // stop sensor
             }
             if ( rc) {
                break;
             }
          }
       }
       cout << " disable complete rc " << to_string( rc) << endl;
       if ( rc == SALT_RC_SUCCESS) {
          psalt->sleep( 1); // hope to let disable get logged
          cout << "PASS" << endl;
       } else {
          cout << "FAIL" << endl;
       }
    }
    return 0;
}
