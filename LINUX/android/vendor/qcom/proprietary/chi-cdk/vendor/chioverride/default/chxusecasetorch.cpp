////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasetorch.cpp
/// @brief Torch Usecase class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxusecasetorch.h"
#include "chxutils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::Create
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UsecaseTorch* UsecaseTorch::Create(
    UINT32                          cameraId,       ///< Camera Id
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    CDKResult     result        = CDKResultSuccess;
    UsecaseTorch* pUsecaseTorch = CHX_NEW UsecaseTorch;

    if (NULL != pUsecaseTorch)
    {
        result = pUsecaseTorch->Initialize(cameraId, pStreamConfig);

        if (CDKResultSuccess != result)
        {
            pUsecaseTorch->Destroy(FALSE);
            pUsecaseTorch = NULL;
        }
    }
    else
    {
        result = CDKResultEFailed;
    }

    return pUsecaseTorch;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::Destroy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseTorch::Destroy(BOOL isForced)
{
    CHX_LOG("UsecaseTorch::Destroy(), isForced %d E.", isForced);

    if (NULL != m_pTorchSession)
    {
        m_pTorchSession->Destroy(isForced);
        m_pTorchSession = NULL;
    }

    if (NULL != m_pTorchPipeline)
    {
        m_pTorchPipeline->Destroy();
        m_pTorchPipeline = NULL;
    }


    if (m_pTorchBufferManager != NULL)
    {
        m_pTorchBufferManager->Destroy();
        m_pTorchBufferManager = NULL;
    }

    if (NULL != m_pTorchResultMutex)
    {
        m_pTorchResultMutex->Destroy();
        m_pTorchResultMutex = NULL;
    }

    if (NULL != m_pTorchResultAvailable)
    {
        m_pTorchResultAvailable->Destroy();
        m_pTorchResultAvailable = NULL;
    }

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
/// UsecaseTorch::Initialize
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseTorch::Initialize(
    UINT32                          cameraId,       ///< Camera Id
    camera3_stream_configuration_t* pStreamConfig)  ///< Stream configuration
{
    CHX_ASSERT(StreamConfigModeQTITorchWidget == pStreamConfig->operation_mode);

    CDKResult               result = CDKResultSuccess;
    ChiPortBufferDescriptor pipelineOutputBuffer;

    ChxUtils::Memset(&pipelineOutputBuffer, 0, sizeof(pipelineOutputBuffer));

    Usecase::Initialize();

    m_pTorchResultMutex        = Mutex::Create();
    m_pTorchResultAvailable    = Condition::Create();
    m_cameraId                 = cameraId;
    m_usecaseId                = UsecaseId::Torch;

    m_pChiUsecase = UsecaseSelector::DefaultMatchingUsecase(pStreamConfig);

    if (NULL != m_pChiUsecase)
    {
        CHX_LOG("USECASE Name: %s, ", m_pChiUsecase->pUsecaseName);

        // Create a Dummy Bufer manager and buffers for Torch.
        BufferManagerCreateData createData         ={ 0 };

        createData.width         = 176;
        createData.height        = 144;
        createData.format        = HAL_PIXEL_FORMAT_YCbCr_420_888;
        createData.producerFlags = GRALLOC1_PRODUCER_USAGE_CAMERA;
        createData.consumerFlags = GRALLOC1_CONSUMER_USAGE_CAMERA;
        createData.numBuffers    = 2;

        m_pTorchBufferManager    = BufferManager::Create(&createData);
        m_pTorchStream           = pStreamConfig->streams[0];

        ChiTargetPortDescriptorInfo* pSinkTarget     = &m_pChiUsecase->pPipelineTargetCreateDesc[0].sinkTarget;
        ChiTargetPortDescriptor*     pSinkTargetDesc = &pSinkTarget->pTargetPortDesc[0];

        pipelineOutputBuffer.pStream  = pSinkTargetDesc->pTarget->pChiStream;
        pipelineOutputBuffer.nodePort = pSinkTargetDesc->nodeport;

        // Create pipeline for torch usecase
        m_pTorchPipeline = Pipeline::Create(cameraId, PipelineType::Default);

        if (NULL != m_pTorchPipeline)
        {
            m_pTorchPipeline->SetOutputBuffers(pSinkTarget->numTargets, &pipelineOutputBuffer);
            m_pTorchPipeline->SetPipelineNodePorts(&m_pChiUsecase->pPipelineTargetCreateDesc[0].pipelineCreateDesc);
            result = m_pTorchPipeline->CreateDescriptor();

            if (CDKResultSuccess == result)
            {
                ChiCallBacks callbacks = { 0 };

                callbacks.ChiNotify               = SessionCbNotifyMessage;
                callbacks.ChiProcessCaptureResult = SessionCbCaptureResult;

                m_torchSessionPvtData.sessionId  = 0;
                m_torchSessionPvtData.pUsecase   = this;

                m_pTorchSession = Session::Create(&m_pTorchPipeline, 1, &callbacks, &m_torchSessionPvtData);

                if (NULL == m_pTorchSession)
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
/// UsecaseTorch::ExecuteCaptureRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseTorch::ExecuteCaptureRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult result = CDKResultSuccess;

    result = SubmitChiRequest(pRequest);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::SessionCbNotifyMessage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseTorch::SessionCbNotifyMessage(
    const ChiMessageDescriptor* pMessageDescriptor,
    VOID*                       pPrivateCallbackData)
{
    // no op for torch usecase
    (VOID) pMessageDescriptor;
    (VOID) pPrivateCallbackData;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::SessionCbCaptureResult
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID UsecaseTorch::SessionCbCaptureResult(
    ChiCaptureResult* pResult,
    VOID*             pPrivateCallbackData)
{
    (VOID) pResult;
    SessionPrivateData* pSessionPrivateData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
    UsecaseTorch*       pUsecaseTorch       = static_cast<UsecaseTorch*>(pSessionPrivateData->pUsecase);

    // Signal the request submission context, as it has a wait on this result.
    pUsecaseTorch->m_pTorchResultMutex->Lock();
    pUsecaseTorch->m_pTorchResultAvailable->Signal();
    pUsecaseTorch->m_pTorchResultMutex->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::SubmitChiRequest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseTorch::SubmitChiRequest(
    camera3_capture_request_t* pRequest)
{
    CDKResult         result     = CDKResultSuccess;
    CHICAPTUREREQUEST chiRequest = { 0 };
    VOID*             pMetaData  = NULL;
    camera3_stream_buffer camStreamBuffer[1];


    if (NULL == m_pTorchSession)
    {
        CHX_LOG_ERROR("m_pTorchSession NULL while submitting CHI request");
        result = CDKResultEInvalidPointer;
    }

    if (result == CDKResultSuccess)
    {
        chiRequest.frameNumber       = pRequest->frame_number;
        chiRequest.hPipelineHandle   = reinterpret_cast<CHIPIPELINEHANDLE>(m_pTorchSession->GetPipelineHandle());
        chiRequest.numOutputs        = pRequest->num_output_buffers;

        camStreamBuffer[0].acquire_fence = -1;
        camStreamBuffer[0].buffer        = m_pTorchBufferManager->GetBuffer();
        camStreamBuffer[0].stream        = m_pTorchStream;
        chiRequest.pOutputBuffers        =
            reinterpret_cast<CHISTREAMBUFFER*>(const_cast<camera3_stream_buffer*>(camStreamBuffer));
        chiRequest.pMetadata             = pRequest->settings;

        chiRequest.pPrivData               =  &m_privData[chiRequest.frameNumber % MaxOutstandingRequests];
        chiRequest.pPrivData->requestIndex = 0;

        CHIPIPELINEREQUEST submitRequest;

        submitRequest.pSessionHandle   = reinterpret_cast<CHIHANDLE>(m_pTorchSession->GetSessionHandle());
        submitRequest.numRequests      = 1;
        submitRequest.pCaptureRequests = &chiRequest;

        if (FALSE == m_pTorchSession->IsPipelineActive())
        {
            result = ExtensionModule::GetInstance()->ActivatePipeline(m_pTorchSession->GetSessionHandle(),
                                                                      m_pTorchSession->GetPipelineHandle());
            if (CDKResultSuccess == result)
            {
                m_pTorchSession->SetPipelineActivateFlag();
            }
        }

        // Wait for the result of this request. Give Torch sometime to process.
        m_pTorchResultMutex->Lock();
        result = ExtensionModule::GetInstance()->SubmitRequest(&submitRequest);
        CHX_LOG("Submitted request for torch frameNumber: %" PRIu64, chiRequest.frameNumber);
        m_pTorchResultAvailable->Wait(m_pTorchResultMutex->GetNativeHandle());
        m_pTorchResultMutex->Unlock();
    }


    ChxUtils::FreeMetaData(pMetaData);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// UsecaseTorch::Flush
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDKResult UsecaseTorch::Flush()
{
    if (NULL != m_pTorchSession)
    {
        ExtensionModule::GetInstance()->Flush(reinterpret_cast<CHIHANDLE>(m_pTorchSession));
    }
    return CDKResultSuccess;
}
