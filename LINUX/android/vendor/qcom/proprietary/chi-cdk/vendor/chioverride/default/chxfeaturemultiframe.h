////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeaturemultiframe.h
/// @brief CHX feature multiframe class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATUREMULTIFRAME_H
#define CHXFEATUREMULTIFRAME_H

#include <assert.h>

#include "chxincs.h"
#include "chxfeature.h"
#include "chxusecase.h"
#include "chxusecaseutils.h"

////////////////////////
/// Forward Declarations
////////////////////////
class AdvancedCameraUsecase;

class FeatureMultiframe : public Feature
{
public:
    static  FeatureMultiframe* Create(
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

    virtual FeatureType GetFeatureType()
    {
        return FeatureType::SWMF;
    }

    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        BOOL isInternal = ((pStream == m_pRdiStream) || (pStream == m_pBayer2YuvStream));
        return isInternal;
    }

    virtual INT32 GetRequiredPipelines(
        AdvancedPipelineType* pPipelines,
        INT32 size);

    virtual UINT32 GetRequiredFramesForSnapshot(
        const camera_metadata_t *pMetadata)
    {
        (VOID)pMetadata;
        return m_numMultiFramesRequired;
    }

protected:
    FeatureMultiframe() = default;
    virtual ~FeatureMultiframe() = default;

    AdvancedCameraUsecase* m_pUsecase;                                       ///< Pointer to owning usecase class

    ChiUsecase*         m_pChiUsecase;                                       ///< Copy of ZSL usecase that gets adjusted
    ChiStream*          m_pRdiStream;                                        ///< Allocated internal RDI stream
    ChiStream*          m_pPreviewStream;                                    ///< Tracking of the stream used for preview
    ChiStream*          m_pSnapshotStream;                                   ///< Tracking of the stream used for snapshot
    UINT                m_rdiStreamIndex;                                    ///< Stream/target index for the RDI stream
    TargetBuffer*       m_pRdiTargetBuffer;                                  ///< TargetBuffer in the CameraUsecase
    UINT64              m_shutterTimestamp[MaxOutstandingRequests];          ///< Tracking of shutter timestamp for each request
    UINT32              m_maxSnapshotReqId;                                  ///< Last valid ID to move snapshotReqId to
                                                                             ///  Owned by the main thread
    UINT32              m_snapshotReqIdToFrameNum[MaxOutstandingRequests];   ///< Mapping of snapshotReqId to framework frame

    VOID*               m_pSnapshotInputMeta[MaxOutstandingRequests];        ///< The metadata for the request
    camera3_stream_buffer_t  m_snapshotBuffers[MaxOutstandingRequests][2];   ///< Result buffers from app for snapshot
    UINT                     m_snapshotBufferNum[MaxOutstandingRequests];    ///< Buffer count

    BOOL                m_isSnapshotFrame[MaxOutstandingRequests];           ///< Is a snapshot request

    CHIPRIVDATA         m_privData[MaxOutstandingRequests];                  ///< Private data for each request

    UINT32              m_snapshotPipelineIndex;                             ///< Snapshot Pipeline Index
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
    static const UINT   DefaultSWMFNumFrames     = 3;                        ///< Default number of frames required for SWMF
    static const UINT   MinOutputBuffers         = 8;                        ///< Minimum number of Chi output buffers

    // Multiframe stage-1
    ChiStream*          m_pBayer2YuvStream;                                  ///< Tracking of internal stream used for stage-1
    UINT                m_Bayer2YuvStreamIndex;                              ///< Stream/target index for the stage-1 stream
    TargetBuffer*       m_pBayer2YuvTargetBuffer;                            ///< TargetBuffer in the CameraUsecaseBase
    VOID*               m_pBayer2YuvInputMeta[MaxOutstandingRequests];       ///< The metadata for the request
    // Multiframe stage-2
    ChiStream*          m_pMergeYuvStream;                                   ///< Tracking of internal stream used for stage-2
    UINT                m_mergeStreamIndex;                                  ///< Stream/target index for the stage-2 stream
    TargetBuffer*       m_pMergeTargetBuffer;                                ///< TargetBuffer in the CameraUsecaseBase
    VOID*               m_pMergeInputMeta[MaxOutstandingRequests];           ///< The metadata for the request
    ChiTarget           m_mergePipelineInputTargets[MaxInputPorts];          ///< InputTargets for HDR merge pipeline
    ChiTarget           m_mergePipelineOutputTarget;                         ///< OutputTarget for HDR merge pipeline
    CHIBufferManager*   m_pMergeOutputBufferManager;                         ///< BufferManager for HDR merge pipeline output
    TargetBuffer        m_mergeTargetBuffers;                                ///< TargetBuffers for HDR merge pipeline output

    ChiStream*          m_pJPEGInputStream;                                  ///< Common JPEG input stream

    UINT32              m_numMultiFramesRequired;                            ///< Number of yuv frames required
    CHITAGSOPS          m_vendorTagOps;                                      ///< Vendor Tag Ops
    UINT32              m_previewRawReqId;                                   ///< Internal request id for realtime pipeline
    UINT32              m_lastShutterFrameNum;                               ///< Latest received shutter frame number
    UINT32              m_numBayer2YuvFrames;                                ///< Currently received bayer2yuv frames
    ChiStream*          m_pMergePipelineInputStreams[MaxInputPorts];         ///< Input streams for HDR merge pipeline
    UINT32              m_snapshotAppFrameNum;                               ///< App frame number for snapshot

    /// for aux camera
    ChiStream*          m_pRdiStreamAux;                                     ///< RDI stream for Aux Camera
    UINT32              m_bayer2YuvAuxSessionId;                             ///< Bayer2Yuv Session Id for Aux Camera
    UINT32              m_bayer2YuvAuxPipelineIndex;                         ///< Bayer2Yuv Pipeline Index for Aux Camera
    UINT32              m_masterCameraId;                                    ///< Master Camera Id
    UINT32              m_activePipelineID;                                  ///< Active Pipeline ID

private:
    // Do not allow the copy constructor or assignment operator
    FeatureMultiframe(const FeatureMultiframe&) = delete;
    FeatureMultiframe& operator= (const FeatureMultiframe&) = delete;

    // Dump Debug/Tuning data
    DebugData           m_debugDataOffline;                             ///< Offline copy for debug-data

    // Helper to submit request
    VOID SubmitRequest(UINT32 sessionIdx, UINT frameNumber,
                       TargetBuffer* pTargetBuffer,
                       camera3_stream_buffer_t *pOutputBuffer,
                       UINT32 inputPipelineReqId);
};

#endif // CHXFEATUREMULTIFRAME_H
