/** 
  @file  EFISPMI.h
  @brief SPMI UEFI Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2012,2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/20/13   llg     (Tech Pubs) Edited function syntax; otherwise, Doxygen will not run
 06/21/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 09/20/12   unr     Added ReadModifyWriteLongByte API
 07/05/12   unr     API changes as per UEFI standard
 06/27/12   unr     Initial revision for UEFI

=============================================================================*/

#ifndef __EFISPMI_H__
#define __EFISPMI_H__

#include "SpmiTypes.h"

/** @ingroup efi_spmi_constants
  Protocol version. 
*/
#define SPMI_DXE_REVISION 0x0000000000010000

/** @cond */
/*
  SPMI UEFI typedefs
*/
typedef struct _EFI_QCOM_SPMI_PROTOCOL   EFI_QCOM_SPMI_PROTOCOL;
/** @endcond */

/*  SPMI UEFI Procotol GUID */
/** @ingroup efi_spmi_protocol */
extern EFI_GUID gQcomSPMIProtocolGuid;

/*-------------------------------------------------------------------------
* Function Declarations and Documentation
* ----------------------------------------------------------------------*/
/* SPMI_READLONG */
/** @ingroup spmi_readlong
  @par Summary
  Reads data from an SPMI slave device. The register address is in 
  long (16-bit) format.

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  uRegisterAddress    Register address on the SPMI slave device 
                                  from which the read is to be initiated. 
                                  16 LSB bits of this parameter are used as 
                                  the register address.
  @param[in]  pucData             Pointer to a data array. Data read from the 
                                  SPMI bus is filled in this array.
  @param[in]  uDataLen            Number of bytes to read.
  @param[out] puTotalBytesRead    Pointer to a UINT32 that is used to 
                                  return the total number of bytes read from 
                                  the SPMI device.
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.

  @sa
  SPMI_WRITELONG()
*/
typedef Spmi_Result (*SPMI_READLONG) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress, 
    UINT8 *pucData,
    UINT32 uDataLen, 
    UINT32 *puTotalBytesRead
    );


/* SPMI_WRITELONG */
/** @ingroup spmi_writelong
  @par Summary
  Writes data to an SPMI slave device. The register address is in 
  long (16-bit) format.

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  uRegisterAddress    Register address on the SPMI slave device 
                                  from which the write is to be initiated. 
                                  16 LSB bits of this parameter are used as 
                                  the register address.
  @param[in]  pucData             Pointer to a data array containing data to 
be written on the bus.
  @param[in]  uDataLen            Number of bytes to write. 
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.

  @sa
  SPMI_READLONG()
*/
typedef Spmi_Result (*SPMI_WRITELONG) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData, 
    UINT32 uDataLen
    );


/* SPMI_READMODIFYWRITELONGBYTE */
/** @ingroup spmi_readmodifywritelongbyte
  @par Summary
  Reads, modifies, and writes a byte on an SPMI slave device.
  @par
  This function reads a byte from an SPMI slave device, modifies it, 
  and writes it back to the SPMI slave device. The register address is 
  in long (16-bit) format. 

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  uRegisterAddress    Register address on the SPMI slave device 
                                  to which the write is to be initiated. 
                                  16 LSB bits of this parameter are used as 
                                  the register address.
  @param[in]  uWriteData          Data byte to be read, modified, and written. 
  @param[in]  uMask               Mask of the bits that must be modified.
  @param[out] pucDataWritten      Pointer to a UINT8 that is used to return 
                                  the value of the byte written on the bus 
                                  after the read, modify, and write.
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult.
*/
typedef Spmi_Result (*SPMI_READMODIFYWRITELONGBYTE) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT32 uWriteData, 
    UINT32 uMask,
    UINT8 *pucDataWritten
    );


/* SPMI_COMMAND */
/** @ingroup spmi_command
  @par Summary
  Sends a command to an SPMI slave device. 

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  eSpmiCommand        Command type; see #SpmiBus_CommandType for 
                                  details. 
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.
*/
typedef Spmi_Result (* SPMI_COMMAND) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    Spmi_Command eSpmiCommand
    ); 

/* SPMI_READLONGDEBUG */
/** @ingroup spmi_readlongdebug
  @par Summary
  Reads data from an SPMI slave device. The register address is in 
  long (16-bit) format.

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  uRegisterAddress    Register address on the SPMI slave device 
                                  from which the read is to be initiated. 
                                  16 LSB bits of this parameter are used as 
                                  the register address.
  @param[in]  pucData             Pointer to a data array. Data read from the 
                                  SPMI bus is filled in this array.
  @param[in]  uDataLen            Number of bytes to read.
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.

  @sa
  SPMI_WRITELONG()
*/
typedef Spmi_Result (*SPMI_READLONGDEBUG) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress, 
    UINT8 *pucData,
    UINT32 uDataLen
    );


/* SPMI_WRITELONGDEBUG */
/** @ingroup spmi_writelongdebug
  @par Summary
  Writes data to an SPMI slave device. The register address is in 
  long (16-bit) format.

  @param[in]  SpmiProtocolHandle  UEFI I/O protocol handle.
  @param[in]  uSlaveId            Slave ID of the device.
  @param[in]  eAccessPriority     Priority with which the command must 
                                  be sent on the SPMI bus; see 
                                  #SpmiBus_AccessPriorityType for details.
  @param[in]  uRegisterAddress    Register address on the SPMI slave device 
                                  from which the write is to be initiated. 
                                  16 LSB bits of this parameter are used as 
                                  the register address.
  @param[in]  pucData             Pointer to a data array containing data to 
be written on the bus.
  @param[in]  uDataLen            Number of bytes to write. 
  @param[out] eResult             Error code in case of an error; 
                                  see #SpmiBus_ResultType for details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  EFI_DEVICE_ERROR -- Operation failed; the error code is returned in eResult. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.

  @sa
  SPMI_READLONG()
*/
typedef Spmi_Result (*SPMI_WRITELONGDEBUG) (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData, 
    UINT32 uDataLen
    );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_spmi_protocol
  @par Summary
  Qualcomm System Power Management Interface (SPMI) Protocol interface.

  @par Parameters
  @inputprotoparams{spmi_proto_params.tex} 
*/
struct _EFI_QCOM_SPMI_PROTOCOL {
  UINT64                        Revision;
  SPMI_READLONG                 ReadLong;
  SPMI_WRITELONG                WriteLong;
  SPMI_READMODIFYWRITELONGBYTE  ReadModifyWriteLongByte;
  SPMI_COMMAND                  Command;
  SPMI_READLONGDEBUG            ReadLongDebug;
  SPMI_WRITELONGDEBUG           WriteLongDebug;
};

#endif	/* __EFISPMI_H__ */
