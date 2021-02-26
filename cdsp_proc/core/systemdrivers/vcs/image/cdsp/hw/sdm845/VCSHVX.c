/*
===========================================================================
*/
/**
  @file VCSHVX.c 
  
  The file contains the resource definitions for HVX management on the
  Turing Subsystem processor.
*/
/*  
  ====================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/image/cdsp/hw/sdm845/VCSHVX.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/06/16   dcf     Created for TURING on SDM845.

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "HALhwio.h"
#include "HALvcsHWIO.h"
#include "HALvcsQDSP6v65.h"
#include "VCSDriver.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * VCS_HVXType
 *
 * This structure contains the data associated with managing the HVX coproc.
 *
 *  HALdesc         - HAL coproc structure.
 *  nReferenceCount - Number of outstanding enable requests.
 *  LockObj         - Sync object.
 *  hLock           - Handle to mutex.
 */
typedef struct VCS_HVXType
{
  HAL_vcs_CoprocDescType  HALdesc;
  uint32                  nReferenceCount;
  DALSYSSyncObj           LockObj;
  DALSYSSyncHandle        hLock;
} VCS_HVXType;


/*=========================================================================
      Data
==========================================================================*/

static VCS_HVXType VCS_HVX =
{
  .HALdesc =
  {
    .nAddr = HWIO_ADDR(TURING_QDSP6SS_VERSION),
    .pmControl = &HAL_vcs_QDSP6v65CoprocControl,
  },
};


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : VCS_EnableHVX
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_EnableHVX(VCSDrvCtxt *pDrvCtxt)
{
  /*-----------------------------------------------------------------------*/
  /* Enter the critical section.                                           */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncEnter(VCS_HVX.hLock);

  /*-----------------------------------------------------------------------*/
  /* If the reference count is zero then we enable HVX.                    */
  /*-----------------------------------------------------------------------*/

  if (VCS_HVX.nReferenceCount == 0)
  {
    HAL_vcs_EnableCoproc(&VCS_HVX.HALdesc);

    /*
     * Log the enable.
     */
    ULOG_RT_PRINTF_0(pDrvCtxt->hVCSLog, "[HVX] state=on.");
  }

  /*-----------------------------------------------------------------------*/
  /* Increment the reference count.                                        */
  /*-----------------------------------------------------------------------*/

  VCS_HVX.nReferenceCount++;

  /*-----------------------------------------------------------------------*/
  /* Exit the critical section.                                            */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(VCS_HVX.hLock);

  return DAL_SUCCESS;

} /* END VCS_EnableHVX */


/* =========================================================================
**  Function : VCS_DisableHVX
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_DisableHVX(VCSDrvCtxt *pDrvCtxt)
{
  /*-----------------------------------------------------------------------*/
  /* Enter the critical section.                                           */
  /*-----------------------------------------------------------------------*/
  
  DALSYS_SyncEnter(VCS_HVX.hLock);

  /*-----------------------------------------------------------------------*/
  /* If the reference count is one then we disable HVX.                    */
  /*-----------------------------------------------------------------------*/

  if (VCS_HVX.nReferenceCount == 1)
  {
    HAL_vcs_DisableCoproc(&VCS_HVX.HALdesc);

    /*
     * Log the disable.
     */
    ULOG_RT_PRINTF_0(pDrvCtxt->hVCSLog, "[HVX] state=off.");
  }

  /*-----------------------------------------------------------------------*/
  /* Deccrement the reference count.                                       */
  /*-----------------------------------------------------------------------*/

  if (VCS_HVX.nReferenceCount > 0)
  {
    VCS_HVX.nReferenceCount--;
  }

  /*-----------------------------------------------------------------------*/
  /* Exit the critical section.                                            */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(VCS_HVX.hLock);

  return DAL_SUCCESS;

} /* END VCS_DisableHVX */


/* =========================================================================
**  Function : VCS_InitHVX
** =========================================================================*/
/**
  Initialize the HVX coprocessor structures.

  This function initializes the portion of the VCS driver that is specific
  to managing the HVX coprocessor.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Init was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_InitHVX(VCSDrvCtxt *pDrvCtxt)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Map HVX register base address.                                        */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->pBSP->nNumBases)
  {
    eResult = VCS_MapAddress(pDrvCtxt, &VCS_HVX.HALdesc.nAddr);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Create the synchronization object.                                    */
  /*-----------------------------------------------------------------------*/

  eResult =
    DALSYS_SyncCreate(
      DALSYS_SYNC_ATTR_RESOURCE,
      &VCS_HVX.hLock,
      &VCS_HVX.LockObj);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: DALSYS_SyncCreate failed.");

    return eResult;
  }

  return DAL_SUCCESS;

} /* END VCS_InitHVX */

