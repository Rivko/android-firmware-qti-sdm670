/*========================================================================
   This file contains audio clock specific configuration.

Copyright (c) 2015-2018 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_manager.cpp#3 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "Elite.h"
#include "adsp_afe_service_commands.h"
#include "ClockDefs.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "hwd_mmpm.h"
#include "lpasshwio_devcfg.h"
#include "hwd_devcfg.h"
#include "clock_manager_type_i.h"
#include "clock_manager.h"
#include "clock_table_v1_i.h"
#include "hal_clock_i.h"
#include "clock_debug_i.h"
#include "AudioIfClkDriver.h"
#include "afe_lpass_core_clk.h"

/*==========================================================================
  Macro Definitions
========================================================================== */

/* Default M value in M/N.  */
#define AFE_CLOCK_DEFAULT_M_VALUE  0
  
/* Default N value in M/N.  */  
#define AFE_CLOCK_DEFAULT_N_VALUE  0

/* Default D value for any M/N counter */
#define AFE_CLOCK_DEFAULT_D_VALUE  0

/* Default Integer Divider */
#define AFE_CLOCK_DEFAULT_INTEGER_DIVIDER 0

/*==========================================================================
  Function Prototypes
========================================================================== */
// Locate a (dividend,divisor) clock-id pair from a clock-id.
static ADSPResult afe_loc_clkid_pair(uint32_t clkid, uint32_t *dividend_clkid_ptr, uint32_t *divisor_clkid_ptr);

// Retrieve the valid oe-clkid and oe-clkfreq from devcfg and clkid
static ADSPResult afe_loc_oeclkid_and_freq(HwdGenericClkPropertyType *devcfg_ptr,
                                           uint32_t clkid, uint32_t *oeclkid_ptr, uint32_t *oefreq_ptr);

// To enable/disable non-coupled clk with enable count management
static ADSPResult afe_enable_disable_non_coupled_clk_with_count(DalDeviceHandle *handle_ptr,
                                                                generic_clk_cfg_info_t *clk_cfg_ptr, uint32_t enable);

// To enable/disable a couple clock pair based on a pair of clkid. and using enable count management
static ADSPResult afe_enable_disable_coupled_clk_with_count(uint32_t dividend_clkid, uint32_t divisor_clkid, uint32_t clk_attr, uint32_t enable);

// To actually enable/disable clock by calling clock regime driver
static ADSPResult afe_actual_enable_disable_clk(DalDeviceHandle *pHandle,
                                                generic_clk_cfg_info_t *clk_cfg_ptr, uint32_t divider, uint32_t enable);

// Locate the map node from clkidx
static generic_map_tab_info_t* afe_loc_map_clk_node(uint32_t *last_idx_ptr, uint32_t clkid);
// Locate the cfg node from clk id
static generic_clk_cfg_info_t* afe_loc_cfg_clk_node(uint32_t id);

// reconfig for I2S bit clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_SetClockDivider (Optional)
static DALResult afe_reconfig_i2s_bit_clk(DalDeviceHandle *pHandle, uint32_t divider,
                                          generic_clk_cfg_info_t *clk_cfg_ptr);

// Read the configuration for init
static ADSPResult afe_clock_manager_dev_cfg_init(void);


/*==========================================================================
  Globals
========================================================================== */
// Global information to track the clk configuration
generic_clk_cfg_global_info_t afe_generic_clk_cfg_global_ctx;
// Pointer setup to the above info for easier access
generic_clk_cfg_global_info_t *afe_generic_clk_cfg_global_ctx_ptr = &afe_generic_clk_cfg_global_ctx;

/*==========================================================================
  Function implementations
========================================================================== */

bool_t is_clock_mode_external(uint32_t clock_id)
{
   switch (clock_id)
   {
      case AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_SEN_MI2S_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT:
      case AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT:
      {
         return TRUE;
      }
      default:
         return FALSE;
   }
}

ADSPResult afe_clock_manager_set_clk(uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size)
{
   ADSPResult result = ADSP_EOK;
   afe_param_id_clock_set_t *clk_setting_ptr =
                                               (afe_param_id_clock_set_t *)param_buffer_ptr;
   generic_clk_cfg_info_t * cfg_node_ptr,*cfg_oe_node_ptr;
   uint32_t dividend_clkid, divisor_clkid;

   if (CLOCK_MANAGER_INIT != afe_generic_clk_cfg_global_ctx_ptr->state)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_clock_manager_set_clk has not been initialized");
      return ADSP_EUNSUPPORTED;
   }

   switch (param_id)
   {
      case AFE_PARAM_ID_CLOCK_SET:
      {
         /* Minor config version check */
         uint32_t minor_version =  *((uint32_t *)param_buffer_ptr);
         if (minor_version > AFE_API_VERSION_CLOCK_SET)
         {
            minor_version = AFE_API_VERSION_CLOCK_SET;
         }
         if (AFE_API_VERSION_CLOCK_SET != minor_version)
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Wrong version");
            result = ADSP_EUNSUPPORTED;
         }
         else
         {
            /* Validate the size of payload */
            if (param_size < sizeof(afe_param_id_clock_set_t))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d, Param Id: 0x%lx", param_size, param_id);
               return ADSP_EBADPARAM;
            }

            MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                  "Ver: (%ld) Clk_id :(0x%lx) Freq : (0x%lx) Attr: (0x%x) Root: (0x%x) Enable: (0x%lx)",
                  clk_setting_ptr->clk_set_minor_version, clk_setting_ptr->clk_id,
                  clk_setting_ptr->clk_freq_in_hz, clk_setting_ptr->clk_attri,
                  clk_setting_ptr->clk_root, clk_setting_ptr->enable);

            // Validate the clk-id is defined here in the cfg table
            cfg_node_ptr = afe_loc_cfg_clk_node(clk_setting_ptr->clk_id);
            if (NULL == cfg_node_ptr)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dal handle : 0x%x, Unsupported clk id (0x%05lx)",
                     (uint32)afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr, clk_setting_ptr->clk_id);
#ifdef DEBUG_GENERIC_CLK
               afe_print_dev_cfg(afe_generic_clk_cfg_global_ctx_ptr);
               afe_print_map(afe_generic_clk_cfg_global_ctx_ptr);
               afe_print_cfg_nodes(afe_generic_clk_cfg_global_ctx_ptr);
#endif
               return ADSP_EBADPARAM;
            }

            /**<Enable clock request*/
            if (clk_setting_ptr->enable)
            {
               if (0 != cfg_node_ptr->clk_enable_cnt)
               {
                  /**<Since first clock enable request was already received,
                   * just check the settings and return in case of mismatch.*/

                  if (clk_setting_ptr->clk_freq_in_hz != cfg_node_ptr->clk_freq_in_hz)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Attempt to overwrite clk freq");
#ifdef DEBUG_GENERIC_CLK
                     afe_print_cfg_nodes(afe_generic_clk_cfg_global_ctx_ptr);
#endif
                     return ADSP_EBADPARAM;
                  }
                  if (clk_setting_ptr->clk_attri != cfg_node_ptr->clk_attr)
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Attempt to overwrite clk attr");
#ifdef DEBUG_GENERIC_CLK
                     afe_print_cfg_nodes(afe_generic_clk_cfg_global_ctx_ptr);
#endif
                     return ADSP_EBADPARAM;
                  }
               }
               else
               {
                  /**< Bypassing zero frequency check for external clocks only.
                   *
                   * We enable external clock to enable CRC registers, where
                   * we receive frequency as zero. So, we are allowing external
                   * clocks with zero frequency */
                  if ((FALSE == is_clock_mode_external(clk_setting_ptr->clk_id)) &&
                      (0 == clk_setting_ptr->clk_freq_in_hz))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Clock frequency is zero, which is invalid");
                     return ADSP_EBADPARAM;
                  }

                  /**<Set up the frequencies and attributes only for the first enable request.*/
                  cfg_node_ptr->clk_freq_in_hz = clk_setting_ptr->clk_freq_in_hz;
                  cfg_node_ptr->clk_attr = clk_setting_ptr->clk_attri;
               }
            }
            else
            {
              /*Disable clock request*/

              /* If clock disable command comes, before enabling the clock,
               * for graceful handling, need to update the clk_attr.*/
              if(FALSE == cfg_node_ptr->is_clock_enabled)
              {
                cfg_node_ptr->clk_attr = clk_setting_ptr->clk_attri;
              }
            }

            switch (cfg_node_ptr->clk_attr)
            {
               case AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVERT_COUPLE_NO:
               case AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO:
                  result = afe_enable_disable_non_coupled_clk_with_count(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                                                         cfg_node_ptr, clk_setting_ptr->enable);
                  // Look for the corresponding PCM OE node if present and set it
                  cfg_oe_node_ptr = afe_loc_cfg_clk_node(cfg_node_ptr->oe_clkid);
                  if (NULL != cfg_oe_node_ptr)
                  {
                     cfg_oe_node_ptr->clk_freq_in_hz = cfg_node_ptr->oe_clk_freq;
                     cfg_oe_node_ptr->clk_attr = AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_NO;
                     result = afe_enable_disable_non_coupled_clk_with_count(
                              afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                              cfg_oe_node_ptr, clk_setting_ptr->enable);
                  }
                  break;

               case AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVIDEND:
               case AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVISOR:
                  // Input is a clk id, either dividend or divisor
                  // Locate the dividend and divisor paired id
                  if (ADSP_EOK == afe_loc_clkid_pair(
                        clk_setting_ptr->clk_id, &dividend_clkid, &divisor_clkid))
                  {
                     result = afe_enable_disable_coupled_clk_with_count(
                              dividend_clkid, divisor_clkid, cfg_node_ptr->clk_attr, clk_setting_ptr->enable);
                  }
                  else
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported couple clk 0x%lx",
                           clk_setting_ptr->clk_id);
                     result = ADSP_EBADPARAM;
                  }
                  break;
               default:
                  // If enable, we do care attr.  Otherwise, it is ok to ignore
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Bad Attr");
                  result = ADSP_EBADPARAM;
                  break;
            }
#ifdef DEBUG_GENERIC_CLK
            afe_print_cfg_nodes(afe_generic_clk_cfg_global_ctx_ptr);
#endif
         }
      }
         break;
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported Param id ::0x%lx", param_id);
         result = ADSP_EUNSUPPORTED;
      }
         break;
   }
   return result;
}

static ADSPResult afe_enable_disable_non_coupled_clk_with_count(DalDeviceHandle *pHandle,
                                                                generic_clk_cfg_info_t *clk_cfg_ptr, uint32_t enable)
{
   ADSPResult result = ADSP_EOK;

   // Count management before actual enable/disable
   if (enable)
   {
      // Enable call: increment  the count
      clk_cfg_ptr->clk_enable_cnt++;
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "Enable ClkId, incr cnt: ID: 0x%05x enable count: %ld",
            clk_cfg_ptr->clkid, clk_cfg_ptr->clk_enable_cnt);


      /* Call to clock regime driver to actually enable the clock*/
      if (FALSE == clk_cfg_ptr->is_clock_enabled)
      {
         result = afe_actual_enable_disable_clk(pHandle, clk_cfg_ptr, 0, 1);
      }
   }
   else
   {
      // Disable call: decrement the count
      if (clk_cfg_ptr->clk_enable_cnt)
      {
         clk_cfg_ptr->clk_enable_cnt--;
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
               "Disable ClkId, decr cnt: ID: 0x%05x enable count: %ld",
               clk_cfg_ptr->clkid, clk_cfg_ptr->clk_enable_cnt);
      }
      else if (0 == clk_cfg_ptr->clk_enable_cnt)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Disable ClkId cnt is already 0 ClkId 0x%05x", clk_cfg_ptr->clkid);
      }

      /* Call to clock regime driver to actually disable the clock*/
      if ((0 == clk_cfg_ptr->clk_enable_cnt) && (TRUE == clk_cfg_ptr->is_clock_enabled))
      {
         result = afe_actual_enable_disable_clk(pHandle, clk_cfg_ptr, 0, 0);
      }
   }

   return result;
}

ADSPResult afe_enable_disable_coupled_clk_with_count(uint32_t dividend_clkid, uint32_t divisor_clkid, uint32_t clk_attr, uint32_t enable)
{
   generic_clk_cfg_info_t *dividend_node_ptr;
   generic_clk_cfg_info_t *divisor_node_ptr;
   uint32_t divider;
   ADSPResult result =  ADSP_EOK;

   // Locate the dividend and divisor nodes
   dividend_node_ptr = afe_loc_cfg_clk_node(dividend_clkid);
   divisor_node_ptr = afe_loc_cfg_clk_node(divisor_clkid);

   if ((NULL == dividend_node_ptr)
       || (NULL == divisor_node_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot locate node");
      return ADSP_EFAILED;
   }

   /**<request to enable clock received.*/
   if (enable)
   {
      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVIDEND == clk_attr)
      {
         dividend_node_ptr->clk_enable_cnt++;
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Received enable request for DIVIDEND, dividend_clk_enable_count:%d",\
                  dividend_node_ptr->clk_enable_cnt);
      }

      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVISOR == clk_attr)
      {
         divisor_node_ptr->clk_enable_cnt++;
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Received enable request for DIVISOR, divisor_clk_enable_count:%d",\
                  divisor_node_ptr->clk_enable_cnt);
      }

      /* If clock pair is actually enabled,
       * then no need to execute the steps inside this check */
      if ((FALSE == dividend_node_ptr->is_clock_enabled) || (FALSE == divisor_node_ptr->is_clock_enabled))
      {
         /* Actual enable takes place only when, enable requests received for both
          * DIVIDEND (source clk) and DIVISOR (destination clock) clocks.
          * */

         /*<Divisor (destination clk) is not set yet.*/
         if (0 == divisor_node_ptr->clk_enable_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Divisor not set yet");
            return ADSP_EOK;
         }

         /*<Dividend (source clk) is not set yet.*/
         if (0 == dividend_node_ptr->clk_enable_cnt)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Dividend not set yet");
            return ADSP_EOK;
         }

         /*< Validate the division.(A/B) == 0 .*/
         if ((dividend_node_ptr->clk_freq_in_hz < divisor_node_ptr->clk_freq_in_hz))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Set freq error");
            return ADSP_EFAILED;
         }
         divider  = dividend_node_ptr->clk_freq_in_hz / divisor_node_ptr->clk_freq_in_hz;

         /* Above checks ensures that, both enable requests received
          * and it is first actual enable of this clk id pair*/

         if (FALSE == dividend_node_ptr->is_clock_enabled)
         {
            result = afe_actual_enable_disable_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                                   dividend_node_ptr, 0, 1);
         }

         if (FALSE == divisor_node_ptr->is_clock_enabled)
         {
            result |= afe_actual_enable_disable_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                                    divisor_node_ptr, divider, 1);
         }
      }
   }
   else
   {
      /* Disable request received*/

      /* DIVIDEND disable request received, so decrement the dividend coupled clock count */
      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVIDEND == clk_attr)
      {
         if (dividend_node_ptr->clk_enable_cnt)
         {
            dividend_node_ptr->clk_enable_cnt--;
         }

         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Received disable request for DIVIDEND, dividend_clk_enable_count:%d",\
                  dividend_node_ptr->clk_enable_cnt);
      }

      /* DIVISOR disable request received, so decrement the divisor coupled clock count */
      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_COUPLE_DIVISOR == clk_attr)
      {
         if (divisor_node_ptr->clk_enable_cnt)
         {
            divisor_node_ptr->clk_enable_cnt--;
         }

         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Received disable request for DIVISOR, divisor_clk_enable_count:%d",\
                  divisor_node_ptr->clk_enable_cnt);
      }

      /* This is actual clock pair disable.
       * When any one of the clocks enable count reaches zero,
       * then disable both the clocks*/
      if ((0 == dividend_node_ptr->clk_enable_cnt) || (0 == divisor_node_ptr->clk_enable_cnt))
      {
         if (TRUE == dividend_node_ptr->is_clock_enabled)
         {
            result = afe_actual_enable_disable_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                                   dividend_node_ptr, 0, 0);
         }

         if (TRUE == divisor_node_ptr->is_clock_enabled)
         {
            result |= afe_actual_enable_disable_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                                    divisor_node_ptr, 0, 0);
         }
      }
   }

   return result;
}

// Change the sampling rate to unsigned
ADSPResult afe_clock_manager_reconfig_i2s_clk(uint32_t clkid,
                                              uint32_t newSampleRate, uint32_t orgSampleRate)
{
   DALResult res = DAL_SUCCESS;
   uint32_t dividend_clkid;
   uint32_t divisor_clkid;
   uint32_t divider;
   generic_clk_cfg_info_t *ibit_cfg_node_ptr;
   generic_clk_cfg_info_t *osr_cfg_node_ptr;
   ADSPResult result;
   uint32_t multiFactor = 1;
   uint32_t divideFactor = 1;

   if (CLOCK_MANAGER_INIT != afe_generic_clk_cfg_global_ctx_ptr->state)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_clock_manager_reconfig_i2s_clk has not been initialized");
      return ADSP_EUNSUPPORTED;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "AFE SVC: afe_clock_manager_reconfig_I2S_clk: (%ld, %ld)",
         newSampleRate, orgSampleRate);

   // Block the 0 freq case
   if ((0 == newSampleRate)
       || (0 == orgSampleRate))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "zero sampling rate");
      return ADSP_EFAILED;
   }

   ibit_cfg_node_ptr = afe_loc_cfg_clk_node(clkid);
   if (NULL == ibit_cfg_node_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot locate cfg node");
      return ADSP_EFAILED;
   }
   // Calcaulate the factor first based on sampling rate for multply or divide
   // and adjust the frequency
   if (newSampleRate > orgSampleRate)
   {
      multiFactor = newSampleRate / orgSampleRate;
      ibit_cfg_node_ptr->clk_freq_in_hz *= multiFactor;
   }
   else
   {
      divideFactor = orgSampleRate / newSampleRate;
      ibit_cfg_node_ptr->clk_freq_in_hz /= divideFactor;
   }

   // Locate the (dividend, divisor) pair
   result = afe_loc_clkid_pair(clkid, &dividend_clkid, &divisor_clkid);
   // A pair is located
   if (ADSP_EOK == result)
   {
      // Adjust the divider case
      osr_cfg_node_ptr = afe_loc_cfg_clk_node(dividend_clkid);
      if (NULL == osr_cfg_node_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot locate cfg node");
         return ADSP_EFAILED;
      }

      if ((0 == ibit_cfg_node_ptr->clk_freq_in_hz)
          || (osr_cfg_node_ptr->clk_freq_in_hz < ibit_cfg_node_ptr->clk_freq_in_hz))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Divide err");
         return ADSP_EFAILED;
      }

      divider = osr_cfg_node_ptr->clk_freq_in_hz / ibit_cfg_node_ptr->clk_freq_in_hz;

      res = afe_reconfig_i2s_bit_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                     divider, osr_cfg_node_ptr);
   }
   // A pair is not located
   else
   {
      // Adjust the clock directly
      res = afe_reconfig_i2s_bit_clk(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                     0, ibit_cfg_node_ptr);
   }

   return res;
}

ADSPResult afe_clock_manager_deinit(void)
{
   DALResult res;

    afe_lpass_core_clk_deinit();


  /* For 8994 / earlier targets, legacy LPAIF clock config API's are used
   * SupportedClkIdCnt will be zero for targets that uses Legacy clock config APIs */
  if (0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt)
  {
    afe_audioif_clk_deinit();
  }
  else /* 8996, 8953/37 onwards new clock config API's are used */
  {
    // It is better to re-order like init().
    // Release the virtual addr map
    if (0 != (afe_generic_clk_cfg_global_ctx_ptr->tcsr_qmem_region))
    {
      if (ADSP_EOK ==
          HwdDevCfg_DestoryVirtAddress(afe_generic_clk_cfg_global_ctx_ptr->tcsr_qmem_region))
      {
        afe_generic_clk_cfg_global_ctx_ptr->tcsr_qmem_region = 0;
      }
    }

    // Release the dynamic cfg nodes if necessary
    if (NULL != afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr)
    {
      qurt_elite_memory_free(afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr);
      afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr = NULL;
    }

      //Deregister ADSPPM
      if (0 != afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)
      {
         res = HwdMmpm_Deregister(afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid);
         if (DAL_SUCCESS == res)
         {
            afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid = 0;
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: HwdMmpm_Deregister fail (%d)(%d)",
                  afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid, res);
         }
      }
#ifndef SIM
    if (0 != afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr)
    {
      /*dal device detach will release the proxy votes as well which are done as part of
         create proxy*/
      res = DAL_DeviceDetach(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr);
      if (DAL_SUCCESS == res)
      {
        afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr = NULL;
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: Device detach failed");
      }
    }
#endif

    afe_generic_clk_cfg_global_ctx_ptr->state =  CLOCK_MANAGER_DEINIT;
  }

  return ADSP_EOK;
}

ADSPResult afe_clock_manager_dev_cfg_init()
{
   DALSYSPropertyVar prop_var;
   HwdGenericClkPropertyType *pgGnericclkProp = NULL;
   const char *dev_id = "GENERICCLK";

   DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

   /* Get device handle */
   if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Error getting the DAL device handle for clock manager");
      return ADSP_EFAILED;
   }

   /* Read property ptr */
   if (DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl,
                                              "GenericclkPropStructPtr", 0, &prop_var))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Error getting clock manager Property Struct Ptr");
      return ADSP_EFAILED;
   }
   /* Copy the stucture directly */
   pgGnericclkProp = (HwdGenericClkPropertyType *)prop_var.Val.pStruct;
   afe_generic_clk_cfg_global_ctx_ptr->devcfg = *pgGnericclkProp;

   return ADSP_EOK;
}

ADSPResult afe_clock_manager_init(void)
{
   uint32_t i;
   uint32_t CfgTabSizeInBytes;
   ClockIdType clkRegimeId;
   DALResult res;
   generic_clk_cfg_info_t *curr_cfg_node_ptr;
   const generic_map_tab_info_t *curr_map_node_ptr;
   uint32_t last_map_idx;
   uint32_t pcmoe_clkid;
   uint32_t pcmoeFreq;
   ADSPResult result;

   // Clean up the global memory
   memset(afe_generic_clk_cfg_global_ctx_ptr, 0, sizeof(generic_clk_cfg_global_info_t));

   // Retrieve dev cfg info
   if (ADSP_EOK != afe_clock_manager_dev_cfg_init())
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: Cannot get property");
      afe_clock_manager_deinit();
      return ADSP_EFAILED;
   }

   if (ADSP_FAILED(result = afe_lpass_core_clk_init((afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)))
   {
     /* only in unsuppoted case, dont return error */
     if (ADSP_EUNSUPPORTED == result)
     {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPASS core clock not supported or failed");
     }
     else
     {
       return ADSP_EFAILED;
     }
   }


   /* SupportedClkIdCnt will be non-zero only for targets that uses latest clock APIs */
   if( 0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt )
   {
     /* Old clock API*/

      if (ADSP_FAILED(afe_audioif_clk_init((afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)))
      {
         return ADSP_EFAILED;
      }
   }
   else
   {
     /* Latest clock API*/

      // Reterive the virt addr for phy addr (TCSR addr) if necessary
      // If the .tcsrRegBase is 0, this means this dev-cfg don't need
      // additional setup for TCSR address related MUX
      if ((0 != (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegBase) &&
          (ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(
                                 &(afe_generic_clk_cfg_global_ctx_ptr->tcsr_qmem_region),
                                 (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegBase,
                                 (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegSize,
                                 &(afe_generic_clk_cfg_global_ctx_ptr->tcsr_virt_addr)))))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
             "Error obtaining virtual addr for clock mgr tcsr base");
         afe_clock_manager_deinit();
         return ADSP_EFAILED;
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Clock mgr virtual addr 0x%x phy addr 0x%x",
            afe_generic_clk_cfg_global_ctx_ptr->tcsr_virt_addr,
            (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegBase);

      // Register for ADSPPM
      afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid = afe_mmpm_register_lpass_core();

      if (0 == afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: HwdMmpm_Register fail ");
        afe_clock_manager_deinit();
        return ADSP_EFAILED;
      }

#ifndef SIM
      // Attach the device for clock regime driver via DAL
      res = DAL_DeviceAttach(DALDEVICEID_CLOCK,
                             (DalDeviceHandle **)&afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr);
      if (DAL_SUCCESS != res)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: DAL_DeviceAttach fail res: 0x%x", res);
         afe_clock_manager_deinit();
         return ADSP_EFAILED;
      }

      //create DAL device proxy for targets with user PD to avoid crash in PD restart
      if (ADSP_EFAILED == (res = afe_clock_manager_device_proxy(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr)))
      {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: DAL_DeviceCreateProxy fail res: 0x%x", res);
          afe_clock_manager_deinit();
          return ADSP_EFAILED;
      }

#endif

      /* Switchable Clks Map table support */
      if ((LPASS_HW_VER_4_0_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision) ||
          (LPASS_HW_VER_4_0_2 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision) ||
          (LPASS_HW_VER_4_0_3 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision) ||
          (LPASS_HW_VER_4_1_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision) ||
          (LPASS_HW_VER_4_2_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision))
      {
         /* Select v1 table */
         if (LPASS_HW_VER_4_0_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
         {
            afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v1;
            /* Calculate the number of entries in the table */
            afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
               sizeof(g_generic_clk_map_table_v1) / sizeof(generic_map_tab_info_t);
         }
         else if ((LPASS_HW_VER_4_0_2 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision) ||
                  (LPASS_HW_VER_4_1_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision))
         {
            /* Select v3 table */
            afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v3;
            /* Calculate the number of entries in the table */
            afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
               sizeof(g_generic_clk_map_table_v3) / sizeof(generic_map_tab_info_t);
         }
         else if (LPASS_HW_VER_4_2_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
         {
            /* Select v8 table */
            afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v8;
            /* Calculate the number of entries in the table */
            afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
               sizeof(g_generic_clk_map_table_v8) / sizeof(generic_map_tab_info_t);
         }
         else
         {
            /* Select v5 table */
            afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v5;
            /* Calculate the number of entries in the table */
            afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
               sizeof(g_generic_clk_map_table_v5) / sizeof(generic_map_tab_info_t);
         }

         /* Assign the function pointer for clock ID pair */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair =\
            HAL_clock_locate_clkid_pair;

         /* Assign the function pointer for mi2s/pcm mux selection */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel = HAL_clock_mux_sel;
      }
      else if (LPASS_HW_VER_3_10_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
      {
         /* Select v2 table */
         afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v2;

         /* Calculate the number of entries in the table */
         afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
            sizeof(g_generic_clk_map_table_v2) / sizeof(generic_map_tab_info_t);

         /* Clock ID pair function is set to NULL  */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair = NULL;

         /* Mux selection func pointer is set to NULL */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel = NULL;
      }
      else if (LPASS_HW_VER_3_3_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
      {
         /* Select v4 table */
         afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v4;

         /* Calculate the number of entries in the table */
         afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
            sizeof(g_generic_clk_map_table_v4) / sizeof(generic_map_tab_info_t);

         /* Clock ID pair function is set to NULL  */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair = NULL;

         /* Mux selection func pointer is set to NULL */
         afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel = NULL;
      }
      else if (LPASS_HW_VER_3_5_0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
      {
        /* Select v6 table */
        afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v6;

        /* Calculate the number of entries in the table */
        afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
           sizeof(g_generic_clk_map_table_v6) / sizeof(generic_map_tab_info_t);

        /* Clock ID pair function is set to NULL  */
        afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair = NULL;

        /* Mux selection func pointer is set to NULL */
        afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel = NULL;
      }
      else if (LPASS_HW_VER_3_5_1 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
      {
        /* Select v7 table */
        afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr = g_generic_clk_map_table_v7;

        /* Calculate the number of entries in the table */
        afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries =\
           sizeof(g_generic_clk_map_table_v7) / sizeof(generic_map_tab_info_t);

        /* Clock ID pair function is set to NULL  */
        afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair = NULL;

        /* Mux selection func pointer is set to NULL */
        afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel = NULL;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AFE SVC CLOCK: Unknown map table version (0x%lx)",
               (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision);

         /* De-init the clock manager */
         afe_clock_manager_deinit();

         return ADSP_EFAILED;
      }

      // Bail out if number of clk id is 0
      if (0 == (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: 0 ENTRIES");
         afe_clock_manager_deinit();
         return ADSP_EFAILED;
      }

      // Leave an err print if cfg entries are more than map entries
      if ((afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt >
             afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: dev cfg have more entries than map (%ld, %ld)",
               (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt,
               afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries);
      }

      // Setup the cfg nodes from dev config
      afe_generic_clk_cfg_global_ctx_ptr->num_cfg_nodes =\
         (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt;
      CfgTabSizeInBytes =\
         afe_generic_clk_cfg_global_ctx_ptr->num_cfg_nodes * sizeof(generic_clk_cfg_info_t);
      afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr =\
         (generic_clk_cfg_info_t *)qurt_elite_memory_malloc(CfgTabSizeInBytes, QURT_ELITE_HEAP_DEFAULT);

      // Bail out for no memory
      if (NULL == afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC CLOCK: No memory to create cfg nodes");
         afe_clock_manager_deinit();
         return ADSP_ENOMEMORY;
      }

      // Clean the cfg nodes
      memset(afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr, 0, CfgTabSizeInBytes);

      // Populate clkid, OE clkid, OE clk freq, dividend clkid, divisor clkid
      curr_cfg_node_ptr = afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr;
      last_map_idx = 0;

      for (i = 0; i < afe_generic_clk_cfg_global_ctx_ptr->num_cfg_nodes; i++)
      {
         // Copy the clkid to cfg node from dev cfg
         curr_cfg_node_ptr->clkid =\
            (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkId[i];

         // Search the map node to based on clkid
         curr_map_node_ptr = afe_loc_map_clk_node(&last_map_idx, curr_cfg_node_ptr->clkid);
         if (NULL != curr_map_node_ptr)
         {
            // Use a fake id for SIM
#ifdef SIM
            clkRegimeId = (ClockIdType)i;
            res = DAL_SUCCESS;
#else
            res = DalClock_GetClockId(afe_generic_clk_cfg_global_ctx_ptr->dal_handle_ptr,
                                      (const char *)curr_map_node_ptr->clk_name, &clkRegimeId);
#endif

            if (DAL_SUCCESS == res)
            {
               // Init clock regime id and enable, disable function
               curr_cfg_node_ptr->clk_reg_id = clkRegimeId;
               curr_cfg_node_ptr->clk_set_enable_func =\
                  curr_map_node_ptr->clk_set_enable_func;
               curr_cfg_node_ptr->clk_set_disable_func =\
                  curr_map_node_ptr->clk_set_disable_func;
               // Locate the PCM OE CLK info and setup cfg node
               if (ADSP_EOK == afe_loc_oeclkid_and_freq(
                     &(afe_generic_clk_cfg_global_ctx_ptr->devcfg),
                     curr_cfg_node_ptr->clkid, &pcmoe_clkid, &pcmoeFreq))
               {
                  curr_cfg_node_ptr->oe_clkid = pcmoe_clkid;
                  curr_cfg_node_ptr->oe_clk_freq = pcmoeFreq;
               }
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
                     "AFE SVC: DalClock_GetClockId not supported for this clk: Index: 0x%x, res: 0x%x",
                     curr_map_node_ptr->clkid, res);
            }
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE SVC: cannot find dev clkid in map %d",
                  curr_cfg_node_ptr->clkid);
         }
         curr_cfg_node_ptr++;
      }
#ifdef DEBUG_GENERIC_CLK
      afe_print_dev_cfg(afe_generic_clk_cfg_global_ctx_ptr);
      afe_print_map(afe_generic_clk_cfg_global_ctx_ptr);
      afe_print_cfg_nodes(afe_generic_clk_cfg_global_ctx_ptr);
#endif

      afe_generic_clk_cfg_global_ctx_ptr->state = CLOCK_MANAGER_INIT;
   }

   /**<SW workaround to avoid GPIO contention issue between active and idle interfaces,
    * which causes SCLK signal amplitude low and which finally cause audio quality issue.
    *
    * Workaround is that to set all LPASS_AUDIO_CORE_LPAIF_XXX_MODE_MUXSEL default to 1
    * (for EBIT input) during Clock Manager init time */
   if (afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel)
   {
     MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "vote for pwr on");

     /* For few chipsets, LPASS core clock control is moved to LPASS core.
      * To access LCC registers, need to have power request for
      * LPASS Core for these chipsets.  
      *
      * Vote for power ON */
     if (DAL_SUCCESS != (res = HwdMmpm_RequestPwr(
         afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)))
     {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: HwdMmpm_RequestPwr fail ");
       return ADSP_EFAILED;
     }

    HAL_clock_mux_sel_default(afe_generic_clk_cfg_global_ctx_ptr->tcsr_virt_addr);

    /* vote for power OFF */
    if (DAL_SUCCESS != (res = HwdMmpm_ReleasePwr(
         afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)))
     {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC HwdMmpm_ReleasePwr fail");
       return ADSP_EFAILED;
     }

     MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "vote for pwr OFF successful");
   }

   return ADSP_EOK;
}


//Part set function - enable for I2S OSR and PCM clks..
//1. DalClock_SetClockFrequency
//2. Optionally: DalClock_SetClockDivider
//3. DalClock_EnableClock
//NOTE: If divider is 0, no divider will be set
static DALResult afe_clk_set_freq_divider_enable(DalDeviceHandle *pHandle, uint32_t divider,
                                                 generic_clk_cfg_info_t *clk_cfg_ptr)
{
   DALResult res = DAL_SUCCESS;

   if (clk_cfg_ptr)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AFE SVC: afe_clk_set_freq_divider_enable: clkid: 0x%lx ClkRegId: 0x%lx clk_freq_in_hz: %ld, divider: %ld clk_enable_cnt: %ld",
            clk_cfg_ptr->clkid, clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz,
            divider, clk_cfg_ptr->clk_enable_cnt);

#ifdef SIM
      res = DAL_SUCCESS;
#else
      uint32_t resultFreq;

      if (DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, clk_cfg_ptr->clk_reg_id,
                                                           clk_cfg_ptr->clk_freq_in_hz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockFrequency fail: Clk Reg Id: %d freq: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, res);
      }
      else if (resultFreq != clk_cfg_ptr->clk_freq_in_hz)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockFrequency fail: Clk Reg Id: %d freq_expect: %d freq_acrual_set: %d", clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, resultFreq);
      }
      else if ((0 != divider) && (DAL_SUCCESS != (res = DalClock_SetClockDivider(pHandle, clk_cfg_ptr->clk_reg_id, divider))))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockDivider fail: Clk Reg Id: %d divider: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, divider, res);
      }
      else if (DAL_SUCCESS != (res = DalClock_EnableClock(pHandle, clk_cfg_ptr->clk_reg_id)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_EnableClock fail: Clk Reg Id: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, res);
      }
#endif
   }

   return res;
}

//Part set function - enable for all external clks, include
//1. DalClock_EnableClock
//NOTE : divider is ignored here
static DALResult afe_clk_set_enable(DalDeviceHandle *pHandle, uint32_t divider,
                                    generic_clk_cfg_info_t *clk_cfg_ptr)
{
   DALResult res = DAL_ERROR;

   if (clk_cfg_ptr)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AFE SVC: afe_clk_set_enable: clkid: 0x%lx ClkRegId: 0x%lx clk_freq_in_hz: %ld clk_enable_cnt: %ld",
            clk_cfg_ptr->clkid, clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, clk_cfg_ptr->clk_enable_cnt);

#ifdef SIM
      res = DAL_SUCCESS;
#else
      /*This function is used to set the voltage corner for EBIT clocks. For internal clocks, clck regime sets the */
      /*voltage corner as part of enabling clock. But for external clocks, need to call this API explicitly to set */
      /*the voltage corner. Ignore the error returned by this function. This is only implementing from 8998 onwards,*/
      /* so for chips where this function not implememted error will return by clock regime driver*/

      /*we can compare with  HW version for 8998, but for every latest chip need to add version here.*/
      /*Ignore the error is least code changes. Once corebsp comes up with capability API we can go with that.*/
      if (DAL_SUCCESS != (res = DalClock_SelectExternalSource(pHandle, clk_cfg_ptr->clk_reg_id,clk_cfg_ptr->clk_freq_in_hz, \
                                                              AFE_CLOCK_SET_CLOCK_ROOT_DEFAULT, AFE_CLOCK_DEFAULT_INTEGER_DIVIDER,\
                                                              AFE_CLOCK_DEFAULT_M_VALUE,AFE_CLOCK_DEFAULT_N_VALUE,  \
                                                              AFE_CLOCK_DEFAULT_D_VALUE)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE SVC: DalClock_SelectExternalSource unsupported: clk_reg_id: %d res: 0x%x",
               clk_cfg_ptr->clk_reg_id, res);
      } 
      
      if (DAL_SUCCESS != (res = DalClock_EnableClock(pHandle, clk_cfg_ptr->clk_reg_id)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_EnableClock fail: clk_reg_id: %d res: 0x%x",
               clk_cfg_ptr->clk_reg_id, res);
      }
#endif
   }

   return res;
}

//Part set function disable for all clks, include
//1. DalClock_DisableClock
//NOTE: divider is ignored.
static DALResult afe_clk_set_disable(DalDeviceHandle *pHandle, uint32_t divider,
                                     generic_clk_cfg_info_t *clk_cfg_ptr)
{
   DALResult res = DAL_ERROR;

   if (clk_cfg_ptr)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AFE SVC: afe_clk_set_disable: clkid: 0x%lx ClkRegId: 0x%lx clk_freq_in_hz: %ld clk_enable_cnt: %ld",
            clk_cfg_ptr->clkid, clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, clk_cfg_ptr->clk_enable_cnt);

#ifdef SIM
      res = DAL_SUCCESS;
#else
      if (DAL_SUCCESS != (res = DalClock_DisableClock(pHandle, clk_cfg_ptr->clk_reg_id)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_DisableClock fail: ClkId: 0x%lx res: 0x%x",
               clk_cfg_ptr->clk_reg_id, res);
      }
#endif
   }
   return res;
}

static generic_map_tab_info_t* afe_loc_map_clk_node(uint32_t *last_idx_ptr, uint32_t clkid)
{
   uint32_t i;
   generic_map_tab_info_t *map_node_ptr;

   // Split loop search based on last search point
   // Add one to last search point and wrap around
   (*last_idx_ptr)++;
   if (*last_idx_ptr >= afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries)
   {
      *last_idx_ptr = 0;
   }

   // Search for second half idx...(num_map_table_entries-1)
   map_node_ptr = (generic_map_tab_info_t *)
                  afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr;
   map_node_ptr += (*last_idx_ptr);
   for (i = *last_idx_ptr;
        i < afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries; i++)
   {
      if (clkid == map_node_ptr->clkid)
      {
         *last_idx_ptr = i;
         return map_node_ptr;
      }
      map_node_ptr++;
   }

   // Search for first half 0..idx-1
   map_node_ptr = (generic_map_tab_info_t *)
                  afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr;
   for (i = 0; i < *last_idx_ptr; i++)
   {
      if (clkid == map_node_ptr->clkid)
      {
         *last_idx_ptr = i;
         return map_node_ptr;
      }
      map_node_ptr++;
   }

   *last_idx_ptr = 0;
   return NULL;
}

static generic_clk_cfg_info_t* afe_loc_cfg_clk_node(uint32_t id)
{
   uint32_t i;
   generic_clk_cfg_info_t *cfg_node_ptr;
   cfg_node_ptr = afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr;

   for (i = 0; i < afe_generic_clk_cfg_global_ctx_ptr->num_cfg_nodes; i++)
   {
      if (id == cfg_node_ptr->clkid)
      {
         return cfg_node_ptr;
      }
      cfg_node_ptr++;
   }
   return NULL;
}

static ADSPResult afe_actual_enable_disable_clk(DalDeviceHandle *pHandle,
                                                generic_clk_cfg_info_t *clk_cfg_ptr, uint32_t divider, uint32_t enable)
{
   DALResult res;
   ADSPResult result = ADSP_EOK;

   // The following section will handle the count from 0->1 or 1->0
   // This will call clock regime driver via
   // clk_set_enable_func or clk_set_disable_func
   if (enable)
   {
      // Vote for pwr if necessary, otherwise just increase count
      if (0 == afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt)
      {
        MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "vote for pwr on");

        //vote power on and set vote cnt if success
        if (DAL_SUCCESS != (res = HwdMmpm_RequestPwr(
            afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: HwdMmpm_RequestPwr fail ");
          return ADSP_EFAILED;
        }
        afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt = 1;
      }
      else
      {
        afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt++;
      }
      
      /* For few chipsets, LPASS core clock control is moved to LPASS core.
       * To access LCC registers, need to have power request for
       * LPASS Core for these chipsets. */

      // Enable the HAL dependent MUX if necessary
      if (NULL != afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel)
      {
        afe_generic_clk_cfg_global_ctx_ptr->p_func_mux_sel
        (clk_cfg_ptr->clkid, afe_generic_clk_cfg_global_ctx_ptr->tcsr_virt_addr);
      }

#ifdef DEBUG_DUMP_MUX_CLK
      if (LPASS_HW_VER_4_0_0 ==
          (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision)
      {
        HAL_clock_dump_mux(afe_generic_clk_cfg_global_ctx_ptr->tcsr_virt_addr);
      }
#endif
      
      // Invert the clock as necessary for enable clk
      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVERT_COUPLE_NO == clk_cfg_ptr->clk_attr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE SVC: AFE SVC: Set the Clock invert state to invert: clk_reg_id: %d", clk_cfg_ptr->clk_reg_id);

#ifdef SIM
         res = DAL_SUCCESS;
#else
         res = DalClock_InvertClock(pHandle, clk_cfg_ptr->clk_reg_id,  true);
#endif

         if (DAL_SUCCESS != res)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_InvertClock fail: clk_reg_id: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, res);
            return ADSP_EFAILED;
         }
      }

      // Call the enable function to enable clk
      // Set is_clock_enabled to CLK_ENABLE, if successful
      if (DAL_SUCCESS == (res = clk_cfg_ptr->clk_set_enable_func(pHandle,
                                                                 divider, clk_cfg_ptr)))
      {
         clk_cfg_ptr->is_clock_enabled = TRUE;
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Enable clock fail");
         result = ADSP_EBADPARAM;
      }
   }
   else
   {
      uint32_t temp_clk_attr  = clk_cfg_ptr->clk_attr;

      // Call the disable function to disable clk
      // Set is_clock_enabled to CLK_DISABLE
      if (DAL_SUCCESS == (res = clk_cfg_ptr->clk_set_disable_func(pHandle,
                                                                  divider, clk_cfg_ptr)))
      {
         clk_cfg_ptr->is_clock_enabled = FALSE;
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Disable clock fail");
         result = ADSP_EFAILED;
      }

      // Invert the clock back to normal for disable clk
      if (AFE_CLOCK_SET_CLOCK_ATTRIBUTE_INVERT_COUPLE_NO == temp_clk_attr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE SVC: Set the Clock invert state to normal: clk_reg_id: %d", clk_cfg_ptr->clk_reg_id);

#ifdef SIM
         res = DAL_SUCCESS;
#else
         res = DalClock_InvertClock(pHandle, clk_cfg_ptr->clk_reg_id,  false);
#endif

         if (DAL_SUCCESS != res)
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_InvertClock() fail: clk_reg_id: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, res);
            return ADSP_EFAILED;
         }
      }

      // Vote for pwr off if necessary, otherwise just decrease count
      if (1 == afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt)
      {
        if (DAL_SUCCESS != (res = HwdMmpm_ReleasePwr(
            afe_generic_clk_cfg_global_ctx_ptr->adsp_pm_clientid)))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC HwdMmpm_ReleasePwr fail");
          return ADSP_EFAILED;
        }
        afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt = 0;
      }
      else if (0 != afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt)
      {
        afe_generic_clk_cfg_global_ctx_ptr->adsp_pwr_vote_cnt--;
      }
   }

   return result;
}

static ADSPResult afe_loc_clkid_pair(uint32_t clkid,
                                     uint32_t *dividend_clkid_ptr, uint32_t *divisor_clkid_ptr)
{
   if (NULL == afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair)
      return ADSP_EFAILED;

   return (afe_generic_clk_cfg_global_ctx_ptr->p_func_loc_clkid_pair(
         clkid, dividend_clkid_ptr, divisor_clkid_ptr));
}

static ADSPResult afe_loc_oeclkid_and_freq(HwdGenericClkPropertyType *devcfg_ptr,
                                           uint32_t ClkId, uint32_t *oe_clkid_ptr, uint32_t *oe_freq_ptr)
{
   uint32_t i;

   i = 0;
   for (i = 0; i < devcfg_ptr->infPcmOeCnt; i++)
   {
      if (ClkId == devcfg_ptr->infPcmOeSupportedClkId[i])
      {
         *oe_clkid_ptr = devcfg_ptr->infPcmOeClkIdToUse[i];
         *oe_freq_ptr = devcfg_ptr->infPcmOeClkFreq[i];
         return ADSP_EOK;
      }
   }
   return ADSP_EFAILED;
}

// reconfig for I2S bit clks, include
//1. DalClock_SetClockFrequency
//2. Optionally: DalClock_SetClockDivider
static DALResult afe_reconfig_i2s_bit_clk(DalDeviceHandle *pHandle, uint32_t divider,
                                          generic_clk_cfg_info_t *clk_cfg_ptr)
{
   DALResult res = DAL_ERROR;

   if (clk_cfg_ptr)
   {
      MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AFE SVC: afe_reconfig_i2s_bit_clk: clkid: 0x%lx ClkRegId: 0x%lx clk_freq_in_hz: %ld divider: %ld clk_enable_cnt: %ld",
            clk_cfg_ptr->clkid, clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, divider,
            clk_cfg_ptr->clk_enable_cnt);

#ifdef SIM
      res = DAL_SUCCESS;
#else
      uint32_t resultFreq = 0;

      if (DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, clk_cfg_ptr->clk_reg_id,
                                                           clk_cfg_ptr->clk_freq_in_hz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockFrequency fail: Clk Reg Id: %d freq: %d res: 0x%x",
               clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, res);
      }
      else if (resultFreq != clk_cfg_ptr->clk_freq_in_hz)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockFrequency fail: Clk Reg Id: %d freq_expect: %d freq_acrual_set: %d",
               clk_cfg_ptr->clk_reg_id, clk_cfg_ptr->clk_freq_in_hz, resultFreq);
      }
      else if ((0 != divider) && (DAL_SUCCESS != (res = DalClock_SetClockDivider(pHandle,
                                                                                 clk_cfg_ptr->clk_reg_id, divider))))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE SVC: DalClock_SetClockDivider fail: Clk Reg Id: %d divider: %d res: 0x%x", clk_cfg_ptr->clk_reg_id, divider, res);
      }
#endif
   }
   return res;
}

