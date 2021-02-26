/** @file npawrapper_init.c
   
  Map NPA framework init routines in common code to NPA DXE interface

  Copyright (c) 2013-2015, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/05/15   rli     Add error log if NPA protocol is not located
03/27/13   rli     Initial revision.
=============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFINpa.h>


/**
  @brief npa_init - Initializes the NPA subsystem. No other NPA functions may
  be invoked prior to npa_init.
*/
void npa_init(void)
{
    EFI_NPA_PROTOCOL * npa_protocol;
    EFI_STATUS status;
        status = gBS->LocateProtocol(&gEfiNpaProtocolGuid, NULL, (void**)&npa_protocol);
        if(status == EFI_SUCCESS)
        {
            npa_protocol->NpaInit();
        }
        else
        {
            DEBUG ((EFI_D_ERROR, "Locate NpaProtocol failed:%r\n", status));
        }
}



/**
  @brief npa_timer_init - Sets up a marker to indicate to NPA that the timer
  subsystem is up. Invoked from rcinit.
*/
void npa_timer_init( void );
