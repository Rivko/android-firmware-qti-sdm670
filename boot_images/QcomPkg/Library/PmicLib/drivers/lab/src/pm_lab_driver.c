/*! \file
*
*  \brief  pm_lab_driver.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE


when        who     what, where, why
--------    ----    ---------------------------------------------------------
12/02/16    ks      Added support for new IBB Type/Subtype
08/20/14    al      Updating comm lib
08/29/14    al      KW fixes
05/20/14    al      Architecture update
04/30/14    al      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "CoreVerify.h"
#include "device_info.h"
#include "pm_utils.h"

#include "pm_lab_driver.h"


/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
/* Static global variable to store the LAB data */
static pm_lab_data_type *pm_lab_data_arr[PM_MAX_NUM_PMICS];


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_lab_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_lab_data_type *lab_ptr = NULL;

    lab_ptr = pm_lab_data_arr[pmic_index];

    if (NULL == lab_ptr)
    {
        pm_malloc( sizeof(pm_lab_data_type), (void**)&lab_ptr);

        /* Assign Comm ptr */
        lab_ptr->comm_ptr = comm_ptr;

        /* IBB Register Info - Obtaining Data through dal config */
        lab_ptr->lab_reg_table = (pm_lab_register_info_type*)pm_target_information_get_common_info(PM_PROP_LAB_REG);
        CORE_VERIFY_PTR(lab_ptr->lab_reg_table);

        lab_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_LAB_NUM, pmic_index);
        CORE_VERIFY(lab_ptr->num_of_peripherals  != 0);

        /* Added Periph revision info to differentiate at Lib level APIs */
        lab_ptr->periph_revision.periph_type = peripheral_info->peripheral_type;
        lab_ptr->periph_revision.periph_subtype = peripheral_info->peripheral_subtype;
        lab_ptr->periph_revision.periph_dig_major = peripheral_info->digital_major_version;
        lab_ptr->periph_revision.periph_dig_minor = peripheral_info->digital_minor_version;

        pm_lab_data_arr[pmic_index] = lab_ptr;
    }
}


pm_lab_data_type* pm_lab_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_lab_data_arr[pmic_index];
    }
    return NULL;
}

uint8 pm_lab_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_lab_data_arr[pmic_index] !=NULL))
  {
      return pm_lab_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}
