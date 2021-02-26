/** 
  @file  EFISPI.h
  @brief SPI UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
  EDIT HISTORY


  when       who     what, where, why
  --------   ---     -----------------------------------------------------------
  09/23/14   sg      XBL changes
  05/01/14   sg      Updated new device parameter, HS mode
  01/26/12   vishalo Merge in Techpubs Doxygen change  
  11/02/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
  05/28/11   unr     Initial revision for UEFI

  =============================================================================*/

#ifndef __EFISPI_H__
#define __EFISPI_H__

#include "SpiApi.h"

/** @addtogroup efi_spi_constants 
@{ */
/** 
  Protocol version.
*/


/**
  SPI UEFI typedefs
 */
typedef struct _EFI_QCOM_SPI_PROTOCOL   EFI_QCOM_SPI_PROTOCOL;
/** @endcond */

/*  SPI UEFI Procotol GUID */
/** @ingroup efi_spi_protocol */
extern EFI_GUID gQcomSPIProtocolGuid;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* spi_open */ 
/** @ingroup efi_spi_open
  @par Summary
  This function is called by the client code to initialize the respective 
  GSBI SPI core used by the client. On success, SPI_Handle points to the handle 
  for the SPI core. 

  @note1hang This function must be called before any other functions are 
             called.

  @param[in]  instance              SPI core that the client intends to
                                    initialize; see #EFI_QCOM_SPI_CORE for 
                                    details.
  @param[out] spi_handle            Pointer location to be filled by the
                                    driver with a handle to the core.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
spi_status
(EFIAPI *OPEN) (
    spi_instance instance,
    void **spi_handle
);

/* SPI_TRANSFER */ 
/** @ingroup efi_spi_transfer
  @par Summary
  This function is called by the client code to transfer data.

  @param[in] spi_handle             Handle to the SPI core.
  @param[in] SpiDeviceInfoType      SPI device and transfer information; 
                                    see #EFI_SPI_DEVICE_INFO_TYPE for details.
  @param[in] write_buffer           Buffer that contains the data bytes to be
                                    written to the slave.
  @param[in] write_len              Number of bytes to be written to the slave.
  @param[out] read_buffer           Buffer containing the bytes read from the
                                    slave.
  @param[in]  read_len              Number of bytes to be read from the slave.

  @return
  See #EFI_SPI_PD_STATUS_TYPE for details.

  @dependencies
  spi_open() must have already been called.
*/
typedef
spi_status
(EFIAPI *TRANSFER) (
    void	*spi_handle,
	SpiDeviceInfoType	*devInfo,
	CONST UINT8	*write_buffer,
	UINT32	write_len,
	UINT8	*read_buffer,
	UINT32	read_len
);

/* spi_close */ 
/** @ingroup efi_spi_close
  @par Summary
  De-initializes the SPI core. After spi_close() is called, read/write 
  transfers are not allowed.

  @param[in] SPI_Handle             Handle to the SPI core.

  @return
  EFI_SUCCESS      -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Physical device reported an error.

  @dependencies
  spi_open() must have already been called.
*/
typedef
spi_status
(EFIAPI *CLOSE) (
    void *spi_handle
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_spi_protocol
  @par Summary
  Qualcomm Serial Peripheral Interface (SPI) Protocol interface.

  @par Parameters
  @inputprotoparams{spi_proto_params.tex} 
*/
struct _EFI_QCOM_SPI_PROTOCOL {
  UINT64        Revision;
  OPEN          open;
  TRANSFER      transfer;
  CLOSE         close;
};
#endif  /* __EFISPI_H__ */
