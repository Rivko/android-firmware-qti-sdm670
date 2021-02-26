////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxdefs.h
/// @brief CHX common definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXDEFS_H
#define CHXDEFS_H

#include <hardware/camera3.h>
#include "camxcdktypes.h"
#include "common/chivendortag.h"

#define CHX_STATIC_ASSERT(condition)   static_assert(condition, #condition)

static const UINT MaxChiStreams = 16;

// List of Vendor Tags
enum VendorTag
{
    SensorBpsModeIndex = 0,                    ///< SensorBpsModeIndex
    SensorBpsGain      = 1,                    ///< SensorBpsGain
    DebugDataTag       = 2,                    ///< DebugData
    SensorModeIndex    = 3,                    ///< SensorModeIndex
    CropRegions        = 4,                    ///< Crop Regions
    TuningMode         = 5,                    ///< TagTuningMode
    RefCropSize        = 6,                    ///< Ref crop size
    MultiCamera        = 7,                    ///< multicamera
    IsFlashRequiredTag = 8,                    ///< Is Flash Required for Snapshot
    Feature1Mode       = 9,                    ///< Feature1Mode
    Feature2Mode       = 10,                   ///< Feature1Mode
    VideoHDR10Mode     = 11,                   ///< Video HDR10 Mode
    NumVendorTags      = 12                    ///< Max Tags
};

#endif // CHXDEFS_H
