/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2011, 2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       ULP AMT module

GENERAL DESCRIPTION
  This file contains functions being called from ulp brain to
  configure/start/stop DR Provider engine used for Automotive external DR solution.
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

// Internal include files
#include <ulp_engine.h>

#include "loc_cfg.h"
#include "msg_q.h"

#include "ulp_quipc.h"

#include "ulp_data.h"
#include "ulp_internal.h"


/*===========================================================================
FUNCTION    ulp_dr_engine_running

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
bool ulp_dr_engine_running ()
{
    bool dr_running = true;

    LOC_LOGD ("%s, DR state = %d\n", __func__, ulp_data.dr_provider_info.state);
    if (ulp_data.dr_provider_info.state == DR_STATE_IDLE)
    {
       dr_running = false;
    }
    else
    {
       dr_running = true;
    }

    return dr_running;
}

/*===========================================================================
FUNCTION    ulp_dr_start_engine

DESCRIPTION
   This function is called to configure and start External DR. libulp module posts
   messages to LocAmtProxy for the request.

DEPENDENCIES
   None

RETURN VALUE
   0: DR state remains ON
   1: DR state changes from OFF to ON
   -1: failure

SIDE EFFECTS
   N/A
===========================================================================*/
int ulp_dr_start_engine ()
{
   int                   ret_val = -1;
   LocGpsPositionRecurrence dr_recurrence_type    = LOC_GPS_POSITION_RECURRENCE_SINGLE;
   uint32_t              dr_fix_interval       = 0; // -1: not set yet
   LocGpsPositionMode       dr_mode               = LOC_GPS_POSITION_MODE_STANDALONE;
   bool                  dr_need_configuration = false;
   LocAdapterBase*       adapter = ulp_data.loc_proxy->getAdapter();

   LOC_LOGD ("%s, DR state = %d, first fix pending = %d, \n"
             "ulp recurrence type = %d, ulp_data.phoneSetting.context_type: 0x%x\n,"
             "agps enabled = %d, adapter = %p\n",
             __func__,
             ulp_data.gnss_provider_info.state,
             ulp_data.gnss_provider_info.first_fix_pending,
             ulp_data.gnss_recurrence_type,
             ulp_data.phoneSetting.context_type,
             ulp_data.phoneSetting.is_agps_enabled,
             adapter);

   do
   {
      // Wait for first fix to complete
      if (ulp_data.dr_provider_info.first_fix_pending == true)
      {
         break;
      }

      if (NULL == adapter) {
         LOC_LOGW("Loc HAL handshake did not happen yet...");
         break;
      }

      // GNSS recurrence type
      if (ulp_data.gnss_recurrence_type == ULP_LOC_RECURRENCE_PERIODIC)
      {
         dr_recurrence_type = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
      }
      else
      {
         dr_recurrence_type = LOC_GPS_POSITION_RECURRENCE_SINGLE;
      }

      dr_fix_interval = ulp_data.gnss_fix_interval;
      // If there is a high accuracy fix and QUIPC is enabled to run,
      // use TBF of 1 second
      if (dr_recurrence_type == LOC_GPS_POSITION_RECURRENCE_SINGLE)
      {
         dr_recurrence_type = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
         dr_fix_interval = ULP_SINGLE_SHOT_MIN_TRACKING_INTERVAL_MSEC;
      }

      dr_need_configuration = false;
      // Figure out whether we need to reconfigure the GNSS mode or no
      if (ulp_dr_engine_running () == false)
      {
         dr_need_configuration = true;
      }
      else if (ulp_data.dr_provider_info.recurrence_type != dr_recurrence_type ||
               ulp_data.dr_provider_info.fix_interval    != dr_fix_interval ||
               ulp_data.dr_provider_info.position_mode   != dr_mode)
      {
         dr_need_configuration = true;
      }
      else if ((ulp_data.dr_provider_info.new_request_active == true) &&
               (ulp_data.dr_provider_info.fix_interval > ULP_GNSS_RECONFIGURE_MODE_THRESHOLD_MSEC))
      {
         dr_need_configuration = true;
      }

      if (dr_need_configuration == true)
      {
         LocPositionMode mode = (loc_position_mode_type) dr_mode;
         LocPosMode params(mode,
                           dr_recurrence_type,
                           dr_fix_interval,
                           0, /* preferred accuracy */
                           0, /* preferred time*/
                           true, /* preferred location sharing */
                           NULL,
                           NULL);

        /*Send position mode to LocAmtProxy*/
        ulp_data.loc_proxy->sendFixModeToDr(params);

        /*Send position mode to Adapter*/
        adapter->setPositionModeCommand(params);

        ulp_data.dr_provider_info.recurrence_type = dr_recurrence_type;
        ulp_data.dr_provider_info.fix_interval    = dr_fix_interval;
        ulp_data.dr_provider_info.position_mode   = dr_mode;

      }

      if (ulp_dr_engine_running () == false || dr_need_configuration == true)
      {

         //StartDrSession to LocAmtProxy
         ulp_data.loc_proxy->startDrSession();

         //Start the Internal GNSS session with LocApiV02 for measurements and SV Poly
         adapter->startTrackingCommand();
      }

      // DR state has transitioned from IDLE to INITIALIZING
      if (ulp_data.dr_provider_info.state == DR_STATE_IDLE)
      {
         ulp_dr_set_state (DR_STATE_INITIALIZING);
         ulp_data.dr_provider_info.first_fix_pending = true;
         ret_val = 1;
      }
      else if (dr_need_configuration == true)
      {
         ulp_dr_set_state (DR_STATE_INITIALIZING);
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
FUNCTION    ulp_dr_stop_engine

DESCRIPTION
   This function is called to stop external DR solution. libulp module posts messages to
   LocAmtProxy for sending the StopSession request

DEPENDENCIES
   None

RETURN VALUE
   0: success
   -1: failure

SIDE EFFECTS
   N/A

===========================================================================*/
int ulp_dr_stop_engine ()
{
   int ret_val = -1;

   LOC_LOGD ("%s, DR state = %d\n", __func__, ulp_data.dr_provider_info.state);
   do
   {
      LocAdapterBase* adapter = ulp_data.loc_proxy->getAdapter();
      // First, stop other provider
      if (ulp_dr_engine_running () == true && adapter)
      {
         adapter->stopTrackingCommand();

         //StopDrSession to LocAmtProxy
         ulp_data.loc_proxy->stopDrSession();

         ulp_dr_set_state (DR_STATE_IDLE);
         ulp_data.dr_provider_info.first_fix_pending = false;
         ret_val = 0;
      }
   } while (0);

   EXIT_LOG(%d, ret_val);
   return ret_val;
}

/*===========================================================================
FUNCTION    ulp_dr_set_state

DESCRIPTION
   This function is called to update the current DR engine state.

DEPENDENCIES
   None

RETURN VALUE
   0: success
   -1: failure

SIDE EFFECTS
   N/A

===========================================================================*/
int ulp_dr_set_state (dr_state_e_type new_state)
{
   LOC_LOGD ("%s, current DR state = %d, new DR state = %d\n",
             __func__,
             ulp_data.dr_provider_info.state,
             new_state);

   if (new_state == DR_STATE_IDLE ||
       new_state == DR_STATE_FAILED)
   {
      ulp_data.dr_provider_info.high_speed = false;
      ulp_data.dr_provider_info.strong_sv_cnt = 0;
   }

   if (ulp_data.dr_provider_info.state != new_state)
   {
      if (ulp_data.dr_provider_info.state == DR_STATE_IDLE)
      {
         ulp_data.dr_provider_info.last_started_time_ms = ulp_util_get_time_ms ();
      }
      else if (new_state == DR_STATE_IDLE)
      {
         ulp_data.dr_provider_info.last_stopped_time_ms = ulp_util_get_time_ms ();
      }

      ulp_data.dr_provider_info.state = new_state;
   }

   return 0;
}
