/*============================================================================
@file ChargerTest.h

ChargerTest header file

Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/29/14   va      New File

=============================================================================*/

#ifndef CHARGERTEST_H
#define CHARGERTEST_H


/*=========================================================================
      Include Files
==========================================================================*/
   
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/EFIChargerEx.h>
#include <Library/QcomLib.h>

/* Include ChargerTest helper */
#include "ChargerTestHelper.h"
/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Function Prototype
==========================================================================*/


/* Sleep without burning CPU cycles */
EFI_STATUS WaitForEventTimeout (IN UINT32   Timeoutms);


#endif // CHARGERTEST_H

