#ifndef __PM_PON_H__
#define __PM_PON_H__

/*! \file pm_pon.h
 *  \n
 *  \brief  This file contains public API interface to support PowerON driver.
 *  \n
 *  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tz/2.0.2/api/pmic/pm/pm_pon.h#1 $
  $DateTime: 2018/02/06 02:57:17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
05/11/15   ps      Added pm_pon_reset_source_type 
06/24/13   ps      Added PON Stage3 reset config API CR#495834
04/10/13   kt      Added PMIC Watchdog config support
02/27/13   kt      Added PON IRQ related APIs
12/06/12   hw      Remove comdef.h and use com_dtypes.h
22/11/12   umr     Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_lib_err.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  PM_PON_RESET_CFG_WARM_RESET,
  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  /* Shutdown to a state of main battery removal */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
  /* Shutdown to a state of main battery and coin cell removal*/
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_HARD_RESET,
  /* Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_D_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_INVALID
}pm_pon_reset_cfg_type;


/*! \enum pm_pon_reset_source_type
   \brief Reset source type.
   DO NOT CHANGE THE SEQUENCE OF TOP 4 enums
 */
typedef enum
{
  PM_PON_RESET_SOURCE_KPDPWR,           /* Power key */
  PM_PON_RESET_SOURCE_RESIN,            /* Resin in form MSM */
  PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, /* Pwr Key + Resin */
  PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR,  /* Pwr Key or Resin */
  PM_PON_RESET_SOURCE_GP1,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_GP2,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_INVALID  
}pm_pon_reset_source_type;


/*! \enum pm_app_pon_reset_cfg_type
   \brief PS_HOLD reset configure type.
  */
typedef enum
{
  PM_APP_PON_CFG_WARM_RESET,
  PM_APP_PON_CFG_HARD_RESET,
  PM_APP_PON_CFG_NORMAL_SHUTDOWN,
  PM_APP_PON_CFG_DVDD_HARD_RESET,
  PM_APP_PON_CFG_DVDD_SHUTDOWN,
  PM_APP_PON_CFG_MAX
}pm_app_pon_reset_cfg_type;


typedef enum
{
  PM_APP_PON_RESET_SOURCE_KPDPWR,              /* Power key */
  PM_APP_PON_RESET_SOURCE_RESIN,               /* Resin in form MSM */
  PM_APP_PON_RESET_SOURCE_RESIN_AND_KPDPWR,    /* Key Combo ... RESIN AND Vol Key use the same pin*/
  PM_APP_PON_RESET_SOURCE_RESIN_OR_KPDPWR,     /* Key Combo ... RESIN OR Vol Key use the same pin*/
  PM_APP_PON_RESET_SOURCE_MAX
}pm_app_pon_reset_source_type;

/*===========================================================================

                        HEADER FILES

===========================================================================*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *
 * @details Configure PMIC to act on MSM PS_HOLD state
 *
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *
 *
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_ps_hold_cfg(uint32 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg);


/**
 * @brief Configures PMIC to act on MSM PS_HOLD toggle. This
 *        is an app level API which handles all the required
 *        PS_HOLD config for all the applicable PMICs
 *        internally.
 *
 * @details Configure PMIC to act on MSM PS_HOLD state.
 *
 * @param ps_hold_cfg
 *                               PM_APP_PON_CFG_WARM_RESET,
 *                               PM_APP_PON_CFG_HARD_RESET,
 *                               PM_APP_PON_CFG_NORMAL_SHUTDOWN*
 *
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS = SUCCESS else
 *         ERROR.
 */
pm_err_flag_type
pm_app_ps_hold_cfg(pm_app_pon_reset_cfg_type ps_hold_cfg);

#endif // PM_PON__H
