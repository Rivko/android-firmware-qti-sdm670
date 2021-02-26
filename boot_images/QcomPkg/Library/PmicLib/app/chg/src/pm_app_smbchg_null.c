/*! \file
*  
*  \brief  pm_app_smbchg_alg.c
*  \details Implementation file for pmic sbl charging algorithm
*    
 *  \n &copy; Copyright (c) 2012-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/app/chg/src/pm_app_smbchg_null.c#1 $
$DateTime: 2017/09/18 09:26:13 $
$Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/09/15   aab     Added pm_sbl_chg_cfg_cls_platform() to support platform type DALPLATFORMINFO_TYPE_CLS 
02/22/15   aab     Updated pm_sbl_chg_check_weak_battery_status() to support wipower
02/12/15   aab     Added Multiple try to read ADC READ is Ready
11/18/14   aab     Updated SBL charger driver to use Vbatt ADC
11/16/14   aab     Disabled log messages
10/15/14   aab     Added pm_sbl_config_chg_parameters() 
10/14/14   aab     Added pm_sbl_config_fg_sram(). Updated pm_sbl_chg_config_vbat_low_threshold()
08/20/14   aab     Updated to get DBC bootup threshold voltage from Dal Config.  Enabled DBC.
06/24/14   aab     Updated pm_sbl_chg_check_weak_battery_status() to include RED LED blinking
04/28/14   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "pm_app_smbchg.h"

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/


/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/


/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type pm_sbl_chg_check_weak_battery_status(uint32 device_index)
{
   (void)device_index;
   return PM_ERR_FLAG_SUCCESS;
}



pm_err_flag_type pm_sbl_config_fg_sram(uint32 device_index)
{
    (void)device_index;
    return PM_ERR_FLAG_SUCCESS;
}




pm_err_flag_type pm_sbl_config_chg_parameters(uint32 device_index)
{
   (void)device_index;
   return PM_ERR_FLAG_SUCCESS;
}
 

pm_err_flag_type pm_sbl_chg_cfg_cls_platform(uint32 device_index)
{
   (void)device_index;
   return PM_ERR_FLAG_SUCCESS;
}

