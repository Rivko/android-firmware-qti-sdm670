/*========================================================================
 *//** @file avsync_stats_lib.cpp
This file contains functions for AVSync Statistics Library

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_stats_lib/lib/src/avsync_stats_lib.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
8/8/12     RP      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "avsync_stats_lib.h"
#include "adsp_mtmx_strtr_api.h"


/* =======================================================================
TYPEDEFS
========================================================================== */
struct region_stats_t
{
	uint64_t accum_duration;
	int64_t  accum_diff;
	uint32_t accum_frames;
};

struct overall_stats_t
{
	region_stats_t region_A;
	region_stats_t region_B;
	region_stats_t region_C;
};

struct avsync_stats_t
{
	int64_t stats_window_start;
	int64_t stats_window_end;
	bool_t  is_window_start_set;
	bool_t  is_window_end_set;
	overall_stats_t inst_stats;
	overall_stats_t cumu_stats;
	overall_stats_t temp_stats;
	bool_t is_reset;
	bool_t should_commit;
	uint64_t abs_time;
};


/* =======================================================================
FUNCTIONS
========================================================================== */

uint32_t avsync_stats_getsize()
{
	return sizeof(avsync_stats_t);
}


ADSPResult avsync_stats_init(void *inst_ptr)
{
	if(NULL == inst_ptr)
	{
		return ADSP_EBADPARAM;
	}

	memset(inst_ptr, 0, sizeof(avsync_stats_t));

	return ADSP_EOK;
}


ADSPResult avsync_stats_reinit(void *inst_ptr)
{
	if(NULL == inst_ptr)
	{
		return ADSP_EBADPARAM;
	}

	avsync_stats_t *stats_ptr = (avsync_stats_t *)inst_ptr;
	memset(&stats_ptr->inst_stats, 0, sizeof(overall_stats_t));
	stats_ptr->is_reset = TRUE;

	return ADSP_EOK;
}

ADSPResult avsync_stats_set_param(void *inst_ptr, e_param_t param_type, void *param_val)
{
	if((NULL == inst_ptr) || (NULL == param_val))
	{
		return ADSP_EBADPARAM;
	}

	avsync_stats_t *stats_ptr = (avsync_stats_t *)inst_ptr;

	switch(param_type)
	{
	case STAT_WINDOW_START:
		stats_ptr->stats_window_start = *((int64_t *)param_val);
		stats_ptr->is_window_start_set = TRUE;
		break;
	case STAT_WINDOW_END:
		stats_ptr->stats_window_end = *((int64_t *)param_val);
		stats_ptr->is_window_end_set = TRUE;
		break;
	default:
		return ADSP_EBADPARAM;
	}

	return ADSP_EOK;
}

ADSPResult avsync_stats_update(void *inst_ptr, int64_t difference, uint64_t duration)
{
	if(NULL == inst_ptr)
	{
		return ADSP_EBADPARAM;
	}

	avsync_stats_t *stats_ptr = (avsync_stats_t *)inst_ptr;

	if(!stats_ptr->is_window_start_set || !stats_ptr->is_window_end_set)
	{
		return ADSP_EOK;
	}

	stats_ptr->is_reset  = FALSE;
	stats_ptr->should_commit = TRUE;

	if(difference < stats_ptr->stats_window_start)
	{
		stats_ptr->temp_stats.region_A.accum_duration += duration;
		stats_ptr->temp_stats.region_A.accum_diff += difference;
		stats_ptr->temp_stats.region_A.accum_frames++;
	}
	else if((stats_ptr->stats_window_start <= difference) && (difference <= stats_ptr->stats_window_end))
	{
      stats_ptr->temp_stats.region_B.accum_duration += duration;
      stats_ptr->temp_stats.region_B.accum_diff += difference;
      stats_ptr->temp_stats.region_B.accum_frames++;
	}
	else
	{
      stats_ptr->temp_stats.region_C.accum_duration += duration;
      stats_ptr->temp_stats.region_C.accum_diff += difference;
      stats_ptr->temp_stats.region_C.accum_frames++;
	}

	return ADSP_EOK;
}


ADSPResult avsync_stats_commit(void *inst_ptr, uint64_t abs_time)
{
	if(NULL == inst_ptr)
    {
		return ADSP_EBADPARAM;
    }

	avsync_stats_t *stats_ptr = (avsync_stats_t *)inst_ptr;

	//commit only once every update.
	if (!stats_ptr->should_commit)
	{
	   return ADSP_EOK;
	}
	stats_ptr->should_commit = FALSE;

	stats_ptr->abs_time = abs_time;

	//do not commit the inst stats if reinit was called after the last update
	if(!stats_ptr->is_reset)
	{
		stats_ptr->inst_stats.region_A.accum_duration += stats_ptr->temp_stats.region_A.accum_duration;
		stats_ptr->inst_stats.region_B.accum_duration += stats_ptr->temp_stats.region_B.accum_duration;
		stats_ptr->inst_stats.region_C.accum_duration += stats_ptr->temp_stats.region_C.accum_duration;

		stats_ptr->inst_stats.region_A.accum_diff  += stats_ptr->temp_stats.region_A.accum_diff;
	   stats_ptr->inst_stats.region_B.accum_diff  += stats_ptr->temp_stats.region_B.accum_diff;
		stats_ptr->inst_stats.region_C.accum_diff  += stats_ptr->temp_stats.region_C.accum_diff;

		stats_ptr->inst_stats.region_A.accum_frames += stats_ptr->temp_stats.region_A.accum_frames;
		stats_ptr->inst_stats.region_B.accum_frames += stats_ptr->temp_stats.region_B.accum_frames;
		stats_ptr->inst_stats.region_C.accum_frames += stats_ptr->temp_stats.region_C.accum_frames;
	}

	stats_ptr->cumu_stats.region_A.accum_duration += stats_ptr->temp_stats.region_A.accum_duration;
	stats_ptr->cumu_stats.region_B.accum_duration += stats_ptr->temp_stats.region_B.accum_duration;
	stats_ptr->cumu_stats.region_C.accum_duration += stats_ptr->temp_stats.region_C.accum_duration;

	stats_ptr->cumu_stats.region_A.accum_diff += stats_ptr->temp_stats.region_A.accum_diff;
	stats_ptr->cumu_stats.region_B.accum_diff += stats_ptr->temp_stats.region_B.accum_diff;
	stats_ptr->cumu_stats.region_C.accum_diff += stats_ptr->temp_stats.region_C.accum_diff;

	stats_ptr->cumu_stats.region_A.accum_frames += stats_ptr->temp_stats.region_A.accum_frames;
	stats_ptr->cumu_stats.region_B.accum_frames += stats_ptr->temp_stats.region_B.accum_frames;
	stats_ptr->cumu_stats.region_C.accum_frames += stats_ptr->temp_stats.region_C.accum_frames;

	memset(&stats_ptr->temp_stats, 0, sizeof(overall_stats_t));

	return ADSP_EOK;
}

ADSPResult avsync_stats_query(void *inst_ptr, uint32_t param_id, void *param_val)
{
	if((NULL == inst_ptr) || (NULL == param_val))
	{
		return ADSP_EBADPARAM;
	}

	avsync_stats_t *stats_ptr = (avsync_stats_t *)inst_ptr;

	if(!stats_ptr->is_window_start_set || !stats_ptr->is_window_end_set)
	{
		return ADSP_ENOTREADY;
	}

	switch(param_id)
	{
	case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_INST_STATISTICS_V2:
	{
		int64_t average;
		asm_session_mtmx_strtr_session_inst_statistics_v2_t *inst_stats_ptr = (asm_session_mtmx_strtr_session_inst_statistics_v2_t *)param_val;
		inst_stats_ptr->absolute_time_msw = (uint32_t)(stats_ptr->abs_time >> 32);
		inst_stats_ptr->absolute_time_lsw = (uint32_t)stats_ptr->abs_time;

		//region A stats
		inst_stats_ptr->duration_region_A_msw = (uint32_t)(stats_ptr->inst_stats.region_A.accum_duration >> 32);
		inst_stats_ptr->duration_region_A_lsw = (uint32_t)stats_ptr->inst_stats.region_A.accum_duration;
		if (0 != stats_ptr->inst_stats.region_A.accum_frames)
		{
			average = stats_ptr->inst_stats.region_A.accum_diff/stats_ptr->inst_stats.region_A.accum_frames;
		}
		else
		{
			average = 0;
		}
		inst_stats_ptr->average_region_A_msw = (uint32_t)(average >> 32);
		inst_stats_ptr->average_region_A_lsw = (uint32_t)average;

		//region B stats
		inst_stats_ptr->duration_region_B_msw = (uint32_t)(stats_ptr->inst_stats.region_B.accum_duration >> 32);
		inst_stats_ptr->duration_region_B_lsw = (uint32_t)stats_ptr->inst_stats.region_B.accum_duration;
		if (0 != stats_ptr->inst_stats.region_B.accum_frames)
		{
			average = stats_ptr->inst_stats.region_B.accum_diff/stats_ptr->inst_stats.region_B.accum_frames;
		}
		else
		{
			average = 0;
		}
		inst_stats_ptr->average_region_B_msw = (uint32_t)(average >> 32);
		inst_stats_ptr->average_region_B_lsw = (uint32_t)average;

		//region C stats
		inst_stats_ptr->duration_region_C_msw = (uint32_t)(stats_ptr->inst_stats.region_C.accum_duration >> 32);
		inst_stats_ptr->duration_region_C_lsw = (uint32_t)stats_ptr->inst_stats.region_C.accum_duration;
		if (0 != stats_ptr->inst_stats.region_C.accum_frames)
		{
			average = stats_ptr->inst_stats.region_C.accum_diff/stats_ptr->inst_stats.region_C.accum_frames;
		}
		else
		{
			average = 0;
		}
		inst_stats_ptr->average_region_C_msw = (uint32_t)(average >> 32);
		inst_stats_ptr->average_region_C_lsw = (uint32_t)average;

		break;
	}
	case ASM_SESSION_MTMX_STRTR_PARAM_SESSION_CUMU_STATISTICS_V2:
	{
		int64_t average;
		asm_session_mtmx_strtr_session_cumu_statistics_v2_t *cumu_stats_ptr = (asm_session_mtmx_strtr_session_cumu_statistics_v2_t *)param_val;
		cumu_stats_ptr->absolute_time_msw = (uint32_t)(stats_ptr->abs_time >> 32);
		cumu_stats_ptr->absolute_time_lsw = (uint32_t)stats_ptr->abs_time;

		//region A stats
		cumu_stats_ptr->duration_region_A_msw = (uint32_t)(stats_ptr->cumu_stats.region_A.accum_duration >> 32);
		cumu_stats_ptr->duration_region_A_lsw = (uint32_t)stats_ptr->cumu_stats.region_A.accum_duration;
		if (0 != stats_ptr->cumu_stats.region_A.accum_frames)
		{
			average = stats_ptr->cumu_stats.region_A.accum_diff/stats_ptr->cumu_stats.region_A.accum_frames;
		}
		else
		{
			average = 0;
		}
		cumu_stats_ptr->average_region_A_msw = (uint32_t)(average >> 32);
		cumu_stats_ptr->average_region_A_lsw = (uint32_t)average;

		//region B stats
		cumu_stats_ptr->duration_region_B_msw = (uint32_t)(stats_ptr->cumu_stats.region_B.accum_duration >> 32);
		cumu_stats_ptr->duration_region_B_lsw = (uint32_t)stats_ptr->cumu_stats.region_B.accum_duration;
		if (0 != stats_ptr->cumu_stats.region_B.accum_frames)
		{
			average = stats_ptr->cumu_stats.region_B.accum_diff/stats_ptr->cumu_stats.region_B.accum_frames;
		}
		else
		{
			average = 0;
		}
		cumu_stats_ptr->average_region_B_msw = (uint32_t)(average >> 32);
		cumu_stats_ptr->average_region_B_lsw = (uint32_t)average;

		//region C stats
		average = 0;
		cumu_stats_ptr->duration_region_C_msw = (uint32_t)(stats_ptr->cumu_stats.region_C.accum_duration >> 32);
		cumu_stats_ptr->duration_region_C_lsw = (uint32_t)stats_ptr->cumu_stats.region_C.accum_duration;
		if (0 != stats_ptr->cumu_stats.region_C.accum_frames)
		{
			average = stats_ptr->cumu_stats.region_C.accum_diff/stats_ptr->cumu_stats.region_C.accum_frames;
		}
		else
		{
			average = 0;
		}
		cumu_stats_ptr->average_region_C_msw = (uint32_t)(average >> 32);
		cumu_stats_ptr->average_region_C_lsw = (uint32_t)average;

		break;

	}
	default:
	{
		return ADSP_EBADPARAM;
	}
	}

	return ADSP_EOK;
}





