////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chxusecase.h
/// @brief CHX usecase base class declarations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHXUSECASE_H
#define CHXUSECASE_H

#include <assert.h>

#include "chxincs.h"
#include "chxpipeline.h"
#include "chxsession.h"
#include "chxusecaseutils.h"
#include "chivendortag.h"
#include "chxperf.h"

// generated headers
#include "g_pipelines.h"

/// Forward declarations
struct ChiPipelineTargetCreateDescriptor;
class  Session;

/// Static constants
static const UINT32 MaxExternalBuffers = 4;
static const UINT32 INVALIDSEQUENCEID  = 0xFFFFFFFF;

///@ todo Optimize this
static const UINT32 ReplacedMetadataEntryCapacity = 1024;                  ///< Replaced metadata entry capacity
static const UINT32 ReplacedMetadataDataCapacity  = 256 * 1024;            ///< Replaced metadata data capacity

/// @brief Data about a created pipeline and its dynamic state
struct PipelineData
{
    UINT        id;
    Pipeline*   pPipeline;
    CHISTREAM*  pStreams[MaxChiStreams];
    UINT        numStreams;
    UINT        seqId;
    UINT        seqIdToFrameNum[MaxOutstandingRequests];
};

/// @brief Data about a created session
struct SessionData
{
    Session*     pSession;                          ///< Pointer to session
    UINT32       numPipelines;                      ///< Number of created pipelines
    UINT32       rtPipelineIndex;                   ///< Pipeline index that is realtime
    PipelineData pipelines[MaxPipelinesPerSession]; ///< Per pipeline information
};

/// @brief Session private data
struct SessionPrivateData
{
    Usecase*  pUsecase;                    ///< Per usecase class
    UINT32    sessionId;                   ///< Session Id that is meaningful to the usecase in which the session belongs
};

/// @brief Session private data
struct RequestMetadataInfo
{
    BOOL      isMetadataSent;              ///< Per usecase class
    UINT32    FrameworkFrameNumber;        ///< Session Id that is meaningful to the usecase in which the session belongs
};

/// @brief Buffers information
struct TargetBufferInfo
{
    UINT32             frameNumber;                               ///< Frame number
    camera_metadata_t* pMetadata;                                 ///< Metadata
    VOID*              pDebugData;                                ///< Debug data
    CHISTREAMBUFFER*   pRdiOutputBuffer;                          ///< RDI buffer
    BOOL               isBufferReady;                             ///< Buffer ready flag
    BOOL               isMetadataReady;                           ///< Metadata ready flag
    BOOL               isBufferUsed;                              ///< Used bit, set if buffer is used by snapshot request
};

/// @brief Collection of information for tracking internal buffers
struct TargetBuffer
{
    TargetBufferInfo    bufferQueue[BufferQueueDepth];  ///< Buffer queues for internal targets
    CHIBufferManager*   pBufferManager;                 ///< Buffer manager to maintain buffers
    Mutex*              pMutex;                         ///< Mutex protecting access to the manager and buffers
    Condition*          pCondition;                     ///< Condition
    UINT32              queueDepth;                     ///< Queue depth
    UINT32              lastReadySequenceID;            ///< Last sequence id which buffer and metadata are ready
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Base Usecase class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Usecase
{
public:

    // Perform any base class functionality for processing the request and call the usecase specific derived class
    CDKResult ProcessCaptureRequest(
        camera3_capture_request_t* request);
    VOID FlushLock(BOOL bLock);

    // Perform any base class functionality for processing the result and call the usecase specific derived class
    CDKResult ProcessCaptureResult(
        const CHICAPTURERESULT* pResult);

    // Perform any base class functionality for processing the message notification
    CDKResult ProcessMessage(
        const CHIMESSAGEDESCRIPTOR* pMessageDescriptor);

    // Destroy the usecase object
    VOID DestroyObject(
        BOOL isForced);

    // Update the sensor mode index, per frame
    CDKResult UpdateSensorModeIndex(
        camera_metadata_t* pMetaData);

    // Update the feature mode index, per frame
    CDKResult UpdateFeatureModeIndex(
        camera_metadata_t* pMetaData);

    // Singular interface to return buffers/metadata back to the application
    VOID ReturnFrameworkResult(
        const camera3_capture_result_t* pResult,
        UINT32 cameraID);

    // Singular interface to return message back to the application
    VOID ReturnFrameworkMessage(
        const camera3_notify_msg_t* pMessage,
        UINT32 cameraID);

    // To flush the session
    virtual CDKResult Flush();


    // Accessor to results
    CHX_INLINE camera3_capture_result_t* GetCaptureResult(
        UINT resultId
    )
    {
        return &m_captureResult[resultId];
    }

    // Camera Id
    UINT GetCameraId() { return m_cameraId; }

    // Usecase Id
    UsecaseId GetUsecaseId() { return m_usecaseId; }

    // Flush Status
    BOOL GetFlushStatus() { return m_isFlushEnabled; }

    // Get m_pAppResultMutex pointer
    CHX_INLINE Mutex* GetAppResultMutex() { return m_pAppResultMutex; }

protected:

    // Perform any base class intialization and call the derived class to do usecase specific initialization
    CDKResult Initialize();

    // Implemented by the derived class to execute the capture request
    virtual CDKResult ExecuteCaptureRequest(
        camera3_capture_request_t* request) = 0;

    // Implemented by the derived class to process the saved results
    virtual VOID ProcessResults() = 0;

    VOID HandleProcessRequestErrorAllPCRs(
        camera3_capture_request_t* pRequest);

    virtual VOID Destroy(BOOL isForced) = 0;

    Usecase() = default;
    virtual ~Usecase();

    UsecaseId                m_usecaseId;                                           ///< UsecaseId
    camera3_capture_result_t m_captureResult[MaxOutstandingRequests];               ///< Capture results to be sent to the
                                                                                    ///< app/framework
    camera3_notify_msg_t     m_notifyMessage[MaxOutstandingRequests];               ///< App shutter callback msg
    BOOL                     m_MessageAvailable[MaxOutstandingRequests];            ///<App shutter callback msg ready?
    BOOL                     m_PendingMessage[MaxOutstandingRequests];
    UINT32                   m_numAppPendingOutputBuffers[MaxOutstandingRequests];  ///< Number of pending app output buffers
                                                                                    ///  not sent out yet per request
    UINT64                   m_nextAppResultFrame;                                  ///< Next frame result to be sent back
    UINT64                   m_nextAppMessageFrame;                                 ///< Next frame message to be sent back
    UINT64                   m_lastAppMessageFrameReceived;                         ///< Last frame message to be sent back
    UINT64                   m_lastAppRequestFrame;                                 ///< Last app request frame number
    PerThreadData            m_resultProcessingThread;                              ///< Thread to process the results
    Mutex*                   m_pAppResultMutex;                                     ///< App Result mutex
    Mutex*                   m_pAllResultsMutex;                                    ///< All Results mutex
    Condition*               m_pAppResultAvailable;                                 ///< Wait till app results are available
    Condition*               m_pAllResultsAvailable;                                ///< Wait till all results are available
    volatile BOOL            m_appResultThreadTerminate;                            ///< Indication to app result thread to
                                                                                    ///  termiate itself
    volatile BOOL            m_allResultsAvailable;                                 ///< Are all results available at any point
                                                                                    ///  in time
    UINT32                   m_cameraId;                                            ///< Camera id to which the usecase belongs
    /// @todo Need to add result sequencing mechanism
    INT64                    m_lastResultMetadataFrameNum;                          ///< Frame number whose metadata was sent
                                                                                    ///  last
    ChiUsecase*              m_pChiUsecase;                                         ///< Matched usecase
    UINT32                   m_selectedSensorModeIndex;                             ///< selected sensor mode index
    CHIPRIVDATA              m_privData[MaxOutstandingRequests];                    ///< private data for request

    // Vendor tag values
    // Add vendor tag values that need to be stored here

private:

    /// Main entry function for the resul thread
    static VOID* ResultThreadMain(VOID* pArg);

    /// Do not allow the copy constructor or assignment operator
    Usecase(const Usecase& rUsecase) = delete;
    Usecase& operator= (const Usecase& rUsecase) = delete;

    UINT32 GetAppFrameNum(UINT32 chiOverrideFrameNum) const
    {
        return m_originalIncomingFrameNum[chiOverrideFrameNum % MaxPendingFrameNumber];
    }

    UINT32 GetChiOverrideFrameNum(UINT32 appFrameNum)
    {
        UINT32 chiFrameNumIdx = (m_chiOverrideFrameNum % MaxPendingFrameNumber);

        m_originalIncomingFrameNum[chiFrameNumIdx] = appFrameNum;

        return m_chiOverrideFrameNum++;
    }

    /// Replace the capture request metadata with a copy of it with additional new settings inserted by the Chi override
    CDKResult ReplaceRequestMetadata(
        const VOID* pMetadata);                            ///< Request parameters

    /// Restore the original metadata that was passed in
    VOID RestoreRequestMetadata(
        camera3_capture_request_t* pRequest);              ///< Request parameters

    VOID ReturnFrameworkMessageFlushError(
        const camera3_notify_msg_t* pMessage);

    VOID ReturnFrameworkResultFlush(
        const camera3_capture_result_t* pResult);

    UINT32                      m_originalIncomingFrameNum[MaxPendingFrameNumber];      ///< Original incoming app frame num
    UINT32                      m_chiOverrideFrameNum;                                  ///< Sequential frame number
    const VOID*                 m_pOriginalMetadata;                                    ///< Original pointer to the metadata
    camera_metadata_t*          m_pReplacedMetadata;                                    ///< Replaced metadata
    size_t                      m_replacedMetadataSize;                                 ///< Replaced metadata size
    BOOL                        m_isFlushEnabled;                                       ///< Flag to check if flush is enabled
    Mutex*                      m_pMapLock;
    camera3_capture_request_t   m_pendingPCRs[MaxOutstandingRequests];
    UINT32                      m_numberOfPendingOutputBuffers[MaxOutstandingRequests];  ///< pending app output buffers
    RequestMetadataInfo         m_numberOfPendingOutputMetadata[MaxOutstandingRequests]; ///< pending app output metadata
};

#endif // CHXUSECASE_H
