#pragma once
/*=============================================================================
  @file sns_client_manager_batch_timer.h

  This module handles all batch timer functionality

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_client_manager.h"
#include "sns_fw_sensor.h"
#include "sns_isafe_list.h"
#include "sns_std_type.pb.h"
#include "sns_timer.h"

/*=============================================================================
  Constants
  ===========================================================================*/

// Fastest batch period supported by CM in nanoseconds.
#define SNS_CM_SHORTEST_BATCH_PERIOD_NS   1000000ULL

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

typedef struct timer_user_info
{
  uint32_t client_id;
  uint32_t req_id;
  sns_std_client_processor proc_type;
}timer_user_info;

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * This function evaluates the timeout needed for a processor type and updates
 * the CM sensor state's processor specific timer info structure with the timeout.
 *
 * @param[i] this            The CM sensor.
 * @param[i] proc_info       The processor info structure.
 * @param[i] time_left       The time left in ticks from previous timer stop.
 * @param[i] cm_req          The client request being modified/added/removed.
 */
void sns_cm_configure_batch_timer(
    sns_fw_sensor *this,
    cm_proc_info *proc_info,
    sns_time time_left,
    sns_cm_request* cm_req);

/**
 * This function triggers the flushing of the sensor based on the delivery type.
 *
 * @param[i] instance        The CM instance, whose timers need to be updated.
 * @param[i] delivery_type   The delivery type being used to request flush.
 * @param[i] proc_info       The processor info for which the timer fired.
 */
void sns_cm_flush_instance_reqs(
   sns_sensor_instance * instance,
   sns_client_delivery delivery_type,
   cm_proc_info *proc_info);

/**
 * This function handles the timer call back event for a processor.
 *
 * @param[i] sensor      The CM sensor.
 * @param[i] timer_event The timer event.
 *
 */
void sns_cm_handle_timer_event(
    sns_fw_sensor *sensor,
    cm_timer_event *timer_event);

/**
 * This function register a timer with the timer thread for a processor.
 *
 * @param[i] cm_state    The CM sensor state.
 * @param[i] proc_info   The processor info structure.
 * @param[i] this        The CM sensor.
 *
 *
 * @return: True - Timer Registered Successfully.
 *          False
 */
bool sns_cm_register_batch_timer (
   sns_cm_state *cm_state,
   cm_proc_info *proc_info,
   sns_sensor *this);

/**
 * This function flushes data for all clients for a particular
 * processor.
 *
 * @param[i] sensor           The framework sensor pointer for CM sensor.
 * @param[i] delivery_type    The delivery type of the timer.
 * @param[i] proc_info        The processor whose batch timers need to be updated.
 */
void sns_cm_flush_all_clients_for_proc(
    sns_sensor *const sensor,
    sns_client_delivery delivery_type,
    cm_proc_info *proc_info);

/**
 * This function starts the timer associated with a processor type.
 *
 * @param[i] cm_state   The CM sensor state.
 * @param[i] proc_info  The structure associated with the processor.
 * @param[i] this       The CM sensor pointer.
 *
 * @return   True       Timer started successfully,false otherwise .
 */

bool sns_cm_start_timer(
    sns_cm_state *cm_state,
    cm_proc_info *proc_info,
    sns_sensor *this);

/**
 * This function stops the timer associated with a processor type.
 *
 * @param[i] cm_state   The CM sensor state.
 * @param[i] proc_info  The structure associated with the processor.
 * @param[o] time_left  The time left for the timer to fire, when it was stopped.
 */
void sns_cm_stop_batch_timer(
    sns_cm_state *cm_state,
    cm_proc_info *proc_info,
    sns_time * time_left);

/**
 * This function creats a timer and associates it with the
 * processor type
 *
 * @param[i] proc_info  The structure associated with the processor.
 */
void sns_cm_create_batch_timer(
   cm_proc_info *proc_info);

