/**
 * @file      swvdec_types.h
 * @brief     SwVdec framework data type definitions.
 * @copyright
 *            Copyright (c) 2015-2018,2020 Qualcomm Technologies, Inc.
 *            All Rights Reserved.
 *            Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SWVDEC_TYPES_H_
#define _SWVDEC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
/// Non-Win32 macro to get time in nanoseconds.
#define GET_TIME_IN_NANOSECONDS(p)                                       \
    do {                                                                 \
        struct timespec t;                                               \
        clock_gettime(CLOCK_MONOTONIC, &t);                              \
        *p = (((unsigned long long) t.tv_sec) * 1000000000) + t.tv_nsec; \
    } while(0)

/// SwVdec version date
#define SWVDEC_VERSION_DATE "2017-12-21@13:47:54"
#define VC1_DSP_LIBS_LABEL "VIDEO_CODECS_SW_1.15"

/// error code enumeration
typedef enum {
    SWVDEC_STATUS_SUCCESS,                ///< success
    SWVDEC_STATUS_FAILURE,                ///< failure
    SWVDEC_STATUS_NULL_POINTER,           ///< null pointer
    SWVDEC_STATUS_INVALID_PARAMETERS,     ///< invalid parameters
    SWVDEC_STATUS_INVALID_STATE,          ///< function called in invalid state
    SWVDEC_STATUS_INSUFFICIENT_RESOURCES, ///< insufficient resources
    SWVDEC_STATUS_UNSUPPORTED,            ///< unsupported feature
    SWVDEC_STATUS_NOT_IMPLEMENTED         ///< feature not yet implemented
} SWVDEC_STATUS;

// NOTE: The following flags _MUST_ match the OMX IL API spec.

#define SWVDEC_FLAG_EOS         0x00000001 ///< end-of-stream
#define SWVDEC_FLAG_DATACORRUPT 0x00000008 ///< data corrupt
#define SWVDEC_FLAG_SYNCFRAME   0x00000020 ///< sync frame
#define SWVDEC_FLAG_READONLY    0x00000200 ///< read-only
#define SWVDEC_FLAG_CODEC_CONFIG 0x00000080 ///<codec-config

/// codec type enumeration
typedef enum {
    SWVDEC_CODEC_INVALID = 0, ///< invalid
    SWVDEC_CODEC_H263,        ///< H.263
    SWVDEC_CODEC_MPEG4,       ///< MPEG-4 Part 2
    SWVDEC_CODEC_VC1          ///< VC1
} SWVDEC_CODEC;

/// macro for printing codec
#define SWVDEC_CODEC_STRING(x)               \
    ((x == SWVDEC_CODEC_H263) ? "H.263" :    \
     ((x == SWVDEC_CODEC_MPEG4) ? "MPEG-4" : \
      ((x == SWVDEC_CODEC_VC1) ? "VC1" :     \
       "unknown")))

/// H.263 profile enumeration
typedef enum {
    SWVDEC_PROFILE_H263_INVALID = 0, ///< invalid
    SWVDEC_PROFILE_H263_BASELINE     ///< baseline profile
} SWVDEC_PROFILE_H263;

/// macro for printing H.263 profile
#define SWVDEC_PROFILE_H263_STRING(x)                           \
    ((x == SWVDEC_PROFILE_H263_BASELINE) ? "baseline profile" : \
     "unknown")

/// MPEG-4 profile enumeration
typedef enum {
    SWVDEC_PROFILE_MPEG4_INVALID = 0,    ///< invalid
    SWVDEC_PROFILE_MPEG4_SIMPLE,         ///< simple profile
    SWVDEC_PROFILE_MPEG4_ADVANCED_SIMPLE ///< advanced simple profile
} SWVDEC_PROFILE_MPEG4;

/// macro for printing MPEG-4 profile
#define SWVDEC_PROFILE_MPEG4_STRING(x)                       \
    ((x == SWVDEC_PROFILE_MPEG4_SIMPLE) ? "simple profile" : \
     ((x == SWVDEC_PROFILE_MPEG4_ADVANCED_SIMPLE) ?          \
      "advanced simple profile" :                            \
      "unknown"))

/// profile union
typedef union {
    SWVDEC_PROFILE_H263  h263;  ///< H.263 profile
    SWVDEC_PROFILE_MPEG4 mpeg4; ///< MPEG-4 profile
} SWVDEC_PROFILE;

/// H.263 level enumeration
typedef enum {
    SWVDEC_LEVEL_H263_INVALID = 0, ///< invalid
    SWVDEC_LEVEL_H263_10,          ///< level 10
    SWVDEC_LEVEL_H263_20,          ///< level 20
    SWVDEC_LEVEL_H263_30,          ///< level 30
    SWVDEC_LEVEL_H263_40,          ///< level 40
    SWVDEC_LEVEL_H263_45,          ///< level 45
    SWVDEC_LEVEL_H263_50,          ///< level 50
    SWVDEC_LEVEL_H263_60,          ///< level 60
    SWVDEC_LEVEL_H263_70           ///< level 70
} SWVDEC_LEVEL_H263;

/// macro for printing H.263 level
#define SWVDEC_LEVEL_H263_STRING(x)                    \
    ((x == SWVDEC_LEVEL_H263_10) ? "level 10" :        \
     ((x == SWVDEC_LEVEL_H263_20) ? "level 20" :       \
      ((x == SWVDEC_LEVEL_H263_30) ? "level 30" :      \
       ((x == SWVDEC_LEVEL_H263_40) ? "level 40" :     \
        ((x == SWVDEC_LEVEL_H263_45) ? "level 45" :    \
         ((x == SWVDEC_LEVEL_H263_50) ? "level 50" :   \
          ((x == SWVDEC_LEVEL_H263_60) ? "level 60" :  \
           ((x == SWVDEC_LEVEL_H263_70) ? "level 70" : \
            "unknown"))))))))

/// MPEG-4 level enumeration
typedef enum {
    SWVDEC_LEVEL_MPEG4_INVALID = 0, ///< invalid
    SWVDEC_LEVEL_MPEG4_0,           ///< level 0
    SWVDEC_LEVEL_MPEG4_0B,          ///< level 0b
    SWVDEC_LEVEL_MPEG4_1,           ///< level 1
    SWVDEC_LEVEL_MPEG4_2,           ///< level 2
    SWVDEC_LEVEL_MPEG4_3,           ///< level 3
    SWVDEC_LEVEL_MPEG4_3B,          ///< level 3b
    SWVDEC_LEVEL_MPEG4_4,           ///< level 4
    SWVDEC_LEVEL_MPEG4_4A,          ///< level 4a
    SWVDEC_LEVEL_MPEG4_5,           ///< level 5
    SWVDEC_LEVEL_MPEG4_6,           ///< level 6
    SWVDEC_LEVEL_MPEG4_7,           ///< level 7
    SWVDEC_LEVEL_MPEG4_8,           ///< level 8
    SWVDEC_LEVEL_MPEG4_9            ///< level 9
} SWVDEC_LEVEL_MPEG4;

/// macro for printing MPEG-4 level
#define SWVDEC_LEVEL_MPEG4_STRING(x)                       \
    ((x == SWVDEC_LEVEL_MPEG4_0) ? "level 0" :             \
     ((x == SWVDEC_LEVEL_MPEG4_0B) ? "level 0b" :          \
      ((x == SWVDEC_LEVEL_MPEG4_1) ? "level 1" :           \
       ((x == SWVDEC_LEVEL_MPEG4_2) ? "level 2" :          \
        ((x == SWVDEC_LEVEL_MPEG4_3) ? "level 3" :         \
         ((x == SWVDEC_LEVEL_MPEG4_3B) ? "level 3b" :      \
          ((x == SWVDEC_LEVEL_MPEG4_4) ? "level 4" :       \
           ((x == SWVDEC_LEVEL_MPEG4_4A) ? "level 4a" :    \
            ((x == SWVDEC_LEVEL_MPEG4_5) ? "level 5" :     \
             ((x == SWVDEC_LEVEL_MPEG4_6) ? "level 6" :    \
              ((x == SWVDEC_LEVEL_MPEG4_7) ? "level 7" :   \
               ((x == SWVDEC_LEVEL_MPEG4_8) ? "level 8" :  \
                ((x == SWVDEC_LEVEL_MPEG4_9) ? "level 9" : \
                 "unknown")))))))))))))

/// level union
typedef union {
    SWVDEC_LEVEL_H263  h263;  ///< H.263 level
    SWVDEC_LEVEL_MPEG4 mpeg4; ///< MPEG-4 level
} SWVDEC_LEVEL;

/// frame dimensions structure
typedef struct {
    unsigned int width;  ///< width
    unsigned int height; ///< height
} SWVDEC_FRAME_DIMENSIONS;

/// color format enumeration
typedef enum {
    SWVDEC_COLOR_FORMAT_INVALID = 0,     ///< invalid
    SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV12, ///< NV12 semiplanar format
    SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV12_32,///<NV12_32m semiplanar format
    SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV21  ///< NV21 semiplanar format
} SWVDEC_COLOR_FORMAT;

/// macro for printing color format
#define SWVDEC_COLOR_FORMAT_STRING(x)                                   \
    ((x == SWVDEC_COLOR_FORMAT_INVALID) ? "invalid" :                   \
     ((x == SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV12) ? "semiplanar NV12" :  \
     ((x == SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV12_32) ? "semiplanar NV12_32" : \
      ((x == SWVDEC_COLOR_FORMAT_SEMIPLANAR_NV21) ? "semiplanar NV21" : \
       "unknown"))))

/// frame attributes structure
typedef struct {
    SWVDEC_COLOR_FORMAT color_format; ///< color format
    unsigned int        stride;       ///< framebuffer stride
    unsigned int        scanlines;    ///< framebuffer scanlines
    unsigned int        size;         ///< size of luma + chroma planes
} SWVDEC_FRAME_ATTRIBUTES;

/// buffer requirements structure
typedef struct {
    unsigned int size;      ///< buffer size
    unsigned int mincount;  ///< buffer count (minimum)
    unsigned int maxcount;  ///< buffer count (maximum)
    unsigned int alignment; ///< buffer alignment
} SWVDEC_BUFFER_REQ;

/// buffer structure for input or output buffers
typedef struct {
    unsigned char     *p_buffer;      ///< buffer pointer
    unsigned int       size;          ///< allocated size
    unsigned int       flags;         ///< buffer flags
    unsigned int       offset;        ///< offset
    unsigned int       filled_length; ///< filled length
    unsigned long long timestamp;     ///< timestamp in microseconds
    void              *p_client_data; ///< pointer to client data
    unsigned int       fd;            ///<fd of buffer
    unsigned int       VDspAddr;      ///<mapped DSP address
} SWVDEC_BUFFER;

/// flush type enumeration
typedef enum {
    SWVDEC_FLUSH_TYPE_ALL, ///< flush input & output buffers
    SWVDEC_FLUSH_TYPE_OP   ///< flush only output buffers
} SWVDEC_FLUSH_TYPE;

/// property ID enumeration
typedef enum {
    SWVDEC_PROPERTY_ID_FRAME_DIMENSIONS, ///< frame dimensions
    SWVDEC_PROPERTY_ID_FRAME_ATTRIBUTES, ///< frame attributes
    SWVDEC_PROPERTY_ID_BUFFER_REQ_IP,    ///< buffer requirements (input)
    SWVDEC_PROPERTY_ID_BUFFER_REQ_OP,    ///< buffer requirements (output)
    SWVDEC_PROPERTY_ID_ADAPTIVE_PLAYBACK, ///< adaptive playback
    SWVDEC_PROPERTY_ID_THUMBNAIL_MODE    ///< thumbnail mode
} SWVDEC_PROPERTY_ID;

/// property information union
typedef union {
    SWVDEC_FRAME_DIMENSIONS frame_dimensions; ///< frame dimensions
    SWVDEC_FRAME_ATTRIBUTES frame_attributes; ///< frame attributes
    SWVDEC_BUFFER_REQ       buffer_req_ip;    ///< buffer requirements (input)
    SWVDEC_BUFFER_REQ       buffer_req_op;    ///< buffer requirements (output)
} SWVDEC_PROPERTY_INFO;

/// property structure
typedef struct {
    SWVDEC_PROPERTY_ID   id;   ///< property ID
    SWVDEC_PROPERTY_INFO info; ///< property information
} SWVDEC_PROPERTY;

/// event enumeration
typedef enum {
    SWVDEC_EVENT_FLUSH_ALL_DONE,     ///< flush all done
    SWVDEC_EVENT_FLUSH_OP_DONE,      ///< flush output done
    SWVDEC_EVENT_RELEASE_REFERENCE,  ///< release reference
    SWVDEC_EVENT_RECONFIG_REQUIRED,  ///< reconfig required
    SWVDEC_EVENT_DIMENSIONS_UPDATED, ///< dimensions updated
    SWVDEC_EVENT_FATAL_ERROR         ///< fatal error
} SWVDEC_EVENT;

/// software decoder object handle dummy typedef
typedef void * SWVDEC_HANDLE;

/// callback function pointers structure
typedef struct {

    /**
     * @brief Callback function pointer to return input buffer to client.
     *
     * @param swvdec:      SwVdec handle.
     * @param p_buffer_ip: Pointer to input buffer.
     * @param p_client:    Client handle.
     *
     * @retval SWVDEC_STATUS_SUCCESS if success, error status otherwise.
     */
    SWVDEC_STATUS (*pfn_empty_buffer_done)(SWVDEC_HANDLE  swvdec,
                                           SWVDEC_BUFFER *p_buffer_ip,
                                           void          *p_client);
    /**
     * @brief Callback function pointer to return output buffer to client.
     *
     * @param swvdec:      SwVdec handle.
     * @param p_buffer_op: Pointer to output buffer.
     * @param p_client:    Client handle.
     *
     * @retval SWVDEC_STATUS_SUCCESS if success, error status otherwise.
     */
    SWVDEC_STATUS (*pfn_fill_buffer_done)(SWVDEC_HANDLE  swvdec,
                                          SWVDEC_BUFFER *p_buffer_op,
                                          void          *p_client);
    /**
     * @brief Callback function pointer to notify client of an event.
     *
     * @param swvdec:   SwVdec handle.
     * @param event:    Event.
     * @param p_data:   Pointer to event data.
     * @param p_client: Client handle.
     *
     * @retval SWVDEC_STATUS_SUCCESS if success, error status otherwise.
     */
    SWVDEC_STATUS (*pfn_event_notification)(SWVDEC_HANDLE swvdec,
                                            SWVDEC_EVENT  event,
                                            void         *p_data,
                                            void         *p_client);
    /**
     * @brief Client callback handle.
     */
    void *p_client;

} SWVDEC_CALLBACK;

#ifdef __cplusplus
} // closing brace for: extern "C" {
#endif

#endif // #ifndef _SWVDEC_TYPES_H_
