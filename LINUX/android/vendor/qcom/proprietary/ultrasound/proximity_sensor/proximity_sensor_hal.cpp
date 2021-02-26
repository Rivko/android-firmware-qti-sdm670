/*===========================================================================
                           proximity_sensor_hal.cpp

DESCRIPTION: Android Sensor HAL interface for proximity device

NOTE: Limited use of #ifdefs support sensor API 1.1 (KK-based, default) or
API 1.3 (required in L release).

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Not a Contribution.
Apache license notifications and license are retained
for attribution purposes only.

Copyright (C) 2012 The Android Open Source Project

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.

You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/

#define LOG_TAG "proximity_sensor_hal"

/*-----------------------------------------------------------------------------
  Includes
-----------------------------------------------------------------------------*/
#include <hardware/hardware.h>
#include <hardware/sensors.h>
#include <log/log.h>
#include <errno.h>
#include "proximity_sensor_hal.h"
#include "usf_proximity_driver.h"
#include "keep_alive_driver_proxy.h"
#include "proximity_sensor_policy.h"
#include "mutex_lock.h"

/*-----------------------------------------------------------------------------
  Global variables
-----------------------------------------------------------------------------*/
static UnSocketReceiver s_socket;
static UsfProximityDriver s_daemon(&s_socket);
static KeepAliveDriverProxy s_proxy(&s_daemon);
static UltrasoundProximitySensor s_defaultSensor(&s_proxy);
static bool deviceIsActive = false;
static bool deviceIsFlushed = false;
static int activeSensorHandle;
static pthread_mutex_t activateMutex;

// Global sensor object; can be changed for mock objects in test
static UltrasoundProximitySensor *s_sensorPtr = &s_defaultSensor;

static sensor_t PROXIMITY_SENSOR = {
    .name = "ultrasound proximity sensor",
    .vendor = "Qualcomm Technologies, Inc.",
    .version = 1,
    .handle = SENSOR_HANDLE,
    .type = SENSOR_TYPE_PROXIMITY,
    .maxRange = ProximitySensorPolicy::getMaxRange(),
    .resolution = ProximitySensorPolicy::getResolution(),
    .power = ProximitySensorPolicy::getPower(),
    .minDelay = 0,// on-change: 0
    .fifoReservedEventCount = 0,
    .fifoMaxEventCount = 0,
#ifdef SENSORS_DEVICE_API_VERSION_1_3
    .stringType = SENSOR_STRING_TYPE_PROXIMITY,
    .requiredPermission = NULL,
    .maxDelay = ProximitySensorPolicy::getMaxDelay(),
    .flags = SENSOR_FLAG_WAKE_UP | SENSOR_FLAG_ON_CHANGE_MODE,
#endif
    .reserved = {}
};

static sensor_t PROXIMITY_POCKET_SENSOR = {
    .name = "ultrasound proximity pocket sensor",
    .vendor = "Qualcomm Technologies, Inc.",
    .version = 1,
    .handle = SENSOR_POCKET_HANDLE,
    .type = SENSOR_TYPE_PROXIMITY,
    .maxRange = ProximitySensorPolicy::getMaxRange(),
    .resolution = ProximitySensorPolicy::getResolution(),
    .power = ProximitySensorPolicy::getPower(),
    .minDelay = 0,// on-change: 0
    .fifoReservedEventCount = 0,
    .fifoMaxEventCount = 0,
#ifdef SENSORS_DEVICE_API_VERSION_1_3
    .stringType = SENSOR_STRING_TYPE_PROXIMITY,
    .requiredPermission = NULL,
    .maxDelay = ProximitySensorPolicy::getMaxDelay(),
    .flags = SENSOR_FLAG_WAKE_UP | SENSOR_FLAG_ON_CHANGE_MODE,
#endif
    .reserved = {}
};

#ifdef SENSORS_DEVICE_API_VERSION_1_3
static sensor_t SENSOR_LIST[] = { PROXIMITY_SENSOR, PROXIMITY_POCKET_SENSOR };
#else
static sensor_t SENSOR_LIST[] = { PROXIMITY_SENSOR };
#endif

static sensors_poll_device_1_t ultrasound_proximity_device;

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  set_proximity_sensor
==============================================================================*/
/**
  Overrides default sensor implementation while unit testing.
 */
void set_proximity_sensor(UltrasoundProximitySensor *newSensorPtr) {
    s_sensorPtr = newSensorPtr;
}

/*==============================================================================
  FUNCTION:  get_sensors_list
==============================================================================*/
/**
  Returns the list of sensors supported by this module.
 */
static int get_sensors_list(struct sensors_module_t* unused_module,
        struct sensor_t const** list) {
    (void) unused_module;
    *list = SENSOR_LIST;

    return sizeof(SENSOR_LIST) / sizeof(*SENSOR_LIST);
}

/*==============================================================================
  FUNCTION:  device_activate
==============================================================================*/
/**
  Activate the sensor on the device. Note, since the device supports only one
  sensor, all device calls go directly to that sensor.
 */
static int device_activate(struct sensors_poll_device_t *dev, int sensor_handle,
        int enabled) {
    (void) dev;

    MutexLock lock(activateMutex);

    ALOGD("%s: handle %d, enabled %d", __FUNCTION__, sensor_handle, enabled);

    if (deviceIsActive && (enabled == 1))
    {
        ALOGW("%s: ultrasound proximity sensor is already active (handle=%d), cannot activate another sensor (handle=%d)",
              __FUNCTION__,
              activeSensorHandle,
              sensor_handle);
        return 1;
    }
    if (deviceIsActive && (enabled == 0) && (activeSensorHandle != sensor_handle))
    {
        ALOGW("%s: ultrasound proximity device is active (handle=%d), cannot deactivate different sensor (handle=%d)",
              __FUNCTION__,
              activeSensorHandle,
              sensor_handle);
        return 1;
    }

    if (enabled == 1) {
        deviceIsActive = true;
        activeSensorHandle = sensor_handle;
    }
    else {
        deviceIsActive = false;
        deviceIsFlushed = false;
    }

    return s_sensorPtr->activate(enabled, sensor_handle);
}

/*==============================================================================
  FUNCTION:  device_close
==============================================================================*/
/**
  Close the sensor device.
 */
static int device_close(struct hw_device_t *dev) {
    (void) dev;
    return 0;
}

/*==============================================================================
  FUNCTION:  device_poll
==============================================================================*/
/**
  Poll the device for up to "count" sensor events.
 */
static int device_poll(struct sensors_poll_device_t *dev, sensors_event_t* data,
        int count) {
    (void) dev;
    (void) count;
    return s_sensorPtr->poll(data);
}

/*==============================================================================
  FUNCTION:  device_set_delay
==============================================================================*/
/**
  Set the delay on the device.
 */
static int device_set_delay(struct sensors_poll_device_t *dev,
        int sensor_handle, int64_t sampling_period_ns) {
    (void) dev;
    (void) sensor_handle;
    (void) sampling_period_ns;
    return 0;
}

/*==============================================================================
  FUNCTION:  device_batch
==============================================================================*/
/**
  Set the batch parameters for the sensor on the device.
 */
static int device_batch(struct sensors_poll_device_1* dev,
            int sensor_handle, int flags, int64_t sampling_period_ns,
            int64_t max_report_latency_ns) {
    (void) dev;
    (void) sensor_handle;
    (void) flags;
    (void) sampling_period_ns;
    (void) max_report_latency_ns;
    return 0;
}

/*==============================================================================
  FUNCTION:  device_flush
==============================================================================*/
/**
  Flush the sensor on the device.
 */
static int device_flush(struct sensors_poll_device_1* dev,
            int sensor_handle) {
    (void) dev;
    (void) sensor_handle;
    return s_sensorPtr->flush();
}

/*==============================================================================
  FUNCTION:  module_open
==============================================================================*/
/**
  Return a device from the hardware module interface.
 */
static int module_open(const struct hw_module_t* module, const char* id,
        struct hw_device_t** device) {
    (void) module;
    (void) id;

    // initialize device

    memset(&ultrasound_proximity_device, 0,
            sizeof(ultrasound_proximity_device));

    ultrasound_proximity_device.common.tag = HARDWARE_DEVICE_TAG;
#ifdef SENSORS_DEVICE_API_VERSION_1_3
    ultrasound_proximity_device.common.version = SENSORS_DEVICE_API_VERSION_1_3;
#else
    ultrasound_proximity_device.common.version = SENSORS_DEVICE_API_VERSION_1_1;
#endif
    ultrasound_proximity_device.common.module =
            const_cast<hw_module_t*>(module);
    ultrasound_proximity_device.common.close = device_close;
    ultrasound_proximity_device.activate = device_activate;
    ultrasound_proximity_device.setDelay = device_set_delay;
    ultrasound_proximity_device.poll = device_poll;
    ultrasound_proximity_device.batch = device_batch;
    ultrasound_proximity_device.flush = device_flush;

    *device = &ultrasound_proximity_device.common;

    pthread_mutex_init(&activateMutex, NULL);

    return 0;
}

#ifdef SENSORS_DEVICE_API_VERSION_1_4
static int set_operation_mode(unsigned int mode) {
    int ret = 0;
    if (mode) {
        ret = -EINVAL;
    }
    return ret;
}
#endif

/*-----------------------------------------------------------------------------
  Function tables
-----------------------------------------------------------------------------*/
static hw_module_methods_t module_methods = {
    .open = module_open
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = 1,
        .hal_api_version = 0,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "Ultrasound Sensors",
        .author = "Qualcomm Technologies, Inc.",
        .methods = &module_methods,
        .dso = NULL,
        .reserved = {}
    },
    .get_sensors_list = &get_sensors_list,
#ifdef SENSORS_DEVICE_API_VERSION_1_4
    .set_operation_mode = set_operation_mode
#endif
};
