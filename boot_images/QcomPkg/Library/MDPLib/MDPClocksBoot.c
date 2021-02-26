/*=============================================================================

  File: MDPClocksBoot.c

  Source file for MDP/Panel clock configuration


  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "MDPLib_i.h"
#include "MDSSPlatform_4xx.h"
#include "MDPSystem.h"
#include "com_dtypes.h"
#include "ClockRamDump.h"
/*=========================================================================
      Defines
==========================================================================*/

extern HAL_HW_VersionType  gsVersionInfo;
/* ---------------------------------------------------------------------- */
/**
** FUNCTION: SetupClock()
**
** DESCRIPTION:
**   Setup a specific clock
**
** ---------------------------------------------------------------------- */
static MDP_Status SetupClock( MDPClockEntry *pClock, UINT32 uFlags)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  if (NULL != pClock)
  {
    UINTN    uClockId = Clock_RD_GetClockID(pClock->szName);

    // Is this clock an external source?
    if (NULL != pClock->pExtSrc)
    {
      //Setup clock as an external source.
      if (TRUE != Clock_RD_SelectExternalSource(uClockId,
                                                0,                               // FreqHz: 0 -- No voltage control
                                                pClock->pExtSrc->uClkSource,
                                                pClock->pExtSrc->nClkDiv,
                                                pClock->pExtSrc->uClkPLL_M,      // M
                                                pClock->pExtSrc->uClkPLL_N,      // N
                                                pClock->pExtSrc->uClkPLL_2D))    // 2*D
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib : SelectExternalSource: Clock \"%a\" returned an error!\n", pClock->szName);

        eStatus = MDP_STATUS_FAILED;
      }
    }
    else if (pClock->nFreqHz > 0)
    {
      // Request a specific clock frequency
      if (TRUE != Clock_RD_SetClockFreqKHz(uClockId,pClock->nFreqHz))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib : SetClockFreqHz: Clock \"%a\" to %dHz returned an error!\n", pClock->szName, pClock->nFreqHz);
        eStatus = MDP_STATUS_FAILED;
      }
    }

    // Finally enable the clock (regardless of any errors above)
    if(TRUE != Clock_RD_EnableClock(uClockId))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: Clock \"%a\" cannot be enabled!\n", pClock->szName);

      eStatus = MDP_STATUS_FAILED;
    }
  }
  else
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }

  return eStatus;
}
/* ---------------------------------------------------------------------- */
/**
** FUNCTION: StrCompare()
**
** DESCRIPTION:
**   String compare helper function
**
** ---------------------------------------------------------------------- */
static INT32 StrCompare(CHAR8 *szSrc, CHAR8 *szDest)
{
  UINT32 uCount;
  INT32 nResult = 1;

  if ((NULL != szSrc) && (NULL != szDest))
  {
    for (uCount = 0;uCount < MDPLIB_MAX_CLOCK_NAME;uCount++)
    {
      if (szSrc[uCount] != szDest[uCount])
      {
        break;
      }
      else if (('\0' == szSrc[uCount]) && ('\0' == szDest[uCount]))
      {
        nResult = 0;
        break;
      }
      else if (('\0' == szSrc[uCount]) || ('\0' == szDest[uCount]))
      {
        break;
      }
    }
  }

  return nResult;
}
/* ---------------------------------------------------------------------- */
/**
** FUNCTION: FindExtClock()
**
** DESCRIPTION:
**   Find an external clock configuration that matches the current clock
**
** ---------------------------------------------------------------------- */
static MDPExternalClockEntry *FindExtClock(CHAR8 *szClockName, MDPExternalClockEntry *pExtClockList)
{
  UINT32 uCount = 0;
  MDPExternalClockEntry *pExtClock = NULL;

  // If we have an external clock list, search for a matching clock
  if (pExtClockList)
  {
    while ((pExtClockList[uCount].szName[0] != '\0'))
    {
      if (StrCompare(szClockName, (CHAR8*)pExtClockList[uCount].szName) == 0)
      {
        pExtClock = (MDPExternalClockEntry*)&pExtClockList[uCount];
        break;
      }
      uCount++;
    }
  }

  return pExtClock;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: ConfigurePowerDomain()
**
** DESCRIPTION:
**   Enabled/disable a specific power domain
**
** ---------------------------------------------------------------------- */
static MDP_Status ConfigurePowerDomain(BOOLEAN bEnable, CHAR8 *szDomain)
{
  MDP_Status   eStatus = MDP_STATUS_OK;
  UINTN        uClockPowerDomainId;

  uClockPowerDomainId = Clock_RD_GetClockPowerDomainID(szDomain);

  if (TRUE == bEnable)
  {
    if (TRUE != Clock_RD_EnableClockPowerDomain( uClockPowerDomainId))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: Clock_RD_EnableClockPowerDomain failed = %s!\n", szDomain);
      eStatus = MDP_STATUS_NO_RESOURCES;
    }
  }
  else if (FALSE == bEnable)
  {
    if (TRUE != Clock_RD_DisableClockPowerDomain( uClockPowerDomainId))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: Clock_RD_DisableClockPowerDomain failed = %s!\n",szDomain);
      eStatus = MDP_STATUS_NO_RESOURCES;
    }
  }

  return eStatus;
}

static MDP_Status GetMDPVersionIndex(MDP_HwPrivateInfo  *psMDPHwPrivateInfo)
{
  MDP_Status   eStatus = MDP_STATUS_OK;

  if (psMDPHwPrivateInfo->sMDPVersionInfo.uMajorVersion == 0)
  {
    // Default, set SDM845 family i.e MDSS 4.0
    psMDPHwPrivateInfo->sMDPVersionInfo.uMajorVersion = 0x04;
    psMDPHwPrivateInfo->sMDPVersionInfo.uMinorVersion = 0x00;
  }
  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPSetupClocks()
**
** DESCRIPTION:
**   Display Clock configuration.
**
** ---------------------------------------------------------------------- */
MDP_Status MDPSetupClocks(MDPClockTypes eClockType, MDPExternalClockEntry *pExtClockList)
{
  MDP_Status           eStatus = MDP_STATUS_OK;
  MDP_HwPrivateInfo   *psMDPHwPrivateInfo = MDP_GETPRIVATEINFO();

  if ((EFI_PLATFORMINFO_TYPE_VIRTIO  == psMDPHwPrivateInfo->eEFIPlatformInfo) ||
      (EFI_PLATFORMINFO_TYPE_RUMI    == psMDPHwPrivateInfo->eEFIPlatformInfo) ||
      (EFI_PLATFORMINFO_TYPE_UNKNOWN == psMDPHwPrivateInfo->eEFIPlatformInfo))
  {
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib: For VIRTIO or RUMI, ignore this error and continue\n");
    eStatus = MDP_STATUS_OK;
  }
  else
  {
    MDPClockEntry                  *pClockList = NULL;
    MDP_HwMajorFamilyResourceList  *pResListMajor;
    MDP_HwMinorFamilyResourceList  *pResListMinor;
    MDP_ResourceList               *pClkResList;
    if (MDP_STATUS_OK != GetMDPVersionIndex(psMDPHwPrivateInfo))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayLib:GetMDPVersionIndex  failed!\n");
    }
    else
    {
      pResListMajor = (MDP_HwMajorFamilyResourceList*)&(asHarwareFamilyMajor[psMDPHwPrivateInfo->sMDPVersionInfo.uMajorVersion]);
      pResListMinor = (MDP_HwMinorFamilyResourceList*)&(pResListMajor->pMinorFamily[psMDPHwPrivateInfo->sMDPVersionInfo.uMinorVersion]);
      pClkResList   = (MDP_ResourceList*)pResListMinor->pDisplayPlatfromResources->pResList;
      
      // if we have a valid resource list
      if (NULL != pClkResList)
      {
        // Handle pre-clock events
        switch (eClockType)
        {
        case MDP_CLOCKTYPE_CORE:

          if(NULL != pClkResList->pDispCCXOClk)
          {
              MDPClockEntry sClockEntry;

              pClockList = (MDPClockEntry*)(pClkResList->pDispCCXOClk);
              while (pClockList->szName[0] != '\0')
              {
                  sClockEntry = *pClockList;
                  (void)SetupClock(&sClockEntry, 0);
                  pClockList++;
              }
          }

          pClockList = (MDPClockEntry*)(pClkResList->pMDPClocks);

          /* Enable the power domain for access in to the MDSS clocks */
          if (NULL != pClkResList->pPowerDomain)
          {
            UINT32                    uCount = 0;

            /* go through the power domain list to enable it in order */
            while (pClkResList->pPowerDomain[uCount].szName[0] != '\0')
            {
              ConfigurePowerDomain(TRUE, (CHAR8*)pClkResList->pPowerDomain[uCount].szName);
              uCount++;
            }
          }
          break;
        case MDP_CLOCKTYPE_DSI0:
          pClockList = (MDPClockEntry*)(pClkResList->pDSI0Clocks);
          break;
        case MDP_CLOCKTYPE_DSI1:
          pClockList = (MDPClockEntry*)(pClkResList->pDSI1Clocks);
          break;
        case MDP_CLOCKTYPE_HDMI:
          pClockList = (MDPClockEntry*)(pClkResList->pHDMIClocks);
          break;
        case MDP_CLOCKTYPE_DP:
          pClockList = (MDPClockEntry*)(pClkResList->pDPClocks);
          break;
        default:
          break;
        }
      }
      // if we have a valid clock list
      if (NULL != pClockList)
      {
        UINT32 uCount = 0;

        while ((pClockList[uCount].szName[0] != '\0'))
        {
          // Create a local copy of the clock entry
          MDPClockEntry sClockEntry = pClockList[uCount];

          // Override with external clock list (if it is not configured already)
          if (NULL == sClockEntry.pExtSrc)
          {
            sClockEntry.pExtSrc = FindExtClock((CHAR8*)&sClockEntry.szName, pExtClockList);
          }

          // Setup the clock (ignore failures)
          (void)SetupClock(&sClockEntry, 0);

          // Go to the next clock
          uCount++;
        }
      }
    }
  }

  return eStatus;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPDisableClocks()
**
** DESCRIPTION:
**   Display Clock configuration
**
** ---------------------------------------------------------------------- */
MDP_Status MDPDisableClocks(MDPClockTypes eClockType)
{
  /*Stub function, XBL RAM dump mode won't turn off the display only*/ 
  return MDP_STATUS_OK;
}



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPGetExternalClockList()
**
** DESCRIPTION:
**   Return list of external clocks for this chipset
**
** ---------------------------------------------------------------------- */
MDP_ExtClockResourceList *MDPGetExternalClockList(void)
{
  MDP_HwPrivateInfo               *psMDPHwPrivateInfo  = MDP_GETPRIVATEINFO();
  MDP_HwMajorFamilyResourceList   *pResListMajor;
  MDP_HwMinorFamilyResourceList   *pResListMinor;
  MDP_ExtClockResourceList        *pExtClockResList;
  
  pResListMajor      = (MDP_HwMajorFamilyResourceList*)&(asHarwareFamilyMajor[psMDPHwPrivateInfo->sMDPVersionInfo.uMajorVersion]);
  pResListMinor      = (MDP_HwMinorFamilyResourceList*)&(pResListMajor->pMinorFamily[psMDPHwPrivateInfo->sMDPVersionInfo.uMinorVersion]);
  pExtClockResList   = (MDP_ExtClockResourceList*)pResListMinor->pDisplayPlatfromResources->pExtClockResList;

  return pExtClockResList;
}

