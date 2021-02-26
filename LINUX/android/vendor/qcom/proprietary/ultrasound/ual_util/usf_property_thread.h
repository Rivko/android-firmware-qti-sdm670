#ifndef _USF_PROPERTY_THREAD_H_
#define _USF_PROPERTY_THREAD_H_

/*============================================================================
                           usf_property_thread.h

DESCRIPTION:  This file declares the propert thread

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

/*-----------------------------------------------------------------------------
  Include Files
-----------------------------------------------------------------------------*/
#include <pthread.h>
#include <cutils/properties.h>

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  property_thread_create
==============================================================================*/
/**
 * This function creates the property thread and launches it. The property
 * thread polls the stop property and when it identifies the property
 * was set, sends the daemon a SIGTERM to close it gracefully.
 *
 * @param daemon_name The name of the calling daemon
 *
 * @return int -1 when the parameters are invalid
 *             The thread's return value otherwise
 */
int property_thread_create(char *daemon_name);

/*==============================================================================
  FUNCTION:  property_thread_exit
==============================================================================*/
/**
 * This function closes the property thread and cleans the stop
 * property.
 *
 * @param daemon_name The name of the calling daemon
 */
void property_thread_exit(char *daemon_name);

#endif // _USF_PROPERTY_THREAD_H_
