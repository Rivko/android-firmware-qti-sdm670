#pragma once
/*=============================================================================
  @file sns_client_manager_instance.h

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  This module
  will translate incoming QMI request messages into Client Request objects,
  and similarly will translate Sensor Events into QMI indications.  This
  translation will be trivial, and this module will only concern itself with
  the requested batch period and the client processor.  The configuration
  request will be passed along to the appropriate Sensor, and this module
  will batch all data received (as appropriate).

  This file only handles the sensor instance part of the client manager and
  interacts with the QMI interface.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include "qmi_csi.h"
#include "sns_client_api_v01.h"
#include "sns_client_manager.h"
#include "sns_client_manager_batch_timer.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_osa_thread.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_sensor_uid.h"
#include "sns_std_type.pb.h"

/*=============================================================================
  Defines
  ===========================================================================*/

/*Used to drop 10 data events on the batch buffer list for a request*/
#define SNS_CM_BATCH_BIN_DROP_COUNT               10

#ifndef ABS
#define ABS(VAL) (((VAL)>0)?(VAL):(-(VAL)))
#endif

#define SNS_CM_REQ_CRIT_MSG_ID_MAX 255
#define SNS_CM_REQ_DATA_EVENT_CNT 5

#define SNS_CM_FLUSHP_PERIOD_PADDING_TIME_USEC 750000ULL

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/*
 * Structure used to maintain the batch timeout details
 * for a request.
 */
typedef struct
{
  /* Timestamp when the timer call back event happens */
  sns_time timestamp;
  sns_std_client_processor proc_type;
} timeout_event;

/**
 * State associated with a particular client.  This serves as the Sensor
 * Instance state.
 */
typedef struct sns_cm_inst_state
{
  /* QMI client handle as received in the connect_cb */
  qmi_client_handle qmi_handle;
  /* Unique identifier for this client */
  uint32_t client_id;
  /* Busy Flag per QMI */
  bool busy;
  /* Whether sns_client_jumbo_report_ind_msg_v01 can be sent */
  bool use_jumbo_report;
  /* The list of sns_cm_request per connection/instance */
  sns_isafe_list req_list;
} sns_cm_inst_state;


/*
 * Structure used to copy the event into the batch list.
 */
typedef struct sns_cm_batch_bin
{
  /* Stores items of type sns_cm_batch_bin */
  sns_isafe_list_item item;
  sns_sensor_event event;
} sns_cm_batch;

/*
 * Structure used to track the data event message id and time stamp.
 */
typedef struct sns_cm_data_evnt_msg
{
  /* Stores items of type sns_cm_batch_bin */
  sns_isafe_list_item item;
  int msg_id;
  sns_time ts;
} sns_cm_data_evnt_msg;

/*
 * Structure used to read the attribute value
 */
typedef struct sns_cm_read_attrib
{
  sns_sensor_instance *instance;
  int32_t attr_id;
  int32_t attr_value;
  sns_cm_request *cm_request;
}sns_cm_attrib;

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * * This function handles the scenario of a client resuming from a busy state.
 *
 * @param[i] this              The handle associated with a QMI connection.
 *
 */
void sns_cm_handle_client_resume(
   sns_fw_sensor_instance *this);

/**
 * This function flushes all the requests on an instance belonging
 * to a particular processor.
 *
 * @param[i] instance       The CM sensor instance.
 * @param[i] proc_type      The processor type for which the data should be sent.
 *
 */
void sns_cm_send_flush(
   sns_sensor_instance *instance,
   sns_std_client_processor proc_type);

/**
 * This function sends all data for a particular processor.
 *
 * @param[i] sensor       The sensor which generated the event.
 * @param[i] proc_type    The processor to which the event is to be sent.
 * @param[i] drop_events The low memory flag.
 *
 *
 *
 * @return SNS_RC_FAILED or SNS_RC_SUCCESS.
 */
sns_rc sns_cm_send_all_data_for_proc(
   sns_fw_sensor *sensor,
   sns_std_client_processor proc_type,
   bool drop_events);

/**
 * This function copies all batch buffers from uImage heap to
 * DDR heap.
 *
 * @param[i] cm_sensor   The framework sensor pointer for CM sensor.
 */
void sns_cm_handle_flush_to_ddr_memory(
    sns_fw_sensor *cm_sensor);

