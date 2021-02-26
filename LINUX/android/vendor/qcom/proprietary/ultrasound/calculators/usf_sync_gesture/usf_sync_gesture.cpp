/*===========================================================================
                           usf_sync_gesture.cpp

DESCRIPTION: Implementation of the Synchronized Gesture daemon.


INITIALIZATION AND SEQUENCING REQUIREMENTS:
  If not started through java app then make sure to have
  correct /data/usf/sync_gesture/usf_sync_gesture.cfg file linked to the wanted cfg file
  placed in /data/usf/sync_gesture/cfg/.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#define LOG_TAG "usf_sync_gesture"

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#include "usf_echo_calculator.h"
#include "sync_gesture_echo_client.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  main
==============================================================================*/
/**
  Main function of the Gesture daemon. Handle all the Gesture operations.
*/
int main (void)
{
  EchoClient *sync_gesture_client = new SyncGestureEchoClient();

  echo_calculator_start(sync_gesture_client);
}
