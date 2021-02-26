#pragma once
/*=============================================================================
  @file sns_stream_service.h

  Manages stream requests from Sensors or Sensor Instances.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_rc.h"
#include "sns_sensor_uid.h"
#include "sns_service.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor;
struct sns_sensor_instance;
struct sns_data_stream;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * The Stream Manager.  Will be obtained from sns_service_manager::get_service.
 */
typedef struct sns_stream_service
{
  /* Service information */
  sns_service service;

  /* Public api provided by the framework to be used by the sensor. */
  struct sns_stream_service_api *api;
} sns_stream_service;

typedef struct sns_stream_service_api
{
  uint32_t struct_len;

  /**
   * Initialize a new stream to be used by a Sensor.
   *
   * @param[i] this Stream Service reference
   * @param[i} sensor The Sensor requesting the stream
   * @param[i] sensor_uid To whom to make the connection
   * @param[o] data_stream Stream reference allocated by the Framework
   *
   * @return
   * SNS_RC_POLICY - The Sensor has exceeded the maximum stream count
   * SNS_RC_NOT_AVAILABLE - The given SUID is not presently available
   * SNS_RC_SUCCESS
   */
  sns_rc (*create_sensor_stream)(
    sns_stream_service *this,
    struct sns_sensor *sensor,
    sns_sensor_uid sensor_uid,
    struct sns_data_stream **data_stream);

  /**
   * Initialize a new stream to be used by a Sensor Instance.
   *
   * @param[i] this Stream Service reference
   * @param[i} sensor The Sensor Instance requesting the stream
   * @param[i] sensor_uid To whom to make the connection
   * @param[o] data_stream Stream reference allocated by the Framework
   *
   * @return
   * SNS_RC_POLICY - The Sensor has exceeded the maximum stream count
   * SNS_RC_NOT_AVAILABLE - The given SUID is not presently available
   * SNS_RC_SUCCESS
   */
  sns_rc (*create_sensor_instance_stream)(
    sns_stream_service *this,
    struct sns_sensor_instance *instance,
    sns_sensor_uid sensor_uid,
    struct sns_data_stream **data_stream);

  /**
   * Remove a stream created by create_sensor_stream/
   * create_sensor_instance_stream.
   *
   * @param[i] this Stream Service reference
   * @param[i] data_stream Data Stream to remove and free
   *
   * @return
   * SNS_RC_SUCCESS
   */
  sns_rc (*remove_stream)(
    sns_stream_service *this,
    struct sns_data_stream *data_stream);
} sns_stream_service_api;
