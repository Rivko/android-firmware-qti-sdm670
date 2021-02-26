/* ===================================================================
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: see_workhorse.cpp
** DESC: stream physical or virtual sensor @ rate for duration
** ================================================================ */
#include <iostream>
#include <time.h>

#include "see_salt.h"

using namespace std;

void sleep_and_awake( uint32_t milliseconds);

char *my_exe_name;

#define SYMBOLIC_MIN_ODR   1
#define SYMBOLIC_MAX_ODR   2

struct app_config {
    bool  is_valid = true;
    char  data_type[ 32];           // type: accel, byro, mag, ...

    bool  has_name = false;
    char  name[ 32];

    bool  has_on_change = false;
    bool  on_change;

    bool  has_batch_period = false;
    float batch_period;             // seconds

    bool  has_flush_period = false;
    float flush_period;             // seconds

    bool  has_flush_only = false;
    bool  flush_only;

    bool  has_max_batch = false;
    bool  max_batch;

    bool  has_debug = false;
    bool  debug;

    bool  has_island_log = false;
    bool  island_log;

    bool  has_memory_log = false;
    bool  memory_log;

    bool  has_usta_log = false;
    bool  usta_log;

    bool  has_calibrated = false;
    bool  calibrated;

    bool  has_flush_request_interval = false;
    float flush_request_interval;

    bool  has_distance_bound = false;
    float distance_bound = 1.0;     // meters

    bool  has_processor = false;
    see_std_client_processor_e processor = SEE_STD_CLIENT_PROCESSOR_APSS;

    bool  has_wakeup = false;
    see_client_delivery_e wakeup = SEE_CLIENT_DELIVERY_WAKEUP;

    int  times = 1;                 // number of times

    int symbolic_sample_rate = 0;
    float sample_rate = 10.0;       // hz

    float duration = 10.0;          // seconds

    app_config() {}
};

app_config config;

void init_see_std_request( see_std_request *pstd_request)
{
   if ( config.has_batch_period) {
      cout << "+ batch_period: " << to_string( config.batch_period);
      cout << " seconds" << endl;
      pstd_request->set_batch_period( config.batch_period * 1000000); // microsec
   }
   if ( config.has_flush_period) {
      cout << "+ flush_period: " << to_string( config.flush_period);
      cout << " seconds" << endl;
      pstd_request->set_flush_period( config.flush_period * 1000000); // microsec
   }
   if ( config.has_flush_only) {
      cout << "+ flush_only: " << to_string( config.flush_only) << endl;
      pstd_request->set_flush_only( config.flush_only);
   }
   if ( config.has_max_batch) {
      cout << "+ max_batch: " << to_string( config.max_batch) << endl;
      pstd_request->set_max_batch( config.max_batch);
   }
}

void init_processor_wakeup( see_client_request_message &request)
{
   if (config.has_processor) {
      request.set_processor( config.processor);
      cout << "+ processor: "
           << request.get_client_symbolic( config.processor) << endl;
   }
   if (config.has_wakeup) {
      request.set_wakeup( config.wakeup);
      cout << "+ wakeup: "
           << request.get_wakeup_symbolic( config.wakeup) << endl;
   }
}

salt_rc disable_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   cout << "disable_sensor( " << target << ")" << endl;
   see_std_request std_request;
   see_client_request_message request( suid,
                                       MSGID_DISABLE_REQ,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "disable_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc flush_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   cout << "flush_sensor( " << target << ")" << endl;
   see_std_request std_request;
   see_client_request_message request( suid,
                                       MSGID_FLUSH_REQ,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "flush_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

// return pointer to target_sensor's suid
sens_uid *get_sensor_suid( see_salt *psalt, string target)
{
    cout << "+ get_sensor_suid( " << target << ")" << endl;
    vector<sens_uid *> sens_uids;
    psalt->get_sensors( target, sens_uids);
    if ( sens_uids.size() == 0) {
       cout << "fatal suid not found" << endl;
       exit( 4);
    }
    cout << "+ " << target;
    cout << " suid = [high " << hex << sens_uids[0]->high;
    cout << ", low " << hex << sens_uids[0]->low << "]" << endl;

    return sens_uids[0];
}

salt_rc trigger_diag_sensor_log( see_salt *psalt,
                                 see_diag_trigger_log_type log_type)
{
   cout << "trigger_diag_sensor_log( " << log_type << ")" << endl;
   if ( log_type == SEE_LOG_TYPE_MEMORY_USAGE_LOG) {
      psalt->sleep(2);     // hoping ssc goes idle, as per Dilip email
   }

   uint64_t cookie = ( log_type == SEE_LOG_TYPE_MEMORY_USAGE_LOG) ? 0 : 123456789;
   cout << "+ cookie: " << dec << cookie << endl;

   string lit_diag_sensor("diag_sensor");
   sens_uid *suid = get_sensor_suid( psalt, lit_diag_sensor);
   see_diag_log_trigger_req trigger( log_type, cookie);
   see_std_request std_request;
   std_request.set_payload( &trigger);
   see_client_request_message request( suid,
                                       MSGID_DIAG_SENSOR_TRIGGER_REQ,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "trigger_diag_sensor_log() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc on_change_sensor(see_salt *psalt, sens_uid *suid, string target)
{
   cout << "on_change_sensor( " << target << ")" << endl;

   see_std_request std_request;
   init_see_std_request( &std_request);

   see_client_request_message request( suid,
                                       MSGID_ON_CHANGE_CONFIG,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "on_change_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc distance_bound_sensor( see_salt *psalt, sens_uid *suid)
{
   cout << "distance_bound_sensor()" << endl;
   see_std_request std_request;
   init_see_std_request( &std_request);

   see_set_distance_bound distance_bound( config.distance_bound); // meters
   std_request.set_payload( &distance_bound);
   see_client_request_message request( suid,
                                       MSGID_SET_DISTANCE_BOUND,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "distance_bound_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc basic_gestures_sensor( see_salt *psalt, sens_uid *suid)
{
   cout << "basic_gestures_sensor()" << endl;
   see_std_request std_request;
   init_see_std_request( &std_request);

   see_basic_gestures_config config;
   std_request.set_payload( &config);
   see_client_request_message request( suid,
                                       MSGID_BASIC_GESTURES_CONFIG,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "basic_gestures_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc multishake_sensor( see_salt *psalt, sens_uid *suid)
{
   cout << "multishake_sensor()" << endl;
   see_std_request std_request;
   init_see_std_request( &std_request);

   see_multishake_config config;
   std_request.set_payload( &config);
   see_client_request_message request( suid,
                                       MSGID_MULTISHAKE_CONFIG,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "multishake_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc psmd_sensor( see_salt *psalt, sens_uid *suid)
{
   cout << "psmd_sensor()" << endl;
   see_std_request std_request;
   init_see_std_request( &std_request);

   see_psmd_config config( SEE_PSMD_TYPE_STATIONARY);
   std_request.set_payload( &config);
   see_client_request_message request( suid,
                                       MSGID_PSMD_CONFIG,
                                       &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "psmd_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

salt_rc stream_sensor( see_salt *psalt, see_msg_id_e msg_id,
                       sens_uid *suid, string target)
{
   cout << "stream_sensor( " << target << ")" << endl;
   see_std_request std_request;
   init_see_std_request( &std_request);

   cout << "+ sample_rate: " << to_string( config.sample_rate);
   cout << " hz" << endl;
   see_std_sensor_config sample_rate( config.sample_rate); // hz
   std_request.set_payload( &sample_rate);
   see_client_request_message request( suid, msg_id, &std_request);
   init_processor_wakeup(request);
   salt_rc rc = psalt->send_request(request);
   cout << "config_stream_sensor() complete rc " << to_string(rc) << endl;
   return rc;
}

void usage( void)
{
    cout << "usage: " << my_exe_name;
    cout << " -sensor=<sensor_type>";
    cout << " [-name=<name>] [-on_change=<0 | 1>]";
    cout << endl;
    cout << "       [-sample_rate=<min | max | digits>]";
    cout << endl;
    cout << "       [-batch_period=<seconds>] [-flush_period=<seconds>]";
    cout << " [-max_batch=<0 | 1>] [-flush_only=<0 | 1>]";
    cout << endl;
    cout << "       [-flush_request_interval=<seconds>]";
    cout << endl;
    cout << "       [-usta_log=<0 | 1>] [-memory_log=<0 | 1>]";
    cout << " [-island_log=<0 | 1>]";
    cout << endl;
    cout << "       [-calibrated=<0 | 1>] [-distance_bound=<meters>]";
    cout << endl;
    cout << "       [-wakeup=<0 | 1>]";
    cout << " [-processor=<ssc | apss | adsp | mdsp | cdsp>]";
    cout << endl;
    cout << "       -duration=<seconds> [-times=<number>] [-debug=<0 | 1>]";
    cout << " [-h | -help]";
    cout << endl;
    cout << "       where: <sensor_type> :: accel | gyro | ..." << endl;
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

void parse_arguments(int argc, char *argv[])
{
    // command line args:
    string sensor_eq( "-sensor");
    string name_eq( "-name");
    string on_change_eq( "-on_change");
    string sample_rate_eq( "-sample_rate");
    string batch_period_eq( "-batch_period");
    string flush_request_interval_eq( "-flush_request_interval");
    string flush_period_eq( "-flush_period");
    string flush_only_eq( "-flush_only");
    string max_batch_eq( "-max_batch");
    string island_log_eq( "-island_log");
    string memory_log_eq( "-memory_log");
    string usta_log_eq( "-usta_log");
    string duration_eq( "-duration");
    string debug_eq( "-debug");
    string calibrated_eq( "-calibrated");
    string distance_bound_eq( "-distance_bound");
    string wakeup_eq( "-wakeup");
    string processor_eq( "-processor");
    string times_eq( "-times");

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
           else if ( name_eq == key) {
              config.has_name = true;
              strncpy( config.name, value.c_str(), sizeof( config.name));
           }
           else if ( on_change_eq == key) {
              config.has_on_change = true;
              config.on_change = atoi( value.c_str());
           }
           else if ( max_batch_eq == key) {
              config.has_max_batch = true;
              config.max_batch = atoi( value.c_str());
           }
           else if ( flush_request_interval_eq == key) {
              config.has_flush_request_interval = true;
              config.flush_request_interval = atof( value.c_str());
           }
           else if ( sample_rate_eq == key) {
              if ( value.compare( "min") == 0) {
                 config.symbolic_sample_rate = SYMBOLIC_MIN_ODR;
              }
              else if ( value.compare( "max") == 0) {
                 config.symbolic_sample_rate = SYMBOLIC_MAX_ODR;
              }
              else {
                 config.sample_rate = atof( value.c_str());
              }
           }
           else if ( batch_period_eq == key) {
              config.has_batch_period = true;
              config.batch_period = atof( value.c_str());
           }
           else if ( flush_period_eq == key) {
              config.has_flush_period = true;
              config.flush_period = atof( value.c_str());
           }
           else if ( flush_only_eq == key) {
              config.has_flush_only = true;
              config.flush_only = atoi( value.c_str());
           }
           else if ( duration_eq == key) {
              config.duration = atof( value.c_str());
           }
           else if ( debug_eq == key) {
              config.has_debug = true;
              config.debug = atoi( value.c_str());
           }
           else if ( island_log_eq == key) {
              config.has_island_log = true;
              config.island_log = atoi( value.c_str());
           }
           else if ( memory_log_eq == key) {
              config.has_memory_log = true;
              config.memory_log = atoi( value.c_str());
           }
           else if ( usta_log_eq == key) {
              config.has_usta_log = true;
              config.usta_log = atoi( value.c_str());
           }
           else if ( calibrated_eq == key) {
              config.has_calibrated = true;
              config.calibrated = atoi( value.c_str());
           }
           else if ( distance_bound_eq == key) {
              config.has_distance_bound = true;
              config.distance_bound = atof( value.c_str());
           }
           else if ( times_eq == key) {
              config.times = atoi( value.c_str());
           }
           else if ( wakeup_eq == key) {
              config.has_wakeup = true;
              config.wakeup = SEE_CLIENT_DELIVERY_WAKEUP;
              if ( 0 == atoi( value.c_str()) ) {
                 config.wakeup = SEE_CLIENT_DELIVERY_NO_WAKEUP;
              }
           }
           else if ( processor_eq == key) {
              config.has_processor = true;
              if ( value == string("ssc")) {
                 config.processor =  SEE_STD_CLIENT_PROCESSOR_SSC;
              }
              else if ( value == string("apss")) {
                 config.processor =  SEE_STD_CLIENT_PROCESSOR_APSS;
              }
              else if ( value == string("adsp")) {
                 config.processor =  SEE_STD_CLIENT_PROCESSOR_ADSP;
              }
              else if ( value == string("mdsp")) {
                 config.processor =  SEE_STD_CLIENT_PROCESSOR_MDSP;
              }
              else if ( value == string("cdsp")) {
                 config.processor =  SEE_STD_CLIENT_PROCESSOR_CDSP;
              }
              else {
                 config.is_valid = false;
                 cout << "unrecognized processor " << value << endl;
              }
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
}

void get_clock_mono( struct timespec &start)
{
    clock_gettime(CLOCK_MONOTONIC, &start);
}

void display_elapse(struct timespec &start,
                    struct timespec &end,
                    string &eyewitness)
{
    long mtime, seconds, nseconds;
    seconds  = end.tv_sec  - start.tv_sec;
    nseconds = end.tv_nsec - start.tv_nsec;
    mtime = ((seconds) * 1000 + nseconds/1000000.0);

    cout << eyewitness << dec << mtime << " milliseconds" << endl;
}

int main( int argc, char *argv[])
{
    cout << "see_workhorse version 1.21" << endl;
    parse_arguments(argc, argv);

    struct timespec start, end;
    get_clock_mono( start);

    if ( config.has_usta_log) {
       set_usta_logging( config.usta_log);   // enable or disable usta logging
    }

    see_salt *psalt = see_salt::get_instance();
    if ( config.has_debug) {
       psalt->set_debugging( config.debug);
    }
    psalt->begin();

    get_clock_mono( end);
    string begin_witness("psalt->begin() took ");
    display_elapse( start, end, begin_witness);

    /* get vector of sensor names */
    vector<string> sensor_types;
    psalt->get_sensors(sensor_types);
    cout << "get_sensors( sensor_types): " << endl;
    for (size_t i = 0; i < sensor_types.size(); i++) {
       cout << "    " << sensor_types[i] << endl;
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

    string plan_of_record = "is NOT POR.";
    if ( psensor->has_por() && psensor->get_por()) {
       plan_of_record = "is POR.";
    }
    cout << plan_of_record << endl;

    if (config.symbolic_sample_rate) {
       if ( psensor->has_rates()) {
          string lit_rate("min_odr");
          vector<float> rates;
          psensor->get_rates( rates);
          if ( config.symbolic_sample_rate == SYMBOLIC_MIN_ODR) {
             config.sample_rate = rates[ 0]; // min odr
          }
          else {
             lit_rate = "max_odr";
             config.sample_rate = rates[ rates.size() - 1]; // max odr
          }
          cout << "using " << lit_rate << " sample_rate: "
               << config.sample_rate << " hz" << endl;
       }
       else {
          cout << "symbolic sample_rate supplied, but sensor has no rates";
          cout << endl;
       }
    }
    else if ( psensor->has_max_rate()) {
       float max_rate = psensor->get_max_rate();
       cout << "has max_rate: " << dec << max_rate << " hz" << endl;
    }
    cout << endl;

    // conditionally lookup calibration control sensor
    sens_uid *calibrate_suid = NULL;
    string target_cal_control = "";
    if ( config.has_calibrated && config.calibrated) {
       if ( target_sensor == string("gyro")) {
          target_cal_control = "gyro_cal";
          calibrate_suid = get_sensor_suid( psalt, target_cal_control);
       }
       else if ( target_sensor == string("mag")) {
          target_cal_control = "mag_cal";
          calibrate_suid = get_sensor_suid( psalt, target_cal_control);
       }
    }

    salt_rc rc = SALT_RC_SUCCESS;
    int iteration = 1;
    while ( rc == SALT_RC_SUCCESS
            && iteration <= config.times) {
       cout << "iteration: " << to_string( iteration) << endl;
       iteration++;

       // conditionally trigger log memory.
       // hope used memory here == used memory at end
       if ( config.has_memory_log && config.memory_log) {
          rc = trigger_diag_sensor_log( psalt, SEE_LOG_TYPE_MEMORY_USAGE_LOG);
       }
       if ( config.has_island_log && config.island_log) {
          rc = trigger_diag_sensor_log( psalt, SEE_LOG_TYPE_ISLAND_LOG);
       }

       // conditionally request calibration
       if ( rc == SALT_RC_SUCCESS && calibrate_suid != NULL) {
          rc = on_change_sensor( psalt, calibrate_suid, target_cal_control);
       }

       // activate appropriate sensor as configured
       if ( rc == SALT_RC_SUCCESS)  {
          if ( "distance_bound" == psensor->get_type()) {
             cout << "using -distance_bound="
                  << to_string( config.distance_bound) << endl;
             rc = distance_bound_sensor( psalt, suid);
          }
          else if ( "basic_gestures" == psensor->get_type()) {
             rc = basic_gestures_sensor( psalt, suid);
          }
          else if ( "multishake" == psensor->get_type()) {
             rc = multishake_sensor( psalt, suid);
          }
          else if ( "psmd" == psensor->get_type()) {
             rc = psmd_sensor( psalt, suid);
          }
          else if ( psensor->has_stream_type()
               && ( psensor->is_stream_type_on_change()
                    || psensor->is_stream_type_single_output())) {
             rc = on_change_sensor( psalt, suid, target_sensor);
          }
          else {
             see_msg_id_e msg_id = MSGID_STD_SENSOR_CONFIG;
             rc = stream_sensor( psalt, msg_id, suid, target_sensor);
          }
       }

       if ( rc == SALT_RC_SUCCESS) {
          float duration = config.duration;
          if ( config.has_flush_request_interval) {
             while ( duration > 0.0) {
                psalt->sleep( config.flush_request_interval); // seconds
                rc = flush_sensor( psalt, suid, target_sensor);
                duration -= config.flush_request_interval;
             }
          }
          else {
             cout << "sleep( " << duration << ") seconds" << endl;
#ifdef BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE_LOCAL
             sleep_and_awake( (int)(duration * 1000));
             cout << "awoke" << endl;
#else
             psalt->sleep( (int)duration); // seconds
#endif
          }
          rc = disable_sensor( psalt, suid, target_sensor); // stop sensor
       }

       // conditionally disable calibration
       if ( rc == SALT_RC_SUCCESS && calibrate_suid != NULL) {
          rc = disable_sensor( psalt, calibrate_suid, target_cal_control);
       }

       if ( rc == SALT_RC_SUCCESS) {
          if ( config.has_island_log && config.island_log) {
             rc = trigger_diag_sensor_log( psalt, SEE_LOG_TYPE_ISLAND_LOG);
          }
          if ( config.has_memory_log && config.memory_log) {
             rc = trigger_diag_sensor_log( psalt, SEE_LOG_TYPE_MEMORY_USAGE_LOG);
          }
       }
       psalt->sleep( 1);   // hope to get disable/trigger packets logged
    } // while

    delete psalt;

    if ( rc == SALT_RC_SUCCESS) {
       cout << "PASS" << endl;
       return 0;
    }
    else {
       cout << "FAIL" << endl;
       return 4;
    }
}
