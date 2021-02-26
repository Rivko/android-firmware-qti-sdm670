/* ======================================================================== */
/**
   @file capi_v2_utils.h

   Header file to implement utility functions for CAPIv2 Interface for
   DECIMATE example
 */

/* =========================================================================
   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
   All rights reserved. Qualcomm Technologies Proprietary and Confidential.
   ========================================================================= */
/*------------------------------------------------------------------------
 * Include files and Macro definitions
 * -----------------------------------------------------------------------*/

#ifndef CAPI_V2_DECIMATE_UTILS_H
#define CAPI_V2_DECIMATE_UTILS_H

#include "Elite_CAPI_V2_properties.h"
#include "Elite_CAPI_V2.h"
#include "HAP_farf.h"
#include "HAP_mem.h"
#include "string.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define CAPI_V2_DECIMATE_MAX_IN_PORTS     1
#define CAPI_V2_DECIMATE_MAX_OUT_PORTS    1
#define CAPI_V2_DECIMATE_MAX_CHANNELS 2
#define TEMP_BUF_SIZE_IN_SAMPLES 240
#define DECIMATE_DELAY_US 1000
static const uint32_t CAPI_V2_DECIMATE_STACK_SIZE   	= 4096;
static const uint32_t CAPI_V2_DECIMATE_MAX_SAMPLE_RATE = 384000/2; // Max Sample rate Supported by the DECIMATE Library
static const uint32_t PCM_16BIT_Q_FORMAT = 15;

typedef struct io_buffer_ptr
{
	uint32_t input_num_samples;
	uint32_t output_num_samples;
	uint32_t words_per_samples;  // 1 for 16 bit, 2 for 32 bit
	void *inp_ptrs[CAPI_V2_DECIMATE_MAX_CHANNELS];
	void *out_ptrs[CAPI_V2_DECIMATE_MAX_CHANNELS];
}io_buffer_ptr_t;

typedef struct capi_v2_decimate_media_fmt
{
	capi_v2_set_get_media_format_t main;
	capi_v2_standard_data_format_t std_fmt;

} capi_v2_decimate_media_fmt_t;

typedef struct capi_v2_decimate_events_config
{
	uint32_t enable;
	uint32_t KPPS;
	uint32_t delay;
} capi_v2_decimate_events_config_t;

typedef struct capi_v2_decimate_lib_config
{
	uint32_t decimatest_frame;
	uint32_t frame_process_mode;
	int32_t numOutPredSamples;
	int32_t numInPredSamples;
	int32_t config_params[20];
} capi_v2_decimate_lib_config_t;

typedef struct capi_v2_decimate_memory
{
	int16_t* coeff_ptr;
	int16_t* temp_lin_ptr;
	int16_t* temp_rin_ptr;

} capi_v2_decimate_memory_t;

typedef struct capi_v2_decimate
{
	capi_v2_t                               vtbl;
	capi_v2_event_callback_info_t           cb_info;
	capi_v2_decimate_media_fmt_t   input_media_fmt[CAPI_V2_DECIMATE_MAX_IN_PORTS];
	capi_v2_decimate_media_fmt_t   output_media_fmt[CAPI_V2_DECIMATE_MAX_OUT_PORTS];
	capi_v2_decimate_lib_config_t  lib_config;
	capi_v2_decimate_memory_t      decimate_mem;
	capi_v2_decimate_events_config_t events_config;
	io_buffer_ptr_t                         data_ptrs;
	uint32_t enable_flag;
	uint32_t decimation_factor;
	uint32_t *out_samples;

} capi_v2_decimate_t;

capi_v2_err_t capi_v2_decimate_process_set_properties(capi_v2_decimate_t* me_ptr,
		capi_v2_proplist_t *proplist_ptr);
capi_v2_err_t capi_v2_decimate_process_get_properties(capi_v2_decimate_t *me_ptr, capi_v2_proplist_t *proplist_ptr);

void capi_v2_decimate_raise_process_event(capi_v2_decimate_t *me_ptr);
void capi_v2_decimate_init_media_fmt(capi_v2_decimate_t *me_ptr);
void capi_v2_decimate_release_memory(capi_v2_decimate_t *me_ptr);
void capi_v2_decimate_raise_event(capi_v2_decimate_t *me_ptr);
void capi_v2_decimate_raise_output_media_format_event(capi_v2_decimate_t *me_ptr);
int capi_v2_decimate_get_kpps(capi_v2_decimate_t* me_ptr);

#ifdef __cplusplus
}
#endif
#endif /* CAPI_V2_DECIMATE_UTILS_H */
