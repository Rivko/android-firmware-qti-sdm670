/*========================================================================*/
/**
@file avsync_lib.h

@brief AV Sync Library public header.

 */

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "avsync_lib" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_lib/lib/inc/avsync_lib.h#1 $

when       who     what, where, why
4/19/2014   YW      Created file.
========================================================================== */

#ifndef _AVSYNC_PARAMS_H
#define _AVSYNC_PARAMS_H
#include "qurt_elite.h"

enum avsync_rendering_decision_t
{
   RENDER = 0,
   DROP,
   HOLD
};

typedef struct avsync_param_t avsync_param_t;//TODO: Declare types here without defining it.

enum avsync_internal_param_id_t
{
   SESSION_CLOCK = 0,
   EXPECTED_SESSION_CLOCK,
   START_TIME,
   ABSOLUTE_TS,
   RENDERING_DECISION_TYPE,
   RENDER_WINDOW_START,
   RENDER_WINDOW_END,
   CLOCK_RECOVERY_TYPE,
   DEVICE_DRIFT_PTR,
   START_TIME_OFFSET,

};

static const uint64_t AVSYNC_LIB_ZERO_SESSION_CLOCK = 0;

/*get the size of AV Sync instance*/
uint32_t avsync_lib_getsize(void);

/*Initialize all instance parameters
 * To be called at init of port structure
 *
 * return  results*/
ADSPResult avsync_lib_init(void *inst_ptr, char *instance_name);

/*Deinitialize all instance parameters
 * to be called at deinit of port structure
 * Coupling avsync_lib_init
 *
 * return results*/
ADSPResult avsync_lib_deinit(void *inst_ptr);
/*Open HW resources, such as avtimer driver */
ADSPResult avsync_lib_open_hw_resources(void *inst_ptr);
/*Close HW resources that are opened*/
ADSPResult avsync_lib_close_hw_resources(void *inst_ptr);

/*used to handle all avsync setparams sent from HLOS
 * Including but not limited to Render/Stat Window Start/End
 * Rendering Decision, etc.
 * param[in] param_type  type of params
 * param[in] param_val   params payload, not necessarily one value.*/
ADSPResult avsync_lib_set_param(void *inst_ptr, uint32_t param_type, const void *param_val, uint32_t param_size);

/*Used to set internal params
 * param[in] param_type  type of params
 * param[in] param_val   params payload, not necessarily one value.*/
ADSPResult avsync_lib_set_internal_param(void *inst_ptr, uint32_t param_type, const void *param_val,uint32_t param_size);
/*Used to get internal params
 * param[in] param_type  type of params
 * param[in] param_val   params payload, not necessarily one value.*/
ADSPResult avsync_lib_get_internal_param(void *inst_ptr, uint32_t param_type, void *param_val);

/*Used to initialize s2d drift*/
ADSPResult avsync_lib_init_s2d_drift(void *inst_ptr);

/*Used to update stream to device drift*/
ADSPResult avsync_lib_update_s2d_drift(void *inst_ptr, uint32_t input_data_length, uint64_t current_timestamp);

/*Used to de-init stream to device drift*/
ADSPResult avsync_lib_deinit_s2d_drift(void *inst_ptr);

/*Used to make rendering decision based on current and incoming time values
 * param[in] input_timestamp      timestamp value of current input buffer
 * param[in] downstream_delay     delay between rendering at MXSR to point of playback off AFE
 * param[in] is_running           whether the desired service is in running(or waiting) state
 * param[in] start time           marked time of start of playback
 * param[out] delta               pointer to the delta value to be calculated
 * param[out] rendering_decision  pointer to the determined rendering decision
 * */
ADSPResult avsync_lib_make_rendering_decision(void *inst_ptr,
      uint64_t input_timestamp,
      uint64_t downstream_delay,
      bool_t is_running,
      int64_t *delta,
      avsync_rendering_decision_t *rendering_decision);

/*Make rendering decision with given delta value, used in MxOut
 * param[in] hold_drop_buffer_duration     given delta value
 * param[out] rendering decision           pointer to the determined rendering decision*/
ADSPResult avsync_lib_make_simple_rendering_decision(void *inst_ptr,
      int64_t hold_drop_buffer_duration,
      avsync_rendering_decision_t *rendering_decision);

/*Used as a wrapper function to update avsync stats*/
ADSPResult avsync_lib_update_stat(void *inst_ptr, int64_t delta, uint64_t data_burst_duration);

/*Used as a wrapper function to query avsync stat*/
ADSPResult avsync_lib_query_stat(void *inst_ptr, uint32_t param_id, void *param_val);
/*Used as a wrapper function to commit avsync stat*/
ADSPResult avsync_lib_commit_stat(void *inst_ptr);
/*Used as a wrapper function to reinit avaync stat*/
ADSPResult avsync_lib_reinit_stat(void *inst_ptr);

/*Used to update stc clock
 * Applicable to both local and remote stc clocks
 * */
ADSPResult avsync_lib_update_stc_clock(void * inst_ptr);

/*Used to update session clock
 * This is applicable both at steady state and first buffer
 * Note that caller is responsible to calculate the proposed session time values
 * param[out] session clock                  pointer to the session clock to be updated
 * param[in]  proposed_default_session_clock proposed session clock if under default rendering decision
 * param[in]  proposed_stc_value             proposed session clock if under stc based rendering decision
 * */
ADSPResult avsync_lib_update_session_clock(void *inst_ptr,
      uint64_t proposed_default_session_clock);
/*Used to increment expected sesion clock value*/
ADSPResult avsync_lib_increment_expected_session_clock(void *inst_ptr, uint64_t proposed_increment);

/*Used as a wrapper function to update device drift pointer*/
void avsync_lib_set_device_drift_pointer(void *inst_ptr, const volatile uint64_t* dev_drift_ptr);
/*Used to mark rendering decision to default under run immediate*/
void avsync_lib_process_run_immediate(void *inst_ptr);
/*used to update the value for get_session_time if the rendering decision is stc based
 * To be used in RUN state only for SR.
 * param[out] session_time pointer to the session time to be checked against
 * param[in]  start_time   if under stc based rendering decision, the result would be substracted of start_time*/
void avsync_lib_check_and_adjust_get_session_time(void *inst_ptr, uint64_t *session_time);
/*Increment the absolute time*/
ADSPResult avsync_lib_update_absolute_time(void *inst_ptr,int64_t adjustment, bool_t commit_stat);
/*Check if default rendering window has been set, and set it if not*/
ADSPResult avsync_lib_check_set_default_rendering_window(void *inst_ptr,uint32_t proposed_value);
/*Set the avsync related parameters when processing run command*/
ADSPResult avsync_lib_process_run(void *inst_ptr, uint32_t start_flag, uint32_t downstream_delay);
/*Set the start time for run immediate with STC render decision mode */
ADSPResult avsync_lib_set_immediate_localstc_starttime(void *inst_ptr, uint64_t ip_timestamp, uint64_t downstream_delay);

#endif /*_AVSYNC_PARAMS_H*/
