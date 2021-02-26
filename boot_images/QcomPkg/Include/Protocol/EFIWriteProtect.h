/**
  @file EFIWriteProtect.h
  @brief Sdcc eMMC Write Protection protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 09/24/12    llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 05/15/12    vishalo Cleanup warnings
 01/06/12    bn      Initial version.
 
=============================================================================*/
#ifndef __EFIWRITEPROTECT_H__
#define __EFIWRITEPROTECT_H__

/** @cond */
typedef struct _EFI_EMMC_WP_PROTOCOL   EFI_EMMC_WP_PROTOCOL;
/** @endcond */

/** @ingroup efi_writeProtect_constants
  Protocol version.
*/
#define EFI_EMMC_WP_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_writeProtect_protocol */
#define EFI_EMMC_WP_PROTOCOL_GUID \
   {0xd35dc019, 0x3eab, 0x4655, { 0xba, 0x9a, 0x73, 0x4a, 0x8d, 0x8b, 0x98, 0x47 } }

/** @cond */
/* External reference to the Emmc WP Protocol GUID */
extern EFI_GUID gEfiEmmcWpProtocolGuid;
/** @endcond */


/** @addtogroup efi_writeProtect_data_types 
@{ */
/**
  eMMC write protect capabilities. Refer to @xhyperref{S1,[S1]} for details.
*/ 
typedef struct _EFI_EMMC_WP_CAPABILITIES
{  
   BOOLEAN WriteProtectGroupEnable;
   /**< Value of WP_GRP_ENABLE CSD-slice[31:31] indicating if group write 
        protect is supported on the eMMC device. */
   BOOLEAN HardwareResetEnabled;
   /**< Value of RST_n_FUNCTION EXT_CSD-slice[162] indicating if hardware 
        reset has been enabled on the eMMC device. */
   UINT32 WriteProtectGroupSize;
   /**< Value of the write protect group size, in sectors. Depending on the 
        state of the ERASE_GROUP_DEF value, this is calculated using either 
        HC_WP_GRP_SIZE or WP_GRP_SIZE. */
} EFI_EMMC_WP_CAPABILITIES;

/** 
  eMMC write protect type. 
*/
typedef enum _EFI_EMMC_WP_TYPE
{
   EmmcWpNotProtected = 0x00, /**< Not protected. */
   EmmcWpTemporary    = 0x01, /**< Temporary. */
   EmmcWpPowerOn      = 0x02, /**< Power-on. */
   EmmcWpPermanent    = 0x03  /**< Permanent. */
} EFI_EMMC_WP_TYPE;
/** @} */ /* end_addtogroup efi_writeProtect_data_types */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_EMMC_WP_GET_DEVICE_CAPABILITIES */
/** @ingroup efi_wp_get_device_capabilities
  @par Summary
  Returns information about write the protection capabilities of the eMMC 
  device. Information includes whether write protect is supported, the state 
  of the hardware reset, and the write protect group size. 

  @param[in]  This          Pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param[out] Capabilities  Capabilities of the eMMC device; see 
                            #_EFI_EMMC_WP_CAPABILITIES for details. 

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_WP_GET_DEVICE_CAPABILITIES)(
   IN EFI_EMMC_WP_PROTOCOL *This, 
   OUT EFI_EMMC_WP_CAPABILITIES *Capabilities 
);

/* EFI_EMMC_WP_SET_WRITE_PROTECT */
/** @ingroup efi_wp_set_write_protect
  @par Summary
  Enables power-on write protection for the sector range passed in. 

  @param[in]  This         Pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param[in]  SectorStart  Start sector for the write protect operation.
  @param[in]  SectorCount  Number of sectors to which to apply the power-on 
                           write protection.

  @par Description
  This function applies write protection to the range specified. Both the 
  SectorStart and SectorCount parameters must be aligned to the 
  write protect group size. If either is not aligned, EFI_INVALID_PARAMETER is 
  returned. If the parameters are valid, this function sends the commands to 
  the eMMC to apply power-on write protection to the range. US_PERM_WP_EN 
  must be set to 0, and US_PWR_WP_EN must be set to 1 
  (per JESD84-A441 @xhyperref{S1,[S1]} Table 15). Then CMD28 (SET_WRITE_PROT) 
  is sent to apply the protection. Refer to @xhyperref{S1,[S1]} for details.
    @par
  This interface only supports the Power-On write protect mode. 

  @return
  EFI_SUCCESS           -- Function completed successfully. 
  @par
  EFI_INVALID_PARAMETER -- Parameter is invalid, including invalid alignment 
                           of the start and count values.  
  @par
  EFI_DEVICE_ERROR      -- Physical device reported an error in applying the 
                           write protect commands. 
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_EMMC_WP_SET_WRITE_PROTECT)(
   IN EFI_EMMC_WP_PROTOCOL *This, 
   IN UINT32 SectorStart, 
   IN UINT32 SectorCount 
);

/* EFI_EMMC_WP_SEND_WRITE_PROTECT_TYPE */
/** @ingroup efi_wp_send_write_protect_type
  @par Summary
  For a specified sector, returns the write protect mode of the containing 
  group.
 
  @param[in]  This         Pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param[in]  Sector       Sector address to the write protect group. This 
                           must be aligned to the write protect group size.
  @param[out] ProtectType  Type of protect applied; see #_EFI_EMMC_WP_TYPE 
                           for details. 

  @par Description
  This function returns the type of protection for the group containing the 
  sector specified by the Sector parameter. CMD31 (SEND_WRITE_PROT_TYPE) is 
  used to get the write protection type. The function then masks off the two 
  LSBs to retrieve the protection type for the protection group specified by 
  the Sector parameter. Refer to @xhyperref{S1,[S1]} Table 26 for details. 

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_DEVICE_ERROR      -- Physical device reported an error. 
*/
typedef
EFI_STATUS
(EFIAPI * EFI_EMMC_WP_SEND_WRITE_PROTECT_TYPE)(
   IN EFI_EMMC_WP_PROTOCOL *This, 
   IN UINT32 Sector, 
   OUT EFI_EMMC_WP_TYPE *ProtectType
   );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_writeProtect_protocol
  @par Summary
  Embedded Multimedia Card (eMMC) Write Protect Protocol interface.

  @par Parameters
  @inputprotoparams{write_protect_proto_params.tex} 
*/
struct _EFI_EMMC_WP_PROTOCOL 
{  
   UINT64 Revision;
   EFI_EMMC_WP_GET_DEVICE_CAPABILITIES GetDeviceCapabilities; 
   EFI_EMMC_WP_SET_WRITE_PROTECT SetWriteProtect; 
   EFI_EMMC_WP_SEND_WRITE_PROTECT_TYPE SendWriteProtectType; 
};

#endif	/* __EFIWRITEPROTECT_H__ */

