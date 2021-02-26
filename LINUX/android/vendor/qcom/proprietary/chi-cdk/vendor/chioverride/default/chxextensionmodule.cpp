////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxextensionmodule.cpp
/// @brief Extension Module implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#if defined (_WIN32)    // This file for Win32 build only
#include <malloc.h>
#else
#include <stdlib.h>
#include <cutils/log.h>
#endif // WIN32

#include "chi.h"
#include "chioverride.h"
#include "camxcdktypes.h"
#include "chxusecaseutils.h"

#include "chxextensionmodule.h"
#include "chxsensorselectmode.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxmulticamcontroller.h"

extern CHICONTEXTOPS g_chiContextOps;

/// @brief Primarily an example for how to use the VOID* in ExtendOpen and ModifySettings.  Must match data structure
///        being passed into the function
struct ChiOverrideToken
{
    UINT32    id;
    UINT32    size;
};

/// @brief Primarily an example for how to use the VOID* in ExtendOpen and ModifySettings  Must match data structure
///        being passed into the function
struct ChiOverrideExtendOpen
{
    UINT32              numTokens;
    ChiOverrideToken*   pTokens;
};

/// @brief Primarily an example for how to use the VOID* in ExtendClose and ModifySettings  Must match data structure
///        being passed into the function
struct ChiOverrideExtendClose
{
    UINT32              numTokens;
    ChiOverrideToken*   pTokens;
};

/// @brief Primarily an example for how to use the VOID* in ExtendOpen and ModifySettings  Must match data structure
///        being passed into the function
struct ChiOverrideModifySetting
{
    ChiOverrideToken    token;
    VOID*               pData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetInstance
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExtensionModule* ExtensionModule::GetInstance()
{
    static ExtensionModule s_extensionModuleSingleton;

    return &s_extensionModuleSingleton;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ExtensionModule
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExtensionModule::ExtensionModule()
    : m_hCHIContext(NULL)
    , m_numPhysicalCameras(0)
    , m_numLogicalCameras(0)
    , m_previousPowerHint(PERF_LOCK_COUNT)
    , m_numMetadataResults(0)
{

    memset(m_logicalCameraInfo, 0, sizeof(m_logicalCameraInfo));
    m_pUsecaseSelector = UsecaseSelector::Create(this);
    m_pUsecaseFactory  = UsecaseFactory::Create(this);

    m_longExposureFrame = static_cast<UINT32>(InvalidFrameNumber);
    m_isFlushLocked     = FALSE;

#if defined(_LP64)
    const CHAR* pChiDriver  = "/vendor/lib64/hw/camera.qcom.so";
#else // _LP64
    const CHAR* pChiDriver  = "/vendor/lib/hw/camera.qcom.so";
#endif // _LP64
    const CHAR* pPerfModule = "libqti-perfd-client.so";

    m_perfLibHandle = ChxUtils::LibMap(pPerfModule);

    if (NULL == m_perfLibHandle)
    {
        CHX_LOG_ERROR("Failed to load perf lib");
    }

    ChxUtils::Memset(&m_pPerfLockManager[0], 0, sizeof(m_pPerfLockManager));
    for (UINT32 cameraId = 0; cameraId < MaxNumImageSensors; cameraId++)
    {
        m_pPCRLock[cameraId] = NULL;
    }
    m_pConfigSettings = static_cast<UINT32*>(CHX_CALLOC(sizeof(UINT32) * MaxConfigSettings));

    OSLIBRARYHANDLE handle  = ChxUtils::LibMap(pChiDriver);

    if (NULL != handle)
    {
        CHX_LOG("CHI Opened driver library");

        PCHIENTRY funcPChiEntry = reinterpret_cast<PCHIENTRY>(ChxUtils::LibGetAddr(handle, "ChiEntry"));

        if (NULL != funcPChiEntry)
        {
            CHX_LOG("CHI obtained ChiEntry point function");

            funcPChiEntry(&g_chiContextOps);

            m_hCHIContext = g_chiContextOps.pOpenContext();

            CHX_LOG("CHI context functions - CreateSession  %p", g_chiContextOps.pCreateSession);

            if (NULL != m_hCHIContext)
            {
                ChiVendorTagsOps vendorTagOps = { 0 };
                g_chiContextOps.pTagOps(&vendorTagOps);

                // Since this is a one time operation do it at boot up time
                if (NULL != vendorTagOps.pQueryVendorTagLocation)
                {
                    m_pvtVendorTags[SensorBpsModeIndex].pSectionName = "com.qti.sensorbps";
                    m_pvtVendorTags[SensorBpsModeIndex].pTagName     = "mode_index";
                    m_pvtVendorTags[SensorBpsGain].pSectionName      = "com.qti.sensorbps";
                    m_pvtVendorTags[SensorBpsGain].pTagName          = "gain";
                    m_pvtVendorTags[DebugDataTag].pSectionName       = "org.quic.camera.debugdata";
                    m_pvtVendorTags[DebugDataTag].pTagName           = "DebugDataAll";
                    m_pvtVendorTags[SensorModeIndex].pSectionName    = "org.codeaurora.qcamera3.sensor_meta_data";
                    m_pvtVendorTags[SensorModeIndex].pTagName        = "sensor_mode_index";
                    m_pvtVendorTags[CropRegions].pSectionName        = "com.qti.cropregions";
                    m_pvtVendorTags[CropRegions].pTagName            = "crop_regions";
                    m_pvtVendorTags[TuningMode].pSectionName         = "org.quic.camera2.tuning.mode";
                    m_pvtVendorTags[TuningMode].pTagName             = "TuningMode";
                    m_pvtVendorTags[RefCropSize].pSectionName        = "org.quic.camera2.ref.cropsize";
                    m_pvtVendorTags[RefCropSize].pTagName            = "RefCropSize";
                    m_pvtVendorTags[MultiCamera].pSectionName        = "com.qti.chi.multicamerainfo";
                    m_pvtVendorTags[MultiCamera].pTagName            = "MultiCameraIdRole";
                    m_pvtVendorTags[IsFlashRequiredTag].pSectionName = "com.qti.stats_control";
                    m_pvtVendorTags[IsFlashRequiredTag].pTagName     = "is_flash_snapshot";
                    m_pvtVendorTags[Feature1Mode].pSectionName       = "org.quic.camera2.tuning.feature";
                    m_pvtVendorTags[Feature1Mode].pTagName           = "Feature1Mode";
                    m_pvtVendorTags[Feature2Mode].pSectionName       = "org.quic.camera2.tuning.feature";
                    m_pvtVendorTags[Feature2Mode].pTagName           = "Feature2Mode";
                    m_pvtVendorTags[VideoHDR10Mode].pSectionName     = "org.quic.camera2.streamconfigs";
                    m_pvtVendorTags[VideoHDR10Mode].pTagName         = "HDRVideoMode";

                    //Init parameters
                    for (UINT32 cameraId = 0; cameraId < MaxNumImageSensors; cameraId++)
                    {
                        m_pPCRLock[cameraId] = Mutex::Create();
                    }

                    for (UINT32 id = 0; id < MaxNumImageSensors; id++)
                    {
                        m_pStreamConfig[id] = NULL;
                    }
                    // Go thru each tag and get the tag id from the driver
                    for (UINT index = 0; index < NumVendorTags; index++)
                    {
                        CDKResult result = vendorTagOps.pQueryVendorTagLocation(m_pvtVendorTags[index].pSectionName,
                                                                                m_pvtVendorTags[index].pTagName,
                                                                                &m_pvtVendorTags[index].tagId);
                        if (CDKResultSuccess == result)
                        {
                            CHX_LOG("Vendor Tag %d: Section %s, TagName %s, TagId: 0x%x",
                                index,
                                m_pvtVendorTags[index].pSectionName,
                                m_pvtVendorTags[index].pTagName,
                                m_pvtVendorTags[index].tagId);
                        }
                        else
                        {
                            CHX_LOG("Failed to find TagId: %d, Section %s, TagName %s",
                                index,
                                m_pvtVendorTags[index].pSectionName,
                                m_pvtVendorTags[index].pTagName);
                        }
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::~ExtensionModule
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExtensionModule::~ExtensionModule()
{
    m_pUsecaseSelector->Destroy();
    m_pUsecaseSelector = NULL;

    MultiCamControllerManager::GetInstance()->Destroy();

    for (UINT i = 0; i < MaxNumImageSensors; i++)
    {
        if (NULL != m_pPerfLockManager[i])
        {
            m_pPerfLockManager[i]->Destroy();
            m_pPerfLockManager[i] = NULL;
        }
    }

    g_chiContextOps.pCloseContext(m_hCHIContext);
    m_hCHIContext = NULL;

    for (UINT i = 0; i < m_numLogicalCameras; i++)
    {
        // if physical camera numbers is bigger than 1, it should be multi camera.
        // and the static meta is allocated in chiextension module. Free it when
        // exit
        if (1 < m_logicalCameraInfo[i].numPhysicalCameras)
        {
            if (NULL != m_logicalCameraInfo[i].m_cameraInfo.static_camera_characteristics)
            {
                free_camera_metadata(
                    const_cast<camera_metadata_t*>(m_logicalCameraInfo[i].m_cameraInfo.static_camera_characteristics));
                m_logicalCameraInfo[i].m_cameraInfo.static_camera_characteristics = NULL;
            }
        }
        if (NULL != m_logicalCameraInfo[i].pSensorModeInfo)
        {
            CHX_FREE(m_logicalCameraInfo[i].pSensorModeInfo);
            m_logicalCameraInfo[i].pSensorModeInfo = NULL;
        }

        for (UINT j = 0; j < m_logicalCameraInfo[i].numPhysicalCameras; j++)
        {
            if (m_logicalCameraInfo[i].ppDeviceInfo[j])
            {
                CHX_FREE(m_logicalCameraInfo[i].ppDeviceInfo[j]);
                m_logicalCameraInfo[i].ppDeviceInfo[j] = NULL;
            }
        }

        if (NULL != m_logicalCameraInfo[i].ppDeviceInfo)
        {
            CHX_FREE(m_logicalCameraInfo[i].ppDeviceInfo);
            m_logicalCameraInfo[i].ppDeviceInfo = NULL;
        }
    }

    if (NULL != m_pConfigSettings)
    {
        CHX_FREE(m_pConfigSettings);
        m_pConfigSettings = NULL;
    }

    for (UINT32 id = 0; id < MaxNumImageSensors; id++)
    {
        if (NULL != m_pStreamConfig[id])
        {
            if (NULL != m_pStreamConfig[id]->streams)
            {
                CHX_FREE(m_pStreamConfig[id]->streams);
                m_pStreamConfig[id]->streams = NULL;
            }
            CHX_FREE(m_pStreamConfig[id]);
            m_pStreamConfig[id] = NULL;
        }
    }

    for (UINT32 cameraId = 0; cameraId < MaxNumImageSensors; cameraId++)
    {
        if (NULL != m_pPCRLock[cameraId])
        {
            m_pPCRLock[cameraId]->Destroy();
            m_pPCRLock[cameraId] = NULL;
        }
    }
    if (m_pDefaultSettings != NULL)
    {
#if defined (_LINUX)
        free_camera_metadata(m_pDefaultSettings);
#endif
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::SetHALOps
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::SetHALOps(
    const chi_hal_ops_t* pHalOps, UINT32 logicalCameraId)
{
    m_pHALOps[logicalCameraId] = pHalOps;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::SetCHIContextOps
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::SetCHIContextOps()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::RemapCameraId
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ExtensionModule::RemapCameraId(
    UINT32              frameworkCameraId,
    CameraIdRemapMode   mode)
{
    (VOID)mode;

    if (frameworkCameraId < m_numLogicalCameras)
    {
        switch (mode)
        {
            case IdRemapCamera:
                frameworkCameraId = m_cameraMap[frameworkCameraId];
                break;

            case IdRemapTorch:
                frameworkCameraId = m_logicalCameraInfo[m_cameraMap[frameworkCameraId]].ppDeviceInfo[0]->cameraId;
                break;
        }
    }

    return frameworkCameraId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ExtendOpen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ExtendOpen(
    uint32_t  logicalCameraId,
    VOID*     pPriv)
{
    // The ExtensionModule has been initialized, and if there is any work needed to be done during HAL open, this is where
    // to add that code

    // sample code...this uses the input to create a data structure used to hold configuration settings
    ChiOverrideExtendOpen* pExtend = static_cast<ChiOverrideExtendOpen*>(pPriv);
    if (NULL == m_pConfigSettings || (pExtend->numTokens > MaxConfigSettings))
    {
        CHX_LOG_ERROR("No memory allocated for m_pConfigSettings!");
        return;
    }

    if (NULL == m_pPerfLockManager[logicalCameraId])
    {
        m_pPerfLockManager[logicalCameraId] = PerfLockManager::Create();

        if (NULL == m_pPerfLockManager[logicalCameraId])
        {
            CHX_LOG_ERROR("Failed to create perflock manager %d", logicalCameraId);
        }
    }

    if (NULL != m_pPerfLockManager[logicalCameraId])
    {
        m_pPerfLockManager[logicalCameraId]->AcquirePerfLock(PERF_LOCK_OPEN_CAMERA, 1000);
    }

    for (UINT i = 0; i < pExtend->numTokens; i++)
    {
        switch (pExtend->pTokens[i].id)
        {
            case 0:
                m_pForceUsecaseId             = &m_pConfigSettings[i];
                break;
            case 1:
                m_pDisableZSL                 = &m_pConfigSettings[i];
                break;
            case 2:
                m_pGPUNodeRotateUsecase       = &m_pConfigSettings[i];
                break;
            case 3:
                m_pEnableMFNRUsecase          = &m_pConfigSettings[i];
                break;
            case 4:
                m_pAnchorSelectionAlgoForMFNR = &m_pConfigSettings[i];
                break;
            case 5:
                m_pHFRNo3AUsecase             = &m_pConfigSettings[i];
                break;
            case 6:
                m_pForceSensorMode            = &m_pConfigSettings[i];
                break;
            case 7:
                m_pDefaultMaxFPS              = &m_pConfigSettings[i];
                break;
            case 8:
                m_pEnableFOVC                 = &m_pConfigSettings[i];
                break;
            case 9:
                m_pOverrideCameraClose        = &m_pConfigSettings[i];
                // Overriding the camera close indication in Extendopen
                *m_pOverrideCameraClose       = 0;
                break;
            case 10:
                m_pOverrideCameraOpen         = &m_pConfigSettings[i];
                // Overriding the camera open indication in Extendopen
                *m_pOverrideCameraOpen        = 1;
                break;
            case 11:
                m_pEISV2Enable                = &m_pConfigSettings[i];
                break;
            case 12:
                m_pEISV3Enable                = &m_pConfigSettings[i];
                break;
            case 13:
                m_pNumPCRsBeforeStreamOn      = &m_pConfigSettings[i];
                break;
            case 14:
                m_pStatsSkipPattern           = &m_pConfigSettings[i];
                break;
            case 15:
                m_pEnableMultiCameraSAT       = &m_pConfigSettings[i];
                break;
            case 16:
                m_pEnableDumpDebugData        = &m_pConfigSettings[i];
                break;
            case 17:
                m_pEnableMultiVRCamera        = &m_pConfigSettings[i];
                break;
            case 18:
                m_pGPUNodeDownscaleUsecase    = &m_pConfigSettings[i];
                break;
            case 19:
                m_pAdvanceFeataureMask        = &m_pConfigSettings[i];
                break;
            case 20:
                m_pDisableASDProcessing       = &m_pConfigSettings[i];
                break;
            case 21:
                m_pEnableMultiCameraFrameSync = &m_pConfigSettings[i];
                break;
            case 22:
                m_pOutputFormat               =  &m_pConfigSettings[i];
                break;
            case 24:
                m_pIdealRawEnable             = &m_pConfigSettings[i];
                break;
            case 25:
                m_pLDCEnable                  = &m_pConfigSettings[i];
                break;
            case 26:
                m_pEISDGEnable                = &m_pConfigSettings[i];
                break;
            case 27:
                m_pSelectIHDRUsecase          = &m_pConfigSettings[i];
                break;
            case 28:
                m_pSHDRModeType               = &m_pConfigSettings[i];
                break;
            default:
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ExtendOpen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ExtendClose(
    uint32_t  logicalCameraId,
    VOID*     pPriv)
{
    // The ExtensionModule has been initialized, and if there is any work needed to be done during HAL close, this is where
    // to add that code

    if (NULL != m_pPerfLockManager[logicalCameraId])
    {
        m_pPerfLockManager[logicalCameraId]->AcquirePerfLock(PERF_LOCK_CLOSE_CAMERA, 1000);
    }

    camera3_stream_configuration_t* pStreamConfig = m_pStreamConfig[logicalCameraId];
    if (NULL != pStreamConfig)
    {
        if (NULL != pStreamConfig->streams)
        {
            CHX_FREE(pStreamConfig->streams);
            pStreamConfig->streams = NULL;
        }
        CHX_FREE(pStreamConfig);
        pStreamConfig = NULL;
        m_pStreamConfig[logicalCameraId] = NULL;
    }

    // sample code...this uses the input to create a data structure used to hold configuration settings
    ChiOverrideExtendClose* pExtend = static_cast<ChiOverrideExtendClose*>(pPriv);
    if (NULL == m_pConfigSettings || (pExtend->numTokens > MaxConfigSettings))
    {
        CHX_LOG_ERROR("No memory allocated for m_pConfigSettings!");
        return;
    }

    for (UINT i = 0; i < pExtend->numTokens; i++)
    {
        switch (pExtend->pTokens[i].id)
        {
        case 0:
            m_pForceUsecaseId             = &m_pConfigSettings[i];
            break;
        case 1:
            m_pDisableZSL                 = &m_pConfigSettings[i];
            break;
        case 2:
            m_pGPUNodeRotateUsecase       = &m_pConfigSettings[i];
            break;
        case 3:
            m_pEnableMFNRUsecase          = &m_pConfigSettings[i];
            break;
        case 4:
            m_pAnchorSelectionAlgoForMFNR = &m_pConfigSettings[i];
            break;
        case 5:
            m_pHFRNo3AUsecase             = &m_pConfigSettings[i];
            break;
        case 6:
            m_pForceSensorMode            = &m_pConfigSettings[i];
            break;
        case 7:
            m_pDefaultMaxFPS              = &m_pConfigSettings[i];
            break;
        case 8:
            m_pEnableFOVC                 = &m_pConfigSettings[i];
            break;
        case 9:
            m_pOverrideCameraClose        = &m_pConfigSettings[i];
            // Overriding the camera close indication in ExtendClose
            *m_pOverrideCameraClose       = 1;
            break;
        case 10:
            m_pOverrideCameraOpen         = &m_pConfigSettings[i];
            // Overriding the camera open indication in ExtendClose
            *m_pOverrideCameraOpen        = 0;
            break;
        case 18:
            m_pGPUNodeDownscaleUsecase    = &m_pConfigSettings[i];
            break;
        case 24:
            m_pSelectIHDRUsecase          = &m_pConfigSettings[i];
            break;
        default:
            break;
        }
    }

    FreeLastKnownRequestSetting();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::SearchNumBatchedFrames
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::SearchNumBatchedFrames(
    uint32_t                        cameraId,
    camera3_stream_configuration_t* pStreamConfigs,
    UINT*                           pBatchSize,
    UINT*                           pFPSValue)
{
    INT32 width  = 0;
    INT32 height = 0;

    // We will take the following steps -
    //  1) We will search SupportedHFRVideoSizes, for the matching Video/Preview stream.
    //     Note: For the use case of multiple output streams, application must select one unique size from this metadata
    //           to use (e.g., preview and recording streams must have the same size). Otherwise, the high speed capture
    //           session creation will fail
    //  2) If a single entry is found in SupportedHFRVideoSizes, we choose the batchsize from that entry

    for (UINT streamIndex = 0; streamIndex < pStreamConfigs->num_streams; streamIndex++)
    {
        if (CAMERA3_STREAM_OUTPUT == pStreamConfigs->streams[streamIndex]->stream_type)
        {
            width  = pStreamConfigs->streams[streamIndex]->width;
            height = pStreamConfigs->streams[streamIndex]->height;
            break;
        }
    }

    SIZE_T numDefaultHFRVideoSizes    = 0;
    /// @todo Implement metadata merging
    camera_metadata_entry_t entry   = { 0 };

    entry.tag = ANDROID_CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS;

#if defined (_LINUX)
    find_camera_metadata_entry((camera_metadata_t*)(
        m_logicalCameraInfo[cameraId].m_cameraInfo.static_camera_characteristics), entry.tag, &entry);
#endif // _LINUX

    numDefaultHFRVideoSizes = entry.count;

    const HFRConfigurationParams* pHFRParams[MaxHFRConfigs] = { NULL };
    HFRConfigurationParams*       pDefaultHFRVideoSizes     = reinterpret_cast<HFRConfigurationParams*>(entry.data.i32);
    UINT                          numHFREntries             = 0;

    if ((0 != width) && (0 != height))
    {
        for (UINT i = 0; i < numDefaultHFRVideoSizes; i++)
        {
            if ((pDefaultHFRVideoSizes[i].width == width) && (pDefaultHFRVideoSizes[i].height == height))
            {
                // Out of the pair of entries in the table, we would like to store the second entry
                pHFRParams[numHFREntries++] = &pDefaultHFRVideoSizes[i + 1];
                // Make sure that we don't hit the other entry in the pair, again
                i++;
            }
        }

        if (numHFREntries > 0)
        {
            *pBatchSize = pHFRParams[0]->batchSizeMax;
            *pFPSValue  = pHFRParams[0]->maxFPS;
        }
        else
        {
            CHX_LOG_ERROR("Failed to find supported HFR entry");
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetNumCameras
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::GetNumCameras(
    UINT32* pNumFwCameras,
    UINT32* pNumLogicalCameras)
{

    if (0 == m_numLogicalCameras)
    {
        EnumerateCameras();
    }

    /// Sort camera to be published in required order
    SortCameras();

    *pNumLogicalCameras = m_numLogicalCameras;
    *pNumFwCameras = m_numLogicalCameras;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::SortCameras
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::SortCameras()
{
    UINT order1 = 0, order2 = 0, order3 = 0;
    UINT offset1 = 0, offset2 = 0, offset3 = 0;

    // Change here to modify order in which we publish we camera to application.

    ///< Sorting like below.
    /// 1) All main and rear aux camera (Rear, front and rear_aux)
    /// 2) All logical camera (Dual camera)
    /// 3) Secure cameras (Fromt aux camera)
    for (UINT i = 0; i < m_numLogicalCameras; i++)
    {
        LogicalCameraInfo *logicalcam = &m_logicalCameraInfo[i];
        if (logicalcam->numPhysicalCameras > 1)
        {
            order2++;
        }
        else if (logicalcam->m_cameraCaps.sensorCaps.positionType == FRONT_AUX)
        {
            order3++;
        }
        else
        {
            order1++;
        }
    }

    offset1 = 0;
    offset2 = order1;
    offset3 = order1 + order2;

    // physical cameras, logical cameras and secure cameras are sorted
    // Now add all of these cameras at proper index/offset
    for (UINT i = 0; i < m_numLogicalCameras; i++)
    {
        LogicalCameraInfo *logicalcam = &m_logicalCameraInfo[i];
        if (logicalcam->numPhysicalCameras > 1)
        {
            m_cameraMap[offset2++] = i;
        }
        else if (logicalcam->m_cameraCaps.sensorCaps.positionType == FRONT_AUX)
        {
            m_cameraMap[offset3++] = i;
        }
        else
        {
            m_cameraMap[offset1++] = i;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::EnumerateCameras
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::EnumerateCameras()
{
    CDKResult      result        = CDKResultEFailed;
    UINT32         cameracount   = 0;
    CHICAMERAINFO* caminfo       = NULL;

    /// Temporary local variables.
    UINT mainCameras[MaxNumImageSensors];
    UINT auxCameras [MaxNumImageSensors];
    UINT nummaincamera   = 0;
    UINT numauxcamera    = 0;

    /// Get probed sensor count
    cameracount         = g_chiContextOps.pGetNumCameras(m_hCHIContext);

    for (UINT i = 0; i < cameracount; i++)
    {
        LogicalCameraInfo *logicalcam = &m_logicalCameraInfo[m_numLogicalCameras];

        caminfo             = &logicalcam->m_cameraCaps;
        caminfo->pLegacy    = &logicalcam->m_cameraInfo;

        // Get Camera info and capability for probed device.
        result = g_chiContextOps.pGetCameraInfo(m_hCHIContext, i, caminfo);

        if (CDKResultSuccess == result)
        {
            camera_metadata_ro_entry_t entry = { 0 };
            const camera_metadata_t* pMetadata = (static_cast<camera_info_t*>(caminfo->pLegacy))->static_camera_characteristics;
            if (0 == find_camera_metadata_ro_entry(pMetadata, ANDROID_REQUEST_PARTIAL_RESULT_COUNT, &entry))
            {
                UINT32 value = static_cast<UINT32>(*(entry.data.i32));
                m_numMetadataResults = (m_numMetadataResults > value) ? m_numMetadataResults : value;
            }

            logicalcam->pSensorModeInfo =
                    static_cast<CHISENSORMODEINFO*>
                    (CHX_CALLOC(sizeof(CHISENSORMODEINFO) * logicalcam->m_cameraCaps.numSensorModes));

            if (NULL != logicalcam->pSensorModeInfo)
            {
                 // get sensor mode info
                 result = EnumerateSensorModes(i, logicalcam->m_cameraCaps.numSensorModes,
                                               logicalcam->pSensorModeInfo);

                if (CDKResultSuccess == result)
                {
                    UINT        deviceCount   = 0;

                    logicalcam->ppDeviceInfo =
                            static_cast<DeviceInfo**>(CHX_CALLOC(sizeof(DeviceInfo *)));

                    if (NULL != logicalcam->ppDeviceInfo)
                    {
                        DeviceInfo *deviceinfo = static_cast<DeviceInfo*>(CHX_CALLOC(sizeof(DeviceInfo)));

                        if (NULL != deviceinfo)
                        {
                            logicalcam->cameraId        = m_numLogicalCameras;
                            deviceinfo->cameraId        = i;
                            deviceinfo->m_pDeviceCaps   = caminfo;
                            deviceinfo->pSensorModeInfo = logicalcam->pSensorModeInfo;
                            logicalcam->numPhysicalCameras++;
                            logicalcam->ppDeviceInfo[deviceCount] = deviceinfo;
                            /// Retrieve multi camera info
                            if ((REAR_AUX == caminfo->sensorCaps.positionType) ||
                                (FRONT_AUX == caminfo->sensorCaps.positionType))
                            {
                                auxCameras[numauxcamera]   = m_numLogicalCameras;
                                numauxcamera++;
                            }
                            else if ((REAR == caminfo->sensorCaps.positionType) ||
                                (FRONT == caminfo->sensorCaps.positionType))
                            {
                                mainCameras[nummaincamera] = m_numLogicalCameras;
                                nummaincamera++;
                            }
                            m_numLogicalCameras++;
                            CHX_LOG("cameraId %d, positionType %d numauxcamera %d, nummaincamera %d",
                                deviceinfo->cameraId, caminfo->sensorCaps.positionType,
                                numauxcamera, nummaincamera);
                        }
                    }
                }
                else
                {
                    CHX_LOG_ERROR("Failed to enumerate sensor modes");
                    CHX_FREE(logicalcam->pSensorModeInfo);
                }
            }
            else
            {
                CHX_LOG_ERROR("logicalcam->pSensorModeInfo is NULL");
            }
        }
    }

    m_numPhysicalCameras = m_numLogicalCameras;

    for (UINT i = 0; i < nummaincamera; i++)
    {
        UINT32             mainCameraId  = mainCameras[i];
        UINT               deviceCount   = 0;
        LogicalCameraInfo* logicalcam    = &m_logicalCameraInfo[mainCameraId];
        DeviceInfo**       deviceinfo    = NULL;
        BOOL               matchFound    = FALSE;

        if (0 != numauxcamera)
        {
            deviceinfo = static_cast<DeviceInfo**>(CHX_CALLOC(sizeof(DeviceInfo *) * DualCamCount));
        }

        if (NULL != deviceinfo)
        {
            m_logicalCameraInfo[m_numLogicalCameras].pSensorModeInfo =
                    static_cast<CHISENSORMODEINFO*>
                    (CHX_CALLOC(sizeof(CHISENSORMODEINFO) * logicalcam->m_cameraCaps.numSensorModes));

            deviceinfo[deviceCount] = static_cast<DeviceInfo*>(CHX_CALLOC(sizeof(DeviceInfo)));

            deviceinfo[deviceCount]->cameraId        = logicalcam->ppDeviceInfo[0]->cameraId;
            deviceinfo[deviceCount]->m_pDeviceCaps   = logicalcam->ppDeviceInfo[0]->m_pDeviceCaps;
            deviceinfo[deviceCount]->pSensorModeInfo = logicalcam->ppDeviceInfo[0]->pSensorModeInfo;
            deviceCount++;

            CHICAMERAINFO consolidatedChiCamInfo = { 0 };
            camera_info_t consolidatedCamInfo = { 0 };

            for (UINT j = 0; j < numauxcamera; j++)
            {
                deviceCount = 1;
                UINT32   auxCameraId = auxCameras[j];

                logicalcam = &m_logicalCameraInfo[auxCameraId];

                deviceinfo[deviceCount] = static_cast<DeviceInfo*>(CHX_CALLOC(sizeof(DeviceInfo)));
                deviceinfo[deviceCount]->cameraId       = logicalcam->ppDeviceInfo[0]->cameraId;
                deviceinfo[deviceCount]->m_pDeviceCaps   = logicalcam->ppDeviceInfo[0]->m_pDeviceCaps;
                deviceinfo[deviceCount]->pSensorModeInfo = logicalcam->ppDeviceInfo[0]->pSensorModeInfo;

                CHICAMERAINFO** ppChiCameraInfo = static_cast<CHICAMERAINFO**>(CHX_CALLOC(DualCamCount * sizeof(CHICAMERAINFO*)));
                CHX_ASSERT(NULL != ppChiCameraInfo);

                LogicalCameraInfo* logicalCamMain = &m_logicalCameraInfo[mainCameraId];
                LogicalCameraInfo* logicalCamAux  = &m_logicalCameraInfo[auxCameraId];

                ppChiCameraInfo[0] = const_cast<CHICAMERAINFO*>(logicalCamMain->ppDeviceInfo[0]->m_pDeviceCaps);
                ppChiCameraInfo[1] = const_cast<CHICAMERAINFO*>(logicalCamAux->ppDeviceInfo[0]->m_pDeviceCaps);

                consolidatedChiCamInfo = { 0 };
                consolidatedCamInfo = { 0 };
                consolidatedChiCamInfo.pLegacy = &consolidatedCamInfo;

                camera_info_t* pCameraInfoMain = static_cast<camera_info_t*>(ppChiCameraInfo[0]->pLegacy);
                camera_info_t* pCameraInfoAux = static_cast<camera_info_t*>(ppChiCameraInfo[1]->pLegacy);

                ChxUtils::Memcpy(&consolidatedCamInfo, pCameraInfoAux, sizeof(camera_info_t));

                consolidatedCamInfo.static_camera_characteristics =
                    allocate_copy_camera_metadata_checked(pCameraInfoMain->static_camera_characteristics,
                        get_camera_metadata_size(pCameraInfoMain->static_camera_characteristics));

                result = MultiCamController::ConsolidateCameraInfo(DualCamCount, ppChiCameraInfo, &consolidatedChiCamInfo);

                CHX_FREE(ppChiCameraInfo);

                if (CDKResultSuccess == result)
                {
                    matchFound = TRUE;
                    deviceCount++;
                    // Break the auxilary camera loop if match is found
                    break;
                }
                else
                {
                    if (NULL != consolidatedCamInfo.static_camera_characteristics)
                    {
                        free_camera_metadata(const_cast<camera_metadata_t*>(consolidatedCamInfo.static_camera_characteristics));
                        consolidatedCamInfo.static_camera_characteristics = NULL;
                    }
                }
            }

            // if find aux and main camera match, use consolidate camera information for
            // multi camera logical camera info.
            if (TRUE == matchFound)
            {
                m_logicalCameraInfo[m_numLogicalCameras].pSensorModeInfo  = logicalcam->pSensorModeInfo;
                m_logicalCameraInfo[m_numLogicalCameras].m_cameraInfo     = consolidatedCamInfo;
                m_logicalCameraInfo[m_numLogicalCameras].m_cameraCaps     = logicalcam->m_cameraCaps;

                m_logicalCameraInfo[m_numLogicalCameras].numPhysicalCameras = deviceCount;
                m_logicalCameraInfo[m_numLogicalCameras].ppDeviceInfo       = deviceinfo;
                m_logicalCameraInfo[m_numLogicalCameras].cameraId           = m_numLogicalCameras;

                m_numLogicalCameras++;
            }
            else
            {
                CHX_FREE(m_logicalCameraInfo[m_numLogicalCameras].pSensorModeInfo);
                m_logicalCameraInfo[m_numLogicalCameras].pSensorModeInfo = NULL;
                CHX_FREE(deviceinfo);
                deviceinfo = NULL;
            }

        }
    }

    CHX_LOG("Camera Count: m_numLogicalCameras = %d Physical camera = %d",m_numLogicalCameras, m_numPhysicalCameras);
    for (UINT i = 0; i < m_numLogicalCameras; i++)
    {
        CHX_LOG("App ID = %d Num Devices = %d", i, m_logicalCameraInfo[i].numPhysicalCameras);
        for (UINT j = 0; j < m_logicalCameraInfo[i].numPhysicalCameras; j++)
        {
            CHX_LOG("CameraID Published ID = %d DeviceID = %d", i, m_logicalCameraInfo[i].ppDeviceInfo[j]->cameraId);
        }
    }

    // here is the sensor hardware sync configuration for MTP dual camera solution. customer need to
    // configure sensor hardware sync mode as their hardware design.
    for (UINT i = 0; i < m_numPhysicalCameras; i++)
    {
        CHIHANDLE    staticMetaDataHandle = const_cast<camera_metadata_t*>(
            m_logicalCameraInfo[i].m_cameraInfo.static_camera_characteristics);

        CHITAGSOPS   tagOps        = { 0 };
        UINT32 tag;
        g_chiContextOps.pTagOps(&tagOps);

        CHX_LOG("position:%d,index:%d,", m_logicalCameraInfo[i].m_cameraCaps.sensorCaps.positionType, i);

        if (REAR == m_logicalCameraInfo[i].m_cameraCaps.sensorCaps.positionType)
        {
            if (CDKResultSuccess == tagOps.pQueryVendorTagLocation("com.qti.chi.multicamerasensorconfig",
                "sensorsyncmodeconfig", &tag))
            {
                SensorSyncModeMetadata syncMode = { 0 };
                syncMode.isValid        = TRUE;
                syncMode.sensorSyncMode = MasterMode;
                tagOps.pSetMetaData(staticMetaDataHandle, tag, &syncMode, sizeof(syncMode));
            }

        }
        else if (REAR_AUX == m_logicalCameraInfo[i].m_cameraCaps.sensorCaps.positionType)
        {
            if (CDKResultSuccess == tagOps.pQueryVendorTagLocation("com.qti.chi.multicamerasensorconfig",
                "sensorsyncmodeconfig", &tag))
            {
                SensorSyncModeMetadata syncMode = { 0 };
                syncMode.isValid        = TRUE;
                syncMode.sensorSyncMode = SlaveMode;
                tagOps.pSetMetaData(staticMetaDataHandle, tag, &syncMode, sizeof(syncMode));
            }
        }
    }

    // The following are sample code on how CHI override can read/write to static meta using vendor tag
    for (UINT i = 0; i < m_numLogicalCameras; i++)
    {
        CHIHANDLE    staticMetaDataHandle = const_cast<camera_metadata_t*>(
                m_logicalCameraInfo[i].m_cameraInfo.static_camera_characteristics);
        CHITAGSOPS   tagOps        = { 0 };

        g_chiContextOps.pTagOps(&tagOps);
        if (NULL != tagOps.pQueryVendorTagLocation)
        {
            UINT32 tag;
            INT    writeData = 0xFFFF0000;
            INT    readData;

            if (CDKResultSuccess == tagOps.pQueryVendorTagLocation("com.qti.chi.override", "override_tag1", &tag))
            {
                tagOps.pSetMetaData(staticMetaDataHandle, tag, &writeData, sizeof(INT));
                tagOps.pGetMetaData(staticMetaDataHandle, tag, &readData, sizeof(INT));
                CHX_ASSERT(writeData == readData);
            }

            if (CDKResultSuccess == tagOps.pQueryVendorTagLocation("com.qti.chi.override2", "override2_tag1", &tag))
            {
                readData = 0;
                writeData = 0x0000FFFF;
                tagOps.pSetMetaData(staticMetaDataHandle, tag, &writeData, sizeof(INT));
                tagOps.pGetMetaData(staticMetaDataHandle, tag, &readData, sizeof(INT));
                CHX_ASSERT(writeData == readData);
            }

            readData = 0;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetCameraInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::GetCameraInfo(
    uint32_t     logicalCameraId,
    camera_info* cameraInfo)
{
    CDKResult     result = CDKResultEFailed;

    if (logicalCameraId < m_numLogicalCameras)
    {
        // no need for a deep copy...assuming 1 physical camera per logical camera (for now)
        ChxUtils::Memcpy(cameraInfo,
            &m_logicalCameraInfo[logicalCameraId].m_cameraInfo, sizeof(camera_info));

        result = CDKResultSuccess;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ModifySettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ModifySettings(
    VOID*     pPriv)
{
    // pPriv is an implementation-specific data type.  Implementations can cast this to the format they expect to consume, and
    // store/modify any data which is available in this class

    // sample code...this uses the input to create a data structure used to hold configuration settings
    ChiOverrideModifySetting* pMod = static_cast<ChiOverrideModifySetting*>(pPriv);
    CHX_ASSERT(pMod->token.size <= sizeof(UINT32));

    if (pMod->token.id == 9)
    {
        // Config settings for camera close indication based on ExtendClose
        *static_cast<UINT32*>(pMod->pData) = ExtensionModule::GetInstance()->IsCameraClose();
        m_pConfigSettings[pMod->token.id] = *static_cast<UINT32*>(pMod->pData);
    }
    else if (pMod->token.id == 10)
    {
        // Config settings for camera open indication based on ExtendOpen
        *static_cast<UINT32*>(pMod->pData) = ExtensionModule::GetInstance()->IsCameraOpen();
        m_pConfigSettings[pMod->token.id] = *static_cast<UINT32*>(pMod->pData);
    }
    else if (pMod->token.id == 0xFFFF)
    {
        if (NULL != pMod->pData)
        {
            g_enableChxLogs = *static_cast<UINT32*>(pMod->pData);
        }
    }
    else
    {
        if (NULL != pMod->pData)
        {
            m_pConfigSettings[pMod->token.id] = *static_cast<UINT32*>(pMod->pData);
        }
        else
        {
            m_pConfigSettings[pMod->token.id] = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::DefaultRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::DefaultRequestSettings(
    uint32_t                  cameraId,
    int                       requestTemplate,
    const camera_metadata_t** settings)
{
    (VOID)cameraId;
    (VOID)requestTemplate;
    (VOID)settings;
#if defined (_LINUX)
    if (NULL == m_pDefaultSettings)
    {
        m_pDefaultSettings = allocate_camera_metadata(DefaultSettingsNumEntries, DefaultSettingsDataSize);
    }
    else
    {
        ChxUtils::Memset(m_pDefaultSettings, 0, DefaultSettingsDataSize);
    }

    if (m_pDefaultSettings != NULL)
    {
        // Fill in any default settings (vendor tags) that needs to be added
        *settings = (const camera_metadata_t*)(m_pDefaultSettings);
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::InitializeOverrideSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::InitializeOverrideSession(
    uint32_t                        logicalCameraId,
    const camera3_device_t*         pCamera3Device,
    const chi_hal_ops_t*            chiHalOps,
    camera3_stream_configuration_t* pStreamConfig,
    bool*                           pIsOverrideEnabled,
    VOID**                          pPrivate)
{
    CDKResult          result             = CDKResultSuccess;
    UINT32             modeCount          = 0;
    ChiSensorModeInfo* pAllModes          = NULL;
    UINT32             fps                = *m_pDefaultMaxFPS;
    BOOL               isVideoMode        = FALSE;
    uint32_t           operation_mode;
    static BOOL        fovcModeCheck      = EnableFOVCUseCase();
    uint32_t           isSensorModeForce  = 0;

    *pPrivate             = NULL;
    *pIsOverrideEnabled   = FALSE;
    m_aFlushInProgress[logicalCameraId]    = FALSE;
    m_firstResult         = FALSE;
    m_logicalCameraId     = logicalCameraId;
    m_isUsecaseInBadState = FALSE;

    if (NULL == m_hCHIContext)
    {
        m_hCHIContext = g_chiContextOps.pOpenContext();
    }

    ChiVendorTagsOps vendorTagOps = { 0 };
    g_chiContextOps.pTagOps(&vendorTagOps);

    isSensorModeForce             = (pStreamConfig->operation_mode >> 24) & 0x1;
    operation_mode                = pStreamConfig->operation_mode >> 16;
    operation_mode                = operation_mode & 0x000F;
    pStreamConfig->operation_mode = pStreamConfig->operation_mode & 0xFFFF;

    for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if (0 != (pStreamConfig->streams[stream]->usage & GrallocUsageHwVideoEncoder))
        {
            isVideoMode = TRUE;
            break;
        }
    }

    GetAllSensorModes(logicalCameraId, &modeCount, &pAllModes);
    if (operation_mode > 0)
    {
        fps = (isVideoMode == TRUE && operation_mode != 0) ? pAllModes[operation_mode - 1].frameRate : *m_pDefaultMaxFPS;
    }

    if (isSensorModeForce != 0)
    {
        *m_pForceSensorMode = operation_mode - 1;
        m_usecaseMaxFPS = pAllModes[*m_pForceSensorMode].frameRate;
        m_usecaseNumBatchedFrames = 1;

        if (StreamConfigModeConstrainedHighSpeed == pStreamConfig->operation_mode)
        {
            m_usecaseNumBatchedFrames = m_usecaseMaxFPS/30;
            m_CurrentpowerHint = PERF_LOCK_POWER_HINT_VIDEO_ENCODE_HFR;
        }
    }
    else if (StreamConfigModeConstrainedHighSpeed == pStreamConfig->operation_mode)
    {
        SearchNumBatchedFrames(logicalCameraId, pStreamConfig, &m_usecaseNumBatchedFrames, &m_usecaseMaxFPS);
        m_CurrentpowerHint = PERF_LOCK_POWER_HINT_VIDEO_ENCODE_HFR;
    }
    else
    {
        // Not a HFR usecase, batch frames value need to be set to 1.
        m_usecaseNumBatchedFrames = 1;
        m_usecaseMaxFPS = fps;
        if (TRUE == isVideoMode)
        {
            if (30 >= fps)
            {
                m_CurrentpowerHint = PERF_LOCK_POWER_HINT_VIDEO_ENCODE;
            }
            else
            {
                m_CurrentpowerHint = PERF_LOCK_POWER_HINT_VIDEO_ENCODE_60FPS;
            }
        }
        else
        {
            m_CurrentpowerHint = PERF_LOCK_POWER_HINT_PREVIEW;
        }
    }

    if ((NULL != m_pPerfLockManager[logicalCameraId]) && (m_CurrentpowerHint != m_previousPowerHint))
    {
        m_pPerfLockManager[logicalCameraId]->ReleasePerfLock(m_previousPowerHint);
    }

    // Example [B == batch]: (240 FPS / 4 FPB = 60 BPS) / 30 FPS (Stats frequency goal) = 2 BPF i.e. skip every other stats
    *m_pStatsSkipPattern = m_usecaseMaxFPS / m_usecaseNumBatchedFrames / 30;
    if (*m_pStatsSkipPattern <= 0)
    {
        *m_pStatsSkipPattern = 1;
    }

    CHX_LOG_INFO("BatchSize: %u FPS: %u SkipPattern: %u", m_usecaseNumBatchedFrames, m_usecaseMaxFPS, *m_pStatsSkipPattern);

    m_VideoHDRMode = (StreamConfigModeVideoHdr == pStreamConfig->operation_mode);

    if (m_VideoHDRMode && (m_pUsecaseSelector->IsSHDRSensor(&m_logicalCameraInfo[logicalCameraId])))
    {
        m_VideoHDRModeType = *m_pSHDRModeType;
        if (*m_pSHDRModeType == InSensorVideoMode2RAWFrameSHDR)
        {
            *m_pForceSensorMode = 1;
        }
        else
        {
            *m_pForceSensorMode = *m_pSHDRModeType;
        }
        CHX_LOG_INFO("ForceMode :%d", *m_pForceSensorMode);
    }
    if (m_VideoHDRMode)
    {
        pStreamConfig->operation_mode = (pStreamConfig->operation_mode ^ StreamConfigModeVideoHdr);
    }
    m_torchWidgetUsecase = (StreamConfigModeQTITorchWidget == pStreamConfig->operation_mode);

    // this check is introduced to avoid set *m_pEnableFOVC == 1 if fovcEnable is disabled in overridesettings & fovc bit is set in operation mode.
    // as well as to avoid set,when we switch Usecases.
    if (TRUE == fovcModeCheck)
    {
        *m_pEnableFOVC = ((pStreamConfig->operation_mode & StreamConfigModeQTIFOVC) == StreamConfigModeQTIFOVC) ? 1 : 0;
    }

    SetHALOps(chiHalOps, logicalCameraId);

    m_logicalCameraInfo[logicalCameraId].m_pCamera3Device = pCamera3Device;

    UsecaseId selectedUsecaseId = m_pUsecaseSelector->GetMatchingUsecase(&m_logicalCameraInfo[logicalCameraId],
                                                                         pStreamConfig);

    CHX_LOG("For cameraId = %d selected use case = %d", logicalCameraId, selectedUsecaseId);
    if (UsecaseId::NoMatch != selectedUsecaseId)
    {
        m_pSelectedUsecase[logicalCameraId] =
            m_pUsecaseFactory->CreateUsecaseObject(&m_logicalCameraInfo[logicalCameraId],
                                                   selectedUsecaseId, pStreamConfig);

        if (NULL != m_pSelectedUsecase[logicalCameraId])
        {
            m_pStreamConfig[logicalCameraId] = static_cast<camera3_stream_configuration_t*>(
                CHX_CALLOC(sizeof(camera3_stream_configuration_t)));
            m_pStreamConfig[logicalCameraId]->streams = static_cast<camera3_stream_t**>(
                CHX_CALLOC(sizeof(camera3_stream_t*) * pStreamConfig->num_streams));
            m_pStreamConfig[logicalCameraId]->num_streams = pStreamConfig->num_streams;

            for (UINT32 i = 0; i< m_pStreamConfig[logicalCameraId]->num_streams; i++)
            {
                m_pStreamConfig[logicalCameraId]->streams[i] = pStreamConfig->streams[i];
            }

            m_pStreamConfig[logicalCameraId]->operation_mode = pStreamConfig->operation_mode;

            // use camera device / used for recovery only for regular sessioin
            m_SelectedUsecaseId[logicalCameraId] = (UINT32)selectedUsecaseId;
            CHX_LOG("Logical cam Id = %d usecase addr = %p", logicalCameraId, m_pSelectedUsecase[
                logicalCameraId]);

            m_pCamera3Device = pCamera3Device;

            m_pCameraDeviceInfo[logicalCameraId].m_pCamera3Device = m_pCamera3Device;

            *pIsOverrideEnabled = TRUE;
        }
        else
        {
            CHX_LOG_ERROR("For cameraId = %d CreateUsecaseObject failed", logicalCameraId);
            m_logicalCameraInfo[logicalCameraId].m_pCamera3Device = NULL;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::FinalizeOverrideSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::FinalizeOverrideSession(
     const camera3_device_t* camera3_device,
     VOID*                   pPriv)
{
    (VOID)camera3_device;
    (VOID)pPriv;

    return CDKResultSuccess;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////
// ExtensionModule::GetSelectedUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////
Usecase* ExtensionModule::GetSelectedUsecase(
    const camera3_device_t *pCamera3Device)
{
    Usecase* pUsecase = NULL;

    //TODO: enable when FastAec is enabled
    //look for matching entry in the fast aec list first

    //for (UINT32 id = 0; id < MaxNumImageSensors; id++)
    //{
    //    if ((NULL != m_pFastAecSession[id]) &&
    //        (m_pFastAecSession[id]->GetCamera3Device() == pCamera3Device))
    //    {
    //        pUsecase = m_pFastAecSession[id]->GetUsecase();
    //        break;
    //    }
    //}

    if (NULL == pUsecase)
    {
        for (UINT32 id = 0; id < MaxNumImageSensors; id++)
        {
            if (m_pCameraDeviceInfo[id].m_pCamera3Device && m_pCameraDeviceInfo[id].m_pCamera3Device
                == pCamera3Device)
            {
                pUsecase = m_pSelectedUsecase[id];
                break;
            }
        }
    }

    return pUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::TeardownOverrideSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::TeardownOverrideSession(
    const camera3_device_t* camera3_device,
    UINT64                  session,
    VOID*                   pPriv)
{
    (VOID)session;
    (VOID)pPriv;

    UINT32 logicalCameraId = GetCameraIdfromDevice(camera3_device);

    if (logicalCameraId >= MaxNumImageSensors)
    {
        CHX_LOG_ERROR("logicalCameraId: %d excess MaxnumImageSensors", logicalCameraId);
        return;
    }

    if (logicalCameraId < MaxNumImageSensors)
    {
        if (logicalCameraId != CDKInvalidId)
        {

            if (NULL != m_pSelectedUsecase[logicalCameraId])
            {

                m_pSelectedUsecase[logicalCameraId]->DestroyObject(FALSE);
                m_pSelectedUsecase[logicalCameraId] = NULL;

            }
        }
        m_logicalCameraInfo[logicalCameraId].m_pCamera3Device = NULL;
    }

    //TODO: enable when FastAec is in
    //if (FALSE == isFastAECState(logicalCameraId))
    //{
        CHX_LOG("Free up stream config memory");
        m_pCameraDeviceInfo[logicalCameraId].m_pCamera3Device = NULL;
        camera3_stream_configuration_t* pStreamConfig = m_pStreamConfig[logicalCameraId];
        if (NULL != pStreamConfig)
        {
            if (NULL != pStreamConfig->streams)
            {
                CHX_FREE(pStreamConfig->streams);
                pStreamConfig->streams = NULL;
            }
            CHX_FREE(pStreamConfig);
            pStreamConfig = NULL;
            m_pStreamConfig[logicalCameraId] = NULL;
        }
    //}
    m_logicalCameraInfo[logicalCameraId].m_pCamera3Device = NULL;

    if (NULL != m_pPerfLockManager[logicalCameraId])
    {
        m_pPerfLockManager[logicalCameraId]->ReleasePerfLock(m_CurrentpowerHint);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::TeardownOverrideUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::TeardownOverrideUsecase(
    const camera3_device_t* camera3_device)
{
    CHX_LOG_INFO("This is where it starts");

    // Overriding the camera close indication in ExtendClose
    UINT32 logicalCameraId = GetCameraIdfromDevice(camera3_device);
    if ((logicalCameraId < MaxNumImageSensors) && (NULL != m_pSelectedUsecase[logicalCameraId]))
    {
        m_pSelectedUsecase[logicalCameraId]->DestroyObject(TRUE);
        m_pSelectedUsecase[logicalCameraId] = NULL;
    }
    CHX_LOG_INFO("This is where it ends");

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::OverrideProcessRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::OverrideProcessRequest(
    const camera3_device_t*     camera3_device,
    camera3_capture_request_t*  pCaptureRequest,
    VOID*                       pPriv)
{
    CDKResult result = CDKResultSuccess;
    UINT32 logicalCameraId = GetCameraIdfromDevice(camera3_device);
    //TODO: Backporting-flush resolve WaitOnAcquireFence and uncomment this
    //Wait on acquire fence here and close it to eliminated any leaks in usecases.
    /*
    for (UINT32 i = 0; i < pCaptureRequest->num_output_buffers; i++)
    {
        ChxUtils::WaitOnAcquireFence((CHISTREAMBUFFER*)&pCaptureRequest->output_buffers[i]);
        INT*   pAcquireFence = (INT*)&pCaptureRequest->output_buffers[i].acquire_fence;
        *pAcquireFence = -1;
    }
    */
    if (TRUE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aFlushInProgress[logicalCameraId])))
    {
        CHX_LOG_INFO("flush enabled, frame %d", pCaptureRequest->frame_number);
        HandleProcessRequestErrorAllPCRs(pCaptureRequest, logicalCameraId);
        return CDKResultSuccess;
    }

    if (ChxUtils::IsLongExposureCapture(const_cast<camera_metadata_t*>(pCaptureRequest->settings)))
    {
        ChxUtils::AtomicStoreU32(&m_aLongExposureInProgress, TRUE);
        m_longExposureFrame = pCaptureRequest->frame_number;
        CHX_LOG_INFO("Long exposure enabled in frame %d", pCaptureRequest->frame_number);
    }

    m_pPCRLock[logicalCameraId]->Lock(); // logicalCameraId?
    // Save the original metadata
    const camera_metadata_t* pOriginalMetadata = pCaptureRequest->settings;
    (VOID)pPriv;

    if (logicalCameraId != CDKInvalidId && NULL != m_pSelectedUsecase[logicalCameraId])
    {
        m_originalFrameWorkNumber = pCaptureRequest->frame_number;

        if (NULL != pCaptureRequest->settings)
        {
            FreeLastKnownRequestSetting();
            m_pLastKnownRequestSettings = allocate_copy_camera_metadata_checked(pCaptureRequest->settings,
                get_camera_metadata_size(pCaptureRequest->settings));
        }

        //Recovery happened if framework didn't send any metadata, send valid metadata
        if (m_firstFrameAfterRecovery == pCaptureRequest->frame_number &&
            NULL == pCaptureRequest->settings)
        {
            CHX_LOG_INFO("Setting Request for first frame after case =%d", m_firstFrameAfterRecovery);
            pCaptureRequest->settings = m_pLastKnownRequestSettings;
            m_firstFrameAfterRecovery = 0;
        }

        result = m_pSelectedUsecase[logicalCameraId]->ProcessCaptureRequest(pCaptureRequest);
    }

    if (pCaptureRequest->settings != NULL)
    {
        // Restore the original metadata pointer that came from the framework
        pCaptureRequest->settings = pOriginalMetadata;
    }

    // handle error
    // return result skipping error handling for FastAEC
    if (result == CDKResultETimeout)
        //TODO: Enable when FastAec is merged
        //&& (FALSE == isFastAECState(logicalCameraId)))
    {
        CHX_LOG_INFO("CHI override is recovering from an error, lets create case again");

        TeardownOverrideUsecase(m_pCamera3Device);

        m_pSelectedUsecase[logicalCameraId] =
            m_pUsecaseFactory->CreateUsecaseObject(&m_logicalCameraInfo[logicalCameraId],
                static_cast<UsecaseId>(m_SelectedUsecaseId[logicalCameraId]),
                m_pStreamConfig[logicalCameraId]);
        m_firstFrameAfterRecovery = m_originalFrameWorkNumber + 1;
        CHX_LOG_INFO("CHI override has successfully recovered. use case is created for next request =%d", m_firstFrameAfterRecovery);
        m_isUsecaseInBadState = FALSE;
        result = CDKResultSuccess;
    }
    //Need to return success on PCR to allow FW to continue sending requests
    else if (result == CDKResultEBusy)
    {
        result = CDKResultSuccess;
    }

    m_pPCRLock[logicalCameraId]->Unlock(); // logicalCameraId?
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::FreeLastKnownRequestSetting
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::FreeLastKnownRequestSetting()
{
    if (NULL != m_pLastKnownRequestSettings)
    {
        CHX_LOG_INFO("Freeing last known request setting");
        free_camera_metadata(m_pLastKnownRequestSettings);
        m_pLastKnownRequestSettings = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::OverrideFlush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::OverrideFlush(
    const camera3_device_t*     camera3_device)
{
    CDKResult   result          = CDKResultSuccess;
    UINT32      logicalCameraId = GetCameraIdfromDevice(camera3_device);

    //We do not want to block flush from executing when long exposure capture is in progress
    if (FALSE == static_cast<BOOL>(ChxUtils::AtomicLoadU32(&m_aLongExposureInProgress)))
    {
        m_pPCRLock[logicalCameraId]->Lock();    // logicalCameraId?
        m_isFlushLocked = TRUE;
    }

    ChxUtils::AtomicStoreU32(&m_aFlushInProgress[logicalCameraId], TRUE);

    if (logicalCameraId != CDKInvalidId)
    {
        if (NULL != m_pPerfLockManager[logicalCameraId])
        {
            m_pPerfLockManager[logicalCameraId]->AcquirePerfLock(PERF_LOCK_CLOSE_CAMERA, 1000);
        }
        result = m_pSelectedUsecase[logicalCameraId]->Flush();
    }

    ChxUtils::AtomicStoreU32(&m_aFlushInProgress[logicalCameraId], FALSE);
    if (m_isFlushLocked == TRUE)
    {
        m_pPCRLock[logicalCameraId]->Unlock(); // logicalCameraId?
        m_isFlushLocked = FALSE;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::HandleProcessRequestErrorAllPCRs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::HandleProcessRequestErrorAllPCRs(
    camera3_capture_request_t* pRequest,
    UINT32 logicalCameraId)
{
    ChiMessageDescriptor messageDescriptor;

    messageDescriptor.messageType = ChiMessageTypeError;
    messageDescriptor.message.errorMessage.frameworkFrameNum = pRequest->frame_number;
    messageDescriptor.message.errorMessage.errorMessageCode = MessageCodeRequest;
    messageDescriptor.message.errorMessage.pErrorStream = NULL;

    CHX_LOG("Sending errorMessageCode for frame %d ", pRequest->frame_number);
    ReturnFrameworkMessage((camera3_notify_msg_t*)&messageDescriptor, logicalCameraId);

    camera3_capture_result_t result;
    result.frame_number = pRequest->frame_number;
    result.result = NULL;
    result.num_output_buffers = pRequest->num_output_buffers;
    result.output_buffers = reinterpret_cast<const camera3_stream_buffer_t *>(pRequest->output_buffers);

    for (UINT i = 0; i < pRequest->num_output_buffers; i++)
    {
        camera3_stream_buffer_t* pStreamBuffer =
            const_cast<camera3_stream_buffer_t*>(&pRequest->output_buffers[i]);
        pStreamBuffer->release_fence = -1;
        pStreamBuffer->status = 1;
    }

    if(NULL != pRequest->input_buffer)
    {
        result.input_buffer = reinterpret_cast<const camera3_stream_buffer_t *>(pRequest->input_buffer);
        camera3_stream_buffer_t* pStreamBuffer =
            const_cast<camera3_stream_buffer_t*>(pRequest->input_buffer);
        pStreamBuffer->release_fence = -1;
        pStreamBuffer->status = 1;
    }
    else
    {
        result.input_buffer = NULL;
    }

    result.partial_result = 1;

    //Usecase::ReturnFrameworkResultFlush(reinterpret_cast<const camera3_capture_result_t*>(&result));
    CHX_LOG_ERROR("Sending Request error for frame %d ", pRequest->frame_number);
    ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(&result), logicalCameraId);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::CreatePipelineDescriptor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIPIPELINEDESCRIPTOR ExtensionModule::CreatePipelineDescriptor(
    PipelineCreateData* pPipelineCreateData) ///< Pipeline create descriptor
{
    return (g_chiContextOps.pCreatePipelineDescriptor(m_hCHIContext,
                                                      pPipelineCreateData->pPipelineCreateDescriptor,
                                                      pPipelineCreateData->numOutputs,
                                                      pPipelineCreateData->pOutputDescriptors,
                                                      pPipelineCreateData->numInputs,
                                                      pPipelineCreateData->pInputOptions));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::DestroyPipelineDescriptor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::DestroyPipelineDescriptor(
    CHIPIPELINEDESCRIPTOR pipelineHandle)
{
    return g_chiContextOps.pDestroyPipelineDescriptor(m_hCHIContext, pipelineHandle);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::CreateSession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIHANDLE ExtensionModule::CreateSession(
    SessionCreateData* pSessionCreateData) ///< Session create descriptor
{
    return g_chiContextOps.pCreateSession(m_hCHIContext,
                                          pSessionCreateData->numPipelines,
                                          pSessionCreateData->pPipelineInfo,
                                          pSessionCreateData->pCallbacks,
                                          pSessionCreateData->pPrivateCallbackData,
                                          pSessionCreateData->flags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::DestroySession
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::DestroySession(
    CHIHANDLE sessionHandle,
    BOOL isForced)
{
    return g_chiContextOps.pDestroySession(m_hCHIContext, sessionHandle, isForced);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::Flush(CHIHANDLE sessionHandle)
{
    //TODO: Add recovery logic
    return g_chiContextOps.pFlushSession(m_hCHIContext, sessionHandle, FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ForcedFlush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ForcedFlush(CHIHANDLE sessionHandle)
{
    g_chiContextOps.pFlushSession(m_hCHIContext, sessionHandle, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::SubmitRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::SubmitRequest(
    CHIPIPELINEREQUEST* pSubmitRequest)
{
    CDKResult         result = CDKResultSuccess;

    if(TRUE == m_isUsecaseInBadState)
    {
       CHX_LOG_ERROR("Request timed out returning");
       return CDKResultETimeout;
    }

    result = g_chiContextOps.pSubmitPipelineRequest(m_hCHIContext, pSubmitRequest);

    if(result == CDKResultETimeout && m_isUsecaseInBadState == FALSE)
    {
        CHX_LOG_ERROR("Offline session is in bad state.");
         m_isUsecaseInBadState = TRUE;

         /// Offline pipeline is in bad state.Flush all results to get new
         /// realtime requests to hit recovery faster.
        m_pSelectedUsecase[m_logicalCameraId]->Flush();
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ActivatePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::ActivatePipeline(
    CHIHANDLE             sessionHandle,
    CHIPIPELINEDESCRIPTOR pipelineHandle)
{
    return g_chiContextOps.pActivatePipeline(m_hCHIContext, sessionHandle, pipelineHandle, NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::DeactivatePipeline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::DeactivatePipeline(
    CHIHANDLE                 sessionHandle,
    CHIPIPELINEDESCRIPTOR     pipelineHandle,
    CHIDEACTIVATEPIPELINEMODE modeBitmask)
{
    return g_chiContextOps.pDeactivatePipeline(m_hCHIContext, sessionHandle, pipelineHandle, modeBitmask);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::EnumerateSensorModes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::EnumerateSensorModes(
    UINT32             physCameraId,
    UINT32             numSensorModes,
    CHISENSORMODEINFO* pSensorModeInfo)
{
    return g_chiContextOps.pEnumerateSensorModes(m_hCHIContext, physCameraId, numSensorModes, pSensorModeInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetVendorTagOps
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::GetVendorTagOps(
    CHITAGSOPS* pVendorTagOps)
{
    g_chiContextOps.pTagOps(pVendorTagOps);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetAllSensorModes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ExtensionModule::GetAllSensorModes(
    UINT32              cameraId,
    UINT32*             pNumSensorModes,
    CHISENSORMODEINFO** ppAllSensorModes)
{
    CDKResult result = CDKResultEInvalidArg;

    if ((NULL != pNumSensorModes) && (NULL != ppAllSensorModes))
    {
        *pNumSensorModes  = m_logicalCameraInfo[cameraId].m_cameraCaps.numSensorModes;
        *ppAllSensorModes = m_logicalCameraInfo[cameraId].pSensorModeInfo;
        result            = CDKResultSuccess;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ReturnFrameworkResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ReturnFrameworkResult(
    const camera3_capture_result_t* pResult,
    UINT32 cameraID)
{
    if ((NULL != m_pPerfLockManager[cameraID]) && (FALSE == m_firstResult))
    {
        m_pPerfLockManager[cameraID]->AcquirePerfLock(m_CurrentpowerHint);
        m_previousPowerHint = m_CurrentpowerHint;
        m_firstResult = TRUE;
    }

    if (pResult->frame_number == m_longExposureFrame)
    {
        if (pResult->num_output_buffers != 0)
        {
            CHX_LOG_INFO("Returning long exposure snapshot");
            ChxUtils::AtomicStoreU32(&m_aLongExposureInProgress, FALSE);
            m_longExposureFrame = static_cast<UINT32>(InvalidFrameNumber);
        }
    }
    if (NULL != m_pHALOps[cameraID]) {
        m_pHALOps[cameraID]->process_capture_result(m_logicalCameraInfo[cameraID].m_pCamera3Device, pResult);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::ReturnFrameworkMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ExtensionModule::ReturnFrameworkMessage(
    const camera3_notify_msg_t* pMessage,
    UINT32 cameraID)
{
    if (NULL != m_pHALOps[cameraID]) {
        m_pHALOps[cameraID]->notify_result(m_logicalCameraInfo[cameraID].m_pCamera3Device, pMessage);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetCameraIdfromDevice
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ExtensionModule::GetCameraIdfromDevice(const camera3_device_t *pCamera3Device)
{
    UINT32 id = CDKInvalidId;

    for (UINT32 i = 0; i < MaxNumImageSensors; i++)
    {
        if ((NULL != m_logicalCameraInfo[i].m_pCamera3Device) &&
            (m_logicalCameraInfo[i].m_pCamera3Device == pCamera3Device))
        {
            id = i;
            break;
        }
    }
    return id;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtensionModule::GetCameraIdfromDevice
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 ExtensionModule::GetNumPCRsBeforeStreamOn(VOID* pMetaData) const
{
    CamxResult      result  = CamxResultSuccess;
    UINT32          metaTag = 0;
    CHITAGSOPS      m_vendorTagOps;
    static UINT8    isEarlyPCREnabledVendorTag = 0;

    // In android P the metadata pointer wil be part of session parameter. This logic will change accordingly.
    if (NULL != pMetaData)
    {
        GetInstance()->GetVendorTagOps(&m_vendorTagOps);

        result = m_vendorTagOps.pQueryVendorTagLocation("org.quic.camera.EarlyPCRenable", "EarlyPCRenable", &metaTag);

        if (CDKResultSuccess == result)
        {
            result = m_vendorTagOps.pGetMetaData(
                reinterpret_cast<camera_metadata_t*>(const_cast<VOID*>(pMetaData)),
                metaTag,
                &isEarlyPCREnabledVendorTag,
                sizeof(isEarlyPCREnabledVendorTag));

            if (CDKResultSuccess != result)
            {
                CHX_LOG_ERROR("pGetMetaData failed result %d", result);
            }
        }
        else
        {
            CHX_LOG_ERROR("pQueryVendorTagLocation failed result %d", result);
        }
    }

    if (FALSE == isEarlyPCREnabledVendorTag)
    {
        *m_pNumPCRsBeforeStreamOn = 0;
    }

    CHX_LOG("EarlyPCR: chiextensionModule %d", *m_pNumPCRsBeforeStreamOn);

    return *m_pNumPCRsBeforeStreamOn;
}
