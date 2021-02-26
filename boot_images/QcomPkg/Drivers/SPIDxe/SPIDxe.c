/** @file SPIDxe.c
   
  SPI UEFI Driver code

  Copyright (c) 2010-2014, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/23/14   sg      XBL changes.
 28/04/14   sg      Added new device parameter, HS mode .
 12/11/12   unr     Power down before de init
 07/28/11   unr     Added DALSYS_DeInitMod to fix UEFI bootup issues
 02/28/11   unr     Initial revision of UEFI SPI Driver

=============================================================================*/

#include <Uefi.h>
#include <Protocol/EFISPI.h>
#include <Library/UefiBootServicesTableLib.h>
#include "DALSys.h"

#define SPI_DXE_REVISION 0x0000000000010000

/**
  SPI UEFI Protocol implementation
 */
EFI_QCOM_SPI_PROTOCOL SPIProtocolImplementation = 
{
  SPI_DXE_REVISION,
  spi_open,
  spi_transfer,
  spi_close,
};

/**
  SPI DXE Driver Entry Point
 */
EFI_STATUS
SPIProtocolInit (
	IN EFI_HANDLE         ImageHandle,
	IN EFI_SYSTEM_TABLE   *SystemTable)
{
	EFI_STATUS Status;

    DALSYS_InitMod(NULL);

	Status = gBS->InstallMultipleProtocolInterfaces(
			&ImageHandle,
			&gQcomSPIProtocolGuid,
			&SPIProtocolImplementation,
			NULL);

	return Status;
}

