/*===========================================================================
                           proximity_sensor_policy.cpp

DESCRIPTION: Implements the Proximity Sensor Policy

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*-----------------------------------------------------------------------------
  Includes
-----------------------------------------------------------------------------*/
#include "proximity_sensor_policy.h"

/*-----------------------------------------------------------------------------
  Macros and constants
-----------------------------------------------------------------------------*/

static const float MAX_RANGE = 5.0f; // in cm
static const float RESOLUTION = 1.0f; // in cm
static const float POWER = 6.0f; // in mA
static const float NEAR_RANGE_VALUE = 0.0f; // in cm
static const float FAR_RANGE_VALUE = MAX_RANGE;
static const int MAX_DELAY = 1;

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getMaxRange
 ============================================================================*/
/**
 Returns maximum range of sensor in cm
 */
float ProximitySensorPolicy::getMaxRange() {
    return MAX_RANGE;
}

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getResolution
 ============================================================================*/
/**
 Returns resolution of sensor in cm
 */
float ProximitySensorPolicy::getResolution() {
    return RESOLUTION;
}

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getPower
 ============================================================================*/
/**
 Returns estimated power of sensor in mA
 */
float ProximitySensorPolicy::getPower() {
    return POWER;
}

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getNearRangeValue
 ============================================================================*/
/**
 Returns value that should be returned when "near" in cm
 */
float ProximitySensorPolicy::getNearRangeValue() {
    return NEAR_RANGE_VALUE;
}

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getFarRangeValue
 ============================================================================*/
/**
 Returns value that should be returned when "far" in cm
 */
float ProximitySensorPolicy::getFarRangeValue() {
    return FAR_RANGE_VALUE;
}

/*============================================================================
 FUNCTION:  ProximitySensorPolicy::getMaxDelay
 ============================================================================*/
/**
 Returns max delay
 */
int ProximitySensorPolicy::getMaxDelay() {
    return MAX_DELAY;
}