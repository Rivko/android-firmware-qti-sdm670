////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3Test.h
/// @brief base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _QCAMERA_HAL3_TEST_
#define _QCAMERA_HAL3_TEST_
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <log/log.h>
#include <string.h>
#include <mutex>
#include "QCameraHAL3Device.h"
#include "QCameraHAL3TestConfig.h"
#include "TestLog.h"
#define JPEG_QUALITY_DEFAULT 85
#define PREVIEW_STREAM_BUFFER_MAX 12
#define SNAPSHOT_STREAM_BUFFER_MAX 8
#define VIDEO_STREAM_BUFFER_MAX 18

typedef enum {
    PREVIEW_TYPE,
    SNAPSHOT_TYPE,
    VIDEO_TYPE,
}StreamType;
typedef struct _StreamCapture{
    StreamType type;
    int count;
}StreamCapture;
class QCameraHAL3Test {
public:
    QCameraHAL3Device* mDevice;
    TestConfig* mConfig;
protected:
    camera_module_t* mModule;
};
#endif
