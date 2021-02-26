////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  QCameraHAL3TestConfig.cpp
/// @brief for TestConfig handles
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QCameraHAL3TestConfig.h"
#include <unistd.h>
#include <string.h>
#include <vector>
#define LOG_TAG "CameraTest"
#include <log/log.h>

using namespace android;
/************************************************************************
* name : TestConfig
* function: construct object.
************************************************************************/
TestConfig::TestConfig()
{
    mDump = new TestLog("/data/misc/camera/test1.log");
    memset(&mMetaDump,       0, sizeof(meta_dump_t));
    memset(&mMetaStat,       0, sizeof(meta_stat_t));
    memset(&mPreviewStream,  0, sizeof(stream_info_t));
    memset(&mSnapshotStream, 0, sizeof(stream_info_t));
    memset(&mVideoStream,    0, sizeof(stream_info_t));
    memset(&mVideoRateConfig, 0, sizeof(video_bitrate_config_t));
    mVideoRateConfig.isBitRateConstant = false;
    mTestMode = -1;
    mIsH265 = 0;
    mCameraId = -1;
    // show FPS by default
    mShowFps = 1;
    // In ZSL by default
    mZslEnabled = true;
    mFpsRange[0] = 1;
    mFpsRange[1] = 30;
}

/************************************************************************
* name : ~TestConfig
* function: destory object.
************************************************************************/
TestConfig::~TestConfig()
{
    delete mDump;
    mDump = NULL;
}

/************************************************************************
* name : parseCommandlineMetaUpdate
* function: get update meta info from cmd.
************************************************************************/
int TestConfig::parseCommandlineMetaUpdate(char* order,android::CameraMetadata* metaUpdate)
{
    enum {
        MANUAL_EXPOSURE = 0,
        MANUAL_ISO,
        ANTBANDING,
        MANUAL_SENSITIVIYT,
        MANUAL_AWB_MODE,
        MANUAL_AF_MODE,
        MANUAL_AE_MODE,
        MANUAL_AE_ANTIBANDING_MODE,
        MANUAL_COLOR_CORRECTION_MODE,
        MANUAL_CONTROL_MODE,
        MANUAL_SCALER_CROP_REGION,
        MANUAL_ZSL_MODE,
        TOTAL_NUM_FRAMES,
        EXPOSURE_METERING,
        SELECT_PRIORITY,
        EXP_PRIORITY,
        JPEG_QUALITY,
    };
    char *const token[] = {
        [MANUAL_EXPOSURE]                   = (char *const)"manual_exp",
        [MANUAL_ISO]                        = (char *const)"manual_iso",
        [ANTBANDING]                        = (char *const)"antimode",
        [MANUAL_SENSITIVIYT]                = (char *const)"manualsens",
        [MANUAL_AWB_MODE]                   = (char *const)"manualawbmode",
        [MANUAL_AF_MODE]                    = (char *const)"manualafmode",
        [MANUAL_AE_MODE]                    = (char *const)"manualaemode",
        [MANUAL_AE_ANTIBANDING_MODE]        = (char *const)"manualantimode",
        [MANUAL_COLOR_CORRECTION_MODE]      = (char *const)"manualcorcorrectionmode",
        [MANUAL_CONTROL_MODE]               = (char *const)"manualctrmode",
        [MANUAL_SCALER_CROP_REGION]         = (char *const)"manualcropregion",
        [MANUAL_ZSL_MODE]                   = (char *const)"manualzslmode",
        [TOTAL_NUM_FRAMES]                  = (char *const)"numframes",
        [EXPOSURE_METERING]                 = (char *const)"expmetering",
        [SELECT_PRIORITY]                   = (char *const)"selPriority",
        [EXP_PRIORITY]                      = (char *const)"expPriority",
        [JPEG_QUALITY]                      = (char *const)"jpegquality",
        NULL
    };
    char* value;
    int errfnd = 0;
    int width;
    int height;
    int res = 0;
    int sensitivity = 0;
    uint8_t awbmode;
    uint8_t afmode;
    uint8_t antibandingmode;
    uint8_t colorCorrectMode;
    uint8_t ctrlMode;
    int32_t cropregion[4] = {0};
    uint8_t zslMode;

    sp<VendorTagDescriptor> vTags =
        android::VendorTagDescriptor::getGlobalVendorTagDescriptor();


    while (*order != '\0' && !errfnd) {
        switch ( getsubopt(&order, token, &value)){
        case MANUAL_EXPOSURE:{
            printf("exposure value:%s\n",value);
            float expTimeMs;
            sscanf(value,"%f",&expTimeMs);
            int64_t expTimeNs = (int64_t)(expTimeMs*1000000L);
            uint8_t aemode = 0;
            (*metaUpdate).update(ANDROID_CONTROL_AE_MODE,&(aemode),1);
            (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            break;
        }
        case MANUAL_ISO:{
            printf("iso value:%s\n",value);
            int32_t iso;
            uint8_t aemode = 0;
            sscanf(value,"%d",&iso);
            (*metaUpdate).update(ANDROID_CONTROL_AE_MODE,&(aemode),1);
            (*metaUpdate).update(ANDROID_SENSOR_SENSITIVITY,&(iso),1);
            break;
        }
        case ANTBANDING:{
            uint8_t antimode;
            sscanf(value,"%d",&antimode);
            (*metaUpdate).update(ANDROID_CONTROL_AE_ANTIBANDING_MODE,&antimode,1);
            break;
        }
        case MANUAL_SENSITIVIYT:{
            int64_t expTimeNs;
            printf("sensor sensitivity:%s\n",value);
            sscanf(value,"%d",&sensitivity);
            (*metaUpdate).update(ANDROID_SENSOR_SENSITIVITY, &(sensitivity),1);
            if (100 == sensitivity) {
                expTimeNs = 90604540;
                (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            } else if (200 == sensitivity) {
                expTimeNs = 40000000;
                (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            } else if (400 == sensitivity) {
                expTimeNs = 25000000;
                (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            } else if (800 == sensitivity) {
                expTimeNs = 15000000;
                (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            } else if (1600 == sensitivity) {
                expTimeNs = 8000000;
                (*metaUpdate).update(ANDROID_SENSOR_EXPOSURE_TIME,&(expTimeNs),1);
            }
            break;
        }
        case MANUAL_AWB_MODE:{
            printf("awb mode:%s 0:off 1:auto\n",value);
            sscanf(value,"%d",&awbmode);
            (*metaUpdate).update(ANDROID_CONTROL_AWB_MODE, &(awbmode), 1);
        }
            break;
        case MANUAL_AF_MODE:{
            printf("af mode:%s 0:off 1:auto\n",value);
            sscanf(value,"%d",&afmode);
            (*metaUpdate).update(ANDROID_CONTROL_AF_MODE, &(afmode), 1);
        }
            break;
        case MANUAL_AE_MODE:{
            uint8_t aemode = 0;
            sscanf(value,"%d",&aemode);
            printf("ae mode:%d 0:off 1:on\n",aemode);
            (*metaUpdate).update(ANDROID_CONTROL_AE_MODE, &(aemode), 1);
        }
            break;
        case MANUAL_AE_ANTIBANDING_MODE:{
            printf("antibanding mode :%s 0:off 1:50hz 2:60hz 3:auto\n",value);
            sscanf(value,"%d",&antibandingmode);
            (*metaUpdate).update(ANDROID_CONTROL_AE_ANTIBANDING_MODE, &(antibandingmode), 1);
        }
            break;
        case MANUAL_COLOR_CORRECTION_MODE:{
            printf("color correction mode:%s 0:matrix 1:fast 2:hightQuality\n",value);
            sscanf(value,"%d",&colorCorrectMode);
            (*metaUpdate).update(ANDROID_COLOR_CORRECTION_MODE, &(colorCorrectMode), 1);
        }
            break;
        case MANUAL_CONTROL_MODE:{
            printf("contorl mode:%s 0:off 1:auto\n",value);
            sscanf(value,"%d",&ctrlMode);
            (*metaUpdate).update(ANDROID_CONTROL_MODE, &(ctrlMode), 1);
        }
            break;
        case MANUAL_SCALER_CROP_REGION:{
            printf("crop region:%s\n",value);
            sscanf(value,"%dx%dx%dx%d",&cropregion[0], &cropregion[1], &cropregion[2], &cropregion[3]);
            (*metaUpdate).update(ANDROID_SCALER_CROP_REGION, cropregion, 4);
        }
            break;
        case MANUAL_ZSL_MODE:{
            sscanf(value,"%d",&zslMode);
            printf("enble zslMode:%d\n 0:false 1:true\n",zslMode);
            (*metaUpdate).update(ANDROID_CONTROL_ENABLE_ZSL, &(zslMode), 1);
            break;
        }
        case TOTAL_NUM_FRAMES:{
            uint32_t tag = 0;
            int32_t numFrames= 0;
            sscanf(value,"%d",&numFrames);
            CameraMetadata::getTagFromName("org.quic.camera2.mfnrconfigs.MFNRTotalNumFrames",vTags.get(), &tag);
            (*metaUpdate).update(tag,&numFrames,1);
            break;
        }
        case EXPOSURE_METERING:{
            uint32_t tag = 0;
            int32_t metering= 0;
            sscanf(value,"%d",&metering);
            CameraMetadata::getTagFromName("org.codeaurora.qcamera3.exposure_metering.exposure_metering_mode",vTags.get(), &tag);
            (*metaUpdate).update(tag,&metering,1);
            break;
        }
        case SELECT_PRIORITY:{
            uint32_t tag = 0;
            int32_t selPriority= 0;
            sscanf(value,"%d",&selPriority);
            CameraMetadata::getTagFromName("org.codeaurora.qcamera3.iso_exp_priority.select_priority",vTags.get(), &tag);
            (*metaUpdate).update(tag,&selPriority,1);
            break;
        }
        case EXP_PRIORITY:{
            uint32_t tag = 0;
            int64_t expPriority= 0;
            sscanf(value,"%lld",&expPriority);
            CameraMetadata::getTagFromName("org.codeaurora.qcamera3.iso_exp_priority.use_iso_exp_priority",vTags.get(), &tag);
            (*metaUpdate).update(tag,&expPriority,1);
            break;
        }
        case JPEG_QUALITY:{
            printf("jpegquality value:%s\n",value);
            uint8_t jpegquality = 0;
            sscanf(value,"%d",&jpegquality);
            (*metaUpdate).update(ANDROID_JPEG_QUALITY,&(jpegquality),1);
            break;
        }
        default:
            break;
        }
    }
    if (errfnd) res = -1;
    return res;
}


/************************************************************************
* name : parseCommandlineMetaDump
* function: get dump meta info from cmd.
************************************************************************/
int TestConfig::parseCommandlineMetaDump(int ordersize,char* order)
{
    enum {
        EXPOSURE_VALUE = 0,
        ISO_VALUE ,
        AE_MODE,
        AWB_MODE,
        AF_MODE,
        AF_VALUE,
        AE_ANTIBANDING_MODE,
        COLOR_CORRECTION_MODE,
        COLOR_CORRECTION_VALUE,
        CONTROL_MODE,
        SCENE_MODE,
        HDR_MODE,
        ZOOM_VALUE,
        ZSL_MODE,
        TOTAL_NUM_FRAMES,
        EXPOSURE_METERING,
        SELECT_PRIORITY,
        EXP_PRIORITY,
        SHOW_FPS,
        JPEG_QUALITY,
        RESULT_FILE,
        RESULT_LOG_TAG,
    };
    char *const token[] = {
        [EXPOSURE_VALUE]         = (char *const)"expvalue",
        [ISO_VALUE]              = (char *const)"isovalue",
        [AE_MODE]                = (char *const)"aemode",
        [AWB_MODE]               = (char *const)"awbmode",
        [AF_MODE]                = (char *const)"afmode",
        [AF_VALUE]               = (char *const)"afvalue",
        [AE_ANTIBANDING_MODE]    = (char *const)"ae_antimode",
        [COLOR_CORRECTION_MODE]  = (char *const)"color_correctmode",
        [COLOR_CORRECTION_VALUE] = (char *const)"color_correctvalue",
        [CONTROL_MODE]           = (char *const)"controlmode",
        [SCENE_MODE]             = (char *const)"scenemode",
        [HDR_MODE]               = (char *const)"hdrmode",
        [ZOOM_VALUE]             = (char *const)"zoomvalue",
        [ZSL_MODE]               = (char *const)"zslmode",
        [TOTAL_NUM_FRAMES]       = (char *const)"numframes",
        [EXPOSURE_METERING]      = (char *const)"expmetering",
        [SELECT_PRIORITY]        = (char *const)"selPriority",
        [EXP_PRIORITY]           = (char *const)"expPriority",
        [SHOW_FPS]               = (char *const)"showfps",
        [JPEG_QUALITY]           = (char *const)"jpegquality",
        [RESULT_FILE]            = (char *const)"file",
        [RESULT_LOG_TAG]         = (char *const)"tag",
        NULL
    };
    char* value;
    int errfnd = 0;
    int width;
    int height;
    int res = 0;
    ALOGD("order:%s\n",order);
    while (*order != '\0' && !errfnd) {
        char opt = getsubopt(&order, token, &value);
        switch (opt){
        case EXPOSURE_VALUE:{
            int exp = atoi(value);
            mMetaDump.exposureValue = exp;
            printf("exposure time:%d\n",mMetaDump.exposureValue);
            break;
        }
        case ISO_VALUE:{
            int iso = atoi(value);
            mMetaDump.isoValue = iso;
            printf("iso value:%d\n",mMetaDump.isoValue);
            break;
        }
        case AE_MODE:{
            int ae_mode = atoi(value);
            mMetaDump.aeMode = ae_mode;
            printf("AE mode:%d\n",mMetaDump.aeMode);
            break;
        }
        case AWB_MODE:{
            int awb_mode = atoi(value);
            mMetaDump.awbMode = awb_mode;
            printf("AWB mode:%d\n",mMetaDump.awbMode);
            break;
        }
        case AF_MODE:{
            int af_mode = atoi(value);
            mMetaDump.afMode = af_mode;
            printf("AF mode:%d\n",mMetaDump.afMode);
            break;
        }
        case AF_VALUE:{
            int af_value = atoi(value);
            mMetaDump.afValue = af_value;
            printf("AF value:%d\n",mMetaDump.afValue);
            break;
        }
        case AE_ANTIBANDING_MODE:{
            int anti = atoi(value);
            mMetaDump.aeAntiMode = anti;
            printf("aeAnti Mode:%d\n",mMetaDump.aeAntiMode);
            break;
        }
        case COLOR_CORRECTION_MODE:{
            int cc_mode = atoi(value);
            mMetaDump.colorCorrectMode = cc_mode;
            printf("colorCorrect Mode:%d\n",mMetaDump.colorCorrectMode);
            break;
        }
        case COLOR_CORRECTION_VALUE:{
            int cc_value = atoi(value);
            mMetaDump.colorCorrectValue = cc_value;
            printf("colorCorrect Value:%d\n",mMetaDump.colorCorrectValue);
            break;
        }
        case CONTROL_MODE:{
            int control = atoi(value);
            mMetaDump.controlMode = control;
            printf("control mode:%d\n",mMetaDump.controlMode);
        }
        case SCENE_MODE:{
            int scene = atoi(value);
            mMetaDump.sceneMode = scene;
            printf("scene mode:%d\n",mMetaDump.sceneMode);
            break;
        }
        case HDR_MODE:{
            int hdr = atoi(value);
            mMetaDump.hdrMode = hdr;
            printf("hdr mode:%d\n",mMetaDump.hdrMode);
            break;
        }
        case ZOOM_VALUE:{
            int zoom = atoi(value);
            mMetaDump.zoomValue = zoom;
            printf("zoom mode:%d\n",mMetaDump.zoomValue);
            break;
        }
        case ZSL_MODE:{
            int zsl = atoi(value);
            mMetaDump.zslMode = zsl;
            printf("zsl mode:%d\n",mMetaDump.zslMode);
            break;
        }
        case TOTAL_NUM_FRAMES:{
            int frame_num = atoi(value);
            mMetaDump.numFrames = frame_num;
            printf("exp metering:%d\n",mMetaDump.numFrames);
            break;
        }
        case EXPOSURE_METERING:{
            int exp_metering = atoi(value);
            mMetaDump.expMetering = exp_metering;
            printf("exp metering:%d\n",mMetaDump.expMetering);
            break;
        }
        case SELECT_PRIORITY:{
            int priority = atoi(value);
            mMetaDump.selPriority = priority;
            printf("exp metering:%d\n",mMetaDump.selPriority);
            break;
        }
        case EXP_PRIORITY:{
            int exp_priority = atoi(value);
            mMetaDump.expPriority = exp_priority;
            printf("exp metering:%d\n",mMetaDump.expPriority);
            break;
        }
        case RESULT_FILE:{
            char file[200] = {0};
            sscanf(value,"%s",file);
            mDump->setPath(file);
            break;
        }
        case RESULT_LOG_TAG:{
            if (mDump) {
                mDump->mTag = "[" + string(value) + "] ";
            }
            break;
        }
        case SHOW_FPS:{
            int showfps = 0;
            sscanf(value,"%d",&showfps);
            printf("show fps:%d\n",showfps);
            mShowFps = showfps;
            break;
        }
        case JPEG_QUALITY:{
            int jpeg_quality = atoi(value);
            mMetaDump.jpegquality = jpeg_quality;
            printf("jpegquality value:%d\n",mMetaDump.jpegquality);
            break;
        }
        default:{
            printf("opt:%d order:%s not support\n",opt,value);
            break;
        }
        }
    }
    if (errfnd) res = -1;
    return res;
}

/************************************************************************
* name : parseCommandlineAdd
* function: get Initialize info from cmd for open camera.
************************************************************************/
int TestConfig::parseCommandlineAdd(int ordersize,char* order)
{
    enum {
        ID_OPT = 0,
        PREVIEW_SIZE_OPT,
        SNAPSHOT_SIZE_OPT,
        VIDEO_SIZE_OPT,
        PREVIEW_FORMAT_OPT,
        SNAPSHOT_FORMAT_OPT,
        VIDEO_FORMAT_OPT,
        SHOT_NUMBER,
        RESULT_FILE,
        LOG_FILE,
        FPS_RANGE,
        CODEC_TYPE,
        ZSL_MODE,
        BITRATE,
        TARGET_BITRATE,
        IS_BITRATE_CONST,
    };
    char *const token[] = {
        [ID_OPT]                = (char *const)"id",
        [PREVIEW_SIZE_OPT]      = (char *const)"psize",
        [SNAPSHOT_SIZE_OPT]     = (char *const)"ssize",
        [VIDEO_SIZE_OPT]        = (char *const)"vsize",
        [PREVIEW_FORMAT_OPT]    = (char *const)"pformat",
        [SNAPSHOT_FORMAT_OPT]   = (char *const)"sformat",
        [VIDEO_FORMAT_OPT]      = (char *const)"vformat",
        [SHOT_NUMBER]           = (char *const)"snapnum",
        [RESULT_FILE]           = (char *const)"resultfile",
        [LOG_FILE]              = (char *const)"logfile",
        [FPS_RANGE]             = (char *const)"fpsrange",
        [CODEC_TYPE]            = (char *const)"codectype",
        [ZSL_MODE]              = (char *const)"zsl",
        [BITRATE]               = (char *const)"bitrate",
        [TARGET_BITRATE]        = (char *const)"targetbitrate",
        [IS_BITRATE_CONST]      = (char *const)"isbitrateconst",
        NULL
    };
    enum {
        CONTROL_RATE_CONSTANT = 1,
    };

    char* value;
    int errfnd = 0;
    int width;
    int height;
    int res = 0;
    int modeConfig = 0;
    ALOGD("Command Add:%s\n",order);
    while (*order != '\0' && !errfnd) {
        switch ( getsubopt(&order, token, &value)){
        case ID_OPT:{
            printf("camera:%s\n",value);
            int id = atoi(value);
            mCameraId = id;
        }
            break;
        case PREVIEW_SIZE_OPT:{
            printf("preview size:%s\n",value);
            sscanf(value,"%dx%d",&width,&height);
            mPreviewStream.width = width;
            mPreviewStream.height = height;
            modeConfig |= (1 << TESTMODE_PREVIEW);
        }
            break;
        case SNAPSHOT_SIZE_OPT:{
            printf("snapshot size:%s\n",value);
            sscanf(value,"%dx%d",&width,&height);
            mSnapshotStream.width = width;
            mSnapshotStream.height = height;
            modeConfig |= (1 << TESTMODE_SNAPSHOT);
        }
            break;
        case VIDEO_SIZE_OPT:{
            printf("video size:%s\n video format:%s\n",value,"yuv420");
            sscanf(value,"%dx%d",&width,&height);
            mVideoStream.width = width;
            mVideoStream.height = height;
            mVideoStream.format = HAL_PIXEL_FORMAT_YCBCR_420_888;
            modeConfig |= (1 << TESTMODE_VIDEO);
        }
            break;
        case PREVIEW_FORMAT_OPT:{
            printf("preview format:%s\n",value);
            /****
              support:
              yuv420 HAL_PIXEL_FORMAT_YCBCR_420_888 35
              jpeg HAL_PIXEL_FORMAT_BLOB 33
              yuv_ubwc HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED 34
              raw10 HAL_PIXEL_FORMAT_RAW10 37
              raw16 HAL_PIXEL_FORMAT_RAW16 32
              raw_opaque HAL_PIXEL_FORMAT_RAW_OPAQUE 36
             *****/
            if (!strcmp("yuv420", value)) {
                mPreviewStream.format = HAL_PIXEL_FORMAT_YCBCR_420_888;
            } else if (!strcmp("yuv_ubwc", value)) {
                mPreviewStream.format = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
            }
        }
            break;
        case SNAPSHOT_FORMAT_OPT:{
            printf("snapshot format:%s\n",value);
            if (!strcmp("yuv420", value)) {
                mSnapshotStream.format = HAL_PIXEL_FORMAT_YCBCR_420_888;
            } else if (!strcmp("jpeg", value)) {
                mSnapshotStream.format = HAL_PIXEL_FORMAT_BLOB;
            } else if (!strcmp("raw10", value)) {
                mSnapshotStream.format = HAL_PIXEL_FORMAT_RAW10;
            } else if (!strcmp("raw16", value)) {
                mSnapshotStream.format = HAL_PIXEL_FORMAT_RAW16;
            }
        }
            break;
        case VIDEO_FORMAT_OPT:{
            printf("video format:%s\n","yuv420");
        }
            break;
        case SHOT_NUMBER:{
            printf("shot number:%s\n",value);
            int num;
            sscanf(value,"%d",&num);
            mSnapshotStream.requestNumber = num;
        }
            break;
        case RESULT_FILE:
            printf("result file:%s\n",value);
            break;
        case LOG_FILE:
            printf("log file:%s\n",value);
            break;
        case FPS_RANGE:{
            int fps_range[2];
            sscanf(value,"%d-%d",&fps_range[0], &fps_range[1]);
            if (fps_range[0]>fps_range[1]) {
                printf("FPS_RANGE Wrong: min:%d is bigger than max:%d exchange them\n",
                    fps_range[0],fps_range[1]);
                mFpsRange[0] = fps_range[1];
                mFpsRange[1] = fps_range[0];
            } else {
                printf("FPS_RANGE: min:%d max:%d\n",fps_range[0],fps_range[1]);
                mFpsRange[0] = fps_range[0];
                mFpsRange[1] = fps_range[1];
            }
            break;
        }

        case CODEC_TYPE:
            printf("codec type: %s\n",value);
            mIsH265 = atoi(value);
            break;
        case ZSL_MODE:{
            int isZSL = atoi(value);
            printf("in %sZSL mode",isZSL ? "":"Non-");
            if (isZSL == 0){
               mZslEnabled = false;
            }
            break;
        }
        case BITRATE:{
            uint32_t bitrate = 0;
            sscanf(value, "%u", &bitrate);
            printf("bitrate: %u\n", bitrate);
            mVideoRateConfig.bitrate = bitrate * 1024 * 1024;
            break;
        }
        case TARGET_BITRATE:{
            uint32_t targetBitrate = 0;
            sscanf(value, "%u", &targetBitrate);
            printf("targetBitrate: %u\n", targetBitrate);
            mVideoRateConfig.targetBitrate = targetBitrate * 1024 * 1024;
            break;
        }
        case IS_BITRATE_CONST:{
            int isBitRateConstant = atoi(value);
            printf("%sbitRateConstant mode\n", isBitRateConstant ? "" : "Non-");
            if (isBitRateConstant == (int) CONTROL_RATE_CONSTANT) {
                mVideoRateConfig.isBitRateConstant = true;
            }
            break;
        }
        default:
            printf("WARNING Command Add unsupport order param: %s \n", value);
            break;
        }
    }
    switch (modeConfig){
        case (1 << TESTMODE_PREVIEW):
            mTestMode = TESTMODE_PREVIEW;
            break;
        case ((1 << TESTMODE_PREVIEW) | (1 << TESTMODE_SNAPSHOT)):
            mTestMode = TESTMODE_SNAPSHOT;
            if (!mZslEnabled){
                // when NonZSL is set, enter preview-only mode first.
                // switch to snapshot mode when snapshot.
                mTestMode = TESTMODE_PREVIEW;
            }
            break;
        case ((1 << TESTMODE_PREVIEW) | (1 << TESTMODE_SNAPSHOT) | (1 << TESTMODE_VIDEO)):
            mTestMode = TESTMODE_VIDEO;
            break;
        default:
            res = -1;
            break;
    }
    if (errfnd) res = -1;
    return res;
}
