/*! \file pm_smps_driver.c
*  \n
*  \brief This file contains SMPS peripheral driver initialization during which the driver
*         driver data is stored. 
*  \n   
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/15   al      Updated rails API
07/10/15   al      Adding support for HP type
12/06/14   al      Porting optimization changes
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smps_driver.h"
#include "pm_target_information.h"
#include "pm_version.h"
#include "pm_comm.h"
#include "device_info.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the SMPS driver data */
static pm_smps_data_type *pm_smps_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

static pm_pwr_volt_info_type* pm_smps_get_volt_setting_info(peripheral_info_type *peripheral_info);
static pm_pwr_range_info_type* pm_smps_get_range_setting_info(peripheral_info_type *peripheral_info);

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/

void pm_smps_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_smps_data_type *smps_ptr = NULL;
    uint16 smps_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg = 0x0;
    pm_register_data_type data = 0;
    pm_register_data_type max_volt[2] = {0};

    // Initialize the driver for CTL peripheral subtypes only and return in other cases
    switch(peripheral_info->peripheral_type)
    {
    case PM_HW_MODULE_FTS:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_FTS__FTS_CTL:
        case PM_HW_MODULE_FTS__FTS2p5_CTL:
        case PM_HW_MODULE_FTS__FTS426_CTL:
           break;
        default:
           return;
        }
        break;
    case PM_HW_MODULE_HF_BUCK:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_HF_BUCK_GP_CTL:
        case PM_HW_MODULE_HF_BUCK_RF_CTL:
        case PM_HW_MODULE_HF_BUCK_VPWM_CTL:
           break;
        default:
           return;
        }
        break;
    case PM_HW_MODULE_ULT_BUCK:
        switch(peripheral_info->peripheral_subtype)
        {
        case PM_HW_MODULE_ULT_BUCK_CTL_LV:
        case PM_HW_MODULE_ULT_BUCK_CTL2:
        case PM_HW_MODULE_ULT_BUCK_CTL3:
        case PM_HW_MODULE_ULT_BUCK_CTL_MV:
           break;
        default:
           return;
        }
        break;
    default:
        return;
    }

    smps_ptr = pm_smps_data_arr[pmic_index];

    if (smps_ptr == NULL)
    {
        pm_malloc( sizeof(pm_smps_data_type), (void**)&smps_ptr);
                                                    
        /* Assign Comm ptr */
        smps_ptr->comm_ptr = comm_ptr;

        /* SMPS Register Info - Obtaining Data through dal config */
        smps_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_SMPS_REG);

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_reg_table);

        smps_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_SMPS_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(smps_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* SMPS pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(smps_ptr->pm_pwr_data.num_of_peripherals), (void**)&(smps_ptr->pm_pwr_data.pwr_specific_info));

        CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info);

        /* Save first SMPS peripheral's base address */
        smps_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

        pm_smps_data_arr[pmic_index] = smps_ptr;
    }
    
    if (smps_ptr != NULL)
    {
        base_address = smps_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        periph_offset = smps_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Peripheral Baseaddress should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        /* Calculate SMPS peripheral index */
        smps_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Peripheral Index should be less than number of peripherals */
        CORE_VERIFY(smps_index < (smps_ptr->pm_pwr_data.num_of_peripherals));

        /* Save SMPS's Peripheral Type value */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type = peripheral_info->peripheral_type;

        /* Save each SMPS peripheral's base address */
        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_base_address = peripheral_info->base_address;

        if (((smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type == PM_HW_MODULE_HF_BUCK) && 
            (peripheral_info->peripheral_subtype >= PM_HW_MODULE_HF_BUCK_VPWM_CTL)) || 
            ((smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type == PM_HW_MODULE_FTS) && 
             (peripheral_info->peripheral_subtype >= PM_HW_MODULE_FTS__FTS426_CTL)))
        {
           /* Flag this rail peripheral as unified register map peripheral */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].is_periph_unified_reg = TRUE;

            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range = 0;

            /* Read current SMPS's upper limit voltage value and save it */
            reg = (pm_register_address_type)(peripheral_info->base_address + smps_ptr->pm_pwr_data.pwr_reg_table->ULS_VSET_LB);
            err_flag = pm_comm_read_byte_array(smps_ptr->comm_ptr->slave_id, reg, 2, max_volt, 0);
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

            pm_malloc(sizeof(pm_pwr_volt_info_type), (void**)&(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset));

            /* SMPS Vset Info - 1mV step for HT rails */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset->RangeMin = 0;
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset->RangeMax = ((max_volt[1]<<8) | (max_volt[0]))*1000;
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset->VStep = 1000; 

            pm_malloc(sizeof(pm_pwr_range_info_type), (void**)&(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info));

            /* SMPS Range Info - 0 for new rails */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info->start = 0;
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info->end = 0;
        }
        else if (PM_HW_MODULE_ULT_BUCK == smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].periph_type)
        {
            /* Read current SMPS's range value and save it */
            reg = (pm_register_address_type)(peripheral_info->base_address + smps_ptr->pm_pwr_data.pwr_reg_table->VOLTAGE_CTRL2);
            err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, &data, 0);
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);

            /* Extracting bit 6 and bit 5 of VOLTAGE_CTRL2 Register to determine the range set */
            data &=  0x60;
            if (0x60 == data)
            {
                smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range = 1;
            }
            else
            {
                smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range = 0;
            }

            /* SMPS Vset Info - Obtaining Data through dal config */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset = pm_smps_get_volt_setting_info(peripheral_info);
            CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset);

            /* SMPS Range Info - Obtaining Data through dal config */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info = pm_smps_get_range_setting_info(peripheral_info);
            CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info);
        }
        else
        {
            /* Read current SMPS's range value and save it */
            reg = (pm_register_address_type)(peripheral_info->base_address + smps_ptr->pm_pwr_data.pwr_reg_table->VOLTAGE_CTRL1);
            err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, &data, 0);
            CORE_VERIFY (err_flag == PM_ERR_FLAG_SUCCESS);
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range = data;

            /* SMPS Vset Info - Obtaining Data through dal config */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset = pm_smps_get_volt_setting_info(peripheral_info);
            CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_vset);

            /* SMPS Range Info - Obtaining Data through dal config */
            smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info = pm_smps_get_range_setting_info(peripheral_info);
            CORE_VERIFY_PTR(smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].pwr_range_info);
        }

        smps_ptr->pm_pwr_data.pwr_specific_info[smps_index].is_periph_stepper = TRUE;
    }
}

pm_pwr_volt_info_type* pm_smps_get_volt_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_volt_info_type *volt_info = NULL;

  switch((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_FTS:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_FTS__FTS_CTL:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_FTS_VOLT);
      break;
    case PM_HW_MODULE_FTS__FTS2p5_CTL:
      volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_FTS2p5_VOLT);
      break;
    default:
      volt_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_HF_BUCK:
    volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_HFS_VOLT);
    break;
  case PM_HW_MODULE_ULT_BUCK:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_ULT_BUCK_CTL_LV:
    case PM_HW_MODULE_ULT_BUCK_CTL2:
    case PM_HW_MODULE_ULT_BUCK_CTL3:
      {
         volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_VOLT_1);
        break;
      }
    case PM_HW_MODULE_ULT_BUCK_CTL_MV:
      {
         volt_info = (pm_pwr_volt_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_VOLT_2);
        break;
      }
    default:
      {
        // we should not reach this point
        break;
      }
    }
    break;
  default:
    volt_info = NULL;
    break;
  }

  return volt_info;
}

pm_pwr_range_info_type* pm_smps_get_range_setting_info(peripheral_info_type *peripheral_info)
{
  pm_pwr_range_info_type *range_info = NULL;

  switch((pm_hw_module_type)peripheral_info->peripheral_type)
  {
  case PM_HW_MODULE_FTS:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_FTS__FTS_CTL:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_FTS_RANGE);
      break;
    case PM_HW_MODULE_FTS__FTS2p5_CTL:
      range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_FTS2p5_RANGE);
      break;
    default:
      range_info = NULL;
      break;
    }
    break;
  case PM_HW_MODULE_HF_BUCK:
    range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_HFS_RANGE);
    break;
  case PM_HW_MODULE_ULT_BUCK:
    switch(peripheral_info->peripheral_subtype)
    {
    case PM_HW_MODULE_ULT_BUCK_CTL_LV:
    case PM_HW_MODULE_ULT_BUCK_CTL2:
    case PM_HW_MODULE_ULT_BUCK_CTL3:
      {
         range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_RANGE_1);
        break;
      }
    case PM_HW_MODULE_ULT_BUCK_CTL_MV:
      {
         range_info = (pm_pwr_range_info_type*)pm_target_information_get_common_info(PM_PROP_ULT_BUCK_RANGE_2);
        break;
      }
    default:
      {
        // we should not reach this point
        break;
      }
    }
    break;
  default:
    range_info = NULL;
    break;
  }

  return range_info;
}

pm_smps_data_type* pm_smps_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_smps_data_arr[pmic_index];
  }

    return NULL;
}

uint8 pm_smps_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_smps_data_arr[pmic_index] != NULL))
  {
      return pm_smps_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return 0;
}

