/*===========================================================================
                           proximity_socket_adapter.cpp

DESCRIPTION: Implements the proximity socket adapter class.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#define LOG_TAG "ProximitySocketAdapter"

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "proximity_socket_adapter.h"
#include "usf_log.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/

/*============================================================================
  CONSTRUCTOR:
============================================================================*/
/**
 * See function definition in header file
 */
ProximitySocketAdapter::ProximitySocketAdapter()
:daemon_pid_(-1),
 status(DISCONNECT)
{
}

/*============================================================================
  FUNCTION:  get_event_mapping
============================================================================*/
/**
 * See function definition in header file
 */
event_mapping_t ProximitySocketAdapter::get_event_mapping()
{
  return NOT_MAPPED;
}

/*============================================================================
  FUNCTION:  get_config
============================================================================*/
/**
 * See function definition in header file
 */
cfg_params_t &ProximitySocketAdapter::get_config()
{
  return cfg_params;
}

/*============================================================================
  FUNCTION:  wait_n_update
============================================================================*/
/**
 * See function definition in header file
 */
adapter_status_t ProximitySocketAdapter::wait_n_update()
{
  if (NULL == m_proximity_data_socket)
  {
    m_proximity_data_socket =
      new DataUnSocket("/data/usf/proximity/data_socket");

    if (0 != m_proximity_data_socket->start())
    {
      LOGE("%s: Starting data socket failed.",
           __FUNCTION__);
      status = FAILURE;
    }
    else
    {
      status = ACTIVATE;
    }
  }

  return status;
}

/*============================================================================
  FUNCTION:  get_status
============================================================================*/
/**
 * See function definition in header file.
 */
adapter_status_t ProximitySocketAdapter::get_status()
{
  return status;
}

/*============================================================================
  FUNCTION:  disconnect
============================================================================*/
/**
 * See function definition in header file.
 */
int ProximitySocketAdapter::disconnect()
{
  LOGI("%s: Ultrasound daemon disconnecting",
       __FUNCTION__);
  if (NULL != m_proximity_data_socket)
  {
    delete m_proximity_data_socket;
    m_proximity_data_socket = NULL;
  }
  status = DISCONNECT;
  return 0;
}

/*============================================================================
  FUNCTION:  send_event
============================================================================*/
/**
 * See function definition in header file
 */
int ProximitySocketAdapter::send_event(int event,
                                         event_source_t event_source,
                                         int distance)
{
  if (status != ACTIVATE)
  {
    return 0;
  }

  if (0 >= m_proximity_data_socket->send_proximity_event(event,
                                                         distance,
                                                         event_source))
  {
    LOGE("%s: Couldn't send proximity events to socket",
         __FUNCTION__);
    return 1;
  }
  return 0;
}

/*============================================================================
  FUNCTION:  on_error
============================================================================*/
/**
 * See function definition in header file.
 */
void ProximitySocketAdapter::on_error()
{
  LOGI("%s: called",
       __FUNCTION__);
}

/*============================================================================
  FUNCTION:  activate
============================================================================*/
/**
 * See function definition in header file.
 */
void ProximitySocketAdapter::activate()
{
  LOGI("%s: Received",
       __FUNCTION__);
}

/*============================================================================
  FUNCTION:  deactivate
============================================================================*/
/**
 * See function definition in header file.
 */
void ProximitySocketAdapter::deactivate()
{
  LOGI("%s: Received",
         __FUNCTION__);
}

/*============================================================================
  FUNCTION:  set_pid
============================================================================*/
/**
 * See function definition in header file
 */
void ProximitySocketAdapter::set_pid(int pid)
{
  daemon_pid_ = pid;
}

/*============================================================================
  FUNCTION:  get_adapter
============================================================================*/
/**
 * See function definition in header file
 */
FrameworkAdapter *get_adapter(char *gesture_fw_lib_path)
{
  UNUSED(gesture_fw_lib_path);
  LOGD("%s: Proximity socket adapter instance created",
       __FUNCTION__);
  static ProximitySocketAdapter adapter;
  return &adapter;
}
