/*! \file pm_app_key_press.c
*  
*  \brief Implementation file for KEY PRESS APP APIs.
*    
*  &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2017/09/18 $ 
$Change: 14402555 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

03/21/16   al     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/


#include <Library/BaseLib.h>
#include "TimetickUEFI.h"


#include "pm_pon.h"
#include "pm_dal_prop_ids.h"
#include "pm_target_information.h"
#include "pm_log_utils.h"
#include "pm_app_pon.h"

#define PM_DEVICE_INDEX             0
#define CONVERSION_MILLISEC_TO_SEC  1000

/*This API checks for conditions that are required for checking invalid power key press check*/
static pm_err_flag_type pm_app_pwrkey_check_press_contions(boolean* chk_condtions);

/*This API monitors the power keypress for specified time*/
static pm_err_flag_type pm_app_pwrkey_monitor_press(uint32 dbnc_time_msec, boolean* valid_keypress);


pm_err_flag_type 
pm_app_pwrkey_long_press_check(pm_pon_pwrkey_dbnc_chk_at_type  dbnc_chk_at)
{
  pm_err_flag_type         err_flag = PM_ERR_FLAG_SUCCESS;
  boolean                  chk_condtions = FALSE;
  boolean                  valid_keypress = FALSE;
  pm_pwrkey_dbnc_chk_type  *dbnc_chk = NULL;

  dbnc_chk = (pm_pwrkey_dbnc_chk_type *)pm_target_information_get_specific_info(PM_PROP_LONG_PWRKEY_HOLD_BOOT_TIME);
  if(NULL == dbnc_chk)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /*return if check is not to be done from current image or wait time is 0 millisecond*/
  if((dbnc_chk->chk_at != dbnc_chk_at) || (dbnc_chk->dbnc_time_msec == 0))
  {
    return PM_ERR_FLAG_SUCCESS;
  }

  /*verify if all conditions to check for invalid power keypress are satisfied */
  err_flag = pm_app_pwrkey_check_press_contions(&chk_condtions);
  if(((FALSE == chk_condtions) && (PM_ERR_FLAG_SUCCESS == err_flag)) 
     || (PM_ERR_FLAG_SUCCESS != err_flag ))
  {
      return err_flag;
  }

 /*monitor key press for dbnc_time_msec time*/
  err_flag |= pm_app_pwrkey_monitor_press(dbnc_chk->dbnc_time_msec, &valid_keypress);
  if((FALSE == valid_keypress) && (PM_ERR_FLAG_SUCCESS == err_flag))
  {
    pm_log_message("Invalid pwrkey press. Shutting down! \n\r");
    pm_device_shutdown();
  }

  return err_flag;
}



static pm_err_flag_type 
pm_app_pwrkey_check_press_contions(boolean* chk_condtions)
{
  pm_err_flag_type            err_flag = PM_ERR_FLAG_SUCCESS;
  pm_pon_on_state_reason_type on_reason;
  pm_pon_reason_type          pon_reason;

  if(NULL == chk_condtions)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  /*initialize */
  *chk_condtions = FALSE;

  /*read the on reason and if warm reset then return*/
  err_flag = pm_pon_get_on_reason(PM_DEVICE_INDEX, &on_reason);
  if((on_reason.pon_seq != TRUE) || (err_flag != PM_ERR_FLAG_SUCCESS))
  {
    return err_flag;
  }

  err_flag |= pm_pon_get_pon_reason(PM_DEVICE_INDEX, &pon_reason);
  if(PM_ERR_FLAG_SUCCESS != err_flag)
  {
    return err_flag;
  }
  
  /*if PON reason is key press alone then set the condition TRUE*/
  if((pon_reason.kpdpwr) && ( !( pon_reason.smpl | pon_reason.rtc | pon_reason.dc_chg | 
                               pon_reason.usb_chg | pon_reason.pon1 | pon_reason.cblpwr | 
                               pon_reason.hard_reset))
     )
  {
    *chk_condtions = TRUE;
  }

  return err_flag;
}


static pm_err_flag_type 
pm_app_pwrkey_monitor_press(uint32 dbnc_time_msec, boolean* valid_keypress)
{
  pm_err_flag_type            err_flag = PM_ERR_FLAG_SUCCESS;
  uint64                      elapsed_tick = 0;
  uint32                      timetick_freq = 0;
  uint64                      wait_tick = 0;
  boolean                     key_press_rt_sts = TRUE;

  if((NULL == valid_keypress) || (dbnc_time_msec == 0))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /*initialize with TRUE*/
  *valid_keypress = TRUE;

  /*Initialize Timetick, get current frequency*/
  Timetick_Init();
  Timetick_GetFreq(TIMETICK_QTIMER, &timetick_freq);

   /*convert wait time to tick at current fequency*/
  wait_tick = MultU64x32(dbnc_time_msec, timetick_freq);
  if(wait_tick)
  {
    wait_tick = DivU64x32(wait_tick, CONVERSION_MILLISEC_TO_SEC);
  }
  else
  {
    return err_flag;
  }

  /*Get ticks elapsed since device powered on and compare*/
  (void)Timetick_GetCount(TIMETICK_QTIMER, &elapsed_tick);

  while((elapsed_tick < wait_tick) && (err_flag == PM_ERR_FLAG_SUCCESS))
  {
    pm_log_message("Elapsed tick: 0x%x, Wait tick: 0x%x \n\r", elapsed_tick, wait_tick);
    /*wait 50usec in between reads*/
    pm_busy_wait(50);

    /*check for if key still pressed. If FALSE then read again to confirm*/
    err_flag |= pm_pon_irq_status(PM_DEVICE_INDEX, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_STATUS_RT, &key_press_rt_sts);
    if(key_press_rt_sts == FALSE)
    {
      err_flag |= pm_pon_irq_status(PM_DEVICE_INDEX, PM_PON_IRQ_KPDPWR_ON, PM_IRQ_STATUS_RT, &key_press_rt_sts);
      if(key_press_rt_sts == FALSE)
      {
        *valid_keypress = FALSE;
        break;
      }
    }

    Timetick_GetCount(TIMETICK_QTIMER, &elapsed_tick);
  }

  return err_flag;
}


