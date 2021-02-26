////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxmulticamcontroller.cpp
/// @brief MultiCamController class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxmulticamcontroller.h"
#include "chxzoomtranslator.h"
#include "chiifedefs.h"
#include "camxcommontypes.h"

#undef LOG_TAG
#define LOG_TAG "CHIMULTICAMCONTROLLER"

ChiVendorTagsOps MultiCamControllerManager::s_vendorTagOps;
UINT32           MultiCamControllerManager::m_vendorTagOpticalZoomResultMeta;
UINT32           MultiCamControllerManager::m_vendorTagOpticalZoomInputMeta;
UINT32           MultiCamControllerManager::m_vendorTagBokehResultMeta;
UINT32           MultiCamControllerManager::m_vendorTagBokehInputMeta;
UINT32           MultiCamControllerManager::m_vendorTagMultiCameraRole;
UINT32           MultiCamControllerManager::m_vendorTagCropRegions;
UINT32           MultiCamControllerManager::m_vendorTagIFEAppliedCrop;
UINT32           MultiCamControllerManager::m_vendorTagSensorIFEAppliedCrop;
UINT32           MultiCamControllerManager::m_vendorTagIFEResidualCrop;
UINT32           MultiCamControllerManager::m_vendorTagLuxIndex;
UINT32           MultiCamControllerManager::m_vendorTagMasterInfo;
UINT32           MultiCamControllerManager::m_vendorTagLPMInfo;
UINT32           MultiCamControllerManager::m_vendorTagSyncInfo;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamControllerManager::MultiCamControllerManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiCamControllerManager::MultiCamControllerManager()
{
    ChxUtils::Memset(&m_pController, 0, MaxNumImageSensors * sizeof(MultiCamController*));

    // Get CHI vendor tag ops
#if defined(_LP64)
    const CHAR*     pChiDriver = "/vendor/lib64/hw/camera.qcom.so";
#else // _LP64
    const CHAR*     pChiDriver = "/vendor/lib/hw/camera.qcom.so";
#endif // _LP64
    OSLIBRARYHANDLE handle = ChxUtils::LibMap(pChiDriver);
    CHICONTEXTOPS   chiContextOps;
    PCHIENTRY       funcPChiEntry = reinterpret_cast<PCHIENTRY>(ChxUtils::LibGetAddr(handle, "ChiEntry"));
    if (NULL == funcPChiEntry)
    {
        CHX_LOG("Invalid pointer of ChiEntry");
        return;
    }
    funcPChiEntry(&chiContextOps);
    chiContextOps.pTagOps(&s_vendorTagOps);

    // Get the vendor tags needed by MultiCamControllers

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicameraoutputmetadata",
                                           "OutputMetadataOpticalZoom",
                                           &m_vendorTagOpticalZoomResultMeta);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainputmetadata",
                                           "InputMetadataOpticalZoom",
                                           &m_vendorTagOpticalZoomInputMeta);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicameraoutputmetadata",
                                           "OutputMetadataBokeh",
                                           &m_vendorTagBokehResultMeta);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainputmetadata",
                                           "InputMetadataBokeh",
                                           &m_vendorTagBokehInputMeta);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainfo",
                                           "MultiCameraIdRole",
                                           &m_vendorTagMultiCameraRole);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.cropregions",
                                           "crop_regions",
                                           &m_vendorTagCropRegions);

    s_vendorTagOps.pQueryVendorTagLocation("org.quic.camera.ifecropinfo",
                                           "ResidualCrop",
                                           &m_vendorTagIFEResidualCrop);

    s_vendorTagOps.pQueryVendorTagLocation("org.quic.camera.ifecropinfo",
                                           "AppliedCrop",
                                           &m_vendorTagIFEAppliedCrop);

    s_vendorTagOps.pQueryVendorTagLocation("org.quic.camera.ifecropinfo",
                                           "SensorIFEAppliedCrop",
                                           &m_vendorTagSensorIFEAppliedCrop);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.statsaec",
                                           "AecLux",
                                           &m_vendorTagLuxIndex);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainfo",
                                           "MasterCamera",
                                           &m_vendorTagMasterInfo);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainfo",
                                           "LowPowerMode",
                                           &m_vendorTagLPMInfo);

    s_vendorTagOps.pQueryVendorTagLocation("com.qti.chi.multicamerainfo",
                                           "SyncMode",
                                           &m_vendorTagSyncInfo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamControllerManager::GetController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiCamController* MultiCamControllerManager::GetController(
    mcc_create_data_t* pMccCreateData)
{
    CHX_ASSERT(NULL != pMccCreateData);
    CHX_ASSERT(pMccCreateData->desiredControllerMode != CONTROLLER_TYPE_NONE);
    CHX_ASSERT(pMccCreateData->logicalCameraId < MaxNumImageSensors);

    // Check if the current camera already has an active controller
    MultiCamController* pMultiCamController = m_pController[pMccCreateData->logicalCameraId];

    // If the controller type matches with the new initialize request, reconfigure the controller
    if ((NULL != pMultiCamController) &&
        (pMccCreateData->desiredControllerMode == pMultiCamController->GetType()))
    {
        // Reconfigure the controller
        pMultiCamController->Reconfigure(pMccCreateData);
    }
    else
    {
        // Destroy the active controller and create a new controller
        if (NULL != pMultiCamController)
        {
            pMultiCamController->Destroy();
            pMultiCamController = NULL;
        }

        switch (pMccCreateData->desiredControllerMode)
        {
        case CONTROLLER_TYPE_DUALFOV:
            pMultiCamController = DualFovController::Create(pMccCreateData);
            break;
        case CONTROLLER_TYPE_RTB:
            pMultiCamController = RTBController::Create(pMccCreateData);
            break;
        case CONTROLLER_TYPE_BAYERMONO:
            pMultiCamController = BayerMonoController::Create(pMccCreateData);
            break;
         case CONTROLLER_TYPE_VR:
            pMultiCamController = VRController::Create(pMccCreateData);
            break;
        default:
            pMultiCamController = NULL;
            CHX_LOG("Invalid controller type");
            break;
        }

        if (NULL != pMultiCamController)
        {
            m_pController[pMccCreateData->logicalCameraId] = pMultiCamController;
        }
        else
        {
            CHX_LOG("Error in creating multicamcontroller");
        }
    }

    return pMultiCamController;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamControllerManager::DestroyController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamControllerManager::DestroyController(
    MultiCamController* pMultiCamController)
{
    if (NULL != pMultiCamController)
    {
        for (UINT32 i = 0; i < MaxNumImageSensors; ++i)
        {
            if (m_pController[i] == pMultiCamController)
            {
                m_pController[i]->Destroy();
                m_pController[i] = NULL;
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamControllerManager::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamControllerManager::Destroy()
{
    for (UINT32 i = 0; i < MaxNumImageSensors; ++i)
    {
        if (NULL != m_pController[i])
        {
            m_pController[i]->Destroy();
            m_pController[i] = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamControllerManager::~MultiCamControllerManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiCamControllerManager::~MultiCamControllerManager()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::~MultiCamController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MultiCamController::~MultiCamController()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::ConsolidateCameraInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult MultiCamController::ConsolidateCameraInfo(
    UINT32            numBundledCameras,
    CHICAMERAINFO**   ppCamInfo,
    CHICAMERAINFO*    pConsolidatedCamInfo)
{
    CDKResult result = CDKResultEFailed;

    CHX_ASSERT(NULL != pConsolidatedCamInfo);

    multicam_controller_type_t controllerType = CONTROLLER_TYPE_NONE;

    if (DualCamCount == numBundledCameras)
    {
        // Check if the pair consists of one main and one aux camera
        BOOL isRearMainCamInBundle = FALSE;
        BOOL isRearAuxCamInBundle  = FALSE;
        BOOL isFrontMainCamInBundle = FALSE;
        BOOL isFrontAuxCamInBundle  = FALSE;
        if ((ppCamInfo[0]->sensorCaps.positionType == REAR) ||
            (ppCamInfo[1]->sensorCaps.positionType == REAR))
        {
            isRearMainCamInBundle = TRUE;
        }

        if ((ppCamInfo[0]->sensorCaps.positionType == REAR_AUX) ||
            (ppCamInfo[1]->sensorCaps.positionType == REAR_AUX))
        {
            isRearAuxCamInBundle = TRUE;
        }

        if ((ppCamInfo[0]->sensorCaps.positionType == FRONT) ||
            (ppCamInfo[1]->sensorCaps.positionType == FRONT))
        {
            isFrontMainCamInBundle = TRUE;
        }

        if ((ppCamInfo[0]->sensorCaps.positionType == FRONT_AUX) ||
            (ppCamInfo[1]->sensorCaps.positionType == FRONT_AUX))
        {
            isFrontAuxCamInBundle = TRUE;
        }

        CHX_LOG("Rear Main, Aux : %d, %d, Front Main, Aux : %d, %d ", isRearMainCamInBundle,isRearAuxCamInBundle,
            isFrontMainCamInBundle, isFrontAuxCamInBundle);

        result = (isRearMainCamInBundle && isRearAuxCamInBundle) || (isFrontMainCamInBundle && isFrontAuxCamInBundle)
            ? CDKResultSuccess : CDKResultEFailed;

        if (CDKResultSuccess == result)
        {
            // Check the focal lengths to identify the controller type
            FLOAT focalLengthMain = ppCamInfo[0]->lensCaps.focalLength;
            FLOAT focalLengthAux  = ppCamInfo[1]->lensCaps.focalLength;

            CHX_LOG("focalLengthMain %f focalLengthAux %f", focalLengthMain, focalLengthAux);

            if (focalLengthMain != focalLengthAux)
            {
                controllerType = CONTROLLER_TYPE_DUALFOV;
            }
            else if ((ppCamInfo[0]->lensCaps.horViewAngle > CAMERA_VIEW_ANGLE_THRESHOLD) &&
                    (ppCamInfo[1]->lensCaps.horViewAngle > CAMERA_VIEW_ANGLE_THRESHOLD))
            {
                controllerType = CONTROLLER_TYPE_VR;
            }
            else
            {
                controllerType = CONTROLLER_TYPE_BAYERMONO;
            }
        }
    }

    if (CDKResultSuccess == result)
    {
        switch (controllerType)
        {
        case CONTROLLER_TYPE_DUALFOV:
            result = DualFovController::ConsolidateCameraInfo(numBundledCameras, ppCamInfo, pConsolidatedCamInfo);
            break;
        case CONTROLLER_TYPE_BAYERMONO:
            result = BayerMonoController::ConsolidateCameraInfo(numBundledCameras, ppCamInfo, pConsolidatedCamInfo);
            break;
        case CONTROLLER_TYPE_VR:
            result = VRController::ConsolidateCameraInfo(numBundledCameras, ppCamInfo, pConsolidatedCamInfo);
            break;
        default:
            break;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::TranslateRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult MultiCamController::TranslateRequestSettings(
    multicam_req_settings* pMultiCamSettings)
{
    // If the concrete class hasn't implemented this method, return Success.
    (VOID)pMultiCamSettings;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::Reconfigure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult MultiCamController::Reconfigure(
    mcc_create_data_t* pMccCreateData)
{
    // If the concrete class hasn't implemented this method, return Success.
    (VOID)pMccCreateData;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::ProcessResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamController::ProcessResultMetadata(
    VOID* pResultMetadata)
{
    // If the concrete class hasn't implemented this method, do nothing.
    (VOID)pResultMetadata;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::GetResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
controller_result_t MultiCamController::GetResult()
{
    // If the concrete class hasn't implemented this method, return result as invalid
    m_result.isValid = FALSE;
    return m_result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::FillOfflinePipelineInputMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult MultiCamController::FillOfflinePipelineInputMetadata(
    multicam_result_metadata_t* pMultiCamResultMetadata,
    VOID*                       pOfflineMetadata,
    BOOL                        isSnapshotPipeline)
{
    // If the concrete class hasn't implemented this method, return Success by default.
    (VOID)pMultiCamResultMetadata;
    (VOID)pOfflineMetadata;
    (VOID)isSnapshotPipeline;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::UpdateVendorTags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamController::UpdateVendorTags(
    multicam_req_settings* pMultiCamSettings)
{
    CDKResult result = CDKResultEFailed;

    camera_metadata_t* pMetadata           = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[0]);
    camera_metadata_t* pTranslatedMetadata = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[1]);

    // Add Sync info
    BOOL isSyncActive        = FALSE;
    BOOL isWideActive        = pMultiCamSettings->currentRequestMCCResult.activeCameras[0].isActive;
    BOOL isTeleActive        = pMultiCamSettings->currentRequestMCCResult.activeCameras[1].isActive;
    BOOL isDualZoneActive    = ((TRUE == isWideActive) && (TRUE == isTeleActive)) ? TRUE : FALSE;

    // Enable frame sync if override settings is enabled and if its in in DUAL ZONE
    if ((TRUE == pMultiCamSettings->kernelFrameSyncEnable) && (TRUE == isDualZoneActive))
    {
        isSyncActive = TRUE;
    }

    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
        MultiCamControllerManager::m_vendorTagSyncInfo, &isSyncActive, sizeof(SyncModeInfo));
    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata,
        MultiCamControllerManager::m_vendorTagSyncInfo, &isSyncActive, sizeof(SyncModeInfo));
    CHX_LOG("isSyncActive %d", isSyncActive);

    // Add LPM info
    if (CDKResultSuccess == result)
    {
        LowPowerModeInfo lowPowerMode;
        lowPowerMode.isLPMEnabled               = (TRUE == isDualZoneActive) ? FALSE : TRUE;
        lowPowerMode.isSlaveOperational         = isDualZoneActive;
        lowPowerMode.lowPowerMode[0].isEnabled  = (TRUE == isWideActive) ? FALSE : TRUE;
        lowPowerMode.lowPowerMode[1].isEnabled  = (TRUE == isTeleActive) ? FALSE : TRUE;

        result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagLPMInfo,
            &lowPowerMode, sizeof(LowPowerModeInfo));
        result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata,
            MultiCamControllerManager::m_vendorTagLPMInfo,
            &lowPowerMode, sizeof(LowPowerModeInfo));

        CHX_LOG("WIDE LPM : %d , TELE LPM : %d ", lowPowerMode.lowPowerMode[0].isEnabled,
            lowPowerMode.lowPowerMode[1].isEnabled);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::ExtractCameraMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamController::ExtractCameraMetadata(
    VOID*           pResultMetadata,
    CameraMetadata* pExtractedCameraMetadata)
{
    camera_metadata_entry_t entry = { 0 };
    camera_metadata_t* pMetadata = static_cast<camera_metadata_t*>(pResultMetadata);
    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_CONTROL_AF_REGIONS, &entry))
    {
        pExtractedCameraMetadata->afFocusROI.xMin   = entry.data.i32[0];
        pExtractedCameraMetadata->afFocusROI.yMin   = entry.data.i32[1];
        pExtractedCameraMetadata->afFocusROI.xMax   = entry.data.i32[2];
        pExtractedCameraMetadata->afFocusROI.yMax   = entry.data.i32[3];
        pExtractedCameraMetadata->afFocusROI.weight = entry.data.i32[4];
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_CONTROL_AF_STATE, &entry))
    {
        pExtractedCameraMetadata->afState = entry.data.u8[0];
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_SENSOR_SENSITIVITY, &entry))
    {
        pExtractedCameraMetadata->isoSensitivity = entry.data.i32[0];
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_SENSOR_EXPOSURE_TIME, &entry))
    {
        pExtractedCameraMetadata->exposureTime = entry.data.i64[0];
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_SENSOR_TIMESTAMP, &entry))
    {
        pExtractedCameraMetadata->sensorTimestamp = entry.data.i64[0];
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_STATISTICS_FACE_RECTANGLES, &entry))
    {
        UINT32 numElemsRect = sizeof(CHIRECT) / sizeof(UINT32);
        pExtractedCameraMetadata->fdMetadata.numFaces = entry.count / numElemsRect;

        UINT32 dataIndex = 0;
        for (INT32 i = 0; i < pExtractedCameraMetadata->fdMetadata.numFaces; ++i)
        {
            INT32 xMin = entry.data.i32[dataIndex++];
            INT32 yMin = entry.data.i32[dataIndex++];
            INT32 xMax = entry.data.i32[dataIndex++];
            INT32 yMax = entry.data.i32[dataIndex++];
            pExtractedCameraMetadata->fdMetadata.faceRect[i].left   = xMin;
            pExtractedCameraMetadata->fdMetadata.faceRect[i].top    = yMin;
            pExtractedCameraMetadata->fdMetadata.faceRect[i].width  = xMax - xMin + 1;
            pExtractedCameraMetadata->fdMetadata.faceRect[i].height = yMax - yMin + 1;
        }
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_STATISTICS_FACE_SCORES, &entry))
    {
        for (INT32 i = 0; i < pExtractedCameraMetadata->fdMetadata.numFaces; ++i)
        {
            pExtractedCameraMetadata->fdMetadata.faceScore[i] = entry.data.u8[i];
        }
    }

    IFECropInfo ifeResidualCrop;
    if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagIFEResidualCrop, &ifeResidualCrop, sizeof(IFECropInfo)))
    {
        pExtractedCameraMetadata->ifeResidualCrop.left   = ifeResidualCrop.fullPath.left;
        pExtractedCameraMetadata->ifeResidualCrop.top    = ifeResidualCrop.fullPath.top;
        pExtractedCameraMetadata->ifeResidualCrop.width  = ifeResidualCrop.fullPath.width;
        pExtractedCameraMetadata->ifeResidualCrop.height = ifeResidualCrop.fullPath.height;
    }

    IFECropInfo ifeAppliedCrop;
    if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagIFEAppliedCrop, &ifeAppliedCrop, sizeof(IFECropInfo)))
    {
        pExtractedCameraMetadata->ifeAppliedCrop.left   = ifeAppliedCrop.fullPath.left;
        pExtractedCameraMetadata->ifeAppliedCrop.top    = ifeAppliedCrop.fullPath.top;
        pExtractedCameraMetadata->ifeAppliedCrop.width  = ifeAppliedCrop.fullPath.width;
        pExtractedCameraMetadata->ifeAppliedCrop.height = ifeAppliedCrop.fullPath.height;
    }

    IFECropInfo sensorIFEAppliedCrop;
    if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagSensorIFEAppliedCrop, &sensorIFEAppliedCrop, sizeof(IFECropInfo)))
    {
        pExtractedCameraMetadata->sensorIFEAppliedCrop.left   = sensorIFEAppliedCrop.fullPath.left;
        pExtractedCameraMetadata->sensorIFEAppliedCrop.top    = sensorIFEAppliedCrop.fullPath.top;
        pExtractedCameraMetadata->sensorIFEAppliedCrop.width  = sensorIFEAppliedCrop.fullPath.width;
        pExtractedCameraMetadata->sensorIFEAppliedCrop.height = sensorIFEAppliedCrop.fullPath.height;
    }

    FLOAT lux = 0.0f;
    if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagLuxIndex, &lux, sizeof(FLOAT)))
    {
        pExtractedCameraMetadata->lux = lux;
    }

    if (0 == find_camera_metadata_entry(pMetadata, ANDROID_LENS_FOCUS_DISTANCE, &entry))
    {
        /* Unit of optimal focus distance in diopter, convert it to cm */
        if (entry.data.f[0] > 0.0f)
        {
            pExtractedCameraMetadata->focusDistCm = 10.0f / entry.data.f[0];
        }
        else
        {
            pExtractedCameraMetadata->focusDistCm = FocusDistanceCmMax;
        }
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MultiCamController::UpdateScalerCropForSnapshot
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID MultiCamController::UpdateScalerCropForSnapshot(
    VOID* pMetadata)
{
    (VOID)pMetadata;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::~DualFovController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DualFovController::~DualFovController()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID DualFovController::Destroy()
{
    if (NULL != m_pLock)
    {
        m_pLock->Destroy();
        m_pLock = NULL;
    }

    if (NULL != m_pZoomTranslator)
    {
        m_pZoomTranslator->Destroy();
        m_pZoomTranslator = NULL;
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DualFovController* DualFovController::Create(
    mcc_create_data_t* pMccCreateData)
{
    CHX_ASSERT(DualCamCount == pMccCreateData->numBundledCameras);

    DualFovController* pDualFovController = CHX_NEW DualFovController;

    if (NULL != pDualFovController)
    {
        pDualFovController->m_controllerType = CONTROLLER_TYPE_DUALFOV;

        // Parse the camera info to build the controller
        cam_info_t* pCamInfoMain            = pMccCreateData->pBundledCamInfo[0];
        cam_info_t* pCamInfoAux             = pMccCreateData->pBundledCamInfo[1];
        cam_info_t* pCamInfoWide            = NULL;
        cam_info_t* pCamInfoTele            = NULL;
        camera_info_t* pAppCameraInfoMain   = static_cast<camera_info_t*>(pCamInfoMain->pChiCamInfo->pLegacy);
        camera_info_t* pAppCameraInfoAux    = static_cast<camera_info_t*>(pCamInfoAux->pChiCamInfo->pLegacy);

        // @note: Remove the facing check after resolving Fovwider(false) case
        if ((pCamInfoMain->pChiCamInfo->lensCaps.focalLength < pCamInfoAux->pChiCamInfo->lensCaps.focalLength) ||
            (FacingFront == pAppCameraInfoAux->facing))
        {
            pDualFovController->m_isMainCamFovWider = TRUE;
            pCamInfoWide = pCamInfoMain;
            pCamInfoTele = pCamInfoAux;
        }
        else
        {
            pDualFovController->m_isMainCamFovWider = FALSE;
            pCamInfoWide = pCamInfoAux;
            pCamInfoTele = pCamInfoMain;
        }

        CHX_LOG("Main focal %f, Aux focal %f, Main camera id %d, Aux camera id %d",
            pCamInfoMain->pChiCamInfo->lensCaps.focalLength,
            pCamInfoAux->pChiCamInfo->lensCaps.focalLength,
            pCamInfoMain->camId, pCamInfoTele->camId);

        CHX_LOG("Main facing %d Aux facing %d",
            pAppCameraInfoMain->facing, pAppCameraInfoAux->facing);

        // Save wide, tele and logical camera ids
        pDualFovController->m_camIdWide    = pCamInfoWide->camId;
        pDualFovController->m_camIdTele    = pCamInfoTele->camId;
        pDualFovController->m_camIdLogical = pMccCreateData->logicalCameraId;

        // Get focal length, pixel size, sensor dimensions and active pixel array size
        pDualFovController->m_focalLengthWide      = pCamInfoWide->pChiCamInfo->lensCaps.focalLength;
        pDualFovController->m_focalLengthTele      = pCamInfoTele->pChiCamInfo->lensCaps.focalLength;
        pDualFovController->m_pixelPitchWide       = pCamInfoWide->pChiCamInfo->sensorCaps.pixelSize;
        pDualFovController->m_pixelPitchTele       = pCamInfoTele->pChiCamInfo->sensorCaps.pixelSize;
        pDualFovController->m_sensorDimensionWide  = pCamInfoWide->sensorOutDimension;
        pDualFovController->m_sensorDimensionTele  = pCamInfoTele->sensorOutDimension;
        pDualFovController->m_activeArraySizeWide  = pCamInfoWide->pChiCamInfo->sensorCaps.activeArray;
        pDualFovController->m_activeArraySizeTele  = pCamInfoTele->pChiCamInfo->sensorCaps.activeArray;
        pDualFovController->m_fovRectIFEWide       = pCamInfoWide->fovRectIFE;
        pDualFovController->m_fovRectIFETele       = pCamInfoTele->fovRectIFE;
        pDualFovController->m_pRawOTPData          = pCamInfoWide->pChiCamInfo->sensorCaps.pRawOTPData;
        pDualFovController->m_rawOTPDataSize       = pCamInfoWide->pChiCamInfo->sensorCaps.rawOTPDataSize;

        pDualFovController->m_isVideoStreamSelected = FALSE;
        for (UINT32 i = 0; i < pMccCreateData->pStreamConfig->numStreams; i++)
        {
            if (0 != (GRALLOC_USAGE_HW_VIDEO_ENCODER & pMccCreateData->pStreamConfig->pStreamInfo[i].usage))
            {
                pDualFovController->m_isVideoStreamSelected = TRUE;
            }
            if ((CAMERA3_STREAM_OUTPUT == pMccCreateData->pStreamConfig->pStreamInfo[i].streamType) &&
                (GRALLOC_USAGE_HW_COMPOSER == (GRALLOC_USAGE_HW_COMPOSER & pMccCreateData->pStreamConfig->pStreamInfo[i].usage)))
            {
                pDualFovController->m_previewDimensions = pMccCreateData->pStreamConfig->pStreamInfo[i].streamDimension;
            }
        }

        CDKResult result = pDualFovController->CalculateTransitionParams();

        if(CDKResultSuccess == result)
        {
            pDualFovController->m_pLock = Mutex::Create();

            if (NULL == pDualFovController->m_pLock)
            {
                result = CDKResultENoMemory;
            }
        }

        if (CDKResultSuccess == result)
        {
            // Create and initialize the zoom translator
            pDualFovController->m_pZoomTranslator = ZoomTranslator::Create();

            if (NULL != pDualFovController->m_pZoomTranslator)
            {
                zoom_translator_init_data_t zoomTranslatorInitData = { 0 };
                zoomTranslatorInitData.adjustedFovRatio     = pDualFovController->m_adjustedFovRatio;
                zoomTranslatorInitData.sensorDimensionWide  = pDualFovController->m_sensorDimensionWide;
                zoomTranslatorInitData.sensorDimensionTele  = pDualFovController->m_sensorDimensionTele;
                zoomTranslatorInitData.activeArraySizeWide  = pDualFovController->m_activeArraySizeWide;
                zoomTranslatorInitData.activeArraySizeTele  = pDualFovController->m_activeArraySizeTele;
                zoomTranslatorInitData.previewDimension     = pDualFovController->m_previewDimensions;

                zoomTranslatorInitData.pRawOTPData          = pDualFovController->m_pRawOTPData;
                zoomTranslatorInitData.rawOTPDataSize       = pDualFovController->m_rawOTPDataSize;

                zoomTranslatorInitData.fovRectIFEWide       = pDualFovController->m_fovRectIFEWide;
                zoomTranslatorInitData.fovRectIFETele       = pDualFovController->m_fovRectIFETele;

                result = pDualFovController->m_pZoomTranslator->Init(&zoomTranslatorInitData);

                // Set initial result state
                pDualFovController->SetInitialResultState();
            }
            else
            {
                result = CDKResultEFailed;
            }
        }

        // Destroy the object in case of failure
        if (CDKResultSuccess != result)
        {
            pDualFovController->m_pZoomTranslator->Deinit();
            pDualFovController->Destroy();
            pDualFovController = NULL;
        }
    }

    return pDualFovController;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::Reconfigure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DualFovController::Reconfigure(
    mcc_create_data_t* pMccCreateData)
{
    CDKResult result = CDKResultSuccess;

    CHX_ASSERT(DualCamCount == pMccCreateData->numBundledCameras);

    // Parse the camera info to reconfigure the controller
    cam_info_t* pCamInfoWide = NULL;
    cam_info_t* pCamInfoTele = NULL;

    if (TRUE == m_isMainCamFovWider)
    {
        pCamInfoWide = pMccCreateData->pBundledCamInfo[0];
        pCamInfoTele = pMccCreateData->pBundledCamInfo[1];
    }
    else
    {
        pCamInfoWide = pMccCreateData->pBundledCamInfo[1];
        pCamInfoTele = pMccCreateData->pBundledCamInfo[0];
    }

    // Get sensor output dimensions
    m_sensorDimensionWide = pCamInfoWide->sensorOutDimension;
    m_sensorDimensionTele = pCamInfoTele->sensorOutDimension;

    result = CalculateTransitionParams();

    m_isVideoStreamSelected = FALSE;
    for (UINT32 i = 0; i < pMccCreateData->pStreamConfig->numStreams; i++)
    {
        if (0 != (GRALLOC_USAGE_HW_VIDEO_ENCODER & pMccCreateData->pStreamConfig->pStreamInfo[i].usage))
        {
            m_isVideoStreamSelected = TRUE;
        }
        if ((CAMERA3_STREAM_OUTPUT == pMccCreateData->pStreamConfig->pStreamInfo[i].streamType) &&
            (GRALLOC_USAGE_HW_COMPOSER == (GRALLOC_USAGE_HW_COMPOSER & pMccCreateData->pStreamConfig->pStreamInfo[i].usage)))
        {
            m_previewDimensions = pMccCreateData->pStreamConfig->pStreamInfo[i].streamDimension;
        }
    }

    if (CDKResultSuccess == result)
    {
        result = m_pZoomTranslator->Deinit();

        if (CDKResultSuccess == result)
        {
            // Initialize the zoom translator
            zoom_translator_init_data_t zoomTranslatorInitData = { 0 };
            zoomTranslatorInitData.adjustedFovRatio     = m_adjustedFovRatio;
            zoomTranslatorInitData.sensorDimensionWide  = m_sensorDimensionWide;
            zoomTranslatorInitData.sensorDimensionTele  = m_sensorDimensionTele;
            zoomTranslatorInitData.activeArraySizeWide  = m_activeArraySizeWide;
            zoomTranslatorInitData.activeArraySizeTele  = m_activeArraySizeTele;
            zoomTranslatorInitData.previewDimension     = m_previewDimensions;

            zoomTranslatorInitData.pRawOTPData    = pCamInfoWide->pChiCamInfo->sensorCaps.pRawOTPData;
            zoomTranslatorInitData.rawOTPDataSize = pCamInfoWide->pChiCamInfo->sensorCaps.rawOTPDataSize;

            zoomTranslatorInitData.fovRectIFEWide = m_fovRectIFEWide;
            zoomTranslatorInitData.fovRectIFETele = m_fovRectIFETele;

            result = m_pZoomTranslator->Init(&zoomTranslatorInitData);

            // Set initial result state
            SetInitialResultState();
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::CalculateTransitionParams
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DualFovController::CalculateTransitionParams()
{
    CDKResult result = CDKResultEFailed;

    FLOAT fovWide = m_sensorDimensionWide.width * m_pixelPitchWide / m_focalLengthWide;
    FLOAT fovTele = m_sensorDimensionTele.width * m_pixelPitchTele / m_focalLengthTele;

    if (fovTele > 0.0f)
    {
        m_adjustedFovRatio = fovWide / fovTele;

        // Calculate the transition parameters
        m_transitionTeleToWide = m_adjustedFovRatio;
        m_transitionWideToTele = m_transitionTeleToWide + m_adjustedFovRatio * PercentMarginHysterisis;
        m_transitionLow        = m_transitionWideToTele - m_adjustedFovRatio * PercentMarginTransitionZone;
        m_transitionHigh       = m_transitionTeleToWide + m_adjustedFovRatio * PercentMarginTransitionZone;

        // Adjust transitionLow and transitionHigh based on fusion thresholds
        m_transitionLow  = (SNAPSHOT_FUSION_ZOOM_MIN < m_transitionLow)  ? SNAPSHOT_FUSION_ZOOM_MIN : m_transitionLow;
        m_transitionHigh = (SNAPSHOT_FUSION_ZOOM_MAX > m_transitionHigh) ? SNAPSHOT_FUSION_ZOOM_MAX : m_transitionHigh;

        CHX_LOG("transition param: low : %f", m_transitionLow);
        CHX_LOG("transition param: t2w : %f", m_transitionTeleToWide);
        CHX_LOG("transition param: w2t : %f", m_transitionWideToTele);
        CHX_LOG("transition param: high: %f", m_transitionHigh);
        CHX_LOG("fovWide: %f fovTele: %f m_adjustedFovRatio %f", fovWide, fovTele, m_adjustedFovRatio);

        result = CDKResultSuccess;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::SetInitialResultState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID DualFovController::SetInitialResultState()
{
    m_result.activeCameras[0].cameraId = m_camIdWide;
    m_result.activeCameras[1].cameraId = m_camIdTele;

    if (m_zoomUser <= m_transitionWideToTele)
    {
        m_result.masterCameraId            = m_camIdWide;
        m_result.masterCameraRole          = CameraRoleTypeWide;
        m_recommendedMasterCameraRole      = CameraRoleTypeWide;
        m_result.activeCameras[0].isActive = TRUE;
        m_result.activeCameras[1].isActive = FALSE;
    }
    else
    {
        m_result.masterCameraId            = m_camIdTele;
        m_result.masterCameraRole          = CameraRoleTypeTele;
        m_recommendedMasterCameraRole      = CameraRoleTypeTele;
        m_result.activeCameras[0].isActive = FALSE;
        m_result.activeCameras[1].isActive = TRUE;
    }

    m_result.snapshotFusion = FALSE;
    // Activate both sensors if LPM is disabled

    if (FALSE == ENABLE_LPM)
    {
        m_result.activeCameras[0].isActive = TRUE;
        m_result.activeCameras[1].isActive = TRUE;
    }
    if (TRUE == m_isVideoStreamSelected)
    {
        m_result.snapshotFusion = FALSE;
    }
    else
    {
        m_result.snapshotFusion = TRUE;
    }
    m_result.isValid        = TRUE;

    CHX_LOG("Active cameras Wide[%d], Tele[%d]", m_result.activeCameras[0].isActive, m_result.activeCameras[1].isActive);
    CHX_LOG("Master camera id: %d fusion: %d", m_result.masterCameraId, m_result.snapshotFusion);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::ConsolidateCameraInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DualFovController::ConsolidateCameraInfo(
    UINT32            numBundledCameras,
    CHICAMERAINFO**   ppCamInfo,
    CHICAMERAINFO*    pConsolidatedCamInfo)
{
    CHX_ASSERT(NULL != ppCamInfo);
    CHX_ASSERT(NULL != pConsolidatedCamInfo);
    CHX_ASSERT(DualCamCount == numBundledCameras);
    CHX_ASSERT(NULL != ppCamInfo[0]);
    CHX_ASSERT(NULL != ppCamInfo[1]);

    CDKResult result = CDKResultSuccess;

    camera_info_t* pCameraInfoMain = static_cast<camera_info_t*>(ppCamInfo[0]->pLegacy);
    const camera_metadata_t* pMetadataMain = pCameraInfoMain->static_camera_characteristics;

    camera_info_t* pCameraInfoAux = static_cast<camera_info_t*>(ppCamInfo[1]->pLegacy);
    const camera_metadata_t* pMetadataAux = pCameraInfoAux->static_camera_characteristics;

    camera_info_t* pCameraInfoConsolidated = static_cast<camera_info_t*>(pConsolidatedCamInfo->pLegacy);
    const camera_metadata_t* pMetadataConsolidated = pCameraInfoConsolidated->static_camera_characteristics;

    pConsolidatedCamInfo->lensCaps       = ppCamInfo[0]->lensCaps;
    pConsolidatedCamInfo->numSensorModes = ppCamInfo[0]->numSensorModes;
    pConsolidatedCamInfo->sensorCaps     = ppCamInfo[0]->sensorCaps;
    ChxUtils::Memcpy(pConsolidatedCamInfo->pLegacy, ppCamInfo[0]->pLegacy, sizeof(camera_info_t));
    pCameraInfoConsolidated->static_camera_characteristics = pMetadataConsolidated;

    pConsolidatedCamInfo->size = ppCamInfo[0]->size;

    if ((NULL != pMetadataMain) && (NULL != pMetadataAux))
    {
        camera_metadata_entry_t entryMain = { 0 };
        camera_metadata_entry_t entryAux  = { 0 };
        camera_metadata_entry_t entryConsolidated = { 0 };

        // update available stream configure for consolidate metadata
        // this is dual camera usecase. So availabe stream configure should be
        // compilation of main and aux camera.
        if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
                ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryMain)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataAux),
                ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryAux)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
                ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryConsolidated)))
        {
            UINT32 consolidatedEntryCount = 0;
            // 4 elements in the tuple of ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS
            UINT32 tupleSize      = 4;
            UINT32 numEntriesMain = entryMain.count / tupleSize;
            UINT32 numEntriesAux  = entryAux.count / tupleSize;

            INT32* pEntryData     = static_cast<INT32*>(ChxUtils::Calloc(
                (numEntriesAux + numEntriesMain) * tupleSize * sizeof(INT32)));
            if (NULL != pEntryData)
            {
                ChxUtils::Memcpy(pEntryData, entryMain.data.i32, entryMain.count * sizeof(INT32));
                consolidatedEntryCount = numEntriesMain;

                for (UINT32 j = 0; j < numEntriesAux; j++)
                {
                    UINT32 auxIndex  = j * tupleSize;
                    BOOL matchFound  = FALSE;

                    for (UINT32 i = 0; i < numEntriesMain; i++)
                    {
                        UINT32 mainIndex = i * tupleSize;
                        if ((entryMain.data.i32[mainIndex]     == entryAux.data.i32[auxIndex])     &&
                            (entryMain.data.i32[mainIndex + 1] == entryAux.data.i32[auxIndex + 1]) &&
                            (entryMain.data.i32[mainIndex + 2] == entryAux.data.i32[auxIndex + 2]) &&
                            (entryMain.data.i32[mainIndex + 3] == entryAux.data.i32[auxIndex + 3]))
                        {
                            matchFound = TRUE;
                            break;
                        }
                    }

                    // if this stream configure is not in main metadata, add it to consolidate metadata.
                    if (FALSE == matchFound)
                    {
                        UINT32 consolidatedIndex          = consolidatedEntryCount * tupleSize;

                        pEntryData[consolidatedIndex]     = entryAux.data.i32[auxIndex];
                        pEntryData[consolidatedIndex + 1] = entryAux.data.i32[auxIndex + 1];
                        pEntryData[consolidatedIndex + 2] = entryAux.data.i32[auxIndex + 2];
                        pEntryData[consolidatedIndex + 3] = entryAux.data.i32[auxIndex + 3];

                        consolidatedEntryCount++;
                    }
                }

                entryConsolidated.count = consolidatedEntryCount * tupleSize;

                if (0 != update_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated), entryConsolidated.index,
                        pEntryData, entryConsolidated.count, &entryConsolidated))
                {
                    CHX_LOG_WARN("update availablestreamconfigure for consolidate metadata failed!");
                }

                ChxUtils::Free(pEntryData);
                pEntryData = NULL;
            }
            else
            {
                CHX_LOG("No memory allocated for pEntryData");
                result = CDKResultENoMemory;
            }
        }
    }

    if ((NULL != pMetadataMain) && (NULL != pMetadataAux))
    {
        camera_metadata_entry_t entryMain = { 0 };
        camera_metadata_entry_t entryAux  = { 0 };
        camera_metadata_entry_t entryConsolidated = { 0 };
        if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
                ANDROID_JPEG_MAX_SIZE, &entryMain)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataAux),
                ANDROID_JPEG_MAX_SIZE, &entryAux)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
                ANDROID_JPEG_MAX_SIZE, &entryConsolidated)))
        {

            INT32 maxJPEGSizeOfMain = *entryMain.data.i32;
            INT32 maxJPEGSizeOfAux  = *entryAux.data.i32;
            INT32 maxJPEGSize       = maxJPEGSizeOfMain;
            // we need to use big JPEG size for consolidate metadata
            if (maxJPEGSizeOfAux > maxJPEGSizeOfMain)
            {
                maxJPEGSize = maxJPEGSizeOfAux;
            }

            if (0 != update_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
                                                    entryConsolidated.index,
                                                    &maxJPEGSize,
                                                    entryConsolidated.count,
                                                    &entryConsolidated))
            {
                CHX_LOG_WARN("update MAXJPEGSize metadata for consolidate metadata failed!");
            }

        }

    }

    if ((NULL != pMetadataMain) && (NULL != pMetadataAux))
    {
        camera_metadata_entry_t entryMain = { 0 };
        camera_metadata_entry_t entryAux  = { 0 };
        camera_metadata_entry_t entryConsolidated = { 0 };
        if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
                ANDROID_LENS_FACING, &entryMain)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataAux),
                ANDROID_LENS_FACING, &entryAux)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
                ANDROID_LENS_FACING, &entryConsolidated)))
        {
            // Use front face as consolidate metadata face,
            // app needs the aux info ( front facing ) to select the logical camera
            INT32 iLensFace = entryAux.data.i32[0];
            if (0 != update_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated), entryConsolidated.index,
                &iLensFace, entryConsolidated.count, &entryConsolidated))
            {
                CHX_LOG_WARN("update lence face metadata for consolidate metadata failed!");
            }
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::TranslateRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DualFovController::TranslateRequestSettings(
    multicam_req_settings* pMultiCamSettings)
{
    CDKResult result = CDKResultEFailed;

    if (NULL != pMultiCamSettings)
    {
        CHX_ASSERT(DualCamCount == pMultiCamSettings->numSettingsBuffers);

        camera_metadata_t* pMetadata           = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[0]);
        camera_metadata_t* pTranslatedMetadata = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[1]);

        if ((NULL != pMetadata) && (NULL != pTranslatedMetadata))
        {
            // Translate the reference crop window if app set this vendor tag for wide
            RefCropWindowSize refCropWindowWide, refCropWindowTele;
            UINT32 refCropSizeTag = 0;

            result = MultiCamControllerManager::s_vendorTagOps.pQueryVendorTagLocation(
                "org.quic.camera2.ref.cropsize", "RefCropSize", &refCropSizeTag);
            if (CDKResultSuccess == result)
            {
                result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData((CHIHANDLE)pMetadata,
                    refCropSizeTag, &refCropWindowWide, sizeof(refCropWindowWide));

                if ((CDKResultSuccess == result) && (refCropWindowWide.width > 0) && (refCropWindowWide.height > 0))
                {
                    refCropWindowTele.width  =
                        refCropWindowWide.width * m_activeArraySizeTele.width / m_activeArraySizeWide.width;
                    refCropWindowTele.height =
                        refCropWindowWide.height * m_activeArraySizeTele.height / m_activeArraySizeWide.height;
                    CHX_LOG("Translate RefCropSize Wide w:%d h:%d Tele w:%d h:%d", refCropWindowWide.width,
                        refCropWindowWide.height, refCropWindowTele.width, refCropWindowTele.height);
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata, refCropSizeTag,
                        &refCropWindowTele, sizeof(refCropWindowTele));
                }
            }
            // Translate the zoom crop window
            camera_metadata_entry_t entryCropRegion      = { 0 };
            camera_metadata_entry_t entryCropRegionTrans = { 0 };
            if (0 == find_camera_metadata_entry(pMetadata, ANDROID_SCALER_CROP_REGION, &entryCropRegion))
            {
                translated_zoom_t translatedZoom;

                CHIRECT userZoom;
                userZoom.left   = entryCropRegion.data.i32[0];
                userZoom.top    = entryCropRegion.data.i32[1];
                userZoom.width  = entryCropRegion.data.i32[2];
                userZoom.height = entryCropRegion.data.i32[3];

                if (CDKResultSuccess == m_pZoomTranslator->GetTranslatedZoom(&userZoom, &translatedZoom))
                {
                    // Update the zoom value for wide camera
                    zoom_regions_t zoomPreview = translatedZoom.zoomPreview;
                    update_camera_metadata_entry(pMetadata, entryCropRegion.index,
                        &zoomPreview.zoomWideTotal, sizeof(zoomPreview.zoomWideTotal), NULL);

                    /* Update the vendor tag for the CropRegions containing
                    user crop, pipeline crop and IFE crop limit */
                    CaptureRequestCropRegions cropRegionsWide;
                    CaptureRequestCropRegions cropRegionsTele;

                    cropRegionsWide.userCropRegion     = userZoom;
                    cropRegionsWide.pipelineCropRegion = zoomPreview.zoomWideTotal;
                    cropRegionsWide.ifeLimitCropRegion = zoomPreview.zoomWideIsp;

                    cropRegionsTele.userCropRegion     = userZoom;
                    cropRegionsTele.pipelineCropRegion = zoomPreview.zoomTeleTotal;
                    cropRegionsTele.ifeLimitCropRegion = zoomPreview.zoomTeleIsp;

                    CHX_LOG("inputcropregion:wide:%d,%d,%d,%d",zoomPreview.zoomWideTotal.left,
                        zoomPreview.zoomWideTotal.top,zoomPreview.zoomWideTotal.width,zoomPreview.zoomWideTotal.height);
                    ChxUtils::SetVendorTagValue(pMetadata, CropRegions,
                        sizeof(CaptureRequestCropRegions), &cropRegionsWide);
                    ChxUtils::SetVendorTagValue(pTranslatedMetadata, CropRegions,
                        sizeof(CaptureRequestCropRegions), &cropRegionsTele);

                    m_zoomWide = m_activeArraySizeWide.width / (FLOAT)translatedZoom.zoomSnapshot.zoomWideTotal.width;
                    m_zoomTele = m_activeArraySizeTele.width / (FLOAT)translatedZoom.zoomSnapshot.zoomTeleTotal.width;
                    m_zoomUser = m_activeArraySizeWide.width / (FLOAT)userZoom.width;

                    CHX_LOG("Transalate Settings Zoom User: %.2f, Zoom Wide: %.2f, Zoom Tele: %.2f",
                        m_zoomUser, m_zoomWide, m_zoomTele);

                    if (0 == find_camera_metadata_entry(pTranslatedMetadata, ANDROID_SCALER_CROP_REGION, &entryCropRegionTrans))
                    {
                        update_camera_metadata_entry(pTranslatedMetadata, entryCropRegionTrans.index,
                            &zoomPreview.zoomTeleTotal, sizeof(zoomPreview.zoomTeleTotal), NULL);
                    CHX_LOG("inputcropregion:tele:%d,%d,%d,%d",zoomPreview.zoomTeleTotal.left,
                        zoomPreview.zoomTeleTotal.top,zoomPreview.zoomTeleTotal.width,zoomPreview.zoomTeleTotal.height);

                    }
                }
            }

            // Translate the Focus ROI
            camera_metadata_entry_t entryAFRegionMain = { 0 };
            camera_metadata_entry_t entryAFRegionAux  = { 0 };

            if ((0 == find_camera_metadata_entry(pMetadata, ANDROID_CONTROL_AF_REGIONS, &entryAFRegionMain)) &&
                (0 == find_camera_metadata_entry(pTranslatedMetadata, ANDROID_CONTROL_AF_REGIONS, &entryAFRegionAux)))
            {
                // AF_REGION tag tuple has 5 elements [xMin, yMin, xMax, yMax, weight]
                UINT32 tupleSize = 5;
                for (UINT32 i = 0; i < entryAFRegionMain.count / tupleSize; i++)
                {
                    WeightedRegion afRegion = { 0 };
                    UINT32 index = i * tupleSize;

                    afRegion.xMin   = entryAFRegionMain.data.i32[index];
                    afRegion.yMin   = entryAFRegionMain.data.i32[index + 1];
                    afRegion.xMax   = entryAFRegionMain.data.i32[index + 2];
                    afRegion.yMax   = entryAFRegionMain.data.i32[index + 3];
                    afRegion.weight = entryAFRegionMain.data.i32[index + 4];

                    // Get the translated AF ROI for the wide camera
                    WeightedRegion afRegionTrans = TranslateMeteringRegion(&afRegion, CameraRoleTypeWide);

                    // Update the metadata
                    update_camera_metadata_entry(pMetadata, entryAFRegionMain.index,
                        &afRegionTrans, sizeof(WeightedRegion), NULL);

                    // Get the translated AF ROI for the tele camera
                    afRegionTrans = TranslateMeteringRegion(&afRegion, CameraRoleTypeTele);

                    // Update the metadata
                    update_camera_metadata_entry(pTranslatedMetadata, entryAFRegionAux.index,
                        &afRegionTrans, sizeof(WeightedRegion), NULL);
                }
            }

            // Translate the Metering ROI
            camera_metadata_entry_t entryAERegionMain = { 0 };
            camera_metadata_entry_t entryAERegionAux  = { 0 };

            if ((0 == find_camera_metadata_entry(pMetadata, ANDROID_CONTROL_AE_REGIONS, &entryAERegionMain)) &&
                (0 == find_camera_metadata_entry(pTranslatedMetadata, ANDROID_CONTROL_AE_REGIONS, &entryAERegionAux)))
            {
                // AE_REGION tag tuple has 5 elements [xMin, yMin, xMax, yMax, weight]
                UINT32 tupleSize = 5;
                for (UINT32 i = 0; i < entryAERegionMain.count / tupleSize; i++)
                {
                    WeightedRegion aeRegion = { 0 };
                    UINT32 index = i * tupleSize;

                    aeRegion.xMin   = entryAERegionMain.data.i32[index];
                    aeRegion.yMin   = entryAERegionMain.data.i32[index + 1];
                    aeRegion.xMax   = entryAERegionMain.data.i32[index + 2];
                    aeRegion.yMax   = entryAERegionMain.data.i32[index + 3];
                    aeRegion.weight = entryAERegionMain.data.i32[index + 4];

                    // Get the translated AE ROI for the wide camera
                    WeightedRegion aeRegionTrans = TranslateMeteringRegion(&aeRegion, CameraRoleTypeWide);

                    // Update the metadata
                    update_camera_metadata_entry(pMetadata, entryAERegionMain.index,
                        &aeRegionTrans, sizeof(WeightedRegion), NULL);

                    // Get the translated AE ROI for the tele camera
                    aeRegionTrans = TranslateMeteringRegion(&aeRegion, CameraRoleTypeTele);

                    // Update the metadata
                    update_camera_metadata_entry(pTranslatedMetadata, entryAERegionAux.index,
                        &aeRegionTrans, sizeof(WeightedRegion), NULL);
                }
            }

            // Get master camera of this request
            CameraRoleType masterCameraRole = pMultiCamSettings->currentRequestMCCResult.masterCameraRole;

            // Send the camera role and camera id for the first frame.
            if (0 == pMultiCamSettings->frameNum)
            {
                // Set the MultiCameraIdRole for the main camera
                MultiCameraIdRole inputMetadata;
                inputMetadata.currentCameraId   = m_isMainCamFovWider ? m_camIdWide : m_camIdTele;
                inputMetadata.currentCameraRole = m_isMainCamFovWider ? CameraRoleTypeWide : CameraRoleTypeTele;
                inputMetadata.logicalCameraId   = m_camIdLogical;
                inputMetadata.masterCameraRole  = masterCameraRole;
                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
                    MultiCamControllerManager::m_vendorTagMultiCameraRole,
                    &inputMetadata, sizeof(MultiCameraIdRole));
                 CHX_LOG("currentCameraId %d Camera role %d", inputMetadata.currentCameraId, inputMetadata.currentCameraRole);

                // Set the MultiCameraIdRole for the aux camera
                inputMetadata.currentCameraId   = m_isMainCamFovWider ? m_camIdTele : m_camIdWide;
                inputMetadata.currentCameraRole = m_isMainCamFovWider ? CameraRoleTypeTele : CameraRoleTypeWide;
                inputMetadata.logicalCameraId   = m_camIdLogical;
                inputMetadata.masterCameraRole  = masterCameraRole;
                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata,
                    MultiCamControllerManager::m_vendorTagMultiCameraRole,
                    &inputMetadata, sizeof(MultiCameraIdRole));
                CHX_LOG("currentCameraId %d Camera role %d", inputMetadata.currentCameraId, inputMetadata.currentCameraRole);
            }

            // Add master camera info
            BOOL isMaster = FALSE;
            isMaster = (CameraRoleTypeWide == masterCameraRole) ? TRUE : FALSE;
            result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
                MultiCamControllerManager::m_vendorTagMasterInfo, &isMaster, sizeof(BOOL));

            isMaster = (CameraRoleTypeTele == masterCameraRole) ? TRUE : FALSE;
            result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata,
                MultiCamControllerManager::m_vendorTagMasterInfo, &isMaster, sizeof(BOOL));
            CHX_LOG("masterCameraRole %d", masterCameraRole);

            // Update all the common vendor tags
            UpdateVendorTags(pMultiCamSettings);

            result = CDKResultSuccess;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::ProcessResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID DualFovController::ProcessResultMetadata(
    VOID* pResultMetadata)
{
    OutputMetadataOpticalZoom metadataOpticalZoom;
    MultiCameraIdRole MultiCameraRole;

    m_pLock->Lock();

    if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pResultMetadata,
        MultiCamControllerManager::m_vendorTagOpticalZoomResultMeta, &metadataOpticalZoom, sizeof(OutputMetadataOpticalZoom)))
    {
        CHX_LOG("Current master: %d Recommended master: %d", metadataOpticalZoom.masterCamera,
            metadataOpticalZoom.recommendedMasterCamera);

        // Update the master camera info
        m_result.masterCameraId   = (CameraRoleTypeWide == metadataOpticalZoom.recommendedMasterCamera) ? m_camIdWide : m_camIdTele;
        m_result.masterCameraRole = metadataOpticalZoom.recommendedMasterCamera;

        m_recommendedMasterCameraRole = metadataOpticalZoom.recommendedMasterCamera;

        // Update LPM info
        m_result.activeCameras[0].cameraId = m_camIdWide;
        m_result.activeCameras[1].cameraId = m_camIdTele;

        if (CameraRoleTypeWide == metadataOpticalZoom.lowPowerMode[0].cameraRole)
        {
            m_result.activeCameras[0].isActive = metadataOpticalZoom.lowPowerMode[0].isEnabled ? 0 : 1;
            m_result.activeCameras[1].isActive = metadataOpticalZoom.lowPowerMode[1].isEnabled ? 0 : 1;
        }
        else
        {
            m_result.activeCameras[0].isActive = metadataOpticalZoom.lowPowerMode[1].isEnabled ? 0 : 1;
            m_result.activeCameras[1].isActive = metadataOpticalZoom.lowPowerMode[0].isEnabled ? 0 : 1;
        }

        if (FALSE == ENABLE_LPM)
        {
            m_result.activeCameras[0].isActive = TRUE;
            m_result.activeCameras[1].isActive = TRUE;
        }

        // Never put the master into LPM
        if (CameraRoleTypeWide == m_recommendedMasterCameraRole && FALSE == m_result.activeCameras[0].isActive)
        {
            CHX_LOG("Wide is master and inactive, setting wide to active");
            m_result.activeCameras[0].isActive = TRUE;
        }
        else if (CameraRoleTypeTele == m_recommendedMasterCameraRole && FALSE == m_result.activeCameras[1].isActive)
        {
            CHX_LOG("Tele is master and inactive, setting tele to active");
            m_result.activeCameras[1].isActive = TRUE;
        }

        if ((TRUE == m_result.activeCameras[0].isActive) && (TRUE == m_result.activeCameras[1].isActive) &&
            (TRUE != m_isVideoStreamSelected))
        {
            m_result.snapshotFusion = TRUE;
        }
        else
        {
            m_result.snapshotFusion = FALSE;
        }

        // Update spatial alignement pixel shift
        INT32 horzShiftPreview = metadataOpticalZoom.outputShiftPreview.horizonalShift;
        INT32 vertShiftPreview = metadataOpticalZoom.outputShiftPreview.verticalShift;
        INT32 horzShiftSnapshot = metadataOpticalZoom.outputShiftSnapshot.horizonalShift;
        INT32 vertShiftSnapshot = metadataOpticalZoom.outputShiftSnapshot.verticalShift;

        m_pixelShiftWidePreview  = { horzShiftPreview, vertShiftPreview };
        m_pixelShiftWideSnapshot = { horzShiftSnapshot, vertShiftSnapshot };

        m_result.isValid = TRUE;

        CHX_LOG("Zoom User: %.2f, Zoom Wide: %.2f, Zoom Tele: %.2f", m_zoomUser, m_zoomWide, m_zoomTele);
        CHX_LOG("Active cameras Wide[%d], Tele[%d]", m_result.activeCameras[0].isActive, m_result.activeCameras[1].isActive);
        CHX_LOG("Master camera id: %d, Fusion: %d", m_result.masterCameraId, m_result.snapshotFusion);
    }
    // Read LUX index and object focus distance for master camera
    else if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pResultMetadata,
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole)))
    {
        if (m_recommendedMasterCameraRole == MultiCameraRole.currentCameraRole)
        {
            FLOAT lux = 0.0f;
            if (CDKResultSuccess == MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pResultMetadata,
                    MultiCamControllerManager::m_vendorTagLuxIndex, &lux, sizeof(FLOAT)))
            {
                m_currentLuxIndex = lux;
            }
            m_currentFocusDistCm = 100;
            // Update the fusion enable flag
            if ( ENABLE_SNAPSHOT_FUSION                                  &&
                (m_zoomUser >= SNAPSHOT_FUSION_ZOOM_MIN)                 &&
                (m_zoomUser <= SNAPSHOT_FUSION_ZOOM_MAX)                 &&
                (m_currentLuxIndex >= SNAPSHOT_FUSION_LUX_IDX_THRESHOLD) &&
                (m_currentFocusDistCm >= SNAPSHOT_FUSION_FOCUS_DIST_CM_MIN))
            {
                m_result.snapshotFusion = TRUE;
            }
            else
            {
                m_result.snapshotFusion = FALSE;
            }
        }
    }

    m_pLock->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::GetResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
controller_result_t DualFovController::GetResult()
{
    m_pLock->Lock();
    controller_result_t result = m_result;
    m_pLock->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::TranslateMeteringRegion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL DualFovController::isFusionEnabled()
{
    m_pLock->Lock();
    BOOL isFusionEnabled = m_result.snapshotFusion;
    m_pLock->Unlock();
    return isFusionEnabled;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::TranslateMeteringRegion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WeightedRegion DualFovController::TranslateMeteringRegion(
    WeightedRegion* pRegion,
    CameraRoleType  camRole)
{
    FLOAT fovRatio = 1.0f;

    WeightedRegion    regionInput = *pRegion;
    WeightedRegion    regionTrans = regionInput;
    pixel_shift_t     pixelShift = { 0 };
    CHIRECT           activeArray = { 0 };

    // Acquire mutex to read spatial alignment shifts which are written by other thread
    m_pLock->Lock();
    if (CameraRoleTypeWide == camRole)
    {
        fovRatio    = 1.0f;
        pixelShift  = m_pixelShiftWidePreview;
        activeArray = m_activeArraySizeWide;
    }
    else
    {
        fovRatio = (m_zoomTele / m_zoomWide) * m_adjustedFovRatio;
        pixelShift.xShift = (m_adjustedFovRatio / m_zoomUser) * m_pixelShiftWidePreview.xShift;
        pixelShift.yShift = (m_adjustedFovRatio / m_zoomUser) * m_pixelShiftWidePreview.yShift;
        activeArray = m_activeArraySizeTele;
    }
    m_pLock->Unlock();

    // ROI should be with respect to the corresponding active array
    FLOAT xScale = ((FLOAT)activeArray.width  / m_activeArraySizeWide.width);
    FLOAT yScale = ((FLOAT)activeArray.height / m_activeArraySizeWide.height);

    regionInput.xMin *= xScale;
    regionInput.yMin *= yScale;
    regionInput.xMax *= xScale;
    regionInput.yMax *= yScale;

    INT32 regionWidth       = regionInput.xMax - regionInput.xMin + 1;
    INT32 regionHeight      = regionInput.yMax - regionInput.yMin + 1;
    INT32 regionTransWidth  = regionWidth * fovRatio;
    INT32 regionTransHeight = regionHeight * fovRatio;

    INT32 regionHorzDelta = (regionTransWidth - regionWidth) / 2;
    INT32 regionVertDelta = (regionTransHeight - regionHeight) / 2;

    regionTrans.xMin = regionInput.xMin - regionHorzDelta - pixelShift.xShift;
    regionTrans.yMin = regionInput.yMin - regionVertDelta - pixelShift.yShift;

    regionTrans.xMax = regionTrans.xMin + regionTransWidth - 1;
    regionTrans.yMax = regionTrans.yMin + regionTransHeight - 1;

    INT32 activeArrayWidth  = activeArray.width;
    INT32 activeArrayHeight = activeArray.height;

    // Check ROI bounds and correct it if necessary
    if ((regionTrans.xMin < 0) ||
        (regionTrans.yMin < 0) ||
        (regionTrans.xMax > activeArrayWidth) ||
        (regionTrans.yMax > activeArrayHeight))
    {
        if (regionTrans.xMin < 0)
        {
            regionTrans.xMin = 0;
        }
        if (regionTrans.yMin < 0)
        {
            regionTrans.yMin = 0;
        }
        if (regionTrans.xMax >= activeArrayWidth)
        {
            regionTrans.xMax = activeArrayWidth - 1;
        }
        if (regionTrans.yMax >= activeArrayHeight)
        {
            regionTrans.yMax = activeArrayHeight - 1;
        }
    }

    return regionTrans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::FillOfflinePipelineInputMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult DualFovController::FillOfflinePipelineInputMetadata(
    multicam_result_metadata_t* pMultiCamResultMetadata,
    VOID*                       pOfflineMetadata,
    BOOL                        isSnapshotPipeline)
{
    // Populate the vendor tag for struct InputMetadataSAT
    CDKResult result = CDKResultSuccess;

    camera_metadata_t* pMetadataWide = NULL;
    camera_metadata_t* pMetadataTele = NULL;

    // Identify wide and tele metadata
    MultiCameraIdRole MultiCameraRole;
    result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMultiCamResultMetadata->ppResultMetadata[0],
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole));

    if (CDKResultSuccess == result)
    {
        if (CameraRoleTypeWide == MultiCameraRole.currentCameraRole)
        {
            pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[0]);
        }
        else
        {
            pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[0]);
        }
    }

    if (1 < pMultiCamResultMetadata->numResults)
    {
        result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMultiCamResultMetadata->ppResultMetadata[1],
            MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole));

        if (CDKResultSuccess == result)
        {
            if (CameraRoleTypeWide == MultiCameraRole.currentCameraRole)
            {
                pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[1]);
            }
            else
            {
                pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[1]);
            }
        }
    }

    InputMetadataOpticalZoom metadataOpticalZoom;
    ChxUtils::Memset(&metadataOpticalZoom, 0, sizeof(InputMetadataOpticalZoom));

    m_pLock->Lock();
    pixel_shift_t spatialShift  = m_pixelShiftWideSnapshot;
    m_pLock->Unlock();

    // Read the master camera info
    INT32 isWideMaster = 0;

    if (NULL != pMetadataWide)
    {
        MultiCamControllerManager::s_vendorTagOps.pGetMetaData(
            pMetadataWide,
            MultiCamControllerManager::m_vendorTagMasterInfo,
            &isWideMaster,
            sizeof(INT32));
    }
    else if(NULL != pMetadataTele)
    {
        INT32 isTeleMaster = 0;

        MultiCamControllerManager::s_vendorTagOps.pGetMetaData(
            pMetadataTele,
            MultiCamControllerManager::m_vendorTagMasterInfo,
            &isTeleMaster,
            sizeof(INT32));

        isWideMaster = isTeleMaster ? 0 : 1;
    }

    if (NULL != pMetadataWide)
    {
        ExtractCameraMetadata(pMetadataWide, &metadataOpticalZoom.cameraMetadata[0]);

        metadataOpticalZoom.cameraMetadata[0].isValid          = TRUE;
        metadataOpticalZoom.cameraMetadata[0].cameraRole       = CameraRoleTypeWide;
        metadataOpticalZoom.cameraMetadata[0].masterCameraRole =
            isWideMaster ? CameraRoleTypeWide : CameraRoleTypeTele;
        metadataOpticalZoom.cameraMetadata[0].fovRectIFE       = m_fovRectIFEWide;
        metadataOpticalZoom.cameraMetadata[0].fovRectIPE       = m_fovRectIFEWide;
        metadataOpticalZoom.cameraMetadata[0].activeArraySize  = m_activeArraySizeWide;

        CaptureRequestCropRegions cropRegions;
        result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadataWide,
            MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions,
            sizeof(CaptureRequestCropRegions));

        if (CDKResultSuccess == result)
        {
            metadataOpticalZoom.cameraMetadata[0].userCropRegion = cropRegions.userCropRegion;
            metadataOpticalZoom.cameraMetadata[0].pipelineCropRegion = cropRegions.pipelineCropRegion;
            metadataOpticalZoom.cameraMetadata[0].ifeLimitCropRegion = cropRegions.ifeLimitCropRegion;
            CHX_LOG("wide cropregion:%d,%d,%d,%d", cropRegions.pipelineCropRegion.left, cropRegions.pipelineCropRegion.top,
                cropRegions.pipelineCropRegion.width, cropRegions.pipelineCropRegion.height);

            if (TRUE == isSnapshotPipeline)
            {
                translated_zoom_t translatedZoom;
                m_pZoomTranslator->GetTranslatedZoom(&cropRegions.userCropRegion, &translatedZoom);
                zoom_regions_t zoomSnapshot = translatedZoom.zoomSnapshot;

                metadataOpticalZoom.cameraMetadata[0].userCropRegion = cropRegions.userCropRegion;
                metadataOpticalZoom.cameraMetadata[0].pipelineCropRegion = zoomSnapshot.zoomWideTotal;
                metadataOpticalZoom.cameraMetadata[0].ifeLimitCropRegion = zoomSnapshot.zoomWideIsp;

                cropRegions.pipelineCropRegion = zoomSnapshot.zoomWideTotal;
                cropRegions.ifeLimitCropRegion = zoomSnapshot.zoomWideIsp;
                MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataWide,
                    MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));

            }
        }
    }

    if (NULL != pMetadataTele)
    {
        ExtractCameraMetadata(pMetadataTele, &metadataOpticalZoom.cameraMetadata[1]);

        metadataOpticalZoom.cameraMetadata[1].isValid          = TRUE;
        metadataOpticalZoom.cameraMetadata[1].cameraRole       = CameraRoleTypeTele;
        metadataOpticalZoom.cameraMetadata[1].masterCameraRole =
            isWideMaster ? CameraRoleTypeWide : CameraRoleTypeTele;;
        metadataOpticalZoom.cameraMetadata[1].fovRectIFE       = m_fovRectIFETele;
        metadataOpticalZoom.cameraMetadata[1].fovRectIPE       = m_fovRectIFETele;
        metadataOpticalZoom.cameraMetadata[1].activeArraySize  = m_activeArraySizeTele;

        CaptureRequestCropRegions cropRegions;
        result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadataTele,
            MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions,
            sizeof(CaptureRequestCropRegions));
        CHX_LOG("tele cropregion:%d,%d,%d,%d", cropRegions.pipelineCropRegion.left, cropRegions.pipelineCropRegion.top,
                cropRegions.pipelineCropRegion.width, cropRegions.pipelineCropRegion.height);
        if (CDKResultSuccess == result)
        {
            metadataOpticalZoom.cameraMetadata[1].userCropRegion = cropRegions.userCropRegion;
            metadataOpticalZoom.cameraMetadata[1].pipelineCropRegion = cropRegions.pipelineCropRegion;
            metadataOpticalZoom.cameraMetadata[1].ifeLimitCropRegion = cropRegions.ifeLimitCropRegion;

            if (TRUE == isSnapshotPipeline)
            {
                translated_zoom_t translatedZoom;
                m_pZoomTranslator->GetTranslatedZoom(&cropRegions.userCropRegion, &translatedZoom);
                zoom_regions_t zoomSnapshot = translatedZoom.zoomSnapshot;

                metadataOpticalZoom.cameraMetadata[1].userCropRegion = cropRegions.userCropRegion;
                metadataOpticalZoom.cameraMetadata[1].pipelineCropRegion = zoomSnapshot.zoomTeleTotal;
                metadataOpticalZoom.cameraMetadata[1].ifeLimitCropRegion = zoomSnapshot.zoomTeleIsp;

                cropRegions.pipelineCropRegion = zoomSnapshot.zoomTeleTotal;
                cropRegions.ifeLimitCropRegion = zoomSnapshot.zoomTeleIsp;
                MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataWide,
                    MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));
            }
        }
    }

    metadataOpticalZoom.frameId   = pMultiCamResultMetadata->frameNum;
    metadataOpticalZoom.numInputs = pMultiCamResultMetadata->numResults;
    metadataOpticalZoom.outputShiftSnapshot.horizonalShift = spatialShift.xShift;
    metadataOpticalZoom.outputShiftSnapshot.verticalShift  = spatialShift.yShift;

    // Copy the master result metadata first and then add the vendor tag data
    camera_metadata_t* pMasterMetadata = NULL;
    if (TRUE == isWideMaster)
    {
        pMasterMetadata = pMetadataWide;
    }
    else
    {
        pMasterMetadata = pMetadataTele;
    }

    CHX_LOG("isWideMaster %d pMetadataWide %p pMetadataTele %p", isWideMaster, pMetadataWide, pMetadataTele);

    ChxUtils::ResetMetadata(static_cast<camera_metadata_t*>(pOfflineMetadata));

    if (CDKResultSuccess != append_camera_metadata(static_cast<camera_metadata_t*>(pOfflineMetadata), pMasterMetadata))
    {
        CHX_LOG("Failure in append meta, dst meta size %zu src meta size %zu",
            get_camera_metadata_size(static_cast<camera_metadata_t*>(pOfflineMetadata)),
            get_camera_metadata_size(static_cast<camera_metadata_t*>(pMasterMetadata)));
    }

    if (CDKResultSuccess != MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pOfflineMetadata,
        MultiCamControllerManager::m_vendorTagOpticalZoomInputMeta, &metadataOpticalZoom, sizeof(InputMetadataOpticalZoom)))
    {
        CHX_LOG("Failure in pSetMetaData of m_vendorTagOpticalZoomInputMeta");
    }

    if (CDKResultSuccess != MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pOfflineMetadata,
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole)))
    {
        CHX_LOG("Failure in pSetMetaData m_vendorTagMultiCameraRole");
    }

    if (TRUE == isWideMaster)
    {
        ChxUtils::FillCameraId(const_cast<VOID*>(reinterpret_cast<const VOID*>(pOfflineMetadata)),
            m_camIdWide);
    }
    else
    {
        ChxUtils::FillCameraId(const_cast<VOID*>(reinterpret_cast<const VOID*>(pOfflineMetadata)),
            m_camIdTele);
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DualFovController::UpdateScalerCropForSnapshot
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID DualFovController::UpdateScalerCropForSnapshot(
    VOID* pMetadata)
{
    CDKResult result = CDKResultSuccess;
    // Check if the metadata belongs to wide or tele
    BOOL isWideCamMetadata = TRUE;
    MultiCameraIdRole MultiCameraRole;
    result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole));

    if (CDKResultSuccess == result)
    {
        isWideCamMetadata = (CameraRoleTypeWide == MultiCameraRole.currentCameraRole) ? TRUE : FALSE;
    }

    CaptureRequestCropRegions cropRegions;
    result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadata,
        MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));
    if (CDKResultSuccess == result)
    {
        translated_zoom_t translatedZoom;
        m_pZoomTranslator->GetTranslatedZoom(&cropRegions.userCropRegion, &translatedZoom);
        zoom_regions_t zoomSnapshot = translatedZoom.zoomSnapshot;

        CHIRECT zoomTotal;
        CHIRECT zoomIspLimit;
        CHIRECT activeArraySize;
        if (TRUE == isWideCamMetadata)
        {
            zoomTotal       = zoomSnapshot.zoomWideTotal;
            zoomIspLimit    = zoomSnapshot.zoomWideIsp;
            activeArraySize = m_activeArraySizeWide;
        }
        else
        {
            zoomTotal       = zoomSnapshot.zoomTeleTotal;
            zoomIspLimit    = zoomSnapshot.zoomTeleIsp;
            activeArraySize = m_activeArraySizeTele;
        }

        /* Update the vendor tag for the CropRegions containing
        user crop, pipeline crop and IFE crop limit */

        // Set the snapshot crop from zoom translator
        // Use the spatial shift to move the crop region
        INT32 updatedLeft = m_pixelShiftWideSnapshot.xShift + zoomTotal.left;
        INT32 updatedTop  = m_pixelShiftWideSnapshot.yShift + zoomTotal.top;

        if ((updatedLeft >= 0) &&
            ((updatedLeft + zoomTotal.width) < activeArraySize.width) &&
            (updatedTop >= 0)  &&
            ((updatedTop + zoomTotal.height) < activeArraySize.height))
        {
            zoomTotal.left = updatedLeft;
            zoomTotal.top  = updatedTop;
            cropRegions.pipelineCropRegion = zoomTotal;
        }

        updatedLeft = m_pixelShiftWideSnapshot.xShift + zoomIspLimit.left;
        updatedTop  = m_pixelShiftWideSnapshot.yShift + zoomIspLimit.top;

        if ((updatedLeft >= 0) &&
            ((updatedLeft + zoomIspLimit.width) < activeArraySize.width) &&
            (updatedTop >= 0)  &&
            ((updatedTop + zoomIspLimit.height) < activeArraySize.height))
        {
            zoomIspLimit.left = updatedLeft;
            zoomIspLimit.top  = updatedTop;
            cropRegions.ifeLimitCropRegion = zoomIspLimit;
        }

        MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
            MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));

        // Update the scaler crop region
        camera_metadata_entry_t entryCropRegion = { 0 };
        if (0 == find_camera_metadata_entry(static_cast<camera_metadata_t*>(pMetadata),
                ANDROID_SCALER_CROP_REGION, &entryCropRegion))
        {
            CHIRECT cropRect;
            cropRect = {zoomTotal.left, zoomTotal.top, zoomTotal.width, zoomTotal.height};
            update_camera_metadata_entry(static_cast<camera_metadata_t*>(pMetadata),
                entryCropRegion.index, &cropRect, sizeof(cropRect), NULL);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::~RTBController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RTBController::~RTBController()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID RTBController::Destroy()
{
    if (NULL != m_pLock)
    {
        m_pLock->Destroy();
        m_pLock = NULL;
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RTBController* RTBController::Create(
    mcc_create_data_t* pMccCreateData)
{
    CHX_ASSERT(DualCamCount == pMccCreateData->numBundledCameras);

    RTBController* pRTBController = CHX_NEW RTBController;

    if (NULL != pRTBController)
    {
        pRTBController->m_controllerType = CONTROLLER_TYPE_RTB;

        CDKResult result = CDKResultSuccess;

        pRTBController->m_pLock = Mutex::Create();
        if (NULL == pRTBController->m_pLock)
        {
            result = CDKResultENoMemory;
        }

        if (result == CDKResultSuccess)
        {
            // Parse the camera info to build the controller
            cam_info_t* pCamInfoMain = pMccCreateData->pBundledCamInfo[0];
            cam_info_t* pCamInfoAux  = pMccCreateData->pBundledCamInfo[1];
            cam_info_t* pCamInfoWide = NULL;
            cam_info_t* pCamInfoTele = NULL;
            camera_info_t* pAppCameraInfoMain   = static_cast<camera_info_t*>(pCamInfoMain->pChiCamInfo->pLegacy);
            camera_info_t* pAppCameraInfoAux    = static_cast<camera_info_t*>(pCamInfoAux->pChiCamInfo->pLegacy);

            if (pCamInfoMain->pChiCamInfo->lensCaps.focalLength <
                    pCamInfoAux->pChiCamInfo->lensCaps.focalLength)
            {
                pRTBController->m_isMainCamFovWider = TRUE;
                pCamInfoWide = pCamInfoMain;
                pCamInfoTele = pCamInfoAux;
            }
            else
            {
                pRTBController->m_isMainCamFovWider = FALSE;
                pCamInfoWide = pCamInfoAux;
                pCamInfoTele = pCamInfoMain;
            }

            CHX_LOG("Main focal %f, Aux focal %f, Main camera id %d, Aux camera id %d",
                pCamInfoMain->pChiCamInfo->lensCaps.focalLength,
                pCamInfoAux->pChiCamInfo->lensCaps.focalLength,
                pCamInfoMain->camId, pCamInfoTele->camId);

            CHX_LOG("Main facing %d Aux facing %d",
                pAppCameraInfoMain->facing, pAppCameraInfoAux->facing);

            // Save wide, tele and logical camera ids
            pRTBController->m_camIdWide    = pCamInfoWide->camId;
            pRTBController->m_camIdTele    = pCamInfoTele->camId;
            pRTBController->m_camIdLogical = pMccCreateData->logicalCameraId;

            pRTBController->m_activeArraySizeWide = pCamInfoWide->pChiCamInfo->sensorCaps.activeArray;
            pRTBController->m_activeArraySizeTele = pCamInfoTele->pChiCamInfo->sensorCaps.activeArray;
            pRTBController->m_fovRectIFEWide      = pCamInfoWide->fovRectIFE;
            pRTBController->m_fovRectIFETele      = pCamInfoTele->fovRectIFE;

            pRTBController->m_pRawOTPData         = pCamInfoWide->pChiCamInfo->sensorCaps.pRawOTPData;
            pRTBController->m_rawOTPDataSize      = pCamInfoWide->pChiCamInfo->sensorCaps.rawOTPDataSize;

            // Calculate the FOV ratio between the two cameras
            FLOAT focalLengthWide = pCamInfoWide->pChiCamInfo->lensCaps.focalLength;
            FLOAT focalLengthTele = pCamInfoTele->pChiCamInfo->lensCaps.focalLength;
            FLOAT pixelPitchWide  = pCamInfoWide->pChiCamInfo->sensorCaps.pixelSize;
            FLOAT pixelPitchTele  = pCamInfoTele->pChiCamInfo->sensorCaps.pixelSize;
            dimension_t sensorDimensionWide = pCamInfoWide->sensorOutDimension;
            dimension_t sensorDimensionTele = pCamInfoTele->sensorOutDimension;

            FLOAT fovWide = sensorDimensionWide.width * pixelPitchWide / focalLengthWide;
            FLOAT fovTele = sensorDimensionTele.width * pixelPitchTele / focalLengthTele;

            if (fovTele > 0.0f)
            {
                pRTBController->m_adjustedFovRatio = fovWide / fovTele;
            }

            pRTBController->SetInitialResultState();
        }
        else
        {
            // Destroy the object in case of failure
            pRTBController->Destroy();
            pRTBController = NULL;
        }
    }

    return pRTBController;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::TranslateRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult RTBController::TranslateRequestSettings(
    multicam_req_settings* pMultiCamSettings)
{
    CDKResult result = CDKResultEFailed;

    if (NULL != pMultiCamSettings)
    {
        CHX_ASSERT(DualCamCount == pMultiCamSettings->numSettingsBuffers);

        camera_metadata_t* pMetadataWide  = NULL;
        camera_metadata_t* pMetadataTele  = NULL;

        if (TRUE == m_isMainCamFovWider)
        {
            pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[0]);
            pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[1]);
        }
        else
        {
            pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[1]);
            pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[0]);
        }

        if ((NULL != pMetadataWide) && (NULL != pMetadataTele))
        {
            // Set the reference crop window for both Wide and Tele
            RefCropWindowSize refCropWindowWide, refCropWindowTele;
            UINT32 refCropSizeTag = 0;

            result = MultiCamControllerManager::s_vendorTagOps.pQueryVendorTagLocation(
                "org.quic.camera2.ref.cropsize", "RefCropSize", &refCropSizeTag);
            if (CDKResultSuccess == result)
            {
                refCropWindowWide.width = m_activeArraySizeWide.width;
                refCropWindowWide.height = m_activeArraySizeWide.height;

                refCropWindowTele.width = m_activeArraySizeTele.width;
                refCropWindowTele.height = m_activeArraySizeTele.height;

                CHX_LOG("RefCropSize Wide w:%d h:%d Tele w:%d h:%d", refCropWindowWide.width,
                        refCropWindowWide.height, refCropWindowTele.width, refCropWindowTele.height);

                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataWide, refCropSizeTag,
                    &refCropWindowWide, sizeof(refCropWindowWide));
                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataTele, refCropSizeTag,
                    &refCropWindowTele, sizeof(refCropWindowTele));
            }

            camera_metadata_entry entryCropRegionWide;
            camera_metadata_entry entryCropRegionTele;
            if ((0 == find_camera_metadata_entry(pMetadataWide,
                        ANDROID_SCALER_CROP_REGION, &entryCropRegionWide)) &&
                (0 == find_camera_metadata_entry(pMetadataTele,
                        ANDROID_SCALER_CROP_REGION, &entryCropRegionTele)))
            {
                // Update the metadata
                update_camera_metadata_entry(pMetadataWide, entryCropRegionWide.index,
                    &m_activeArraySizeWide, sizeof(m_activeArraySizeWide), NULL);

                update_camera_metadata_entry(pMetadataTele, entryCropRegionTele.index,
                    &m_activeArraySizeTele, sizeof(m_activeArraySizeTele), NULL);

            }
            else
            {
                CHX_LOG_WARN("No crop region information in this metadata!");
            }
            // Translate the Focus ROI
            camera_metadata_entry_t entryAFRegionWide = { 0 };
            camera_metadata_entry_t entryAFRegionTele = { 0 };

            if ((0 == find_camera_metadata_entry(pMetadataWide, ANDROID_CONTROL_AF_REGIONS, &entryAFRegionWide)) &&
                (0 == find_camera_metadata_entry(pMetadataTele, ANDROID_CONTROL_AF_REGIONS, &entryAFRegionTele)))
            {
                // AF_REGION tag tuple has 5 elements [xMin, yMin, xMax, yMax, weight]
                UINT32 tupleSize = 5;
                for (UINT32 i = 0; i < entryAFRegionTele.count / tupleSize; i++)
                {
                    WeightedRegion afRegion = { 0 };
                    UINT32 index = i * tupleSize;

                    afRegion.xMin   = entryAFRegionTele.data.i32[index];
                    afRegion.yMin   = entryAFRegionTele.data.i32[index + 1];
                    afRegion.xMax   = entryAFRegionTele.data.i32[index + 2];
                    afRegion.yMax   = entryAFRegionTele.data.i32[index + 3];
                    afRegion.weight = entryAFRegionTele.data.i32[index + 4];

                    // Get the translated AF ROI for the wide camera
                    WeightedRegion afRegionWide = TranslateMeteringRegion(&afRegion, CameraRoleTypeWide);

                    // Update the metadata
                    update_camera_metadata_entry(pMetadataWide, entryAFRegionWide.index,
                        &afRegionWide, sizeof(WeightedRegion), NULL);
                }
            }

            // Translate the Metering ROI
            camera_metadata_entry_t entryAERegionWide = { 0 };
            camera_metadata_entry_t entryAERegionTele = { 0 };

            if ((0 == find_camera_metadata_entry(pMetadataWide, ANDROID_CONTROL_AE_REGIONS, &entryAERegionWide)) &&
                (0 == find_camera_metadata_entry(pMetadataTele, ANDROID_CONTROL_AE_REGIONS, &entryAERegionTele)))
            {
                // AE_REGION tag tuple has 5 elements [xMin, yMin, xMax, yMax, weight]
                UINT32 tupleSize = 5;
                for (UINT32 i = 0; i < entryAERegionTele.count / tupleSize; i++)
                {
                    WeightedRegion aeRegion = { 0 };
                    UINT32 index = i * tupleSize;

                    aeRegion.xMin   = entryAERegionTele.data.i32[index];
                    aeRegion.yMin   = entryAERegionTele.data.i32[index + 1];
                    aeRegion.xMax   = entryAERegionTele.data.i32[index + 2];
                    aeRegion.yMax   = entryAERegionTele.data.i32[index + 3];
                    aeRegion.weight = entryAERegionTele.data.i32[index + 4];

                    // Get the translated AE ROI for the wide camera
                    WeightedRegion aeRegionWide = TranslateMeteringRegion(&aeRegion, CameraRoleTypeWide);

                    // Update the metadata
                    update_camera_metadata_entry(pMetadataWide, entryAERegionWide.index,
                        &aeRegionWide, sizeof(WeightedRegion), NULL);
                }
            }

            // Send the camera role and camera id for the first frame.
            if (0 == pMultiCamSettings->frameNum)
            {
                // Set the vendor tag for the MultiCameraIdRole
                UINT32 tag = 0;
                CDKResult result = MultiCamControllerManager::s_vendorTagOps.pQueryVendorTagLocation(
                    "com.qti.chi.multicamerainfo", "MultiCameraIdRole", &tag);

                if (CDKResultSuccess == result)
                {
                    // Get master camera of this request
                    CameraRoleType masterCameraRole = pMultiCamSettings->currentRequestMCCResult.masterCameraRole;

                    // Set the MultiCameraIdRole for the main camera
                    MultiCameraIdRole inputMetadata;
                    inputMetadata.currentCameraId   = m_camIdTele;
                    inputMetadata.currentCameraRole = CameraRoleTypeTele;
                    inputMetadata.logicalCameraId   = m_camIdLogical;
                    inputMetadata.masterCameraRole  = masterCameraRole;
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataTele, tag,
                        &inputMetadata, sizeof(MultiCameraIdRole));
                    CHX_LOG("m_camIdTele %d Camera role %d", m_camIdTele, inputMetadata.currentCameraRole);

                    // Set the MultiCameraIdRole for the aux camera
                    inputMetadata.currentCameraId   = m_camIdWide;
                    inputMetadata.currentCameraRole = CameraRoleTypeWide;
                    inputMetadata.logicalCameraId   = m_camIdLogical;
                    inputMetadata.masterCameraRole  = masterCameraRole;
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataWide, tag,
                        &inputMetadata, sizeof(MultiCameraIdRole));
                    CHX_LOG("m_camIdWide %d Camera role %d", m_camIdWide, inputMetadata.currentCameraRole);
                }
            }

            // Add master camera info
            BOOL isMaster = FALSE;
            CameraRoleType masterCameraRole = pMultiCamSettings->currentRequestMCCResult.masterCameraRole;

            isMaster = (CameraRoleTypeTele == masterCameraRole) ? TRUE : FALSE;
            result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataTele,
                MultiCamControllerManager::m_vendorTagMasterInfo, &isMaster, sizeof(BOOL));

            isMaster = (CameraRoleTypeWide == masterCameraRole) ? TRUE : FALSE;
            result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadataWide,
                MultiCamControllerManager::m_vendorTagMasterInfo, &isMaster, sizeof(BOOL));
            CHX_LOG ("masterCameraRole %d", masterCameraRole);

            // Update all the common vendor tags
            UpdateVendorTags(pMultiCamSettings);

            result = CDKResultSuccess;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::SetInitialResultState
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID RTBController::SetInitialResultState()
{
    // Set the active camera state
    m_result.activeCameras[0].cameraId = m_camIdWide;
    m_result.activeCameras[1].cameraId = m_camIdTele;
    m_result.activeCameras[0].isActive = TRUE;
    m_result.activeCameras[1].isActive = TRUE;

    // Set the master camera info
    m_result.masterCameraId   = m_camIdTele;
    m_result.masterCameraRole = CameraRoleTypeTele;
    m_recommendedMasterCameraRole = CameraRoleTypeTele;

    m_result.snapshotFusion = TRUE;
    m_result.isValid        = TRUE;

    CHX_LOG("Active cameras Wide[%d], Tele[%d]", m_result.activeCameras[0].isActive, m_result.activeCameras[1].isActive);
    CHX_LOG("Master camera id: %d", m_result.masterCameraId);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::ProcessResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID RTBController::ProcessResultMetadata(
    VOID* pResultMetadata)
{
    OutputMetadataBokeh metadataBokeh;

    CDKResult result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pResultMetadata,
        MultiCamControllerManager::m_vendorTagBokehResultMeta, &metadataBokeh, sizeof(OutputMetadataBokeh));

    if (CDKResultSuccess == result)
    {
        CHX_LOG("Current master: %d Recommended master: %d", metadataBokeh.masterCamera, metadataBokeh.recommendedMasterCamera);

        m_pLock->Lock();

        // Update the master camera info
        m_result.masterCameraId   = (CameraRoleTypeWide == metadataBokeh.masterCamera) ? m_camIdWide : m_camIdTele;
        m_result.masterCameraRole = metadataBokeh.masterCamera;

        m_recommendedMasterCameraRole = metadataBokeh.recommendedMasterCamera;

        m_pLock->Unlock();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::GetResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
controller_result_t RTBController::GetResult()
{
    m_pLock->Lock();
    controller_result_t result = m_result;
    m_pLock->Unlock();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::TranslateMeteringRegion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WeightedRegion RTBController::TranslateMeteringRegion(
    WeightedRegion* pRegion,
    CameraRoleType  camRole)
{
    FLOAT fovRatio = 1.0f;

    WeightedRegion    regionInput = *pRegion;
    WeightedRegion    regionTrans = regionInput;
    CHIRECT           activeArray = { 0 };

    // Acquire mutex to read spatial alignment shifts which are written by other thread
    m_pLock->Lock();
    if (CameraRoleTypeWide == camRole)
    {
        fovRatio    = 1.0f / m_adjustedFovRatio;
        activeArray = m_activeArraySizeWide;
    }
    else
    {
        fovRatio    = m_adjustedFovRatio;
        activeArray = m_activeArraySizeTele;
    }
    m_pLock->Unlock();

    // ROI should be with respect to the corresponding active array
    FLOAT xScale = ((FLOAT)activeArray.width  / m_activeArraySizeTele.width);
    FLOAT yScale = ((FLOAT)activeArray.height / m_activeArraySizeTele.height);

    regionInput.xMin *= xScale;
    regionInput.yMin *= yScale;
    regionInput.xMax *= xScale;
    regionInput.yMax *= yScale;

    INT32 regionWidth       = regionInput.xMax - regionInput.xMin + 1;
    INT32 regionHeight      = regionInput.yMax - regionInput.yMin + 1;
    INT32 regionTransWidth  = regionWidth * fovRatio;
    INT32 regionTransHeight = regionHeight * fovRatio;

    INT32 regionHorzDelta = (regionTransWidth - regionWidth) / 2;
    INT32 regionVertDelta = (regionTransHeight - regionHeight) / 2;

    regionTrans.xMin = regionInput.xMin - regionHorzDelta;
    regionTrans.yMin = regionInput.yMin - regionVertDelta;

    regionTrans.xMax = regionTrans.xMin + regionTransWidth - 1;
    regionTrans.yMax = regionTrans.yMin + regionTransHeight - 1;

    INT32 activeArrayWidth  = activeArray.width;
    INT32 activeArrayHeight = activeArray.height;

    // Check ROI bounds and correct it if necessary
    if ((regionTrans.xMin < 0) ||
        (regionTrans.yMin < 0) ||
        (regionTrans.xMax > activeArrayWidth) ||
        (regionTrans.yMax > activeArrayHeight))
    {
        if (regionTrans.xMin < 0)
        {
            regionTrans.xMin = 0;
        }
        if (regionTrans.yMin < 0)
        {
            regionTrans.yMin = 0;
        }
        if (regionTrans.xMax >= activeArrayWidth)
        {
            regionTrans.xMax = activeArrayWidth - 1;
        }
        if (regionTrans.yMax >= activeArrayHeight)
        {
            regionTrans.yMax = activeArrayHeight - 1;
        }
    }

    return regionTrans;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::FillOfflinePipelineInputMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult RTBController::FillOfflinePipelineInputMetadata(
    multicam_result_metadata_t* pMultiCamResultMetadata,
    VOID*                       pOfflineMetadata,
    BOOL                        isSnapshotPipeline)
{
    (VOID)isSnapshotPipeline;
    // Populate the vendor tag for struct InputMetadataBokeh
    CDKResult result = CDKResultSuccess;

    camera_metadata_t* pMetadataWide = NULL;
    camera_metadata_t* pMetadataTele = NULL;

    // Identify wide and tele metadata
    MultiCameraIdRole MultiCameraRole;
    result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMultiCamResultMetadata->ppResultMetadata[0],
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole));

    if (CDKResultSuccess == result)
    {
        if (CameraRoleTypeWide == MultiCameraRole.currentCameraRole)
        {
            pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[0]);
        }
        else
        {
            pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[0]);
        }
    }

    result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMultiCamResultMetadata->ppResultMetadata[1],
        MultiCamControllerManager::m_vendorTagMultiCameraRole, &MultiCameraRole, sizeof(MultiCameraIdRole));

    if (CDKResultSuccess == result)
    {
        if (CameraRoleTypeWide == MultiCameraRole.currentCameraRole)
        {
            pMetadataWide = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[1]);
        }
        else
        {
            pMetadataTele = static_cast<camera_metadata_t*>(pMultiCamResultMetadata->ppResultMetadata[1]);
        }
    }

    CHX_ASSERT((NULL != pMetadataWide) && (NULL != pMetadataTele));

    InputMetadataBokeh metadataBokeh;
    ChxUtils::Memset(&metadataBokeh, 0, sizeof(InputMetadataBokeh));

    m_pLock->Lock();
    CameraRoleType masterCamera = m_recommendedMasterCameraRole;
    m_pLock->Unlock();

    // Read the master camera info to check if wide masterinfo is true else tele is master
    INT32 isWideMaster = 0;

    if (NULL != pMetadataWide)
    {
        MultiCamControllerManager::s_vendorTagOps.pGetMetaData(
            pMetadataWide,
            MultiCamControllerManager::m_vendorTagMasterInfo,
            &isWideMaster,
            sizeof(INT32));
    }

    if (NULL != pMetadataWide)
    {
        ExtractCameraMetadata(pMetadataWide, &metadataBokeh.cameraMetadata[0]);

        metadataBokeh.cameraMetadata[0].isValid          = TRUE;
        metadataBokeh.cameraMetadata[0].cameraRole       = CameraRoleTypeWide;
        metadataBokeh.cameraMetadata[0].masterCameraRole = masterCamera;
        metadataBokeh.cameraMetadata[0].fovRectIFE       = m_fovRectIFEWide;
        metadataBokeh.cameraMetadata[0].fovRectIPE       = m_fovRectIFEWide;
        metadataBokeh.cameraMetadata[0].activeArraySize  = m_activeArraySizeWide;

        CaptureRequestCropRegions cropRegions;
        result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadataWide,
            MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));

        if (CDKResultSuccess == result)
        {
            metadataBokeh.cameraMetadata[0].userCropRegion     = cropRegions.userCropRegion;
            metadataBokeh.cameraMetadata[0].pipelineCropRegion = cropRegions.pipelineCropRegion;
            metadataBokeh.cameraMetadata[0].ifeLimitCropRegion = cropRegions.ifeLimitCropRegion;
        }
    }

    if (NULL != pMetadataTele)
    {
        ExtractCameraMetadata(pMetadataTele, &metadataBokeh.cameraMetadata[1]);

        metadataBokeh.cameraMetadata[1].isValid          = TRUE;
        metadataBokeh.cameraMetadata[1].cameraRole       = CameraRoleTypeTele;
        metadataBokeh.cameraMetadata[1].masterCameraRole = masterCamera;
        metadataBokeh.cameraMetadata[1].fovRectIFE       = m_fovRectIFETele;
        metadataBokeh.cameraMetadata[1].fovRectIPE       = m_fovRectIFETele;
        metadataBokeh.cameraMetadata[1].activeArraySize  = m_activeArraySizeTele;

        CaptureRequestCropRegions cropRegions;
        result = MultiCamControllerManager::s_vendorTagOps.pGetMetaData(pMetadataTele,
            MultiCamControllerManager::m_vendorTagCropRegions, &cropRegions, sizeof(CaptureRequestCropRegions));

        if (CDKResultSuccess == result)
        {
            metadataBokeh.cameraMetadata[1].userCropRegion     = cropRegions.userCropRegion;
            metadataBokeh.cameraMetadata[1].pipelineCropRegion = cropRegions.pipelineCropRegion;
            metadataBokeh.cameraMetadata[1].ifeLimitCropRegion = cropRegions.ifeLimitCropRegion;
        }
    }

    if (FALSE == m_isMainCamFovWider)
    {
        // Swap the camera meta data if Aux Camera is wider
        CameraMetadata tempMeta;
        tempMeta = metadataBokeh.cameraMetadata[0];
        metadataBokeh.cameraMetadata[0] = metadataBokeh.cameraMetadata[1];
        metadataBokeh.cameraMetadata[1] = tempMeta;
    }

    metadataBokeh.frameId        = pMultiCamResultMetadata->frameNum;
    metadataBokeh.blurLevel      = 0;
    metadataBokeh.blurMinValue   = 0;
    metadataBokeh.blurMaxValue   = 0;
    // Copy the master result metadata first and then add the vendor tag data
    camera_metadata_t* pMasterMetadata = NULL;
    if (TRUE == isWideMaster)
    {
        pMasterMetadata = pMetadataWide;
    }
    else
    {
        pMasterMetadata = pMetadataTele;
    }

    CHX_LOG("isWideMaster %d pMetadataWide %p pMetadataTele %p", isWideMaster, pMetadataWide, pMetadataTele);

    ChxUtils::ResetMetadata(static_cast<camera_metadata_t*>(pOfflineMetadata));

    if (CDKResultSuccess != ChxUtils::MergeMetadata(pMasterMetadata, pOfflineMetadata))
    {
        CHX_LOG("Failure in append meta, dst meta size %zu src meta size %zu",
            get_camera_metadata_size(static_cast<camera_metadata_t*>(pOfflineMetadata)),
            get_camera_metadata_size(static_cast<camera_metadata_t*>(pMasterMetadata)));
    }

    if (CDKResultSuccess != MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pOfflineMetadata,
        MultiCamControllerManager::m_vendorTagBokehInputMeta, &metadataBokeh, sizeof(InputMetadataBokeh)))
    {
        CHX_LOG("Failure in pSetMetaData of m_vendorTagBokehInputMeta");
    }

    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RTBController::UpdateScalerCropForSnapshot
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID RTBController::UpdateScalerCropForSnapshot(
    VOID* pMetadata)
{
    (VOID)pMetadata;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::~BayerMonoController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BayerMonoController::~BayerMonoController()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID BayerMonoController::Destroy()
{
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BayerMonoController* BayerMonoController::Create(
    mcc_create_data_t* pMccCreateData)
{
    (VOID)pMccCreateData;

    BayerMonoController* pBayerMonoController = CHX_NEW BayerMonoController;

    CHX_ASSERT(pBayerMonoController);
    pBayerMonoController->m_controllerType = CONTROLLER_TYPE_BAYERMONO;

    // Parse the camera info to build the controller.

    return pBayerMonoController;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::Reconfigure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BayerMonoController::Reconfigure(
    mcc_create_data_t* pMccCreateData)
{
    (VOID)pMccCreateData;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::ConsolidateCameraInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BayerMonoController::ConsolidateCameraInfo(
    UINT32            numBundledCameras,
    CHICAMERAINFO**   ppCamInfo,
    CHICAMERAINFO*    pConsolidatedCamInfo)
{
    (VOID)numBundledCameras;
    CDKResult result = CDKResultSuccess;

    // Consolidate the camera info with BayerMono controller logic
    ChxUtils::Memcpy(pConsolidatedCamInfo, ppCamInfo[0], sizeof(CHICAMERAINFO));

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::TranslateRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BayerMonoController::TranslateRequestSettings(
    multicam_req_settings* pMultiCamSettings)
{
    (VOID)pMultiCamSettings;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BayerMonoController::ProcessResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID BayerMonoController::ProcessResultMetadata(
    VOID* pResultMetadata)
{
    (VOID)pResultMetadata;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::~VRController
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VRController::~VRController()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID VRController::Destroy()
{
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VRController* VRController::Create(
    mcc_create_data_t* pMccCreateData)
{
    (VOID)pMccCreateData;

    VRController* pVRController = CHX_NEW VRController;
    cam_info_t* pCamInfoMainVR  = pMccCreateData->pBundledCamInfo[0];
    cam_info_t* pCamInfoSlaveVR = pMccCreateData->pBundledCamInfo[1];

    CHX_ASSERT(pVRController);
    if (pVRController != NULL)
    {
        pVRController->m_controllerType = CONTROLLER_TYPE_VR;
        pVRController->m_camIdVRMaster = pCamInfoMainVR->camId;
        pVRController->m_camIdVRMaster  = pCamInfoSlaveVR->camId;
        pVRController->m_recommendedMasterCameraRole = CameraRoleTypeVR;
    }
    return pVRController;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::Reconfigure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult VRController::Reconfigure(
    mcc_create_data_t* pMccCreateData)
{
    (VOID)pMccCreateData;
    return CDKResultSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::ConsolidateCameraInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult VRController::ConsolidateCameraInfo(
    UINT32            numBundledCameras,
    CHICAMERAINFO**   ppCamInfo,
    CHICAMERAINFO*    pConsolidatedCamInfo)
{
    CHX_ASSERT(NULL != ppCamInfo);
    CHX_ASSERT(NULL != pConsolidatedCamInfo);
    CHX_ASSERT(DualCamCount == numBundledCameras);
    CHX_ASSERT(NULL != ppCamInfo[0]);
    CHX_ASSERT(NULL != ppCamInfo[1]);
    CDKResult result = CDKResultSuccess;
    static const INT supportedCustomRes   = 2;
    static const INT availConfigTupleSize = 4;
    static const INT availConfigInfo      = supportedCustomRes * 12;
    // Make sure that this array is always sorted in descending order
    // while updating it
    static const INT32 customSupportedSize[availConfigInfo][availConfigTupleSize] = {
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 5760, 2880, 0 },
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 5760, 2880, 1 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 5760, 2880, 0 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 5760, 2880, 1 },
        { HAL_PIXEL_FORMAT_BLOB, 5760, 2880, 0 },
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 4096, 2048, 0 },
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 4096, 2048, 1 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 4096, 2048, 0 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 4096, 2048, 1 },
        { HAL_PIXEL_FORMAT_BLOB, 4096, 2048, 0 },
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 1920, 1080, 0 },
        { HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, 1920, 1080, 1 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 1920, 1080, 0 },
        { HAL_PIXEL_FORMAT_YCbCr_420_888, 1920, 1080, 1 },
        { HAL_PIXEL_FORMAT_BLOB, 3840, 2160, 0 },
        { HAL_PIXEL_FORMAT_BLOB, 3264, 2448, 0 },
        { HAL_PIXEL_FORMAT_RAW10, 8112, 3040, 0 },
        { HAL_PIXEL_FORMAT_RAW10, 8112, 3040, 1 },
        { HAL_PIXEL_FORMAT_RAW12, 8112, 3040, 0 },
        { HAL_PIXEL_FORMAT_RAW12, 8112, 3040, 1 },
        { HAL_PIXEL_FORMAT_RAW16, 8112, 3040, 0 },
        { HAL_PIXEL_FORMAT_RAW16, 8112, 3040, 1 },
        { HAL_PIXEL_FORMAT_RAW_OPAQUE, 8112, 3040, 0 },
        { HAL_PIXEL_FORMAT_RAW_OPAQUE, 8112, 3040, 1 },
    };

    camera_info_t* pCameraInfoMain = static_cast<camera_info_t*>(ppCamInfo[0]->pLegacy);
    const camera_metadata_t* pMetadataMain = pCameraInfoMain->static_camera_characteristics;

    camera_info_t* pCameraInfoAux = static_cast<camera_info_t*>(ppCamInfo[1]->pLegacy);
    const camera_metadata_t* pMetadataAux = pCameraInfoAux->static_camera_characteristics;

    camera_info_t* pCameraInfoConsolidated = static_cast<camera_info_t*>(pConsolidatedCamInfo->pLegacy);
    const camera_metadata_t* pMetadataConsolidated = pCameraInfoConsolidated->static_camera_characteristics;

    pConsolidatedCamInfo->lensCaps       = ppCamInfo[0]->lensCaps;
    pConsolidatedCamInfo->numSensorModes = ppCamInfo[0]->numSensorModes;
    pConsolidatedCamInfo->sensorCaps     = ppCamInfo[0]->sensorCaps;
    ChxUtils::Memcpy(pConsolidatedCamInfo->pLegacy, ppCamInfo[0]->pLegacy, sizeof(camera_info_t));
    pCameraInfoConsolidated->static_camera_characteristics = pMetadataConsolidated;
    pConsolidatedCamInfo->size = ppCamInfo[0]->size;

    if ((NULL != pMetadataMain) && (NULL != pMetadataAux))
    {
        camera_metadata_entry_t entryMain = { 0 };
        camera_metadata_entry_t entryAux  = { 0 };
        camera_metadata_entry_t entryConsolidated = { 0 };
        UINT32 tupleSize = 4;
        INT ret;
        UINT32 consolidatedEntryCount = 0;

        if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryMain)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataAux),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryAux)) &&
            (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
            ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entryConsolidated)))
        {
            // 4 elements in the tuple of ANDROID_SCALER_AVAILABLE_STREAM_CONFIGURATIONS
            UINT32 tupleSize = 4;
            UINT32 numEntriesMain = entryMain.count / tupleSize;
            UINT32 numEntriesAux  = entryAux.count / tupleSize;
            INT32* pEntryData       = static_cast<INT32*>(ChxUtils::Calloc(
                (numEntriesAux + numEntriesMain) * tupleSize * sizeof(INT32)));
            if (NULL != pEntryData)
            {
                ChxUtils::Memcpy(pEntryData, entryMain.data.i32, entryMain.count * sizeof(INT32));
                UINT32 consolidatedIndex;
                for (INT i = 0; i < availConfigInfo; i++)
                {
                    consolidatedIndex = consolidatedEntryCount * tupleSize;
                    pEntryData[consolidatedIndex]     = customSupportedSize[i][0];
                    pEntryData[consolidatedIndex + 1] = customSupportedSize[i][1];
                    pEntryData[consolidatedIndex + 2] = customSupportedSize[i][2];
                    pEntryData[consolidatedIndex + 3] = customSupportedSize[i][3];
                    consolidatedEntryCount++;
                }

                entryConsolidated.count = consolidatedEntryCount * tupleSize;

                ret = update_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated), entryConsolidated.index,
                        pEntryData, entryConsolidated.count, &entryConsolidated);
                if (ret != 0)
                {
                    CHX_LOG_WARN("update availablestreamconfigure for consolidate metadata failed!");
                }
                ChxUtils::Free(pEntryData);
                pEntryData = NULL;
            }
            else
            {
                CHX_LOG_ERROR("No memory allocated for pEntryData");
                result = CDKResultENoMemory;
            }
        }
   }

   if (NULL != pMetadataConsolidated)
   {
       camera_metadata_entry_t entryConsolidated = { 0 };
       camera_metadata_entry_t entryMain = { 0 };
       INT32 maxJPEGSizeofMain = 0;
       if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
                                                      ANDROID_JPEG_MAX_SIZE, &entryMain)))
       {
           maxJPEGSizeofMain = *entryMain.data.i32;
       }
       if (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated),
                ANDROID_JPEG_MAX_SIZE, &entryConsolidated))
       {
           INT32 maxJPEGSize = 0;
           // we need to use big JPEG size for consolidate metadata
           // Calculate data as per the largest width and height
           maxJPEGSize = customSupportedSize[0][1]*customSupportedSize[0][2]*3 + sizeof(CamX::Camera3JPEGBlob);
           if (maxJPEGSize > maxJPEGSizeofMain)
           {
               if (0 != update_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataConsolidated), entryConsolidated.index,
                            &maxJPEGSize, entryConsolidated.count, &entryConsolidated))
               {
                   CHX_LOG_WARN("update MAXJPEGSize metadata for consolidate metadata failed!");
               }
           }
       }
   }
   return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::TranslateRequestSettings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult VRController::TranslateRequestSettings(
    multicam_req_settings* pMultiCamSettings)
{
    CDKResult result = CDKResultEFailed;

    if (NULL != pMultiCamSettings)
    {
        CHX_ASSERT(DualCamCount == pMultiCamSettings->numSettingsBuffers);

        camera_metadata_t* pMetadata           = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[0]);
        camera_metadata_t* pTranslatedMetadata = static_cast<camera_metadata_t*>(pMultiCamSettings->ppSettings[1]);

        if ((NULL != pMetadata) && (NULL != pTranslatedMetadata))
        {
             if (0 == pMultiCamSettings->frameNum)
            {
                // Set the vendor tag for the MultiCameraIdRole
                UINT32 tag = 0;
                BOOL isSyncMode = FALSE;
                if (TRUE == pMultiCamSettings->kernelFrameSyncEnable)
                {
                    isSyncMode = TRUE;
                }
                CDKResult result = MultiCamControllerManager::s_vendorTagOps.pQueryVendorTagLocation(
                        "com.qti.chi.multicamerainfo", "MultiCameraIdRole", &tag);

                if (CDKResultSuccess == result)
                {
                    // Set the MultiCameraIdRole for the main camera
                    MultiCameraIdRole inputMetadata;
                    inputMetadata.currentCameraId   = m_camIdVRMaster;
                    inputMetadata.currentCameraRole = CameraRoleTypeVR;
                    inputMetadata.logicalCameraId   = m_camIdLogical;
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata, tag,
                        &inputMetadata, sizeof(MultiCameraIdRole));

                    // Set the MultiCameraIdRole for the aux camera
                    inputMetadata.currentCameraId   = m_camIdVRSlave;
                    inputMetadata.currentCameraRole = CameraRoleTypeVR;
                    inputMetadata.logicalCameraId   = m_camIdLogical;
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata, tag,
                        &inputMetadata, sizeof(MultiCameraIdRole));

                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata,
                        MultiCamControllerManager::m_vendorTagSyncInfo, &isSyncMode, sizeof(SyncModeInfo));
                    result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata,
                        MultiCamControllerManager::m_vendorTagSyncInfo, &isSyncMode, sizeof(SyncModeInfo));
                }
            }

            // Add the master camera info in vendor tag
            UINT32 tagMaster = 0;
            result = MultiCamControllerManager::s_vendorTagOps.pQueryVendorTagLocation(
                "com.qti.chi.multicamerainfo", "MasterCamera", &tagMaster);

            if (CDKResultSuccess == result)
            {
                // Get the master camera
                BOOL isMaster = FALSE;

                isMaster = TRUE;
                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pMetadata, tagMaster,
                    &isMaster, sizeof(BOOL));

                isMaster = FALSE;
                result = MultiCamControllerManager::s_vendorTagOps.pSetMetaData(pTranslatedMetadata, tagMaster,
                    &isMaster, sizeof(BOOL));
            }
            result = CDKResultSuccess;
        }
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// VRController::ProcessResultMetadata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID VRController::ProcessResultMetadata(
    VOID* pResultMetadata)
{
    (VOID)pResultMetadata;
}

