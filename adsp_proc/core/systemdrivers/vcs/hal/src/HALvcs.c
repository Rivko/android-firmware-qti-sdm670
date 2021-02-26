/*
==============================================================================

FILE:         HALvcs.c

DESCRIPTION:
  Common code for the VCS HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/hal/src/HALvcs.c#2 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALvcs.h>
#include "HALvcsInternal.h"
#include <HALhwio.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/

/*
 * HAL_vcs_Type
 *
 * Consolidated set of internal static data for the ldo HAL.
 *
 * mHALCtxt - HAL LDO driver context data
 */
typedef struct
{
  HAL_vcs_ContextType mHALCtxt;
} HAL_vcs_Type;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_vcs
 *
 * Consolidated static data for the LDO HAL.
 *
 * We fill in some non-zero data to ensure that this ends up in the RW
 * segment instead of ZI since the LDO HAL is used in the bootloader
 * before the ZI is cleared.
 */
static HAL_vcs_Type HAL_vcs;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_vcs_Init
**
** ======================================================================== */

void HAL_vcs_Init
(
  HAL_vcs_ContextType *pHALCtxt
)
{
  /*
   * Sanity check
   */
  if (pHALCtxt != NULL)
  {
     /*
      * Save the busy wait function pointer
      */
     HAL_vcs.mHALCtxt.BusyWait = pHALCtxt->BusyWait;

     /*
      * Save the chip info
      */
     HAL_vcs.mHALCtxt.nChipVersion = pHALCtxt->nChipVersion;
     HAL_vcs.mHALCtxt.nChipId = pHALCtxt->nChipId;
     HAL_vcs.mHALCtxt.nChipFamily = pHALCtxt->nChipFamily;
  }

} /* END HAL_vcs_Init */

/* ===========================================================================
**  HAL_vcs_GetChipVersion
**
** ======================================================================== */

uint32 HAL_vcs_GetChipVersion
(
  void
)
{
  return HAL_vcs.mHALCtxt.nChipVersion;

} /* END HAL_vcs_GetChipVersion */


/* ===========================================================================
**  HAL_vcs_GetChipId
**
** ======================================================================== */

uint32 HAL_vcs_GetChipId
(
  void
)
{
  return HAL_vcs.mHALCtxt.nChipId;

} /* END HAL_vcs_GetChipId */


/* ===========================================================================
**  HAL_vcs_GetChipFamily
**
** ======================================================================== */

uint32 HAL_vcs_GetChipFamily
(
  void
)
{
  return HAL_vcs.mHALCtxt.nChipFamily;

} /* END HAL_vcs_GetChipFamily */


/* ===========================================================================
**  HAL_vcs_Config
**
** ======================================================================== */

void HAL_vcs_ConfigLDO
(
  HAL_vcs_LDODescType   *pDesc,
  HAL_vcs_LDOConfigType *pConfig
)
{
  if (pDesc != NULL &&
      pConfig != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->Config != NULL)
  {
    pDesc->pmControl->Config(pDesc, pConfig);
  }

} /* END HAL_vcs_ConfigLDO */


/* ===========================================================================
**  HAL_vcs_EnableLDO
**
** ======================================================================== */

void HAL_vcs_EnableLDO
(
  HAL_vcs_LDODescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->Enable != NULL)
  {
    pDesc->pmControl->Enable(pDesc);
  }

} /* END HAL_vcs_EnableLDO */


/* ===========================================================================
**  HAL_vcs_DisableLDO
**
** ======================================================================== */

void HAL_vcs_DisableLDO
(
  HAL_vcs_LDODescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->Disable != NULL)
  {
    pDesc->pmControl->Disable(pDesc);
  }

} /* END HAL_vcs_DisableLDO */


/* ===========================================================================
**  HAL_vcs_IsLDOEnabled
**
** ======================================================================== */

void HAL_vcs_IsLDOEnabled
(
  HAL_vcs_LDODescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->IsEnabled != NULL)
  {
    pDesc->pmControl->IsEnabled(pDesc);
  }

} /* END HAL_vcs_IsLDOEnabled */


/* ===========================================================================
**  HAL_vcs_SetLDOVoltage
**
** ======================================================================== */

void HAL_vcs_SetLDOVoltage
(
  HAL_vcs_LDODescType *pDesc,
  uint32               nVoltageUV
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->SetVoltage != NULL)
  {
    pDesc->pmControl->SetVoltage(pDesc, nVoltageUV);
  }

} /* END HAL_vcs_SetLDOVoltage */


/* ===========================================================================
**  HAL_vcs_GetLDOVoltage
**
** ======================================================================== */

void HAL_vcs_GetLDOVoltage
(
  HAL_vcs_LDODescType *pDesc,
  uint32              *pVoltageUV,
  uint32              *pVoltageHW
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->GetVoltage != NULL)
  {
    pDesc->pmControl->GetVoltage(pDesc, pVoltageUV, pVoltageHW);
  }

} /* END HAL_vcs_GetLDOVoltage */


/* ===========================================================================
**  HAL_vcs_EnableCoproc
**
** ======================================================================== */

void HAL_vcs_EnableCoproc
(
  HAL_vcs_CoprocDescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->Enable != NULL)
  {
    pDesc->pmControl->Enable(pDesc);
  }

} /* END HAL_vcs_EnableCoproc */


/* ===========================================================================
**  HAL_vcs_DisableCoproc
**
** ======================================================================== */

void HAL_vcs_DisableCoproc
(
  HAL_vcs_CoprocDescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->Disable != NULL)
  {
    pDesc->pmControl->Disable(pDesc);
  }

} /* END HAL_vcs_DisableCoproc */


/* ===========================================================================
**  HAL_vcs_IsCoprocEnabled
**
** ======================================================================== */

void HAL_vcs_IsCoprocEnabled
(
  HAL_vcs_CoprocDescType *pDesc
)
{
  if (pDesc != NULL &&
      pDesc->pmControl != NULL &&
      pDesc->pmControl->IsEnabled != NULL)
  {
    pDesc->pmControl->IsEnabled(pDesc);
  }

} /* END HAL_vcs_IsCoprocEnabled */


/* ===========================================================================
**  HAL_vcs_BusyWait
**
** ======================================================================== */

void HAL_vcs_BusyWait
(
  uint32 nTimeUS
)
{
  if (HAL_vcs.mHALCtxt.BusyWait != NULL)
  {
     HAL_vcs.mHALCtxt.BusyWait(nTimeUS);
  }
  else
  {
    /* We can have our own nop loop here but it won't be precise since
     * we don't know how many time to iterate to meet the required uSec
     * pause - need to know cpu frequency.
     */
  }
}

