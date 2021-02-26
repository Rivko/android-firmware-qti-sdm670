////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxzoomtranslator.cpp
/// @brief  CHX Zoom translator class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxzoomtranslator.h"
#include <dlfcn.h>

#undef LOG_TAG
#define LOG_TAG "CHIZOOMTRANSLATOR"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::ZoomTranslator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ZoomTranslator::ZoomTranslator()
    : m_libHandle(NULL)
    , m_adjustedFovRatio(0.0f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::~ZoomTranslator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ZoomTranslator::~ZoomTranslator()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ZoomTranslator::Destroy()
{
    // dlclose the lib here. Do not dlclose in Deinit
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ZoomTranslator* ZoomTranslator::Create()
{
    ZoomTranslator* pZoomTranslator = CHX_NEW ZoomTranslator;

    if (NULL != pZoomTranslator)
    {
        // dlopen and dlsym here. Set m_libHandle.
    }

    return pZoomTranslator;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::Init
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ZoomTranslator::Init(
    const zoom_translator_init_data_t* initData)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == m_libHandle)
    {
        // Default implementation for zoom translation
        m_adjustedFovRatio     = initData->adjustedFovRatio;
        m_sensorDimensionWide  = initData->sensorDimensionWide;
        m_sensorDimensionTele  = initData->sensorDimensionTele;
        m_activeArraySizeWide  = initData->activeArraySizeWide;
        m_activeArraySizeTele  = initData->activeArraySizeTele;

        CHX_LOG("Using the default zoom translation implementation");
    }
    else
    {
        /* Use initData to initialize the ZoomTranslator object.
        If the initialization fails set the result code to CDKResultEFailed */

        CHX_LOG("Using the customized zoom translation implementation");
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::Deinit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ZoomTranslator::Deinit()
{
    CDKResult result = CDKResultSuccess;

    /* If the De-initialization fails set the result code to CDKResultEFailed */

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ZoomTranslator::GetTranslatedZoom
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ZoomTranslator::GetTranslatedZoom(
    const CHIRECT*     userZoom,
    translated_zoom_t* translatedZoom)
{
    CDKResult result = CDKResultSuccess;

    if (NULL == m_libHandle)
    {
        // Default implementation for zoom translation

        FLOAT scaleWidth  = m_activeArraySizeWide.width  / (FLOAT)userZoom->width;
        FLOAT scaleHeight = m_activeArraySizeWide.height / (FLOAT)userZoom->height;
        FLOAT zoom = scaleWidth > scaleHeight ? scaleWidth : scaleHeight;

        translatedZoom->zoomPreview.zoomWideTotal = *userZoom;
        translatedZoom->zoomPreview.zoomWideIsp   = translatedZoom->zoomPreview.zoomWideTotal;

        if (zoom <= m_adjustedFovRatio)
        {
            translatedZoom->zoomPreview.zoomTeleTotal.top    = 0;
            translatedZoom->zoomPreview.zoomTeleTotal.left   = 0;
            translatedZoom->zoomPreview.zoomTeleTotal.width  = m_activeArraySizeTele.width;
            translatedZoom->zoomPreview.zoomTeleTotal.height = m_activeArraySizeTele.height;
        }
        else
        {
            FLOAT scaleWidthTele  = scaleWidth  / m_adjustedFovRatio;
            FLOAT scaleHeightTele = scaleHeight / m_adjustedFovRatio;

            translatedZoom->zoomPreview.zoomTeleTotal.width  = m_activeArraySizeTele.width / scaleWidthTele;
            translatedZoom->zoomPreview.zoomTeleTotal.height = m_activeArraySizeTele.height / scaleHeightTele;
            translatedZoom->zoomPreview.zoomTeleTotal.left   =
                (m_activeArraySizeTele.width - translatedZoom->zoomPreview.zoomTeleTotal.width) / 2.0f;
            translatedZoom->zoomPreview.zoomTeleTotal.top    =
                (m_activeArraySizeTele.height - translatedZoom->zoomPreview.zoomTeleTotal.height) / 2.0f;
        }
        translatedZoom->zoomPreview.zoomTeleIsp = translatedZoom->zoomPreview.zoomTeleTotal;
        translatedZoom->zoomSnapshot = translatedZoom->zoomPreview;
    }
    else
    {
        /* Implement the zoom translation logic.
           Set result to CDKResultEFailed in case of error*/
    }

    return result;
}