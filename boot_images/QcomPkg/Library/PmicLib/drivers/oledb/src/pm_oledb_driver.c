/*! \file pm_oledb_driver.c
*  \n
*  \brief This file contains oledb peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/15/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_oledb_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
/* Static global variable to store the OLEDB data */
static pm_oledb_data_type *pm_oledb_data_arr[PM_MAX_NUM_PMICS] = {0};


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_oledb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info)
{
    pm_oledb_data_type *oledb_ptr = NULL;
    uint8 pmic_index = comm_ptr->pmic_index;

    oledb_ptr = pm_oledb_data_arr[pmic_index];

    if (NULL == oledb_ptr)
    {
        pm_malloc(sizeof(pm_oledb_data_type), (void **)&oledb_ptr);

        /* Assign Comm ptr */
        oledb_ptr->comm_ptr = comm_ptr;

        /* OLEDB Register Info - Obtaining Data through dal config */
        oledb_ptr->oledb_reg = (pm_oledb_register_info_type *)pm_target_information_get_common_info(PM_PROP_OLEDB_REG);
        CORE_VERIFY_PTR(oledb_ptr->oledb_reg);

        pm_oledb_data_arr[pmic_index] = oledb_ptr;
    }
}

pm_oledb_data_type* pm_oledb_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_oledb_data_arr[pmic_index];
    }

    return NULL;
}

/** Not supported as it's not required
uint8 pm_oledb_get_num_peripherals(uint8 pmic_index)
{
  if((pm_oledb_data_arr[pmic_index] !=NULL)&&
      (pmic_index < PM_MAX_NUM_PMICS))
  {
      return pm_oledb_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}
*/

