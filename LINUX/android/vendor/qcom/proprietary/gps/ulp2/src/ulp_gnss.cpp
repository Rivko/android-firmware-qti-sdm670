/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2011-2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       ULP GNSS module

GENERAL DESCRIPTION
  This file contains functions being called from ulp brain to
  configure/start/stop GNSS engine.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013 Qualcomm Atheros, Inc.
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
#include <new>
#include <comdef.h>

// Internal include files
#include <ulp_engine.h>

#include "loc_cfg.h"
#include "msg_q.h"

#include "ulp_quipc.h"

#include "ulp_data.h"
#include "ulp_internal.h"


/*===========================================================================
FUNCTION    ulp_gnss_engine_running

DESCRIPTION
   This function returns true if GNSS engine is running.

DEPENDENCIES
   None

RETURN VALUE
   true: engine is running
   false: engine is not running

SIDE EFFECTS
   N/A
===========================================================================*/
bool ulp_gnss_engine_running ()
{
    bool gnss_running = true;

    LOC_LOGD ("%s, gnss state = %d\n", __func__, ulp_data.gnss_provider_info.state);
    if (ulp_data.gnss_provider_info.state == GNSS_STATE_IDLE)
    {
       gnss_running = false;
    }
    else
    {
       gnss_running = true;
    }

    return gnss_running;
}

/*===========================================================================
FUNCTION    ulp_gnss_start_engine

DESCRIPTION
   This function is called to configure and start GNSS. libulp module posts
   messages to GPS HAL layer via message queue for the request.

DEPENDENCIES
   None

RETURN VALUE
   0: GNSS state remains ON
   1: GNSS state changes from OFF to ON
   -1: failure

SIDE EFFECTS
   N/A
===========================================================================*/
int ulp_gnss_start_engine ()
{
   int                      ret_val                 = -1;
   LocGpsPositionRecurrence gnss_recurrence_type    = LOC_GPS_POSITION_RECURRENCE_SINGLE;
   uint32_t                 gnss_fix_interval       = 0; // -1: not set yet
   LocGpsPositionMode       gnss_mode               = LOC_GPS_POSITION_MODE_STANDALONE;
   bool                     gnss_need_configuration = false;
   LocAdapterBase*          adapter                 = ulp_data.loc_proxy->getAdapter();
   uint32_t                 acc                     = 0;

   ENTRY_LOG();
   LOC_LOGD ("%s, gnss state = %d, first fix pending = %d, \n"
             "ulp recurrence type = %d, ulp_data.phoneSetting.context_type: 0x%x\n,"
             "agps enabled = %d, gps_conf capabilities = 0x%lx, adapter = %p\n",
             __func__,
             ulp_data.gnss_provider_info.state,
             ulp_data.gnss_provider_info.first_fix_pending,
             ulp_data.gnss_recurrence_type,
             ulp_data.phoneSetting.context_type,
             ulp_data.phoneSetting.is_agps_enabled,
             ulp_data.loc_proxy->mCapabilities,
             adapter);

   do
   {
      if (NULL == adapter) {
         LOC_LOGW("Loc HAL handshake did not happen yet...");
         break;
      }

      // GNSS recurrence type
      if (ulp_data.gnss_recurrence_type == ULP_LOC_RECURRENCE_PERIODIC)
      {
         gnss_recurrence_type = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
      }
      else
      {
         gnss_recurrence_type = LOC_GPS_POSITION_RECURRENCE_SINGLE;
      }

      // Decide on GPS mode with current Android logic
      // Existing GPS position mode selection logic is preserved.
      // If setting has Assisted GPS enabled
      //                If phone has MSA capability and is Single Shot, use MSA
      //                If phone has MSB capability, use MSB
      //                All other scenario, use standalone
      gnss_mode = LOC_GPS_POSITION_MODE_STANDALONE;
      if ((ulp_data.phoneSetting.context_type & ULP_PHONE_CONTEXT_AGPS_SETTING) &&
          (ulp_data.phoneSetting.is_agps_enabled == true))
      {
          if ((ulp_data.loc_proxy->mCapabilities & LOC_GPS_CAPABILITY_MSA) &&
              (gnss_recurrence_type == LOC_GPS_POSITION_RECURRENCE_SINGLE))
          {
              gnss_mode = LOC_GPS_POSITION_MODE_MS_ASSISTED;
          }
          else if (ulp_data.loc_proxy->mCapabilities & LOC_GPS_CAPABILITY_MSB)
          {
             gnss_mode = LOC_GPS_POSITION_MODE_MS_BASED;
          }

      }

      gnss_fix_interval = ulp_data.gnss_fix_interval;
      // If there is a high accuracy fix and QUIPC is enabled to run,
      // use TBF of 1 second
      if (gnss_recurrence_type == LOC_GPS_POSITION_RECURRENCE_SINGLE)
      {
         gnss_recurrence_type = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
         gnss_fix_interval = ULP_SINGLE_SHOT_MIN_TRACKING_INTERVAL_MSEC;
      }
      else // (gnss_recurrence_type == LOC_GPS_POSITION_RECURRENCE_PERIODIC)
      {
         if (ulp_quipc_engine_running() == true)
         {
            gnss_fix_interval = ULP_HIGH_ACCURACY_MIN_TRACKING_INTERVAL_MSEC;
         }
      }

      if (ulp_data.low_accuracy_request_active &&
          !ulp_data.high_accuracy_request_active &&
          !ulp_data.gps_provider_request_active) {
        // lower the accuracy requirement if we are turning on gnss for
        // low accuracy request.
        acc = 2000;
      }

      gnss_need_configuration = false;
      // Figure out whether we need to reconfigure the GNSS mode or no
      if (ulp_gnss_engine_running () == false)
      {
         gnss_need_configuration = true;
      }
      else if (ulp_data.gnss_provider_info.recurrence_type != gnss_recurrence_type ||
               ulp_data.gnss_provider_info.fix_interval    != gnss_fix_interval ||
               ulp_data.gnss_provider_info.position_mode   != gnss_mode ||
               ulp_data.gnss_provider_info.accuracy        != acc)
      {
         gnss_need_configuration = true;
      }
      else if ((ulp_data.gnss_provider_info.new_request_active == true) &&
               (ulp_data.gnss_provider_info.fix_interval > ULP_GNSS_RECONFIGURE_MODE_THRESHOLD_MSEC))
      {
         gnss_need_configuration = true;
      }

      if (gnss_need_configuration != true) {
        // Wait for first fix to complete
        if (ulp_data.gnss_provider_info.first_fix_pending == true)
        {
          LOC_LOGD ("Got tbf update of %d when first fix pending of tbf %d\n",
                    ulp_data.gnss_fix_interval,
                    ulp_data.gnss_provider_info.fix_interval);
          ulp_data.gnss_provider_info.tbf_update_pending = true;
          break;
        }
      } else {
         LocPositionMode mode = (loc_position_mode_type) gnss_mode;
         LocPosMode params(mode,
                           gnss_recurrence_type,
                           gnss_fix_interval,
                           acc, /* preferred accuracy */
                           0, /* preferred time*/
                           true, /* preferred location sharing */
                           NULL,
                           NULL);

        adapter->setPositionModeCommand(params);

        ulp_data.gnss_provider_info.recurrence_type = gnss_recurrence_type;
        ulp_data.gnss_provider_info.fix_interval    = gnss_fix_interval;
        ulp_data.gnss_provider_info.position_mode   = gnss_mode;
        ulp_data.gnss_provider_info.accuracy        = acc;
      }

      if (ulp_gnss_engine_running () == false || gnss_need_configuration == true)
      {
         adapter->startTrackingCommand();
         ulp_gnss_process_gnss_status(LOC_GPS_STATUS_SESSION_BEGIN);
      }

      // GNSS state has transitioned from IDLE to INITIALIZING
      if (ulp_data.gnss_provider_info.state == GNSS_STATE_IDLE)
      {
         ulp_gnss_set_state (GNSS_STATE_INITIALIZING);
         ulp_data.gnss_provider_info.first_fix_pending = true;
         ret_val = 1;
      }
      else if (gnss_need_configuration == true)
      {
         ulp_gnss_set_state (GNSS_STATE_INITIALIZING);
         ret_val = 1;
      }
      else
      {
         ret_val = 0;
      }
   } while (0);

   EXIT_LOG(%d, ret_val);
   return ret_val;
}

/*===========================================================================
FUNCTION    ulp_gnss_stop_engine

DESCRIPTION
   This function is called to stop GNSS. libulp module posts messages to
   GPS HAL layer via message queue for the request.

DEPENDENCIES
   None

RETURN VALUE
   0: success
   -1: failure

SIDE EFFECTS
   N/A

===========================================================================*/
int ulp_gnss_stop_engine ()
{
   int ret_val = -1;

   ENTRY_LOG();
   LOC_LOGD ("%s, gnss state = %d\n", __func__, ulp_data.gnss_provider_info.state);
   do
   {
      LocAdapterBase* adapter = ulp_data.loc_proxy->getAdapter();
      // First, stop other provider
      if (ulp_gnss_engine_running () == true && adapter)
      {
         adapter->stopTrackingCommand();
         ulp_gnss_process_gnss_status(LOC_GPS_STATUS_SESSION_END);

         ulp_gnss_set_state (GNSS_STATE_IDLE);
         ulp_data.gnss_provider_info.first_fix_pending = false;
         ret_val = 0;
      }
   } while (0);

   EXIT_LOG(%d, ret_val);
   return ret_val;
}

/*===========================================================================
FUNCTION    ulp_gnss_set_state

DESCRIPTION
   This function is called to update the current GNSS engine state.
   In the meantime, it will save the old state.

DEPENDENCIES
   None

RETURN VALUE
   0: success
   -1: failure

SIDE EFFECTS
   N/A

===========================================================================*/
int ulp_gnss_set_state (gnss_state_e_type new_state)
{
   LOC_LOGD ("%s, current gnss state = %d, new gnss state = %d\n",
             __func__,
             ulp_data.gnss_provider_info.state,
             new_state);

   if (new_state == GNSS_STATE_IDLE ||
       new_state == GNSS_STATE_FAILED)
   {
      ulp_data.gnss_provider_info.high_speed = false;
      ulp_data.gnss_provider_info.strong_sv_cnt = 0;
   }

   if (ulp_data.gnss_provider_info.state != new_state)
   {
      if (ulp_data.gnss_provider_info.state == GNSS_STATE_IDLE)
      {
         ulp_data.gnss_provider_info.last_started_time_ms = ulp_util_get_time_ms ();
      }
      else if (new_state == GNSS_STATE_IDLE)
      {
         ulp_data.gnss_provider_info.last_stopped_time_ms = ulp_util_get_time_ms ();
      }

      ulp_data.gnss_provider_info.last_state = ulp_data.gnss_provider_info.state;
      ulp_data.gnss_provider_info.state = new_state;
   }

   return 0;
}

int ulp_gnss_process_gnss_status (const uint16 status) {
  if (status == LOC_GPS_STATUS_ENGINE_OFF &&
        (ulp_data.gnss_provider_info.tbf_update_pending == true ))
  {
     LOC_LOGD ("%s, status = %d,clear tbf_update_pending flag\n",__func__,status);
     // clear tbf_update_pending when ulp receives ENGINE_OFF msg
     ulp_data.run_provider_selection_logic = true;
     ulp_data.gnss_provider_info.tbf_update_pending = false;
  }
  ulp_brain_report_gnss_status (status);
  return 0;
}
