/** @file ScmCmdLib.c
   
  Implements generic library functions for SCM commands. 

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/21/13   shl     ScmDxe cleanup.
 02/24/11   niting  Added physical address pointer for SCM buffer to pass to TZ
 10/20/11   niting  Initial revision.

=============================================================================*/

#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ScmCmdLib.h>

/** SCM parameters used to pass data to TZ */
SCM_CMD_PARAMS_T ScmCmdParams;
BOOLEAN ScmCmdParamsInitialized = FALSE;

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
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  do
  {
    if (ScmCmdInitParams == NULL)
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if ((ScmCmdInitParams->ScmCmdBuffer == NULL) || 
        (ScmCmdInitParams->ScmCmdBufferSize == 0))
    {
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    ScmCmdParams.ScmCmdBuffer = ScmCmdInitParams->ScmCmdBuffer;
    ScmCmdParams.ScmCmdBufferSize = ScmCmdInitParams->ScmCmdBufferSize;
    ScmCmdParams.ScmAppId = ScmCmdInitParams->ScmAppId;
    ScmCmdParams.ScmCmdBufferPhys = ScmCmdInitParams->ScmCmdBufferPhys;

    ScmCmdParamsInitialized = TRUE;

  } while (0);
  
  return Status;
}
