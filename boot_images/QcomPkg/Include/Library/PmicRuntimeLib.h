#ifndef __PMICRUNTIMELIB_H___
#define __PMICRUNTIMELIB_H___

/*===========================================================================

                     PMIC RUNTIME SERVICES HEADER FILE

DESCRIPTION
  This file contains functions prototypes and variable/type/constant 
  declarations for the RUNTIME services developed for the Qualcomm Power
  Management IC.
  
Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/11   dy      Created.
===========================================================================*/

/* =========================================================================
                      INCLUDED RUNTIME SERVICES
========================================================================= */
#include "PmicShutdown.h"

/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/**
  Initialize any infrastructure required for Lib to function.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_STATUS    Returns the status of PmicRuntimeLibInitialize.
**/
EFI_STATUS
EFIAPI
PmicRuntimeLibInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
);

#endif // __PMICRUNTIMELIB_H___


