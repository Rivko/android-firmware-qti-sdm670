/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       ULP Main Source File

GENERAL DESCRIPTION
  This file contains the entry point for the ULP module.


     Copyright (c) 2012-2013 Qualcomm Atheros, Inc.
     All Rights Reserved.
     Qualcomm Atheros Confidential and Proprietary.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_libulp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/stat.h>
#include <fcntl.h>
#include <linux/types.h>
#include <errno.h>
#include <sys/time.h>

// Internal include files
#include <ulp_engine.h>
#include "ulp_quipc.h"

#include "loc_cfg.h"
#include "msg_q.h"

#include "ulp_data.h"
#include "ulp_internal.h"


/*=====================================================================================
 * Function ulp_monitor_main thread processing routine
 *
 * Description
 *  This is the processing routine of ulp_monitor thread. It will ping ULP main
 *  thread if all sub-system has not reported back while there is still active
 *  request.
 *
 * Parameters:
 *   context: not used
 *
 * Return value:
 *   NULL: on exit
 =======================================================================================*/
void ulp_monitor_main (void * /*context*/)
{
   struct timespec expire_time;
   int             ret_val = 0; // ETIMEDOUT is a non-zero error code

   ENTRY_LOG();
   pthread_mutex_lock (&ulp_data.monitor_mutex);
   while (1)
   {
      LOC_LOGD ("%s, run monitor thread = %d, timedout = %d \n",
                __func__,
                ulp_data.run_monitor_thread,
                (ret_val == ETIMEDOUT) ? 1:0);

      if (ulp_data.run_monitor_thread == false)
      {
         // wait forever until someone signals us
         pthread_cond_wait(&ulp_data.monitor_cond,
                           &ulp_data.monitor_mutex);
         ret_val = 0;
      }
      else
      {
         // This means the main thread need to do state transition
         if (ret_val == ETIMEDOUT)
         {
            ulp_msg_send_monitor_request ();
         }

         // Calculate absolute expire time
         clock_gettime(CLOCK_MONOTONIC, &expire_time);
         expire_time.tv_sec  += ULP_MONITOR_THREAD_POLL_INTERVAL;

         ret_val = pthread_cond_timedwait(&ulp_data.monitor_cond,
                                          &ulp_data.monitor_mutex,
                                          &expire_time);
      }
   }

   pthread_mutex_unlock (&ulp_data.monitor_mutex);

   EXIT_LOG(%d, 0);

   return;
}

/*=============================================================================================
 * Function: ulp_monitor_init
 *
 * Description
 *   This function initializes ULP monitor module. ULP monitor will help to monitor the system
 *   in case some sub-systems go off weeds.
 *
 *   The monitor module will spawn a thread and if there is a request pending, and the
 *   ULP is not getting any response from sub-systems, it will re-evaluate the system.
 *
 * Parameters:
 *   None
 *
 * Return value:
 *   0: on success
 *   non-zero: on failure
 =============================================================================================*/
int ulp_monitor_init ()
{
   int ret_val = -1;
   pthread_t thread_id = (pthread_t)NULL;
   bool mutex_inited = false;
   bool cond_inited = false;

   ENTRY_LOG();
   do
   {
      ret_val = pthread_mutex_init(&(ulp_data.monitor_mutex),
                                   NULL);
      if (ret_val != 0)
      {
         break;
      }
      mutex_inited = true;

      pthread_condattr_t condAttr;
      pthread_condattr_init(&condAttr);
      pthread_condattr_setclock(&condAttr, CLOCK_MONOTONIC);
      ret_val = pthread_cond_init(&(ulp_data.monitor_cond), &condAttr);
      pthread_condattr_destroy(&condAttr);

      if (ret_val != 0)
      {
         break;
      }
      cond_inited = true;

      thread_id = ulp_data.ulp_create_thread_cb("ulp_monitor",
                                                ulp_monitor_main,
                                                (void *)&ulp_data);
      if ((pthread_t)NULL == thread_id)
      {
         LOC_LOGE ("%s: failed to create ulp_monitor therad, errno = %d\n", __func__, errno);
         break;
      }

      ret_val = 0;
   } while (0);

   EXIT_LOG(%d, ret_val);

   if (ret_val != 0)
   {
      if (mutex_inited == true)
      {
         pthread_mutex_destroy (&(ulp_data.monitor_mutex));
      }

      if (cond_inited == true)
      {
         pthread_cond_destroy (&(ulp_data.monitor_cond));
      }
   }
   return ret_val;
}
