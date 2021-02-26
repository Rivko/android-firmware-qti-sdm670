////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturemfnr.h
/// @brief CHX feature mfnr class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATUREMFNR_H
#define CHXFEATUREMFNR_H

#include <assert.h>

#include "chi.h"
#include "chxfeature.h"
#include "chxincs.h"
#include "chxusecaseutils.h"

////////////////////////
/// Forward Declarations
////////////////////////
class AdvancedCameraUsecase;

class FeatureMFNR : public Feature
{
public:
    static FeatureMFNR* Create(
        AdvancedCameraUsecase* pUsecase);

    CDKResult Initialize(
        AdvancedCameraUsecase* pUsecase);

    virtual VOID Destroy(
        BOOL isForced);

    virtual VOID    Pause(
        BOOL isForced);

    virtual VOID        Flush(BOOL isInProgress);

    virtual ChiUsecase* OverrideUsecase(
        LogicalCameraInfo*              pCameraInfo,
        camera3_stream_configuration_t* pStreamConfig);

    virtual VOID PipelineCreated(
        UINT32 sessionId,
        UINT32 pipelineId);

    virtual CDKResult ExecuteProcessRequest(
        camera3_capture_request_t* pRequest);

    virtual VOID ProcessResult(
        CHICAPTURERESULT* pResult,
        VOID*             pPrivateCallbackData);

    virtual VOID ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData);

    virtual FeatureType GetFeatureType()
    {
        return FeatureType::MFNR;
    }

    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        BOOL isInternal = (pStream != NULL &&
                           (pStream == m_pRdiStream[CameraType::Wide] || pStream == m_pRdiStream[CameraType::Tele]));
        return isInternal;
    }

    virtual INT32 GetRequiredPipelines(
        AdvancedPipelineType* pPipelines,
        INT32                 size);

    virtual CDKResult GetRequestInfo(
        camera3_capture_request_t* pRequest,
        FeatureRequestInfo*        pOutputRequests);

protected:
    FeatureMFNR() = default;
    virtual ~FeatureMFNR() = default;

    struct MFNRInputInfo
    {
        CHISTREAMBUFFER bufferArray[BufferQueueDepth];  ///< CHI Stream buffer will be as MFNR buffer input
        VOID*           pMetadata[BufferQueueDepth];    ///< Metadata array will be as MFNR pipeline metadata input
        UINT32          numOfBuffers;                   ///< number of buffers needed for this MFNR process
    };

    UINT64 m_shutterTimestamp[MaxOutstandingRequests];  ///< Is shutter message sent
    UINT32 m_mfnrZslQAnchorIndices[BufferQueueDepth];   ///< Best to least good anchor frames
    UINT32 m_nextStageAnchorIndex;                      ///< Next stage anchor index

    static const UINT        ZSLInputFrameOffset = 2;   ///< ZSL Input frame to select
    camera_metadata_t*       m_pReplacedMetadata;       ///< Replaced metadata
    size_t                   m_replacedMetadataSize;    ///< Replaced metadata size
    const camera_metadata_t* m_pOriginalMetadata;       ///< Original pointer to the metadata

    enum class AnchorFrameSelectionMode
    {
         TimeStamp,                                     ///< Will use the desired image as anchor; process others in increasing temporal difference
         Sharpness,                                     ///< Process images in order of decreasing sharpness (focus value)
         Lighting                                       ///< Process images with similar lighting first
    };

    enum class AnchorFrameSelectionAlgorithm
    {
         None,                                          ///< As per current implementation
         Fixed,                                         ///< Apply fixed known order
         NonFixed                                       ///< Apply non-fixed order based on AnchorFrameSelectionMode
    };

    struct AnchorFrameSelectionData
    {
         FLOAT                    focusValue[BufferQueueDepth];
         UINT32*                  histogram[BufferQueueDepth];
         UINT32                   minHistrogramBin;
         UINT32                   maxHistrogramBin;
         UINT64                   timestamps[BufferQueueDepth];
         UINT32                   numOfImagesToBlend;
         AnchorFrameSelectionMode anchorFrameSelectionMode;
         UINT32                   desiredAnchorFrameIndex;
         UINT64                   anchorFrameTimeRange;
         UINT32                   brightnessTolerance;
         BOOL                     removeExpectedBadImages;
    };

    CDKResult FetchVendorTagsForAnchorFrameSelection(
        UINT32* pVendorTagLocationFocusValue,
        UINT32* pVendorTagLocationBuckets,
        UINT32* pVendorTagLocationStats);

    CDKResult PopulateAnchorFrameSelectionDataFromVTags(
        AnchorFrameSelectionData* pAnchorFrameSelectionData);

    UINT32 CalculateBrightness(
        UINT32* histogram,
        UINT    minHistrogramBin,
        UINT    maxHistrogramBin);

    CDKResult PopulateAnchorFrameSelectionData(VOID);

    CAMX_INLINE UINT32 getZSLInputFrameOffset()
    {
        return ZSLInputFrameOffset;
    }

    CDKResult SelectFixedFrameOrder(
        UINT* pFrameNumber,
        UINT  totalNumerOfFrames);

    CDKResult SelectNonFixedFrameOrder(
        UINT* pFrameNumber,
        UINT  totalNumerOfFrames);

    CDKResult SelectMFNRAnchorFrameAndFrameOrder(
        MFNRInputInfo*                 mfnrInputInfo,
        camera3_stream_buffer_t*       selectionInputBuffer,
        size_t                         selectionInputBufferSize,
        camera_metadata_t**            ppMergedMetadata,
        AnchorFrameSelectionAlgorithm* anchorSelectionAlgorithm);

    CDKResult CreateMFNRInputInfo(
        MFNRInputInfo*             pInputInfo,
        camera3_capture_request_t* pRequest);

    virtual UINT32 GetRequiredFramesForSnapshot(
        const camera_metadata_t* pMetadata)
    {
        (VOID)pMetadata;

        return m_mfnrTotalNumFrames;
    }

    enum CameraType
    {
        Wide = 0,
        Tele = 1,
        Count
    };

    AdvancedCameraUsecase*     m_pUsecase;                                         ///< Pointer to owning usecase class

    ChiUsecase*                m_pChiUsecase;                                      ///< Copy of ZSL usecase that gets adjusted
    ChiStream*                 m_pRdiStream[CameraType::Count];                    ///< Allocated internal RDI stream
    ChiStream*                 m_pPreviewStream;                                   ///< Tracking of the stream used for preview
    ChiStream*                 m_pSnapshotStream;                                  ///< Tracking of the stream used for snapshot
    UINT32                     m_rdiStreamIndex;                                   ///< Stream/target index for RDI stream
    TargetBuffer*              m_pRdiTargetBuffer;                                 ///< TargetBuffer in the CameraUsecase

    UINT32                     m_maxSnapshotReqId;                                 ///< Last valid ID to move snapshotReqId to
                                                                                   ///  Owned by the main thread
    UINT32                     m_snapshotReqIdToFrameNum[MaxOutstandingRequests];  ///< snapshotReqId to framework frame mapping
    VOID*                      m_pSnapshotInputMeta[MaxOutstandingRequests];       ///< The metadata for the request
    camera3_stream_buffer_t    m_snapshotBuffers[MaxOutstandingRequests][2];       ///< Result buffers from app for snapshot
    UINT                       m_snapshotBufferNum[MaxOutstandingRequests];        ///< Buffer count
    BOOL                       m_isSnapshotFrame[MaxOutstandingRequests];          ///< Is a snapshot request
    BOOL                       m_isSnapshotMetadataNeeded;                         ///< Is snapshot metadata needed.

    UINT32                     m_sensorModeIndex;                                  ///< sensorModeIndex
    CHIPRIVDATA                m_privData[MaxOutstandingRequests];

    UINT32                     m_snapshotPipelineIndex;
    UINT32                     m_prefilterPipelineIndex;
    UINT32                     m_blendPipelineIndex;
    UINT32                     m_postfilterPipelineIndex;
    UINT32                     m_scalePipelineIndex;
    UINT32                     m_previewPipelineIndex;

    UINT32                     m_realtime;
    UINT32                     m_offline;
    UINT32                     m_requestFrameNumber;
    UINT32                     m_internalRequestId;
    UINT32                     m_activeCameraId;

    enum MFNRStage
    {
        MfnrStageSnpashot = 0,
        MfnrStagePrefilter,
        MfnrStageBlend,
        MfnrStagePostfilter,
        MfnrStageMax
    };

    enum MFNRReference
    {
        MfnrReferenceFull = 0,
        MfnrReferenceDS4,
        MfnrReferenceMax
    };

    const CHAR* mfnrBufferManagerNames[4] =
    {
        "MfnrReferenceFullBufferManager",
        "MfnrReferenceDS4BufferManager",
        "MfnrReferenceDS16BufferManager",
        "MfnrReferenceDS64BufferManager",
    };

    static const UINT MfnrMaxPreFilterStageBuffers = 2;
    static const UINT MfnrMaxBpsRegOutBuffers      = 1;
    static const UINT MfnrDefaultInputFrames       = 5;
    static const UINT MfnrMaxInputRDIFrames        = 12;

    UINT32                     m_mfnrTotalNumFrames;                                        ///< Total num of MFNR frames [3..5]
    UINT32                     m_autoMfnr;

    ChiStream*                 m_pPrefilterOutStream[CameraType::Count][MfnrReferenceMax];  ///< Prefilter out stream
    ChiStream*                 m_pBlendOutStream[CameraType::Count][MfnrReferenceMax];      ///< Blend out stream
    ChiStream*                 m_pBlendInStream[CameraType::Count][MfnrReferenceMax];       ///< Blend in stream
    ChiStream*                 m_pScaleInStream[CameraType::Count][MfnrReferenceMax];       ///< Scale in stream
    ChiStream*                 m_pMfnrBpsRegOutStream[CameraType::Count];                   ///< Reg out stream
    ChiStream*                 m_pMfnrBpsRegInStream[CameraType::Count];                    ///< Reg in stream
    ChiStream*                 m_pMfnrPostFilterOutStream[CameraType::Count];               ///< Post filter out stream

    camera_metadata_t*         m_pInterStageMetadata;
    camera_metadata_t*         m_pApplicationInputMeta;

    CHIBufferManager*          m_pMfnrBpsRegOutBufferManager[CameraType::Count];            ///< MFNR BPS Reg out Buffer manager
    UINT                       m_mfnrSessionReqId;                                          ///< Chi request ID for MFNR
    UINT                       m_mfnrReqIdToFrameNum[MaxOutstandingRequests];               ///< Chi req Id to framework frame num
    CHIBufferManager*          m_pMfnrBufferManager[CameraType::Count][MfnrStageMax][MfnrReferenceMax]; ///< MFNR Buffer Managers

    Mutex*                     m_pMfnrResultMutex;                             ///< Blend Result mutex
    Condition*                 m_pMfnrResultAvailable;                         ///< Wait till all results are available
    volatile BOOL              m_resultsAvailable;
    UINT                       m_processingOrder[BufferQueueDepth /* - 1 */];  ///< Max images to Blend
    UINT                       m_numOfImagesCaptured;
    BOOL                       m_triggerMFNRReprocess[MaxOutstandingRequests]; ///< If trigger reprocess for a give internal Req ID
    Mutex*                     m_pRDIResultMutex;                              ///< RDI result mutex
    Condition*                 m_pRDIResultAvailable;                          ///< Condition to wait all RDI buffers are ready
    BOOL                       m_allRDIResultsAvaliable;                       ///< ALL required RDI buffers and metadatas are available
    BOOL                       m_isLLSSnapshot;                                ///< Flag to indicate LLS snapshot or not

    BOOL                       m_blockPreviewForSnapshot;                      ///< If block up-coming preview during snapshot request
    Mutex*                     m_pSnapshotResultMutex;                         ///< Snapshot result mutex
    Condition*                 m_pSnapshotResultAvailable;                     ///< Condition to wait final snapshot buffer ready
    BOOL                       m_snapshotResultAvailable;                      ///< Flag to inidicate if final snapshot result availabe
    volatile UINT32            m_aFlushInProgress;                             ///< Is flush in progress
    volatile UINT32            m_aPauseInProgress;                             ///< Is pause in progress

    struct MfnrStageResult
    {
        VOID*            pMetadata;                                                    ///< Metadata
        CHISTREAMBUFFER  refOutputBuffer[MfnrReferenceMax];                            ///< Ref buffer placeholder
    };

    UINT32                     m_numExpectedStageBuffers;
    MfnrStageResult            m_preFilterStageResult;
    MfnrStageResult            m_scaleStageResult;
    MfnrStageResult            m_blendStageResult;
    MfnrStageResult            m_postFilterStageResult;

    CHISTREAMBUFFER            m_bpsRegResultBuffer;

    UINT32                     m_remainingPrefilterStageResults;
    UINT32                     m_remainingScaleStageResults;
    UINT32                     m_remainingBlendStageResults;
    UINT32                     m_remainingPostFilterStageResults;
    UINT32                     m_remainingSnapshotStageResults;

    // Request thread info
    PerThreadData              m_offlineRequestProcessThread;                  ///< Thread to process the results
    Mutex*                     m_pOfflineRequestMutex;                         ///< App Result mutex
    Condition*                 m_pOfflineRequestAvailable;                     ///< Wait till SensorListener results
                                                                               ///  are available
    volatile BOOL              m_offlineRequestProcessTerminate;               ///< Indication to SensorListener result
                                                                               ///  thread to terminate itself
    camera3_capture_request_t  m_captureRequest;
    PerfLockManager*           m_pPerfLockManager;

    // Dump Debug/Tuning data
    DebugData                   m_debugDataOffline;                             ///< Offline copy for debug-data
    DebugData                   m_debugDataOfflineSnapshot;                     ///< Snapshot copy for debug-data

private:
    // Do not allow the copy constructor or assignment operator
    FeatureMFNR(const FeatureMFNR&) = delete;
    FeatureMFNR& operator=(const FeatureMFNR&) = delete;

    // Disallow the move constructor or assignment operator
    FeatureMFNR(const FeatureMFNR&&) = delete;
    FeatureMFNR& operator=(const FeatureMFNR&&) = delete;

    INT GetTargetIndex(
        ChiTargetPortDescriptorInfo* pTargetsInfo,
        const char*                  pTargetName);

    CDKResult SubmitOfflineMfnrRequest(
        UINT32                     frameNumber,
        camera3_capture_request_t* pRequest,
        UINT32                     rtPipelineReqId);

    CDKResult TriggerInternalLLSRequests(
        camera3_capture_request_t* pRequest);

    CDKResult GenerateLLSRequestSettings(
        const camera_metadata_t*  pInputSetting,
        UINT32                    numFrames,
        const camera_metadata_t** pOutputSettingArray);

    CAMX_INLINE UINT32 GetRequiredInputFrames()
    {
        return m_mfnrTotalNumFrames;
    }

    CDKResult WaitRDIResultsReady();

    CDKResult GetOutputBuffer(
        CHIBufferManager*        pBufferManager,
        ChiStream*               pChiStream,
        camera3_stream_buffer_t* pOutputBuffer);

    CDKResult ExecuteMfnrRequest(
        MFNRStage                pipelineStage,
        UINT32                   frameNumber,
        UINT32                   numOutputs,
        camera3_stream_buffer_t* pOutputBuffers,
        UINT32                   numInputs,
        camera3_stream_buffer_t* pInputBuffers,
        const VOID*              pSettings);

    UINT GetMFNRTotalFramesByGain(
       camera_metadata_t* pMeta);

    CDKResult PublishMFNRTotalFrames(
        camera_metadata_t* pMeta);

    VOID InitializeInternalStreams(
        CameraType type);

    VOID SetupInternalPipelines(
        CameraType type);

    VOID SetupInternalMFNRPreFilterPipeline(
        CameraType type);

    VOID SetupInternalMFNRBlendPipeline(
        CameraType type);

    VOID SetupInternalMFNRPostFilterPipeline(
        CameraType type);

    VOID SetupInternalMFNRSnapshotPipeline(
        CameraType type);

    UINT GetPipelineIndex(
        MFNRStage pipelineStage);

    /// Main entry function for the Request thread
    static VOID* RequestThread(
        VOID* pArg);

    VOID  RequestThreadProcessing();
};

#endif // CHXFEATUREMFNR_H
