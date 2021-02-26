#pragma once
/*=============================================================================
  @file sns_ext_svc.h

  External Service Sensor

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include "pb.h"
#include "qmi_client.h"
#include "sns_isafe_list.h"

#include "sns_osa_lock.h"
#include "sns_osa_sem.h"
#include "sns_osa_thread.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_instance;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_ext_svc_sensor_state
{
  sns_isafe_list services;

  // Thread which publishes to SEE messages received from QMI
  sns_osa_thread *events_thread;
  // Semaphore to signal when adding to events
  sns_osa_sem *events_sem;
  // Lock protects the events list
  sns_osa_lock *events_lock;
  // Contains sns_ext_svc_event
  sns_isafe_list events;
} sns_ext_svc_sensor_state;

typedef struct sns_ext_svc_inst_state
{
  int32_t svc_id;
  // QMI handle as generated within qmi_client_init_instance
  qmi_client_type qmi_handle;
  // Sensor state, in order to add to events list;
  // Breaks normal SEE requirements.
  sns_ext_svc_sensor_state *sensor_state;
} sns_ext_svc_inst_state;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Get the QMI service object corresponding to the given svc_id.
 *
 * @param[i] svc_list sns_ext_svc_sensor_state::events
 * @param[i] svc_id Identifier from enum sns_ext_svc
 *
 * @return Found service object, or NULL
 */
qmi_idl_service_object_type sns_ext_svc_get(sns_isafe_list*services, uint32_t svc_id);

/**
 * Add new QMI Service Object, and associate it with the given svc_id.
 *
 * @param[i] svc_list sns_ext_svc_sensor_state::events
 * @param[i] svc_id Identifier from enum sns_ext_svc
 * @param[i] svc_obj QMI Service object from IDL file
 */
void sns_ext_svc_add(sns_isafe_list*services, uint32_t svc_id,
    qmi_idl_service_object_type svc_obj);

/**
 * Add a new pending event to the queue.
 *
 * @param[i] instance Instance which needs to send the event
 * @param[i] fields sns_ext_svc_resp_fields or sns_ext_svc_ind_fields
 * @param[i] c_struct Decoded PB message
 * @param[i] msg_id As from sns_ext_svc_msgid
 */
void add_pending_event(struct sns_sensor_instance *instance,
    pb_field_t const *fields, void const *c_struct, uint32_t msg_id);

/**
 * Remove all pending events associated with this Sensor Instance.
 *
 * @param[i] instance
 */
void remove_pending_events(struct sns_sensor_instance const *instance);
