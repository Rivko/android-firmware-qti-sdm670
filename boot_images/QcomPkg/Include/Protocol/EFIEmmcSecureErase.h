/**
  @file EFIEmmcSecureErase.h
  @brief eMMC Secure Erase Protocol definitions
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY

//source/qcom/qct/core/uefi/edk2/dev/pkalapat.pkalapat_CR407053_8930_fixes/QcomPkg/Include/Protocol/EFIEmmcSecureErase.h#1 $

 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 02/03/14    llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/31/13    pa      Initial version.
 
=============================================================================*/
#ifndef __EFI_EMMC_SECURE_ERASE_H__
#define __EFI_EMMC_SECURE_ERASE_H__

/** @cond */
typedef struct _EFI_EMMC_SECURE_ERASE_PROTOCOL   EFI_EMMC_SECURE_ERASE_PROTOCOL;
/** @endcond */

/** @ingroup efi_emmcSecureErase_constants
  Protocol version.
*/
#define EFI_EMMC_SECURE_ERASE_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_emmcSecureErase_protocol */
#define EFI_EMMC_SECURE_ERASE_PROTOCOL_GUID \
   { 0xc3f526f2, 0xe900, 0x4c6c, { 0x84, 0x1, 0xa, 0x9a, 0xf6, 0x5f, 0x70, 0xef } }


/** @cond */
/* External reference to the Emmc Secure Erase Protocol GUID */
extern EFI_GUID gEfiEmmcSecureEraseProtocolGuid;
/** @endcond */

/** @ingroup efi_emmcSecureErase_data_types
  Device information for the secure erase operation. 
*/
typedef struct /** @cond */_EFI_EMMC_SECURE_ERASE_DEVICE_INFO/** @endcond */
{  
   UINT32 EraseGroupSize;  /**< eMMC Secure Erase group size. */
} EFI_EMMC_SECURE_ERASE_DEVICE_INFO;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_EMMC_SECURE_ERASE_GET_DEVICE_INFO */ 
/** @ingroup efi_emmcSecureErase_get_device_info
  @par Summary
  Retrieves the eMMC's information required to perform the secure 
  erase operation.

  @param[in]  This             Pointer to the EFI_EMMC_SECURE_ERASE_PROTOCOL 
                               instance.
  @param[out] EraseDeviceInfo  eMMC's secure erase information; see 
                               #EFI_EMMC_SECURE_ERASE_DEVICE_INFO for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_SECURE_ERASE_GET_DEVICE_INFO)(
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This, 
   OUT EFI_EMMC_SECURE_ERASE_DEVICE_INFO *EraseDeviceInfo
);

/* EFI_EMMC_SECURE_ERASE_BLOCKS */ 
/** @ingroup efi_emmcSecureErase_erase_blocks
  @par Summary
  Performs a secure erase operation on the specified sectors. 

  @param[in] This         Pointer to the EFI_EMMC_SECURE_ERASE_PROTOCOL instance.
  @param[in] SectorStart  Start sector for the secure erase operation.
  @param[in] SectorCount  Number of sectors on which to apply the secure erase 
                          operation.

  @par Description
  This function securely erases the eMMC sectors specified by the SectorStart 
  and SectorCount parameters. If the parameters are valid, the function sends 
  the commands to the eMMC part to apply the secure erase operation on the 
  specified sectors.
  
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  @par
  EFI_INVALID_PARAMETER -- Parameter is invalid, including an invalid
                           alignment of the start and count values. \n
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_SECURE_ERASE_BLOCKS)(
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This, 
   IN UINT32 SectorStart, 
   IN UINT32 SectorCount 
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_emmcSecureErase_protocol
  @par Summary
  eMMC Secure Erase Protocol interface.

  @par Parameters
  @inputprotoparams{emmc_secure_erase_proto_params.tex} 
*/
struct _EFI_EMMC_SECURE_ERASE_PROTOCOL 
{  
   UINT64 Revision;
   EFI_EMMC_SECURE_ERASE_GET_DEVICE_INFO EmmcEraseGetDeviceInfo;  
   EFI_EMMC_SECURE_ERASE_BLOCKS EmmcEraseBlocks; 
};

#endif  /* __EFI_EMMC_SECURE_ERASE_H__*/
