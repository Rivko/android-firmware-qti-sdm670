////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Copyright (c) 2017, 2019 Qualcomm Technologies, Inc.
//// All Rights Reserved.
//// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chiifedefs.h
/// @brief Define Qualcomm Technologies, Inc. IFE proprietary data for defining internal data structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIIFEDEFS_H
#define CHIIFEDEFS_H
#include "camxcdktypes.h"
#include "chicommontypes.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus
#pragma pack(push, CDK_PACK_SIZE)

    /// @brief IFE Crop Data associated with vendor tag
    typedef struct
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// fullPath
        ///
        /// Description:   IFE Full output path crop info
        /// Type:          CHIRectangle
        /// Default Value: 0
        /// Range:         [-8192, 8191]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CHIRectangle fullPath;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// FDPath
        ///
        /// Description:   IFE FD output path crop info
        /// Type:          CHIRectangle
        /// Default Value: 0
        /// Range:         [-8192, 8191]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CHIRectangle FDPath;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// DS4Path
        ///
        /// Description:   IFE DS4 output path crop info
        /// Type:          CHIRectangle
        /// Default Value: 0
        /// Range:         [-8192, 8191]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CHIRectangle DS4Path;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// DS16Path
        ///
        /// Description:   IFE DS16 output path crop info
        /// Type:          CHIRectangle
        /// Default Value: 0
        /// Range:         [-8192, 8191]
        /// Bit Depth:     1u
        /// Conversion:    No Conversion needed
        /// Notes:         None
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CHIRectangle DS16Path;

    } IFECropInfo;


    /// @ brief IFE Scale Data associated with vendor tag
    typedef struct
    {
        CHIDimension             output;         ///< Scaler out put width and height
        CHIDimension             input;          ///< Scaler input dimension
        FLOAT                    scalingFactor;  ///< Scaler input to output scaling
    }IFEScalerInfo;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIIFEDEFS_H



