////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxzoomtranslator.h
/// @brief CHX Zoom translator class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXZOOMTRANSLATOR_H
#define CHXZOOMTRANSLATOR_H

//#include "chxdefs.h"
#include "chxmulticamcontroller.h"

/// @brief Initialization data for zoom translator
typedef struct zoom_translator_init_data
{
    VOID*       pRawOTPData;                ///< Pointer to raw OTP data
    UINT32      rawOTPDataSize;             ///< Size of raw OTP data
    dimension_t previewDimension;           ///< Preview dimension
    CHIRECT     fovRectIFEWide;             ///< IFE FOV wrt to active sensor array for wide camera
    CHIRECT     fovRectIFETele;             ///< IFE FOV wrt to active sensor array for tele camera
    dimension_t sensorDimensionWide;        ///< Wide sensor output dimension
    dimension_t sensorDimensionTele;        ///< Tele sensor output dimension
    CHIRECT     activeArraySizeWide;        ///< Wide sensor active array size
    CHIRECT     activeArraySizeTele;        ///< Tele sensor active array size
    FLOAT       adjustedFovRatio;           ///< Adjusted FOV ratio between two cameras
} zoom_translator_init_data_t;

/// @brief Translated zoom struct
typedef struct zoom_regions
{
    CHIRECT     zoomWideTotal;              ///< Total zoom on wide camera
    CHIRECT     zoomTeleTotal;              ///< Total zoom on tele camera
    CHIRECT     zoomWideIsp;                ///< ISP zoom to limit for wide camera
    CHIRECT     zoomTeleIsp;                ///< ISP zoom to limit for tele camera
} zoom_regions_t;

/// @brief Translated zoom data for preview and snapshot
typedef struct translated_zoom
{
    zoom_regions_t zoomPreview;             ///< Translated zoom for preview
    zoom_regions_t zoomSnapshot;            ///< Translated zoom for snapshot
} translated_zoom_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief ZoomTranslator class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ZoomTranslator
{
public:

    /// Create the ZoomTranslator Object
    static ZoomTranslator* Create();

    /// Initialize ZoomTranslator object
    CDKResult Init(
        const zoom_translator_init_data_t* initData);

    /// Deinitialize ZoomTranslator object
    CDKResult Deinit();

    /// Get the translated zoom values
    CDKResult GetTranslatedZoom(
        const CHIRECT*      userZoom,
        translated_zoom_t*  translatedZoom);

    /// Destroy ZoomTranslator object
    VOID Destroy();

protected:

    ZoomTranslator();
    virtual ~ZoomTranslator();

private:

    /// Do not allow the copy constructor or assignment operator
    ZoomTranslator(const ZoomTranslator& rZoomTranslator) = delete;
    ZoomTranslator& operator= (const ZoomTranslator& rZoomTranslator) = delete;

    VOID*       m_libHandle;                        ///< Custom zoom translator library handle
    FLOAT       m_adjustedFovRatio;                 ///< Adjusted FOV ratio between two cameras
    dimension_t m_sensorDimensionWide;              ///< Wide sensor output dimension
    dimension_t m_sensorDimensionTele;              ///< Tele sensor output dimension
    CHIRECT     m_activeArraySizeWide;              ///< Wide sensor active array size
    CHIRECT     m_activeArraySizeTele;              ///< Tele sensor active array size
};

#endif // CHXZOOMTRANSLATOR_H
