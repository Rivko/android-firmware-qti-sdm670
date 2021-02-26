////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasetorch.h
/// @brief CHX basic camcorder declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASETORCH_H
#define CHXUSECASETORCH_H

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxusecaseutils.h"

// generated headers
#include "g_pipelines.h"

/// Forward declarations
struct ChiPipelineTargetCreateDescriptor;
class  Session;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Torch widget usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseTorch : public Usecase
{
public:
    /// Static create function to create an instance of the object
    static UsecaseTorch* Create(
        UINT32                          cameraId,       ///< [In] Camera Id
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration;
protected:
    /// Destroy/Cleanup the object
    virtual VOID Destroy(BOOL isForced);

private:
    UsecaseTorch() = default;
    virtual ~UsecaseTorch() = default;

    // Do not allow the copy constructor or assignment operator
    UsecaseTorch(const UsecaseTorch&) = delete;
    UsecaseTorch& operator= (const UsecaseTorch&) = delete;

    static VOID SessionCbCaptureResult(
        ChiCaptureResult* pCaptureResult,                       ///< Capture result
        VOID*             pPrivateCallbackData);                ///< Private callback data

    static VOID SessionCbNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor,         ///< Message Descriptor
        VOID*                       pPrivateCallbackData);      ///< Private callback data

    /// Execute capture request
    CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    /// Generate a Chi request from HAL request
    CDKResult SubmitChiRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    /// To flush the session
    CDKResult Flush();

    // Implemented by the derived class to process the saved results
    virtual VOID ProcessResults() { }

    /// Does one time initialization of the created object
    CDKResult Initialize(
        UINT32                          cameraId,       ///< [In] Camera Id
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration

    struct SessionPrivateData
    {
        Usecase* pUsecase;  ///< Per usecase class
        UINT32   sessionId; ///< Session Id that is meaningful to the usecase in which the session belongs
    };

    SessionPrivateData m_torchSessionPvtData;    ///< Per session private data
    Session*           m_pTorchSession;          ///< torch Session handle
    Pipeline*          m_pTorchPipeline;         ///< Pipeline handle
    camera3_stream_t*  m_pTorchStream;           ///< Stream for torch
    BufferManager*     m_pTorchBufferManager;    ///< buffer manager for torch pipeline output
    Mutex*             m_pTorchResultMutex;      ///< Torch process capture Result availability mutex
    Condition*         m_pTorchResultAvailable;  ///< Wait for availablability of result.
};

#endif // CHXUSECASETORCH_H
