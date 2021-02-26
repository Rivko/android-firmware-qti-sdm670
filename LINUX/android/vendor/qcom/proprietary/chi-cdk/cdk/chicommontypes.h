////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chicommontypes.h
/// @brief Defines Chi Common Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHICOMMONTYPES_H
#define CHICOMMONTYPES_H

#include "camxcdktypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Structure describing the rectangle coordinates
typedef struct
{
    INT32   left;   ///< The x coordinate of the Rectangle
    INT32   top;    ///< The y coordinate of the Rectangle
    INT32   width;  ///< The width of the Rectangle
    INT32   height; ///< The height of the Rectangle
} CHIRectangle;

/// @brief: Structure representing generic two dimensional value
typedef struct
{
    UINT32  width;  ///< The width of the rectangle
    UINT32  height; ///< The height of the rectangle
} CHIDimension;

/// @brief: Structure representing a pixel
typedef struct
{
    INT32   x;  ///< The x coordinate of the Point
    INT32   y;  ///< The y coordinate of the Point
} CHIPoint;

/// @brief Structure defining Date/time
struct CHIDateTime
{
    UINT32  seconds;                    ///< Seconds after the minute 0-61*
    UINT32  microseconds;               ///< Microseconds 0-999999
    UINT32  minutes;                    ///< Time in minutes 0-59
    UINT32  hours;                      ///< Time in hours 0-23
    UINT32  dayOfMonth;                 ///< Day of month 1-31
    UINT32  month;                      ///< Months since January 0-11, where 0 is January
    UINT32  year;                       ///< Year since 1900
    UINT32  weekday;                    ///< Day of week 0-6
    UINT32  yearday;                    ///< Day of year 0-365
    UINT32  dayLightSavingTimeFlag;     ///< Day light saving time flag
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHICOMMON_H

