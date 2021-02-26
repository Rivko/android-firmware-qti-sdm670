/*
============================================================================

FILE:          rs_driver.h

DESCRIPTION:   Resampler HW driver header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================
============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/inc/rs_driver.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

09/08/11   FZ      Created file

========================================================================== */

#include <stringl/stringl.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef RS_DRIVER_H
#define RS_DRIVER_H

#include "typedef_drivers.h"
#include "qurt_elite.h"
#include "qurt.h"

#define MAX_MULTI_RESAMP_CHANNELS        32   

/**
 * The following define is for HW Interrupt triger event type
 */ 

typedef enum _rs_job_event_type
{
    RS_JOB_EVEN_INVALID     = -1,
    RS_JOB_EVEN_JOB_DONE    = 1,
    RS_JOB_EVEN_JOB_ERROR   = 2,
} rs_job_event_type;

/**
* The following define is for HW resampler job status
*/
typedef enum _rs_job_status_type
{
   RS_JOB_STATUS_INVALID = -1,
   RS_JOB_STATUS_ONGOING = 0,      //job not finish yet
   RS_JOB_STATUS_DONE,             //job done
   RS_JOB_STATUS_AHB_ERROR,            //job error
   RS_JOB_STATUS_SAMPLE_COUNT_ERROR,            //job error
} rs_job_status_type;

/**
* The following define is for HW resampler operation mode.
*/
typedef enum _rs_operation_type
{
   RS_OPERATION_INVALID = -1,
   RS_OPERATION_UPSAMPLING = 1,    //up sampling only
   RS_OPERATION_DNSAMPLING,        //down sampling only
   RS_OPERATION_RESAMPLING,        //up sampling first then down sampling = Both, 
                                   //for example of 48 KHz to 44.1 KHz: upsampling 48 K to 88.2 K first then divide it by 2
} rs_operation_type;

/**
* The following define is for HW resampler operation priority.
*/
typedef enum _rs_priority_type
{
   RS_PRIORITY_INVALID = -1,
   RS_PRIORITY_LOW = 0,    
   RS_PRIORITY_HIGH,        
} rs_priority_type;

/**
* The following define is for HW resampler job submit type.
*/
typedef enum _rs_job_submit_type
{
   RS_JOB_INVALID = -1,
   RS_JOB_CANCEL = 0, 
   RS_JOB_SUBMIT,           
} rs_job_submit_type;

/**
* The following define is for HW resampler coeffcient source type.
*/
typedef enum _rs_coeff_src_type
{
   RS_COEFF_SRC_INVALID = -1,
   RS_COEFF_SRC_ROM = 0,  
   RS_COEFF_SRC_CUSTOMER,               //coeff array is put in RAM      
} rs_coeff_src_type;

/**
* The following define is for HW resampler coeffcient type. It is valid only when coeff source is ROM.
*/
typedef enum _rs_coeff_quality_type
{
   RS_COEFF_QUALITY_INVALID = -1,
   RS_COEFF_QUALITY_HQ = 0,    //coeff bit width = 16
   RS_COEFF_QUALITY_SHQ,       //coeff bit width = 20 
} rs_coeff_quality_type;

/**
* The following define is for HW resampler coeffcient type. It is valid only when coeff source is ROM.
*/
typedef enum _rs_sample_width_type
{
   RS_SAMPLE_WIDTH_INVALID = -1,
   RS_SAMPLE_WIDTH_16 = 0,    
   RS_SAMPLE_WIDTH_32,       
} rs_sample_width_type;

/**
* The following define is for HW upsampler filter type.
*/
typedef enum _rs_upsamp_filter_type
{
   RS_UPSAMPLER_FILTER_ID_INVALID = -1,
   RS_UPSAMPLER_FILTER_ID_1344 = 0,    
   RS_UPSAMPLER_FILTER_ID_8960,
   RS_UPSAMPLER_FILTER_ID_MAX,       
} rs_upsamp_filter_type;

/**
* The following define is for HW downsampler filter type.
*/
typedef enum _rs_dnsamp_filter_type
{
   RS_DNSAMPLER_FILTER_ID_INVALID = -1,
   RS_DNSAMPLER_FILTER_ID_1344 = 0,    
   RS_DNSAMPLER_FILTER_ID_8960,           //not supported    
   RS_DNSAMPLER_FILTER_ID_MAX,       
} rs_dnsamp_filter_type;

/**
* The following define is for HW resampler interpolation type.
*/
typedef enum _rs_interp_type
{
   RS_INTERP_MODE_INVALID = -1,
   RS_INTERP_MODE_LINEAR = 0,
   RS_INTERP_MODE_QUAD,
   RS_INTERP_MODE_MAX,       
} rs_interp_type;

/**
* The following define is for HW resampler force interpolation type. 
*/
typedef enum _rs_force_interpol_type
{
   RS_FORCE_INTERPOL_INVALID = -1,
   RS_FORCE_INTERPOL_OFF = 0,    //normal operation
   RS_FORCE_INTERPOL_ON,       //force fractional operation, used in streaming case
} rs_force_interpol_type;


/**
* The following define is for HW resampler dynamic resampler mode. 
*/
typedef enum _rs_dynamic_resampler_type
{
   RS_DYNAMIC_RESAMPLER_INVALID = -1,
   RS_DYNAMIC_RESAMPLER_OFF = 0,    //normal operation
   RS_DYNAMIC_RESAMPLER_ON,       //dynamic resampler mode
} rs_dynamic_resampler_type;


/** \brief resampler driver upsampler parameter struct */
typedef struct _struct_rs_upsampler
{
   /** integer phase step size */
   uint16  int_phase_step_size;     // = samp_inc;         
   /** current integer phase */
   uint16  cur_int_phase;           // = samp;             
   /** fraction phase step size */
   uint32  frac_phase_step_size;    // = sub_samp_inc;     
   /** current fraction phase */
   uint32  cur_frac_phase;          // = sub_samp;         
   /** upsampler out freq */
   uint32  out_freq;  
   /** fraction const */
   uint32  frac_const;   
   /** shift const */
   uint16  shift_const;
   /** up sampler filter memory size */
   int16   up_filt_mem_size;
} struct_rs_upsampler;  

/** \brief resampler driver downsampler parameter struct */
typedef struct _struct_rs_dnsampler
{
   /** upsampler structure */
   struct_rs_upsampler  up_samp_struct;
   /** down sampler filter memory size */
   int16    dn_filt_mem_size;       
   /** down sampler ratio. input stream is divided by this factor */
   int16    ds_factor;              
   /** down sampler step size - number of coefficeints that needs to be skipped*/
   int16    ds_step;                
   /** location of the end of the UpSampler filter = ptrDnSampStruct->filtMemOffset */
   int16   up_sampler_flt_fptr;     //UPSAMPLER ONLY  up_sampler_flt_fptr = upsamp.filtMemOffset  //DOWN SAMPLER ONLY  up_sampler_flt_fptr = 0                    //RESAMPLER  up_sampler_flt_fptr = upsamp.filtMemOffset
   /** location of next sample generated by UPsampler */
   int16   up_sampler_flt_optr;     //UPSAMPLER ONLY  up_sampler_flt_optr = upsamp.outputOffset   //DOWN SAMPLER ONLY  up_sampler_flt_optr = 0                    //RESAMPLER  up_sampler_flt_optr = dnsamp.inputOffset 
   /** location of next input sample read from memory  = ptrDnSampStruct->inputOffset */
   int16   resampler_flt_iptr;      //UPSAMPLER ONLY  resampler_flt_iptr = upsamp.inputOffset     //DOWN SAMPLER ONLY  resampler_flt_iptr = dnsamp.inputOffset    //RESAMPLER  resampler_flt_iptr = upsamp.inputOffset
   /** location of the end of the DnSampler Filter */
   int16   dn_sampler_flt_fptr;     //UPSAMPLER ONLY  dn_sampler_flt_fptr = 0                     //DOWN SAMPLER ONLY  dn_sampler_flt_fptr = dnsamp.filtMemOffset  //RESAMPLER  dn_sampler_flt_fptr = dnsamp.filtMemOffset
}struct_rs_dnsampler;

/** \brief resampler driver customer coeff parameter struct */ 
typedef struct _struct_rs_cust_coeff
{
   /** Starting address of custom coefficients */
   uint32 cust_coeff_base_addr;     
   /** customer coeff length */
   uint32 cust_coeff_length;        
   /** up sampler customer coeff offset  */
   uint16 us_cust_coeff_cfg_offset;  //in 16 bit word unit
   /** up sampler customer coeff filter taps */
   uint16 us_cust_coeff_cfg_filter_taps;
   /** up sampler customer coeff polyphase */
   uint16 us_cust_coeff_cfg_plyphase;
   /** down sampler customer coeff offset */
   uint16 ds_cust_coeff_cfg_offset;   //in 16 bit word unit
   /** down sampler customer coeff filter taps */
   uint16 ds_cust_coeff_cfg_filter_taps;
   /** need driver to allocate internal buffer for customer coeffcients. 1 - need  0 - not needed */
   uint16 internal_coeff_buf_needed;
}struct_rs_cust_coeff;


/** \brief resampler driver hw resampler job parameter struct */ 
typedef struct _struct_rs_job_config
{
   /** pointer to down sampler structure */
   struct_rs_dnsampler *ptr_dn_samp_struct;   
   /** pointer to customer coeff structure */
   struct_rs_cust_coeff *ptr_cust_coeff;
   /** operation type: Up/Down/Both */
   rs_operation_type operation;              
   /** priority: High/Low */
   rs_priority_type prio;                     
   /** number of channels, 1-8 */
   uint16 num_channels;                      
   /** sampler bit width: 16bit/24bit */
   rs_sample_width_type data_width;           
   /** coeff quality: 16bit/17bit/18bit  */
   rs_coeff_quality_type coeff_width;         
   /** submit this job or not: 1:submit 0:cancel */
   rs_job_submit_type submit_job;
   /** upsampler filter ID */
   rs_upsamp_filter_type upsamp_filt_id;
   /** dnsampler filter ID */
   rs_dnsamp_filter_type dnsamp_filt_id;
   /** hw resampler interpolation mode*/
   rs_interp_type interp_mode;
   /** hw resampler sync mode*/
   rs_force_interpol_type force_interpol;
   /** hw resampler dynamic resampler mode*/
   rs_dynamic_resampler_type dynamic_rs;
   /** coeff source: ROM and RAM */
   rs_coeff_src_type coeff_src;
   /** for internal input/output buffer 0: don't need, 1: need */
   uint16 internal_buf_needed;                
   /** the max num of input samples per channel, valid if internal_buf_needed = 1, 4096 is maxium */
   /** if this field is set as zero, driver will set it as 4096 */
   uint16 input_num_sample_max;                
} struct_rs_job_config;

/** \brief resampler driver buffer parameter struct */
typedef struct _struct_rs_buf
{
   /** buffer pointer. all samples in filt memory are uint32 format */
   uint32 buf_ptr;     
   /** buffer length in bytes */
   uint16 buf_length;   
} struct_rs_buf;

/** \brief HW Resampler driver for stream structure data updating */
typedef struct _struct_rs_stream_data
{
   /** integer phase step size */
   uint16  int_phase_step_size;     // = samp_inc;         
   /** fraction phase step size */
   uint32  frac_phase_step_size;    // = sub_samp_inc;     
   /** current fraction phase */
   uint32  cur_frac_phase;          // = sub_samp;         
   /** fraction const */
   uint32  frac_const;   
   /** shift const */
   uint16  shift_const;
   /** upsampler out freq */
   uint32  med_freq;  
} struct_rs_stream_data;

/** \brief HW Resampler driver callback data type */
typedef struct _struct_rs_cb_data
{
   /** Resampler driver return result */
   /** ADSP_EOK SUCCESS others FAILURE */
   ADSPResult           result;
   /** job ID */
   uint16               job_id;
   /** Error type */
   rs_job_status_type   hw_error_type; 
} struct_rs_cb_data;

/** \brief HW Resampler driver callback function type */
typedef void (*rs_job_event_callback_func)
(
   /** A void pointer to user defined context */
   void                   *user_ctx_ptr,
   /** Callback event type */
   rs_job_event_type      event,
   /** Callback data type */
   void      *cb_data_ptr
);


/** \brief resampler driver hw resampler job timing info struct */ 
typedef struct _struct_rs_job_timing_info
{
   int32  num_input_sample_per_frame;
   int32  num_output_sample_per_frame;
   /** frame size in microsecond unit. the frame szie should be matched with the num input sample sent via rs_send_job() */
   /** for example, for 48000 Hz to 44100 Hz resampling case, if frame_size = 10000 ns, then the num_input_sample for every frame is 48. */
   int32 frame_size;
   /** the time in microsecond unit client requires hw resampler done the job for this frame */
   int32 budget_time_to_done;
   /** the time in microsecond unit that hw resampler can finish the job. it is returned back from hw rs driver*/
   int32 estimated_time_to_done;
} struct_rs_job_timing_info;

/**
* This resampler driver init function and it is only called once.
*
* @return Success/failure of resampler init.
*/
ADSPResult rs_init(void);

/**
* This resampler driver deinit function and it is only called once.
*
* @return Success/failure of resampler deinit.
*/
ADSPResult rs_deinit(void);

/**
* This function is for resampler job init.
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_struct - point to job configure struct.
* @param[in] cb_func - callback function when job is done
* @param[in] user_ctx - user defined ctx used in callback function
* @return job ID, valid job ID is 0 to 15.
*/
int16 rs_init_job(uint16 px, struct_rs_job_config* job_struct, rs_job_event_callback_func cb_func, void* user_ctx);

/**
* This function is for sending the input samplers to driver
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_id - job ID.
* @param[in] in_samples - the num of input samples
* @param[in] out_samples - the expected num of output samples.
* @param[in] in_buf_ptr - pointer to an array which specific the input buffer pointer for every channel.
* @param[in] out_buf_ptr - pointer to an array which specific the output buffer pointer for every channel.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_send_job(uint16 px, int16 job_id, uint16 in_samples, uint16 out_samples, int32** in_buf_ptr, int32** out_buf_ptr);

/**
* This function is for reset hw resampler
* It is used in fast farward/rewind cases 
* The frame sent via rs_send_job() is treated as the first frame after rs_reset_job() called
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_id - job ID.
* @param[in] job_struct - point to job configure struct. Some parameters are updated
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_reset_job(uint16 px, int16 job_id, struct_rs_job_config* job_struct);

/**
* This function is for confirming the job. It is called after client getting the callback function. 
* The output samples will be in output buffer after this function call
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_id - job ID.
* @param[in/out] samples_processed - num of samples has been processed
* @return job status.
*/
rs_job_status_type rs_confirm_job(uint16 px, int16 job_id, uint32 *samples_processed);

/**
* This function is for uninit this job
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_id - job ID.
* @return void
*/
void rs_uninit_job(uint16 px, int16 job_id);

/**
* This function is for querying the stream configuration
*
* @param[in] job_id - job ID.
* @param[in] ptr_rs_dnsamp - point to struct_rs_dnsampler struct.
* @return Success/failure of query.
*/
ADSPResult rs_get_stream_config(int16 job_id, struct_rs_dnsampler *ptr_rs_dnsamp);

/**
* This function is for setting the stream configuration
*
* @param[in] job_id - job ID.
* @param[in] ptr_rs_dnsamp - point to struct_rs_dnsampler struct.
* @return Success/failure of query.
*/
ADSPResult rs_set_stream_config(int16 job_id, struct_rs_dnsampler *ptr_rs_dnsamp);

/**
* This function is for updaing the stream configuration
*
* @param[in] job_id - job ID.
* @param[in] ptr_rs_str_data - point to struct_rs_stream_data struct.
* @param[in] flag - for internal use only, 6 - update all parameters in struct_rs_stream_data; others: only int_phase_step_size and frac_phase_step_size
* @return Success/failure of query.
*/
ADSPResult rs_set_stream_data(int16 job_id, struct_rs_stream_data *ptr_rs_str_data, uint16 flag);

/**
 * This function is for estimating the core clk
 *
 * @param[in] job_id the job_id of the stream
 * @param[in] ptr_job_timing_info pointer to struct struct_rs_job_timing_info
 * @return ADSP_EOK if succesful, else failure
 */
ADSPResult rs_estimate_core_clks(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info);


/**
 * This function is for querying hw resampler version
 *
 * @return hw resampler version
 */
uint32 rs_query_hw_version(void);


#endif

#ifdef __cplusplus
}
#endif //__cplusplus


