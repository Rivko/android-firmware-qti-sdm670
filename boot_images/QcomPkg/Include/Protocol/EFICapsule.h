/** 
  @file  EFICapsule.h
  @brief Capsule Services Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/11/13   niting  Created.

=============================================================================*/
#ifndef __EFICAPSULE_H__
#define __EFICAPSULE_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_capsule_constants
  Protocol version. 
*/
#define EFI_CAPSULE_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_capsule_protocol */
#define EFI_CAPSULE_PROTOCOL_GUID \
  { 0xbfafad65, 0x4acc, 0x5eeb, { 0x28, 0x6a, 0xd5, 0x9f, 0x55, 0x49, 0x62, 0xf6 } }


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/* External reference to the Capsule Protocol GUID */
/** @cond */
extern EFI_GUID gEfiCapsuleProtocolGuid;
/** @endcond */


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* Protocol declaration */
/** @cond */
typedef struct _EFI_CAPSULE_PROTOCOL EFI_CAPSULE_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_FLUSHCAPSULEPARTITION */
/** @ingroup efi_capsule_flush_capsule_partition
  @par Summary
  Updates and flushes the capsule partition to nonvolatile memory.
  
  @param[in]  This   Pointer to the EFI_CAPSULE_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_FLUSHCAPSULEPARTITION)(
  IN     EFI_CAPSULE_PROTOCOL    *This
);


/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_capsule_protocol
  @par Summary
  Capsule Protocol Interface.
  
  @par Parameters
  @inputprotoparams{capsule_proto_params.tex} 
*/
struct _EFI_CAPSULE_PROTOCOL {
  UINT64                       Revision;
  EFI_FLUSHCAPSULEPARTITION    FlushCapsulePartition;
};

#endif /* __EFICAPSULE_H__ */
