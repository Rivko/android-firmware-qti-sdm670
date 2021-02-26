/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/hal_rs_func_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */
/**
@file hal_rs_func_defs.h

@brief This file contains avtimer hardware abstraction layer.
*/

#ifndef HAL_RS_FUNC_DEFS_H__
#define HAL_RS_FUNC_DEFS_H__

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "HAL_rs.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
Init RS hal. 

@return
*/
typedef void (*HAL_rs_init_func)(uint32 base_virt_addr);

/**
Start rs hw. 

@return
*/
typedef void (*HAL_rs_start_func)(uint16 px, uint16 mask);


/**
Get the current channels

@return
 which channel.
*/
typedef uint16 (*HAL_rs_get_cur_channel_func)(uint16 px);

/**
Query current job ID

@return
*/
typedef uint16 (*HAL_rs_get_cur_job_id_func)(uint16 px);


/**
Query current operation status

@return

*/
typedef uint16 (*HAL_rs_get_cur_op_status_func)(uint16 px);


/**

Set rs stream configuration

@return
*/
typedef void (*HAL_rs_set_stream_config_func)(uint16 px, struct_rs_config* ptr_config_struct);

/**

Get rs stream configuration

@return
*/
typedef void (*HAL_rs_get_stream_config_func)(uint16 px, struct_rs_config* ptr_config_struct);



/**
Get rs stream structure size
num_channels starts from 1 to 32
 
@return

*/
typedef uint16 (*HAL_get_stream_struct_size_func)(uint16 num_channels);

/**
Set rs stream structure
num_channels starts from 1 to 32

@return
*/
typedef void (*HAL_rs_set_stream_struct_func)(uint16 px, struct_rs_stream* stream_struct, uint32* ptr_stream_mem, uint32* ptr_stream_mem_phys, uint16 num_channels);

/**
Get rs stream structure
 
 
@return
*/
typedef void (*HAL_rs_get_stream_struct_func)(uint16 px, struct_rs_stream* stream_struct, uint32* ptr_stream_mem, uint32* ptr_stream_mem_phys);


/**
Get rs ack
 
@return
*/
typedef boolean (*HAL_rs_is_ack_valid_func)(uint16 px);


/**
get rs job accepted status
 
 
@return
*/
typedef boolean (*HAL_rs_get_job_accepted_status_func)(uint16 px, uint16 *job_id);


/**
get rs job done status
 
 
@return
*/
typedef void (*HAL_rs_get_job_done_status_func)(uint16 px, uint16 *ptr_job_array);

/**
Set job clear
 
 
@return
*/
typedef void (*HAL_rs_set_job_clear_func)(uint16 px, uint16 job_id, uint16 this_job_status);


/**
get this job estimated clks
 
 
@return
*/
typedef uint32 (*HAL_rs_get_estimated_total_clks_hiq_func)(uint16 px);


/**
get this job estimated clks
 
 
@return
*/
typedef uint32 (*HAL_rs_get_estimated_total_clks_loq_func)(uint16 px);


/**
set this job estimated clks
 
 
@return
*/
typedef void (*HAL_rs_set_estimated_clks_func)(uint16 px, uint32 clks);


/////////
/**
get filter length
 
 
@return
*/
typedef uint32 (*HAL_rs_get_upsampler_flt_mem_length_func)(void);

/**
get filter length
 
 
@return
*/
typedef uint32 (*HAL_rs_get_upsampler_flt_coeff_length_hq_func)(void);

/**
get filter length
 
 
@return
*/
typedef uint32 (*HAL_rs_get_upsampler_flt_coeff_length_shq_func)(void);


/**
get filter length
 
 
@return
*/
typedef uint32 (*HAL_rs_get_downsampler_flt_coeff_length_hq_func)(void);


/**
get filter length
 
 
@return
*/
typedef uint32 (*HAL_rs_get_downsampler_flt_coeff_length_shq_func)(void);

/**
get down sampler output buffer size 

 
 
@return
*/
typedef uint32 (*HAL_rs_get_downsampler_flt_output_buf_size_func)(uint16 div_factor);

/**
get hw resampler burst load size 

The burst length for loading and saving filter states, input/oupt samples
 
@return  num of bytes
*/
typedef uint32 (*HAL_rs_get_memory_load_burst_size_func)(void);



/**
get num of resampler core parallel process pipeline
 
@return  num of pipeline
*/
typedef uint32 (*HAL_rs_get_parallel_pipeline_num_func)(void);


/**
Query data from clock table
 
@return  
*/
typedef uint32 (*HAL_rs_get_data_from_clk_tbl_func)(rs_hw_power_mode power_mode, rs_clock_idx clk_idx);


/**
get the lowest supported power mode
 
@return  power mode
*/
typedef rs_hw_power_mode (*HAL_rs_get_power_mode_lowest_func)(void);

/**
get the highest supported power mode
 
@return  power mode
*/
typedef rs_hw_power_mode (*HAL_rs_get_power_mode_highest_func)(void);


typedef struct
{
   HAL_rs_init_func HAL_rs_init_f;
   HAL_rs_start_func HAL_rs_start_f;
   HAL_rs_get_cur_channel_func HAL_rs_get_cur_channel_f;
   HAL_rs_get_cur_job_id_func HAL_rs_get_cur_job_id_f;
   HAL_rs_get_cur_op_status_func HAL_rs_get_cur_op_status_f;
   HAL_rs_set_stream_config_func HAL_rs_set_stream_config_f;
   HAL_rs_get_stream_config_func HAL_rs_get_stream_config_f;
   HAL_get_stream_struct_size_func HAL_get_stream_struct_size_f;
   HAL_rs_set_stream_struct_func HAL_rs_set_stream_struct_f;
   HAL_rs_get_stream_struct_func HAL_rs_get_stream_struct_f;
   HAL_rs_is_ack_valid_func HAL_rs_is_ack_valid_f;
   HAL_rs_get_job_accepted_status_func HAL_rs_get_job_accepted_status_f;
   HAL_rs_get_job_done_status_func HAL_rs_get_job_done_status_f;
   HAL_rs_set_job_clear_func HAL_rs_set_job_clear_f;
   HAL_rs_get_estimated_total_clks_hiq_func HAL_rs_get_estimated_total_clks_hiq_f;
   HAL_rs_get_estimated_total_clks_loq_func HAL_rs_get_estimated_total_clks_loq_f;
   HAL_rs_set_estimated_clks_func HAL_rs_set_estimated_clks_f;

   HAL_rs_get_upsampler_flt_mem_length_func HAL_rs_get_upsampler_flt_mem_length_f;
   HAL_rs_get_upsampler_flt_coeff_length_hq_func HAL_rs_get_upsampler_flt_coeff_length_hq_f;
   HAL_rs_get_upsampler_flt_coeff_length_shq_func HAL_rs_get_upsampler_flt_coeff_length_shq_f;
   HAL_rs_get_downsampler_flt_coeff_length_hq_func HAL_rs_get_downsampler_flt_coeff_length_hq_f;
   HAL_rs_get_downsampler_flt_coeff_length_shq_func HAL_rs_get_downsampler_flt_coeff_length_shq_f;

   HAL_rs_get_downsampler_flt_output_buf_size_func HAL_rs_get_downsampler_flt_output_buf_size_f;
   HAL_rs_get_memory_load_burst_size_func HAL_rs_get_memory_load_burst_size_f;

   HAL_rs_get_parallel_pipeline_num_func HAL_rs_get_parallel_pipeline_num_f;
   HAL_rs_get_data_from_clk_tbl_func HAL_rs_get_data_from_clk_tbl_f;

   HAL_rs_get_power_mode_lowest_func HAL_rs_get_power_mode_lowest_f;
   HAL_rs_get_power_mode_highest_func HAL_rs_get_power_mode_highest_f;

} hal_rs_func_def_t; 

extern hal_rs_func_def_t* hal_rs_func_def_ptr_g;

/** @} */ /* end_addtogroup hal_rs_func_defs.h */


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef HAL_RS_FUNC_DEFS_H__

