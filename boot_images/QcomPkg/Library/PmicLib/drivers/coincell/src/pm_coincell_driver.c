/*! \file pm_coincell_driver.c 
*  \n
*  \brief This file contains coincell peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/coincell/src/pm_coincell_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/14   akm     Creation
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_coincell_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the COINCELL data */
static pm_coincell_data_type *pm_coincell_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_coincell_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
	pm_coincell_data_type *coincell_ptr = NULL;

    coincell_ptr = pm_coincell_data_arr[pmic_index];

    if (NULL == coincell_ptr)
    {
        pm_malloc( sizeof(pm_coincell_data_type), (void**)&coincell_ptr);
                                                    
        /* Assign Comm ptr */
        coincell_ptr->comm_ptr = comm_ptr;
		
		/* COINCELL Register Info - Obtaining Data through dal config */
        coincell_ptr->coincell_reg_table = (pm_coincell_register_info_type*)pm_target_information_get_common_info(PM_PROP_COINCELL_REG);
        CORE_VERIFY_PTR(coincell_ptr->coincell_reg_table);
		
        coincell_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_COINCELL_NUM, pmic_index);
        CORE_VERIFY(coincell_ptr->num_of_peripherals  != 0);

        pm_coincell_data_arr[pmic_index] = coincell_ptr;
    }
}


pm_coincell_data_type* pm_coincell_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_coincell_data_arr[pmic_index];
    }
    return NULL;
}

uint8 pm_coincell_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_coincell_data_arr[pmic_index] !=NULL))
  {
      return pm_coincell_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}


