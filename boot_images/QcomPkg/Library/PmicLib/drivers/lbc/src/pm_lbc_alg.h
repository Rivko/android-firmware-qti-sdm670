#ifndef __PM_LBC_API_ALG_H__
#define __PM_LBC_API_ALG_H__

/*! \file
 *  \n
 *  \brief  pm_lbc_api_alg.h
 *  \details  
 *  \n &copy;  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/14   sv     pm_lbc_irq_status updated to read correct peripheral irq status.(CR-656724)
02/13/14   ps      File created

========================================================================== */


#include "pm_uefi_lbc.h"
#include "pmic_device.h"

pm_err_flag_type pm_lbc_set_mode_alg(pmiC_PmicResource *rsrc,  pm_lbc_mode_type mode);
pm_err_flag_type pm_lbc_get_battery_present_alg(pmiC_PmicResource *rsrc,  boolean *present);
pm_err_flag_type pm_lbc_enable_wdog_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_wdog_alg(pmiC_PmicResource *rsrc,  uint32 delay_seconds, uint32 timeout_seconds);
pm_err_flag_type pm_lbc_pet_chg_wdog_alg(pmiC_PmicResource *rsrc);
pm_err_flag_type pm_lbc_enable_ttrkl_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_ttrkl_alg(pmiC_PmicResource *rsrc,  uint8 value_minutes);
pm_err_flag_type pm_lbc_set_vtrkl_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_set_itrickle_alg(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
pm_err_flag_type pm_lbc_set_itrickle_switch_mode_alg(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
pm_err_flag_type pm_lbc_set_vdd_safe_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_set_vdd_max_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_set_vin_min_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_set_temp_threshold_alg(pmiC_PmicResource *rsrc,  uint32 stop_deg_c, uint32 resume_deg_c);
pm_err_flag_type pm_lbc_set_ibat_max_alg(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
pm_err_flag_type pm_lbc_set_ibat_safe_alg(pmiC_PmicResource *rsrc,  uint32 in_milliampere);
//pm_err_flag_type pm_lbc_set_vbat_det_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_set_vbat_weak_thr_alg(pmiC_PmicResource *rsrc,  uint32 in_millivolt);
pm_err_flag_type pm_lbc_get_vbat_weak_thr_alg(pmiC_PmicResource *rsrc,  uint32 *value_mv);
pm_err_flag_type pm_lbc_get_battery_weak_alg(pmiC_PmicResource *rsrc,  boolean *weak);
pm_err_flag_type pm_lbc_get_vbat_status_alg(pmiC_PmicResource *rsrc,  pm_lbc_vbat_status_type vbat_status, boolean *status);
pm_err_flag_type pm_lbc_get_attached_charger_alg(pmiC_PmicResource *rsrc,  pm_lbc_attached_chgr_type *type);
pm_err_flag_type pm_lbc_set_tchg_alg(pmiC_PmicResource *rsrc,  uint32 value_minutes);
pm_err_flag_type pm_lbc_enable_tchg_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_iterm_chgr_alg(pmiC_PmicResource *rsrc,  pm_lbc_ieoc_method_type method, boolean enable, uint32 in_milliampere);
pm_err_flag_type pm_lbc_get_usb_ovp_ctl_alg(pmiC_PmicResource *rsrc,  pm_lbc_ovp_thr_type *ovp_thr, pm_lbc_uvd_thr_type *uvd_thr, pm_lbc_valid_db_type *valid_db);
pm_err_flag_type pm_lbc_set_usb_ovp_ctl_alg(pmiC_PmicResource *rsrc,  pm_lbc_ovp_thr_type ovp_thr, pm_lbc_uvd_thr_type uvd_thr, pm_lbc_valid_db_type valid_db);
pm_err_flag_type pm_lbc_enable_low_power_mode_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_enable_boot_timer_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_adaptive_boot_type_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_boot_timer_alg(pmiC_PmicResource *rsrc,  pm_lbc_boot_timer_type boot_timer);
pm_err_flag_type pm_lbc_get_vbat_boot_thres_alg(pmiC_PmicResource *rsrc,  uint32 *in_millivolt);
pm_err_flag_type pm_lbc_get_batt_therm_presence_alg(pmiC_PmicResource *rsrc,  boolean *present);
pm_err_flag_type pm_lbc_get_batt_id_presence_alg(pmiC_PmicResource *rsrc,  boolean *present);
pm_err_flag_type pm_lbc_get_batt_temp_status_alg(pmiC_PmicResource *rsrc,  pm_lbc_bat_temp_status_type *temp_status);
pm_err_flag_type pm_lbc_is_vref_bat_thm_on_alg(pmiC_PmicResource *rsrc,  boolean *status);
pm_err_flag_type pm_lbc_is_vref_bat_thm_source_avdd_alg(pmiC_PmicResource *rsrc,  boolean *status);
pm_err_flag_type pm_lbc_is_vref_bat_thm_source_vreg_xoadc_alg(pmiC_PmicResource *rsrc,  boolean *status);
pm_err_flag_type pm_lbc_enable_bpd_bat_thm_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_enable_bpd_bat_id_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_set_bat_temp_comp_alg(pmiC_PmicResource *rsrc,  boolean enable_btc, pm_lbc_batt_temp_cold_thr_type cold_thd, pm_lbc_batt_temp_hot_thr_type hot_thd);
pm_err_flag_type pm_lbc_set_vref_bat_thm_ctrl_alg(pmiC_PmicResource *rsrc,  pm_lbc_vref_bat_thm_enable_type enable_type);
pm_err_flag_type pm_lbc_get_vref_bat_thm_ctrl_alg(pmiC_PmicResource *rsrc,  pm_lbc_vref_bat_thm_enable_type *enable_type);
pm_err_flag_type pm_lbc_enable_vbat_dischg_alg(pmiC_PmicResource *rsrc,  boolean enable);
pm_err_flag_type pm_lbc_is_battery_removed_alg(pmiC_PmicResource *rsrc,  boolean *battery_removed);
pm_err_flag_type pm_lbc_is_batt_trckl_charged_alg(pmiC_PmicResource *rsrc,  boolean *trickle_charged);
pm_err_flag_type pm_lbc_irq_enable_alg(pmiC_PmicResource *rsrc, pm_lbc_irq_type irq, boolean enable);
pm_err_flag_type pm_lbc_irq_clear_alg (pmiC_PmicResource *rsrc, pm_lbc_irq_type irq);
pm_err_flag_type pm_lbc_irq_set_trigger_alg(pmiC_PmicResource *rsrc, pm_lbc_irq_type irq, pm_irq_trigger_type trigger);
pm_err_flag_type pm_lbc_irq_status_alg(pmiC_PmicResource *rsrc, unsigned pheripheral_index, pm_lbc_irq_type irq, pm_irq_status_type type, boolean *status);

#endif // __PM_LBC_API_ALG_H__
