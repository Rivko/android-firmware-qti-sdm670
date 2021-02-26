#ifndef __PMICSHUTDOWN_H__
#define __PMICSHUTDOWN_H__

/*===========================================================================

                     PMIC SHUTDOWN SERVICES HEADER FILE

DESCRIPTION
  This file contains functions prototypes and variable/type/constant 
  declarations for the Shutdown services developed for the Qualcomm Power
  Management IC.
  
Copyright (c) 2011 - 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/17/15   al      Updating path for pm_pon.h file  
03/26/14   al      Updating header file 
07/01/13   al      Add header file Uefi.h   
06/20/11   dy      Created.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Uefi.h>
#include "../api/pmic/pm/pm_pon.h"

/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/*===========================================================================
FUNCTION PmicShutdown                                EXTERNAL FUNCTION  
DESCRIPTION
    This function disables the PMICs watchdog reset detect 
  feature and disable SMPL bit. With these features 
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
PmicShutdown
(
  void
);


/*===========================================================================
FUNCTION PmicHardReset                                 
DESCRIPTION
    This function performs hard reset if PS_HOLD pin goes low.

  EXAMPLE:
  // shutdown the PMIC. 
  EFI_STATUS Status = PmicHardReset();

  // Was shutdown successful? 
  if (EFI_SUCCESS == Status) 
  {
    //Pull down PS_HOLD
  }

RETURN VALUE
  Return value type: EFI_STATUS.
    - EFI_SUCCESS  
    - EFI_DEVICE_ERROR 
===========================================================================*/
EFI_STATUS
EFIAPI
PmicHardReset
(
  void
);


/*===========================================================================
FUNCTION PmicReset                                 
DESCRIPTION:
    This function configures reset for PS_HOLD.
PARAM:  Reset 
    Type of PMIC reset to be performed. Supported resets are as below:
      for warm reset:                          PM_PON_RESET_CFG_WARM_RESET
      for immediate XVDD shutdown:             PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN
      for normal shutdown:                     PM_PON_RESET_CFG_NORMAL_SHUTDOWN
      for DVDD shutdown:                       PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN
      for XVDD shutdown:                       PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN
      for normal hard reset:                   PM_PON_RESET_CFG_HARD_RESET 
      for DVDD hard reset :                    PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET
      for XVDD hard reset :                    PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET 
      for warm reset and DVDD shutdown:        PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN
      for warm reset and XVDD shutdown:        PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN
      for warm reset and shutdown:             PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN 
      for warm reset and then hard reset:      PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET
      for warm reset and then DVDD hard reset: PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET
      for warm reset and then XVDD hard reset: PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET

  EXAMPLE:
  // shutdown the PMIC. 
  EFI_STATUS Status = PmicReset();

  // Was shutdown successful? 
  if (EFI_SUCCESS == Status) 
  {
    //Pull down PS_HOLD
  }

RETURN VALUE
  Return value type: EFI_STATUS.
    - EFI_SUCCESS  
    - EFI_DEVICE_ERROR 
===========================================================================*/
EFI_STATUS
EFIAPI
PmicReset
(
  pm_pon_reset_cfg_type Reset
);


/*===========================================================================
FUNCTION PmicShutdownInit                                 
DESCRIPTION
    This function initializes PmicShutdown feature for runtime. This must be called before configuring any kind of PMIC reset.

  EXAMPLE:
  // shutdown the PMIC. 
  EFI_STATUS Status = PmicShutdownInit();


RETURN VALUE
  Return value type: EFI_STATUS.
    - EFI_SUCCESS  
    - EFI_DEVICE_ERROR 
===========================================================================*/

EFI_STATUS
EFIAPI
PmicShutdownInit
(
  void
);

#endif // __PMICSHUTDOWN_H__

