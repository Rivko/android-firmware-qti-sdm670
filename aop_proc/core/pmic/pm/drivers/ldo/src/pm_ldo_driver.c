/*! \file pm_ldo_driver.c
*  \n
*  \brief This file contains LDO peripheral driver initialization during which the driver
*         driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/ldo/src/pm_ldo_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs   
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_ldo_driver.h"
#include "pm_target_information.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_debug.h"


/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the LDO driver data */
static pm_ldo_data_type *pm_ldo_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

static pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info, boolean *is_stepper);

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void pm_ldo_driver_init(pm_comm_info_type * comm_ptr, peripheral_info_type * peripheral_info, uint8 pmic_index)
{
  pm_ldo_data_type *ldo_ptr = NULL;
  uint32 ldo_index = 0;
  pm_register_address_type base_address = 0;
  pm_register_address_type periph_offset = 0;
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type reg = 0x0;
  boolean is_stepper = FALSE;
  pm_register_data_type max_volt[2] = { 0 };

  pm_debug_info_type *pmic_debug = pm_get_debug_info();
  CORE_VERIFY_PTR(pmic_debug);
  pmic_debug->pm_ldo_data_arr = pm_ldo_data_arr;

  ldo_ptr = pm_ldo_data_arr[pmic_index];

  if (ldo_ptr == NULL)
  {
    pm_malloc(sizeof(pm_ldo_data_type), (void **)&ldo_ptr);

    /* Assign Comm ptr */
    ldo_ptr->comm_ptr = comm_ptr;

    /* LDO Register Info - Obtaining Data through dal config */
    ldo_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type *)pm_target_information_get_common_info(PM_PROP_LDO_REG);

    CORE_VERIFY_PTR(ldo_ptr->pm_pwr_data.pwr_reg_table);

    ldo_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LDO_NUM, pmic_index);

    /* Num of peripherals cannot be 0 if this driver init gets called */
    CORE_VERIFY(ldo_ptr->pm_pwr_data.num_of_peripherals != 0);

    /* LDO pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
    pm_malloc(sizeof(pm_pwr_specific_info_type) * (ldo_ptr->pm_pwr_data.num_of_peripherals), (void **)&(ldo_ptr->pm_pwr_data.pwr_specific_info));

    /* Save first LDO peripheral's base address */
    ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

    /* LDO Subtype pointer malloc */
    pm_malloc(sizeof(uint8) * (ldo_ptr->pm_pwr_data.num_of_peripherals), (void **)&(ldo_ptr->periph_subtype));

    pm_ldo_data_arr[pmic_index] = ldo_ptr;
  }

  if (ldo_ptr != NULL)
  {
    base_address = ldo_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
    periph_offset = ldo_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

    /* Peripheral Baseaddress should be >= first peripheral's base addr */
    CORE_VERIFY(peripheral_info->base_address >= base_address);

    /* Calculate LDO peripheral index */
    ldo_index = ((peripheral_info->base_address - base_address) / periph_offset);

    /* Peripheral Index should be less than max number of peripherals */
    CORE_VERIFY(ldo_index < ldo_ptr->pm_pwr_data.num_of_peripherals);

    /* Save LDO's Peripheral Subtype value */
    ldo_ptr->periph_subtype[ldo_index] = peripheral_info->peripheral_subtype;

    /* Peripheral Index should be less than number of peripherals */
    CORE_VERIFY(ldo_index < (ldo_ptr->pm_pwr_data.num_of_peripherals));

    /* Save LDO's Peripheral Type value */
    ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type = peripheral_info->peripheral_type;

    /* Save each LDO peripheral's base address */
    ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_base_address = peripheral_info->base_address;

    if ((ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].periph_type == PM_HW_MODULE_LDO) &&
        (ldo_ptr->periph_subtype[ldo_index] >= PM_HW_MODULE_LDO_HT_N300_STEPPER))
    {
      /* Flag this rail peripheral as unified register map peripheral */
      ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].is_periph_unified_reg = TRUE;

      if (ldo_ptr->periph_subtype[ldo_index] >= PM_HW_MODULE_LDO_500_N300_100)
      {
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pmic4_flag = FALSE;
        reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->ULS_VSET_LB);
      }
      else
      {
        ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pmic4_flag = TRUE;
        reg = (pm_register_address_type)(peripheral_info->base_address + ldo_ptr->pm_pwr_data.pwr_reg_table->PMIC4_ULS_VSET_LB);
      }

      /* Read current LDO's upper limit voltage value and save it */
      err_flag = pm_comm_read_byte_array(ldo_ptr->comm_ptr->slave_id, reg, 2, max_volt, 0);
      CORE_VERIFY(err_flag == PM_ERR_FLAG_SUCCESS);

      /* LDO Vset Info - 1mV step for HT rails */
      ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset.RangeMin = 0;
      ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset.RangeMax = ((max_volt[1] << 8) | (max_volt[0])) * 1000;
      ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].pwr_vset.VStep = 1000;

      pm_ldo_get_volt_setting_info(peripheral_info, &is_stepper);
    }
    else
    {
      // Abort if not unified periph reg is detected.
      // RPMh only supports unified periph reg interface.
      CORE_VERIFY(0);
    }

    ldo_ptr->pm_pwr_data.pwr_specific_info[ldo_index].is_periph_stepper = is_stepper;
  }
}

pm_pwr_volt_info_type* pm_ldo_get_volt_setting_info(peripheral_info_type *peripheral_info, boolean *is_stepper)
{
  pm_pwr_volt_info_type *volt_info = NULL;

  switch ((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_LDO_N600_STEPPER:
    case PM_HW_MODULE_LDO_N1200_STEPPER:
    case PM_HW_MODULE_LDO_HT_N300_STEPPER:
    case PM_HW_MODULE_LDO_HT_N600_STEPPER:
    case PM_HW_MODULE_LDO_HT_N1200_STEPPER:
      *is_stepper = TRUE;
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_ULT_LDO:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_ULT_LDO_N300_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N600_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N900_STEPPER:
    case PM_HW_MODULE_ULT_LDO_N1200_STEPPER:
      *is_stepper = TRUE;
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  default:
    volt_info = NULL;
    break;
  }

  return volt_info;
}


pm_ldo_data_type* pm_ldo_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_ldo_data_arr[pmic_index];
  }

  return NULL;
}

__attribute__((section("pm_cram_reclaim_pool")))
uint8 pm_ldo_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_ldo_data_arr[pmic_index] !=NULL))
  {
      return pm_ldo_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return NULL;
}



