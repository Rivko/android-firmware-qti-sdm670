/** @file EFISdccConfig.h
   
  Sdcc Host Controller Configuration protocol definitions.

  Copyright (c) 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential

*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---     -----------------------------------------------------------
 10/14/14    bn      Branched from 8994 UEFI
 
=============================================================================*/
#ifndef __EFI_SDCC_CONFIG_H__
#define __EFI_SDCC_CONFIG_H__

/** @cond */
typedef struct _EFI_SDCC_CONFIG_PROTOCOL   EFI_SDCC_CONFIG_PROTOCOL;
/** @endcond */

/** @ingroup efi_sdcc_config_constants
  Protocol version.
*/
#define EFI_SDCC_CONFIG_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition. */
/** @ingroup efi_sdcc_config_protocol  */
#define EFI_SDCC_CONFIG_PROTOCOL_GUID \
   { 0x6C7567DF, 0x412B, 0x28E6, { 0xB6, 0xB6, 0xE2, 0x89, 0x81, 0x72, 0xFA, 0xBF } }


/** @cond */
/**
  External reference to the Sdcc Config Protocol GUID 
*/
extern EFI_GUID gEfiSdccConfigProtocolGuid;
/** @endcond */


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_SDCC_ENABLE_SDHCI_MODE */
/** @ingroup efi_sdcc_enable_sdhci_mode
  @par Summary
  Enables the SDCC Host Controller specification.
 
  @param[in]   This   Pointer to the EFI_SDCC_CONFIG_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid.
*/
typedef
EFI_STATUS 
(EFIAPI * EFI_SDCC_ENABLE_SDHCI_MODE)(
   IN EFI_SDCC_CONFIG_PROTOCOL *This 
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_sdcc_config_protocol
  @par Summary
  SDCC Config Protocol interface.

  @par Parameters
  @inputprotoparams{sdcc_config_proto_params.tex} 
*/
struct _EFI_SDCC_CONFIG_PROTOCOL 
{  
   UINT64 Revision;
   EFI_SDCC_ENABLE_SDHCI_MODE SdccEnableSdhciMode; 
};

#endif	/* __EFI_SDCC_CONFIG_H__ */

