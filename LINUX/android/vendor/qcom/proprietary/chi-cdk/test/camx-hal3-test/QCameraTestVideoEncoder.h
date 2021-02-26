////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraTestVideoEncoder.h
/// @brief mid objetct for video encoder test
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_TEST_VIDEO_ENCODER
#define _QCAMERA_TEST_VIDEO_ENCODER
#include <pthread.h>
#include <queue>
#include <list>
#include <map>
#include <gralloc_priv.h>
#include <signal.h>
#include <stdio.h>
#include <OMX_Video.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_QCOMExtns.h>
#include <OMX_VideoExt.h>

#include <QComOMXMetadata.h>

#include "OMX_Encoder.h"
#include "QCameraHAL3Device.h"
#include "QCameraHAL3TestConfig.h"

using namespace std;
using namespace android;
class QCameraTestVideoEncoder: public QCameraBufferHolder{

public:
    QCameraTestVideoEncoder(TestConfig *);
    void run();
    void stop();
    int Read(OMX_BUFFERHEADERTYPE *buf);
    OMX_ERRORTYPE Write(OMX_BUFFERHEADERTYPE *buf);
    OMX_ERRORTYPE EmptyDone(OMX_BUFFERHEADERTYPE *buf);
    void EnqueueFrameBuffer(CameraStream *stream, buffer_handle_t *buf_handle);
    ~QCameraTestVideoEncoder();
private:
    FILE*                           mOutFd;
    uint64_t                        mTimeOffset;
    uint32_t                        mTimeOffsetInc;
    map<int, buffer_handle_t *>     mFrameHandleMap;
    list<buffer_handle_t *> *       mBufferQueue;
    CameraStream *                  mStream;
    pthread_mutex_t                 mLock;
    pthread_mutex_t                 mBufferLock;
    QCameraOMX_Encoder *            mCoder;
    pthread_cond_t                  mCond;
    omx_config_t                    mConfig;
    bool                            mIsStop;
};
#endif
