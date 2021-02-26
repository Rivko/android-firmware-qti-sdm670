#pragma once
/*=============================================================================
  @file sns_fw_stream_service.h

  Private state to be used by the Framework for the Stream Manager.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_isafe_list.h"
#include "sns_sensor_uid.h"
#include "sns_stream_service.h"
#include "sns_thread_manager.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_request;
struct sns_fw_request;
struct sns_fw_sensor_event;
struct sns_fw_data_stream;
struct sns_fw_sensor;
struct sns_fw_sensor_instance;
struct sns_thread_mgr_task;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Private state to be used by the Framework for the Stream Manager.
 */
typedef struct sns_fw_stream_service
{
  sns_stream_service service;
} sns_fw_stream_service;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize and return a handle to the stream service.
 */
sns_fw_stream_service* sns_stream_service_init(void);

/**
 * Add new event to the stream's event array.
 *
 * @param[io] stream This data stream
 * @param[i] fw_event Event to add (event is located on global circular buffer)
 */
void sns_stream_service_add_event(struct sns_fw_data_stream *stream,
    struct sns_fw_sensor_event *fw_event);

/**
 * Add a pending request to queue to be processed.
 */
void sns_stream_service_add_request(struct sns_fw_data_stream *stream,
    struct sns_request *request);
