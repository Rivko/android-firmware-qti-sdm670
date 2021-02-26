////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013,2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _IS_SNS_LIB_H
#define _IS_SNS_LIB_H


/* Co-ordinate convention */
#include <stdint.h> // definition of fixed width integers

/* Gyro Sensor New Co-ordinate system
    https://developer.android.com/guide/topics/sensors/sensors_overview.html#sensors-coords

    The following crude sketch is that of a device sitting face up say on a table in the portrait orientation. Then

    +X  axis is along the (typical) longer axis of the screen, and points away from the observer.
    +Y  axis is along the (typical) shorter axis of the screen, and points to the observer's right.
    +Z  axis is towards the earth - and points towards the earth's center.

    (+Y)      _  (+Z - outfrom the page)
    I  /|\    /|
    I   |    /
    ----|---/---
    |   |  /   |
    |   | /    |
    |   | ________________\ (+X)
    |          |          /
    |          |
    |          |
    |    O     |
    ------------
*/

/* Camera Orientation
    https://developer.android.com/reference/android/hardware/Camera.CameraInfo.html#orientation
*/

/* EIS Co-ordinate system, units and conventions

    The following crude sketch is that of a device sitting face up say on a table in the portrait orientation. Then

    +X  axis is along the (typical) longer axis of the screen, and points away from the observer.
    +Y  axis is along the (typical) shorter axis of the screen, and points to the observer's right.
    +Z  axis is towards the earth - and points towards the earth's center.

       (+X)  _ (+Z - into the page)
    I  /|\    /|
    I   |    /
    ----|---/---
    |   |  /   |
    |   | /    |
    |   | ________________\ (+Y)
    |          |          /
    |          |
    |          |
    |    O     |
    ------------
*/

/* Time stamps are in microseconds as measured by a common clock on the application pocessor, We use CLOCK_REALTIME */
/* Gyro - samples are in radian/second, in Q16 format, and follow the co-ordinate convention below                 */
/* Quaternion -

   are unit quaternions - {w, x, y, z} where
    w = cos(theta/2),
    x = Axis_x * sin(theta/2)
    y = Axis_y * sin(theta/2)
    z = Axis_z * sin(theta/2)

   represents a rotation of theta degrees, about the axis Axis_x*i + Axis_y*j + Axis_z*k
   rotations are positive in the anticlockwise direction.
*/
#define MAX_GYRO_SAMPLES        (512 * 500/400)
#define MAX_GYRO_SAMPLE_RATE    500


typedef struct {
  int32_t   data[3];  /* 0==x, 1==y, 2==z */
  uint64_t  ts;       /* time stamp in microseconds */
} gyro_sample_s;

typedef struct {
  uint32_t        num_elements;   /* number of elements in the buffer */
  gyro_sample_s*  gyro_data;      /* gyro data */
} gyro_data_s;

typedef struct {
  uint64_t t_start;
  uint64_t t_end;
} time_interval_s;

typedef struct {
  double      data[4];         /* (Quat) = data[0] + data[1]*i + data[2]*j + data[3]*k */
  uint32_t    n_gyro_samples;  /* this is the number of samples used during the
                                  integration - needed for alpha calculation */
} quaternion_s;


// TODO: (rgaizman) remove from here and move only to tests, remove call from eis2 and change so that the alg will get input gyro readings

#endif
