// NOWHINE ENTIRE FILE
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#pragma once

#include "ImageTransform.h"

struct LrmeInStruct_v10;


typedef     short int      ChannelType;

struct SLrmeMv
{

    float xOld;
    float yOld;
    float xNew;
    float yNew;
    int robustnessMeasure;
    bool robustnessIndication;
};


/**
*  @brief   Calculates LRME transform and transform confidence.
*
*  @param [in]  meResultPtr          LRME results
*  @param [in]  fullW                Full image width (before downscale)
*  @param [in]  fullH                Full image height (before downscale)
*  @param [in]  tarW                 Full image width after downscale and crop (may include padding)
*  @param [in]  tarH                 Full image height after downscale and crop (may include padding)
*  @param [in]  tarOffsetX           Crop on target on X-axis
*  @param [in]  tarOffsetY           Crop on target on Y-axis
*  @param [in]  refOffsetX           Crop on reference on X-axis
*  @param [in]  refOffsetY           Crop on reference on Y-axis
*  @param [in]  stepX                LRME result step on X-axis, On Napali should be fixed value - 12
*  @param [in]  stepY                LRME result step on Y-axis, On Napali should be fixed value - 8
*  @param [in]  resultsFormat        LRME result format, if equal to 1 then result format is long, otherwise short
*  @param [in]  subPelSearchEnable   LRME subPelSearchEnable parameter
*  @param [in]  upscaleFactor        Transform upscale factor
*  @param [in]  method               findHomography() method (valid values: 0, 1, 2, 3)
*               0                    Perspective transform
*               1                    Rigid transform (in Chromatix the value for Rigid is 2)
*               2                    Affine transform (in Chromatix the value for Affine is 1)
*               3                    Rigid transform with post processing
*  @param [out]  transform           LRME transform
*  @param [out]  transformConfidence LRME transform confidence
*
*  @return NC_LIB_SUCCESS in case of success, otherwise failed.
*/
int32_t ProcessMeResult(ChannelType *meResultPtr, int fullW, int fullH ,int tarW, int tarH,
    int tarOffsetX, int tarOffsetY, int refOffsetX, int refOffsetY,
    int stepX, int stepY, int resultsFormat, int subPelSearchEnable,
    int upscaleFactor, int method, CPerspectiveTransform& transform, int& transformConfidence);

