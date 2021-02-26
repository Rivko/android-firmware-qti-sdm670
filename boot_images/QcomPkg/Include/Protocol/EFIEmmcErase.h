/**
  @file EFIEmmcErase.h
  @brief eMMC Erase Protocol definitions
*/
/*=============================================================================
  Copyright (c) 2012 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 10/19/12    llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 10/04/12    sb      Initial version.
 
=============================================================================*/
#ifndef __EFI_EMMC_ERASE_H__
#define __EFI_EMMC_ERASE_H__


/** @cond */
typedef struct _EFI_EMMC_ERASE_PROTOCOL   EFI_EMMC_ERASE_PROTOCOL;
/** @endcond */

/** @ingroup efi_emmcErase_constants
  Protocol version.
*/
#define EFI_EMMC_ERASE_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_emmcErase_protocol */
#define EFI_EMMC_ERASE_PROTOCOL_GUID \
   { 0x913620ed, 0x8ea, 0x4ac0, { 0x8b, 0xba, 0xd8, 0x8a, 0x9d, 0xaa, 0xb7, 0x51 } }


/* External reference to the Emmc Erase Protocol GUID */
/** @cond */
extern EFI_GUID gEfiEmmcEraseProtocolGuid;
/** @endcond */

/** @ingroup efi_emmcErase_data_types
  Device information for the erase operation. 
*/
typedef struct _EFI_EMMC_ERASE_DEVICE_INFO
{  
   UINT32 EraseGroupSize;  /**< eMMC Erase group size. */
} EFI_EMMC_ERASE_DEVICE_INFO;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_EMMC_ERASE_GET_DEVICE_INFO */ 
/** @ingroup efi_emmcErase_get_device_info
  @par Summary
  Retrieves the eMMC's information required to perform the erase operation.

Description:
  This function retrieves the eMMC's info needed to perform the erase operation

  @param[in]  This             Pointer to the EFI_EMMC_ERASE_PROTOCOL instance.
  @param[out] EraseDeviceInfo  eMMC's erase information; see 
                               #_EFI_EMMC_ERASE_DEVICE_INFO for details.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_ERASE_GET_DEVICE_INFO)(
   IN EFI_EMMC_ERASE_PROTOCOL *This, 
   OUT EFI_EMMC_ERASE_DEVICE_INFO *EraseDeviceInfo
);

/* EFI_EMMC_ERASE_BLOCKS */ 
/**@ingroup efi_emmcErase_erase_blocks
  @par Summary
  Performs the erase operation on the specified sectors. 

  @param[in] This         Pointer to the EFI_EMMC_ERASE_PROTOCOL instance.
  @param[in] SectorStart  Start sector for the erase operation.
  @param[in] SectorCount  Number of sectors on which to apply the erase 
                          operation.

  @par Description
  This function erases the eMMC sectors specified by the SectorStart and 
  SectorCount parameters. The SectorStart parameter must align to 
  EraseGroupSize in the #_EFI_EMMC_ERASE_DEVICE_INFO structure, and the 
  SectorCount parameter must be a multiple of EraseGroupSize. Otherwise, 
  EFI_INVALID_PARAMETER is returned. If the parameters are valid, this 
  function sends the commands to the eMMC part to apply the erase operation 
  on the specified sectors.
  
  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid, including an invalid
                           alignment of the start and count values. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_ERASE_BLOCKS)(
   IN EFI_EMMC_ERASE_PROTOCOL *This, 
   IN UINT32 SectorStart, 
   IN UINT32 SectorCount 
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_emmcErase_protocol
  @par Summary
  eMMC Erase Protocol interface.

  @par Parameters
  @inputprotoparams{emmc_erase_proto_params.tex} 
*/
struct _EFI_EMMC_ERASE_PROTOCOL 
{  
   UINT64 Revision;
   EFI_EMMC_ERASE_GET_DEVICE_INFO EmmcEraseGetDeviceInfo;  
   EFI_EMMC_ERASE_BLOCKS EmmcEraseBlocks; 
};

#endif	/* __EFI_EMMC_ERASE_H__*/

