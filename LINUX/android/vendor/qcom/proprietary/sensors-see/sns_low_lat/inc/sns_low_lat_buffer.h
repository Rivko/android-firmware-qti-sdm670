#ifndef SNS_LOW_LAT_BUFFER_H_
#define SNS_LOW_LAT_BUFFER_H_

/*============================================================================
  @file sns_low_lat_buffer.h

  @brief
  API functions, data structures, and constants used by the Low Latency Stream
  Library in relation with its output buffer functionality

  @note
  File line length should generally be limited to <= 80 columns.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

#include <stdint.h>

/**< Definition for packed structures */
#if defined __GNUC__
  #define PACK(x)   x __attribute__((__packed__))
#elif defined _WIN32
  #define PACK(x)   x
#endif

#define SNS_LOW_LAT_NUM_AXES     3


/**
 * Defines the sensors available for HDRLL mode.
 */
typedef enum sns_low_lat_sensor_type_e
{
  SNS_LOW_LAT_ACCEL = 0,
  SNS_LOW_LAT_GYRO,

  SNS_LOW_LAT_NUM_SENSOR_TYPES, // NOTE: This must always be the last entry in the
                              //       sensor_type enum!

  SNS_LOW_LAT_MAX_SENSOR_TYPE = 0xFFFFFFFF // Force sensor_type to a 32-bit int
} sns_low_lat_sensor_type_e;

/**
 * 64-byte aligned structure
 */
typedef PACK(struct) sns_low_lat_circ_buf_header
{
  union
  {
    PACK(struct)
    {
      uint32_t                  most_recent_index;
      uint32_t                  max_samples;
    } info;

    uint8_t                     reserved[64];
  };
} sns_low_lat_circ_buf_header;


typedef PACK(struct) sns_low_lat_sample
{
  // PEND: uint64_t suid
  sns_low_lat_sensor_type_e     sensor_type;
  uint64_t                      timestamp; // PEND: Consider placing this at the top of the struct for packing purposes
  /**< The accurate timestamp of the sample. This is adjusted for the device's
     internal filter delay. (in QTimer ticks, 19.2 MHz clock) */
  float                         sample[SNS_LOW_LAT_NUM_AXES];
  /**< Three-element vector representing the sensor data. These samples must
     follow the Android Specification:
         Accel  - m/s/s
         Gyro   - rad/s
     */
} sns_low_lat_sample;


/**
 * 64-byte aligned structure
 */
typedef PACK(struct) sns_low_lat_circ_buf_sample
{
  union
  {
    sns_low_lat_sample          data;

    uint8_t                     reserved[64];
  };
} sns_low_lat_circ_buf_sample;

typedef PACK(struct) sns_low_lat_circ_buf
{
  // Circular buffer header information
  sns_low_lat_circ_buf_header   header;

  // Extendable array for the samples
  sns_low_lat_circ_buf_sample   samples[1];
} sns_low_lat_circ_buf;

#endif /* SNS_LOW_LAT_BUFFER_H_ */
