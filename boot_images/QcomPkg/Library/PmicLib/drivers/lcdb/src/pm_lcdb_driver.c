/*! \file pm_lcdb_driver.c
*  \n
*  \brief This file contains lcdb peripheral driver initialization during which the driver data is stored.
*  \n
*  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved.
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
#include "pm_lcdb_driver.h"
#include "CoreVerify.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
/* Static global variable to store the LCDB data */
static pm_lcdb_data_type *pm_lcdb_data_arr[PM_MAX_NUM_PMICS] = {0};


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_lcdb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info)
{
    pm_lcdb_data_type *lcdb_ptr = NULL;
    uint8 pmic_index = comm_ptr->pmic_index;

    lcdb_ptr = pm_lcdb_data_arr[pmic_index];

    if (NULL == lcdb_ptr)
    {
        pm_malloc(sizeof(pm_lcdb_data_type), (void **)&lcdb_ptr);

        /* Assign Comm ptr */
        lcdb_ptr->comm_ptr = comm_ptr;

        /* LCDB Register Info - Obtaining Data through dal config */
        lcdb_ptr->lcdb_reg = (pm_lcdb_register_info_type*)pm_target_information_get_common_info(PM_PROP_LCDB_REG);
        CORE_VERIFY_PTR(lcdb_ptr->lcdb_reg);

        // lcdb_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LCDB_NUM, pmic_index);
        // CORE_VERIFY(lcdb_ptr->num_of_peripherals  != 0);

        pm_lcdb_data_arr[pmic_index] = lcdb_ptr;
    }
}

pm_lcdb_data_type* pm_lcdb_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_lcdb_data_arr[pmic_index];
    }
    return NULL;
}

/** Not required as it's not used
uint8 pm_lcdb_get_num_peripherals(uint8 pmic_index)
{
    if((pm_lcdb_data_arr[pmic_index] !=NULL)&&
        (pmic_index < PM_MAX_NUM_PMICS))
    {
        return pm_lcdb_data_arr[pmic_index]->num_of_peripherals;
    }

    return 0;
}
*/

