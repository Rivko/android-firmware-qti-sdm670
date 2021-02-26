/*============================================================================
  @file sensors_calibrate.h

  calibration algorithm header file

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H
#include <sys/cdefs.h>

#define SENSOR_CAL_MODULE_TAG   0x12373948
#define SENSOR_CAL_MODULE_VERSION 1
#define SENSOR_CAL_MODULE_INFO   sensorcal
#define SENSOR_CAL_MODULE_INFO_STR   "sensorcal"

//using namespace std;


struct sensor_cal_methods_t {
    std::string& (*calibrate) (const std::string& data_type, int8_t& test_type);
};

struct sensor_cal_module_t {
    /* Tag of the module */
    uint32_t        tag;
    /* Id of the module */
    const char            *id;
    /* Version of the calibration module */
    uint32_t        version;
    /* Vendor of the calibration lib */
    const char            *vendor;
    /* Point to the handle of this module */
    void            *dso;
    /* Callbacks of the calibration lib provided */
    struct sensor_cal_methods_t     *methods;
    /* The compatible sensors list for this library */
    int             reserved[6];
};

#endif
