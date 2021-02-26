/*! \file pm_haptics_driver.c 
*  \n
*  \brief This file contains Haptics peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/21/15   al      New file 
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_haptics_driver.h"
#include "CoreVerify.h"
#include "pm_comm.h"
#include "pm_utils.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the HAPTICS driver data */
static pm_haptics_data_type *pm_haptics_data_arr[PM_MAX_NUM_PMICS];
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_haptics_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint32 pmic_index)
{
    pm_haptics_data_type *haptics_ptr = NULL;

    haptics_ptr = pm_haptics_data_arr[pmic_index];
    
    if (haptics_ptr == NULL)
    {
        pm_malloc( sizeof(pm_haptics_data_type), (void**)&haptics_ptr);
                                                    
        /* Assign Comm ptr */
        haptics_ptr->comm_ptr = comm_ptr;

        /* HAPTICS Register Info - Obtaining Data through dal config */
        haptics_ptr->haptics_reg_table = (pm_haptics_register_info_type*)pm_target_information_get_common_info(PM_PROP_HAPTICS_REG);
        CORE_VERIFY_PTR(haptics_ptr->haptics_reg_table);

        //haptics_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_HAPTICS;
        haptics_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_HAPTICS_NUM, pmic_index);
        CORE_VERIFY(haptics_ptr->num_of_peripherals  != 0);

        pm_haptics_data_arr[pmic_index] = haptics_ptr;
    }
}

pm_haptics_data_type* pm_haptics_get_data(uint32 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_haptics_data_arr[pmic_index];
  }

    return NULL;
}

uint32 pm_haptics_get_num_peripherals(uint32 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS)&& 
  	  (pm_haptics_data_arr[pmic_index] !=NULL))
  {
      return pm_haptics_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}
