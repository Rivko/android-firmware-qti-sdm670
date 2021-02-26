/*========================================================================
 *//** @file avsync_param_lib.cpp
This file contains stubbed functions for AVSync Library

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_lib/lib/stub_src/avsync_lib_stub.cpp#1 $

when            who    what, where, why
--------           ---        -------------------------------------------------------
07/16/15    KR      Created file.
========================================================================== */
#include "qurt_elite.h"
#include "avsync_lib.h"

uint32_t avsync_lib_getsize()
{
	return 0;
}

ADSPResult avsync_lib_init(void *inst_ptr, char *instance_name)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_open_hw_resources(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_close_hw_resources(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_deinit(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_set_param(void *inst_ptr, uint32_t param_type, const void *param_val, uint32_t param_size)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_update_s2d_drift(void *inst_ptr, uint32_t input_data_length, uint64_t current_timestamp)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_make_rendering_decision(void *inst_ptr,
		uint64_t input_timestamp,
		uint64_t downstream_delay,
		bool_t is_running,
		int64_t *delta,
		avsync_rendering_decision_t *rendering_decision)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_make_simple_rendering_decision(void *inst_ptr,
		int64_t hold_drop_buffer_duration,
		avsync_rendering_decision_t *rendering_decision)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_query_stat(void *inst_ptr, uint32_t param_id, void *param_val)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_update_stat(void *inst_ptr, int64_t delta, uint64_t data_burst_duration)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_commit_stat(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_init_s2d_drift(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_deinit_s2d_drift(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}
ADSPResult avsync_lib_update_stc_clock(void * inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_update_session_clock(void *inst_ptr, uint64_t proposed_default_session_clock)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_get_internal_param(void *inst_ptr, uint32_t param_type, void *param_payload)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_set_internal_param(void *inst_ptr, uint32_t param_type, const void *param_payload,uint32_t param_size)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_increment_expected_session_clock(void *inst_ptr, uint64_t proposed_increment)
{
	return ADSP_ENOTIMPL;
}

void avsync_lib_set_device_drift_pointer(void *inst_ptr, const volatile uint64_t* dev_drift_ptr)
{
	return;
}

void avsync_lib_process_run_immediate(void *inst_ptr)
{
	return;
}

ADSPResult avsync_lib_process_run(void *inst_ptr, uint32_t start_flag, uint32_t downstream_delay)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_reinit_stat(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_update_absolute_time(void *inst_ptr,int64_t adjustment, bool_t commit_stat)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_lib_check_set_default_rendering_window(void *inst_ptr,uint32_t proposed_value)
{
	return ADSP_ENOTIMPL;
}
