// NOWHINE ENTIRE FILE
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef __NC_LIB_WARP_COMMON_DEF__
#define __NC_LIB_WARP_COMMON_DEF__

/*------------------------------------------------------------------------
 / @file  NcLibWarpCommonDef.h
 / @brief Common definitions
 /------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
*       Include Files
* ----------------------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>

#include "ipe_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
*       Type Declarations
* ----------------------------------------------------------------------- */
#pragma pack(push)
#pragma pack(4)

/** A single perspective transform matrix */
typedef struct NcLibPerspTransformSingle_t
{
    float T[9];         /**< 9 parameters define perspective transform */
}NcLibPerspTransformSingle;

/** Perspective transform center location */
typedef enum NcLibPerspTransformCenter_t
{
    CENTERED = 0,   /**< Each matrix is centered with regard to image slice (ICA implementation) */
    STRETCHED = 1   /**< First and last matrices are located on first and last image rows and the rest are spread evenly */
}NcLibPerspTransformSpread;

/** Warp matrices data structure */
typedef struct NcLibWarpMatrices_t
{
    bool                        enable;             /**< If true, enable perspective matrices */
    ImageDimensions             transformDefinedOn; /**< Perspective transform defined on dimension */
    NcLibPerspTransformSpread   centerType;         /**< Type of perspective transform  */

    uint32_t                    confidence;         /**< Perspective transform confidence parameter - Values range [0,256] */

    uint32_t                    numRows;            /**< Number of rows in array */
    uint32_t                    numColumns;         /**< Number of columns in array */
    NcLibPerspTransformSingle*  perspMatrices;      /**< 2-dimensional array of perspective transform matrices */
}NcLibWarpMatrices;

/** A single grid point */
typedef struct NcLibWarpGridCoord_t
{
    float x;        /**< x coordinate in floating point format */
    float y;        /**< y coordinate in floating point format */
}NcLibWarpGridCoord;

/** Extrapolation type of the grid */
typedef enum NcLibWarpGridExtrapolationType_t
{
    EXTRAPOLATION_TYPE_MIN_VAL = 0,                       /**< For SW purpose */

    EXTRAPOLATION_TYPE_NONE = 0,                          /**< no extrapolation points */
    EXTRAPOLATION_TYPE_FOUR_CORNERS = 1,                  /**< Napali like - four corners (4 points must be provided in gridExtrapolate).*/
    EXTRAPOLATION_TYPE_EXTRA_POINT_ALONG_PERIMETER = 2,   /**< Hana/Kona like (must be provided in grid). */


    EXTRAPOLATION_TYPE_MAX_VAL                            /**< For SW purpose - not valid */
}NcLibWarpGridExtrapolationType;

/** Warp grid data structure */
typedef struct NcLibWarpGrid_t
{
    bool                            enable;             /**< If true, enables grid */
    ImageDimensions                 transformDefinedOn; /**< Grid transform defined on dimension */

    uint32_t                        numRows;            /**< Number of rows in array */
    uint32_t                        numColumns;         /**< Number of columns in array */
    NcLibWarpGridCoord*             grid;               /**< 2-dimensional array of grid points */

    NcLibWarpGridExtrapolationType  extrapolateType;    /**< Grid extrapolation type */
    NcLibWarpGridCoord*             gridExtrapolate;    /**< Grid extrapolation points array */
}NcLibWarpGrid;

/** Direction of warp matrices and grid  */
typedef enum NcLibWarpDirection_t
{
    OUT_2_IN = 0,       /**< Output to Input */
}NcLibWarpDirection;

/** Warping data structure. */
typedef struct NcLibWarp_t
{
    NcLibWarpMatrices    matrices;      /**< Perspective matrices transform data structure */

    NcLibWarpGrid        grid;          /**< Grid transform data structure */

    NcLibWarpDirection   direction;     /**< Direction of warp matrices and grid */
}NcLibWarp;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif /* __NC_LIB_WARP_COMMON_DEF__ */
