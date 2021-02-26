/** @file afe_main.cpp

 @brief This file contains AFE Service main init and deinit functions

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/utils/misc/src/afe_main.cpp#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 11/19/2014 rv   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "afe_main.h"

#include "afe_lpm.h"
#include "avtimer.h"
#include "hwd_devcfg.h"
#include "rs_driver_ext.h"


ADSPResult afe_main_init()
{
  ADSPResult result = ADSP_EOK;

  if (ADSP_FAILED(result = HwdDevCfg_Init()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init hwdDecCfg Driver with status %d", result);
     return ADSP_EFAILED;
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HwdDevCfg is initialized");

  if (ADSP_FAILED(result = lpm_init()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init LPM with status %d", result);
     return ADSP_EFAILED;
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LPM is initialized");

  if (ADSP_FAILED(result = avtimer_init()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init AVTimer with status %d", result);
     return ADSP_EFAILED;
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVTimer is initialized");

  if (ADSP_FAILED(result = rs_drv_init()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init rs_drv with status %d", result);
     return ADSP_EFAILED;
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "rs_drv is initialized");

  return result;
}


ADSPResult afe_main_deinit()
{
  ADSPResult result = ADSP_EOK;

  if (ADSP_FAILED(result = lpm_deinit()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to deinit LPM with status %d", result);
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LPM is successfully deinitialized");
  }

  if (ADSP_FAILED(result = avtimer_deinit()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to deinit avtimer with status %d", result);
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "avtimer is successfully deinitialized");
  }

  if (ADSP_FAILED(result = rs_drv_deinit()))
  {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to deinit rs_drv with status %d", result);
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "rs_drv is successfully deinitialized");
  }

  return ADSP_EOK;
}
