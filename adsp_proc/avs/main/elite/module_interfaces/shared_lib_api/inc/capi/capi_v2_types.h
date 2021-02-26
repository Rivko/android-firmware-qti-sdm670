#ifndef CAPI_V2_TYPES_H
#define CAPI_V2_TYPES_H

/* ========================================================================*/
/**
 @file capi_v2_types.h
 Common Audio Processing Interface v2 common types.

 This file defines the basic data types for the Common Audio Processing
 Interface.
 */
/*======================================================================
 Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 Export of this technology or software is regulated by the U.S. Government,
 Diversion contrary to U.S. law prohibited.
 ====================================================================== */
/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 07/16/16    sw     (Tech Pubs) Merged Doxygen comments from 8996 and 8953.2.7;
                    edited Doxygen comments/markup for 8998.2.8.
 05/14/15    sw     (Tech Pubs) Merged Doxygen comments from 8994 (2.6).
 ========================================================================= */

#include "mmdefs.h"

/** @addtogroup capiv2_error_codes
@{ */

/** Error code type for CAPIv2. */
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

/** Macro that checks whether a CAPIv2 error code has any error bits set. */
#define CAPI_V2_FAILED(x) (CAPI_V2_EOK != (x))

/** Macro that checks whether a CAPIv2 error code represents a success case. */
#define CAPI_V2_SUCCEEDED(x) (CAPI_V2_EOK == (x))

/** Macro that sets an error flag in a CAPIv2 error code. */
#define CAPI_V2_SET_ERROR(error_flags, return_code) ((error_flags) |= (return_code))

/** Macro that checks whether a specific error flag is set in a CAPIv2 error
    code. */
#define CAPI_V2_IS_ERROR_CODE_SET(error_flags, error_code) (((error_flags) & (error_code)) != CAPI_V2_EOK)

/** @} *//* end_addtogroup capiv2_error_codes */


typedef struct capi_v2_buf_t capi_v2_buf_t;

/** @addtogroup capiv2_data_types
@{ */
/** Contains input buffers, output buffers, property payloads, event payloads,
  and parameters that are passed into the CAPIv2 functions.
*/
struct capi_v2_buf_t
{
   int8_t* data_ptr;
   /**< Data pointer to the raw data. The alignment depends on the format
        of the raw data. */

   uint32_t actual_data_len;
   /**< Length of the valid data (in bytes).

        For input buffers:
        - The caller fills this field with the number of bytes of valid data in
          the buffer.
        - The callee fills this field with the number of bytes of data it read.

        For output buffers:
        - The caller leaves this field uninitialized.
        - The callee fills it with the number of bytes of data it filled.
        @tablebulletend */

   uint32_t max_data_len;
   /**< Total allocated size of the buffer (in bytes).

        This value is always filled by the caller, and it is not modified by
        the callee. */
};

typedef struct capi_v2_stream_flags_t capi_v2_stream_flags_t;

/** Flags that are passed with every input buffer and must be filled by the
  module for every output buffer. These flags apply only to the buffer with
  which they are associated.
*/
struct capi_v2_stream_flags_t
{
   uint32_t is_timestamp_valid :1;
   /**< Specifies whether the timestamp is valid.

        @values
        - 0 -- Not valid
        - 1 -- Valid @tablebulletend */

   uint32_t end_of_frame :1;
   /**< Specifies whether the buffer ends with the end of an encoded frame.

        @values
        - 0 -- The buffer might have an incomplete frame (relevant for
          compressed data only)
        - 1 --The buffer ends with the end of a frame. This allows the module
          to start processing without scanning for the end of frame.
         @tablebulletend */

   uint32_t marker_eos :1;
   /**< Indicates that this data is the last valid data from the upstream
        port. */

   uint32_t marker_1 :1;
   /**< Data marker 1 used by the service to track data.

        The module must propagate this marker from the input port to any output
        port that gets input from this port. */

   uint32_t marker_2 :1;
   /**< Data marker 2 used by the service to track data.

        The module must propagate this marker from the input port to any output
        port that gets input from this port. */

   uint32_t marker_3 :1;
   /**< Data marker 3 used by the service to track data.

        The module must propagate this marker from the input port to any output
        port that gets input from this port. */

   uint32_t reserved :26;
   /**< Reserved for future use. The client must ignore this value for input
        ports and set it to zero for output ports. */
};

typedef struct capi_v2_stream_data_t capi_v2_stream_data_t;

/** Data structure for one stream.
 */
struct capi_v2_stream_data_t
{
   capi_v2_stream_flags_t flags;
   /**< Flags that indicate the stream properties. For more information on
        the flags, see the capi_v2_stream_flags_t structure. */

   int64_t timestamp;
   /**< Timestamp of the first data sample, in microseconds.

        The time origin is not fixed; it must be inferred from the timestamp of
        the first buffer. Negative values are allowed. */

   capi_v2_buf_t* buf_ptr;
   /**< Array of CAPI_V2 buffer elements.

        For deinterleaved unpacked uncompressed data, one buffer is to be
        used per channel. For all other cases, only one buffer is to be
        used. */

   uint32_t bufs_num;
   /**< Number of buffer elements in the buf_ptr array.

        For deinterleaved unpacked uncompressed data, this is equal to the
        number of channels in the stream. For all other cases, all the data
        is put in one buffer, so this field is set to 1. */
};

/** Maximum number of channels supported in a stream. */
#define CAPI_V2_MAX_CHANNELS 16

/** Valid values for data formats of the inputs and outputs of a module.
*/
enum data_format_t
{
   CAPI_V2_FIXED_POINT,
   /**< Fixed point PCM data.

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_FLOATING_POINT,
   /**< Floating point PCM data.

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_RAW_COMPRESSED,
   /**< Compressed audio data bitstream in an unpacketized form.

        Payload structure: capi_v2_raw_compressed_data_format_t @vertspace{6} */

   CAPI_V2_IEC61937_PACKETIZED,
   /**< Compressed audio data bitstream packetized according to
        @xhyperref{S1,IEC~61937}.

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_DSD_DOP_PACKETIZED,
   /**< Compressed audio data bitstream packetized in Qualcomm DSD-over-PCM
        (DSD_DOP) format (16 bit, MSB first, word interleaved).

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_COMPR_OVER_PCM_PACKETIZED,
   /**< Compressed bitstreams packetized like PCM using a Qualcomm-designed
        packetizer.

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_GENERIC_COMPRESSED,
   /**< Compressed audio data bitstream that will be passed through as is,
        without knowing or modifying the underlying structure.

        Payload structure: capi_v2_standard_data_format_t @vertspace{6} */

   CAPI_V2_MAX_FORMAT_TYPE = 0x7FFFFFFF
   /**< Maximum value that a data format can take. */
};

typedef enum data_format_t data_format_t;

/** Used in any field of capi_v2_data_format_t to indicate that the value is
    unspecified. */
#define CAPI_V2_DATA_FORMAT_INVALID_VAL 0xFFFFFFFF

/** Valid values for data interleaving or deinterleaving.
 */
enum capi_v2_interleaving_t
{
  CAPI_V2_INTERLEAVED,
  /**< Data for all channels is present in one buffer. The samples are
       interleaved per channel. */

  CAPI_V2_DEINTERLEAVED_PACKED,
  /**< Data for all channels is present in one buffer. All of the samples of
       one channel are present, followed by all of the samples of
       the next channel, etc. */

  CAPI_V2_DEINTERLEAVED_UNPACKED,
  /**< Data for each channel is present in a different buffer. In this case,
       the capi_v2_stream_data_t::buf_ptr field points to an array of buffer
       structures with the number of elements equal to the number of channels.
       The bufs_num field must be set to the number of channels. */

  CAPI_V2_INVALID_INTERLEAVING = CAPI_V2_DATA_FORMAT_INVALID_VAL
  /**< Interleaving is not valid. */
};

typedef enum capi_v2_interleaving_t capi_v2_interleaving_t;

/** @} *//* end_addtogroup capiv2_data_types */

typedef struct capi_v2_data_format_header_t capi_v2_data_format_header_t;

/** @addtogroup capiv2_payload_structs
@{ */
/** Header structure for a data format that is passed into the module.
    Following this header is the appropriate media format payload.
 */
struct capi_v2_data_format_header_t
{
   data_format_t data_format;
   /**< Indicates the format in which the data is represented.
        The rest of the payload depends on the data format. @newpagetable */
};

typedef struct capi_v2_set_get_media_format_t capi_v2_set_get_media_format_t;

/** Header structure used to set and get a media format.
    Following this header is the appropriate media format payload.
 */
struct capi_v2_set_get_media_format_t
{
   capi_v2_data_format_header_t format_header;
   /**< Header of the media format. */
};

typedef struct capi_v2_standard_data_format_t capi_v2_standard_data_format_t;

/** Payload structure for FIXED_POINT, FLOATING_POINT, and
    IEC61937_PACKETIZED data formats.
 */
struct capi_v2_standard_data_format_t
{
   uint32_t bitstream_format;
   /**< Valid types are ADSP_MEDIA_FMT_* as defined in
        @xrefcond{Q3,80-NF774-1,80-NA610-1}. */
   /* adsp_media_fmt.h */

   uint32_t num_channels;
   /**< Number of channels. */

   uint32_t bits_per_sample;
   /**< Number of bits used to store each sample.

        For example, if the data is 24-bit audio packed in 24 bits,
        this value is 24. If it is 24-bit audio packed in 32 bits,
        this value is 32. */

   uint32_t q_factor;
   /**< Number of fractional bits in the fixed point representation of the
        data.

        If the data is floating point, this field must be set to
        #CAPI_V2_DATA_FORMAT_INVALID_VAL. */

   uint32_t sampling_rate;
   /**< Sampling rate in samples per second. */

   uint32_t data_is_signed;
   /**< Specifies whether data is signed.

        @values
        - 1 -- Signed
        - 0 -- Unsigned @tablebulletend */

   capi_v2_interleaving_t data_interleaving;
   /**< Indicates whether the data is interleaved. This value is not relevant
        for packetized data.

        @values
        - #CAPI_V2_INTERLEAVED
        - #CAPI_V2_DEINTERLEAVED_PACKED
        - #CAPI_V2_DEINTERLEAVED_UNPACKED @tablebulletend */

   uint16_t channel_type[CAPI_V2_MAX_CHANNELS];
   /**< Channel types for each of the num_channels. Valid channel types are
        the PCM_CHANNEL_* types defined in @xrefcond{Q3,80-NF774-1,80-NA610-1}.
        @newpagetable */
   /* adsp_media_fmt.h */
};

typedef struct capi_v2_raw_compressed_data_format_t capi_v2_raw_compressed_data_format_t;

/** Payload structure header for the RAW_COMPRESSED data format.
  Following this structure is the media format structure for the specific data
  format as defined in adsp_media_fmt.h.
 */
struct capi_v2_raw_compressed_data_format_t
{
   uint32_t bitstream_format;
   /**< Valid types are ADSP_MEDIA_FMT_* as defined in
        @xrefcond{Q3,80-NF774-1,80-NA610-1}. */
   /* adsp_media_fmt.h */
};

/** @} *//* end_addtogroup capiv2_payload_structs */

typedef struct capi_v2_port_info_t capi_v2_port_info_t;

/** @ingroup capiv2_data_types
  Port information payload structure.
 */
struct capi_v2_port_info_t
{
   bool_t is_valid;
   /**< Indicates whether port_index is valid.

        @values
        - 0 -- Not valid
        - 1 -- Valid @tablebulletend */

   bool_t is_input_port;
   /**< Indicates the type of port.

        @values
        - TRUE -- Input port
        - FALSE -- Output port @tablebulletend */

   uint32_t port_index;
   /**< Identifies the port.

        Index values must be sequential numbers starting from zero. There are
        separate sequences for input and output ports. For example, if a
        module has three input ports and two output ports:
        - The input ports have index values of 0, 1, and 2.
        - The output ports have index values of 0 and 1.

        When capi_v2_vtbl_t::process() is called:
        - The data in input[0] is for input port 0.
        - The data in input[1] is for input port 1.
        - Etc.
        - Output port 0 must fill data into output[0].
        - Output port 1 must fill data into output[1].
        - Etc. @tablebulletend @newpagetable */
};

#endif /* #ifndef CAPI_V2_TYPES_H */
