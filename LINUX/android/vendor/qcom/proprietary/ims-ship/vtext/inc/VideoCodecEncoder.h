/*****************************************************************************

 ============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoCodecEncoder.h
 Description: Implementation of VideoCodecEncoder

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/
#include<MediaCodecBase.h>
#include <gui/Surface.h>
#include <utils/StrongPointer.h>

#include<VTRecorder.h>
using namespace android;
#include <pthread.h>
class VideoCodecEncoder : public MediaCodecBase, public VTRecorder
{
  public:
    VideoCodecEncoder();

    virtual ~VideoCodecEncoder();
    /** Returns true if the thread started */
    bool StartRecordThread() {
      CRITICAL1("Start RecordLoop Thread ");
      return (pthread_create(&recorderthread, NULL, RecorderThreadFunction,
                             this) == 0);
    }

    /** Will not return until the internal thread has exited. */
    void WaitforRecordThread() {
      CRITICAL1("Waiting for RecordLoop Thread to exit");
      (void) pthread_join(recorderthread, NULL);
      CRITICAL1("RecordLoop Thread exited");
    }
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void *pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG *pCodecConfig);
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig);
    virtual QPE_VIDEO_ERROR Start();
    virtual QPE_VIDEO_ERROR Stop();
    //Recorder specific functions
    virtual QPE_VIDEO_ERROR CreateInputRecordingSurface(sp<IGraphicBufferProducer>
        *bufferproducer, uint32_t height, uint32_t width);
    virtual QPE_VIDEO_ERROR AdaptVideoBitRate(uint32_t iBitRate);
    virtual QPE_VIDEO_ERROR DeInit();
    QPE_VIDEO_ERROR ProcessRecordingFrame(uint64_t timestamp, void *dataPtr,
                                          size_t size);
    virtual QPE_VIDEO_ERROR GenerateH264IDRFrame();
    QPE_VIDEO_ERROR ConfigureProfileAndLevel();
    QPE_VIDEO_ERROR SetRotationAngle(uint16_t rotation);
    virtual QPE_VIDEO_ERROR AdaptVideoFrameRate(uint32_t iFrameRate);
    virtual QPE_VIDEO_ERROR AdaptVideoIdrPeriodicity(int32_t iFrameInt);
    void videoRecordLoop();

    /* This API will be called to mark LTR frame.*/
    void ConfigLTRMark(int32_t slLtrIdx);
    /* This API will be called to use LTR frame.*/
    void ConfigLTRUse(int32_t slLtrIdx);
  private:
    static void * RecorderThreadFunction(void * This) {
      ((VideoCodecEncoder *)This)->videoRecordLoop();
      return NULL;
    }
    QPE_VIDEO_ERROR GetEncodedFrame();

    pthread_t recorderthread;
    enum eVideoState iRecorderState;
    QP_VIDEO_CONFIG m_codecConfig;
    uint32 m_LastIDRtimestamp;
    bool m_IDRRetransmitOn;
    uint8_t m_IDRframeinterval;
    uint8_t videoRecordThreadCreated;

    //DATA_CODEC_CALLBACK datacallback;

};
