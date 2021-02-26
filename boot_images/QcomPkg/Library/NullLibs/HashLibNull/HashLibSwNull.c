/** @file HashLibSwNull.c
  
  Stub functions for HashLibSw

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/08/15   plc     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "HashSw.h"
#include "CeML.h"
//#include <Library/HashLib/HwB/8996/CeCL.h>
#include <Library/BaseMemoryLib.h>

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/**
 * @brief This is the interface fuction of DMA hash
 *
 * @return EFI_STATUS
 *
 *
 */
EFI_STATUS HashInitSw(SW_SHA_Ctx *pCtx ) 
{
  EFI_STATUS Status = EFI_SUCCESS;
  return Status;
}

EFI_STATUS HashUpdateSw(SW_SHA_Ctx *pCtx, UINT8* pData, UINT32 nDataLen)
{
  EFI_STATUS Status = EFI_SUCCESS;
  return Status;
}

EFI_STATUS HashFinalSw(SW_SHA_Ctx *pCtx )
{
  EFI_STATUS Status = EFI_SUCCESS;
  return Status;
}

