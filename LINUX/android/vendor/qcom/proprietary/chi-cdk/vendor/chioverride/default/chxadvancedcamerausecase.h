////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxadvancedcamerausecase.h
/// @brief CHX Advanced camera usecase class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXADVANCEDCAMERAUSECASE_H
#define CHXADVANCEDCAMERAUSECASE_H

#include "chistatspropertydefines.h"
#include "cdkutils.h"
#include "chxdefs.h"
#include "chxextensionmodule.h"
#include "chxfeaturezsl.h"
#include "chxfeatureyuvcb.h"
#include "chxfeaturehdr.h"
#include "chxfeatureshdr.h"
#include "chxfeaturemultiframe.h"
#include "chxfeaturemfnr.h"
#include "chxfeaturerawjpeg.h"
#include "chxpipeline.h"
#include "chxusecase.h"
#include "chxusecaseutils.h"


#define DYNAMIC_FEATURE_SWITCH 1

enum AdvanceFeatureType
{
    AdvanceFeatureNone   = 0x0,     ///< mask for none features
    AdvanceFeatureZSL    = 0x1,     ///< mask for feature ZSL
    AdvanceFeatureMFNR   = 0x2,     ///< mask for feature MFNR
    AdvanceFeatureHDR    = 0x4,     ///< mask for feature HDR(AE_Bracket)
    AdvanceFeatureSWMF   = 0x8,     ///< mask for feature SWMF
    AdvanceFeatureSHDR   = 0x10,    ///< mask for feature SHDR
    AdvanceFeatureCount,
};

const static INT    MaxFeatureSupported    = 10;
/// Threshold of ASD severity, it can be set by OEM
const static UINT32 ThresholdOfASDSeverity = 50;

/// Forward declarations
struct ChiPipelineTargetCreateDescriptor;
class  Session;

/// @brief Mapping of pipelines included in a session
struct SessionPipelines
{
    UINT pipelineId[MaxPipelinesPerSession];  ///< Indices of pipelines in the usecase
    UINT numPipelines;                        ///< Count of valid entries in pipelineId
};

struct PipelineStatus
{
    AdvancedPipelineType pipelineId;
    BOOL                 used;
};

struct SessionRequestIdFeatureMapping
{
    UINT        sessionId;
    UINT64      requestId;
    Feature*    pFeature;
};

struct RequestMapInfo
{
    UINT32   frameNumber;                     ///< Internal frame number for this request
    BOOL     isPreviewReturnNeeded;           ///< Whether to return preview frame to framework
    BOOL     isSnapshotReturnNeeded;          ///< Whether to return snapshot frame to framework
    BOOL     isShutterReturnNeeded;           ///< Whether to return shutter to framework
    BOOL     isMetaReturnNeeded;              ///< Whether to return metadata to framework
    BOOL     triggerOfflineReprocess;         ///< If need to trigger offline request
    BOOL     needTriggerOfflineProcess;       ///< If need to trigger offline request
    UINT32   inputFrameNumber;                ///< Input frame number
    Feature* pFeature;                        ///< Feature used to process this request
    UINT32   masterCameraID;                  ///< Master Camera ID
    UINT32   activePipelineID;                ///< Active Pipeline ID
    BOOL     isSkipPreview;                   ///< If skip preview frame or not
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Intermediate Class representing the base of most camera operations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CameraUsecaseBase : public Usecase
{
public:
    CHX_INLINE const PipelineData* GetPipelineData(
        UINT sessionId,
        UINT pipelineId
    ) const
    {
        return &m_sessions[sessionId].pipelines[pipelineId];
    }

    CHX_INLINE const SessionData* GetSessionData(
        UINT sessionId
    ) const
    {
        return &m_sessions[sessionId];
    }

    CHX_INLINE TargetBuffer* GetTargetBuffer(
        UINT bufferId)
    {
        return &m_targetBuffers[bufferId];
    }

    CHX_INLINE TargetBuffer* GetRDITargetBuffer(
        UINT realtimePipelineId)
    {
        return &m_targetBuffers[realtimePipelineId];
    }
    CHX_INLINE BOOL IsMetadataSent(
        UINT requestId
    ) const
    {
        return m_isMetadataSent[requestId];
    }

    CHX_INLINE VOID SetMetadataAvailable(
        UINT requestId)
    {
        m_isMetadataAvailable[requestId] = TRUE;
    }

    CHX_INLINE CDKResult SetPipelineToSessionMapping(
        UINT        numSessions,
        UINT        numPipelines,
        const UINT* pMapping)
    {
        CDKResult result = CDKResultSuccess;

        m_numSessions        = numSessions;
        m_pPipelineToSession = reinterpret_cast<UINT*>(CHX_CALLOC(sizeof(UINT) * numPipelines));

        if (NULL != m_pPipelineToSession)
        {
            ChxUtils::Memcpy(m_pPipelineToSession, pMapping, sizeof(UINT) * numPipelines);
        }
        else
        {
            result = CDKResultENoMemory;
        }

        return result;
    }

    CHX_INLINE CDKResult SetPipelineToCameraMapping(
        UINT        numPipelines,
        const UINT* pMapping)
    {
        CDKResult result = CDKResultSuccess;

        m_pPipelineToCamera = reinterpret_cast<UINT*>(CHX_CALLOC(sizeof(UINT) * numPipelines));

        if (NULL != m_pPipelineToCamera)
        {
            ChxUtils::Memcpy(m_pPipelineToCamera, pMapping, sizeof(UINT) * numPipelines);
        }
        else
        {
            result = CDKResultENoMemory;
        }

        return result;
    }
    CHX_INLINE const RequestMapInfo GetRequestMapInfo(
        UINT32      requestFrameNum) const
    {
        return m_requestIDToFrameNumber[requestFrameNum % MaxOutstandingRequests];
    }

    VOID DestroyInternalBufferQueue(
        UINT32 bufferID);

    VOID DestroyAllInternalBufferQueues();

    CDKResult AddNewInternalBufferQueue(
        ChiStream*                  pChiStream,
        CHIBufferManagerCreateData* pBufferCreateData,
        UINT32*                     pBufferID,
        const CHAR*                 pBufferManagerName);

    CHIBufferManager* GetBufferManager(
        UINT32 pipelineIndex);

    CDKResult ReleaseRDIBufferReference(
        UINT32              pipelineIndex,
        buffer_handle_t*    pBufferHandle);

    CDKResult GetOutputBufferFromRDIQueue(
        UINT32                      pipelineIndex,
        camera3_stream_buffer_t*    pOutputbuffer);

    BOOL IsRDIBufferMetaReadyForInput(
        UINT32 frameNumber,
        UINT32 pipelineIndex);

    CDKResult GetInputBufferFromRDIQueue(
        UINT32                      frameNumber,
        UINT32                      pipelineIndex,
        camera3_stream_buffer_t*    pInputBuffer,
        camera_metadata_t**         ppMetadata);

    camera_metadata_t* FillMetadataForRDIQueue(
        UINT32                      frameNumber,
        UINT32                      pipelineIndex,
        const camera_metadata_t*    pMetadata);

    CDKResult UpdateBufferReadyForRDIQueue(
        CHISTREAMBUFFER*    pBuffer,
        UINT32              frameNumber,
        UINT32              pipelineIndex,
        BOOL                isBufferReady);

    CDKResult WaitForBufferMetaReady(
        UINT32 frameNumber,
        UINT32 pipelineIndex);

    CHX_INLINE UINT32 GetLastReadyFrameNum(UINT32 pipelineIndex)
    {
        return m_MCTargetBuffer[pipelineIndex].lastReadySequenceID;
    }

    CHX_INLINE BOOL IsMultiCameraUsecase()
    {
        return (UsecaseId::MultiCamera == GetUsecaseId());
    }

    CHX_INLINE UINT32 GetAppFrameNumFromReqId(
        const PipelineData* pipelineData,
        UINT32 pipelineReqId) const
    {
        return pipelineData->seqIdToFrameNum[pipelineReqId % MaxPendingFrameNumber];
    }

    CHX_INLINE UINT32 GetAppFrameNumFromReqId(
        UINT32 sessionId,
        UINT32 pipelineId,
        UINT32 pipelineReqId) const
    {
        const PipelineData* pipelineData = GetPipelineData(sessionId, pipelineId);
        return GetAppFrameNumFromReqId(pipelineData, pipelineReqId);
    }

    CHX_INLINE UINT32 GetReqIdForAppFrameNum(
        PipelineData* pipelineData,
        UINT32 appFrameNum)
    {
        UINT32 index = (pipelineData->seqId % MaxPendingFrameNumber);

        pipelineData->seqIdToFrameNum[index] = appFrameNum;

        return pipelineData->seqId++;
    }

    CHX_INLINE UINT32 GetLastReqId(
        PipelineData* pipelineData) const
    {
        return (pipelineData->seqId);
    }

    CDKResult GetTargetBuffer(
        UINT32                   frameNumber,
        TargetBuffer*            pTargetBuffer,
        camera_metadata_t*       pMetaData,     /// Leave null to get metadata from buffer as is (for reprocess)
        camera3_stream_buffer_t* inputBuffer);

    CDKResult GetZSLMode(
        UINT32*                 zslMode,
        camera_metadata_t*      pMetadata);
    BOOL IsFlashRequired(
        camera_metadata_t*  pMetadata);

    VOID ProcessAndReturnFinishedResults();

    VOID ReturnFinishedResult(const camera3_capture_result_t *pResult)
    {
        Usecase::ReturnFrameworkResult(pResult, m_cameraId);
    }

    VOID ProcessErrorMessage(
        const ChiMessageDescriptor* pMessageDescriptor);

    CHX_INLINE UINT32* GetEffectMode()
    {
        return &m_effectModeValue;
    }

    CHX_INLINE UINT32* GetSceneMode()
    {
        return &m_sceneModeValue;
    }

    CHX_INLINE UINT32* GetFeature1Mode()
    {
        return &m_tuningFeature1Value;
    }

    CHX_INLINE UINT32* GetFeature2Mode()
    {
        return &m_tuningFeature2Value;
    }

    CHX_INLINE VOID SetFeature2Mode(
        UINT32 tuningFeature2Value)
    {
         m_tuningFeature2Value = tuningFeature2Value;
    }

    virtual CDKResult Flush();

    VOID WaitForDeferThread();

    /// Dump into a file debug data from nodes
    VOID      DumpDebugData(
        const VOID*     pDebugData,                                 ///< Pointer to debug-data to be dump into file
        const SIZE_T    sizeDebugData,                              ///< Size of debug-data
        const UINT32    sessionId,                                  ///< Session identification number
        const UINT32    cameraId,                                   ///< Corresponding camera ID
        const UINT32    resultFrameNumber);                         ///< Corresponding result frame

    /// Process debug-data for all usecase and features
    VOID      ProcessDebugData(
        const CHICAPTURERESULT*     pResult,                        ///< Capture result information
        const VOID*                 pPrivateCallbackData,           ///< Usefull to get call-back data
        const UINT32                resultFrameNum);                ///< Corresponding result frame

protected:
    static const UINT32 InvalidId = static_cast<UINT32>(-1);

    /// Usecase initialization data
    struct UsecaseInitializationData
    {
        UINT32                             cameraId;                   ///< Camera Id
        UINT32                             numPipelines;               ///< Number of entries in pPipelineTargetCreateDescs
        ChiPipelineTargetCreateDescriptor* pPipelineTargetCreateDescs; ///< Array of complete (with streams) create descriptors
    };

    /// Information about queued request
    struct RequestInfo
    {
        UINT32              frameNumber;
        CHISTREAMBUFFER*    pBuffer;
        const VOID*         pSettings;
    };

    static VOID SessionCbCaptureResult(
        ChiCaptureResult* pCaptureResult,                      ///< Capture result
        VOID*             pPrivateCallbackData);               ///< Private callback data

    static VOID SessionCbNotifyMessage(
        const ChiMessageDescriptor* pMessageDescriptor,        ///< Message Descriptor
        VOID*                       pPrivateCallbackData);     ///< Private callback data

    /// Destroy/Cleanup the object
    virtual VOID Destroy(
        BOOL isForced);

    /// Does one time initialization of the created object
    CDKResult Initialize(
        ChiCallBacks*      pCallbacks); ///< Callbacks to be called for each session/pipeline

    /// Called after each pipeline creation for overriding state after each pipeline creation
    virtual VOID PipelineCreated(
        UINT32 sessionId,  ///< Index of the session for which the pipeline was created
        UINT32 pipelineId) ///< Index of the pipeline created (within the context of the session)
    {
        (void)sessionId;
        (void)pipelineId;
        CHX_LOG_ERROR("MULTIFRAME CameraUsecaseBase pipelineId: %d", pipelineId);
    }

    /// Execute capture request
    virtual CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* pRequest);              ///< Request parameters

    /// Used to determine if stream needs buffers allocated
    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        (void)pStream;

        return FALSE;
    }

    CameraUsecaseBase() = default;
    virtual ~CameraUsecaseBase();

    UINT         m_numSessions;                                   ///< Represents entry count in m_pSessionPipelines
    UINT*        m_pPipelineToSession;                            ///< Mapping of pipelines to sessions
                                                                  ///  if NULL, 1-1 mapping will be used
    UINT*        m_pPipelineToCamera;                             ///< Mapping of pipelines to cameraId
    BOOL         m_isMetadataAvailable[MaxOutstandingRequests];   ///< Is metadata ready to sent
    BOOL         m_isMetadataSent[MaxOutstandingRequests];        ///< Has metadata been sent already
    TargetBuffer m_targetBuffers[MaxChiStreams];                  ///< Array of RDI information
    SessionData  m_sessions[MaxSessions];                         ///< Array of session information
    UINT         m_rtSessionIndex;                                ///< Index of session containing last realtime pipeline
    UINT32       m_effectModeValue;                               ///< effect value
    UINT32       m_sceneModeValue;                                ///< scenemode value
    UINT32       m_tuningFeature1Value;                           ///< tuning Feature1Value value
    UINT32       m_tuningFeature2Value;                           ///< tuning Feature2Value value
    ChiStream*   m_pFrameworkOutStreams[MaxChiStreams];           ///< frameworks stream
    ChiStream*   m_pClonedStream[MaxChiStreams];                  ///< reproc internal stream
    BOOL         m_bCloningNeeded;                                ///< cloning case or not
    UINT32       m_numberOfOfflineStreams;                        ///< offline streams count
    TargetBuffer m_MCTargetBuffer[MaxChiStreams];                 ///< target buffer array for multi camera
    UINT32       m_targetBuffersCount;                            ///< target buffer count for multi camera
    UINT32       m_RDIBufferID[2];                                ///< RDI buffer ID
    RequestMapInfo m_requestIDToFrameNumber[MaxOutstandingRequests]; ///< request map information
    ChiStream*   m_pRdiStream;                                    ///< Allocated internal RDI stream
    ChiStream*   m_pRdiStreamAux;
    BOOL         m_GpuNodePresence;
    CHITAGSOPS   m_vendorTagOps;                                  ///< Vendor Tag Ops
    UINT32       m_firstFrameAfterIrLedOn;                        ///< Framework Frame number for which IR LED mode is requested
    camera_metadata_t* m_pLastSettingsWithIrLedOn;                ///< Save last known metadata to send aftet IR Mode on
private:

    // Do not allow the copy constructor or assignment operator
    CameraUsecaseBase(const CameraUsecaseBase&) = delete;
    CameraUsecaseBase& operator= (const CameraUsecaseBase&) = delete;

    /// Helper for creating sessions based on pipeline mappings
    CDKResult CreateRTSessions(
        ChiCallBacks* pCallbacks);

    /// Helper for creating sessions based on pipeline mappings
    CDKResult CreateOfflineSessions(
        ChiCallBacks* pCallbacks);

    /// Helper for creating a pipeline object from descriptor
    CDKResult CreatePipeline(
        UINT32                              cameraId,
        ChiPipelineTargetCreateDescriptor*  pPipelineDesc,
        PipelineData*                       pPipelineData);

    /// Replace the capture request metadata with a copy of it + additional new settings inserted by the Chi override
    CDKResult MergeMetadata(
        camera_metadata_t*       pDst,
        const camera_metadata_t* pSrc);

    /// Process the saved results
    virtual VOID ProcessResults() { }

    VOID SessionProcessResult(
        ChiCaptureResult*           pCaptureResult,            ///< Result Descriptor
        const SessionPrivateData*   pSessionPrivateData);      ///< Private callback data

    VOID SessionProcessMessage(
        const ChiMessageDescriptor* pMessageDescriptor,        ///< Message Descriptor
        const SessionPrivateData*   pSessionPrivateData);      ///< Private callback data

    VOID DeferOfflineSession();

    VOID StartDeferThread();

    static VOID* DeferOfflineSessionThread(VOID * pThreadData);

    VOID DestroyDeferResource();


    SessionPrivateData      m_perSessionPvtData[MaxSessions];    ///< Per session private data
    CHIPRIVDATA             m_privData[MaxOutstandingRequests];  ///< private data for request

    volatile BOOL           m_deferOfflineSessionDone;              ///< Flag indicate if snapshot session
    volatile BOOL           m_deferOfflineThreadCreateDone;         ///< Flag indicate if thread is created
    Condition*              m_pDeferOfflineDoneCondition;           ///< Condition for defer snapshot done
    Mutex*                  m_pDeferOfflineDoneMutex;               ///< Mutex for defer snapshot done
    PerThreadData           m_deferOfflineSessionThreadData;        ///< Thread data for defer
    ChiCallBacks*           m_pCallBacks;                           ///< Notify and captureResult callbacks for each pipeline
    // Dump Debug/Tuning data
    UINT32      m_debugLastResultFrameNumber;                       ///< Helper to track last processed frame number
    UINT32      m_debugMultiPassCount;                              ///< Count proc calls for a single multi-frame capture
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Advanced camera Usecase usecase
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AdvancedCameraUsecase : public CameraUsecaseBase
{
public:

    /// Static create function to create an instance of the object
    static AdvancedCameraUsecase* Create(
        LogicalCameraInfo*              pCameraInfo,   ///< Camera info
        camera3_stream_configuration_t* pStreamConfig,  ///< Stream configuration;
        UsecaseId                       usecaseId);     ///< Identifier for the usecase function

    ChiPipelineTargetCreateDescriptor* GetPipelineDescriptor(
        AdvancedPipelineType type);

    ChiUsecase* GetChiUseCase()
    {
        return m_pChiUsecase;
    }

    INT32  GetPipelineIdByAdvancedPipelineType(AdvancedPipelineType type);
    INT32  GetAdvancedPipelineTypeByPipelineId(INT32 pipelineId);
    UINT64 GetRequestShutterTimestamp(UINT64 frameNumber);

    VOID        SetRequestToFeatureMapping(
        UINT sessionId,
        UINT64 requestId,
        Feature* pFeature);
    Feature*    GetFeatureFromRequestMapping(
        UINT sessionId,
        UINT64 requestId);

    enum SharedStreamType
    {
        PreviewStream,
        RdiStream,
        RdiStreamAux,
        Bayer2YuvStream,
        JPEGInputStream,
        SnapshotStream,
        YuvCBStream,
        VideoStream,
        YuvInputStream,
    };
    ChiStream*  GetSharedStream(SharedStreamType streamType);

    /*
     * return a unique session id for the given pipeline group
     * if any id in the pipelineGroup already has the session id, then
     * all of the pipeline will share this session id
     *
     * return a new session id if none of the pipeline id was associated with any session
     * return the existing if one of the pipeline id is already associated with a session
     */
    UINT32      GetUniqueSessionId(
        AdvancedPipelineType*   pipelineGroup,
        UINT32                  pipelineCount);

    /*
     * Set the cameraId for each pipeline
     */
    VOID        SetPipelineCameraId(
        AdvancedPipelineType*   pipelineGroup,
        UINT*                   cameraId,
        UINT32                  pipelineCount);

    /*
     * Config the shared Rdi stream
     */
    VOID        ConfigRdiStream(ChiSensorModeInfo* pSensorInfo);

    /*
     * Get the actual physical cameraid per CameraRoleType
     */
    UINT        GetPhysicalCameraId(CameraRoleType role);

    CHX_INLINE BOOL IsLLSNeeded()
    {
        return m_isLLSNeeded;
    }

protected:

    /// Destroy/Cleanup the object
    virtual VOID Destroy(
    BOOL isForced);

    // Flush
    virtual CDKResult Flush();

    /// Callback to inform of a pipeline create
    virtual VOID PipelineCreated(
        UINT sessionId,     ///< Id of session created
        UINT pipelineId);   ///< Pipeline of session created

    /// Performs checks for whether a stream is internal
    virtual BOOL StreamIsInternal(
        ChiStream* pStream);

    /// Execute capture request
    virtual CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* pRequest);              ///< Request parameters

    AdvancedCameraUsecase() = default;
    virtual ~AdvancedCameraUsecase();

protected:

    /// Callback for a result from the driver
    static VOID      ProcessResultCb(
        CHICAPTURERESULT*   pResult,
        VOID*               pPrivateCallbackData)
    {
        SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
        static_cast<AdvancedCameraUsecase*>(pCbData->pUsecase)->ProcessResult(pResult, pPrivateCallbackData);
    }

    /// Callback for a message from the driver
    static VOID      ProcessMessageCb(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData)
    {
        SessionPrivateData* pCbData = static_cast<SessionPrivateData*>(pPrivateCallbackData);
        static_cast<AdvancedCameraUsecase*>(pCbData->pUsecase)->ProcessMessage(pMessageDescriptor, pPrivateCallbackData);
    }

    // Do not allow the copy constructor or assignment operator
    AdvancedCameraUsecase(const AdvancedCameraUsecase&) = delete;
    AdvancedCameraUsecase& operator= (const AdvancedCameraUsecase&) = delete;

    /// Does one time initialization of the created object
    CDKResult Initialize(
        LogicalCameraInfo*              pCameraInfo,     ///< Camera info
        camera3_stream_configuration_t* pStreamConfig,  ///< Stream configuration
        UsecaseId                       usecaseId);     ///< Identifier for the usecase function

    /// Sets up the class based on enabled features
    CDKResult FeatureSetup(
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration

    /// Performs usecase struct selection and stream matching
    CDKResult SelectUsecaseConfig(
        LogicalCameraInfo*              pCameraInfo,   ///< Camera info
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration

    /// Perform actions after [Camera]Usecase has been created and initialized
    CDKResult PostUsecaseCreation(
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration

    /// Parse result metadata
    CDKResult ParseResultMetadata(
        CHICAPTURERESULT*           pResult);

    /// Processor called by callback
    VOID      ProcessResult(
        CHICAPTURERESULT*           pResult,
        VOID*                       pPrivateCallbackData);

    /// Processor called by callback
    VOID      ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData);

    VOID      SelectFeatures(camera3_stream_configuration_t* pStreamConfig);

    Feature*  SelectFeatureToExecuteCaptureRequest(camera3_capture_request_t* pRequest);

    VOID      SetupSharedPipeline(
        ChiPipelineTargetCreateDescriptor* pPipeline,
        AdvancedPipelineType pipelineType);

    VOID      BuildUsecase(
        LogicalCameraInfo*              pCameraInfo,   ///< Camera info
        camera3_stream_configuration_t* pStreamConfig);  ///< Stream configuration

    Feature*  FindFeatureToProcessResult(
        CHICAPTURERESULT*           pResult,
        VOID*                       pPrivateCallbackData);

    Feature*  FindFeaturePerType(FeatureType type);

    ChiPipelineTargetCreateDescriptor GetPipelineDescriptorFromUsecase(
        ChiUsecase* pChiUsecase,
        AdvancedPipelineType id);

    BOOL       AdvacnedFeatureEnabled()
    {
        return 0 != m_enabledFeaturesCount;
    }

    /// check if there is snapshot stream for this request
    BOOL hasSnapshotStreamRequest(
        camera3_capture_request_t* pRequest) const
    {
        BOOL hasSnapshotRequest = FALSE;
        for (UINT i = 0; i < pRequest->num_output_buffers; i++)
        {
            if ((NULL != m_pSnapshotStream) &&
                (CHISTREAM *)pRequest->output_buffers[i].stream == m_pSnapshotStream)
            {
                hasSnapshotRequest = TRUE;
                break;
            }
        }
        return hasSnapshotRequest;
    }

    /// @todo need feature list instead of static pointers?
    FeatureRawJPEG* m_pRawJPEG;

    Feature*    m_enabledFeatures[MaxFeatureSupported];
    Feature*    m_pActiveFeature;
    UINT32      m_enabledFeaturesCount;

    /* a mapping of pPipelineTargetCreateDesc and it's information */
    PipelineStatus m_pipelineStatus[AdvancedPipelineType::PipelineCount];
    UINT32         m_pipelineCount;

    /* mapping between the pipeline to the session */
    INT32          m_pipelineToSession[AdvancedPipelineType::PipelineCount];
    UINT32         m_uniqueSessionId;
    UINT32         m_realtimeSessionId;
    /* mapping between the pipeline to the cameraid */
    UINT           m_pipelineToCameraId[AdvancedPipelineType::PipelineCount];

    Mutex*         m_pResultMutex; ///< Result mutex
    UINT64         m_shutterTimestamp[MaxOutstandingRequests];      ///< Is shutter message sent

    SessionRequestIdFeatureMapping  m_sessionRequestIdFeatureMapping[MaxOutstandingRequests];
    UINT32                          m_sessionRequestIdFeatureMappingCount;

    // preview pipeline
    ChiStream*  m_pPreviewStream;              ///< Tracking of the stream used for preview
    ChiStream*  m_pBayer2YuvStream;
    ChiStream*  m_pJPEGInputStream;
    ChiStream*  m_pVideoStream;
    ChiStream*  m_pYuvCBStream;
    ChiStream*  m_pYuvInStream;

    // snapshot pipeline
    ChiStream*  m_pSnapshotStream;             ///< Tracking of the stream used for snapshot

    BOOL        m_isRdiStreamImported;

    LogicalCameraInfo*  m_pLogicalCameraInfo;
    ASDOutput           m_asdResult;           ///< ASD result

    // flags to control advance snapshot
    BOOL                m_isLLSNeeded;         ///< Flag to indicate low light snapshot (LLS)
    ChiCallBacks*       m_pCallbacks;          ///< Notify and captureResult callbacks for each pipeline
    BOOL                m_isFlashRequired;     ///< Flag to indicate if flash is required
};

#endif // CHXADVANCEDCAMERAUSECASE_H
