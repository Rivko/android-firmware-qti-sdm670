/** 
  @file  EFIRpmServices.h
  @brief API for RPM services.
*/
/*=============================================================================
  Copyright (c) 2014-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
07/23/15   rli     Added EFI_rpm_protocol
08/20/14   rli     Initial revision.

=============================================================================*/
#ifndef __EFIRPMSERVICES_H__
#define __EFIRPMSERVICES_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/


/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @addtogroup efi_rpmservices_constants 
@{ */
/**
  Protocol version.
*/
#define EFI_RPMSERVICES_PROTOCOL_REVISION 0x0000000000010002
/** @} */ /* end_addtogroup efi_rpmservices_constants */

/*  Protocol GUID definition */
/** @ingroup efi_rpmservices_protocol */
#define EFI_RPMSERVICES_PROTOCOL_GUID \
  { 0x276732ab, 0xbb39, 0x4dce, { 0xb5, 0xd9, 0x27, 0x46, 0xcd, 0xd6, 0xd5, 0x18 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/**
  External reference to the RPM Services Protocol GUID.
 */
extern EFI_GUID gEfiRpmServicesProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/

/**
 * <!-- EFI_DDRHEALTHMONITOR -->
 *
 * @brief DDR Health monitor request
 *
 * @return       EFI_SUCCESS
 */
 typedef EFI_STATUS (EFIAPI* EFI_DDRHEALTHMONITOR)
(
    UINT64 address,
    UINT32 size
);

/**
 * <!-- EFI_RPMPROTOCOL -->
 *
 * @brief Install Rpm Protocol
 *
 * @return       EFI_SUCCESS
 */
 typedef EFI_STATUS (EFIAPI* EFI_INSTALLRPMPROTOCOL)( VOID );

/**
 * <!-- EFI_RPMPROTOCOL -->
 *
 * @brief Uninstall RpmApi Protocol
 *
 * @return       EFI_SUCCESS
 */
 typedef EFI_STATUS (EFIAPI* EFI_UNINSTALLRPMPROTOCOL)( VOID );

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_rpmservices_protocol
  @par Summary
  Rpm Services Protocol interface.

  @par Parameters
*/
typedef struct _EFI_RPMSERVICES_PROTOCOL {
  UINT64                           Revision;
  EFI_DDRHEALTHMONITOR             DdrHealthMonitor;
  EFI_INSTALLRPMPROTOCOL           InstallRpmProtocol;
  EFI_UNINSTALLRPMPROTOCOL         UninstallRpmProtocol;
 }EFI_RPMSERVICES_PROTOCOL;

#endif /* __EFIRPMSERVICES_H__ */
