////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file chiipedefs.h
/// @brief Define Qualcomm Technologies, Inc. IPE proprietary data for defining internal data structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHIIPEDEFS_H
#define CHIIPEDEFS_H
#include "camxcdktypes.h"

#ifdef __cplusplus

extern "C"
{
#endif // __cplusplus


/// @brief Data structures that are exposed to OEM must be packed to have the expected layout.
#pragma pack(push, CDK_PACK_SIZE)

static const UINT ICAMaxPerspectiveTransform           = 9;
static const UINT ICAParametersPerPerspectiveTransform = 9;
static const UINT ICAGridTransformWidth                = 33;
static const UINT ICAGridTransformHeight               = 25;

/// @brief IPE ICA Grid Array coordinates
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ICAGridArray x coordinate
    ///
    /// Description:   points x coordinate
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range:         [-8192.0f-8191.75f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT x;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ICAGridArray y coordinate
    ///
    /// Description:   points y coordinate
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range          [-6144.0f-6144.0f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT y;
} ICAGridArray;

/// @brief IPE ICA perspective transform associated with vendor tag
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// perspectiveTransformEnable
    ///
    /// Description:   Enable or Disable perspective transform
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 perspectiveTransformEnable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ReusePerspectiveTransform
    ///
    /// Description:   Reuse stored perspective transform from older frame
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 ReusePerspectiveTransform;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// perspetiveGeometryNumColumns
    ///
    /// Description:   The geometry of the perspective transform partitioning is described in terms of MxN. N stands for column
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [1, 9]. MxN = 9
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 perspetiveGeometryNumColumns;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// perspectiveGeometryNumRows
    ///
    /// Description:   The geometry of the perspective transform partitioning is described in terms of MxN. M stands for row
    /// Type:          UINT8
    /// Default Value: 0
    /// Range:         [1, 9]. MxN <= 9
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT8 perspectiveGeometryNumRows;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// perspectiveTransformArray
    ///
    /// Description:   Perspective transform parameters, Made up of 81 floats (up to 9 sets * 9 parameters each)
    /// Type:          FLOAT
    /// Default Value: 1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    ///                1.0f 0.0f 0.0f 0.0f 1.0f 0.0f 0.0f 0.0f 1.0f
    /// Range:         -4294836224.0f - 2147418112.0f
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT perspectiveTransformArray[ICAMaxPerspectiveTransform *
        ICAParametersPerPerspectiveTransform];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// perspectiveConfidence
    ///
    /// Description:   Confidence related parameter.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0-1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 perspectiveConfidence;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transformDefinedOnWidth
    ///
    /// Description:   Width on which transform is defined.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [4, 8192]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 transformDefinedOnWidth;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transformDefinedOnHeight
    ///
    /// Description:   Height on which transform is defined.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [4, 8192]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 transformDefinedOnHeight;
} IPEICAPerspectiveTransform;

/// @brief IPE ICA grid transform associated with vendor tag
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gridTransformEnable
    ///
    /// Description:   Parameter that indicates whether grid transform is enabled and in which mode.
    ///                0 = disabled, 1 = enabled using the Chromatix settings, 2 = enabled using the settings from this structure
    /// Type:          UINT32

    /// Default Value: 1
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 gridTransformEnable;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reuseGridTransform
    ///
    /// Description:   Reuse stored grid transform from older frame
    /// Type:          UINT8
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 reuseGridTransform;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gridTransformArray (x,y) coordinates
    ///
    /// Description:   Made up of 825 integers: 33 * 25 entries, spanning uniformly across the output image:
    ///                Top-left point maps coordinate (0,0) of the output image.
    ///                Top-right point maps coordinate (OutputWidth,0) of the output image.
    ///                Bottom-left point maps coordinate (0,OutputHeight) of the output image.
    ///                Bottom-left point maps coordinate (OutputWidth,OutputHeight) of the output image.
    /// Type:          ICAGridArray
    /// Default Value: 0
    /// Range:         X: [-8192.0f-8191.75f], Y : [-6144.0f-6144.0f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         The origins are located in the center of the input image.
    ///                Grid points map output coordinate to input coordinates. (0,0) is in the center of image
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICAGridArray gridTransformArray[ICAGridTransformWidth *
        ICAGridTransformHeight];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gridTransformArrayExtrapolatedCorners
    ///
    /// Description:   Parameter that indicates whether corners should be extrapolated or custom values are provided for corners
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 gridTransformArrayExtrapolatedCorners;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// gridTransformArrayCorners (x,y) coordinates
    ///
    /// Description:   Made up of 4 float corresponding to the corner pixels: top-left, top-right, bottom-left, bottom-right.
    ///                Used when grid_transform_array_extrapolated_corners is 1 or true.
    /// Type:          ICAGridArray
    /// Default Value: 0
    /// Range:         [-8192.0f-8191.75f].
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICAGridArray gridTransformArrayCorners[4];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transformDefinedOnWidth
    ///
    /// Description:   Width on which transform is defined.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [4, 8192]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 transformDefinedOnWidth;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// transformDefinedOnHeight
    ///
    /// Description:   Height on which transform is defined.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [4, 8192]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 transformDefinedOnHeight;
} IPEICAGridTransform;

/// @brief IPE ICA custom interpolation associated with vendor tag
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// customInterpolationEnabled
    ///
    /// Description:   Enable custom interpolation parameters. If disabled and y_interpolation_mode is 0 the interpolation is bicubic.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 customInterpolationEnabled;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reuseCustomInterpolationParams
    ///
    /// Description:   Reuse custom interpolation parameters from past frame
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 reuseCustomInterpolationParams;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// customInterpolationCoefficients0
    ///
    /// Description:   Interpolation coefficients for luma samples when y_interpolation_mode uses 4x4 kernel.
    /// Type:          INT16
    /// Default Value: 0 -60 -113 -158 -196 -228 -254 -273 -288 -298 -303 -303 -300 -293 -284 -271
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT16 customInterpolationCoefficients0[16];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// customInterpolationCoefficients1
    ///
    /// Description:   Interpolation coefficients for luma samples when y_interpolation_mode uses 4x4 kernel.
    /// Type:          INT16
    /// Default Value: 2304 4086 4058 4011 3948 3869 3777 3670 3552 3423 3284 3136 2980 2818 2651 2479
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT16 customInterpolationCoefficients1[16];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// customInterpolationCoefficients2
    ///
    /// Description:   Interpolation coefficients for luma samples when y_interpolation_mode uses 4x4 kernel.
    /// Type:          INT16
    /// Default Value: -256 72 159 259 372 497 632 776 928 1087 1253 1422 1596 1772 1950 2127
    /// Range:         [-8192, 8191]
    /// Bit Depth:     14s
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    INT16 customInterpolationCoefficients2[16];

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// yInterpolationMode
    ///
    /// Description:   Parameter that defines the interpolation mode. 0-4x4 kernel, 1-Bi-linear.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 yInterpolationMode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// outOfFramePixelFillMode
    ///
    /// Description:   Parameter that defines Out-of-frame pixel is populated with a predefined value or duplication.
    ///                0: Out-of-frame pixel is populated with a predefined value.
    ///                1: Out-of-frame pixel is populated using duplication.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 outOfFramePixelFillMode;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// outOfFramePixelFillConstY
    ///
    /// Description:   Y Output sample values for out-of-input-frame pixels when populated with predefined value.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 outOfFramePixelFillConstY;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// outOfFramePixelFillConstCb
    ///
    /// Description:   Cb Output sample values for out-of-input-frame pixels when populated with predefined value.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 outOfFramePixelFillConsCb;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// outOfFramePixelFillConsCr
    ///
    /// Description:   Cr Output sample values for out-of-input-frame pixels when populated with predefined value.
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [0, 1023]
    /// Bit Depth:     10u
    /// Conversion:    No Conversion needed
    /// Notes:         None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 outOfFramePixelFillConsCr;
} IPEICAInterpolationParams;

/// @ triangulation of grid coordinates
typedef enum
{
    NE_DIRECTION       = 0,  ///< triangle strip NE direction
    NW_DIRECTION       = 1,  ///< triangle strip NW direction
    TOGGLING_DIRECTION = 2   ///< triangle strip Toggling direction
} UniformGridTriangulation;

/// @ extrapolation type used for out of grid samples
typedef enum
{
    GRID_EXTRAPOLATION_NEAREST       = 0, ///< using the coordinate of nearest neighbor
    GRID_EXTRAPOLATION_MV_NEAREST    = 1, ///< using the motion vector of nearest neighbor
    GRID_EXTRAPOLATION_APPROX_LINEAR = 2  ///< Approximate linear extrapolation (this is the recommended value)
} InverseGridExtrapolationType;

/// @brief IPE ICA inverse grid transform input associated with vendor tag
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// numRows
    ///
    /// Description:   # of samples in srcGridRow and # of rows in dstGridArray
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [1, ICAGridTransformHeight]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                   numRows;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// numColumns
    ///
    /// Description:   # of samples in srcGridColumn and # of col in dstGridArray
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [1, ICAGridTransformWidth]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                   numColumns;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// srcGridRow
    ///
    /// Description:   Source rectangular grid y coordinates of rows
    /// Type:          FLOAT*
    /// Default Value: 0
    /// Range:         [-6144.00f-6144.00f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid,
    ///                The origins are located in the center of the input image.
    ///                (0, 0) is in the center of image
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT*                   srcGridRow;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// srcGridColumn
    ///
    /// Description:   Source rectangular grid x coordinates of columns
    /// Type:          FLOAT*
    /// Default Value: 0
    /// Range:         [-8192.0f-8191.75f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid
    ///                The origins are located in the center of the input image.
    ///                (0, 0) is in the center of image
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT*                   srcGridColumn;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// dstGridArray
    ///
    /// Description:   2-dimensional array of transform definedOn destination coordinates
    /// Type:          ICAGridArray*
    /// Default Value: 0
    /// Range:         [-8192.0f-8191.75f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         Max size ICAGridTransformHeight * ICAGridTransformWidth. parameters of forward grid.
    ///                The origins are located in the center of the input image. (0, 0) is in the center of image
    ///                at transformDefinedOnWidth/2, transformDefinedOnHeight/2 to keep centered coordinates
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICAGridArray*            dstGridArray;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// inputTriangulationType
    ///
    /// Description:   triangulation type to be used at source
    /// Type:          enum UniformGridTriangulation
    /// Default Value: 0
    /// Range:         [0-2]
    /// Bit Depth:     enum
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UniformGridTriangulation inputTriangulationType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// extrapolationType
    ///
    /// Description:   extrapolation type used for out-of-grid samples, Recommended value is
    ///                GRID_EXTRAPOLATION_APPROX_LINEAR.
    /// Type:          enum InverseGridExtrapolationType
    /// Default Value: 0
    /// Range:         [0-2]
    /// Bit Depth:     enum
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of forward grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    InverseGridExtrapolationType extrapolationType;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridNumRows
    ///
    /// Description:   Number of rows in inverse grid
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [1-ICAGridTransformHeight]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                   reversedGridNumRows;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridNumColumns
    ///
    /// Description:   Number of columns in inverse grid
    /// Type:          UINT32
    /// Default Value: 0
    /// Range:         [1-ICAGridTransformWidth]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32                   reversedGridNumColumns;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridSourceTopLeftX
    ///
    /// Description:   Inverse grid: first x coordinate at source
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range:         [-4096.00f-4096.00f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                    reversedGridSourceTopLeftX;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridSourceTopLeftY
    ///
    /// Description:   Inverse grid: first y coordinate at source
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range:         [-3072.00f-3072.00f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                    reversedGridSourceTopLeftY;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridSourceBottomRightX
    ///
    /// Description:   Inverse grid: last x coordinate at source
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range:         [-4096.00f-4096.00f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                    reversedGridSourceBottomRightX;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// reversedGridSourceBottomRightY
    ///
    /// Description:   Inverse grid: last y coordinate at source
    /// Type:          FLOAT
    /// Default Value: 0
    /// Range:         [-3072.00f-3072.00f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         parameters of inverse grid
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT                    reversedGridSourceBottomRightY;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// useExtraCalcsDuringExtrapolation
    ///
    /// Description:   Use extra calculation for corner cases during extrapolation
    /// Type:          BOOL
    /// Default Value: 0
    /// Range:         [0-1]
    /// Bit Depth:     1u
    /// Conversion:    No Conversion needed
    /// Notes:         flags controlling function execution, has runtime penalty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL                     useExtraCalcsDuringExtrapolation;
} IPEICAInverGridInput;

/// @brief IPE ICA grid transform output associated with vendor tag
typedef struct
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// inverseGridArray
    ///
    /// Description:   2-dimensional array of transform definedOn destination coordinates
    /// Type:          ICAGridArray*
    /// Default Value: 0
    /// Range:         [-8192.0f-8191.75f]
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         Max size ICAGridTransformHeight * ICAGridTransformWidth. parameters of inverse grid.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ICAGridArray* inverseGridArray;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// extrapolationPercentage
    ///
    /// Description:   percentage of the Grid points that were extrapolated
    /// Type:          FLOAT
    /// Default Value: 0
    ///
    /// Bit Depth:     float
    /// Conversion:    No Conversion needed
    /// Notes:         Both forward and Reverse grids to defined on the same
    ///                resolution for correct extrapolation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    FLOAT         extrapolationPercentage;
} IPEICAInverseGridOutput;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHIIPEDEFS_H
