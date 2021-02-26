/** @file SPIApi.c
   
  SPI API code

  Copyright (c) 2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/05/14   sg      Created

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

/* SPI Interfaces */
#include <Protocol/EFISPI.h>

static EFI_QCOM_SPI_PROTOCOL *spi_io = NULL;
  
/* spi_open */ 
spi_status
spi_open(
    spi_instance instance,
    void **spi_handle
)
{
  EFI_STATUS estatus = EFI_SUCCESS;

  /* Get the SPI protocol handle */
  estatus = gBS->LocateProtocol(&gQcomSPIProtocolGuid, NULL, (VOID **) &spi_io);
  if ((EFI_SUCCESS != estatus) || (NULL == spi_io))
  {
    DEBUG((EFI_D_ERROR, "Failed to get SPI protocol 0x%08x\n", estatus));
    return SPI_ERROR_DRV_FWK_INIT;
  }

  return spi_io->open(
      instance,
      spi_handle);
}


/* spi_transfer */
spi_status 
spi_transfer( 
		void	*spi_handle,
		SpiDeviceInfoType	*devInfo,
		CONST UINT8	*write_buffer,
		UINT32	write_len,
		UINT8	*read_buffer,
		UINT32	read_len
)
{
  if (spi_io == NULL)
  {
    DEBUG((EFI_D_ERROR, "SPI protocol handle is invalid\n"));
    return SPI_ERROR_DRV_FWK_NULL_HANDLE;
  }

  return spi_io->transfer(
      spi_handle,
      devInfo,
      write_buffer,
      write_len,
      read_buffer,
      read_len);
}

/* spi_close */ 
spi_status
spi_close(
    void *spi_handle
)
{
  if (spi_io == NULL)
  {
    DEBUG((EFI_D_ERROR, "SPI protocol handle is invalid\n"));
    return SPI_ERROR_DRV_FWK_NULL_HANDLE;
  }

  return spi_io->close(
      spi_handle);
}