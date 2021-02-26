/*========================================================================*/
/**
@file adsp_asm_data_commands.h

This file contains ASM data commands and events structures definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
======================================================================== */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_asm_data_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/20/17   sw      (Tech Pubs) Edited Doxygen comments for AVS.2.8 (SDM845).
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS.2.8 (MSM8998).
10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
05/08/15   sw      (Tech Pubs) Edited Doxygen comments for 8952.
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0.
08/06/12   sw      (Tech Pubs) Edited comments; updated Doxygen markup to
                   Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_ASM_DATA_COMMANDS_H_
#define _ADSP_ASM_DATA_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asmdata_cmd_write_data
@{ */
/** Delivers a data buffer to the ASM.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_WRITE_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_write_v2_t}
    @table{weak__asm__data__cmd__write__v2__t}

  @detdesc
    If the client wants the aDSP to use valid timestamps in this command, and
    the data sent through this command is in RAW format, the last data
    byte must constitute an end of frame. The end-of-frame flag (bit&nbsp;4
    of the flags field) must be set to 1. Otherwise, the aDSP will not
    be able to handle the timestamps correctly, which can result in gaps
    during audio playback.
    @par
    When the buffer delivers uncompressed PCM data (in
    #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 format), the buffer must contain the
    same number of PCM samples for all channels. If not, the aDSP discards the
    buffer and generates an #ASM_DATA_EVENT_WRITE_DONE_V2 event with the
    ADSP_EBADPARAM error code. (For more details on the error code, refer to
    @xrefcond{Q5,80-NF774-4,80-NA610-4}.)

  @return
    #ASM_DATA_EVENT_WRITE_DONE_V2 is raised when the buffer is fully consumed.

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_DATA_CMD_WRITE_V2                                          0x00010DAB

/* Payload for an ASM Data Write command. */
typedef struct asm_data_cmd_write_v2_t asm_data_cmd_write_v2_t;

/** Definition of a timestamp valid flag bitmask. */
#define ASM_BIT_MASK_TIMESTAMP_VALID_FLAG                           (0x80000000UL)

/** Definition of a timestamp valid flag shift value. */
#define ASM_SHIFT_TIMESTAMP_VALID_FLAG                               31

/** Definition of the last buffer flag bitmask. The last buffer flag is part
    of the flags word in the ASM_DATA_CMD_WRITE_V2 command.
*/
#define ASM_BIT_MASK_LAST_BUFFER_FLAG                               (0x40000000UL)

/** Definition of the last buffer flag shift value. The last buffer flag is
    part of the flags word in the ASM_DATA_CMD_WRITE_V2 command.
*/
#define ASM_SHIFT_LAST_BUFFER_FLAG                                   30

/** Definition of the shift value for the timestamp continue flag.
    The timestamp continue flag is part of the flags field in the
    asm_data_cmd_write_v2_t structure.
*/
#define ASM_BIT_MASK_TS_CONTINUE_FLAG                               (0x20000000UL)

/** Definition of the shift value for the timestamp continue flag.
    The timestamp continue flag is part of the flags field in the
    asm_data_cmd_write_v2_t structure.
*/
#define ASM_SHIFT_TS_CONTINUE_FLAG                                   29

/** Definition of an End of Frame (EOF) flag bitmask. */
#define ASM_BIT_MASK_EOF_FLAG                                       (0x00000010UL)

/** Definition of an EOF flag shift value. */
#define ASM_SHIFT_EOF_FLAG                                           4

/** @} *//* end_addtogroup asmdata_cmd_write_data */

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_write_v2_t
@{ */
/* Payload of the ASM_DATA_CMD_WRITE_V2 command, which delivers a data
    buffer to the ASM for decoding.
*/
struct asm_data_cmd_write_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< Lower 32 bits of the address of the buffer containing the data to be
         decoded. */

    uint32_t                  buf_addr_msw;
    /**< Upper 32 bits of the address of the buffer containing the data to be
         decoded.

         The buffer address for each frame must be a valid physical address
         that was mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
         aligned to a 32-byte boundary.

         @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw field
         must be set to 0.

         @tblsubhd{For a 36-bit address} Bits 31 to 4 of this
         buf_addr_msw field must be set to 0. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  buf_size;
    /**< Number of valid bytes available in the buffer for decoding. The
         first byte starts at the buffer address.

         @values @ge 0

         For PCM data, the buf_size must be a multiple of 
         (number of channels * bytes per sample). Bytes per sample is 2 for
         16-bit data, 4 for 24-bit unpacked data, and 3 for 24-bit packed data. */

    uint32_t                  seq_id;
    /**< Optional buffer sequence ID. */

    uint32_t                  timestamp_lsw;
    /**< Lower 32 bits of the 64-bit session time in microseconds of the
         first buffer sample. */

    uint32_t                  timestamp_msw;
    /**< Upper 32 bits of the 64-bit session time in microseconds of the
         first buffer sample. */

    uint32_t                  flags;
    /**< Bitfield of flags.

         @values{for bit 31}
         - 1 -- Valid timestamp
         - 0 -- Invalid timestamp
         - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
           #ASM_SHIFT_TIMESTAMP_VALID_FLAG

         @values{for bit 30}
         - 1 -- Last buffer
         - 0 -- Not the last buffer
         - To set this bit, use #ASM_BIT_MASK_LAST_BUFFER_FLAG and
           #ASM_SHIFT_LAST_BUFFER_FLAG

         @values{for bit 29}
         - 1 -- Continue the timestamp from the previous buffer
         - 0 -- Timestamp of the current buffer is not related to the timestamp
                of the previous buffer
         - To set this bit, use #ASM_BIT_MASK_TS_CONTINUE_FLAG and
           #ASM_SHIFT_TS_CONTINUE_FLAG

         @values{for bit 4 (end-of-frame flag)}
         - 1 -- End of the frame
         - 0 -- Not the end of frame, or this information is not known
         - To set this bit, use #ASM_BIT_MASK_EOF_FLAG as the bitmask and
           #ASM_SHIFT_EOF_FLAG

         All other bits are reserved; clients must set them to 0.

         @tblsubhd{If bit 31=0 and bit 29=1}
         The timestamp of the first sample in this buffer continues from the
         timestamp of the last sample in the previous buffer.

         The samples in the current buffer do not have a valid timestamp in
         the following situations:
         - If there is no previous buffer (this buffer is the first
           buffer sent after opening the stream or after a flush operation)
         - If the previous buffer does not have a valid timestamp

         In these cases, the samples in the current buffer are played out as
         soon as possible.

         @tblsubhd{If bit 31=0 and bit 29=0}
         No timestamp is associated with the first sample in this buffer. The
         samples are played out as soon as possible.

         @tblsubhd{If bit 31=1 and bit 29 is ignored}
         The timestamp specified in this payload is honored.

         @tblsubhd{If bit 30=0}
         This buffer is not the last buffer in the stream. This case is useful
         in removing trailing samples.

         @contcell
         @tblsubhd{For bit 4}
         The client can set this end-of-frame flag for every buffer sent in
         which the last byte is the end of a frame. If this flag is set, the
         buffer can contain data from multiple frames, but it must always end
         at a frame boundary. Restrictions allow the aDSP to detect an end of
         frame without requiring additional processing. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_write_v2_t */

/** @ingroup asmdata_cmd_read
    Delivers an empty data buffer to be filled by the ASM.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_READ_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_read_v2_t}
    @tablens{weak__asm__data__cmd__read__v2__t} @newpage

  @return
    #ASM_DATA_EVENT_READ_DONE_V2 is raised when the buffer is consumed.

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_DATA_CMD_READ_V2                                         0x00010DAC

/* Payload for an ASM Data Read command. */
typedef struct asm_data_cmd_read_v2_t asm_data_cmd_read_v2_t;

#include "adsp_begin_pack.h"
                      
/** @weakgroup weak_asm_data_cmd_read_v2_t
@{ */
/* Payload of the ASM_DATA_CMD_READ_V2 command, which delivers a data
    buffer to the ASM for encoded data. */
/** The buffer address for each frame must be a physical address that was
    mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
*/
struct asm_data_cmd_read_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< Lower 32 bits of the address of the buffer where the aDSP puts the
         encoded data. */

    uint32_t                  buf_addr_msw;
    /**< Upper 32 bits of the address of the buffer where the aDSP puts the
         encoded data.

         The buffer address for each frame must be a valid physical address
         that was mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         The address can be at an offset specified in the offset field of
         #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2.

         The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
         aligned to a 32-byte boundary.

         @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw
         field must be set to 0.

         @tblsubhd{For a 36-bit address} Bits 31 to 4 of this
         buf_addr_msw field must be set to 0. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from the buffer address.

         @values > 0  */

    uint32_t                  seq_id;
    /**< Optional buffer sequence ID. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_read_v2_t */

/** @ingroup asmdata_cmd_read_compressed
    Delivers an empty data buffer to be filled by the ASM in Compressed Read
    mode.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_READ_COMPRESSED_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_read_compressed_v2_t}
    @table{weak__asm__data__cmd__read__compressed__v2__t}

  @return
    #ASM_DATA_EVENT_READ_DONE_V2 is raised when the buffer is consumed, or if
    #ASM_STREAM_CMD_FLUSH is issued.

  @dependencies
    This command must be applied to a valid session opened using the
    #ASM_STREAM_CMD_OPEN_READ_COMPRESSED command.
    @par
    The memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_DATA_CMD_READ_COMPRESSED_V2                                0x00010DC1

/* Payload for an ASM Data Read Compressed command. */
typedef struct asm_data_cmd_read_compressed_v2_t asm_data_cmd_read_compressed_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_read_compressed_v2_t
@{ */
/* Payload of the ASM_DATA_CMD_READ_COMPRESSED_V2 command, which delivers
    a data buffer to the ASM for encoded data.
*/
struct asm_data_cmd_read_compressed_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< Lower 32 bits of the address of the buffer where the aDSP puts the
         encoded data. */

    uint32_t                  buf_addr_msw;
    /**< Upper 32 bits of the address of the buffer where the aDSP puts the
         encoded data.

         The buffer address for each frame must be a valid physical address
         that was mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         The address can be at an offset specified in the offset field of
         #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2.

         The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
         aligned to a 32-byte boundary.

         @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw
         field must be set to 0.

         @tblsubhd{For a 36-bit address} Bits 31 to 4 of this
         buf_addr_msw field must be set to 0. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from the buffer address.

         @values > 0  */

    uint32_t                  seq_id;
      /**< Optional buffer sequence ID. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_read_compressed_v2_t */

/** @ingroup asmdata_cmd_eos
    Communicates an End of Stream (EoS) marker, which indicates that the last
    buffer in a stream has been delivered. For two or more streams routed to
    the same AFE port, the client sends unique token IDs in the APR header of
    the EoS command.

    Because the AFE generates #ASM_DATA_EVENT_RENDERED_EOS with the same token
    ID, the client can correlate the events with the EoS commands it sent for
    each stream.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_EOS
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    #ASM_DATA_EVENT_RENDERED_EOS is raised when the final sample before the
    EoS has been rendered.

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_DATA_CMD_EOS                                            0x00010BDB

/** @addtogroup asmdata_cmd_eosv2
@{ */
/** Communicates an EoS marker, which indicates that the last buffer in a
    stream was delivered. Data is read from a write or read/write stream that
    reached the EoS.

    Compared to #ASM_DATA_CMD_EOS, which results in
    #ASM_DATA_EVENT_RENDERED_EOS, this version 2 command results in
    #ASM_DATA_EVENT_RENDERED_EOS_V2.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_EOS_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_eos_v2_t}
    @table{weak__asm__data__cmd__eos__v2__t}

  @return
    #ASM_DATA_EVENT_RENDERED_EOS_V2 is raised when the final sample before the
    EoS has been rendered.

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_DATA_CMD_EOS_V2                                         0x00012F3C

/** Bit shift required on the mask to determine if
    #ASM_DATA_EVENT_RENDERED_EOS_V2 is raised or if any connected devices
    raise the EoS.
 */
#define ASM_BIT_SHIFT_EOS_V2_RAISE_IF_ANY_OR_ALL 0

/** Bitmask required to determine if #ASM_DATA_EVENT_RENDERED_EOS_V2 is
    raised or if any connected devices are to raise the EoS.
 */
#define ASM_BIT_MASK_EOS_V2_RAISE_IF_ANY_OR_ALL (0x00000001UL)

/** Bit value for #ASM_BIT_SHIFT_EOS_V2_RAISE_IF_ANY_OR_ALL when
    #ASM_DATA_EVENT_RENDERED_EOS_V2 is raised after any connected device has
    rendered EoS.
 */
#define ASM_BIT_EOS_V2_RAISE_IF_ANY 0

/** Bit value for #ASM_BIT_SHIFT_EOS_V2_RAISE_IF_ANY_OR_ALL when
    #ASM_DATA_EVENT_RENDERED_EOS_V2 is raised after all connected devices have
    rendered EoS.
 */
#define ASM_BIT_EOS_V2_RAISE_IF_ALL 1

/** @} *//* end_addtogroup asmdata_cmd_eosv2 */

/* Payload for an ASM Data EOS V2 command. */
typedef struct asm_data_cmd_eos_v2_t asm_data_cmd_eos_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_eos_v2_t
@{ */
/* Payload of the ASM_DATA_CMD_EOS_V2 command, which indicates end of stream
*/
struct asm_data_cmd_eos_v2_t
{
    uint32_t                  mask;
    /**< Mask that is used to decide the timing of
         #ASM_DATA_EVENT_RENDERED_EOS_V2.

         @values{for bit 0}
         - #ASM_BIT_EOS_V2_RAISE_IF_ANY
         - #ASM_BIT_EOS_V2_RAISE_IF_ALL (Default)

         All other bits are reserved. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_eos_v2_t */


/** @ingroup asmdata_cmd_mark_buffer
    Communicates a marked buffer containing a token for identification.
    When the marked buffer has been lost or processed, the relevant service
    raises an event (#ASM_DATA_EVENT_MARK_BUFFER_V2) using the token.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_MARK_BUFFER_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @msgpayload{asm_data_cmd_mark_buffer_v2_t}
    @table{weak__asm__data__cmd__mark__buffer__v2__t}

  @return
    None

  @dependencies
    The session/stream must be a valid and opened write, read/write, or
    compressed write session/stream.
*/
#define ASM_DATA_CMD_MARK_BUFFER_V2                                      0x00012F15
/* Structure for a Mark Buffer command. */
typedef struct asm_data_cmd_mark_buffer_v2_t asm_data_cmd_mark_buffer_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_mark_buffer_v2_t
@{ */
/* Payload of the #ASM_DATA_CMD_MARK_BUFFER_V2 command, which indicates an
   update to the media format that applies to future buffers written to this
   stream.
*/
struct asm_data_cmd_mark_buffer_v2_t
{
    uint32_t                    token_lsw;
    /**< Lower 32 bits of the token value. The aDSP does not care about the
         value. */

    uint32_t                    token_msw;
    /**< Upper 32 bits of the token value. The aDSP does not care about the
         value. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_mark_buffer_v2_t */

/** @addtogroup asmdata_event_mark_buffer
@{ */
/** Defines the event for a marked buffer that is either processed or lost.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_MARK_BUFFER_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @msgpayload{asm_data_event_mark_buffer_v2_t}
    @table{weak__asm__data__event__mark__buffer__v2__t}

  @return
    None

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_DATA_EVENT_MARK_BUFFER_V2                                      0x00012F16

/* Structure for a Mark Buffer event. */
typedef struct asm_data_event_mark_buffer_v2_t asm_data_event_mark_buffer_v2_t;

/** Indicates that a marked buffer has been processed. */
#define ASM_DATA_EVENT_MARK_BUFFER_PROCESSED                              0

/** Indicates that a marked buffer has been discarded. */
#define ASM_DATA_EVENT_MARK_BUFFER_DISCARDED                              1

/** @} *//* end_addtogroup asmdata_event_mark_buffer */

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_mark_buffer_v2_t
@{ */
/* Payload of the #ASM_DATA_EVENT_MARK_BUFFER_V2 command, which indicates an event
 * regarding an adsp marked buffer is either processed or lost
*/
struct asm_data_event_mark_buffer_v2_t
{
    uint32_t                    token_lsw;
    /**< Lower 32 bits of the token value. The aDSP does not care about the
         value. */

    uint32_t                    token_msw;
    /**< Upper 32 bits of the token value. The aDSP does not care about the
         value. */

    uint32_t                    result;
    /**< Result of the marked buffer.

         @values
         - #ASM_DATA_EVENT_MARK_BUFFER_PROCESSED
         - #ASM_DATA_EVENT_MARK_BUFFER_DISCARDED @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_mark_buffer_v2_t */

/** @ingroup asmdata_cmd_media_format_update
    Indicates an update to the media format, which applies to future buffers
    written to this stream.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_media_fmt_update_t}
    @tablens{weak__asm__data__cmd__media__fmt__update__t}

  @detdesc
    This message must be properly sequenced with corresponding
    #ASM_DATA_CMD_WRITE_V2 messages (it must be sent on the data path).
    @par
    The client also sends this command after opening a write stream or
    read/write stream and before sending any #ASM_DATA_CMD_WRITE_V2 commands.
    This command is required to configure the decoder properly. If
    ASM_DATA_CMD_WRITE_V2 commands arrive before this command, the data buffers
    are discarded and #ASM_DATA_EVENT_WRITE_DONE_V2 events are raised with an
    error status for these commands.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session/stream must be a valid and opened write or read/write
    session/stream.
*/
#define ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2                            0x00010D98

/* Structure for a Media Format Update command. */
typedef struct asm_data_cmd_media_fmt_update_v2_t asm_data_cmd_media_fmt_update_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_media_fmt_update_t
@{ */
/* Payload of the #ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command, which indicates an
   update to the media format that applies to future buffers written to this
   stream. */
/** Immediately following this structure is a media format block of size
    fmt_blk_size.
*/
struct asm_data_cmd_media_fmt_update_v2_t
{
    uint32_t                    fmt_blk_size;
    /**< Media format block size in bytes. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_media_fmt_update_t */

/** @ingroup asmstrm_evt_dtmf_tone_end
    Indicates that an ASM stream event for DTMF tone has ended. The AFE raises
    this event for write streams.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_EVENT_DTMF_TONE_ENDED
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    After completion of DTMF tone generation, the aDSP sends this
    event to the client under the following conditions:
      - When a finite duration tone is completely played out
      - When an infinite duration tone is stopped
      - When a tone is pre-empted by a new tone while still playing
      - By the destination AFE port when the last sample has been rendered,
        like the #ASM_DATA_EVENT_RENDERED_EOS event.
    @par
    The destination port in this event is same as the source port sent in
    the #ASM_STREAM_CMD_DTMF_CTL command.
    @par
    This event cannot be raised when the client sends a flush command
    immediately after sending the tone end command. The client is responsible
    for tagging each DTMF command (such as using a counter value in the APR
    token field) so it can safely ignore the ASM_DATA_EVENT_RENDERED_EOS
    event that has been flushed.

  @return
    None.

  @dependencies
    None.
*/
#define ASM_STREAM_EVENT_DTMF_TONE_ENDED                           0x00010C1E

/** @ingroup asmdata_event_rendered_eos
    Indicates that the last sample in the write or read/write stream has been
    successfully rendered after an EoS data command is received, and that no
    more samples will be rendered.

    This event is raised by the AFE for tunneled playback streams, or by the
    encoder service for non-tunneled streams.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_RENDERED_EOS
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    The stream must be open for write.
*/
#define ASM_DATA_EVENT_RENDERED_EOS                                 0x00010C1C

/** @ingroup asmdata_event_rendered_eosv2
    Indicates that the last sample in the write or read/write stream was
    successfully rendered after #ASM_DATA_CMD_EOS_V2 was received, and that
    no more samples will be rendered.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_RENDERED_EOS_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    This event is raised as follows for #ASM_DATA_CMD_EOS_V2:
    - By the matrix for tunneled playback streams
    - By the decoder for ULL and ULL-PP on the stream side
    - By the encoder for non-tunneled streams

  @return
    None.

  @dependencies
    The stream must be open for write.
*/
#define ASM_DATA_EVENT_RENDERED_EOS_V2                              0x00012F3D


/** @ingroup asmdata_event_early_eos
    Indicates that the #ASM_DATA_EVENT_RENDERED_EOS_V2 event will occur at
    the time specified in #ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_EARLY_EOS
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    The stream must be open for write in Gapless mode.
    @par
    #ASM_DATA_CMD_EOS and #ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION
    must have been sent.
*/
#define ASM_DATA_EVENT_EARLY_EOS                                 0x00012F3A

/** @ingroup asmdata_event_eos
    Indicates that data is being read from a read or read/write stream that
    has reached the EoS.

    @note1hang Do not confuse this event with the acknowledgment to
    #ASM_DATA_CMD_EOS received on a write stream. This ASM_DATA_EVENT_EOS is
    an independent event generated toward the client.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_EOS
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    The stream must be open for read or read/write.
*/
#define ASM_DATA_EVENT_EOS                                          0x00010BDD

/** @ingroup asmdata_event_write_done
    Indicates that the referenced buffer has been fully consumed and is again
    available to the client.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_WRITE_DONE_V2
    @par
    Token  -- Matches the token in the #ASM_DATA_CMD_WRITE_V2 command for
              which this event is being raised
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_write_done_v2_t}
    @table{weak__asm__data__event__write__done__v2__t}

  @return
    None.

  @dependencies
    The session/stream must be a valid and open write or read/write
    session/stream.
    @par
    A corresponding #ASM_DATA_CMD_WRITE_V2 command must have been received and
    processed.
*/
#define ASM_DATA_EVENT_WRITE_DONE_V2                                   0x00010D99

/* Structure for a data write done event payload. */
typedef struct asm_data_event_write_done_v2_t asm_data_event_write_done_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_write_done_v2_t
@{ */
/* Payload of the ASM_DATA_EVENT_WRITE_DONE_V2 event, which indicates
    that the referenced buffer has been fully consumed and is again available
    to the client.    
*/
struct asm_data_event_write_done_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< Lower 32 bits of the address of the buffer being returned. */

    uint32_t                  buf_addr_msw;
    /**< Upper 32 bits of the address of the buffer being returned.

         The valid, mapped, 64-bit physical address is the same address that
         the client provides in #ASM_DATA_CMD_WRITE_V2. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_DATA_CMD_WRITE_V2. */

    uint32_t                  status;
    /**< Status message (error code) that indicates whether the referenced
         buffer has been successfully consumed.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_write_done_v2_t */

/** @addtogroup asmdata_event_read_done
@{ */
/** Indicates that the referenced buffer has been filled and is available to
    the client.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_READ_DONE_V2
    @par
    Token  -- Matches the token in the #ASM_DATA_CMD_READ_V2 command for which
              this event is being raised
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_read_done_v2_t}
    @table{weak__asm__data__event__read__done__v2__t}

  @detdesc
    The referenced buffer contains an optional array of metadata if the client
    requested it in an open command. The buffer is followed by a variable
    amount of empty space, assuming metadata is present, followed by an integer
    number of encoded frames. Metadata presence is indicated by bit 30 of the
    flags field in the APR message payload structure.

  @par Frame metadata (asm_data_read_buf_metadata_v2_t)
    Information for each encoded frame is inside the buffer. If bit 30 of the
    flags field is set to 1, this metadata structure is present in the buffer
    when ASM_DATA_EVENT_READ_DONE_V2 is issued. Following this metadata
    structure is the resulting buffer from the read event (see
    <b>Buffer format</b>).
    @tablens{weak__asm__data__read__buf__metadata__v2__t}

  @par Buffer format
    A multiframe buffer becomes an array of Hexagon frame metadata
    information structures, and then an array of audio frames. Encoded
    frames start with <i>offset</i>.
    @verbatim
    MetaData 0 (optional)
    MetaData 1 (optional)
       . . .
    MetaData n (optional)
    Extra space determined by the Hexagon processor (optional)
    Frame 0 Payload (this always starts at an offset from buf_addr)
    Frame 1 Payload
       . . .
    Frame n Payload

    @endverbatim
    @par
    Resulting buffer from a read event: \n \n
    @inputtable{Frame_metadata_buffer_content.tex}

  @return
    None.

  @dependencies
    The session/stream must be a valid and open write or read/write
    session/stream.
    @par
    A corresponding #ASM_DATA_CMD_READ_V2 command must have been received and
    processed.
*/
#define ASM_DATA_EVENT_READ_DONE_V2                                    0x00010D9A

/** Definition of the frame metadata flag bitmask. */
#define ASM_BIT_MASK_FRAME_METADATA_FLAG                            (0x40000000UL)

/** Definition of the frame metadata flag shift value. */
#define ASM_SHIFT_FRAME_METADATA_FLAG                               30

/** @} *//* end_addtogroup asmdata_event_read_done */

/* Structure for the data read done event payload. */
typedef struct asm_data_event_read_done_v2_t asm_data_event_read_done_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_read_done_v2_t
@{ */
/* Payload of the ASM_DATA_EVENT_READ_DONE_V2 event, which indicates
    that the referenced buffer has been filled and is available to the client.
*/
struct asm_data_event_read_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

        @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */

   uint32_t                  buf_addr_lsw;
   /**< Lower 32 bits of the valid, mapped address of the buffer containing
        the decoded data. */

   uint32_t                  buf_addr_msw;
   /**< Upper 32 bits of the valid, mapped address of the buffer containing
        the decoded data. 

        The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
        aligned to a 32-byte boundary. This address is the same as the
        address provided by the client in #ASM_DATA_CMD_READ_V2.

        @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw field
        must be set to 0.

        @tblsubhd{For a 36-bit address} Bits 31 to 4 of this
        buf_addr_msw field must be set to 0. */

   uint32_t                  mem_map_handle;
   /**< Unique identifier for the shared memory address.

        The aDSP returns this handle through #ASM_DATA_CMD_READ_V2. */

   uint32_t                  enc_frames_total_size;
   /**< Total size of the encoded frames in bytes.

        @values @ge 0  */

   uint32_t                  offset;
   /**< Offset from the buffer address to the first byte of the first encoded
        frame. All encoded frames are consecutive, starting from this offset.

        @values > 0 */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer. */

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer.

        If bit 5 of the mode_flags field of #ASM_STREAM_CMD_OPEN_READ_V3 is set
        to 1, the 64-bit timestamp is an absolute capture time. Otherwise, it
        is a relative session time. The absolute timestamp does not reset
        unless the system is reset. */

   uint32_t                  flags;
   /**< Bit field of flags.

        @values{for bit 31}
        - 1 -- Timestamp is valid
        - 0 -- Timestamp is invalid
        - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG

        @values{for bit 30}
        - 1 -- Frame metadata is present
        - 0 -- Frame metadata is absent
        - To set this bit, use #ASM_BIT_MASK_FRAME_METADATA_FLAG and
          #ASM_SHIFT_FRAME_METADATA_FLAG

        All other bits are reserved; the aDSP sets them to 0.

        If bit 30 is set to 1, num_frames of consecutive instances of
        %asm_data_read_buf_metadata_v2_t start at the buffer address
        (see <b>Buffer format</b>). */

   uint32_t                  num_frames;
   /**< Number of encoded frames in the buffer. */

   uint32_t                  seq_id;
   /**< Optional buffer sequence ID. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_read_done_v2_t */

/* Structure for metadata that can be put in the data read buffer. */
typedef struct asm_data_read_buf_metadata_v2_t asm_data_read_buf_metadata_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_read_buf_metadata_v2_t
@{ */
/* Payload of the metadata that can be put in the data read buffer.
*/
struct asm_data_read_buf_metadata_v2_t
{
   uint32_t          offset;
   /**< Offset from the buffer address in %asm_data_event_read_done_v2_t to
        the frame associated with the metadata.

        @values > 0 */

   uint32_t          frm_size;
   /**< Size of the encoded frame in bytes.

        @values > 0 */

   uint32_t          num_encoded_pcm_samples;
   /**< Number of encoded PCM samples (per channel) in the frame
        associated with the metadata.

        @values > 0 */

   uint32_t          timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame. */

   uint32_t          timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame.

        If bit 5 of the mode_flags field of #ASM_STREAM_CMD_OPEN_READ_V3 is set
        to 1, the 64-bit timestamp is an absolute capture time. Otherwise, it
        is a relative session time. The absolute timestamp does not reset
        unless the system is reset. */

   uint32_t          flags;
   /**< Frame flags.

        @values{for bit 31}
        - 1 -- Timestamp is valid
        - 0 -- Timestamp is not valid
        - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG

        All other bits are reserved; the aDSP sets them to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_read_buf_metadata_v2_t */

/** @ingroup asmdata_event_read_compr_done
    Indicates that the referenced buffer has been filled and is available to
    the client.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2
    @par
    Token  -- Matches the token in the #ASM_DATA_CMD_READ_COMPRESSED_V2 command
              for which this event is being raised
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_read_compressed_done_v2_t}
    If bit 30 of the flags field is set to 1, frame metadata is present in
    the buffer (the number of frames indicated in the event payload is
    greater than zero). In this case, num_frames consecutive instances of
    <b>Frame metadata</b> start at the buffer address.
    @tablens{weak__asm__data__event__read__compressed__done__v2__t}

  @par Frame metadata (asm_data_read_buf_metadata_compressed_v2_t)
    Information for each encoded frame is inside the buffer. If bit 30 of the
    flags field is set to 1, this metadata structure is present in the buffer
    when #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 is issued. Following this
    metadata structure is the resulting buffer from the read event (see
    (see <b>Buffer format</b>).
    @tablens{weak__asm__data__read__buf__metadata__compressed__v2__t}

  @par Buffer format
    A multiframe buffer becomes an array of Hexagon frame metadata
    information structures, and then an array of audio frames. Encoded
    frames start with <i>offset</i>.
    @verbatim
       MetaData 0 (optional)
       MetaData 1 (optional)
          . . .
       MetaData n (optional)
       Extra space determined by the Hexagon processor (optional)
       Frame 0 Payload (this always starts at an offset from buf_addr)
       Frame 1 Payload
          . . .
       Frame n Payload
    @endverbatim @newpage
    @par
    Resulting buffer from a read event: \n \n
    @inputtable{Frame_metadata_buffer_content.tex}

  @detdesc
    The #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 event is raised when the
    specified number of frames are filled in the buffer. The client can force
    this event by issuing #ASM_STREAM_CMD_FLUSH.
    @par
    The referenced buffer contains an optional array of metadata if the client
    requested it in an open command. The buffer is followed by a variable
    amount of empty space, assuming metadata is present, followed by an
    integer number of encoded frames. Metadata presence is indicated by
    bit 30 of the flags field in the APR message payload structure.

  @return
    None.

  @dependencies
    This command must be applied to a valid stream opened using the
    #ASM_STREAM_CMD_OPEN_READ_COMPRESSED command.
    @par
    A corresponding #ASM_DATA_CMD_READ_COMPRESSED_V2 command must have been
    received and processed.
*/
#define ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2                        0x00010DC2

/* Structure for the data read done event payload. */
typedef struct asm_data_event_read_compressed_done_v2_t asm_data_event_read_compressed_done_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_read_compressed_done_v2_t
@{ */
/* Payload of the ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 event, which
    indicates that the referenced buffer has been filled and is available to
    the client.
*/
struct asm_data_event_read_compressed_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

        @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */

   uint32_t                  buf_addr_lsw;
   /**< Lower 32 bits of the address of the buffer where the aDSP put the
        encoded data. */

   uint32_t                  buf_addr_msw;
   /**< Upper 32 bits of the address of the buffer where the aDSP put the
        encoded data.

        The client provides this address in #ASM_DATA_CMD_READ_COMPRESSED_V2.
        The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
        aligned to a 32-byte boundary.

        @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw field
        must be set to 0.

        @tblsubhd{For a 36-bit address} Bits 31 to 4 of this
        buf_addr_msw field must be set to 0. */

   uint32_t                  mem_map_handle;
   /**< Unique identifier for the shared memory address.

        The aDSP returns this handle through #ASM_DATA_CMD_READ_COMPRESSED_V2. */

   uint32_t                  enc_frames_total_size;
   /**< Total size of the encoded frames. This field is set to 0 if no frames
        are present in the buffer.

        @values @ge 0  bytes */

   uint32_t                  offset;
   /**< Offset from the buffer address to the first byte of the first encoded
        frame. All encoded frames are consecutive, starting from this offset.

        @values > 0 */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame. */

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame.

        The timestamp is valid only if there is at least one valid frame.

        If bit 5 of the mode_flags field of #ASM_DATA_CMD_READ_COMPRESSED_V2
        is set to 1, the 64-bit timestamp is an absolute capture time.
        Otherwise, it is a relative session time. The absolute timestamp does
        not reset unless the system is reset. */

   uint32_t                  flags;
   /**< Bit field of flags.

        @values{for bit 31}
        - 1 -- Timestamp is valid
        - 0 -- Timestamp is invalid
        - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG

        @values{for bit 30}
        - 1 -- Frame metadata is present
        - 0 -- Frame metadata is absent
        - To set this bit, use #ASM_BIT_MASK_FRAME_METADATA_FLAG and
          #ASM_SHIFT_FRAME_METADATA_FLAG

        All other bits are reserved; the aDSP sets them to 0. */

   uint32_t                  num_frames;
   /**< Number of encoded frames in the buffer. */

   uint32_t                  seq_id;
   /**< Optional buffer sequence ID. This ID is the same as the ID provided in
        #ASM_DATA_CMD_READ_COMPRESSED_V2.*/
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_read_compressed_done_v2_t */

/* Structure for metadata that can be put in the data read compressed buffer. */
typedef struct asm_data_read_buf_metadata_compressed_v2_t asm_data_read_buf_metadata_compressed_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_read_buf_metadata_compressed_v2_t
@{ */
/* Payload of the metadata that can be put in the data read buffer when
    ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 is issued.
*/
struct asm_data_read_buf_metadata_compressed_v2_t
{
    uint32_t          bitstream_id;
   /**< If multiple audio bit streams are transferred over the same
        IEC 61937 data streams, this ID is the sequence ID of the stream to
        which the frame belongs. */

   uint32_t          format_id;
   /**< Indicates the format of the incoming stream captured from the HDMI or
        SPDIF interface.

        @values Any valid format ID */

   uint32_t          data_offset;
   /**< Offset from the buffer address in the
        #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 payload to the frame associated
        with the metadata.

        @values > 0 bytes */

   uint32_t          data_size;
   /**< Size of the encoded frame.

        @values > 0 bytes */

   uint32_t          command_offset;
   /**< Offset from the buffer address in the
        #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 payload to the command
        associated with the encoded frame.

        @values > 0 bytes

        This command offset is valid only if the command size is greater than 0.
        Commands are present only for certain streams received
        through HDMI input. */

   uint32_t          command_size;
   /**< Size of the command associated with the encoded frame.

        @values 0 bytes (no command is present)

        Currently, the aDSP does not support commands that are
        present for any streams received through HDMI input. This field
        is set to 0. */

   uint32_t          num_encoded_pcm_samples;
   /**< Number of encoded PCM samples per channel in the frame
        associated with the metadata.

        @values > 0 */

   uint32_t          timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame. */

   uint32_t          timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the frame.

        If bit 5 of the mode_flags field of #ASM_STREAM_CMD_OPEN_READ_V3 is
        set to 1, the 64-bit timestamp is an absolute capture time.
        Otherwise, it is a relative session time. The absolute timestamp does
        not reset unless the system is reset. */

   uint32_t          flags;
   /**< Frame flags.

        @values{for bit 31}
        - 1 -- Timestamp is valid
        - 0 -- Timestamp is not valid
        - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG

        All other bits are reserved; the aDSP sets them to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_read_buf_metadata_compressed_v2_t */

/** @ingroup asmdata_event_dec_sr_cm_update
    Notifies the client of a change in the data sampling rate or Channel mode.
    This event is raised by the decoder service.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_sr_cm_change_notify_t}
    @table{weak__asm__data__event__sr__cm__change__notify__t}

  @detdesc
    This event is raised when both of these conditions are true:
    - The event is enabled through the mode flags of
      #ASM_STREAM_CMD_OPEN_WRITE_V3 or #ASM_STREAM_CMD_OPEN_READWRITE_V2.
    - The decoder detects a change in the output sampling frequency or the
      number/positioning of output channels, or if it is the first frame
      decoded.
    @par
    @note1hang This event is not be raised for MIDI (even if it is enabled)
               because there is no concept of sampling frequency or number of
               channels in a MIDI file.
    @par
    The new sampling frequency or the new channel configuration is
    communicated back to the client asynchronously. Channel configuration
    consists of the number of channels and the positioning of each channel.
    The event is helpful for clients to update the user interface upon
    changes in decoded bit stream parameters. @newpage

  @return
    None.

  @dependencies
    The stream must be in the Open state.
    @par
    At least one complete frame of encoded data must be available to the
    decoder.
*/
#define ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY                        0x00010C65

/* Payload for decoder sampling rate or channel mode change event. */
typedef struct asm_data_event_sr_cm_change_notify_t asm_data_event_sr_cm_change_notify_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_sr_cm_change_notify_t
@{ */
/* Payload of the ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY event.
*/
struct asm_data_event_sr_cm_change_notify_t
{
    uint32_t                  sample_rate;
    /**< New sampling rate after detecting a change in the bit stream.

         @values 2000 to 48000 Hz */
    
    uint16_t                  num_channels;
    /**< New number of channels after detecting a change in the bit stream.

         @values 1 to 8 */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */

    uint8_t                   channel_mapping[8];
    /**< Mapping for each of the channels. Channel[i] mapping describes the
         position of channel i, where 0 @le num_channels. Each entry in the
         array can have the following values.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_sr_cm_change_notify_t */

/** @ingroup asmdata_event_enc_sr_cm_update
    Notifies the client of a data sampling rate or channel mode change. This
    event is raised by the encoder service.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_enc_sr_cm_change_notify_t}
    @table{weak__asm__data__event__enc__sr__cm__change__notify__t}

  @detdesc
    This event is raised when all of the following conditions are true:
     - The stream is opened through #ASM_STREAM_CMD_OPEN_READWRITE_V2 or
       #ASM_STREAM_CMD_OPEN_READ_V3.
     - The encoding format is #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2.
     - Native mode encoding was requested in the encoder configuration
       (the channel number was 0), the sample rate was 0, or both were 0.
     - The input data frame at the encoder is the first one, or the sampling
       rate/channel mode is different from the previous input data frame.
    @par
    Immediately after this event is raised, the encoder service stops
    processing input data and waits for the client to issue
    #ASM_STREAM_CMD_FLUSH_READBUFS. Data processing resumes after successfully
    handling #ASM_STREAM_CMD_FLUSH_READBUFS.

  @return
    None.

  @dependencies
    The stream must have been opened by #ASM_STREAM_CMD_OPEN_READWRITE_V2 or
    #ASM_STREAM_CMD_OPEN_READ_V3.
*/
#define ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY                    0x00010BDE

/* Payload for encoder sampling rate, channel mode change event. */
typedef struct asm_data_event_enc_sr_cm_change_notify_t asm_data_event_enc_sr_cm_change_notify_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_enc_sr_cm_change_notify_t
@{ */
/* Payload of the ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY event.
 */
struct asm_data_event_enc_sr_cm_change_notify_t
{
    uint32_t                  sample_rate;
    /**< New sampling rate after detecting a change in the input data.

         @values 2000 to 48000 Hz */

    uint16_t                  num_channels;
    /**< New number of channels after detecting a change in the input data.

         @values 1 to 8 */

    uint16_t                  bits_per_sample;
    /**< New bits per sample after detecting a change in the input data.

         @values 16, 24 */

    uint8_t                   channel_mapping[8];
    /**< New channel mapping. Channel[i] mapping describes the position of
         channel i, where 0 @le i < num_channels. Each entry in the array
         can have the following values.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_enc_sr_cm_change_notify_t */


/** @ingroup asmdata_cmd_remove_init_silence
    Indicates the number of samples per channel to be removed from the
    beginning of the stream.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_REMOVE_INITIAL_SILENCE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_remove_initial_silence_t}
    @table{weak__asm__data__cmd__remove__initial__silence__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session/stream must be valid.
    @par
    This command must be sent before the first data buffer. If it is sent
    after the first frame, the ADSP_EFAILED error code is returned, and
    samples to be returned are not updated. (For more details on the error
    code, refer to @xrefcond{Q5,80-NF774-4,80-NA610-4}.)
    @par
    If this message is sent multiple times before the first data buffer,
    only the last value sent is considered.
*/
#define ASM_DATA_CMD_REMOVE_INITIAL_SILENCE                             0x00010D67

/* Structure for the number of silence samples to be removed from the
    beginning of the track. */
typedef struct asm_data_cmd_remove_initial_silence_t asm_data_cmd_remove_initial_silence_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_remove_initial_silence_t
@{ */
/* Payload of the ASM_DATA_CMD_REMOVE_INITIAL_SILENCE command, which removes
    a specified number of silence samples from the beginning of the track.
*/
struct asm_data_cmd_remove_initial_silence_t
{
   uint32_t                  num_samples_to_remove;
   /**< Number of samples per channel to be removed.

        @values 0 to 2^31 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_remove_initial_silence_t */

/** @ingroup asmdata_cmd_remove_trail_silence
    Indicates the number of samples per channel to be removed from
    the end of the stream.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_REMOVE_TRAILING_SILENCE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_remove_trailing_silence_t}
    @table{weak__asm__data__cmd__remove__trailing__silence__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session/stream must be valid.
    @par
    This command must be sent before the last data buffer is sent to the
    aDSP. Also, the last buffer flag must be set for the last data buffer. If
    this command is sent after the last buffer is sent down, the error code
    ADSP_EFAILED is returned, and samples to be removed are not updated.
    (For more details on the error code, refer to
    @xrefcond{Q5,80-NF774-4,80-NA610-4}.)
    @par
    If this message is sent multiple times before the last buffer, only the
    last value sent is considered.
*/
#define ASM_DATA_CMD_REMOVE_TRAILING_SILENCE                            0x00010D68

/* Structure for the number of silence samples to be removed from the
    end of a track. */
typedef struct asm_data_cmd_remove_trailing_silence_t asm_data_cmd_remove_trailing_silence_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_remove_trailing_silence_t
@{ */
/* Payload of the ASM_DATA_CMD_REMOVE_TRAILING_SILENCE command, which
    removes a specified number of silence samples from end the of the track.
*/
struct asm_data_cmd_remove_trailing_silence_t
{
   uint32_t                  num_samples_to_remove;
   /**< Number of samples per channel to be removed.

        @values 0 to 2^31 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_remove_trailing_silence_t */

/** @ingroup asmdata_cmd_iec60958_fr_rate
    Indicates the IEC 60958 frame rate of the IEC 61937 packetized audio
    stream.
    This command is used when the aDSP is operating in Pass-through mode for
    compressed audio playback.

    Currently, sampling rate changes midstream are not supported, and the
    client is not expected to issue this command in the middle of playback.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_IEC_60958_FRAME_RATE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_iec_60958_frame_rate}
    @table{weak__asm__data__cmd__iec__60958__frame__rate}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session must be valid, and it must be opened for compressed audio
    playback in Pass-through mode.
    @par
    This command must be sent before the first data buffer. Any data buffers
    received in Pass-through mode before this message are not be processed.
    If this message is sent multiple times before the first data buffer, the
    latest value takes effect.
*/
#define ASM_DATA_CMD_IEC_60958_FRAME_RATE                            0x00010D87

/* Structure for the number of silence samples to be removed from the
    end of a track. */
typedef struct asm_data_cmd_iec_60958_frame_rate asm_data_cmd_iec_60958_frame_rate;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_iec_60958_frame_rate
@{ */
/* Payload of the ASM_DATA_CMD_IEC_60958_FRAME_RATE command, which is used to
    indicate the IEC 60958 frame rate of a given packetized audio stream.
*/
struct asm_data_cmd_iec_60958_frame_rate
{
   uint32_t                  frame_rate;
   /**< IEC 60958 frame rate of the incoming IEC 61937 packetized stream.

        @values Any valid frame rate */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_iec_60958_frame_rate */

/** @ingroup asmdata_cmd_iec60958_media_fmt
    Indicates the IEC 60958 frame rate and number of channels for an
    IEC 61937 packetized audio stream.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_IEC_60958_MEDIA_FMT
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_cmd_iec_60958_media_format_t}
    @table{weak__asm__data__cmd__iec__60958__media__format__t}

  @detdesc
    When the aDSP is operating in Pass-through mode for compressed audio
    playback, this command provides the flexibility to set the channel count
    required for playing true HD bitstream.
    @par
    Currently, mid-stream changes in the sampling rate or number of channels
    are not supported. The client is not expected to issue this command in the
    middle of playback.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session must be valid, and it must be opened for compressed audio
    playback in Pass-through mode.
    @par
    This command must be sent before the first data buffer. Any data buffers
    received in Pass-through mode before this message are not be processed.
    If this message is sent multiple times before the first data buffer, the
    latest value takes effect.
*/
#define ASM_DATA_CMD_IEC_60958_MEDIA_FMT                     0x0001321E

/* Structure for the number of silence samples to be removed from the
    end of a track. */
typedef struct asm_data_cmd_iec_60958_media_format_t asm_data_cmd_iec_60958_media_format_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_cmd_iec_60958_media_format_t
@{ */
/* Payload of the ASM_DATA_CMD_IEC_60958_FRAME_RATE command, which is used to
    indicate the IEC 60958 frame rate of a given packetized audio stream.
*/
struct asm_data_cmd_iec_60958_media_format_t
{
   uint32_t                  sampling_rate;
   /**< IEC 60958 sampling rate rate of the incoming IEC 61937 packetized stream.

        @values Any valid sampling rate */

   uint32_t                  num_channels;
   /**< Number of channels (2 or 8) of the incoming IEC61937 packetized steam.

        @values
        - 2 for LBR
        - 8 for HBR @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_cmd_iec_60958_media_format */


/** @ingroup asmdata_event_watermark
    Indicates that a preconfigured watermark has been hit during Pull mode
    playback or Push mode record. The client can use this event to write more
    data or consume existing data in the buffer.

    This event is sent to the same client that opens the stream
    (to the same APR address).

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_WATERMARK
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_watermark_t}
    @table{weak__asm__data__event__watermark__t}

  @return
    None.

  @dependencies
    The session/stream must be a valid and open Pull mode write or Push mode
    read session/stream.
*/
#define ASM_DATA_EVENT_WATERMARK                                   0x00010DDB

/* Structure for a data water mark event payload. */
typedef struct asm_data_event_watermark_t asm_data_event_watermark_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_data_event_watermark_t
@{ */
/*  Payload of the ASM_DATA_EVENT_WATERMARK event, which indicates the
    approximate position of the aDSP and triggers the client to supply
    more data (playback) or consume existing data (record).
*/
struct asm_data_event_watermark_t
{
   uint32_t    watermark_level_bytes;
   /**< Watermark level that is reached.

        For Pull mode playback, this means the read pointer on the aDSP has
        reached this number of bytes from the start of the buffer.

        For Push mode record, this means that the write pointer on the aDSP
        has reached this number of bytes from the start of the buffer.

        @values 0 to circular buffer size minus 1

        This value is for information only, and the actual read or write index
        must be read using #asm_push_mode_read_position_buffer_t or
        #asm_pull_mode_write_position_buffer_t. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_data_event_watermark_t */

/** @ingroup asmdata_event_dec_error_notify
    Notifies the client of consecutive errors that reach a preset threshold.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_DEC_ERR_NOTIFY \n
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_data_event_dec_err_t}
    None.

  @detdesc
    The decoder service raises this event when all of the following conditions
    are true:
      - Functionality has been enabled by an
        #ASM_STREAM_CMD_SET_ENCDEC_PARAM command where the preset number is
        specified (denoted as N) and the preset timeout is also specified
        (denoted as T).
      - One of the following errors has occurred:
          - N consecutive decoder errors have occurred.
          - >1 consecutive decoder error has occurred and T milliseconds have
            passed.
    @par
    After this event is raised, the decoder continues processing and waits for
    instructions from the client.

  @return
    None.

  @dependencies
    None.
*/
#define ASM_DATA_EVENT_DEC_ERR_NOTIFY                        0x00010DF7

/** @ingroup asmdata_cmd_resync
    Communicates a resynchronization marker to handle a discontinuity in
    the next buffer timestamp.

    This marker ensures that the session clock is reset and a fresh render
    decision begins with the next valid incoming buffer timestamp.

  @apr_hdr_fields
    Opcode -- ASM_DATA_CMD_RESYNC
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    The session/stream must be a valid and opened write session/stream.
*/
#define ASM_DATA_CMD_RESYNC                                            0x00013216

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_DATA_COMMANDS_H_ */
