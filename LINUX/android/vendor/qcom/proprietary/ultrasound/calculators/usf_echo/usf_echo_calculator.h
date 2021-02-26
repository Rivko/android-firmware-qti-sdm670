/*===========================================================================
                           usf_echo_client.h

DESCRIPTION: Echo client interface class.
  Holds function definitions and enums for concrete echo clients.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef __USF_ECHO_CALCULATOR__
#define __USF_ECHO_CALCULATOR__
/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "echo_client.h"
#include <framework_adapter.h>

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define US_MAX_EVENTS 20

/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/

/**
 * Must forward declare here, because echo calculator includes echo client
 * which includes echo calculator.This way echo calculator is compiled without
 * declaring EchoClient
 */
class EchoClient;

/*-----------------------------------------------------------------------------
  Static Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/
/*============================================================================
  FUNCTION:  echo_calculator_start
============================================================================*/
/**
 * Main function of the Echo daemon. Handle all the Echo operations.
 * Blocks until completion.
 *
 * @param echo_client Concrete echo object of the calling daemon
 */
void echo_calculator_start(EchoClient *echo_client);

/*============================================================================
  FUNCTION:  echo_calculator_stop
============================================================================*/
/**
 * Asynchronously signal the daemon to stop.
 */
void echo_calculator_stop();

/*============================================================================
  FUNCTION:  echo_exit
============================================================================*/
/**
 * Perform clean exit of the daemon.
 *
 * @param status Exit status
 * @return int Exit status
 */
int echo_exit(int status);

/*============================================================================
  FUNCTION:  add_event_key
============================================================================*/
/**
 * Creates event and adds it to the echo_context.m_events[].
 *
 * @param key The key to add
 * @param nPressure The key's pressure
 */
void add_event_key(int key,
                   int nPressure);

#endif //__USF_ECHO_CALCULATOR__
