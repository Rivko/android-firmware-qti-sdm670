/*===========================================================================
                           usf_property_thread.cpp

DESCRIPTION: This file implements the property thread.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "usf_property_thread"

/*-----------------------------------------------------------------------------
  Includes
-----------------------------------------------------------------------------*/
#include "usf_property_thread.h"
#include "usf_log.h"
#include <sys/stat.h>
#include <string.h>

#define PROPERTY_NAME_FORMAT "sys.usf.%s.stop"

/*-----------------------------------------------------------------------------
  Constants
-----------------------------------------------------------------------------*/
static pthread_t thread_id;

/*-----------------------------------------------------------------------------
  Functions
-----------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  property_thread_interrupt_func
==============================================================================*/
/**
  See function description at header file under the function declaration.
*/
static void property_thread_interrupt_func(int sig)
{
  LOGW("%s, Property thread got signal %d", __FUNCTION__, sig);
  pthread_exit(NULL);
}

/*==============================================================================
  FUNCTION:  property_thread_func
==============================================================================*/
/**
  See function description at header file under the function declaration.
*/
static void *property_thread_func(void *daemon_name)
{
  LOGD("%s, Property thread running", __FUNCTION__);

  char prop_name[PROPERTY_KEY_MAX];
  char prop_val[PROPERTY_VALUE_MAX];

  snprintf(prop_name,
           PROPERTY_KEY_MAX,
           PROPERTY_NAME_FORMAT,
           (char *)daemon_name);

  while(true)
  {
    property_get((const char *)prop_name,
                 prop_val, "0");

    if (0 == strcmp(prop_val, "1"))
    {
      LOGD("%s, Property thread received exit request",
           __FUNCTION__);
      int ret = property_set(prop_name, "0");
      if (0 != ret)
      {
        LOGD("%s, Property set failed",
             __FUNCTION__);
        return NULL;
      }
      kill(getpid(), SIGTERM);
    }
    usleep(500*1000);
  }

  return NULL;
}

/*==============================================================================
  FUNCTION:  property_thread_create
==============================================================================*/
/**
  See function description at header file under the function declaration.
*/
int property_thread_create(char *daemon_name) {
  if (NULL == daemon_name)
  {
    LOGE("%s, Invalid parameters", __FUNCTION__);
    return -1;
  }

  // Initialize signals
  struct sigaction act;
  act.sa_handler = property_thread_interrupt_func;
  // Use signal with disposition other than "stop", "continue" or "terminate".
  // Otherwise, the action will affect the whole process. SIGCHLD is ignored
  // by default and therefore won't affect other threads in the process.
  sigaction(SIGCHLD,
            &act,
            NULL);
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  return pthread_create((pthread_t*)&thread_id,
                        &attr,
                        property_thread_func,
                        daemon_name);
}

/*==============================================================================
  FUNCTION:  property_thread_exit
==============================================================================*/
/**
  See function description at header file under the function declaration.
*/
void property_thread_exit(char *daemon_name) {
  LOGD("%s, Exiting property thread ", __FUNCTION__);
  char prop_name[PROPERTY_KEY_MAX];
  snprintf(prop_name,
           PROPERTY_KEY_MAX,
           PROPERTY_NAME_FORMAT,
           (char *)daemon_name);

  property_set(prop_name, "0");
  pthread_kill(thread_id,
               SIGCHLD);
  pthread_join(thread_id,
                 NULL);
}
