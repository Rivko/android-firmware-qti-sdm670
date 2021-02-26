/** 
  @file  EFII2C.h
  @brief I2C UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2010-2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/19/14   unr     API modified for XBL support
 11/14/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/03/12   vishalo Merge in Techpubs Doxygen change
 03/20/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/26/12   vishalo Merge in Techpubs Doxygen change
 11/02/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 12/23/10   unr     Initial revision for UEFI

=============================================================================*/

#ifndef __EFII2C_H__
#define __EFII2C_H__

#include "i2c_api.h"

/** @addtogroup efi_i2c_constants 
@{ */
/** 
  Protocol version. 
*/
#define I2C_DXE_REVISION 0x0000000000010000
/** @} */ /* end_addtogroup efi_i2c_constants */

/**
  I2C UEFI typedefs
*/
typedef struct _EFI_QCOM_I2C_PROTOCOL   EFI_QCOM_I2C_PROTOCOL;
/** @endcond */

/*  I2C UEFI Procotol GUID */
/** @ingroup efi_i2c_protocol */
extern EFI_GUID gQcomI2CProtocolGuid;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* OPEN */ 
/** @ingroup OPEN
  @par Summary
  This function is called by the client code to initialize the respective 
  I2C core instance used by the client. On success, i2c_handle points to the handle 
  for the I2C core. 
  
  @param[in]  instance              I2C core instance that the client intends to
                                    initialize; see #i2c_instance for 
                                    details.
  @param[out] i2c_handle            Pointer location to be filled by the
                                    driver with a handle to the core.

  @return
  See #i2c_status for details.
*/
typedef
i2c_status
(EFIAPI *OPEN) (
    i2c_instance instance,
    void **i2c_handle
);

/* READ */
/** @ingroup READ
  @par Summary
  Performs an i2c read. Optionally reads from an offset.

  @param[in]  i2c_handle            Handle to the I2C core.
  @param[in]  config                Slave configuration. See #i2c_config for
                                    details.
  @param[in]  offset                If offset_len is non zero, this parameter
                                    specifies either the register or memory
                                    offset to read data from.
                                    #i2c_transfer for details.
  @param[in]  offset_len            Length of the offset. For simple slaves,
                                    this is 0. For register addressed slaves, it
                                    is 1 and for memory addressed slaves it is
                                    2.
  @param[out] buffer                Buffer into which data is read.
  @param[in]  buffer_len            Length of data that needs to be read.
                                    the slave.
  @param[out] read                  Number of bytes actually read.
  @param[in]  timeout_ms            Timeout value in milliseconds for transfer
                                    to complete.

  @return 
  See #i2c_status for details.
*/
typedef
i2c_status
(EFIAPI *READ) (
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *read,
    uint32 timeout_ms
);

/* WRITE */
/** @ingroup WRITE
  @par Summary
  Performs an i2c write. Optionally writes to an offset.

  @param[in]  i2c_handle            Handle to the I2C core.
  @param[in]  config                Slave configuration. See #i2c_config for
                                    details.
  @param[in]  offset                If offset_len is non zero, this parameter
                                    specifies either the register or memory
                                    offset to write data to.
                                    #i2c_transfer for details.
  @param[in]  offset_len            Length of the offset. For simple slaves,
                                    this is 0. For register addressed slaves, it
                                    is 1 and for memory addressed slaves it is
                                    2.
  @param[out] buffer                Buffer from which data is written.
  @param[in]  buffer_len            Length of data that needs to be written.
                                    the slave.
  @param[out] written               Number of bytes actually written.
  @param[in]  timeout_ms            Timeout value in milliseconds for transfer
                                    to complete.

  @return 
  See #i2c_status for details.
*/
typedef
i2c_status
(EFIAPI *WRITE) (
    void *i2c_handle,
    i2c_slave_config *config,
    uint16 offset,
    uint16 offset_len,
    uint8  *buffer,
    uint16 buffer_len,
    uint32 *written,
    uint32 timeout_ms
);

/* TRANSFER */ 
/** @ingroup TRANSFER
  @par Summary
  Transfers an array of I2C transfers.

  @param[in]  i2c_handle            Handle to the I2C core.
  @param[in]  config                Slave configuration. See #i2c_config for
                                    details.
  @param[in]  transfers             An array of read and write trasnfers. See
                                    #i2c_transfer for details.
  @param[out] no_of_transfers       Number of transfers in the transfers array.
  @param[out] written               Number of bytes successfully written to
                                    the slave.
  @param[out] read                  Number of bytes actually read.
  @param[in]  timeout_ms            Timeout value in milliseconds for transfer
                                    to complete.

  @return 
  See #i2c_status for details.
*/
typedef
i2c_status
(EFIAPI *TRANSFER) (
    void *i2c_handle,
    i2c_slave_config *config,
    i2c_descriptor *desc,
    uint16 num_descriptors,
    callback_func func,
    void *ctxt,
    uint32 delay,
    uint32 *transferred
);

/* CLOSE */ 
/** @ingroup CLOSE
  @par Summary
  De-initializes the I2C core.

  @param[in] i2c_handle             Handle to the I2C core.

  @return
  See #i2c_status for details.
*/
typedef
i2c_status
(EFIAPI *CLOSE) (
    void *i2c_handle
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_i2c_protocol
  @par Summary
  Qualcomm Inter-Integrated Circuit (I2C) Protocol interface.

  @par Parameters
  @inputprotoparams{i2c_proto_params.tex} 
*/
struct _EFI_QCOM_I2C_PROTOCOL {
  UINT64        Revision;
  OPEN          open;
  READ          read;
  WRITE         write;
  TRANSFER      transfer;
  CLOSE         close;
};

#endif	/* __EFII2C_H__ */
