/*! \file pm_coincell.c
*  
*  \brief  pm_coincell.c
*    
*  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/coincell/src/pm_coincell.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/16/14   akm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_coincell.h"
#include "pm_coincell_driver.h"
#include "pm_comm.h"

pm_err_flag_type pm_coincell_enable
(
   uint8 pmic_chip, 
   pm_on_off_type enable
)
{
    pm_err_flag_type err_flag           = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg        = 0;
    pm_coincell_data_type *coincell_ptr = pm_coincell_get_data(pmic_chip);

    if (coincell_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    reg = coincell_ptr->coincell_reg_table->base_address + coincell_ptr->coincell_reg_table->en_ctl;

    err_flag = pm_comm_write_byte_mask(coincell_ptr->comm_ptr->slave_id, reg, 0x80, (pm_register_data_type)((enable)<<7), 0);

    return err_flag;
}


pm_err_flag_type pm_coincell_get_status
(
  uint8 pmic_chip,
  boolean  *status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_coincell_data_type *coincell_ptr = pm_coincell_get_data(pmic_chip);

    pm_register_address_type reg_coincell_en_ctl = 0;
    pm_register_data_type en_ctl_val = 0;
 
    if (coincell_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    reg_coincell_en_ctl     = coincell_ptr->coincell_reg_table->base_address  + coincell_ptr->coincell_reg_table->en_ctl;
 
    err_flag   = pm_comm_read_byte(coincell_ptr->comm_ptr->slave_id, reg_coincell_en_ctl, &en_ctl_val, 0);

    if (en_ctl_val & (0x80))
   	{
      *status = TRUE;
   	}
    else
   	{
   	  *status = FALSE;
   	}
    return err_flag;
}


pm_err_flag_type pm_coincell_set_charge_current_limit_resistor
(
  uint8 pmic_chip,
  pm_coincell_chg_rset_type coin_chg_rset_value
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_coincell_data_type *coincell_ptr = pm_coincell_get_data(pmic_chip);
    pm_register_address_type reg_coincell_coin_chg_rset = 0;

 
    if (coincell_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (coin_chg_rset_value >= PM_COINCELL_RSET_INVALID)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
	
    reg_coincell_coin_chg_rset = coincell_ptr->coincell_reg_table->base_address  + coincell_ptr->coincell_reg_table->coin_chg_rset;
    err_flag   = pm_comm_write_byte_mask(coincell_ptr->comm_ptr->slave_id,reg_coincell_coin_chg_rset,0x03,coin_chg_rset_value,0);
	
    return err_flag;
}

pm_err_flag_type pm_coincell_set_charging_voltage
(
  uint8 pmic_chip,
  pm_coincell_chg_vset_type coin_chg_vset_value
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_coincell_data_type *coincell_ptr = pm_coincell_get_data(pmic_chip);
    pm_register_address_type reg_coincell_coin_chg_vset = 0;

 
    if (coincell_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (coin_chg_vset_value >= PM_COINCELL_VSET_INVALID)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
	
    reg_coincell_coin_chg_vset = coincell_ptr->coincell_reg_table->base_address  + coincell_ptr->coincell_reg_table->coin_chg_vset;
    err_flag   = pm_comm_write_byte_mask(coincell_ptr->comm_ptr->slave_id,reg_coincell_coin_chg_vset,0x03,coin_chg_vset_value,0);
	
    return err_flag;
}


