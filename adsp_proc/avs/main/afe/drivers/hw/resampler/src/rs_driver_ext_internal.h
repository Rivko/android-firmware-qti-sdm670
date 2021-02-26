/*
============================================================================

FILE:          rs_driver_ext_internal.h

DESCRIPTION:   Resampler HW driver internal header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================

============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/rs_driver_ext_internal.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

09/08/11   FZ      Created file

========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef RS_DRIVER_EXT_INTERNAL_H
#define RS_DRIVER_EXT_INTERNAL_H

#include "typedef_drivers.h"
#include "qurt_elite.h"
#include "qurt.h"
#include "rs_driver.h"
#include "rs_driver_ext.h"

#include  "resampler_32b_int.h"
#include  "resampler_32b_ext.h"

// These are the values to which the Dynamic resampling is tested using Resampler HW 
#define DYNAMIC_DN_EXTRA_DELAY                68
#define DYNAMIC_UP_EXTRA_DELAY                34
#define DYNAMIC_MAX_DSFACTOR                  2
#define UPSAMPLER_FILT_MEM_LENGTH             128
#define DNSAMPLER_FILT_MEM_LENGTH_1632_DIV_FACTOR_2             168
#define DNSAMPLER_FILT_OSR                    24

typedef struct
{
   uint32 input_samples_to_hw;
   uint32 output_samples_to_hw;
   rs_drv_job_callback_func job_event_cb_func;   //for async support only
   void *user_ctx_ptr;
}rs_drv_job_async_call_cfg_t;

typedef struct
{
   int16 job_id;
   qurt_elite_signal_t sig_job;      // signal for the job submitted
   qurt_elite_channel_t channel_job; // channel used for embeded the signal
   uint32 sig_mask;
   rs_operation_type op_mode;
   int32 osr;
   uint16 internal_buf_needed;
   rs_drv_job_cfg_t* in_job_cfg_ptr;
   rs_drv_job_async_call_cfg_t async_call_cfg;
   uint32 prev_out_freq;      //this parameter used only in FORCE_FRACTION mode 
   void* dynamic_rs_dn_samp_ptr;   //for dynamic resampler mode
}rs_drv_job_info_t;


typedef struct
{
   int32 num_samp_in;
   int32 num_samp_out;
} rs_drv_num_samp_t;

int32 rs_drv_init_upsamp(void* ptrUS, int32* param);
int32 rs_drv_init_dnsamp(void* pStruct, int32* param);

int32 rs_drv_estimate_upsampler_input_samples(int32 numOutSamps, uint16 job_id, int32 upSampOsr, rs_drv_dynamic_resampler_type dynamic_rs);   
int32 rs_drv_estimate_dnsampler_input_samples(int32 numOutSamps, uint16 job_id, int32 upSampOsr, int32 operation, rs_drv_dynamic_resampler_type dynamic_rs);   
int32 rs_drv_estimate_upsampler_output_samples(int32 numInSamps, uint16 job_id, int32 upSampOsr, rs_drv_dynamic_resampler_type dynamic_rs);  
int32 rs_drv_estimate_dnsampler_output_samples(int32 numInSamps, uint16 job_id, int32 upSampOsr, int32 operation, rs_drv_dynamic_resampler_type dynamic_rs);   

ADSPResult rs_drv_config_job(rs_drv_job_cfg_t* job_cfg_ptr, 
                             struct_rs_job_config* job_config_struct_ptr, 
                             struct_rs_dnsampler* dn_samp_hw_ptr,
                             typeUpSampStruct* up_samp_ptr,
                             typeDnSampStruct* dn_samp_ptr);

ADSPResult rs_drv_cal_num_samp_to_hw(rs_drv_job_info_t* job_info_ptr, rs_drv_num_samp_t* num_samp_max_ptr, rs_drv_num_samp_t* num_samp_to_hw_ptr);

ADSPResult rs_drv_cal_stream_data(rs_drv_job_info_t* job_info_ptr, int32 in_freq, int32 out_freq, int16 osr, struct_rs_stream_data *ptr_rs_str_data, rs_drv_sample_rate_drift_mode mode);

ADSPResult rs_drv_ioctl_global_prop(rs_drv_ioctl_prop_info_t *prop_info_ptr);



int32 rs_drv_init_dynamic_resamp(void* pStruct, int32* param);

ADSPResult rs_drv_reinit_dynamic_resamp_wrapper(rs_drv_job_info_t* job_info_ptr, int32 in_freq, int32 out_freq);

void rs_drv_reinit_dynamic_resamp(void* pStruct, int32 in_freq, int32 out_freq, int16 osr);

void rs_drv_reinit_upsamp(void* ptrUS, int32 inFreq, int32 outFreq);

#endif

#ifdef __cplusplus
}
#endif //__cplusplus


