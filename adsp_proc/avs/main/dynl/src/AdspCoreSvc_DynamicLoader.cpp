/**
@file AdspCoreSvc_DynamicLoader.cpp***********************

@brief This file contains the implementation for the ADSP Dynamic Loader.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/dynl/src/AdspCoreSvc_DynamicLoader.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/13   DG      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2013-2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AdspCoreSvc_DynamicLoader.h"
#include "elite_thrd_prio.h"
#include "AdspCoreSvc_InstanceStruct.h"

#include "DALSys.h"
#include "adsp_amdb_static.h"
#include "VoiceMsgs.h"
#include "voice_proc_mgr.h"
#include "dlfcn.h"
#include "qurt_elite_adsppm_wrapper.h"
#include "module_mgr.h"

#ifndef SIM

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#include "HAP_fs.h"
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SIM

// Having this field enabled will help verify the codepath
// on ADSP Test framework (and continuous integration/testing)
// #define VOICE_DYN_LOAD_TEST

#ifdef VOICE_DYN_LOAD_TEST
#include "appi_gain.h"
#endif // VOICE_DYN_LOAD_TEST
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
typedef struct _adsp_dynamic_loader_t
{
	qurt_elite_thread_t thread;
	qurt_elite_signal_t thread_exit_signal;

   uint32_t adsppm_core_id;
   qurt_elite_adsppm_client_t *adsppm_wrapper;
} adsp_dynamic_loader_t;


/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
static int adsp_dynamic_loader_thread(void *obj);

/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */
static const char ADSP_DYNAMIC_LOADER_THREAD_NAME[] = "DYNL";
static const size_t ADSP_DYNAMIC_LOADER_STACK_SIZE = 6*1024;
/* =======================================================================
**                          Function Definitions
** ======================================================================= */

#define ENABLE_CLOCK_MAX
#ifdef ENABLE_CLOCK_MAX
void register_with_adsppm(adsp_dynamic_loader_t *pMe)
{
#if (ADSPPM_INTEGRATION==1)
   ADSPResult result = ADSP_EOK;

   MmpmRegParamType regParam;
   char threadname[16];
   qurt_thread_get_name(threadname, 16-1);

   regParam.rev             = MMPM_REVISION;
   regParam.instanceId      = MMPM_CORE_INSTANCE_0;
   regParam.pwrCtrlFlag     = PWR_CTRL_NONE; //PWR_CTRL_STATIC_DISPLAY, PWR_CTRL_THERMAL
   regParam.callBackFlag    = CALLBACK_NONE; //CALLBACK_STATIC_DISPLAY, CALLBACK_THERMAL, CALLBACK_REQUEST_COMPLETE
   regParam.MMPM_Callback   = NULL;
   regParam.cbFcnStackSize  = 0;

   regParam.coreId          = MMPM_CORE_ID_LPASS_ADSP; //no need to request power, register access.
   regParam.pClientName     = threadname;

   result = qurt_elite_adsppm_wrapper_register(&regParam, &pMe->adsppm_core_id, &pMe->adsppm_wrapper);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM register by AVCS. Result %lu. Client id %lu", result, pMe->adsppm_core_id);

#endif
   return ;
}

void deregister_from_adsppm(adsp_dynamic_loader_t *pMe)
{
   ADSPResult result = ADSP_EOK;

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->adsppm_core_id) )
   {
       return ;
   }

   uint32_t client_id = pMe->adsppm_core_id;
   result = qurt_elite_adsppm_wrapper_deregister(&pMe->adsppm_core_id, &pMe->adsppm_wrapper);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM deregister by AVCS. Result %lu. Client id %lu", result, client_id);
   return ;
}

static void max_out_pm(adsp_dynamic_loader_t *pMe, bool_t is_max_out)
{

#if (ADSPPM_INTEGRATION==1)
   ADSPResult result = ADSP_EOK;

   if ( !qurt_elite_adsppm_wrapper_is_registered(pMe->adsppm_core_id) )
   {
       return ;
   }

   static const uint8_t NUM_REQUEST=2;
   MmpmRscParamType rscParam[NUM_REQUEST];
   MMPM_STATUS      retStats[NUM_REQUEST];
   MmpmRscExtParamType reqParam;
   uint8_t req_num=0;
   MmpmMppsReqType mmpmMppsParam;
   MmpmGenBwValType bwReqVal;
   MmpmGenBwReqType bwReq;

   reqParam.apiType                    = MMPM_API_TYPE_SYNC;
   reqParam.pExt                       = NULL;       //for future
   reqParam.pReqArray                  = rscParam;
   reqParam.pStsArray                  = retStats;   //for most cases mmpmRes is good enough, need not check this array.
   reqParam.reqTag                     = 0;          //for async only
   uint32_t mpps,total;
   uint64_t newBw;
   {
      mpps =  500; //MPPS
      total = 10;
      if (!is_max_out)
      {
         mpps = 0; total=0;
      }

      mmpmMppsParam.mppsTotal                  = total;
      mmpmMppsParam.adspFloorClock             = mpps;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_MPPS;
      rscParam[req_num].rscParam.pMppsReq       = &mmpmMppsParam;
      req_num++;

   }

   {
      newBw = 400*1024*1024; //Bps
      if (!is_max_out) newBw = 0;

      bwReqVal.busRoute.masterPort                 = MMPM_BW_PORT_ID_ADSP_MASTER;
      bwReqVal.busRoute.slavePort                  = MMPM_BW_PORT_ID_DDR_SLAVE;
      bwReqVal.bwValue.busBwValue.bwBytePerSec     = newBw;
      bwReqVal.bwValue.busBwValue.usagePercentage  = 100;
      bwReqVal.bwValue.busBwValue.usageType        = MMPM_BW_USAGE_LPASS_DSP;

      bwReq.numOfBw            = 1;
      bwReq.pBandWidthArray    = &bwReqVal;

      rscParam[req_num].rscId                   = MMPM_RSC_ID_GENERIC_BW_EXT;
      rscParam[req_num].rscParam.pGenBwReq      = &bwReq;
      req_num++;
   }

   if (req_num )
   {
      reqParam.numOfReq                   = req_num;

      if ( is_max_out )
      {
         result = qurt_elite_adsppm_wrapper_request(pMe->adsppm_core_id, &pMe->adsppm_wrapper, &reqParam);
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM request MPPS %lu and BW %lu KBPS by AVCS (%lu). Result %lu", mpps, (uint32_t)(newBw/1024), pMe->adsppm_core_id, result);
      }
      else
      {
         result = qurt_elite_adsppm_wrapper_release(pMe->adsppm_core_id, &pMe->adsppm_wrapper, &reqParam);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPPM release BW and MPPS by AVCS (%lu). Result %lu", pMe->adsppm_core_id, result);
      }
   }

#endif //#if (ADSPPM_INTEGRATION==1)

   return ;
}
#endif //ENABLE_CLOCK_MAX

adsp_dynamic_loader_t *adsp_dynamic_loader_create(qurt_elite_channel_t *thread_exit_notification_channel)
{
	adsp_dynamic_loader_t *obj_ptr = (adsp_dynamic_loader_t*)qurt_elite_memory_malloc(sizeof(adsp_dynamic_loader_t), QURT_ELITE_HEAP_DEFAULT);
	if (NULL == obj_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Dynamic loader object failed in malloc");
		return NULL;
	}

	ADSPResult result;
	result = qurt_elite_signal_init(&obj_ptr->thread_exit_signal);
	if (ADSP_FAILED(result))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Dynamic loader object failed to init signal");
		qurt_elite_memory_free(obj_ptr);
		return NULL;
	}

	result = qurt_elite_channel_add_signal(thread_exit_notification_channel, &obj_ptr->thread_exit_signal, 0);
	if (ADSP_FAILED(result))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Dynamic loader object failed to add signal to channel");
		qurt_elite_signal_deinit(&obj_ptr->thread_exit_signal);
		qurt_elite_memory_free(obj_ptr);
		return NULL;
	}

	result = qurt_elite_thread_launch(&obj_ptr->thread, const_cast<char*>(ADSP_DYNAMIC_LOADER_THREAD_NAME), NULL, ADSP_DYNAMIC_LOADER_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_ADSP_CORE_SVC_PRIO_ID), adsp_dynamic_loader_thread, obj_ptr, QURT_ELITE_HEAP_DEFAULT);
	if (ADSP_FAILED(result))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Dynamic loader object failed to launch thread");
		qurt_elite_signal_deinit(&obj_ptr->thread_exit_signal);
		qurt_elite_memory_free(obj_ptr);
		return NULL;
	}

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: Dynamic loader thread created.");
	return obj_ptr;
}

void adsp_dynamic_loader_destroy(adsp_dynamic_loader_t *obj_ptr)
{
	int status;

	qurt_elite_thread_join(obj_ptr->thread, &status);
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: Dynamic loader thread joined with status %d.", status);

	qurt_elite_signal_clear(&obj_ptr->thread_exit_signal);
	qurt_elite_signal_deinit(&obj_ptr->thread_exit_signal);
	qurt_elite_memory_free(obj_ptr);
}

uint32_t adsp_dynamic_loader_get_signal_mask(adsp_dynamic_loader_t *obj_ptr)
{
	return qurt_elite_signal_get_channel_bit(&obj_ptr->thread_exit_signal);
}

static int adsp_dynamic_loader_thread(void *obj)
{
	adsp_dynamic_loader_t *obj_ptr __attribute__ ((unused)) = (adsp_dynamic_loader_t*)(obj);

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dynamic loading thread started. Waiting for remote file system to be up.");

#ifndef SIM
	HAP_rfs_wait();
#endif

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dynamic loading thread done waiting for remote file system");

#ifdef ENABLE_CLOCK_MAX
   register_with_adsppm(obj_ptr);
   max_out_pm(obj_ptr, TRUE);
#endif

   uint64_t start_time = qurt_elite_timer_get_time_in_msec();

#ifdef DYNAMIC_LOADING_THREAD_PROC

	//takes care of loading builtin if acdb file is absent.
	ADSPResult result = mm_register_modules_based_on_acdb();
   if (ADSP_FAILED(result))
   {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure in mm_register_modules_based_on_acdb %d", result);
   }

#else
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dynamic loading is disabled!!!");
#endif //DYNAMIC_LOADING_THREAD_PROC

#ifdef ENABLE_CLOCK_MAX
   max_out_pm(obj_ptr, FALSE);
   deregister_from_adsppm(obj_ptr);
#endif

	qurt_elite_signal_send(&obj_ptr->thread_exit_signal);

	uint64_t end_time = qurt_elite_timer_get_time_in_msec();
	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dynamic loading thread exiting. Time taken %lu ms ", (end_time-start_time));

	return ADSP_EOK;
}


