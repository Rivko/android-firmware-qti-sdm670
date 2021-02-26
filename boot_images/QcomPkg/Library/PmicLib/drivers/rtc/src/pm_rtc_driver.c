/*! \file
 *  \n
 *  \brief  pm_rtc_driver.c
 *  \details  
 *  \n &copy; Copyright 2010-2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14    al      Updating comm lib 
08/29/14    al      KW fixes
05/20/14    al      Architecture update
05/09/14    va      Using common debug and assert Marco
08/01/13    al      New arch
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc_driver.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_utils.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the RTC data */
static pm_rtc_data_type *pm_rtc_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_rtc_data_type *rtc_ptr = NULL;

    rtc_ptr = pm_rtc_data_arr[pmic_index];

    if(NULL == rtc_ptr)
    {
        pm_malloc(sizeof(pm_rtc_data_type), (void **)&rtc_ptr);
        pm_rtc_data_arr[pmic_index] = rtc_ptr;
   }
    if (rtc_ptr != NULL)
    {
        /* Assign Comm ptr */
        rtc_ptr->comm_ptr = comm_ptr;

        /* RTC Register Info - Obtaining Data through dal config */
        rtc_ptr->rtc_reg_table = (pm_rtc_register_info_type *)pm_target_information_get_common_info(PM_PROP_RTC_REG);
        CORE_VERIFY_PTR(rtc_ptr->rtc_reg_table);

        //rtc_ptr->num_of_peripherals = (uint32)PM_MAX_NUM_RTC;
        rtc_ptr->num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_RTC_NUM, pmic_index);
        CORE_VERIFY(rtc_ptr->num_of_peripherals != 0);
    }
}

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_rtc_data_arr[pmic_index];
    }

    return NULL;
}


uint8 pm_rtc_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_rtc_data_arr[pmic_index] != NULL))
  {
      return pm_rtc_data_arr[pmic_index]->num_of_peripherals;
  }

    return 0;
}
