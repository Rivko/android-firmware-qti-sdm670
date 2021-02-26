/*
============================================================================

FILE:          rs_driver.h

DESCRIPTION:   Resampler HW driver internal header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-25   qm   Initial version.
============================================================================
*/
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef RS_DRIVER_INTERNAL_H
#define RS_DRIVER_INTERNAL_H

#include "HAL_rs.h"
#include "rs_driver.h"

#include "hwd_devcfg.h"

#ifndef DRIVER_DEBUG 
//#define DRIVER_DEBUG
#endif

#define RS_ALIGN_32_BYTE(vaddr,vsize,rs_vaddr,rs_vsize) \
    { \
        uint32 rs_offset = ((vaddr) & 0x1f); \
        rs_vaddr = ((vaddr) - rs_offset); \
        rs_vsize = ((vsize) + rs_offset); \
        rs_vsize = (((rs_vsize+31)>>5)<<5); \
    }

#define MAX_LOOP 0xFFFF; //cycles for waiting act in rs_send_job() function

#define OSR_24                                            24
#define OSR_160                                           160

#define MAX_NUM_SAMPLES_PER_RS_FRAME                    4096
#define MAX_RATIO_OUTPUT_TO_INPUT_FREQ                  24


#define NUM_CORE_CLK_US_PER_SAMPLE_QUAD_MODE            123
#define NUM_CORE_CLK_US_PER_SAMPLE_LINEAR_MODE          72
#define NUM_CORE_CLK_US_PER_SAMPLE_INTERPOL_OFF         40

#define NUM_CORE_CLK_DS_PER_SAMPLE(div_factor)  ((34 * div_factor) + 9)      //[(DS_FILTER_LENGTH * DS_FACTOR / OSR)/2 + 9]

typedef enum 
{
   RS_JOB_STATE_INVALID = -1,
   RS_JOB_STATE_UNINITED = 0,
   RS_JOB_STATE_INITED,
   RS_JOB_STATE_RUNNING,
   RS_JOB_STATE_IDLE,
} rs_job_state;


typedef struct
{
   uint32 job_cnt;
   uint32 client_id;
   uint64 bw;
   uint32 bw_percent;
   uint32 mips;
   uint32 core_clk;
   rs_hw_power_mode power_mode;

   rs_hw_power_mode power_mode_high;
   rs_hw_power_mode power_mode_low;
} struct_mmpm_info;

typedef struct 
{
   /** frame size in microsecond unit. the frame szie should be matched with the num input sample sent via rs_send_job() */
   /** for example, for 48000 Hz to 44100 Hz resampling case, if frame_size = 10000 ns, then the num_input_sample for every frame is 48. */
   int32 frame_size;
   /** the time in microsecond unit client requires hw resampler done the job for this frame */
   int32 budget_time_to_done;
   /** the time in microsecond unit that hw resampler can finish the job. it is returned back from hw rs driver*/
   int32 estimated_time_to_done;
   int32 num_core_clk_process_per_frame;
   int32 num_dws_filt_mem_rw_per_frame;
   int32 num_dws_sample_mem_rw_per_frame;
   int32 flt_mem_size;
} struct_job_timing_info;

typedef struct
{
   boolean is_initialized;
   struct_mmpm_info mmpm_info;        //used for finally ADSPPM vote
   HwdResamplerPropertyType rs_prop;
   qurt_elite_mutex_t lock;
   qurt_elite_mutex_t lock_hw_access;
   qurt_elite_mutex_t lock_mmpm_info;
   qurt_mem_region_t   rsQmemRegion;   /* resampler register mapping region */

   //below are common for all jobs
   uint32 mem_load_burst_size;
   uint32 num_parallel_pipeline;

   void* hal_func_def_ptr;
} struct_driver_ctx;

typedef struct
{
  uint32 phys_addr;
  uint32 virt_ptr;
  uint32 size;
} struct_rs_internal_buf;


//driver internal data structure, store all info/status about that job
typedef struct
{
   struct_rs_config config_struct;  //job config structure, check HAL_rs.h for detail, TBD -- change this to pointer and allocate memory dynamically
   struct_rs_stream stream_struct;  //job stream structure, check HAL_rs.h for detail, TBD -- change this to pointer and allocate memory dynamically
   uint32* ptr_stream_mem;          // pointer to external memnory that used for stream structure
   uint32* ptr_stream_mem_phys;          // pointer to external memnory that used for stream structure
   rs_job_status_type job_status;              //indicate the job status, 0:unfinished, 1:done, 2:error
   boolean internal_buf_needed;             // for internal input/output buffer 0: don't need, 1: need
   boolean internal_coeff_buf_needed;             // for internal input/output buffer 0: don't need, 1: need
   uint16 input_num_sample_max;                // the max num of input samples per channel, valid if internal_buf_needed = 1, 4096 is maxium
   struct_job_timing_info timing_info;  //for this job
   rs_job_state job_state;
}struct_driver_config;

// runinng job registration struct array
typedef struct
{
   struct_driver_config       *driver_config;
   rs_job_event_callback_func cb_func;
   void                       *user_ctx_ptr;
   boolean occupied; 
}struct_job_map;

typedef struct
{
   uint16 up_sampler_flt_fptr;      //location of the end of the UpSampler filter = ptrDnSampStruct->filtMemOffset
   uint16 up_sampler_flt_optr;      //location of next sample generated by UPsampler
   uint16 dn_sampler_flt_fptr;      //location of the end of the DnSampler Filter
   uint16 resampler_flt_iptr;       //location of next input sample read from memory  = ptrDnSampStruct->inputOffset
}struct_internal_buf_pointer;

// function declairation
int16  rs_gen_job_id(void);
void    rs_dismiss_job_id(int16 job_id);
uint32 alloc_phy_mem(uint32 size, struct_rs_internal_buf* rs_buf_ptr);
void free_phy_mem(void* ptr);
void copy_to_phy_mem(uint32* in_buf_addr, uint32* phy_buffer);

void rs_add_config_to_job_id(int16 new_id, struct_driver_config *ptr_driver_config, rs_job_event_callback_func cb_func, void *user_ctx_ptr);

uint16 get_cur_channel(uint16 px);
uint16 get_cur_job_id(uint16 px);
uint16 get_cur_op_status(uint16 px);

void rs_dismiss_job_id(int16 job_id);

struct_driver_config *rs_get_driver_config(int16 job_id);

uint32 create_filt_mem(struct_rs_job_config* job_struct_ptr,
                       struct_rs_internal_buf* rs_buf_ptr);

void reset_filt_mem(struct_rs_job_config* job_struct_ptr,
                       uint32 filt_virt_ptr);

void set_internal_buf_initial_ptr(struct_rs_job_config* job_struct_ptr,
                                  struct_internal_buf_pointer *int_buf_set_ptr);


uint32 get_filter_mem_full_size_per_channel(rs_operation_type operation, rs_dynamic_resampler_type dynamic_rs, uint16 div_factor, uint16 div_step);

//filter_mem_state_size is the whole filter length including HW required working buffers
uint32 get_dnsampler_filter_mem_state_size(uint32 dnsamp_taps, uint16 div_factor);

//filter_mem_length = filter taps
uint32 get_dnsampler_filter_taps(uint32 dnsamp_filt_length, uint32 dnsamp_step);

uint32 get_upsampler_filter_taps(uint32 upsamp_filt_length, uint32 osr);

void free_alloc_memory(struct_driver_config *ptr_driver_config);

void debug_msg_print(struct_driver_config *ptr_driver_config);

int16 calculate_output_input_freq_ratio(struct_rs_job_config* job_struct);

ADSPResult drv_misc_init(void);

ADSPResult drv_misc_deinit(void);

ADSPResult set_stream_struct(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct);

ADSPResult set_job_config(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct, uint16 job_id);

ADSPResult allocate_internal_input_output_buffers(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct);

ADSPResult estimate_hw_operation_clks(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info);

ADSPResult estimate_hw_operation_time(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info);

ADSPResult re_estimate_hw_operation_time_for_all(int16 job_id);

int32 estimate_hw_operation_time_for_all_in_this_pw_mode(rs_hw_power_mode hw_pw_mode, boolean check_running_state);

ADSPResult set_mmpm_info(struct_driver_ctx* ptr_driver_config);

#endif

#ifdef __cplusplus
}
#endif //__cplusplus


