/*! \file pm_pbs_client_driver.c
*  \n
*  \brief PBS Client driver initialization.
*  \n  
*  \n &copy; Copyright 2014-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/15   aab     Updated to support MSM8998 target
04/29/15   aab     Added support for PMK8001 
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
06/13/13   kt      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "CoreVerify.h"
#include "hw_module_type.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the PBS data */
static pm_pbs_client_data_type *pm_pbs_client_data_arr[PM_MAX_NUM_PMICS]; 


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_pbs_client_data_type *pbs_client_ptr = NULL;

    // Abort if the new PBS client driver is used on HW with old PBS client subtype
    // New driver doesn't support the old PBS client peripheral
    if (peripheral_info->peripheral_subtype == PM_HW_MODULE_PBS_CLIENT) 
    {
        CORE_VERIFY(0);
    }

    if ((peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT_1TRIG) &&
        (peripheral_info->peripheral_subtype != PM_HW_MODULE_PBS_CLIENT_4TRIG))
    {
        return;
    }

    pbs_client_ptr = pm_pbs_client_data_arr[pmic_index];
    
    if (pbs_client_ptr == NULL)
    {
        pm_malloc( sizeof(pm_pbs_client_data_type), (void**)&pbs_client_ptr);

        /* Assign Comm ptr */
        pbs_client_ptr->comm_ptr = comm_ptr;

        /* PBS Client Register Info - Obtaining Data through dal config */
        pbs_client_ptr->pbs_client_reg_table = (pm_pbs_client_register_info_type*)pm_target_information_get_common_info(PM_PROP_PBS_CLIENT_REG);
        CORE_VERIFY_PTR(pbs_client_ptr->pbs_client_reg_table);
                                                              
        /* PBS Client Num of peripherals - Obtaining Data through dal config */
        pbs_client_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_PBS_CLIENT_NUM, pmic_index);
                                                              
        CORE_VERIFY(pbs_client_ptr->num_of_peripherals != 0);
        pm_pbs_client_data_arr[pmic_index] = pbs_client_ptr;
    }
}

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index)
{
  if(pmic_index < PM_MAX_NUM_PMICS) 
  {
      return pm_pbs_client_data_arr[pmic_index];
  }

  return NULL;
}


uint8 pm_pbs_client_get_num_peripherals(uint8 pmic_index)
{
  if( (pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_pbs_client_data_arr[pmic_index] != NULL))
  {
      return pm_pbs_client_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}

