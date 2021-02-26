////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturerawjpeg.h
/// @brief CHX feature raw + jpeg(simultaneous raw and jpeg capture) class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATURERAWJPEG_H
#define CHXFEATURERAWJPEG_H

#include <assert.h>

#include "chxincs.h"
#include "chxfeature.h"
#include "chxusecaseutils.h"

////////////////////////
/// Forward Declarations
////////////////////////
class AdvancedCameraUsecase;

class FeatureRawJPEG : public Feature
{
public:
    static  FeatureRawJPEG* Create(
        AdvancedCameraUsecase* pUsecase);

    CDKResult           Initialize(
        AdvancedCameraUsecase* pUsecase);

    virtual VOID        Destroy(BOOL isForced);
    virtual VOID    Pause(
                BOOL isForced);
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

    // We need one internal stream to handle Raw+JPEG.
    BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        return (pStream == m_pRdiStream) ? TRUE : FALSE;
    }

protected:
    FeatureRawJPEG() = default;
    virtual ~FeatureRawJPEG() = default;

    AdvancedCameraUsecase* m_pUsecase;                                    ///< Pointer to owning usecase class

    ChiUsecase*        m_pChiUsecase;                                     ///< Copy of RawJPEG usecase that gets adjusted
    ChiStream*         m_pRdiStream;                                      ///< Allocated internal RDI stream
    ChiStream*         m_pRdiFrameworkStream;                             ///< Framework RDI output stream
    ChiStream*         m_pPreviewStream;                                  ///< Tracking of the stream used for preview
    ChiStream*         m_pSnapshotStream;                                 ///< Tracking of the stream used for snapshot
    UINT               m_rdiStreamIndex;                                  ///< Stream/target index for the RDI stream
    TargetBuffer*      m_pRdiTargetBuffer;                                ///< TargetBuffer in the CameraUsecase
    UINT64             m_shutterTimestamp[MaxOutstandingRequests];        ///< Tracking of shutter timestamp for each request
    UINT32             m_maxSnapshotReqId;                                ///< Last valid ID to move snapshotReqId to
                                                                          ///  Owned by the main thread
    UINT32             m_snapshotReqId;                                   ///< Next ID to provide snapshot request
    UINT32             m_snapshotReqIdToFrameNum[MaxOutstandingRequests]; ///< Mapping of snapshotReqId to framework frame

    camera_metadata_t*       m_pSnapshotInputMeta[MaxOutstandingRequests]; ///< The metadata for the request
    camera3_stream_buffer_t  m_snapshotBuffers[MaxOutstandingRequests][2]; ///< Result buffers from app for snapshot
    UINT                     m_snapshotBufferNum[MaxOutstandingRequests];  ///< Buffer count

    BOOL               m_isSnapshotFrame[MaxOutstandingRequests];         ///< Is a snapshot request
    BOOL               m_isRdiFrameRequested[MaxOutstandingRequests];     ///< Is Rdi frame requested from frameworks
    Mutex*             m_pResultMutex;                                    ///< Result mutex

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
    volatile UINT32    m_aFlushInProgress;
    volatile UINT32    m_aPauseInProgress;                                ///< Is pause in progress

    BOOL               m_useDummyPreview;                                 ///< Whether to use internal dummy preview

    static const UINT  PreviewPipelineIndex  = 1;
    static const UINT  SnapshotPipelineIndex = 0;

    static const UINT32 DefaultPreviewWidth  = 640;                       /// default width for dummy preview stream
    static const UINT32 DefaultPreviewHeight = 480;                       /// default height for dummy preview stream
    static const UINT32 MaxStreamBuffers     = 8;                         /// Max number of stream buffers

    camera3_stream_buffer_t m_pRdiFrameworkStreamBuffer[MaxOutstandingRequests];    ///< Rdi buffer provided by framework
    BOOL                    m_isRdiFormatRaw16;                                     ///< Is Rdi format raw16

    CHIPRIVDATA             m_privData[MaxOutstandingRequests];                     ///< Result private data
private:
    // Do not allow the copy constructor or assignment operator
    FeatureRawJPEG(const FeatureRawJPEG&) = delete;
    FeatureRawJPEG& operator= (const FeatureRawJPEG&) = delete;

    /// Main entry function for the offline Request thread
    static VOID* RequestThread(VOID* pArg);
    VOID RequestThreadProcessing();
};

#endif // CHXFEATURERAWJPEG_H
