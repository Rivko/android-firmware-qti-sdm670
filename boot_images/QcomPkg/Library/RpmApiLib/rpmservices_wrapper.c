/*
===========================================================================
*/
/**
  @file RPM protocol initialization

*/
/*
  ====================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  07/27/15   rli      Initial revision.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

/*
 * External header files.
 */
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIRpmServices.h>
#include <Protocol/EFIRpm.h>

/*=========================================================================
      Prototypes
==========================================================================*/
EFI_STATUS InitRpmProtocol( VOID );
EFI_STATUS DeinitRpmProtocol( VOID );

/*=========================================================================
      Variables
==========================================================================*/
EFI_RPM_PROTOCOL *gRpmProtocol = NULL;


/* =========================================================================
**  Function : InitRpmProtocol
** =========================================================================*/
/*
  Sends RPM services request to publish RPM protocol
*/
EFI_STATUS InitRpmProtocol( VOID )
{
  EFI_RPMSERVICES_PROTOCOL *rpmservices_protocol;
  EFI_STATUS status = gBS->LocateProtocol(&gEfiRpmServicesProtocolGuid, NULL, (void**)&rpmservices_protocol);
  
    if(status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_ERROR, "Locate RpmServicesProtocol failed:%r\n", status));
    }
    else
    {
        rpmservices_protocol->InstallRpmProtocol();
        status =  gBS->LocateProtocol(&gEfiRpmProtocolGuid, NULL, (void**)&gRpmProtocol);
        if(status != EFI_SUCCESS)
        {
            DEBUG ((EFI_D_ERROR, "Locate RpmProtocol failed:%r\n", status));
        }
    }

   return status;
} 

/* =========================================================================
**  Function : DeinitRpmProtocol
** =========================================================================*/
/*
  Sends RPM services request to unpublish RPM protocol
*/
EFI_STATUS DeinitRpmProtocol( VOID )
{
  EFI_RPMSERVICES_PROTOCOL *rpmservices_protocol;
  EFI_STATUS status = gBS->LocateProtocol(&gEfiRpmServicesProtocolGuid, NULL, (void**)&rpmservices_protocol);
  
    if(status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_ERROR, "Locate RpmServicesProtocol failed:%r\n", status));
    }
    else
    {
        rpmservices_protocol->UninstallRpmProtocol();
        status =  gBS->LocateProtocol(&gEfiRpmProtocolGuid, NULL, (void**)&gRpmProtocol);
        if(status == EFI_SUCCESS)
        {
            DEBUG ((EFI_D_ERROR, "Uninstalling RpmProtocol failed:%r\n", status));
        }
        else
        {
           gRpmProtocol = NULL;
           status = EFI_SUCCESS;
        }
    }

   return status;
} 
