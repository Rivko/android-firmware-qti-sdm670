/*
============================================================================

FILE:          rs_driver_utils.c

DESCRIPTION:   Resampler HW driver file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/rs_driver_utils.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

09/08/11   FZ      Created file

========================================================================== */

#include "HAL_rs.h"
#include "HAL_rs_l2vic.h"
#include "rs_driver.h"
#include "rs_driver_internal.h"

#include "hwd_mmpm.h"
#include "rs_mmpm.h"

extern struct_driver_ctx rs_driver_global_ctx;
extern struct_job_map running_jobs[MAX_JOB_NUM]; 

static void rs_q6_intr_handler(void *ist_arg);

/**< resampler interrupt 0 IST info */
static qurt_elite_interrupt_ist_t resampler_ist_int_info;

#define RS_IST_STACK_SIZE_BYTES        1024

/* ============================================================================
**                  Global Object Definitions
** ========================================================================= */

/**
 * Get the channel number of current job, return value is the channel number
 */
uint16 get_cur_channel(uint16 px)
{
   return HAL_rs_get_cur_channel(px);
}

/**
 * Get the job ID of current job. Current job ID={Processor ID, Job ID}
 */
uint16 get_cur_job_id(uint16 px)
{
   return HAL_rs_get_cur_job_id(px);
}

/**
 * Get current operation status, 0:sleep, 1:active.
 */
uint16 get_cur_op_status(uint16 px)
{
   return HAL_rs_get_cur_op_status(px);
}

/**
 *  Generate the new job ID for current submitting job, range: 0-15
 *  NOT support multi thread yet.
 *
 *  @return new job ID
 *
 */
int16 rs_gen_job_id(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   int i;
   int16   job_id = 0xFF;

   qurt_elite_mutex_lock(&ptr_driver_ctx->lock);
   for(i=0; i < MAX_JOB_NUM ; i++)
   {
      if(!running_jobs[i].occupied)
      {
         running_jobs[i].occupied = 1;
         job_id = i;
         break;
      }
   }
   qurt_elite_mutex_unlock(&ptr_driver_ctx->lock);

   return job_id;
}

/**
 *  Dismiss the input job ID, range: 0-15
 *
 *  @return new job ID
 *
 */
void rs_dismiss_job_id(int16 job_id)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   qurt_elite_mutex_lock(&ptr_driver_ctx->lock);

   running_jobs[job_id].occupied=0;
   running_jobs[job_id].driver_config = NULL;
   running_jobs[job_id].cb_func = NULL;
   running_jobs[job_id].user_ctx_ptr = NULL;

   qurt_elite_mutex_unlock(&ptr_driver_ctx->lock);
}

/**
 * link driver config with the job ID in running job array
 */
void rs_add_config_to_job_id(int16 new_id, struct_driver_config *ptr_driver_config, rs_job_event_callback_func cb_func, void *user_ctx_ptr)
{
   running_jobs[new_id].driver_config = ptr_driver_config;
   running_jobs[new_id].cb_func = cb_func;
   running_jobs[new_id].user_ctx_ptr = user_ctx_ptr;
}

/**
 * get the driver config struct corresponding to the job ID
 */
struct_driver_config *rs_get_driver_config(int16 job_id)
{
   struct_driver_config * driver_config_ptr = NULL;

   if( (0 <= job_id) && (MAX_JOB_NUM > job_id))
   {
      driver_config_ptr = running_jobs[job_id].driver_config;
   }
   return driver_config_ptr;
}

/**
 * Allocate physically contiguous memory, 32 bytes align, lengh is mutiply of 32 bytes
 */
uint32 alloc_phy_mem(uint32 size, struct_rs_internal_buf* rs_buf_ptr)
{
   //call RTOS malloc API to allocate physical contiguous memory
   uint32 virt_addr;
   uint32 phys_addr;

   //size need to be multiply of 32
   if(size%32>0)
      size = ((size/32)+1)*32;

   virt_addr = (uint32) qurt_elite_memory_aligned_malloc(size,32,QURT_ELITE_HEAP_DEFAULT);

   if(virt_addr)
   {
      phys_addr = ( ( uint32_t ) qurt_lookup_physaddr( ( unsigned int ) virt_addr ) );
      rs_buf_ptr->virt_ptr = virt_addr;
      rs_buf_ptr->phys_addr = phys_addr;
      rs_buf_ptr->size = size;

      memset((void *)virt_addr, 0, size);
   }
   else
   {
      rs_buf_ptr->virt_ptr = 0;
      rs_buf_ptr->phys_addr = 0;
      rs_buf_ptr->size = 0;
   }

   return virt_addr;    
}

/**
 * Free physically contiguous memory
 */
void free_phy_mem(void* ptr)
{
   //call RTOS malloc API to free physical contiguous memory
   qurt_elite_memory_aligned_free((void *)ptr);
}

uint32 create_filt_mem(struct_rs_job_config* job_struct_ptr,
                       struct_rs_internal_buf* rs_buf_ptr)
{
   //allocate filter memory
   uint16 channels = job_struct_ptr->num_channels;  
   rs_operation_type operation = job_struct_ptr->operation; 
   rs_dynamic_resampler_type dynamic_mode = job_struct_ptr->dynamic_rs;
   uint16 div_factor = job_struct_ptr->ptr_dn_samp_struct->ds_factor;
   uint16 div_step = job_struct_ptr->ptr_dn_samp_struct->ds_step;

   uint32 size = get_filter_mem_full_size_per_channel(operation, dynamic_mode, div_factor, div_step);
  
   alloc_phy_mem( sizeof(uint32) * channels * size, rs_buf_ptr);

   if(rs_buf_ptr->virt_ptr)
   {
      memset((void *)rs_buf_ptr->virt_ptr, 0, sizeof(uint32) * channels * size);
      qurt_elite_memorymap_cache_flush((uint32)rs_buf_ptr->virt_ptr, sizeof(uint32) * channels * size);
   }

   return rs_buf_ptr->virt_ptr;
}

void reset_filt_mem(struct_rs_job_config* job_struct_ptr,
                       uint32 filt_virt_ptr)
{
   uint16 channels = job_struct_ptr->num_channels; 
   rs_operation_type operation = job_struct_ptr->operation; 
   rs_dynamic_resampler_type dynamic_mode = job_struct_ptr->dynamic_rs;
   uint16 div_factor = job_struct_ptr->ptr_dn_samp_struct->ds_factor;
   uint16 div_step = job_struct_ptr->ptr_dn_samp_struct->ds_step;

   uint32 size = get_filter_mem_full_size_per_channel(operation, dynamic_mode, div_factor, div_step);

   if(filt_virt_ptr)
   {
      uint32 rs_vaddr_32_byte;
      uint32 rs_vsize_32_byte;
      memset((void *)filt_virt_ptr, 0, sizeof(uint32) * channels * size);
      RS_ALIGN_32_BYTE((uint32)filt_virt_ptr, sizeof(uint32) * channels * size, rs_vaddr_32_byte, rs_vsize_32_byte)
      qurt_elite_memorymap_cache_flush(rs_vaddr_32_byte, rs_vsize_32_byte);
   }
}

////For V2
//1. For up sampler only case, the filter memory legth is always 80 DWs per channels
//2. For down sampler only case, the filter memory length is 80 DWs + dnsampler filter length (dn sampler filter state size) per channels. The HW will copy
//   the filter memory states from/to HW internal filter memory from the dnsampler filter starting address (exclude 80 DWs)
//3. For resampler case (both), the filter memory size is the same as #2

////For V3
//1. For up sampler only case, the filter memory legth is always 128 DWs per channels
//2. For down sampler only case, the filter memory length is dnsampler filter length (dn sampler filter state size) per channels. 
//3. For resampler case (both), the filter memory size is 128 DWs + dnsampler filter length
uint32 get_filter_mem_full_size_per_channel(rs_operation_type operation, rs_dynamic_resampler_type dynamic_rs, uint16 div_factor, uint16 div_step)
{
   //allocate filter memory
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   uint32 size = 0; 
   uint32 dnsamp_taps = 0;
   //rs_operation_type operation = job_struct_ptr->operation; 
   //uint16 div_factor = job_struct_ptr->ptr_dn_samp_struct->ds_factor;
   //uint16 div_step = job_struct_ptr->ptr_dn_samp_struct->ds_step;

   switch(operation)
   {
      case RS_OPERATION_UPSAMPLING:
         size = HAL_rs_get_upsampler_flt_mem_length();
         break;
      case RS_OPERATION_DNSAMPLING:
         dnsamp_taps = get_dnsampler_filter_taps(HAL_rs_get_downsampler_flt_coeff_length_shq(), div_step);
         if(RESAMPLER_HW_VERSION_3 == ptr_driver_ctx->rs_prop.hw_revision)
         {
            //v3
            size = get_dnsampler_filter_mem_state_size(dnsamp_taps, div_factor);
         }
         else
         {
             //v2, both down sampler and resampler are the same.
            size = HAL_rs_get_upsampler_flt_mem_length() + get_dnsampler_filter_mem_state_size(dnsamp_taps, div_factor);
         }
         break;
      case RS_OPERATION_RESAMPLING:
         dnsamp_taps = get_dnsampler_filter_taps(HAL_rs_get_downsampler_flt_coeff_length_shq(), div_step);

         if(RS_DYNAMIC_RESAMPLER_OFF == dynamic_rs)
         {
            size = HAL_rs_get_upsampler_flt_mem_length() + get_dnsampler_filter_mem_state_size(dnsamp_taps, div_factor);
         }
         else
         {
            //For dynamic resampler, it is always the div_factor as 2 as below:
            //filter_len   div_factor     div_step   filter_tap  DS_filter_memory
            //1632         2              12         136         168  

            size = HAL_rs_get_upsampler_flt_mem_length() + get_dnsampler_filter_mem_state_size(136, 2);
         }
         break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"@HW Resampler get_filter_mem_full_size_per_channel operation mode is wrong: %d\n", operation); 
         break;
   }

   return size;

}

uint32 get_dnsampler_filter_mem_state_size(uint32 dnsamp_taps, uint16 div_factor)

{
   uint32 size = 0;

   size = dnsamp_taps + HAL_rs_get_downsampler_flt_output_buf_size(div_factor) * div_factor;

   if((size % 8) > 0)
   {
      size = (size/8 + 1) * 8;
   }

   if(0 != size % 8)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@get_dnsampler_filter_mem_state_size: ERROR size is not multiple of 8: %lu\n", size);
   }

   return size;
}

uint32 get_dnsampler_filter_taps(uint32 dnsamp_filt_length, uint32 dnsamp_step)
{
   return dnsamp_filt_length / dnsamp_step;
}

uint32 get_upsampler_filter_taps(uint32 upsamp_filt_length, uint32 osr)
{
   return upsamp_filt_length / osr;
}

void set_internal_buf_initial_ptr(struct_rs_job_config* job_struct_ptr,
                                  struct_internal_buf_pointer *int_buf_set_ptr)
{
   uint32 length;
   uint32 upsamp_filt_length, osr;
   rs_operation_type operation = job_struct_ptr->operation; 

   if(RS_UPSAMPLER_FILTER_ID_1344 == job_struct_ptr->upsamp_filt_id)
   {
      upsamp_filt_length = HAL_rs_get_upsampler_flt_coeff_length_hq();
      osr = OSR_24;
   }
   else
   {
      upsamp_filt_length = HAL_rs_get_upsampler_flt_coeff_length_shq();
      osr = OSR_160;
   }

   if(RS_OPERATION_UPSAMPLING == operation)
   {
      int_buf_set_ptr->dn_sampler_flt_fptr = 0;  //do not care
      int_buf_set_ptr->up_sampler_flt_optr = 0;

      int_buf_set_ptr->up_sampler_flt_fptr = get_upsampler_filter_taps(upsamp_filt_length, osr) - 1;
      int_buf_set_ptr->resampler_flt_iptr = get_upsampler_filter_taps(upsamp_filt_length, osr);
   }
   else if(RS_OPERATION_DNSAMPLING == operation)
   {
      length = get_dnsampler_filter_taps(HAL_rs_get_downsampler_flt_coeff_length_shq(), job_struct_ptr->ptr_dn_samp_struct->ds_step);

      int_buf_set_ptr->up_sampler_flt_fptr = 0;   //do not care
      int_buf_set_ptr->up_sampler_flt_optr = 0;   //do not care
      int_buf_set_ptr->dn_sampler_flt_fptr = length - 1;
      int_buf_set_ptr->resampler_flt_iptr = length;   //do not care
   }
   else
   {
      //operation = RS_OPERATION_RESAMPLING
      length = get_dnsampler_filter_taps(HAL_rs_get_downsampler_flt_coeff_length_shq(), job_struct_ptr->ptr_dn_samp_struct->ds_step);

      int_buf_set_ptr->up_sampler_flt_optr = length;   
      int_buf_set_ptr->dn_sampler_flt_fptr = length - 1;

      int_buf_set_ptr->up_sampler_flt_fptr = get_upsampler_filter_taps(upsamp_filt_length, osr) - 1;
      int_buf_set_ptr->resampler_flt_iptr = get_upsampler_filter_taps(upsamp_filt_length, osr);
   }
}

int16 calculate_output_input_freq_ratio(struct_rs_job_config* job_struct)
{
   int16 ration = MAX_RATIO_OUTPUT_TO_INPUT_FREQ;
   int32 in_freq, out_freq;

   if(RS_OPERATION_DNSAMPLING == job_struct->operation)
   {
      ration = 1;
   }
   else
   {
      if(0 == job_struct->ptr_dn_samp_struct->up_samp_struct.int_phase_step_size)
      {
         //for in_freq = out_freq case
         ration = 1;
      }
      else
      {
         uint32 osr = OSR_24;

         if(RS_UPSAMPLER_FILTER_ID_8960 == job_struct->upsamp_filt_id)
         {
            osr = OSR_160;
         }

         out_freq = job_struct->ptr_dn_samp_struct->up_samp_struct.out_freq;
         in_freq = (job_struct->ptr_dn_samp_struct->up_samp_struct.int_phase_step_size * out_freq + osr) / osr;

         ration = out_freq / in_freq + ((out_freq % in_freq) > 0 ? 1 : 0);
      }
   }

#ifdef DRIVER_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@calculate_output_input_freq_ratio: ration: %d\n", ration); 
#endif //#ifdef DRIVER_DEBUG

   return ration;
}

void free_alloc_memory(struct_driver_config *ptr_driver_config)
{
   if(ptr_driver_config)
   {
      if(ptr_driver_config->stream_struct.filt_mem_base_addr)
      {
         free_phy_mem((void *)ptr_driver_config->stream_struct.filt_mem_base_addr);
         ptr_driver_config->stream_struct.filt_mem_base_addr = 0;
      }

      if(ptr_driver_config->ptr_stream_mem)
      {
         //free stream struct memory
         free_phy_mem((void*)ptr_driver_config->ptr_stream_mem);
         ptr_driver_config->ptr_stream_mem = NULL;
      }

      if(ptr_driver_config->stream_struct.inp_buf_addr[0])
      {
         free_phy_mem((void *)ptr_driver_config->stream_struct.inp_buf_addr[0]);
         ptr_driver_config->stream_struct.inp_buf_addr[0] = 0;
      }

      if(ptr_driver_config->stream_struct.out_buf_addr[0])
      {
         free_phy_mem((void *)ptr_driver_config->stream_struct.out_buf_addr[0]);
         ptr_driver_config->stream_struct.out_buf_addr[0] = 0; 
      }

      if(ptr_driver_config)
      {
         //free driver config structure
         qurt_elite_memory_free((void *)ptr_driver_config);
         ptr_driver_config = NULL;
      }
   }
}

ADSPResult drv_misc_init(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   ADSPResult rc = QURT_EOK;

   if (ADSP_FAILED(rc = qurt_elite_interrupt_register( &resampler_ist_int_info,(uint16_t)ptr_driver_ctx->rs_prop.isrHookPinNum,
                          rs_q6_intr_handler,NULL,"resampler_ist0", RS_IST_STACK_SIZE_BYTES)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to register resampler interrupt 0 with status %d\n",rc);
   }
   else
   {
      //ADSPPM calls. Note: BW, CORE CLK, and MIPS are voted basing on user cases
      //1. register
      if(ADSP_EOK != (rc = rs_mmpm_register()))
      {
         rc = ADSP_EFAILED;
      }  
      //2. request for power - one time only
      //3. request register programming - one time only
      //4. request latency - one time only
      else if(ADSP_EOK != (rc = rs_mmpm_misc_static_vote()))
      {
         rc = ADSP_EFAILED;
      }
      else
      {
         //Vote for core clk before register access
         rs_mmpm_enable_core_clks(ptr_driver_ctx->mmpm_info.core_clk);
         //wake up the resampler HW
         HAL_rs_start(0, 0);    //not mask the interrupt
         //DeVote for core clk before register access
         rs_mmpm_disable_core_clks();
      }
   }

   return rc;
}

ADSPResult drv_misc_deinit(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   //deregister interrupt with callback function.
   if (ADSP_FAILED(qurt_elite_interrupt_unregister(&resampler_ist_int_info)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "rs_uninit_job failed to unregister resampler interrupt\n");
   }

   //Vote for core clk before register access
   rs_mmpm_enable_core_clks(ptr_driver_ctx->mmpm_info.core_clk);
   HAL_rs_start(0, 3);    //mask the interrupt
   //DeVote for core clk before register access
   rs_mmpm_disable_core_clks();

   //ADSPPM calls
   //1. derequest latency - one time only 
   //2. derequest register programming - one time only
   //3. derequest for power - one time only
   rs_mmpm_misc_static_devote();
   //4. deregister
   rs_mmpm_deregister();

   return ADSP_EOK;
}

ADSPResult set_stream_struct(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct)
{
   struct_internal_buf_pointer struc_int_buf_set;
   uint32 dnsamp_taps = get_dnsampler_filter_taps(HAL_rs_get_downsampler_flt_coeff_length_shq(), job_struct->ptr_dn_samp_struct->ds_step);

   set_internal_buf_initial_ptr(job_struct, &struc_int_buf_set);

   //Fill Stream Structure  
   ptr_driver_config->stream_struct.in_frame_size = 0;
   ptr_driver_config->stream_struct.out_frame_size = 0;
   ptr_driver_config->stream_struct.up_sampler_flt_fptr = struc_int_buf_set.up_sampler_flt_fptr;
   ptr_driver_config->stream_struct.up_sampler_flt_optr = struc_int_buf_set.up_sampler_flt_optr;
   ptr_driver_config->stream_struct.resampler_flt_iptr = struc_int_buf_set.resampler_flt_iptr;
   ptr_driver_config->stream_struct.dn_sampler_flt_fptr = struc_int_buf_set.dn_sampler_flt_fptr;  
   ptr_driver_config->stream_struct.cur_frac_phase = job_struct->ptr_dn_samp_struct->up_samp_struct.cur_frac_phase;
   ptr_driver_config->stream_struct.cur_int_phase = job_struct->ptr_dn_samp_struct->up_samp_struct.cur_int_phase;
   ptr_driver_config->stream_struct.shift_const = job_struct->ptr_dn_samp_struct->up_samp_struct.shift_const;    
   ptr_driver_config->stream_struct.frac_const = job_struct->ptr_dn_samp_struct->up_samp_struct.frac_const;
   ptr_driver_config->stream_struct.frac_phase_step_size = job_struct->ptr_dn_samp_struct->up_samp_struct.frac_phase_step_size;
   ptr_driver_config->stream_struct.int_phase_step_size = job_struct->ptr_dn_samp_struct->up_samp_struct.int_phase_step_size;
   ptr_driver_config->stream_struct.out_freq = job_struct->ptr_dn_samp_struct->up_samp_struct.out_freq;
   ptr_driver_config->stream_struct.dn_factor = job_struct->ptr_dn_samp_struct->ds_factor;
   ptr_driver_config->stream_struct.dn_step = job_struct->ptr_dn_samp_struct->ds_step;
   ptr_driver_config->stream_struct.dn_filt_mem_size = get_dnsampler_filter_mem_state_size(dnsamp_taps, job_struct->ptr_dn_samp_struct->ds_factor);
   ptr_driver_config->stream_struct.status_i_value = 0;                
   ptr_driver_config->stream_struct.status_ii_value = 0;
   ptr_driver_config->stream_struct.spare_value = 0;

   return ADSP_EOK;
}

ADSPResult set_job_config(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct, uint16 job_id)
{
   ptr_driver_config->config_struct.submit_job = job_struct->submit_job;
   ptr_driver_config->config_struct.num_channels = job_struct->num_channels; 
   ptr_driver_config->config_struct.job_id = job_id;
   ptr_driver_config->config_struct.operation = job_struct->operation;
   ptr_driver_config->config_struct.us_coeff_id = job_struct->upsamp_filt_id;
   ptr_driver_config->config_struct.ds_coeff_id = job_struct->dnsamp_filt_id;
   ptr_driver_config->config_struct.data_width = job_struct->data_width;
   ptr_driver_config->config_struct.first_frame = 1;
   ptr_driver_config->config_struct.us_interp_mode = job_struct->interp_mode;
   ptr_driver_config->config_struct.force_interpol = job_struct->force_interpol;
   ptr_driver_config->config_struct.dynamic_mode = job_struct->dynamic_rs;

   return ADSP_EOK;
}

ADSPResult allocate_internal_input_output_buffers(struct_driver_config *ptr_driver_config, struct_rs_job_config* job_struct)
{
   ADSPResult rc = ADSP_EOK;
   uint32 size_in, size_out;
   uint16 chan;
   uint32 in_buf_ptr, out_buf_ptr;
   uint32 in_buf_ptr_phys, out_buf_ptr_phys;
   struct_rs_internal_buf rs_buf;

   if(0 == job_struct->input_num_sample_max)
   {
      job_struct->input_num_sample_max = MAX_NUM_SAMPLES_PER_RS_FRAME;
   }

   ptr_driver_config->input_num_sample_max = job_struct->input_num_sample_max;

   size_in = ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * job_struct->num_channels * job_struct->input_num_sample_max;
   size_out = ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * job_struct->num_channels * job_struct->input_num_sample_max * calculate_output_input_freq_ratio(job_struct);

   alloc_phy_mem(size_in, &rs_buf);
   in_buf_ptr = rs_buf.virt_ptr;
   in_buf_ptr_phys = rs_buf.phys_addr;

   alloc_phy_mem(size_out, &rs_buf);
   out_buf_ptr = rs_buf.virt_ptr;
   out_buf_ptr_phys = rs_buf.phys_addr;

   if((0 == in_buf_ptr) || (0 == out_buf_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@allocate_internal_input_output_buffers alloc_phy_mem input/output sample buffers failed\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      for(chan=0; chan<job_struct->num_channels; chan++)
      {
         ptr_driver_config->stream_struct.inp_buf_addr[chan] = in_buf_ptr + ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * chan * job_struct->input_num_sample_max;  
         ptr_driver_config->stream_struct.inp_buf_addr_phys[chan] = in_buf_ptr_phys + ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * chan * job_struct->input_num_sample_max;  
         ptr_driver_config->stream_struct.out_buf_addr[chan] = out_buf_ptr + ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * chan * job_struct->input_num_sample_max * calculate_output_input_freq_ratio(job_struct);
         ptr_driver_config->stream_struct.out_buf_addr_phys[chan] = out_buf_ptr_phys + ((job_struct->data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4) * chan * job_struct->input_num_sample_max * calculate_output_input_freq_ratio(job_struct);
      }
   }

   return rc;
}

/**
 * Resampler ISR
 * check/update job done status and signal the job owner thread
 */
static void rs_q6_intr_handler(void *ist_arg)
{
   uint32 idx;
   uint16 job_array[MAX_JOB_NUM]={0};
   rs_job_event_type event = RS_JOB_EVEN_JOB_ERROR;
   struct_rs_cb_data cb_data_type;

   //read job status
   HAL_rs_get_job_done_status(0, job_array);

   for(idx = 0; idx < MAX_JOB_NUM; idx++)
   {
      if(job_array[idx]!=0)
      {
         switch(job_array[idx])
         {
            case RS_JOB_DONE:
               running_jobs[idx].driver_config->job_status = RS_JOB_STATUS_DONE; 
               event = RS_JOB_EVEN_JOB_DONE;
               cb_data_type.result = ADSP_EOK;
               break;
            case RS_JOB_AHB_ERROR:
               running_jobs[idx].driver_config->job_status = RS_JOB_STATUS_AHB_ERROR;
               event = RS_JOB_EVEN_JOB_ERROR;
               cb_data_type.result = ADSP_EFAILED;
               break;
            case RS_JOB_SAMPLE_COUNT_ERROR:
               running_jobs[idx].driver_config->job_status = RS_JOB_STATUS_SAMPLE_COUNT_ERROR;
               event = RS_JOB_EVEN_JOB_ERROR;
               cb_data_type.result = ADSP_EFAILED;
               break;
            default:
               break;
         }

          if(running_jobs[idx].cb_func)
          {
             cb_data_type.hw_error_type = running_jobs[idx].driver_config->job_status;
             cb_data_type.job_id = idx;
             running_jobs[idx].cb_func(running_jobs[idx].user_ctx_ptr, event, (void*)&cb_data_type);
          }

          HAL_rs_set_job_clear(0, idx, job_array[idx]);   
      }
   }
}

//
//This function estimates total core clk cycle needed for this job
//This function is called after rs_init_job() before rs_send_job()
//
//There are two filters for up-sampler: 1344/24 and 8960/160
//As the taps for both filters are the same (1344/24 = 8960/160)
//so the core clk cycle calculation is not related to which filter
//

ADSPResult estimate_hw_operation_clks(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info)
{
   ADSPResult   status = ADSP_EOK;

   if(MAX_JOB_NUM > job_id)
   {
      struct_driver_config *ptr_driver_config = rs_get_driver_config(job_id);

      if(ptr_driver_config && ptr_job_timing_info && (RS_JOB_STATE_INITED == ptr_driver_config->job_state))
      {
         uint16 operation = ptr_driver_config->config_struct.operation;
         rs_dynamic_resampler_type dynamic_mode = ptr_driver_config->config_struct.dynamic_mode;
         int32 num_output_sample_per_frame = ptr_job_timing_info->num_output_sample_per_frame;
         int32 num_input_sample_per_frame = ptr_job_timing_info->num_input_sample_per_frame;

         int32 byte_per_sample = (ptr_driver_config->config_struct.data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4;

         int32 flt_mem_size = get_filter_mem_full_size_per_channel(operation, dynamic_mode, ptr_driver_config->stream_struct.dn_factor, ptr_driver_config->stream_struct.dn_step);   //in DWs

         int32 num_med_sample_per_frame = 0;
         int32 num_core_clk_us_per_frame = 0;
         int32 num_core_clk_us_per_sample = 0;
         int32 num_core_clk_ds_per_frame = 0;
         
         switch(operation)
         {
            case RS_OPERATION_UPSAMPLING:
            {
               if((0 == ptr_driver_config->stream_struct.frac_phase_step_size) && (RS_FORCE_INTERPOL_OFF == ptr_driver_config->config_struct.force_interpol))
               {
                  num_core_clk_us_per_sample = NUM_CORE_CLK_US_PER_SAMPLE_INTERPOL_OFF;
               }
               else
               {
                  num_core_clk_us_per_sample = (RS_INTERP_MODE_LINEAR == ptr_driver_config->config_struct.us_interp_mode) ? NUM_CORE_CLK_US_PER_SAMPLE_LINEAR_MODE : NUM_CORE_CLK_US_PER_SAMPLE_QUAD_MODE;
               }
               num_med_sample_per_frame = num_output_sample_per_frame;
            }
            break;

            case RS_OPERATION_DNSAMPLING:
            {
               num_core_clk_ds_per_frame = num_output_sample_per_frame * NUM_CORE_CLK_DS_PER_SAMPLE(ptr_driver_config->stream_struct.dn_factor);

               num_med_sample_per_frame = 0;
            }
            break;

            case RS_OPERATION_RESAMPLING:
            default:
            {
               if((0 == ptr_driver_config->stream_struct.frac_phase_step_size) && (RS_FORCE_INTERPOL_OFF == ptr_driver_config->config_struct.force_interpol))
               {
                  num_core_clk_us_per_sample = NUM_CORE_CLK_US_PER_SAMPLE_INTERPOL_OFF;
               }
               else
               {
                  num_core_clk_us_per_sample = (RS_INTERP_MODE_LINEAR == ptr_driver_config->config_struct.us_interp_mode) ? NUM_CORE_CLK_US_PER_SAMPLE_LINEAR_MODE : NUM_CORE_CLK_US_PER_SAMPLE_QUAD_MODE;
               }

               num_core_clk_ds_per_frame = num_output_sample_per_frame * NUM_CORE_CLK_DS_PER_SAMPLE(ptr_driver_config->stream_struct.dn_factor);

               num_med_sample_per_frame = num_output_sample_per_frame * ptr_driver_config->stream_struct.dn_factor;
            }
            break;
         }

         //HW Resampler core process time
         num_core_clk_us_per_frame = num_med_sample_per_frame * num_core_clk_us_per_sample;
         ptr_driver_config->timing_info.num_core_clk_process_per_frame = num_core_clk_us_per_frame + num_core_clk_ds_per_frame;   

         //Filter memory Rd & Wr. 
         //1. The 2 below is for (Rd + Wr)
         //2. For V3 HW, there are two pipeline (each pipeline for one channel) so the filter memory read/write BW is doubled.
         //3. HW Resampler do burst read/write load. One AHBE clock cycle is one DW.
         //4. Memory latecny will be counted later
         ptr_driver_config->timing_info.num_dws_filt_mem_rw_per_frame = flt_mem_size * 2 * HAL_rs_get_parallel_pipeline_num();  

         //Sample Rd & Wr
         //1. For V3 HW, there are two pipeline (each pipeline for one channel) so the filter memory read/write BW is doubled.
         //2. HW Resampler do burst read/write in one AHBE clock cycle.
         //3. Memory latecny will be counted later
         ptr_driver_config->timing_info.num_dws_sample_mem_rw_per_frame = (byte_per_sample * (num_input_sample_per_frame + num_output_sample_per_frame) * HAL_rs_get_parallel_pipeline_num()) / sizeof(uint32); 

         ptr_driver_config->timing_info.flt_mem_size = flt_mem_size;

         ptr_driver_config->timing_info.frame_size = ptr_job_timing_info->frame_size;
         ptr_driver_config->timing_info.budget_time_to_done = ptr_job_timing_info->budget_time_to_done;

         status = ADSP_EOK;
      }
   }

   return status;
}

//
//This function estimates the max time hw need to performance this job
//For worse case, we need to count all current running jobs
//
ADSPResult estimate_hw_operation_time(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info)
{
   ADSPResult   status = ADSP_EOK;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *ptr_driver_config = rs_get_driver_config(job_id);

   if((ptr_driver_config) && (ptr_job_timing_info))
   {
      rs_hw_power_mode hw_pw_mode;

      //First check if hw_pw_mode_cur_running meets this new job timing requirement or not,
      //If hw_pw_mode_cur_running meets timing requirement, use this power mode. Otherwise move to next level until find the right one.
      for(hw_pw_mode = ptr_driver_ctx->mmpm_info.power_mode_low; hw_pw_mode <= ptr_driver_ctx->mmpm_info.power_mode_high; hw_pw_mode++)
      {
         //current job is not running yet
         if(RS_JOB_STATE_INITED == ptr_driver_config->job_state)
         {
            
            //go through all current running jobs
            //Take the worse case
            //estimated_time_to_done for this job = time_required_for_this_job + time_required_for_all_exsit_job
            ptr_job_timing_info->estimated_time_to_done = estimate_hw_operation_time_for_all_in_this_pw_mode(hw_pw_mode, FALSE);
         }

         if(ptr_job_timing_info->estimated_time_to_done <= ptr_job_timing_info->budget_time_to_done)
         {
            break;
         }
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HW Resampler job_id %hd, estimated standalone time to done in nicro sec %ld",job_id, ptr_driver_config->timing_info.estimated_time_to_done);

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"estimate_hw_operation_time frame_size:%ld, estimated_time_to_done:%ld, budget_time_to_done:%ld\n", ptr_job_timing_info->frame_size, ptr_job_timing_info->estimated_time_to_done, ptr_job_timing_info->budget_time_to_done);

      status = ADSP_EOK;
   }

   return status;
}

//
//This function estimates the core clk needed basing on all current opened jobs
//This function is called only once when first submit the job
//This function is also called when the job is closed
//
//In this function:
//1. Get the core clk basing on all running jobs' time budget for worst case
//2. Calculate ahb clk for worst case
//
ADSPResult re_estimate_hw_operation_time_for_all(int16 job_id)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   int16 job_idx;
   struct_driver_config *ptr_driver_config_this_job;

   int32 estimated_time_to_done_cur;
   int16 clk_est_go = 0;

   rs_hw_power_mode hw_pw_mode;

   //Go through all hw resampler core clk from low to high
   for(hw_pw_mode = ptr_driver_ctx->mmpm_info.power_mode_low; hw_pw_mode <= ptr_driver_ctx->mmpm_info.power_mode_high; hw_pw_mode++)
   {
      //1. get the worst total for this power mode
      estimated_time_to_done_cur = estimate_hw_operation_time_for_all_in_this_pw_mode(hw_pw_mode, TRUE);

      //2. check if this power mode meets timing requirement for all running jobs
      for(job_idx = 0; job_idx < MAX_JOB_NUM; job_idx++)
      {
         ptr_driver_config_this_job = rs_get_driver_config(job_idx);

         if( ptr_driver_config_this_job && (RS_JOB_STATE_RUNNING == ptr_driver_config_this_job->job_state)) 
         {
            if(estimated_time_to_done_cur > ptr_driver_config_this_job->timing_info.budget_time_to_done)
            {
               //not meet the budget time for this job
               //move to next clk level
               clk_est_go = 1;
               break;
            }
         }
      }

      if(clk_est_go)
      {
         //3. this power mode does not meet timing requirement, move to next power level
         clk_est_go = 0;
         continue;
      }
      else
      {
         //4. this power mode meets all running job's timing requirement, use this power mode to set hw clock level.
         break;
      }
   }

   if(hw_pw_mode > ptr_driver_ctx->mmpm_info.power_mode_high)
   {
      hw_pw_mode = ptr_driver_ctx->mmpm_info.power_mode_high;
   }

   ptr_driver_ctx->mmpm_info.power_mode = hw_pw_mode;

   set_mmpm_info(ptr_driver_ctx);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"re_estimate_hw_operation_time_for_all core_clk:%ld, bw:%llu\n", ptr_driver_ctx->mmpm_info.core_clk, ptr_driver_ctx->mmpm_info.bw);

   return ADSP_EOK;
}


//this function estimates the worst timing basing on input power mode
int32 estimate_hw_operation_time_for_all_in_this_pw_mode(rs_hw_power_mode hw_pw_mode, boolean check_running_state)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   int16 job_idx;
   struct_driver_config *ptr_driver_config_this_job;

   int32 estimated_time_to_done = 0;

   uint32 ahb_latency;
   uint32 core_clk_period;
   uint32 ahb_clk_period;

   int64 time_to_done_total_in_ns = 0;
   int64 time_to_done_total_temp_in_ns = 0;

   int64 core_process_time_this_job_in_ns = 0;
   int64 filt_mem_load_time_this_job_in_ns = 0;
   int64 sample_mem_load_time_this_job_in_ns = 0;

   //get power mode related info
   core_clk_period = HAL_rs_get_data_from_clk_tbl(hw_pw_mode, CORE_CLOCK_PERIOD);
   ahb_clk_period = HAL_rs_get_data_from_clk_tbl(hw_pw_mode, AHB_CLOCK_PERIOD);
   ahb_latency = HAL_rs_get_data_from_clk_tbl(hw_pw_mode, LATENCY);

   //go through all current running jobs
   //Take the worse case
   //estimated_time_to_done for this job = time_required_for_this_job + time_required_for_all_exsit_jobs
   for(job_idx = 0; job_idx < MAX_JOB_NUM; job_idx++)
   {
      ptr_driver_config_this_job = rs_get_driver_config(job_idx);

      //note: ptr_driver_config_this_job->timing_info.num_core_clk_process_per_frame has been calculated in estimate_hw_operation_clks() for each job
      if((ptr_driver_config_this_job) && (0 < ptr_driver_config_this_job->timing_info.num_core_clk_process_per_frame))
      {
         if( check_running_state && (RS_JOB_STATE_RUNNING != ptr_driver_config_this_job->job_state))
         {
            continue;
         }

         //calculate time for this job basing on current power mode

         //1. Core process time
         core_process_time_this_job_in_ns = (int64)(ptr_driver_config_this_job->timing_info.num_core_clk_process_per_frame * core_clk_period); //in 1000*ns unit

         core_process_time_this_job_in_ns = core_process_time_this_job_in_ns / 1000;   //convert to nicro sec

         //2. Filter memory load time
         filt_mem_load_time_this_job_in_ns = (int64)(ptr_driver_config_this_job->timing_info.num_dws_filt_mem_rw_per_frame * (ahb_latency + ptr_driver_ctx->mem_load_burst_size * ahb_clk_period));
         filt_mem_load_time_this_job_in_ns = filt_mem_load_time_this_job_in_ns / ptr_driver_ctx->mem_load_burst_size; 
         filt_mem_load_time_this_job_in_ns = filt_mem_load_time_this_job_in_ns / 1000;  //convert to nicro sec

         //3. Input and Output sample load time
         sample_mem_load_time_this_job_in_ns = (int64)(ptr_driver_config_this_job->timing_info.num_dws_sample_mem_rw_per_frame * (ahb_latency + ptr_driver_ctx->mem_load_burst_size * ahb_clk_period));
         sample_mem_load_time_this_job_in_ns = sample_mem_load_time_this_job_in_ns / ptr_driver_ctx->mem_load_burst_size; 
         sample_mem_load_time_this_job_in_ns = sample_mem_load_time_this_job_in_ns / 1000;  //convert to nicro sec

         //4. Total time for this job
         //4-1 Core process and sample load can be concurrently. Use what the larger one
         time_to_done_total_temp_in_ns = (core_process_time_this_job_in_ns > sample_mem_load_time_this_job_in_ns) ? core_process_time_this_job_in_ns : sample_mem_load_time_this_job_in_ns;
         //4-2 Add filter memory load time
         time_to_done_total_temp_in_ns = time_to_done_total_temp_in_ns + filt_mem_load_time_this_job_in_ns;
         //4-3 Consider num of channels for this job. Note if num_parallel_pipeline = 2, 2 channels are processing concurrently
         time_to_done_total_temp_in_ns = ptr_driver_config_this_job->config_struct.num_channels * time_to_done_total_temp_in_ns / ptr_driver_ctx->num_parallel_pipeline;

         ptr_driver_config_this_job->timing_info.estimated_time_to_done = (time_to_done_total_temp_in_ns + 1000) / 1000;   //convert to micro-sec

         time_to_done_total_in_ns += time_to_done_total_temp_in_ns;   
      }
   }

   estimated_time_to_done = (time_to_done_total_in_ns + 1000) / 1000;  //convert to micro-sec

   return estimated_time_to_done;
}

#define RS_BW_VOTE_FACTOR      2

ADSPResult set_mmpm_info(struct_driver_ctx* ptr_driver_config)
{
   if(ptr_driver_config)
   {
      rs_hw_power_mode hw_mode;
      uint32 ahb_latency;

      hw_mode = ptr_driver_config->mmpm_info.power_mode;

      ahb_latency = HAL_rs_get_data_from_clk_tbl(hw_mode, LATENCY);

      ptr_driver_config->mmpm_info.bw = (uint64)HAL_rs_get_data_from_clk_tbl(hw_mode, AHB_CLOCK) * 4;                  //global BW for all jobs, bw is byte per-sec. One AHB clock cycle can load one DW (4 bytes).
      ptr_driver_config->mmpm_info.bw_percent = (ptr_driver_config->mem_load_burst_size * 100 * RS_BW_VOTE_FACTOR) / (ptr_driver_config->mem_load_burst_size + ahb_latency/HAL_rs_get_data_from_clk_tbl(hw_mode, AHB_CLOCK_PERIOD));
      ptr_driver_config->mmpm_info.mips = 0;                
      ptr_driver_config->mmpm_info.core_clk = HAL_rs_get_data_from_clk_tbl(hw_mode, CORE_CLOCK);   //global core clock for all jobs

   }

   return ADSP_EOK;
}


void debug_msg_print(struct_driver_config *ptr_driver_config)
{
#ifdef DRIVER_DEBUG

   if(ptr_driver_config)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===============PARAMETER CONFIRM DUMP START=========================\n");
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"==config struct==\n");
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: stream_struct ptr: 0x%x\n", ptr_driver_config->ptr_stream_mem); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: submit job: %d\n", ptr_driver_config->config_struct.submit_job); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: number of channels: %d\n", ptr_driver_config->config_struct.num_channels); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: job ID: %d\n", ptr_driver_config->config_struct.job_id); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: operation mode: %d\n", ptr_driver_config->config_struct.operation); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: us_coeff_id: %d\n", ptr_driver_config->config_struct.us_coeff_id);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: ds_coeff_id: %d\n", ptr_driver_config->config_struct.ds_coeff_id);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: data_width: %d\n", ptr_driver_config->config_struct.data_width);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: first_frame:%d\n" ,ptr_driver_config->config_struct.first_frame);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: us_interp_mode: %d\n", ptr_driver_config->config_struct.us_interp_mode);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: force_interpol:%d\n" ,ptr_driver_config->config_struct.force_interpol);


      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"==stream struct==\n");
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: in samples num: %d\n", ptr_driver_config->stream_struct.in_frame_size); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: out samples num:%d\n" ,ptr_driver_config->stream_struct.out_frame_size);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: cur_int_pahse:%d\n" , ptr_driver_config->stream_struct.cur_int_phase);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: cur_frac_phase:%d \n", ptr_driver_config->stream_struct.cur_frac_phase);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: up_sampler_flt_fptr: %d\n", ptr_driver_config->stream_struct.up_sampler_flt_fptr); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: resampler_flt_iptr: %d\n", ptr_driver_config->stream_struct.resampler_flt_iptr); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: up_sampler_flt_optr: %d\n", ptr_driver_config->stream_struct.up_sampler_flt_optr); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: dn_sampler_flt_fptr: %d\n", ptr_driver_config->stream_struct.dn_sampler_flt_fptr); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: shift_const: %d\n", ptr_driver_config->stream_struct.shift_const); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: frac_const: %d\n", ptr_driver_config->stream_struct.frac_const); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: int_phase_step_size: %d\n", ptr_driver_config->stream_struct.int_phase_step_size); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: frac_phase_step_size: %d\n", ptr_driver_config->stream_struct.frac_phase_step_size); 
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: out_freq: %d\n" ,ptr_driver_config->stream_struct.out_freq);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: dn_factor: %d\n" ,ptr_driver_config->stream_struct.dn_factor);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: dn_step: %d\n" ,ptr_driver_config->stream_struct.dn_step);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: dn_filt_mem_size: %d\n" ,ptr_driver_config->stream_struct.dn_filt_mem_size);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: filt_mem_base_addr:0x%x\n" , ptr_driver_config->stream_struct.filt_mem_base_addr);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: inbuf_addr:0x%x\n" , ptr_driver_config->stream_struct.inp_buf_addr[0]);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: outbuf_addr:0x%x\n" , ptr_driver_config->stream_struct.out_buf_addr[0]);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: inbuf_addr:0x%x\n" , ptr_driver_config->stream_struct.inp_buf_addr[1]);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@debug_msg_print: outbuf_addr:0x%x\n" , ptr_driver_config->stream_struct.out_buf_addr[1]);
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===============PARAMETER CONFIRM DUMP END==========================\n");
      fflush(stdout); 
   }

#endif

}



