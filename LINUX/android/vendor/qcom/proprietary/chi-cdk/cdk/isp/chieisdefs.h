////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chieisdefs.h
/// @brief Define Qualcomm Technologies, Inc. EIS proprietary data for defining internal data structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIEISDEFS_H
#define CHIEISDEFS_H
#include "camxcdktypes.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus


/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const CHAR* VendorTagSectionEISLookAheadConfig = "org.quic.camera.eislookahead"; ///< Lookahead EIS config vendor tag section
static const CHAR* VendorTagNameEISLookAheadConfig    = "EISLookAheadConfig";           ///< Lookahead EIS config vendor tag name

static const CHAR* VendorTagSectionEISRealTimeConfig = "org.quic.camera.eisrealtime"; ///< RealTime EIS config vendor tag section
static const CHAR* VendorTagNameEISRealTimeConfig    = "EISRealTimeConfig";           ///< RealTime EIS config vendor tag name

static const CHAR* VendorTagSectionEISDGRealTimeConfig = "org.quic.camera.eisdgrealtime"; ///< RealTime EIS DG config vendor tag section
static const CHAR* VendorTagNameEISDGRealTimeConfig    = "EISDGRealTimeConfig";           ///< RealTime EIS DG config vendor tag name


/// @brief Structure Stabilization Margins to be set to the ICA or GPU
struct StabilizationMargin
{
    UINT32 widthPixels;  ///< Margins on the width in pixels
    UINT32 heightLines;  ///< Margins on the height in pixels
};

/// @brief Structure Request Margins during buffer negotiation
struct MarginRequest
{
    FLOAT widthMargin;   ///<Request margins on the width in percentage (Eg. 0.2 for 20%)
    FLOAT heightMargin;  ///<Request margins on the height in percentage (Eg. 0.1 for 10%)
};


#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIIPEDEFS_H
