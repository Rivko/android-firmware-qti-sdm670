/*
===========================================================================
*/
/**
  @file VCSDriver.c

  Interface implementation file for the VCS device driver.
*/
/*
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/src/VCSDriver.c#2 $
  $DateTime: 2017/08/07 11:42:50 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "DDIVCS.h"
#include "VCSDriver.h"
#include "DDIChipInfo.h"
#include "HALvcs.h"
#include "VCSSWEVT.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/

/*
 * Handle to the VCS DAL.
 */
static DalDeviceHandle *hVCS;


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : vcs_init
** =========================================================================*/

void vcs_init
(
  void
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Attach to the VCS DAL.                                                */
  /*-----------------------------------------------------------------------*/

  eResult = DAL_StringDeviceAttach("VCS", &hVCS);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: Unable to attach to VCS DAL: %d.", eResult);
    return;
  }

} /* END vcs_init */


/* =========================================================================
**  Function : VCS_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver.

  This function initializes the vcs driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCS_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult             eResult;
  HAL_vcs_ContextType   HALVCSCtxt = {0};

  /*-----------------------------------------------------------------------*/
  /* Initialize the log.                                                   */
  /*-----------------------------------------------------------------------*/

  ULogFront_RealTimeInit(
    &pDrvCtxt->hVCSLog,
    "VCS Log",
    0x1000,
    ULOG_MEMORY_LOCAL,
    ULOG_LOCK_OS);

  ULogCore_HeaderSet(pDrvCtxt->hVCSLog, "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Initialize base addresses.                                            */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitBases(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "VCS_InitBases failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the VCS SW Events.                                         */
  /*-----------------------------------------------------------------------*/

  VCS_SWEvent(VCS_EVENT_INIT_START, 0);

  /*-----------------------------------------------------------------------*/
  /* Initialize the VCS HAL.                                               */
  /*-----------------------------------------------------------------------*/

  /*
   * Use DALSys busy wait function for HAL modules.
   */
  HALVCSCtxt.BusyWait = DALSYS_BusyWait;

  /*
   * Get chip version to send to the HAL.
   */
  HALVCSCtxt.nChipVersion = DalChipInfo_ChipVersion();
  HALVCSCtxt.nChipId = DalChipInfo_ChipId();
  HALVCSCtxt.nChipFamily = DalChipInfo_ChipFamily();

  HAL_vcs_Init(&HALVCSCtxt);

  /*-----------------------------------------------------------------------*/
  /* Initialize the rails.                                                 */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitRails(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_InitRails failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitTarget(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_InitTarget failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize NPA resources for each rail and LDO if applicable.         */
  /*                                                                       */
  /* Create NPA resource node for this rail. The resource name, initial    */
  /* state, and dependencies are defined in the BSP.                       */
  /*                                                                       */
  /* Note: This part needs to happen after the 'set rail corner' function  */
  /*       pointers have been installed, because node creation can happen  */
  /*       right away and trigger the rail's driver function.              */
  /*-----------------------------------------------------------------------*/

  eResult = VCS_InitNPA(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCS_InitRailNPAResources failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Log Initialization complete VCS SW Event.                             */
  /*-----------------------------------------------------------------------*/

  VCS_SWEvent(VCS_EVENT_INIT_COMPLETE, 0);

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_DriverInit */


/* =========================================================================
**  Function : VCSStub_DriverInit
** =========================================================================*/
/**
  Initialize the VCS driver stub.

  This function initializes the VCS driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult VCSStub_DriverInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver stub.                            */
  /*-----------------------------------------------------------------------*/

  eResult = VCSStub_InitTarget(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      0,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "DALLOG Device VCS: VCSStub_InitTarget failed.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_DriverInit */


/* =========================================================================
**  Function : VCS_MapAddress
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_MapAddress
(
  VCSDrvCtxt *pDrvCtxt,
  uint32     *pnAddr
)
{
  VCSBaseType *pBase;
  uint32       i;

  if (*pnAddr == 0)
  {
    return DAL_SUCCESS;
  }

  for (i = 0; i < pDrvCtxt->pBSP->nNumBases; i++)
  {
    pBase = &pDrvCtxt->pBSP->pBases[i];

    if (*pnAddr >= pBase->nPhysAddr &&
        *pnAddr < pBase->nPhysAddr + pBase->nSize)
    {
      *pnAddr = *pnAddr - pBase->nPhysAddr + *pBase->pnVirtAddr;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END VCS_MapAddress */


/* =========================================================================
**  Function : VCS_InitBases
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_InitBases
(
  VCSDrvCtxt *pDrvCtxt
)
{
  VCSBSPType  *pBSP;
  VCSBaseType *pBase;
  DALResult    eResult;
  uint32       i;

  pBSP = pDrvCtxt->pBSP;
  if (pBSP->pBases == NULL || pBSP->nNumBases == 0)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Map base regions.                                                     */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pBSP->nNumBases; i++)
  {
    pBase = &pBSP->pBases[i];
    eResult = VCS_MapHWIORegion(pBase->nPhysAddr, pBase->nSize, pBase->pnVirtAddr);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  return DAL_SUCCESS;

} /* END VCS_InitBases */


/* =========================================================================
**  Function : VCS_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult VCS_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCS_DriverDeInit */


/* =========================================================================
**  Function : VCSStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the VCS driver stub.

  This function shuts-down the VCS driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult VCSStub_DriverDeInit
(
  VCSDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END VCSStub_DriverDeInit */


/*=========================================================================
**  Function : VCS_IsBSPSupported
** =========================================================================*/
/**
 *
  See VCSDriver.h
*/

boolean VCS_IsBSPSupported
(
  VCSHWVersionType *pHWVersion
)
{
  uint32                   nHWVersion, nHWMaxVersion, nHWMinVersion;
  DalChipInfoIdType        eChipId;
  const DalChipInfoIdType *aeChipInfoId;

  /*-----------------------------------------------------------------------*/
  /* Validate argument pointer is usable.                                  */
  /*-----------------------------------------------------------------------*/

  if (pHWVersion == NULL)
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate chip family. If not provided then we do not check.           */
  /*-----------------------------------------------------------------------*/

  if ( (pHWVersion->eChipInfoFamily != 0) &&
       (pHWVersion->eChipInfoFamily != DalChipInfo_ChipFamily()) )
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Ensure the specific chip ID is supported.                             */
  /*-----------------------------------------------------------------------*/

  aeChipInfoId = pHWVersion->aeChipInfoId;

  if (aeChipInfoId != NULL)
  {
    eChipId = DalChipInfo_ChipId();

    while (*aeChipInfoId < DALCHIPINFO_NUM_IDS)
    {
      if (eChipId == *aeChipInfoId)
      {
        break;
      }

      aeChipInfoId++;
    }

    if (*aeChipInfoId == DALCHIPINFO_NUM_IDS)
    {
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the actual hardware revision.                                     */
  /*-----------------------------------------------------------------------*/

  nHWVersion = HAL_vcs_GetChipVersion();

  nHWMinVersion =
    DALCHIPINFO_VERSION(pHWVersion->Min.nMajor, pHWVersion->Min.nMinor);
  nHWMaxVersion =
    DALCHIPINFO_VERSION(pHWVersion->Max.nMajor, pHWVersion->Max.nMinor);

  /*-----------------------------------------------------------------------*/
  /* No chip version data implies support in all HW versions.              */
  /*-----------------------------------------------------------------------*/

  if ( (nHWMinVersion == 0) && (nHWMaxVersion == 0) )
  {
    return TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the HW version falls within the min/max.                     */
  /*-----------------------------------------------------------------------*/

  if ( (nHWVersion >= nHWMinVersion) && (nHWVersion < nHWMaxVersion) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* END VCS_IsBSPSupported */
