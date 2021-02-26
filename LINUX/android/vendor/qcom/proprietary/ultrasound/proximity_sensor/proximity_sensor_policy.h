/*===========================================================================
                           proximity_sensor_policy.h

DESCRIPTION: Definition of the ProximitySensorPolicy class

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef _PROXIMITY_SENSOR_POLICY_H_
#define _PROXIMITY_SENSOR_POLICY_H_

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

/**
   Ultrasound Proximity Sensor Policy:
   Holds configurable constants for HAL and Sensor classes in one place.
 */
class ProximitySensorPolicy {
public:
    static float getMaxRange();
    static float getMaxDistRange();
    static float getResolution();
    static float getPower();
    static float getNearRangeValue();
    static float getFarRangeValue();
    static int getMaxDelay();
};

#endif /* _PROXIMITY_SENSOR_POLICY_H_ */
