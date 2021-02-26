////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasedefault.h
/// @brief CHX basic camcorder declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASES_H
#define CHXUSECASES_H

#include <assert.h>

#include "chituningmodeparam.h"
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
/// @brief Preview+Snapshot ZSL usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseDefault final : public Usecase
{
public:
    /// Static create function to create an instance of the object
    static UsecaseDefault* Create(
        UINT32                          cameraId,       ///< [In] Camera Id
        camera3_stream_configuration_t* pStreamConfig); ///< Stream configuration;
protected:
    /// Destroy/Cleanup the object
    virtual VOID Destroy(BOOL isForced);

private:
    UsecaseDefault() = default;
    virtual ~UsecaseDefault();

    // Do not allow the copy constructor or assignment operator
    UsecaseDefault(const UsecaseDefault&) = delete;
    UsecaseDefault& operator= (const UsecaseDefault&) = delete;

    static VOID SessionCbCaptureResult(
        ChiCaptureResult* pCaptureResult,                       ///< Capture result
        VOID*             pPrivateCallbackData);                ///< Private callback data

    static VOID SessionCbNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor,         ///< Message Descriptor
        VOID*                       pPrivateCallbackData);      ///< Private callback data

    VOID ProcessCaptureResult(
        ChiCaptureResult* pCaptureResult);                      ///< Capture result

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

    SessionPrivateData m_perSessionPvtData[MaxSessions];                ///< Per session private data
    /// @todo Better organization according to pipeline info
    Session*           m_pSession[MaxSessions];                         ///< Session
    Pipeline*          m_pPipeline[MaxPipelinesPerSession];             ///< Pipelines
    UINT               m_realTimeIdx;                                   ///< Index of the real time pipeline/session
    UINT               m_offlineIdx;                                    ///< Index of the offline pipeline/session
    UINT32             m_effectModeValue;                               ///< effect value
    UINT32             m_sceneModeValue;                                ///< scenemode value
    UINT32             m_tuningFeature1Value;                           ///< tuning Feature1Value value
    UINT32             m_tuningFeature2Value;                           ///< tuning Feature2Value value
    camera3_stream_t*  m_pTorchStream;                                  ///< Stream for torch
    BufferManager*     m_pTorchBufferManager;                           ///< buffer manager for torch pipeline output
    BOOL               m_torchMode;                                     ///< torch mode indication
};

#endif // CHXUSECASES_H
