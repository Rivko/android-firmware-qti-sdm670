#pragma once
/*=============================================================================
  @file sns_qcm.h

  Incoming requests to the Framework will arrive via the QSocket Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  The module
  will translate incoming QSocket  messages into Client Request objects,
  and similarly will translate Sensor Events into QSocket messages.

  This file only handles the sensor api part of the module implementation.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_atomic.h"
#include "sns_isafe_list.h"
#include "sns_osa_lock.h"
#include "sns_sensor_uid.h"
#include "sns_std.pb.h"
#include "sns_suid_util.h"
#include "sns_timer.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define QCM_SENSOR_SUID 0x77, 0x64, 0xe6, 0x88, 0x03, 0xd6, 0x73, 0xbf,\
                        0x4f, 0xbd, 0x62, 0x40, 0x45, 0x09, 0x4b, 0xd0

#define SNS_QCM_SENSOR_NAME "qcm"
#define SNS_QCM_VENDOR_NAME "qualcomm"

/*=============================================================================
  Global Data
  ===========================================================================*/

/* QCM Sensor SUID; same as QCM_SENSOR_SUID */
const sns_sensor_uid qcm_sensor_suid;

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * A known processor on the device.  These will be generated dynamically,
 * whenever a new (and unknown) processor is specified in a client request.
 */
typedef struct sns_qcm_proc
{
  /* On sns_qcm_state::processors */
  sns_isafe_list_item item;

  /* The processor type for the client */
  sns_std_client_processor proc_type;
  /* Is the processor awake or suspended */
  bool is_suspended;
  /* When a batch of data is sent for any reason, the following will be set
   * to the current time + the minimum batch period of all clients from this
   * processor (0 if no clients).*/
  sns_time *next_batch_time;
  /* Minimum batch period amongst all clients from this processor */
  sns_time min_batch_period;
} sns_qcm_proc;

/**
 * A client stream.  A client will have at most one of these per SUID.
 *
 * All fields are protected by sns_qcm_client::streams_lock.  I.e. Mutations
 * to any object of this type must hold the write streams_lock; accesses
 * to any object must hold the read streams_lock.
 */
typedef struct sns_qcm_stream
{
  /* On sns_qcm_client::streams */
  sns_isafe_list_item item;
  /* Client which "owns" this stream */
  struct sns_qcm_client *client;

  /* Client requested batch period */
  sns_time batch_period;
  /* Flush period specified by the client; batch_period if not specified */
  sns_time flush_period;

  bool flush_only;
  /* Client-specified processor */
  sns_qcm_proc *processor;
  /* Whether to wakeup the processor when data is available */
  bool wakeup;

  /* Data stream created with the sensor for this request. */
  struct sns_data_stream *data_stream;
  /* SUID of data received on this stream */
  sns_sensor_uid suid;
  /* FIFO storing batched data for this stream */
  struct sns_qcm_fifo *fifo;
} sns_qcm_stream;

/**
 * An external client, which has zero or more data streams.
 */
typedef struct sns_qcm_client
{
  /* On sns_qcm_state::clients */
  sns_isafe_list_item item;
  /* Connection handle associated with this client */
  struct sns_qcm_conn *conn;

  /* Busy Flag per QSocket */
  bool busy;
  /* The list of sns_qcm_stream per connection/client */
  sns_isafe_list streams;
  sns_osa_lock streams_lock;
} sns_qcm_client;

/**
 * The QSocket Client Manager state.
 */
typedef struct sns_qcm_state
{
  struct sns_qcm_service *service;

  /* QCM Sensor reference */
  struct sns_sensor *sensor;

  /* Easy reference to the SEE Stream Service */
  struct sns_stream_service *stream_service;

  /* The stream associated with remote proc state sensor */
  struct sns_data_stream *rps_stream;
  /* Timer Stream used to register service OOM callback */
  struct sns_data_stream *timer_stream;
  /* remote_proc_state, timer */
  SNS_SUID_LOOKUP_DATA(2) suid_lookup_data;

  /* All known client processors;
   * Entries are added not never removed. */
  sns_isafe_list processors;
  sns_osa_lock processors_lock;

  /* Active clients */
  sns_isafe_list clients;
  sns_osa_lock clients_lock;

  /* A service OOM timer is pending */
  _Atomic unsigned int oom_active;
} sns_qcm_state;
