/*========================================================================
 *//** @file avsync_stats_lib_stub.cpp
This file contains stubbed functions for AVSync Statistics Library

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/avsync_stats_lib/lib/stub_src/avsync_stats_lib_stub.cpp#1 $

when            who    what, where, why
--------           ---        -------------------------------------------------------
07/17/15    KR      Created file.
========================================================================= */
#include "qurt_elite.h"
#include "avsync_stats_lib.h"

uint32_t avsync_stats_getsize()
{
	return 0;
}

ADSPResult avsync_stats_init(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_stats_reinit(void *inst_ptr)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_stats_set_param(void *inst_ptr, e_param_t param_type, void *param_val)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_stats_update(void *inst_ptr, int64_t difference, uint64_t duration)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_stats_commit(void *inst_ptr, uint64_t abs_time)
{
	return ADSP_ENOTIMPL;
}

ADSPResult avsync_stats_query(void *inst_ptr, uint32_t param_id, void *param_val)
{
	return ADSP_ENOTIMPL;
}
