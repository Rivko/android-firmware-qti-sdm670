////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecasemc.h
/// @brief CHX MultiCamera usecase base class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASEMC_H
#define CHXUSECASEMC_H

#include <assert.h>

#include "chxadvancedcamerausecase.h"
#include "chxincs.h"
#include "chxmulticamcontroller.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecaseutils.h"
#include "chxadvancedcamerausecase.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Multi-camera usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class UsecaseMultiCamera final : public AdvancedCameraUsecase
{
public:

    /// Static create function to create an instance of the object
    static UsecaseMultiCamera* Create(
        LogicalCameraInfo*              cameraInfo,       ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration;

    static const UINT32  REALTIMESESSIONIDX     = 0;        ///< Realtime Session
    static const UINT32  OFFLINESESSIONIDX      = 1;        ///< Offline Preview Session
    static const UINT32  OFFLINERDISESSION      = 2;        ///< Offline RDI Session for Main Sensor
    static const UINT32  OFFLINERDISESSIONAUX   = 3;        ///< Offline RDI Session for Aux Sensor
    static const UINT32  OFFLINERTBJPEGSESSION  = 4;        ///< Offline JPEG Session
    static const UINT32  OFFLINEFUSIONSNAPSHOTSESSION = 5;  ///< Offline Fusion snapshot Session
    static const UINT32  OFFLINERAW16SESSION    = 6;        ///< Offline RAW16 Session

    static const UINT32  MAX_REALTIME_PIPELINE  = 2;        ///< Max supported realtimes sensor pipelines in Session
    static const UINT32  META_READY_FLAG        = 1 << 0;   ///< Flag to indicate meta arrived or not
    static const UINT32  BUF_READY_FLAG         = 1 << 1;   ///< Flag to indicate buffer arrived or not

    static const UINT32  REALTIMEPIPELINEMAIN   = 0;        ///< Main sesnor realtime pipeline
    static const UINT32  REALTIMEPIPELINEAUX    = 1;        ///< Aux sensor realtime pipeline
    static const UINT32  OFFLINEPREVIEWPIPELINE = 2;        ///< Offline Preview pipeline
    static const UINT32  OFFLINEYUVPIPELINE     = 3;        ///< Offline RDI -> YUV pipeline for Main sensor
    static const UINT32  OFFLINEYUVPIPELINEAUX  = 4;        ///< Offline RDI -> YUV pipeline for Aux sensor
    static const UINT32  OFFLINEJPEGPIPELINE    = 5;        ///< Offline JPEG pipeline
    static const UINT32  OFFLINEFUSIONPIPELINE  = 6;        ///< Offline Snapshot fusion pipeline
    static const UINT32  OFFLINERAW16PIPELINE   = 7;        ///< Offline RAW16 pipeline

    static const UINT32  DUMMYWIDTH             = 640;      ///< Dummy stream width for pipeline create to work in video
    static const UINT32  DUMMYHEIGHT            = 480;      ///< Dummy stream height for pipeline create to work in video
    static const UINT    INVALID_CAMERAID       = 0xFFFF;   ///< Invalid camera ID

    const static INT     MaxFeatureSupported    = 10;

    static const UINT32  INVALID_PIPELINEID     = 0xFFFF;   ///< Invalide Pipeline ID
    static const UINT32  MAX_NUMBER_TARGETS     = 32;       ///< Max numbers of target
    static const UINT32  MAX_NUMBER_PIPELINES   = 32;       ///< Max numbers of pipeline
    /// Usecase enum required for remapping the pipeline ID's based on respective UsecaseID
    enum DualCameraUsecase
    {
        UsecaseRTB,              ///< RTB Preview/Snapshot
        UsecaseSAT,              ///< SAT Preview/Snapshot
        UsecaseMax               ///< Reserved
    };
    /// dual camera session type
    enum DualCameraSession
    {
        REALTIME_SESSION = 0,
        OFFLINE_PREVIEW_SESSION,
        OFFLINE_RDI_SESSION_MAIN,
        OFFLINE_RDI_SESSION_AUX,
        OFFLINE_SNAPSHOT_FUSSION_SESSION,
        OFFLINE_JPEG_SESSION,
        OFFLINE_RAW16_SESSION,
        MAX_DUAL_CAMERA_SESSION,
    };

    /// pipeline create data
    struct ChxPipelineCreateData
    {
        UINT                               cameraID;
        UINT                               numOfSourceStreams;
        ChiStream*                         pSourceChiStream[MaxChiStreams];
        UINT                               numOfSinkStreams;
        ChiStream*                         pSinkChiStream[MaxChiStreams];
        ChiPipelineTargetCreateDescriptor* pPipelineTargetCreateDescriptor;
        BOOL                               isDeferFinalizeNeeded;
        UINT                               pipelineIndex;
    };

    enum AdvanceFeatureId
    {
        AdvanceFeatureMFNR = 0,
        AdvanceFeatureHDR,
        AdvanceFeatureSWMF,
        AdvanceFeatureMax,
    };

protected:
    /// Destroy/Cleanup the object
    virtual VOID Destroy(BOOL isForced);
private:

    UsecaseMultiCamera() = default;
    virtual ~UsecaseMultiCamera();

    // Do not allow the copy constructor or assignment operator
    UsecaseMultiCamera(const UsecaseMultiCamera&) = delete;
    UsecaseMultiCamera& operator= (const UsecaseMultiCamera&) = delete;

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
        const CHICAPTURERESULT* pResult);                       ///< Capture results

    /// Process result from realtime pipelines
    VOID RealtimeCaptureResult(
        ChiCaptureResult* pResult);                             ///< Request parameters

    /// Process result from offline preview pipeline
    VOID OfflinePreviewResult(
        ChiCaptureResult* pResult);                             ///< Request parameters

    /// Process result from offline RAW16 pipeline
    VOID OfflineRAW16Result(
        ChiCaptureResult* pResult);                             ///< Request parameters

    /// Process result from offline jpeg pipeline
    VOID OfflineRTBJPEGResult(
        ChiCaptureResult* pResult);                             ///< Request parameters

    /// Process result from offline snapshot pipeline
    VOID OfflineSnapshotResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Process result from offline snapshot fusion pipeline
    VOID OfflineFusionSnapshotResult(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    /// Process notify messages from real time session
    VOID ProcessRealTimeNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor);

    /// Dispatch a capture request to realtime pipelines
    CDKResult GenerateRealtimeRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    CDKResult GenerateInternalRequest(
        UINT32             sessionId,                           ///< Session index
        UINT               numRequest,                          ///< Number of CHI request
        CHICAPTUREREQUEST* pRequest);                           ///< Request parameters

    VOID processRDIFrame(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    CDKResult processRAW16Frame(
        const ChiCaptureResult* pResult);                       ///< Request parameters

    // Flush
    virtual CDKResult Flush();

    //Remaps the pipeline index coming from usecase to CHI expected way
    UINT32 RemapPipelineIndex(
        UINT32              pipelineIndexFromUsecase,           ///< Pipeline index from usecase
        DualCameraUsecase   usecase);                           ///< Active usecase

    /// Does one time initialization of the created object
    CDKResult Initialize(
        LogicalCameraInfo*              cameraInfo,             ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig);         ///< Stream configuration

    /// Does one time initialization of the created object
    CDKResult InitializeAdvanceFeatures(
        LogicalCameraInfo*              pCameraInfo,             ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig);         ///< Stream configuration


    /// Create Multi Controller
    CDKResult CreateMultiController(
        LogicalCameraInfo*              pCameraInfo,            ///< Camera Info
        camera3_stream_configuration_t* pStreamConfig,          ///< Stream configuration
        DualCameraUsecase usecase);                             ///< use case

    /// Fill Pipeline Create data
    CDKResult FillPipelineCreateData(
        ChiUsecase*            pChiUsecase,
        LogicalCameraInfo*     pCameraInfo,
        UINT                   index,
        ChxPipelineCreateData* pCreateData);

    /// Create Pipeline
    CDKResult CreateOfflineSession();

    /// Create Pipeline
    CDKResult CreatePipeline(
        ChxPipelineCreateData* pCreateData);

    /// Process the saved results
    VOID ProcessResults();

    /// Function to determine if the stream is a Rdi stream
    BOOL IsRdiStream(
        CHISTREAM* pStream) const;

    BOOL IsRdiRAW16Stream(
        CHISTREAM* pStream) const;

    /// Function to determine if the stream is a realtime preview stream
    BOOL IsRTPreviewStream(
        CHISTREAM* pStream) const;

    BOOL canEnableAdvanceFeature() const
    {
        return ((NULL == m_pTargetVideoStream) && (UsecaseRTB != m_dualCameraUsecase));
    }
    struct SessionPrivateData
    {
        Usecase* pUsecase;  ///< Per usecase class
        UINT32   sessionID; ///< Session Id that is meaningful to the usecase in which the session belongs
    };

    /// dual camera start up optimization function
    CDKResult ActivateDeactivateRealtimePipeline(
        active_cameras_t* activeCameras,
        UINT64            requestId);

    /// Defer snapshot related session thread function
    static VOID* DeferSnapSessionThread(
        VOID* pThreadData);

    /// defer snapshot session create function
    VOID DeferSnapSession();

    CDKResult WaitForDeferSnapThread();

    /// start defer thread
    VOID StartDeferThread();

    /// intialize defer related resource
    VOID InitializeDeferResource();

    /// destroy defer related resource
    VOID DestroyDeferResource();

    /// check if there is preview stream for this request
    BOOL hasPreviewStreamRequest(
        camera3_capture_request_t* pRequest) const
    {
        BOOL hasPreviewRequest = FALSE;
        for (UINT i = 0; i < pRequest->num_output_buffers; i++)
        {
            if (((CHISTREAM *)pRequest->output_buffers[i].stream == m_pTargetPreviewStream) ||
                ((NULL != m_pTargetVideoStream) &&
                (CHISTREAM *)pRequest->output_buffers[i].stream == m_pTargetVideoStream))
            {
                hasPreviewRequest = TRUE;
                break;
            }
        }
        return hasPreviewRequest;
    }

    /// check if there is snapshot stream for this request
    BOOL hasSnapshotStreamRequest(
        camera3_capture_request_t* pRequest) const
    {
        BOOL hasSnapshotRequest = FALSE;
        for (UINT i = 0; i < pRequest->num_output_buffers; i++)
        {
            if ((NULL != m_pTargetSnapshotStream) &&
                (CHISTREAM *)pRequest->output_buffers[i].stream == m_pTargetSnapshotStream)
            {
                hasSnapshotRequest = TRUE;
                break;
            }
        }
        return hasSnapshotRequest;
    }
    /// offline request
    CDKResult CreateOfflineProcessResource();

    VOID DestroyOfflineProcessResource();

    VOID OfflineRequestProcess();

    static VOID* OfflineRequestThread(
        VOID* pArg);

    VOID TriggerOfflineRequest(
        UINT32                     frameNumber,
        camera3_capture_request_t* pRequest);

    VOID GenerateSnapshotRequest(
        UINT32 inputFrameNum,
        UINT32 frameNumber,
        UINT32 pipelineIndex);

    BOOL isOfflineProcessRequired(
        camera3_capture_request_t* pRequest,
        RequestMapInfo*            pRequestMapInfo);

    struct realTimeBufQueue
    {
        UINT32           frameNumber;                               ///< Frame number
        VOID*            pMetadata[MAX_REALTIME_PIPELINE];          ///< Array of metadata
        CHISTREAMBUFFER  buffer[MAX_REALTIME_PIPELINE];             ///< RT buffer
        BOOL             valid[MAX_REALTIME_PIPELINE];              ///< valid flag
    };

    struct realTimeRequest
    {
        UINT32           masterPipelineIndex;                       ///< Tells the master pipeline index
        BOOL             isActive[MAX_REALTIME_PIPELINE];           ///< lpm info per pipeline
        BOOL             prevRequest[MAX_REALTIME_PIPELINE];        ///< Preview Request submitted or not
        BOOL             snapRequest[MAX_REALTIME_PIPELINE];        ///< Snapshot Request submitted or not
        BOOL             rawRequest[MAX_REALTIME_PIPELINE];         ///< raw Request submitted or not
    };

    struct snapshotRequestInfo
    {
        UINT32             frameNumber;                               ///< Frame number
        UINT32             inputRDIFrameNumber;                       ///< Input RDI frame number
        camera_metadata_t* pSnapshotMetadata;                         ///< Snapshot metadata
    };

    struct PendingRequestInfo
    {
        UINT32 pipelineIndex;        ///< Pipeline index of real time pipeline
        UINT32 requestId;            ///< This represens the last request for which pipeline is active
                                     ///  0 signifies no inflight LPM deactivate
        BOOL   isReturnedResult;     ///< TRUE indicates last result returned for pipeline to be deactivated
                                     ///  FALSE indicates still there is pending inflight requests
    };

    struct jpegRequestInfo
    {
        UINT64     requestId;
        CHISTREAM* stream;
    };

    VOID ReturnFrameworkMetadata(
        camera3_capture_result_t *pResult);

    CDKResult SubmitJpegRequest(
            UINT32 frameNum,
            CHISTREAMBUFFER *inputBuffer,
            CHISTREAMBUFFER *JpegBuffer,
            const VOID *pMetadata);

    CDKResult AppendOfflineMetadata(
            UINT32 frameNum,
            VOID *pMetadata1,
            VOID *pMetadata2,
            camera_metadata_t *pResultMeta,
            BOOL isSnapshotMeta);

    /// Offline Request thread info
    PerThreadData               m_offlineRequestProcessThread;                  ///< Thread to process the results
    Mutex*                      m_pOfflineRequestMutex;                         ///< App Result mutex
    Condition*                  m_pOfflineRequestAvailable;                     ///< Wait till SensorListener results
                                                                                ///  are available
    volatile BOOL               m_offlineRequestProcessTerminate;               ///< Indication to SensorListener result
                                                                                ///  thread to terminate itself
    UINT32                      m_maxSnapshotReqId;                             ///< Last valid ID to move snapshotReqId to
                                                                                ///  Owned by the main thread
    UINT32                      m_snapshotReqId;                                ///< Next ID to provide snapshot request
    snapshotRequestInfo         m_snapshotRequestInfo[MaxOutstandingRequests];  ///< Snapshot request map info
    camera_metadata_t*          m_pAppSnapshotMetadata[MaxOutstandingRequests]; ///< App snapshot metadata
    PerThreadData               m_deferSnapSessionThreadData;                   ///< Thread data for defer

    CHIBufferManagerCreateData  m_rdi16BufferCreateData[MAX_REALTIME_PIPELINE];  ///< RDI RAW16 output buffer create data
    CHIBufferManagerCreateData  m_rdiBufferCreateData[MAX_REALTIME_PIPELINE];    ///< RDI RAW10 output buffer create data
    CHIBufferManagerCreateData  m_yuvSnapBufferCreateData[MAX_REALTIME_PIPELINE];///< YUV output buffer create data
    CHIBufferManagerCreateData  m_fusionBufferCreateData;                        ///< Fusion output buffer create data

    volatile BOOL               m_deferSnapshotSessionDone;                ///< Flag indicate if snapshot session
                                                                           ///  create done
    volatile BOOL               m_deferSnapshotThreadCreateDone;           ///< Flag indicate if snapshot session

    Condition*              m_pDeferSnapDoneCondition;                     ///< Condition for defer snapshot done
    Mutex*                  m_pDeferSnapDoneMutex;                         ///< Mutex for defer snapshot done
    UINT                    m_deferedCameraID;                             ///< deferred camera id
    Mutex*                  m_pDeferSyncMutex;                             ///< Mutex for defer sync
    SessionPrivateData      m_perSessionPvtData[MaxSessions];               ///< Per session private data
    Session*                m_pSession[MaxSessions];                        ///< Session
    Pipeline*               m_pPipeline[MaxPipelinesPerSession];            ///< Pipelines
    INT64                   m_shutterFrameNum;                              ///< last framenum processed.
    realTimeBufQueue        rtBuffer[MaxOutstandingRequests];               ///< realtime pipeline request buffer queue
    realTimeBufQueue        rtRAW16Buffer[MaxOutstandingRequests];          ///< realtime raw request buffer queue
    realTimeBufQueue        rtSnapshotBuffer[MaxOutstandingRequests];       ///< offline snapshot pipeline request buffer queue
    realTimeBufQueue        m_FusionResultBuffer[MaxOutstandingRequests];   ///< offline snapshot pipeline request buffer queue
    realTimeRequest         rtRequest[MaxOutstandingRequests];              ///< Realtime request info
    CHIPRIVDATA             privRAW16Offline[MaxOutstandingRequests];       ///< private data for offline preview pipeline
    CHIPRIVDATA             privPreviewOffline[MaxOutstandingRequests];     ///< private data for offline preview pipeline
    CHIPRIVDATA             privJPEGOffline[MaxOutstandingRequests];        ///< private data for offline jpeg pipeline
    CHIPRIVDATA             privRDIOffline1[MaxOutstandingRequests];        ///< private data for offline RDI Main pipeline
    CHIPRIVDATA             privRDIOffline2[MaxOutstandingRequests];        ///< private data for offline RDI Aux pipeline
    CHIPRIVDATA             privSnapshotOffline[MaxOutstandingRequests];    ///< private data for offline Snapshot pipeline
    CHIPRIVDATA             privRealTime1[MaxOutstandingRequests];          ///< private data for realtime pipeline
    CHIPRIVDATA             privRealTime2[MaxOutstandingRequests];          ///< private data for realtime pipeline
    BOOL                    postviewRequest[MaxOutstandingRequests];        ///< have postview frame or not
    BOOL                    videoRequest[MaxOutstandingRequests];           ///< Is video request or not
    CHISTREAM*              m_pTargetPreviewStream;                         ///< Preview Stream configured from framework
    CHISTREAM*              m_pTargetSnapshotStream;                        ///< Snapshot Stream configured from framework
    CHISTREAM*              m_pTargetWideJpegStream;                        ///< Wide Snapshot Stream configured from framework
    CHISTREAM*              m_pTargetPostviewStream;                        ///< Postview Stream configured from framework
    CHISTREAM*              m_pTargetRAW16Stream;                           ///< RAW Stream configured from framework
    CHISTREAM*              m_pTargetVideoStream;                           ///< Video Stream configured from framework
    CHISTREAM*              m_pSnapshotFusionStream;                        ///< Snapshot fusion stream
    CHISTREAM*              m_pTargetRDIStream;                             ///< Target RDI stream
    CHISTREAM*              m_pRTBPreviewStream[MAX_REALTIME_PIPELINE];     ///< realtime pipeline output stream
    CHISTREAM*              m_pRTPreviewStream[MAX_REALTIME_PIPELINE];      ///< offline preview pipeline input stream
    CHISTREAM*              m_pRTRDIStream[MAX_REALTIME_PIPELINE];          ///< realtime RDI output stream
    CHISTREAM*              m_pRTRaw16Stream[MAX_REALTIME_PIPELINE];        ///< realtime RAW16 output stream
    CHISTREAM*              m_pInputRaw16Stream[MAX_REALTIME_PIPELINE];     ///< realtime RAW16 input stream
    CHISTREAM*              m_pRTBSnapshotStream[MAX_REALTIME_PIPELINE];    ///< offline snapshot pipeline input stream
    CHISTREAM*              m_pRTYUVStream[MAX_REALTIME_PIPELINE];          ///< offline Snapshot pipeline output stream
    CHISTREAM*              m_pJPEGStream[MAX_REALTIME_PIPELINE];           ///< offline JPEG pipeline Input stream
    CHISTREAM*              m_pDummyStream[MAX_REALTIME_PIPELINE];          ///< Dummy stream

    camera3_stream_buffer_t m_appSnapshotBuffer[MaxOutstandingRequests];    ///< buffer application request buffer handle for snapshot
    camera3_stream_buffer_t m_appPreviewBuffer[MaxOutstandingRequests];     ///< buffer application request buffer handle for preview
    camera3_stream_buffer_t m_appPostviewBuffer[MaxOutstandingRequests];    ///< buffer application request buffer handle for postview
    camera3_stream_buffer_t m_appVideoBuffer[MaxOutstandingRequests];       ///< buffer application request buffer handle for video
    camera3_stream_buffer_t m_appRAW16Buffer[MaxOutstandingRequests];       ///< buffer application request buffer handle for RAW16
    camera3_stream_buffer_t m_appWideJpegBuffer[MaxOutstandingRequests];    ///< buffer application request buffer handle for snapshot

    CHIBufferManager*       m_pPreviewBufferManager[MAX_REALTIME_PIPELINE]; ///< buffer manager for realtime pipeline preview output
    CHIBufferManager*       m_pRDI16BufferManager[MAX_REALTIME_PIPELINE];   ///< buffer manager for realtime pipeline RDI output
    CHIBufferManager*       m_pYUVBufferManager[MAX_REALTIME_PIPELINE];     ///< buffer manager for offline pipeline YUV output
    CHIBufferManager*       m_pFusionSnapshotBufferManager;                 ///< buffer manager for snapshot fusion pipeline

    MultiCamController*     m_pMultiCamController;                          ///< Multicam controller handle
    camera_metadata_t*      m_pOfflinePipelineInputMetadataPreview;         ///< Input metadata for offline preview pipeline
    camera_metadata_t*      m_pOfflinePipelineInputMetadataSnapshot;        ///< Input metadata for offline snapshot pipeline
    camera_metadata_t*      m_pOfflinePipelineInputMetadataRAW16;           ///< Input metadata for offline RAW pipeline
    camera_metadata_t*      m_pStickyMetadata[MAX_REALTIME_PIPELINE];       ///< Sticky meta data for both the realtime pipelines
    BOOL                    m_isPostViewNeeded;                             ///< Is postview needed
    BOOL                    m_isVideoNeeded;                                ///< Is Video stream needed
    BOOL                    m_isRaw16Needed;                                ///< Is Video stream needed

    UINT32                  m_effectModeValue[MAX_REALTIME_PIPELINE];       ///< effect value
    UINT32                  m_sceneModeValue[MAX_REALTIME_PIPELINE];        ///< scenemode value
    UINT32                  m_tuningFeature1Value[MAX_REALTIME_PIPELINE];   ///< tuning Feature1Value value
    UINT32                  m_tuningFeature2Value[MAX_REALTIME_PIPELINE];   ///< tuning Feature2Value value
    DualCameraUsecase       m_dualCameraUsecase;                            ///< dual camera usecase
    UINT32                  m_previewBufferID[MAX_REALTIME_PIPELINE];       ///< Preview internal buffer queue ID
    UINT32                  m_realtimeRequestID;
    PendingRequestInfo      m_pendingRequest[MAX_REALTIME_PIPELINE];        ///< Pending results for requests on deactivated camera
    BOOL                    m_stickyMetaNeeded[MAX_REALTIME_PIPELINE];      ///< This indicates whether sticky meta needed or not
    controller_result_t     m_prevMCCResult;                                ///< Holds Previous multi camera controller reslut
    UINT32                  m_kernelFrameSyncEnable;                        ///< This tells whether frame sync enabled or not
    BOOL                    m_isLLSSnapshot;                                ///< Flag to indicate LLS snapshot or not
    BOOL                    m_flushLock;                                    ///< Flag to indicate flush locked.

    BOOL                    m_bokehWideCapture;                             ///< capture wide jpeg as well in bokeh mode
    UINT64                  m_jpegRequestCount;                             ///< RequestId for jpeg pipeline
    jpegRequestInfo         m_jpegRequestId[MaxOutstandingRequests];        ///< store jpeg requestId
    static volatile UINT32  m_aFlushInProgress;                             ///< Is flush in progress
};

#endif // CHXUSECASEMC_H
