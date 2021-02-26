/*! \file
*  \n
*  \brief  pm_fg_driver.c
*  \details
*  Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     ---------------------------------------------------------------- 
12/19/16    sm     Added API to get FG PmicIndex
07/18/16    llioe   Handled register-address differences v1.1(and earlier) vs v2.0
09/22/14    aab     Porting FG driver to SBL        
08/20/14    al      Updating comm lib 
08/29/14    al      KW fixes
06/09/14    al      Arch update 
04/01/14    va      New file
================================================================================== */

#include "pm_fg_driver.h"
#include "CoreVerify.h"
#include "pm_utils.h"
#include "device_info.h"
#include "hw_module_type.h"

/*===========================================================================

                        STATIC VARIABLES

===========================================================================*/
/* Static global variable to store the FG driver data */
static pm_fg_data_type *pm_fg_data_arr[PM_MAX_NUM_PMICS];


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_fg_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_fg_data_type *fg_ptr = NULL;
    fg_batt_info_register_ds *pFgBattInfoRegs = NULL;

    fg_ptr = pm_fg_data_arr[pmic_index];

    if (NULL == fg_ptr)
    {
        pm_malloc( sizeof(pm_fg_data_type), (void**)&fg_ptr);

        /* Assign Comm ptr */
        fg_ptr->comm_ptr = comm_ptr;

        /* fg Register Info - Obtaining Data through dal config */
        fg_ptr->fg_register = (fg_register_ds*)pm_target_information_get_common_info(PM_PROP_FG_REG);
        CORE_VERIFY_PTR(fg_ptr->fg_register);

        fg_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_FG_NUM, pmic_index);
        CORE_VERIFY(fg_ptr->num_of_peripherals != 0);

        pm_fg_data_arr[pmic_index] = fg_ptr;
    }

    if (((pm_hw_module_fg_subtype)peripheral_info->peripheral_subtype) == PM_HW_MODULE_FG_MEM_IF)
    {
        switch (peripheral_info->digital_major_version)
        {
            case 0:
                //use earlier revision of the register-addresses list
                pFgBattInfoRegs = fg_ptr->fg_register->batt_info_register;
                fg_ptr->fg_register->batt_info_register = (pFgBattInfoRegs + 1);
                CORE_VERIFY_PTR(fg_ptr->fg_register->batt_info_register);
                break;

            case 1:
            case 2:
            default:
                //use latest version of the register-addresses list
                //no adjustment needed
                break;
        }
    }
}

pm_fg_data_type* pm_fg_get_data(uint8 pmic_index)
{
  if(pmic_index <PM_MAX_NUM_PMICS)
  {
      return pm_fg_data_arr[pmic_index];
  }

  return NULL;
}

uint8 pm_fg_get_num_peripherals(uint8 pmic_index)
{
  if((pm_fg_data_arr[pmic_index] !=NULL)&&
  	 (pmic_index < PM_MAX_NUM_PMICS))
  {
      return pm_fg_data_arr[pmic_index]->num_of_peripherals;
  }

  return 0;
}

void pm_fg_get_pmic_info(uint8 *pmic_index, uint8 *slave_id)
{
    uint8 i = 0;

    if(!pmic_index || !slave_id)
        return;

    for(i = 0; i < PM_MAX_NUM_PMICS; i++)
    {
        if(pm_fg_data_arr[i])
        {
            *pmic_index = pm_fg_data_arr[i]->comm_ptr->pmic_index;
            *slave_id   = pm_fg_data_arr[i]->comm_ptr->slave_id;
            break;
        }
    }

    return;
}

