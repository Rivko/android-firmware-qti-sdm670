#ifndef ELITE_CAPI_V2_TYPES_H
#define ELITE_CAPI_V2_TYPES_H

/* ========================================================================*/
/**
 @file Elite_CAPI_V2_types.h
 Common Audio Processing Interface v2 common types.

 This file defines the basic data types for the Common Audio Processing
 Interface.
 */

/*======================================================================
 Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

#include "mmdefs.h"

typedef uint32_t capi_v2_err_t;

/** Success. The operation completed with no errors. */
#define CAPI_V2_EOK 0
/** General failure. */
#define CAPI_V2_EFAILED             ((uint32_t)1)
/** Invalid parameter value set. */
#define CAPI_V2_EBADPARAM           (((uint32_t)1) << 1)
/** Unsupported routine or operation. */
#define CAPI_V2_EUNSUPPORTED        (((uint32_t)1) << 2)
/** Operation does not have memory. */
#define CAPI_V2_ENOMEMORY           (((uint32_t)1) << 3)
/** Operation needs more data or buffer space. */
#define CAPI_V2_ENEEDMORE           (((uint32_t)1) << 4)

#define CAPI_V2_FAILED(x) (CAPI_V2_EOK != (x))
#define CAPI_V2_SUCCEEDED(x) (CAPI_V2_EOK == (x))

#define CAPI_V2_SET_ERROR(error_flags, return_code) ((error_flags) |= (return_code))
#define CAPI_V2_IS_ERROR_CODE_SET(error_flags, error_code) (((error_flags) & (error_code)) != CAPI_V2_EOK)

typedef struct capi_v2_buf_t capi_v2_buf_t;
struct capi_v2_buf_t
{
   int8_t* data_ptr;
   /**< Data pointer to the raw data. The alignment depends on the format
    * of the data that is being pointed to. */

   uint32_t actual_data_len;
   /**< For input buffers,
    * 	the caller fills this field with the number of bytes
    * 	of valid data in the buffer and the callee fills this field
    * 	with the number of bytes of data it read.
    * 	For output buffers,
    * 	the caller leaves this field uninitialized and the callee fills
    * 	it with the number of bytes of data it filled.
    */

   uint32_t max_data_len;
   /**< Total allocated size (in bytes) of the buffer. This value is always
    * filled by the caller and it is not modified by the callee.
    */
};

typedef struct capi_v2_stream_flags_t capi_v2_stream_flags_t;
struct capi_v2_stream_flags_t
{
   uint32_t is_timestamp_valid :1; /**< 0 = timestamp not valid
    1 = timestamp valid */
   uint32_t end_of_frame :1; /**< 0 = indicates that the buffer may have an
    incomplete frame (relevant for compressed
    data only).
    1 = indicates that the buffer ends with the end
    of a frame. This allows the module to start
    processing without scanning for the end of
    frame. */
   uint32_t marker_eos :1; /**< A marker that indicates that this data is the
    last valid data from the upstream port. */
   uint32_t marker_1 :1; /**< marker_1, marker_2 and marker_3 are data markers
    which are used by the service to track data. The
    module must propagate these markers from the input
    port to any output port that gets input from this
    port. */
   uint32_t marker_2 :1;
   uint32_t marker_3 :1;

   uint32_t reserved :26;
};

/**
 @brief Structure for the data for one stream.
 */
typedef struct capi_v2_stream_data_t capi_v2_stream_data_t;
struct capi_v2_stream_data_t
{
   capi_v2_stream_flags_t flags; /**< Flags indicating the stream properties */

   int64_t timestamp; /**< Timestamp of the first sample of the data
    in microseconds. The time origin is not fixed,
    it should be inferred from the timestamp of
    the first buffer. Negative values are allowed. */
   capi_v2_buf_t* buf_ptr; /**< Array of CAPI_V2 buffer elements. For deinterleaved
    unpacked uncompressed data, one buffer is to be
    used per channel. For all other cases, only one buffer
    is to be used. */
   uint32_t bufs_num; /**< Number of buffer elements in the buf_ptr array. For
    deinterleaved unpacked uncompressed data, this is equal
    to the number of channels in the stream. For all other
    cases, all the data will be put in one buffer, so this is
    set to 1. */
};

#define CAPI_V2_MAX_CHANNELS 16 /**< The maximum number of channels supported in a stream. */

enum data_format_t
{
   CAPI_V2_FIXED_POINT,
   CAPI_V2_FLOATING_POINT,
   CAPI_V2_RAW_COMPRESSED,
   CAPI_V2_IEC61937_PACKETIZED,
   CAPI_V2_MAX_FORMAT_TYPE = 0x7FFFFFFF
};
typedef enum data_format_t data_format_t;

#define CAPI_V2_DATA_FORMAT_INVALID_VAL 0xFFFFFFFF /* This value can be used in any field of capi_v2_data_format_t
                                                   to indicate that that value is unspecified. */

enum capi_v2_interleaving_t
{
   CAPI_V2_INTERLEAVED,
   CAPI_V2_DEINTERLEAVED_PACKED,
   CAPI_V2_DEINTERLEAVED_UNPACKED,
   CAPI_V2_INVALID_INTERLEAVING = CAPI_V2_DATA_FORMAT_INVALID_VAL
};
typedef enum capi_v2_interleaving_t capi_v2_interleaving_t;

/** @ingroup data_types
 Header structure for a data format that is passed into the module.
 */
typedef struct capi_v2_data_format_header_t capi_v2_data_format_header_t;
struct capi_v2_data_format_header_t
{
   data_format_t data_format; /**< Indicates the format in which the data is represented. The rest of the payload.
    will depend on the data format. */
   // Following this header is the appropriate media format payload.
};

/** @ingroup data_types
 Header structure for setting and getting the media format.
 */
typedef struct capi_v2_set_get_media_format_t capi_v2_set_get_media_format_t;
struct capi_v2_set_get_media_format_t
{
   capi_v2_data_format_header_t format_header;
   // Following this header is the appropriate media format payload.
};

/** @ingroup data_types
 Payload structure for data formats FIXED_POINT, FLOATING_POINT and IEC61937_PACKETIZED.
 */
typedef struct capi_v2_standard_data_format_t capi_v2_standard_data_format_t;
struct capi_v2_standard_data_format_t
{
   uint32_t bitstream_format; /**< The valid types are ADSP_MEDIA_FMT_* defined in adsp_media_fmt.h */
   uint32_t num_channels; /**< Number of channels. */
   uint32_t bits_per_sample; /**< Number of bits used to store each sample.
    For example, if the data is 24 bit audio packed in 24 bits,
    this value should be 24. If it is 24 bit audio packed in 32 bits
    this value should be 32.
    */
   uint32_t q_factor; /**< Number of fractional bits in the fixed point representation of the data. If the
    data is floating point, this field should be set to
    CAPI_V2_DATA_FORMAT_INVALID_VAL. */
   uint32_t sampling_rate; /**< Sampling rate in samples per second. */
   uint32_t data_is_signed; /**< Data is signed (1) or unsigned (0). */
   capi_v2_interleaving_t data_interleaving; /**< Indicates whether the data is interleaved, deinterleaved
    packed or deinterleaved unpacked. This value is not
    relevant for packetized data. */
   uint16_t channel_type[CAPI_V2_MAX_CHANNELS];
   /**< Channel types for each of the num_channels. The valid channel types
    are the PCM_CHANNEL_* types defined in @xhyperref{Q3,[Q3]}.*//* adsp_media_fmt.h */
   /*Q3 = Hex MM ADSP.BF.2.0: Audio API Interface Spec (80-xxxxx-x) */
};

/** @ingroup data_types
 Payload structure header for data format RAW_COMPRESSED.
 */
typedef struct capi_v2_raw_compressed_data_format_t capi_v2_raw_compressed_data_format_t;
struct capi_v2_raw_compressed_data_format_t
{
   uint32_t bitstream_format; /**< The valid types are ADSP_MEDIA_FMT_* defined in adsp_media_fmt.h */
   /**< Following this structure is the media format structure for the particular data format as defined
    * in adsp_media_fmt.h
    */
};

/** @ingroup data_types
 Payload structure of port info type.
 */
typedef struct capi_v2_port_info_t capi_v2_port_info_t;
struct capi_v2_port_info_t
{
   bool_t is_valid;      /* whether port_index is valid or not. */
   bool_t is_input_port; /* TRUE for input ports and FALSE for output ports. */
   uint32_t port_index;
};
#endif /* #ifndef ELITE_CAPI_V2_TYPES_H */
