/*****************************************************************************

============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================

 File: VideoCodecEncoder.cpp
 Description: Implementation of VideoCodecEncoder

 Revision History
 ===============================================================================
 Date    |   Author's Name    |  BugID  |        Change Description
 ===============================================================================
 1-Dec-16   Sanjeev Mittal      First Version
 *****************************************************************************/
#include<VideoCodecEncoder.h>
#define COLOR_FORMAT_SURFACE            (0x7f000789)
#define COLOR_FORMAT_YUV420_FLEXIBLE    (0x7f420888)
#define COLOR_FORMAT_YUV420_SEMIPLANAR  (0x00000015)

#define MEDIA_KEY_BITRATE_MODE       (char *)"bitrate-mode\0"
#define MEDIA_H264_VIDEO_MIME_TYPE    (char*)"video/avc\0"
#define MEDIA_H263_VIDEO_MIME_TYPE  (char*)"video/3gpp\0"
#define MEDIA_HEVC_VIDEO_MIME_TYPE   (char*)"video/hevc\0"

#define MEDIA_KEY_GENERATE_IDR (char*)"request-sync\0"
#define MEDIA_KEY_SPS_PPS_PREPEND (char*)"prepend-sps-pps-to-idr-frames\0"
#define MEDIA_KEY_VIDEO_BITRATE (char*)"video-bitrate\0"
#define MEDIA_KEY_LEVEL (char*)"level\0"
#define MEDIA_KEY_PROFILE (char*)"profile\0"
#define MEDIA_KEY_ROTATION (char*)"vendor.qti-ext-enc-preprocess-rotate.angle\0"
#define MEDIA_KEY_AVTIMER (char*)"vendor.qti-ext-enc-timestamp-source-avtimer.enable\0"
#define MEDIA_KEY_CONSTRAINED_BASELINE (char*)"vendor.qti-ext-enc-custom-profile-level.profile\0"
#define MEDIA_KEY_CUSTOM_LEVEL (char*)"vendor.qti-ext-enc-custom-profile-level.level\0"
#define MEDIA_KEY_RESYNC_SLICE_MODE (char*)"vendor.qti-ext-enc-error-correction.resync-marker-spacing-bits\0"
#define MEDIA_KEY_I_REFRESH_PERIOD_NPFRAMES (char *)"vendor.qti-ext-enc-avc-intra-period.n-pframes\0"
#define MEDIA_KEY_I_REFRESH_N_IDR_PERIOD (char *)"vendor.qti-ext-enc-avc-intra-period.n-idr-period\0"
#define MEDIA_KEY_VIDEO_FRAMERATE (char*)"vendor.qti-ext-enc-dynamic-frame-rate.frame-rate\0"
#define MEDIA_KEY_OUT_FRAME_WIDTH (char*)"vendor.qti-ext-down-scalar.output-width\0"
#define MEDIA_KEY_OUT_FRAME_HEIGHT (char*)"vendor.qti-ext-down-scalar.output-height\0"
#define MEDIA_KEY_EXTRA_DATA_STRING (char *)"vendor.qti-ext-extradata-enable.types\0"
#define MEDIA_KEY_LTR_EXTRA_DATA    (char *)"ltrinfo\0"
#define MEDIA_KEY_LTR_NUM_FRAMES  (char *)"vendor.qti-ext-enc-ltr-count.num-ltr-frames\0"
#define MEDIA_KEY_LTR_MAX_COUNT   (char *)"vendor.qti-ext-enc-caps-ltr.max-count\0"
#define MEDIA_KEY_LTR_MARK_FRAME  (char *)"vendor.qti-ext-enc-ltr.mark-frame\0"
#define MEDIA_KEY_LTR_USE_FRAME   (char *)"vendor.qti-ext-enc-ltr.use-frame\0"
#define MEDIA_KEY_MIN_I_QP_RANGE_VALUE  (char *)"vendor.qti-ext-enc-qp-range.qp-i-min\0"
#define MEDIA_KEY_MAX_I_QP_RANGE_VALUE   (char *)"vendor.qti-ext-enc-qp-range.qp-i-max\0"
#define MEDIA_KEY_MIN_P_QP_RANGE_VALUE  (char *)"vendor.qti-ext-enc-qp-range.qp-p-min\0"
#define MEDIA_KEY_MAX_P_QP_RANGE_VALUE   (char *)"vendor.qti-ext-enc-qp-range.qp-p-max\0"

VideoCodecEncoder::VideoCodecEncoder()
{
  iRecorderState = eVideoStateNotReady;
  m_format = NULL;
  AMediaCodecnativeWindow = NULL;
  mCodec = NULL;
  m_bMediaCodecError = false;
  CRITICAL1("VideoCodecEncoder ctor called");
}

VideoCodecEncoder::~VideoCodecEncoder()
{
  CRITICAL1("VideoCodecEncoder dtor called");
}

void VideoCodecEncoder::videoRecordLoop()
{
  MINIMAL1("[THINCLIENT]videoRecordLoop thread Started");
  QPE_VIDEO_ERROR eStatus = VIDEO_ERROR_OK;

  /* If static image flag enabled, exit the loop. */
  while (iRecorderState == eVideoStateActive) {
    DEBUG1("[THINCLIENT]dequeuing next frame");
    eStatus = GetEncodedFrame();

    if (VIDEO_ERROR_OK != eStatus) {
      ERROR2("videoRecordLoop failed with error %d ", eStatus);
      break;
    }
  }

  CRITICAL2("Exit videoRecordLoop thread RecorderStartState = %d ",
            iRecorderState);
  return;
}

QPE_VIDEO_ERROR VideoCodecEncoder::Init(QP_VIDEO_CALLBACK tVideoCallBack,
                                        void *pUserData, QPE_VIDEO_DEV eVDev,
                                        QP_VIDEO_CONFIG *pCodecConfig)
{
  media_status_t mstatus = AMEDIA_OK;
  CRITICAL1("called the initialised callback");
  m_bMediaCodecError = false;
//please never remove these register api call code as it is first step of
  //registration of callback and variables
  RegisterRecorderCallBack(tVideoCallBack, pUserData,  eVDev, pCodecConfig);
  RegisterCallBacks(tVideoCallBack, pUserData, eVDev);
  iRecorderState = eVideoStateNotReady;

  if (AMediaCodecnativeWindow.get() == NULL) {
    ANativeWindow* AnativeWindow;
    mstatus = AMediaCodec_createPersistentInputSurface(&AnativeWindow);
    AMediaCodecnativeWindow = AnativeWindow;
  }

  if (mstatus != AMEDIA_OK) {
    ERROR2("CreatePersistentInputRecordingSurface returned status %d",
           (int)mstatus);
    return VIDEO_ERROR_UNKNOWN;
  } else {
    CRITICAL1("Surface is created successful");
  }

  SendEventsToThinclient(VIDEO_MSG_DEV_INITIALISED);
  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoCodecEncoder::CreateInputRecordingSurface(
  sp<IGraphicBufferProducer> *bufferproducer,
  uint32_t height, uint32_t width)
{
  ERROR3("Creating encoder input surface h=%d w=%d", height, width);
  //ANativeWindow *AMediaCodecnativeWindow;
  media_status_t mstatus;

  if (AMediaCodecnativeWindow.get() != NULL) {
    android::sp<android::Surface> surface(static_cast<android::Surface *>
                                          (AMediaCodecnativeWindow.get()));

    if (surface.get() != NULL) {
      *bufferproducer = surface->getIGraphicBufferProducer();
    } else {
      CRITICAL1("surface is null");
    }

    // ERROR1("setting surface to codec");
    //AMediaCodec_setInputSurface(mCodec,AMediaCodecnativeWindow);
  } else {
    ERROR1("ANativeWindow Surface is null");
  }

  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoCodecEncoder::Configure(QP_VIDEO_CONFIG tCodecConfig)
{
  CHECK_MEDIA_ERROR;
  QPE_VIDEO_ERROR eStatus = VIDEO_ERROR_OK;
  media_status_t mstatus;
  uint32_t iWidth;
  uint32_t iHeight;
  enum eVideoState prevState = iRecorderState;
  EVENT3("VideoTransmitter::Configure:: prev codec is %d and new codec received is %d",
         m_codecConfig.eCodec, tCodecConfig.eCodec);
  /* We do not need to copy pointers, reset to NULL */
  memcpy(&m_codecConfig, &tCodecConfig, sizeof(QP_VIDEO_CONFIG) );
  m_codecConfig.pNALHeader = m_codecConfig.pVolHeader = NULL;
  CRITICAL4("Configure: ePktnMode - %d Config Mode - %d, rc - %d",
            m_codecConfig.ePktnMode, m_codecConfig.iInputVideoBufferMode,
            m_codecConfig.eBitRateMode);
  CRITICAL3("Configure: eH265ProfileLevel - %d eH265ProfileType - %d",
            tCodecConfig.eH265ProfileLevel, tCodecConfig.eH265ProfileType);
  CRITICAL3("Configure:  Rotation and HALVersion is %d & %d",
            tCodecConfig.rotation, tCodecConfig.eHALVersion);

  do {
    if (iRecorderState == eVideoStateActive) {
      MINIMAL1("Configure:  Codec is already Active, stop before configuring it");
      bool OutsideInternalEvent = m_bInternalEvent;
      m_bInternalEvent = true;
      eStatus = Stop();
      m_bInternalEvent = OutsideInternalEvent;

      if (VIDEO_ERROR_OK != eStatus) {
        ERROR2("Configure:  Codec stop failed status %d", eStatus);
        break;
      }
    }

    /* Delete format object, if it is already created */
    if (m_format != NULL) {
      CRITICAL1("Configure: format is clearedup");
      AMediaFormat_delete(m_format);
      m_format = NULL;
    }

    if (mCodec) {
      CRITICAL1("Configure: mCodec is clearedup");
      AMediaCodec_delete(mCodec);
      mCodec = NULL;
    }

    m_format = AMediaFormat_new();

    if (NULL == m_format) {
      ERROR1("Configure: m_format is null")
      eStatus = VIDEO_ERROR_UNKNOWN;
      break;
    }

    CRITICAL2("Configure: new format obj created %p", m_format);
#ifdef GPU_ROTATION
    ERROR2("Rotation: %d", m_codecConfig.rotation);
    iWidth = m_codecConfig.cameraConfigWidth;
    iHeight = m_codecConfig.cameraConfigHeight;
    AMediaFormat_setInt32(m_format, MEDIA_KEY_ROTATION, m_codecConfig.rotation);
#else
    iWidth = m_codecConfig.iWidth;
    iHeight = m_codecConfig.iHeight;
    ERROR1("No Rotation");
#endif

    //! If LTR Support is diabled by RTP, then mark the flag to FALSE
    if (!m_codecConfig.iLTRSupportEnabled) {
      CRITICAL1("Tx-LTR: LTR support disabled from RTP");
      m_bLTRFeatureOn = false;
      m_IDRRetransmitOn = true;

      /* If IDR interval is between 1 and 6sec, then do not retransmit IDR frames*/
      if (m_codecConfig.iFrameInterval < IDR_RETRANS_INTERVAL &&
          m_codecConfig.iFrameInterval > 0) {
        m_IDRRetransmitOn = false;
      }
    } else {
      //! Extra data for LTR is set
      m_bLTRFeatureOn = true;
      m_IDRRetransmitOn = true;
      AMediaFormat_setString(m_format, MEDIA_KEY_EXTRA_DATA_STRING,
                             MEDIA_KEY_LTR_EXTRA_DATA);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_LTR_NUM_FRAMES,
                            NUM_LTR_MARK_FRAMES);
      //! Set IDR period to max value
      m_codecConfig.iFrameInterval = MAX_IDR_INTERVAL;
      CRITICAL1("Tx-LTR: Extra Data enable for Encoder");
    } //!if (m_bLTRFeatureOn)
    if (VIDEO_BUFFER_NON_META_MODE == m_codecConfig.iInputVideoBufferMode) {
      m_codecConfig.iFrameInterval =0;
    }
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_WIDTH, iWidth);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_HEIGHT, iHeight);
    CRITICAL6("Configure: Height %d, Width %d, FrameRate %d, BitRate %d, IFrame interval %d",
              iHeight, iWidth,
              m_codecConfig.iFrameRate, m_codecConfig.iBitRate,
              m_codecConfig.iFrameInterval);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_BIT_RATE,
                          m_codecConfig.iBitRate);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_FRAME_RATE,
                          m_codecConfig.iFrameRate);
    AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL,
                          m_codecConfig.iFrameInterval);
    AMediaFormat_setInt32(m_format, MEDIA_KEY_MAX_I_QP_RANGE_VALUE,
                          m_ulMaxQpValue);
    AMediaFormat_setInt32(m_format, MEDIA_KEY_MAX_P_QP_RANGE_VALUE,
                          m_ulMaxQpValue);
    AMediaFormat_setInt32(m_format, MEDIA_KEY_MIN_I_QP_RANGE_VALUE,
                          m_ulMinQpValue);
    AMediaFormat_setInt32(m_format, MEDIA_KEY_MIN_P_QP_RANGE_VALUE,
                          m_ulMinQpValue);

    //COLOR_FormatSurface need to get actual ENUM to configure this static image is non meta mode
    if (VIDEO_BUFFER_META_MODE == m_codecConfig.iInputVideoBufferMode) {
      AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_COLOR_FORMAT,
                            COLOR_FORMAT_SURFACE);
    } else {
      AMediaFormat_setInt32(m_format, AMEDIAFORMAT_KEY_COLOR_FORMAT,
                            COLOR_FORMAT_YUV420_SEMIPLANAR);
    }

    if (m_codecConfig.eCodec != VIDEO_CODEC_H263) {
      AMediaFormat_setInt32(m_format, MEDIA_KEY_SPS_PPS_PREPEND, 1);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_BITRATE_MODE,
                            m_codecConfig.eBitRateMode);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_AVTIMER, 1);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_OUT_FRAME_WIDTH,
                            m_codecConfig.iOut_Width);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_OUT_FRAME_HEIGHT,
                            m_codecConfig.iOut_Height);
      CRITICAL3("Configure: out Height %d, out Width %d",
                m_codecConfig.iOut_Height, m_codecConfig.iOut_Width);
    }

    eStatus = ConfigureProfileAndLevel();

    if (VIDEO_ERROR_OK != eStatus || NULL == mCodec) {
      ERROR1("Configure: ConfigureProfileAndLevel failed");
      break;
    }

    CRITICAL2("Configure :: ePktnMode - %d", m_codecConfig.ePktnMode);

    if ((m_SliceMode || m_codecConfig.ePktnMode == VIDEO_CODEC_H264_SINGLE_NALU) &&
        (VIDEO_CODEC_H263 != m_codecConfig.eCodec) ) {
      CRITICAL2("VideoCodecEncoder::Configure :: configuring slice mode for =%d bits",
                (m_codecConfig.iRtpMTUSize) * 8);
      int32_t bits = ((m_codecConfig.iRtpMTUSize) * 8);
      AMediaFormat_setInt32(m_format, MEDIA_KEY_RESYNC_SLICE_MODE, bits);
    }

    EVENT1("Configure: called for encoder");
    mstatus = AMediaCodec_configure(mCodec, m_format, NULL, NULL,
                                    AMEDIACODEC_CONFIGURE_FLAG_ENCODE);

    if (AMEDIA_OK != mstatus) {
      ERROR2("Configure failed status %d", (int)mstatus);
      eStatus =  VIDEO_ERROR_UNKNOWN;
      break;
    }

    iRecorderState = eVideoStateIdle;
    CRITICAL2("RecorderStateTransition:%d Codec configured", iRecorderState);

    /* Do not provide recording surface if encoder configured in byteBuf
       Mode. Static image feature uses byteBuffer mode. */
    if ((AMediaCodecnativeWindow.get() != NULL &&
         VIDEO_BUFFER_META_MODE == m_codecConfig.iInputVideoBufferMode ))  {
      CRITICAL1("Configure: set Recording surface to codec");
      mstatus = AMediaCodec_setInputSurface(mCodec, AMediaCodecnativeWindow.get());

      if (AMEDIA_OK != mstatus) {
        ERROR2("Configure: AMediaCodec_setInputSurface failed, status %d",
               (int)mstatus);
        eStatus = VIDEO_ERROR_UNKNOWN;
        break;
      }
    }

    //this is for callwaiting scenarios or
    if (eVideoStateActive == prevState) {
      mstatus = AMediaCodec_start(mCodec);

      if (AMEDIA_OK != mstatus) {
        ERROR2("configure: Start failed, status %d", (int)mstatus);
        eStatus = VIDEO_ERROR_UNKNOWN;
        break;
      }

      iRecorderState = eVideoStateActive;

      /* Always start Output frame extraction thread. If input raw bit-stream
         needs to be given to encoder, give it through separate thread */
      if (!StartRecordThread()) {
        ERROR1("Error during creation of the thread\n");
      } else {
        MINIMAL1("videoRecordLoop thread Started");
        videoRecordThreadCreated = 1;
      }

      CRITICAL2("RecorderStateTransition:%d Started mCodec from configure API",
                iRecorderState);
    }
  } while (0);

  if (VIDEO_ERROR_OK == eStatus) {
    CRITICAL2("RecorderStateTransition:%d:VIDEO_MSG_CODEC_CHANGED",
              iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_CODEC_CHANGED);
  } else {
    ERROR2("RecorderStateTransition:%d:VIDEO_MSG_CODEC_CHANGED failed",
           iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_ERROR);
  }

  return eStatus;
}

QPE_VIDEO_ERROR VideoCodecEncoder::Start()
{
  CHECK_MEDIA_ERROR;
  QPE_VIDEO_ERROR eStatus = VIDEO_ERROR_OK;

  if (eVideoStateNotReady == iRecorderState) {
    if (mCodec != NULL && m_format != NULL) {
      CRITICAL1("Configure encoder from Start as codec was previosly configured");
      media_status_t mstatus = AMediaCodec_configure(mCodec, m_format, NULL,
                               NULL,
                               AMEDIACODEC_CONFIGURE_FLAG_ENCODE);

      if (AMEDIA_OK != mstatus) {
        ERROR2("Configure from Start failed, status %d", (int)mstatus);
        eStatus = VIDEO_ERROR_UNKNOWN;
      } else {
        iRecorderState = eVideoStateIdle;
        CRITICAL2("RecorderStateTransition:%d:VIDEO_MSG_CODEC_CHANGED: mCodec configured",
                  iRecorderState);

        /* Do not provide recording surface if encoder configured in byteBuf
           Mode. Static image feature uses byteBuffer mode. */
        if (AMediaCodecnativeWindow.get() != NULL &&
            VIDEO_BUFFER_META_MODE == m_codecConfig.iInputVideoBufferMode) {
          CRITICAL1("Start: set Recording surface to codec");
          mstatus = AMediaCodec_setInputSurface(mCodec, AMediaCodecnativeWindow.get() );

          if (AMEDIA_OK != mstatus) {
            ERROR2("Start AMediaCodec_setInputSurface failed, status %d", (int)mstatus);
            eStatus = VIDEO_ERROR_UNKNOWN;
          }
        }
      }
    } else {
      ERROR4("RecorderStateTransition:%d mCodec %p, m_format %p failed to configure",
             iRecorderState, mCodec, m_format);
      eStatus = VIDEO_ERROR_UNKNOWN;
    }
  } //! if (eVideoStateNotReady == iRecorderState)

  if (eVideoStateIdle == iRecorderState && mCodec) {
    EVENT1("RecordStart : Start API called");
    media_status_t result = AMediaCodec_start(mCodec);

    if (result != AMEDIA_OK) {
      ERROR2("Started returned status %d", (int)result);
      eStatus = VIDEO_ERROR_UNKNOWN;
    } else {
      iRecorderState = eVideoStateActive;

      /* Always start Output frame extraction thread. If input raw bit-stream
         needs to be given to encoder, give it through separate thread */
      if (!StartRecordThread()) {
        ERROR1("Error during creation of the thread\n");
      } else {
        MINIMAL1("videoRecordLoop thread Started");
        videoRecordThreadCreated = 1;
      }
    }
  }

  if (VIDEO_ERROR_OK == eStatus) {
    CRITICAL2("RecorderStateTransition:%d:Start:VIDEO_MSG_RECORDER_STARTED",
              iRecorderState);
    m_LastIDRtimestamp = 0;
    m_IDRframeinterval = 3;
    SendEventsToThinclient(VIDEO_MSG_RECORDER_STARTED);
  } else {
    ERROR2("RecorderStateTransition:%d:Start failed", iRecorderState);
    SendEventsToThinclient(VIDEO_MSG_ERROR);
  }

  return eStatus;
}

/* This API will be called to generate LTR-MARK frame.*/
void VideoCodecEncoder::ConfigLTRMark(int32_t slLtrIdx)
{
  AMediaFormat* sLtrMark = AMediaFormat_new();

  if (NULL != sLtrMark) {
    AMediaFormat_setInt32(sLtrMark, MEDIA_KEY_LTR_MARK_FRAME, slLtrIdx);
  } else {
    ERROR1("Tx-LTR: ConfigLTRMark local format is null")
    return;
  }

  media_status_t result = AMediaCodec_setParameters(mCodec, sLtrMark);

  if (AMEDIA_OK != result) {
    ERROR2("Tx-LTR: ConfigLTRMark returned status %d", (int)result);
  } else {
    CRITICAL2("Tx-LTR: LTR MARK request with index %d", slLtrIdx);
  }

  AMediaFormat_delete(sLtrMark);
  return;
}

/* This API will be called to generate LTR-USE frame.*/
void VideoCodecEncoder::ConfigLTRUse(int32_t slLtrIdx)
{
  AMediaFormat* sLtrMark = AMediaFormat_new();

  if (NULL != sLtrMark) {
    AMediaFormat_setInt32(sLtrMark, MEDIA_KEY_LTR_USE_FRAME, (1 << slLtrIdx));
  } else {
    ERROR1("Tx-LTR: ConfigLTRUse local format is null")
    return;
  }

  media_status_t result = AMediaCodec_setParameters(mCodec, sLtrMark);

  if (AMEDIA_OK != result) {
    ERROR2("TX-LTR: ConfigLTRMark returned status %d", (int)result);
  } else {
    CRITICAL3("Tx-LTR: LTR USE request with index %d, Id requested to encoder 0x%x",
              slLtrIdx, (1 << slLtrIdx));
  }

  AMediaFormat_delete(sLtrMark);
  return;
}

QPE_VIDEO_ERROR VideoCodecEncoder::GetEncodedFrame()
{
  QP_MULTIMEDIA_FRAME iQpFrame;
  MEDIA_PACKET_INFO_TX* pMediaPktInfoTx =
    &(iQpFrame.sMediaPacketInfo.sMediaPktInfoTx);
  timeval tv;
  uint64_t  currentAppsTS;
  AMediaCodecBufferInfo info;
  DEBUG3 ("entering GetEncodedFrame recorder state %d %d",
          iRecorderState, m_codecConfig.iInputVideoBufferMode);
  CHECK_MEDIA_ERROR;

  if (eVideoStateActive == iRecorderState && mCodec) {
    size_t bufsize;
    memset(&iQpFrame, 0, sizeof(QP_MULTIMEDIA_FRAME));
    //time out in microsecond 100ms
    auto status = AMediaCodec_dequeueOutputBuffer(mCodec, &info, 100000);

    if (status >= 0) {
      if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
        CRITICAL1("GetEncodedFrame output EOS");
      }

      MINIMAL4("GetEncodedFrame: dequeueoutputbuffer buffer size %d index %d offset %d",
               info.size, status, info.offset);
      auto buf = AMediaCodec_getOutputBuffer(mCodec, status, NULL);
      AMediaFormat* pFormat = AMediaCodec_getBufferFormat(mCodec, status);

      if (buf == NULL) {
        ERROR1("AMediaCodec_getOutputBuffer returned NULL, short decode");
        return VIDEO_ERROR_UNKNOWN;
      }

      if (m_bLTRFeatureOn && pFormat) {
        void* pExtraData;
        size_t ulExtraDataLen;
        bool bStatus = AMediaFormat_getBuffer(pFormat,
                                              MEDIA_KEY_LTR_EXTRA_DATA,
                                              &pExtraData,
                                              &ulExtraDataLen);

        if (pExtraData && true == bStatus) {
          uint32_t ulDataVal = *((uint32_t*)pExtraData);

          if (0xFFFF != ulDataVal) {
            CRITICAL3("Tx-LTR: ulExtraDataLen %u, ulDataVal 0x%x",
                      (unsigned int)ulExtraDataLen, ulDataVal);
            iQpFrame.sMediaPacketInfo.sMediaPktInfoTx.ulLtrExtraData = ulDataVal;
          }
        }

        AMediaFormat_delete(pFormat);
      } //! if (m_bLTRFeatureOn)

      gettimeofday(&tv, NULL);
      currentAppsTS = TIMEVAL2MS(tv);
      uint64_t ullEncDelay = (getCurrentAVTime() - info.presentationTimeUs) / 1000;
      iQpFrame.pData = (uint8_t *)(buf + info.offset);
      iQpFrame.iDataLen = info.size;
      pMediaPktInfoTx->bMarkerBit = QP_TRUE;
      pMediaPktInfoTx->iTimeStamp = info.presentationTimeUs / 1000;
      pMediaPktInfoTx->appTimeStamp = currentAppsTS - ullEncDelay;
      pMediaPktInfoTx->ullSysVidFrameOPts = getCurrentAVTime() / 1000;
      CRITICAL5_STR("FTB_DONE : data[4] : 0x%x IFrame: %d, Encoder Delay: %llu, appsTS (at frame generated) %llu",
                    buf[4], (info.flags & 0x01),
                    (unsigned long long)ullEncDelay,
                    (unsigned long long)pMediaPktInfoTx->appTimeStamp);

      //! Retry logic for sending I-frame only if IDR interval is more than
      //! 6sec or if periodic IDR frame generation disabled.
      if (m_IDRRetransmitOn) {
        if ((info.flags & 0x01) && !m_LastIDRtimestamp) {
          m_LastIDRtimestamp = (uint32)tv.tv_sec;
        }

        DEBUG4_STR("m_IDRRetransmitOn %d tv.tv_sec %lu m_LastIDRtimestamp : %d",
                   m_IDRRetransmitOn, tv.tv_sec, m_LastIDRtimestamp);

        if (m_IDRRetransmitOn && m_LastIDRtimestamp
            && (((uint32)tv.tv_sec - (uint32)m_LastIDRtimestamp) > m_IDRframeinterval)) {
          GenerateH264IDRFrame();
          CRITICAL1("FTB Trigger IDRframe ");

          if (m_IDRframeinterval == 3) {
            m_IDRframeinterval = 6;
          } else {
            m_IDRRetransmitOn = 0;
          }
        }
      } //! if (m_IDRRetransmitOn)

      // uint64_t avtimer = getCurrentAVTime();
      MINIMAL3_STR("SendEncodedFramesToAppfromIOMX :: Sending frame to RTP frametime = %llu, avtime %llu"
                   , (unsigned long long)pMediaPktInfoTx->iTimeStamp,
                   (unsigned long long)pMediaPktInfoTx->ullSysVidFrameOPts);

      if (eVideoStateActive == iRecorderState) {
        m_pCallback(VIDEO_MSG_RECORDED_DATA, (void *)(&iQpFrame),
                    m_eVidDevType, SUCCESS, m_pUserData);
      } else {
        ERROR2("RecorderStateTransition is not active %d", iRecorderState);
      }

      DEBUG1("encoder output buffers released");
      AMediaCodec_releaseOutputBuffer(mCodec, status, false);
    } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
      MINIMAL1("GetEncodedFrame: encoder output buffers changed");
    } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
      /* Ignore this message, if LTR feature is turned ON. This status comes for
         each frame with LTR feature. */
      if (!m_bLTRFeatureOn) {
        auto format = AMediaCodec_getOutputFormat(mCodec);
        MINIMAL2_STR("encoder format changed to: %s",
                     AMediaFormat_toString(format));
        AMediaFormat_delete(format);
      }
    } else if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
      CRITICAL1("GetEncodedFrame: encoder no output buffer right now");
    } else {
      ERROR2("GetEncodedFrame: unexpected info code: %zd", status);

      // there are cases where camera switch is happening and we are getting this error
      // we will see if we there is still some race condition happening then we might
      // need to expose a API to UI to call and tell that camera has been going to stop so that
      // we can stop ahead of camera and when setrotationangle comes we can start it again
      if (iRecorderState == eVideoStateActive) {
        SendEventsToThinclient(VIDEO_MSG_ERROR);
        m_bMediaCodecError = true;
      }

      //MediaServerDied();
    }
  } else {
    MINIMAL2("GetEncodedFrame: Recorder state is not active %d",
             iRecorderState);
  }

  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoCodecEncoder::ProcessRecordingFrame(uint64_t  timestamp,
    void *dataPtr,
    size_t ulFrameBufSize)
{
  QPE_VIDEO_ERROR eStatus = VIDEO_ERROR_OK;

  do {
    if (VIDEO_BUFFER_NON_META_MODE != m_codecConfig.iInputVideoBufferMode ||
        nullptr == dataPtr || !ulFrameBufSize) {
      MINIMAL3 ("ProcessRecordingFrame called in wrong state: bufferMode %d, dataptr %p",
                m_codecConfig.iInputVideoBufferMode, dataPtr);
      break;
    }

    if (eVideoStateActive != iRecorderState || !mCodec) {
      MINIMAL2("ProcessRecordingFrame: Recorder state is not active %d",
               iRecorderState);
      break;
    }

    MINIMAL4 ("entering ProcessRecordingFrame recorder state %d %d %p",
              iRecorderState, m_codecConfig.iInputVideoBufferMode, dataPtr);
    //time out in microsecond 100ms
    auto InputBufId = AMediaCodec_dequeueInputBuffer(mCodec, 1000000);

    if (InputBufId < 0) {
      ERROR2("ProcessRecordingFrame AMediaCodec_getInputBuffer %lu failed",
             InputBufId);
      break;
    }

    size_t slEncIpBufLen;
    auto pBuf = AMediaCodec_getInputBuffer(mCodec, InputBufId, &slEncIpBufLen);

    if (NULL == pBuf) {
      ERROR1("ProcessRecordingFrame AMediaCodec_getInputBuffer returned NULL buf");
      eStatus =  VIDEO_ERROR_UNKNOWN;
      break;
    } else if (slEncIpBufLen < ulFrameBufSize) {
      ERROR3("ProcessRecordingFrame slEncIpBufLen  %d < ulFrameBufSize %d",
             (int)slEncIpBufLen, (int)ulFrameBufSize);
      eStatus = VIDEO_ERROR_UNKNOWN;
      break;
    }

    memcpy(pBuf, dataPtr, ulFrameBufSize);
    AMediaCodec_queueInputBuffer(mCodec, InputBufId, 0, ulFrameBufSize,
                                 timestamp, 0);
  } while (0);

  return eStatus;
}

QPE_VIDEO_ERROR VideoCodecEncoder::Stop()
{
  QPE_VIDEO_ERROR eStatus = VIDEO_ERROR_OK;
  CRITICAL2("RecorderStateTransition:%d: stop requested", iRecorderState);

  if (eVideoStateActive == iRecorderState && mCodec) {
    iRecorderState = eVideoStateNotReady;

    if (videoRecordThreadCreated == 1) {
      WaitforRecordThread();
      videoRecordThreadCreated = 0;
    }

    media_status_t result = AMediaCodec_stop(mCodec);

    if (AMEDIA_OK != result) {
      ERROR2("stop failed, status %d", (int)result);
      SendEventsToThinclient(VIDEO_MSG_ERROR);
      eStatus = VIDEO_ERROR_UNKNOWN;
    } else {
      CRITICAL2("RecorderStateTransition:Stop:%d:VIDEO_MSG_RECORDER_STOPPED",
                iRecorderState);
      SendEventsToThinclient(VIDEO_MSG_RECORDER_STOPPED);
    }
  } else {
    CRITICAL2("RecorderStateTransition:%d: not in active state, send dummy indication",
              iRecorderState);
  }

  return eStatus;
}

QPE_VIDEO_ERROR VideoCodecEncoder::DeInit()
{
  CRITICAL2("RecorderStateTransition, recorder state %d DeInit requested",
            iRecorderState);

  if (videoRecordThreadCreated == 1) {
    WaitforRecordThread();
    videoRecordThreadCreated = 0;
  }

  AMediaFormat_delete(m_format);
  AMediaCodec_delete(mCodec);

  if (AMediaCodecnativeWindow.get()) {
    ANativeWindow_release(AMediaCodecnativeWindow.get() );
    iRecorderState = eVideoStateReleasing;
  }

  CRITICAL2("RecorderStateTransition:DeInit:%d:VIDEO_MSG_DEV_UNINITIALISED",
            iRecorderState);
  SendEventsToThinclient(VIDEO_MSG_DEV_UNINITIALISED);
  AMediaCodecnativeWindow = NULL;
  return VIDEO_ERROR_OK;
}

QPE_VIDEO_ERROR VideoCodecEncoder::AdaptVideoBitRate(uint32_t bitrate)
{
  QPE_VIDEO_ERROR status = VIDEO_ERROR_OK;
  CRITICAL3("AdaptVideoBitRate Configuring Bitrate %d RecorderState %d",
            bitrate, iRecorderState);

  if (eVideoStateActive == iRecorderState && mCodec) {
    AMediaFormat* bitrateformat = AMediaFormat_new();

    if (NULL != bitrateformat) {
      AMediaFormat_setInt32(bitrateformat, MEDIA_KEY_VIDEO_BITRATE, bitrate);
    } else {
      ERROR1("AdaptVideoBitRate local format is null")
      return VIDEO_ERROR_UNKNOWN;
    }

    media_status_t result = AMediaCodec_setParameters(mCodec, bitrateformat);

    if (AMEDIA_OK != result) {
      ERROR2("AdaptVideoBitRate returned status %d", (int)result);
      status = VIDEO_ERROR_UNKNOWN;
    }
  } else {
    ERROR1(" AdaptVideoBitRate RecorderState is not active");
    status = VIDEO_ERROR_UNKNOWN;
  }

  return status;
}


QPE_VIDEO_ERROR VideoCodecEncoder::GenerateH264IDRFrame()
{
  QPE_VIDEO_ERROR result = VIDEO_ERROR_OK;
  uint32_t iValue = 1;
  AMediaFormat *genIFrame = NULL;

  do {
    struct timeval currtime;
    gettimeofday(&currtime, NULL);
    uint64_t ullCurTime = TIMEVAL2MS(currtime);
    MINIMAL3_STR("VideoCodecEncoder:: GenerateH264IDRFrame IDR=%llu,Curr=%llu",
                 (unsigned long long)m_ullLastIdrTime,
                 (unsigned long long)ullCurTime);

    if (eVideoStateActive != iRecorderState) {
      CRITICAL2("VideoCodecEncoder::GenerateH264IDRFrame: Video state is %d which is not active",
                iRecorderState);
      result = VIDEO_ERROR_OK;
      break;
    }

    /* If last IDR generation time is less than 400ms, then ignore it. Too many
       close by IDR requests cannot be given to encoder. */
    if ((ullCurTime - m_ullLastIdrTime < 400) && (!m_bLTRFeatureOn)) {
      CRITICAL1("VideoCodecEncoder::GenerateH264IDRFrame: frequent IDR requests, ignore this req");
      result = VIDEO_ERROR_OK;
      break;
    }

    m_ullLastIdrTime = ullCurTime;
    genIFrame = AMediaFormat_new();

    if (NULL == genIFrame) {
      ERROR1("AMediaFormat_new failed");
      result = VIDEO_ERROR_UNKNOWN;
      break;
    }

    AMediaFormat_setInt32(genIFrame, MEDIA_KEY_GENERATE_IDR, iValue);

    if (AMEDIA_OK != AMediaCodec_setParameters(mCodec, genIFrame)) {
      ERROR1("AMediaCodec_setParameters failed");
      result = VIDEO_ERROR_UNKNOWN;
      break;
    }
  } while (0);

  if (genIFrame) {
    AMediaFormat_delete(genIFrame);
  }

  return result;
}

QPE_VIDEO_ERROR VideoCodecEncoder::ConfigureProfileAndLevel()
{
  QPE_VIDEO_ERROR ret_val = VIDEO_ERROR_OK;

  /*DO WE NEED TO CHECK FOR HAL1 / HAL3 ?
  */
  switch (m_codecConfig.eCodec) {
    case VIDEO_CODEC_H263:
      AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME,
                             MEDIA_H263_VIDEO_MIME_TYPE);

      /*
       * check h263 map and assign profile
       */
      switch (m_codecConfig.eH263ProfileType) {
        case VIDEO_H263_PROFILE_H320_CODING:
        case VIDEO_H263_PROFILE_BACKWARD_COMPATIBLE:
        case VIDEO_H263_PROFILE_ISWV2:
        case VIDEO_H263_PROFILE_ISWV3:
        case VIDEO_H263_PROFILE_HIGH_COMPRESSION:
        case VIDEO_H263_PROFILE_INTERNET:
        case VIDEO_H263_PROFILE_INTERLACE:
        case VIDEO_H263_PROFILE_HIGH_LATENCY:
          ERROR2("Configure: H263 profile %d not supported",
                 m_codecConfig.eH263ProfileType);
          ret_val = VIDEO_ERROR_UNKNOWN;
          break;

        case VIDEO_H263_PROFILE_BASELINE:
        default:
          AMediaFormat_setInt32(m_format, MEDIA_KEY_PROFILE,
                                VT_H263ProfileBaseline);
          CRITICAL2("Configure: H263 profile set to 0x%x",
                    VT_H263ProfileBaseline);
          break;
      }

      if (m_h263LevelMap.find(m_codecConfig.eH263ProfileLevel) !=
          m_h263LevelMap.end()) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_LEVEL,
                              m_h263LevelMap[m_codecConfig.eH263ProfileLevel]);
        CRITICAL2("Configure: H263 level set to 0x%x",
                  m_h263LevelMap[m_codecConfig.eH263ProfileLevel]);
      } else {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_LEVEL,
                              m_h263LevelMap[VIDEO_H263_LEVEL10]);
        CRITICAL2("Configure: H263 level set to 0x%x (default level)",
                  m_h263LevelMap[VIDEO_H263_LEVEL10]);
      }

      mCodec = AMediaCodec_createEncoderByType(MEDIA_H263_VIDEO_MIME_TYPE);
      break;

    case VIDEO_CODEC_H265:
      AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME,
                             MEDIA_HEVC_VIDEO_MIME_TYPE);

      if (m_codecConfig.eH265ProfileType == VIDEO_H265_PROFILE_MAIN) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_PROFILE, VT_HEVCProfileMain);
        CRITICAL2("Configure: H265 profile set to %d",
                  VT_HEVCProfileMain);
      } else if (m_codecConfig.eH265ProfileType == VIDEO_H265_PROFILE_MAIN10) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_PROFILE, VT_HEVCProfileMain10);
        CRITICAL2("Configure: H265 profile set to 0x%x",
                  VT_HEVCProfileMain10);
      } else {
        ERROR2("Configure: H265 profile %d not supported",
               m_codecConfig.eH265ProfileType);
        ret_val = VIDEO_ERROR_UNKNOWN;
        break;
      }

      if (m_h265LevelMap.find(m_codecConfig.eH265ProfileLevel) !=
          m_h265LevelMap.end()) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_LEVEL,
                              m_h265LevelMap[m_codecConfig.eH265ProfileLevel]);
        CRITICAL2("Configure: H265 level set to 0x%x",
                  m_h265LevelMap[m_codecConfig.eH265ProfileLevel]);
      } else {
        ERROR2("Configure: H265 level not supported %d",
               m_codecConfig.eH265ProfileLevel);
        ret_val = VIDEO_ERROR_UNKNOWN;
        break;
      }

      mCodec = AMediaCodec_createEncoderByType(MEDIA_HEVC_VIDEO_MIME_TYPE);
      break;

    case VIDEO_CODEC_H264:
      AMediaFormat_setString(m_format, AMEDIAFORMAT_KEY_MIME,
                             MEDIA_H264_VIDEO_MIME_TYPE);

      if (m_codecConfig.eH264ProfileType == VIDEO_AVC_PROFILE_BASELINE) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_CONSTRAINED_BASELINE,
                              VT_AVCProfileCBPProfile);
        CRITICAL2("Configure: H264 profile set to 0x%x", VT_AVCProfileBaseline);
      } else {
        ERROR2("Configure: h264 profile %d not supported",
               m_codecConfig.eH264ProfileType);
        ret_val = VIDEO_ERROR_UNKNOWN;
      }

      if (m_h264LevelMap.find(m_codecConfig.eH264ProfileLevel) !=
          m_h264LevelMap.end()) {
        AMediaFormat_setInt32(m_format, MEDIA_KEY_CUSTOM_LEVEL,
                              m_h264LevelMap[m_codecConfig.eH264ProfileLevel]);
        CRITICAL2("Configure: H264 level set to 0x%x",
                  m_h264LevelMap[m_codecConfig.eH264ProfileLevel]);
      } else {
        ERROR2("Configure: H264 profile level %d not supported",
               m_codecConfig.eH264ProfileLevel);
        ret_val = VIDEO_ERROR_UNKNOWN;
        break;
      }

      mCodec = AMediaCodec_createEncoderByType(MEDIA_H264_VIDEO_MIME_TYPE);
      break;

    case VIDEO_CODEC_MPEG4_ISO:
    case VIDEO_CODEC_MPEG4_XVID:
      CRITICAL2("VideoCodecEncoder: ConfigureProfileAndLevel No-op for codec %d",
                m_codecConfig.eCodec);
      break;

    default:
      ERROR2("VideoCodecEncoder::Configure. Codec %d not implemented. please check your code",
             m_codecConfig.eCodec);
      ret_val = VIDEO_ERROR_UNKNOWN;
  }

  if (NULL == mCodec) {
    ERROR2("ConfigureProfileAndLevel: mCodec is NULL for Codec %d",
           m_codecConfig.eCodec);
    ret_val = VIDEO_ERROR_UNKNOWN;
  }

  return ret_val;
}

QPE_VIDEO_ERROR VideoCodecEncoder::SetRotationAngle(uint16_t rotation)
{
#ifdef GPU_ROTATION
  QPE_VIDEO_ERROR result = VIDEO_ERROR_UNKNOWN;
  CRITICAL3("VideoCodecEncoder::SetRotationAngle rotation = %d, rec state %d",
            rotation, iRecorderState);

  do {
    if ((iRecorderState != eVideoStateNotReady)
        && ((m_codecConfig.rotation != rotation) || (!rotation))){
      OMX_EVENTDATA event;
      event.pData = (void *) &rotation;
      event.iLen = sizeof(uint16_t);
      m_pCallback((QPE_VIDEO_MSG)VIDEO_MSG_INTERNAL_EVENT_TRIGGER_RECORDER,
                  (void*) &event, m_eVidDevType, SUCCESS, m_pUserData);
      result = VIDEO_ERROR_OK;
    }
  } while (0);

#else
  CRITICAL1("SetRotationAngle not needed for older Target");
  QPE_VIDEO_ERROR result = VIDEO_ERROR_OK;
#endif
  return result;
}


QPE_VIDEO_ERROR VideoCodecEncoder::AdaptVideoFrameRate(uint32_t iFrameRate)
{
  QPE_VIDEO_ERROR status = VIDEO_ERROR_OK;
  CRITICAL3("AdaptVideoFrameRate Configuring Framerate %d RecorderState %d",
            iFrameRate, iRecorderState);
  //updating frame rate here so that it can be used for
  //periodicity and next codec configuration without reconfiguration
  m_codecConfig.iFrameRate = iFrameRate;

  do {
    if (eVideoStateActive != iRecorderState) {
      CRITICAL2("AdaptVideoFrameRate: Video state is %d which is not active",
                iRecorderState);
      break;
    }

    AMediaFormat *pFrameRateFmt = AMediaFormat_new();

    if (NULL == pFrameRateFmt) {
      ERROR1("AdaptVideoFrameRate local format is null")
      status = VIDEO_ERROR_UNKNOWN;
      break;
    }

    /* Frame rate needs to be set in Q16 format. Left shift data by 16bits */
    AMediaFormat_setInt32(pFrameRateFmt, MEDIA_KEY_VIDEO_FRAMERATE,
                          m_codecConfig.iFrameRate << 16);
    media_status_t result = AMediaCodec_setParameters(mCodec, pFrameRateFmt);

    if (AMEDIA_OK != result) {
      ERROR2("AdaptVideoFrameRate setParameters API failed %d", (int)result);
      status = VIDEO_ERROR_UNKNOWN;
      break;
    }

    AMediaFormat_delete(pFrameRateFmt);
  } while (0);

  //Updating IFrame Interval based on new FrameRate.
  AdaptVideoIdrPeriodicity(m_codecConfig.iFrameInterval);
  return status;
}

QPE_VIDEO_ERROR VideoCodecEncoder::AdaptVideoIdrPeriodicity(int32_t iFrameInt)
{
  QPE_VIDEO_ERROR status = VIDEO_ERROR_OK;
  CRITICAL5("AdaptVideoIdrPeriodicity for FrameRate %d  iFrameInt %d RecorderState %d, ltr %d",
            m_codecConfig.iFrameRate, iFrameInt, iRecorderState, m_bLTRFeatureOn);
  m_codecConfig.iFrameInterval = iFrameInt;
  //calculate nPFrames and IDRPeriod
  int nPFrames = (int) (m_codecConfig.iFrameRate * iFrameInt) - 1;
  int nIDRPeriod = (int) 1;

  /*If all the frames need to be marked as IDR frames*/
  if (0 == iFrameInt) {
    nPFrames = 0;
    nIDRPeriod = 1;
  } else if (m_bLTRFeatureOn || MAX_IDR_INTERVAL == iFrameInt) {
    /* If IDR periodicity is maximum value, then generate Maximum P-frames*/
    /* If LTR is enabled, then set IDR frame interval to Max value*/
    /* Only on-demand IDR frame will be generated in this case. */
    nPFrames = MAX_IDR_PERIOD;
    nIDRPeriod = 0;
  }

  if (eVideoStateActive == iRecorderState) {
    AMediaFormat *IFrameIntervalFormat = AMediaFormat_new();

    if (NULL != IFrameIntervalFormat) {
      AMediaFormat_setInt32(IFrameIntervalFormat, MEDIA_KEY_I_REFRESH_PERIOD_NPFRAMES,
                            nPFrames);
      AMediaFormat_setInt32(IFrameIntervalFormat, MEDIA_KEY_I_REFRESH_N_IDR_PERIOD,
                            nIDRPeriod);
    } else {
      ERROR1("AdaptVideoIdrPeriodicity local format is null")
      return VIDEO_ERROR_UNKNOWN;
    }

    media_status_t result = AMediaCodec_setParameters(mCodec, IFrameIntervalFormat);

    if (AMEDIA_OK != result) {
      ERROR2("AdaptVideoIdrPeriodicity returned status %d", (int)result);
      status = VIDEO_ERROR_UNKNOWN;
    }

    AMediaFormat_delete(IFrameIntervalFormat);
  } else {
    ERROR1(" AdaptVideoIdrPeriodicity RecorderState is not active");
    status = VIDEO_ERROR_UNKNOWN;
  }

  return status;
}
