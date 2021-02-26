/*=============================================================================
 
  File: MDPLIb.c
 
  Source file for MDP functions
  
 
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"

/*=========================================================================
     Default Defines
==========================================================================*/

/* Maximum number of minor versions supported */
#define MDP_MAX_MINOR_VERSION            6
  
              
/*=========================================================================
     Local Static Variables
==========================================================================*/



/*=========================================================================
     Local Static Functions
==========================================================================*/

/* MDSS3xx Resolution Capabilities
*/
static const MDP_ResolutionCaps sResolutionCaps_MDSS4xx =
{
    2,     /* uMinLayerWidth  */
    2,     /* uMinLayerHeight */
    2560,  /* uMaxLayerWidth  */
    2048,  /* uMaxLayerHeight */
};


/**************************************************************
* MDSS3xx PP (Ping-Pong) Capabilities
*
* Note: set MDP_PINGPONG_CAP_PINGPONG_SPLIT in uPPFeatureFlags to enable pingpong-split
*************************************************************/
static const MDP_PingPongCaps sPPCaps_MDSS4xx[HAL_MDP_PINGPONG_MAX] =
{
    /* bSupported, uMaxWidth, uPPFeatureFlags */
    { FALSE,       0,         0 },                                // HAL_MDP_PING_PONG_NONE
    { TRUE,        2560,      MDP_PINGPONG_CAP_PINGPONG_SPLIT },  // HAL_MDP_PING_PONG_0
    { TRUE,        2560,      MDP_PINGPONG_CAP_PINGPONG_SPLIT },  // HAL_MDP_PING_PONG_1
    { TRUE,        4096,      0 },                                // HAL_MDP_PING_PONG_2
    { TRUE,        4096,      0 },                                // HAL_MDP_PING_PONG_3
    { FALSE,       0,         0 },                                // HAL_MDP_PING_PONG_4
};


/* Mapping from master ping-pong to slave ping-pong used ONLY when ping-pong split is enabled. */
static const HAL_MDP_PingPongId sMDSS4xx_PPSplitSlaveMap[HAL_MDP_PINGPONG_MAX] =
{
    HAL_MDP_PINGPONG_NONE, // HAL_MDP_PINGPONG_NONE,
    HAL_MDP_PINGPONG_4,    // HAL_MDP_PINGPONG_0,
    HAL_MDP_PINGPONG_4,    // HAL_MDP_PINGPONG_1,
    HAL_MDP_PINGPONG_NONE, // HAL_MDP_PINGPONG_2,
    HAL_MDP_PINGPONG_NONE, // HAL_MDP_PINGPONG_3,
    HAL_MDP_PINGPONG_NONE  // HAL_MDP_PINGPONG_4,
};

/* Slave PingPong Buffer ID for PP split for all MDP versions
*/
static const HAL_MDP_PingPongId *PingPongSplitSlaveMapInfo[MDP_MAX_MINOR_VERSION] =
 {
    &sMDSS4xx_PPSplitSlaveMap[0],   /* MDSS 4.0.0  */
    &sMDSS4xx_PPSplitSlaveMap[0],   /* MDSS 4.1.0  */
    NULL,                           /* MDSS 4.2.0  */
    NULL,                           /* MDSS 4.3.0  */
    NULL,                           /* MDSS 4.4.0  */
    NULL                            /* MDSS 4.5.0  */
};

/* MDSS4xx Device Capabilities
 */
static const MDP_DeviceCaps sDeviceCaps_MDSS4xx =
{
  &sResolutionCaps_MDSS4xx,
  &sPPCaps_MDSS4xx[0]
};


/* MDP Device caps  for all MDP versions
 */
static const MDP_DeviceCaps *DeviceCapInfo[MDP_MAX_MINOR_VERSION] =
{
  &sDeviceCaps_MDSS4xx,           /* MDSS 4.0.0  */
  &sDeviceCaps_MDSS4xx,           /* MDSS 4.1.0  */
  NULL,                           /* MDSS 4.2.0  */
  NULL,                           /* MDSS 4.3.0  */
  NULL,                           /* MDSS 4.4.0  */
  NULL                            /* MDSS 4.5.0  */
};

/*=========================================================================
      Public APIs
==========================================================================*/

MDP_Status MDPInitHwPrivateInfo(MDP_HwPrivateInfo *pHwPrivateInfo)
{
  MDP_Status          eStatus             = MDP_STATUS_OK;
  HAL_MDP_HwInfo      sHwInfo; 

  if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_ReadVersionInfo(&(pHwPrivateInfo->sMDPVersionInfo), &sHwInfo))
  {
     eStatus = MDP_STATUS_HW_ERROR;
  }
  else if ((pHwPrivateInfo->sMDPVersionInfo.uMinorVersion >= MDP_MAX_MINOR_VERSION) ||
           (NULL == DeviceCapInfo[pHwPrivateInfo->sMDPVersionInfo.uMinorVersion]))
  {
    // Invalid or unsupported hardware version
    eStatus = MDP_STATUS_NOT_SUPPORTED;
  }
  else
  {
    // MDP Device Caps
    pHwPrivateInfo->pDeviceCaps = DeviceCapInfo[pHwPrivateInfo->sMDPVersionInfo.uMinorVersion];

    // MDP PingPong buffer split slave ID mapping table
    pHwPrivateInfo->pPPSplitSlaveMap = PingPongSplitSlaveMapInfo[pHwPrivateInfo->sMDPVersionInfo.uMinorVersion];
  }

  return eStatus;
}

