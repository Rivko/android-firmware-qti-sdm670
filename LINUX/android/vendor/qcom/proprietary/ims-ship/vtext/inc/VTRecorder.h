/*****************************************************************************

 ============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VTRecorder.h
 Description: Implementation of VTRecorder

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/

#ifndef __VTRECORDER_H__
#define __VTRECORDER_H__
#include <VideoCodecBase.h>
#include <qplog.h>
#include <map>

using namespace android;

#define VT_HEVCProfileMain 0x00000001
#define VT_HEVCProfileMain10 0x00000002
#define VT_HEVCProfileMain10HDR10 0x00001000
#define VT_H263ProfileBaseline 0x00000001
#define VT_AVCProfileBaseline 0x00000001
#define VT_AVCProfileCBPProfile 0x7F000001
#define VT_AVCProfileCHPProfile 0x7F000002

/* Number of LTR MARK Frames to be set when encoder config happens*/
#define NUM_LTR_MARK_FRAMES (2)
/* Max Idr frequency value. The frequency at which IDR frames will be generated*/
#define MAX_IDR_PERIOD (0xFFFFFF)
/* I-Frame Interval */
#define MAX_IDR_INTERVAL (-1)

class VtCamera
{

  public:
    VtCamera() {};
    virtual ~VtCamera() {};
    virtual void registerCameraCallbacks(boolean flag) {
      ERROR2("registerCameraCallbacks Not Implemented: flag %d", flag);
    };

};
class VTRecorder : public VideoCodecBase
{

  public:
    VTRecorder();
    virtual ~VTRecorder();
    int postEvent(void *info);
    virtual QPE_VIDEO_ERROR Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                 void* pUserData, QPE_VIDEO_DEV eVDev,
                                 QP_VIDEO_CONFIG* pCodecConfig) = 0;
    virtual QPE_VIDEO_ERROR DeInit() = 0;
    virtual QPE_VIDEO_ERROR Start() = 0;
    virtual QPE_VIDEO_ERROR Stop() = 0;
    virtual QPE_VIDEO_ERROR Pause() {
      ERROR1("Pause NOT IMPLEMENTED");
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR Resume() {
      ERROR1("Resume NOT IMPLEMENTED");
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR Configure(QP_VIDEO_CONFIG CodecConfig) = 0;
    virtual QPE_VIDEO_ERROR RegisterRecorderCallBack(QP_VIDEO_CALLBACK
        tVideoCallBack,
        void* pUserData, QPE_VIDEO_DEV eVDev,
        QP_VIDEO_CONFIG* pCodecConfig);
    virtual void HandleEvent(void* data) {
      if (data == NULL) {
        ERROR1("HandleEvent NOT IMPLEMENTED");
      }
    };
    //Recorder specific functions
    virtual QPE_VIDEO_ERROR CreateInputRecordingSurface(sp<IGraphicBufferProducer>
        *bufferproducer, uint32_t height, uint32_t width) = 0;
    virtual QPE_VIDEO_ERROR AdaptVideoBitRate(uint32_t iBitRate) {
      ERROR2("AdaptVideoBitrate NOT IMPLEMENTED %d", iBitRate);
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR AdaptVideoFrameRate(uint32_t iFrameRate) {
      ERROR2("AdaptVideoFrameRate NOT IMPLEMENTED %d", iFrameRate);
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR SetRotationAngle(uint16_t rotation) {
      ERROR2("SetRotation Angle NOT IMPLEMENTED %d", rotation);
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR SetSourceTime(uint8_t useSystemTime) {
      ERROR2("SetSourceTime NOT IMPLEMENTED %d", useSystemTime);
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR GenerateH264IDRFrame() {
      ERROR1("GenerateH264IDRTime NOT IMPLEMENTED");
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR ProcessRecordingFrame(uint64_t timestamp, void *dataPtr,
        size_t size) = 0;
    virtual QPE_VIDEO_ERROR WaitTillAllBuffersAreConsumed() {
      ERROR1("WaitTillAllBuffersAreConsumed NOT IMPLEMENTED");
      return VIDEO_ERROR_OK;
    };
    virtual QPE_VIDEO_ERROR AdaptVideoIdrPeriodicity(int32_t iFrameInt)  {
      ERROR2("AdaptVideoIdrPeriodicity NOT IMPLEMENTED %d", iFrameInt);
      return VIDEO_ERROR_OK;
    };

    /* This API will be called to mark LTR frame.*/
    virtual void ConfigLTRMark(int32_t slLtrIdx) = 0;
    /* This API will be called to use LTR frame.*/
    virtual void ConfigLTRUse(int32_t slLtrIdx) = 0;

    /*define profile and level values*/
    typedef enum H263_Level {
      VT_H263Level10 = 0x00000001,
      VT_H263Level20 = 0x00000002,
      VT_H263Level30 = 0x00000004,
      VT_H263Level40 = 0x00000008,
      VT_H263Level45 = 0x00000010,
      VT_H263Level50 = 0x00000020,
      VT_H263Level60 = 0x00000040,
      VT_H263Level70 = 0x00000080
    } H263_Level;

    typedef enum AVC_Level {
      VT_AVCLevel1   = 0x00000001,
      VT_AVCLevel1b  = 0x00000002,
      VT_AVCLevel11  = 0x00000004,
      VT_AVCLevel12  = 0x00000008,
      VT_AVCLevel13  = 0x00000010,
      VT_AVCLevel2   = 0x00000020,
      VT_AVCLevel21  = 0x00000040,
      VT_AVCLevel22  = 0x00000080,
      VT_AVCLevel3   = 0x00000100,
      VT_AVCLevel31  = 0x00000200,
      VT_AVCLevel32  = 0x00000400,
      VT_AVCLevel4   = 0x00000800,
      VT_AVCLevel41  = 0x00001000,
      VT_AVCLevel42  = 0x00002000,
      VT_AVCLevel5   = 0x00004000,
      VT_AVCLevel51  = 0x00008000,
      VT_AVCLevel52  = 0x00020000,
    } AVC_Level;

    typedef enum H265_Level {
      VT_HEVCMainTierLevel1   = 0x00000001,
      VT_HEVCMainTierLevel2   = 0x00000004,
      VT_HEVCMainTierLevel21  = 0x00000010,
      VT_HEVCMainTierLevel3   = 0x00000040,
      VT_HEVCMainTierLevel31  = 0x00000100,
      VT_HEVCMainTierLevel4   = 0x00000400,
      VT_HEVCMainTierLevel41  = 0x00001000,
      VT_HEVCMainTierLevel5   = 0x00004000,
      VT_HEVCMainTierLevel51  = 0x00010000,
      VT_HEVCMainTierLevel52  = 0x00040000,
      VT_HEVCMainTierLevel6   = 0x00100000,
      VT_HEVCMainTierLevel61  = 0x00400000,
      VT_HEVCMainTierLevel62  = 0x01000000,
    } H265_Level;

  protected:
    QP_VIDEO_ENCODER_INFO encoderOutputInfo;
    uint8_t m_SliceMode;
    boolean m_bLTRFeatureOn;
    uint64_t m_ullLastIdrTime;
    //Min QP factor used by multimedia driver and firmware to configure encoder
    uint32_t m_ulMinQpValue;
    //Max QP factor used by multimedia driver and firmware to configure encoder
    uint32_t m_ulMaxQpValue;
    //TODO : set for H263 and other profiles as well
    std::map<QPE_VIDEO_H265_PROFILE_LEVEL, int32_t> m_h265LevelMap;
    std::map<QPE_VIDEO_AVC_PROFILE_LEVEL,  int32_t> m_h264LevelMap;
    std::map<QPE_VIDEO_H263_PROFILE_LEVEL, int32_t> m_h263LevelMap;
};
#endif //  __VIDEO_OMX_H_
