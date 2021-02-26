/** 
  @file  EFIVariableServices.h
  @brief Variable Services Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2012, 2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/22/17   bd      SCM call for deleting secure boot variables 
 07/24/12   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 08/23/11   niting  Created.

=============================================================================*/
#ifndef __EFIVARIABLESERVICES_H__
#define __EFIVARIABLESERVICES_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_variableServices_constants
  Protocol version. 
*/
#define EFI_VARIABLESERVICES_PROTOCOL_REVISION 0x0000000000010000

/* Protocol GUID definition */
/** @ingroup efi_variableServices_protocol */
#define EFI_VARIABLESERVICES_PROTOCOL_GUID \
  { 0xf9085b9d, 0x9304, 0x40fb, { 0x8f, 0xe0, 0x4a, 0xee, 0x3b, 0x1a, 0x78, 0x4b } }


/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/* External reference to the Variable Services Protocol GUID */
/** @cond */
extern EFI_GUID gEfiVariableServicesProtocolGuid;
/** @endcond */


/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* Protocol declaration */
/** @cond */
typedef struct _EFI_VARIABLESERVICES_PROTOCOL EFI_VARIABLESERVICES_PROTOCOL;
/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_FLUSHVARIABLENV */
/** @ingroup efi_variableServices_flush_variable_nv
  @par Summary
  Flushes all nonvolatile storage tables to nonvolatile memory.
  
  @param[in]  This   Pointer to the EFI_VARIABLESERVICES_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_FLUSHVARIABLENV)(
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
);

/* EFI_CLEAR_SECBOOT_VARIABLES */
/** @ingroup efi_variableServices_clear_secboot_variables
  @par Summary
  Deletes the secboot variables - PK/KEK/DB/DBX
  
  @param[in]  This   Pointer to the EFI_VARIABLESERVICES_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_DEL_SECBOOT_VARIABLES)(
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
);

/* EFI_SEND_DISABLE_DELSECBOOTVARS_MILESTONE_CALL */
/** @ingroup efi_variableServices_send_milestone_call
  @par Summary
  Sends milestone call to UEFI SECAPP.
  
  @param[in]  This   Pointer to the EFI_VARIABLESERVICES_PROTOCOL instance.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context.
*/
typedef
EFI_STATUS
(EFIAPI *EFI_SEND_DISABLE_DELSECBOOTVARS_MILESTONE_CALL)(
  IN     EFI_VARIABLESERVICES_PROTOCOL    *This
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_variableServices_protocol
  @par Summary
  Variable Services Protocol Interface.
  
  @par Parameters
  @inputprotoparams{variable_services_proto_params.tex} 
*/
struct _EFI_VARIABLESERVICES_PROTOCOL {
  UINT64                       Revision;
  EFI_FLUSHVARIABLENV          FlushVariableNV;
  EFI_DEL_SECBOOT_VARIABLES    DelSecBootVars;
  EFI_SEND_DISABLE_DELSECBOOTVARS_MILESTONE_CALL      SendMileStoneCall;  
};

#endif /* __EFIVARIABLESERVICES_H__ */
