/*======================================================================== */
/**
@file adsp_asm_session_commands.h

This file contains ASM session commands and events structures definitions.
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

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_asm_session_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited DCNs in references for AVS 2.8.
10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
05/08/15   sw      (Tech Pubs) Edited Doxygen comments for 8952.
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0.
08/06/12   sw      (Tech Pubs) Edited comments for Badger; updated Doxygen markup
                   to Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_ASM_SESSION_COMMANDS_H_
#define _ADSP_ASM_SESSION_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup asm_common
    Maximum number of streams per session. */
#define ASM_MAX_STREAMS_PER_SESSION                                 (8)

/** @addtogroup asmsess_cmd_run
@{ */
/** Starts all streams in a session. The command can take effect
    immediately, or it can be set to run at a specific absolute time.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_RUN_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmd_run_v2_t}
    @table{weak__asm__session__cmd__run__v2__t}

  @detdesc
    If the first buffer has a nonzero timestamp, it is rendered at the
    appropriate offset from the absolute start time according to its
    timestamp.
    @par
    The time value specified in this command is interpreted as a signed
    64-bit quantity. If the current wall clock is a very small value and
    the client wants to set the start time in the past, this command allows
    negative time offsets or negative absolute time to be specified.
    @par
    Run With Delay mode can be used to introduce delays in the loopback path.
    This mode is useful for AV synchronization, where audio must be
    delayed to match video delays.
    @par
    Before issuing this command, the client must query the audio path delay
    using #ASM_SESSION_CMD_GET_PATH_DELAY_V2, and compare it with the video
    delay. If the video delay is greater, the video delay is sent in this
    command. For more information, see Section
    @xref{sec:AsmLoopbackTimestamps}.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    At least one stream in this session must have been opened and not yet
    closed.
*/
/* API_developer_note: ASM_SESSION_CMD_RUN from A family is replaced with
   ASM_SESSION_CMD_RUN_V2. Reason: endianness.
*/
#define ASM_SESSION_CMD_RUN_V2                                         0x00010DAA

/** Indicates that ASM_SESSION_CMD_RUN_V2 is run immediately. */
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE                0

/** Indicates that ASM_SESSION_CMD_RUN_V2 is run at a given absolute time. */
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME         1

/** Indicates that ASM_SESSION_CMD_RUN_V2 is run at a specified offset from
    the current absolute time.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME         2

/** @} *//* end_addtogroup asmsess_cmd_run */

/** @ingroup asm_deprecated
    Deprecated. Use #ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RESERVED                     3

/** @addtogroup asmsess_cmd_run
@{ */
/** Indicates that ASM_SESSION_CMD_RUN_V2 is run with a delay. In other words,
    a delay is added to incoming timestamps, and rendering is done at the
    resulting time.

    Currently, this command is applicable only for an ASM loopback on the Rx
    playback path. It is not applicable on the Tx path.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY               3

/** Bitmask used to specify the start time for the ASM_SESSION_CMD_RUN_V2
    command.
*/
#define ASM_BIT_MASK_RUN_START_TIME                                 (0x00000003UL)

/** Bit shift value used to specify the start time for the
    ASM_SESSION_CMD_RUN_V2 command.
*/
#define ASM_SHIFT_RUN_START_TIME                                    0

/** @} *//* end_addtogroup asmsess_cmd_run */

/* ASM Session Run command payload structure. */
typedef struct asm_session_cmd_run_v2_t asm_session_cmd_run_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_run_v2_t
@{ */
/* Payload of the ASM_SESSION_CMD_RUN_V2 command, which starts all
    streams in a playback session.
*/
struct asm_session_cmd_run_v2_t
{
    uint32_t                  flags;
    /**< Specifies whether to run immediately or at a specific rendering
         time or with a specified delay.

         @values{for bits 1 to 0}
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY
         - To set this 2-bit flag, use #ASM_BIT_MASK_RUN_START_TIME and
           #ASM_SHIFT_RUN_START_TIME

         All other bits are reserved; clients must set them to 0.

         Run With Delay mode is useful for delaying the stream in the absolute
         sense. It is supported only on the playback paths and for an ASM
         loopback.

         On the Tx path, only Run Immediate mode is supported. */

    uint32_t                  time_lsw;
    /**< Lower 32 bits of the 64-bit time in microseconds used to align the
         session start/resume time. Currently, the session time is 0.

         When bits 1 to 0 of the flags field are set to
         #ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY, time_lsw is the least
         significant word of the delay in microseconds. In this case, the
         maximum value of the 64-bit delay is 150 ms. */

    uint32_t                  time_msw;
    /**< Upper 32 bits of the 64-bit time in microseconds used to align the
         session start/resume time. Currently, the session time is 0.

         When bits 1 to 0 of the flags field are set to
         #ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY, time_msw is the most
         significant word of the delay in microseconds. In this case, the
         maximum value of the 64-bit delay is 150 ms. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_run_v2_t */

/** @ingroup asmsess_cmd_suspend
    Suspends all streams in a specified session. All hardware resources held
    by the session are relinquished.

   This command can be issued only if the session is paused.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_SUSPEND
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The specified session ID must have one or more streams assigned to it.
*/
#define ASM_SESSION_CMD_SUSPEND                                     0x00010DEC

/** @ingroup asmsess_cmd_pause
    Pauses all streams in a specified playback session.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_PAUSE
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The specified session ID must have one or more streams assigned to it.
*/
#define ASM_SESSION_CMD_PAUSE                                       0x00010BD3

/** @ingroup asmsess_cmd_reg_rx_underflow
    Allows a client to register for an Rx session underflow event.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmd_rgstr_rx_underflow_t}
    @tablens{weak__asm__session__cmd__rgstr__rx__underflow__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid session.
*/
#define ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS            0x00010BD5

/* Payload for an ASM session command for registering Rx underflow events. */
typedef struct asm_session_cmd_rgstr_rx_underflow_t asm_session_cmd_rgstr_rx_underflow_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_rgstr_rx_underflow_t
@{ */
/* Payload of the ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS
    command, which is used to register Rx underflow events. */
/** A client can register for #ASM_SESSION_EVENT_RX_UNDERFLOW. This event is
    sent to the client handle present in the received Asynchronous Packet
    Router (APR) header.
*/
struct asm_session_cmd_rgstr_rx_underflow_t
{
    uint16_t                  enable_flag;
    /**< Specifies whether a client is to receive events when an Rx session
         underflows.

         @values
         - 0 -- Do not send underflow events
         - 1 -- Send underflow events @tablebulletend */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_rgstr_rx_underflow_t */

/** @ingroup asmsess_cmd_reg_tx_underflow
    Allows a client to register for a Tx session underflow event.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmd_reg_tx_overflow_t}
    @tablens{weak__asm__session__cmd__reg__tx__overflow__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    This command must be applied to a valid session.
*/
#define ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS             0x00010BD6

/* Payload for an ASM session command for registering Tx overflow events. */
typedef struct asm_session_cmd_reg_tx_overflow_t asm_session_cmd_reg_tx_overflow_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_reg_tx_overflow_t
@{ */
/* Payload of the ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS
    command, which is used to register Tx overflow events. A client can
    register for #ASM_SESSION_EVENT_TX_OVERFLOW. */
/** This event is sent to the client handle present in the received APR
    header.
*/
struct asm_session_cmd_reg_tx_overflow_t
{
    uint16_t                  enable_flag;
    /**< Specifies whether a client is to receive events when a Tx session
         overflows.

         @values
         - 0 -- Do not send overflow events
         - 1 -- Send overflow events @tablebulletend */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_reg_tx_overflow_t */

/** @ingroup asmsess_event_rx_underflow
    Reports a data starvation in a session, resulting in insertion of silence
    to the renderer.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_EVENT_RX_UNDERFLOW
    @par
    Src_port -- 0

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    None.
*/
#define ASM_SESSION_EVENT_RX_UNDERFLOW                              0x00010C17

/** @ingroup asmsess_event_tx_overflow
    Reports that the client has not provided enough empty read buffers and
    some samples had to be discarded.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_EVENT_TX_UNDERFLOW
    @par
    Src_port -- 0

  @apr_msgpayload
    None.

  @return
    None.

  @dependencies
    None.
*/
#define ASM_SESSION_EVENT_TX_OVERFLOW                               0x00010C18


/** @ingroup asmsess_cmd_adj_session_clock
    Adjusts the audio session clock for a specified session.

  @note1hang This command applies only to tunneled playback sessions;
             otherwise, an error is returned.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmd_adjust_session_clock_v2_t}
    @table{weak__asm__session__cmd__adjust__session__clock__v2__t}

  @detdesc
    When the client finds that the clock returned by
    #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 is behind the master AV
    synchronization clock, the client can issue this command with a positive
    value to advance The clock or a negative value to delay the clock.
    Because the time specified in this command might not translate exactly
    into an integral number of audio samples to insert or delete, the
    command response indicates the actual time that the aDSP will adjust
    (accurate to the nearest microsecond). The client might notice the returned
    value and use the feedback loop to adjust the clock correctly.
    @par
    Command processing latency (the time required for full adjustment
    of the session clock) is implementation specific. Typically, the larger
    the absolute value of the time specified in this command, the longer it
    takes to adjust the session clock. However, the command response is
    issued immediately, which provides latency to the command processing.
    @par
    The client can issue this command any number of times and at any rate.
    When a new command is issued, and if at this time the aDSP is still
    adjusting the session clock in response to a previous command, this
    adjustment is terminated and the aDSP starts processing the new command.
    @newpage
    @par
    If the client wants to realize this adjustment in a smoother fashion,
    #ASM_SESSION_MTMX_PARAM_ADJUST_SESSION_TIME_FINE_CTL can be set using
    #ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2. This command must be issued
    before the audio session is put into the RUN state.
    @par
    @keep{6} For command timing:
     - If the command is issued when the session is running, the session
       clock adjustment begins immediately.
     - If the command is issued when the session is paused, the session clock
       does not begin adjusting until the session is moved to the Run state.
     - If the command is issued when the session is paused and then the
       session is flushed, the session clock adjustment is canceled.

  @return
    #ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2.

  @dependencies
    At least one stream in this session must have already been opened and
    not yet closed.
    @par
    The streams must be tunneled playback streams opened through
    #ASM_STREAM_CMD_OPEN_WRITE_V3 or #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED.
*/
/* API_developer_note: ASM_SESSION_CMD_ADJUST_SESSION_CLOCK from A family is
   deleted. Reason: endianness. B family starts with V2.
*/
#define ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2                        0x00010D9F

/* Payload for the ASM session clock adjustment command. */
typedef struct asm_session_cmd_adjust_session_clock_v2_t asm_session_cmd_adjust_session_clock_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_adjust_session_clock_v2_t
@{ */
/* Payload of the ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 command.
    This is a signed 64-bit quantity that specifies an adjustment to the
    session clock.
*/
struct asm_session_cmd_adjust_session_clock_v2_t
{
   uint32_t                  adjust_time_lsw;
   /**< Lower 32 bits of the signed 64-bit quantity that specifies the
        adjustment time in microseconds to the session clock. */

    uint32_t                  adjust_time_msw;
    /**< Upper 32 bits of the signed 64-bit quantity that specifies the
         adjustment time in microseconds to the session clock.

         The 64-bit number formed by actual_adjust_time_lsw and
         actual_adjust_time_msw specifies the actual adjustment that the aDPS
         performs on the session clock. A positive value indicates advancement
         of the session clock. A negative value indicates delay of the session
         clock. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_adjust_session_clock_v2_t */

/** @ingroup asmsess_resp_adj_session_clock
    Response to an #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 command. The
    response is returned immediately after receiving the command.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmdrsp_adjust_session_clock_v2_t}
    @table{weak__asm__session__cmdrsp__adjust__session__clock__v2__t}

  @detdesc
    An error in the status code can happen if no streams are opened in
    the session or if the session is not a tunneled playback case.
    @par
    Because the time specified in #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 can
    translate exactly into an integral number of audio samples to insert or
    delete, the command response indicates the actual time that the aDSP will
    adjust (accurate to the nearest microsecond). The client might note the
    returned value and use the feedback loop to adjust the clock correctly.
    @par
    The command response also specifies the command processing latency (the
    amount of time in microseconds that the aDSP takes to complete the
    session clock adjustment).

  @return
    None.

  @dependencies
    #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 must have been issued.
*/
/* API_developer_note: ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK from A family
   is deleted. Reason: endianness. B family starts with V2.
*/
#define ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2                     0x00010DA0

/* Payload for an ASM session clock adjustment command response. */
typedef struct asm_session_cmdrsp_adjust_session_clock_v2_t asm_session_cmdrsp_adjust_session_clock_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmdrsp_adjust_session_clock_v2_t
@{ */
/* Payload of the #ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2 message.
*/
struct asm_session_cmdrsp_adjust_session_clock_v2_t
{
    uint32_t                  status;
    /**< Status message (error code). An error means the session clock is not
         adjusted. In this case, the next two fields are irrelevant.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */

    uint32_t                  actual_adjust_time_lsw;
    /**< Lower 32 bits of the signed actual time adjustment in microseconds. */

    uint32_t                  actual_adjust_time_msw;
    /**< Upper 32 bits of the signed actual time adjustment in microseconds.

         The 64-bit number formed by actual_adjust_time_lsw and
         actual_adjust_time_msw specifies the actual adjustment that the aDPS
         performs on the session clock. A positive value indicates advancement
         of the session clock. A negative value indicates delay of the session
         clock. */

    uint32_t                  cmd_latency_lsw;
    /**< Lower 32 bits of the unsigned latency value.*/

    uint32_t                  cmd_latency_msw;
    /**< Upper 32 bits of the unsigned latency value.

         The 64-bit number formed by cmd_latency_lsw and cmd_latency_msw
         specifies the amount of time in microseconds that the aDSP takes to
         perform the session clock adjustment. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmdrsp_adjust_session_clock_v2_t */

/** @ingroup asmsess_cmd_get_path_delay
    Queries for the audio delay in this session for the specified stream.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_GET_PATH_DELAY_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload
    None.

  @detdesc
    This command can be used by the client for accurate AV synchronization
    purposes. It is supported only in the following modes:
     - On the loopback path, this command gives the delay experienced by the
       audio data as it propagates from the Tx port to the Rx port. The client
       can use this information to check whether audio must be delayed with
       respect to video.
     - On the playback path (Pull mode, ULL, Gapless, and regular playback),
       this command gives the delay from the point when the data is read from
       the shared client buffers to the time it is rendered in the AFE.
       The value of the delay can vary depending on the time of the query.
     - On the Push mode record path, this command gives the delay from the time
       the AFE gets the samples to the time the encoder writes to the shared
       client buffers.
    @par
    A change in the calibration at a later time can change the actual audio
    delay. Hence, this command provides a delay only at the time of the query.
    The client must make sure the session is established and is calibrated
    before the query.
    @par
    Any hardware latency post aDSP is not included in the returned value.

  @return
    #ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2 with the status and delay values.

  @dependencies
    This command must be applied to a valid session.
    @par
    The entire session must be established and calibrated before the query.
*/
#define ASM_SESSION_CMD_GET_PATH_DELAY_V2                         0x00010DAF

/** @ingroup asmsess_resp_get_path_delay
    In response to the ASM_SESSION_CMD_GET_PATH_DELAY_V2 query, contains the
    audio delay for this session for the specified stream.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID is ignored

  @apr_msgpayload{asm_session_cmdrsp_get_path_delay_v2_t}
    @table{weak__asm__session__cmdrsp__get__path__delay__v2__t}

  @return
    None.

  @dependencies
    An #ASM_SESSION_CMD_GET_PATH_DELAY_V2 command must have been issued.
*/
#define ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2                         0x00010DB0

/* Payload for an ASM session get path delay command response. */
typedef struct asm_session_cmdrsp_get_path_delay_v2_t asm_session_cmdrsp_get_path_delay_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmdrsp_get_path_delay_v2_t
@{ */
/* Payload of the ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2 message, which
    reports the audio delay of this session.
*/
struct asm_session_cmdrsp_get_path_delay_v2_t
{
    uint32_t                  status;
    /**< Status message (error code) that indicates whether the Get Delay
         operation is successful. The delay value is valid only if status
         indicates success.

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */

    uint32_t                  audio_delay_lsw;
    /**< Lower 32 bits of the aDSP delay in microseconds. */

    uint32_t                  audio_delay_msw;
    /**< Upper 32 bits of the aDSP delay in microseconds. */

}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmdrsp_get_path_delay_v2_t */

/** @ingroup asmsess_cmd_set_matrix_rtr_par
    Sets a parameter on the matrix mixer or stream router for an open session.

    @note1hang In some use cases, like non-tunnel playback, the the matrix
               mixer or stream router entities are not supported and an error
               is returned.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- Ignored

  @apr_msgpayload{asm_session_cmd_set_mtmx_strtr_params_v2_t}
    @tablens{weak__asm__session__cmd__set__mtmx__strtr__params__v2__t}

  @par Parameter data variable payload (asm_session_param_data_v2_t)
    @tablens{weak__asm__session__param__data__v2__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q3,80-N1463-2,80-NA601-5}).

  @dependencies
    The session must be open, and it must have a matrix mixer or stream router.
*/
#define ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2                   0x00010DCE

/* Structure for a set param command on matrix or stream router. */
typedef struct asm_session_cmd_set_mtmx_strtr_params_v2_t asm_session_cmd_set_mtmx_strtr_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_set_mtmx_strtr_params_v2_t
@{ */
/* Payload of the ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2 command, which
    allows one or more parameters to be set on a matrix or stream router
    present in a session. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%asm_session_param_data_v2_t).
*/
struct asm_session_cmd_set_mtmx_strtr_params_v2_t
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
            - A series of %asm_session_param_data_v2_t structures immediately
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

    uint32_t                  direction;
    /**< Path on which the parameter is set.

         @values
         - 0 -- Rx (for Rx stream router or Rx matrix mixer)
         - 1 -- Tx (for Tx stream router or Tx matrix mixer) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_set_mtmx_strtr_param_t */

/* Structure for session parameter data. */
typedef struct asm_session_param_data_v2_t asm_session_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_param_data_v2_t
@{ */
/* Payload of the session parameter data of the
    ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2 and
    ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2 commands. */
/** Immediately following this structure are param_size bytes of parameter
    data, which must be aligned to 4 bytes. The structure and size depend on
    the module_id/param_id combination.
*/
struct asm_session_param_data_v2_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module.

         @values See Section @xref{hdr:AudioMatrixRouterModule} */

    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values See Section@ xref{hdr:AudioMatrixRouterModule} */

    uint16_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_param_data_v2_t */

/** @ingroup asmsess_cmd_get_matrix_rtr_par
    Allows a query of matrix mixer or stream router parameters on an open
    session.

    @note1hang In some use cases, like non-tunnel playback, the the matrix
               mixer or stream router entities are not supported and an error
               is returned.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2
    @par
    Dst_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- ignored

  @apr_msgpayload{asm_session_cmd_get_mtmx_strtr_params_v2_t}
    @tablens{weak__asm__session__cmd__get__mtmx__strtr__params__v2__t}

  @par Parameter data variable payload (asm_session_param_data_v2_t)
    @tablens{weak__asm__session__param__data__v2__t}

  @return
    #ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2

  @dependencies
    The session must be open, and it must have a matrix mixer or stream router.
*/
#define ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2                       0x00010DCF

/* Structure for a stream get parameters command. */
typedef struct asm_session_cmd_get_mtmx_strtr_params_v2_t asm_session_cmd_get_mtmx_strtr_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmd_get_mtmx_strtr_params_v2_t
@{ */
/* Payload of the ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2 command. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%asm_session_param_data_v2_t).
*/
struct asm_session_cmd_get_mtmx_strtr_params_v2_t
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
            - A series of %asm_session_param_data_v2_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the specified physical address.
            - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  direction;
    /**< Path on which the parameter is set.

         @values
         - 0 -- Rx (for Rx stream router or Rx matrix mixer)
         - 1 -- Tx (for Tx stream router or Tx matrix mixer) @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module.

         @values See Section @xref{hdr:AudioMatrixRouterModule} */

    uint32_t                  param_id;
    /**< Valid ID of the parameter.

         @values See Section@ xref{hdr:AudioMatrixRouterModule} */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmd_get_mtmx_strtr_params_v2_t */

/** @ingroup asmsess_resp_matrix_rtr_par
    Returns parameter values in response to an
    #ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2 command.

  @apr_hdr_fields
    Opcode -- ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2
    @par
    Src_port:
            - Session ID 1 -- 15 (0 is reserved)
            - Stream ID 1 -- ignored

  @apr_msgpayload{asm_session_cmdrsp_get_mtmx_strtr_params_v2_t}
    @tablens{weak__asm__session__cmdrsp__get__mtmx__strtr__params__v2__t}

  @par Parameter data variable payload (asm_session_param_data_v2_t)
    @tablens{weak__asm__session__param__data__v2__t} @newpage

  @return
    None.

  @dependencies
    #ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2 must have been issued.
*/
#define ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2                    0x00010DD0

/* Structure for an ASM session get matrix/stream router parameters ACK event. */
typedef struct asm_session_cmdrsp_get_mtmx_strtr_params_v2_t asm_session_cmdrsp_get_mtmx_strtr_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_session_cmdrsp_get_mtmx_strtr_params_v2_t
@{ */
/* Payload of the ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2 message, which
    returns parameter values in response to an ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2 command. */
/** For in-band data, immediately following this structure is the
    acknowledgment <b>Parameter data variable payload</b>
    (%asm_session_param_data_v2_t). The payload depends on the size of the
    parameter. For out-of-band data, the payload is present at the address
    provided by the client.
*/
struct asm_session_cmdrsp_get_mtmx_strtr_params_v2_t
{
    uint32_t                  status;
    /**< Status message (error code).

         @values Refer to @xrefcond{Q5,80-NF774-4,80-NA610-4} */
}
#include "adsp_end_pack.h"
;
/** @} *//* end_weakgroup weak_asm_session_cmdrsp_get_mtmx_strtr_params_v2_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_SESSION_COMMANDS_H_ */
