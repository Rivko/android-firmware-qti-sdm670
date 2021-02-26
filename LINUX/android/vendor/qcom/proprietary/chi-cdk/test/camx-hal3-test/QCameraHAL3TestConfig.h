////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestConfig.h
/// @brief for TestConfig handle
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_TEST_CONFIG
#define _CAMERA_TEST_CONFIG

#include <hardware/camera_common.h>
#include <hardware/camera3.h>
#include <string>
#include <camera/CameraMetadata.h>
#include <camera/VendorTagDescriptor.h>
#include "TestLog.h"

/******************************************************************************************************************************
 * There are 3 Test Modes: preview, snapshot, video
 * preview : There is only preview stream, in NonZSL case, preview
 *           mode is needed when there is no snapshot request
 * snapshot : There are preview stream and snapshot stream, it is
 *            needed when snapshot requests in NonZSL or ZSL mode
 * Video: There are preview, video and snapshot streams, it is needed
 *        when video/liveshot
 ******************************************************************************************************************************/

#define TESTMODE_PREVIEW 0
#define TESTMODE_SNAPSHOT 1
#define TESTMODE_VIDEO 2

// stream info struct for a stream
typedef struct _stream_info {
    camera3_request_template_t type;  // preview , snapshot or video
    int width;
    int height;
    int format;
    int requestNumber;
}stream_info_t;

// Settings for item selection which is needed to dump/show as user's order.
typedef struct _meta_dump {
    int exposureValue;
    int isoValue;
    int aeMode;
    int awbMode;
    int afMode;
    int afValue;
    int aeAntiMode;
    int colorCorrectMode;
    int colorCorrectValue;
    int controlMode;
    int sceneMode;
    int hdrMode;
    int zoomValue;
    int zslMode;
    int numFrames;
    int expMetering;
    int selPriority;
    int expPriority;
    int jpegquality;
}meta_dump_t;

// Saving the Metadata Value status
typedef struct _meta_stat{
    int expTime;
    int isoValue;
    int aeMode;
    int awbMode;
    int afMode;
    float afValue;
    int aeAntiMode;
    int colorCorrectMode;
    float colorCorrectValue;
    int controlMode;
    uint32_t asdresults[10];
    int hdrMode;
    int32_t cropregion[4];
    int zslMode;
    int numFrames;
    int expMetering;
    int selPriority;
    int64_t expPriority;
    int jpegquality;
}meta_stat_t;

typedef struct _video_bitrate_config{
    uint32_t bitrate;
    uint32_t targetBitrate;
    bool isBitRateConstant;
}video_bitrate_config_t;

// Calss for Geting and saving Configure from user
class TestConfig {
public:
    int           mTestMode;
    int           mIsH265;
    int           mCameraId;
    stream_info_t mPreviewStream;
    stream_info_t mSnapshotStream;
    stream_info_t mVideoStream;
    video_bitrate_config_t mVideoRateConfig;
    //zsl
    bool          mZslEnabled;
    //dump
    /*
     * Video  Snapshot  Preview
     * bit[2] bit[1]    bit[0]
     */
    meta_dump_t   mMetaDump;
    TestLog*      mDump;
    int           mShowFps;
    int           mFpsRange[2];

    meta_stat_t   mMetaStat;
    TestConfig();
    ~TestConfig();
    void setDefaultConfig();
    void setCameraConfig(int camera,int argc,char* argv[]);
    int parseCommandline(int argc, char* argv[]);
    int parseCommandlineAdd(int ordersize,char* order);
    int parseCommandlineChange(int ordersize,char* order);
    int parseCommandlineMetaDump(int ordersize,char* order);
    int parseCommandlineMetaUpdate(char* order,android::CameraMetadata* metaUpdate);
};

#endif

