/*===========================================================================
                           proximity_sensor.h

DESCRIPTION: This header file exposes function for handling feedback requests
from the EPOS library

Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef _ULTRASOUND_PROXIMITY_SENSOR_H_
#define _ULTRASOUND_PROXIMITY_SENSOR_H_

/*-----------------------------------------------------------------------------
  Include Files
-----------------------------------------------------------------------------*/
#include <hardware/sensors.h>
#include "usf_proximity_driver.h"

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

/**
   Ultrasound Proximity Sensor:
   Layer between Sensor HAL and the ultrasound proximity daemon driver.
 */
class UltrasoundProximitySensor {
public:
    UltrasoundProximitySensor(DaemonDriver *daemon);
    virtual ~UltrasoundProximitySensor();
    virtual int activate(int enable, int sensorHandle);
    virtual int poll(sensors_event_t *);
    virtual int flush();
private:
    DaemonDriver *driver;
    int handle;
    bool isEnabled;
};

#endif /* _ULTRASOUND_PROXIMITY_SENSOR_H_ */
