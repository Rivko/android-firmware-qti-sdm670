/*========================================================================*/
/**
@file adsp_asm_stream_commands.h

This file contains ASM stream commands and events structures definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_asm_stream_commands.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
07/23/16   HB      Moved properiatery information.
07/16/16   sw      (Tech Pubs) Merged Doxygen comments from 8953.2.7;
                    edited Doxygen comments for AVS 2.8.
10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
05/08/15   sw      (Tech Pubs) Edited Doxygen comments for 8952.
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
09/06/13   sw      (Tech Pubs) Merged Doxygen comments from 2.0; edited
                    new Doxygen comments for 2.2.
08/06/12   sw      (Tech Pubs) Edited comments for Badger; updated Doxygen markup
                   to Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
04/22/11    ss     Introducing Audproc API file that would hold all post processing
                   Module IDs, Parameter IDs and Topology IDs
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_ASM_STREAM_COMMANDS_H_
#define _ADSP_ASM_STREAM_COMMANDS_H_

#include "mmdefs.h"
#include "adsp_audproc_api.h"
#include "adsp_asm_stream_commands_prop.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asmstrm_cmd_open_write
@{ */
/** Creates an audio stream for playback. There is one decoder and one
    postprocessor chain, however, there is no decoder for PCM playback.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_WRITE_V3
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_write_v3_t}
    @table{weak__asm__stream__cmd__open__write__v3__t}

  @detdesc
    When creating a stream, the client must specify the group and stream IDs.
    The current release supports up to 15 groups, and each
    group can have up to #ASM_MAX_STREAMS_PER_SESSION streams.
    @par
    This command has only the media format ID to open the decoder. The media
    format block is communicated to the aDSP through the
    #ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command before the data is sent.
    @par
    After successfully opening the stream, ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2
    must be sent in the data path to configure the decoder before sending
    any data.
    @par
    To achieve gapless playback, the client must open successive streams with
    the same session ID, but different stream IDs. Up to two streams can be
    open at the same time per session in Gapless mode. If the client tries to
    open a stream when two streams are already open in the session, an
    ADSP_EALREADY error code is returned.
    @par
    MIDI and DTMF formats are not supported in gapless playback. Any attempt
    to open streams in these formats in Gapless Playback mode returns the
    ADSP_EUNSUPPORTED error code. For more details on the error codes, refer
    to @xrefcond{Q5,80-NF774-4,80-NA610-4}. @newpage

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_WRITE_V3                                   0x00010DB3

/** Enumeration for a device matrix as an endpoint. */
#define ASM_STREAM_END_POINT_DEVICE_MTX                             0

/** Bitmask for an SR/CM change notification flag. */
#define ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG                       (0x00000004UL)

/** Bit shift for an SR/CM change notification flag. */
#define ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG                          2

/** Bitmask for Gapless mode. */
#define ASM_BIT_MASK_GAPLESS_MODE_FLAG                              (0x80000000UL)

/** Bit shift for Gapless mode. */
#define ASM_SHIFT_GAPLESS_MODE_FLAG                                 31

/** Bitmask for the stream_perf_mode subfield. */
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE            (0x70000000UL)

/** Bit shift for the stream_perf_mode subfield. */
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE                     28
/** @} *//* end_addtogroup asmstrm_cmd_open_write */

/** @addtogroup asm_common
@{ */
/** Identifies a legacy stream session. */
#define ASM_LEGACY_STREAM_SESSION                                      0

/** Identifies a low latency stream session. */
#define ASM_LOW_LATENCY_STREAM_SESSION                                  1

/** Identifies an ultra low latency stream session. */
#define ASM_ULTRA_LOW_LATENCY_STREAM_SESSION                            2

/** Identifies a low latency no-processing stream session. */
#define ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION                          3

/** Identifies an ultra low latency postprocessing stream session. */
#define ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION                  4
/** @} *//* end_addtogroup asm_common */

/* Payload format for a stream open write command. */
typedef struct asm_stream_cmd_open_write_v3_t asm_stream_cmd_open_write_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_write_v3_t
@{ */
/*  Payload of the ASM_STREAM_CMD_OPEN_WRITE_V3 command, which opens a
     write stream for a given session ID and stream ID.The dst_port field in
    the APR packet contains both the session ID and stream ID.
*/
struct asm_stream_cmd_open_write_v3_t
{
         /* DTMF is OK below for ISVs */
    uint32_t                    mode_flags;
    /**< Configures several aspects such as notification of SR/CM events,
         Gapless mode, Performance mode, etc.

         @note1hang MIDI and DTMF streams cannot be opened in Gapless mode.

         @values{for bit 2}
         - 0 -- SR/CM change notification event is disabled
         - 1 -- SR/CM change notification event is enabled
         - To set/get this bit, use #ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG and
           #ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG

         @values{for bits 30 to 28} (stream_perf_mode subfield)
         - #ASM_LEGACY_STREAM_SESSION -- Ensures backward compatibility to the
           original behavior of ASM_STREAM_CMD_OPEN_WRITE_V2.
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a playback session by using
           shortened buffers in low latency POPP.
           - Recommendation: Do not enable high latency algorithms. They might
             negate the benefits of opening a low latency stream, and they
             might also suffer quality degradation from unexpected jitter.

         @contcell
         @values{for bits 30 to 28} (cont.)
         - #ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION -- Opens a low latency
           no-processing playback stream session by using shortened buffers
           in the decoder.
           - Recommendation: To avoid processing in the AFE, match the decoding
             and AFE rates.
           - There is no POPP processing in these stream sessions.
           - This mode is intended for faster than real-time applications
             because quality of service cannot be guaranteed.
           - The stream and device bits per sample, number of channels,
             sampling rate, channel mapping, and all other media formats must
             match. Otherwise, the quality of audio is not guaranteed.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
         - #ASM_ULTRA_LOW_LATENCY_STREAM_SESSION -- Opens a ULL
           playback session by using shortened buffers in the decoder. There is
           no POPP processing in the ULL stream session.
           - Recommendation: The stream and device bits per sample, number of
             channels, sampling rate, channel mapping, and all other media
             formats must match. Otherwise, the quality of audio is not
             guaranteed.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
         - #ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION -- Opens a ULLPP
           stream by using shortened buffers in the audio path.
           - Recommendation: To avoid processing in the AFE, the sample rate,
             bit width, and channel mapping must match the device rate.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
           - A ULLPP stream can connect to a ULLPP or ULL device.
           - The client can use a different media format to route a ULLPP or
             ULL ASM stream to a ULLPP or ULL ADM device. However, the
             necessary modules must be instantiated in the postprocessing
             topology to ensure that media format conversions applied across
             the two services are compatible.
           - The same guidelines for modules in the low latency path apply to
             modules in the ULLPP path.
         - To configure this subfield, use
           #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE.

         @contcell
         @values{for bit 31}
         - 0 -- Stream to be opened in Non-Gapless mode
         - 1 -- Stream to be opened in Gapless mode; successive streams must
           be opened with the same session ID but different stream IDs
         - To set/get this bit, use #ASM_BIT_MASK_GAPLESS_MODE_FLAG and
           #ASM_SHIFT_GAPLESS_MODE_FLAG

         All other bits are reserved; clients must set them to 0. */

    uint16_t                    sink_endpoint_type;
    /**< Sink point type.

         @values 0 (ASM_SINK_MATRIX_STRTR) -- Device matrix (gateway to the
         hardware ports) */

    uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

          @values 16, 24, 32 */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.

         @values See Chapter @xref{hdr:AudioPostProc}

         This field can also be enabled through Set Parameter flags.

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

    uint32_t                    dec_fmt_id;
    /**< Configuration ID of the decoder media format.

         @values See Chapter @xref{dox:AsmStreamMediaFormats} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_write_v3_t */

/** @addtogroup asmstrm_cmd_open_read
@{ */
/** Creates an audio stream for recording. There is one preprocessing chain
    and one encoder. If the encoder type is PCM playback, it is a pass-through
    encoder.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_READ_V3
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_read_v3_t}
    @table{weak__asm__stream__cmd__open__read__v3__t} @newpage

  @detdesc
    This command creates a new record stream. It specifies the media format of
    the encoding to be performed by the aDSP. The encoder configuration block
    must be communicated to the aDSP through the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command before sending data buffers to
    get the encoded data from the aDSP.
    @par
    After successfully opening the stream, ASM_STREAM_CMD_SET_ENCDEC_PARAM
    must be sent in the command path to configure the encoder before encoded
    frames can be sent back to the client.
    @par
    @note1hang Sending multiple ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for
               changing the encoder configuration is not allowed.
    @par
    Twenty-four-bit recording is supported only for the Linear PCM media
    format.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_READ_V3                                    0x00010DB4

/** Definition of the metadata information flag bitmask. */
#define ASM_BIT_MASK_META_INFO_FLAG                                 (0x00000010UL)

/** Definition of the metadata information flag shift value. */
#define ASM_SHIFT_META_INFO_FLAG                                    4

/** Definition of the timestamp type flag bitmask. */
#define ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG                            (0x00000020UL)

/** Definition of the timestamp type flag shift value. */
#define ASM_SHIFT_TIMESTAMP_TYPE_FLAG                                    5

/** Relative timestamp is identified by this value. */
#define ASM_RELATIVE_TIMESTAMP                                          0

/** Absolute timestamp is identified by this value. @newpage */
#define ASM_ABSOLUTE_TIMESTAMP                                          1

/** Bitmask for the stream_perf_mode subfield. */
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ            (0xe0000000UL)

/** Bit shift for the stream_perf_mode subfield. */
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ                     29

/** @} *//* end_addtogroup asmstrm_cmd_open_read */

/* Payload format for a stream open read command. */
typedef struct asm_stream_cmd_open_read_v3_t asm_stream_cmd_open_read_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_read_v3_t
@{ */
/* Payload of the #ASM_STREAM_CMD_OPEN_READ_V3 command, which opens a
    read stream for a given session ID and stream ID. The dst_port field in
    the APR packet contains both the session ID and the stream ID.
*/
struct asm_stream_cmd_open_read_v3_t
{
    uint32_t                    mode_flags;
    /**< Specifies whether metadata per encoded frame is to be provided,
         whether the timestamp is relative or absolute, and the performance
         mode in which this stream was opened.

         @values{for bit 4}
         - 0 -- Return data buffer contains all encoded frames only; it does
           not contain frame metadata
         - 1 -- Return data buffer contains an array of metadata and encoded
           frames
         - To set this bit, use #ASM_BIT_MASK_META_INFO_FLAG and
           #ASM_SHIFT_META_INFO_FLAG

         @values{for bit 5}
         - #ASM_RELATIVE_TIMESTAMP -- #ASM_DATA_EVENT_READ_DONE_V2 has a
           relative timestamp
         - #ASM_ABSOLUTE_TIMESTAMP -- #ASM_DATA_EVENT_READ_DONE_V2 has an
           absolute timestamp
         - To set this bit, use #ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG and
           #ASM_SHIFT_TIMESTAMP_TYPE_FLAG

         @values{for bits 31 to 29} (stream_perf_mode subfield)
         - #ASM_LEGACY_STREAM_SESSION -- Ensures backward compatibility to the
           original behavior of ASM_STREAM_CMD_OPEN_READ_V2.

         @contcell
         @values{for bits 31 to 29} (stream_perf_mode subfield, cont.)
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a record session using
           shortened buffers in low latency POPreP.
           - Recommendation: Do not enable high latency algorithms. They might
             negate the benefits of opening a low latency stream, and they
             might also suffer quality degradation from unexpected jitter.
         - #ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION -- Opens a low latency
           no-processing record stream session by using shortened buffers in
           the encoder. There is no POPP processing in these stream sessions.
           - Recommendation: To avoid processing in the AFE, match the decoding
             and AFE rates.
           - This mode is intended for faster than real-time applications
             because quality of service cannot be guaranteed.
           - The stream and device bits per sample, number of channels,
             sampling rate, channel mapping, and all other media formats must
             match.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
         - To configure this subfield, use
           #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ.

         All other bits are reserved; clients must set them to 0. */

    uint32_t                    src_endpoint_type;
    /**< Specifies the endpoint providing the input samples.

         @values
         - 0 -- Device matrix (gateway from the tunneled Tx ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

    uint32_t                    preproc_topo_id;
    /**< Specifies the topology (order of processing) of preprocessing
         algorithms. <i>None</i> means no preprocessing.

         @values See Chapter @xref{hdr:AudioPostProc}

         This field can also be enabled through Set Parameter flags.

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

    uint32_t                    enc_cfg_id;
    /**< Media configuration ID for encoded output.

         @values See Chapter @xref{dox:AsmStreamMediaFormats} */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

          @values 16, 24, 32 */

     uint16_t                    reserved;
     /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_read_v3_t */

/** @addtogroup asmstrm_cmd_open_readwrite
@{ */
/** Opens a read/write stream with configurable bits per sample.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_READWRITE_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_readwrite_v2_t}
    @table{weak__asm__stream__cmd__open__readwrite__v2__t}

  @detdesc
    A read/write stream is opened for a given session ID and stream ID. The
    dst_port field in the APR packet contains both the session ID and the
    stream ID.
    @par
    After successfully opening the stream, a set_encdec_params command must be
    sent in the command path to configure the encoder before encoded frames
    can be sent back to the client (even if it is a PCM encoder used for NT
    decoding). Also, a media format update command must be sent on the data
    path to configure the decoder before sending any data.
    @par
    This command specifies the media format of the encoding to be performed by
    the Hexagon processor. The encoder configuration block must be
    communicated to the Hexagon processor through the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command before sending data buffers to
    get the encoded data from the Hexagon processor.
    @par
    @note1hang Sending multiple ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for
               changing the encoder configuration is not allowed.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
/* API_developer_note: A family also had ASM_STREAM_CMD_OPEN_READWRITE. Since
   ASM_STREAM_CMD_OPEN_READWRITE_V2 is more general,
   ASM_STREAM_CMD_OPEN_READWRITE is removed in B family.
*/
#define ASM_STREAM_CMD_OPEN_READWRITE_V2                                0x00010D8D

/** Enumeration for native sampling rate signaling at the POPP output. */
#define ASM_POPP_OUTPUT_SR_NATIVE_RATE                                  0

/** Enumeration for the maximum sampling rate at the POPP output. */
#define ASM_POPP_OUTPUT_SR_MAX_RATE                                     48000

/** @} *//* end_addtogroup asmstrm_cmd_open_readwrite */

/* Payload format for a stream open read/write command. */
typedef struct asm_stream_cmd_open_readwrite_v2_t asm_stream_cmd_open_readwrite_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_readwrite_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_READWRITE_V2 command.
*/
struct asm_stream_cmd_open_readwrite_v2_t
{
    uint32_t                    mode_flags;
    /**< Configures the stream to notify the client when it detects a CM/SR
         change at the input to its POPP. This field also specifies whether
         metadata per encoded frame is to be provided.

         @values{for bit 2}
         - 0 -- SR/CM change notification event is disabled
         - 1 -- SR/CM change notification event is enabled
         - To set/get this bit, use #ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG and
           #ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG

         @values{for bit 4}
         - 0 -- Return read data buffer contains all encoded frames only; it
           does not contain frame metadata
         - 1 -- Return read data buffer contains an array of metadata and
           encoded frames
         - To set/get this bit, use #ASM_BIT_MASK_META_INFO_FLAG and
           #ASM_SHIFT_META_INFO_FLAG

         All other bits are reserved; clients must set them to 0. */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.

         @values See Chapter @xref{hdr:AudioPostProc}

         This field can also be enabled through Set Parameter flags.

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

    uint32_t                    dec_fmt_id;
    /**< Specifies the media type of the input data. PCM indicates that no
         decoding must be performed (e.g., this session is an NT encoder
         session.

         @values  See Chapter @xref{dox:AsmStreamMediaFormats} */

    uint32_t                    enc_cfg_id;
    /**< Specifies the media type for the output of the stream. PCM indicates
         that no encoding must be performed (e.g., this session is an NT
         decoder session.

         @values See Chapter @xref{dox:AsmStreamMediaFormats} */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

          @values 16, 24, 32 */

     uint16_t                    reserved;
     /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_readwrite_v2_t */

/** @ingroup asmstrm_cmd_open_loopback
    Opens a loopback stream for a given session ID and stream ID.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_LOOPBACK_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_loopback_v2_t}
    @table{weak__asm__stream__cmd__open__loopback__v2__t}

  @keep{8} @detdesc
    This command sets up the stream portion of the ASM Loopback mode
    topology. It is different from a read stream and a write stream in that
    there are no buffers exchanged between the aDSP and client. The session to
    which this stream belongs cannot have any other streams.
    @par
    The POPP topology ID must be specified. This command sets up the connection
    from the Tx Matrix output port to the POPP input port, and from the POPP
    output port to the Rx Matrix input port.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
/* API_developer_note: A family also had ASM_STREAM_CMD_OPEN_LOOPBACK. Since
   ASM_STREAM_CMD_OPEN_LOOPBACK_V2 is more general,
   ASM_STREAM_CMD_OPEN_LOOPBACK is removed in B family.
*/
#define ASM_STREAM_CMD_OPEN_LOOPBACK_V2                              0x00010D8E

/* Payload format for stream open loopback */
typedef struct asm_stream_cmd_open_loopback_v2_t asm_stream_cmd_open_loopback_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_loopback_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_LOOPBACK_V2 command, which opens a loopback
    stream with configurable bits per sample. The dst_port field in the APR
    packet contains both the session ID and stream ID.
*/
struct asm_stream_cmd_open_loopback_v2_t
{
    uint32_t                    mode_flags;
    /**< All bits are reserved. Clients must set them to 0. */

    uint16_t                    src_endpoint_type;
    /**< Specifies the source endpoint that provides the input samples.

         @values
         - 0 -- Tx device matrix (gateway to the hardware ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

    uint16_t                    sink_endpoint_type;
    /**< Specifies the sink endpoint type.

         @values
         - 0 -- Rx device matrix (gateway to the hardware ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

    uint32_t                    postproc_topo_id;
    /**< Postprocessing topology ID, which specifies the topology (order of
         processing) of postprocessing algorithms.

         @values See Chapter @xref{hdr:AudioPostProc}

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

     uint16_t                    bits_per_sample;
      /**< Number of bits per sample processed by the ASM modules.

          @values 16, 24, 32 */

     uint16_t                    reserved;
     /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_loopback_v2_t */

/** @addtogroup asmstrm_cmd_open_tcode_lpbk
@{ */
/** Opens a transcode loopback stream, which routes the stream from a
    device (either compressed or PCM) to another device (compressed or PCM)
    after decoding or encoding, or as is.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_transcode_loopback_t}
    @table{weak__asm__stream__cmd__open__transcode__loopback__t}
  
  @detdesc
    Following are the possible loopbacks:
    - PCM to PCM: \n
      Tx matrix @rarrow POPP @rarrow Rx matrix
    - PCM to compressed: \n
      Tx matrix @rarrow POPP @rarrow Encoder @rarrow Rx stream router
    - Compressed to PCM: \n
      Tx stream router @rarrow Decoder @rarrow POPP @rarrow Rx matrix
    - @keep{5} Compressed to compressed: \n
      Tx Stream router @rarrow Decoder @rarrow POPP @rarrow Encoder @rarrow
      Rx stream router
      (selected if source and sink non-PCM formats are different, and the
      converter mode = #ASM_CONVERTER_MODE_NONE or
      #ASM_POST_PROCESS_CONVERTER_MODE)
    - Compressed to compressed: \n
      Tx stream router @rarrow decoder converter @rarrow Rx stream router
      (selected based on the Converter mode)
    - Compressed to compressed: \n
      Tx stream router @rarrow Rx stream router (direct passthrough)
      (selected if source and sink non-PCM formats are the same, and the
      Converter mode = ASM_CONVERTER_MODE_NONE)

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as an input.

  @dependencies
    None. @newpage
*/
#define ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK                         0x00010DBA

/** Bitmask for the Performance mode of the stream. */
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK    (0x70000000UL)

/** Bit shift for the Performance mode of the stream. */
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK           28

/** Bitmask for the decoder converter enable flag. */
#define ASM_BIT_MASK_DECODER_CONVERTER_FLAG                          (0x00000078UL)

/** Shift value for the decoder converter enable flag. */
#define ASM_SHIFT_DECODER_CONVERTER_FLAG                              3

/** Converter mode is None (Default). */
#define ASM_CONVERTER_MODE_NONE                    0

/** One of the converter modes. */
#define ASM_CONVERTER_MODE_1                                     1

/** Identifies a special Converter mode where source and sink formats are the
    same but postprocessing must applied. Therefore, Decode @rarrow Re-encode
    is necessary. */
#define ASM_POST_PROCESS_CONVERTER_MODE                                     2

/** @} *//* end_addtogroup asmstrm_cmd_open_tcode_lpbk */

/* Payload format for stream open loopback */
typedef struct asm_stream_cmd_open_transcode_loopback_t asm_stream_cmd_open_transcode_loopback_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_transcode_loopback_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK command, which opens
    a loopback stream with configurable bits per sample.
*/
struct asm_stream_cmd_open_transcode_loopback_t
{
    uint32_t                    mode_flags;
    /**< Specifies the performance mode in which this stream is to be opened.

         @values{for bits 30 to 28} (stream_perf_mode flag)
         - #ASM_LEGACY_STREAM_SESSION -- This mode ensures backward
           compatibility to the original behavior of
           ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a loopback session by using
           shortened buffers in low latency POPP
           - Recommendation: Do not enable high latency algorithms. They might
             negate the benefits of opening a low latency stream, and they
             might also suffer quality degradation from unexpected jitter.
           - This Low Latency mode is supported only for the PCM leg in the
             loopback. For example, in a compressed-to-PCM loopback, Low Latency
             mode is enabled only for the POPP and Rx Mixer services.
         - To configure this subfield, use
           #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK.

         @contcell
         @values{for bits 6 to 3} (Decoder Converter mode compatibility)
         - #ASM_CONVERTER_MODE_NONE (0x0) -- Default
         - #ASM_CONVERTER_MODE_1 (0x1)
         - #ASM_POST_PROCESS_CONVERTER_MODE (0x2)
         - 0x3-0xF -- Reserved for future use
         - To set these bits, use #ASM_BIT_MASK_DECODER_CONVERTER_FLAG and
           #ASM_SHIFT_DECODER_CONVERTER_FLAG

         All other bits are reserved; clients must set them to 0. */

    uint32_t                    src_format_id;
    /**< Specifies the media format of the input audio stream.

         @values
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4
         @if ISV_only @tablebulletend @endif

         @if OEM_only @inputfile{group__weakf__src__format__id.tex} @endif */

    uint32_t                    sink_format_id;
    /**< Specifies the media format of the output stream.

         @values
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4
         @if ISV_only @tablebulletend @endif

         @if OEM_only @inputfile{group__weakf__sink__format__id.tex} @endif */

    uint32_t                    audproc_topo_id;
    /**< Postprocessing topology ID, which specifies the topology (order of
         processing) of postprocessing algorithms.

         @values See Chapter @xref{hdr:AudioPostProc}

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES.

         This field is ignored for the Converter mode, in which no
         postprocessing is performed. */

    uint16_t                    src_endpoint_type;
    /**< Specifies the source endpoint that provides the input samples.

         @values
         - 0 -- Tx device matrix or stream router (gateway to the hardware
           ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

    uint16_t                    sink_endpoint_type;
    /**< Specifies the sink endpoint type.

         @values
         - 0 -- Rx device matrix or stream router (gateway to the hardware
           ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by the ASM modules.

          @values 16, 24, 32 */

     uint16_t                    reserved;
     /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_transcode_loopback_t */

/** @ingroup asmstrm_cmd_close
    Stops and closes a specified stream.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_CLOSE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The stream must be a valid session stream.
*/
#define ASM_STREAM_CMD_CLOSE                                        0x00010BCD

/** @ingroup asmstrm_cmd_flush
    Flushes a specified stream.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_FLUSH
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The stream's session must be paused; otherwise, an error is returned.
*/
#define ASM_STREAM_CMD_FLUSH                                        0x00010BCE

/** @addtogroup asmstrm_cmd_flush_v2
@{ */
/** Flushes a specified stream. This command differs from ASM_STREAM_CMD_FLUSH
    in that it provides an option of whether to flush a bit stream.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_FLUSH_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    @table{weak__asm__stream__cmd__flush__v2__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The stream's session must be paused; otherwise, an error is returned.
*/
#define ASM_STREAM_CMD_FLUSH_V2                                        0x00010DEB

/** Bitmask for the mode flags. */
#define ASM_BIT_MASK_FLUSH_V2_MODE                                        (0x00000001UL)

/** Bit shift for the mode flag that is used to specify whether to flush the
    bit stream. */
#define ASM_SHIFT_MASK_FLUSH_MODE_NOT_TO_FLUSH_BISTREAM                   0

/** Do not flush the bit stream. */
#define ASM_STREAM_NOT_TO_FLUSH_BITSTREAM                                 1

/** @} *//* end_addtogroup asmstrm_cmd_flush_v2 */

/* Payload format for flush command v2 */
typedef struct asm_stream_cmd_flush_v2_t asm_stream_cmd_flush_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_flush_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_FLUSH_V2 command, which has option either
    to flush the bitstream or not.
*/
struct asm_stream_cmd_flush_v2_t
{
    uint32_t                    mode_flags;
    /**< Bit 0 specifies whether to flush the bit stream.

         @values
         - 0 = Flush
         - 1 = Do not flush
         - To set this flag, use #ASM_BIT_MASK_FLUSH_V2_MODE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_flush_v2_t */

/** @ingroup asmstrm_cmd_flush_readbufs
    Flushes a tunneled Tx or non-tunneled stream's output buffers.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_FLUSH_READBUFS
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    This command is used in read and read/write streams to allow the client
    to retrieve all shared memory buffers for size reallocation, without
    flushing any of the data pipelined in the stream. An example is a sample
    rate or channel configuration change in a non-tunneled decoder.
    @par
    The behavior is to deliver first any partially filled output buffers
    with #ASM_DATA_EVENT_READ_DONE_V2, followed by all the queued empty output
    buffers.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The stream must be either a tunneled Tx stream opened by
    #ASM_STREAM_CMD_OPEN_READ_V3 or a non-tunneled stream opened by
    #ASM_STREAM_CMD_OPEN_READWRITE_V2.
*/
#define ASM_STREAM_CMD_FLUSH_READBUFS                               0x00010C09

/** @ingroup asmstrm_cmd_set_pp_params_v2
    Sets one or more ASM stream parameters in a POPP chain.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_SET_PP_PARAMS_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_set_pp_params_v2_t}
    @tablens{weak__asm__stream__cmd__set__pp__params__v2__t}

  @par Parameter data variable payload (asm_stream_param_data_v2_t)
    @tablens{weak__asm__stream__param__data__v2__t}

  @detdesc
    This command is typically used to apply settings to the POPP or
    per-group-object pre/postprocessing (PGOPP) chains.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
/* API_developer_note: A family API ASM_STREAM_CMD_SET_PP_PARAMS is replaced
   by ASM_STREAM_CMD_SET_PP_PARAMS_V2.
*/
#define ASM_STREAM_CMD_SET_PP_PARAMS_V2                                0x00010DA1

/* Payload format for a stream Set Parameters command. */
typedef struct asm_stream_cmd_set_pp_params_v2_t asm_stream_cmd_set_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_set_pp_params_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_SET_PP_PARAMS_V2 command, which allows
    one or more parameters to be set on a stream. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%asm_stream_param_data_v2_t).
*/
struct asm_stream_cmd_set_pp_params_v2_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %asm_stream_param_data_v2_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  data_payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_set_pp_params_v2_t */

/* Payload format for stream parameter data. */
typedef struct asm_stream_param_data_v2_t asm_stream_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_param_data_v2_t
@{ */
/* Payload of the stream parameter data of the
    ASM_STREAM_CMD_SET_PP_PARAMS_V2 command. */
/** Immediately following this structure are param_size bytes of parameter
    data, which must be aligned to 4 bytes. The structure and size depend on
    the module_id/param_id combination.
*/
struct asm_stream_param_data_v2_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint16_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_param_data_v2_t */

/** @ingroup asmstrm_cmd_set_pp_params_v3
    Sets one or more ASM stream parameters on a POPP chain.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_SET_PP_PARAMS_V3
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_set_pp_params_v3_t}
    @tablens{weak__asm__stream__cmd__set__pp__params__v3__t}

  @par Parameter data variable payload (asm_stream_param_data_v3_t)
    @tablens{weak__asm__stream__param__data__v3__t}

  @detdesc
    This command is typically used to apply settings to the POPP or
    PGOPP chains.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_STREAM_CMD_SET_PP_PARAMS_V3                                0x0001320D

/* Payload format for a stream Set Parameters command. */
typedef struct asm_stream_cmd_set_pp_params_v3_t asm_stream_cmd_set_pp_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_set_pp_params_v3_t
@{ */
/* Payload of the ASM_STREAM_CMD_SET_PP_PARAMS_V3 command, which allows
    one or more parameters to be set on a stream. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%asm_stream_param_data_v3_t).
*/
struct asm_stream_cmd_set_pp_params_v3_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this handle through #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %asm_stream_param_data_v3_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  data_payload_size;
    /**< Actual size of the variable payload accompanying the message or in
         shared memory. This field is used for parsing both in-band and
         out-of-band data.

         @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_set_pp_params_v3_t */

/* Payload format for stream parameter data. */
typedef struct asm_stream_param_data_v3_t asm_stream_param_data_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_param_data_v3_t
@{ */
/* Payload of the stre+am parameter data of the
    ASM_STREAM_CMD_SET_PP_PARAMS_V3 command. */
/** Immediately following this structure are param_size bytes of parameter
    data, which must be aligned to 4 bytes. The structure and size depend on
    the module_id/param_id combination.
 */
struct asm_stream_param_data_v3_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint16_t                  instance_id;
    /**< Valid ID of the module instance. */

    uint16_t                   reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_param_data_v3_t */


/** @ingroup asmstrm_cmd_get_pp_params_v2
    Queries for an audio processing service parameter of a stream.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_GET_PP_PARAMS_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_get_pp_params_v2_t}
    @tablens{weak__asm__stream__cmd__get__pp__params__v2__t}

  @return
    #ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_STREAM_CMD_GET_PP_PARAMS_V2                                0x00010DA2

/* Payload format for a stream get parameters command. */
typedef struct asm_stream_cmd_get_pp_params_v2_t asm_stream_cmd_get_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_get_pp_params_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_GET_PP_PARAMS_V2 command, which allows
    a query for one pre/postprocessing parameter of a stream. */
/** Immediately following this structure are param_size bytes of parameter
    data. The structure and size depend on the module_id/param_id combination.
*/
struct asm_stream_cmd_get_pp_params_v2_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         For out-of-band messages, the aDSP returns this handle through
         #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %asm_session_param_data_v2_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint16_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_get_pp_params_v2_t */

/** @ingroup asmstrm_cmd_get_pp_params_v3
    Queries for an audio processing service parameter of a stream.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_GET_PP_PARAMS_V3
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_get_pp_params_v3_t}
    @tablens{weak__asm__stream__cmd__get__pp__params__v3__t}

  @return
    #ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_STREAM_CMD_GET_PP_PARAMS_V3                                0x0001320E

/* Payload format for a stream get parameters command. */
typedef struct asm_stream_cmd_get_pp_params_v3_t asm_stream_cmd_get_pp_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_get_pp_params_v3_t
@{ */
/* Payload of the ASM_STREAM_CMD_GET_PP_PARAMS_V3 command, which allows
    a query for one pre/postprocessing parameter of a stream. */
/** Immediately following this structure are param_size bytes of parameter
    data. The structure and size depend on the module_id/param_id combination.
*/
struct asm_stream_cmd_get_pp_params_v3_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         For out-of-band messages, the aDSP returns this handle through
         #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %asm_stream_param_data_v3_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint16_t                  instance_id;
    /**< Valid ID of the module instance. */

    uint16_t                   reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_get_pp_params_v3_t */

/** @ingroup asmstrm_cmd_get_pp_topo_mod_list
    Queries the ASM service for a list of modules in a POPP/POPreP topology.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID  1 -- #ASM_MAX_STREAMS_PER_SESSION

  @msgpayload{asm_stream_cmd_get_pp_topo_module_list_t}
    @tablens{weak__asm__stream__cmd__get__pp__topo__module__list__t}

  @par Parameter data variable payload (audproc_topology_info_t)
    @table{weak__audproc__topology__info__t}

  @return
    #ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST                   0x00012F21

/* Payload format for a stream get topo modules list command. */
typedef struct asm_stream_cmd_get_pp_topo_module_list_t asm_stream_cmd_get_pp_topo_module_list_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_get_pp_topo_module_list_t
@{ */
/* Payload of the ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST command, which allows
    a query for one pre/postprocessing parameter of a stream. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%audproc_topology_info_t).
    For out-of-band data, the payload is present at the shared memory address
    provided by the client.
*/
struct asm_stream_cmd_get_pp_topo_module_list_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         For out-of-band messages, the aDSP returns this handle through
         #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - Parameter data payloads in the acknowledgment are within the
              message payload.
         - Non-NULL -- Out-of-band
            - Parameter data payloads in the acknowledgment begin at the
              shared memory address specified by data_payload_addr_lsw and
              data_payload_addr_msw.
            - The total size is param_max_size bytes. @tablebulletend */

    uint16_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_get_pp_topo_module_list_t */

/** @ingroup asmstrm_cmd_get_pp_topo_mod_list_v2
    Queries the ASM service for a list of modules in a POPP/POPreP
    topology.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID  1 -- #ASM_MAX_STREAMS_PER_SESSION

  @msgpayload{asm_stream_cmd_get_pp_topo_module_list_v2_t}
    @tablens{weak__asm__stream__cmd__get__pp__topo__module__list__v2__t}

  @par Parameter data variable payload (audproc_topology_info_t)
    Immediately following this structure are num_modules of the
    <b>Module instance payload</b> (%audproc_topology_module_instance_info_t).
    @tablens{weak__audproc__topology__info__t}

  @par Module instance payload (audproc_topology_module_instance_info_t)
    @table{weak__audproc__topology__module__instance__info__t}

  @return
    #ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2

  @dependencies
    This command must be applied to a valid stream.
    @par
    A memory map handle must be obtained from the aDSP using the
    #ASM_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2                   0x00013210

/* Payload format for a stream get topo modules list command. */
typedef struct asm_stream_cmd_get_pp_topo_module_list_v2_t asm_stream_cmd_get_pp_topo_module_list_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_get_pp_topo_module_list_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2 command, which allows
    a query for one pre/postprocessing parameter of a stream. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%audproc_topology_info_t).
    For out-of-band data, the payload is present at the shared memory address
    provided by the client.
*/
struct asm_stream_cmd_get_pp_topo_module_list_v2_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the physical payload address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the physical payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be aligned to a 32-byte boundary and be
         contiguous in the virtual memory space of the aDSP.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         For out-of-band messages, the aDSP returns this handle through
         #ASM_CMD_SHARED_MEM_MAP_REGIONS.
         For in-band messages, this field must be set to 0.

         @contcell
         @values
         - NULL -- In-band
            - Parameter data payloads in the acknowledgment are within the
              message payload.
         - Non-NULL -- Out-of-band
            - Parameter data payloads in the acknowledgment begin at the
              shared memory address specified by data_payload_addr_lsw and
              data_payload_addr_msw.
            - The total size is param_max_size bytes. @tablebulletend */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_get_pp_topo_module_list_v2_t */

/** @ingroup asmstrm_cmd_set_encdec_params
    Sets one parameter for the encoder/decoder on a stream. This command is
    typically used to change encoder/decoder settings during runtime.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_SET_ENCDEC_PARAM
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_set_encdec_param_t}
    @tablens{weak__asm__stream__cmd__set__encdec__param__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid stream. @newpage
*/
#define ASM_STREAM_CMD_SET_ENCDEC_PARAM                             0x00010C10

/* Payload format for an ASM stream Set Encoder/Decoder Parameter
    command. */
typedef struct asm_stream_cmd_set_encdec_param_t asm_stream_cmd_set_encdec_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_set_encdec_param_t
@{ */
/* Payload of the ASM_STREAM_CMD_SET_ENCDEC_PARAM command. */
/** Immediately following this structure are param_size bytes representing
    the parameter data.
*/
struct asm_stream_cmd_set_encdec_param_t
{
    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values
         - #ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2
         - #ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP
         - #ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION
         - #ASM_PARAM_ID_REG_DEC_ERR_EVENT

         Proprietary and non-proprietary parameters are also supported. */

    uint32_t                  param_size;
    /**< Size of the parameter data.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_set_encdec_param_t */

/** @ingroup asmstrm_cmd_set_encdec_params
    ID of the Audio Encoder/Decoder Bitrate parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @msgpayload{asm_bitrate_param_t}
    @table{weak__asm__bitrate__param__t}
*/
#define ASM_PARAM_ID_ENCDEC_BITRATE                                 0x00010C13

/* Structure for the bitrate parameter. */
typedef struct asm_bitrate_param_t asm_bitrate_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_bitrate_param_t
@{ */
/* Payload of the ASM_PARAM_ID_ENCDEC_BITRATE parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_bitrate_param_t
{
    uint32_t                  bitrate;
    /**< Maximum supported bitrate. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_bitrate_param_t */

/** @ingroup asmstrm_cmd_set_encdec_params
    ID of the Immediate Decode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @msgpayload{asm_enc_cfg_blk_param_v2_t}
    @tablens{weak__asm__enc__cfg__blk__param__v2__t} @newpage
*/
#define ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2                             0x00010DA3

/* Structure for an encoder configuration block. */
typedef struct asm_enc_cfg_blk_param_v2_t asm_enc_cfg_blk_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_enc_cfg_blk_param_v2_t
@{ */
/* Payload of the ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2 parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command. */
/** Immediately following this structure is an encoder configuration block
    of size enc_cfg_blk_size bytes (see Chapter @xref{dox:AsmStreamMediaFormats}).
*/
struct asm_enc_cfg_blk_param_v2_t
{
    uint32_t                  frames_per_buf;
    /**< Number of encoded frames to pack into each buffer.

         @values > 0

         @note1hang This value is simply guidance information for the aDSP. The
                    number of encoded frames put into each buffer (specified
                    by the client) is less than or equal to this value. */

    uint32_t                  enc_cfg_blk_size;
    /**< Size of the encoder configuration block that follows this member.

         @values @ge 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_enc_cfg_blk_param_v2_t */

/** @ingroup asmstrm_resp_get_pp_topo_mod_list
    Returns a list of POPP/POPreP topology modules in response to an
    #ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST command.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @msgpayload{asm_stream_cmdrsp_get_pp_topo_module_list_t}
    @table{weak__asm__stream__cmdrsp__get__pp__topo__module__list__t}

  @return
    None.

  @dependencies
    This command must be applied to a valid stream.
*/
#define ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST                           0x00012F22

/* Payload for an ASM stream get pre/postprocessing parameters ACK event. */
typedef struct asm_stream_cmdrsp_get_pp_topo_module_list_t asm_stream_cmdrsp_get_pp_topo_module_list_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmdrsp_get_pp_topo_module_list_t
@{ */
/* Payload of the ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST message, which returns
    module ids in response to an ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST command.
 */
struct asm_stream_cmdrsp_get_pp_topo_module_list_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmdrsp_get_pp_topo_module_list_t */

/** @ingroup asmstrm_resp_get_pp_topo_mod_list_v2
    Returns a list of POPP/POPreP topology modules in response to an
    #ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
    command.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @msgpayload{asm_stream_cmdrsp_get_pp_topo_module_list_v2_t}
    @tablens{weak__asm__stream__cmdrsp__get__pp__topo__module__list__v2__t}

  @par Parameter data variable payload (audproc_topology_info_t)
    Immediately following this structure are num_modules of the
    <b>Module instance payload</b> (%audproc_topology_module_instance_info_t).
    @tablens{weak__audproc__topology__info__t}

  @par Module instance payload (audproc_topology_module_instance_info_t)
    @table{weak__audproc__topology__module__instance__info__t} @newpage

  @return
    None.

  @dependencies
    #ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2 must have been applied to a
    valid stream.
*/
#define ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2                           0x00013211

/* Payload for an ASM stream get pre/postprocessing parameters ACK event. */
typedef struct asm_stream_cmdrsp_get_pp_topo_module_list_v2_t asm_stream_cmdrsp_get_pp_topo_module_list_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmdrsp_get_pp_topo_module_list_v2_t
@{ */
/* Payload of the ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2 message, which returns
    module ids-instance ids in response to an ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2 command. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%audproc_topology_info_t).
*/
struct asm_stream_cmdrsp_get_pp_topo_module_list_v2_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmdrsp_get_pp_topo_module_list_v2_t */

/** @ingroup asmstrm_resp_get_pp_params_v2
    Acknowledgment response to an #ASM_STREAM_CMD_GET_PP_PARAMS_V2 command.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmdrsp_get_pp_params_v2_t}
    @tablens{weak__asm__stream__cmdrsp__get__pp__params__v2__t}

  @par Parameter data variable payload (asm_stream_param_data_v2_t)
    @tablens{weak__asm__stream__param__data__v2__t}

  @return
    None.

  @dependencies
    This command must be applied to a valid stream.
*/
#define ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2                             0x00010DA4

/* Payload for an ASM stream get pre/postprocessing parameters ACK event. */
typedef struct asm_stream_cmdrsp_get_pp_params_v2_t asm_stream_cmdrsp_get_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmdrsp_get_pp_params_v2_t
@{ */
/* Payload of the ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2 message, which returns
    parameter values in response to an ASM_STREAM_CMD_GET_PP_PARAMS_V2 command. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%asm_stream_param_data_v2_t). The payload depends on the size of the
    parameter. For out-of-band data, the payload is present at the shared
    memory address provided by the client.
*/
struct asm_stream_cmdrsp_get_pp_params_v2_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmdrsp_get_pp_params_v2_t */

/** @ingroup asmstrm_resp_get_pp_params_v3
    Acknowledgment response to an #ASM_STREAM_CMD_SET_PP_PARAMS_V3 command.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmdrsp_get_pp_params_v3_t}
    @tablens{weak__asm__stream__cmdrsp__get__pp__params__v3__t}

  @par Parameter data variable payload (asm_stream_param_data_v3_t)
    @tablens{weak__asm__stream__param__data__v3__t}

  @return
    None.

  @dependencies
    This command must be applied to a valid stream.
*/
#define ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3                             0x0001320F

/* Payload for an ASM stream get pre/postprocessing parameters ACK event. */
typedef struct asm_stream_cmdrsp_get_pp_params_v3_t asm_stream_cmdrsp_get_pp_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmdrsp_get_pp_params_v3_t
@{ */
/* Payload of the ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3 message, which returns
    parameter values in response to an ASM_STREAM_CMD_GET_PP_PARAMS_V3 command. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%asm_stream_param_data_v3_t). The payload depends on the size of the
    parameter. For out-of-band data, the payload is present at the shared
    memory address provided by the client.
*/
struct asm_stream_cmdrsp_get_pp_params_v3_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmdrsp_get_pp_params_v3_t */

/** @ingroup asmstrm_cmd_dtmf_ctl
    Starts or stops an ASM stream DTMF tone. This command is sent to the ASM
    when a DTMF tone is generated.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_DTMF_CTL
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_dtmf_ctl_t}
    @table{weak__asm__stream__cmd__dtmf__ctl__t}

  @detdesc
    The client populates the APR token field with a unique value for each
    DTMF APR payload. The aDSP returns that token in the
    #ASM_STREAM_EVENT_DTMF_TONE_ENDED event to the client. The client uses
    the returned token to look up which DTMF tone has just ended.
    @par
    After completion of DTMF tone generation, the aDSP sends the
    ASM_STREAM_EVENT_DTMF_TONE_ENDED to the client. This event is raised:
      - When a finite duration tone is completely played out
      - When an infinite duration tone is stopped
      - When a tone is pre-empted by a new tone while still playing
      - By the destination AFE port when the last sample has been rendered
        like the #ASM_DATA_EVENT_RENDERED_EOS event.
    @par
    @keep{6} If the client sends a new DTMF command before the previous tone
    is fully played out, the new tone will pre-empt the old tone and tone
    generation will continue with the new parameters specified in the payload.
    @par
    To stop the infinite tone, the client must send the
    #ASM_STREAM_CMD_DTMF_CTL with duration=0 in the payload. After
    receiving this command, the DTMF performs proper ramp down of the tone
    to avoid any pops.
    @par
    For finite tones, there is no need to send a stop command. The library
    stops the generation of samples after a finite duration is reached.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid stream.
*/
#define ASM_STREAM_CMD_DTMF_CTL                     0x00010D69

/* Payload for an ASM stream DTMF command. */
typedef struct asm_stream_cmd_dtmf_ctl_t asm_stream_cmd_dtmf_ctl_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_dtmf_ctl_t
@{ */
/* Payload of the ASM_STREAM_CMD_DTMF_CTL command, which starts/stops
    the DTMF tone.
*/
struct asm_stream_cmd_dtmf_ctl_t
{
    uint16_t                 tone_1;
    /**< Frequency of DTMF tone 1.

         @values 100 to 4000 Hz */

    uint16_t                  tone_2;
    /**< Frequency of DTMF tone 2.

         @values 100 to 4000 Hz */

    uint16_t                  gain;
    /**< DTMF linear gain in Q13 format. */

    int16_t                  duration;
    /**< Duration of the DTMF tone in milliseconds. Duration includes both
         ramp up and ramp down.

         @values
         - -1 -- Infinite tone
         - 0 -- Disable or stop infinite tone
         - > 0 -- Finite duration @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_weak_asm_stream_cmd_dtmf_ctl_t */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** Parameter ID for decoder output channel mapping in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    The client can use this command to match the channel mapping at the output
    of the decoder to the channel mapping supported by the rendering device.
    If the same stream is to be routed to multiple rendering devices with
    different supported speaker positions, the client can use this command to
    specify a channel mapping that is a union of the speaker positions
    supported by each of the devices.

    @keep{6} This command is optional. If it is absent, the decoder
    performs a conversion from its internal channel representation. This
    conversion can be codec specific to the standard channel representation by
    preserving as many channels as possible.

    In non-tunneled mode playback mode, the client can retain the decoder
    output channel mapping. In this case, the number of channels must be set
    to 0. This option is not valid for tunneled mode.

    This command can be issued multiple times with the last command taking
    precedence.

    @msgpayload{asm_dec_out_chan_map_param_t}
    @table{weak__asm__dec__out__chan__map__param__t}
*/
#define ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP                   0x00010D82

/** Maximum number of decoder output channels. @newpage */
#define MAX_CHAN_MAP_CHANNELS                               16

/** @} *//* end_addtogroup asmstrm_cmd_set_encdec_params */

/* Structure for decoder output channel mapping. */
typedef struct asm_dec_out_chan_map_param_t asm_dec_out_chan_map_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dec_out_chan_map_param_t
@{ */
/* Payload of the ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dec_out_chan_map_param_t
{
    uint32_t                 num_channels;
    /**< Number of decoder output channels.

         @values 0 to #MAX_CHAN_MAP_CHANNELS

         A value of 0 indicates native channel mapping, which is valid only
         for NT mode. This value means the output of the decoder is
         preserved as is. */

    uint8_t                  channel_mapping[MAX_CHAN_MAP_CHANNELS];
    /**< Channel array of size num_channels.

         Channel[i] mapping describes channel i inside the decoder output
         buffer. Valid channel mapping values are to be present at the
         beginning of the array.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_dec_out_chan_map_param_t */

/** @addtogroup asmstrm_cmd_open_write_compr
@{ */
/** Opens a playback channel to route compressed audio streams to an external
    receiver after applying the necessary packetization as specified by the
    IEC 61937 and IEC 60958 standards.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_write_compressed_t}
    @table{weak__asm__stream__cmd__open__write__compressed__t} @vertspace{-25}

  @keep{4} @detdesc
    For this command, only one stream per session is allowed. Otherwise, an
    error is returned. The client can perform the packetization itself or
    have the aDSP perform packetization.
    @par
    If IEC 61937 packetization is performed by the client, this operation mode
    is called Pass-through mode. The media format block must be
    communicated, if applicable, to the aDSP through the
    #ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command before sending the actual data.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED                       0x00010D84

/** Bitmask for the IEC 61937 enable flag. */
#define ASM_BIT_MASK_IEC_61937_STREAM_FLAG                         (0x00000001UL)

/** Shift value for the IEC 61937 enable flag. */
#define ASM_SHIFT_IEC_61937_STREAM_FLAG                             0

/** Bitmask for the IEC 60958 enable flag. */
#define ASM_BIT_MASK_IEC_60958_STREAM_FLAG                          (0x00000002UL)

/** Shift value for the IEC 60958 enable flag. */
#define ASM_SHIFT_IEC_60958_STREAM_FLAG                              1

/** @} *//* end_addtogroup asmstrm_cmd_open_write_compr */

/* Payload format for open write compressed comand */
typedef struct asm_stream_cmd_open_write_compressed_t asm_stream_cmd_open_write_compressed_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_write_compressed_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED comand,
    which opens a stream for a given session ID and stream ID to be
    rendered in the compressed format.
*/
struct asm_stream_cmd_open_write_compressed_t
{
    uint32_t                    flags;
    /**< Mode flags that configure the stream for a specific format.

         @note1hang Currently, IEC 60958 packetized input streams are not
                    supported.

         @values{for bit 0} (IEC 61937 compatibility)
         - 0 -- Stream is not in IEC 61937 format
         - 1 -- Stream is in IEC 61937 format
         - To set this bit, use #ASM_BIT_MASK_IEC_61937_STREAM_FLAG and
           #ASM_SHIFT_IEC_61937_STREAM_FLAG

         @values{for bit 1} (IEC 60958 compatibility)
         - 0 -- Stream is not in IEC 60958 format
         - 1 -- Stream is in IEC 60958 format
         - To set this bit, use #ASM_BIT_MASK_IEC_60958_STREAM_FLAG and
           #ASM_SHIFT_IEC_60958_STREAM_FLAG

         @values{for bits 6 to 3} (Decoder Converter mode compatibility)
         - #ASM_CONVERTER_MODE_NONE (0x0) -- Default
         - #ASM_CONVERTER_MODE_1 (0x1) -- Currently supported value
         - 0x2-0xF -- Reserved for future use
         - To set these bits, use #ASM_BIT_MASK_DECODER_CONVERTER_FLAG and
           #ASM_SHIFT_DECODER_CONVERTER_FLAG

         @contcell
         Bit 2 is left, to be consistent with other stream commands.

         Bit 2 and all other bits are reserved; clients must set them to 0.

         For the same stream: bit 0 cannot be set to 0, and bit 1 cannot be set
         to 1. A compressed stream cannot have IEC 60958 packetization applied
         without IEC 61937 packetization. */

    uint32_t                    fmt_id;
    /**< Specifies the media type of the HDMI stream to be opened.

         This field must be set to a valid media type even if the aDSP does not
         perform IEC 61937 packetization.

         If the stream is already packetized in IEC 61937 format, the client
         can use fmt_id = 0. The aDSP will automatically detect the format for
         inserting pause bursts.

         @if OEM_only @inputfile{group__weakf__fmt__id.tex} @endif */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_write_compressed_t */

/** @ingroup asmstrm_cmd_open_read_compr
    Opens a stream to receive compressed audio content from an external
    receiver after removing the IEC&nbsp;61937 and IEC&nbsp;60958
    packetization.

    Only one stream per session is allowed for this command. Otherwise, an
    error is returned.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_READ_COMPRESSED
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_read_compressed_t}
    @table{weak__asm__stream__cmd__open__read__compressed__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_READ_COMPRESSED                        0x00010D95

/* Payload format for open read compressed comand */
typedef struct asm_stream_cmd_open_read_compressed_t asm_stream_cmd_open_read_compressed_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_read_compressed_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_READ_COMPRESSED comand, which opens
    a stream for a given session ID and stream ID to be received from an
    external source.
*/
struct asm_stream_cmd_open_read_compressed_t
{
     uint32_t                    mode_flags;
     /**< Specifies whether metadata per encoded frame is to be provided.

          @values{for bit 4}
          - 0 -- Return data buffer contains all encoded frames only; it does
            not contain frame metadata
          - 1 -- Return data buffer contains an array of metadata and encoded
            frames
          - To set the shift value for this bit, use
            #ASM_BIT_MASK_META_INFO_FLAG to set the bitmask and
            #ASM_SHIFT_META_INFO_FLAG

          All other bits are reserved; clients must set them to 0. */

    uint32_t                    frames_per_buf;
    /**< Indicates the number of frames that must be returned per read buffer.

         @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_read_compressed_t */

/** @addtogroup asmstrm_cmd_open_pull_write
@{ */
/** Creates an audio stream for PCM playback in Pull mode.

    Pull mode differs from regular playback (#ASM_STREAM_CMD_OPEN_WRITE_V3) in
    that no data commands are supported in Pull mode. The aDSP continuously
    reads data from a shared circular buffer, assuming the client writes data
    on time.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_pull_mode_write_t}
    @table{weak__asm__stream__cmd__open__pull__mode__write__t}
    This structure is followed by:
     - %asm_pull_mode_write_position_buffer_t
     - %asm_multi_channel_pcm_fmt_blk_v3_t (if dec_fmt_id is
       #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
     - %avs_shared_map_region_payload_t (repeated shared_pos_buf_num_regions
       times -- typically 1)
     - %avs_shared_map_region_payload_t (repeated shared_circ_buf_num_regions
       times)
     - %asm_pull_mode_watermark_level_t (repeated num_water_mark_levels times)

  @par Pull mode write position (asm_pull_mode_write_position_buffer_t
    @table{weak__asm__pull__mode__write__position__buffer__t}

  @par Multichannel PCM (asm_multi_channel_pcm_fmt_blk_v3_t)
    If dec_fmt_id is #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3, the size of this
    region is defined by the media format structure.
    @tablens{weak__asm__multi__channel__pcm__fmt__blk__v3__t} @newpage

  @par Memory mapped regions (avs_shared_map_region_payload_t)
    Repeated shared_pos_buf_num_regions times and shared_circ_buf_num_regions
    times.
    @tablens{weak__avs__shared__map__region__payload__t}

  @par Watermark levels (asm_pull_mode_watermark_level_t)
    If num_watermark_levels > 0.
    @tablens{weak__asm__pull__mode__watermark__level__t}

  @detdesc
    When creating a stream, the client must specify the group and stream IDs.
    The current release supports up to 15 groups, and each
    group can have up to #ASM_MAX_STREAMS_PER_SESSION streams.
    @par
    The #ASM_CMD_SHARED_MEM_MAP_REGIONS command assumes that memory is cached.
    This ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE command contains memory mapping
    information because in some cases the memory must be uncached:
     - The position buffer must always be uncached.
     - On the playback path, the circular buffer can be cached in the aDSP
       because the aDSP only reads.
    @par
    Therefore, the position buffer and the circular buffer must be from
    different regions. The aDSP automatically unmaps the mapped memory
    during stream close.
    @par
    This command contains media format information (PCM media format ID and the
    media format block) because data path commands are absent.
    Only PCM formats are supported.
    @par
    The shared position buffer is read-only for the clients.
    @par
    Gapless playback is not supported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE                          0x00010DD9

/** Bitmask for the stream_perf_mode subfield. */
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE            (0xE0000000UL)

/** Bit shift for the stream_perf_mode subfield. */
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE                29

/** @} *//* end_addtogroup asmstrm_cmd_open_pull_write */

/* position buffer for pull mode write */
typedef struct asm_pull_mode_write_position_buffer_t asm_pull_mode_write_position_buffer_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_pull_mode_write_position_buffer_t
@{ */
/* Position buffer for Pull mode writes in ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE.
*/
struct asm_pull_mode_write_position_buffer_t
{
   volatile uint32_t frame_counter;
   /**< Counter used to handle interprocessor synchronization issues associated
        with reading read_index, wall_clock_us_lsw, and wall_clock_us_msw.
        These are invalid when frame_counter = 0.

        @values @ge 0

        Read the frame_counter value both before and after reading these values
        to make sure the aDSP did not update them while the client was reading
        them. */

   volatile uint32_t read_index;
   /**< Index in bytes from where the aDSP is reading.

        @values 0 to the circular buffer size minus 1 */

   volatile uint32_t wall_clock_us_lsw;
   /**< Lower 32 bits of the wall clock time in microseconds when the
        read_index was updated.

        @values @ge 0 */

   volatile uint32_t wall_clock_us_msw;
   /**< Upper 32 bits of the wall clock time in microseconds when the
        read_index was updated.

        @values @ge 0 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_pull_mode_write_position_buffer_t */

typedef struct asm_pull_mode_watermark_level_t asm_pull_mode_watermark_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_pull_mode_watermark_level_t
@{ */
/* Payload of the watermark level structure in
    ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE.
*/
struct asm_pull_mode_watermark_level_t
{
   uint32_t watermark_level_bytes;
   /**< Watermark level in bytes. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_pull_mode_watermark_level_t */

/* Payload format for a stream open write command. */
typedef struct asm_stream_cmd_open_pull_mode_write_t asm_stream_cmd_open_pull_mode_write_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_pull_mode_write_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE command, which opens a
    write stream for a given session ID and stream ID in Pull mode. The
    dst_port field in the APR packet contains both the session ID and the
    stream ID.
    The structure of the payload consists of the following:
     - asm_stream_cmd_open_pull_mode_write_t
     - asm_multi_channel_pcm_fmt_blk_v3_t (if dec_fmt_id is
       ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
     - avs_shared_map_region_payload_t (repeated shared_pos_buf_num_regions
       times -- usually 1)
     - avs_shared_map_region_payload_t (repeated shared_circ_buf_num_regions
       times)
     - asm_pull_mode_watermark_level_t (repeated num_water_mark_levels times)
*/
struct asm_stream_cmd_open_pull_mode_write_t
{
    uint32_t                    mode_flags;
    /**< Specifies the performance mode in which this stream must be opened.

         @values{for bits 31 to 29} (stream_perf_mode subfield)
         - #ASM_LEGACY_STREAM_SESSION -- Opens a playback session by using
           regular buffers in legacy POPP.
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a playback session by using
           shortened buffers in low latency POPP.
           - Recommendation: Do not enable high latency algorithms. They might
             negate the benefits of opening a low latency stream, and they
             might also suffer quality degradation from unexpected jitter.

         @contcell
         @values{for bits 31 to 29} (cont.)
         - #ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION -- Opens a low latency
           no-processing playback stream session by using shortened buffers in
           the decoder.
           - Recommendation: To avoid processing in the AFE, match the decoding
             and AFE rates.
           - There is no POPP processing in these stream sessions.
           - This mode is intended for faster than real-time applications
             because quality of service cannot be guaranteed.
           - Recommendation: The stream and device bits per sample, number of
             channels,sampling rate, channel mapping, and all other media
             formats must match. Otherwise, the quality of audio is not
             guaranteed.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
         - #ASM_ULTRA_LOW_LATENCY_STREAM_SESSION -- Opens a ULL playback
           session by using shortened buffers in the decoder. There is no
           POPP processing in the ULL stream session.
           - Recommendation: The stream and device bits per sample, number of
             channels,sampling rate, channel mapping, and all other media
             formats must match. Otherwise, the quality of audio is not
             guaranteed.
         - To configure this subfield, use
           #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PULL_MODE_WRITE.
         - #ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION -- Opens a ULLPP
           stream by using shortened buffers in the audio path.
           - Recommendation: To avoid processing in the AFE, the sample rate,
             bit width, and channel mapping must match the device rate.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
           - A ULLPP stream can connect to a ULLPP or ULL device.
           - The client can use a different media format to route a ULLPP or
             ULL ASM stream to a ULLPP or ULL ADM device. However, the
             necessary modules must be instantiated in the postprocessing
             topology to ensure that media format conversions applied across
             the two services are compatible.
           - The same guidelines for modules in the low latency path apply to
             modules in the ULLPP path.

         All other bits are reserved; clients must set them to 0. */

    uint16_t                    sink_endpoint_type;
    /**< Type of sink endpoint.

         @values
         - 0 -- Device matrix (gateway to the hardware ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */

    uint16_t                    topo_bits_per_sample;
     /**< Number of bits per sample processed by ASM modules. This value is
          different from the input sample bits per sample.

          @values 16, 24, 32 */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.

         @values See Chapter @xref{hdr:AudioPostProc}

         This field can also be enabled through Set Parameter flags.

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

    uint32_t                    dec_fmt_id;
    /**< Configuration ID of the decoder media format.

         @values
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 @tablebulletend*/

    uint32_t                   shared_pos_buf_phy_addr_lsw;
    /**< Lower 32 bits of the physical address of the shared position buffer. */

    uint32_t                   shared_pos_buf_phy_addr_msw;
    /**< Upper 32 bits of the physical address of the shared position buffer.

         The 64-bit buffer formed by shared_pos_buf_phy_addr_lsw and
         shared_pos_buf_phy_addr_msw must have enough memory allocated for
         asm_pull_mode_write_position_buffer_t. */

    uint16_t                   shared_pos_buf_mem_pool_id;
    /**< Type of memory on which this memory region is mapped for the shared
         position buffer.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         This ID implicitly defines the characteristics of the memory.
         Characteristics include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                    shared_pos_buf_num_regions;
    /**< Number of regions to map for the shared position buffer.

         @values > 0 */

    uint32_t                    shared_pos_buf_property_flag;
    /**< Configures one common property for all regions in the payload for the
         shared position buffer. No two regions can have different properties
         for the shared position buffer.

         @values 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
           - 0 -- The shared memory address provided in
                  avs_shared_map_region_payload_t is a physical address. The
                  shared memory must be mapped (a hardware TLB entry), and a
                  software entry must be added for internal bookkeeping.
           - 1 -- The shared memory address provided in the map
                  payload[usRegions] is a virtual address. The shared memory
                  must not be mapped (because the hardware TLB entry is already
                  available), but a software entry must be added for internal
                  bookkeeping. \n
                  This bit value (1) is useful if two services within the aDSP
                  are communicating via the APR. They can directly communicate
                  via the virtual address instead of the physical address. The
                  virtual regions must be contiguous.

         Bits 31 to 1 are reserved and must be set to 0. */

    uint32_t                   shared_circ_buf_start_phy_addr_lsw;
    /**< Lower 32 bits of the 64-bit physical address of the shared circular
         buffer. */

    uint32_t                   shared_circ_buf_start_phy_addr_msw;
    /**< Upper 32 bits of the 64-bit physical address of the shared circular
         buffer. */

    uint32_t                   shared_circ_buf_size;
    /**< Number of valid bytes available in the shared circular buffer.

         @values > 0

         The value must be an integral multiple of the number of
         (sample word size * number of channels). */

    uint16_t                   shared_circ_buf_mem_pool_id;
    /**< Type of memory on which this memory region is mapped for the shared
         circular buffer.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                  shared_circ_buf_num_regions;
    /**< Number of regions to map for the shared circular buffer.

         @values > 0 */

    uint32_t                  shared_circ_buf_property_flag;
    /**< Configures one common property for all regions in the payload for the
         shared circular buffer. No two regions can have different properties
         for the shared circular buffer.

         @values 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
           - 0 -- The shared memory address provided in the
                  avs_shared_map_region_payload_t is a physical address. The
                  shared memory must be mapped (a hardware TLB entry), and a
                  software entry must be added for internal bookkeeping.
           - 1 -- The shared memory address provided in the map
                  payload[usRegions] is a virtual address. The shared memory
                  must not be mapped (because the hardware TLB entry is already
                  available), but a software entry must be added for internal
                  bookkeeping. \n
                  This bit value (1) is useful if two services within the aDSP
                  are communicating via the APR. They can directly communicate
                  via the virtual address instead of the physical address. The
                  virtual regions must be contiguous.

         Bits 31 to 1 are reserved and must be set to 0. */

    uint32_t                 num_watermark_levels;
    /**< Number of watermark-level structures of type
         asm_pull_mode_watermark_level_t following this structure.

         @values @ge 0

         If set to zero, no watermark levels are specified, and hence no
         watermark events are supported. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_pull_mode_write_t */

/** @addtogroup asmstrm_cmd_open_push_read
@{ */
/** Creates an audio stream for recording in Push mode. There is one
    preprocessing chain and one encoder.

    Push mode differs from regular record (#ASM_STREAM_CMD_OPEN_READ_V3)
    in that no data commands are supported in Push mode. The aDSP continuously
    writes data to a shared circular buffer, assuming the client reads data on
    time.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_OPEN_READ_PUSH_MODE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_open_push_mode_read_t}
    @table{weak__asm__stream__cmd__open__push__mode__read__t}
    This structure is followed by:
     - %asm_push_mode_read_position_buffer_t
     - %asm_multi_channel_pcm_enc_cfg_v3_t (if enc_cfg_id is
       #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
     - %avs_shared_map_region_payload_t (repeated shared_pos_buf_num_regions
       times -- typically 1)
     - %avs_shared_map_region_payload_t (repeated shared_circ_buf_num_regions
       times)
     - %asm_push_mode_watermark_level_t (repeated num_watermark_levels times)
     @newpage

  @par Push mode read position (asm_push_mode_read_position_buffer_t)
    @table{weak__asm__push__mode__read__position__buffer__t}

  @par Multichannel PCM (asm_multi_channel_pcm_enc_cfg_v3_t)
    If enc_cfg_id is #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3.
    The size of this regions is defined by the media format structure.
    @tablens{weak__asm__multi__channel__pcm__enc__cfg__v3__t}

  @par Memory mapped regions (avs_shared_map_region_payload_t)
    Repeated shared_pos_buf_num_regions times and shared_circ_buf_num_regions
    times.
    @tablens{weak__avs__shared__map__region__payload__t}
    @newpage

  @par Watermark levels (asm_push_mode_watermark_level_t)
    If num_watermark_levels is > 0 (in asm_stream_cmd_open_push_mode_read_t):
    @tablens{weak__asm__push__mode__watermark__level__t}

  @detdesc
    This command creates a new record stream in Push mode. It specifies the
    media format of the encoding to be performed by the aDSP. The encoder
    configuration block must be communicated in this command because data
    commands indicating sample rate and channel mapping changes are absent.
    Further, Native mode is not supported (sample rate and number of 
    channels cannot be zero).
    @par
    Encoder configuration information is present in this command because data
    path commands indicating an SR_CM event are absent.
    @par
    Memory mapping information is present in this command because in some
    cases this memory must be uncached:
      - The position buffer must always be uncached.
      - On the record path, the circular buffer cannot be cached in the aDSP
        because the aDSP writes in bytes and the byte length cannot match the
        aDSP cache line.
    @par
    @keep{3} Therefore, the position buffer and the circular buffer must be
    from different regions. The mapped memory is unmapped automatically by the
    aDSP during stream close.
    @par
    This command contains media format information (PCM media format ID and the
    media format block) because data path commands are absent.
    Only PCM formats are supported.
    @par
    @note1hang Sending #ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for
               changing the encoder configuration is not allowed.\n
               The #ASM_STREAM_CMD_FLUSH_READBUFS command is not supported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
#define ASM_STREAM_CMD_OPEN_PUSH_MODE_READ                                    0x00010DDA

/** Bitmask for the stream_perf_mode subfield. */
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ            (0xE0000000UL)

/** Bit shift for the stream_perf_mode subfield. */
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ                     29

/** @} *//* end_addtogroup asmstrm_cmd_open_push_read */

/* position buffer for push mode read */
typedef struct asm_push_mode_read_position_buffer_t asm_push_mode_read_position_buffer_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_push_mode_read_position_buffer_t
@{ */
/* Position buffer for Push mode reads in ASM_STREAM_CMD_OPEN_READ_PUSH_MODE.
*/
struct asm_push_mode_read_position_buffer_t
{
   volatile uint32_t frame_counter;
   /**< Counter used to handle interprocessor synchronization issues associated
        with reading write_index, wall_clock_us_lsw, and wall_clock_us_msw.
        These are invalid when frame_counter = 0.

        @values @ge 0

        Read the frame_counter value both before and after reading these values
        to make sure the aDSP did not update them while the client was reading
        them. */

   volatile uint32_t write_index;
   /**< Index in bytes to where the aDSP is writing.

        @values 0 to the circular buffer size minus 1 */

   volatile uint32_t wall_clock_us_lsw;
   /**< Lower 32 bits of the 64-bit wall clock time in microseconds when the
        read_index was updated.

        @values @ge 0 */

   volatile uint32_t wall_clock_us_msw;
   /**< Upper 32 bits of the 64-bit wall clock time in microseconds when the
        read_index was updated.

        @values @ge 0 */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_push_mode_read_position_buffer_t */

typedef struct asm_push_mode_watermark_level_t asm_push_mode_watermark_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_push_mode_watermark_level_t
@{ */
/* used by ASM_STREAM_CMD_OPEN_READ_PUSH_MODE */
struct asm_push_mode_watermark_level_t
{
   uint32_t watermark_level_bytes;
   /**< Watermark level in bytes. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_push_mode_watermark_level_t */

/* Payload format for a stream open push mode read command. */
typedef struct asm_stream_cmd_open_push_mode_read_t asm_stream_cmd_open_push_mode_read_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_stream_cmd_open_push_mode_read_t
@{ */
/* Payload of the ASM_STREAM_CMD_OPEN_PUSH_MODE_READ command, which opens a
    read stream for a given session ID and stream ID. The APR packet contains
    both the session ID and the stream ID.
    The structure of the payload consists of the following:
     -asm_stream_cmd_open_push_mode_read_t
     - asm_multi_channel_pcm_enc_cfg_v3_t (if enc_cfg_id is
       ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3)
     - avs_shared_map_region_payload_t (repeated shared_pos_buf_num_regions
       times -- usually 1)
     - avs_shared_map_region_payload_t (repeated shared_circ_buf_num_regions
       times)
     -asm_push_mode_watermark_level_t (repeated num_watermark_levels times)
*/
struct asm_stream_cmd_open_push_mode_read_t
{
    uint32_t                    mode_flags;
    /**< Specifies whether the session is low latency.

         @values{for bits 31 to 29} (stream_perf_mode subfield)
         - #ASM_LEGACY_STREAM_SESSION -- Opens a record session using regular
           buffers in legacy POPreP.
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a record session using
           shortened buffers in low latency POPreP.
           - Recommendation: Do not enable high latency algorithms. They might
             negate the benefits of opening a low latency stream, and they
             might also suffer quality degradation from unexpected jitter.

         @contcell
         @values{for bits 31 to 29} (cont.)
         - #ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION -- Opens a low latency
           no-processing record stream session by using shortened buffers in the
           encoder.
           - Recommendation: To avoid processing in the AFE, match the decoding
             and AFE rates.
           - There is no POPP processing in these stream sessions.
           - This mode is intended for faster than real-time applications
             because quality of service cannot be guaranteed.
           - The stream and device bits per sample, number of channels,
             sampling rate, channel mapping, and all other media formats must
             match.
           - Only PCM 16-bit/24-bit/32-bit data formats are supported.
           - The client can open multiple concurrent sessions in this mode.
         - To configure this subfield, use
           #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_PUSH_MODE_READ.

         All other bits are reserved; clients must set them to 0. */

    uint16_t                    src_endpoint_type;
    /**< Device matrix endpoint providing the input samples.

         @values
         - 0 -- Device matrix (gateway from the tunneled Tx ports)
         - All other values are reserved

         Clients must set this field to 0. Otherwise, an error is returned. */


    uint16_t                    topo_bits_per_sample;
     /**< Number of bits per sample processed by the ASM modules.
          This value is different from the output sample bits per sample.

          @values 16, 24, 32 */

    uint32_t                    preproc_topo_id;
    /**< Specifies the topology (order of processing) of preprocessing
         algorithms. <i>None</i> means no preprocessing.

         @values See Chapter @xref{hdr:AudioPostProc}

         This field can also be enabled through Set Parameter flags.

         Topologies can be added through #ASM_CMD_ADD_TOPOLOGIES. */

    uint32_t                     enc_cfg_id;
    /**< Media configuration ID for the encoded output.
         Native mode is not supported.

         @values
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 @tablebulletend */

    uint32_t                   shared_pos_buf_phy_addr_lsw;
    /**< Lower 32 bits of the physical address of the shared position buffer. */

    uint32_t                   shared_pos_buf_phy_addr_msw;
    /**< Upper 32 bits of the physical address of the shared position buffer.

         Enough memory must be allocated for
         #asm_push_mode_read_position_buffer_t. */

    uint16_t                  shared_pos_buf_mem_pool_id;
    /**< Type of memory on which this memory region is mapped for the shared
         position buffer.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                  shared_pos_buf_num_regions;
    /**< Number of regions to map for the shared position buffer.

         @values Any value greater than zero */

    uint32_t                  shared_pos_buf_property_flag;
    /**< Configures one common property for all regions in the payload. No
         two regions can have different properties for the shared position
         buffer.

         @values 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
           - 0 -- The shared memory address provided in
                  avs_shared_map_region_payload_t is a physical address. The
                  shared memory must be mapped (a hardware TLB entry), and a
                  software entry must be added for internal bookkeeping.
           - 1 -- The shared memory address provided in the map
                  payload[usRegions] is a virtual address. The shared memory
                  must not be mapped (because the hardware TLB entry is already
                  available), but a software entry must be added for internal
                  bookkeeping. \n
                  This bit value (1) is useful if two services within the aDSP
                  are communicating via the APR. They can directly communicate
                  via the virtual address instead of the physical address. The
                  virtual regions must be contiguous.

         Bits 31 to 1 are reserved and must be set to 0. */

    uint32_t                   shared_circ_buf_start_phy_addr_lsw;
    /**< Lower 32 bits of the 64-bit physical address of the shared circular
         buffer. */

    uint32_t                   shared_circ_buf_start_phy_addr_msw;
    /**< Upper 32 bits of the 64-bit physical address of the shared circular
         buffer. */

    uint32_t                   shared_circ_buf_size;
    /**< Number of valid bytes available in the shared circular buffer.

         @values > 0

         The value must be an integral multiple of the number of
         (sample word size * number of channels). */

    uint16_t                   shared_circ_buf_mem_pool_id;
    /**< Type of memory on which this memory region is mapped for the shared
         circular buffer.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                  shared_circ_buf_num_regions;
    /**< Number of regions to map for the shared circular buffer.

         @values > 0 */

    uint32_t                  shared_circ_buf_property_flag;
    /**< Configures one common property for all regions in the payload. No
         two regions can have different properties for the shared circular
         buffer.

         @values 0x00000000 to 0x00000001

         Bit 0 is the IsVirtual flag that indicates physical or virtual
         mapping:
           - 0 -- The shared memory address provided in
                  avs_shared_map_region_payload_t is a physical address. The
                  shared memory must be mapped (a hardware TLB entry), and a
                  software entry must be added for internal bookkeeping.
           - 1 -- The shared memory address provided in the map
                  payload[usRegions] is a virtual address. The shared memory
                  must not be mapped (because the hardware TLB entry is already
                  available), but a software entry must be added for internal
                  bookkeeping. \n
                  This bit value (1) is useful if two services within the aDSP
                  are communicating via the APR. They can directly communicate
                  via the virtual address instead of the physical address. The
                  virtual regions must be contiguous.

         Bits 31 to 1 are reserved and must be set to 0. */

    uint32_t                 num_watermark_levels;
    /**< Number of watermark-level structures of type
         %asm_push_mode_watermark_level_t following this structure.

         @values @ge 0

         If set to zero, no watermark levels are specified, and hence no
         watermark events are supported. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_open_read_v3_t */

/** @ingroup asmstrm_cmd_set_encdec_params
    ID of the Register Decoder Error Event parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter registers or deregisters how many consecutive decoder errors
    occur before an event is raised. The HLOS is notified that either a serious
    decoding error has occurred or that the decoder format is wrong.

    @msgpayload(asm_dec_err_param_t)
    @table{weak__asm__dec__err__param__t} @newpage
*/
#define ASM_PARAM_ID_REG_DEC_ERR_EVENT                                0x00010DF6

typedef struct asm_dec_err_param_t asm_dec_err_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dec_err_param_t
@{ */
/* Payload of the ASM_PARAM_ID_REG_DEC_ERR_EVENT parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dec_err_param_t
{
    uint16_t                 n_dec_err_threshold;
    /**< Specifies how many consecutive decoder errors occur before an event
         is raised.

         @values
         - 0 -- Deregister event raising
         - > 0 @tablebulletend */

    uint16_t                 timeout_ms;
    /**< Specifies how many milliseconds occur before an event is raised
         on consecutive decoder errors before the n_dec_err_threshold is met.

         @values
         - 0 -- No timeout; always wait until the threshold is reached
         - All positive values @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_dec_err_param_t */


/** @ingroup asmstrm_cmd_set_encdec_params
    ID of the parameter to be used with #ASM_STREAM_CMD_SET_ENCDEC_PARAM
    for configuring early-EoS notification for Gapless mode.

    This notification makes sense only for tunneled Gapless mode, however there
    might not be any error checking. The parameter only ensures that there is
    a gap of specified time between early-EoS and actual EoS. If the specified
    time is zero, there is no early-EoS notification.

    @note1hang Qualcomm recommends setting this parameter before issuing
               #ASM_SESSION_CMD_RUN_V2.

    The decoder is not guaranteed to provide the requested delay. The delay is
    guaranteed only when the input data is always available and decoding is
    fast enough to fill the buffers before they are rendered. Also, for
    timestamp discontinuities, delay is not supported.

    The time value given for early-EoS is a session property (not a stream
    property). This is because there is no reason for different streams in a
    session to have different time values for early-EoS. However, currently
    there are no commands to set session properties, so use
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM.

    @msgpayload(asm_dec_cfg_early_eos_notif_t)
    @table{weak__asm__dec__cfg__early__eos__notif__t} @newpage
*/
#define ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION                                0x00012F3B

/* Structure for early-eos configuration. */
typedef struct asm_dec_cfg_early_eos_notif_t asm_dec_cfg_early_eos_notif_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dec_cfg_early_eos_notif_t
@{ */
/* Payload of the ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dec_cfg_early_eos_notif_t
{
    uint32_t                 time_ms;
    /**< Time in milliseconds.

         @values 0 <= time <= 300 ms (Default = 0)

         This parameter decides the approximate time at which early-EoS
         notification (#ASM_DATA_EVENT_EARLY_EOS) is raised before the actual
         EoS notification (#ASM_DATA_EVENT_RENDERED_EOS_V2).

         The time is counted only after the decoder receives
         #ASM_DATA_CMD_EOS_V2.
          - 0 -- Disables the notification
          - 300 ms -- Maximum limit to reduce misuse
          @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_dec_cfg_early_eos_notif_t */

/** @ingroup asmstrm_cmd_reg_pp_evts
    Enables the client to register POPP/POPreP events with the ASM.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_REGISTER_PP_EVENTS

  @apr_msgpayload{asm_stream_cmd_register_pp_events_t}
    Immediately following this structure is an
    asm_stream_cmd_register_pp_events_payload_t structure for each event.
    @tablens{weak__asm__stream__cmd__register__pp__events__t}

  @apr_msgpayload{asm_stream_cmd_register_pp_events_payload_t}
    @table{weak__asm__stream__cmd__register__pp__events__payload__t}

  @detdesc
    The client sends the details about the number of events to be registered
    and details of each event. Details include:
    - Event ID
    - Associated module ID
    - Instance ID of the module to which this event is intended

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The stream must have been opened.
*/
#define ASM_STREAM_CMD_REGISTER_PP_EVENTS                    0x00013213

typedef struct asm_stream_cmd_register_pp_events_t asm_stream_cmd_register_pp_events_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_cmd_register_pp_events_t
@{ */
/* Payload of the ASM_STREAM_CMD_REGISTER_PP_EVENTS command
*/
struct asm_stream_cmd_register_pp_events_t
{
   uint32_t version;
   /**< Version number. The current version is 0. */

   uint32_t num_events;
   /**< Number of events to be registered with the service 

        Each event has the structure of adm_register_pp_events_payload_t. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_weak_asm_stream_cmd_register_pp_events_t */

/** @addtogroup audproc_common
@{ */
/** Event registration for a module. */
#define AUDPROC_MODULE_REGISTER_EVENT_FLAG    (1 >> 0)

/** Event deregistration for a module. */
#define AUDPROC_MODULE_DEREGISTER_EVENT_FLAG  (1 >> 1)

/** @} *//* end_addtogroup audproc_common */

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_cmd_register_pp_events_payload_t
@{ */
typedef struct asm_stream_cmd_register_pp_events_payload_t
{
    uint32_t event_id;
   /**< Unique ID of the event. */

    uint32_t module_id;
   /**< Valid ID of the module. */

    uint16_t instance_id;
   /**< Valid ID of the module instance in the current topology.

        If both module_id and instance_id ID are set to 0, the event is
        registered with all modules and instances in the topology.

        If module_id is set to 0 and instance_id is set to a non-zero value,
        the payload is considered to be invalid. */

    uint16_t reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t config_mask;
   /**< Bit field for enabling or disabling event registration.

        @values{for bit 0}
        - #AUDPROC_MODULE_DEREGISTER_EVENT_FLAG
        - #AUDPROC_MODULE_REGISTER_EVENT_FLAG

        Other bits are reserved and set to 0. */
}/** @cond */asm_stream_cmd_register_pp_events_payload_t/** @endcond */;

#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_register_pp_events_payload_t */

/** @ingroup asmstrm_evt_pp
    Raised by the service when a POPP/POPreP event occurs. The client must
    have registered for the event through #ADM_CMD_REGISTER_PP_EVENTS.

    @msgpayload{audproc_event_response_payload_header_t}
    Immediately following this header payload are event-specific payloads.
    @tablens{weak__audproc__event__response__payload__header__t}
 */
#define ASM_STREAM_PP_EVENT                            0x00013214

/** @ingroup asmstrm_cmd_set_encdec_params_v2
    Sets one parameter for the encoder/decoder on a stream. This command is
    typically used to change encoder/decoder settings during runtime.

    This version differs from #ASM_STREAM_CMD_SET_ENCDEC_PARAM in that it
    allows for encoder and decoder service IDs.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_SET_ENCDEC_PARAM_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_stream_cmd_set_encdec_param_v2_t}
    @tablens{weak__asm__stream__cmd__set__encdec__param__v2__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid stream. @newpage
*/
#define ASM_STREAM_CMD_SET_ENCDEC_PARAM_V2                             0x00013218

/** @addtogroup asm_common
@{ */
/** ID of the ASM encoder service */
#define ASM_ENCODER_SVC 0

/** ID of the ASM decoder service */
#define ASM_DECODER_SVC 1

/** @} *//* end_addtogroup asm_common */

/* Payload format for an ASM stream Set Encoder/Decoder Parameter
    command. */
typedef struct asm_stream_cmd_set_encdec_param_v2_t asm_stream_cmd_set_encdec_param_v2_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_cmd_set_encdec_param_v2_t
@{ */
/* Payload of the ASM_STREAM_CMD_SET_ENCDEC_PARAM_V2 command. */
/** Immediately following this structure are param_size bytes representing
    the parameter data.
*/
struct asm_stream_cmd_set_encdec_param_v2_t
{
    uint16_t                   service_id;
    /**< ID of the encoder or decoder service.

         @values
         - #ASM_ENCODER_SVC
         - #ASM_DECODER_SVC @tablebulletend */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values
         - #ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2
         - #ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP
         - #ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION
         - #ASM_PARAM_ID_REG_DEC_ERR_EVENT

         Proprietary and non-proprietary parameters are also supported. */

    uint32_t                  param_size;
    /**< Size of the parameter data.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_set_encdec_param_v2_t */

/** @addtogroup asmstrm_cmd_reg_encdec_evts
@{ */
/** Command for registering events with the encoder or decoder service.

  @apr_hdr_fields
    Opcode -- ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS

  @msgpayload{asm_stream_cmd_register_encdec_events_t}
    @tablens{weak__asm__stream__cmd__register__encdec__events__t}

  @apr_msgpayload{asm_stream_cmd_register_encdec_event_t}
    @table{weak__asm__stream__cmd__register__encdec__event__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    To register an event with a stream, the stream must have been opened.
*/
#define ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS  0x00013219

/** Shift value for enabling or disabling encoder/decoder events. */
#define ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__SHIFT 0x0

/** Mask value for enabling or disabling encoder/decoder events. */
#define ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__MASK  0x1

typedef struct asm_stream_cmd_register_encdec_event_t  asm_stream_cmd_register_encdec_event_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_cmd_register_encdec_event_t
@{ */
struct asm_stream_cmd_register_encdec_event_t
{
   uint32_t       event_id;
   /**< Unique ID of the event. */

   uint32_t       config_mask;
   /**< Bit field for enabling event registration.

        @values{for bit 0}
        - #ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__SHIFT
        - #ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__MASK

        Other bits are reserved and set to 0. */
};
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_register_encdec_event_t */

/* struct of the payload of the command to register encoder and decoder events. */
typedef struct asm_stream_cmd_register_encdec_events_t asm_stream_cmd_register_encdec_events_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_cmd_register_encdec_events_t
@{ */
/* struct of the payload of the command to register encoder and decoder events. */
/** Following this structure are num_events of the <b>Event payload</b>
    (%asm_stream_cmd_register_encdec_event_t).
 */
struct asm_stream_cmd_register_encdec_events_t
{
   uint16_t       version;
   /**< Version of this structure. Currently, only version 0 is supported. */

   uint16_t       service_id;
   /**< ID of the service to which the event is registered.

        @values
        - #ASM_ENCODER_SVC
        - #ASM_DECODER_SVC @tablebulletend */

   uint32_t       num_events;
   /**< Number of events to be registered. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_cmd_register_encdec_events_t */
/** @} *//* end_addtogroup asmstrm_cmd_reg_encdec_evts */

/** @ingroup asmstrm_evt_encdec
    Event raised by the encoder or decoder service.
    This event uses the same version as registration.

    @msgpayload{asm_stream_encdec_event_t}
    @table{weak__asm__stream__encdec__event__t}
 */
#define ASM_STREAM_ENCDEC_EVENT        0x0001321A

typedef struct asm_stream_encdec_event_t asm_stream_encdec_event_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_asm_stream_encdec_event_t
@{ */
struct asm_stream_encdec_event_t
{
   uint16_t       service_id;
   /**< ID of the service.

        @values
        - #ASM_ENCODER_SVC
        - #ASM_DECODER_SVC @tablebulletend */

   uint16_t       reserved;
   /**< Used for alignment; must be set to 0. */

   uint32_t       event_id;
   /**< Unique ID of the event. */

   uint32_t       payload_size;
   /**< Size of the event payload. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_stream_encdec_event_t */

/** @ingroup asmsess_cmd_reg_iec_61937_fmt_update
    Allows a client to register for an Rx session underflow event.

  @apr_hdr_fields
    Opcode -- ASM_CMD_REGISTER_IEC_61937_FMT_UPDATE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_cmd_rgstr_iec_61937_fmt_update_t}
    @tablens{weak__asm__cmd__rgstr__iec__61937__fmt__update__t}

  @detdesc
    This command is used to register an IEC61937 media format change event.
    @par
    The event is sent to the client handle present in the received APR header.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid session.
*/
#define ASM_STREAM_CMD_REGISTER_IEC_61937_FMT_UPDATE            0x0001321C

/* Payload for an ASM session command for registering iec_619377 media format
   change update events. */
typedef struct asm_cmd_rgstr_iec_61937_fmt_update_t asm_cmd_rgstr_iec_61937_fmt_update_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_cmd_rgstr_iec_61937_fmt_update_t
@{ */
/* Payload of the ASM_STREAM_CMD_REGISTER_IEC_61937_FMT_UPDATE
    command, which is used to register iec_61937 media format change event */
/* A client can register for #ASM_CMD_REGISTER_IEC_61937_FMT_UPDATE.
    This event is sent to the client handle present in the received
    Asynchronous Packet Router (APR) header.
*/
struct asm_cmd_rgstr_iec_61937_fmt_update_t
{
    uint32_t                  enable;
    /**< Specifies whether a client is to receive events.

         @values
         - 0 -- Do not send underflow events
         - 1 -- Send underflow events @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_cmd_rgstr_iec_61937_fmt_update_t */

/** @ingroup asmsvc_iec_61937_fmt_event
   Indicates a change in the incoming compressed format.

   @apr_hdr_fields
    Opcode -- ADM_IEC_61937_MEDIA_FMT_EVENT \n
    Src_port:
           - Session ID 1 -– 15 (0 is reserved)
           - Stream ID 1 -– ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msgpayload{asm_iec_61937_media_fmt_event_t}
    @table{weak__asm__iec__61937__media__fmt__event__t}

  @detdesc
    This event occurs under the following conditions:
     - The event has been registered.
     - After registration, the incoming compressed data format is different
       from what the media format already set or reported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session must be opened through #ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK
    or #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED.
    @par
    The client must have registered for #ASM_CMD_REGISTER_IEC_61937_FMT_UPDATE.
*/
#define ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT       0x0001321D

typedef struct asm_iec_61937_media_fmt_event_t asm_iec_61937_media_fmt_event_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_iec_61937_media_fmt_event_t
@{ */
struct asm_iec_61937_media_fmt_event_t
{
   uint32_t  new_format_id;
   /**< Incoming format ID.

        @values All supported values per the IEC 61937 standard (refer to
        <i>IEC 61937</i>, Nov 2004) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_iec_61937_media_fmt_event_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_STREAM_COMMANDS_H_ */
