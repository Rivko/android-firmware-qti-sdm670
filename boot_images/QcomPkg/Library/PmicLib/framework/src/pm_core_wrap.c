/*! \file pm_init.c
*   \n
*   \brief This file contains PMIC initialization function which initializes the PMIC Comm
*   \n layer, PMIC drivers and PMIC applications.
*   \n
*   \n &copy; Copyright 2010-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/08/15   aab     Added pm_device_setup()
07/23/15   al      Removing NPA from common
12/06/14   al      enabling npa
11/12/14   al      adding for compilation
07/16/14   akm     Comm change Updates
03/31/14   akm     Cleanup
01/15/13   aab     Fixed KW error
05/10/11   jtn     Fix RPM init bug for 8960
07/01/10   umr     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include <Uefi.h>
#include <Library/TimerLib.h>
#include "com_dtypes.h"
#include "device_info.h"
#include "pm_utils.h"
#include "pm_version.h"
#include "pm_comm.h"

pm_err_flag_type pm_device_setup(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    
	err_flag = pm_comm_channel_init_internal();
  
    err_flag |= pm_version_detect();
	
    return err_flag;
}

