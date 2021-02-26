////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxfeature.h
/// @brief CHX feature base class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXFEATURE_H
#define CHXFEATURE_H

#include <assert.h>

#include "chxincs.h"
#include "g_pipelines.h"
#include "chxusecaseutils.h"

enum AdvancedPipelineType
{
    ZSLSnapshotJpegType = 0,        // snapshot to JPEG
    ZSLSnapshotYUVType,             // snapshot to YUV
    InternalZSLYuv2JpegType,        // YUV to JPEG
    InternalZSLYuv2JpegMFNRType,    // YUV to JPEG for MFNR
    Merge3YuvCustomTo1YuvType,      // merge 3 YUV to 1 YUV
    ZSLPreviewRawType,              // preview
    ZSLPreviewRawYUVType,           // preview+Video
    MFNRPrefilterType,              // MFNR prefilter
    MFNRBlendType,                  // MFNR blend
    MFNRPostFilterType,             // MFNR post filter
    SWMFMergeYuvType,               // SW multi frame
    ZSLSnapshotYUVAuxType,          // Bayer to Yuv for aux
    InternalZSLYuv2JpegMFNRAuxType, // YUV to JPEG for MFNR Aux
    MFNRPrefilterAuxType,           // MFNR prefilter Aux
    MFNRBlendAuxType,               // MFNR blend Aux
    MFNRPostFilterAuxType,          // MFNR post filter Aux
    ZSLYuv2YuvType,                 // Yuv to Yuv reprocess
    ZSLSnapshotJpegGPUType,         // snapshot to GPU to JPEG
    SHDRPreviewRawYUVType,          // Staggered Raw to Yuv
    SHDRSnapShotType,               // Staggered Snapshot
    SHDRPreviewYUVType,             // Staggered Yuv
    SHDRPreview3FrameYUVType,       // 3 frame Yuv Staggered
    SHDRPreview3FrameYUVTypeWithHWStats, // 3 frame Yuv Staggered
    PipelineCount                   // the pipelines count
};

enum FeatureType
{
    UNINIT = 0,
    ZSL    = 1,
    SWMF   = 2,
    MFNR   = 3,
    HDR    = 4,
    SHDR   = 5,
};

struct FeatureRequestInfo
{
    camera3_capture_request_t request[MaxOutstandingRequests];
    UINT32                    numOfRequest;
    BOOL                      isReturnResult[MaxOutstandingRequests];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Base Feature class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Feature
{
public:
    virtual ChiUsecase* OverrideUsecase(
        LogicalCameraInfo*              pCameraInfo,
        camera3_stream_configuration_t* pStreamConfig)    = 0;

    virtual VOID PipelineCreated(
        UINT32 sessionId,
        UINT32 pipelineId)
    {
        (void)sessionId;
        (void)pipelineId;
    }

    virtual CDKResult   ExecuteProcessRequest(
        camera3_capture_request_t*  pRequest)             = 0;

    virtual VOID        ProcessResult(
        CHICAPTURERESULT*           pResult,
        VOID*                       pPrivateCallbackData) = 0;

    virtual VOID        ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor,
        VOID*                       pPrivateCallbackData) = 0;

    virtual VOID        Destroy(BOOL isForced)      = 0;

    virtual VOID        Pause(BOOL isForced)        = 0;

    virtual VOID        Flush(BOOL isInProgress)    = 0;

    virtual CDKResult   GetRequestInfo(
        camera3_capture_request_t*  pRequest,
        FeatureRequestInfo*                pOutputRequests)
    {
        (VOID)pRequest;
        (VOID)pOutputRequests;

        return CDKResultSuccess;
    }

    virtual FeatureType GetFeatureType()
    {
        return FeatureType::UNINIT;
    }

    virtual BOOL StreamIsInternal(
        ChiStream* pStream)
    {
        (VOID)pStream;
        return FALSE;
    }

    virtual UINT32 GetRequiredFramesForSnapshot(
        const camera_metadata_t *pMetadata)
    {
        (VOID)pMetadata;
        return 1;
    }

    /*
     * fill the required pipeline ids to the given pPipelines
     *
     * return the number of pipelines have been filled
     */
    virtual INT32 GetRequiredPipelines(
        AdvancedPipelineType* pPipelines,
        INT32 size)
    {
        (VOID)pPipelines;
        (VOID)size;
        return 0;
    }
protected:
    virtual ~Feature() = default;
};

#endif // CHXFEATURE_H
