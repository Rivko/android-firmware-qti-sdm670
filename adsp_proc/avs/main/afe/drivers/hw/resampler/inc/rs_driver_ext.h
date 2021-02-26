/*
============================================================================

FILE:          rs_driver_ext.h

DESCRIPTION:   Resampler HW driver extension header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================
============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/inc/rs_driver_ext.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

05/24/12   FZ      Created file

========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef RS_DRIVER_EXT_H
#define RS_DRIVER_EXT_H

#include "qurt_elite.h"

//The max number of jobs in queue is 8. 
//The max number of jobs that resampler can process is 8.
//The number of IDs that can be used is 16 IDs. (valid ID is 0 to 15)

#define MAX_NUM_OF_HW_RESAMPLER_JOB        8   

typedef void* rs_drv_handle_t;

/**
 * HW Resampler driver extension IOCtrl call property
 */
typedef enum _rs_drv_property_type
{
   RS_DRV_PROPERTY_UNDEFINED,
   RS_DRV_PROPERTY_FORCE_INTERPOL_CONFIG = 0x1,  
   RS_DRV_PROPERTY_REGISTER_JOB_EVENT_CALLBACK = 0x2,  //this is used only for async call
   RS_DRV_PROPERTY_GET_REQUIRED_NUM_SAMPLES = 0x3,  
   RS_DRV_PROPERTY_ESTIMATE_CORE_CLK = 0x4, 
   RS_DRV_PROPERTY_DYNAMIC_RESAMPLER_CONFIG = 0x5, 

   RS_DRV_PROPERTY_GLOBAL = 0x20000000,
   RS_DRV_PROPERTY_HW_VERSION_QUERY, 
   RS_DRV_PROPERTY_MAX
}rs_drv_property_type;

/**
* HW resampler driver extension upsampler filter type define.
*/
typedef enum _rs_drv_upsamp_filter_type
{
   RS_DRV_UPSAMPLER_FILTER_ID_INVALID = -1,
   RS_DRV_UPSAMPLER_FILTER_ID_1344 = 0,    
   RS_DRV_UPSAMPLER_FILTER_ID_8960,
   RS_DRV_UPSAMPLER_FILTER_ID_MAX,       
} rs_drv_upsamp_filter_type;

/**
* HW resampler driver extension interpolation type define.
*/
typedef enum _rs_drv_interp_type
{
   RS_DRV_INTERP_MODE_INVALID = -1,
   RS_DRV_INTERP_MODE_LINEAR = 0,
   RS_DRV_INTERP_MODE_QUAD,
   RS_DRV_INTERP_MODE_MAX,       
} rs_drv_interp_type;

/**
* HW resampler driver extension force interpolation type.define 
*/
typedef enum _rs_drv_force_interpol_type
{
   RS_DRV_FORCE_INTERPOL_INVALID = -1,
   RS_DRV_FORCE_INTERPOL_OFF = 0,    //normal operation
   RS_DRV_FORCE_INTERPOL_ON,         //force fractional operation, used in streaming case
} rs_drv_force_interpol_type;


/**
* HW resampler driver extension dynamic resampler enable type.define 
*/
typedef enum _rs_drv_dynamic_resampler_type
{
   RS_DRV_DYNAMIC_RESAMPLER_INVALID = -1,
   RS_DRV_DYNAMIC_RESAMPLER_OFF = 0,    //normal operation
   RS_DRV_DYNAMIC_RESAMPLER_ON,         //dynamic resampler is enabled. used for fast/slow playabck
} rs_drv_dynamic_resampler_type;

/**
* The following define is to inform HW resampler driver extension which clock mismatch detect mode used by client. 
* This is used only in force fraction operation mode
* 
* HW Resampler is running on local system clcok. The input stream maybe from network which is running in remote clock domain
* The output stream maybe driven from audio render device which maybe in another clock domain
* For example, for no drift case, the input freq is 48000 Hz and output freq is 48000 Hz.
* If there is the drift between remote clock domain and local system clock, when we use the local system clock to count the incoming
* audio samples, we can get for example 48005 smaple in one sec
* If there is the drift between device render clock domain and local system clock, when we use the local system clock to count the output
* audio samples, we can get for example 47995 smaple in one sec
* So we have the below user cases:
* 1) The drift is happened in INPUT path only         --> RS_DRV_SAMPLE_RATE_DRIFT_MODE_INPUT
* 2) The drift is happened in OUTPUT path only        --> RS_DRV_SAMPLE_RATE_DRIFT_MODE_OUTPUT
* 3) The drift is happened in both                    --> RS_DRV_SAMPLE_RATE_DRIFT_MODE_BOTH
*
* Call sequence for FORCE_FRACTION_OPERATION mode
* 1. Configure job as FORCE_FRACTION_OPERATION mode and start job as normal 
* 2. Client detect the input or/and output sample rate period
* 3. If drift is detected, 
*          3-1 Use RS_DRV_PROPERTY_FORCE_INTERPOL_CONFIG to configure FORCE_FRACTION_OPERATION mode
*          3-2 Calculate the num of input/output smaples
*          3-3 Send the job
*
* for case like 8 Khz -> 6.1 KHz, the resampler should be set as
*   8 -> 24.4 -> 6.1
*   8 -> 24.4  -> force fraction mode
*   24.4 -> 6.1 -> normal down sampleing
*/
typedef enum _rs_drv_sample_rate_drift_mode
{
   RS_DRV_SAMPLE_RATE_DRIFT_MODE_INVALID = -1,
   RS_DRV_SAMPLE_RATE_DRIFT_MODE_INPUT = 0,      
   RS_DRV_SAMPLE_RATE_DRIFT_MODE_OUTPUT,        
   RS_DRV_SAMPLE_RATE_DRIFT_MODE_BOTH,         
} rs_drv_sample_rate_drift_mode;

/**
* HW resampler driver extension prediction mode define for calculate num of input/output samples 
*/
typedef enum _rs_drv_num_sample_predict_mode
{
   RS_DRV_NUM_SAMPLE_PREDICT_MODE_INVALID = -1,
   RS_DRV_NUM_SAMPLE_PREDICT_MODE_FIXED_INPUT = 0,      //Baseing on num of input samples to calculate the num of output samples
   RS_DRV_NUM_SAMPLE_PREDICT_MODE_FIXED_OUTPUT,         //Baseing on num of output samples to calculate the needed num of input samples
} rs_drv_num_sample_predict_mode;

/**
* HW Resampler driver extension IOCtl call property info struct.define 
*/
typedef struct 
{
    /** Property call type*/
    rs_drv_property_type  prop;       
    /** Data size of property call*/
    int32                  data_size;
    /** Data pointer of property call*/
    void                  *data_ptr;
    /** Reserved*/
    int32                  flags;             
} rs_drv_ioctl_prop_info_t;


/**
*  The structure define for a buffer that is passed into resampler driver extension functions
*/
typedef struct
{
   int8* data_ptr;                 /**< Data pointer to the raw data. */

   uint32 actual_data_len;     /**< Length of the valid data (in bytes). This
                                     field has multiple uses, depending on the
                                     context:\n
     - When this buffer is an input to a function, the caller specifies
       the number of valid bytes using this field. The callee overwrites this
       field with the number of bytes that were consumed.

     - When this buffer is the output of a function, the caller does not
       use this field. The callee fills this field with the number of generated
       valid bytes in the output buffer.
     */
   uint32 max_data_len;        /**< Total allocated size of the buffer (in
                                     bytes). When a pointer of type rs_drv_buf_t
                                     is passed as an input argument of a
                                     function, this parameter specifies the 
                                     allocated size of the buffer and is
                                     untouched by the callee. */
} rs_drv_buf_t;

/**
*  The structure is for a list of rs_drv_buf buffers. It can be used for functions
*  that require multiple input or output buffers.
*
*  This structure consists of a pointer to an array of type rs_drv_buf. For
*  multichannel de-interleaved data, each buffer contains data from all 
*  the channels in sequence.
*/
typedef struct
{
   rs_drv_buf_t* buf_ptr;     /**< Array of rs_drv_buf buffer elements. */
   uint32 bufs_num;            /**< Number of elements in the array. */
} rs_drv_buflist_t;


/**
* HW Resampler driver extension job open parameter struct.define 
*/
typedef struct _rs_drv_job_cfg
{
   /** input sample rate */
   int32  in_freq;
   /** output sample rate */
   int32  out_freq;
   /** number of channels, 1-8 */
   uint16 num_channels;                      
   /** sampler bit width: 16bit/32bit */
   uint16 samp_bit_width; 
    /** hw resampler upsampler filter type*/
   rs_drv_upsamp_filter_type upsamp_filt_id;
   /** hw resampler interpolation mode*/
   rs_drv_interp_type interp_mode;
   /** hw resampler force interpol mode*/
   rs_drv_force_interpol_type force_interpol;
   /** hw resampler dynamic resampler mode*/
   rs_drv_dynamic_resampler_type dynamic_resampler;
   /** for internal input/output buffer 0: don't need, 1: need */
   /** if this is set as 0, client should guarantee that both buffer pointer and buffer max length for each channel  */
   /** of input/output are 32 byte aligned. The memory for each channel is physical continus */
   /** if force_interpol is set as ON, set internal_buf_needed to 0 to avoid any internal buffer overwriten as the internal */
   /** input/output buffer sizes are calculated basing on the in_freq and out_freq which are not drifted */
   uint16 internal_buf_needed;                
   /** the max num of input samples per channel, valid if internal_buf_needed = 1, 4096 is maxium */
   /** if this field is set as zero, driver will set it as 4096 */
   uint16 input_num_sample_max;                
} rs_drv_job_cfg_t;

/**
* HW Resampler driver extension IOCtl property call - async mode configure struct.define 
*/
typedef struct 
{
   rs_drv_sample_rate_drift_mode mode;
   /** input sample rate */
   int32  in_freq;
   /** output sample rate */
   int32  out_freq;
} rs_drv_ioctl_prop_force_interpol_cfg_t;


/**
* HW Resampler driver extension IOCtl property call - get the num of required samples 
* For "Fixed Input Mode" --  mode = RS_DRV_NUM_SAMPLE_PREDICT_MODE_FIXED_INPUT
*      num_sample_in : specific the num of input sample by client
*      num_sample_out: the num of output sample returned by resampler driver 
* For "Fixed Output Mode" -- mode = RS_DRV_NUM_SAMPLE_PREDICT_MODE_FIXED_OUTPUT
*      num_sample_in : the num of input sample returned by resampler driver 
*      num_sample_out: specific the num of output sample by client
*/
typedef struct 
{
   rs_drv_num_sample_predict_mode mode;
   /** num of input samples */
   uint32  num_sample_in;
   /** num of output samples */
   uint32  num_sample_out;
} rs_drv_ioctl_prop_get_num_sample_t;

/**
* HW Resampler driver extension IOCtl property call - estimate hw resampler core clk 
*/
typedef struct 
{
   /** frame size in microsecond unit. the frame szie should be matched with the num input sample sent via rs_drv_job_process() */
   /** for example, for 48000 Hz to 44100 Hz resampling case, if frame_size = 10000 us, then the num_input_sample for every frame is 480. */
   int32 frame_size;
   /** the time in microsecond unit client requires hw resampler done the job for this frame */
   int32 budget_time_to_done;
   /** the time in microsecond unit that hw resampler can finish the job. it is returned back from hw rs driver*/
   int32 estimated_time_to_done;
} rs_drv_ioctl_prop_estimate_core_clk_t;


/**
* HW Resampler driver extension IOCtl property call - dynamic resampler mode configure struct.define 
*/
typedef struct 
{
   /** input sample rate */
   int32  in_freq;
   /** output sample rate */
   int32  out_freq;
} rs_drv_ioctl_prop_dynamic_resampler_cfg_t;

/**
* This resampler driver driver extension init function and it is only called once.
*
* @return Success/failure of resampler init.
*/
ADSPResult rs_drv_init(void);

/**
* This resampler driver driver extension deinit function and it is only called once.
*
* @return Success/failure of resampler deinit.
*/
ADSPResult rs_drv_deinit(void);

/**
* This function is for resampler driver extension job init.
* @param[in/out] rs_drv_h - this will be the service entry handle returned to the caller.
* @param[in] job_cfg_ptr - point to job configure struct.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_drv_job_open(rs_drv_handle_t *rs_drv_h, rs_drv_job_cfg_t* job_cfg_ptr);

/**
* This function is for processing the input/output data
*
* @param[in] rs_drv_h - input handle.
* @param[in] in_buflist_ptr - pointer to rs_drv_buflist.
* @param[in] out_buflist_ptr - pointer to rs_drv_buflist.
* @return Success/failure of sending this frame job.
* Note:
* 1. Client should use RS_DRV_PROPERTY_GET_REQUIRED_NUM_SAMPLES property call to get the num of input samples (for fixed output mode)
*    OR num of output samples (for fixed input mode) before calling this function
* 2. Supposing: 
*           the num of input samples = num_input_samples
*           the num of output samples = num_output_samples
* 3. Then set the input and output buffer as below: 
*    in_buflist_ptr->buf_ptr->actual_data_len = num_input_samples * sample_size_in_bytes
*    out_buflist_ptr->buf_ptr->actual_data_len = num_output_samples * sample_size_in_bytes
*
*/
ADSPResult rs_drv_job_process(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr);

/**
* This function is for processing the input/output data
*
* @param[in] rs_drv_h - input handle.
* @return Success/failure of sending this frame job.
* Note: 
* This function is used for fast farward/rewind cases 
* The frame sent via rs_drv_job_process() is treated as the first frame after rs_drv_job_reset() called.
*/
ADSPResult rs_drv_job_reset(rs_drv_handle_t rs_drv_h);

/**
* This function is for uninit this job
*
* @param[in] rs_drv_h - input handle
* @return void
*/
ADSPResult rs_drv_job_close(rs_drv_handle_t rs_drv_h);

/**
* HW Resampler driver extension IOCtl function
*
* @param[in] rs_drv_h - input handle
* @param[in] prop_info_ptr point to property info structure
* @return Success/failure IOCtl call.
*/
ADSPResult rs_drv_ioctl(rs_drv_handle_t rs_drv_h, rs_drv_ioctl_prop_info_t *prop_info_ptr); 

//
//async call APIs
//
/**
 * The following define is for HW Interrupt triger event type
 */ 

typedef enum _rs_drv_job_event_type
{
    RS_DRV_JOB_EVEN_INVALID     = -1,
    RS_DRV_JOB_EVEN_JOB_DONE    = 1,
    RS_DRV_JOB_EVEN_JOB_ERROR   = 2,
} rs_drv_job_event_type;

/** \brief HW Resampler driver callback function type */
typedef void (*rs_drv_job_callback_func)
(
   rs_drv_handle_t rs_drv_h,
   /** Callback event type */
   rs_drv_job_event_type      event,
   /** Callback data type */
   void      *cb_data_ptr
);


/**
* HW Resampler driver extension IOCtl property call - job event notification struct.define 
*/
typedef struct 
{
    /** Event type, not used now*/
    rs_drv_job_event_type               event;
    /** Callback function */
    rs_drv_job_callback_func    job_event_callback_func;
    /** User defined context */
    void *user_ctx_ptr;
} rs_drv_ioctl_prop_job_event_notification_t;


/**
* This function is for getting the output data and how many input data consumed
* This API is just used for async call case
*
* @param[in] rs_drv_h - input handle.
* @param[in] in_buflist_ptr - pointer to rs_drv_buflist.
* @param[in] out_buflist_ptr - pointer to rs_drv_buflist.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_drv_job_process_done(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr);
#endif

#ifdef __cplusplus
}
#endif //__cplusplus


