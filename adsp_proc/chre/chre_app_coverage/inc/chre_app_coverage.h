#ifndef CHRE_APP_COVERAGE_H
#define CHRE_APP_COVERAGE_H

/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

#include "chre/sensor.h"


#define ACCEL_RATE_1KHZ_NS                   (1000000ull)     // 1000 Hz
#define ACCEL_RATE_100HZ_NS                 (10000000ull)     // 100 Hz
#define ACCEL_RATE_50HZ_NS                  (20000000ull)     // 50 Hz
#define ACCEL_RATE_25HZ_NS                  (40000000ull)     // 25 Hz
#define ACCEL_RATE_10HZ_NS                 (100000000ULL)     // 10 Hz

#define WIFI_RATE_5_SEC                    (5000000000ULL)     // 5 second
//
#define ACCEL_LATENCY_64_NS                  640000000ull   // 0.64s in nsec
#define ACCEL_LATENCY_96_NS                  960000000ull   // 0.96s in nsec
#define ACCEL_LATENCY_128_NS                1280000000ull   // 1.28s in nsec
#define ACCEL_LATENCY_256_NS                2560000000ull   // 2.56s in nsec


#define SENSOR_WIFI_SCAN_RESULTS             UINT32_C(3)
#define SENSOR_SAMPLING_STATUS               UINT32_C(2)
#define SENSOR_FOUND_CONFIGURE_WAITING       UINT32_C(1)
#define SENSOR_NOT_FOUND        UINT32_C(-1)
#define SENSOR_FAIL_CONFIGURE                UINT32_C(-2)
#define SENSOR_FAIL_GET_STATUS               UINT32_C(-3)

#ifndef CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE
#define CONTEXT_HUB_TYPE_PRIVATE_MSG_BASE 0x00400
#endif

#define COVERAGE_ECHO 1
#define COVERAGE_ABORT 2
#define COVERAGE_TIMER 3
#define COVERAGE_EVENT_TO_SELF 4
#define COVERAGE_SENSOR_CONFIG  5
#define COVERAGE_CONFIG_MODE_DONE           6
#define COVERAGE_GET_SENSOR_SAMPLING_STATUS 7
#define COVERAGE_GET_SENSOR_INFO            8
#define COVERAGE_GET_WIFI_SCAN_RESULTS      9

struct sensorConfig {
  struct chreSensorSamplingStatus SSStatus;
  struct chreSensorInfo snsInfo;
  uint32_t sensType;
  uint32_t evtType;
  uint32_t mode;
  uint32_t code;
};

// TODO: Need to do this with a better way as cases are growing
extern struct sensorConfig configCont;
extern struct sensorConfig configCont2;
extern struct sensorConfig configCont_Default;
extern struct sensorConfig configCont_1KHz;
extern struct sensorConfig configPassiveCont;
extern struct sensorConfig configOneShot;
extern struct sensorConfig configPassiveOneShot;
extern struct sensorConfig configMotion;
extern struct sensorConfig configStationary;
extern struct sensorConfig configMotion_0;
extern struct sensorConfig configStationary_0;
extern struct sensorConfig configGyroCont;
extern struct sensorConfig configCont_Wifi;
extern struct sensorConfig configPssv_Wifi;

int check_sensor_config(const void* data);

#ifdef __cplusplus
}
#endif

#endif // CHRE_APP_COVERAGE_H

