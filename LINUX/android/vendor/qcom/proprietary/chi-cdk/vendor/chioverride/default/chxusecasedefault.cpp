////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasedefault.cpp
/// @brief Default Usecase class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxusecasedefault.h"
#include "chistatspropertydefines.h"
#include "chxutils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::~UsecaseDefault
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseDefault::~UsecaseDefault()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseDefault* UsecaseDefault::Create(
    UINT32                          cameraId,       ///< Camera Id
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    CDKResult       result          = CDKResultSuccess;
    UsecaseDefault* pUsecaseDefault = CHX_NEW UsecaseDefault;

    if (NULL != pUsecaseDefault)
    {
        result = pUsecaseDefault->Initialize(cameraId, pStreamConfig);

        if (CDKResultSuccess != result)
        {
            pUsecaseDefault->Destroy(FALSE);
            pUsecaseDefault = NULL;
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return pUsecaseDefault;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseDefault::Destroy(BOOL isForced)
{
    for (UINT i = 0 ; i < MaxSessions; i++)
    {
        if (NULL != m_pSession[i])
        {
            m_pSession[i]->Destroy(isForced);
            m_pSession[i] = NULL;
        }
    }

    for (UINT i = 0 ; i < MaxPipelinesPerSession; i++)
    {
        if (NULL != m_pPipeline[i])
        {
            m_pPipeline[i]->Destroy();
            m_pPipeline[i] = NULL;
        }
    }

    if (m_pTorchBufferManager != NULL)
    {
        m_pTorchBufferManager->Destroy();
        m_pTorchBufferManager = NULL;
    }


    /// @todo Enable Multithreading
    // Wait till all results are processed
    // m_pAllResultsMutex->Lock();

    // while (FALSE == m_allResultsAvailable)
    // {
    //     m_pAllResultsAvailable->Wait(m_pAllResultsMutex->GetNativeHandle());
    // }

    // m_pAppResultMutex->Lock();
    // m_appResultThreadTerminate = TRUE;
    // m_pAppResultMutex->Unlock();

    // m_pAllResultsMutex->Unlock();
    // m_pAppResultAvailable->Signal();

    // ChxUtils::ThreadTerminate(m_resultProcessingThread.hThreadHandle);

    /// @todo Enable Multithreading: Remove sleep when we enable multithreading
    // while (m_nextAppResultFrame <= m_lastAppRequestFrame)
    // {
    //     ChxUtils::SleepMicroseconds(2000);
    // }

    if (NULL != m_pAppResultMutex)
    {
        m_pAppResultMutex->Destroy();
        m_pAppResultMutex = NULL;
    }

    if (NULL != m_pAllResultsMutex)
    {
        m_pAllResultsMutex->Destroy();
        m_pAllResultsMutex = NULL;
    }

    if (NULL != m_pAppResultAvailable)
    {
        m_pAppResultAvailable->Destroy();
        m_pAppResultAvailable = NULL;
    }

    if (NULL != m_pAllResultsAvailable)
    {
        m_pAllResultsAvailable->Destroy();
        m_pAllResultsAvailable = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseDefault::Initialize(
    UINT32                          cameraId,       ///< Camera Id
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    CDKResult               result = CDKResultSuccess;
    ChiPortBufferDescriptor pipelineOutputBuffer[MaxChiStreams];
    ChiPortBufferDescriptor pipelineInputBuffer[MaxChiStreams];

    ChxUtils::Memset(pipelineOutputBuffer, 0, sizeof(pipelineOutputBuffer));
    ChxUtils::Memset(pipelineInputBuffer, 0, sizeof(pipelineInputBuffer));

    Usecase::Initialize();

    m_cameraId                   = cameraId;
    m_usecaseId                  = UsecaseId::Default;
    m_lastResultMetadataFrameNum = -1;
    m_effectModeValue            = ANDROID_CONTROL_EFFECT_MODE_OFF;
    m_sceneModeValue             = ANDROID_CONTROL_SCENE_MODE_DISABLED;
    m_tuningFeature1Value        = 0;
    m_tuningFeature2Value        = 0;

    m_pChiUsecase = UsecaseSelector::DefaultMatchingUsecase(pStreamConfig);
    m_torchMode   = FALSE;

    if (NULL != m_pChiUsecase)
    {
        CHX_LOG("USECASE Name: %s, pipelines: %d, numTargets: %d",
            m_pChiUsecase->pUsecaseName, m_pChiUsecase->numPipelines, m_pChiUsecase->numTargets);

        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            CHX_LOG("PIPELINE[%d] Name: %s, Source Target num: %d, Sink Target Num: %d",
                i,
                m_pChiUsecase->pPipelineTargetCreateDesc[i].pPipelineName,
                m_pChiUsecase->pPipelineTargetCreateDesc[i].sourceTarget.numTargets,
                m_pChiUsecase->pPipelineTargetCreateDesc[i].sinkTarget.numTargets);
        }

        if (StreamConfigModeQTITorchWidget == pStreamConfig->operation_mode)
        {
                // Create a Dummy Bufer manager and buffers for Torch.
                BufferManagerCreateData createData         ={ 0 };

                createData.width         = 176;
                createData.height        = 144;
                createData.format        = HAL_PIXEL_FORMAT_YCbCr_420_888;
                createData.producerFlags = GRALLOC1_PRODUCER_USAGE_CAMERA;
                createData.consumerFlags = GRALLOC1_CONSUMER_USAGE_CAMERA;
                createData.numBuffers    = 2;

                m_pTorchBufferManager    = BufferManager::Create(&createData);

                m_torchMode              = TRUE;
                m_pTorchStream           = pStreamConfig->streams[0];
        }

        for (UINT stream = 0; stream < pStreamConfig->num_streams; stream++)
        {
            CHX_LOG("stream = %p streamType = %d streamFormat = %d streamWidth = %d streamHeight = %d",
                    pStreamConfig->streams[stream],
                    pStreamConfig->streams[stream]->stream_type,
                    pStreamConfig->streams[stream]->format,
                    pStreamConfig->streams[stream]->width,
                    pStreamConfig->streams[stream]->height);
        }

        for (UINT i = 0; i < m_pChiUsecase->numPipelines; i++)
        {
            ChiTargetPortDescriptorInfo* pSinkTarget = &m_pChiUsecase->pPipelineTargetCreateDesc[i].sinkTarget;
            ChiTargetPortDescriptorInfo* pSrcTarget  = &m_pChiUsecase->pPipelineTargetCreateDesc[i].sourceTarget;

            for (UINT sinkIdx = 0; sinkIdx < pSinkTarget->numTargets; sinkIdx++)
            {
                ChiTargetPortDescriptor* pSinkTargetDesc = &pSinkTarget->pTargetPortDesc[sinkIdx];

                pipelineOutputBuffer[sinkIdx].pStream  = pSinkTargetDesc->pTarget->pChiStream;
                pipelineOutputBuffer[sinkIdx].nodePort = pSinkTargetDesc->nodeport;
            }

            for (UINT sourceIdx = 0; sourceIdx < pSrcTarget->numTargets; sourceIdx++)
            {
                ChiTargetPortDescriptor* pSrcTargetDesc = &pSrcTarget->pTargetPortDesc[sourceIdx];

                pipelineInputBuffer[sourceIdx].pStream  = pSrcTargetDesc->pTarget->pChiStream;
                pipelineInputBuffer[sourceIdx].nodePort = pSrcTargetDesc->nodeport;
            }

            m_pPipeline[i] = Pipeline::Create(cameraId, PipelineType::Default);

            if (NULL != m_pPipeline[i])
            {
                m_pPipeline[i]->SetOutputBuffers(pSinkTarget->numTargets, &pipelineOutputBuffer[0]);
                m_pPipeline[i]->SetInputBuffers(pSrcTarget->numTargets, &pipelineInputBuffer[0]);
                m_pPipeline[i]->SetPipelineNodePorts(&m_pChiUsecase->pPipelineTargetCreateDesc[i].pipelineCreateDesc);

                result = m_pPipeline[i]->CreateDescriptor();

                /// @note support only two pipelines for the default usecase
                if (TRUE == m_pPipeline[i]->IsRealTime())
                {
                    m_realTimeIdx = i;
                }
                else
                {
                    m_offlineIdx = i;
                }
            }

            if (CDKResultSuccess == result)
            {
                ChiCallBacks callbacks = { 0 };

                callbacks.ChiNotify               = SessionCbNotifyMessage;
                callbacks.ChiProcessCaptureResult = SessionCbCaptureResult;

                m_perSessionPvtData[i].sessionId  = i;
                m_perSessionPvtData[i].pUsecase   = this;

                m_pSession[i] = Session::Create(&m_pPipeline[i], 1, &callbacks, &m_perSessionPvtData[i]);

                if (NULL == m_pSession[i])
                {
                    result = CDKResultEFailed;
                }
            }
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseDefault::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result = CDKResultSuccess;

    if (FALSE == m_torchMode)
    {
        m_selectedSensorModeIndex = m_pPipeline[m_realTimeIdx]->GetSensorModeInfo()->modeIndex;
        result                    = UpdateSensorModeIndex(const_cast<camera_metadata_t*>(pRequest->settings));
        if (CDKResultSuccess != result)
        {
            CHX_LOG_ERROR("Failed to update sensor mode index");
        }
    }
    CHX_LOG("Default usecase capture request for frame %d with %d output buffers",
            pRequest->frame_number, pRequest->num_output_buffers);

    result = SubmitChiRequest(pRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::SessionCbNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseDefault::SessionCbNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    /// @todo Provide common Get functions to do the below two casts
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseDefault*     pUsecaseDefault     = static_cast<UsecaseDefault*>(pSessionPrivateData->pUsecase);

    if (ChiMessageTypeSof == pMessageDescriptor->messageType)
    {
        // SOF notifications are not sent to the HAL3 application
        CHX_LOG("Chi Notify SOF frameNum %u framework frameNum %u, timestamp %" PRIu64,
            pMessageDescriptor->message.sofMessage.sofId,
            pMessageDescriptor->message.sofMessage.frameworkFrameNum,
            pMessageDescriptor->message.sofMessage.timestamp);
    }
    else
    {
        if (FALSE == pUsecaseDefault->m_torchMode)
        {
            pUsecaseDefault->ReturnFrameworkMessage((camera3_notify_msg_t*)pMessageDescriptor,
                pUsecaseDefault->m_cameraId);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::SessionCbCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseDefault::SessionCbCaptureResult(
    ChiCaptureResult* pResult,
    VOID*             pPrivateCallbackData)
{
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseDefault*     pUsecaseDefault     = static_cast<UsecaseDefault*>(pSessionPrivateData->pUsecase);

    if (FALSE == pUsecaseDefault->m_torchMode)
    {
        pUsecaseDefault->ReturnFrameworkResult(reinterpret_cast<const camera3_capture_result_t*>(pResult),
            pUsecaseDefault->m_cameraId);
        pUsecaseDefault->ProcessCaptureResult(pResult);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::ProcessCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseDefault::ProcessCaptureResult(
    ChiCaptureResult* pResult)
{
    UINT32 frameIndex = (pResult->frameworkFrameNum % MaxOutstandingRequests);

    m_numAppPendingOutputBuffers[frameIndex] -= pResult->numOutputBuffers;

    // Moving forward if there is no pending output buffer and metadata
    if (0 == m_numAppPendingOutputBuffers[frameIndex])
    {
        m_nextAppResultFrame++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::SubmitChiRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseDefault::SubmitChiRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult         result     = CDKResultSuccess;
    CHICAPTUREREQUEST chiRequest = { 0 };
    UINT              sessionIdx = 0;
    VOID*             pMetaData  = NULL;
    camera3_stream_buffer camStreamBuffer[1];

    if (NULL == pRequest->input_buffer)
    {
        sessionIdx = m_realTimeIdx;
    }
    else
    {
        chiRequest.numInputs     = 1;
        chiRequest.pInputBuffers = reinterpret_cast<CHISTREAMBUFFER*>(pRequest->input_buffer);
        sessionIdx               = m_offlineIdx;
    }

    if ((pRequest->settings != NULL) &&
        (FALSE == m_torchMode))
    {
        UINT32 sensorModeIndex = m_pSession[sessionIdx]->GetSensorModeInfo()->modeIndex;
        pMetaData = ChxUtils::AllocateAppendMetaData(reinterpret_cast<const VOID*>(pRequest->settings), NumVendorTags, 1024);
        ChxUtils::FillTuningModeData(pMetaData,pRequest,sensorModeIndex,&m_effectModeValue,&m_sceneModeValue,
            &m_tuningFeature1Value, &m_tuningFeature2Value);
    }

    if (NULL == m_pSession[sessionIdx])
    {
        CHX_LOG_ERROR(" Error submitting CHI request m_pSession[%d]= NULL", sessionIdx);
        result = CDKResultEInvalidPointer;
    }

    if (result == CDKResultSuccess)
    {
        chiRequest.frameNumber       = pRequest->frame_number;
        chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pSession[sessionIdx]->GetPipelineHandle());
        chiRequest.numOutputs        = pRequest->num_output_buffers;
        if (FALSE == m_torchMode)
        {
            chiRequest.pOutputBuffers    =
                reinterpret_cast<CHISTREAMBUFFER*>
                    (const_cast<camera3_stream_buffer*>(pRequest->output_buffers));
            chiRequest.pMetadata         = pMetaData;
        }
        else
        {
            camStreamBuffer[0].acquire_fence = -1;
            camStreamBuffer[0].buffer        = m_pTorchBufferManager->GetBuffer();
            camStreamBuffer[0].stream        = m_pTorchStream;
            chiRequest.pOutputBuffers        =
                reinterpret_cast<CHISTREAMBUFFER*>(const_cast<camera3_stream_buffer*>(camStreamBuffer));
            chiRequest.pMetadata             = pRequest->settings;
        }
        chiRequest.pPrivData               =  &m_privData[chiRequest.frameNumber % MaxOutstandingRequests];
        chiRequest.pPrivData->requestIndex = sessionIdx;

        CHIPIPELINEREQUEST submitRequest;

        submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(m_pSession[sessionIdx]->GetSessionHandle());
        submitRequest.numRequests      = 1;
        submitRequest.pCaptureRequests = &chiRequest;

        if (FALSE == m_pSession[sessionIdx]->IsPipelineActive())
        {
            if (((m_realTimeIdx                                            == sessionIdx) &&
               (ExtensionModule::GetInstance()->GetNumPCRsBeforeStreamOn(const_cast<camera_metadata_t*>(pRequest->settings))
                   == pRequest->frame_number)) ||
               (m_offlineIdx                                               == sessionIdx))
            {
                CHX_LOG_ERROR(" Activate pipeline req %d, session idx %d", pRequest->frame_number, sessionIdx);
                result = ExtensionModule::GetInstance()->ActivatePipeline(m_pSession[sessionIdx]->GetSessionHandle(),
                                                                          m_pSession[sessionIdx]->GetPipelineHandle());
                if (CDKResultSuccess == result)
                {
                    m_pSession[sessionIdx]->SetPipelineActivateFlag();
                }
            }
            else
            {
                CHX_LOG_ERROR(" Activate didnt happen yet pipeline req %d, session idx %d",
                              pRequest->frame_number, sessionIdx);
            }
        }
        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
    }

    if (TRUE == m_torchMode)
    {
        /// @todo Wait for 100ms for the Torch to process. Check back if this can be avoided.
        ChxUtils::SleepMicroseconds(100000);
    }

    ChxUtils::FreeMetaData(pMetaData);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseDefault::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseDefault::Flush()
{
    Usecase::FlushLock(TRUE);
    for (UINT i = 0; i < MaxSessions; i++)
    {
        if (NULL != m_pSession[i])
        {
            ExtensionModule::GetInstance()->Flush(reinterpret_cast<CHIHANDLE>(m_pSession[i]));
        }
    }
    Usecase::FlushLock(FALSE);
    return CDKResultSuccess;
}
