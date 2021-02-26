////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasedefault.h
/// @brief CHX basic camcorder declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASEQUADCFA_H
#define CHXUSECASEQUADCFA_H

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecase.h"
#include "chxusecaseutils.h"
#include "chistatspropertydefines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief QuadCFA usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseQuadCFA final : public Usecase
{
public:
    /// Static create function to create an instance of the object
    static UsecaseQuadCFA* Create(
        LogicalCameraInfo*              cameraInfo,             ///< Camera info
        camera3_stream_configuration_t* pStreamConfig);         ///< Stream configuration;

protected:
    /// Destroy/Cleanup the object
    virtual VOID Destroy(BOOL isForced);

private:
    UsecaseQuadCFA() = default;
    virtual ~UsecaseQuadCFA() = default;

    // Do not allow the copy constructor or assignment operator
    UsecaseQuadCFA(const UsecaseQuadCFA&) = delete;
    UsecaseQuadCFA& operator= (const UsecaseQuadCFA&) = delete;

    static VOID SessionCbCaptureResult(
        ChiCaptureResult* pResult,                              ///< Capture result
        VOID*             pPrivateCallbackData);                ///< Private callback data

    static VOID SessionCbNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor,         ///< Message Descriptor
        VOID*                       pPrivateCallbackData);      ///< Private callback data

    VOID SessionProcessMessage(
        const ChiMessageDescriptor* pMessageDescriptor);        ///< Message Descriptor

    CDKResult ProcessRdiResult(
        ChiCaptureResult* pResult);                             ///< Capture result

    CDKResult ProcessPreviewResult(
        ChiCaptureResult* pResult);                             ///< Capture result

    CDKResult ProcessSnapshotResult(
        ChiCaptureResult* pResult);                             ///< Capture result

    CDKResult ProcessYuvResult(
        ChiCaptureResult* pResult);                             ///< Capture result

    CDKResult UpdateSensorTimestamp(
         ChiCaptureResult* pResult);                            ///< Capture result

    /// Execute capture request
    CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    CDKResult GenerateSnapshotRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    CDKResult GenerateSnapshotYuvRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    CDKResult GenerateRDIRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    // Implemented by the derived class to process the saved results
    virtual VOID ProcessResults() { }

    /// Does one time initialization of the created object
    CDKResult Initialize(
        LogicalCameraInfo*              cameraInfo,             ///< Camera info
        camera3_stream_configuration_t* pStreamConfig);         ///< Stream configuration

    CDKResult CreateAllPipelineDescriptors();

    CDKResult CreatePipelineDescriptor(
        UINT32 pipelineIdx);                                    ///< pipeline index

    CDKResult StartPiplineAndSession(
        UINT32 pipelineIdx);                                    ///< pipeline index

    CDKResult DestroyPiplineAndSession(
        UINT32 pipelineIdx);                                    ///< pipeline index

    virtual CDKResult Flush();

    /// QCFARawQueue Queue
    struct QCFARawQueue
    {
        BOOL             isBufferValid;                         ///< Buffer valid flag
        UINT32           frameNumber;                           ///< Frame number
        VOID*            pMetadata;                             ///< Metadata
        CHISTREAMBUFFER* pRdiOutputBuffer;                      ///< RDI buffer
    };

    struct SessionPrivateData
    {
        Usecase* pUsecase;  ///< Per usecase class
        UINT32   sessionId; ///< Session Id that is meaningful to the usecase in which the session belongs
    };

    SessionPrivateData m_perSessionPvtData[MaxSessions];         ///< Per session private data

    /// @todo Better organization according to pipeline info
    Session*       m_pSession[MaxSessions];                      ///< Session
    Pipeline*      m_pPipeline[MaxPipelinesPerSession];          ///< Pipelines

    UINT           m_previewIdx;                                 ///< Index of the realtime preview pipeline/sesson
    UINT           m_QCFARawIdx;                                 ///< Index of the Quad CFA raw pipeline/session
    UINT           m_offlineIdx;                                 ///< Index of the offline pipeline/session
    UINT           m_OfflineYuvIdx;                              ///< Index of the offlineYuv pipeline/session

    CHISTREAM*     m_pQCFARawStream;                             ///< RDI stream
    CHISTREAM*     m_pPreviewStream;                             ///< Preivew stream
    CHISTREAM*     m_pSnapshotStream;                            ///< Snapshot stream
    CHISTREAM*     m_pYuvStream;                                 ///< Yuv stream
    CHISTREAM*     m_pYuvDummyStream;                            ///< Yuv dummy stream

    UINT64         m_shutterTimestamp[MaxOutstandingRequests];   ///< Is shutter message sent
    BOOL           m_isMetadataSent[MaxOutstandingRequests];     ///< Is metadata sent

    Mutex*         m_pRawResultMutex;                            ///< Rdi Result mutex
    Condition*     m_pRawResultAvailable;                        ///< Wait till all results are available

    BufferManager* m_pRDIBufferManager;                          ///< RDI Buffer manager
    QCFARawQueue   m_QCFARawQueue[ZSLQueueDepth];                ///< ZSL queue

    const VOID*    m_pCachedMeta;                                ///< Cached preview metadata
    VOID*          m_pOfflineInputMeta;                          ///< Input meta for offline reprocess
    VOID*          m_pSnapshotInputMeta;                         ///< Input meta for offline reprocess

    DebugData      m_debugDataOffline;                           ///< Offline copy for debug-data

    UINT32         m_effectModeValue;                            ///< effect value
    UINT32         m_sceneModeValue;                             ///< scenemode value
    UINT32         m_tuningFeature1Value;                        ///< tuning Feature1Value value
    UINT32         m_tuningFeature2Value;                        ///< tuning Feature2Value value
    static volatile UINT32  m_aFlushInProgress;                  ///< Is flush in progress
};

#endif // CHXUSECASEQUADCFA_H
