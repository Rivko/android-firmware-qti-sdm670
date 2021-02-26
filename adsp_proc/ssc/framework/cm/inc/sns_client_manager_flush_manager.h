#pragma once
/*=============================================================================
  @file sns_client_manager_flush_manager.h

  This module handles all flush requirements for CM.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_client_manager_instance.h"

/*=============================================================================
  #Defines
  ===========================================================================*/

#define SNS_CM_IS_FLUSH_REQ(x)  ((SNS_CM_NO_FLUSH == x->flush_type) &&\
                                !(x->is_streaming_sensor))
/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * Handles the flush request from the external client
 *
 * @param[i] cm_instance    Current CM instance.
 * @param[i] cm_request     The original request for which
 *                          flush is being triggered.
 * @param[i] data_stream    The data stream associated with the cm request.
 *
 * @return   SNS_RC_FAILED - If flush initiation failed
 *           SNS_RC_SUCCESS
 */
sns_rc sns_cm_handle_client_flush_request(
   sns_sensor_instance *cm_instance,
   sns_cm_request *cm_request,
   sns_data_stream *data_stream);

/**
 * Handles the flush event from the sensor
 *
 * @param[i] this        The CM instance.
 * @param[i] event       The flush event.
 * @param[i] cm_request  The original request for which
                         flush is being triggered.
 */
void sns_cm_handle_client_flush_event(
   sns_sensor_instance *this,
   sns_sensor_event *event,
   sns_cm_request *cm_request);

