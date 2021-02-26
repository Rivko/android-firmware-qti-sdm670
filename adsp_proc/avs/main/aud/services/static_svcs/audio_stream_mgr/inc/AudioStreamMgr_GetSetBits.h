/**
@file AudioStreamMgr_GetSetBits.h
@brief This file defines functions used by AudioStreamMgr to get/set
specific bits in the message

*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/AudioStreamMgr_GetSetBits.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/09/2010 raj      Created file.
10/20/10   sw       (Tech Pubs) Edited/added Doxygen comments and markup.
12/16/10   sw       (TP) For Rev B of PDF, edited Doxygen comments in new
                    functions (lines 562 to EOF).
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_GETSETBITS_H
#define AUDIO_STREAM_MGR_GETSETBITS_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "adsp_asm_stream_commands.h"
#include "adsp_asm_session_commands.h"
#include "adsp_asm_service_commands.h"
#include "adsp_asm_data_commands.h"
#include "adsp_private_api_ext.h"

/** @addtogroup audio_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/**
  Gets the metadata present flag from the mode flags.

  @param[in] mode_flags   Mode flags of the ASM_STREAM_CMD_OPEN_* command.

  @return
  0 -- Metadata is not requested.\n
  1 -- Metadata is requested.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_meta_info_flag (uint32_t mode_flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_META_INFO_FLAG;
    uint16_t shift = ASM_SHIFT_META_INFO_FLAG;

    return ((mode_flags & bit_mask) >> shift );
}

/**
  Gets the timestamp type flag from the mode flags.

  @param[in] mode_flags   Mode flags of the ASM_STREAM_CMD_OPEN_* command.

  @return
   - ASM_RELATIVE_TIMESTAMP -- relative time-stamp.
   - ASM_ABSOLUTE_TIMESTAMP -- absolute time-stamp.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_timestamp_type_flag (uint32_t mode_flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG;
    uint16_t shift = ASM_SHIFT_TIMESTAMP_TYPE_FLAG;

    return ((mode_flags & bit_mask) >> shift );
}

/**
  Sets the metadata present flag in the mode flags.

  @param[in,out] mode_flags_ptr   Pointer to the mode flags of the
                                  ASM_STREAM_CMD_OPEN_* command.
  @param[in]     meta_info_flag   Metadata Information Requested flag:\n
                                  - 0 -- Metadata is not requested.
                                  - 1 -- Metadata is requested.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_meta_info_flag (uint32_t* mode_flags_ptr, uint32_t meta_info_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_META_INFO_FLAG;
    uint16_t shift = ASM_SHIFT_META_INFO_FLAG;
    uint32_t temp = 0;

    if (!mode_flags_ptr) return;

    temp = ( meta_info_flag << shift ) & bit_mask;
    *mode_flags_ptr = (*mode_flags_ptr & ~bit_mask) | temp;
}

/**
  Sets the timestamp type flag in the mode flags.

  @param[in,out] mode_flags_ptr   Pointer to the mode flags of the
                                  ASM_STREAM_CMD_OPEN_* command.
  @param[in]     timestamp_type_flag   timestamp type flag
                              - ASM_RELATIVE_TIMESTAMP -- relative time-stamp.
                              - ASM_ABSOLUTE_TIMESTAMP -- absolute time-stamp.
  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_timestamp_type_flag (uint32_t* mode_flags_ptr, uint32_t timestamp_type_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG;
    uint16_t shift = ASM_SHIFT_TIMESTAMP_TYPE_FLAG;
    uint32_t temp = 0;

    if (!mode_flags_ptr) return;

    temp = ( timestamp_type_flag << shift ) & bit_mask;
    *mode_flags_ptr = (*mode_flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the run starting time for the session run command.

  @param[in] payload_ptr    Pointer to the session run command payload.

  @return
  An ASM_SESSION_CMD_RUN_START_TIME_* macro.

  @dependencies
  None.
*/
static __inline ADSPResult asm_get_run_start_time(asm_session_cmd_run_v2_t *payload_ptr, uint32_t *start_type)
{
    uint32_t bitmask = ASM_BIT_MASK_RUN_START_TIME;
    uint16_t shift = ASM_SHIFT_RUN_START_TIME;

    *start_type = 0;
    if(!payload_ptr) return ADSP_EBADPARAM;

    *start_type = ((payload_ptr->flags & bitmask) >> shift);

    return ADSP_EOK;
}

/**
  Sets the run starting time for a session run command.

  @param[in,out] payload_ptr    Pointer to the session run command payload.
  @param[in]     start_time     One of the ASM_SESSION_CMD_RUN_START_TIME_*
                                macros.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_run_start_time(asm_session_cmd_run_v2_t *payload_ptr, uint32_t start_time)
{
    uint32_t bitmask = ASM_BIT_MASK_RUN_START_TIME;
    uint16_t shift = ASM_SHIFT_RUN_START_TIME;
    uint32_t temp = 0;

    if (!payload_ptr) return;

    temp = ( start_time << shift ) & bitmask;
    payload_ptr->flags = (payload_ptr->flags & ~bitmask) | temp;
}

/**
  Gets the timestamp valid flag.

  @param[in] flags    Bitfield flags.

  @return
  0 -- Timestamp is not valid.\n
  1 -- Timestamp is valid.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_timestamp_valid_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_TIMESTAMP_VALID_FLAG;
    uint16_t shift = ASM_SHIFT_TIMESTAMP_VALID_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the the timestamp valid flag.

  @param[in,out] flags_ptr         Pointer to the bitfield of the flags.
  @param[in]     timestamp_flag    Timestamp valid flag:\n
                                   - 0 -- Not valid.
                                   - 1 -- Valid.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_timestamp_valid_flag (uint32_t* flags_ptr, uint32_t timestamp_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_TIMESTAMP_VALID_FLAG;
    uint16_t shift = ASM_SHIFT_TIMESTAMP_VALID_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( timestamp_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the the frame metadata flag.

  @param[in] flags      Bitfield of the flags.

  @return
  0 -- Frame metadata is absent.\n
  1 -- Frame metadata is present.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_frame_metadata_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_FRAME_METADATA_FLAG;
    uint16_t shift = ASM_SHIFT_FRAME_METADATA_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the the frame metadata flag.

  @param[in,out] flags_ptr        Pointer to the bitfield of flags.
  @param[in]     metadata_flag    Metadata flag:\n
                                  - 0 -- Frame metadata is absent.
                                  - 1 -- Frame metadata is present.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_frame_metadata_flag (uint32_t* flags_ptr, uint32_t metadata_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_FRAME_METADATA_FLAG;
    uint16_t shift = ASM_SHIFT_FRAME_METADATA_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( metadata_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the the end-of-frame flag.

  @param[in] flags    Bitfield of the flags.

  @return
  0 -- End of frame is absent.\n
  1 -- End of frame is present.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_eof_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_EOF_FLAG;
    uint16_t shift = ASM_SHIFT_EOF_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the end-of-frame flag.

  @param[in,out] flags_ptr    Pointer to bitfield of the flags.
  @param[in]     eof_flag     End-of-frame flag:\n
                              - 0 -- End of frame is absent.
                              - 1 -- End of frame is present.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_eof_flag (uint32_t* flags_ptr, uint32_t eof_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_EOF_FLAG;
    uint16_t shift = ASM_SHIFT_EOF_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( eof_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the Sampling Rate/Channel Mode notify flag in the mode_flags field
  of stream open commands.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- Flag is not set.\n
  1 -- Flag is set.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_sr_cm_notify_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG;
    uint16_t shift = ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Gets the Gapless mode flag in the mode_flags field of stream open commands.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- Gapless mode is not set.\n
  1 -- Gapless mode is set.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_gapless_mode_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_GAPLESS_MODE_FLAG;
    uint16_t shift = ASM_SHIFT_GAPLESS_MODE_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the Gapless mode flag in the mode_flags field of stream open commands.

  @param[in,out] flags_ptr            Pointer to the mode_flags field.
  @param[in]     gapless_mode_flag    Gapless mode flag:\n
                                      - 0 -- Not gapless mode.
                                      - 1 -- Gapless mode.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_gapless_mode_flag (uint32_t* flags_ptr, uint32_t gapless_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_GAPLESS_MODE_FLAG;
    uint16_t shift = ASM_SHIFT_GAPLESS_MODE_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( gapless_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}


/**
  Gets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_WRITE_V(>=3) commands.

  @param[in] flags     Bitfield for the flag.

  @return
  ASM_LEGACY_STREAM_SESSION -- stream will be opened in legacy mode\n
  ASM_LOW_LATENCY_STREAM_SESSION -- stream will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_perf_mode_flag_in_open_write_cmd (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_WRITE_V(>=3) commands.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     stream_perf_mode_flag    stream_perf_mode flag:\n
                                            - refer to return values of
                                              asm_get_stream_perf_mode_flag_in_open_write_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_perf_mode_flag_in_open_write_cmd (uint32_t* flags_ptr, uint32_t stream_perf_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( stream_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}


/**
  Gets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_READ_V(>=3) commands.

  @param[in] flags     Bitfield for the flag.

  @return
  ASM_LEGACY_STREAM_SESSION -- stream will be opened in legacy mode\n
  ASM_LOW_LATENCY_STREAM_SESSION -- stream will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_perf_mode_flag_in_open_read_cmd (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_READ_V(>=3) commands.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     stream_perf_mode_flag    stream_perf_mode flag:\n
                                            - refer to return values of
                                              asm_get_stream_perf_mode_flag_in_open_read_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_perf_mode_flag_in_open_read_cmd (uint32_t* flags_ptr, uint32_t stream_perf_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( stream_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}


/**
  Gets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK command.

  @param[in] flags     Bitfield for the flag.

  @return
  ASM_LEGACY_STREAM_SESSION -- stream will be opened in legacy mode\n
  ASM_LOW_LATENCY_STREAM_SESSION -- stream will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_perf_mode_flag_in_open_transcode_loopback_cmd (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK command.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     stream_perf_mode_flag    stream_perf_mode flag:\n
                                            - refer to return values of
                                              asm_get_stream_perf_mode_flag_in_open_transcode_loopback_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_perf_mode_flag_in_open_transcode_loopback_cmd (uint32_t* flags_ptr, uint32_t stream_perf_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK ;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK ;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( stream_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the last buffer flag in the flags field of data write commands.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- Not the last buffer in the stream.\n
  1 -- Last buffer in the stream.

  @dependencies
  None.
 */
static __inline uint32_t asm_get_last_buffer_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_LAST_BUFFER_FLAG;
    uint16_t shift = ASM_SHIFT_LAST_BUFFER_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the last buffer flag in the flags field of data write commands.

  @param[in,out] flags_ptr           Pointer to the flags field.
  @param[in]     last_buffer_flag    Last buffer flag:\n
                                     - 0 -- Not the last buffer.
                                     - 1 -- Last buffer.

  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_last_buffer_flag (uint32_t* flags_ptr, uint32_t last_buffer_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_LAST_BUFFER_FLAG;
    uint16_t shift = ASM_SHIFT_LAST_BUFFER_FLAG;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( last_buffer_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Get timestamp continue flag in the flags field of
  asm_data_cmd_write_v2_t.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- TS continue flag is not set.\n
  1 -- TS continue flag is set.

  @dependencies
  None.
 */
static __inline uint32_t asm_get_ts_continue_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_TS_CONTINUE_FLAG;
    uint16_t shift = ASM_SHIFT_TS_CONTINUE_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Get the IEC 61937 packetization flag in the flags field of
  asm_stream_cmd_open_write_compressed_t.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- Stream is not IEC 61937 packetized.\n
  1 -- stream is IEC 61937 packetized.

  @dependencies
  None.
 */

static __inline uint32_t asm_get_IEC_61937_stream_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_IEC_61937_STREAM_FLAG;
    uint16_t shift = ASM_SHIFT_IEC_61937_STREAM_FLAG;

    return ((flags & bit_mask) >> shift);
}


/**
  Get the IEC 60958 packetization flag in the flags field of
  asm_stream_cmd_open_write_compressed_t.

  @param[in] flags     Bitfield for the flag.

  @return
  0 -- Stream is not IEC60958 packetized.\n
  1 -- Stream is IEC60958 packetized.

  @dependencies
  None.
 */

static __inline uint32_t asm_get_IEC_60958_stream_flag (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_IEC_60958_STREAM_FLAG;
    uint16_t shift = ASM_SHIFT_IEC_60958_STREAM_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Get the convert mode in the flags field of
  asm_stream_cmd_open_write_compressed_t.

  @param[in] flags     Bitfield for the flag.

  @return
  1 -- Decoder-converter Mode.
  other values are reserved for future purpose

  @dependencies
  None.
 */

static __inline uint32_t asm_get_decoder_converter_mode (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_DECODER_CONVERTER_FLAG;
    uint16_t shift = ASM_SHIFT_DECODER_CONVERTER_FLAG;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_PUSH_MODE_READ commands.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     stream_perf_mode_flag    stream_perf_mode flag:\n
                                            - refer to return values of
                                              asm_get_stream_perf_mode_flag_in_open_push_mode_read_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_perf_mode_flag_in_open_push_mode_read_cmd (uint32_t* flags_ptr, uint32_t stream_perf_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( stream_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Sets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE commands.

  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     stream_perf_mode_flag    stream_perf_mode flag:\n
                                            - refer to return values of
                                              asm_get_stream_perf_mode_flag_in_open_pull_mode_write_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_perf_mode_flag_in_open_pull_mode_write_cmd (uint32_t* flags_ptr, uint32_t stream_perf_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( stream_perf_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}

/**
  Gets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE commands.

  @param[in] flags     Bitfield for the flag.

  @return
  ASM_LEGACY_STREAM_SESSION -- stream will be opened in legacy mode\n
  ASM_LOW_LATENCY_STREAM_SESSION -- stream will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_perf_mode_flag_in_open_pull_mode_write_cmd (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE;
    uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE;

    return ((flags & bit_mask) >> shift);
}

/**
  Gets the stream_perf_mode flag in the mode_flags field of ASM_STREAM_CMD_OPEN_PUSH_MODE_READ commands.

  @param[in] flags     Bitfield for the flag.

  @return
  ASM_LEGACY_STREAM_SESSION -- stream will be opened in legacy mode\n
  ASM_LOW_LATENCY_STREAM_SESSION -- stream will be opened in low latency mode.
  Other values -- reserved for future use

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_perf_mode_flag_in_open_push_mode_read_cmd (uint32_t flags)
{
   uint32_t bit_mask = ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ;
   uint16_t shift = ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ;

    return ((flags & bit_mask) >> shift);
}
/**
  Gets the bit stream flush mode flag in the mode_flags field of ASM_STREAM_CMD_FLUSH_V2 commands.

  @dependencies
  None.
*/
static __inline uint32_t asm_get_stream_bitstream_flush_mode_flag_in_flush_v2_cmd (uint32_t flags)
{
    uint32_t bit_mask = ASM_BIT_MASK_FLUSH_V2_MODE;
    uint16_t shift = ASM_SHIFT_MASK_FLUSH_MODE_NOT_TO_FLUSH_BISTREAM;

    return ((flags & bit_mask) >> shift);
}

/**
  Sets the bit stream flush mode flag in the mode_flags field of ASM_STREAM_CMD_FLUSH_V2 commands.
  @param[in,out] flags_ptr                Pointer to the mode_flags field.
  @param[in]     bitstream_flush_mode_flag    bitstream_flush_mode flag:\n


                                            - refer to return values of
                                              asm_set_stream_bitstream_flush_mode_flag_in_flush_v2_cmd().


  @return
  None.

  @dependencies
  None.
*/
static __inline void asm_set_stream_bitstream_flush_mode_flag_in_flush_v2_cmd (uint32_t* flags_ptr, uint32_t bitstream_flush_mode_flag)
{
    uint32_t bit_mask = ASM_BIT_MASK_FLUSH_V2_MODE;
    uint16_t shift = ASM_SHIFT_MASK_FLUSH_MODE_NOT_TO_FLUSH_BISTREAM;
    uint32_t temp = 0;

    if (!flags_ptr) return;

    temp = ( bitstream_flush_mode_flag << shift ) & bit_mask;
    *flags_ptr = (*flags_ptr & ~bit_mask) | temp;
}
/** @} */ /* end_addtogroup audio_stream */

#endif // #ifndef AUDIO_STREAM_MGR_GETSETBITS_H

