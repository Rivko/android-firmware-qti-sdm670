/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/cdc_pwr_ctl/src/AFECdcPwrCtl.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFECdcPwrCtl.cpp

DESCRIPTION: Main Interface to the AFE Codec power control

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/cdc_pwr_ctl/src/AFECdcPwrCtl.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
09-30-2016  mk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "ddislimbus.h"
#include "DALDeviceId.h"
#include "AFESlimbusDriver.h"
#include "AFECodecService.h"
#include "adsp_error_codes.h"
#include "AFECdcPwrCtl.h"

/*=====================================================================
 Globals
 ======================================================================*/
/*=====================================================================
 Static Functions
 ======================================================================*/

/*=====================================================================
 Define Functions
 ======================================================================*/

#define SB_CLK_GEAR_10 10
/* Slimbus clock gear 10 */

#define CDC_PWR_STATE_SVS_REG_VAL 1
/* register value for voting codec power state to SVS */

#define CDC_PWR_STATE_DONT_CARE_REG_VAL 0
/* register value for don't care on codec power state */

/*=====================================================================
 Functions
 ======================================================================*/
void* afe_sb_cdc_clk_cb( void *pBuf1, uint32 dwParam, void *pBuf2, uint32 uSize2 )
{
  SlimBusSchedParamsType *sb_sched_params_ptr = (SlimBusSchedParamsType*)dwParam;
  uint8_t clk_value = 0;
  bool_t is_clk_update_needed = FALSE;

  if((sb_sched_params_ptr->uClkGear >= SB_CLK_GEAR_10) && (sb_sched_params_ptr->uPrevClkGear < SB_CLK_GEAR_10)
      && (SLIMBUS_SCHED_PARAM_BEFORE_UPDATE == sb_sched_params_ptr->UpdateType))
  {
    /* Slimbus is updated to SVS clock gear */
    /* Override codec clock to SVS */

    clk_value = CDC_PWR_STATE_SVS_REG_VAL;
    is_clk_update_needed = TRUE;
  }

  if((sb_sched_params_ptr->uClkGear < SB_CLK_GEAR_10) && (sb_sched_params_ptr->uPrevClkGear >= SB_CLK_GEAR_10)
      && (SLIMBUS_SCHED_PARAM_AFTER_UPDATE == sb_sched_params_ptr->UpdateType))
  {
    /* Slimbus is down to SVS2 clock gear */
    /* vote for SVS2 codec clock  */
    clk_value = CDC_PWR_STATE_DONT_CARE_REG_VAL;
    is_clk_update_needed = TRUE;
  }

  if(TRUE == is_clk_update_needed)
  {
    if(ADSP_EOK != afe_cdc_update_cdc_pwr_state_register(&clk_value))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "failed to update cdc pwr state to %d", clk_value);
    }
  }

  return NULL;
}

/**
 * Configure codec power state control
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state  *
 *
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_cfg_cdc_pwr_state_cntl(void* p_drv)
{
  ADSPResult result = ADSP_EOK;

  DALResult dal_result = DAL_SUCCESS;
  DALSYSEventHandle hClockEvent;

  /* Open the Slimbus device */
  if(ADSP_EOK != (result = afe_slimbus_core_driver_open(p_drv)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
    return ADSP_EFAILED;
  }

  /* create event for clock gear change */
  if(DAL_SUCCESS != (dal_result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &hClockEvent, NULL)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to create clock gear event 0x%x",
          dal_result);
    goto __cdc_pwr_cfg_bail_out;
  }

  /* setup callback for clock gear change */
  if(DAL_SUCCESS != (dal_result = DALSYS_SetupCallbackEvent(hClockEvent, afe_sb_cdc_clk_cb, NULL)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to setup callback for clock gear event 0x%x",
          dal_result);
    goto __cdc_pwr_cfg_bail_out;
  }

  /* register with slimbus driver for call back for codec state change */
  if(DAL_SUCCESS != (dal_result = DalSlimBus_RegisterClockGearEvent((DalDeviceHandle*)afe_sb_get_core_drv_hdl(p_drv), hClockEvent)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to register for clock gear event 0x%x",
          dal_result);
    goto __cdc_pwr_cfg_bail_out;
  }

  __cdc_pwr_cfg_bail_out:

  /* Close the Slimbus device. This will not cause loosing the event callback state as long as the device
   * is not detached */
  afe_slimbus_core_driver_close(p_drv);

  return result;
}



