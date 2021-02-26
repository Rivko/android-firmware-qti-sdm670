/**
  @file EFIStorageWriteProtect.h
  @brief Storage Write Protection protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 06/13/16    jt      Initial version.
 
=============================================================================*/
#ifndef __EFI_STORAGE_WRITEPROTECT_H__
#define __EFI_STORAGE_WRITEPROTECT_H__

/** @cond */
typedef struct _EFI_STORAGE_WP_PROTOCOL   EFI_STORAGE_WP_PROTOCOL;
/** @endcond */

/** @ingroup efi_storage_writeProtect_constants
  Protocol version.
*/
#define EFI_STORAGE_WP_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_storage_writeProtect_protocol */
#define EFI_STORAGE_WP_PROTOCOL_GUID \
   {0xae1cdd2b, 0xb59a, 0x413a, { 0x8d, 0xff, 0x56, 0xd2, 0xfc, 0x63, 0xb7, 0xe0 } }

/** @cond */
/* External reference to the Write Protect Protocol GUID */
extern EFI_GUID gEfiStorageWpProtocolGuid; 
/** @endcond */

/** 
  Write protect types. 
*/
typedef enum _EFI_WP_TYPE
{
   WP_NOT_PROTECTED = 0x00, /**< Not protected. */
   WP_TEMPORARY     = 0x01, /**< Temporary. */
   WP_POWERON       = 0x02, /**< Power-on. */
   WP_PERMANENT     = 0x04, /**< Permanent. */
   WP_SECURE_NV     = 0x08, /**< Secure + non-volatile */
   WP_SECURE_P      = 0x10, /**< Secure + power on disable  */
   WP_SECURE_P_EN   = 0x20  /**< Secure + power on enable */
} EFI_WP_TYPE;

/** 
  Write protect entry information 
*/
typedef struct _EFI_WP_ENTRY
{
   EFI_WP_TYPE WpType; 
   /**< Type of write protection for the entry */
   UINT64 Lba; 
   /**< Logical Block Address of the write protect area. */
   UINT32 NumBlocks; 
   /**< Number of contiguous logical blocks that belong to the write protect area. */
} EFI_WP_ENTRY;

/** 
  Write protect entry configuration 
*/
typedef struct _EFI_WP_CONFIG
{
   UINT8 NumEntries; 
   /**< Number of Write Protect Entries being configured for the logical unit. */
   EFI_WP_ENTRY *WpEntries; 
   /**< Write Protect Entries describing the LBA ranges that 
        will be write protected. */
} EFI_WP_CONFIG;

/** @} */ /* end_addtogroup efi_writeProtect_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/* EFI_GET_SUPPORTED_WP_TYPES */
/** @ingroup efi_get_supported_wp_types
  @par Summary
  Gets the supported LBA write protect types for the logical unit 

  @param[in]   This         Pointer to the EFI_STORAGE_WP_PROTOCOL instance.
  @param[out]  LbaWpTypes   Pointer to a value indicating the different
                            EFI_WP_TYPE that are supported

  @par Description
  This function specifies which LBA write protect types are allowed for
  the particular logical unit. 

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- Invalid argument is provided.   
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_GET_SUPPORTED_WP_TYPES)(
   IN EFI_STORAGE_WP_PROTOCOL *This, 
   OUT UINT32 *LbaWpTypes
);

/* EFI_SET_LU_WRITE_PROTECT */
/** @ingroup efi_set_lu_write_protect
  @par Summary
  Enables write protection for the entire logical unit.  

  @param[in]  This         Pointer to the EFI_STORAGE_WP_PROTOCOL instance.

  @par Description
  This function applies write protection to the entire logical unit. On UFS devices,
  power-on write protection will be applied to all LUNs that have been configured
  as power on write protected (bLUWriteProtect = 1). On eMMC devices, write protecting the
  entire physical partition is not supported and EFI_UNSUPPORTED will be returned. 

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- Invalid argument is provided.   
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error in applying the 
                           write protection.
  @par
  EFI_UNSUPPORTED       -- Physical device does not support write protection over
                           the entire LU
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_SET_LU_WRITE_PROTECT)(
   IN EFI_STORAGE_WP_PROTOCOL *This
);

/* EFI_SET_LBA_WRITE_PROTECT */
/** @ingroup efi_set_lba_write_protect
  @par Summary
  Enables write protection for the sector range passed in. 

  @param[in]  This         Pointer to the EFI_STORAGE_WP_PROTOCOL instance.
  @param[in]  WpConfig     Write protect entry configuration information

  @par Description
  This function applies write protection to the range specified. 

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- Invalid argument is provided.   
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error in applying the 
                           write protection.
  @par
  EFI_UNSUPPORTED       -- Write protect type not supported. 
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_SET_LBA_WRITE_PROTECT)(
   IN EFI_STORAGE_WP_PROTOCOL *This,     
   IN EFI_WP_CONFIG *WpConfig
);

/* EFI_CLEAR_WRITE_PROTECT */
/** @ingroup efi_clear_write_protect
  @par Summary
  Clear the write protection
 
  @param[in]  This         Pointer to the EFI_STORAGE_WP_PROTOCOL instance.
                  
 
  @par Description
  This function clears the write protection that was set using
  efi_set_lba_write_protect. It is only valid for sector ranges that have been
  previously protected with the following types: WpTemporary or any of the secure
  types (WpSecureNv, WpSecureP, WpSecurePEn). If the sector range provided is
  write protected using another type, EFI_UNSUPPORTED will be returned. 

  @return
  EFI_SUCCESS           -- Function completed successfully.
  @par
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error.
  @par
  EFI_UNSUPPORTED       -- Feature is not supported. 
*/
typedef
EFI_STATUS
(EFIAPI * EFI_CLEAR_WRITE_PROTECT)(
   IN EFI_STORAGE_WP_PROTOCOL *This
);

/* EFI_GET_LU_WP_INFO */
/** @ingroup efi_get_lu_wp_info
  @par Summary
  Return the write protect information for a given logical unit
 
  @param[in]  This         Pointer to the EFI_STORAGE_WP_PROTOCOL instance.
  @param[out] ProtectInfo  Information about write protection for the LU

  @par Description
  This function returns the write protection information for the LU 

  @return
  EFI_SUCCESS           -- Function completed successfully.
  @par
  EFI_INVALID_PARAMETER -- Parameter is invalid.
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error. 
*/
typedef
EFI_STATUS
(EFIAPI * EFI_GET_LU_WP_INFO)(
   IN EFI_STORAGE_WP_PROTOCOL *This, 
   OUT EFI_WP_CONFIG *ProtectInfo
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_writeProtect_protocol
  @par Summary
  Write Protect Protocol interface. 

  @par Parameters
  @inputprotoparams{write_protect_proto_params.tex} 
*/
struct _EFI_STORAGE_WP_PROTOCOL 
{  
   UINT64 Revision;
   EFI_GET_SUPPORTED_WP_TYPES GetSupportedLbaWriteProtectTypes; 
   EFI_SET_LU_WRITE_PROTECT SetLUWriteProtect; 
   EFI_SET_LBA_WRITE_PROTECT SetLbaWriteProtect;
   EFI_CLEAR_WRITE_PROTECT ClearWriteProtect;  
   EFI_GET_LU_WP_INFO GetLUWriteProtectInfo; 
};

#endif	/* __EFIWRITEPROTECT_H__ */

