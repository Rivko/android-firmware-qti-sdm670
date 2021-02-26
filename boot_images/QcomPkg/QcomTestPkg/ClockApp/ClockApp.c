/** @file ClockApp.c

  This file contains test clock driver entry point.  On UEFI,
  there are two different ways to use clock driver: (1) DAL
  framework (2) EFI clock protocol.  This application shows
  sample code for both options.

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/23/10   vs      Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Function Prototypes
==========================================================================*/
//extern void ClockTestUsingDALFramework(void);
//extern void ClockTestUsingEFIProtocol(void);
extern void ClockBISTUsingEFIProtocol(void);
extern void ClockTestLPM(void);
extern void ClockTestDispClocks(void);

/*=========================================================================
      Functions
==========================================================================*/


/**
  Test clock application entry point.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
ClockAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{

  /*
   * There are two ways to use driver on UEFI:
   * (1) Pure DAL based interface
   * (2) UEFI protocol interface
   *
   * The following code shows both ways.  Please note the clients/users
   * of the clock driver can pick either option.
   */
   ClockBISTUsingEFIProtocol();
   /* Option (1) */
   //ClockTestUsingDALFramework();

   /* Option (2) */

#if 0
   ClockTestDispClocks();
   ClockTestLPM();
#endif
   return EFI_SUCCESS;
}

