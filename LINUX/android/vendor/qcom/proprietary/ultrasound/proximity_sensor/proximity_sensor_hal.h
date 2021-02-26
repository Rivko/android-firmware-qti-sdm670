/*===========================================================================
                           proximity_sensor_hal.h

DESCRIPTION: Public interface for proximity_sensor_hal.cpp

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef _PROXIMITY_SENSOR_HAL_H_
#define _PROXIMITY_SENSOR_HAL_H_

/*-----------------------------------------------------------------------------
  Include Files
-----------------------------------------------------------------------------*/
#include "proximity_sensor.h"

/*------------------------------------------------------------------------------
  Function declarations
------------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  set_proximity_sensor
==============================================================================*/
/**
 * Overrides default sensor implementation while unit testing.
 */
void set_proximity_sensor(UltrasoundProximitySensor *newSensorPtr);

#endif /* _PROXIMITY_SENSOR_HAL_H_ */
