////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeatureyuvcb.h
/// @brief CHX feature yuv callback (zsl) class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATUREYUVCB_H
#define CHXFEATUREYUVCB_H

#include <assert.h>

#include "chxincs.h"
#include "chxfeature.h"
#include "chxusecaseutils.h"

////////////////////////
/// Forward Declarations
////////////////////////
class AdvancedCameraUsecase;
struct TargetBuffer;

class FeatureYuvCB : public Feature
{
public:
    static  FeatureYuvCB* Create(
        AdvancedCameraUsecase* pUsecase);

    CDKResult           Initialize(
        AdvancedCameraUsecase* pUsecase);

    virtual VOID        Destroy(BOOL isForced);
    virtual VOID        Pause(BOOL isForced);
    virtual VOID        Flush(BOOL isInProgress);

    virtual ChiUsecase* OverrideUsecase(
        LogicalCameraInfo*              pCameraInfo,
        camera3_stream_configuration_t* pStreamConfig);

    virtual VOID        PipelineCreated(
        UINT32 sessionId,
        UINT32 pipelineId);

    virtual CDKResult   ExecuteProcessRequest(
        camera3_capture_request_t*  pRequest);

    virtual VOID        ProcessResult(
        CHICAPTURERESULT*           pResult,
        VOID*                       pPrivateCallbackData);

    virtual VOID        ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData);

    virtual FeatureType GetFeatureType()
    {
        return FeatureType::ZSL;
    }

    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        return (pStream == m_pRdiStream) ? TRUE : FALSE;
    }

    virtual INT32 GetRequiredPipelines(
        AdvancedPipelineType* pPipelines,
        INT32 size);

protected:
    FeatureYuvCB() = default;
    virtual ~FeatureYuvCB() = default;
    BOOL IsPseudoZSL(camera_metadata_t* pMetadata);

    AdvancedCameraUsecase* m_pUsecase;                                    ///< Pointer to owning usecase class

    ChiUsecase*        m_pChiUsecase;                                     ///< Copy of ZSL usecase that gets adjusted
    ChiStream*         m_pRdiStream;                                      ///< Allocated internal RDI stream
    ChiStream*         m_pPreviewStream;                                  ///< Tracking of the stream used for preview
    ChiStream*         m_pYuvCBStream;
    ChiStream*         m_pYuvInStream;
    ChiStream*         m_pBlobStream;
    UINT               m_rdiStreamIndex;                                  ///< Stream/target index for the RDI stream
    TargetBuffer*      m_pRdiTargetBuffer;                                ///< TargetBuffer in the CameraUsecase
    UINT32             m_maxSnapshotReqId;                                ///< Last valid ID to move snapshotReqId to
                                                                          ///  Owned by the main thread
    UINT32             m_snapshotReqId;                                   ///< Next ID to provide snapshot request
    UINT32             m_snapshotReqIdToFrameNum[MaxOutstandingRequests]; ///< Mapping of snapshotReqId to framework frame

    camera_metadata_t*       m_pSnapshotInputMeta[MaxOutstandingRequests]; ///< The metadata for the request
    camera3_stream_buffer_t  m_snapshotBuffers[MaxOutstandingRequests][2]; ///< Result buffers from app for snapshot
    UINT                     m_snapshotBufferNum[MaxOutstandingRequests];  ///< Buffer count
    UINT                     m_snapshotInputNum[MaxOutstandingRequests];   ///< Input frame number needed for snapshot.

    CHIPRIVDATA              m_privData[MaxOutstandingRequests];

    BOOL               m_isSnapshotFrame[MaxOutstandingRequests];         ///< Is a snapshot request
    BOOL               m_isRealTimeBlobFrame[MaxOutstandingRequests];     ///< Is it Blob request or YUV request
    Mutex*             m_pResultMutex;                                    ///< Result mutex
    BOOL               m_isFlashRequired;                                 ///< Indicates if Flash is required for snapshot

    Mutex*             m_pRdiAndMetaResultMutex;                          ///< Rdi Result availability mutex
    Condition*         m_pRdiAndMetaResultAvailable;                      ///< Wait till all realtime results are available

    UINT               m_lastRdiFrameAvailable;                           ///< Last valid frame number of
                                                                          ///  available rdi buffer
    UINT               m_lastRealtimeMetadataAvailable;                   ///< Last valid frame number of
                                                                          ///  available metadata from realtime
    UINT               m_offlineRequestWaitingForFrame;                   ///< Real time frame number for which offline request
                                                                          ///  is waiting(for rdi and meta availability).

    /// Offline Request thread info
    PerThreadData      m_offlineRequestProcessThread;                     ///< Thread to process the results
    Mutex*             m_pOfflineRequestMutex;                            ///< App Result mutex
    Condition*         m_pOfflineRequestAvailable;                        ///< Wait till SensorListener results
                                                                          ///  are available
    volatile BOOL      m_offlineRequestProcessTerminate;                  ///< Indication to SensorListener result
                                                                          ///  thread to terminate itself


    UINT32 m_previewPipelineIndex;
    UINT32 m_YuvCBPipelineIndex;
    UINT32 m_offlineBlobPipelineIndex;
    UINT32 m_offlineYuvPipelineIndex;
    UINT32 m_realTimeBlobPipelineIndex;
    UINT32 m_previewSessionId;
    UINT32 m_YuvCBSessionId;
    UINT32 m_offlineBlobSessionId;
    UINT32 m_offlineYuvSessionId;

private:
    // Do not allow the copy constructor or assignment operator
    FeatureYuvCB(const FeatureYuvCB&) = delete;
    FeatureYuvCB& operator= (const FeatureYuvCB&) = delete;

    /// Main entry function for the offline Request thread
    static VOID* RequestThread(VOID* pArg);

    VOID RequestThreadProcessing();
};

#endif // CHXFEATUREYUVCB_H
