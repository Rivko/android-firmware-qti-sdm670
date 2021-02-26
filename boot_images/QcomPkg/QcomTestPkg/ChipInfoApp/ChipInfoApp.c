/** @file ChipInfoApp.c
   
  This file contains test ChipInfo driver entry point.  On 
  UEFI, there are two different ways to use ChipInfo driver:
  (1) DAL framework (2) EFI ChipInfo protocol.  This 
  application shows sample code for both options. 

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/16/11   pbi      Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include "DALStdDef.h"
#include "DALStdErr.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Function Prototypes
==========================================================================*/
extern DALResult ChipInfoTestUsingDALFramework(void);
extern EFI_STATUS ChipInfoTestUsingEFIProtocol(void);


/*=========================================================================
      Functions
==========================================================================*/


/**
  Test ChipInfo application entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

EFI_STATUS
EFIAPI
ChipInfoAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{

  EFI_STATUS status;
  DALResult result;
  /*
   * There are two ways to use driver on UEFI: 
   * (1) Pure DAL based interface 
   * (2) UEFI protocol interface 
   *  
   * The following code shows both ways.  Please note the clients/users
   * of the ChipInfo driver can pick either option.
   */
  
  /* Option (1) */
  result = ChipInfoTestUsingDALFramework();
  if (DAL_SUCCESS != result) {
    return EFI_UNSUPPORTED;    
  }
  /* Option (2) */
  status = ChipInfoTestUsingEFIProtocol();

  return status;
}

