/*! \file
*  
*  \brief  pm_schg_driver.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when       	who     	 what, where, why
--------   	---    		 ---------------------------------------------------------- 
12/19/16    sm           Added API to get SCHG Charger PmicIndex
09/07/16    sm           Added pm_schg_get_dig_major API to return digital major
04/18/14    al           Initial revision
========================================================================== */

/*===========================================================================

                    INCLUDE FILES

===========================================================================*/
#include "pm_schg_driver.h"
#include "CoreVerify.h"
#include "pm_utils.h"
#include "hw_module_type.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/

/* Static global variable to store the SCHG driver data */
static pm_schg_data_type *pm_schg_data_arr[PM_MAX_NUM_PMICS];
static pm_register_data_type dig_major = 0;

#define SCHG_MISC_REVISION2   0x1601

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_schg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_schg_data_type *schg_ptr = NULL;
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
     
    /*return if coincell since it is not supported at the moment*/
    if(PM_HW_MODULE_CHARGER_COINCELL == peripheral_info->peripheral_subtype)
    {
      return;
    }

    schg_ptr = pm_schg_data_arr[pmic_index];

    if (NULL == schg_ptr)
    {
        pm_malloc( sizeof(pm_schg_data_type), (void**)&schg_ptr);

        /* Assign Comm ptr */
        schg_ptr->comm_ptr = comm_ptr;

        /* schg Register Info - Obtaining Data through dal config */
        schg_ptr->schg_register = (schg_register_ds*)pm_target_information_get_common_info(PM_PROP_CHARGER_REG);
        CORE_VERIFY_PTR(schg_ptr->schg_register);

        schg_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_CHARGER_NUM, pmic_index);
        CORE_VERIFY(schg_ptr->num_of_peripherals != 0);

        pm_schg_data_arr[pmic_index] = schg_ptr;

        err_flag = pm_comm_read_byte(schg_ptr->comm_ptr->slave_id, SCHG_MISC_REVISION2, &dig_major, 0);
        CORE_VERIFY(err_flag == PM_ERR_FLAG_SUCCESS);
    }
}

pm_schg_data_type* pm_schg_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS)
  {
      return pm_schg_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_schg_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS)&&
  	  (pm_schg_data_arr[pmic_index] != NULL))
  {
      return pm_schg_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}

uint8 pm_schg_get_dig_major(void)
{
  return dig_major;
}


void pm_schg_get_pmic_info(uint8 *pmic_index, uint8 *slave_id)
{
    uint8 i = 0;

    if(!pmic_index || !slave_id)
        return;

    for(i = 0; i < PM_MAX_NUM_PMICS; i++)
    {
        if(pm_schg_data_arr[i])
        {
            *pmic_index = pm_schg_data_arr[i]->comm_ptr->pmic_index;
            *slave_id   = pm_schg_data_arr[i]->comm_ptr->slave_id;
            break;
        }
    }

    return;
}

