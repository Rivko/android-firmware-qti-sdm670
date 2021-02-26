/*============================================================================
  @file sensors_calibrate.cpp

  Sensor calibration source file

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/
#include <iostream>
#include <cinttypes>
#include <stdio.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "sensors_calibrate.h"
#include "ssc_calibrate.h"

std::string s_result;

static std::string& calibrate(const std::string& data_type, int8_t &test_type)
{
    sensors_log::set_tag("sensors_calibrate");
    sensors_log::set_level(sensors_log::VERBOSE);
    sensors_log::set_stderr_logging(true);

    sns_logd("sensor calibrate begin");
    if(test_type < SNS_PHYSICAL_SENSOR_TEST_TYPE_SW
       || test_type > SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
        test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;

    ssc_calibrate sensor_calibrate;
    s_result  = sensor_calibrate.calibrate(data_type, test_type);

    sns_logd("s_result: %s", s_result.c_str());
    return s_result;

}


static struct sensor_cal_methods_t cal_methods = {
    .calibrate = calibrate,
};

struct sensor_cal_module_t SENSOR_CAL_MODULE_INFO = {
    .tag = SENSOR_CAL_MODULE_TAG,
    .id = "sensor_cal_module_qti",
    .version = SENSOR_CAL_MODULE_VERSION,
    .vendor = "qualcomm technologies inc.",
    .dso = NULL,
    .methods = &cal_methods,
    .reserved = {0},
};

