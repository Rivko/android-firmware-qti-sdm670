#pragma once
/*=============================================================================
  @file sns_signal.h

  Provides internal signal service that can spawn an internal thread to handle
  asynchronus events

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#include "sns_island.h"
#include "sns_osa_thread.h"
#include "sns_rc.h"

#define SNS_SIGNAL_MAX_SIGNALS 32
struct sns_fw_sensor;
struct sns_sensor;

/**
 *  Callback that is called when a registered signal is
 *  received by a signal thread
 *
 *  @Note: The library mutex of the sensor that owns this thread
 *         will be locked prior to calling the callback
 *
 *  @param[i] sensor    Sensor that owns the signal thread
 *  @param[i] signal    Signal that was hit
 *  @param[i] user_arg  User defined arg that is recorded at the
 *                      time of registering for a new signal
 */
typedef void (*sns_handle_signal_event)(struct sns_sensor *sensor,
    uint32_t signal,
                                                 void const *user_args);

/**
 *  Signal task function which will be the main function for the
 *  newly created task
 *
 *  @param[i] args  Arguments to the main function for this task
 */
typedef void (*sns_signal_task_ptr)( void *args);

typedef struct
{
  /* Thread handle provide by sns_osa */
  sns_osa_thread *thread;

  /* Sensor that owns this thread */
  struct sns_sensor *sensor;

  /* Set to true when a signal registered */
  bool registered[SNS_SIGNAL_MAX_SIGNALS];

  /* User arg for a registered */
  void *user_args[SNS_SIGNAL_MAX_SIGNALS];

  /* Callback to be called when signalled */
  sns_handle_signal_event cb;

  /* Set to true when a thread is active*/
  bool active;

  /* Island client*/
  sns_island_client_handle island_client;
}sns_signal_thread;


/**
 * Creates and records a new signal thread that is owned by a
 * sensor.
 *
 * @param[i] sensor            Sensor that will own this thread
 * @param[i] thread_name       String that will be used to name
 *                              this thread
 * @param[i] thread_priority   Priority of thread
 * @param[i] cb                Callback to be called when
 *                              signalled
 * @paran[i] fn                Signal task main function pointer
 * @param[i] island_thread     true if task to be created is island
 *                             thread, false otherwise
 * @return pointer to the spawned thread if successfull. NULL
 *         otherwise
 */
sns_signal_thread *
sns_signal_thread_create(struct sns_sensor *sensor,
                                  char const *thread_name,
                                  uint8_t thread_priority,
                                  sns_handle_signal_event cb,
                                  sns_signal_task_ptr fn,
                                  bool island_thread);

/**
 * Destroys signal thread
 *
 * @param[i] thread      Thread to be destroyed
 *
 * @return SNS_RC_SUCCESS if successful
 */
sns_rc
sns_signal_thread_destroy(sns_signal_thread *thread);

/**
 * Register for a signal in a thread.
 *
 * @param[i] thread        Thread that will be signalled
 * @param[i] user_arg      User provided args that will be
 *                         passed with the
 *                         sns_handle_signal_event
 *                         callback function when signalled
 * @param[o] new_signal    New signal that is assigned
 *
 * @note The sensor's library lock must be acquired before
 *       calling this function
 *
 * @return SNS_RC_SUCCESS         if successful
 *         SNS_RC_NOT_AVAILABLE   if this thread can not accept
 *                                  any more signals
 *         SNS_RC_FAILED          otherwise
 */
sns_rc
sns_signal_register_signal(sns_signal_thread *thread,
                                    void *user_arg,
                                    uint32_t *new_signal);

/**
 * Unregister a signal in a thread.
 *
 * @param[i] thread        Thread that is currently signalled
 * @param[i] signal        Signal to be unregistered
 *
 * @note The sensor's library lock must be acquired before
 *       calling this function
 *
 * @return SNS_RC_SUCCESS         if successful
 *         SNS_RC_FAILED          otherwise
 */
sns_rc
sns_signal_unregister_signal(sns_signal_thread *thread,
                                      uint32_t signal);

