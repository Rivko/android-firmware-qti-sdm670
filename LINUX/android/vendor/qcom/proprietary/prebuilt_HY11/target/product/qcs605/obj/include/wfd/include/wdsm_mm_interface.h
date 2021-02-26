#ifndef __WDSM_MM_INTERFACE_H__
#define __WDSM_MM_INTERFACE_H__

/*==============================================================================
*  wdsm_mm_interface.h
*
*  DESCRIPTION:
*  Definition of the wireless display session manager MM inteface
*
*
*  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
================================================================================
*/

/*==============================================================================
                             Edit History
================================================================================
*/

/*==============================================================================
**               Includes and Public Data Declarations
**==============================================================================
*/
#include "AEEStdDef.h"
#include "AEEstd.h"

enum NetType : int;

typedef void* WFD_MM_HANDLE;

#define WFD_SINK_HDCP_PORT 9876
#define WFD_SINK_RTP_PORT 1234

#define WFD_MAX_NUM_VIDEO_CODEC 20


/* WFD run time commands */
/* it corrosponds to RuntimecmdType in wfdenums */
typedef enum
{
    WFD_MM_CMD_INVALID = -1,
    WFD_MM_CMD_OPEN_AUDIO_PROXY = 0,
    WFD_MM_CMD_CLOSE_AUDIO_PROXY,
    WFD_MM_CMD_ENABLE_BITRATE_ADAPT,
    WFD_MM_CMD_DISABLE_BITRATE_ADAPT,
    WFD_MM_CMD_BLANK_REMOTE_DISPLAY,
    WFD_MM_CMD_ENABLE_STREAMING_FEATURE,
    WFD_MM_CMD_DISABLE_STREAMING_FEATURE,
    WFD_MM_CMD_DISABLE_AUDIO,
    WFD_MM_CMD_ENABLE_AUDIO,
    WFD_MM_CMD_DISABLE_VIDEO,
    WFD_MM_CMD_ENABLE_VIDEO,
}WFD_runtime_cmd_t;

/* WFD device type */
typedef enum {
    WFD_DEVICE_UNK,                          /* unknown device type */
    WFD_DEVICE_SOURCE,                       /* WFD source  */
    WFD_DEVICE_PRIMARY_SINK,                 /* WFD primary sink  */
    WFD_DEVICE_SECONDARY_SINK,              /* WFD secondary sink  */
    WFD_DEVICE_SOURCE_PRIMARY_SINK,         /* WFD source / primary sink  */
    WFD_DEVICE_INVALID                         /* invalid device type */
} WFD_device_t;


typedef enum {
    WFD_VIDEO_CEA_MODE = 0,
    WFD_VIDEO_VESA_MODE = 1,
    WFD_VIDEO_HH_MODE = 2,
    WFD_VIDEO_NUM_MODES = 3
}WFD_Video_modes;

#if 0
/*==============================================================================
** H264 codec configuration data structure
**==============================================================================
*/
typedef struct
{
    uint8        h264_profile;
    uint8        h264_level;
    uint16       max_hres;
    uint16       max_vres;
    uint32       supported_cea_mode;
    uint32       supported_vesa_mode;
    uint32       supported_hh_mode;
    uint8        decoder_latency;
    uint16       min_slice_size;
    uint8        slice_enc_params;
    uint8        frame_rate_control_support;
} WFD_h264_codec_capability_t;
#endif

#define MAX_VIDEO_MODE_BITS 48
#define MAX_AUDIO_MODE_BITS 8

typedef struct
{
    uint32      width;
    uint32      height;
}WFD_mode_res_t;

/* This enumerated type lists the different types of video. */
typedef enum {
    WFD_VIDEO_UNK,      /* unknown video codec type, must be fist*/
    WFD_VIDEO_H264,     /* H264 video  */
    WFD_VIDEO_H265,     /* H265 video  */
    WFD_VIDEO_MPEG4,    /* Mpeg4 video */
    WFD_VIDEO_VP8,      /* VP8 video   */
    WFD_VIDEO_VP9,      /* VP9 video   */
    WFD_VIDEO_JPEG,     /* JPEG video  */
    WFD_VIDEO_PNG,      /* PNG video   */
    WFD_VIDEO_3D,       /* 3D video    */
    WFD_VIDEO_INVALID   /* invalid video type, must be last*/
} WFD_video_type;

typedef struct WFD_video_codec_capability
{
#ifdef __cplusplus
    WFD_video_codec_capability();
    void dump();
    WFD_video_type getVideoMethod() const;
#endif //__cplusplus
    char            name[6];
    uint8           profile; // sequence number, not bitmap
    uint8           level;
    uint16          max_hres;
    uint16          max_vres;
    uint32          supported_cea_mode;
    WFD_mode_res_t  cea_mode_list[MAX_VIDEO_MODE_BITS];
    uint64          supported_vesa_mode;
    WFD_mode_res_t  vesa_mode_list[MAX_VIDEO_MODE_BITS];
    uint32          supported_hh_mode;
    WFD_mode_res_t  hh_mode_list[MAX_VIDEO_MODE_BITS];
    uint8           decoder_latency;
    uint16          min_slice_size;
    uint8           slice_enc_params;
    uint8           frame_rate_control_support;
    WFD_mode_res_t  max_mode_list[WFD_VIDEO_NUM_MODES];
    bool            transcoding_mode;
    bool            non_transcoding_mode;
}WFD_video_codec_capability_t;

typedef struct
{
    uint8        h264_profile;
    uint8        h264_level;
    uint16       max_hres;
    uint16       max_vres;
    uint32       supported_modes_bitmap;
    uint8        decoder_latency;
    uint16       min_slice_size;
    uint8        slice_enc_params;
    uint8        frame_rate_control_support;
} WFD_3d_h264_codec_config_t;

typedef struct
{
  uint8       numBlocks;
  uint8      *pEdidData;
}WFD_edid_info_type;

typedef struct
{
    uint64  udp_buffer_size;
    uint64  tcp_buffer_size;
}WFD_buffering_delay_type;

#define BIT0       0x00000001
#define BIT1       0x00000002
#define BIT2       0x00000004
#define BIT3       0x00000008
#define BIT4       0x00000010
#define BIT5       0x00000020
#define BIT6       0x00000040
#define BIT7       0x00000080
#define BIT8       0x00000100
#define BIT9       0x00000200
#define BIT10      0x00000400
#define BIT11      0x00000800
#define BIT12      0x00001000
#define BIT13      0x00002000
#define BIT14      0x00004000
#define BIT15      0x00008000
#define BIT16      0x00010000
#define BIT17      0x00020000
#define BIT18      0x00040000
#define BIT19      0x00080000
#define BIT20      0x00100000
#define BIT21      0x00200000
#define BIT22      0x00400000
#define BIT23      0x00800000
#define BIT24      0x01000000
#define BIT25      0x02000000
#define BIT26      0x04000000
#define BIT27      0x08000000
#define BIT28      0x10000000
#define BIT29      0x20000000
#define BIT30      0x40000000
#define BIT31      0x80000000
#define BIT32     0x100000000
#define BIT33     0x200000000
#define BIT34     0x400000000

/*HDCP Capability versions*/
#define WFD_HDCP_2_0 BIT0
#define WFD_HDCP_2_1 BIT1

/* CEA Resolution/Refresh Rate */
#define        H264_MANDATORY_CEA_640x480p60         BIT0

/* Display Native Resolution */
#define        H264_DISPLAY_NATIVE_SELECTION_CEA   0x00
#define        H264_DISPLAY_NATIVE_SELECTION_VESA  0x01
#define        H264_DISPLAY_NATIVE_SELECTION_HH     0x02


/* 3D Video modes */
#define        THREED_1920x540PLUS540_p24_TBH           BIT0
#define        THREED_1280x360PLUS360_p60_TBH           BIT1
#define        THREED_1280x360PLUS360_p50_TBH           BIT2
#define        THREED_1920x1080_p24x2_FS                BIT3
#define        THREED_1280x720_p60x2_FS                 BIT4
#define        THREED_1280x72_p30x2_FS                  BIT5
#define        THREED_1280x72_p50x2_FS                  BIT6
#define        THREED_1280x72_p25x2_FS                  BIT7
#define        THREED_1920x1080PLUS45PLUS1080_p24_FP    BIT8
#define        THREED_1280x720PLUS30PLUS720_p60_FP      BIT9
#define        THREED_1280x720PLUS30PLUS720_p30_FP      BIT10
#define        THREED_1280x720PLUS30PLUS720_p50_FP      BIT11
#define        THREED_1280x720PLUS30PLUS720_p25_FP      BIT12
#define        THREED_960PLUS960x1080_i60_SSH           BIT13
#define        THREED_960PLUS960x1080_i50_SSH           BIT14


/*==============================================================================
** LPCM codec data structure
**==============================================================================
*/

typedef struct
{
    uint32        supported_modes_bitmap;
    uint8       decoder_latency;
} WFD_lpcm_codec_config_t;

//#define        LPCM_(SamplingFrequency)_(BitWidth)_(channels)
#define        LPCM_441_16_2            BIT0
#define        LPCM_48_16_2             BIT1
#define        LPCM_48_16_4             BIT2
#define        LPCM_48_16_6             BIT3
#define        LPCM_48_16_8             BIT4
#define        LPCM_48_20_24_2          BIT5
#define        LPCM_48_20_24_4          BIT6
#define        LPCM_48_20_24_6          BIT7
#define        LPCM_48_20_24_8          BIT8
#define        LPCM_96_16_2             BIT9
#define        LPCM_96_16_4             BIT10
#define        LPCM_96_16_6             BIT11
#define        LPCM_96_16_8             BIT12
#define        LPCM_96_20_24_2          BIT13
#define        LPCM_96_20_24_4          BIT14
#define        LPCM_96_20_24_6          BIT15
#define        LPCM_96_20_24_8          BIT16
#define        LPCM_192_16_2            BIT17
#define        LPCM_192_16_4            BIT18
#define        LPCM_192_16_6            BIT19
#define        LPCM_192_20_24_2         BIT20
#define        LPCM_192_20_24_4         BIT21
#define        LPCM_192_20_24_6         BIT22
#define        LPCM_20_24_OPTION_MASK   BIT31

/*==============================================================================
** Video frame rate change - Support for frame skipping
**==============================================================================
*/

#define WFD_FRAME_RATE_CHANGE_UNSUPPORTED 0
#define WFD_FRAME_RATE_CHANGE_SUPPORTED BIT0

/*==============================================================================
** AAC codec data structure
**==============================================================================
*/

typedef struct
{
    uint16        supported_modes_bitmap;
    uint8        decoder_latency;
} WFD_aac_codec_config_t;

//#define        AAC_(SamplingFrequency)_(BitWidth)_(channels)
#define        AAC_48_16_2            BIT0
#define        AAC_48_16_4            BIT1
#define        AAC_48_16_6            BIT2
#define        AAC_48_16_8            BIT3


/*==============================================================================
** Dolby Digital (AC3) codec data structure
**==============================================================================
*/

typedef struct
{
    uint32        supported_modes_bitmap;
    uint8        decoder_latency;
} WFD_dolby_digital_codec_config_t;

//#define        DOLBY_DIGITAL_(SamplingFrequency)_(BitWidth)_(channels)_(option)
#define        DOLBY_DIGITAL_48_16_2_AC3    BIT0
#define        DOLBY_DIGITAL_48_16_4_AC3    BIT1
#define        DOLBY_DIGITAL_48_16_6_AC3    BIT2
#define        DOLBY_DIGITAL_48_16_8_EAC3   BIT3


/*==============================================================================
** Content Protection Capability data structure
**==============================================================================
*/

typedef struct
{
    uint8        content_protection_capability;
    uint16        content_protection_ake_port;
} WFD_content_protection_capability_config_t;

/* This enumeration type list the different types of RTP port */
typedef enum {
    RTP_PORT_UDP = 0,   /* RTP port type UDP */
    RTP_PORT_TCP       /* RTP port type TCP */
}WFD_rtp_port_type;

/*==============================================================================
**  Transport Capability data structure
**==============================================================================
*/
typedef struct
{
    uint8               transport_capability;
    int                 rtpSock;
    int                 rtcpSock;
    uint16              port1_id;
    uint16              port1_rtcp_id;
    uint16              port2_id;
    WFD_rtp_port_type   eRtpPortType;
    WFD_rtp_port_type   eBeforeDSPortType;
} WFD_transport_capability_config_t;


/*==============================================================================
** IP address of peer device(s)  ---the transport subelement has only port info,
**                          so we need this to hold IP address of peer device(s)
**==============================================================================
*/
typedef struct
{
    uint32            ipv4_addr1;
    uint32            ipv4_addr2;
    uint8             device_addr1[24]; //mac address terminated with 0
    uint8             device_addr2[24]; //mac address terminated with 0
} WFD_ipv4_addr_t;


/*==============================================================================
** QoS setting for RTP connection(s) ---  the QoS setting includes DSCP vlaues,
**                         which will be put in IP header of RTP connection(s)
**==============================================================================
*/
typedef struct
{
    uint8            qos_dscp1;
    uint8            qos_dscp2;
} WFD_rtp_qos_t;

#define WFD_H264_PROFILE_CONSTRAINED_BASE  0
#define WFD_H264_PROFILE_CONSTRAINED_HIGH  1
#define WFD_H264_PROFILE_CONSTRAINED_HIGH2 2
#define WFD_H264_PROFILE_BASELINE          3
#define WFD_H264_PROFILE_MAIN              4
#define WFD_H264_PROFILE_HIGH              5
#define WFD_H264_NUM_PROFILES              6

#define WFD_H264_LEVEL_3_1                 0
#define WFD_H264_LEVEL_3_2                 1
#define WFD_H264_LEVEL_4                   2
#define WFD_H264_LEVEL_4_1                 3
#define WFD_H264_LEVEL_4_2                 4
#define WFD_H264_LEVEL_5                   5
#define WFD_H264_LEVEL_5_1                 6
#define WFD_H264_LEVEL_5_2                 7
#define WFD_H264_NUM_LEVELS                8


#define WFD_H265_PROFILE_MAIN              0
#define WFD_H265_PROFILE_MAIN444           1
#define WFD_H265_PROFILE_SCC8b444          2
#define WFD_H265_PROFILE_MAIN44410b        3
#define WFD_H265_PROFILE_MAIN10b           4
#define WFD_H265_PROFILE_MAIN_STILL        5
#define WFD_H265_NUM_PROFILES              6

#define WFD_H265_LEVEL_3_1                 0
#define WFD_H265_LEVEL_4                   1
#define WFD_H265_LEVEL_4_1                 2
#define WFD_H265_LEVEL_5                   3
#define WFD_H265_LEVEL_5_1                 4
#define WFD_H265_NUM_LEVELS                5


/* This enumerated type lists the different types of audio. */
typedef enum {
    WFD_AUDIO_UNK,          /* unknown audio */
    WFD_AUDIO_LPCM,         /* LPCM audio  */
    WFD_AUDIO_AAC,          /* AAC audio  */
    WFD_AUDIO_DOLBY_DIGITAL, /* Dolby Digital audio  (AC3) */
    WFD_AUDIO_INVALID,       /* invalid audio */
    WFD_AUDIO_MAX
} WFD_audio_type;

/* This enumerated type lists the different types of generic. */
typedef enum {
    WFD_GENERIC_UNK,          /* unknown generic */
    WFD_GENERIC_XML,          /* XML generic     */
    WFD_GENERIC_PNG,          /* PNG generic     */
    WFD_GENERIC_JPEG,         /* JPEG generic    */
    WFD_GENERIC_H265,         /* H.265 generic   */
    WFD_GENERIC_INVALID       /* invalid generic */
} WFD_generic_type;

#if 0
/* Video codec type */
//#define WFD_MAX_NUM_H264_PROFILES 8 // lubiny TODO
typedef struct {
    uint8            native_bitmap;
    uint8            preferred_display_mode_supported;
    /* H264 codec type  */
    WFD_h264_codec_capability_t* h264_codec;
    uint8            num_h264_profiles;
} WFD_video_codec_capability_t;
#endif

typedef struct {
#ifdef __cplusplus
    void dump(bool dumpCodecs = false);
#endif //__cplusplus
    uint8           native_bitmap;
    uint8           preferred_display_mode_supported;
    /* Extended codec type  */
    WFD_video_codec_capability_t codec[WFD_MAX_NUM_VIDEO_CODEC];
    uint8           num_codec;
} WFD_video_capability_t;

typedef struct WFD_HDCP_config
{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_HDCP_config &);
#endif //__cplusplus
    int64 hHDCPSession;
    bool  bHDCPSEssionValid;
}WFD_HDCP_config_t;
/*------------------------------------------------------------------------------
 A structure for storing audio common params
--------------------------------------------------------------------------------
*/
typedef struct audioCommonParams
{
#ifdef __cplusplus
    void dump();
    bool operator==(audioCommonParams &);
#endif //__cplusplus
    uint32 nSampleRate;
    uint32 nSamplesPerPacket;
    uint32 nBitsPerSample;
    uint32 nBitrate;
    uint8  nChannels;
}WFD_MM_AudioCommonParamsType;

/*------------------------------------------------------------------------------
 A structure for storing video common params
--------------------------------------------------------------------------------
*/
typedef struct videoCommonParams
{
#ifdef __cplusplus
    void dump();
    bool operator==(videoCommonParams&);
#endif //__cplusplus
    uint32 nWidth;
    uint32 nHeight;
    uint32 nFrameRate;
    uint32 nNominalBitrate;       // Codec can Override
    uint32 nNominalMinBitrate;    // Codec can Override
    uint64 nMBsPerFrameMBsPerSec; // MBsPerFrame * 2^32 + MBsPerSec
    bool   bSupported;
    bool   bInterlaced;
}WFD_MM_VideoCommonParamsType;

#ifdef __cplusplus
extern "C" {
#endif
extern const WFD_MM_VideoCommonParamsType gCEAModes[MAX_VIDEO_MODE_BITS];
extern const WFD_MM_VideoCommonParamsType gVESAModes[MAX_VIDEO_MODE_BITS];
extern const WFD_MM_VideoCommonParamsType gHHModes[MAX_VIDEO_MODE_BITS];
extern const WFD_MM_VideoCommonParamsType
                     *WFD_MM_VideoParamsTables[WFD_VIDEO_NUM_MODES];

extern const int gH264Profiles[WFD_H264_NUM_PROFILES];
extern const int gH264Levels[WFD_H264_NUM_LEVELS];
extern const int gH264OMXProfiles[WFD_H264_NUM_PROFILES];
extern const int gH264OMXLevels[WFD_H264_NUM_LEVELS];
extern const int gH265Profiles[WFD_H265_NUM_PROFILES];
extern const int gH265Levels[WFD_H265_NUM_LEVELS];
extern const int gH265OMXProfiles[WFD_H265_NUM_PROFILES];
extern const int gH265OMXLevels[WFD_H265_NUM_LEVELS];

extern const WFD_MM_AudioCommonParamsType gLPCMModes[MAX_AUDIO_MODE_BITS];
extern const WFD_MM_AudioCommonParamsType gAACModes[MAX_AUDIO_MODE_BITS];
extern const WFD_MM_AudioCommonParamsType gAC3Modes[MAX_AUDIO_MODE_BITS];
#ifdef __cplusplus
}
#endif

typedef enum IntraRefreshMode
{
    INTRA_REFRESH_CYCLIC,
    INTRA_REFRESH_ADAPTIVE,
    INTRA_REFRESH_RANDOM
}IntraRefreshModeType;

typedef struct WFD_h264_codec_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_h264_codec_config&);
#endif //__cplusplus
    //uint32         nSlicePerFrame; //!Either specify slices per frame
    uint32               nSliceMBs;//!MBs per slice. If both are set, slice per
                                   //!frame will be used.
    uint32               nIntraRefreshMBs; //!Num MBs to refresh per frame
    IntraRefreshModeType eIntraRefreshType;
    uint8                nSlicingEnabled;//!Slicing is enabled
    uint8                nIntraRefreshEnabled;//! Intra refresh enabled
}WFD_h264_codec_config_t;

typedef struct WFD_h265_codec_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_h265_codec_config&);
#endif //__cplusplus
    //uint32         nSlicePerFrame; //!Either specify slices per frame
    uint32               nSliceMBs;//!MBs per slice. If both are set, slice per
                                   //!frame will be used.
    uint32               nIntraRefreshMBs; //!Num MBs to refresh per frame
    IntraRefreshModeType eIntraRefreshType;
    uint8                nSlicingEnabled;//!Slicing is enabled
    uint8                nIntraRefreshEnabled;//! Intra refresh enabled
}WFD_h265_codec_config_t;

typedef struct WFD_video_codec_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_video_codec_config&);
#endif //__cplusplus
    WFD_video_type               eVideoCodec;
    WFD_MM_VideoCommonParamsType sVideoCommonParams;
    struct {
        WFD_h264_codec_config_t  sH264Params;
        WFD_h265_codec_config_t  sH265Params;
    };
    uint32         nIDRIntervalMs;
    uint32         nMaxFrameSkipIntervalMs;
    uint32         nProfileTableIdx;      //!Index to profile table
    uint32         nLevelTableIdx;        //!index to level table
    uint8          nCanSkipFrames;
    uint8          nUseBFrames;
}WFD_video_codec_config_t;

typedef struct WFD_audio_codec_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_audio_codec_config&);
#endif //__cplusplus
    WFD_audio_type               eAudioCodec;
    WFD_MM_AudioCommonParamsType sAudioCommonParams;
    uint8                        nProfileTableIdx;
}WFD_audio_codec_config_t;

typedef struct WFD_png_codec_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_png_codec_config &);
#endif //__cplusplus
    uint32 nImageWidth;
    uint32 nImageHeight;
}WFD_png_codec_config_t;

typedef struct WFD_generic_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_generic_config&);
#endif //__cplusplus
    WFD_generic_type eGenericMethod;
    WFD_png_codec_config_t sPNGInfo;
    uint8  nIsSecure;
}WFD_generic_config_t;

/*Properties pertinent to sink*/

/*To hold the raw co-ordinates of a view, not relative to parent*/
typedef struct WFD_view_rect{
#ifdef __cplusplus
    WFD_view_rect();
    void dump() const;
    bool operator==(const WFD_view_rect& )const;
    WFD_view_rect& operator=(const WFD_view_rect& rect);
#endif
    int   left;
    int   top;
    int   right;
    int   bottom;
    int   orientation;
}WFD_view_rect_t;

typedef struct WFD_sink_prop{
#ifdef __cplusplus
    WFD_sink_prop();
    void dump() const;
    bool operator==(const WFD_sink_prop& )const;
    WFD_sink_prop& operator=(const WFD_sink_prop& sinkProp);
    bool populateSurfRect(const int32_t* arr, size_t len);
    bool populateScreenRect(const int32_t* arr, size_t len);
#endif
    void*         pSurface;
    int           relSurfaceWidth;
    int           relSurfaceHeight;
    int           dispOrientation;
    WFD_view_rect surfaceRect;
    WFD_view_rect screenRect;
    int           cursorSocket;
    int           clockDeltaMs; // system time diff wrt source
}WFD_sink_prop_t;

typedef struct WFD_Multimedia_config{
#ifdef __cplusplus
    void dump();
    bool operator==(WFD_Multimedia_config&);
#endif
    WFD_audio_codec_config_t     sAudioConfig;
    WFD_video_codec_config_t     sVideoConfig;
    WFD_generic_config_t         sGenericConfig;
    WFD_HDCP_config_t            sHdcpCtx;
    uint32                       nAVSyncDelay;
    void                        *pStreamPort;//! IStreamPort Object
    bool                         bNonLatencyCritical;
    NetType                      eNetType;
    WFD_sink_prop                sSinkProp;
}WFD_Multimedia_config_t;

typedef struct {
    uint8           native_bitmap;
    uint8           preferred_display_mode_supported;
    /* H264 codec type  */
    WFD_3d_h264_codec_config_t* h264_codec;
    uint8           num_h264_profiles;
} WFD_3d_video_codec_config_t;

#if 0
typedef union {
    WFD_video_codec_capability_t    video_config;
    WFD_3d_video_codec_config_t    threed_video_config;
} WFD_video_config;
#endif



/* Audio codec type */
typedef struct {
    /* lpcm codec type  */
    WFD_lpcm_codec_config_t            lpcm_codec;
    /* aac codec type  */
    WFD_aac_codec_config_t            aac_codec;
    /* dolby digital codec type  */
    WFD_dolby_digital_codec_config_t    dolby_digital_codec;
} WFD_audio_config;
/* Definitions of constants for extended capability*/
#define WFD_EXTENDED_CAPABILITY_SUB_ELEMENT_ID      7
#define WFD_EXTENDED_CAPABILITY_SUB_ELEMENT_LENGTH  2
#define WFD_EXTENDED_CAPABILITY_UIBC                BIT0

typedef struct
{
    uint8    sub_element_id;    /* Should be set to 7 for this structure*/
    uint8    length;        /* Should be set to 2 */
    uint16    ext_capability_bitmap;    /*Based on Table  5.27, spec 1.17 */
}WFD_extended_capability_config_t;

typedef struct {
    unsigned int nss;
    unsigned int rateflags;
    unsigned int freq;
    unsigned int enable_4k;
} WFD_link_properties_t;

/* Multimedia capability */
typedef struct {
    WFD_audio_type                  audio_method;
    WFD_generic_type                generic_method;
    //WFD_video_config                video_config;
    WFD_audio_config                audio_config;
    WFD_video_capability_t          video_capability;
    bool                            standby_resume_support;

    WFD_content_protection_capability_config_t content_protection_config;
    WFD_transport_capability_config_t   transport_capability_config;
    WFD_link_properties_t           link_properties;
    WFD_ipv4_addr_t                 ip_addrs;
    WFD_rtp_qos_t                   rtp_qos_settings;
    WFD_edid_info_type              edid;
    WFD_buffering_delay_type        bufferSize;
    WFD_HDCP_config_t               HdcpCtx;
    uint64                          decoder_latency;
    bool                            direct_streaming_mode;
    WFD_sink_prop                   sinkProp;
} WFD_MM_capability_t;

typedef struct {
    uint8                           count;
    uint8                           name;
    uint8                           maxOverlay;
    uint8                           supportedBitMap;
}WFD_MM_Aux_capability_t;

typedef enum
{
    AV_CONTROL_PLAY,
    AV_CONTROL_PAUSE,
    AV_CONTROL_FLUSH,
    AV_CONTROL_SET_VOLUME,
    AV_CONTROL_SET_DECODER_LATENCY,
    AV_CONTROL_DISABLE_AUDIO,
    AV_CONTROL_ENABLE_AUDIO
}WFD_MM_AV_Stream_Control_t;

typedef struct
{
    char codecType[10];
    uint16 profile;
    uint16 level;
    uint16 width;
    uint16 height;
} codecInfoType;


/* Status */
typedef enum {
    WFD_STATUS_SUCCESS,
    WFD_STATUS_FAIL,
    WFD_STATUS_NOTSUPPORTED,
    WFD_STATUS_BADPARAM,
    WFD_STATUS_MEMORYFAIL,
    WFD_STATUS_RUNTIME_ERROR,
    WFD_STATUS_READY,
    WFD_STATUS_RTCP_RR_MESSAGE,
    WFD_STATUS_CONNECTED,
    WFD_STATUS_PROXY_CLOSED,
    WFD_STATUS_PROXY_OPENED,
    WFD_STATUS_STREAMING_REQ,
    WFD_STATUS_AUDIO_STREAM_ENABLED,
    WFD_STATUS_AUDIO_STREAM_DISABLED,
    WFD_STATUS_VIDEO_STREAM_ENABLED,
    WFD_STATUS_VIDEO_STREAM_DISABLED,
    WFD_STATUS_INVALID
} WFD_status_t;

/* General MM Updates to SM-B */
typedef enum {
    WFD_EVENT_NO_EVENT,
    WFD_EVENT_HDCP_CONNECT_DONE,
    WFD_EVENT_MM_VIDEO,
    WFD_EVENT_MM_AUDIO,
    WFD_EVENT_MM_SESSION_EVENT,
    WFD_EVENT_MM_RTP,
    WFD_EVENT_MM_HDCP,
    WFD_EVENT_MM_STREAMING,
    WFD_EVENT_MM_OVERLAY,
    WFD_EVENT_MM_CONNECTION_INFO_REQ,
    WFD_EVENT_MM_IDR_REQ,
    WFD_EVENT_MAX_EVENT,
}WFD_MM_Event_t;

/* Audio/Video stream control selection */
typedef enum {
    WFD_STREAM_AV,       /* play/pause both Video and Audio */
    WFD_STREAM_VIDEO,    /* play/pause video only */
    WFD_STREAM_AUDIO,    /* play/pause audio only */
    WFD_STREAM_GENERIC   /* play/pause generic only */
} WFD_AV_select_t;

/* WFD HDCP version detials */
typedef enum {
    WFD_HDCP_VERSION_2_0 = 1,
    WFD_HDCP_VERSION_2_1,
    WFD_HDCP_VERSION_2_2,
    WFD_HDCP_VERSION_2_3,
    WFD_HDCP_VERSION_MAX = WFD_HDCP_VERSION_2_3
}WFD_HDCP_version_t;

typedef enum {
    WFD_HDCP_STREAM_AUDIO,
    WFD_HDCP_STREAM_VIDEO,
    WFD_HDCP_STREAM_OTHER
}WFD_HDCP_Stream_t;

typedef int fdtype;


typedef void (*wfd_mm_capability_change_cb)(WFD_MM_HANDLE);
typedef void (*wfd_mm_stream_play_cb)(WFD_MM_HANDLE, WFD_status_t status);
typedef void (*wfd_mm_stream_pause_cb)(WFD_MM_HANDLE, WFD_status_t status);
typedef void (*wfd_mm_update_session_cb)(WFD_MM_HANDLE, WFD_status_t status);
typedef void (*wfd_mm_update_event_cb)(WFD_MM_Event_t, WFD_status_t, void* pEvtData);
typedef void (*wfd_mm_request_IDRframe_cb)(WFD_MM_HANDLE);

typedef void (*wfd_av_format_change_timing_cb)(WFD_MM_HANDLE);

typedef struct
{
    wfd_mm_capability_change_cb           capability_cb;  // Callback to inform session manager if there is any change in the MM capabilities
    wfd_mm_request_IDRframe_cb            idr_cb;         // Callback to inform sink session manager to send a request an IDR frame
    wfd_av_format_change_timing_cb        av_format_change_cb;  // Callback to notify AV format chnage timing to session manager
    wfd_mm_update_event_cb                update_event_cb; //Callback to notify general events
}WFD_MM_callbacks_t;

#endif //__WDSM_MM_INTERFACE_H__
