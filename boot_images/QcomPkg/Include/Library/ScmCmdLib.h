/** @file ScmCmdLib.h
   
 Declarations of generic library functions for SCM commands.

 Copyright (c) 2011-2014 Qualcomm Technologies, Inc. All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 03/18/14    sm      Updated types
 02/13/13    bn      Fixed klockwork warnings
 02/24/11    niting  Added physical address pointer for SCM buffer to pass to TZ
 10/19/11    niting  Initial version.

=============================================================================*/
#ifndef _SCMCMDLIB_H_
#define _SCMCMDLIB_H_

#include <Uefi.h>

/* Macros required to generate the CommandId for the particular service */

#define TZEXEC_SVC_UEFI_AUTH_ID         0x0002000
#define TZEXEC_SVC_UEFI_VAR_ID          0x0008000
#define TZEXEC_SVC_UEFI_RPMB_ID         0x0010000

#define TZBSP_SVC_TZAPPS                253  ///< TZ OS application services
#define TZBSP_TZAPPS_SEND_CMD_ID     \
        TZBSP_SYSCALL_CREATE_CMD_ID(TZBSP_SVC_TZAPPS, 0x01)

#define TZBSP_SYSCALL_SUCCESS 1

/*===========================================================================
  SCM Command Initialization
===========================================================================*/
typedef struct
{
  /** SCM Buffer used to pass parameters to TZ */
  VOID               *ScmCmdBuffer;

  /** Size of the SCM Buffer used to pass parameters to TZ */
  UINTN               ScmCmdBufferSize;

  /** App Id used to pass commands to TZ */
  UINT32              ScmAppId;

  /** Physical address of SCM Buffer to pass to TZ slowcall */
  VOID               *ScmCmdBufferPhys;
} SCM_CMD_PARAMS_T;

/** SCM parameters used to pass data to TZ */
extern SCM_CMD_PARAMS_T ScmCmdParams;
extern BOOLEAN ScmCmdParamsInitialized;

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/**

  This initializes the parameters used to pass data to TZ.
   
  @param ScmCmdInitParams           Pointer to the structure used to initialize 
                                    the the parameters needed by the ScmCmdLib.
                      
  @return EFI_SUCCESS               Initialization successful.
  @return EFI_INVALID_PARAMETER     Invalid parameter.

**/
EFI_STATUS
EFIAPI
ScmCmdInit (
  IN      SCM_CMD_PARAMS_T  *ScmCmdInitParams
);


#endif /* _SCMCMDLIB_H_ */
