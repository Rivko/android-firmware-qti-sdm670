#ifndef DALPLATFORMINFO_H
#define DALPLATFORMINFO_H

/**
  @file PlatformInfo.c

  Public header file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.tz/2.0.2/api/systemdrivers/PlatformInfo.h#1 $
  $DateTime: 2018/02/06 02:57:17 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfoDefs.h"
#include "DALSysTypes.h"


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : PlatformInfo_Init
** =========================================================================*/
/**
  Initialize the PlatformInfo driver.
*/
DALResult PlatformInfo_Init (void);


/* =========================================================================
**  Function : PlatformInfo_GetPlatform
** =========================================================================*/
/**
  Return the current platform.
*/
DalPlatformInfoPlatformType PlatformInfo_GetPlatform (void);


/* =========================================================================
**  Function : PlatformInfo_GetPlatformInfo
** =========================================================================*/
/**
  Return information on the current platform.

  @return
    TRUE -- Success.
    FALSE -- The pInfo parameter is NULL.
*/
DALResult PlatformInfo_GetPlatformInfo
(
  DalPlatformInfoPlatformInfoType *pInfo
);


/* =========================================================================
**  Function : PlatformInfo_IsFusion
** =========================================================================*/
/**
  Determine if the platform is a Fusion variant.

  @return
    TRUE -- Platform is a Fusion variant.
    FALSE -- Platform is not a Fusion variant.
*/
boolean PlatformInfo_IsFusion (void);


/* =========================================================================
**  Function : PlatformInfo_GetKeyValue
** =========================================================================*/
/**
  Return a value for a given key.

  @return
    TRUE -- Key was found in the CDT.
    FALSE -- Key wasn't found in the CDT.
*/
DALResult PlatformInfo_GetKeyValue
(
  DalPlatformInfoKeyType   eKey,
  uint32                  *nValue
);

#endif

