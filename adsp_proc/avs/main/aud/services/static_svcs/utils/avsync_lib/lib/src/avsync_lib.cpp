/*========================================================================
 *//** @file avsync_param_lib.cpp
This file contains functions for AVSync Statistics Library

Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_lib/lib/src/avsync_lib.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
4/19/14     YW      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "avsync_lib.h"
#include "adsp_mtmx_strtr_api.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "adsp_asm_session_commands.h"
#include "avsync_stats_lib.h"
#include "stream_dev_drift_calc.h"
#include "avtimer_drv_api.h"

//#define DBG_AVSYNC_LIB
/* =======================================================================
TYPEDEFS
========================================================================== */
#define AVSYNC_MAX_STR_LEN 32
enum stc_value_validity
{
   STC_VALUE_DEFAULT = 0, //Default state
   STC_VALUE_INVALID, //Invalid state
   STC_VALUE_VALID, //This state will be set once the STC value has been logged (in real time)
};
struct avsync_param_t
{
   //Session clk for avsync requirements
    char                    instance_name[AVSYNC_MAX_STR_LEN];
    uint64_t                session_clock;
    uint64_t                expected_session_clock;
    uint64_t                stc_value_rendering;  //Local STC value of the rendering buffer
    int64_t                 start_time;           //The value provided by client as part of RUN command
    int64_t                 start_time_calc;      //The actual start time of the Rx matrix
    int64_t                 start_time_offset;   //The duration of hold before RUN in Rx Matrix
    uint32_t                start_flag;
    uint32_t                start_time_ds_delay; //Downstream Delay during Rx Matrix Run
    uint64_t                absolute_ts;
    bool_t                  is_timestamp_valid;
    avtimer_drv_handle_t    avt_drv_handle;      //Handle to avtimer

    //Advanced AVSync requirements applicable only to the Rx Matrix
    uint32_t avsync_rendering_decision;     /** Enumeration to indicate the rendering decision mode
                                                default/local STC */
    uint32_t avsync_clk_recovery_mechanism; /** Enumeration to indicate the clock recovery mechanism
                                                that needs to be used for rate matching */
    uint32_t connected_asm_id;              /** The session/stream ID of connected ASM session */
    sd_dc_t *avsync_drift_s2pd_lib_ptr;     /** Pointer to the AV-Sync Utilities library which
                                                handles the stream to primary device calculation
                                                 (only drift detection) */

    //AVSync Stats Library
    int64_t  render_window_start;       //Start of rendering window
    int64_t  render_window_end;         //End of rendering window
    uint32_t render_stat_window_status; // LSB[0-1] =render window [start-end] LSB[2-3] = stat window [start-end]
    void     *avsync_stats_ptr;         //Pointer to AVSync Stats Library

};
#define RENDER_WINDOW_START_MASK 1
#define RENDER_WINDOW_END_MASK 2
#define STAT_WINDOW_START_MASK 4
#define STAT_WINDOW_END_MASK 8

/* =======================================================================
FUNCTIONS
========================================================================== */
/*Note: All documentation about functions in header file except for locally defined functions*/

uint32_t avsync_lib_getsize()
{
   return sizeof(avsync_param_t);
}

ADSPResult avsync_lib_init(void *inst_ptr, char *instance_name)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EBADPARAM;
   }
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib: Init");
   //Memory Init
   memset(inst_ptr,0,sizeof(avsync_param_t));
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;

   strlcpy(avsync_lib_ptr->instance_name,instance_name,AVSYNC_MAX_STR_LEN);

   avsync_lib_open_hw_resources(avsync_lib_ptr);

   return ADSP_EOK;
}
ADSPResult avsync_lib_open_hw_resources(void *inst_ptr)
{
   ADSPResult result = ADSP_EOK;
   if(NULL == inst_ptr )
   {
      return ADSP_EBADPARAM;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   //Open AVTimer Driver
   avtimer_open_param_t avt_open_param;
   avt_open_param.client_name = avsync_lib_ptr->instance_name;
   avt_open_param.flag = 0;
   if(NULL != avsync_lib_ptr->avt_drv_handle)
   {
      return ADSP_EOK;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib %s Opening AVTimer Driver at %p",avsync_lib_ptr->instance_name,avsync_lib_ptr->avt_drv_handle);
   if (ADSP_EOK != (result = avtimer_drv_hw_open(&(avsync_lib_ptr->avt_drv_handle), &avt_open_param)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver with result: %d",result);
      avsync_lib_ptr->avt_drv_handle = NULL;
   }
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib pointer %p, Avtimer pointer %p",avsync_lib_ptr,avsync_lib_ptr->avt_drv_handle);

   return ADSP_EOK;
}

ADSPResult avsync_lib_close_hw_resources(void *inst_ptr)
{
   ADSPResult result = ADSP_EOK;
   if(NULL == inst_ptr)
   {
      return ADSP_EBADPARAM;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib Closing AVTimer Driver");

   // For TX matrix this is not applicable and should pass right through (NULLness check).
   if (NULL != avsync_lib_ptr->avt_drv_handle)
   {
      if (ADSP_EOK != (result = avtimer_drv_hw_close(avsync_lib_ptr->avt_drv_handle)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Close AVTimer Driver with result: %d",result);
      }
      avsync_lib_ptr->avt_drv_handle = NULL;
   }

   return result;
}

ADSPResult avsync_lib_deinit(void *inst_ptr)
{
   ADSPResult result = ADSP_EOK;
   if(NULL == inst_ptr)
   {
      return ADSP_EBADPARAM;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib %s: De-Init",avsync_lib_ptr->instance_name);

   if (NULL != avsync_lib_ptr->avsync_stats_ptr)
   {
      qurt_elite_memory_free(avsync_lib_ptr->avsync_stats_ptr);
      avsync_lib_ptr->avsync_stats_ptr = NULL;
   }
   avsync_lib_close_hw_resources(inst_ptr);
   return result;
}

ADSPResult avsync_lib_set_param(void *inst_ptr, uint32_t param_type, const void *param_val, uint32_t param_size)
{
   if((NULL == inst_ptr) || (NULL == param_val))
   {
      return ADSP_EBADPARAM;
   }
   ADSPResult res = ADSP_EOK;
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;

   switch(param_type)
   {
   case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_WINDOW_START_V2:
   case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_WINDOW_END_V2:
   case ASM_SESSION_MTMX_STRTR_PARAM_STAT_WINDOW_START_V2:
   case ASM_SESSION_MTMX_STRTR_PARAM_STAT_WINDOW_END_V2:
   {
      if(param_size != sizeof(asm_session_mtmx_strtr_param_window_v2_t))
      {
         res = ADSP_EBADPARAM;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib SetParam: param size mismatch, incoming size %lu",
               param_size);
         return res;
      }
      int64_t window_val;

      asm_session_mtmx_strtr_param_window_v2_t *window_val_ptr =
            (asm_session_mtmx_strtr_param_window_v2_t *)param_val;
      window_val = (int64_t)(((uint64_t)window_val_ptr->window_msw << 32) | (uint64_t)window_val_ptr->window_lsw);

      switch(param_type)
      {
      case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_WINDOW_START_V2:
         //Make sure that the Param Size is exactly equal to the structure size it is supposed to be holding

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Render Window Start = %lu(MSW), %lu(LSW)",
               window_val_ptr->window_msw, window_val_ptr->window_lsw);
         if(window_val > 0)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Render window start value +ve");
            avsync_lib_ptr->render_stat_window_status &= (~RENDER_WINDOW_START_MASK);
            return ADSP_EBADPARAM;
         }
         avsync_lib_ptr->render_window_start = window_val;
         avsync_lib_ptr->render_stat_window_status |= RENDER_WINDOW_START_MASK;
         //reset inst stats
         avsync_stats_reinit(avsync_lib_ptr->avsync_stats_ptr);
         break;
      case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_WINDOW_END_V2:

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Render Window End = %lu(MSW), %lu(LSW)",
               window_val_ptr->window_msw, window_val_ptr->window_lsw);
         if(window_val < 0)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Render window end value -ve");
            avsync_lib_ptr->render_stat_window_status &= (~RENDER_WINDOW_END_MASK);
            return ADSP_EBADPARAM;
         }
         avsync_lib_ptr->render_window_end = window_val;
         avsync_lib_ptr->render_stat_window_status |= RENDER_WINDOW_END_MASK;
         //resent inst stats
         avsync_stats_reinit(avsync_lib_ptr->avsync_stats_ptr);
         break;
      case ASM_SESSION_MTMX_STRTR_PARAM_STAT_WINDOW_START_V2:
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Stat Window Start = %lu(MSW), %lu(LSW)",
               window_val_ptr->window_msw, window_val_ptr->window_lsw);
         if(window_val > 0)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Stat window start value +ve");
            avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
            return ADSP_EBADPARAM;
         }
         if(NULL == avsync_lib_ptr->avsync_stats_ptr)
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: creating new instance of stats pointer");
            uint32_t size = avsync_stats_getsize();
            avsync_lib_ptr->avsync_stats_ptr = qurt_elite_memory_malloc(size, QURT_ELITE_HEAP_DEFAULT);
            if(NULL == avsync_lib_ptr->avsync_stats_ptr)
            {
               avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not create AV Sync Stats module");
               return ADSP_ENOMEMORY;
            }
            res = avsync_stats_init(avsync_lib_ptr->avsync_stats_ptr);
            if(ADSP_EOK != res)
            {
               avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not init AV Sync Stats module");
               return res;
            }
         }

         res = avsync_stats_set_param(avsync_lib_ptr->avsync_stats_ptr, STAT_WINDOW_START, &window_val);
         if(ADSP_EOK != res)
         {
            avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not set AV Sync stat window start");
            return res;
         }
         avsync_lib_ptr->render_stat_window_status |= STAT_WINDOW_START_MASK;
         //reset inst stats
         avsync_stats_reinit(avsync_lib_ptr->avsync_stats_ptr);

         break;
      case ASM_SESSION_MTMX_STRTR_PARAM_STAT_WINDOW_END_V2:
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Stat Window End = %lu(MSW), %lu(LSW)",
               window_val_ptr->window_msw, window_val_ptr->window_lsw);
         if(window_val < 0)
         {
            avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_END_MASK);
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Stat window end value -ve, value");
            return ADSP_EBADPARAM;
         }
         if(NULL == avsync_lib_ptr->avsync_stats_ptr)
         {
            uint32_t size = avsync_stats_getsize();
            avsync_lib_ptr->avsync_stats_ptr = qurt_elite_memory_malloc(size, QURT_ELITE_HEAP_DEFAULT);
            if(NULL == avsync_lib_ptr->avsync_stats_ptr)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not create AV Sync Stats module");
               avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
               return ADSP_ENOMEMORY;
            }
            res = avsync_stats_init(avsync_lib_ptr->avsync_stats_ptr);
            if(ADSP_EOK != res)
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not init AV Sync Stats module");
               avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
               return res;
            }
         }
         res = avsync_stats_set_param(avsync_lib_ptr->avsync_stats_ptr, STAT_WINDOW_END, &window_val);
         if(ADSP_EOK != res)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: Could not set AV Sync stat window end");
            avsync_lib_ptr->render_stat_window_status &= (~STAT_WINDOW_START_MASK);
            return res;
         }
         //reset inst stats
         avsync_stats_reinit(avsync_lib_ptr->avsync_stats_ptr);
         avsync_lib_ptr->render_stat_window_status |= STAT_WINDOW_END_MASK;
         break;
      }
   }
   break;
   case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION:
   {
      //Make sure that the Param Size is exactly equal to the structure size it is supposed to be holding
      if(param_size != sizeof(asm_session_mtmx_strtr_param_render_decision_t))
      {
         res = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib:Error, Param Size: %ld != RenderDecision SSize: %d",
               param_size, sizeof(asm_session_mtmx_strtr_param_render_decision_t));
         return res;
      }
      //Access the structure to get hold of the data
      asm_session_mtmx_strtr_param_render_decision_t *pData = (asm_session_mtmx_strtr_param_render_decision_t*)param_val;

      //Screen the value to make sure it is valid
      if(ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_LOCAL_STC < pData->flags)
      {

         return ADSP_EBADPARAM;
      }
      //Successful case
      avsync_lib_ptr->avsync_rendering_decision = pData->flags;

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSync Param Lib: RenderingDecision: %lu", pData->flags);
      break;
   }
   case ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY:
   {
      //Make sure that the Param Size is exactly equal to the structure size it is supposed to be holding
      if(param_size != sizeof(asm_session_mtmx_strtr_param_clock_recovery_t))
      {
         res = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib:Error, Param Size: %ld != ClockRecovery SSize: %d",
               param_size, sizeof(asm_session_mtmx_strtr_param_clock_recovery_t));
         return res;
      }
      //Access the structure to get hold of the data
      asm_session_mtmx_strtr_param_clock_recovery_t *pData = (asm_session_mtmx_strtr_param_clock_recovery_t*)param_val;

      //Screen the value to make sure it is valid
      if(ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_INDEPENDENT < pData->flags)
      {

         return ADSP_EBADPARAM;
      }

      //Successful case
      avsync_lib_ptr->avsync_clk_recovery_mechanism = pData->flags;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSync Param Lib: ClockRecoveryMode: %lu", pData->flags);

      break;
   }
   default:
      return ADSP_EBADPARAM;
   }

   return ADSP_EOK;
}

ADSPResult avsync_lib_update_s2d_drift(void *inst_ptr, uint32_t input_data_length, uint64_t current_timestamp)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;

   if(ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT == avsync_lib_ptr->avsync_clk_recovery_mechanism)
   {
      return ADSP_EOK;
   }
#ifdef DBG_AVSYNC_LIB
   volatile const int32_t *drift_s2pd_ptr = sd_drift_calc_get_drift_pointer(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr);
   int32_t drift_s2pd = 0;
   drift_s2pd = (NULL != drift_s2pd_ptr) ? (*drift_s2pd_ptr) : (0);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSync Lib, S2PD Drift: %ld",
         drift_s2pd);
#endif
   sd_drift_calc_update_drift(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr, current_timestamp, input_data_length);
   return ADSP_EOK;
}

ADSPResult avsync_lib_make_rendering_decision(void *inst_ptr,
      uint64_t input_timestamp,
      uint64_t downstream_delay,
      bool_t is_running,
      int64_t *delta,
      avsync_rendering_decision_t *rendering_decision)
{
   if(NULL == inst_ptr||NULL == delta||NULL == rendering_decision)
   {
      return ADSP_EFAILED;
   }

   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if((ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT == avsync_lib_ptr->avsync_rendering_decision) && (TRUE == is_running))
   {
      *delta = (int64_t)avsync_lib_ptr->expected_session_clock - input_timestamp;
#ifdef DBG_AVSYNC_LIB
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSyncLib: expected session clock %ld, input timestamp %ld",
            (uint32_t)avsync_lib_ptr->expected_session_clock,
            (uint32_t)input_timestamp);
#endif
   }
   else
   {
      *delta = (int64_t)avsync_lib_ptr->stc_value_rendering + (int64_t)downstream_delay - avsync_lib_ptr->start_time - (int64_t)input_timestamp;
#ifdef DBG_AVSYNC_LIB
      //Debug message
      MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSyncLib:input timestamp %lu,current time %lu, delta%ld, downstream delay %ld, start time %ld",
            (uint32_t)input_timestamp, (uint32_t)avsync_lib_ptr->stc_value_rendering, (int32_t)*delta, (int32_t)(downstream_delay),(int32_t)avsync_lib_ptr->start_time);
#endif
   }

   if(*delta >avsync_lib_ptr->render_window_end)
   {
      *rendering_decision =  DROP;
   }
   else if (*delta < avsync_lib_ptr->render_window_start )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSyncLib: determined to hold. render window %ld",
            (int32_t)avsync_lib_ptr->render_window_start);
      *rendering_decision = HOLD;
   }
   else
   {
      *rendering_decision = RENDER;
   }

   return ADSP_EOK;
}

ADSPResult avsync_lib_make_simple_rendering_decision(void *inst_ptr,
      int64_t hold_drop_buffer_duration,
      avsync_rendering_decision_t *rendering_decision)
{

   if(NULL == inst_ptr||NULL == rendering_decision)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;

   if(hold_drop_buffer_duration < avsync_lib_ptr->render_window_start )
   {
      *rendering_decision = HOLD;
   }
   else if(hold_drop_buffer_duration > avsync_lib_ptr->render_window_end)
   {
      *rendering_decision = DROP;
   }
   else
   {
      *rendering_decision = RENDER;
   }
   return ADSP_EOK;
}


ADSPResult avsync_lib_query_stat(void *inst_ptr, uint32_t param_id, void *param_val)
{
   if(NULL == inst_ptr||NULL == param_val)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   return avsync_stats_query(avsync_lib_ptr->avsync_stats_ptr,param_id,param_val);
}

ADSPResult avsync_lib_update_stat(void *inst_ptr, int64_t delta, uint64_t data_burst_duration)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(avsync_lib_ptr->render_stat_window_status & (STAT_WINDOW_START_MASK | STAT_WINDOW_END_MASK))
   {
      avsync_stats_update(avsync_lib_ptr->avsync_stats_ptr, delta, data_burst_duration);
      return ADSP_EOK;
   }
   return ADSP_ENOTREADY;
}

ADSPResult avsync_lib_commit_stat(void *inst_ptr)
{
   ADSPResult result = ADSP_EOK;
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   result =avsync_stats_commit(avsync_lib_ptr->avsync_stats_ptr, avsync_lib_ptr->absolute_ts);
   return result;
}

ADSPResult avsync_lib_init_s2d_drift(void *inst_ptr)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Init s2d drift");
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(NULL == avsync_lib_ptr->avsync_drift_s2pd_lib_ptr)
   {
      //Call create
      avsync_lib_ptr->avsync_drift_s2pd_lib_ptr = sd_drift_calc_create();
      if(NULL == avsync_lib_ptr->avsync_drift_s2pd_lib_ptr)
      {
         return ADSP_ENOMEMORY;
      }
   }
   return ADSP_EOK;
}

ADSPResult avsync_lib_deinit_s2d_drift(void *inst_ptr)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVSyncLib: Deinit s2d drift");

   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(NULL != avsync_lib_ptr->avsync_drift_s2pd_lib_ptr)
   {
      //Call destroy
      sd_drift_calc_destroy(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr);
      avsync_lib_ptr->avsync_drift_s2pd_lib_ptr = NULL;
   }
   return ADSP_EOK;
}
ADSPResult avsync_lib_update_stc_clock(void * inst_ptr)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   {
      if(NULL != avsync_lib_ptr->avt_drv_handle)
      {
         avsync_lib_ptr->stc_value_rendering = avtimer_drv_get_time(avsync_lib_ptr->avt_drv_handle);
      }
   }
   return ADSP_EOK;
}

ADSPResult avsync_lib_update_session_clock(void *inst_ptr,
      uint64_t proposed_default_session_clock)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   switch(avsync_lib_ptr->avsync_rendering_decision)
   {
   case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT:
      avsync_lib_ptr->session_clock = proposed_default_session_clock;
      break;
   case ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_LOCAL_STC:
      avsync_lib_ptr->session_clock = avsync_lib_ptr->stc_value_rendering;
      avsync_lib_ptr->session_clock -= avsync_lib_ptr->start_time;
      break;
   }

#ifdef DBG_AVSYNC_LIB
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib: Session Clock %ld",(uint32_t)avsync_lib_ptr->session_clock);
#endif

   return ADSP_EOK;
}

ADSPResult avsync_lib_get_internal_param(void *inst_ptr, uint32_t param_type, void *param_payload)
{
   if(NULL == inst_ptr|| NULL == param_payload)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   switch(param_type)
   {
   case SESSION_CLOCK:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->session_clock),
            &(avsync_lib_ptr->session_clock),
            sizeof(avsync_lib_ptr->session_clock));
      break;
   case EXPECTED_SESSION_CLOCK:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->expected_session_clock),
            &(avsync_lib_ptr->expected_session_clock),
            sizeof(avsync_lib_ptr->expected_session_clock));
      break;
   case START_TIME:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->start_time),
            &(avsync_lib_ptr->start_time),
            sizeof(avsync_lib_ptr->start_time));
      break;
   case ABSOLUTE_TS:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->absolute_ts),
            &(avsync_lib_ptr->absolute_ts),
            sizeof(avsync_lib_ptr->absolute_ts));
      break;
   case RENDERING_DECISION_TYPE:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->avsync_rendering_decision),
            &(avsync_lib_ptr->avsync_rendering_decision),
            sizeof(avsync_lib_ptr->avsync_rendering_decision));
      break;
   case CLOCK_RECOVERY_TYPE:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->avsync_clk_recovery_mechanism),
            &(avsync_lib_ptr->avsync_clk_recovery_mechanism),
            sizeof(avsync_lib_ptr->avsync_clk_recovery_mechanism));
      break;
   case DEVICE_DRIFT_PTR:
      memscpy(param_payload,
            sizeof(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr),
            &avsync_lib_ptr->avsync_drift_s2pd_lib_ptr,
            sizeof(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr));
      break;
   case START_TIME_OFFSET:
       memscpy(param_payload,
               sizeof(avsync_lib_ptr->start_time_offset),
               &avsync_lib_ptr->start_time_offset,
               sizeof(avsync_lib_ptr->start_time_offset));
       break;
    default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib:Error: Unsupported Params.\n");
      return ADSP_EBADPARAM;

   }
   return ADSP_EOK;
}

ADSPResult avsync_lib_set_internal_param(void *inst_ptr, uint32_t param_type, const void *param_payload,uint32_t param_size)
{
   ADSPResult result = ADSP_EBADPARAM;
   if(NULL == inst_ptr|| NULL == param_payload)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   switch(param_type)
   {
   case SESSION_CLOCK:
      if (sizeof(uint64_t)==param_size)
      {
         avsync_lib_ptr->session_clock = *(uint64_t *)param_payload;
         result = ADSP_EOK;
#ifdef DBG_AVSYNC_LIB
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib: Session Clock %ld",(uint32_t)avsync_lib_ptr->session_clock);
#endif
      }

      break;
   case START_TIME:
      if (sizeof(int64_t)==param_size)
      {
         uint64_t current_wall_time = (NULL != avsync_lib_ptr->avt_drv_handle) ? (avtimer_drv_get_time(avsync_lib_ptr->avt_drv_handle)):(0);

         /* Update the variable used in the render decision with the client provided value*/
         if(ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME != avsync_lib_ptr->start_flag)
         {
            avsync_lib_ptr->start_time = *(uint64_t *)param_payload;
         }
         else
         {
            avsync_lib_ptr->start_time = *(uint64_t *)param_payload + current_wall_time;
         }

         /* Calculate the initial hold for the Rx Matrix along with the actual start time */
         switch(avsync_lib_ptr->start_flag)
         {
             case ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE:
             {
                 avsync_lib_ptr->start_time_calc = current_wall_time;
                 avsync_lib_ptr->start_time_offset = 0;
             }
             break;
             case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME:
             {
                 avsync_lib_ptr->start_time_calc = *(uint64_t *)param_payload;
                 avsync_lib_ptr->start_time_offset = (avsync_lib_ptr->start_time) - current_wall_time - (avsync_lib_ptr->start_time_ds_delay);
             }
             break;

             case ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY:
             {
                 avsync_lib_ptr->start_time_calc = *(uint64_t*)param_payload + current_wall_time;
                 avsync_lib_ptr->start_time_offset = *(uint64_t*)param_payload - (avsync_lib_ptr->start_time_ds_delay);
             }
             break;

             case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME:
             {
                 avsync_lib_ptr->start_time_calc = *(uint64_t*)param_payload + current_wall_time;
                 avsync_lib_ptr->start_time_offset = *(uint64_t*)param_payload;
             }
             break;

             //This case should never be hit
             default:
                 result=ADSP_EBADPARAM;
                 break;
         }

         if(avsync_lib_ptr->start_time_offset < 0)
             avsync_lib_ptr->start_time_offset = 0;

         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvSyncLib: Client Start Time as %ld with offset %ld and downstream delay %ld",(uint32_t)avsync_lib_ptr->start_time, (uint32_t)avsync_lib_ptr->start_time_offset,(uint32_t)avsync_lib_ptr->start_time_ds_delay);
         result = ADSP_EOK;

      }
      break;
   case ABSOLUTE_TS:
      if (sizeof(int64_t)==param_size)
      {
         avsync_lib_ptr->absolute_ts = *(uint64_t *)param_payload;
         result = ADSP_EOK;
      }
      break;
   case EXPECTED_SESSION_CLOCK:
      if (sizeof(uint64_t)==param_size)
      {
         avsync_lib_ptr->expected_session_clock = *(uint64_t *)param_payload;
         result = ADSP_EOK;
      }
      break;
   case RENDER_WINDOW_START:
      if(sizeof(int64_t) == param_size)
      {
         avsync_lib_ptr->render_window_start = *(int64_t *)param_payload;
         result = ADSP_EOK;
      }
      break;
   case RENDER_WINDOW_END:
      if(sizeof(int64_t) == param_size)
      {
         avsync_lib_ptr->render_window_end = *(int64_t *)param_payload;
         result = ADSP_EOK;
      }
      break;
   default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib:Error: Unsupported Params.\n");
      return ADSP_EBADPARAM;

   }
   return result;
}

ADSPResult avsync_lib_increment_expected_session_clock(void *inst_ptr, uint64_t proposed_increment)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   avsync_lib_ptr->expected_session_clock += proposed_increment;
   return ADSP_EOK;
}
void avsync_lib_set_device_drift_pointer(void *inst_ptr, const volatile uint64_t* dev_drift_ptr)
{
   if(NULL == inst_ptr||NULL == dev_drift_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib:Instance pointer invalid.\n");
      return;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(NULL == avsync_lib_ptr->avsync_drift_s2pd_lib_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSyncLib: device drift pointer invalid.\n");
      return;
   }
   sd_drift_calc_set_device_drift_pointer(avsync_lib_ptr->avsync_drift_s2pd_lib_ptr, dev_drift_ptr);
   return;
}
void avsync_lib_process_run_immediate(void *inst_ptr)
{
   if(NULL == inst_ptr)
   {
      return;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT != avsync_lib_ptr->avsync_rendering_decision)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AVSyncLib: Run immediate issued for render decision mode %ld. Reverting to default",avsync_lib_ptr->avsync_rendering_decision);
       avsync_lib_ptr->avsync_rendering_decision = ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT;
   }
   if(ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT != avsync_lib_ptr->avsync_clk_recovery_mechanism)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AVSyncLib: Run immediate issued for clock recovery mode %ld. Reverting to default",avsync_lib_ptr->avsync_clk_recovery_mechanism);
       avsync_lib_ptr->avsync_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;
   }
}

ADSPResult avsync_lib_process_run(void *inst_ptr, uint32_t start_flag, uint32_t downstream_delay)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   avsync_lib_ptr->start_time_ds_delay = downstream_delay;

   switch (start_flag)
   {
   case ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE:
   {

      //avsync_lib_process_run_immediate(inst_ptr);
      break;
   }
   case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME:
   case ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY:
   {
      //Ensure that Start Time is positive.
      if (0 > avsync_lib_ptr->start_time)
      {
         return ADSP_EBADPARAM;
      }
      /* Ensure that default render decision mode is not
       * paired with independent clock recovery */
      if((ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_DEFAULT == avsync_lib_ptr->avsync_rendering_decision)&&
            (ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT != avsync_lib_ptr->avsync_clk_recovery_mechanism))
      {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AVSyncLib: Default render decision configured for clock recovery mode %ld. Reverting to default",avsync_lib_ptr->avsync_clk_recovery_mechanism);
          avsync_lib_ptr->avsync_clk_recovery_mechanism = ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_DEFAULT;
      }

      break;
   }
   case ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME:
   {
      avsync_lib_process_run_immediate(inst_ptr);
      break;
   }
   }
   avsync_lib_ptr->start_flag = start_flag;
   return ADSP_EOK;
}

ADSPResult avsync_lib_reinit_stat(void *inst_ptr)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EBADPARAM;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   return avsync_stats_reinit(avsync_lib_ptr->avsync_stats_ptr);

}

ADSPResult avsync_lib_update_absolute_time(void *inst_ptr,int64_t adjustment, bool_t commit_stat)
{

   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   avsync_lib_ptr->absolute_ts = (NULL != avsync_lib_ptr->avt_drv_handle) ? (avtimer_drv_get_time(avsync_lib_ptr->avt_drv_handle)):(0);
   avsync_lib_ptr->absolute_ts += adjustment;
   if(commit_stat)
   {
      avsync_stats_commit(avsync_lib_ptr->avsync_stats_ptr,avsync_lib_ptr->absolute_ts);
   }
   return ADSP_EOK;

}

ADSPResult avsync_lib_check_set_default_rendering_window(void *inst_ptr,uint32_t proposed_value)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   if(!(avsync_lib_ptr->render_stat_window_status & (RENDER_WINDOW_START_MASK | RENDER_WINDOW_END_MASK)))
   {
      avsync_lib_ptr->render_window_end = (int64_t)proposed_value;
      avsync_lib_ptr->render_window_start = 0-avsync_lib_ptr->render_window_end;
   }
   return ADSP_EOK;

}

ADSPResult avsync_lib_set_immediate_localstc_starttime(void *inst_ptr, uint64_t ip_timestamp, uint64_t downstream_delay)
{
   if(NULL == inst_ptr)
   {
      return ADSP_EFAILED;
   }
   avsync_param_t *avsync_lib_ptr = (avsync_param_t *)inst_ptr;
   
   avsync_lib_ptr->start_time = (int64_t)avsync_lib_ptr->stc_value_rendering + (int64_t)downstream_delay - (int64_t)ip_timestamp;
   
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvSyncLib: Run immediate configured for STC render decision mode. Start time as:%ld, and current time %lu, downstream delay %ld, input timestamp %lu",
            (int32_t)avsync_lib_ptr->start_time,(uint32_t)avsync_lib_ptr->stc_value_rendering, (int32_t)(downstream_delay),(uint32_t)ip_timestamp );
   
   return ADSP_EOK;
}

