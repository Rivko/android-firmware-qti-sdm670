////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chisoftwarestatsinterface.h
/// @brief Software Stats Generation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHISOFTWARESTATSINTERFACE_H
#define CHISOFTWARESTATSINTERFACE_H

#include "chiafcommon.h"
#include "chistatsinterfacedefs.h"
#include "camxdefs.h"
#include "camxosutils.h"
#include "camxutils.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Pack data structures to ensure consistent layout.
#pragma pack(push, CDK_PACK_SIZE)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GenerateBFStats
///
/// @brief  This method creates an instance to the GenerateSoftwareBFStats
///
/// @param  pImage          Y-plane image buffer address
/// @param  pImageDimension Pointer to Image Width and Height
/// @param  stride          image width + pixel padding
/// @param  pConfiguration  Pointer to AF Configuration. ROI needs to be 1/3rd of the imageDimension
/// @param  pBFStats        Output BF stat structure to be filled
///
/// @return Failed only if all pixel values are zero.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*CREATEGENERATEBFSTATS)(
    const UINT8*            pImage,
    const StatsDimension*   pImageDimension,
    const UINT32            stride,
    const AFConfigParams*   pConfiguration,
    AFBAFStatistics*        pBFStats);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// GenerateRSStats
///
/// @brief  This method creates an instance to the GenerateSoftwareRSStats
///
/// @param  pImage          Y-plane image buffer address
/// @param  pImageDimension Pointer to Image Width and Height
/// @param  stride          image width + pixel padding
/// @param  pConfiguration  Pointer to RS stats configuration from stats
/// @param  pRowSumStats    Output RS stat structure to be filled
///
/// @return Failed only if all pixel values are zero.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CamxResult (*CREATEGENERATERSSTATS)(
    const UINT8*            pImage,
    const StatsDimension*   pImageDimension,
    const UINT32            stride,
    const AFDStatsRSConfig* pConfiguration,
    StatsRowSum*            pRowSumStats);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHISOFTWARESTATSINTERFACE_H
