#pragma once

/*============================================================================
  @file sns_low_lat_buffer_android.h

  @brief
  API functions, data structures, and constants used by the Low Latency Stream
  Library in relation with its Android-style output buffer functionality

  @note
  File line length should generally be limited to <= 80 columns.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <comdef.h>
#include <stdint.h>

/* Definition for packed structures */
#define PACK(x)   x __attribute__((__packed__))

/* Taken from sensors_base.h */
#define SENSOR_TYPE_ACCELEROMETER                       (1)
#define SENSOR_TYPE_GEOMAGNETIC_FIELD                   (2)
#define SENSOR_TYPE_MAGNETIC_FIELD                      SENSOR_TYPE_GEOMAGNETIC_FIELD
#define SENSOR_TYPE_GYROSCOPE                           (4)
#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED         (14)
#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED              (16)
#define SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED          (35)

/* LLCM private type */
#define SENSOR_TYPE_UNKNOWN                             (-1)

#define SNS_LOW_LAT_SENSOR_EVENT_VERSION                sizeof(sensors_event_t)

/* Taken from sensors.h */
/**
 * sensor event data
 */
/**
 * sensor event data
 */
typedef struct {
    union {
        float v[3];
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
        };
    };
    int8_t status;
    uint8_t reserved[3];
} sensors_vec_t;

/* Taken from sensors.h */
/**
 * uncalibrated accelerometer, gyroscope and magnetometer event data
 */
typedef struct {
  union {
    float uncalib[3];
    struct {
      float x_uncalib;
      float y_uncalib;
      float z_uncalib;
    };
  };
  union {
    float bias[3];
    struct {
      float x_bias;
      float y_bias;
      float z_bias;
    };
  };
} uncalibrated_event_t;

/* Stripped down version of sensors_event_t from sensors.h */
/**
 * Union of the various types of sensor data
 * that can be returned.
 */
typedef PACK(struct) sensors_event_t {
    /* must be sizeof(struct sensors_event_t) */
    int32_t version;

    /* sensor identifier */
    int32_t sensor;

    /* sensor type */
    int32_t type;

    /* reserved */
    int32_t reserved0;

    /* time is in nanosecond */
    int64_t timestamp;

    union {
        union {
            float           data[16];

            /* acceleration values are in meter per second per second (m/s^2) */
            sensors_vec_t   acceleration;

            /* magnetic vector values are in micro-Tesla (uT) */
            sensors_vec_t   magnetic;

            /* orientation values are in degrees */
            sensors_vec_t   orientation;

            /* gyroscope values are in rad/s */
            sensors_vec_t   gyro;

            /* temperature is in degrees centigrade (Celsius) */
            float           temperature;

            /* distance in centimeters */
            float           distance;

            /* light in SI lux units */
            float           light;

            /* pressure in hectopascal (hPa) */
            float           pressure;

            /* relative humidity in percent */
            float           relative_humidity;

            /* uncalibrated gyroscope values are in rad/s */
            uncalibrated_event_t uncalibrated_gyro;

            /* uncalibrated magnetometer values are in micro-Teslas */
            uncalibrated_event_t uncalibrated_magnetic;

            /* uncalibrated accelerometer values
             * in  meter per second per second (m/s^2) */
            uncalibrated_event_t uncalibrated_accelerometer;
        };

        union {
            uint64_t        data[8];

            /* step-counter */
            uint64_t        step_counter;
        } u64;
    };

    /* Reserved flags for internal use. Set to zero. */
    uint32_t flags;

    uint32_t reserved1[3];
} sensors_event_t;

typedef PACK(struct) sns_low_lat_circ_buf_android
{
  /* Extendable array for the samples */
  sensors_event_t samples[1];
} sns_low_lat_circ_buf_android;
