/*
============================================================================

FILE:          rs_driver.c

DESCRIPTION:   Resampler HW driver file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================

============================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/hw/resampler/src/rs_driver.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

09/08/11   FZ      Created file

========================================================================== */

#include "HAL_rs.h"
#include "rs_driver.h"
#include "rs_driver_internal.h"

#include "qurt_elite.h"
#include "qurt.h"

#include "hwd_mmpm.h"
#include "rs_mmpm.h"


#define LIMIT_TO_ONE_JOB_ONLY
#undef LIMIT_TO_ONE_JOB_ONLY

/* ============================================================================
**                  Global Object Definitions
** ========================================================================= */
struct_driver_ctx rs_driver_global_ctx = {FALSE, };

struct_job_map running_jobs[MAX_JOB_NUM];

/************************************************
 *    Resampler  DRIVER   APIs                  *
 ************************************************/

/**
 * driver initial function, for now, it only register interrupt callback
 * User need to call this fucntion before using the driver
 */
ADSPResult rs_init(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   ADSPResult status = ADSP_EOK;

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init START\n");
#endif

   //inc a counter
   if (!ptr_driver_ctx->is_initialized)
   {
      // read dev config info
      if(ADSP_EOK == (status = HwdDevCfg_ReadModuleConfigData(HWD_MODULE_TYPE_RESAMPLER, (void *) &(ptr_driver_ctx->rs_prop))))
      {
         if(0 != ptr_driver_ctx->rs_prop.hw_revision)
         {
            memset((void *)&running_jobs[0], 0, sizeof(running_jobs));

#if defined(LIMIT_TO_ONE_JOB_ONLY)
            int i;
            for(i=1; i < MAX_JOB_NUM ; i++)
            {
               //Only allow job index 0 here
               running_jobs[i].occupied = 1;
            }
#endif

            qurt_elite_mutex_init(&ptr_driver_ctx->lock);
            qurt_elite_mutex_init(&ptr_driver_ctx->lock_hw_access);
            qurt_elite_mutex_init(&ptr_driver_ctx->lock_mmpm_info);

            if(ADSP_EOK == status)
            {
               //address mapping
               uint32 reg_addr_virt = 0;
               if(ADSP_EOK != (status = HwdDevCfg_GetVirtAddress(&(ptr_driver_ctx->rsQmemRegion), ptr_driver_ctx->rs_prop.baseRegAddr, ptr_driver_ctx->rs_prop.regSize,
                  &reg_addr_virt)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW RS: Error obtaining virtual addr \n");
               }
               else
               {
                   //allocate hal function map table
                  if(NULL ==(ptr_driver_ctx->hal_func_def_ptr = (void *)qurt_elite_memory_malloc(sizeof(hal_rs_func_def_t), QURT_ELITE_HEAP_DEFAULT)))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate hal function map table");
                     status= ADSP_EFAILED;
                  }
                  else
                  {
                     memset((void*) ptr_driver_ctx->hal_func_def_ptr, 0, sizeof(hal_rs_func_def_t));

                     HAL_rs_init_common(reg_addr_virt, ptr_driver_ctx->rs_prop.hw_revision, ptr_driver_ctx->hal_func_def_ptr);

                     ptr_driver_ctx->mem_load_burst_size = HAL_rs_get_memory_load_burst_size();
                     ptr_driver_ctx->num_parallel_pipeline = HAL_rs_get_parallel_pipeline_num();

                     ptr_driver_ctx->mmpm_info.job_cnt = 0;             //this cnt is used to track how many jobs are opened. Inc 1 when rs_init_job() called and Dec 1 when rs_uninit_job() called
                    
                     ptr_driver_ctx->mmpm_info.power_mode = HAL_rs_get_power_mode_highest();

                     ptr_driver_ctx->mmpm_info.power_mode_high = HAL_rs_get_power_mode_highest();
                     ptr_driver_ctx->mmpm_info.power_mode_low = HAL_rs_get_power_mode_lowest();

                     set_mmpm_info(ptr_driver_ctx);

                     ptr_driver_ctx->is_initialized = TRUE;
                  }
               }
            }
            else
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW RS: Error read dev config data failed\n");
            }
         }
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init END\n");
#endif

   return status;
}

ADSPResult rs_deinit(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_deinit START\n");
#endif

   if (ptr_driver_ctx->is_initialized)
   {
      ptr_driver_ctx->mmpm_info.job_cnt = 0;
      qurt_elite_mutex_destroy(&ptr_driver_ctx->lock);
      qurt_elite_mutex_destroy(&ptr_driver_ctx->lock_hw_access);
      qurt_elite_mutex_destroy(&ptr_driver_ctx->lock_mmpm_info);

      if(ptr_driver_ctx->rsQmemRegion)
      {
         HwdDevCfg_DestoryVirtAddress(ptr_driver_ctx->rsQmemRegion);
         ptr_driver_ctx->rsQmemRegion = 0;
      }

      //de-allocate hal function map table
      if(ptr_driver_ctx->hal_func_def_ptr)
      {
         qurt_elite_memory_free((void*)ptr_driver_ctx->hal_func_def_ptr);
         ptr_driver_ctx->hal_func_def_ptr = NULL;
      }

      ptr_driver_ctx->is_initialized = FALSE;
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_deinit END\n");
#endif

   return ADSP_EOK;
}


/**
 * SW client initialize resampling job of the stream
 *
 * @param[in] processor ID
 * @param[in] job_struct structure contain all parameters about resampling job
 * @param[in] cb_func callback function ptr when job is done
 * @param[in] user_ctx user defined context
 * @return job ID if succesful, -1 if failure
 */
int16 rs_init_job(uint16 px, struct_rs_job_config* job_struct, rs_job_event_callback_func cb_func, void* user_ctx)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   int16 new_id = -1;
   struct_driver_config *ptr_driver_config;
   int32 exit_code = 0;
   struct_rs_internal_buf rs_buf;
   ADSPResult rc = ADSP_EOK;

//#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init_job START\n");
//#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
   }
   else
   {
      if(job_struct)
      {
         //generate new job ID
         new_id = rs_gen_job_id();

         if(MAX_JOB_NUM > new_id)
         {
            qurt_elite_mutex_lock(&ptr_driver_ctx->lock_mmpm_info);
            ptr_driver_ctx->mmpm_info.job_cnt++;
            if(1 == ptr_driver_ctx->mmpm_info.job_cnt)
            {
               //only once
               rc = drv_misc_init();
            }
            qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_mmpm_info);

            if(ADSP_EOK == rc)
            {
               do
               {
                  ptr_driver_config = (struct_driver_config *)qurt_elite_memory_malloc(sizeof(struct_driver_config), QURT_ELITE_HEAP_DEFAULT);
                  if(NULL == ptr_driver_config)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init_job qurt_malloc failed\n");
                     exit_code = 1;
                     break;
                  }

                  memset((void *)ptr_driver_config, 0, sizeof(struct_driver_config));

                  //acquire stream struct memory from HAL
                  alloc_phy_mem(HAL_get_stream_struct_size(job_struct->num_channels), &rs_buf);
                  ptr_driver_config->ptr_stream_mem = (uint32*)rs_buf.virt_ptr;
                  ptr_driver_config->ptr_stream_mem_phys = (uint32*) rs_buf.phys_addr;

                  //create filter memory
                  create_filt_mem(job_struct, &rs_buf);
                  ptr_driver_config->stream_struct.filt_mem_base_addr = rs_buf.virt_ptr;
                  ptr_driver_config->stream_struct.filt_mem_base_addr_phys = rs_buf.phys_addr;

                  if((NULL == ptr_driver_config->ptr_stream_mem) ||
                     (0 == ptr_driver_config->stream_struct.filt_mem_base_addr))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init_job alloc_phy_mem failed\n");
                     exit_code = 1;
                     break;
                  }

                  ptr_driver_config->internal_buf_needed = job_struct->internal_buf_needed;
                  //allocate internal buffer
                  if(ptr_driver_config->internal_buf_needed)
                  {
                     if(ADSP_EOK != (rc = allocate_internal_input_output_buffers(ptr_driver_config, job_struct)))
                     {
                        exit_code = 1;
                        break;
                     }
                  }

                  set_stream_struct(ptr_driver_config, job_struct);

                  set_job_config(ptr_driver_config, job_struct, new_id);

                  ptr_driver_config->job_state = RS_JOB_STATE_INITED;

                  //debug_msg_print(ptr_driver_config);

                  //add driver config to running job array
                  rs_add_config_to_job_id(new_id, ptr_driver_config, cb_func, user_ctx);

               } while (0);

               switch (exit_code)
               {
                  case 0:
                  default:
                     break;
                  case 1:
                  {
                     free_alloc_memory(ptr_driver_config);

                     qurt_elite_mutex_lock(&ptr_driver_ctx->lock_mmpm_info);
                     ptr_driver_ctx->mmpm_info.job_cnt--;

                     if(0 == ptr_driver_ctx->mmpm_info.job_cnt)
                     {
                        drv_misc_deinit();
                     }

                     qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_mmpm_info);

                     rs_dismiss_job_id(new_id);
                     new_id = -1;
                  }
                  break;
               }
            }
         }
         else
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init_job no hardware resource avaiable to do new job\n");
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_init_job input struct_rs_job_config struct pointer is NULL\n");
      }
   }

//#ifdef DRIVER_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@@rs_init_job END job_id: %d\n", new_id);
//#endif

   return new_id;
}

/**
 * SW client submit resampling job using assigned job ID from driver
 *
 * @param[in] px processor ID
 * @param[in] job_id the job_id of the stream, provide by driver when call rs_init_job()
 * @param[in] phy_cont: 1: buffer is physical contiguous, no need for internal buffer, 0:need internal buffer transfer
 * @param[in] in_buf_addr input buffer pointer
 * @param[in] out_buf_addr output buffer pointer
 * @return 1 if succesful, else 0
 */
ADSPResult rs_send_job(uint16 px, int16 job_id, uint16 in_samples, uint16 out_samples, int32** in_buf_ptr, int32** out_buf_ptr)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   ADSPResult rc = ADSP_EOK;
   struct_driver_config *driver_config;
   uint16 chan, tmp_id, samp_bit_width;
   boolean job_ack = 0;
   uint32 rs_vaddr_32_byte;
   uint32 rs_vsize_32_byte;
   int32 count;

   #ifdef DRIVER_DEBUG
     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job START, in_samples: %d, out_samples: %d\n",in_samples, out_samples);
   #endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      // retrive driver config structure based on job ID
      driver_config = rs_get_driver_config(job_id);

      if(driver_config)
      {
         driver_config->job_status = 0;

         // in/out buffer is not 32 bytes align, mutiply of 32, use internal buffer
         samp_bit_width = (driver_config->config_struct.data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4;

         if(driver_config->internal_buf_needed)
         {
            if(driver_config->input_num_sample_max >= in_samples)
            {
               for(chan=0; chan< driver_config->config_struct.num_channels; chan++)
               {
                 //copy user input buffer to internal buffer
                 memscpy((void *)driver_config->stream_struct.inp_buf_addr[chan],
                         driver_config->input_num_sample_max * samp_bit_width,    /* Max Dst buf size per channel */
                         (void *)*in_buf_ptr,
                         in_samples * samp_bit_width);


                 RS_ALIGN_32_BYTE((uint32)driver_config->stream_struct.inp_buf_addr[chan], in_samples * samp_bit_width, rs_vaddr_32_byte, rs_vsize_32_byte)
                 qurt_elite_memorymap_cache_flush( rs_vaddr_32_byte, rs_vsize_32_byte);

                 //Output buffer can be corrupted by cache evict during HW operation.
                 //To avoid this, a cache flush is needed before starting HW operation.
                 RS_ALIGN_32_BYTE((uint32)driver_config->stream_struct.out_buf_addr[chan], out_samples * samp_bit_width, rs_vaddr_32_byte, rs_vsize_32_byte)
                 qurt_elite_memorymap_cache_flush( rs_vaddr_32_byte, rs_vsize_32_byte);

                 //store user output buffer
                 driver_config->stream_struct.usr_out_buf_addr[chan]=  (uint32)*out_buf_ptr;

                 in_buf_ptr++;
                 out_buf_ptr++;
               }
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job the num of input samples %d is larger than input_num_sample_max %d\n",in_samples, driver_config->input_num_sample_max );
               rc = ADSP_EBADPARAM;
            }
         }
         else
         {  //use user input/output buffer directly
           for(chan=0; chan< driver_config->config_struct.num_channels; chan++)
           {
              driver_config->stream_struct.inp_buf_addr[chan] = (uint32)*in_buf_ptr;
              driver_config->stream_struct.inp_buf_addr_phys[chan] = qurt_lookup_physaddr( ( unsigned int ) (*in_buf_ptr)  );   //(uint32)*in_buf_ptr;

              driver_config->stream_struct.out_buf_addr[chan]=  (uint32)*out_buf_ptr;
              driver_config->stream_struct.out_buf_addr_phys[chan] = qurt_lookup_physaddr( ( unsigned int ) (*out_buf_ptr ) );   //(uint32)*out_buf_ptr;

              in_buf_ptr++;
              out_buf_ptr++;
           }
         }

         if(ADSP_EOK == rc)
         {
            driver_config->stream_struct.in_frame_size = in_samples;
            driver_config->stream_struct.out_frame_size = out_samples;

            debug_msg_print(driver_config);

            qurt_elite_mutex_lock(&ptr_driver_ctx->lock_hw_access);

            if(RS_JOB_STATE_INITED == driver_config->job_state)
            {
               driver_config->job_state = RS_JOB_STATE_RUNNING;

               //this job submits the first frame data for hw operation
               //re-estimate the core clk
               //it just do once only for each job
               re_estimate_hw_operation_time_for_all(job_id);
               rc = rs_mmpm_misc_dynamic_vote_clks(job_id);
            }

            //driver_config->job_state = RS_JOB_STATE_RUNNING;

            //vote for clks before accessing hw
            if(ADSP_EOK != rc)
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job rs_mmpm_misc_dynamic_vote is failed\n");
            }
            else
            {
               // write stream struct register and stream config register
               HAL_rs_set_stream_struct(px, &driver_config->stream_struct, driver_config->ptr_stream_mem, driver_config->ptr_stream_mem_phys, driver_config->config_struct.num_channels);

               HAL_rs_set_stream_config(px, &driver_config->config_struct);

               //update first frame parameter
               if(1 == driver_config->config_struct.first_frame)
               {
                  driver_config->config_struct.first_frame = 0;
               }

               if(ADSP_EOK == rc)
               {
                  //wait ack from HW
                  count = MAX_LOOP;

                  do
                  {
                     if(HAL_rs_is_ack_valid(px))
                     {
                        job_ack = HAL_rs_get_job_accepted_status(px, &tmp_id);
                        // job accepted
                        if(job_ack)
                        {
                          if( tmp_id != job_id)
                          {
                             MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"PANIC@rs_send_job job_id: %d, tmp_id:%d\n",job_id, tmp_id); /*should not happen*/
                          }
                           break;
                        }
                     }

                     count--;

                     if(0 == count)
                     {
                        break;
                     }

                  } while(1);

                  if(!job_ack)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job hw does not accept the job\n");
                     rc = ADSP_EFAILED;
                  }
               }
            }
            qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);
         }
      }
      else
      {
         rc = ADSP_EFAILED;
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job driver_config is NULL\n");
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_send_job END\n");
#endif

   return rc;
}


/*
 * need to be called after receive the job finished signal, to confirm wether if the job is done or error
 *
 * @param[in] px processor ID
 * @param[in] job_id the job_id of the stream, provide by driver when call rs_init_job()
 * @param[in] samples_processed updated with processed samples within this jobs when return
 * @return job status 1:job success, 2:job error
 */
rs_job_status_type rs_confirm_job(uint16 px, int16 job_id, uint32* samples_processed)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *driver_config;
   uint32 i, samples;
   uint16 samp_bit_width;
   uint32 rs_vaddr_32_byte;
   uint32 rs_vsize_32_byte;
   rs_job_status_type job_status = RS_JOB_STATUS_INVALID;

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_cinfirm_job START\n");
#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
   }
   else
   {
      driver_config = rs_get_driver_config(job_id);

      if(driver_config)
      {
         HAL_rs_get_stream_struct(px, &driver_config->stream_struct, driver_config->ptr_stream_mem, driver_config->ptr_stream_mem_phys);
         //HAL_rs_get_stream_config(px, &driver_config->config_struct);

         if(driver_config->config_struct.first_frame==1)
         {
            driver_config->config_struct.first_frame = 0;
         }

         //debug_msg_print(driver_config);

         samples = driver_config->stream_struct.out_frame_size;

         //if internal buffer is needed, copy output samples from internal buffer to user buffer
         if(driver_config->internal_buf_needed)
         {
            samp_bit_width = (driver_config->config_struct.data_width == RS_SAMPLE_WIDTH_16) ? 2 : 4;

            for(i=0; i<driver_config->config_struct.num_channels; i++)
            {
               RS_ALIGN_32_BYTE((uint32)driver_config->stream_struct.out_buf_addr[i], samples * samp_bit_width, rs_vaddr_32_byte, rs_vsize_32_byte)
               qurt_elite_memorymap_cache_invalidate(rs_vaddr_32_byte, rs_vsize_32_byte);

               memscpy((void *)driver_config->stream_struct.usr_out_buf_addr[i],
                       samples*samp_bit_width,
                       (void *)driver_config->stream_struct.out_buf_addr[i],
                       samples*samp_bit_width);
            }
         }
         else
         {
            //client need to invalidate.
            //reset inp_buf_addr and out_buf_addr to NULL
             for(i=0; i < driver_config->config_struct.num_channels; i++)
             {
                driver_config->stream_struct.inp_buf_addr[i] = 0;
                driver_config->stream_struct.out_buf_addr[i] = 0;
             }
         }

         *samples_processed = samples;

        job_status = driver_config->job_status;

        //job is done
        //do devote
        //driver_config->job_state = RS_JOB_STATE_IDLE;
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_cinfirm_job END\n");
#endif

   return job_status;
}

/**
* This function is for reset hw resampler
* It is used in fast farward/rewind cases
* The frame sent via rs_send_job() is treated as the first frame after rs_reset_job() called
*
* @param[in] px - processor ID: 0 QDSP  1 APPLICATION PROCESSOR.
* @param[in] job_id - job ID.
* @return Success/failure of sending this frame job.
*/
ADSPResult rs_reset_job(uint16 px, int16 job_id, struct_rs_job_config* job_struct)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   ADSPResult rc = ADSP_EOK;
   struct_driver_config *driver_config;

   #ifdef DRIVER_DEBUG
     MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_reset_job START, job_id: %d\n",job_id);
   #endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      // retrive driver config structure based on job ID
      driver_config = rs_get_driver_config(job_id);

      if(driver_config && job_struct)
      {
         //1. Reset filter memory
         reset_filt_mem(job_struct, driver_config->stream_struct.filt_mem_base_addr);

         //2. Reset stream struct
         set_stream_struct(driver_config, job_struct);

         //3. Set as the first frame
         driver_config->config_struct.first_frame = 1;
      }
      else
      {
         rc = ADSP_EFAILED;
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_reset_job driver_config OR job_struct is NULL\n");
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_reset_job END\n");
#endif

   return rc;
}

/*
 *uninitialize the resampler job, free all resouce
 *
 * @param[in] px processor ID
 * @param[in] job_id the job_id of the stream, provide by driver when call rs_init_job()
 */
void rs_uninit_job(uint16 px, int16 job_id)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *ptr_driver_config;
   ADSPResult rc = ADSP_EOK;

//#ifdef DRIVER_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@@rs_unint_job START job_id: %d\n", job_id);
//#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
   }
   else
   {
      ptr_driver_config = rs_get_driver_config(job_id);

      if(ptr_driver_config)
      {
         //this job is done
         ptr_driver_config->job_state = RS_JOB_STATE_UNINITED;

         qurt_elite_mutex_lock(&ptr_driver_ctx->lock_mmpm_info);
         ptr_driver_ctx->mmpm_info.job_cnt--;

         if(0 == ptr_driver_ctx->mmpm_info.job_cnt)
         {
            //reset mmpm related global data
            ptr_driver_ctx->mmpm_info.power_mode = HAL_rs_get_power_mode_highest();

            set_mmpm_info(ptr_driver_ctx);

            if(ADSP_EOK != (rc = rs_mmpm_misc_dynamic_devote_clks(job_id)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"@rs_uninit_job rs_mmpm_misc_dynamic_devote is failed\n");
            }
            else
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"@rs_uninit_job hw resampler core clk and AHB BW are released\n");
            }

            drv_misc_deinit();
         }
         else
         {
            //this job is closed
            //re-estimate the core clk
            re_estimate_hw_operation_time_for_all(job_id);

            if(ADSP_EOK != (rc = rs_mmpm_misc_dynamic_vote_clks(job_id)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"@rs_uninit_job rs_mmpm_misc_dynamic_vote is failed\n");
            }
         }

         //release the memory allocated for thsi job
         if(ptr_driver_config)
         {
            free_alloc_memory(ptr_driver_config);
            rs_dismiss_job_id(job_id);
         }

         qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_mmpm_info);
      }
   }

//#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_unint_job END\n");
//#endif

}

/**
 * SW client submit resampling job using assigned job ID from driver
 *
 * @param[in] job_id the job_id of the stream, provide by driver when call rs_init_job()
 * @param[in] ptr_rs_dnsamp pointer to struct_rs_dnsampler
 * @return ADSP_EOK if succesful, else ADSP_EBADPARAM
 */
ADSPResult rs_get_stream_config(int16 job_id, struct_rs_dnsampler *ptr_rs_dnsamp)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *ptr_driver_config;
   ADSPResult rc = ADSP_EOK;


#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_get_stream_config START====\n");
#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      ptr_driver_config = rs_get_driver_config(job_id);

      if((ptr_rs_dnsamp) && (ptr_driver_config))
      {
         ptr_rs_dnsamp->up_samp_struct.int_phase_step_size = ptr_driver_config->stream_struct.int_phase_step_size;
         ptr_rs_dnsamp->up_samp_struct.cur_int_phase = ptr_driver_config->stream_struct.cur_int_phase;
         ptr_rs_dnsamp->up_samp_struct.frac_phase_step_size = ptr_driver_config->stream_struct.frac_phase_step_size;
         ptr_rs_dnsamp->up_samp_struct.cur_frac_phase = ptr_driver_config->stream_struct.cur_frac_phase;
         ptr_rs_dnsamp->up_samp_struct.out_freq = ptr_driver_config->stream_struct.out_freq;
         ptr_rs_dnsamp->up_samp_struct.up_filt_mem_size = HAL_rs_get_upsampler_flt_mem_length();

         ptr_rs_dnsamp->up_samp_struct.frac_const = ptr_driver_config->stream_struct.frac_const;
         ptr_rs_dnsamp->up_samp_struct.shift_const = ptr_driver_config->stream_struct.shift_const;

         ptr_rs_dnsamp->dn_filt_mem_size = ptr_driver_config->stream_struct.dn_filt_mem_size;
         ptr_rs_dnsamp->ds_factor = ptr_driver_config->stream_struct.dn_factor;
         ptr_rs_dnsamp->ds_step = ptr_driver_config->stream_struct.dn_step;
         //ptr_rs_dnsamp->filt_mem_conv_index = ptr_driver_config->stream_struct.dn_sampler_flt_fptr;
         //ptr_rs_dnsamp->filt_mem_load_index = ptr_driver_config->stream_struct.up_sampler_flt_optr; ;

         ptr_rs_dnsamp->up_sampler_flt_fptr = ptr_driver_config->stream_struct.up_sampler_flt_fptr;      //location of the end of the UpSampler filter = ptrDnSampStruct->filtMemOffset
         ptr_rs_dnsamp->up_sampler_flt_optr = ptr_driver_config->stream_struct.up_sampler_flt_optr;      //location of next sample generated by UPsampler

          //RESAMPLER_FILT_INDEX_PTRS_II
         ptr_rs_dnsamp->resampler_flt_iptr = ptr_driver_config->stream_struct.resampler_flt_iptr;     //location of next input sample read from memory  = ptrDnSampStruct->inputOffset
         ptr_rs_dnsamp->dn_sampler_flt_fptr = ptr_driver_config->stream_struct.dn_sampler_flt_fptr;     //location of the end of the DnSampler Filter
      }
      else
      {
         rc = ADSP_EBADPARAM;
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_get_stream_config END\n");
#endif

   return rc;
}

ADSPResult rs_set_stream_config(int16 job_id, struct_rs_dnsampler *ptr_rs_dnsamp)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *ptr_driver_config;
   ADSPResult rc = ADSP_EOK;


#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_get_stream_config START====\n");
#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      ptr_driver_config = rs_get_driver_config(job_id);

      if((ptr_rs_dnsamp) && (ptr_driver_config))
      {
         ptr_driver_config->stream_struct.int_phase_step_size = ptr_rs_dnsamp->up_samp_struct.int_phase_step_size;
         ptr_driver_config->stream_struct.cur_int_phase = ptr_rs_dnsamp->up_samp_struct.cur_int_phase;
         ptr_driver_config->stream_struct.frac_phase_step_size = ptr_rs_dnsamp->up_samp_struct.frac_phase_step_size;
         ptr_driver_config->stream_struct.cur_frac_phase = ptr_rs_dnsamp->up_samp_struct.cur_frac_phase;
         ptr_driver_config->stream_struct.out_freq = ptr_rs_dnsamp->up_samp_struct.out_freq;

         ptr_driver_config->stream_struct.frac_const = ptr_rs_dnsamp->up_samp_struct.frac_const;
         ptr_driver_config->stream_struct.shift_const = ptr_rs_dnsamp->up_samp_struct.shift_const;

         ptr_driver_config->stream_struct.dn_filt_mem_size = ptr_rs_dnsamp->dn_filt_mem_size;
         ptr_driver_config->stream_struct.dn_factor = ptr_rs_dnsamp->ds_factor;
         ptr_driver_config->stream_struct.dn_step = ptr_rs_dnsamp->ds_step;

         ptr_driver_config->stream_struct.up_sampler_flt_fptr = ptr_rs_dnsamp->up_sampler_flt_fptr;      //location of the end of the UpSampler filter = ptrDnSampStruct->filtMemOffset
         ptr_driver_config->stream_struct.up_sampler_flt_optr = ptr_rs_dnsamp->up_sampler_flt_optr;      //location of next sample generated by UPsampler

          //RESAMPLER_FILT_INDEX_PTRS_II
         ptr_driver_config->stream_struct.resampler_flt_iptr = ptr_rs_dnsamp->resampler_flt_iptr;     //location of next input sample read from memory  = ptrDnSampStruct->inputOffset
         ptr_driver_config->stream_struct.dn_sampler_flt_fptr = ptr_rs_dnsamp->dn_sampler_flt_fptr;     //location of the end of the DnSampler Filter
      }
      else
      {
         rc = ADSP_EBADPARAM;
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_get_stream_config END\n");
#endif

   return rc;
}

/**
* This function is for updaing the stream configuration
*
* @param[in] job_id - job ID.
* @param[in] ptr_rs_str_data - point to struct_rs_stream_data struct.
* @return Success/failure of query.
*/
ADSPResult rs_set_stream_data(int16 job_id, struct_rs_stream_data *ptr_rs_str_data, uint16 flag)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;
   struct_driver_config *ptr_driver_config;
   ADSPResult rc = ADSP_EOK;

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_set_stream_data START====\n");
#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else
   {
      ptr_driver_config = rs_get_driver_config(job_id);

      if((ptr_rs_str_data) && (ptr_driver_config))
      {
         ptr_driver_config->stream_struct.int_phase_step_size = ptr_rs_str_data->int_phase_step_size;
         ptr_driver_config->stream_struct.frac_phase_step_size = ptr_rs_str_data->frac_phase_step_size;

         if(6 == flag)
         {
            ptr_driver_config->stream_struct.cur_frac_phase = ptr_rs_str_data->cur_frac_phase;
            ptr_driver_config->stream_struct.shift_const = ptr_rs_str_data->shift_const;
            ptr_driver_config->stream_struct.frac_const = ptr_rs_str_data->frac_const;
            ptr_driver_config->stream_struct.out_freq = ptr_rs_str_data->med_freq;
         }
      }
      else
      {
         rc = ADSP_EBADPARAM;
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_set_stream_data END\n");
#endif

   return rc;

}


/**
 * Estimate the core clk for this job
 *
 * @param[in] job_id the job_id of the stream
 * @param[in] ptr_job_timing_info pointer to struct struct_rs_job_timing_info
 * @return ADSP_EOK if succesful, else failure
 */
ADSPResult rs_estimate_core_clks(int16 job_id, struct_rs_job_timing_info* ptr_job_timing_info)
{
   ADSPResult rc = ADSP_EBADPARAM;
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_estimate_core_clks START====\n");
#endif

   if((0 == ptr_driver_ctx->rs_prop.baseRegAddr) && (0 == ptr_driver_ctx->rs_prop.regSize))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW Resampler not supported in this LPASS version\n");
      rc = ADSP_EFAILED;
   }
   else if(ptr_job_timing_info)
   {
      ptr_job_timing_info->estimated_time_to_done = 0;

      if(ADSP_EOK == (rc = estimate_hw_operation_clks(job_id, ptr_job_timing_info)))
      {
         //convert clks to time
         rc = estimate_hw_operation_time(job_id, ptr_job_timing_info);
      }
   }

#ifdef DRIVER_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"===@rs_estimate_core_clks END\n");
#endif

   return rc;
}

/**
 * This function is for querying hw resampler version
 *
 * @return hw resampler version
 */
uint32 rs_query_hw_version(void)
{
   struct_driver_ctx *ptr_driver_ctx = &rs_driver_global_ctx;

   return ptr_driver_ctx->rs_prop.hw_revision;
}


