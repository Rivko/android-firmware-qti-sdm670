/*! \file
*  
*  \brief  pm_app_haptics.c
*  \details Implementation file for rgb led resourece type.
*    
*  &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/26/17   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_haptics.h"
#include "pm_app_haptics.h"
#include "pm_target_information.h"
#include "device_info.h"
#include "CoreVerify.h"


pm_err_flag_type  
pm_app_haptics_vib_ctrl(pm_on_off_type state) 
{ 
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS; 
   pm_device_index_type *device_index_ptr = NULL;
   uint8 pmi_device_index;
 
   device_index_ptr = (pm_device_index_type*)pm_target_information_get_specific_info(PM_PROP_PM_DEVICE_INDEX_DATA);
   CORE_VERIFY_PTR(device_index_ptr);
   if(device_index_ptr == NULL)
   {
	   return PM_ERR_FLAG_INVALID_PARAMETER; 
   }
   pmi_device_index = device_index_ptr->interface_pmic_index;

   if ((state != PM_ON) && (state != PM_OFF))
   {
      return PM_ERR_FLAG_INVALID_PARAMETER; 
   }

   if (state == PM_ON) 
   { 
      err_flag |= pm_haptics_play(pmi_device_index, PM_HAPTICS_PLAY); 
   } 
   else
   { 
      err_flag |= pm_haptics_play(pmi_device_index, PM_HAPTICS_PAUSE); 
   }

   err_flag |= pm_haptics_enable(pmi_device_index, state); 

   return err_flag; 
}

