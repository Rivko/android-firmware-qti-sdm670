#ifndef __PLATFORMINFOINTERNAL_H__
#define __PLATFORMINFOINTERNAL_H__
/*
===========================================================================
*/
/**
  @file PlatformInfo.h

  Internal header file for the PlatformInfo driver.
*/
/*
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================


  ====================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "PlatformInfo.h"
#include "DALSys.h"
#include "DALGlbCtxt.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

typedef struct
{
  uint32 nKey;
  uint32 nValue;
} PlatformInfoKVPSType;

/*
 * PlatformInfoGlbCtxType
 *
 * Global context data structure for sharing the platform across
 * processors.
 */
typedef struct
{
  DALGLB_HEADER                     DALGLBHeader;
  DalPlatformInfoPlatformInfoType   PlatformInfo;
  uint32                            nNumKVPS;
  PlatformInfoKVPSType              aKVPS[];
} PlatformInfoGlbCtxType;

/*
 * PlatformInfo driver context.
 */
typedef struct PlatformInfoDrvCtxt
{
  boolean                          bInitialized;
  PlatformInfoGlbCtxType          *pGlbCtxt;
  DalPlatformInfoPlatformInfoType  PlatformInfo;
  uint32                           nNumKVPS;
  PlatformInfoKVPSType            *aKVPS;
} PlatformInfoDrvCtxt;

/*=========================================================================
      Function Definitions
==========================================================================*/

DalPlatformInfoPlatformType PlatformInfo_DetectPlatformType (void);
DALResult PlatformInfo_GetPropertyValue (const char *szName, void **pValue);
PlatformInfoDrvCtxt *PlatformInfo_GetDrvCtxt();

#endif

