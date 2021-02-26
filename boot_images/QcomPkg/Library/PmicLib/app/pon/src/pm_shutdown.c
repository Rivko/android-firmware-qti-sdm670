/*===========================================================================
                        PMIC SHUTDOWN PROTOCOL
   DESCRIPTION
   This file contains functions prototypes and variable/type/constant
   declarations for supporting Shutdown Services for the Qualcomm 
   PMIC chip set.


   Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/16/15   al      Porting latest
===========================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "PmicShutdown.h"
#include <Uefi.h>
#include "pm_err_flags.h"
#include "pm_app_pon.h"

/*===========================================================================
                  FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION PmicShutdown                                EXTERNAL FUNCTION  
DESCRIPTION
    This function disables SMPL bit and writes normal shutdown to PS_HOLD_RESET_CTL register. With these features 
  disabled, PMIC shuts off if PS_HOLD pin goes low.

  EXAMPLE:
  // shutdown the PMIC. 
  EFI_STATUS Status = PmicShutdown();

  // Was shutdown successful? 
  if (EFI_SUCCESS == Status) 
  {
    //Pull down PS_HOLD
  }

RETURN VALUE
  Return value type: EFI_STATUS.
    - EFI_SUCCESS  = PS_HOLD is ready to be pulled down
    - EFI_DEVICE_ERROR = Shutdown failed to disable the WDOG and SMPL bits.
===========================================================================*/
EFI_STATUS
EFIAPI
PmicShutdown()
{
    return PmicReset(PM_PON_RESET_CFG_NORMAL_SHUTDOWN);
}

EFI_STATUS
EFIAPI
PmicHardReset()
{
    return PmicReset(PM_PON_RESET_CFG_HARD_RESET);
}


EFI_STATUS
EFIAPI
PmicReset(pm_pon_reset_cfg_type Reset)
{
   EFI_STATUS Status = EFI_SUCCESS;
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

   switch(Reset)
   {
   case PM_PON_RESET_CFG_WARM_RESET:
        err_flag |= pm_app_pon_pshold_cfg(PON_WARM_RST);
        break;
   case PM_PON_RESET_CFG_NORMAL_SHUTDOWN:
        err_flag |= pm_app_pon_pshold_cfg(PON_SHDN);
        break;
   case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN:
        err_flag |= pm_app_pon_pshold_cfg(PON_DVDD_SHDN);
        break;
   case PM_PON_RESET_CFG_HARD_RESET:
        err_flag |= pm_app_pon_pshold_cfg(PON_HARD_RST);
        break;
   case PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET:
        err_flag |= pm_app_pon_pshold_cfg(PON_DVDD_HARD_RST);
        break;
   /*currently below are not supported*/
   case PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
   case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:
   case PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
   case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_SHUTDOWN:
   case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_SHUTDOWN:   
   case PM_PON_RESET_CFG_WARM_RESET_THEN_SHUTDOWN:                          
   case PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET:                       
   case PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET:     
   case PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET:
   default:
      return EFI_DEVICE_ERROR;
   }

   if (err_flag != PM_ERR_FLAG_SUCCESS) 
   {
      Status = EFI_DEVICE_ERROR;
   }

   return Status;
}


