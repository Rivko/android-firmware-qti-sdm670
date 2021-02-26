/*========================================================================

*//** @file avtimer_drv_wrapper.c
This file contains utilities of AVTimers , such as
timer create, start/restart,delete.

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/multimedia2/Audio/drivers/hw/avtimer/avtimer_driver/rel/avs2.0/src/avtimer_drv_wrapper.c#1

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/06/11   kk     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "avtimer_drv_api.h"
#include "avtimer.h"
#include "avtimer_internal.h"
#include "avtimer_mmpm.h"
#include "avtimer_hal.h"
#include "aud_sync_hal.h"

#include "hwd_devcfg.h"

#include "aud_sync_hal_func_defs.h"
#include "avtimer_hal_func_defs.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

#define DEBUG_QURT_ELITE_HPTIMER
#undef DEBUG_QURT_ELITE_HPTIMER
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define IS_VALID_CLIENT_INDX(index) (((index) < MAX_NUM_OF_AVTIMER_CLIENTS) && ((index) >= 0)) ? TRUE: FALSE

/**
 * avtimer max duration = 8.9 years (1<<48 -1)
 */
#define AVTIMER_DRV_MAX_DURATION             AVTIMER_MAX_DURATION

/**
 * avtimer minimum duration
 */
#define AVTIMER_DRV_MIN_DURATION             AVTIMER_MIN_DURATION


/* -----------------------------------------------------------------------
**                  Global Object Definitions
** ----------------------------------------------------------------------- */
avtimer_drv_ctx_t avtimer_driver_global_ctx = {FALSE, };

avtimer_hw_cfg_t avtimer_hw_cfg;  

/* =======================================================================
**                          Internal Function Definitions
** ======================================================================= */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/****************************************************************************
** Enable/Disable avtimer hardware
*****************************************************************************/

avtimer_hw_index convert_to_avtimer_hw_index(uint32_t client_flag);
ADSPResult check_avtimer_open_param(avtimer_open_param_t* open_param_ptr);

/****************************************************************************
** Timers
*****************************************************************************/

/**
Initialize avtimer. Must be called from the premordial thread before 
using the avtimer

@return
 ADSPResult error code.
*/
ADSPResult avtimer_drv_init(uint32 *p_hw_revision)
{
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;
   ADSPResult status = ADSP_EFAILED;

//#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_init ---> \n");
//#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if(!ptr_driver_ctx->is_initialized)
   {
      int i;
      qurt_elite_mutex_init(&ptr_driver_ctx->lock_hw_access);

      memset((void*) &avtimer_hw_cfg, 0, sizeof(avtimer_hw_cfg_t));

      for(i = 0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
      {
         ptr_driver_ctx->mmpm_info.avtimer_job_cnt[i] = 0;

      }

      ptr_driver_ctx->avt_qmem_region = 0;
      ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt = 0;
      ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt = 0;
      ptr_driver_ctx->avsync_hw_reset = 0;

      for(i = 0; i < MAX_NUM_OF_AVTIMER_CLIENTS; i++)
      {
         ptr_driver_ctx->client_info_ptr[i] = NULL;
      }
  
      // read device config data
      if(ADSP_EOK == (status = HwdDevCfg_ReadModuleConfigData(HWD_MODULE_TYPE_AVTIMER, (void *) &(ptr_driver_ctx->avt_prop))))
      {
         if(0 != ptr_driver_ctx->avt_prop.hw_revision)
         {
            *p_hw_revision = avtimer_hw_cfg.hw_revision = ptr_driver_ctx->avt_prop.hw_revision;
            // memory mapping

            avtimer_hw_cfg.avtimer_base_phys_addr = ptr_driver_ctx->avt_prop.baseRegAddr;

            if(0 != ptr_driver_ctx->avt_prop.baseRegAddr)
            {
               if(ADSP_EOK != (status = HwdDevCfg_GetVirtAddress(&(ptr_driver_ctx->avt_qmem_region), ptr_driver_ctx->avt_prop.baseRegAddr, ptr_driver_ctx->avt_prop.regSize,
                        &avtimer_hw_cfg.avtimer_base_virt_addr)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer: getting virtual addr failed\n");
                  return status; 
               }
            }

            memscpy( (uint32_t *) avtimer_hw_cfg.avtimer_instance_info, sizeof(avtimer_hw_cfg.avtimer_instance_info),
                     (uint32_t *) ptr_driver_ctx->avt_prop.avtimer_instance_info, sizeof(ptr_driver_ctx->avt_prop.avtimer_instance_info) );

            avtimer_hw_cfg.audsync_offset = ptr_driver_ctx->avt_prop.audsync_offset_addr;

            avtimer_hw_cfg.avtimer_l2_int_0_num = ptr_driver_ctx->avt_prop.isrHookPinNum;
#ifdef DEBUG_AVTIMER_HAL
            avtimer_hw_cfg.l2vic_base_virt_addr = avtimer_server_get_virt_addr(
               0xFE290000, 0x10000);
            avtimer_hw_cfg.lcc_base_virt_addr = avtimer_server_get_virt_addr(
               0xFE000000, 0x4000);
            avtimer_hw_cfg.avtimer_l2_int_0_num_in_slice = ptr_driver_ctx->avt_prop.isrHookPinNum - 32 * 1;
#endif
            //allocate hal function map table

            if(NULL ==(avtimer_hw_cfg.avtimer_hal_func_def_ptr = (void *)qurt_elite_memory_malloc(sizeof(avtimer_hal_func_def_t), QURT_ELITE_HEAP_DEFAULT)))
            {
            	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate av timer hal function def object");
            }
            else
            {
               memset((void*) avtimer_hw_cfg.avtimer_hal_func_def_ptr, 0, sizeof(avtimer_hal_func_def_t));
            }


            if(NULL ==(avtimer_hw_cfg.avsync_hal_func_def_ptr = (void *)qurt_elite_memory_malloc(sizeof(avsync_hal_func_def_t), QURT_ELITE_HEAP_DEFAULT)))
            {
            	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate av sync hal function def object");
            }
            else
            {
               memset((void*) avtimer_hw_cfg.avsync_hal_func_def_ptr, 0, sizeof(avsync_hal_func_def_t));
            }

            if((avtimer_hw_cfg.avsync_hal_func_def_ptr) && (avtimer_hw_cfg.avtimer_hal_func_def_ptr))
            {
               //init the avtimer hal
               status = hal_avtimer_hw_init(&avtimer_hw_cfg);

               hal_aud_sync_init(&avtimer_hw_cfg);

               //1. call MMPM to enable clks
               if(ADSP_EOK != (status = avtimer_mmpm_register()))
               {
            	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_mmpm_register failed\n");
               }
               else
               {
            	   ptr_driver_ctx->is_initialized = TRUE;
               }
            }


         }
      }
   }

//#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_init <--- \n");
//#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   return status;
}

/**
Deinitialize avtimer. Must be called from the premordial thread 
before exiting the tests 

@return
 ADSPResult error code.
*/
ADSPResult avtimer_drv_deinit(void)
{
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   ADSPResult status = ADSP_EOK;
   int i;

//#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_deinit ---> \n");
//#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if(ptr_driver_ctx->is_initialized)
   {
      if(ADSP_EOK != (status = avtimer_mmpm_deregister()))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to avtimer mmpm deregister\n");
      }


      if (ptr_driver_ctx->is_initialized)
      {
         for(i = 0; i < AVTIMER_HW_INSTANCE_INDEX_MAX; i++)
         {
            ptr_driver_ctx->mmpm_info.avtimer_job_cnt[i] = 0;
         }

         ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt = 0;
         ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt = 0;
         ptr_driver_ctx->avsync_hw_reset = 0;

         qurt_elite_mutex_destroy(&ptr_driver_ctx->lock_hw_access);
         ptr_driver_ctx->is_initialized = FALSE;

         for(i = 0; i < MAX_NUM_OF_AVTIMER_CLIENTS; i++)
         {
            ptr_driver_ctx->client_info_ptr[i] = NULL;
         }
      }

      if(ptr_driver_ctx->avt_qmem_region)
      {
         HwdDevCfg_DestoryVirtAddress(ptr_driver_ctx->avt_qmem_region);
         ptr_driver_ctx->avt_qmem_region = 0;
      }

      if(avtimer_hw_cfg.avtimer_hal_func_def_ptr)
      {
         qurt_elite_memory_free((void*)avtimer_hw_cfg.avtimer_hal_func_def_ptr);
         avtimer_hw_cfg.avtimer_hal_func_def_ptr = NULL;
      }

      if(avtimer_hw_cfg.avsync_hal_func_def_ptr)
      {
         qurt_elite_memory_free((void*)avtimer_hw_cfg.avsync_hal_func_def_ptr);
         avtimer_hw_cfg.avsync_hal_func_def_ptr = NULL;
      }
   }

//#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_deinit <--- \n");
//#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   return status;
}


/**
Enable avtimer hardware.

@param[in/out] avtimer_drv_handle_t *avtimer_h
@param[in] avtimer_open_param_t* open_param_ptr

@return
 An indication of success or failure.

@dependencies
 Client should call this function before using any avtimer features
*/
ADSPResult avtimer_drv_hw_open(avtimer_drv_handle_t *avtimer_h, avtimer_open_param_t* open_param_ptr)
{
   ADSPResult       nStatus = ADSP_EFAILED;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   *avtimer_h = NULL;

   if(ADSP_EOK == (nStatus = check_avtimer_open_param(open_param_ptr)))
   {
      uint32_t client_index;

      nStatus = ADSP_EFAILED;

      qurt_elite_mutex_lock(&ptr_driver_ctx->lock_hw_access);

      if(ptr_driver_ctx->is_initialized)
      {
         uint64_t this_client_name = 0;

         //Also strlcpy takes the destination's size as a parameter and will not write more than that many bytes, to prevent buffer overflow
         strlcpy((char_t *)&(this_client_name), open_param_ptr->client_name, sizeof(this_client_name));
         
         //step 1: check if the open request from the same client
         for(client_index = 0; client_index < MAX_NUM_OF_AVTIMER_CLIENTS; client_index++)
         {
            if(ptr_driver_ctx->client_info_ptr[client_index])
            {
               avtimer_client_info_t* exist_client_ptr = (avtimer_client_info_t*) ptr_driver_ctx->client_info_ptr[client_index];

               avtimer_hw_index this_client_which_avtimer = convert_to_avtimer_hw_index(open_param_ptr->flag);

               if ((this_client_name == exist_client_ptr->client_name) && (exist_client_ptr->which_avtimer == this_client_which_avtimer))
               {
                   //find the matched one
                  exist_client_ptr->job_open_cnt++;
                  *avtimer_h = exist_client_ptr;
                  nStatus = ADSP_EOK;
                  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_hw_open client name[0x%lx%lx] index[%d] "
                                                       "open count [%d]", 
                        (uint32_t)(exist_client_ptr->client_name >> 32), (uint32_t)(exist_client_ptr->client_name), 
                        exist_client_ptr->index, exist_client_ptr->job_open_cnt);
                  break;
               }
            }
         }

         //step 2: if it is the fresh new open
         if(ADSP_EOK != nStatus)
         {
            avtimer_client_info_t* client_info_ptr = (avtimer_client_info_t *)qurt_elite_memory_malloc(sizeof(avtimer_client_info_t), QURT_ELITE_HEAP_DEFAULT);
            if(NULL==client_info_ptr)
            {
            	*avtimer_h = NULL;
            	 MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "memory allocation failed to allocate avtimer client info object");
            }
            else
            {
               memset((void*) client_info_ptr, 0, sizeof(avtimer_client_info_t));

               for(client_index = 0; client_index < MAX_NUM_OF_AVTIMER_CLIENTS; client_index++)
               {
                  if(NULL == ptr_driver_ctx->client_info_ptr[client_index])
                  {
                     nStatus = ADSP_EOK;
                     break;
                  }
               }

               if(ADSP_EOK == nStatus)
               {
                  client_info_ptr->index = client_index;
                  client_info_ptr->job_open_cnt++;

                  //Also strlcpy takes the destination's size as a parameter and will not write more than that many bytes, to prevent buffer overflow
                  strlcpy((char_t *)&(client_info_ptr->client_name), open_param_ptr->client_name, sizeof(client_info_ptr->client_name));

                  client_info_ptr->which_avtimer = convert_to_avtimer_hw_index(open_param_ptr->flag);
                  
                  nStatus = enable_avtimer_hw(client_info_ptr);

                  if(ADSP_EOK != nStatus)
                  {
                     qurt_elite_memory_free((void*)client_info_ptr);
                     client_info_ptr = NULL;
                  }
                  else
                  {
                     MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_hw_open client name[0x%lx%lx] index[%d] "
                                                       "open count [%d]", 
                        (uint32_t)(client_info_ptr->client_name >> 32), (uint32_t)(client_info_ptr->client_name), 
                        client_info_ptr->index, client_info_ptr->job_open_cnt);
                  }
                
                  ptr_driver_ctx->client_info_ptr[client_index] = client_info_ptr;
                  *avtimer_h = client_info_ptr;
               }
               else
               {
                  qurt_elite_memory_free((void*)client_info_ptr);
                  //print msg
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_hw_open  ERROR out of max avtimer job supported  i = %d\n", client_index);
               }
            }

         }
      }
      else
      {
         *avtimer_h = NULL;
         nStatus = ADSP_EFAILED;
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_open ERROR: not initialized!");
      }
      qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);
   }
   else
   {
      *avtimer_h = NULL;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_open  ERROR input paramter!\n");
   }

   return nStatus;
}

/**
Disable avtimer hardware.

@param[in] avtimer_drv_handle_t avtimer_h

@return
 An indication of success or failure.

@dependencies
 Client should call this function if client does not use any avtimer feature
*/
ADSPResult avtimer_drv_hw_close(avtimer_drv_handle_t avtimer_h)
{
   ADSPResult       nStatus = ADSP_EOK;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;
   uint32_t client_index;

   qurt_elite_mutex_lock(&ptr_driver_ctx->lock_hw_access);

   if(ptr_driver_ctx->is_initialized)
   {
     if(avtimer_h)
     {
       avtimer_client_info_t *client_info_ptr = (avtimer_client_info_t *)avtimer_h;

       nStatus = ADSP_ENOTEXIST;
       // Checking if the client exists in the AV Timer Client List
       for(client_index = 0; client_index < MAX_NUM_OF_AVTIMER_CLIENTS; client_index++)
       {
         if(client_info_ptr == ptr_driver_ctx->client_info_ptr[client_index])
         {
           nStatus = ADSP_EOK;
           break;
         }
       }

       if(nStatus != ADSP_EOK)
       {
         qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close Client not found in AV Timer Client List");
         return nStatus;
       }

       if(client_index != client_info_ptr->index)
       { 
         qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close Client index %d is not matching AV Timer Client context %d", client_info_ptr->index, client_index);
         return nStatus;
       }
       //step 1: decrease this client's job open cnt 1
       client_info_ptr->job_open_cnt--;

       MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_hw_close client name[0x%lx%lx] index[%d] "
             "open count [%d]",
             (uint32_t)(client_info_ptr->client_name >> 32), (uint32_t)(client_info_ptr->client_name),
             client_info_ptr->index, client_info_ptr->job_open_cnt);

       //step 2: if it is this client's last close, do the real close for this client
       if(0 == client_info_ptr->job_open_cnt)
       {
         nStatus = disable_avtimer_hw(client_info_ptr);

         if((0 <= client_info_ptr->index) && (MAX_NUM_OF_AVTIMER_CLIENTS > client_info_ptr->index))
         {
           ptr_driver_ctx->client_info_ptr[client_info_ptr->index] = NULL;
         }
         else
         {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close ERROR index out of range = %d\n", client_info_ptr->index);
         }

         //check avsync bt/fm cnt of this client for debug purpose
         if((0 != client_info_ptr->avsync_bt_job_cnt) || (0 != client_info_ptr->avsync_fm_job_cnt))
         {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close ERROR bt/fm cnt not zero when release the"
                 " client handle avsync_bt_job_cnt = %d avsync_fm_job_cnt = %d\n",
                 client_info_ptr->avsync_bt_job_cnt, client_info_ptr->avsync_fm_job_cnt);
         }

         qurt_elite_memory_free((void *)client_info_ptr);
       }
     }
     else
     {
       MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_hw_close  ERROR input handle is NULL!\n");
     }
   }
   else
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close ERROR: not initialized!");
   }
   qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);

   return nStatus; 
}

/*
Gets the avtimer current time since it was last reset.

@param[in] avtimer_drv_handle_t avtimer_h

@returns the current time since it was last reset in terms of microseconds
*/
uint64_t avtimer_drv_get_time(avtimer_drv_handle_t avtimer_h)
{
  uint64_t ullCurrTime = 0;
  avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

  if(NULL != avtimer_h)
  {
    avtimer_client_info_t *client_info_ptr = (avtimer_client_info_t *)avtimer_h;

    /* Range check for client index, to avoid crash during corruptions */
    if(IS_VALID_CLIENT_INDX(client_info_ptr->index))
    {
      /* Check whether the particular av timer hw mode is enabled or not */
      if(ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer])
      { 
        avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(client_info_ptr->which_avtimer);
        ullCurrTime = (uint64_t)hal_avtimer_get_current_hw_timer_tick(client_info_ptr->which_avtimer, hw_mode);
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_get_time: Invalid hw mode %d", client_info_ptr->which_avtimer); 
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_get_time: Invalid Index %d", client_info_ptr->index); 
    }
  }

  return ((uint64_t)ullCurrTime);
}

/*
latches both avtimer and qtimer.

@param[in] avtimer_h - Handle to avtimer
@param[out] avtimer_ts - avtimer timestamp
@param[out] qtimer_ts - qtimer timestamp

@returns ADSPResult
*/
ADSPResult avtimer_drv_latch_avtimer_qtimer(avtimer_drv_handle_t avtimer_h, uint64_t *avtimer_ts, uint64_t *qtimer_ts)
{

  ADSPResult result = ADSP_EOK;
  avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

  if(NULL != avtimer_h)
  {
    avtimer_client_info_t *client_info_ptr = (avtimer_client_info_t *)avtimer_h;

    /* Range check for client index, to avoid crash during corruptions */
    if(IS_VALID_CLIENT_INDX(client_info_ptr->index))
    {
      /* Check whether the particular av timer hw mode is enabled or not */
      if(ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer])
      {
        avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(client_info_ptr->which_avtimer);
        if(ADSP_EOK != (result = hal_avtimer_latch_av_q_timer_tick(client_info_ptr->which_avtimer, hw_mode, avtimer_ts,
                                                                  qtimer_ts)))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_avtimer_latch_av_q_timer_tick failed with result %d",
                       result);
        }
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_latch_avtimer_qtimer: Invalid hw index %d",
              client_info_ptr->which_avtimer);
        return ADSP_EUNSUPPORTED;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_latch_avtimer_qtimer: Invalid Index %d",
            client_info_ptr->index);
      return ADSP_EUNSUPPORTED;
    }
  }

  return ADSP_EOK;
}

/**
Disable avtimer hardware.

@param[in] avtimer_drv_handle_t avtimer_h
@param[in] avtimer_drv_ioctl_prop_info_t *prop_info_ptr

@return
 An indication of success or failure.

@dependencies
 Client should call this function if client does not use any avtimer feature
*/
ADSPResult avtimer_drv_ioctl(avtimer_drv_handle_t avtimer_h, avtimer_drv_ioctl_prop_info_t *prop_info_ptr)
{
   ADSPResult ret = ADSP_EOK;

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_ioctl ---> \n");
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if(avtimer_h && prop_info_ptr)
   {
      avtimer_client_info_t* client_info_ptr = (avtimer_client_info_t *) avtimer_h;

      switch(prop_info_ptr->prop)
      {
         case AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_BT:
         {
            client_info_ptr->avsync_bt_job_cnt++;

            if(1 == client_info_ptr->avsync_bt_job_cnt)
            {
               ret = enable_avsync_hw(AVTIMER_HW_TYPE_AVSYNC_BT);
            }
            break;
         }

         case AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_BT:
         {
            client_info_ptr->avsync_bt_job_cnt--;

            if(0 == client_info_ptr->avsync_bt_job_cnt)
            {
               ret = disable_avsync_hw(AVTIMER_HW_TYPE_AVSYNC_BT);
            }
            break;
         }

         case AVTIMER_DRV_PROPERTY_ENABLE_AVSYNC_FM:
         {
            client_info_ptr->avsync_fm_job_cnt++;

            if(1 == client_info_ptr->avsync_fm_job_cnt)
            {
               ret = enable_avsync_hw(AVTIMER_HW_TYPE_AVSYNC_FM);
            }
            break;
         }

         case AVTIMER_DRV_PROPERTY_DISABLE_AVSYNC_FM:
         {
            client_info_ptr->avsync_fm_job_cnt--;

            if(0 == client_info_ptr->avsync_fm_job_cnt)
            {
               ret = disable_avsync_hw(AVTIMER_HW_TYPE_AVSYNC_FM);
            }
            break;
         }
         default:
            break;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_ioctl  ERROR input handle is NULL!\n");
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_ioctl <--- \n");
#endif

   return ret;
}



/*
APIs: create, delete, get duration
   Handler: signal
   Timer Type: ONESHOT, PERIODIC, SLEEP
   Duration: In us
 */
/*
qurt_elite_timer_create:
* @param[in] timer_ptr Pointer to qurt_elite timer object
* @param[in] timer_type  One of : QURT_ELITE_TIMER_ONESHOT_DURATION, QURT_ELITE_TIMER_PERIODIC,QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
* @param[in] clock_source: only QURT_ELITE_TIMER_AVS
* @param[in] signal_ptr pointer to signal to be generated when timer expires.

* @return indication of success (0) or failure (non-0)
*/

ADSPResult avtimer_drv_create(qurt_elite_timer_t *timer_ptr, qurt_elite_timer_duration_t timer_type, qurt_elite_signal_t *signal_ptr)
{
   avtimer_attr_t     timer_attr;
   ADSPResult nStatus;

   if (NULL == (timer_ptr->pChannel = qurt_elite_signal_get_channel(signal_ptr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The signal does not belong to any channel");
      return ADSP_EBADPARAM;
   }

   timer_ptr->uTimerType = (unsigned int)timer_type;
   timer_ptr->timer_sigmask = qurt_elite_signal_get_channel_bit(signal_ptr);
   timer_ptr->istimerCreated = FALSE;

   avtimer_attr_init(&timer_attr);

   /* Create one shot timer, but do not start it.*/
   /* Create peridic timer , but start it always*/
   /* Stopping and restarting timers are only applicable for ONESHOT timers*/

   avtimer_attr_settype((avtimer_attr_t *)&(timer_attr), (avtimer_type_t)timer_type );

   if (QURT_ELITE_TIMER_PERIODIC != timer_type)
   {

       if (QURT_ELITE_TIMER_ONESHOT_DURATION == timer_type)
       {
           avtimer_attr_setduration(&timer_attr, AVTIMER_DRV_MAX_DURATION);
#ifdef DEBUG_QURT_ELITE_HPTIMER
           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "timertype is one shot duration %d\n",timer_type);
#endif
       }
       else
       {
           avtimer_attr_setexpiry(&timer_attr, AVTIMER_DRV_MAX_DURATION);
#ifdef DEBUG_QURT_ELITE_HPTIMER
           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "timertype is one shot abs %d\n",timer_type);
#endif
       }

       if ( ADSP_EOK != (nStatus = avtimer_timer_create( (avtimer_t *) &(timer_ptr->qTimerObj), &timer_attr,
                                                    timer_ptr->pChannel, timer_ptr->timer_sigmask)) )
       {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating timer with status=%d!\n", nStatus);
           return nStatus;
       }
    }
#ifdef DEBUG_QURT_ELITE_HPTIMER
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Timer created successfully");
#endif //DEBUG_QURT_ELITE_HPTIMER
    return ADSP_EOK;
}

ADSPResult avtimer_drv_sleep(uint64_t llMicrosec)
{
   avtimer_t          timer;
   avtimer_attr_t     timer_attr;
   avtimer_duration_t timer_duration;
   ADSPResult       nStatus;
   qurt_elite_signal_t sig_job;      
   qurt_elite_channel_t channel_job;    
   uint32 sig_mask;


   
   if (llMicrosec < AVTIMER_DRV_MIN_DURATION)
   {
       timer_duration = (avtimer_duration_t)AVTIMER_DRV_MIN_DURATION;
   }
   else
   {
       timer_duration = (avtimer_duration_t) llMicrosec;
   }


   /* setup timer attributes */
   avtimer_attr_init(&timer_attr);
   avtimer_attr_settype(&timer_attr, AVTIMER_ONESHOT_DURATION );
   avtimer_attr_setduration(&timer_attr, timer_duration);

   qurt_elite_channel_init(&channel_job);
   qurt_elite_signal_init(&sig_job);    
   qurt_elite_channel_add_signal(&channel_job, &sig_job, 0); 
   sig_mask = qurt_elite_signal_get_channel_bit(&sig_job);


   if ( ADSP_EOK != (nStatus = avtimer_timer_create( &timer, &timer_attr, &channel_job, sig_mask)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating sleep timer with status=%d!", nStatus);
      return nStatus;
   }
   if ( ADSP_EOK != (nStatus =avtimer_timer_restart((avtimer_t)(timer),(avtimer_time_t)timer_duration)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Duration timer with status=%d!", nStatus);
       return nStatus;
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "wait until sleep timer expires");
#endif //DEBUG_QURT_ELITE_HPTIMER

   qurt_elite_channel_wait(&channel_job, sig_mask);

   qurt_elite_signal_clear(&sig_job);
   qurt_elite_signal_deinit(&sig_job);      
   qurt_elite_channel_destroy(&channel_job);

   if(ADSP_EOK != (avtimer_timer_delete((avtimer_t)(timer))))
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed deleting the timer");
       nStatus = ADSP_EFAILED;
   }

   return nStatus;

}


ADSPResult avtimer_drv_delete(qurt_elite_timer_t *timer_ptr)
{
    ADSPResult nStatus = 0;

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_delete ---> nStatus = %d\n", nStatus);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if (NULL == timer_ptr || 0 == timer_ptr->qTimerObj)
   {
      return ADSP_EBADPARAM;
   }


    if(ADSP_EOK != (nStatus=avtimer_timer_delete((avtimer_t)(timer_ptr->qTimerObj))))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed deleting timer with status=%d!", nStatus);
        return nStatus;
    }

    memset(timer_ptr,0,sizeof(qurt_elite_timer_t));

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_delete <--- nStatus = %d\n", nStatus);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER
    return ADSP_EOK;
}


uint64_t avtimer_drv_get_duration(qurt_elite_timer_t *timer_ptr)
{
   ADSPResult   nStatus;
   avtimer_attr_t      timer_attr;
   avtimer_duration_t  timer_duration = 0;

   avtimer_attr_init(&timer_attr);

   if ( ADSP_EOK != (nStatus=avtimer_get_attr(timer_ptr->qTimerObj, &timer_attr)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed getting timer attributes with %d\n!", nStatus);
      /* return 0 rather than error status yet interpreted as an unsigned number */
      return 0;
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer attributes state is %lu\n!", timer_attr.state);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer attributes duration is %lld\n!", timer_attr.duration);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer attributes expiry is %lld\n!", timer_attr.expiry);
   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_get_current_hw_timer_tick is %lld \n",avtimer_get_current_hw_timer_tick());
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer attributes remaining is %lld\n!", timer_attr.remaining);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer attributes type is %d\n!", timer_attr.type);
#endif

   /* query the OS for the timer duration */
   avtimer_attr_getduration(&timer_attr, &timer_duration);
   return ((uint64_t) timer_duration);
}

/*
Start/Restart Absolute One shot timer
@param[in] timer object
@param[in] duration : absolute time of the timer in usec
@return indication of success (0) or failure (non-0)
*/



ADSPResult avtimer_drv_start_absolute(qurt_elite_timer_t *timer_ptr, uint64_t time)

{

   ADSPResult nStatus;

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_start_absolute ---> time = %lu\n", (uint32_t)time);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if(QURT_ELITE_TIMER_ONESHOT_ABSOLUTE != timer_ptr->uTimerType)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_start_absolute ---> usage of wrong timer type = %ld", timer_ptr->uTimerType);
      return ADSP_EBADPARAM;
   }

   if ( ADSP_EOK != (nStatus =avtimer_timer_restart((avtimer_t)(timer_ptr->qTimerObj),(avtimer_time_t)time)))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Absolute timer with status=%d!", nStatus);
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_start_absolute <--- \n");
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   return nStatus;
}



/*
Start/Restart Duration based One shot timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/



ADSPResult avtimer_drv_start_duration(qurt_elite_timer_t *timer_ptr, uint64_t duration)

{
   ADSPResult nStatus;

   avtimer_time_t timer_duration;

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_start_duration ---> time = %lu\n", (uint32_t)duration);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if(QURT_ELITE_TIMER_ONESHOT_DURATION != timer_ptr->uTimerType)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_start_duration ---> usage of wrong timer type = %ld", timer_ptr->uTimerType);
      return ADSP_EBADPARAM;
   }

   if ( (AVTIMER_DRV_MAX_DURATION < duration) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Timer too long!");
      return ADSP_EBADPARAM;
   }

   timer_duration = (avtimer_time_t)duration;

   if ( (AVTIMER_DRV_MIN_DURATION > duration))
   {
        timer_duration = (avtimer_time_t)AVTIMER_DRV_MIN_DURATION;
   }

   if ( ADSP_EOK != (nStatus =avtimer_timer_restart((avtimer_t)(timer_ptr->qTimerObj),timer_duration)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Duration timer with status=%d!", nStatus);
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_start_duration <--- \n");
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   return nStatus;
}


/*
Start Periodic timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/


ADSPResult avtimer_drv_start_periodic(qurt_elite_timer_t *timer_ptr, uint64_t duration)

{
   ADSPResult nStatus;
   avtimer_attr_t timer_attr;

   if(QURT_ELITE_TIMER_PERIODIC != timer_ptr->uTimerType)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_start_periodic ---> usage of wrong timer type = %ld", timer_ptr->uTimerType);
      return ADSP_EBADPARAM;
   }

   if(FALSE == timer_ptr->istimerCreated)
   {
       avtimer_attr_init(&timer_attr);
       avtimer_attr_settype((avtimer_attr_t *)&(timer_attr), AVTIMER_PERIODIC );
       avtimer_attr_setduration((avtimer_attr_t *)&(timer_attr), duration);


       if ( ADSP_EOK != (nStatus =avtimer_timer_create( (avtimer_t *) &(timer_ptr->qTimerObj), (avtimer_attr_t *)&(timer_attr),
                                            timer_ptr->pChannel, timer_ptr->timer_sigmask)))
       {
           MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Starting Periodic timer with status=%d!", nStatus);
           return nStatus;
       }
       timer_ptr->istimerCreated = TRUE;
    }
    else
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Restarting is not supported for periodic timer!");
        return ADSP_EFAILED;
    }

    return ADSP_EOK;
}

/*
Cancel Duration based One shot timer
@param[in] timer object
@return indication of success (0) or failure (non-0)
*/
ADSPResult avtimer_drv_cancel_oneshot(qurt_elite_timer_t *timer_ptr)

{

   ADSPResult nStatus = 0;

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_cancel_oneshot ---> nStatus = %d\n", nStatus);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER

   if((QURT_ELITE_TIMER_ONESHOT_DURATION !=timer_ptr->uTimerType) &&
   (QURT_ELITE_TIMER_ONESHOT_ABSOLUTE !=timer_ptr->uTimerType))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_cancel_oneshot ---> usage of wrong timer type = %ld", timer_ptr->uTimerType);
      return ADSP_EBADPARAM;
   }

   if ( ADSP_EOK != (nStatus =avtimer_timer_stop((avtimer_t)(timer_ptr->qTimerObj))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed stop Duration timer with status=%d!", nStatus);
   }

#ifdef DEBUG_QURT_ELITE_HPTIMER
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer_drv_cancel_oneshot <--- nStatus = %d\n", nStatus);
#endif //#ifdef DEBUG_QURT_ELITE_HPTIMER
   return nStatus;
}


uint32 avtimer_server_get_virt_addr(uint32 reg_addr_phy, size_t page_size)
{

   uint32 reg_addr_virt = 0;
   int32 rc = QURT_EOK;
   qurt_mem_cache_mode_t cache_attribs =  QURT_MEM_CACHE_NONE;
   qurt_perm_t       perm = QURT_PERM_READ | QURT_PERM_WRITE | QURT_PERM_EXECUTE ;

   if ( QURT_EOK != (rc = qurt_mem_map_static_query((qurt_addr_t *)&reg_addr_virt,
                                               reg_addr_phy,
                                               page_size,
                                               cache_attribs,
                                               perm )))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_server_get_virt_addr: Error Obtaining qurt_mem_map_static_query %d \n", (int)rc);
      reg_addr_virt = 0;
   }

   return reg_addr_virt;
}

ADSPResult enable_avtimer_hw(avtimer_client_info_t* client_info_ptr)
{
   ADSPResult       nStatus = ADSP_EOK;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;
   avtimer_hw_mode which_mode;

   if(client_info_ptr)
   {
      ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer]++;

      if(1 == ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer])
      {
         //1. vote for clks
         if(ADSP_EOK == (nStatus = avtimer_mmpm_voting(AVTIMER_MMPM_VOTE_AVTIMER_CLK)))
         {
            which_mode = get_avtimer_hw_instance_mode(client_info_ptr->which_avtimer);  
            nStatus = hal_avtimer_hw_enable(client_info_ptr->which_avtimer, which_mode);

            //if(AVTIMER_HW_INSTANCE_INDEX_0 == client_info_ptr->which_avtimer)
            //{
            //   hal_aud_sync_reset_all();
            //}

            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "enable_avtimer_hw for AVTIMER_HW_INSTANCE_INDEX 0x%x  STATUS 0x%x\n", client_info_ptr->which_avtimer, nStatus);
         }
      }
   }

#ifdef DEBUG_HWD_DEV_CONFIG_DATA
   HwdDevCfg_Debug();
#endif

   return nStatus;
}

ADSPResult disable_avtimer_hw(avtimer_client_info_t* client_info_ptr)
{
   ADSPResult       nStatus = ADSP_EOK;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;
   avtimer_hw_mode which_mode;

   if(client_info_ptr)
   {
      ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer]--;

      if(0 > ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer])
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv_hw_close ERROR avtimer_job_cnt = %ld, avtimer_hw_idx = %d\n", ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer], client_info_ptr->which_avtimer);
      }

      if(0 == ptr_driver_ctx->mmpm_info.avtimer_job_cnt[client_info_ptr->which_avtimer])
      {
         which_mode = get_avtimer_hw_instance_mode(client_info_ptr->which_avtimer); 
         if(ADSP_EOK != (nStatus = hal_avtimer_hw_disable(client_info_ptr->which_avtimer, which_mode)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_avtimer_hw_disable failed %d \n", (int)nStatus);
         }
         //call MMPM to disable clks anywa
         else if(ADSP_EOK != (nStatus = avtimer_mmpm_devoting(AVTIMER_MMPM_VOTE_AVTIMER_CLK)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_mmpm_devoting failed %d \n", (int)nStatus);
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "disable_avtimer_hw for AVTIMER_HW_INSTANCE_INDEX 0x%x  STATUS 0x%x\n", client_info_ptr->which_avtimer, nStatus);
         }
      }
   }

   return nStatus;
}

ADSPResult enable_avsync_hw(avtimer_hw_type_t type)
{
   ADSPResult       nStatus = ADSP_EOK;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   qurt_elite_mutex_lock(&ptr_driver_ctx->lock_hw_access);

   switch (type)
   {
      case AVTIMER_HW_TYPE_AVSYNC_BT:
      {
         ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt++;

         if(1 == ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "enable_avsync_hw for AVTIMER_HW_TYPE_AVSYNC_BT\n");
            nStatus = avtimer_mmpm_voting(AVTIMER_MMPM_VOTE_AVSYNC_BT_CLK);

            if(0 == ptr_driver_ctx->avsync_hw_reset)
            {
               ptr_driver_ctx->avsync_hw_reset = 1;
               hal_aud_sync_reset_all();
            }
         }
      }
      break;
      case AVTIMER_HW_TYPE_AVSYNC_FM:
      {
         ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt++;

         if(1 == ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "enable_avsync_hw for AVTIMER_HW_TYPE_AVSYNC_FM\n");
            nStatus = avtimer_mmpm_voting(AVTIMER_MMPM_VOTE_AVSYNC_FM_CLK);

            if(0 == ptr_driver_ctx->avsync_hw_reset)
            {
               ptr_driver_ctx->avsync_hw_reset = 1;
               hal_aud_sync_reset_all();
            }
         }
      }
      break;
      default:
         nStatus = ADSP_EFAILED;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv ERROR enable_avsync_hw type = %d\n", type);
      break;
   }

   qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);

   return nStatus;
}

ADSPResult disable_avsync_hw(avtimer_hw_type_t type)
{
   ADSPResult       nStatus = ADSP_EOK;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   qurt_elite_mutex_lock(&ptr_driver_ctx->lock_hw_access);

   switch (type)
   {
      case AVTIMER_HW_TYPE_AVSYNC_BT:
      {
         ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt--;

         if(0 > ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv ERROR avsync_bt_job_cnt = %ld\n", ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt);
         }

         if(0 == ptr_driver_ctx->mmpm_info.avsync_bt_job_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "disable_avsync_hw for AVTIMER_HW_TYPE_AVSYNC_BT\n");
            nStatus = avtimer_mmpm_devoting(AVTIMER_MMPM_VOTE_AVSYNC_BT_CLK);
         }
      }
      break;
      case AVTIMER_HW_TYPE_AVSYNC_FM:
      {
         ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt--;

         if(0 > ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv ERROR avsync_fm_job_cnt = %ld\n", ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt);
         }

         if(0 == ptr_driver_ctx->mmpm_info.avsync_fm_job_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "disable_avsync_hw for AVTIMER_HW_TYPE_AVSYNC_FM\n");
            nStatus = avtimer_mmpm_devoting(AVTIMER_MMPM_VOTE_AVSYNC_FM_CLK);
         }
      }
      break;
      default:
         nStatus = ADSP_EFAILED;
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "avtimer_drv ERROR enable_avsync_hw type = %d\n", type);
      break;
   }

   qurt_elite_mutex_unlock(&ptr_driver_ctx->lock_hw_access);

   return nStatus;
}

avtimer_hw_index convert_to_avtimer_hw_index(uint32_t client_flag)
{
   avtimer_hw_index hw_idx = AVTIMER_HW_INSTANCE_INDEX_0;

   switch(client_flag)
   {
      case AVTIMER_DRV_OPEN_FLAG_ATIMER_1:
         hw_idx = AVTIMER_HW_INSTANCE_INDEX_1;
         break;
      case AVTIMER_DRV_OPEN_FLAG_ATIMER_2:
         hw_idx = AVTIMER_HW_INSTANCE_INDEX_2;
         break;
      default:
         break;
   }

   return hw_idx;
}

avtimer_hw_mode  get_avtimer_hw_instance_mode(avtimer_hw_index hw_index)
{
   avtimer_hw_mode hw_mode;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   hw_mode = ptr_driver_ctx->avt_prop.avtimer_instance_info[hw_index].mode;

   return hw_mode;
}

ADSPResult check_avtimer_open_param(avtimer_open_param_t* open_param_ptr)
{
   ADSPResult       nStatus = ADSP_EFAILED;
   avtimer_drv_ctx_t *ptr_driver_ctx = &avtimer_driver_global_ctx;

   if(open_param_ptr)
   {
      int open_flag = open_param_ptr->flag;

      switch(ptr_driver_ctx->avt_prop.hw_revision)
      {
         case AVTIMER_HW_VERSION_2:
            if((AVTIMER_DRV_OPEN_FLAG_DEFAULT_AVTIMER == open_flag) || 
               (AVTIMER_DRV_OPEN_FLAG_ATIMER_1 == open_flag))
            {
               nStatus = ADSP_EOK;
            }
            break;
         case AVTIMER_HW_VERSION_2_1:
            if((AVTIMER_DRV_OPEN_FLAG_DEFAULT_AVTIMER == open_flag) || 
               (AVTIMER_DRV_OPEN_FLAG_ATIMER_1 == open_flag)        ||
               (AVTIMER_DRV_OPEN_FLAG_ATIMER_2 == open_flag))
            {
               nStatus = ADSP_EOK;
            }
            break;
         default:
            break;
      }
   }

   return nStatus;
}

/*
Convert the avtimer tick count to time

@returns the time for the hw tick count
*/

uint64_t avtimer_drv_convert_hw_tick_to_time(uint64_t tick_count)
{
  avtimer_hw_mode hw_mode = get_avtimer_hw_instance_mode(AVTIMER_HW_INSTANCE_INDEX_0);

  return hal_avtimer_convert_hw_tick_to_time(hw_mode, tick_count);
}
