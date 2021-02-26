/**
============================
Copyright (c)  2016-2017  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================
*/
#ifndef __VIDEOCOMMON_H
#define __VIDEOCOMMON_H


enum eVIDEO_BUFFER_MODE {
  VIDEO_BUFFER_META_MODE = 0,
  VIDEO_BUFFER_NON_META_MODE,
};

/**
* \var typedef eVIDEO_BUFFER_MODE QPE_VIDEO_BUFFER_MODE
* Created a type name for eVIDEO_BUFFER_MODE
*/
typedef enum eVIDEO_BUFFER_MODE QPE_VIDEO_BUFFER_MODE;

typedef enum eCameraHAL {
  CAMERA_HAL1 = 1,
  CAMERA_HAL3 = 3,

} CAMERA_HAL_VERSION;
/**
 * \enum eQPE_VIDEO_MSG
 * \brief Video Messages to Callback function
 *
 * Specifies the various messages passed from video to the callback function
 */
typedef enum eVIDEO_MSG {
  /** Error Indication.*/
  VIDEO_MSG_ERROR = 0,
  /** Recorded data available.*/
  VIDEO_MSG_RECORDED_DATA = 1,
  /** Frame Played.*/
  VIDEO_MSG_DATA_PLAYED = 2,
  /** Resume Playing.*/
  VIDEO_MSG_RESUME_PLAYING = 3,
  /** Recorder Started. */
  VIDEO_MSG_RECORDER_STARTED = 4,
  /** Recorder Stopped. */
  VIDEO_MSG_RECORDER_STOPPED = 5,
  /** Player Started */
  VIDEO_MSG_PLAYER_STARTED = 6,
  /** Player Stopped */
  VIDEO_MSG_PLAYER_STOPPED = 7,
  /** Device is Initialised */
  VIDEO_MSG_DEV_INITIALISED = 8,
  /** Device is uninitialised */
  VIDEO_MSG_DEV_UNINITIALISED = 9,
  /** Resource for Saving locally is acquired */
  VIDEO_MSG_SAVING_RES_ACQUIRED = 10,
  /** Resource for Saving locally is released */
  VIDEO_MSG_SAVING_RES_RELEASED = 11,
  /** Saving started */
  VIDEO_MSG_SAVING_STARTED = 12,
  /** Saving stopped by user */
  VIDEO_MSG_SAVING_STOPPED_BY_USER = 13,
  /** Saving stopped due to storage area full */
  VIDEO_MSG_SAVING_STOPPED_STORAGE_FULL = 14,
  /** Saving stopped due to duration specified reached */
  VIDEO_MSG_SAVING_STOPPED_DURATION_REACHED = 15,
  /** Saving Error */
  VIDEO_MSG_SAVING_ERROR = 16,
  /** Decoded Far video frame */
  VIDEO_MSG_FAR_FRAME = 17,
  /** Decoded Near(Preview) video frame */
  VIDEO_MSG_NEAR_FRAME = 18,
  /** Video Codec changed */
  VIDEO_MSG_CODEC_CHANGED = 19,
  /** Player Reconfiguring ****/
  VIDEO_MSG_PLAYER_RECONFIGURE = 20,
  /**remove the video components */
  VIDEO_MSG_REMOVE_VIDEO_COMPONENTS,
  /**Internal video state change event */
  VIDEO_MSG_INTERNAL_VIDEO_STATE_CHANGE,
  /** Player Initialise Error */
  VIDEO_MSG_CONFIGURE_ERROR,
  /**Internal video event */
  VIDEO_MSG_INTERNAL_EVENT_TRIGGER_RECORDER_HAL1 = 0xFD,
  /**Internal video event */
  VIDEO_MSG_INTERNAL_EVENT_TRIGGER_PLAYER = 0xFE,
  /**Internal video event */
  VIDEO_MSG_INTERNAL_EVENT_TRIGGER_RECORDER = 0xFF,
} QPE_VIDEO_MSG;

/**
 * \var typedef stMediaPacketInfo MEDIA_PACKET_INFO;
 * Created a type name for stMediaPacketInfo
 */
/*typedef stMediaPacketInfo MEDIA_PACKET_INFO;*/
/**
 * \struct stMediaPacketInfo
 * Holds RTP Data
 *
 * Structure specifying RTP data to be played
 */
typedef struct stMediaPacketInfoRx {
  uint32_t iTimeStamp; /*RTP timestamp received*/
  int64_t iRecvTime; /*IMS_AVPF_RA*/
  uint32_t SSRC;
  uint32_t iSeqNum; /*RTP sequence number*/
  int bMarkerBit;
  int bSilence; /* For audio only: indicates if the pkt is a silence pkt*/
  int8_t iPayloadType; /*RTP Payload type as received from lower layer*/
  uint32_t source_time_video_frame;
  uint32_t source_rtp_timestamp;
  uint8_t rtp_pkt_type; /*information of type of packet*/
} MEDIA_PACKET_INFO_RX;

typedef struct stMediaPacketInfoTx {
  uint64_t iTimeStamp; /*Frame time stamp*/
  uint64_t appTimeStamp; /*apps time stamp*/
  int bMarkerBit;
  uint32_t ulLtrExtraData; /* LTR Extra Data */
  uint64_t ullSysVidFrameOPts;
} MEDIA_PACKET_INFO_TX;

typedef union MediaPacketInfo {
  /*This structure will be filled by dpl*/
  MEDIA_PACKET_INFO_TX sMediaPktInfoTx;
  /*This structure will be filled by core rtp*/
  MEDIA_PACKET_INFO_RX sMediaPktInfoRx;
} MEDIA_PACKET_INFO;

typedef struct {
  uint8_t index;
  int belongs_to_drop_set;
  uint32_t concealmb;
} FillBufferInfo;


typedef enum {
  eSurfaceStateNotReady,
  eSurfaceStateReady
} eSurfaceState;

typedef enum ePlayerMsgType {
  FILLBUFFER_INFO   = 0,
  RESOLUTION_UPDATE = 1,
  MOVE_TO_LOADED    = 2,
  MOVE_TO_EXECUTING = 3,
  PORT_DISABLE      = 4,
  PORT_ENABLE       = 5,
  CROP              = 6,
  CODEC_CONFIG_REQ  = 7,
} PlayerMsgType;

typedef enum eRecorderMsgType {
  ROTATION_CROP_PARAM_NOGPU  = 1,
  ROTATION_CROP_PARAM_GPU = 2,
  STOP_RECORDING  = 3,
  WAIT_FOR_BUFFERS = 4,
} RecorderMsgType;


/* This is the state machine to handle multiple requests */
typedef enum {
  VIDEO_INIT = 0,
  VIDEO_CODEC_PENDING,
  VIDEO_SURFACE_PENDING,
  VIDEO_SURFACE_RECEIVED,
  VIDEO_CODEC_CONFIGURED,
} eVIDEO_PLAYER_STATE;

typedef struct {
  void *pData;
  uint iLen;
  //QPE_VIDEO_DEV eDev;
} OMX_EVENTDATA;

typedef struct ePlayerResolution {
  uint32_t height;
  uint32_t width;
} PlayerResolution;


typedef struct PlayerMsg {
  PlayerMsgType type;
  union  {
    /* If this flag is set to true, player start command also triggered
       after codec Config request. */
    uint8_t           bPlayerStart;
    FillBufferInfo    fillbuffer;
    PlayerResolution  resolution;
  } data;
} PlayerInternalMsg;

typedef struct RecorderMsg {
  RecorderMsgType type;
  int *rotation;
  int *cameraFacing;
  int *cameraConfigHeight;
  int *cameraConfigWidth;
  int *cameraMount;
  int *rotationOffset;
} RecorderInternalMsg;


#define VIDEOOMX_MAX_NAL_LENGTH 100
#define VIDEOOMX_MAX_VOL_LENGTH 100
//MP4 VOL information
typedef struct {
  uint16_t volLength;
  uint8_t rawVol[VIDEOOMX_MAX_VOL_LENGTH];
} VIDEOOMXMP4VOLInfo;

//H264 NAL information
typedef struct {
  uint32_t nalLength;
  uint8_t rawnal[VIDEOOMX_MAX_NAL_LENGTH];
} VIDEOOMXH264NalInfo;

/**
 * \struct stAMRInfoRx
 * Holds AMR specific info coming from RTP layer
 *
 * Structure specifying additional AMR audio frame information needed to correctly decode the AMR data
 */
typedef struct stAMRInfoRx {
  uint8_t iFrameType;
  uint32_t iFrameCRC; /* 32 bit CRC value of the frame. */
  int bAmrFqi; /* Quality of the frame received. This is currently */
  uint8_t iAMRModeRequest; /* AMR mode of operation */
} AMR_INFO;

typedef union {
  AMR_INFO sAMRInfo;
} QP_FRAME_INFO;


/**
 * \struct QpCodecFrame
 * Audio frame parameters
 *
 * Structure specifying the data regarding encoded frames to the application.
 */
typedef struct QpMultimediaFrame {
  /** Pointer to the encoded data or the codec frame. */
  uint8_t *pData;
  /**Cvo Information**/
  uint8_t cvoInfo;
  /**pkt count**/
  int8_t pktCnt;
  int partialDequeue; /*This is to indicate reassembly dint find full frame*/
  /** Length of the encoded data or the codec frame. */
  uint32_t iDataLen;
  /** Length of the allocated data buffer. */
  uint32_t iMaxBuffLen;
  /** MEDIA_PACKET_INFO structure. */
  MEDIA_PACKET_INFO sMediaPacketInfo;
  /** Additional codec speficic frame data*/
  QP_FRAME_INFO sFrameInfo;
} QP_MULTIMEDIA_FRAME;


//define component names
#ifdef MSM8937
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263sw\0"
#else
#ifndef MSM8909
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263\0"
#else
#define H263_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.h263sw\0"
#endif
#endif

#define MPEG4_VIDEO_ENCODER (char*)"OMX.qcom.video.encoder.mpeg4\0"
#define H264_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.avc\0"
#define H265_VIDEO_ENCODER    (char*)"OMX.qcom.video.encoder.hevc\0"
#define DIR_INP 0
#define DIR_OUT 1
#define MAX_BUFFERS 50
/**
 * \def QP_TRUE
 * macro for TRUE value
 *
 * macro for TRUE value
 */
#define QP_TRUE  1L
/**
 * \def QP_FALSE
 * macro for FALSE value
 *
 * macro for FALSE value
 */
#define QP_FALSE 0L
/**
 * \struct QpLipSyncParam
 * Lip Sync Params
 *
 */
struct QpLipSyncParam {
  uint32_t iAudioClockRate;
  uint8_t iAudPktInterval;
};
/**
 * \var typedef QpLipSyncParam QP_LIPSYNC_PARAM;
 * Creates a type name for struct QpLipSyncParam.
 */
typedef struct QpLipSyncParam QP_LIPSYNC_PARAM;
typedef void *QP_VIDEO_DESC;
typedef void (*QP_VIDEO_CALLBACK)(QPE_VIDEO_MSG tVideoMsg, void *pFrameData,
                                  uint32_t eDevType, int iStatus,
                                  void *pUserData); //temporary callback for video initialize

/**
 * \enum eHEVC_PROFILE_TYPE
 * \brief Profile type
 *
 * Specifies the profile type of H264 codec
 */

enum eH265_PROFILE_TYPE {
  VIDEO_H265_PROFILE_MAIN = 0x00,
  VIDEO_H265_PROFILE_MAIN10 = 0x01
};

/**
 * \var typedef eHEVC_PROFILE_TYPE QPE_VIDEO_HEVC_PROFILE_TYPE;
 * Created a type name for eHEVC_PROFILE_TYPE
 */
typedef enum eH265_PROFILE_TYPE QPE_VIDEO_H265_PROFILE_TYPE;

/**
 * \enum eAVC_PROFILE_LEVEL
 * \brief Profile level
 *
 * Specifies the profile level of H264 codec
 */

enum eH265_PROFILE_LEVEL {
  VIDEO_H265_LEVEL1 = 0x00,
  VIDEO_H265_LEVEL2 = 0x01,
  VIDEO_H265_LEVEL21 = 0x02,
  VIDEO_H265_LEVEL3 = 0x03,
  VIDEO_H265_LEVEL31 = 0x04,
  VIDEO_H265_LEVEL4 = 0x05,
  VIDEO_H265_LEVEL41 = 0x06,
  VIDEO_H265_LEVEL5 = 0x07,
  VIDEO_H265_LEVEL51 = 0x08,
  VIDEO_H265_LEVEL52 = 0x09,
  VIDEO_H265_LEVEL6 = 0x0A,
  VIDEO_H265_LEVEL61 = 0x0B,
  VIDEO_H265_LEVEL62 = 0x0C,
};

/**
 * \var typedef eAVC_PROFILE_LEVEL QPE_VIDEO_HEVC_PROFILE_LEVEL;
 * Created a type name for eHEVC_PROFILE_LEVEL
 */
typedef enum eH265_PROFILE_LEVEL QPE_VIDEO_H265_PROFILE_LEVEL;

/**
 * \enum eAVC_PROFILE_TYPE
 * \brief Profile type
 *
 * Specifies the profile type of H264 codec
 */

enum eAVC_PROFILE_TYPE {
  VIDEO_AVC_PROFILE_BASELINE = 0x00,
  VIDEO_AVC_PROFILE_MAIN = 0x01,
  VIDEO_AVC_PROFILE_EXTENDED = 0x02,
  VIDEO_AVC_PROFILE_HIGH = 0x03,
};

/**
 * \var typedef eAVC_PROFILE_TYPE QPE_VIDEO_AVC_PROFILE_TYPE;
 * Created a type name for eAVC_PROFILE_TYPE
 */
typedef enum eAVC_PROFILE_TYPE QPE_VIDEO_AVC_PROFILE_TYPE;

/**
 * \enum eAVC_PROFILE_LEVEL
 * \brief Profile level
 *
 * Specifies the profile level of H264 codec
 */

enum eAVC_PROFILE_LEVEL {
  VIDEO_AVC_LEVEL1 = 0x00,
  VIDEO_AVC_LEVEL1B = 0x01,
  VIDEO_AVC_LEVEL11 = 0x02,
  VIDEO_AVC_LEVEL12 = 0x03,
  VIDEO_AVC_LEVEL13 = 0x04,
  VIDEO_AVC_LEVEL2 = 0x05,
  VIDEO_AVC_LEVEL21 = 0x06,
  VIDEO_AVC_LEVEL22 = 0x07,
  VIDEO_AVC_LEVEL3 = 0x08,
  VIDEO_AVC_LEVEL31 = 0x09,
  VIDEO_AVC_LEVEL32 = 0x0A,
  VIDEO_AVC_LEVEL4 = 0x0B,
  VIDEO_AVC_LEVEL41 = 0x0C,
  VIDEO_AVC_LEVEL42 = 0x0D,
  VIDEO_AVC_LEVEL5 = 0x0E,
  VIDEO_AVC_LEVEL51 = 0x0F,
};

/**
 * \var typedef eAVC_PROFILE_LEVEL QPE_VIDEO_AVC_PROFILE_LEVEL;
 * Created a type name for eAVC_PROFILE_LEVEL
 */
typedef enum eAVC_PROFILE_LEVEL QPE_VIDEO_AVC_PROFILE_LEVEL;


/**
 * \enum eH263_PROFILE_TYPE
 * \brief Profile type
 *
 * Specifies the profile type of H263 codec
 */

enum eH263_PROFILE_TYPE {
  VIDEO_H263_PROFILE_BASELINE = 0x00,
  VIDEO_H263_PROFILE_H320_CODING = 0x01,
  VIDEO_H263_PROFILE_BACKWARD_COMPATIBLE = 0x02,
  VIDEO_H263_PROFILE_ISWV2 = 0x03,
  VIDEO_H263_PROFILE_ISWV3 = 0x04,
  VIDEO_H263_PROFILE_HIGH_COMPRESSION = 0x05,
  VIDEO_H263_PROFILE_INTERNET = 0x06,
  VIDEO_H263_PROFILE_INTERLACE = 0x07,
  VIDEO_H263_PROFILE_HIGH_LATENCY = 0x08,
};

/**
 * \var typedef eH263_PROFILE_TYPE QPE_VIDEO_H263_PROFILE_TYPE;
 * Created a type name for eH263_PROFILE_TYPE
 */
typedef enum eH263_PROFILE_TYPE QPE_VIDEO_H263_PROFILE_TYPE;

/**
 * \enum eH263_PROFILE_LEVEL
 * \brief Profile level
 *
 * Specifies the profile level of H263 codec
 */

enum eH263_PROFILE_LEVEL {
  VIDEO_H263_LEVEL10 = 0x00,
  VIDEO_H263_LEVEL20 = 0x01,
  VIDEO_H263_LEVEL30 = 0x02,
  VIDEO_H263_LEVEL40 = 0x03,
  VIDEO_H263_LEVEL50 = 0x04,
  VIDEO_H263_LEVEL60 = 0x05,
  VIDEO_H263_LEVEL70 = 0x06,
  VIDEO_H263_LEVEL45 = 0x07,
};

/**
 * \var typedef eH263_PROFILE_LEVEL QPE_VIDEO_H263_PROFILE_LEVEL;
 * Created a type name for eH263_PROFILE_LEVEL
 */
typedef enum eH263_PROFILE_LEVEL QPE_VIDEO_H263_PROFILE_LEVEL;
/**
 * \var typedef eVIDEO_CODEC QPE_VIDEO_CODEC;
 * Created a type name for eVIDEO_CODEC
 */
//typedef enum eVIDEO_CODEC QPE_VIDEO_CODEC;


enum eVIDEO_H264_PKTN_MODE {
  /** H.264 Single NAL unit mode */
  VIDEO_CODEC_H264_SINGLE_NALU = 0,
  /** H.264 Non-interleaved mode */
  VIDEO_CODEC_H264_NON_INTERLEAVED = 1,
  /** H.264 Interleaved mode */
  VIDEO_CODEC_H264_INTERLEAVED = 2
};

/**
 * \var typedef eVIDEO_H264_PKTN_MODE QPE_VIDEO_H264_PKTN_MODE;
 * Created a type name for eVIDEO_H264_PKTN_MODE
 */

typedef enum eVIDEO_H264_PKTN_MODE QPE_VIDEO_H264_PKTN_MODE;


/*typedef enum VideoCodecResponseTypes{
  QC_QMI_RESPONSE_SUCCESS,
  QC_QMI_RESPONSE_FAILURE
} QCE_QMI_RESPONSE_TYPES;
*/

/**
 * \enum eVIDEO_CODEC
 * \brief Video Codecs
 *
 * Specifies the various video codecs available.
 */
enum eVIDEO_CODEC {
  /** XVID MPEG4 Codec.*/
  VIDEO_CODEC_MPEG4_XVID,
  /** ISO MPEG4 Codec.*/
  VIDEO_CODEC_MPEG4_ISO,
  /** H.263 Codec.*/
  VIDEO_CODEC_H263,
  /** H.264 Codec.*/
  VIDEO_CODEC_H264,
  /** H.265 Codec */
  VIDEO_CODEC_H265

};
typedef enum eVIDEO_CODEC QPE_VIDEO_CODEC;

enum eVIDEO_DEV {
  /** PLAYER */
  VIDEO_PLAYER,
  /** RECORDER */
  VIDEO_RECORDER
};

/**
 * \var typedef eVIDEO_DEV QPE_VIDEO_DEV;
 * Created a type name for eVIDEO_DEV
 */
typedef enum eVIDEO_DEV QPE_VIDEO_DEV;

/* Client can provide bitrate config mode. These values will map to different
   ENUMs in Video RC algorithm. Comment on each Enum will indicate that.
   For VT usecases, VBR_CFR or CBR_VFR are recommended. By default CBR_VFR is
   choosen for VT usecase. */
typedef enum {
  BITRATE_UNKNOWN = 0,
  /*V4L2_CID_MPEG_VIDC_VIDEO_RATE_CONTROL_VBR_CFR = 2*/
  BITRATE_MODE_VBR_CFR = 1,
  /*V4L2_CID_MPEG_VIDC_VIDEO_RATE_CONTROL_CBR_CFR = 4*/
  BITRATE_MODE_CBR_CFR = 2,
  /*V4L2_CID_MPEG_VIDC_VIDEO_RATE_CONTROL_VBR_VFR = 1*/
  BITRATE_MODE_VBR_VFR = 3,
  /*V4L2_CID_MPEG_VIDC_VIDEO_RATE_CONTROL_CBR_VFR = 3*/
  BITRATE_MODE_CBR_VFR = 4,
} BIT_RATE_MODE;

/**
 * \struct QpVideoConfig
 * Video configuration parameters
 *
 * Structure specifying the various mandatory video configuration parameters
 */
struct QpVideoConfig {
  /**device to be uased */
  QPE_VIDEO_DEV eVideoDevice;
  /** Codec that needs to be used.*/
  QPE_VIDEO_CODEC eCodec;
  /** Width of the video capture */
  int32_t iWidth;
  /** Height of the video capture */
  int32_t iHeight;
  /** Output BitRate expected from the encoder in kbps */
  int32_t iBitRate;
  /** Frame Rate of the capture in frames/sec */
  int32_t iFrameRate;
  /** Buffer containing VOL header */
  char *pVolHeader;
  /** Length of the VOL header */
  uint16_t iVolHeaderLen;
  /** Buffer containing NAL header */
  char *pNALHeader;
  /** Length of the NAL header */
  uint16_t iNALHeaderLen;
  /** Sampling rate */
  uint32_t iClockRate;
  /** Enable/ Disable Lip synchronization*/
  int bLipSyncEnable;
  /** Lip Sync Parameters */
  QP_LIPSYNC_PARAM     sLipSyncParam;
  /** H264 Profile type */
  QPE_VIDEO_AVC_PROFILE_TYPE eH264ProfileType;
  /** H264 Profile level */
  QPE_VIDEO_AVC_PROFILE_LEVEL eH264ProfileLevel;
  /** H263 Profile type */
  QPE_VIDEO_H263_PROFILE_TYPE eH263ProfileType;
  /** H263 Profile level */
  QPE_VIDEO_H263_PROFILE_LEVEL eH263ProfileLevel;

  /** H264 Packetization Mode type */
  QPE_VIDEO_H264_PKTN_MODE ePktnMode;

  /** H265 Profile type */
  QPE_VIDEO_H265_PROFILE_TYPE eH265ProfileType;
  /** H265 Profile level */
  QPE_VIDEO_H265_PROFILE_LEVEL eH265ProfileLevel;

  /** RTP MTU Size */
  uint16_t iRtpMTUSize;
  /* iFrameInterval based on AVP or AVPF*/
  int32_t  iFrameInterval;
  /** Width of the video capture */
  int32_t iOut_Width;
  /** Height of the video capture */
  int32_t iOut_Height;
  /** internal codec configure  */
  int iInternalCodecConfig;
  /**Av sync threshold*/
  uint32_t iAv_sync_threshold;
  /**rotation parameter */
  uint16_t rotation;
  /**GPU Rotation width*/
  uint32_t cameraConfigWidth;
  /**GPU Rotation height*/
  uint32_t cameraConfigHeight;
  uint8_t useSystemTime;
  uint8_t iErrorState;
  uint16_t iEnableFileStreaming;
  uint16_t iDisplayMode;
  /**Disable Drop set flag*/
  uint8_t iDisable_drop_set;
  int iCropSet;
  /**Camera HAL Version */
  CAMERA_HAL_VERSION eHALVersion;
  int iPortReconfigSet;
  /**mode parameter*/
  QPE_VIDEO_BUFFER_MODE iInputVideoBufferMode;
  /** LTR feature flag info */
  uint8_t iLTRSupportEnabled;
  /** rate control Mode */
  BIT_RATE_MODE eBitRateMode;
  /** Session Id */
  uint16_t iSessionId;
};
/**
 * \var typedef QpVideoConfig QP_VIDEO_CONFIG;
 * Creates a type name for struct QpVideoConfig.
 */
typedef struct QpVideoConfig QP_VIDEO_CONFIG;

enum eVideoState {
  eVideoStateNotReady, //fresh initialized of codec creation
  eVideoStateAcquiring,
  eVideoStateIdle, //idle means configured state and surface is configured for Recorder
  eVideoStateStarting,
  eVideoStateActive, //executing or started state
  eVideoStateStopping,
  eVideoStateReleasing,
  eVideoStateReConfiguring,
  eVideoStateReConfigureDone,
  eVideoStatePortDisabling,
  eVideoStatePortDisabled,
  eVideoStatePaused,
  eVideoStateCropping,
  eVideoStateSurfaceReleased,
  eVideoStateSurfaceReleasing
};

typedef enum eVIDEO_ERROR {
  /** No Error OR Success */
  VIDEO_ERROR_OK = 0,
  /** Unknown Error */
  VIDEO_ERROR_UNKNOWN = -1,
  /** Recorder could not be started */
  VIDEO_ERROR_RECORDER_DOWN = 1,
  /** Player could not be started */
  VIDEO_ERROR_PLAYER_DOWN = 2,
  /** Buffer overflow in Recorder or Player */
  VIDEO_ERROR_BUFFER_OVERFLOW = 3,
  /** Buffer underflow in Recorder or Player */
  VIDEO_ERROR_BUFFER_UNDERFLOW = 4,
  /** Recorder busy means recorder has already been started */
  VIDEO_ERROR_RECORDER_BUSY = 5,
  /** Player busy means player has already been started */
  VIDEO_ERROR_PLAYER_BUSY = 6
} QPE_VIDEO_ERROR;


typedef enum {
  CVO_ZERO_ROTATION = 0x00,
  CVO_90_ROTATION = 0x01,
  CVO_180_ROTATION = 0x02,
  CVO_270_ROTATION = 0x03,

  CVO_FLIP_DIABLED = 0x00,
  CVO_FLIP_ENABLED = 0x04,
  CVO_BACK_CAMERA  = 0x08,
  CVO_INVALID_TYPE = 0xFF
} CVO_ROTATION_TYPE;

typedef enum {
  SUCCESS = 0,
  FAILURE = 1
} CALLBACK_STATUS;

//void videoCallback(QPE_VIDEO_MSG tVideoMsg, void *pFrameData, uint32_t eDevType, int iStatus,
//                 void *pUserData);
//typedef void (*DATA_CODEC_CALLBACK)(QPE_VIDEO_MSG msg, void *);

#endif
