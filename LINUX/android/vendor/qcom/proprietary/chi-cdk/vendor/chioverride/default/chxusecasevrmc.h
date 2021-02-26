////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasevrmc.h
/// @brief CHX MultiCamera usecase base class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASEMCVR_H
#define CHXUSECASEMCVR_H

#include <assert.h>

#include "chxincs.h"
#include "chxmulticamcontroller.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecaseutils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Multi-camera usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseMultiVRCamera final : public Usecase
{
public:

    /// Static create function to create an instance of the object
    static UsecaseMultiVRCamera* Create(
        LogicalCameraInfo*              cameraInfo,       ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration;

    static const UINT32  REALTIMESESSIONIDX     = 0;        ///< Realtime Session
    static const UINT32  OFFLINESESSIONIDX      = 1;        ///< Offline Preview Session
    static const UINT32  OFFLINERDISESSION      = 2;        ///< Offline RDI Session for Main Sensor
    static const UINT32  OFFLINERDISESSIONAUX   = 3;        ///< Offline RDI Session for Aux Sensor
    static const UINT32  OFFLINERTBJPEGSESSION  = 4;        ///< Offline JPEG Session
    static const UINT32  OFFLINERAWSESSION      = 5;        ///< Offline Raw Snapshot Session
    static const UINT32  OFFLINEVAMSESSION      = 6;        ///< Offline Video Analytics Session


    static const UINT32  MAX_REALTIME_PIPELINE  = 2;        ///< Max supported realtimes sensor pipelines in Session
    static const UINT32  META_READY_FLAG        = 1 << 0;   ///< Flag to indicate meta arrived or not
    static const UINT32  BUF_READY_FLAG         = 1 << 1;   ///< Flag to indicate buffer arrived or not

    static const UINT32  REALTIMEPIPELINEMAIN   = 0;        ///< Main sesnor realtime pipeline
    static const UINT32  REALTIMEPIPELINEAUX    = 1;        ///< Aux sensor realtime pipeline
    static const UINT32  OFFLINEPREVIEWPIPELINE = 2;        ///< Offline Preview pipeline
    static const UINT32  OFFLINEYUVPIPELINE     = 3;        ///< Offline RDI -> YUV pipeline for Main sensor
    static const UINT32  OFFLINEYUVPIPELINEAUX  = 4;        ///< Offline RDI -> YUV pipeline for Aux sensor
    static const UINT32  OFFLINEJPEGPIPELINE    = 5;        ///< Offline JPEG pipeline
    static const UINT32  OFFLINEVAMPIPELINE     = 6;        ///< Offline Video Analytics Pipeline
    static const UINT32  OFFLINERAWPIPELINE     = 7;        ///< Offline Raw Snapshot pipeline
    static const UINT32  INVALIDPIPELINE        = 8;        ///< Invalid pipeline Index
    static const UINT32  VRVIDEODEPTH           = 6;        ///< Assignment of gralloc buffers
    static const UINT32  FPS30                  = 30;       ///< 30FPS
    static const UINT64 InvalidFrameNumber     = 0xFFFFFFFFFFFFFFFF;    ///< Invalid frame number


/// Usecase enum required for remapping the pipeline ID's based on respective UsecaseID
enum DualCameraUsecase
{
    UsecaseRTB,              ///< RTB Preview/Snapshot
    UsecaseSAT,              ///< SAT Preview/Snapshot
    UsecaseSATVideo,         ///< SAT Video
    UseCaseVR,               ///< VR Camera
    UseCaseVRVideo,          ///< VR Video
    UsecaseMax               ///< Reserved
};

    static const UINT32  DUMMYVAMSTREAMWIDTH   = 480;      ///< Dummy stream width
    static const UINT32  DUMMYVAMSTREAMHEIGHT  = 480;      ///< Dummy stream height

protected:
    /// Destroy/Cleanup the object
    virtual VOID Destroy(BOOL isForced);
private:

    UsecaseMultiVRCamera() = default;
    virtual ~UsecaseMultiVRCamera();

    // Do not allow the copy constructor or assignment operator
    UsecaseMultiVRCamera(const UsecaseMultiVRCamera&) = delete;
    UsecaseMultiVRCamera& operator= (const UsecaseMultiVRCamera&) = delete;

    static VOID SessionCbCaptureResult(
        ChiCaptureResult* pCaptureResult,                       ///< Capture result
        VOID*             pPrivateCallbackData);                ///< Private callback data

    static VOID SessionCbNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor,         ///< Message Descriptor
        VOID*                       pPrivateCallbackData);      ///< Private callback data

    /// Execute capture request
    CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    /// Process the available results
    virtual CDKResult NotifyResultsAvailable(
        const CHICAPTURERESULT* pResult, INT index);                       ///< Capture results

    /// Process result from realtime pipelines
    VOID RealtimeCaptureResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Process result from offline preview pipeline
    VOID OfflinePreviewResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Process result from offline jpeg pipeline
    VOID OfflineRTBJPEGResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Process result from offline Raw snapshot pipeline
    VOID OfflineRTBRawResult(
        const ChiCaptureResult* pResult);                     ///< Request parameters

    /// Process result from offline snapshot pipeline
    VOID OfflineSnapshotResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Dispatch a capture request to realtime pipelines
    CDKResult GenerateRealtimeRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    CDKResult GenerateInternalRequest(
        UINT32             sessionId,                           ///< Session index
        UINT               numRequest,                          ///< Number of CHI request
        CHICAPTUREREQUEST* pRequest);                           ///< Request parameters

    VOID processRDIFrame(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    //Remaps the pipeline index coming from usecase to CHI expected way
    UINT32 RemapPipelineIndex(
        UINT32              pipelineIndexFromUsecase,           ///< Pipeline index from usecase
        DualCameraUsecase   usecase);                           ///< Active usecase

    /// Does one time initialization of the created object
    CDKResult Initialize(
        LogicalCameraInfo*              cameraInfo,             ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig);         ///< Stream configuration

    /// Process the saved results
    VOID ProcessResults();

    /// Function to determine if the stream is a Rdi stream
    BOOL IsRdiStream(
        CHISTREAM* pStream) const;

    /// Function to determine if the stream is a Rdi Raw stream
    BOOL IsRdiRawStream(
            CHISTREAM* pStream) const;

    /// Function to determine if the stream is a realtime preview stream
    BOOL IsRTPreviewStream(
        CHISTREAM* pStream) const;

    /// Function to determine if the stream is a Raw Snapshot stream
    BOOL IsRawSnapshotSupported();

    /// Forward RDI frame for JPEG processing
    VOID processRDIJpegFrame(
        const ChiCaptureResult* pResult, ChiStreamBuffer* RDIBuffer);

    /// Forward RDI frame for Raw snapshot stiching
    VOID processRDIRawSnapshotFrame(
        const ChiCaptureResult* pResult, ChiStreamBuffer* RDIBuffer);

    /// Function to determine if the stream is a realtime vam stream
    BOOL IsRTVamStream(
        CHISTREAM* pStream) const;

    BOOL IsVamSupported();

    /// Function to determine stream format for Impl_Defined
    CHISTREAMFORMAT getImplDefinedFormat(CHISTREAMFORMAT bufFormat);


    struct SessionPrivateData
    {
        Usecase* pUsecase;  ///< Per usecase class
        UINT32   sessionID; ///< Session Id that is meaningful to the usecase in which the session belongs
    };

    struct realTimeBufQueue
    {
        UINT32           frameNumber;                               ///< Frame number
        VOID*            pMetadata[MAX_REALTIME_PIPELINE];          ///< Array of metadata
        CHISTREAMBUFFER  buffer[MAX_REALTIME_PIPELINE];             ///< RT buffer
        BOOL             valid[MAX_REALTIME_PIPELINE];              ///< valid flag
    };

    SessionPrivateData      m_perSessionPvtData[MaxSessions];               ///< Per session private data
    Session*                m_pSession[MaxSessions];                        ///< Session
    Pipeline*               m_pPipeline[MaxPipelinesPerSession];            ///< Pipelines
    INT64                   m_shutterFrameNum;                              ///< last framenum processed.
    realTimeBufQueue        rtBuffer[MaxOutstandingRequests];               ///< realtime pipeline request buffer queue
    realTimeBufQueue        rtVamBuffer[MaxOutstandingRequests];            ///< realtime pipeline request buffer queue
    realTimeBufQueue        rtSnapshotBuffer[MaxOutstandingRequests];       ///< offline snapshot pipeline request buffer queue
    realTimeBufQueue        rtRawSnapshotBuffer[MaxOutstandingRequests];    ///< offline raw snapshot pipeline request buffer queue
    CHIPRIVDATA             privOffline[MaxOutstandingRequests];            ///< private data for offline preview pipeline
    CHIPRIVDATA             rawSnapOffline[MaxOutstandingRequests];         ///< private data for offline raw snapshot pipeline
    CHIPRIVDATA             privRDIOffline1[MaxOutstandingRequests];         ///< private data for offline RDI pipeline
    CHIPRIVDATA             privRDIOffline2[MaxOutstandingRequests];         ///< private data for offline RDI pipeline
    CHIPRIVDATA             privSnapshotOffline[MaxOutstandingRequests];    ///< private data for offline Snapshot pipeline
    CHIPRIVDATA             privRealTime1[MaxOutstandingRequests];          ///< private data for realtime pipeline
    CHIPRIVDATA             privRealTime2[MaxOutstandingRequests];          ///< private data for realtime pipeline
    BOOL                    postviewRequest[MaxOutstandingRequests];        ///< have postview frame or not
    BOOL                    videoRequest[MaxOutstandingRequests];           ///< Is video request or not
    BOOL                    vamRequest[MaxOutstandingRequests];             ///< Is vam request or not
    CHISTREAM*              m_pTargetPreviewStream;                         ///< Preview Stream configured from framework
    CHISTREAM*              m_pTargetVamStream;                             ///< Vam Stream configured from framework
    CHISTREAM*              m_pTargetRawSnapshotStream;                     ///< Preview Stream configured from framework
    CHISTREAM*              m_pTargetSnapshotStream;                        ///< Snapshot Stream configured from framework
    CHISTREAM*              m_pTargetPostviewStream;                        ///< Postview Stream configured from framework
    CHISTREAM*              m_pTargetVideoStream;                           ///< Video Stream configured from framework
    CHISTREAM*              m_pRTBPreviewStream[MAX_REALTIME_PIPELINE];     ///< realtime pipeline output stream
    CHISTREAM*              m_pRTPreviewStream[MAX_REALTIME_PIPELINE];      ///< offline preview pipeline input stream
    CHISTREAM*              m_pRTRDIStream[MAX_REALTIME_PIPELINE];          ///< realtime RDI output stream
    CHISTREAM*              m_pRTRDISnapshotStream[MAX_REALTIME_PIPELINE];  ///< realtime RDI output stream
    CHISTREAM*              m_pRTBRDISnapshotStream[MAX_REALTIME_PIPELINE]; ///< realtime RDI output stream
    CHISTREAM*              m_pRTBSnapshotStream[MAX_REALTIME_PIPELINE];    ///< offline snapshot pipeline input stream
    CHISTREAM*              m_pRTYUVStream[MAX_REALTIME_PIPELINE];          ///< offline Snapshot pipeline output stream
    CHISTREAM*              m_pJPEGStream[MAX_REALTIME_PIPELINE];           ///< offline JPEG pipeline Input stream
    CHISTREAM*              m_pRTVamStream[MAX_REALTIME_PIPELINE];          ///< offline vam pipeline input stream
    CHISTREAM*              m_pRTBVamStream[MAX_REALTIME_PIPELINE];         ///< realtime pipeline output stream

    camera3_stream_buffer_t m_appSnapshotBuffer[MaxOutstandingRequests];    ///< buffer application request buffer handle for snapshot
    camera3_stream_buffer_t m_appPreviewBuffer[MaxOutstandingRequests];     ///< buffer application request buffer handle for preview
    camera3_stream_buffer_t m_appPostviewBuffer[MaxOutstandingRequests];    ///< buffer application request buffer handle for postview
    camera3_stream_buffer_t m_appVideoBuffer[MaxOutstandingRequests];       ///< buffer application request buffer handle for video
    camera3_stream_buffer_t m_appRawSnapshotBuffer[MaxOutstandingRequests]; ///< buffer application request buffer handle for video
    camera3_stream_buffer_t m_appVamBuffer[MaxOutstandingRequests];         ///< buffer application request buffer handle for vam
    BufferManager*          m_pPreviewBufferManager[MAX_REALTIME_PIPELINE]; ///< buffer manager for realtime pipeline preview output
    BufferManager*          m_pVamBufferManager[MAX_REALTIME_PIPELINE];     ///< buffer manager for realtime pipeline VAM output
    BufferManager*          m_pRDIBufferManager[MAX_REALTIME_PIPELINE];     ///< buffer manager for realtime pipeline RDI output
    BufferManager*          m_pRDISnapshotBufferManager[MAX_REALTIME_PIPELINE];///< buffer manager for realtime pipeline RDI output
    BufferManager*          m_pYUVBufferManager[MAX_REALTIME_PIPELINE];     ///< buffer manager for offline pipeline YUV output
    MultiCamController*     m_pMultiCamController;                          ///< Multicam controller handle
    camera_metadata_t*      m_pOfflinePipelineInputMetadata;                ///< Input metadata poiter for offline pipeline
    BOOL                    m_isPostViewNeeded;                             ///< Is postview needed
    BOOL                    m_isVideoNeeded;                                ///< Is Video stream needed
    UINT32                  m_streamDSFactor;                               ///< Stream Downscale Factor
    UINT32                  m_tuningFeature1Value[MAX_REALTIME_PIPELINE];   ///< tuning Feature1Value value
    UINT32                  m_tuningFeature2Value[MAX_REALTIME_PIPELINE];   ///< tuning Feature2Value value
    UINT32                  m_effectModeValue[MAX_REALTIME_PIPELINE];       ///< effect value
    UINT32                  m_sceneModeValue[MAX_REALTIME_PIPELINE];        ///< scenemode value
    camera3_capture_result_t m_captureResult[MaxOutstandingRequests];       ///< Capture results to be sent to the
                                                                                    ///< app/framework
    UINT32                  m_kernelFrameSyncEnable;                        ///< This tells whether frame sync enabled or not
    BOOL                    m_isMetadataAvailable[MaxOutstandingRequests];   ///< Is metadata ready to sent
    BOOL                    m_isMetadataSent[MaxOutstandingRequests];        ///< Has metadata been sent already
    INT64                   m_lastResultMetadataFrameNum;                    ///< Frame number whose metadata was sent
    UINT64                  m_nextAppResultFrame;                            ///< Next frame result to be sent back
    UINT64                  m_nextAppMessageFrame;                           ///< Next frame message to be sent back
    UINT64                  m_lastAppMessageFrameReceived;                   ///< Last frame message to be sent back
    UINT64                  m_lastAppRequestFrame;                           ///< Last app request frame number
};

#endif // CHXUSECASEMCVR_H
