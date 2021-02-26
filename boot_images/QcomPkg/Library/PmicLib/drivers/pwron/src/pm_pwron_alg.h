#ifndef __PM_PWRON_API_ALG_H__
#define __PM_PWRON_API_ALG_H__

/*! \file
 *  \n
 *  \brief  pm_pwron_alg.h 
 *  \details  
 *  \n &copy;  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/14   sm      Added API to get PBL_PON Status
02/20/14   al      Adding watchdog APIs 
11/22/13   va      PmicLib Dec Addition
08/01/13   rh      File created
========================================================================== */

#include "pm_uefi_pwron.h"
#include "pmic_device.h"

pm_err_flag_type pm_pwron_power_key_setup_alg                       (pmiC_PmicResource *rsrc, pm_pwr_key_delay_type  delay, pm_switch_cmd_type     pull_up_en, uint8  perph_index);
pm_err_flag_type pm_pwron_set_pwr_key_delay_alg                     (pmiC_PmicResource *rsrc, uint32  microseconds, uint8  perph_index);
pm_err_flag_type pm_pwron_get_power_on_status_alg                   (pmiC_PmicResource *rsrc, uint32*  pwr_on_status, uint8  perph_index);
pm_err_flag_type pm_pwron_set_pwr_key_pullup_alg                    (pmiC_PmicResource *rsrc, pm_switch_cmd_type  pull_up_en, uint8  perph_index);
pm_err_flag_type pm_pwron_clear_power_on_status_alg                 (pmiC_PmicResource *rsrc, uint8  perph_index);
pm_err_flag_type pm_pwron_watchdog_reset_detect_switch_alg          (pmiC_PmicResource *rsrc, pm_switch_cmd_type  OnOff, uint8  perph_index);
pm_err_flag_type pm_pwron_cbl_pwr_on_switch_alg                     (pmiC_PmicResource *rsrc, pm_cbl_pwr_on_switch_type  cmd, uint8  perph_index);
pm_err_flag_type pm_pwron_cbl_pwr_on_pullup_switch_alg              (pmiC_PmicResource *rsrc, pm_switch_cmd_type       OnOff, pm_cbl_pwr_on_pin_type   cable, uint8  perph_index);
pm_err_flag_type pm_pwron_start_up_abort_timer_switch_alg           (pmiC_PmicResource *rsrc, pm_switch_cmd_type  OnOff, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_soft_reset_alg                      (pmiC_PmicResource *rsrc, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_hard_reset_enable_alg               (pmiC_PmicResource *rsrc, boolean  enable, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_hard_reset_delay_timer_set_alg      (pmiC_PmicResource *rsrc, int  delay_timer_ms, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_hard_reset_debounce_timer_set_alg   (pmiC_PmicResource *rsrc, int  debounce_timer_ms, uint8  perph_index);
pm_err_flag_type pm_pwron_get_wdog_status_alg                       (pmiC_PmicResource *rsrc, boolean  *dog_status, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_stay_on_enable_alg                  (pmiC_PmicResource *rsrc, boolean  enable, uint8  perph_index);
pm_err_flag_type pm_pwron_pwron_signal_reset_with_action_alg        (pmiC_PmicResource *rsrc, pm_power_reset_signal  signal, pm_power_reset_action_type  action, uint8  perph_index);
pm_err_flag_type pm_pwron_get_pwron_trigger_alg                     (pmiC_PmicResource *rsrc, uint8 *pon_trigger , uint8 perph_index);
pm_err_flag_type pm_pwron_get_pon_pbl_status_alg                    (pmiC_PmicResource *rsrc, pm_pwron_pon_pbl_status_type pbl_status_type, boolean  *status, uint8  perph_index);
pm_err_flag_type pm_pwron_get_on_off_reset_reasons_alg              (pmiC_PmicResource *rsrc, pm_pwron_reasons_type reason, uint8 *triggers, uint8  perph_index);
pm_err_flag_type pm_pwron_did_device_warm_reset_alg                 (pmiC_PmicResource *rsrc, uint8  perph_index, boolean  *device_warm_reset);
pm_err_flag_type pm_pwron_config_pon_trigger_alg                    (pmiC_PmicResource *rsrc, pm_pwron_trigger_type trigger, boolean enable, uint8  perph_index);

pm_err_flag_type pm_pwron_wdog_cfg_alg                             (pmiC_PmicResource *rsrc,uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type);
pm_err_flag_type pm_pwron_wdog_enable_alg                          (pmiC_PmicResource *rsrc, pm_on_off_type enable);
pm_err_flag_type pm_pwron_wdog_pet_alg                             (pmiC_PmicResource *rsrc);

#endif // __PM_PWRON_API_ALG_H__
