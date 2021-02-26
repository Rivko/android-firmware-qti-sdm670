/** @file FuseControlLibNull.c
   
  Contains dummy library functions related to fuses.
 
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights
  Reserved. Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/11/13   shl     Added new line at the end of file
 05/31/13   shl     Added check production device
 03/22/13   ag      enable fusecontrol check 
 02/01/13   shl     Warning fix
 09/11/12   shl     Added Not block checking
 05/25/12   niting  Created
=============================================================================*/

#include <Library/FuseControlLib.h>
#include "msmhwiobase.h"
#include "HALhwioSecCtrl.h"


/**
  Check if the necessary conditions are satisfied to auto provision the RPMB

  @retval  FALSE   Auto Provisioning disabled

**/
BOOLEAN 
EFIAPI
AutoProvisionRPMBEnabled(void)
{
  return FALSE;
}

/**
 Check if the debug disable fuses are blown

  @retval FALSE None of the related DEBUG_DISABLE fuses are blown

**/
BOOLEAN 
EFIAPI
JTAGDebugDisableFusesBlown(void)
{
  return FALSE;
}

/**
 Check if this is a production device

  @retval FALSE if it is not

**/
BOOLEAN 
EFIAPI
IsProductionDevice(void)
{
  return FALSE;
}

