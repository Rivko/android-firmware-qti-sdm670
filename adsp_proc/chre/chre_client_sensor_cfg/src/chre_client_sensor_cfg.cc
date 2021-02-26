/*==============================================================================
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "chre/util/nanoapp/app_id.h"
#include "chre_host/host_protocol_host.h"
#include "chre_host/fragmented_load_transaction.h"
#include "chre_host/log.h"
#include "chre_host/socket_client.h"

#include <chre/sensor.h>
#include <chre/wifi.h>
#include <chre/gnss.h>
#include <chre/qc_ble.h>
#include <chre/qc_geofence.h>
#include <chre/wwan.h>

#include <inttypes.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/eventfd.h>

#include <fstream>
#include <thread>

#include <cutils/sockets.h>
#include <utils/StrongPointer.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include "cellinfo_tools.h"

using namespace qcgf;

#define FREEIF(_pv) \
    do { \
        if(_pv) { \
            void* _tmp = (void*)(_pv); \
            (_pv) = 0;\
            free(_tmp); \
        } \
    } while(0)

#define FCLOSEIF(_fd) \
    do { \
        if(_fd) { \
            void* _tmp = (void*)(_fd); \
            (_fd) = 0;\
            fclose(_tmp); \
        } \
    } while(0)

#define APP_ID_VENDOR_QUALCOMM UINT64_C(0x2051434f4d) // " QCOM"
#define APP_SEQ_ID_ANY UINT64_C(0xFFFFFF)
#define APP_ID_MAKE(vendor, app) ((((uint64_t)(vendor)) << 24) | ((app) & APP_SEQ_ID_ANY))

#define CHRE_APP_SENSOR_CFG_FILE "chre_app_sensor_cfg.so"
#define CHRE_APP_SENSOR_CFG_ID APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8)

#define EVENT_SENSOR_CFG    0
#define EVENT_GF_CFG        1

#define GF_ARRAY_SIZE       5
#define GF_BIG_ARRAY_SIZE   20

enum GeofenceCmds {
    GEOFENCE_CMD_INIT   = 0,
    GEOFENCE_CMD_ADD    = 1,
    GEOFENCE_CMD_REMOVE = 2,
    GEOFENCE_CMD_MODIFY = 3,
    GEOFENCE_CMD_PAUSE  = 4,
    GEOFENCE_CMD_RESUME = 5,
    GEOFENCE_CMD_PRINT  = 6,
    GEOFENCE_CMD_DEINIT = 7,
    GEOFENCE_CMD_EXIT   = 8
};

struct GeofenceCmd {
  uint32_t cmd; // enum GeofenceCmds
  uint32_t gfIdArray[GF_ARRAY_SIZE];
  qcGeofenceDataType gfDataArray[GF_ARRAY_SIZE];
  qcGeofenceOptionsType gfOptionsArray[GF_ARRAY_SIZE];
  uint32_t gfCount;
};

struct ActiveGeofences {
    uint32_t gfIdArray[GF_BIG_ARRAY_SIZE];
    qcGeofenceDataType gfDataArray[GF_BIG_ARRAY_SIZE];
    qcGeofenceOptionsType gfOptionsArray[GF_BIG_ARRAY_SIZE];
    uint8_t gfActiveArray[GF_BIG_ARRAY_SIZE];
};

static bool gfCmdInProgress = false;
static uint32_t lastGfCmd = GEOFENCE_CMD_INIT;
static uint8_t activeGfCount = 0;
struct GeofenceCmd currentGfCmd = { 0 };
static struct ActiveGeofences activeGFs = { 0 };

struct ConfigCmd {
  uint64_t latency;
  uint64_t interval;
  uint32_t sensType;
  uint32_t mode;
  uint32_t flags;
};

#define CHRE_WIFI_TYPE_ACTIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 1)
#define CHRE_WIFI_TYPE_PASSIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 2)
#define CHRE_GNSS_TYPE_LOCATION (CHRE_EVENT_SENSOR_LAST_EVENT + 3)
#define CHRE_GNSS_TYPE_DATA (CHRE_EVENT_SENSOR_LAST_EVENT + 4)
#define CHRE_GNSS_TYPE_GF (CHRE_EVENT_SENSOR_LAST_EVENT + 5)
#define CHRE_WWAN_TYPE (CHRE_EVENT_SENSOR_LAST_EVENT + 6)
#define CHRE_QC_BLE_TYPE_PASSIVE (CHRE_EVENT_SENSOR_LAST_EVENT + 7)

struct test_commands {
  uint32_t sensType;
  char name[128];
  uint32_t mode;
  uint32_t modeCur;
  uint64_t latency;
  uint64_t interval;
};

// TODO accel should be one sample per seconds, looks like many more
struct test_commands cmds[] = {
  { CHRE_SENSOR_TYPE_ACCELEROMETER,                  "Accel",                           CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_UNCALIBRATED_ACCELEROMETER,     "Accel Uncal",                     CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_ACCELEROMETER_TEMPERATURE,      "Accel Temp",                      CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_GYROSCOPE,                      "Gyro",                            CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_UNCALIBRATED_GYROSCOPE,         "Gyro Uncal",                      CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_GYROSCOPE_TEMPERATURE,          "Gyro Temp",                       CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_GEOMAGNETIC_FIELD,              "Mag",                             CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_UNCALIBRATED_GEOMAGNETIC_FIELD, "Mag Uncal",                       CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_GEOMAGNETIC_FIELD_TEMPERATURE,  "Mag Temp",                        CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_PRESSURE,                       "Press",                           CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_LIGHT,                          "Light",                           CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_PROXIMITY,                      "Prox",                            CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 1000000000, 1000000000 },
  { CHRE_SENSOR_TYPE_INSTANT_MOTION_DETECT,          "Motion Detect",                   CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT,   CHRE_SENSOR_CONFIGURE_MODE_DONE, (uint32_t)-1, (uint32_t)-1 },
  { CHRE_SENSOR_TYPE_STATIONARY_DETECT,              "Stationary Detect",               CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT,   CHRE_SENSOR_CONFIGURE_MODE_DONE, (uint32_t)-1, (uint32_t)-1 },
  { CHRE_WIFI_TYPE_ACTIVE,                           "WIFI Scan",                       CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 0 },
  { CHRE_WIFI_TYPE_PASSIVE,                          "WIFI Scan Passive",               CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 0 },
  { CHRE_GNSS_TYPE_LOCATION,                         "GNSS Location",                   CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 1000 },
  { CHRE_GNSS_TYPE_DATA,                             "GNSS Data",                       CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 1000 },
  { CHRE_GNSS_TYPE_GF,                               "GNSS Geofence",                   CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 1000 },
  { CHRE_WWAN_TYPE,                                  "WWAN Scan",                       CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT,   CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 0 },
  { CHRE_QC_BLE_TYPE_PASSIVE,                        "QC BLE Scan Passive",             CHRE_SENSOR_CONFIGURE_MODE_CONTINUOUS, CHRE_SENSOR_CONFIGURE_MODE_DONE, 0, 0 },
};

#define HELP_COMMAND    100
#define STOP_COMMAND    101

static ssize_t nbgetchar(char* c)
{
  return read(0, c, 1);
}

int rspSuccess = 0;
static int rspEfd = 0;
static uint64_t rspVal = 0;;
static int initResponse(void)
{
  rspEfd = eventfd(0, 0);
  if (rspEfd == -1) {
    LOGE("failed to create eventfd");
    return -1;
  }
  return 0;
}
static int waitForResponse(void)
{
  read(rspEfd, &rspVal, sizeof(uint64_t));
  return rspSuccess;
}
static void gotResponse(int success)
{
  rspSuccess = success;
  rspVal = 1;
  write(rspEfd, &rspVal, sizeof(uint64_t));
}

static int evtRcvd = 1;

typedef struct timeTrack_t {
  uint64_t lastEvt;
  int64_t difLastEvt;
} timeTrack_t;

#define N_IN_M 1000000
#define N_RND 555555

static void printFloatEvent(timeTrack_t* tt, const char* str, struct chreSensorFloatData* evt)
{
  //int64_t difEvt = 0;

  uint64_t baseTime = evt->header.baseTimestamp;
  for (int i = 0; i < evt->header.readingCount; i++) {
    uint64_t time = baseTime + evt->readings[i].timestampDelta;
    /*
     difEvt = (time - tt->lastEvt + N_RND)  / N_IN_M;
     printf("%5d @%13lluns %s % 7.2f - D% 8lldms DC% 8lldms\n",
           num, time, str, singles[i].fdata, difEvt, difEvt - tt->difLastEvt);*/
    printf("%5d @%13lluns %s % 7.2f\n", evtRcvd++, (long long unsigned)time, str, evt->readings[i].value);
//    tt->lastEvt = time;
//    tt->difLastEvt = difEvt;
    baseTime += evt->readings[i].timestampDelta;
  }
}


static void printByteEvent(timeTrack_t* tt, const char* str, struct chreSensorByteData* evt)
{
  //int64_t difEvt = 0;

  uint64_t baseTime = evt->header.baseTimestamp;
  for (int i = 0; i < evt->header.readingCount; i++) {
    uint64_t time = baseTime + evt->readings[i].timestampDelta;
    //difEvt = (time - tt->lastEvt + N_RND)  / N_IN_M;
    // printf("%5d @%13lluns %s % 7.2f - D% 8lldms DC% 8lldms\n",
    //       num, time, str, singles[i].fdata, difEvt, difEvt - tt->difLastEvt);
    printf("%5d @%13lluns %s % 7d\n", evtRcvd++, (long long unsigned)time, str, evt->readings[i].value);
//    tt->lastEvt = time;
//    tt->difLastEvt = difEvt;
    baseTime += evt->readings[i].timestampDelta;
  }
}

static void printThreeAxisEvent(timeTrack_t* tt, const char* str, struct chreSensorThreeAxisData* evt)
{
  //int64_t difEvt = 0;

  uint64_t baseTime = evt->header.baseTimestamp;
  for (int i = 0; i < evt->header.readingCount; i++) {
    uint64_t time = baseTime + evt->readings[i].timestampDelta;
    /*
     difEvt = (time - tt->lastEvt + N_RND)  / N_IN_M;
     printf("%5d @%13lluns %s % 7.2f,% 7.2f,% 7.2f - D% 8lldms DC% 8lldms\n",
           num, time, str, triples[i].x, triples[i].y, triples[i].z,
           difEvt, difEvt - tt->difLastEvt);*/
    printf("%5d @%13lluns %s % 7.2f,% 7.2f,% 7.2f\n",
           evtRcvd++, (long long unsigned)time, str, evt->readings[i].x, evt->readings[i].y, evt->readings[i].z);
//    tt->lastEvt = time;
//    tt->difLastEvt = difEvt;
    baseTime += evt->readings[i].timestampDelta;
  }
}

static void printOccuranceEvent(timeTrack_t* tt, const char* str, struct chreSensorOccurrenceData* evt)
{
  //int64_t difEvt = 0;

  uint64_t baseTime = evt->header.baseTimestamp;
  for (int i = 0; i < evt->header.readingCount; i++) {
    uint64_t time = baseTime + evt->readings[i].timestampDelta;
    //difEvt = (time - tt->lastEvt + N_RND)  / N_IN_M;
    // printf("%5d @%13lluns %s % 7.2f - D% 8lldms DC% 8lldms\n",
    //       num, time, str, singles[i].fdata, difEvt, difEvt - tt->difLastEvt);
    printf("%5d @%13lluns %s\n", evtRcvd++, (long long unsigned)time, str);
//    tt->lastEvt = time;
//    tt->difLastEvt = difEvt;
    baseTime += evt->readings[i].timestampDelta;
  }
}

/**
 * @file
 * A test utility that connects to the CHRE daemon that runs on the apps
 * processor of MSM chipsets, which is used to help test basic functionality.
 */

#ifndef CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#endif

using android::sp;
using android::chre::IChreMessageHandlers;
using android::chre::SocketClient;
using android::chre::HostProtocolHost;
using flatbuffers::FlatBufferBuilder;

// Aliased for consistency with the way these symbols are referenced in
// CHRE-side code
namespace fbs = ::chre::fbs;

namespace {

//! The host endpoint we use when sending; set to CHRE_HOST_ENDPOINT_UNSPECIFIED
constexpr uint16_t kHostEndpoint = 0xfffe;

class SocketCallbacks : public SocketClient::ICallbacks,
   public IChreMessageHandlers {
public:
  void onMessageReceived(const void* data, size_t length) override
  {
    if (!HostProtocolHost::decodeMessageFromChre(data, length, *this)) {
      LOGE("Failed to decode message");
    }
  }

  void onConnected(void) override
  {
    LOGI("Socket (re)connected");
  }

  void onConnectionAborted(void) override
  {
    LOGI("Socket (re)connection aborted");
  }

  void onDisconnected(void) override
  {
    LOGI("Socket disconnected");
  }

  void handleNanoappMessage(
  const ::chre::fbs::NanoappMessageT& app_message) override
  {
    /*
    int err = 0;
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    if (app_message.message_type >= CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE) {
      LOGI("Got message from nanoapp 0x%" PRIx64 " to endpoint 0x%" PRIx16
           " with type 0x%" PRIx32 " and length %zu", app_message.app_id, app_message.host_endpoint,
           app_message.message_type, app_message.message.size());

      if (app_message.message_type == CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE) {
        LOGI("got message %d", app_message.message_type);
      }
      err = 1;
    } else {
      LOGE("callback with unknown message type %u, size %u, ignore", app_message.message_type, app_message.message.size());
    }
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(err); 
    */

    uint32_t i;
    timeTrack_t ttAccel;
    timeTrack_t ttGyro;
    timeTrack_t ttMag;
    timeTrack_t ttPress;
    timeTrack_t ttLight;
    timeTrack_t ttProx;
    timeTrack_t ttGPS;
    timeTrack_t ttFLP;
    timeTrack_t ttMD;
    timeTrack_t ttSD;
    size_t messageLen = app_message.message.size();
    const void* message = (const void*)app_message.message.data();

    memset(&ttAccel, 0, sizeof(timeTrack_t));
    memset(&ttGyro, 0, sizeof(timeTrack_t));
    memset(&ttMag, 0, sizeof(timeTrack_t));
    memset(&ttPress, 0, sizeof(timeTrack_t));
    memset(&ttLight, 0, sizeof(timeTrack_t));
    memset(&ttProx, 0, sizeof(timeTrack_t));
    memset(&ttGPS, 0, sizeof(timeTrack_t));
    memset(&ttFLP, 0, sizeof(timeTrack_t));

    //printf("callback, id %llX type 0x%X len %d\n",
    //       app_message.app_id, app_message.message_type, app_message.message.size());
    switch(app_message.message_type) {
      case CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE:
        printf("callback: CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE\n");
        if (messageLen) {
          printf("\n  Data:\n  ");
          for (i = 0; i < messageLen; i++) {
            printf("%02X ", ((char*)message)[i]);
            if (0 == (i + 1) % 8) {
              printf("\n  ");
            }
          }
          printf("\n");
        }
        break;

      case CHRE_EVENT_SENSOR_ACCELEROMETER_DATA:
      {
        printThreeAxisEvent(&ttAccel, "Accel      ", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_UNCALIBRATED_ACCELEROMETER_DATA:
      {
        printThreeAxisEvent(&ttAccel, "Accel Uncal", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_ACCELEROMETER_TEMPERATURE_DATA:
      {
        printFloatEvent(&ttAccel, "Accel Temp ", (struct chreSensorFloatData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_GYROSCOPE_DATA:
      {
        printThreeAxisEvent(&ttGyro, "Gyro       ", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_UNCALIBRATED_GYROSCOPE_DATA:
      {
        printThreeAxisEvent(&ttGyro, "Gyro Uncal ", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_GYROSCOPE_TEMPERATURE_DATA:
      {
        printFloatEvent(&ttAccel, "Gyro Temp  ", (struct chreSensorFloatData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_DATA:
      {
        printThreeAxisEvent(&ttMag, "Mag        ", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_UNCALIBRATED_GEOMAGNETIC_FIELD_DATA:
      {
        printThreeAxisEvent(&ttMag, "Mag UnCal  ", (struct chreSensorThreeAxisData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_GEOMAGNETIC_FIELD_TEMPERATURE_DATA:
      {
        printFloatEvent(&ttAccel, "Mag Temp   ", (struct chreSensorFloatData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_PRESSURE_DATA:
      {
        printFloatEvent(&ttPress, "Press      ", (struct chreSensorFloatData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_LIGHT_DATA:
      {
        printFloatEvent(&ttLight, "Light      ", (struct chreSensorFloatData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_PROXIMITY_DATA:
      {
        printByteEvent(&ttProx, "Prox       ", (struct chreSensorByteData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_INSTANT_MOTION_DETECT_DATA:
      {
        printOccuranceEvent(&ttMD, "Motion Det ", (struct chreSensorOccurrenceData*)message);
        break;
      }
      case CHRE_EVENT_SENSOR_STATIONARY_DETECT_DATA:
      {
          printOccuranceEvent(&ttSD, "Station Det", (struct chreSensorOccurrenceData*)message);
          break;
      }
      case CHRE_EVENT_WIFI_SCAN_RESULT:
      {
        struct chreWifiScanEvent* evt = (struct chreWifiScanEvent*)message;
        struct chreWifiScanResult* results = (struct chreWifiScanResult*)(evt + 1);

        for (i = 0; i < evt->resultCount; i++) {
          uint64_t time = evt->referenceTime;
          if (i > 0) {
            //time = packet->referenceTime + results[i].deltaTime;
            // TODO why not use deltaTime?
            time = evt->referenceTime;
          }
          // TODO
          /*printf("%5d @%13lluns WIFI Scan chan %3u band %u, rssi % 4d, 0x%02X%02X%02X%02X%02X%02X, cw %3u, cf0 %3u, cf1 %3u, rtt %8llu, rttstd %8llu %s 0x%X\n",
                  evtRcvd, packet->referenceTime,
                  results[0].channelIndex, results[0].band, results[0].rssi,
                  (unsigned int)results[0].bssid[5],(unsigned int)results[0].bssid[4],(unsigned int)results[0].bssid[3],(unsigned int)results[0].bssid[2],(unsigned int)results[0].bssid[1],(unsigned int)results[0].bssid[0],
                  results[0].channelWidth, results[0].centerFreqIndex0,
                    results[0].centerFreqIndex1, results[0].rtt,
                  results[0].rttStd, results[0].ssid, results[0].flags);
            */
          printf("%5d @%13lluns WIFI Scan band %u, chan %3u, rssi % 4d, 0x%02X%02X%02X%02X%02X%02X, %s, radioChain %4d, rssiChain0 %4d, rssiChain1 %4d\n",
                 evtRcvd++, (long long unsigned)time,
                 results[i].band, results[i].primaryChannel, results[i].rssi,
                 (unsigned int)results[i].bssid[0], (unsigned int)results[i].bssid[1],
                 (unsigned int)results[i].bssid[2], (unsigned int)results[i].bssid[3],
                 (unsigned int)results[i].bssid[4], (unsigned int)results[i].bssid[5],
                 results[i].ssid,
                 results[i].radioChain, results[i].rssiChain0, results[i].rssiChain1);
        }
        printf("radio chain pref %d", evt->radioChainPref);
        break;
      }

      case CHRE_EVENT_QC_BLE_SCAN_RESULT:
      {
        struct chreQcBleScanEvent* evt = (struct chreQcBleScanEvent*)message;

        printf("%5d @%13lluns QC BLE Scan version 0x%X size %d b\n",
               evtRcvd++, (long long unsigned)time, evt->version, evt->size);
        break;
      }

      case CHRE_EVENT_GNSS_LOCATION:
      {
        struct chreGnssLocationEvent* evt = (struct chreGnssLocationEvent*)message;

        printf("%5d @%13lluns GPS Loc   ts %llu, lalo(%d) %d, %d, alt(%d) %f, sp(%d) %f, be(%d) %f, acc(%d) %f\n",
               evtRcvd++, (long long unsigned)evt->timestamp,
               (long long unsigned)evt->timestamp,
               evt->flags & CHRE_GPS_LOCATION_HAS_LAT_LONG, evt->latitude_deg_e7, evt->longitude_deg_e7,
               (evt->flags & CHRE_GPS_LOCATION_HAS_ALTITUDE) >> 1, evt->altitude,
               (evt->flags & CHRE_GPS_LOCATION_HAS_SPEED) >> 2, evt->speed,
               (evt->flags & CHRE_GPS_LOCATION_HAS_BEARING) >> 3, evt->bearing,
               (evt->flags & CHRE_GPS_LOCATION_HAS_ACCURACY) >> 4, evt->accuracy);
        break;
      }

      case CHRE_EVENT_GNSS_DATA:
      {
        struct chreGnssDataEvent* evt = (struct chreGnssDataEvent*)message;
        struct chreGnssMeasurement* measurements = (struct chreGnssMeasurement*)(evt + 1);

        printf("%5d @%13lluns GNSS Clock t %" PRIx64 ", fb(%d) %" PRIx64 ", b(%d) %f, bu(%d) %f, d(%d) %f, du(%d) %f, disc %" PRIx32 "\n",
               evtRcvd++, evt->clock.time_ns,
               (long long)evt->clock.time_ns,
               (evt->clock.flags & CHRE_GNSS_CLOCK_HAS_FULL_BIAS) >> 2, evt->clock.full_bias_ns,
               (evt->clock.flags & CHRE_GNSS_CLOCK_HAS_BIAS) >> 3, evt->clock.bias_ns,
               (evt->clock.flags & CHRE_GNSS_CLOCK_HAS_BIAS_UNCERTAINTY) >> 4, evt->clock.bias_uncertainty_ns,
               (evt->clock.flags & CHRE_GNSS_CLOCK_HAS_DRIFT) >> 5, evt->clock.drift_nsps,
               (evt->clock.flags & CHRE_GNSS_CLOCK_HAS_DRIFT_UNCERTAINTY) >> 6, evt->clock.drift_uncertainty_nsps,
               evt->clock.hw_clock_discontinuity_count);
        printf("                     measCount %d\n", evt->measurement_count);
        for (uint8_t i = 0; i < evt->measurement_count; i++) {
          printf("                     GNSS Meas[%d] t %" PRIx64 ", st %d, id %d, svt %" PRIx64 ", tu %" PRIx64 ", cN0 %f, pr %f, pru %f\n",
                 i,
                 measurements[i].time_offset_ns,
                 measurements[i].svid,
                 measurements[i].state,
                 measurements[i].received_sv_time_in_ns,
                 measurements[i].received_sv_time_uncertainty_in_ns,
                 measurements[i].c_n0_dbhz,
                 measurements[i].pseudorange_rate_mps,
                 measurements[i].pseudorange_rate_uncertainty_mps);
        }
        break;
      }

      case CHRE_EVENT_GNSS_GF_RESPONSE:
      {
          uint32_t i, j;
          uint8_t localCount = 0;

          size_t count = *((size_t*)message);

          if (9999 == count) { /* special case when the command fails, but no event is sent from the driver */
              gfCmdInProgress = false;
              printf("Command failed!\n");
              return;
          }

          printf("%5d GF Response count=%u\n", evtRcvd++, (unsigned)count);

          uint8_t* err = (uint8_t*)((char*)message + sizeof(size_t));
          uint32_t* ids = (uint32_t*)((char*)message + sizeof(size_t) + (sizeof(uint8_t) * count));

          for (i = 0; i < count; i++) {
              printf("err[%d]=%d id[%d]=%d\n", i, *(err + i), i, *(ids + i));
          }
          switch (lastGfCmd) {
          case GEOFENCE_CMD_ADD:
              localCount = 0;
              for (i = 0; i < count; i++) {
                  if (0 == *(err + i)) {
                      for (j = 0; j < GF_BIG_ARRAY_SIZE; j++) {
                          if (0 == activeGFs.gfIdArray[j]) {
                              activeGFs.gfIdArray[j] = *(ids + i);
                              memcpy(&activeGFs.gfDataArray[j],
                                  &currentGfCmd.gfDataArray[i],
                                  sizeof(qcGeofenceDataType));
                              memcpy(&activeGFs.gfOptionsArray[j],
                                  &currentGfCmd.gfOptionsArray[i],
                                  sizeof(qcGeofenceOptionsType));
                              activeGFs.gfActiveArray[j] = 1;
                              localCount++;
                              break;
                          }
                      }
                  }
              }
              activeGfCount += localCount;
              break;

          case GEOFENCE_CMD_REMOVE:
              localCount = 0;
              for (i = 0; i < count; i++) {
                  if (0 == *(err + i)) {
                      for (j = 0; j < GF_BIG_ARRAY_SIZE; j++) {
                          if (*(ids + i) == activeGFs.gfIdArray[j]) {
                              activeGFs.gfIdArray[j] = 0;
                              memset(&activeGFs.gfDataArray[j],
                                  0,
                                  sizeof(qcGeofenceDataType));
                              memset(&activeGFs.gfOptionsArray[j],
                                  0,
                                  sizeof(qcGeofenceOptionsType));
                              activeGFs.gfActiveArray[j] = 0;
                              localCount++;
                              break;
                          }
                      }
                  }
              }
              activeGfCount -= localCount;
              break;

          case GEOFENCE_CMD_MODIFY:
              for (i = 0; i < count; i++) {
                  if (0 == *(err + i)) {
                      for (j = 0; j < GF_BIG_ARRAY_SIZE; j++) {
                          if (*(ids + i) == activeGFs.gfIdArray[j]) {
                              memcpy(&activeGFs.gfOptionsArray[j],
                                  &currentGfCmd.gfOptionsArray[i],
                                  sizeof(qcGeofenceOptionsType));
                              break;
                          }
                      }
                  }
              }
              break;

          case GEOFENCE_CMD_PAUSE:
              for (i = 0; i < count; i++) {
                  if (0 == *(err + i)) {
                      for (j = 0; j < GF_BIG_ARRAY_SIZE; j++) {
                          if (*(ids + i) == activeGFs.gfIdArray[j]) {
                              activeGFs.gfActiveArray[j] = 0;
                              break;
                          }
                      }
                  }
              }
              break;

          case GEOFENCE_CMD_RESUME:
              for (i = 0; i < count; i++) {
                  if (0 == *(err + i)) {
                      for (j = 0; j < GF_BIG_ARRAY_SIZE; j++) {
                          if (*(ids + i) == activeGFs.gfIdArray[j]) {
                              activeGFs.gfActiveArray[j] = 1;
                              break;
                          }
                      }
                  }
              }
              break;
          }
          gfCmdInProgress = false;
          break;
      }

      case CHRE_EVENT_GNSS_GF_BREACH:
      {
          const char* bType[] = { "ENTER", "EXIT", "DWELL IN", "DWELL OUT" };
          size_t count = *((size_t*)message);
          uint32_t* ids = (uint32_t*)((char*)message + sizeof(size_t));
          printf("%5d GF Breach count=%u\n", evtRcvd++, (unsigned)count);
          for (uint32_t i = 0; i < count; i++) {
              printf("GF Loc   id %d\n",
                  *(ids + i));
          }

          qcGeofenceLocationType location = *((qcGeofenceLocationType*)((char*)message + sizeof(size_t) + (sizeof(uint32_t) * count)));
          uint8_t bT = *((uint8_t*)((char*)message + sizeof(size_t) + (sizeof(uint32_t) * count) + sizeof(qcGeofenceLocationType)));
          uint32_t tSL = *((uint32_t*)((char*)message + sizeof(size_t) + (sizeof(uint32_t) * count) + sizeof(qcGeofenceLocationType) + sizeof(uint8_t)));
          uint32_t tSH = *((uint32_t*)((char*)message + sizeof(size_t) + (sizeof(uint32_t) * count) + sizeof(qcGeofenceLocationType) + sizeof(uint8_t) + sizeof(uint32_t)));
          uint64_t tS = ((uint64_t)tSH << 32) | tSL;
          printf("GF Loc   ts %llu %s lalo %lf, %lf\n",
              (long long unsigned)tS,
              bType[bT],
              location.latitude,
              location.longitude);
          break;
      }

      case CHRE_EVENT_WWAN_CELL_INFO_RESULT:
      {
        struct chreWwanCellInfoResult* ci = (struct chreWwanCellInfoResult*)message;
        ci->cells = (struct chreWwanCellInfo*)(ci + 1);
        // TODO evtRcvd not printed or incremented?
        cellinfo_dump_event(ci, printf);
        break;
      }

      default:
        printf("callback with unknown message type 0x%X\n", app_message.message_type);
    }
  }

  void handleHubInfoResponse(
     const ::chre::fbs::HubInfoResponseT& app_info_response) override
  {
    const char *name = android::chre::getStringFromByteVector(app_info_response.name);
    const char *vendor = android::chre::getStringFromByteVector(app_info_response.vendor);
    const char *toolchain = android::chre::getStringFromByteVector(app_info_response.toolchain);

    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got hub info response:");
    LOGI("  Name: '%s'", name);
    LOGI("  Vendor: '%s'", vendor);
    LOGI("  Toolchain: '%s'", toolchain);
    LOGI("  Legacy versions: platform 0x%08" PRIx32 " toolchain 0x%08" PRIx32,
         app_info_response.platform_version, app_info_response.toolchain_version);
    LOGI("  MIPS %.2f Power (mW): stopped %.2f sleep %.2f peak %.2f",
         app_info_response.peak_mips, app_info_response.stopped_power, app_info_response.sleep_power, app_info_response.peak_power);
    LOGI("  Max message len: %" PRIu32, app_info_response.max_msg_len);
    LOGI("  Platform ID: 0x%016" PRIx64 " Version: 0x%08" PRIx32,
         app_info_response.platform_id, app_info_response.chre_platform_version);
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(1);
  }

  void handleNanoappListResponse(
     const fbs::NanoappListResponseT& response) override
  {
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    LOGI("Got nanoapp list response with %zu apps:", response.nanoapps.size());
    for (const std::unique_ptr<fbs::NanoappListEntryT>& nanoapp
         : response.nanoapps){
      LOGI("  App ID 0x%016" PRIx64 " version 0x%" PRIx32 " enabled %d system "
           "%d", nanoapp->app_id, nanoapp->version, nanoapp->enabled,
           nanoapp->is_system);
    }
    LOGI("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    gotResponse(1);
  }

  void handleLoadNanoappResponse(
     const ::chre::fbs::LoadNanoappResponseT& response) override
  {
    if (!response.success) {
      LOGE("failed to load app");
    }
    gotResponse(response.success);
  }

  void handleUnloadNanoappResponse(
     const ::chre::fbs::UnloadNanoappResponseT& response) override
  {
    if (!response.success) {
      // we don't care, LOGE("Failed to uload app");
    }
    gotResponse(response.success);
  }

};

int requestHubInfo(SocketClient& client)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeHubInfoRequest(builder);

  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("failed to send hub info request message");
  }
  return waitForResponse();
}
/*
int requestNanoappList(SocketClient& client)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeNanoappListRequest(builder);

  LOGI("Sending app list request (%" PRIu32 " bytes)", builder.GetSize());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message");
  }
  return waitForResponse();
}
*/
int sendMessageToNanoapp(
   SocketClient& client, uint64_t appId, uint32_t messageType, uint8_t* msg, int len)
{
  FlatBufferBuilder builder(64);
  HostProtocolHost::encodeNanoappMessage(
     builder, appId, CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE + messageType,
     1234 /* transactionId */, msg, len);

  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send message to nanoapp");
  }
  return 1;
}

int sendLoadNanoappRequest(
   SocketClient& client, const char* filename, uint64_t appId,
   uint32_t appVersion)
{
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file) {
    LOGE("Couldn't open file '%s': %s", filename, strerror(errno));
    return 0;
  }
  ssize_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    LOGE("Couldn't read from file: %s", strerror(errno));
    return 0;
  }
  //android::chre::FragmentedLoadRequest request = android::chre::FragmentedLoadRequest(0,1,appId, appVersion, 0x01000000,size, buffer);
  const android::chre::FragmentedLoadRequest &rrequest(android::chre::FragmentedLoadRequest(0,1,appId, appVersion, 0x01000000,size, buffer));

  FlatBufferBuilder builder(128 + rrequest.binary.size());
  HostProtocolHost::encodeFragmentedLoadNanoappRequest(builder, rrequest);

  LOGI("Sending load nanoapp request (%" PRIu32 " bytes total w/%zu bytes of "
       "payload)", builder.GetSize(), buffer.size());
  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send load nanoapp request message");
  }
  return waitForResponse();
}

int sendUnloadNanoappRequest(SocketClient& client, uint64_t appId)
{
  FlatBufferBuilder builder(48);
  constexpr uint32_t kTransactionId = 4321;
  HostProtocolHost::encodeUnloadNanoappRequest(
     builder, kTransactionId, appId, true /* allowSystemNanoappUnload */);

  if (!client.sendMessage(builder.GetBufferPointer(), builder.GetSize())) {
    LOGE("Failed to send unload nanoapp request message");
  }
  return waitForResponse();
}

int gRun = 1;

static int handleGfCommand(char gfCommand, int idx)
{
    int i;
    int numberOfCommands;
    const char* name[] =
    {
        "Init Geofences",
        "Add Geofences",
        "Remove Geofences",
        "Modify Geofences",
        "Pause Geofences",
        "Resume Geofences",
        "Print Geofences",
        "Deinit Geofences",
        "Exit to main menu"
    };

    numberOfCommands = sizeof(name) / sizeof(char*);

    if ((gfCommand == 'h')) {
        // help menu
        printf("Geofence Menu:\n");
        printf("  h --> Help information\n");
        for (i = 0; i < numberOfCommands; i++) {
            printf("  % 3d --> %s\n", i, name[i]);
        }
        return -1;
    }
    else if (gfCommand == '\n') {
        return -2;
    }
    else {
        i = gfCommand - '0';
        printf("Command = %s\n", name[i]);
        return i;
    }
}

static int getCommand(bool bAllowSH)
{
  int i = -1;
  char line[20];
  int j = 0;

  memset(line, 0, 20);
  while (1) {
    while (-1 != j) {
      j = read(0, line, 10);
      if (bAllowSH) {
        if (2 == j) {
          if ('s' == line[0]) {
            return STOP_COMMAND;
          }
          if ('h' == line[0]) {
            return HELP_COMMAND;
          }
        }
      }
      if (j > 1 || ((1 == j) && (line[0] != 0x0A))) {
        i = atoi(line);
      }
      if (i >= 0) {
        break;
      }
    }
    j = 0;
    if (i >= 0) {
      break;
    }
  }
  return i;
}

static double getFloatInput()
{
    double d = 0.0;
    char line[50];
    int j = 0;

    memset(line, 0, 50);
    while (1) {
        while (-1 != j) {
            j = read(0, line, 40);
            if (j > 1 || ((1 == j) && (line[0] != 0x0A))) {
                d = atof(line);
            }
            if (d != 0.0) {
                break;
            }
        }
        j = 0;
        if (d != 0.0) {
            break;
        }
    }
    return d;
}

static void getGfOptionsInput(uint32_t i)
{
    int32_t j;
    struct GeofenceCmd* pGfCmd = &currentGfCmd;

    pGfCmd->gfOptionsArray[i].size = sizeof(qcGeofenceOptionsType);
    j = -1;
    while (j < 1 || j > 4) {
        printf("Select breach type for geofence #%d:\n", i + 1);
        printf("  1. Enter\n");
        printf("  2. Dwell In\n");
        printf("  3. Exit\n");
        printf("  4. Dwell Out\n");
        j = getCommand(false);
        switch (j) {
        case 1:
            pGfCmd->gfOptionsArray[i].breach_type_mask = QC_GEOFENCE_BREACH_ENTER_BIT;
            break;
        case 2:
            pGfCmd->gfOptionsArray[i].breach_type_mask = QC_GEOFENCE_BREACH_ENTER_BIT | QC_GEOFENCE_BREACH_DWELL_IN_BIT;
            break;
        case 3:
            pGfCmd->gfOptionsArray[i].breach_type_mask = QC_GEOFENCE_BREACH_EXIT_BIT;
            break;
        case 4:
            pGfCmd->gfOptionsArray[i].breach_type_mask = QC_GEOFENCE_BREACH_EXIT_BIT | QC_GEOFENCE_BREACH_DWELL_OUT_BIT;
            break;
        }
    }
    printf("Enter responsiveness for geofence #%d:\n", i + 1);
    pGfCmd->gfOptionsArray[i].responsiveness = (uint32_t)getFloatInput();
    printf("Enter dwell_time for geofence #%d:\n", i + 1);
    pGfCmd->gfOptionsArray[i].dwell_time = (uint32_t)getFloatInput();
}

static void getGfAddInput()
{
    int32_t j, mG;
    struct GeofenceCmd* pGfCmd = &currentGfCmd;

    memset(pGfCmd, 0, sizeof(currentGfCmd));
    pGfCmd->cmd = GEOFENCE_CMD_ADD;
    j = -1;
    if (GF_BIG_ARRAY_SIZE - activeGfCount >= GF_ARRAY_SIZE) {
        mG = GF_ARRAY_SIZE;
    }
    else {
        mG = GF_BIG_ARRAY_SIZE - activeGfCount;
    }
    while (j < 1 || j > mG) {
        printf("Enter number of geofences to add (1-%d):\n", mG);
        j = getCommand(false);
        pGfCmd->gfCount = j;
    }
    for (uint32_t i = 0; i < pGfCmd->gfCount; i++) {
        pGfCmd->gfDataArray[i].size = sizeof(qcGeofenceDataType);
        printf("Enter latitude for geofence #%d:\n", i+1);
        pGfCmd->gfDataArray[i].latitude = getFloatInput();
        printf("Enter longitude for geofence #%d:\n", i + 1);
        pGfCmd->gfDataArray[i].longitude = getFloatInput();
        printf("Enter radius for geofence #%d:\n", i + 1);
        pGfCmd->gfDataArray[i].radius = getFloatInput();
        getGfOptionsInput(i);
    }
}

static void getGfRemovePauseResumeInput(GeofenceCmds cmd)
{
    int32_t j, mG;
    uint32_t i, k;
    bool bFound = false;
    struct GeofenceCmd* pGfCmd = &currentGfCmd;

    memset(pGfCmd, 0, sizeof(currentGfCmd));
    pGfCmd->cmd = cmd;
    j = -1;
    if (activeGfCount >= GF_ARRAY_SIZE) {
        mG = GF_ARRAY_SIZE;
    }
    else {
        mG = activeGfCount;
    }
    while (j < 1 || j > mG) {
        printf("Enter number of geofences (max %d):\n", mG);
        j = getCommand(false);
        pGfCmd->gfCount = j;
    }
    for (i = 0; i < pGfCmd->gfCount; i++) {
        j = -1;
        while (j < 0) {
            printf("Enter id for geofence #%d:\n", i + 1);
            j = getCommand(false);
            bFound = false;
            for (k = 0; k < GF_BIG_ARRAY_SIZE; k++) {
                if (j < 0) {
                    break;
                }
                if ((uint32_t)j == activeGFs.gfIdArray[k]) {
                    bFound = true;
                    break;
                }
            }
            if (bFound) {
                pGfCmd->gfIdArray[i] = j;
            }
            else {
                j = -1;
            }
        }
    }
}

static void getGfModifyInput()
{
    int32_t j, mG;
    uint32_t i, k;
    bool bFound = false;
    struct GeofenceCmd* pGfCmd = &currentGfCmd;

    memset(pGfCmd, 0, sizeof(currentGfCmd));
    pGfCmd->cmd = GEOFENCE_CMD_MODIFY;
    j = -1;
    if (activeGfCount >= GF_ARRAY_SIZE) {
        mG = GF_ARRAY_SIZE;
    }
    else {
        mG = activeGfCount;
    }
    while (j < 1 || j > mG) {
        printf("Enter number of geofences to modify (max %d):\n", mG);
        j = getCommand(false);
        pGfCmd->gfCount = j;
    }
    for (i = 0; i < pGfCmd->gfCount; i++) {
        j = -1;
        while (j < 0) {
            printf("Enter id for geofence #%d:\n", i + 1);
            j = getCommand(false);
            bFound = false;
            for (k = 0; k < GF_BIG_ARRAY_SIZE; k++) {
                if (j < 0) {
                    break;
                }
                if ((uint32_t)j == activeGFs.gfIdArray[k]) {
                    bFound = true;
                    break;
                }
            }
            if (bFound) {
                pGfCmd->gfIdArray[i] = j;
                getGfOptionsInput(i);
            }
            else {
                j = -1;
            }
        }
    }
}

static void handleCommand(SocketClient& client, int userin)
{
  uint32_t i;
  int32_t j;
  char gfCommand;
  struct ConfigCmd cmd;

  if ((userin == HELP_COMMAND)) {
    // help menu
    printf("\nCHRE Test Commands:\n\n");
    printf("  h --> Help information\n");
    printf("  s --> Stop and exit\n");
    printf("\n  Enable/Disable sensors\n");
    for (uint32_t i = 0; i < sizeof(cmds) / sizeof(struct test_commands); i++) {
      printf("    % 3d --> %s\n", i, cmds[i].name);
    }
    printf("\n");
  } else if (userin == STOP_COMMAND) {
    // stop chre
    printf("stopping\n");
    gRun = 0;
  } else {

    // send enable/disable command
    i = userin;
    if (i >= sizeof(cmds) / sizeof(struct test_commands)) {
      if ('\n' != userin) {
        printf("unknown command '%c'\n", userin);
      }
      return;
    }

    if (CHRE_GNSS_TYPE_LOCATION == cmds[i].sensType || CHRE_GNSS_TYPE_DATA == cmds[i].sensType) {
      if (0 == cmds[i].modeCur) {
        j = -1;
        while (j < 1000) {
            printf("Enter rate in milliseconds (>=1000):\n");
            j = getCommand(false);
            cmds[i].interval = j;
        }
      }
    }

    if (CHRE_GNSS_TYPE_GF == cmds[i].sensType) {
        currentGfCmd.cmd = GEOFENCE_CMD_INIT;
        handleGfCommand('h', i);
        printf("Enter a command and press enter:\n");
        while (currentGfCmd.cmd != GEOFENCE_CMD_EXIT) {
            j = -1;
            while (gRun) {
                while (-1 != nbgetchar(&gfCommand)) {
                    j = handleGfCommand(gfCommand, i);
                    if (j >= 0) {
                        break;
                    }
                }
                if (j >= 0) {
                    break;
                }
            }
            currentGfCmd.cmd = j;

            switch (currentGfCmd.cmd)
            {
            case GEOFENCE_CMD_INIT:
            case GEOFENCE_CMD_DEINIT:
                if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                    LOGE("failed to send message");
                }
                break;

            case GEOFENCE_CMD_ADD:
                if (gfCmdInProgress) {
                    printf("Wait for callback before a new command\n");
                    continue;
                }
                if (GF_BIG_ARRAY_SIZE == activeGfCount) {
                    printf("Maximum number of geofences has been reached!");
                }
                else {
                    getGfAddInput();
                    gfCmdInProgress = true;
                    lastGfCmd = GEOFENCE_CMD_ADD;
                    if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                        LOGE("failed to send message");
                    }
                }
                break;

            case GEOFENCE_CMD_REMOVE:
                if (gfCmdInProgress) {
                    printf("Wait for callback before a new command\n");
                    continue;
                }
                if (0 == activeGfCount) {
                    printf("There is no geofence to be removed\n");
                }
                else {
                    getGfRemovePauseResumeInput(GEOFENCE_CMD_REMOVE);
                    gfCmdInProgress = true;
                    lastGfCmd = GEOFENCE_CMD_REMOVE;
                    if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                        LOGE("failed to send message");
                    }
                }
                break;

            case GEOFENCE_CMD_MODIFY:
                if (gfCmdInProgress) {
                    printf("Wait for callback before a new command\n");
                    continue;
                }
                if (0 == activeGfCount) {
                    printf("There is no geofence to be modified\n");
                }
                else {
                    getGfModifyInput();
                    gfCmdInProgress = true;
                    lastGfCmd = GEOFENCE_CMD_MODIFY;
                    if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                        LOGE("failed to send message");
                    }
                }
                break;

            case GEOFENCE_CMD_PAUSE:
                if (gfCmdInProgress) {
                    printf("Wait for callback before a new command\n");
                    continue;
                }
                if (0 == activeGfCount) {
                    printf("There is no geofence to be paused\n");
                }
                else {
                    getGfRemovePauseResumeInput(GEOFENCE_CMD_PAUSE);
                    gfCmdInProgress = true;
                    lastGfCmd = GEOFENCE_CMD_PAUSE;
                    if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                        LOGE("failed to send message");
                    }
                }
                break;

            case GEOFENCE_CMD_RESUME:
                if (gfCmdInProgress) {
                    printf("Wait for callback before a new command\n");
                    continue;
                }
                if (0 == activeGfCount) {
                    printf("There is no geofence to be resumed\n");
                }
                else {
                    getGfRemovePauseResumeInput(GEOFENCE_CMD_RESUME);
                    gfCmdInProgress = true;
                    lastGfCmd = GEOFENCE_CMD_RESUME;
                    if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_GF_CFG, (uint8_t*)&currentGfCmd, sizeof(struct GeofenceCmd))) {
                        LOGE("failed to send message");
                    }
                }
                break;

            case GEOFENCE_CMD_PRINT:
                if (0 == activeGfCount) {
                    printf("There are no geofences\n");
                } 
                else {
                    for (uint8_t i = 0; i < GF_BIG_ARRAY_SIZE; i++) {
                        if (0 != activeGFs.gfIdArray[i]) {
                            printf("GF %d %s id=%d lat=%lf lon=%lf rad=%lf time=%d\n",
                                i + 1,
                                (activeGFs.gfActiveArray[i] == 1) ? "ACTIVE" : "PAUSED",
                                activeGFs.gfIdArray[i],
                                activeGFs.gfDataArray[i].latitude,
                                activeGFs.gfDataArray[i].longitude,
                                activeGFs.gfDataArray[i].radius,
                                activeGFs.gfOptionsArray[i].dwell_time);
                        }
                    }
                }
                break;

            case GEOFENCE_CMD_EXIT:
                printf("Exit to main menu\n");
                break;

            default:
                printf("Unknown Geofence command\n");
                break;
            }
        }
        return;
    } else {
      if (CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT == cmds[i].mode) {
        cmd.mode = CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT;
      } else {
        if (CHRE_SENSOR_CONFIGURE_MODE_DONE == cmds[i].modeCur) {
          cmds[i].modeCur = cmds[i].mode;
        } else {
          cmds[i].modeCur = CHRE_SENSOR_CONFIGURE_MODE_DONE;
        }
        cmd.mode = cmds[i].modeCur;
      }

      cmd.latency = cmds[i].latency;
      cmd.interval = cmds[i].interval;
      cmd.sensType = cmds[i].sensType;
      cmd.flags = 0;
      
      printf("send config command type 0x%X mode %d flags 0x%X\n", cmd.sensType, cmd.mode, cmd.flags);
      if (CHRE_SENSOR_CONFIGURE_MODE_ONE_SHOT == cmd.mode) {
        printf("One-Shot %s triggered\n", cmds[i].name);
      } else {
        printf("%s %s\n", cmd.mode ? "Enable" : "Disable", cmds[i].name);
      }
      if (!sendMessageToNanoapp(client, APP_ID_MAKE(APP_ID_VENDOR_QUALCOMM, 8), EVENT_SENSOR_CFG, (uint8_t*)&cmd, sizeof(struct ConfigCmd))) {
        LOGE("failed to send message");
        return;
      }
    }
  }
}

}  // anonymous namespace

int main(void)
{
  SocketClient client;
  sp<SocketCallbacks> callbacks = new SocketCallbacks();
  //struct hub_message_t msg;
  //struct query_apps_request_t apps_req;

  if (0 != initResponse()) {
    goto bail;
  }

  if (!client.connect("chre", callbacks)) {
    LOGE("couldn't connect to socket");
    goto bail;
  }

  requestHubInfo(client);

  printf("load sensor_cfg nanoapp -- 0x%llX\n", (long long unsigned)CHRE_APP_SENSOR_CFG_ID);
  sendUnloadNanoappRequest(client, CHRE_APP_SENSOR_CFG_ID);
  if (!sendLoadNanoappRequest(client, CHRE_APP_SENSOR_CFG_FILE, CHRE_APP_SENSOR_CFG_ID, 0x0)) {
    LOGE("failed to load app");
    goto bail;
  }

  {
    int userin;

    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    handleCommand(client, HELP_COMMAND);

    printf("Enter a command and press enter:\n");

    while (gRun) {
      userin = getCommand(true);
      handleCommand(client, userin);
    }
  }

  sendUnloadNanoappRequest(client, CHRE_APP_SENSOR_CFG_ID);
  printf("client exiting\n");

bail:
  return 0;
}
