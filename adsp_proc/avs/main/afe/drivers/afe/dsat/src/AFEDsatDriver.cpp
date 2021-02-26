/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dsat/src/AFEDsatDriver.cpp#2 $
$DateTime: 2018/03/05 21:24:21 $
$Author: pwbldsvc $
$Change: 15605306 $
$Revision: #2 $

Copyright 2015 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dsat/src/AFEDsatDriver.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
6/4/15    YW      created

============================================================================*/

#include "AFEDsatDriver.h"
#include "avtimer_drv_api.h"
#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
#include "dangergen.h"
#ifdef __cplusplus
}
#endif //__cplusplus
/*==========================================================================
    Macro\enum definitions
  ========================================================================== */
/** Threshold values, when AFE port operates at default latency mode (1ms frame size)*/
#define AFE_PORT_DANGER_THRESH_NS  (700000)
#define AFE_PORT_WINDOW_THRESH_NS  (1000000)

/** Threshold values, when AFE port operates at low latency mode (0.5ms frame size) */
#define AFE_PORT_LL_DANGER_THRESH_NS  (350000)
#define AFE_PORT_LL_WINDOW_THRESH_NS  (500000)

/*==========================================================================
    Struct definitions
  ========================================================================== */
/* Structure of AFE dangergen util  */
typedef struct afe_danger_gen_info
{
   bool_t is_enabled;                  //Whether dangergen watchdog is currently enabled
   dangergen_handle_type dangergen_handle;             //Buffered handled use to access dangergen functionalities
   dangergen_watchdog_params_type watchdog_params;              //Buffered params to set watchdogs
}afe_danger_gen_info_t;
/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_open

 DESCRIPTION  Attempts to open danger gen functionality for the current port.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to open dangergen is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_open(afe_dev_port_t *pDevPort)
{
   dangergen_result_type result = DANGERGEN_SUCCESS;
   char client_name[QURT_ELITE_DEFAULT_NAME_LEN];
   //Try asking for dangergen handle
   if (NULL != pDevPort->port_topo_ptr || NULL == pDevPort->danger_info_ptr)
   {
      //Open Dangergen driver
      dangergen_open_params_type dangergen_params;
      dangergen_init_open_params(&dangergen_params);

      snprintf(client_name, QURT_ELITE_DEFAULT_NAME_LEN, "Afe[%2x]", pDevPort->intf);
      dangergen_params.username = client_name;
      dangergen_handle_type dangergen_handle;
      if (DANGERGEN_SUCCESS == dangergen_open(&dangergen_params, &dangergen_handle))
      {
         pDevPort->danger_info_ptr = (afe_danger_gen_info_t *)qurt_elite_memory_malloc(sizeof(afe_danger_gen_info_t), QURT_ELITE_HEAP_DEFAULT);
         if (NULL == pDevPort->danger_info_ptr)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DSAT registration not successful, continue without DSAT functionality. port-id: 0x%x", pDevPort->intf);
            dangergen_close(dangergen_handle);
            result = DANGERGEN_ERROR;
         }
         else //From this point danger info struct is allocated.
         {
            memset(pDevPort->danger_info_ptr, 0, sizeof(afe_danger_gen_info_t)); //init mem
            afe_danger_gen_info_t *danger_gen_ptr = (afe_danger_gen_info_t *)pDevPort->danger_info_ptr;
            danger_gen_ptr->dangergen_handle = (void *)dangergen_handle;

            //Initially configure watchdog params.
            dangergen_watchdog_params_type *watchdog_ptr = &(danger_gen_ptr->watchdog_params);
            dangergen_init_watchdog_params(watchdog_ptr);

            if (AFE_PORT_LOW_LATENCY_MODE == pDevPort->latency_mode)
            {
               //This is needed since danger gen will need minimal of period and threshold_1 to be set to continue
               watchdog_ptr->period_ns = AFE_PORT_LL_DANGER_THRESH_NS;
               //Setting windows so that danger will not last for too long in case AFE did not disable it.
               watchdog_ptr->window_ns = AFE_PORT_LL_WINDOW_THRESH_NS;
            }
            else
            {
               //This is needed since danger gen will need minimal of period and threshold_1 to be set to continue
               watchdog_ptr->period_ns = AFE_PORT_DANGER_THRESH_NS;
               //Setting windows so that danger will not last for too long in case AFE did not disable it.
               watchdog_ptr->window_ns = AFE_PORT_WINDOW_THRESH_NS;
            }
         }
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DSAT registration not successful, continue without DSAT functionality. port-id: 0x%x", pDevPort->intf);
         result = DANGERGEN_ERROR;
      }
   }
   return (DANGERGEN_SUCCESS == result) ? ADSP_EOK : ADSP_EFAILED;
}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_close

 DESCRIPTION  Close Dangergen functionality.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to close dangergen is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_close(afe_dev_port_t *pDevPort)
{
   if ((NULL != pDevPort->danger_info_ptr))
   {
      afe_danger_gen_info_t *danger_gen_ptr = (afe_danger_gen_info_t *)pDevPort->danger_info_ptr;
      if (NULL != danger_gen_ptr->dangergen_handle)
      {
         if (TRUE == danger_gen_ptr->is_enabled)
         {
            dangergen_disable(danger_gen_ptr->dangergen_handle);
         }
         dangergen_close(danger_gen_ptr->dangergen_handle);
      }
      qurt_elite_memory_free(pDevPort->danger_info_ptr);
      pDevPort->danger_info_ptr = NULL;
   }
   return ADSP_EOK;
}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_enable

 DESCRIPTION  Attempt to enable dangergen watchdog.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to enable watchdog is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_enable(afe_dev_port_t *pDevPort)
{
   dangergen_result_type result = DANGERGEN_SUCCESS;
   if (NULL != pDevPort->danger_info_ptr)
   {
      afe_danger_gen_info_t *danger_gen_ptr = (afe_danger_gen_info_t *)pDevPort->danger_info_ptr;
      dangergen_watchdog_params_type *watchdog_ptr = &danger_gen_ptr->watchdog_params;
      uint64_t current_timestamp = avtimer_drv_get_time(pDevPort->avt_drv);
      uint32_t time_elapsed = (current_timestamp - pDevPort->port_av_timestamp) * 1000;

      uint32_t danger_thresh_ns = AFE_PORT_DANGER_THRESH_NS;

      if (AFE_PORT_LOW_LATENCY_MODE == pDevPort->latency_mode)
      {
         danger_thresh_ns = AFE_PORT_LL_DANGER_THRESH_NS;
      }

      if (time_elapsed < danger_thresh_ns)
      {
         watchdog_ptr->threshold1_ns = danger_thresh_ns - time_elapsed;
      }
      else // In case the time it takes to reach afe port start processing is too late,
           //  we need to fire the danger right away;
      {
         watchdog_ptr->threshold1_ns = 1;
      }
      result = dangergen_configure_watchdog(danger_gen_ptr->dangergen_handle, watchdog_ptr);
      if (DANGERGEN_SUCCESS == result)
      {
         result = dangergen_enable(danger_gen_ptr->dangergen_handle);
      }
      if (DANGERGEN_SUCCESS == result)
      {
         danger_gen_ptr->is_enabled = TRUE;
      } //If any of them failed, display error to the latest error
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DSAT enabling error. port-id: 0x%x, result %lx", pDevPort->intf, result);
      }

   }
   return (DANGERGEN_SUCCESS == result) ? ADSP_EOK : ADSP_EFAILED;
}

/*=============================================================================
 FUNCTION     ADSPResult  afe_port_dangergen_disable

 DESCRIPTION  Attempt to disable dangergen watchdog.
 RETURN
           ADSP_EOK if successful
           ADSP_EFAILED if any failure occured
           Note that failure to disable watchdog is not fatal, we should proceed
           processing even if it failed.
           Reserving the error code in case in the future we need to make use of
           it

 DEPENDENCIES  None


 SIDE EFFECTS
 ===============================================================================*/

ADSPResult afe_port_dangergen_disable(afe_dev_port_t *pDevPort)
{
   dangergen_result_type result = DANGERGEN_SUCCESS;
   if (NULL != pDevPort->danger_info_ptr)
   {
      //disable danger-safe watchdog if so registered
      afe_danger_gen_info_t *danger_gen_ptr = (afe_danger_gen_info_t *)pDevPort->danger_info_ptr;
      if (NULL != danger_gen_ptr->dangergen_handle)
      {
         result = dangergen_disable(danger_gen_ptr->dangergen_handle);
         if (DANGERGEN_SUCCESS == result)
         {
            danger_gen_ptr->is_enabled = FALSE;
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DSAT disabling error. port-id: 0x%x, result %lx", pDevPort->intf, result);
         }
      }
   }
   return (DANGERGEN_SUCCESS == result) ? ADSP_EOK : ADSP_EFAILED;
}
