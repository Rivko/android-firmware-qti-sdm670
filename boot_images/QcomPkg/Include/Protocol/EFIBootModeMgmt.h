/**
  @file EFIBootModeMgmt.h
  @brief Boot Mode management protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
  
=============================================================================*/
#ifndef __EFIBOOTMODEMGMT_H__
#define __EFIBOOTMODEMGMT_H__


/** @cond */
typedef struct _EFI_BOOT_MODE_MGMT_PROTOCOL   EFI_BOOT_MODE_MGMT_PROTOCOL;
/** @endcond */

/** @ingroup efi_boot_mode_constants
  Protocol version.
*/
#define EFI_BOOT_MODE_MGMT_PROTOCOL_REVISION      0x00010000

/* Protocol GUID definition */
/** @ingroup boot_mode_mgmt_proto_params */
#define EFI_BOOT_MODE_MGMT_PROTOCOL_GUID \
   { 0xBE464946, 0x9787, 0x4FEB, { 0xBD, 0x71, 0x14, 0xC1, 0xC5, 0x2D, 0x69, 0xD1 } }

/** @cond */
/* External reference to the EFI Boot Mode Mgmt Protocol GUID */
extern EFI_GUID gEfiBootModeMgmtProtocolGuid;
/** @endcond */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SET_BOOT_MODE_INFO */ 
/** @ingroup efi_set_boot_mode_info
  @par Summary                    
  Set the Boot Mode and Profile Name. 

  @param[in] This                Pointer to EFI_OS_BOOTMODE_MGMT_PROTOCOL instance.
  @param[in] BootMode            Pointer to a UINT32 value for Boot Mode that needs to be set.
  @param[in] ProfileNameElements Pointer to a UINT32 value of number of elements of ProfileName that need to be set.
  @param[in] ProfileName         CHAR16 string holding ProfileName.
		
  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_NOT_FOUND          -- Storage partition not found.
  EFI_VOLUME_CORRUPTED   -- Storage partition is not initialized or corrupted.
  EFI_INVALID_PARAM      -- Invalid parameter was passed to the function.
  EFI_BAD_BUFFER_SIZE    -- ProfileName string is too large
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SET_BOOT_MODE_INFO)(
  IN  EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  IN UINT32                       *BootMode,
  IN UINT32                       *ProfileNameElements OPTIONAL,
  IN CHAR16                       *ProfileName OPTIONAL
  );

/* EFI_GET_BOOT_MODE_INFO */ 
/** @ingroup efi_get_boot_mode_info
  @par Summary 
  Get the boot mode and the profile name
                       
  @param[in]     This                Pointer to EFI_BOOT_MODE_MGMT_PROTOCOL instance.
  @param[out]    BootMode            Pointer to a UINT32 where Boot Mode will be returned.
  @param[in out] ProfileNameElements Pointer to a UINT32 which, on input, indicates number of elements in 
                                     the ProfileName buffer and on output, indicates the actual number of elements.
  @param[out]    ProfileName         CHAR16 string where ProfileName will be returned.

  @return
  EFI_SUCCESS            -- Function completed successfully.
  EFI_NOT_FOUND          -- Storage partition not found.
  EFI_VOLUME_CORRUPTED   -- Storage partition is not initialized or corrupted.
  EFI_INVALID_PARAM      -- Invalid parameter was passed to the function.
  EFI_BUFFER_TOO_SMALL   -- Not sufficient space in the buffer provided.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_GET_BOOT_MODE_INFO)(
  IN EFI_BOOT_MODE_MGMT_PROTOCOL *This,
  OUT    UINT32                  *BootMode,
  IN OUT UINT32                  *ProfileNameElements OPTIONAL, 
  OUT    CHAR16                  *ProfileName OPTIONAL
  );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_os_boot_mode_mgmt_protocol
  @par Summary
   Manage the Boot Mode partition and the profile name corresponding to the mode.
   
  @par Parameters
  @inputprotoparams{boot_mode_mgmt_proto_params.tex} 
*/
struct _EFI_BOOT_MODE_MGMT_PROTOCOL {
  UINT32                          Revision;
  EFI_SET_BOOT_MODE_INFO          SetBootModeInfo;
  EFI_GET_BOOT_MODE_INFO          GetBootModeInfo;
};

#endif /* __EFIBOOTMODEMGMT_H__ */

