////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeatureshdr.h
/// @brief CHX feature HDR class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATURESHDR_H
#define CHXFEATURESHDR_H

#include <assert.h>

#include "chxincs.h"
#include "chxfeature.h"
#include "chxusecase.h"
#include "chxusecaseutils.h"

////////////////////////
/// Forward Declarations
////////////////////////
class AdvancedCameraUsecase;



class FeatureSHDR : public Feature
{
public:
    static  FeatureSHDR* Create(
        AdvancedCameraUsecase* pUsecase);

    CDKResult           Initialize(
        AdvancedCameraUsecase* pUsecase);

    virtual VOID        Destroy(BOOL isForced);

    virtual ChiUsecase* OverrideUsecase(
        LogicalCameraInfo*              pCameraInfo,
        camera3_stream_configuration_t* pStreamConfig);

    virtual VOID        PipelineCreated(
        UINT32 sessionId,
        UINT32 pipelineId);

    virtual CDKResult   ExecuteProcessRequest(
        camera3_capture_request_t*  pRequest);

    CDKResult GenerateRDIRequest(
        camera3_capture_request_t* pRequest);                   ///< Request parameters

    virtual VOID        ProcessResult(
        CHICAPTURERESULT*           pResult,
        VOID*                       pPrivateCallbackData);

    virtual VOID        Pause(BOOL isForced);
    virtual VOID        Flush(BOOL isInProgress);

    virtual VOID        ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData);

    virtual CDKResult   GetRequestInfo(
        camera3_capture_request_t*  pRequest,
        FeatureRequestInfo*                pOutputRequests);

    virtual FeatureType GetFeatureType()
    {
        return FeatureType::HDR;
    }

    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        BOOL isInternal = ((pStream == m_pRdiStream) || (pStream == m_pBayer2YuvStream));
        return FALSE;
    }

    virtual INT32 GetRequiredPipelines(
        AdvancedPipelineType* pPipelines,
        INT32 size);

protected:
    FeatureSHDR() = default;
    virtual ~FeatureSHDR() = default;

    AdvancedCameraUsecase* m_pUsecase;                                       ///< Pointer to owning usecase class
    camera3_stream_configuration_t*  m_InternalStreamConfig;                 ///< Store Current Stream Configuration

    ChiUsecase*         m_pChiUsecase;                                       ///< Copy of ZSL usecase that gets adjusted
    ChiStream*          m_pRdiStream;                                        ///< Allocated internal RDI stream
    ChiStream*          m_pPreviewStream;                                    ///< Tracking of the stream used for preview
    ChiStream*          m_pYUVStream;                                        ///< Tracking of the stream used for preview
    ChiStream*          m_pSnapshotStream;                                   ///< Tracking of the stream used for snapshot
    ChiStream*          m_pVideoStream;                                      ///< Tracking of the stream used for video
    ChiStream*          m_pRealtimeYUVStream;                                ///< Tracking of the stream used for video
    UINT                m_rdiStreamIndex;                                    ///< Stream/target index for the RDI stream
    TargetBuffer*       m_pRdiTargetBuffer;                                  ///< TargetBuffer in the CameraUsecase
    UINT64              m_shutterTimestamp[MaxOutstandingRequests];          ///< Tracking of shutter timestamp for each request
    UINT32              m_maxSnapshotReqId;                                  ///< Last valid ID to move snapshotReqId to
                                                                             ///  Owned by the main thread
    UINT32              m_snapshotReqIdToFrameNum[MaxOutstandingRequests];   ///< Mapping of snapshotReqId to framework frame

    VOID*               m_pSnapshotInputMeta[MaxOutstandingRequests];        ///< The metadata for the request
    VOID*               m_pPreviewAppMeta[MaxOutstandingRequests];           ///< The metadata for the request
    camera3_stream_buffer_t  m_snapshotBuffers[MaxOutstandingRequests][2];   ///< Result buffers from app for snapshot
    camera3_stream_buffer_t  m_previewBuffers[MaxOutstandingRequests][2];   ///< Result buffers from app for snapshot

    UINT                     m_snapshotBufferNum[MaxOutstandingRequests];    ///< Buffer count

    BOOL                m_isSnapshotFrame[MaxOutstandingRequests];           ///< Is a snapshot request

    CHIPRIVDATA         m_privData[MaxOutstandingRequests];                  ///< Private data for each request

    UINT32              m_snapshotPipelineIndex;                             ///< Snapshot Pipeline Index
    UINT32              sortedFWKStreams[MAX_NUM_STREAMS][2];                ///< Sort FWK Streams
    UINT32              m_mergePipelineIndex;                                ///< HDR Merge Pipeline Index
    UINT32              m_bayer2YuvPipelineIndex;                            ///< Bayer2Yuv Pipeline Index
    UINT32              m_previewPipelineIndex;                              ///< Preivew Pipeline Index
    UINT32              m_snapshotSessionId;                                 ///< Snapshot Session Id
    UINT32              m_mergeSessionId;                                    ///< HDR Merge Session Id
    UINT32              m_bayer2YuvSessionId;                                ///< Bayer2Yuv Session Id
    UINT32              m_previewSessionId;                                  ///< Preview Session Id

    static const UINT   MaxInputPorts            = 10;                       ///< Max Input Port Numbers for HDR Node
    static const UINT   MaxMultiFrames           = MaxInputPorts;            ///< Max Input Frame Numbers for HDR Node
    static const UINT   InvalidSessionId         = 0xFFFFFFFF;               ///< Invalid session id
    static const UINT   InvalidPipelineIndex     = 0xFFFFFFFF;               ///< Invalid Pipeline index
    static const UINT   MinOutputBuffers         = 3;                        ///< Minimum number of Chi output buffers

    // Multiframe stage-1
    ChiStream*          m_pBayer2YuvStream;                                  ///< Tracking of internal stream used for stage-1
    UINT                m_Bayer2YuvStreamIndex;                              ///< Stream/target index for the stage-1 stream
    TargetBuffer*       m_pBayer2YuvTargetBuffer;                            ///< TargetBuffer in the CameraUsecaseBase
    VOID*               m_pBayer2YuvInputMeta[MaxOutstandingRequests];       ///< The metadata for the request
    // Multiframe stage-2
    ChiStream*          m_pMergeYuvStream;                                   ///< Tracking of internal stream used for stage-2
    UINT                m_mergeStreamIndex;                                  ///< Stream/target index for the stage-2 stream
    TargetBuffer*       m_pYUVTargetBuffer;                                  ///< TargetBuffer in the CameraUsecaseBase
    VOID*               m_pMergeInputMeta[MaxOutstandingRequests];           ///< The metadata for the request
    ChiTarget           m_mergePipelineInputTargets[MaxInputPorts];          ///< InputTargets for HDR merge pipeline
    ChiTarget           m_mergePipelineOutputTarget;                         ///< OutputTarget for HDR merge pipeline
    CHIBufferManager*   m_pSHDRYUVOutputBufferManager;                       ///< BufferManager for HDR merge pipeline output
    TargetBuffer        m_YUVTargetBuffers;                                  ///< YUV TargetBuffers for Snapshot
    BufferManager*      m_pPreviewBufferManager[2];                          ///< buffer manager for realtime pipeline preview output
    UINT32              m_snapshotReqId;                                     ///< Next ID to provide snapshot request


    ChiStream*          m_pJPEGInputStream;                                  ///< Common JPEG input stream

    INT32               m_expValues[5];                                      ///< Different exposure values for HDR
    UINT32              m_numAeBracketFrames;                                ///< Number of AE Bracket frames for HDR
    CHITAGSOPS          m_vendorTagOps;                                      ///< Vendor Tag Ops
    UINT32              m_previewRawReqId;                                   ///< Internal request id for realtime pipeline
    UINT32              m_lastShutterFrameNum;                               ///< Latest received shutter frame number
    UINT32              m_numBayer2YuvFrames;                                ///< Currently received bayer2yuv frames
    ChiStream*          m_pMergePipelineInputStreams[MaxInputPorts];         ///< Input streams for HDR merge pipeline
    camera_metadata_t*  m_pOverrideAppSetting[MaxMultiFrames];               ///< Overrde EV settings
    UINT32              m_firstNormalExpIdx;                                 ///< First normal exposure in AE bracket array
    UINT32              m_snapshotAppFrameNum;                               ///< App frame number for snapshot

    /// for aux camera
    ChiStream*          m_pRdiStreamAux;                                     ///< RDI stream for Aux Camera
    UINT32              m_bayer2YuvAuxSessionId;                             ///< Bayer2Yuv Session Id for Aux Camera
    UINT32              m_bayer2YuvAuxPipelineIndex;                         ///< Bayer2Yuv Pipeline Index for Aux Camera
    UINT32              m_masterCameraId;                                    ///< Master Camera Id
    UINT32              m_activePipelineID;                                  ///< Active Pipeline ID
    BOOL                m_isSkipPreview[MaxOutstandingRequests];             ///< Skip preview processing
    camera3_stream_buffer_t m_appPreviewBuffer[MaxOutstandingRequests]; ///< application requested buffer handle for preview
        UINT32         m_effectModeValue;                            ///< effect value
    UINT32         m_sceneModeValue;                             ///< scenemode value
    UINT32         m_tuningFeature1Value;                        ///< tuning Feature1Value value
    UINT32         m_tuningFeature2Value;                        ///< tuning Feature2Value value
    BOOL           m_isSnapshotRequested;                        ///< snapshot requested

private:
    // Do not allow the copy constructor or assignment operator
    FeatureSHDR(const FeatureSHDR&) = delete;
    FeatureSHDR& operator= (const FeatureSHDR&) = delete;

    // Dump Debug/Tuning data
    DebugData           m_debugDataOffline;                             ///< Offline copy for debug-data

    // Helper to submit request
    VOID SubmitRequest(UINT32 sessionIdx, UINT frameNumber,
                       TargetBuffer* pTargetBuffer,
                       camera3_stream_buffer_t *pOutputBuffer,
                       UINT32 inputPipelineReqId);

    VOID StreamMap(camera3_stream_configuration_t* pStreamConfig);
    VOID sortFWKStream(camera3_stream_configuration_t* pStreamConfig);

    VOID OverrideUsecaseYUVSHDR(
        LogicalCameraInfo*              pCameraInfo,
        camera3_stream_configuration_t* pStreamConfig,
        UINT shdr_type);


};

#endif // CHXFEATURESHDR_H
