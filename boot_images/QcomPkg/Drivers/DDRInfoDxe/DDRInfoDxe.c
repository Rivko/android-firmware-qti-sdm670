/*============================================================================
  FILE:         DDRInfoDxe.c

  OVERVIEW:     Implementation of the DDRGetInfo EFI protocol interface

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary,
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who      what, where, why
  ----------  ---      -----------------------------------------------------------
  2017-04-19  daisond  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIDDRGetConfig.h>
#include <Library/DDRInfoPartitionLib.h>


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*===========================================================================*/
/* EFI_GetDDRDetails */ 
/**
  @par Summary
  Gets information about the DDR

  @param[in]  This          Pointer to the EFI_DDRGETINFO_PROTOCOL instance.
  @param[out] PlatformInfo  Pointer to the DDR information

  @return
  EFI_SUCCESS        -- Function completed successfully.  \n
*/


static EFI_STATUS
EFI_GetDDRDetails(
	IN EFI_DDRGETINFO_PROTOCOL *This,
	OUT struct ddr_details_entry_info	*DdrDetails
	);


EFI_STATUS
EFI_GetDDRDetails (
   IN EFI_DDRGETINFO_PROTOCOL *This,
   OUT struct ddr_details_entry_info    *DdrDetails
   )
{
  EFI_STATUS	  Status;
  if (DdrDetails == NULL)
    return EFI_INVALID_PARAMETER;
  Status = GetDDRDetails(DdrDetails);

  if (Status != EFI_SUCCESS)
    return EFI_UNSUPPORTED;
  

#ifdef PRE_SIL
  return EFI_UNSUPPORTED;
#else
  return EFI_SUCCESS;
#endif
}



STATIC EFI_DDRGETINFO_PROTOCOL DDRGetInfoProtocol = 
{
  EFI_DDRGETINFO_PROTOCOL_REVISION,
  EFI_GetDDRDetails
};



/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
DDRInfoEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{

   EFI_STATUS Status;
   
   Status = gBS->InstallMultipleProtocolInterfaces (&ImageHandle, 
													&gEfiDDRGetInfoProtocolGuid, 
													&DDRGetInfoProtocol, 
													NULL, NULL);
   return Status;

}

