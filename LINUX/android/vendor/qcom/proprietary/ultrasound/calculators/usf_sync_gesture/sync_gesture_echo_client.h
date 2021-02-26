/*===========================================================================
                           sync_gesture_echo_client.h

DESCRIPTION: Implements sync gesture echo client.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __SYNC_GESTURE_ECHO_CLIENT__
#define __SYNC_GESTURE_ECHO_CLIENT__

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "echo_client.h"
#include <SyncGestureExports.h>
#include "usf_echo_calculator.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define OUTPUT_TYPE_GESTURE_EVENT_MASK (1 << OUTPUT_TYPE_GESTURE_EVENT_SHIFT)

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

class SyncGestureEchoClient : public EchoClient
{
private:
  /*----------------------------------------------------------------------------
    Variable Definitions
  ----------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
    Function definitions
  ----------------------------------------------------------------------------*/

  /*============================================================================
    FUNCTION:  get_directions_subset
  ============================================================================*/
  /**
   * Return subset of directions. Return subset if the property
   * debug.gesture_subset is defined and greater than 0 or if working with
   * gesture framework.
   */
  int get_directions_subset(EchoContext *echo_context);

  /*============================================================================
    FUNCTION:  get_gesture_output_decision
  ============================================================================*/
  /**
   * Get decision from suitable sync gesture libraries and return final
   * result decision.
   */
  GestureOutput get_gesture_output_decision(short *packet,
                                            us_all_info *paramsStruct);

public:

  /*============================================================================
    CONSTRUCTOR:
  ============================================================================*/
  SyncGestureEchoClient() {}

  virtual ~SyncGestureEchoClient() {}

  /*============================================================================
    FUNCTION:  get_client_name
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual char *get_client_name();

  /*============================================================================
    FUNCTION:  get_daemon_name
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual char *get_daemon_name();

  /*============================================================================
    FUNCTION:  get_cfg_path
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual char *get_cfg_path();

  /*============================================================================
    FUNCTION:  get_frame_file_path
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual char *get_frame_file_path();

  /*============================================================================
    FUNCTION:  get_post_mortem_raw_file_a_name
  ============================================================================*/
  /**
   * Returns the pid file name
   *
   * @return char* post Mortem raw File A name to be used
   */
    virtual char *get_post_mortem_raw_file_a_name();

  /*============================================================================
    FUNCTION:  get_post_mortem_events_file_a_name
  ============================================================================*/
  /**
   * Returns the pid file name
   *
   * @return char* post Mortem events File A name to be used
   */
    virtual char *get_post_mortem_events_file_a_name();

  /*============================================================================
    FUNCTION:  get_post_mortem_raw_file_b_name
  ============================================================================*/
  /**
   * Returns the pid file name
   *
   * @return char* post Mortem raw File B name to be used
   */
    virtual char *get_post_mortem_raw_file_b_name();

  /*============================================================================
    FUNCTION:  get_post_mortem_events_file_b_name
  ============================================================================*/
  /**
   * Returns the pid file name
   *
   * @return char* post Mortem events File B name to be used
   */
    virtual char *get_post_mortem_events_file_b_name();


  /*============================================================================
    FUNCTION:  get_pid_file_name
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual char *get_pid_file_name();

  /*============================================================================
    FUNCTION:  get_dsp_event_size
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual uint32_t get_dsp_event_size();

  /*============================================================================
    FUNCTION:  get_num_keyboard_keys
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual int get_num_keyboard_keys();

  /*============================================================================
    FUNCTION:  get_keyboard_key_base
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual int get_keyboard_key_base();

  /*============================================================================
    FUNCTION:  get_num_events
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual int get_num_events();

  /*============================================================================
    FUNCTION:  set_algo_dynamic_params
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual void set_algo_dynamic_params(us_all_info *paramsStruct,
                                       EchoContext *echo_context);

  /*============================================================================
    FUNCTION:  echo_lib_init
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual void echo_lib_init(us_all_info *paramsStruct,
                             EchoContext *echo_context);

  /*============================================================================
    FUNCTION:  get_event_output_type
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual uint32_t get_event_output_type();

  /*============================================================================
    FUNCTION:  get_points
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual int get_points(EchoContext *echo_context,
                         short *packet,
                         us_all_info *paramsStruct);

  /*============================================================================
    FUNCTION:  send_keep_alive
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual void send_keep_alive(EchoContext *echo_context,
                         bool keep_alive,
                         us_all_info *paramsStruct);

  /*============================================================================
    FUNCTION:  save_persistent_data
  ============================================================================*/
  /**
   * See function description in base class
   */
  virtual void save_persistent_data(EchoContext *echo_context,
                                    us_all_info *paramsStruct);
};

#endif //__SYNC_GESTURE_ECHO_CLIENT__
