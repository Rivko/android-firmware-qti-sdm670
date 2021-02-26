/*===========================================================================
                           proximity_sensor.cpp

DESCRIPTION: Implements the Proximity Sensor

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "proximity_sensor"

/*-----------------------------------------------------------------------------
  Includes
-----------------------------------------------------------------------------*/
#include "proximity_sensor.h"
#include "proximity_sensor_policy.h"
#include <log/log.h>

/*-----------------------------------------------------------------------------
  Macros and constants
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/

/*============================================================================
  FUNCTION:  UltrasoundProximitySensor::UltrasoundProximitySensor
============================================================================*/
/**
  C'tor for proximity sensor class
*/
UltrasoundProximitySensor::UltrasoundProximitySensor(
        DaemonDriver *driverArg) :
        driver(driverArg), handle(-1), isEnabled(false) {
}

/*============================================================================
  FUNCTION:  UltrasoundProximitySensor::~UltrasoundProximitySensor
============================================================================*/
/**
  D'tor for proximity sensor class. Empty but necessary for polymorphism.
*/
UltrasoundProximitySensor::~UltrasoundProximitySensor() {
}

/*============================================================================
  FUNCTION:  UltrasoundProximitySensor::activate
============================================================================*/
/**
  Activates or deactivates the sensor. Activate on an active sensor is a NOP,
  likewise for deactivate on an inactive sensor.
*/
int UltrasoundProximitySensor::activate(int enable, int sensorHandle) {
    if (enable == 1) {
        if (isEnabled) {
            return 0;
        } else {
            this->handle = sensorHandle;
            driver->start(sensorHandle);
            isEnabled = true;
        }
    } else {
        if (!isEnabled) {
            return 0;
        } else {
            driver->stop();
            isEnabled = false;
        }
    }
    return 0;
}

/*============================================================================
  FUNCTION:  UltrasoundProximitySensor::poll
============================================================================*/
/**
  Blocks until driver returns data. Fills in sensor event.
*/
int UltrasoundProximitySensor::poll(sensors_event_t *event) {
    int distanceInMm;
    timespec now;
    int retval = driver->poll(&distanceInMm);
    clock_gettime(CLOCK_MONOTONIC, &now);
    if (retval < 0) {
        return retval;
    }

    if (retval == POLL_RETVAL_FLUSH) {
        // Send flush complete event
        sensors_meta_data_event_t *metaData;
        metaData = (sensors_meta_data_event_t *) event;
        metaData->version = META_DATA_VERSION;
        metaData->sensor = 0;
        metaData->type = SENSOR_TYPE_META_DATA;
        metaData->reserved0 = 0;
        metaData->timestamp = 0;
        metaData->meta_data.what = META_DATA_FLUSH_COMPLETE;
        metaData->meta_data.sensor = this->handle;
        return 1;
    }

    if ((this->handle == SENSOR_HANDLE) || (this->handle == SENSOR_POCKET_HANDLE))
    {
        if (distanceInMm == UsfProximityDriver::FAR_DISTANCE_MARKER) {
            // too far for accurate proximity.
            event->distance = ProximitySensorPolicy::getFarRangeValue();
        } else {
            // near proximity.
            event->distance = ProximitySensorPolicy::getNearRangeValue();
        }
    } else {
        ALOGW("%s: unsupported sensor handle %d", __FUNCTION__, this->handle);
    }
    event->type = SENSOR_TYPE_PROXIMITY;
    event->timestamp = now.tv_nsec + 1e9 * now.tv_sec;
    event->sensor = handle;
    event->version = sizeof(sensors_event_t);
    return 1;
}

/*============================================================================
  FUNCTION:  UltrasoundProximitySensor::flush
============================================================================*/
/**
  Flush the sensor on the device.
*/
int UltrasoundProximitySensor::flush() {
    return driver->flush();
}
