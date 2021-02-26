/*===========================================================================
                           proximity_socket_adapter.h

DESCRIPTION: proximity_socket_adapter is the header file for the
             ProximitySocketAdapter class which implements all needed
             functionality to send proximity events to socket.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef __PROXIMITY_SOCKET_ADAPTER__
#define __PROXIMITY_SOCKET_ADAPTER__

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "framework_adapter.h"
#include <usf_unix_domain_socket.h>

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/
extern "C" FrameworkAdapter *get_adapter(char *gesture_fw_lib_path);

class ProximitySocketAdapter : public FrameworkAdapter
{
private:
  /**
   * m_proximity_data_socket is pointer to the thread which
   * handles data socket communication with the service.
   */
  DataUnSocket *m_proximity_data_socket;

  /**
   * The daemon's pid
   */
  int daemon_pid_;

  /**
   * Holds the adapter's current status
   */
  adapter_status_t status;

  /**
   * Holds the adapter's cfg parameters
   */
  cfg_params_t cfg_params;

  /*----------------------------------------------------------------------------
  Function definitions
  ----------------------------------------------------------------------------*/
  /*============================================================================
    CONSTRUCTOR:  GsBusAdapter
  ============================================================================*/
  ProximitySocketAdapter(const ProximitySocketAdapter &to_copy);

  /*============================================================================
    FUNCTION:  operator=
  ============================================================================*/
  ProximitySocketAdapter &operator=(const ProximitySocketAdapter &other);

protected:
  /*============================================================================
    FUNCTION:  activate
  ============================================================================*/
  /**
   * This function does nothing.
   */
  virtual void activate();


  /*============================================================================
    FUNCTION:  deactivate
  ============================================================================*/
  /**
   * This function does nothing.
   */
  virtual void deactivate();


public:
  /*============================================================================
    CONSTRUCTOR:
  ============================================================================*/
  ProximitySocketAdapter();

  /*============================================================================
    FUNCTION:  get_config
  ============================================================================*/
  virtual cfg_params_t &get_config();

  /*============================================================================
    FUNCTION:  wait_n_update
  ============================================================================*/
  /**
   * This function returns the current status.
   *
   * @return adapter_status_t The current adapter status.
   */
  virtual adapter_status_t wait_n_update();


  /*============================================================================
    FUNCTION:  get_status
  ============================================================================*/
  /**
   * Returns the current adapter status.
   *
   * @return adapter_status_t The current adapter status.
   */
  virtual adapter_status_t get_status();

  /*============================================================================
    FUNCTION:  get_event_mapping
  ============================================================================*/
  /**
   * Returns the event mapping state of the adapter.
   * The event mapping state indicates whether the adapter expects a daemon
   * mapped events or regular event index when sending an event to the
   * framework.
   *
   * @return event_mapping_t MAPPED - If the current adapter
   *                                  expects mapped events.
   *                         NOT_MAPPED - If the current adapter
   *                                  does not expect mapped
   *                                  events.
   */
  virtual event_mapping_t get_event_mapping();

  /*============================================================================
    FUNCTION:  disconnect
  ============================================================================*/
  /**
   * This function cleans up the socket instance.
   * It always succeeds.
   *
   * @return int 0 - success.
   */
  virtual int disconnect();


  /*============================================================================
    FUNCTION:  send_event
  ============================================================================*/
  /**
   * This functions sends the proximity events to the proximity
   * socket framework.
   *
   * @param event the event to send
   * @param event_source the source of event
   * @param velocity the velocity of the proximity
   *
   * @return int 0 success
   *             1 failure to send event
   *             -1 proximity ignored
   */
  virtual int send_event(int event, event_source_t event_source, int velocity);

  /*============================================================================
    FUNCTION:  on_error
  ============================================================================*/
  /**
   * This function does nothing.
   */
  virtual void on_error();

  /*============================================================================
    FUNCTION:  set_pid
  ============================================================================*/
  /**
   * This function sets the pid of the daemon in the adapter.
   */
  virtual void set_pid(int pid);
};

/*============================================================================
  FUNCTION:  get_adapter
============================================================================*/
/**
 * This function returns an instance to the ultrasound proximity socket adapter.
 *
 * @param gesture_fw_lib_path not used in socket adapters
 *
 * @return Instance to the ultrasound proximity socket adapter.
 */
FrameworkAdapter *get_adapter(char *gesture_fw_lib_path);

#endif //__PROXIMITY_SOCKET_ADAPTER__
