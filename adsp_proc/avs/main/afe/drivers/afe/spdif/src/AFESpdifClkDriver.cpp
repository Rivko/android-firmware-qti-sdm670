/*========================================================================
This file contains functions for the AFE Spdif clock config

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifClkDriver.cpp#1 $
 ====================================================================== */

#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "qurt_elite.h"
#include "AFESpdifClkDriver_i.h"
#include "hwd_mmpm.h"
#include "adsp_afe_service_commands.h"

typedef enum 
{
    SPDIF_CLK_OFF = 0,
    SPDIF_CLK_ON,
} spdif_clk_status_t;


/**
* spdif_clk_cfg_t
*/
typedef struct _spdif_clk_cfg
{
   ClockIdType             spdif_core_clk_id;          //get from clock regime
   ClockIdType             spdif_npl_clk_id;          //get from clock regime
   uint32                  clk_root;   //TODO need to change clock regime time later if it is ready.
   uint32                  clk_freq;
} spdif_clk_cfg_t;


/**
* spdif_clk_cfg_adspm_info_t
*/
typedef struct _spdif_clk_cfg_adspm_info
{
   uint32 client_id;
   uint32 power_vote_cnt;
}  spdif_clk_cfg_adspm_info_t;


/**
* spdif_clk_cfg_global_info_t
*/
typedef struct _spdif_clk_cfg_global_info
{
   DalDeviceHandle            *p_dal_handle; 
   spdif_clk_cfg_t            spdif_clk_cfg;
   spdif_clk_status_t                     clk_status;
   spdif_clk_cfg_adspm_info_t adsppm_info;
}  spdif_clk_cfg_global_info_t;


#define SPDIF_SIM_TEST_CLOCK_ID 22 /* if it is zero, it will return error */

spdif_clk_cfg_global_info_t spdif_clk_cfg_global_ctx;

static DALResult afe_spdif_clk_set_enable(DalDeviceHandle* p_handle, spdif_clk_cfg_t* p_clk_cfg);
static DALResult afe_spdif_clk_set_disable(DalDeviceHandle* p_handle, spdif_clk_cfg_t* p_clk_cfg);

ADSPResult afe_spdif_init_clks(void)
{
   DALResult res = DAL_ERROR;
   ADSPResult result = ADSP_EFAILED;
   spdif_clk_cfg_global_info_t *p_spdif_clk_cfg_global_ctx = &spdif_clk_cfg_global_ctx;
   ClockIdType clk_regime_id;

   memset((void*)p_spdif_clk_cfg_global_ctx, 0, sizeof(spdif_clk_cfg_global_info_t));

   //1. Register for ADSPPM
   if(0 != (p_spdif_clk_cfg_global_ctx->adsppm_info.client_id = HwdMmpm_Register(MMPM_CORE_ID_LPASS_SPDIF, MMPM_CORE_INSTANCE_0)))

   {
#ifndef SIM
      if(DAL_SUCCESS == (res = DAL_DeviceAttach(DALDEVICEID_CLOCK, (DalDeviceHandle **)&p_spdif_clk_cfg_global_ctx->p_dal_handle)))
#else
      if(1)
#endif
      {
         /* Get the clock regime Id for spdif core clock */
#ifndef SIM
         res = DalClock_GetClockId(p_spdif_clk_cfg_global_ctx->p_dal_handle, (const char *)"spdiftx_bmc_clk", &clk_regime_id);
#else
         clk_regime_id = (ClockIdType)SPDIF_SIM_TEST_CLOCK_ID; // non-zero value for sim test.
         res = DAL_SUCCESS;
#endif
         if(DAL_SUCCESS == res)    
         {
            p_spdif_clk_cfg_global_ctx->spdif_clk_cfg.spdif_core_clk_id = clk_regime_id;
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE_SPDIF: DalClock_GetClockId not supported for spdif core clk: res: 0x%x", res);
         }

         /* Get the clock regime Id for spdif npl clock */
#ifndef SIM
         res = DalClock_GetClockId(p_spdif_clk_cfg_global_ctx->p_dal_handle, (const char *)"spdiftx_bmc_npl_clk", &clk_regime_id);
#else
         clk_regime_id = (ClockIdType)SPDIF_SIM_TEST_CLOCK_ID; // non-zero value for sim test.
         res = DAL_SUCCESS;
#endif
         if(DAL_SUCCESS == res)    
         {
            p_spdif_clk_cfg_global_ctx->spdif_clk_cfg.spdif_npl_clk_id = clk_regime_id;
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE_SPDIF: DalClock_GetClockId not supported for spdif npl clk: res: 0x%x", res);
         }

         
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DAL_DeviceAttach fail res: 0x%x\n", res);
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: HwdMmpm_Register fail \n");
   }

   if(DAL_SUCCESS == res)
   {
      result = ADSP_EOK;
   }

   return result;

}

ADSPResult afe_spdif_deinit_clks(void)
{
   DALResult res = DAL_ERROR;
   ADSPResult result = ADSP_EFAILED;
   spdif_clk_cfg_global_info_t *p_spdif_clk_cfg_global_ctx = &spdif_clk_cfg_global_ctx;
   
   //Deregister ADSPPM
   if(DAL_SUCCESS != (res = HwdMmpm_Deregister(p_spdif_clk_cfg_global_ctx->adsppm_info.client_id)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: HwdMmpm_Deregister fail \n");
   }
#ifndef SIM
   res = DAL_DeviceDetach(p_spdif_clk_cfg_global_ctx->p_dal_handle);
#endif

   if(DAL_SUCCESS == res)
   {
      result = ADSP_EOK;
   }
   return result;

}

//TODO need to handle clock root.
static DALResult afe_spdif_clk_set_enable(DalDeviceHandle* p_handle, spdif_clk_cfg_t* p_clk_cfg)
{
   DALResult res = DAL_ERROR;

   if(p_clk_cfg)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: afe_spdif_clk_set_enable: clk root: %ld clk freq: %ld", p_clk_cfg->clk_root, p_clk_cfg->clk_freq);
#ifndef SIM
      uint32 result_freq = 0;

      if(DAL_SUCCESS != (res = DalClock_SetClockFrequency(p_handle, p_clk_cfg->spdif_core_clk_id, p_clk_cfg->clk_freq, CLOCK_FREQUENCY_HZ_EXACT, &result_freq)))
      {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_SetClockFrequency fail: freq: %d res: 0x%x",  p_clk_cfg->clk_freq, res);
      }
      else if(result_freq != p_clk_cfg->clk_freq)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_SetClockFrequency fail: freq_expect: %d freq_acrual_set: %d",  p_clk_cfg->clk_freq, result_freq);
      }
      else if(DAL_SUCCESS != (res = DalClock_EnableClock(p_handle, p_clk_cfg->spdif_core_clk_id)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_EnableClock spdif core clock fail: res: 0x%x\n", res);
      }
      else if(DAL_SUCCESS != (res = DalClock_EnableClock(p_handle, p_clk_cfg->spdif_npl_clk_id)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_EnableClock spdif npl clock fail: res: 0x%x\n", res);
      }
    
#else
     res = DAL_SUCCESS;
#endif
   }
   return res;
}

//TODO need to handle clock root.
static DALResult afe_spdif_clk_set_disable(DalDeviceHandle* p_handle, spdif_clk_cfg_t*  p_clk_cfg)
{
   DALResult res = DAL_ERROR;
 
   if(p_clk_cfg)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE_SPDIF: afe_spdif_clk_set_disable: root clk : %ld clk freq: %ld", p_clk_cfg->clk_root, p_clk_cfg->clk_freq);

#ifndef SIM
      if(DAL_SUCCESS != (res = DalClock_DisableClock(p_handle, p_clk_cfg->spdif_core_clk_id)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_DisableClock spdif core clock fail: res: 0x%x\n", res);
      }
      else if(DAL_SUCCESS != (res = DalClock_DisableClock(p_handle, p_clk_cfg->spdif_npl_clk_id)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: DalClock_DisableClock spdif npl clock fail: res: 0x%x\n", res);
      }
#else
     res = DAL_SUCCESS;
#endif
   }
   return res;
}

//TODO need to handle clock root.
ADSPResult afe_spdif_set_clks(uint16 clk_root, uint32 clk_value)
{
   ADSPResult result = ADSP_EFAILED;
   DALResult res = DAL_SUCCESS;
   spdif_clk_cfg_global_info_t *p_spdif_clk_cfg_global_ctx = &spdif_clk_cfg_global_ctx;
   spdif_clk_cfg_t * p_spdif_clk_cfg = &spdif_clk_cfg_global_ctx.spdif_clk_cfg;

   p_spdif_clk_cfg->clk_freq = clk_value;
   p_spdif_clk_cfg->clk_root = clk_root; //TODO need to convert clock regime type. 

   if(0 == p_spdif_clk_cfg_global_ctx->adsppm_info.power_vote_cnt)
   {
      //vote power on
      if(DAL_SUCCESS != (res = HwdMmpm_RequestPwr(p_spdif_clk_cfg_global_ctx->adsppm_info.client_id)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: HwdMmpm_RequestPwr fail \n");
      }
      else
      {
         p_spdif_clk_cfg_global_ctx->adsppm_info.power_vote_cnt = 1;
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"SPDIF_CLK Clk vote for power on powerVoteCnt: %ld",p_spdif_clk_cfg_global_ctx->adsppm_info.power_vote_cnt);
      }
   }

   if(DAL_SUCCESS == res)
   {
      if(p_spdif_clk_cfg->clk_freq)
      {
         if(DAL_SUCCESS == (res = afe_spdif_clk_set_enable(p_spdif_clk_cfg_global_ctx->p_dal_handle, p_spdif_clk_cfg)))
         {
            p_spdif_clk_cfg_global_ctx->clk_status = SPDIF_CLK_ON;
         }

      }
      else
      {
         if(DAL_SUCCESS == (res = afe_spdif_clk_set_disable(p_spdif_clk_cfg_global_ctx->p_dal_handle, p_spdif_clk_cfg)))
         {
            p_spdif_clk_cfg_global_ctx->clk_status = SPDIF_CLK_OFF;
         }
      }
   }

   //check if power vote off needed 

   if(DAL_SUCCESS == res)
   {
      if(SPDIF_CLK_OFF == p_spdif_clk_cfg_global_ctx->clk_status)
      {
         //vote for power off
         if(DAL_SUCCESS != (res = HwdMmpm_ReleasePwr(p_spdif_clk_cfg_global_ctx->adsppm_info.client_id)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF HwdMmpm_ReleasePwr fail\n");
         }

         p_spdif_clk_cfg_global_ctx->adsppm_info.power_vote_cnt = 0;
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"SPDIF_CLK Clk vote for power off powerVoteCnt: %ld", p_spdif_clk_cfg_global_ctx->adsppm_info.power_vote_cnt);
      }  
   }
   
   if(DAL_SUCCESS == res)
   {
      result = ADSP_EOK;
   }
   return result;
}

