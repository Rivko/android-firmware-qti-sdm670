/*===========================================================================
                           echo_client.h

DESCRIPTION: Echo client interface class.
  Holds function definitions and enums for concrete echo clients.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __ECHO_CLIENT__
#define __ECHO_CLIENT__
/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "usf_log.h"
#include <cutils/properties.h>
#include <stdlib.h>
#include <errno.h>
#include <ual_util.h>
#include "usf_echo_calculator.h"
#include "framework_adapter.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
class EchoClient;

/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/
/**
  EchoStats holds information about statistics from
  usf_sync_echo running.
*/
typedef struct
{
  int   m_nPointsCalculated;
  int   m_nTotalFrames;
  int   m_nLostFrames;
  int   m_nOutOfOrderErrors;
} EchoStats;

/**
  echo_context holds information needed for Sync Echo
  calculation.
*/
typedef struct
{
  signed char       *m_echo_workspace;
  usf_event_type    *m_events; // Array of struct from sys/stat.h
  bool              m_send_points_to_ual;
  bool              m_send_points_to_socket;
  int               m_next_frame_seq_num;
  int16_t*          m_pattern;
  uint32_t          m_pattern_size; // In bytes
  // Mapping from echo received by the library to a key
  uint8_t           *m_keys;
  int               m_last_event_seq_num;
  bool              m_stub;
  EchoClient        *m_echo_client;
  FrameworkAdapter  *m_adapter;
  EchoStats         m_echoStats;
  int               m_s_eventCounter;
  uint32_t          m_post_mortem_current_frame_count;
  uint32_t          m_post_mortem_max_frame_count;
  uint32_t          m_post_mortem_active_file;
  bool              m_post_mortem_enable;
  FILE*             m_post_mortem_raw_file_handle_a;
  FILE*             m_post_mortem_raw_file_handle_b;
  FILE*             m_post_mortem_event_file_handle_a;
  FILE*             m_post_mortem_event_file_handle_b;
} EchoContext;

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/
/*============================================================================
  CLASS:  FrameworkAdapter
============================================================================*/
/**
 * This class serves as an interface to echo concrete client.
 * The echo concrete classes are meant to implement the differences between
 * different echo concrete clients.
 */
class EchoClient
{
protected:
  /*============================================================================
    CONSTRUCTOR:  EchoClient
  ============================================================================*/
  EchoClient() {}

public:
  /*============================================================================
    D'TOR:  EchoClient
  ============================================================================*/
  virtual ~EchoClient() {}

  /*============================================================================
    FUNCTION:  get_num_keyboard_keys
  ============================================================================*/
  /**
   * This function returns the number of keyboard keys supported
   * for in the daemon.
   *
   * @return int The number of keyboard keys supported
   */
  virtual int get_num_keyboard_keys() = 0;

  /*============================================================================
    FUNCTION:  get_keyboard_key_base
  ============================================================================*/
  /**
   * This function returns the key base value used.
   *
   * @return int The keyboard key base value
   */
  virtual int get_keyboard_key_base() = 0;

  /*============================================================================
    FUNCTION:  get_num_events
  ============================================================================*/
  virtual int get_num_events() = 0;

  /*============================================================================
    FUNCTION:  set_algo_dynamic_params
  ============================================================================*/
  /**
   * This function sets the algorithm dynamic parameters.
   *
   * @param paramsStruct The paramsStruct struct that contains all
   *                     the configuration file values.
   * @param echo_context The context struct of the daemon.
   */
  virtual void set_algo_dynamic_params(us_all_info *paramsStruct,
                                       EchoContext *echo_context) = 0;

  /*============================================================================
    FUNCTION:  get_client_name
  ============================================================================*/
  /**
   * Returns the client's name
   *
   * @return char* Client's name
   */
  virtual char *get_client_name() = 0;

  /*============================================================================
    FUNCTION:  get_daemon_name
  ============================================================================*/
  /**
   * Returns the daemons name
   *
   * @return char* Daemon name
   */
  virtual char *get_daemon_name() = 0;

  /*============================================================================
    FUNCTION:  get_cfg_path
  ============================================================================*/
  /**
   * Returns the cfg path to be used
   *
   * @return char* Cfg file path
   */
  virtual char *get_cfg_path() = 0;

  /*============================================================================
    FUNCTION:  get_post_mortem_raw_file_a_name
  ============================================================================*/
  /**
   * Returns the post mortem raw file a name
   *
   * @return char* post mortem raw file a name to be used
   */
  virtual char* get_post_mortem_raw_file_a_name() = 0;

  /*============================================================================
    FUNCTION:  get_post_mortem_events_file_a_name
  ============================================================================*/
  /**
   * Returns the post mortem events file a name
   *
   * @return char* post mortem events file a name to be used
   */
  virtual char* get_post_mortem_events_file_a_name() = 0;

  /*============================================================================
    FUNCTION:  get_post_mortem_raw_file_b_name
  ============================================================================*/
  /**
   * Returns the post mortem raw file b name
   *
   * @return char* post mortem raw file b name to be used
   */
  virtual char* get_post_mortem_raw_file_b_name() = 0;

  /*============================================================================
    FUNCTION:  get_post_mortem_events_file_b_name
  ============================================================================*/
  /**
   * Returns the post mortem events file b name
   *
   * @return char* post mortem events file b name to be used
   */
  virtual char* get_post_mortem_events_file_b_name() = 0;


  /*============================================================================
    FUNCTION:  get_pid_file_name
  ============================================================================*/
  /**
   * Returns the pid file name
   *
   * @return char* Pid file name to be used
   */
  virtual char *get_pid_file_name() = 0;

  /*============================================================================
    FUNCTION:  get_frame_file_path
  ============================================================================*/
  /**
   * Returns the frame file path
   *
   * @return char* Frame file path to be used
   */
  virtual char *get_frame_file_path() = 0;

  /*============================================================================
    FUNCTION:  get_dsp_event_size
  ============================================================================*/
  /**
   * Returns the DSP even size
   *
   * @return uint32_t DSP event size
   */
  virtual uint32_t get_dsp_event_size() = 0;

  /*============================================================================
    FUNCTION:  echo_lib_init
  ============================================================================*/
  /**
   * This functions inits the algorithm library
   *
   * @param paramsStruct The paramsStruct struct that contains all
   *                     the configuration file values.
   * @param echo_context The context struct of the daemon.
   */
  virtual void echo_lib_init(us_all_info *paramsStruct,
                             EchoContext *echo_context) = 0;

  /*============================================================================
    FUNCTION:  get_event_output_type
  ============================================================================*/
  /**
   * Thid function returns the mask for the event output type
   *
   * @return uint32_t The mask for the event output type
   */
  virtual uint32_t get_event_output_type() = 0;

  /*============================================================================
    FUNCTION:  get_points
  ============================================================================*/
  /**
   * This function receives ultrasound data, calls the algorithm
   * library and returns the echo result.
   *
   * @param echo_context The context struct of the daemon.
   * @param packet The ultrasound data to be proccessed
   * @param paramsStruct The paramsStruct struct that contains all
   *                     the configuration file values.
   *
   * @return int The result of the given data
   */
  virtual int get_points(EchoContext *echo_context,
                         short *packet,
                         us_all_info *paramsStruct) = 0;

  /*============================================================================
    FUNCTION:  send_keep_alive
  ============================================================================*/
  /**
   * Send keep alive request through the socket adapter. When
   * keep_alive is true, the other side should not terminate the
   * client, allowing more smooth ultrasound operation.
   *
   * @param echo_context The context struct of the daemon.
   * @param keep_alive true to send keep_alive, false to send stop
   *                   keep_alive
   * @param paramsStruct The paramsStruct struct that contains all
   *                     the configuration file values.
   *
   * @return int The result of the given data
   */
  virtual void send_keep_alive(EchoContext *echo_context,
                         bool keep_alive,
                         us_all_info *paramsStruct) = 0;

  /*============================================================================
    FUNCTION:  save_persistent_data
  ============================================================================*/
  /**
   * Save persistent data to file.
   *
   * @param echo_context The context struct of the daemon.
   * @param paramsStruct The paramsStruct struct that contains all
   *                     the configuration file values.
   */
  virtual void save_persistent_data(EchoContext *echo_context,
                                    us_all_info *paramsStruct) = 0;
};

#endif //__FRAMEWORK_ADAPTER__
