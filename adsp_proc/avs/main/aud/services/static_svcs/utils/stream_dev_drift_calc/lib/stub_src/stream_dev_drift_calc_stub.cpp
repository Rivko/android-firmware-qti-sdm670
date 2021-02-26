/*========================================================================
 *//** @file stream_dev_drift_calc_stub.cpp
This file contains stubbed functions for Stream to Device Drift Calculation Library

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/stream_dev_drift_calc/lib/stub_src/stream_dev_drift_calc_stub.cpp#1 $

when            who    what, where, why
--------           ---        -------------------------------------------------------
07/17/15    KR      Created file.
========================================================================== */
#include "qurt_elite.h"
#include "stream_dev_drift_calc.h"

sd_dc_t* sd_drift_calc_create(void)
{
	return NULL;
}

void sd_drift_calc_destroy(sd_dc_t* obj_ptr)
{
	return;
}

volatile const int32_t *sd_drift_calc_get_drift_pointer(sd_dc_t *obj_ptr)
{
	return NULL;
}

void sd_drift_calc_set_device_drift_pointer(sd_dc_t* obj_ptr, const volatile uint64_t* dev_drift_ptr)
{
	return;
}

void sd_drift_calc_update_drift(sd_dc_t* obj_ptr, uint64_t ts, uint32_t duration_in_us)
{
	return;
}
void sd_drift_update_given_drift(sd_dc_t* obj_ptr, int32_t drift_val)
{
	return;
}