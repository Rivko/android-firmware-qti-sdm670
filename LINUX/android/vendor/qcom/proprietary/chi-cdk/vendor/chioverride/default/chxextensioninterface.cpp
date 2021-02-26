////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxextensioninterface.cpp
/// @brief Main landing functions for CHX
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxincs.h"
#include "chxextensionmodule.h"
#include "chivendortag.h"

CDK_VISIBILITY_PUBLIC CHICONTEXTOPS g_chiContextOps;
CDK_VISIBILITY_PUBLIC CHIHANDLE     g_chiHandle;

// Dummy chi override vender tags for testing purpose
static const CHAR   ChiOverrideNodeSectionName[] = "com.qti.chi.override";             ///< The section name for override module
static CHIVENDORTAGDATA g_VendorTagDataChiOverride[] =
{
    { "override_tag1", TYPE_INT32, 1 },
    { "override_tag2", TYPE_BYTE,  1 },
    { "override_tag3", TYPE_INT64, 1 },
    { "override_tag4", TYPE_INT64, 2 }
};

static CHIVENDORTAGDATA g_VendorTagDataMulticameraSensorConfig[] =
{
    { "sensorsyncmodeconfig", TYPE_BYTE, sizeof(SensorSyncModeMetadata) },
};

static CHIVENDORTAGDATA g_VendorTagDataMulticameraInputMetadata[] =
{
    { "InputMetadataOpticalZoom",       TYPE_BYTE,  sizeof(InputMetadataOpticalZoom)    },
    { "InputMetadataBokeh",             TYPE_BYTE,  sizeof(InputMetadataBokeh)          }
};

static CHIVENDORTAGDATA g_VendorTagDataMulticameraOutputMetadata[] =
{
    { "OutputMetadataOpticalZoom",      TYPE_BYTE,  sizeof(OutputMetadataOpticalZoom)   },
    { "OutputMetadataBokeh",            TYPE_BYTE,  sizeof(OutputMetadataBokeh)         }
};

static CHIVENDORTAGDATA g_VendorTagDataMulticameraInfo[] =
{
    { "MultiCameraIdRole",              TYPE_BYTE,  sizeof(MultiCameraIdRole)           },
    { "MasterCamera",                   TYPE_INT32, 1                                   },
    { "LowPowerMode",                   TYPE_BYTE,  sizeof(LowPowerModeInfo)            },
    { "SyncMode",                       TYPE_BYTE,  sizeof(SyncModeInfo)                }
};

static CHIVENDORTAGDATA g_VendorTagCropRegion[] =
{
    { "crop_regions",                   TYPE_BYTE,  sizeof(CaptureRequestCropRegions)   },
    { "ChiNodeResidualCrop",            TYPE_BYTE,  sizeof(CHIRECT)                     }
};

static CHIVENDORTAGDATA g_VendorTagAECLux[] =
{
    { "AecLux",                         TYPE_FLOAT,  1                                  }
};


static CHIVENDORTAGSECTIONDATA g_VendorTagSectionDataChiOverride[] =
{
    {
        ChiOverrideNodeSectionName, 0,
        sizeof(g_VendorTagDataChiOverride) / sizeof(g_VendorTagDataChiOverride[0]),
        g_VendorTagDataChiOverride, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.chi.multicamerasensorconfig", 0,
        sizeof(g_VendorTagDataMulticameraSensorConfig) / sizeof(g_VendorTagDataMulticameraSensorConfig[0]),
        g_VendorTagDataMulticameraSensorConfig, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.chi.multicamerainputmetadata", 0,
        sizeof(g_VendorTagDataMulticameraInputMetadata) / sizeof(g_VendorTagDataMulticameraInputMetadata[0]),
        g_VendorTagDataMulticameraInputMetadata, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.chi.multicameraoutputmetadata", 0,
        sizeof(g_VendorTagDataMulticameraOutputMetadata) / sizeof(g_VendorTagDataMulticameraOutputMetadata[0]),
        g_VendorTagDataMulticameraOutputMetadata, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.chi.multicamerainfo", 0,
        sizeof(g_VendorTagDataMulticameraInfo) / sizeof(g_VendorTagDataMulticameraInfo[0]),
        g_VendorTagDataMulticameraInfo, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.cropregions",  0,
        sizeof(g_VendorTagCropRegion) / sizeof(g_VendorTagCropRegion[0]),
        g_VendorTagCropRegion, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },

    {
        "com.qti.chi.statsaec", 0,
        sizeof(g_VendorTagAECLux) / sizeof(g_VendorTagAECLux[0]),
        g_VendorTagAECLux, CHITAGSECTIONVISIBILITY::ChiTagSectionVisibleToAll
    },
};

static ChiVendorTagInfo g_VendorTagInfoChiOverride[] =
{
    {
        & g_VendorTagSectionDataChiOverride[0],
        sizeof(g_VendorTagSectionDataChiOverride) / sizeof(g_VendorTagSectionDataChiOverride[0])
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allows additional modification during HAL open
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_extend_open(
    uint32_t    cameraId,
    void*       priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->ExtendOpen(cameraId, priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allows additional modification during HAL close
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_extend_close(
    uint32_t    cameraId,
    void*       priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->ExtendClose(cameraId, priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Get the number of cameras
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_get_num_cameras(
    uint32_t* numFwCameras,
    uint32_t* numLogicalCameras)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->GetNumCameras(numFwCameras, numLogicalCameras);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allow remapping of framework ID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t chi_remap_camera_id(
    uint32_t            frameworkCameraId,
    CameraIdRemapMode   mode)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    return pExtensionModule->RemapCameraId(frameworkCameraId, mode);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Get camera information
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult chi_get_camera_info(
    int             camera_id,
    camera_info*    camera_info)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    return pExtensionModule->GetCameraInfo(camera_id, camera_info);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Main entry point
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult chi_initialize_override_session(
    uint32_t                        cameraId,
    const camera3_device_t*         camera3_device,
    const chi_hal_ops_t*            chiHalOps,
    camera3_stream_configuration_t* stream_config,
    bool*                           override_config,
    void**                          priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->InitializeOverrideSession(cameraId, camera3_device, chiHalOps, stream_config, override_config, priv);

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Finalize the override session
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult chi_finalize_override_session(
    const camera3_device_t* camera3_device,
    void*                   priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    return pExtensionModule->FinalizeOverrideSession(camera3_device, priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Destroy the session
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_teardown_override_session(
    const camera3_device_t* camera3_device,
    uint64_t                session,
    void*                   priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    return pExtensionModule->TeardownOverrideSession(camera3_device, session, priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Process request call
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static CDKResult chi_override_process_request(
    const camera3_device_t*     camera3_device,
    camera3_capture_request_t*  capture_request,
    void*                       priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    return pExtensionModule->OverrideProcessRequest(camera3_device, capture_request, priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allows implementation-specific settings to be toggled in the override at runtime
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_modify_settings(
    void*       priv)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->ModifySettings(priv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allows implementation-specific settings to be added to the default request template settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID chi_get_default_request_settings(
    uint32_t                  cameraId,
    int                       requestTemplate,
    const camera_metadata_t** settings)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    pExtensionModule->DefaultRequestSettings(cameraId, requestTemplate, settings);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Allows implementation-specific settings to be added to the default flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void chi_override_flush(
    const camera3_device_t*     camera3_device)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();
    pExtensionModule->OverrideFlush(camera3_device);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief HAL Override entry
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void chi_hal_override_entry(
    chi_hal_callback_ops_t* callbacks)
{
    ExtensionModule* pExtensionModule = ExtensionModule::GetInstance();

    CHX_ASSERT(NULL != callbacks);

    if (NULL != pExtensionModule)
    {
        callbacks->chi_get_num_cameras              = chi_get_num_cameras;
        callbacks->chi_get_camera_info              = chi_get_camera_info;
        callbacks->chi_initialize_override_session  = chi_initialize_override_session;
        callbacks->chi_finalize_override_session    = chi_finalize_override_session;
        callbacks->chi_override_process_request     = chi_override_process_request;
        callbacks->chi_teardown_override_session    = chi_teardown_override_session;
        callbacks->chi_extend_open                  = chi_extend_open;
        callbacks->chi_extend_close                 = chi_extend_close;
        callbacks->chi_remap_camera_id              = chi_remap_camera_id;
        callbacks->chi_modify_settings              = chi_modify_settings;
        callbacks->chi_get_default_request_settings = chi_get_default_request_settings;
        callbacks->chi_override_flush               = chi_override_flush;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief chi_hal_query_vendertag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult chi_hal_query_vendertag(
    CHIQUERYVENDORTAG* pQueryVendorTag)
{
    CDKResult result = CDKResultSuccess;
    if (NULL == pQueryVendorTag)
    {
        result = CDKResultEInvalidPointer;
    }

    if (CDKResultSuccess == result)
    {
        if (pQueryVendorTag->size >= sizeof(CHIQUERYVENDORTAG))
        {
            pQueryVendorTag->pVendorTagInfo = g_VendorTagInfoChiOverride;
        }
        else
        {
            result = CDKResultEFailed;
        }
    }
    return result;
}
