/*
============================================================================

FILE:          rs_driver_ext.c

DESCRIPTION:   Resampler HW driver extension file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/rs_driver_ext.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

05/24/12   FZ      Created file

========================================================================== */

#include "rs_driver.h"
#include "rs_driver_ext.h"
#include  "rs_driver_ext_internal.h"

#include "qurt_elite.h"
#include "qurt.h"
#include  "resampler_32b_int.h"
#include  "resampler_32b_ext.h"

#define USE_SIMPLE_CALCULATION_WAY
//#undef USE_SIMPLE_CALCULATION_WAY

static void rs_drv_callback_func(void *user_ctx_ptr, rs_job_event_type event, void *cb_data_ptr);

#define RS_DRV_ALIGN_32_BYTE(vaddr,vsize,rs_vaddr,rs_vsize) \
    { \
        uint32 rs_offset = ((vaddr) & 0x1f); \
        rs_vaddr = ((vaddr) - rs_offset); \
        rs_vsize = ((vsize) + rs_offset); \
        rs_vsize = (((rs_vsize+31)>>5)<<5); \
    }

/**
* This resampler driver init function and it is only called once.
*
* @return Success/failure of resampler init.
*/
ADSPResult rs_drv_init(void)
{
   return rs_init();
}

/**
* This resampler driver deinit function and it is only called once.
*
* @return Success/failure of resampler deinit.
*/
ADSPResult rs_drv_deinit(void)
{
   return rs_deinit();
}

/**
* This function is for resampler driver extension job init.
*
* @param[in/out] rs_drv_h - this will be the service entry handle returned to the caller.
* @param[in] job_cfg_ptr - point to job configure struct.
* @return Success/failure of open this job.
*/
ADSPResult rs_drv_job_open(rs_drv_handle_t *rs_drv_h, rs_drv_job_cfg_t* job_cfg_ptr)
{
   ADSPResult ret = ADSP_EFAILED;

   if(job_cfg_ptr)
   {
      rs_drv_job_info_t* job_info_ptr;
      struct_rs_job_config* job_config_struct_ptr;
      struct_rs_dnsampler* dn_samp_hw_ptr;
      rs_drv_job_cfg_t* in_job_cfg_ptr;

      typeUpSampStruct* up_samp_ptr;
      typeDnSampStruct* dn_samp_ptr;

      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_open, in_freq: %ld, out_freq: %ld, num_channels: %ld, samp_bit_width: %ld\n", job_cfg_ptr->in_freq, job_cfg_ptr->out_freq, job_cfg_ptr->num_channels, job_cfg_ptr->samp_bit_width);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_open, upsamp_filt_id: %d, interp_mode: %d, force_interpol: %d\n", job_cfg_ptr->upsamp_filt_id, job_cfg_ptr->interp_mode, job_cfg_ptr->force_interpol);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_open, dynamic_resampler: %d, internal_buf_needed: %d, input_num_sample_max: %d\n", job_cfg_ptr->dynamic_resampler, job_cfg_ptr->internal_buf_needed, job_cfg_ptr->input_num_sample_max);

      if(NULL==(job_config_struct_ptr = (struct_rs_job_config *)qurt_elite_memory_malloc(sizeof(struct_rs_job_config), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate hw resampler job parameter object");
      }

      if(NULL==(dn_samp_hw_ptr = (struct_rs_dnsampler *)qurt_elite_memory_malloc(sizeof(struct_rs_dnsampler), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate resampler driver downsampler parameter object");
      }

      if(NULL==(job_info_ptr = (rs_drv_job_info_t *)qurt_elite_memory_malloc(sizeof(rs_drv_job_info_t), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate resampler driver job info object");
      }

      if(NULL==(in_job_cfg_ptr = (rs_drv_job_cfg_t*)qurt_elite_memory_malloc(sizeof(rs_drv_job_cfg_t), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate HW Resampler driver extension job open parameter object");
      }

      if(NULL ==(up_samp_ptr = (typeUpSampStruct *)qurt_elite_memory_malloc(sizeof(typeUpSampStruct), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate up sampler object ");
      }
      if(NULL ==(dn_samp_ptr = (typeDnSampStruct *)qurt_elite_memory_malloc(sizeof(typeDnSampStruct), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate down sampler object ");
      }

      if( (job_config_struct_ptr) && (dn_samp_hw_ptr) && (job_info_ptr) && (in_job_cfg_ptr) && (up_samp_ptr) && (dn_samp_ptr))
      {
         memset((void*) job_config_struct_ptr, 0, sizeof(struct_rs_job_config));
         memset((void*) dn_samp_hw_ptr, 0, sizeof(struct_rs_dnsampler));
         memset((void*) job_info_ptr, 0, sizeof(rs_drv_job_info_t));
         memset((void*) in_job_cfg_ptr, 0, sizeof(rs_drv_job_cfg_t));

         memset((void*) up_samp_ptr, 0, sizeof(typeUpSampStruct));
         memset((void*) dn_samp_ptr, 0, sizeof(typeDnSampStruct));

         memscpy((void*)in_job_cfg_ptr, sizeof(rs_drv_job_cfg_t), (void*)job_cfg_ptr, sizeof(rs_drv_job_cfg_t));

         job_info_ptr->in_job_cfg_ptr = in_job_cfg_ptr;

         if(ADSP_EOK == (ret = rs_drv_config_job(in_job_cfg_ptr, job_config_struct_ptr, dn_samp_hw_ptr, up_samp_ptr, dn_samp_ptr)))
         {
             //initialize
            job_info_ptr->job_id = rs_init_job(0, job_config_struct_ptr, &rs_drv_callback_func, (void*)job_info_ptr);

            if((0 > job_info_ptr->job_id ) || (MAX_NUM_OF_HW_RESAMPLER_JOB <= job_info_ptr->job_id))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "rs_drv_job_open - failed job id = %d!!\n", job_info_ptr->job_id);
               ret = ADSP_EFAILED;
            }
            else
            {
               qurt_elite_channel_init(&job_info_ptr->channel_job);
               qurt_elite_signal_init(&job_info_ptr->sig_job);
               qurt_elite_channel_add_signal(&job_info_ptr->channel_job, &job_info_ptr->sig_job, 0);
               job_info_ptr->sig_mask = qurt_elite_signal_get_channel_bit(&job_info_ptr->sig_job);

               job_info_ptr->prev_out_freq = dn_samp_hw_ptr->up_samp_struct.out_freq;   //init prev_out_freq which is for FORCE_FRACTION mode only

               *rs_drv_h = (rs_drv_handle_t)job_info_ptr;

               job_info_ptr->op_mode = job_config_struct_ptr->operation;
               job_info_ptr->osr = (RS_UPSAMPLER_FILTER_ID_1344 == job_config_struct_ptr->upsamp_filt_id) ? 24 : 160;

               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_open DONE: rs_drv_h:0x%x\n" , job_info_ptr);
            }
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_job_open rs_drv_config_job is failed\n");
            //error msg
         }
      }
      else
      {
         //error msg
         *rs_drv_h = NULL;
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_job_open NULL pointer failed\n");
      }

      //release allocated resource
      if(job_config_struct_ptr) qurt_elite_memory_free((void*)job_config_struct_ptr);

      if(dn_samp_hw_ptr) qurt_elite_memory_free((void*)dn_samp_hw_ptr);

      if(up_samp_ptr) qurt_elite_memory_free((void*)up_samp_ptr);

      if(RS_DRV_DYNAMIC_RESAMPLER_ON == job_cfg_ptr->dynamic_resampler)
      {
         if(job_info_ptr)
         {
            job_info_ptr->dynamic_rs_dn_samp_ptr = (void*) dn_samp_ptr;
         }
         else
         {
            if(dn_samp_ptr) qurt_elite_memory_free((void*)dn_samp_ptr);
         }
      }
      else
      {
         if(dn_samp_ptr) qurt_elite_memory_free((void*)dn_samp_ptr);
      }

      //if the job open is failed, release the client's resource
      if((NULL == (*rs_drv_h)) || (ADSP_EFAILED == ret))
      {
         if(job_info_ptr) qurt_elite_memory_free((void*)job_info_ptr);

         if(in_job_cfg_ptr) qurt_elite_memory_free((void*)in_job_cfg_ptr);

         *rs_drv_h = NULL;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_job_open NULL pointer failed\n");
   }

   return ret;
}

/**
* This function is for processing the input/output data
*
* @param[in] job_id - job ID.
* @param[in] in_buflist_ptr - pointer to rs_drv_buflist.
* @param[in] out_buflist_ptr - pointer to rs_drv_buflist.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_drv_job_process(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr)
{
   ADSPResult ret = ADSP_EFAILED;
   rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) rs_drv_h;
   uint32 rs_vaddr_32_byte;
   uint32 rs_vsize_32_byte;

   if(job_info_ptr && in_buflist_ptr && out_buflist_ptr)
   {
      rs_drv_job_cfg_t* in_job_cfg_ptr = job_info_ptr->in_job_cfg_ptr;

      if(in_job_cfg_ptr)
      {
         uint32 i;
         rs_job_status_type rtn;
         uint32 in_buf_addr[MAX_MULTI_RESAMP_CHANNELS]={0};
         uint32 out_buf_addr[MAX_MULTI_RESAMP_CHANNELS]={0};
         uint32 input_samples = (16 == in_job_cfg_ptr->samp_bit_width) ? in_buflist_ptr->buf_ptr[0].actual_data_len >> 1 : in_buflist_ptr->buf_ptr[0].actual_data_len >> 2;   //for 16/32 bitwidth only
         uint32 output_samples = (16 == in_job_cfg_ptr->samp_bit_width) ? out_buflist_ptr->buf_ptr[0].actual_data_len >> 1 : out_buflist_ptr->buf_ptr[0].actual_data_len >> 2;   //for 16/32 bitwidth onl

         uint32 processed_samples = 0;

         for(i =0; i < in_job_cfg_ptr->num_channels; i++)
         {
            in_buf_addr[i] = (uint32)in_buflist_ptr->buf_ptr[i].data_ptr;
            out_buf_addr[i]= (uint32)out_buflist_ptr->buf_ptr[i].data_ptr;

            if(0 == in_job_cfg_ptr->internal_buf_needed)
            {
               //flush the input buffer for each channel before sending to hw
               RS_DRV_ALIGN_32_BYTE((uint32)in_buf_addr[i], in_buflist_ptr->buf_ptr[i].max_data_len, rs_vaddr_32_byte, rs_vsize_32_byte)
               qurt_elite_memorymap_cache_flush( rs_vaddr_32_byte, rs_vsize_32_byte);

              //flush the output buffer to clean any cache line which may be dirty by client
              //
              //1. If a line is dirty in the cache, the cache may freely evict or clean that line to backing store at any time without explicit intervention.
              //2. A cache line invalidate will only stop any write back of a dirty cache line after the completion of the invalidate.  Prior to the invalidate, the cache line could have been written back.
              //3. Invalidates on a clean line, or a line not present on the cache, should have no effect on memory.
              //4. In general, it is safer to use “clean and invalidate”

              RS_DRV_ALIGN_32_BYTE((uint32)out_buf_addr[i], out_buflist_ptr->buf_ptr[i].actual_data_len, rs_vaddr_32_byte, rs_vsize_32_byte)
              qurt_elite_memorymap_cache_flush( rs_vaddr_32_byte, rs_vsize_32_byte);
            }
         }

         if(ADSP_FAILED(ret = rs_send_job(0, job_info_ptr->job_id, (uint16)input_samples, (uint16)output_samples, (int32**)&in_buf_addr, (int32**)&out_buf_addr)))
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_job_process ERROR - job is rejected!!\n");
         }
         else
         {

            if(!job_info_ptr->async_call_cfg.job_event_cb_func)
            {
               //for sync call
               //wait on the signal
               qurt_elite_channel_wait(&job_info_ptr->channel_job, job_info_ptr->sig_mask);

               //clear the signal
               qurt_elite_signal_clear(&job_info_ptr->sig_job);

               //check if the job is done
               if(RS_JOB_STATUS_DONE == (rtn = rs_confirm_job(0, job_info_ptr->job_id, &processed_samples)))
               {
                  if(output_samples != processed_samples)
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_job_process ERROR output_samples %d  resampler generated output_samples %d \n", output_samples, processed_samples);
                  }

                  for(i =0; i < in_job_cfg_ptr->num_channels; i++)
                  {
                     in_buflist_ptr->buf_ptr[i].actual_data_len = (16 == in_job_cfg_ptr->samp_bit_width) ? input_samples << 1 : input_samples << 2;
                     out_buflist_ptr->buf_ptr[i].actual_data_len = (16 == in_job_cfg_ptr->samp_bit_width) ? processed_samples << 1 : processed_samples << 2;

                     //invalidate the output data buffer for each channel after getting from hw
                     if(0 == in_job_cfg_ptr->internal_buf_needed)
                     {
                        RS_DRV_ALIGN_32_BYTE((uint32)out_buflist_ptr->buf_ptr[i].data_ptr, out_buflist_ptr->buf_ptr[i].max_data_len, rs_vaddr_32_byte, rs_vsize_32_byte)
                        qurt_elite_memorymap_cache_invalidate(rs_vaddr_32_byte, rs_vsize_32_byte);
                     }
                  }

                  ret = ADSP_EOK;
               }
               else
               {
                  ret = ADSP_EFAILED;
               }
            }
            else
            {
               job_info_ptr->async_call_cfg.input_samples_to_hw = input_samples;
               job_info_ptr->async_call_cfg.output_samples_to_hw = output_samples;
            }
         }
      }
   }

   return ret;
}

/**
* This function is for getting the output data and how many input data consumed
* This API is just used for async call case
*
* @param[in] rs_drv_h - input handle.
* @param[in] in_buflist_ptr - pointer to rs_drv_buflist.
* @param[in] out_buflist_ptr - pointer to rs_drv_buflist.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_drv_job_process_done(rs_drv_handle_t rs_drv_h, rs_drv_buflist_t* in_buflist_ptr, rs_drv_buflist_t* out_buflist_ptr)
{
   ADSPResult ret = ADSP_EFAILED;
   rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) rs_drv_h;
   rs_job_status_type rtn;
   uint32 processed_samples = 0;
   uint32 i;
   uint32 rs_vaddr_32_byte;
   uint32 rs_vsize_32_byte;

   if(job_info_ptr && in_buflist_ptr && out_buflist_ptr)
   {
       rs_drv_job_cfg_t* in_job_cfg_ptr = job_info_ptr->in_job_cfg_ptr;
      //check if the job is done
      if(RS_JOB_STATUS_DONE == (rtn = rs_confirm_job(0, job_info_ptr->job_id, &processed_samples)))
      {
         if(job_info_ptr->async_call_cfg.output_samples_to_hw != processed_samples)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_job_process_done ERROR output_samples %d  resampler generated output_samples %d \n", job_info_ptr->async_call_cfg.output_samples_to_hw, processed_samples);
         }

         for(i =0; i < in_job_cfg_ptr->num_channels; i++)
         {
            in_buflist_ptr->buf_ptr[i].actual_data_len = (16 == in_job_cfg_ptr->samp_bit_width) ? job_info_ptr->async_call_cfg.input_samples_to_hw << 1 : job_info_ptr->async_call_cfg.input_samples_to_hw << 2;
            out_buflist_ptr->buf_ptr[i].actual_data_len = (16 == in_job_cfg_ptr->samp_bit_width) ? processed_samples << 1 : processed_samples << 2;

            //invalidate the output data buffer for each channel after getting from hw
            if(0 == in_job_cfg_ptr->internal_buf_needed)
            {
               RS_DRV_ALIGN_32_BYTE((uint32)out_buflist_ptr->buf_ptr[i].data_ptr, out_buflist_ptr->buf_ptr[i].max_data_len, rs_vaddr_32_byte, rs_vsize_32_byte)
               qurt_elite_memorymap_cache_invalidate(rs_vaddr_32_byte, rs_vsize_32_byte);
            }
         }

         ret = ADSP_EOK;
      }
      else
      {
         ret = ADSP_EFAILED;
      }
   }

   return ret;
}

/**
* This function is for processing the input/output data
*
* @param[in] rs_drv_h - input handle.
* @return Success/failure of sending this frame job.
* Note:
*
*/
ADSPResult rs_drv_job_reset(rs_drv_handle_t rs_drv_h)
{
   ADSPResult ret = ADSP_EFAILED;
   rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) rs_drv_h;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_reset: rs_drv_h:0x%x\n" , job_info_ptr);

   if(job_info_ptr)
   {
      struct_rs_job_config* job_config_struct_ptr;
      struct_rs_dnsampler* dn_samp_hw_ptr;

      typeUpSampStruct* up_samp_ptr;
      typeDnSampStruct* dn_samp_ptr;

      if(NULL==(job_config_struct_ptr = (struct_rs_job_config *)qurt_elite_memory_malloc(sizeof(struct_rs_job_config), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate hw resampler job parameter object");
      }
      
	  if(NULL==(dn_samp_hw_ptr = (struct_rs_dnsampler *)qurt_elite_memory_malloc(sizeof(struct_rs_dnsampler), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate resampler driver downsampler parameter object");
      }

      if(NULL ==(up_samp_ptr = (typeUpSampStruct *)qurt_elite_memory_malloc(sizeof(typeUpSampStruct), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate up sampler object ");
      }
      if(NULL ==(dn_samp_ptr = (typeDnSampStruct *)qurt_elite_memory_malloc(sizeof(typeDnSampStruct), QURT_ELITE_HEAP_DEFAULT)))
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate down sampler object ");
      }
	  if((job_config_struct_ptr) && (dn_samp_hw_ptr) && (up_samp_ptr) && (dn_samp_ptr))
      {
         memset((void*) job_config_struct_ptr, 0, sizeof(struct_rs_job_config));
         memset((void*) dn_samp_hw_ptr, 0, sizeof(struct_rs_dnsampler));

         memset((void*) up_samp_ptr, 0, sizeof(typeUpSampStruct));
         memset((void*) dn_samp_ptr, 0, sizeof(typeDnSampStruct));

         if(ADSP_EOK == (ret = rs_drv_config_job(job_info_ptr->in_job_cfg_ptr, job_config_struct_ptr, dn_samp_hw_ptr, up_samp_ptr, dn_samp_ptr)))
         {
            ret = rs_reset_job(0, job_info_ptr->job_id, job_config_struct_ptr);
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_job_reset rs_drv_config_job is failed\n");
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_drv_job_reset NULL pointer failed\n");
      }

      //release allocated resource
      if(job_config_struct_ptr) qurt_elite_memory_free((void*)job_config_struct_ptr);

      if(dn_samp_hw_ptr) qurt_elite_memory_free((void*)dn_samp_hw_ptr);

      if(up_samp_ptr) qurt_elite_memory_free((void*)up_samp_ptr);

      if(job_info_ptr->dynamic_rs_dn_samp_ptr)
      {
         //free previous one
         qurt_elite_memory_free((void*)job_info_ptr->dynamic_rs_dn_samp_ptr);
         //set to the new one
         job_info_ptr->dynamic_rs_dn_samp_ptr = (void*) dn_samp_ptr;
      }
      else
      {
         if(dn_samp_ptr) qurt_elite_memory_free((void*)dn_samp_ptr);
      }
   }

   return ret;
}


/**
* This function is for uninit this job
*
* @param[in] job_id - job ID.
* @return void
*/
ADSPResult rs_drv_job_close(rs_drv_handle_t rs_drv_h)
{
   ADSPResult ret = ADSP_EOK;
   rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) rs_drv_h;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_job_close: rs_drv_h:0x%x\n" , job_info_ptr);

   //release the resource allocated in rs_drv_init_job
   if(job_info_ptr)
   {
      qurt_elite_signal_deinit(&job_info_ptr->sig_job);
      qurt_elite_channel_destroy(&job_info_ptr->channel_job);

      rs_uninit_job(0, job_info_ptr->job_id);

      if(job_info_ptr->in_job_cfg_ptr) qurt_elite_memory_free((void *)job_info_ptr->in_job_cfg_ptr);

      if(job_info_ptr->dynamic_rs_dn_samp_ptr) qurt_elite_memory_free((void *)job_info_ptr->dynamic_rs_dn_samp_ptr);

      if(job_info_ptr) qurt_elite_memory_free((void *)job_info_ptr);
   }
   else
   {
      ret = ADSP_EFAILED;
   }

   return ret;
}


/**
* HW Resampler driver extension IOCtl function
*
* @param[in] hAFE input handle 
* @param[in] pPropInfo point to property info structure
* @return Success/failure IOCtl call.
*/
ADSPResult rs_drv_ioctl(rs_drv_handle_t rs_drv_h, rs_drv_ioctl_prop_info_t *prop_info_ptr)
{
   ADSPResult ret = ADSP_EFAILED;

   if(prop_info_ptr)
   {
      if(RS_DRV_PROPERTY_GLOBAL <= prop_info_ptr->prop)
      {
         ret = rs_drv_ioctl_global_prop(prop_info_ptr);
      }
      else
      {
         if(NULL != rs_drv_h)
         {
            rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) rs_drv_h;

            switch(prop_info_ptr->prop)
            {
               case RS_DRV_PROPERTY_FORCE_INTERPOL_CONFIG:
               {
                  //for case 8 Khz -> 6.1 KHz case
                  // 8 -> 24.4 -> 6.1
                  // 8 -> 24.4  -> force fraction mode
                  // 24.4 -> 6.1 -> normal down sampleing
                  if (sizeof(rs_drv_ioctl_prop_force_interpol_cfg_t) == prop_info_ptr->data_size &&
                          NULL != prop_info_ptr->data_ptr )
                     {
                        rs_drv_ioctl_prop_force_interpol_cfg_t *force_interpol_cfg = (rs_drv_ioctl_prop_force_interpol_cfg_t *)(prop_info_ptr->data_ptr);

                        rs_drv_sample_rate_drift_mode mode = force_interpol_cfg->mode;
                        int32  in_freq = force_interpol_cfg->in_freq;
                        int32  out_freq = force_interpol_cfg->out_freq;
                        struct_rs_stream_data rs_stream_data;

                        switch(mode)
                        {
                           case RS_DRV_SAMPLE_RATE_DRIFT_MODE_INPUT:
                           {
                              //need to update only two parameters: int_phase_step_size and frac_phase_step_size
                              if(ADSP_EOK == (ret = rs_drv_cal_stream_data(job_info_ptr, in_freq, out_freq, job_info_ptr->osr, &rs_stream_data, mode)))
                              {
                                 ret = rs_set_stream_data(job_info_ptr->job_id, &rs_stream_data, 2);
                              }
                              break;
                           }
                           case RS_DRV_SAMPLE_RATE_DRIFT_MODE_OUTPUT:
                           case RS_DRV_SAMPLE_RATE_DRIFT_MODE_BOTH:
                           {
      #if defined(USE_SIMPLE_CALCULATION_WAY)
                              //calculate drift back to input side. need to update only two parameters: int_phase_step_size and frac_phase_step_size
                              //adjust the input freq
                              int32 out_freq_no_drift = job_info_ptr->in_job_cfg_ptr->out_freq;
                              int32 in_freq_new = in_freq * out_freq_no_drift / out_freq;

                              //need to update only two parameters: int_phase_step_size and frac_phase_step_size
                              if(ADSP_EOK == (ret = rs_drv_cal_stream_data(job_info_ptr, in_freq_new, out_freq_no_drift, job_info_ptr->osr, &rs_stream_data, RS_DRV_SAMPLE_RATE_DRIFT_MODE_INPUT)))
                              {
                                 ret = rs_set_stream_data(job_info_ptr->job_id, &rs_stream_data, 2);
                              }
      #else
                              //need to update all six parameter
                              if(ADSP_EOK == (ret = rs_drv_cal_stream_data(job_info_ptr, in_freq, out_freq, job_info_ptr->osr, &rs_stream_data, mode)))
                              {
                                 ret = rs_set_stream_data(job_info_ptr->job_id, &rs_stream_data, 6);
                              }

      #endif //#if defined(USE_SIMPLE_CALCULATION_WAY)

                              break;
                           }

                           default:
                              MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_ioctl ERROR - RS_DRV_PROPERTY_FORCE_INTERPOL_CONFIG - mode is wrong!!\n");
                              break;
                        }
                     }
                  break;
               }

               case RS_DRV_PROPERTY_REGISTER_JOB_EVENT_CALLBACK:
               {
                   if (sizeof(rs_drv_ioctl_prop_job_event_notification_t) == prop_info_ptr->data_size &&
                          NULL != prop_info_ptr->data_ptr )
                   {
                        rs_drv_ioctl_prop_job_event_notification_t *job_event_notify_cfg = (rs_drv_ioctl_prop_job_event_notification_t *)(prop_info_ptr->data_ptr);

                        job_info_ptr->async_call_cfg.job_event_cb_func = job_event_notify_cfg->job_event_callback_func;
                        job_info_ptr->async_call_cfg.input_samples_to_hw = 0;
                        job_info_ptr->async_call_cfg.output_samples_to_hw = 0;
                        job_info_ptr->async_call_cfg.user_ctx_ptr = job_event_notify_cfg->user_ctx_ptr;

                        ret = ADSP_EOK;
                   }
                  break;
               }

               case RS_DRV_PROPERTY_GET_REQUIRED_NUM_SAMPLES:
               {
                   if (sizeof(rs_drv_ioctl_prop_get_num_sample_t) == prop_info_ptr->data_size &&
                          NULL != prop_info_ptr->data_ptr )
                   {
                      if(job_info_ptr->in_job_cfg_ptr)
                      {
                           rs_drv_ioctl_prop_get_num_sample_t *get_num_sample_ptr = (rs_drv_ioctl_prop_get_num_sample_t *)(prop_info_ptr->data_ptr);

                           if(RS_DRV_NUM_SAMPLE_PREDICT_MODE_FIXED_INPUT == get_num_sample_ptr->mode)
                           {
                              //1. fixed input mode to get num of output samples according to num_samp_in
                              if(RS_OPERATION_UPSAMPLING == job_info_ptr->op_mode)
                              {
                                 get_num_sample_ptr->num_sample_out = rs_drv_estimate_upsampler_output_samples(get_num_sample_ptr->num_sample_in, job_info_ptr->job_id, (int32) job_info_ptr->osr, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                              }
                              else
                              {
                                 get_num_sample_ptr->num_sample_out = rs_drv_estimate_dnsampler_output_samples(get_num_sample_ptr->num_sample_in, job_info_ptr->job_id, (int32)job_info_ptr->osr, (int32) job_info_ptr->op_mode, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                              }
                           }
                           else
                           {
                              //3. fixed output mode - calculate the num of input according to num_samp_out
                              if(RS_OPERATION_UPSAMPLING == job_info_ptr->op_mode)
                              {
                                 get_num_sample_ptr->num_sample_in = rs_drv_estimate_upsampler_input_samples(get_num_sample_ptr->num_sample_out, job_info_ptr->job_id, (int32) job_info_ptr->osr, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                              }
                              else
                              {
                                 get_num_sample_ptr->num_sample_in = rs_drv_estimate_dnsampler_input_samples(get_num_sample_ptr->num_sample_out, job_info_ptr->job_id, (int32) job_info_ptr->osr, (int32) job_info_ptr->op_mode, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                              }
                           }

                           ret = ADSP_EOK;
                      }
                   }
                  break;
               }

               case RS_DRV_PROPERTY_ESTIMATE_CORE_CLK:
               {
                   if (sizeof(rs_drv_ioctl_prop_estimate_core_clk_t) == prop_info_ptr->data_size &&
                          NULL != prop_info_ptr->data_ptr )
                   {
                      if(job_info_ptr->in_job_cfg_ptr)
                      {
                           rs_drv_ioctl_prop_estimate_core_clk_t *estimate_core_clk_ptr = (rs_drv_ioctl_prop_estimate_core_clk_t *)(prop_info_ptr->data_ptr);;
                           int32 num_input_sample_per_frame;
                           int32 num_output_sample_per_frame;
                           struct_rs_job_timing_info job_timing_info;

                           int64 temp;

                           temp = (int64)estimate_core_clk_ptr->frame_size;
                           temp = temp * job_info_ptr->in_job_cfg_ptr->out_freq;
                           num_output_sample_per_frame = (temp + 1000000) / 1000000;

                           memset((void*) &job_timing_info, 0, sizeof(struct_rs_job_timing_info));

                           estimate_core_clk_ptr->estimated_time_to_done = 0;

                           //Use the fixed output mode to get num of input samples according to num_output_sample_per_frame
                           if(RS_OPERATION_UPSAMPLING == job_info_ptr->op_mode)
                           {
                              num_input_sample_per_frame = rs_drv_estimate_upsampler_input_samples(num_output_sample_per_frame, job_info_ptr->job_id, (int32) job_info_ptr->osr, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                           }
                           else
                           {
                              num_input_sample_per_frame = rs_drv_estimate_dnsampler_input_samples(num_output_sample_per_frame, job_info_ptr->job_id, (int32) job_info_ptr->osr, (int32) job_info_ptr->op_mode, job_info_ptr->in_job_cfg_ptr->dynamic_resampler);
                           }

                           job_timing_info.num_input_sample_per_frame = num_input_sample_per_frame;
                           job_timing_info.num_output_sample_per_frame = num_output_sample_per_frame;
                           job_timing_info.budget_time_to_done = estimate_core_clk_ptr->budget_time_to_done;
                           job_timing_info.frame_size = estimate_core_clk_ptr->frame_size;

                           if(ADSP_EOK == (ret =  rs_estimate_core_clks(job_info_ptr->job_id, &job_timing_info)))
                           {
                              estimate_core_clk_ptr->estimated_time_to_done = job_timing_info.estimated_time_to_done;
                           }
                      }
                   }
                  break;
               }

               case RS_DRV_PROPERTY_DYNAMIC_RESAMPLER_CONFIG:
               {
                  if (sizeof(rs_drv_ioctl_prop_dynamic_resampler_cfg_t) == prop_info_ptr->data_size &&
                       NULL != prop_info_ptr->data_ptr )
                  {
                     rs_drv_ioctl_prop_dynamic_resampler_cfg_t *dynamic_rs_cfg_ptr = (rs_drv_ioctl_prop_dynamic_resampler_cfg_t *)(prop_info_ptr->data_ptr);

                     MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"rs_drv_ioctl-Dynamic Resampler Config in_freq: %ld, out_freq: %ld, rs_drv_h:0x%x\n", dynamic_rs_cfg_ptr->in_freq, dynamic_rs_cfg_ptr->out_freq, job_info_ptr);

                     ret = rs_drv_reinit_dynamic_resamp_wrapper(job_info_ptr, dynamic_rs_cfg_ptr->in_freq, dynamic_rs_cfg_ptr->out_freq);
                  }
                  break;
               }

               default:
                  break;
            }
         }
      }
   }

   return ret;
}

void rs_drv_callback_func(void *user_ctx_ptr, rs_job_event_type event, void *cb_data_ptr)
{
   rs_drv_job_info_t* job_info_ptr = (rs_drv_job_info_t*) user_ctx_ptr;
   if (ADSP_EOK == ((struct_rs_cb_data*)cb_data_ptr)->result)
   {
      if(!job_info_ptr->async_call_cfg.job_event_cb_func)
      {
         //sync call
         switch(event)
         {
            case RS_JOB_EVEN_JOB_DONE:
               qurt_elite_signal_send(&job_info_ptr->sig_job);
               break;
            case RS_JOB_EVEN_JOB_ERROR:
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_callback_func ERROR - job submit error!!\n");
               break;
            default:
               break;
         }
      }
      else
      {
         //async call
         job_info_ptr->async_call_cfg.job_event_cb_func((rs_drv_handle_t)job_info_ptr, (rs_drv_job_event_type)event, job_info_ptr->async_call_cfg.user_ctx_ptr);
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "@rs_drv_callback_func ERROR - job is rejected!!\n");
   }
}




