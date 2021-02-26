/** 
  @file  EFIHWIO.h
  @brief HWIO Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/26/12   vishalo Merge in Techpubs Doxygen change
 10/25/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/17/11   gfr     Created.

=============================================================================*/
#ifndef __EFIHWIO_H__
#define __EFIHWIO_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_hwio_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_HWIO_PROTOCOL_VERSION 0x0000000000010000
/** @} */ /* end_addtogroup efi_hwio_constants */

/*  Protocol GUID definition */
/** @ingroup efi_hwio_protocol */
#define EFI_HWIO_PROTOCOL_GUID \
  { 0x9E61DE7C, 0x9927, 0x4F28, {0x9F, 0x3D, 0x32, 0x07, 0x05, 0xD8, 0x18, 0x01 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the HWIO Protocol GUID
*/
extern EFI_GUID gEfiHwioProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/**
  Protocol declaration.
*/
typedef struct _EFI_HWIO_PROTOCOL EFI_HWIO_PROTOCOL;
/** @endcond */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_HWIO_MAP_REGION */ 
/** @ingroup efi_hwio_map_region
  @par Summary
  Maps an HWIO region into virtual memory and returns a pointer to the base. 
  If the base is not valid for the target or Address is NULL, 
  EFI_PROTOCOL_ERROR is returned.

  @param[in]  This           Pointer to the EFI_HWIO_PROTOCOL instance.
  @param[in]  Base           String name of the base to map, i.e., CLK_CTL.
  @param[out] Address        Pointer to the virtual address to be filled in.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_HWIO_MAP_REGION)(
   IN  EFI_HWIO_PROTOCOL *This,
   IN CONST CHAR8 *Base,
   OUT UINT8 **Address);


/* EFI_HWIO_MAP_REGION_BY_ADDRESS */ 
/** @ingroup efi_hwio_map_region_by_address
  @par Summary
  Maps an HWIO region into virtual memory using the required physical address 
  and returns a pointer to the base. If the base is not valid for the target 
  or Address is NULL, EFI_PROTOCOL_ERROR is returned.

  @param[in]  This           Pointer to the EFI_HWIO_PROTOCOL instance.
  @param[in]  AddressPhys    Base physical address.
  @param[out] Address        Pointer to the virtual address to be filled in.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS
(EFIAPI *EFI_HWIO_MAP_REGION_BY_ADDRESS)(
   IN  EFI_HWIO_PROTOCOL *This,
   IN UINT8 *AddressPhys,
   OUT UINT8 **Address);


/* EFI_HWIO_UNMAP_REGION */ 
/** @ingroup efi_hwio_unmap_region
  @par Summary
  Unmaps an HWIO base region in virtual memory. If the address is not valid, 
  EFI_PROTOCOL_ERROR is returned.
  @par  
  If other users have also mapped the region, it will remain mapped until 
  all users have called EFI_HWIO_UNMAP_REGION().

  @param[in] This            Pointer to the EFI_HWIO_PROTOCOL instance.
  @param[in] Address         Address of the region to unmap.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/ 
typedef
EFI_STATUS
(EFIAPI *EFI_HWIO_UNMAP_REGION)(
   IN  EFI_HWIO_PROTOCOL *This,
   IN  UINT8 *Address);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_hwio_protocol
  @par Summary
  Hardware I/O (HWIO) Protocol interface.

  @par Parameters
  @inputprotoparams{hwio_proto_params.tex} 
*/
struct _EFI_HWIO_PROTOCOL {
   UINT64                          Version;
   EFI_HWIO_MAP_REGION             MapRegion;
   EFI_HWIO_MAP_REGION_BY_ADDRESS  MapRegionByAddress;
   EFI_HWIO_UNMAP_REGION           UnMapRegion;
}; 

#endif  /* __EFIHWIO_H__ */
