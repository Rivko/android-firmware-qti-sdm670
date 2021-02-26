/*! \file 
*  \n
*  \brief  pm_pon_wdog.c PMIC PowerON Peripheral driver
*  \n
*  \n This header file contains class implementation of the PON
*  \n peripheral PMIC Watchdog APIs
*  \n
*  \n &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
11/01/13   sv      Applying CR538994 Changes with power up failure fix. (CR-570053)
10/24/13   sv      Reverting CR538994 Changes (CR-566323)
04/10/13   kt      Created
========================================================================== */

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pm_pon.h"
#include "pmio_pon.h"
#include "hw_module_type.h"
#include "device_info.h"
#include "DALSys.h"
#include "pm_comm.h"

/* PMIC Device Index IDs */
#define PMIC_A_DEVICE_ID   0
#define PMIC_B_DEVICE_ID   1

/* PMIC SlaveIDs */
#define PMIC_A_SLAVEID_PRIM                    0x00

#define PON_RESET_CFG_WARM_RESET_VAL           0x01
#define PON_RESET_CFG_IMMEDIATE_XVDD_SHUTDOWN_VAL         0x02
#define PON_RESET_CFG_NORMAL_SHUTDOWN_VAL      0x04
#define PON_RESET_CFG_D_VDD_SHUTDOWN_VAL       0x05
#define PON_RESET_CFG_X_VDD_SHUTDOWN_VAL       0x06
#define PON_RESET_CFG_HARD_RESET_VAL           0x07

#define PON_RESET_CFG_DVDD_HARD_RESET_VAL                 0x08 
#define PON_RESET_CFG_XVDD_HARD_RESET_VAL                 0x09 
#define PON_RESET_CFG_WARM_RESET_THEN_DVDD_SHUTDOWN_VAL   0x0A 
#define PON_RESET_CFG_WARM_RESET_THEN_XVDD_SHUTDOWN_VAL   0x0B 
#define PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN_VAL        0x0C 
#define PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET_VAL      0x0D 
#define PON_RESET_CFG_WARM_RESET_THEN_DVDD_HARD_RESET_VAL 0x0E 
#define PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL 0x0F 

/* Max PMIC Watchdog S1 and S2 timer value in seconds */
#define PON_WDOG_MAX_S1_S2_TIMER_VAL           127

typedef struct 
{
  uint16 s1_timer_addr; 
  uint16 s1_timer_mask; 
  uint16 s2_timer_addr;
  uint16 s2_timer_mask;
  uint16 reset_ctl_addr;
  uint16 reset_ctl2_addr;
  uint16 reset_en_mask;
  uint16 reset_type_mask;
  uint16 reset_pet_addr;
}pon_wdog_reg_info_type;

static pon_wdog_reg_info_type
pon_wdog_reg =
{
  PMIO_PON_PMIC_WD_RESET_S1_TIMER_ADDR,
  PMIO_PON_PMIC_WD_RESET_S1_TIMER_S1_TIMER_BMSK,
  PMIO_PON_PMIC_WD_RESET_S2_TIMER_ADDR,
  PMIO_PON_PMIC_WD_RESET_S2_TIMER_S2_TIMER_BMSK,
  PMIO_PON_PMIC_WD_RESET_S2_CTL_ADDR,
  PMIO_PON_PMIC_WD_RESET_S2_CTL2_ADDR,
  PMIO_PON_PMIC_WD_RESET_S2_CTL2_S2_RESET_EN_BMSK,
  PMIO_PON_PMIC_WD_RESET_S2_CTL_RESET_TYPE_BMSK,
  PMIO_PON_PMIC_WD_RESET_PET_ADDR
};

static boolean
pm_pon_wdog_initialized = FALSE;

pm_err_flag_type
pm_pon_wdog_init(void)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8              periph_type = 0;
  uint8              dig_major = 0;

  /* Read PON Peripheral info and check if it's valid */
  err_flag = pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_PERPH_TYPE_ADDR, &periph_type, 0);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  if (periph_type != PM_HW_MODULE_PON)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Read Dig Major Version info */
  err_flag = pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, PMIO_PON_REVISION2_ADDR, &dig_major, 0);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  if (!dig_major)
  {
    /* Only needed on very first peripheral versions */
    pon_wdog_reg.reset_ctl2_addr = pon_wdog_reg.reset_ctl_addr;

    pon_wdog_reg.reset_pet_addr = (pon_wdog_reg.reset_pet_addr - 0x1);
  }

  pm_pon_wdog_initialized = TRUE;
  return err_flag;
}

pm_err_flag_type
pm_pon_wdog_cfg(uint32 pmic_device_index, uint32 s1_timer, uint32 s2_timer, 
                pm_pon_reset_cfg_type reset_cfg_type)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8              reset_en = 0;
  uint8              reset_val = 0;

  if (pmic_device_index > PMIC_A_DEVICE_ID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (s1_timer > PON_WDOG_MAX_S1_S2_TIMER_VAL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (s2_timer > PON_WDOG_MAX_S1_S2_TIMER_VAL)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (reset_cfg_type >= PM_PON_RESET_CFG_INVALID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (!pm_pon_wdog_initialized)
  {
    err_flag = pm_pon_wdog_init();
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
  }

  err_flag = pm_comm_read_byte(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.reset_ctl2_addr, &reset_en, 0);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  reset_en = reset_en & pon_wdog_reg.reset_en_mask;

  if (reset_en)
  {
    err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.reset_ctl2_addr,
                                            pon_wdog_reg.reset_en_mask, 0x0, 0);

    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
    /* Requires 10 sleep clock cycles of delay */
    DALSYS_BusyWait(300);
  }

  /* Configure S1 timer */
  err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.s1_timer_addr,
                                          pon_wdog_reg.s1_timer_mask, (uint8)s1_timer, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Configure S2 timer */
  err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.s2_timer_addr, 
                                          pon_wdog_reg.s2_timer_mask, (uint8)s2_timer, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  switch (reset_cfg_type)
  {
     case PM_PON_RESET_CFG_WARM_RESET:
       reset_val = PON_RESET_CFG_WARM_RESET_VAL;
       break;

    case PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
      reset_val = PON_RESET_CFG_IMMEDIATE_XVDD_SHUTDOWN_VAL;
      break;

     case PM_PON_RESET_CFG_NORMAL_SHUTDOWN:
       reset_val = PON_RESET_CFG_NORMAL_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN:
       reset_val = PON_RESET_CFG_D_VDD_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
       reset_val = PON_RESET_CFG_X_VDD_SHUTDOWN_VAL;
       break;

     case PM_PON_RESET_CFG_HARD_RESET:
       reset_val = PON_RESET_CFG_HARD_RESET_VAL;
       break;

    case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET:
      reset_val = PON_RESET_CFG_DVDD_HARD_RESET_VAL;
      break;

    case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
      reset_val = PON_RESET_CFG_XVDD_HARD_RESET_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_DVDD_SHUTDOWN_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_XVDD_SHUTDOWN_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_DVDD_HARD_RESET_VAL;
      break;

    case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
      reset_val = PON_RESET_CFG_WARM_RESET_THEN_XVDD_HARD_RESET_VAL;
      break;

     default:
       /* We should never get here as the check is already made */
       return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  /* Configure reset value */
  err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM,
                                          pon_wdog_reg.reset_ctl_addr,
                                          pon_wdog_reg.reset_type_mask,
                                          reset_val, 0);

  /* Enable reset if it was already in enable state */
  if (reset_en)
  {
    err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.reset_ctl2_addr, 
                                            pon_wdog_reg.reset_en_mask, 0xFF, 0);
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
  }

  return err_flag;
}

pm_err_flag_type
pm_pon_wdog_enable(uint32 pmic_device_index, pm_on_off_type enable)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;
  uint8              data = 0;

  if (pmic_device_index > PMIC_A_DEVICE_ID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (!pm_pon_wdog_initialized)
  {
    err_flag = pm_pon_wdog_init();
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
  }

  data = (enable == PM_ON) ? 0xFF : 0;

  err_flag = pm_comm_write_byte_mask(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.reset_ctl2_addr,
                                          pon_wdog_reg.reset_en_mask, data, 0);
 

  return err_flag;
}

pm_err_flag_type
pm_pon_wdog_pet(uint32 pmic_device_index)
{
  pm_err_flag_type   err_flag = PM_ERR_FLAG_SUCCESS;

  if (pmic_device_index > PMIC_A_DEVICE_ID)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  if (!pm_pon_wdog_initialized)
  {
    err_flag = pm_pon_wdog_init();
    if (err_flag != PM_ERR_FLAG_SUCCESS)
    {
      return err_flag;
    }
  }

  /* Writing 0x1 to the addr clears the PMIC Watchdog timer */
  err_flag = pm_comm_write_byte(PMIC_A_SLAVEID_PRIM, pon_wdog_reg.reset_pet_addr, 0x1, 0);

  /* Requires 5 sleep clock cycles of delay */
  DALSYS_BusyWait(150);

  return err_flag;
}
