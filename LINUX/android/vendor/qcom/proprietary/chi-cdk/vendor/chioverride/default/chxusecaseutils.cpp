////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecaseutils.cpp
/// @brief Usecase utils class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "chxadvancedcamerausecase.h"
#include "chxsensorselectmode.h"
#include "chxusecase.h"
#include "chxusecasedefault.h"
#include "chxusecasemc.h"
#include "chxusecasequadcfa.h"
#include "chxusecasetorch.h"
#include "chxusecasevrmc.h"
#include "chxfeatureshdr.h"


UINT UsecaseSelector::NumImplDefinedFormats = 4;

ChiBufferFormat UsecaseSelector::AllowedImplDefinedFormats[] =
{
    ChiFormatYUV420NV12, ChiFormatYUV420NV21,  ChiFormatUBWCTP10,  ChiFormatUBWCNV12
};

BOOL UsecaseSelector::GPURotationUsecase    = FALSE;
BOOL UsecaseSelector::HFRNo3AUsecase        = FALSE;
UINT UsecaseSelector::VideoEISV2Usecase     = 0;
UINT UsecaseSelector::VideoEISV3Usecase     = 0;
BOOL UsecaseSelector::GPUDownscaleUsecase   = FALSE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::~UsecaseSelector
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseSelector::~UsecaseSelector()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseSelector* UsecaseSelector::Create(
    const ExtensionModule* pExtModule)
{
    CDKResult        result           = CDKResultSuccess;
    UsecaseSelector* pUsecaseSelector = new UsecaseSelector;

    pUsecaseSelector->m_pExtModule = pExtModule;

    result = pUsecaseSelector->Initialize();

    if (CDKResultSuccess != result)
    {
        pUsecaseSelector->Destroy();
        pUsecaseSelector = NULL;
    }

    return pUsecaseSelector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseSelector::Destroy()
{
    delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseSelector::Initialize()
{
    CDKResult result = CDKResultSuccess;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsPreviewStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsPreviewStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL isPreviewStream = FALSE;

    if ((CAMERA3_STREAM_OUTPUT     == pStream->stream_type) &&
        (GRALLOC_USAGE_HW_COMPOSER == (GRALLOC_USAGE_HW_COMPOSER & pStream->usage)))
    {
        isPreviewStream = TRUE;
    }

    return isPreviewStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsVideoStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsVideoStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL isVideoStream = FALSE;

    if ((NULL != pStream ) && (0 != (GRALLOC_USAGE_HW_VIDEO_ENCODER & pStream->usage)))
    {
        isVideoStream = TRUE;
    }

    return isVideoStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsYUVSnapshotStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsYUVSnapshotStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL isYUVSnapshotStream = FALSE;

    if ((CAMERA3_STREAM_OUTPUT          == pStream->stream_type)   &&
        (HAL_PIXEL_FORMAT_YCbCr_420_888 == pStream->format)        &&
        (FALSE                          == IsVideoStream(pStream)) &&
        (FALSE                          == IsPreviewStream(pStream)))
    {
        isYUVSnapshotStream = TRUE;
    }

    return isYUVSnapshotStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsYUVSnapshotStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsJPEGSnapshotStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL isJPEGSnapshotStream = FALSE;

    if ((CAMERA3_STREAM_OUTPUT       == pStream->stream_type) &&
        (HAL_PIXEL_FORMAT_BLOB       == pStream->format)      &&
        ((HAL_DATASPACE_JFIF         == pStream->data_space)  ||
         (HAL_DATASPACE_V0_JFIF      == pStream->data_space)))
    {
        isJPEGSnapshotStream = TRUE;
    }

    return isJPEGSnapshotStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsRawStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsRawStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL isRawStream = FALSE;

    if ((CAMERA3_STREAM_OUTPUT  == pStream->stream_type) &&
        ((HAL_PIXEL_FORMAT_RAW10 == pStream->format) ||
        ((HAL_PIXEL_FORMAT_RAW16 == pStream->format))))
    {
        isRawStream = TRUE;
    }

    return isRawStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsYUVOutStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsYUVOutStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL bIsYUVOutStream = FALSE;

    if ((CAMERA3_STREAM_OUTPUT == pStream->stream_type) &&
        ((HAL_PIXEL_FORMAT_YCbCr_420_888 == pStream->format)))
    {
        bIsYUVOutStream = TRUE;
    }

    return bIsYUVOutStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsYUVInStream
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsYUVInStream(
    const camera3_stream_t* pStream)
{
    CHX_ASSERT(NULL != pStream);

    BOOL bIsYUVInStream = FALSE;

    if ((CAMERA3_STREAM_INPUT  == pStream->stream_type) &&
        ((HAL_PIXEL_FORMAT_YCbCr_420_888 == pStream->format)))
    {
        bIsYUVInStream = TRUE;
    }

    return bIsYUVInStream;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsPreviewZSLYUVStreamConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsPreviewZSLStreamConfig(
    const camera3_stream_configuration_t* pStreamConfig)
{
    CHX_ASSERT(2 == pStreamConfig->num_streams);

    BOOL isMatch = FALSE;

    /// if YUV stream + Preview stream
    if (((TRUE == IsPreviewStream(pStreamConfig->streams[0])) && (TRUE == IsYUVSnapshotStream(pStreamConfig->streams[1]))) ||
        ((TRUE == IsPreviewStream(pStreamConfig->streams[1])) && (TRUE == IsYUVSnapshotStream(pStreamConfig->streams[0]))))
    {
        isMatch = TRUE;
    }
    else
    {
        /// if JPEG stream + Preview stream
        if (((TRUE == IsPreviewStream(pStreamConfig->streams[0]))       &&
             (TRUE == IsJPEGSnapshotStream(pStreamConfig->streams[1]))) ||
            ((TRUE == IsPreviewStream(pStreamConfig->streams[1]))       &&
             (TRUE == IsJPEGSnapshotStream(pStreamConfig->streams[0]))))
        {
            isMatch = TRUE;
        }
    }

    return isMatch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsRawJPEGStreamConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsRawJPEGStreamConfig(
    const camera3_stream_configuration_t* pStreamConfig)
{
    BOOL isRaw   = FALSE;
    BOOL isJPEG  = FALSE;

    for (UINT i = 0; i < pStreamConfig->num_streams; i++)
    {
        if (TRUE == IsRawStream(pStreamConfig->streams[i]))
        {
            isRaw = TRUE;
        }
        else if (TRUE == IsJPEGSnapshotStream(pStreamConfig->streams[i]))
        {
            isJPEG = TRUE;
        }
    }

    return (isRaw && isJPEG);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsYUVInBlobOutConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsYUVInBlobOutConfig(
    const camera3_stream_configuration_t* pStreamConfig)
{
    BOOL bInYUV     = FALSE;
    BOOL bBlobOut   = FALSE;
    UINT yuvOutCnt  = 0;

    for (UINT i = 0; i < pStreamConfig->num_streams; i++)
    {
        if (TRUE == IsYUVInStream(pStreamConfig->streams[i]))
        {
            bInYUV = TRUE;
        }
        else if (TRUE == IsJPEGSnapshotStream(pStreamConfig->streams[i]))
        {
            bBlobOut = TRUE;
        }
        else if (TRUE == IsYUVOutStream(pStreamConfig->streams[i]))
        {
            yuvOutCnt++;
        }
    }
    if (1 < yuvOutCnt)
    {
        bInYUV     = FALSE;
    }

    return ((bInYUV && bBlobOut) ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsVideoLiveShotConfig
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsVideoLiveShotConfig(
    const camera3_stream_configuration_t* pStreamConfig)
{
    BOOL bVideoStream   = FALSE;
    BOOL bBlobOut       = FALSE;
    BOOL bCntMatch      = FALSE;
    UINT yuvOutCnt      = 0;

    if (3 == pStreamConfig->num_streams)
    {
        bCntMatch = TRUE;

        for (UINT i = 0; i < pStreamConfig->num_streams; i++)
        {
            if (TRUE == IsJPEGSnapshotStream(pStreamConfig->streams[i]))
            {
                bBlobOut = TRUE;
            }
            else if (TRUE == IsVideoStream(pStreamConfig->streams[i]))
            {
                bVideoStream = TRUE;
            }
        }
    }

    return (((TRUE == bVideoStream) && (TRUE == bBlobOut) && (TRUE == bCntMatch)) ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::MFNRMatchingUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::MFNRMatchingUsecase(
    const camera3_stream_configuration_t* pStreamConfig)
{
    CHX_ASSERT(2 == pStreamConfig->num_streams);

    BOOL isMatch                = FALSE;
    BOOL advanceProcessingMFNR  = TRUE;

    /// @todo add check for vendor tag or hint to enable MFNR flow
    if (TRUE == advanceProcessingMFNR)
    {
        isMatch = TRUE;
    }

    return isMatch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::QuadCFAMatchingUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::QuadCFAMatchingUsecase(
    const LogicalCameraInfo*              pCamInfo,
    const camera3_stream_configuration_t* pStreamConfig)
{
    BOOL isMatch         = FALSE;
    CHIRECT binning_size = { 0 };

    CHX_ASSERT(2 == pStreamConfig->num_streams);

    for (UINT i = 0; i < pCamInfo->m_cameraCaps.numSensorModes; i++)
    {
        CHX_LOG("i:%d, sensor mode:%d", i, pCamInfo->pSensorModeInfo[i].sensorModeCaps.value);
        if (1 == pCamInfo->pSensorModeInfo[i].sensorModeCaps.u.QuadCFA)
        {
            binning_size.width  = pCamInfo->pSensorModeInfo[i].frameDimension.width  >> 1;
            binning_size.height = pCamInfo->pSensorModeInfo[i].frameDimension.height >> 1;
            CHX_LOG("sensor binning size:%dx%d", binning_size.width, binning_size.height);
            break;
        }
    }

    for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if (IsYUVSnapshotStream(pStreamConfig->streams[stream]) ||
            IsJPEGSnapshotStream(pStreamConfig->streams[stream]))
        {
            if (pStreamConfig->streams[stream]->width  > binning_size.width ||
                pStreamConfig->streams[stream]->height > binning_size.height)
            {
                // if jpge/yuv snapshot stream is larger than binning size, then select Quad CFA usecase
                // otherwise, treat it as a normal one.
                isMatch = TRUE;
            }
            break;
        }
    }

    return isMatch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsQuadCFASensor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsQuadCFASensor(
    const LogicalCameraInfo* pCamInfo)
{
    BOOL isQuadCFASensor = FALSE;

    for (UINT i = 0; i < pCamInfo->m_cameraCaps.numSensorModes; i++)
    {
        CHX_LOG("i:%d, sensor mode:%d", i, pCamInfo->pSensorModeInfo[i].sensorModeCaps.value);
        if (pCamInfo->pSensorModeInfo[i].sensorModeCaps.u.QuadCFA == 1)
        {
            isQuadCFASensor = TRUE;
            break;
        }
    }

    return isQuadCFASensor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::IsSHDRSensor
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsSHDRSensor(
    const LogicalCameraInfo* pCamInfo)
{
    BOOL isSHDRSensor = FALSE;

    for (UINT i = 0; i < pCamInfo->m_cameraCaps.numSensorModes; i++)
    {
        CHX_LOG("i:%d, sensor mode:%d", i, pCamInfo->pSensorModeInfo[i].sensorModeCaps.value);
        if (pCamInfo->pSensorModeInfo[i].sensorModeCaps.u.SHDR == 1)
        {
            isSHDRSensor = TRUE;
            break;
        }
    }

    return isSHDRSensor;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::GetMatchingUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseId UsecaseSelector::GetMatchingUsecase(
    const LogicalCameraInfo*        pCamInfo,
    camera3_stream_configuration_t* pStreamConfig)
{
    UsecaseId usecaseId   = UsecaseId::Default;
    UINT32 enablevideoHDR = ExtensionModule::GetInstance()->GetVideoHDRMode();
    UINT32 VRDCEnable = ExtensionModule::GetInstance()->GetDCVRMode();
    if (pStreamConfig->num_streams == 2 && IsQuadCFASensor(pCamInfo))
    {
        // need to validate preview size <= binning size, otherwise return error

        if (TRUE == QuadCFAMatchingUsecase(pCamInfo, pStreamConfig))
        {
            usecaseId = UsecaseId::QuadCFA;
            CHX_LOG("Quad CFA usecase selected");
            return usecaseId;
        }
    }
    if ((1 == enablevideoHDR) && IsSHDRSensor(pCamInfo))
    {
        usecaseId = UsecaseId::PreviewZSL;
        return usecaseId;
    }

    /// Reset the usecase flags
    VideoEISV2Usecase   = 0;
    VideoEISV3Usecase   = 0;
    GPURotationUsecase  = FALSE;
    GPUDownscaleUsecase = FALSE;

    if ((NULL != pCamInfo) && (pCamInfo->numPhysicalCameras > 1) && VRDCEnable)
    {
        usecaseId = UsecaseId::MultiCameraVR;
    }
    else if ((NULL != pCamInfo) && (pCamInfo->numPhysicalCameras > 1) && (pStreamConfig->num_streams > 1))
    {
        CHX_LOG("MultiCamera usecase selected");
        usecaseId = UsecaseId::MultiCamera;
    }
    else
    {
        switch (pStreamConfig->num_streams)
        {
            case 2:
                if (TRUE == IsRawJPEGStreamConfig(pStreamConfig))
                {
                    CHX_LOG("Raw + JPEG usecase selected");
                    usecaseId = UsecaseId::RawJPEG;
                    break;
                }

                /// @todo Enable ZSL by setting overrideDisableZSL to FALSE
                if (FALSE == m_pExtModule->DisableZSL())
                {
                    if (TRUE == IsPreviewZSLStreamConfig(pStreamConfig))
                    {
                        usecaseId = UsecaseId::PreviewZSL;
                        CHX_LOG_INFO("ZSL usecase selected");
                    }
                }

                if(TRUE == m_pExtModule->UseGPURotationUsecase())
                {
                    CHX_LOG("GPU Rotation usecase flag set");
                    GPURotationUsecase = TRUE;
                }

                if (TRUE == m_pExtModule->UseGPUDownscaleUsecase())
                {
                    CHX_LOG("GPU Downscale usecase flag set");
                    GPUDownscaleUsecase = TRUE;
                }

                if (TRUE == m_pExtModule->EnableMFNRUsecase())
                {
                    if (TRUE == MFNRMatchingUsecase(pStreamConfig))
                    {
                        usecaseId = UsecaseId::MFNR;
                        CHX_LOG("MFNR usecase selected");
                    }
                }

                if (TRUE == m_pExtModule->EnableHFRNo3AUsecas())
                {
                    CHX_LOG("HFR without 3A usecase flag set");
                    HFRNo3AUsecase = TRUE;
                }

                break;

            case 3:
                VideoEISV2Usecase = m_pExtModule->EnableEISV2Usecase();
                VideoEISV3Usecase = m_pExtModule->EnableEISV3Usecase();

                if(TRUE == IsRawJPEGStreamConfig(pStreamConfig))
                {
                    CHX_LOG("Raw + JPEG usecase selected");
                    usecaseId = UsecaseId::RawJPEG;
                }
                else if((0 == VideoEISV3Usecase) && (0 == VideoEISV2Usecase) &&
                    (TRUE == IsVideoLiveShotConfig(pStreamConfig)))
                {
                    CHX_LOG("Video With Liveshot, ZSL usecase selected");
                    usecaseId = UsecaseId::VideoLiveShot;
                }

                break;

            case 4:
                if(TRUE == IsYUVInBlobOutConfig(pStreamConfig))
                {
                    CHX_LOG("YUV callback and Blob");
                    usecaseId = UsecaseId::YUVInBlobOut;
                }

                break;

            default:
                break;

        }
    }

    if (TRUE == ExtensionModule::GetInstance()->IsTorchWidgetUsecase())
    {
        CHX_LOG("Torch widget usecase selected");
        usecaseId = UsecaseId::Torch;
    }

    CHX_LOG("usecase ID:%d",usecaseId);
    return usecaseId;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::DefaultMatchingUsecaseforYUVCamera
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* UsecaseSelector::DefaultMatchingUsecaseforYUVCamera(
    camera3_stream_configuration_t* pStreamConfig)
{
    ChiUsecase*        pSelectedUsecase   = NULL;

    switch (pStreamConfig->num_streams)
    {
        case 1:
            if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases1Target[UsecasePreviewYUVCameraId]))
            {
                pSelectedUsecase = &Usecases1Target[UsecasePreviewYUVCameraId];
            }
            break;

        default:
            break;
    }
    if (NULL != pSelectedUsecase)
    {
        CHX_LOG_INFO("usecase %s, pipelineName %s",
            pSelectedUsecase->pUsecaseName, pSelectedUsecase->pPipelineTargetCreateDesc->pPipelineName);
    }
    else
    {
        CHX_LOG_ERROR("Failed to match usecase. pSelectedUsecase is NULL");
    }
    return pSelectedUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::DefaultMatchingUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChiUsecase* UsecaseSelector::DefaultMatchingUsecase(
    camera3_stream_configuration_t* pStreamConfig)
{
    ChiUsecase*        pSelectedUsecase   = NULL;
    ChiTargetUsecases* pChiTargetUsecases = NULL;
    BOOL               secureMode = FALSE;
    UINT32             disableBit = 0;  //  Disables FOVC bit from operation_mode for proper UseCase selection in EIS usecase.

    for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if (0 != (pStreamConfig->streams[stream]->usage & GRALLOC_USAGE_PROTECTED))
        {
            secureMode = TRUE;
            break;
        }
    }

    if (pStreamConfig->num_streams <= ChiMaxNumTargets)
    {
        if (((TRUE == GPURotationUsecase) || (TRUE == GPUDownscaleUsecase)) &&
            (StreamConfigModeNormal == pStreamConfig->operation_mode))
        {
            CHX_LOG("Only selecting GPU usecases");
            switch (pStreamConfig->num_streams)
            {
            case 2:
                if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases2Target[UsecaseJPEGEncodePreviewSnapshotGPUId]))
                {
                    pSelectedUsecase = &Usecases2Target[UsecaseJPEGEncodePreviewSnapshotGPUId];
                }
                break;
            case 3:
                if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases3Target[UsecaseJPEGEncodeLiveSnapshotGPUId]))
                {
                    pSelectedUsecase = &Usecases3Target[UsecaseJPEGEncodeLiveSnapshotGPUId];
                }
                break;
            default:
                break;
            }
            if (NULL == pSelectedUsecase)
            {
                CHX_LOG("Failed to match with a GPU usecase");
            }
        }
        if ((TRUE == HFRNo3AUsecase)
                && (StreamConfigModeConstrainedHighSpeed == pStreamConfig->operation_mode) && (NULL == pSelectedUsecase))
        {
            if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases2Target[UsecaseVideoHFRNo3AId]))
            {
                pSelectedUsecase = &Usecases2Target[UsecaseVideoHFRNo3AId];

                CHX_LOG("UsecaseVideoHFRNo3A is selected for HFR");
            }
        }
        else if ((TRUE == secureMode) && (NULL == pSelectedUsecase))
        {
            CHX_LOG("Only selecting Secure usecases");
            switch (pStreamConfig->num_streams)
            {
            case 1:
                if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases1Target[UsecaseSecurePreviewId]))
                {
                    pSelectedUsecase = &Usecases1Target[UsecaseSecurePreviewId];
                } else if (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases1Target[UsecaseRawId])){
                    pSelectedUsecase = &Usecases1Target[UsecaseRawId];
                }
                break;
            default:
                break;
            }
            if (NULL == pSelectedUsecase)
            {
                CHX_LOG("Failed to match with a Secure usecase");
            }
        }
        else if (NULL == pSelectedUsecase)
        {
            if ((pStreamConfig->operation_mode & StreamConfigModeQTIFOVC) == StreamConfigModeQTIFOVC)
            {
                disableBit = StreamConfigModeQTIFOVC ^ StreamConfigModeQTIStart;
                pStreamConfig->operation_mode &= ~(disableBit); // Disable FOVC bit for proper usecase selection
            }

            if (0 != VideoEISV3Usecase)
            {
                if (2 == VideoEISV3Usecase)
                {
                    // Force EISv3 usecase
                    pStreamConfig->operation_mode = pStreamConfig->operation_mode | StreamConfigModeQTIEISLookAhead;
                }
                if (((pStreamConfig->operation_mode & StreamConfigModeQTIEISLookAhead) == StreamConfigModeQTIEISLookAhead) &&
                    (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases3Target[UsecaseVideoEIS3PreviewEIS2Id])))
                {
                    CHX_LOG("Selected EISv3 usecase");
                    pSelectedUsecase = &Usecases3Target[UsecaseVideoEIS3PreviewEIS2Id];
                }
            }
            if ((0 != VideoEISV2Usecase) && (NULL == pSelectedUsecase))
            {
                if (2 == VideoEISV2Usecase)
                {
                    // Force EISv2 usecase
                    pStreamConfig->operation_mode = pStreamConfig->operation_mode | StreamConfigModeQTIEISRealTime;
                }
                if (((pStreamConfig->operation_mode & StreamConfigModeQTIEISRealTime) == StreamConfigModeQTIEISRealTime) &&
                    (TRUE == IsMatchingUsecase(pStreamConfig, &Usecases3Target[UsecaseVideoEIS2PreviewEIS2Id])))
                {
                    CHX_LOG("Selected EISv2 usecase");
                    pSelectedUsecase = &Usecases3Target[UsecaseVideoEIS2PreviewEIS2Id];
                }
            }
            if (NULL == pSelectedUsecase)
            {
                if ((StreamConfigModeQTIEISLookAhead == pStreamConfig->operation_mode) ||
                    (StreamConfigModeQTIEISRealTime  == pStreamConfig->operation_mode) ||
                    (StreamConfigModeQTIEISDG        == pStreamConfig->operation_mode) ||
                    (StreamConfigModeQTIEISDGCustom1 == pStreamConfig->operation_mode))
                {
                    // EIS is disabled, ensure that operation_mode is also set accordingly
                    pStreamConfig->operation_mode = 0;
                }
                pChiTargetUsecases = &PerNumTargetUsecases[pStreamConfig->num_streams - 1];
            }

            if (disableBit != 0)
            {
                pStreamConfig->operation_mode |= disableBit; // reset the FOVC bit, if disabledbit set
            }
        }
    }

    if (NULL != pChiTargetUsecases)
    {
        // this check is introduced to mask FOVC operation mode before selecting a Usecase, as alone FOVC usecase is not
        // introduced in xml yet. And makes sure that if FOVC is set in operation mode it will continue for all video recording cases.
        if ((pStreamConfig->operation_mode & StreamConfigModeQTIFOVC) == StreamConfigModeQTIFOVC)
        {
            pStreamConfig->operation_mode = pStreamConfig->operation_mode ^ StreamConfigModeQTIFOVC;
        }

        for (UINT i = 0; i < pChiTargetUsecases->numUsecases; i++)
        {
            if (TRUE == IsMatchingUsecase(pStreamConfig, &pChiTargetUsecases->pChiUsecases[i]))
            {
                pSelectedUsecase = &pChiTargetUsecases->pChiUsecases[i];
                CHX_LOG("selected use case index:%d", i);
                break;
            }
        }
    }

    if (NULL != pSelectedUsecase)
    {
        CHX_LOG_INFO("usecase %s, pipelineName %s",
            pSelectedUsecase->pUsecaseName, pSelectedUsecase->pPipelineTargetCreateDesc->pPipelineName);
    }
    else
    {
        CHX_LOG_ERROR("Failed to match usecase. pSelectedUsecase is NULL");
    }

    return pSelectedUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseSelector::IsMatchingUsecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsMatchingUsecase(
    const camera3_stream_configuration_t* pStreamConfig,
    const ChiUsecase*                     pUsecase)
{
    CHX_ASSERT(NULL != pStreamConfig);
    CHX_ASSERT(NULL != pUsecase);
    CHX_ASSERT(pUsecase->numTargets == pStreamConfig->num_streams);

    UINT       numStreams       = pStreamConfig->num_streams;
    BOOL       isMatching       = FALSE;
    UINT       streamConfigMode = pUsecase->streamConfigMode;

    // The usecase structure generated from the parser contains an array of "HAL Target Info" that describes the HAL
    // buffers allowed for the usecase. We match the currently set stream config with the target info to find a matching
    // usecase from the pipeline XML. The first stream tries to find a match with any one of the 'n' target's in the
    // usecase. If it finds a match, the matching target from the usecase needs to be removed from the search list of
    // the next stream. So this variable keeps track of the targets that we need to compare the stream with. After
    // every successful stream match the matching target will be removed from the array below to prevent the next stream
    // from comparing with the same matching target for a previous stream.
    // We need to search all the targets in the usecase for the first stream
    UINT compareTargetIndexMask = ((1 << numStreams) - 1);

    if (streamConfigMode == static_cast<UINT>(pStreamConfig->operation_mode))
    {
        // For each stream, compare the stream parameters with the pipeline usecase hal target parameters
        for (UINT streamId = 0; streamId < numStreams; streamId++)
        {
            ChiStream* pStream = reinterpret_cast<ChiStream*>(pStreamConfig->streams[streamId]);

            CHX_ASSERT(pStream != NULL);

            if (NULL != pStream)
            {
                INT    streamFormat = pStream->format;
                UINT   streamType   = pStream->streamType;
                UINT32 streamWidth  = pStream->width;
                UINT32 streamHeight = pStream->height;

                CHX_LOG("streamType = %d streamFormat = %d streamWidth = %d streamHeight = %d",
                        streamType, streamFormat, streamWidth, streamHeight);

                // Current stream search begins as not matching any targets in the usecase being compared with
                isMatching = FALSE;

                // For the current stream, try to find a match with one of the targets defined in the usecase
                // If the stream matches with one of the targets, move onto trying to find a match for the next stream. Before
                // trying to find a match for the next stream, remove the currently matching usecase target from the next
                // search list. The usecase targets to consider for matching are given in "compareWithTargetIndex[]"

                for (UINT targetInfoIdx = 0; targetInfoIdx < numStreams; targetInfoIdx++)
                {
                    if (TRUE == ChxUtils::IsBitSet(compareTargetIndexMask, targetInfoIdx))
                    {
                        ChiTarget* pTargetInfo = pUsecase->ppChiTargets[targetInfoIdx];

                        isMatching = IsMatchingFormat(reinterpret_cast<ChiStream*>(pStream),
                                                      pTargetInfo->numFormats,
                                                      pTargetInfo->pBufferFormats);

                        if (TRUE == isMatching)
                        {
                            isMatching = ((streamType == static_cast<UINT>(pTargetInfo->direction)) ? TRUE : FALSE);
                        }

                        if (TRUE == isMatching)
                        {
                            BufferDimension* pRange = &pTargetInfo->dimension;

                            if ((streamWidth  >= pRange->minWidth)  &&
                                (streamWidth  <= pRange->maxWidth)  &&
                                (streamHeight >= pRange->minHeight) &&
                                (streamHeight <= pRange->maxHeight))
                            {
                                isMatching = TRUE;
                            }
                            else
                            {
                                isMatching = FALSE;
                            }
                        }

                        // Current stream (streamId) matches with the current target (targetInfoIdx) in the usecase - so remove
                        // that target from the search list of the next stream
                        if (TRUE == isMatching)
                        {
                            pTargetInfo->pChiStream = pStream;
                            // Remove the target entry from the search list of the next stream
                            compareTargetIndexMask = ChxUtils::BitReset(compareTargetIndexMask, targetInfoIdx);
                            break; // Move onto the next stream because we found a match for the current stream
                        }
                    }
                }

                // Current stream did not find any matching target entry in the currently selected usecase. So bail out of the
                // current search (so that we can move onto the next usecase for matching)
                if (FALSE == isMatching)
                {
                    break;
                }
            }
        }
    }

    return isMatching;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseSelector::IsAllowedImplDefinedFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsAllowedImplDefinedFormat(
    ChiBufferFormat format,
    GrallocUsage    grallocUsage)
{
    BOOL isAllowed = FALSE;

    for (UINT i = 0; i < NumImplDefinedFormats; i++)
    {
        if (AllowedImplDefinedFormats[i] == format)
        {
            if (AllowedImplDefinedFormats[i] == ChiFormatRawPlain16)
            {
                // If it is a display buffer we cannot allow Raw buffer
                if ((0 != (grallocUsage & GRALLOC_USAGE_HW_COMPOSER)) && (0 == (grallocUsage & GRALLOC_USAGE_HW_TEXTURE)))
                {
                    isAllowed = TRUE;
                    break;
                }
            }
            else
            {
                isAllowed = TRUE;
                break;
            }
        }
    }

    return isAllowed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseSelector::IsMatchingFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL UsecaseSelector::IsMatchingFormat(
    ChiStream*             pStream,
    UINT32                 numFormats,
    const ChiBufferFormat* pMatchingFormats)
{
    CHX_ASSERT(pStream != NULL);

    BOOL                isMatching         = FALSE;
    ChiStreamFormat     streamFormat       = pStream->format;
    GrallocUsage        streamGrallocUsage = pStream->grallocUsage;
    ChiStreamType       streamType         = static_cast<ChiStreamType>(pStream->streamType);

    for (UINT32 i = 0; i < numFormats; i++)
    {
        if ((ChiStreamFormatRaw16 == streamFormat) && (ChiFormatRawPlain16 == pMatchingFormats[i]))
        {
            isMatching = TRUE;
        }
        else if (((ChiStreamFormatY8 == streamFormat) && (DataspaceDepth != pStream->dataspace)) &&
                 (ChiFormatRawMIPI8 == pMatchingFormats[i]))
        {
            isMatching = TRUE;
        }
        else if (((ChiStreamFormatRawOpaque == streamFormat) || (ChiStreamFormatRaw10 == streamFormat) ||
                 (ChiStreamFormatRaw12 == streamFormat)) &&
                 (ChiFormatRawMIPI == pMatchingFormats[i]))
        {
            isMatching = TRUE;
        }
        else if (((ChiStreamFormatYCbCr420_888                    == streamFormat)               ||
                  ((ChiStreamFormatImplDefined                    == streamFormat)               &&
                  ((ChiStreamTypeBidirectional                    == streamType)                 ||
                   (ChiStreamTypeInput                            == streamType)                 ||
                  ((streamGrallocUsage & GrallocUsageHwCameraZSL) == GrallocUsageHwCameraZSL)))) &&
                  ((ChiFormatYUV420NV12 == pMatchingFormats[i])  || (ChiFormatYUV420NV21 == pMatchingFormats[i])))
        {
            isMatching = TRUE;
        }
        else if (ChiStreamFormatImplDefined == streamFormat)
        {
            isMatching = IsAllowedImplDefinedFormat(pMatchingFormats[i], streamGrallocUsage);
        }
        else if ((ChiStreamFormatBlob == streamFormat) &&
                 ((ChiFormatJpeg == pMatchingFormats[i]) || (ChiFormatBlob == pMatchingFormats[i])))
        {
            isMatching = TRUE;
        }
        else if ((ChiStreamFormatY16 == streamFormat) && (ChiFormatY16 == pMatchingFormats[i]))
        {
            isMatching = TRUE;
        }
        else if (((ChiStreamFormatY8 == streamFormat) && (DataspaceDepth == pStream->dataspace)) &&
                 (ChiFormatY8 == pMatchingFormats[i]))
        {
            isMatching = TRUE;
        }
    }
    return isMatching;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::FindMaxResolutionCameraID
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 UsecaseSelector::FindMaxResolutionCameraID(
    LogicalCameraInfo*     pCameraInfo)
{
    // Default assignment of camera ID
    UINT32 cameraID = pCameraInfo->ppDeviceInfo[0]->cameraId;

    // Find camera corresponding to maximum resolution in dual camera use case
    if (1 < pCameraInfo->numPhysicalCameras)
    {
        camera_info_t* pCameraInfoMain = static_cast<camera_info_t*>(pCameraInfo->ppDeviceInfo[0]->m_pDeviceCaps->pLegacy);
        const camera_metadata_t* pMetadataMain = pCameraInfoMain->static_camera_characteristics;

        camera_info_t* pCameraInfoAux = static_cast<camera_info_t*>(pCameraInfo->ppDeviceInfo[1]->m_pDeviceCaps->pLegacy);
        const camera_metadata_t* pMetadataAux = pCameraInfoAux->static_camera_characteristics;

        if ((NULL != pMetadataMain) && (NULL != pMetadataAux))
        {
            camera_metadata_entry_t entryMain = { 0 };
            camera_metadata_entry_t entryAux  = { 0 };

            if ((0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataMain),
                    ANDROID_JPEG_MAX_SIZE, &entryMain)) &&
                (0 == find_camera_metadata_entry(const_cast<camera_metadata_t*>(pMetadataAux),
                    ANDROID_JPEG_MAX_SIZE, &entryAux)))
            {
                INT32 maxJPEGSizeOfMain = *entryMain.data.i32;
                INT32 maxJPEGSizeOfAux  = *entryAux.data.i32;

                if (maxJPEGSizeOfAux > maxJPEGSizeOfMain)
                {
                    cameraID = pCameraInfo->ppDeviceInfo[1]->cameraId;
                }
            }
        }
        else
        {
            CHX_LOG_ERROR("One of the metadata is Null and so set to default camera ID %d", cameraID);
        }
    }

    return cameraID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseSelector::getSensorDimension
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseSelector::getSensorDimension(
    const UINT32                          cameraID,
    const camera3_stream_configuration_t* pStreamConfig,
    UINT32                                *pSensorw,
    UINT32                                *pSensorh,
    UINT32                                downscaleFactor)
{
    UINT32 maxWidth = 0;
    UINT32 maxHeight = 0;
    DesiredSensorMode desiredSensorMode = { 0 };
    desiredSensorMode.frameRate = ExtensionModule::GetInstance()->GetUsecaseMaxFPS();
    desiredSensorMode.forceMode = ExtensionModule::GetInstance()->GetForceSensorMode();

    // Select the highest width/height from all the input buffer requirements
    for (UINT32 stream = 0; stream < pStreamConfig->num_streams; stream++)
    {
        if ((pStreamConfig->streams[stream]->width/downscaleFactor) > maxWidth)
        {
            maxWidth = pStreamConfig->streams[stream]->width/downscaleFactor;
        }
        if(pStreamConfig->streams[stream]->height > maxHeight)
        {
            maxHeight = pStreamConfig->streams[stream]->height;
        }
    }

    desiredSensorMode.optimalWidth  = maxWidth;
    desiredSensorMode.optimalHeight = maxHeight;
    desiredSensorMode.maxWidth      = maxWidth;
    desiredSensorMode.maxHeight     = maxHeight;
    desiredSensorMode.minWidth      = maxWidth;
    desiredSensorMode.minHeight     = maxHeight;

    CHISENSORMODEINFO sensorMode = *(ChxSensorModeSelect::FindBestSensorMode(cameraID,
        &desiredSensorMode));
    *pSensorw = sensorMode.frameDimension.width;
    *pSensorh = sensorMode.frameDimension.height;

    CHX_LOG("Sensor Output for cameraID %d, RAW: %dX%d, output: %dX%d", cameraID, *pSensorw, *pSensorh, maxWidth, maxHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseFactory::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseFactory* UsecaseFactory::Create(
    const ExtensionModule* pExtModule)
{
    UsecaseFactory* pFactory = new UsecaseFactory;

    if (NULL != pFactory)
    {
        pFactory->m_pExtModule = pExtModule;
    }
    return pFactory;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseFactory::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseFactory::Destroy()
{
    delete this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseFactory::~UsecaseFactory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseFactory::~UsecaseFactory()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UsecaseFactory::CreateUsecaseObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Usecase* UsecaseFactory::CreateUsecaseObject(
    LogicalCameraInfo*              pLogicalCameraInfo,     ///< camera info
    UsecaseId                       usecaseId,              ///< Usecase Id
    camera3_stream_configuration_t* pStreamConfig)          ///< Stream config
{
    Usecase* pUsecase  = NULL;
    UINT     camera0Id = pLogicalCameraInfo->ppDeviceInfo[0]->cameraId;

    switch (usecaseId)
    {
        case UsecaseId::PreviewZSL:
        case UsecaseId::VideoLiveShot:
            pUsecase = AdvancedCameraUsecase::Create(pLogicalCameraInfo, pStreamConfig, usecaseId);
            break;
        case UsecaseId::MultiCamera:
            pUsecase = UsecaseMultiCamera::Create(pLogicalCameraInfo, pStreamConfig);
            break;
        case UsecaseId::MultiCameraVR:
            pUsecase = UsecaseMultiVRCamera::Create(pLogicalCameraInfo, pStreamConfig);
            break;
        case UsecaseId::QuadCFA:
            pUsecase = UsecaseQuadCFA::Create(pLogicalCameraInfo, pStreamConfig);
            break;
        case UsecaseId::Torch:
            pUsecase = UsecaseTorch::Create(camera0Id, pStreamConfig);
            break;
        default:
            pUsecase = AdvancedCameraUsecase::Create(pLogicalCameraInfo, pStreamConfig, usecaseId);
            break;
    }

    return pUsecase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferManager::~BufferManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BufferManager::~BufferManager()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferManager::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BufferManager* BufferManager::Create(
    BufferManagerCreateData* pCreateData)
{
    CDKResult      result         = CDKResultSuccess;
    BufferManager* pBufferManager = CHX_NEW BufferManager;

    result = pBufferManager->Initialize(pCreateData);

    if (CDKResultSuccess != result)
    {
        pBufferManager->Destroy();
        pBufferManager = NULL;
    }

    return pBufferManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferManager::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID BufferManager::Destroy()
{
    FreeAllBuffers();
#ifndef LE_CAMERA
    gralloc1_close(m_pGralloc1Device);
#endif //LE_CAMERA
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BufferManager::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BufferManager::Initialize(
    BufferManagerCreateData* pCreateData)
{
    CDKResult result    = CDKResultSuccess;
    m_numBuffers        = pCreateData->numBuffers;
    m_nextFreeBuffer    = 0;

    result = SetupGralloc1Interface();

    if (CDKResultSuccess == result)
    {
        AllocateBuffers(m_numBuffers,
                        pCreateData->width,
                        pCreateData->height,
                        pCreateData->format,
                        GRALLOC1_PRODUCER_USAGE_CAMERA,
                        GRALLOC1_CONSUMER_USAGE_CAMERA);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup gralloc1 interface functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BufferManager::SetupGralloc1Interface()
{
    CDKResult result = CDKResultSuccess;
#if !defined(LE_CAMERA)
#if defined (_LINUX)
    hw_get_module(GRALLOC_HARDWARE_MODULE_ID, const_cast<const hw_module_t**>(&m_pHwModule));
#endif

    if (NULL != m_pHwModule)
    {
        gralloc1_open(m_pHwModule, &m_pGralloc1Device);
    }
    else
    {
        result = CDKResultEFailed;
    }

    if ((CDKResultSuccess == result) && (NULL != m_pGralloc1Device))
    {
        m_grallocInterface.CreateDescriptor  = reinterpret_cast<GRALLOC1_PFN_CREATE_DESCRIPTOR>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_CREATE_DESCRIPTOR));
        m_grallocInterface.DestroyDescriptor = reinterpret_cast<GRALLOC1_PFN_DESTROY_DESCRIPTOR>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_DESTROY_DESCRIPTOR));
        m_grallocInterface.SetDimensions     = reinterpret_cast<GRALLOC1_PFN_SET_DIMENSIONS>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_SET_DIMENSIONS));
        m_grallocInterface.SetFormat         = reinterpret_cast<GRALLOC1_PFN_SET_FORMAT>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_SET_FORMAT));
        m_grallocInterface.SetProducerUsage  = reinterpret_cast<GRALLOC1_PFN_SET_PRODUCER_USAGE>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_SET_PRODUCER_USAGE));
        m_grallocInterface.SetConsumerUsage  = reinterpret_cast<GRALLOC1_PFN_SET_CONSUMER_USAGE>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_SET_CONSUMER_USAGE));
        m_grallocInterface.Allocate          = reinterpret_cast<GRALLOC1_PFN_ALLOCATE>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_ALLOCATE));
        m_grallocInterface.GetStride         = reinterpret_cast<GRALLOC1_PFN_GET_STRIDE>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_GET_STRIDE));
        m_grallocInterface.Release           = reinterpret_cast<GRALLOC1_PFN_RELEASE>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_RELEASE));
        m_grallocInterface.Lock              = reinterpret_cast<GRALLOC1_PFN_LOCK>(
                                                   m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                                                                  GRALLOC1_FUNCTION_LOCK));
    }
    else
    {
        result = CDKResultEFailed;
    }
#endif // LE_CAMERA
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Allocate a number of buffers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult BufferManager::AllocateBuffers(
    UINT32 numBuffers,
    UINT32 width,
    UINT32 height,
    UINT32 format,
    UINT64 producerFlags,
    UINT64 consumerFlags)
{
    CDKResult result = CDKResultSuccess;

    CHX_LOG("[buf.chi] AllocateBuffers, E.");

    for (UINT i = 0; i < numBuffers; i++)
    {
#ifdef LE_CAMERA
        AllocateGbmBuffer(width, height, format, producerFlags, consumerFlags, &m_buffers[i], &m_bufferStride);
#else
        AllocateOneBuffer(width, height, format, producerFlags, consumerFlags, &m_buffers[i], &m_bufferStride);
#endif
        CHX_LOG("[buf.chi] i %d, width %d, height %d stride %d", i, width, height, m_bufferStride);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Allocate a number of buffers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LE_CAMERA
CDKResult BufferManager::AllocateGbmBuffer(
    UINT32              width,
    UINT32              height,
    UINT32              format,
    UINT64              producerUsageFlags,
    UINT64              consumerUsageFlags,
    buffer_handle_t*    pAllocatedBuffer,
    UINT32*             pStride)
{
    CDKResult results                = CDKResultSuccess;
    SIZE_T bo_size                   = 0;
    UINT ret                         = 0;
    struct gbm_bo * m_pGbmBuffObject = NULL;
    if (NULL == pStride)
    {
        CHX_LOG_ERROR("Invalid argument !!");
        results = CDKResultEInvalidArg;
    }
    m_pGbmBuffObject = GBMBufferAllocator::GetHandle()->AllocateGbmBufferObj(width,
                                                                             height,
                                                                             format,
                                                                             producerUsageFlags,
                                                                             consumerUsageFlags);
    if (NULL != m_pGbmBuffObject)
    {
        *pStride = gbm_bo_get_stride(m_pGbmBuffObject);
        if (0 == *pStride)
        {
          CHX_LOG_ERROR("Invalid Stride length");
          results = CDKResultEUnsupported;
          goto error;
        }

        ret = gbm_perform(GBM_PERFORM_GET_BO_SIZE, m_pGbmBuffObject, &bo_size);
        if (GBM_ERROR_NONE != ret)
        {
          CHX_LOG_ERROR("Error in querying BO size");
          results = CDKResultEUnsupported;
          goto error;
        }

        CHX_LOG("buffer object size:%d", bo_size);
        *pAllocatedBuffer = GBMBufferAllocator::GetHandle()->AllocateNativeHandle(m_pGbmBuffObject);
        if (NULL == *pAllocatedBuffer)
        {
            CHX_LOG_ERROR("Error allocating native handle buffer");
            results = CDKResultENoMemory;
            goto error;
        }
    }
    else
    {
        CHX_LOG_ERROR("Invalid Gbm Buffer object");
        results = CDKResultENoMemory;
    }
    return results;

error:
    if (NULL != m_pGbmBuffObject)
    {
        GBMBufferAllocator::GetHandle()->FreeGbmBufferObj(m_pGbmBuffObject);
        m_pGbmBuffObject = NULL;
    }
    return results;
}

#else
CDKResult BufferManager::AllocateOneBuffer(
    UINT32           width,
    UINT32           height,
    UINT32           format,
    UINT64           producerUsageFlags,
    UINT64           consumerUsageFlags,
    buffer_handle_t* pAllocatedBuffer,
    UINT32*          pStride)
{
    INT32 result = GRALLOC1_ERROR_NONE;
    gralloc1_buffer_descriptor_t gralloc1BufferDescriptor;

    result = m_grallocInterface.CreateDescriptor(m_pGralloc1Device, &gralloc1BufferDescriptor);

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.SetDimensions(m_pGralloc1Device, gralloc1BufferDescriptor, width, height);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.SetFormat(m_pGralloc1Device, gralloc1BufferDescriptor, format);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.SetProducerUsage(m_pGralloc1Device, gralloc1BufferDescriptor, producerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.SetConsumerUsage(m_pGralloc1Device, gralloc1BufferDescriptor, consumerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.Allocate(m_pGralloc1Device, 1, &gralloc1BufferDescriptor, &pAllocatedBuffer[0]);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = m_grallocInterface.GetStride(m_pGralloc1Device, *pAllocatedBuffer, pStride);
    }

    m_grallocInterface.DestroyDescriptor(m_pGralloc1Device, gralloc1BufferDescriptor);
    return result;
}
#endif // LE_CAMERA

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free all buffers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID BufferManager::FreeAllBuffers()
{
    for (UINT i = 0; i < m_numBuffers; i++)
    {
        if (NULL != m_buffers[i])
        {
#ifdef LE_CAMERA
            native_handle_t * pHandle = const_cast<native_handle_t *>(
                                                    static_cast<const native_handle_t *>(m_buffers[i]));
            if(pHandle->data[1] != 0)
            {
                GBMBufferAllocator::GetHandle()->FreeGbmBufferObj(reinterpret_cast<struct gbm_bo *>
                                                                    (pHandle->data[1]));
            }
            native_handle_delete(pHandle);
#else
            m_grallocInterface.Release(m_pGralloc1Device, m_buffers[i]);
#endif // LE_CAMERA
            m_buffers[i] = NULL;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::ImageBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageBuffer::ImageBuffer()
    : pGrallocBuffer(NULL),
    m_aReferenceCount(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::~ImageBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageBuffer::~ImageBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::ImageBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef LE_CAMERA
ImageBuffer* ImageBuffer::Create(
    UINT32              width,
    UINT32              height,
    UINT32              format,
    UINT64              producerUsageFlags,
    UINT64              consumerUsageFlags,
    UINT32*             pStride)
{
    CHX_ASSERT(NULL != pStride);

    CDKResult       result          = CDKResultSuccess;
    ImageBuffer*    pImageBuffer    = NULL;

    if (NULL == pStride)
    {
        CHX_LOG_ERROR("Invalid arguments, creating ImageBuffer failed.");
    }
    else
    {
        pImageBuffer = CHX_NEW ImageBuffer;

        if (NULL != pImageBuffer)
        {
            result = pImageBuffer->AllocateGbmBuffer(width,
                                                     height,
                                                     format,
                                                     producerUsageFlags,
                                                     consumerUsageFlags,
                                                     pStride);
            if (CDKResultSuccess != result)
            {
                pImageBuffer->Destroy();
                pImageBuffer = NULL;
            }
        }
    }
    return pImageBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ImageBuffer::Destroy()
{
    if(NULL != pGrallocBuffer)
    {
        native_handle_t * pHandle = const_cast<native_handle_t *>(
                                                static_cast<const native_handle_t *>(pGrallocBuffer));
        if(pHandle->data[1] != 0)
        {
            GBMBufferAllocator::GetHandle()->FreeGbmBufferObj(reinterpret_cast<struct gbm_bo *>
                                                                (pHandle->data[1]));
        }
        native_handle_delete(pHandle);
        pGrallocBuffer = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::AllocateGbmBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ImageBuffer::AllocateGbmBuffer(
    UINT32              width,
    UINT32              height,
    UINT32              format,
    UINT64              producerUsageFlags,
    UINT64              consumerUsageFlags,
    UINT32*             pStride)
{
    CDKResult results                = CDKResultSuccess;
    SIZE_T bo_size                   = 0;
    UINT ret                         = 0;
    struct gbm_bo * m_pGbmBuffObject = NULL;
    if (NULL == pStride)
    {
        CHX_LOG_ERROR("Invalid argument !!");
        results = CDKResultEInvalidArg;
    }
    m_pGbmBuffObject = GBMBufferAllocator::GetHandle()->AllocateGbmBufferObj(width,
                                                                             height,
                                                                             format,
                                                                             producerUsageFlags,
                                                                             consumerUsageFlags);
    if (NULL != m_pGbmBuffObject)
    {
        *pStride = gbm_bo_get_stride(m_pGbmBuffObject);
        if (0 == *pStride)
        {
          CHX_LOG_ERROR("Invalid Stride length");
          results = CDKResultEUnsupported;
          goto error;
        }

        ret = gbm_perform(GBM_PERFORM_GET_BO_SIZE, m_pGbmBuffObject, &bo_size);
        if (GBM_ERROR_NONE != ret)
        {
          CHX_LOG_ERROR("Error in querying BO size");
          results = CDKResultEUnsupported;
          goto error;
        }

        CHX_LOG("buffer object size:%d", bo_size);
        pGrallocBuffer = GBMBufferAllocator::GetHandle()->AllocateNativeHandle(m_pGbmBuffObject);
        if (NULL == pGrallocBuffer)
        {
            CHX_LOG_ERROR("Error allocating native handle buffer");
            results = CDKResultENoMemory;
            goto error;
        }
    }
    else
    {
        CHX_LOG_ERROR("Invalid Gbm Buffer object");
        results = CDKResultENoMemory;
    }
    return results;

error:
    if (NULL != m_pGbmBuffObject)
    {
        GBMBufferAllocator::GetHandle()->FreeGbmBufferObj(m_pGbmBuffObject);
        m_pGbmBuffObject = NULL;
    }
    return results;
}

#else
ImageBuffer* ImageBuffer::Create(
    Gralloc1Interface*  pGrallocInterface,
    gralloc1_device_t*  pGralloc1Device,
    UINT32              width,
    UINT32              height,
    UINT32              format,
    UINT64              producerUsageFlags,
    UINT64              consumerUsageFlags,
    UINT32*             pStride)
{
    CHX_ASSERT(NULL != pGrallocInterface);
    CHX_ASSERT(NULL != pGralloc1Device);
    CHX_ASSERT(NULL != pStride);

    CDKResult       result          = CDKResultSuccess;
    ImageBuffer*    pImageBuffer    = NULL;

    if ((NULL == pGrallocInterface) ||
        (NULL == pGralloc1Device) ||
        (NULL == pStride))
    {
        CHX_LOG_ERROR("Invalid arguments, creating ImageBuffer failed.");
    }
    else
    {
        pImageBuffer = CHX_NEW ImageBuffer;

        if (NULL != pImageBuffer)
        {
            result = pImageBuffer->AllocateGrallocBuffer(pGrallocInterface,
                                                         pGralloc1Device,
                                                         width,
                                                         height,
                                                         format,
                                                         producerUsageFlags,
                                                         consumerUsageFlags,
                                                         pStride);
            if (CDKResultSuccess != result)
            {
                pImageBuffer->Destroy(pGrallocInterface, pGralloc1Device);
                pImageBuffer = NULL;
            }
        }
    }
    return pImageBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ImageBuffer::Destroy(Gralloc1Interface*  pGrallocInterface,
                          gralloc1_device_t*  pGralloc1Device)
{
    CHX_ASSERT(NULL != pGrallocInterface);
    CHX_ASSERT(NULL != pGralloc1Device);

    if ((NULL != pGrallocInterface) &&
        (NULL != pGralloc1Device))
    {
        UINT currentRefCount = GetReferenceCount();

        if (0 != currentRefCount)
        {
            CHX_LOG_ERROR("ImageBuffer %p is destroyed with reference count = %d", this, currentRefCount);
        }

        pGrallocInterface->Release(pGralloc1Device, pGrallocBuffer);
    }

    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::AllocateGrallocBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult ImageBuffer::AllocateGrallocBuffer(
    Gralloc1Interface*  pGrallocInterface,
    gralloc1_device_t*  pGralloc1Device,
    UINT32              width,
    UINT32              height,
    UINT32              format,
    UINT64              producerUsageFlags,
    UINT64              consumerUsageFlags,
    UINT32*             pStride)
{
    INT32 result = GRALLOC1_ERROR_NONE;
    gralloc1_buffer_descriptor_t gralloc1BufferDescriptor;

    result = pGrallocInterface->CreateDescriptor(pGralloc1Device, &gralloc1BufferDescriptor);

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->SetDimensions(pGralloc1Device, gralloc1BufferDescriptor, width, height);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->SetFormat(pGralloc1Device, gralloc1BufferDescriptor, format);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->SetProducerUsage(pGralloc1Device, gralloc1BufferDescriptor, producerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->SetConsumerUsage(pGralloc1Device, gralloc1BufferDescriptor, consumerUsageFlags);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->Allocate(pGralloc1Device, 1, &gralloc1BufferDescriptor, &pGrallocBuffer);
    }

    if (GRALLOC1_ERROR_NONE == result)
    {
        result = pGrallocInterface->GetStride(pGralloc1Device, pGrallocBuffer, pStride);
    }

    result = pGrallocInterface->DestroyDescriptor(pGralloc1Device, gralloc1BufferDescriptor);

    if (GRALLOC1_ERROR_NONE != result)
    {
        CHX_LOG_ERROR("AllocateGrallocBuffer failed");
    }
    return result;
}
#endif // LE_CAMERA
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::GetReferenceCount
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT ImageBuffer::GetReferenceCount()
{
    return ChxUtils::AtomicLoadU32(&m_aReferenceCount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::AddReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ImageBuffer::AddReference()
{
    UINT32 currentReferenceCount = GetReferenceCount();

    CHX_LOG("ReferenceCount for ImageBuffer %p is incremented to %d", this, (currentReferenceCount + 1));

    ChxUtils::AtomicStoreU32(&m_aReferenceCount, currentReferenceCount + 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageBuffer::ReleaseReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID ImageBuffer::ReleaseReference()
{
    UINT32 currentReferenceCount = GetReferenceCount();

    CHX_ASSERT(0 < currentReferenceCount);

    if (0 < currentReferenceCount)
    {
        CHX_LOG("ReferenceCount for ImageBuffer %p is decremented to %d", this, (currentReferenceCount - 1));

        ChxUtils::AtomicStoreU32(&m_aReferenceCount, currentReferenceCount - 1);
    }
    else
    {
        CHX_LOG_ERROR("ReleaseReference failed, current reference of ImageBuffer %p is 0", this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::BufferManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIBufferManager::CHIBufferManager()
#ifndef LE_CAMERA
    : m_pHwModule(NULL),
    m_pGralloc1Device(NULL),
    m_pLock(NULL),
#else
    :m_pLock(NULL),
#endif
    m_pFreeBufferList(NULL),
    m_pBusyBufferList(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::~BufferManager
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIBufferManager::~CHIBufferManager()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHIBufferManager* CHIBufferManager::Create(
    const CHAR*                 pBufferManagerName,
    CHIBufferManagerCreateData* pCreateData)
{
    CDKResult           result          = CDKResultSuccess;
    CHIBufferManager*   pBufferManager  = CHX_NEW CHIBufferManager;

    CHX_LOG("Creating BufferManager %s", pBufferManagerName);
    if (NULL != pBufferManager)
    {
        result = pBufferManager->Initialize(pBufferManagerName, pCreateData);

        if (CDKResultSuccess != result)
        {
            pBufferManager->Destroy();
            pBufferManager  = NULL;
            result          = CDKResultEFailed;
        }
    }

    return pBufferManager;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CHIBufferManager::Destroy()
{
    FreeBuffers(FALSE);
#if !defined(LE_CAMERA)
    gralloc1_close(m_pGralloc1Device);
#endif //LE_CAMERA
    if (NULL != m_pLock)
    {
        m_pLock->Destroy();
        m_pLock = NULL;
    }

    if(NULL != m_pWaitFreeBuffer)
    {
        m_pWaitFreeBuffer->Destroy();
        m_pWaitFreeBuffer = NULL;
    }

    CHX_DELETE(m_pFreeBufferList);
    CHX_DELETE(m_pBusyBufferList);
    CHX_DELETE(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::FreeBuffers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID CHIBufferManager::FreeBuffers(
    BOOL isPartialFree)
{
    m_pLock->Lock();

    ImageBuffer*                        pImageBuffer    = NULL;
    LightweightDoublyLinkedListNode*    pNode           = NULL;

    if ((NULL != m_pFreeBufferList) && (NULL != m_pBusyBufferList))
    {
        while (NULL != m_pFreeBufferList->Head())
        {
            if (TRUE == isPartialFree)
            {
                if ( (m_pFreeBufferList->NumNodes() + m_pBusyBufferList->NumNodes()) <= m_pBufferManagerData.immediateBufferCount)
                {
                    break;
                }
            }

            pNode           = m_pFreeBufferList->RemoveFromHead();
            pImageBuffer    = reinterpret_cast<ImageBuffer*>(pNode->pData);
#ifdef LE_CAMERA
            pImageBuffer->Destroy();
#else
            pImageBuffer->Destroy(&m_grallocInterface, m_pGralloc1Device);
#endif
            CHX_DELETE(pNode);
        }

        while (NULL != m_pBusyBufferList->Head())
        {
            if (TRUE == isPartialFree)
            {
                if ( (m_pFreeBufferList->NumNodes() + m_pBusyBufferList->NumNodes()) <= m_pBufferManagerData.immediateBufferCount)
                {
                    break;
                }
            }

            pNode           = m_pBusyBufferList->RemoveFromHead();
            pImageBuffer    = reinterpret_cast<ImageBuffer*>(pNode->pData);
#ifdef LE_CAMERA
            pImageBuffer->Destroy();
#else
            pImageBuffer->Destroy(&m_grallocInterface, m_pGralloc1Device);
#endif
            CHX_DELETE(pNode);
        }
    }
    else
    {
        CHX_LOG_ERROR("FreeBuffers failed! NULL buffer list");
    }

    m_pLock->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CHIBufferManager::Initialize(
    const CHAR*                 pBufferManagerName,
    CHIBufferManagerCreateData* pCreateData)
{
    CHX_ASSERT(NULL != pBufferManagerName);
    CHX_ASSERT(NULL != pCreateData);

    CDKResult                           result          = CDKResultSuccess;
    ImageBuffer*                        pImageBuffer    = NULL;
    LightweightDoublyLinkedListNode*    pNode           = NULL;

    CdkUtils::StrLCpy(m_pBufferManagerName, pBufferManagerName, sizeof(m_pBufferManagerName));

    if (NULL == pCreateData)
    {
        result = CDKResultEInvalidArg;
    }

#ifndef LE_CAMERA
    if (CDKResultSuccess == result)
    {
        result = SetupGralloc1Interface();
    }
#endif

    if (CDKResultSuccess == result)
    {
        m_pLock                 = Mutex::Create();
        m_pWaitFreeBuffer       = Condition::Create();
        m_pFreeBufferList       = CHX_NEW LightweightDoublyLinkedList();
        m_pBusyBufferList       = CHX_NEW LightweightDoublyLinkedList();
        CHX_ASSERT(NULL != m_pLock);
        CHX_ASSERT(NULL != m_pFreeBufferList);
        CHX_ASSERT(NULL != m_pBusyBufferList);

        if ((NULL != m_pLock) &&
            (NULL != m_pFreeBufferList) &&
            (NULL != m_pBusyBufferList))
        {
            m_pBufferManagerData = *pCreateData;

            // Allocate a small number of buffers as bare minimum initialization
            for (UINT i = 0; (i < m_pBufferManagerData.immediateBufferCount) && (i < m_pBufferManagerData.maxBufferCount); i++)
            {
#ifdef LE_CAMERA
                pImageBuffer = ImageBuffer::Create(m_pBufferManagerData.width,
                                                   m_pBufferManagerData.height,
                                                   m_pBufferManagerData.format,
                                                   m_pBufferManagerData.producerFlags,
                                                   m_pBufferManagerData.consumerFlags,
                                                   &m_pBufferManagerData.bufferStride);
#else
                pImageBuffer = ImageBuffer::Create(&m_grallocInterface,
                                                   m_pGralloc1Device,
                                                   m_pBufferManagerData.width,
                                                   m_pBufferManagerData.height,
                                                   m_pBufferManagerData.format,
                                                   m_pBufferManagerData.producerFlags,
                                                   m_pBufferManagerData.consumerFlags,
                                                   &m_pBufferManagerData.bufferStride);
#endif
                CHX_ASSERT(NULL != pImageBuffer);

                if (NULL != pImageBuffer)
                {
                    CHX_LOG("[%s] ImageBuffer created = %p", m_pBufferManagerName, pImageBuffer);

                    pNode = reinterpret_cast<LightweightDoublyLinkedListNode*>(
                        CHX_CALLOC(sizeof(LightweightDoublyLinkedListNode)));

                    CHX_ASSERT(NULL != pNode);

                    if (NULL != pNode)
                    {
                        pNode->pData = pImageBuffer;
                        m_pFreeBufferList->InsertToTail(pNode);
                    }
                }
            }
        }
    }

    return result;
}

#ifndef LE_CAMERA
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::SetupGralloc1Interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CHIBufferManager::SetupGralloc1Interface()
{
    CDKResult result = CDKResultSuccess;
#if defined (_LINUX)
    hw_get_module(GRALLOC_HARDWARE_MODULE_ID, const_cast<const hw_module_t**>(&m_pHwModule));
#endif

    if (NULL != m_pHwModule)
    {
        gralloc1_open(m_pHwModule, &m_pGralloc1Device);
    }
    else
    {
        result = CDKResultEFailed;
    }

    if (NULL != m_pGralloc1Device)
    {
        m_grallocInterface.CreateDescriptor = reinterpret_cast<GRALLOC1_PFN_CREATE_DESCRIPTOR>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_CREATE_DESCRIPTOR));
        m_grallocInterface.DestroyDescriptor = reinterpret_cast<GRALLOC1_PFN_DESTROY_DESCRIPTOR>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_DESTROY_DESCRIPTOR));
        m_grallocInterface.SetDimensions = reinterpret_cast<GRALLOC1_PFN_SET_DIMENSIONS>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_SET_DIMENSIONS));
        m_grallocInterface.SetFormat = reinterpret_cast<GRALLOC1_PFN_SET_FORMAT>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_SET_FORMAT));
        m_grallocInterface.SetProducerUsage = reinterpret_cast<GRALLOC1_PFN_SET_PRODUCER_USAGE>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_SET_PRODUCER_USAGE));
        m_grallocInterface.SetConsumerUsage = reinterpret_cast<GRALLOC1_PFN_SET_CONSUMER_USAGE>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_SET_CONSUMER_USAGE));
        m_grallocInterface.Allocate = reinterpret_cast<GRALLOC1_PFN_ALLOCATE>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_ALLOCATE));
        m_grallocInterface.GetStride = reinterpret_cast<GRALLOC1_PFN_GET_STRIDE>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_GET_STRIDE));
        m_grallocInterface.Release = reinterpret_cast<GRALLOC1_PFN_RELEASE>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_RELEASE));
        m_grallocInterface.Lock = reinterpret_cast<GRALLOC1_PFN_LOCK>(
            m_pGralloc1Device->getFunction(m_pGralloc1Device,
                                           GRALLOC1_FUNCTION_LOCK));
    }
    else
    {
        result = CDKResultEFailed;
    }
    return result;
}
#endif // LE_CAMERA

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::GetImageBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageBuffer* CHIBufferManager::GetImageBuffer()
{
    m_pLock->Lock();

    LightweightDoublyLinkedListNode*    pNode   = NULL;
    ImageBuffer*                        pBuffer = NULL;

    // Check the free List for an available buffer
    if (NULL != m_pFreeBufferList->Head())
    {
        pNode = m_pFreeBufferList->RemoveFromHead();
    }

    // If no free buffers were found in the free list,
    // we check to see if an additional buffer can be allocated immediately.
    if (NULL == pNode)
    {
        UINT numOfFreeBuffers = m_pFreeBufferList->NumNodes();
        UINT numOfBusyBuffers = m_pBusyBufferList->NumNodes();

        if ((numOfFreeBuffers + numOfBusyBuffers) < m_pBufferManagerData.maxBufferCount)
        {
#ifdef LE_CAMERA
            ImageBuffer* pNewImageBuffer = ImageBuffer::Create(m_pBufferManagerData.width,
                                               m_pBufferManagerData.height,
                                               m_pBufferManagerData.format,
                                               m_pBufferManagerData.producerFlags,
                                               m_pBufferManagerData.consumerFlags,
                                               &m_pBufferManagerData.bufferStride);
#else
            ImageBuffer* pNewImageBuffer = ImageBuffer::Create(&m_grallocInterface,
                                                               m_pGralloc1Device,
                                                               m_pBufferManagerData.width,
                                                               m_pBufferManagerData.height,
                                                               m_pBufferManagerData.format,
                                                               m_pBufferManagerData.producerFlags,
                                                               m_pBufferManagerData.consumerFlags,
                                                               &m_pBufferManagerData.bufferStride);
#endif
            CHX_ASSERT(NULL != pNewImageBuffer);

            if (NULL != pNewImageBuffer)
            {
                CHX_LOG("[%s] ImageBuffer created = %p", m_pBufferManagerName, pNewImageBuffer);

                pNode = reinterpret_cast<LightweightDoublyLinkedListNode*>(
                    CHX_CALLOC(sizeof(LightweightDoublyLinkedListNode)));

                CHX_ASSERT(NULL != pNode);

                if (NULL != pNode)
                {
                    pNode->pData = pNewImageBuffer;
                }
            }
        }
    }

    // If no free buffers were found and no more buffers can be allocated,
    // we wait until a busy buffer becomes free
    if(NULL == pNode)
    {
        CDKResult result = CDKResultSuccess;

        result = m_pWaitFreeBuffer->TimedWait(m_pLock->GetNativeHandle(),
                                              WAIT_BUFFER_TIMEOUT);

        CHX_ASSERT(CDKResultSuccess == result);

        if(CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("[%s], wait for buffer timedout", m_pBufferManagerName);
        }
        else
        {
            // Try again to get buffer from free list
            if (NULL != m_pFreeBufferList->Head())
            {
                pNode = m_pFreeBufferList->RemoveFromHead();
            }
        }
    }

    // Found a buffer. Increment it's reference count and add to the busy list.
    if (NULL != pNode)
    {
        pBuffer = reinterpret_cast<ImageBuffer*>(pNode->pData);
        pBuffer->AddReference();
        m_pBusyBufferList->InsertToTail(pNode);

        CHX_LOG("[%s] ImageBuffer = %p, Free buffers = %d, Busy buffers = %d",
                m_pBufferManagerName, pBuffer, m_pFreeBufferList->NumNodes(), m_pBusyBufferList->NumNodes());
    }
    else
    {
        CHX_LOG_ERROR("[%s] GetImageBuffer failed! Free buffers = %d, Busy buffers = %d",
                      m_pBufferManagerName, m_pFreeBufferList->NumNodes(), m_pBusyBufferList->NumNodes());
    }

    m_pLock->Unlock();

    return pBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::AddReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CHIBufferManager::AddReference(buffer_handle_t* pBufferHandle)
{
    CDKResult                           result              = CDKResultSuccess;
    LightweightDoublyLinkedListNode*    pImageBufferNode    = NULL;
    ImageBuffer*                        pImageBuffer        = NULL;

    if (NULL != pBufferHandle)
    {
        m_pLock->Lock();

        pImageBufferNode = LookupImageBuffer(pBufferHandle);

        if (NULL != pImageBufferNode)
        {
            pImageBuffer = reinterpret_cast<ImageBuffer*>(pImageBufferNode->pData);
            CHX_ASSERT(NULL != pImageBuffer);

            if (0 == pImageBuffer->GetReferenceCount())
            {
                m_pFreeBufferList->RemoveNode(pImageBufferNode);
                m_pBusyBufferList->InsertToTail(pImageBufferNode);
            }

            pImageBuffer->AddReference();
        }
        else
        {
            CHX_LOG_ERROR("[%s] AddReference failed, cannot find Image buffer for buffer handle %p",
                          m_pBufferManagerName, pBufferHandle);
            result = CDKResultEFailed;
        }

        m_pLock->Unlock();
    }
    else
    {
        CHX_LOG_ERROR("[%s] Buffer handle is NULL", m_pBufferManagerName);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::ReleaseReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult CHIBufferManager::ReleaseReference(buffer_handle_t* pBufferHandle)
{
    CDKResult                           result              = CDKResultSuccess;
    LightweightDoublyLinkedListNode*    pImageBufferNode    = NULL;
    ImageBuffer*                        pImageBuffer        = NULL;

    if (NULL != pBufferHandle)
    {
        m_pLock->Lock();

        pImageBufferNode = LookupImageBuffer(pBufferHandle);

        if (NULL != pImageBufferNode)
        {
            pImageBuffer = reinterpret_cast<ImageBuffer*>(pImageBufferNode->pData);
            CHX_ASSERT(NULL != pImageBuffer);

            pImageBuffer->ReleaseReference();

            // Move this unreferenced buffer to the free list
            if (0 == pImageBuffer->GetReferenceCount())
            {
                m_pBusyBufferList->RemoveNode(pImageBufferNode);
                m_pFreeBufferList->InsertToTail(pImageBufferNode);
                m_pWaitFreeBuffer->Signal();
            }
        }
        else
        {
            CHX_LOG_ERROR("[%s] ReleaseReference failed, cannot find Image buffer for buffer handle %p",
                          m_pBufferManagerName, pBufferHandle);
            result = CDKResultEFailed;
        }

        m_pLock->Unlock();
    }
    else
    {
        CHX_LOG_ERROR("[%s] Buffer handle is NULL", m_pBufferManagerName);
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::GetReference
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT CHIBufferManager::GetReference(buffer_handle_t* pBufferHandle)
{
    LightweightDoublyLinkedListNode*    pImageBufferNode    = NULL;
    ImageBuffer*                        pImageBuffer        = NULL;
    UINT                                refCount            = 0;

    if (NULL != pBufferHandle)
    {
        m_pLock->Lock();

        pImageBufferNode = LookupImageBuffer(pBufferHandle);

        if (NULL != pImageBufferNode)
        {
            pImageBuffer = reinterpret_cast<ImageBuffer*>(pImageBufferNode->pData);
            CHX_ASSERT(NULL != pImageBuffer);

            refCount = pImageBuffer->GetReferenceCount();
        }
        else
        {
            CHX_LOG_ERROR("[%s] GetReference failed, cannot find Image buffer for buffer handle %p",
                          m_pBufferManagerName, pBufferHandle);
        }

        m_pLock->Unlock();
    }
    else
    {
        CHX_LOG_ERROR("[%s] Buffer handle is NULL", m_pBufferManagerName);
    }

    return refCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHIBufferManager::LookupImageBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LightweightDoublyLinkedListNode* CHIBufferManager::LookupImageBuffer(
    buffer_handle_t* pBufferHandle)
{
    ImageBuffer*                        pBuffer     = NULL;
    UINT                                numNodes    = m_pBusyBufferList->NumNodes();
    LightweightDoublyLinkedListNode*    pNode       = m_pBusyBufferList->Head();
    BOOL                                found       = FALSE;

    // Go through busy list first
    for (UINT i = 0; i < numNodes; i++)
    {
        if (NULL == pNode)
        {
            break;
        }

        pBuffer = reinterpret_cast<ImageBuffer*>(pNode->pData);
        if (pBuffer->GetBufferHandle() == pBufferHandle)
        {
            found = TRUE;
            CHX_LOG("[%s] Found image buffer %p for handle %p in busy list",
                    m_pBufferManagerName, pBuffer, pBufferHandle);
            break;
        }
        pNode = m_pBusyBufferList->NextNode(pNode);
    }

    // If the image buffer is not found in busy list, go through free list then
    if (FALSE == found)
    {
        numNodes    = m_pFreeBufferList->NumNodes();
        pNode       = m_pFreeBufferList->Head();

        for (UINT i = 0; i < numNodes; i++)
        {
            if (NULL == pNode)
            {
                break;
            }

            pBuffer = reinterpret_cast<ImageBuffer*>(pNode->pData);
            if (pBuffer->GetBufferHandle() == pBufferHandle)
            {
                found = TRUE;
                CHX_LOG("[%s] Found image buffer %p for handle %p in free list",
                        m_pBufferManagerName, pBuffer, pBufferHandle);
                break;
            }
            pNode = m_pFreeBufferList->NextNode(pNode);
        }
    }

    if (FALSE == found)
    {
        pNode = NULL;
    }

    return pNode;
}

#ifdef LE_CAMERA
UINT GBMBufferAllocator::GetGbmFormat(uint32_t user_format) {
  UINT format = -1;

  switch (user_format) {
    case ChiStreamFormatBlob:
      format = GBM_FORMAT_BLOB;
      CHX_LOG("GBM_FORMAT_BLOB");
      break;

    case ChiStreamFormatYCbCr420_888:
      format = GBM_FORMAT_YCbCr_420_888;
      CHX_LOG("GBM_FORMAT_YCbCr_420_888");
      break;

    case ChiStreamFormatYCrCb420_SP:
      format = GBM_FORMAT_YCbCr_420_SP;
      CHX_LOG("GBM_FORMAT_YCbCr_420_888");
      break;

    case ChiStreamFormatImplDefined:
      format = GBM_FORMAT_IMPLEMENTATION_DEFINED;
      CHX_LOG("GBM_FORMAT_IMPLEMENTATION_DEFINED");
      break;

    case ChiStreamFormatRaw10:
      format = GBM_FORMAT_RAW10;
      CHX_LOG("GBM_FORMAT_RAW10");
      break;

    case ChiStreamFormatRaw16:
      format = GBM_FORMAT_RAW16;
      CHX_LOG("GBM_FORMAT_RAW16");
      break;

    case ChiStreamFormatRawOpaque:
      format = GBM_FORMAT_RAW_OPAQUE;
      CHX_LOG("GBM_FORMAT_RAW_OPAQUE");
      break;

    case ChiStreamFormatUBWCTP10:
      format = GBM_FORMAT_YCbCr_420_TP10_UBWC;
      CHX_LOG("GBM_FORMAT_YCbCr_420_TP10_UBWC");
      break;

    case ChiStreamFormatUBWCNV12:
      format = GBM_FORMAT_YCbCr_420_SP_VENUS_UBWC;
      CHX_LOG("GBM_FORMAT_YCbCr_420_SP_VENUS_UBWC");
      break;

    case ChiStreamFormatPD10:
      format = GBM_FORMAT_P010;
      CHX_LOG("GBM_FORMAT_P010");
      break;

    default:
      CHX_LOG("%s: Format:0x%x not supported\n", __func__, user_format);
      break;
  }

  return format;
}

GBMBufferAllocator::GBMBufferAllocator()
{
    deviceFD = open(DRM_DEVICE_NAME, O_RDWR);
    if (0 > deviceFD)
    {
        CHX_LOG_ERROR("unsupported device!!");
    }
    else
    {
        m_pGbmDevice = gbm_create_device(deviceFD);
        if (m_pGbmDevice != NULL &&
            gbm_device_get_fd(m_pGbmDevice) != deviceFD)
        {
            CHX_LOG_ERROR("unable to create GBM device!! ");
            gbm_device_destroy(m_pGbmDevice);
            m_pGbmDevice = NULL;
        }
    }
}

GBMBufferAllocator::~GBMBufferAllocator()
{
    if (NULL != m_pGbmDevice)
    {
        gbm_device_destroy(m_pGbmDevice);
    }

    if(deviceFD >= 0)
    {
        close(deviceFD);
    }
}

GBMBufferAllocator * GBMBufferAllocator::GetHandle()
{
    static GBMBufferAllocator GbmInstance;
    return &GbmInstance;
}

struct gbm_bo* GBMBufferAllocator::AllocateGbmBufferObj(UINT32              width,
                                                            UINT32              height,
                                                            UINT32              format,
                                                            UINT64              producerUsageFlags,
                                                            UINT64              consumerUsageFlags)
{
    SIZE_T bo_size                   = 0;
    UINT gbm_format                  = GBMBufferAllocator::GetGbmFormat(format);
    UINT gbm_usage                   = GBMBufferAllocator::GetGbmUsageFlag(gbm_format, consumerUsageFlags, producerUsageFlags);
    struct gbm_bo * m_pGbmBuffObject = NULL;

    if (-1 == gbm_format || -1 == gbm_usage)
    {
        CHX_LOG_ERROR("Invalid Argument");
        return NULL;
    }
    CHX_LOG("gbm format = %x, gbm usage = %x", gbm_format, gbm_usage);

    m_pGbmBuffObject = gbm_bo_create(m_pGbmDevice, width, height,gbm_format, gbm_usage);
    if (NULL == m_pGbmBuffObject)
    {
        CHX_LOG_ERROR("failed to create GBM object !!");
        return NULL;
    }
    CHX_LOG("FD = %d Format: %x => width: %d height:%d stride:%d ",
                                        gbm_bo_get_fd(m_pGbmBuffObject),
                                        gbm_bo_get_format(m_pGbmBuffObject),
                                        gbm_bo_get_width(m_pGbmBuffObject),
                                        gbm_bo_get_height(m_pGbmBuffObject),
                                        gbm_bo_get_stride(m_pGbmBuffObject));
    return m_pGbmBuffObject;
}

buffer_handle_t GBMBufferAllocator::AllocateNativeHandle(struct gbm_bo *bo)
{
    CDKResult results               = CDKResultSuccess;
    native_handle_t* p_handle      = NULL;
    if(bo != NULL)
    {
        p_handle = native_handle_create(1, 1);
        p_handle->data[0] = gbm_bo_get_fd(bo);
        p_handle->data[1] = reinterpret_cast<int>(bo);
        if(NULL == p_handle)
        {
            CHX_LOG_ERROR("Invalid native handle");
        }
    }
    return static_cast<buffer_handle_t>(p_handle);
}

void GBMBufferAllocator::FreeGbmBufferObj(struct gbm_bo *m_pGbmBuffObject)
{
    if(m_pGbmBuffObject != NULL)
    {
        gbm_bo_destroy(m_pGbmBuffObject);
    }
}

UINT GBMBufferAllocator::GetGbmUsageFlag(uint32_t gbm_format, uint32_t cons_usage, uint32_t prod_usage)
{
    uint32_t gbm_usage  = -1;
    switch(gbm_format)
    {
        case GBM_FORMAT_IMPLEMENTATION_DEFINED:
            if ((cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA) &&
                    (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA))
            {
                gbm_usage = (GBM_BO_USAGE_CAMERA_READ_QTI|GBM_BO_USAGE_CAMERA_WRITE_QTI);
            }
            else if (cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_READ_QTI;
            }
            else if (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_WRITE_QTI;
            }
            break;

        case GBM_FORMAT_YCbCr_420_TP10_UBWC:
        case GBM_FORMAT_YCbCr_420_SP_VENUS_UBWC:
            gbm_usage =  GBM_BO_USAGE_UBWC_ALIGNED_QTI;
            break;

        case GBM_FORMAT_YCbCr_420_P010_UBWC:
            // To Do
            break;

        case GBM_FORMAT_YCbCr_420_888:
        case GBM_FORMAT_YCbCr_420_SP:
             if ((cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA) &&
                    (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA))
            {
                gbm_usage = (GBM_BO_USAGE_CAMERA_READ_QTI|GBM_BO_USAGE_CAMERA_WRITE_QTI);
            }
            else if (cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_READ_QTI;
            }
            else if (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_WRITE_QTI;
            }
            break;

        case GBM_FORMAT_RAW16:
        case GBM_FORMAT_RAW10:
             if ((cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA) &&
                    (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA))
            {
                gbm_usage = (GBM_BO_USAGE_CAMERA_READ_QTI|GBM_BO_USAGE_CAMERA_WRITE_QTI);
            }
            else if (cons_usage & GRALLOC1_CONSUMER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_READ_QTI;
            }
            else if (prod_usage & GRALLOC1_PRODUCER_USAGE_CAMERA)
            {
                gbm_usage = GBM_BO_USAGE_CAMERA_WRITE_QTI;
            }
            break;

       default:
        CHX_LOG_ERROR("Unsupported format = %x", gbm_format);
    }
    return gbm_usage;
}

#endif // LE_CAMERA
