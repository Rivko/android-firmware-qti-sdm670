/*! \file pm_wled_driver.c 
*  \n
*  \brief This file contains WLED peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/16/14    akm     Comm change Updates
03/31/14    akm     Updated to the latest PMIC driver architecture   
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_wled_driver.h"
#include "CoreVerify.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the WLED driver data */
static pm_wled_data_type *pm_wled_data_arr[PM_MAX_NUM_PMICS];
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_wled_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_wled_data_type *wled_ptr = NULL;

	wled_ptr = pm_wled_data_arr[pmic_index];

    if (NULL == wled_ptr)
    {
        pm_malloc( sizeof(pm_wled_data_type), (void**)&wled_ptr);
                                                    
        /* Assign Comm ptr */
        wled_ptr->comm_ptr = comm_ptr;


        /* WLED Register Info - Obtaining Data through dal config */
        wled_ptr->wled_reg_table = (pm_wled_register_info_type*)pm_target_information_get_common_info((uint32)PM_PROP_WLED_REG);
        CORE_VERIFY_PTR(wled_ptr->wled_reg_table);

        //wled_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_WLED;
		wled_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_WLED_NUM, pmic_index);
        CORE_VERIFY(wled_ptr->num_of_peripherals != 0);

        pm_wled_data_arr[pmic_index] = wled_ptr;
    }
}

pm_wled_data_type* pm_wled_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_wled_data_arr[pmic_index];
    }

    return NULL;
}

uint8 pm_wled_get_num_peripherals(uint8 pmic_index)
{
  if((pm_wled_data_arr[pmic_index] !=NULL)&&
  	  (pmic_index < PM_MAX_NUM_PMICS))
    {
      return pm_wled_data_arr[pmic_index]->num_of_peripherals;
    }

    return 0;
}
