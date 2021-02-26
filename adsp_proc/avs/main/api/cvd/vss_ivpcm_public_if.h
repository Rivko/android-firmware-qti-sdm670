#ifndef __VSS_IVPCM_PUBLIC_IF_H__
#define __VSS_IVPCM_PUBLIC_IF_H__

/**
  @file vss_ivpcm_public_if.h
  @brief This file contains the APR API definitions for the vococder PCM
  interface.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2012, 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/avs.adsp/2.8.5/api/cvd/vss_ivpcm_public_if.h#1 $
  $Author: pwbldsvc $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Merged Doxygen comments from 8952.2.6.
04/18/12     HL    Deleted VSS_IVPCM_CMD_START, VSS_IVPCM_EVT_PUSH_BUFFER and
                   VSS_IVPCM_EVT_NOTIFY. Introduced 
                   VSS_IVPCM_CMD_START_V2, VSS_IVPCM_EVT_PUSH_BUFFER_V2 and
                   VSS_IVPCM_EVT_NOTIFY_V2.
04/18/11   AT/SB   Initial version.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * VOCODER PCM APIS                                                         *
 ****************************************************************************/

/** @addtogroup cvd_vocoder_pcm_cmd_start 
@{ */
/**
  Starts a vocoder PCM session.

  @apr_msgpayload{vss_ivpcm_cmd_start_v2_t}
  @table{weak__vss__ivpcm__cmd__start__v2__t}

  @par Tap point characteristics (vss_ivpcm_tap_point_t)
  @table{weak__vss__ivpcm__tap__point__t}

  @detdesc
  This command is used to start a vocoder PCM session by specifying one or more
  PCM tap points and a handle to the shared memory that is used for 
  vocoder PCM data buffer transfers. Only one vocoder PCM start is supported by
  the CVD module session at any given time.
  @par
  During a device switch, the client  must stop and reconfigure the
  vocoder PCM. Otherwise, for any configuration changes during a device switch,
  the client must check the new configuration information from
  #VSS_IVPCM_EVT_NOTIFY_V2 event.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  Shared memory that that is used for vocoder PCM data buffer transfers must
  first be mapped with the CVD as specified in Section
  @xref{dox:ShMemGuidelines}.
  @par
  The memory must be valid until #VSS_IVPCM_CMD_STOP is successfully completed.
  @par
  If destroying the CVD module session while vocoder PCM is running, the
  memory must be valid until APRV2_IBASIC_CMD_DESTROY_SESSION is successfully
  completed.
*/
#define VSS_IVPCM_CMD_START_V2 ( 0x00011339 )

/** Default tap point location on the Tx path. */
#define VSS_IVPCM_TAP_POINT_TX_DEFAULT ( 0x00011289 )

/** Default tap point location on the Rx path. */
#define VSS_IVPCM_TAP_POINT_RX_DEFAULT ( 0x0001128A )

/** Tap point direction is output. */
#define VSS_IVPCM_TAP_POINT_DIR_OUT ( 0 )

/** Tap point direction is input. */
#define VSS_IVPCM_TAP_POINT_DIR_IN ( 1 )

/** Tap point direction is both output and input. */
#define VSS_IVPCM_TAP_POINT_DIR_OUT_IN ( 2 )

/** Vocoder PCM sampling rate matches the operating sampling rate of the
    postprocessing chain on the VDSP at the location of the tap point. */
#define VSS_IVPCM_SAMPLING_RATE_AUTO ( 0 )

/** Vocoder PCM sampling rate is 8 kHz. */
#define VSS_IVPCM_SAMPLING_RATE_8K ( 8000 )

/** Vocoder PCM sampling rate is 16 kHz. */
#define VSS_IVPCM_SAMPLING_RATE_16K ( 16000 )

/** @} */ /* end_addtogroup cvd_vocoder_pcm_cmd_start */

/* Type definition for vss_ivpcm_tap_point_t. */
typedef struct vss_ivpcm_tap_point_t vss_ivpcm_tap_point_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivpcm_tap_point_t 
@{ */
/* Structure for specifying tap point characteristics (VSS_IVPCM_CMD_START_V2).
*/
struct vss_ivpcm_tap_point_t
{
  uint32_t tap_point;
  /**< GUID of the tap point location.

       @values
       - #VSS_IVPCM_TAP_POINT_TX_DEFAULT
       - #VSS_IVPCM_TAP_POINT_RX_DEFAULT @tablebulletend */

  uint16_t direction;
  /**< Data flow direction of the tap point.

       @values
       - #VSS_IVPCM_TAP_POINT_DIR_OUT
       - #VSS_IVPCM_TAP_POINT_DIR_IN
       - #VSS_IVPCM_TAP_POINT_DIR_OUT_IN @tablebulletend */

  uint16_t sampling_rate;
  /**< Sampling rate of the tap point. If the tap point is output/input, both
       the output and input sampling rates are the same.

       @values
       - #VSS_IVPCM_SAMPLING_RATE_AUTO
       - #VSS_IVPCM_SAMPLING_RATE_8K
       - #VSS_IVPCM_SAMPLING_RATE_16K @tablebulletend */

  uint16_t duration;
  /**< Duration of the buffer in milliseconds.

       @values Unsupported; this field must be set to zero.*/
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivpcm_tap_point_t */

/* Type definition for vss_ivpcm_cmd_start_v2_t. */
typedef struct vss_ivpcm_cmd_start_v2_t vss_ivpcm_cmd_start_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivpcm_cmd_start_v2_t
@{ */
/* Payload structure for the VSS_IVPCM_CMD_START_V2 command.
*/
struct vss_ivpcm_cmd_start_v2_t
{
  uint32_t mem_handle;
  /**< Unique identifier for the shared memory address of the vocoder PCM data
       buffer transfers. For details, see Section @xref{dox:ShMemGuidelines}. */

  uint32_t num_tap_points;
  /**< Number of tap points specified in this command. */

#if __STDC_VERSION__ >= 199901L
  vss_ivpcm_tap_point_t tap_points[];
  /**< Array of tap point specifiers of size num_tap_points. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_ivpcm_cmd_start_v2_t */

/** @addtogroup cvd_vocoder_pcm_event_push_buf
@{ */
/**
  Provides a vocoder PCM input buffer, output buffer, or both to the CVD.

  The client specifies whether it is providing an output buffer, an input
  buffer, or both using the push_buf_mask bitmask in the payload.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_ivpcm_evt_push_buffer_v2_t}
  @table{weak__vss__ivpcm__evt__push__buffer__v2__t}

  @par Buffer properties
  The buffer size is in bytes.
  @par
  For multichannel input/output, the buffer is non-interleaved and
  partitioned to hold data from each channel.
  @par
  All buffers are instantaneous and are consumed in real time.
  Only one buffer is consumed in a timetick. All additional buffers in the
  same timetick are dropped and sent back with a corresponding error bit set.
  Multiple buffers pushed are consumed in subsequent timeticks.
  @par
  The buffer format must always match the expected format:
    - Excess data in the buffer will be discarded.
    - Partial data buffers will be dropped.

  @return
  None.

  @dependencies
  The output buffer and input buffer must reside in the shared memory pointed
  to by the memory handle that was provided in the #VSS_IVPCM_CMD_START_V2.
  @par
  The memory used for the output buffer and input buffer must not overlap.
*/
#define VSS_IVPCM_EVT_PUSH_BUFFER_V2 ( 0x0001133A )

/** Push buffer event mask that indicates the output buffer is filled. */
#define VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER ( 1 )

/** Push buffer event mask that indicates the input buffer is consumed. */
#define VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER ( 2 )

/** @} */ /* end_addtogroup cvd_vocoder_pcm_event_push_buf */

/* Type definition for vss_ivpcm_evt_push_buffer_v2_t. */
typedef struct vss_ivpcm_evt_push_buffer_v2_t vss_ivpcm_evt_push_buffer_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivpcm_evt_push_buffer_v2_t 
@{ */
/* Payload structure for the VSS_IVPCM_EVT_PUSH_BUFFER_V2 command.
*/
struct vss_ivpcm_evt_push_buffer_v2_t
{
  uint32_t tap_point;
  /**< GUID of the tap point for which the buffer or buffers are provided.

       @values
       - #VSS_IVPCM_TAP_POINT_TX_DEFAULT
       - #VSS_IVPCM_TAP_POINT_RX_DEFAULT @tablebulletend */

  uint32_t push_buf_mask;
  /**< Bitmask that indicates whether an output buffer, input buffer, or both
       are provided.

       @subhead{Bitmask description}
       - Bit 0 -- Output buffer; use #VSS_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER
       - Bit 1 -- Input buffer; use #VSS_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER
       @tablebulletend */

  uint64_t out_buf_mem_address;
  /**< If bit 0 of the push_buf_mask is set, specifies the shared memory address
       of the output buffer. Otherwise, this field is ignored. */

  uint16_t out_buf_mem_size;
  /**< If bit 0 of the push_buf_mask is set, specifies the size of the
       allocated memory for the output buffer. Otherwise, this field is ignored.

       @values > 0 bytes

       The client must allocate the output buffer to accommodate the maximum
       expected sampling rate. Also, the amount of memory allocated at
       out_buf_mem_address must meet the requirement as specified in
       Section @xref{dox:ShMemGuidelines}. */

  uint64_t in_buf_mem_address;
  /**< If bit 1 of the push_buf_mask is set, specifies the shared memory address
       of the input buffer. Otherwise, this field is ignored. */

  uint16_t in_buf_mem_size;
  /**< If bit 1 of the push_buf_mask is set, specifies the size of the
       allocated memory for the input buffer. Otherwise, this field is ignored.

       @values > 0 bytes

       The amount of memory allocated at in_buf_mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}. */

  uint16_t sampling_rate;
  /**< If bit 1 of the push_buf_mask is set, indicates the sampling rate of the
       input buffer. Otherwise, this field is ignored.

       @values
       - #VSS_IVPCM_SAMPLING_RATE_8K
       - #VSS_IVPCM_SAMPLING_RATE_16K @tablebulletend */

  uint16_t num_in_channels;
  /**< If bit 1 of the push_buf_mask is set, indicates the number of channels
       contained in the input buffer. Otherwise, this field is ignored.

       @values 1 channel */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivpcm_evt_push_buffer_v2_t */

/** @addtogroup cvd_vocoder_pcm_event_notify 
@{ */
/**
  Per tap-point vocoder PCM notification issued by the CVD. This event provides
  a buffer transfer notification to the client.

  @apr_msgpayload{vss_ivpcm_evt_notify_v2_t}
  @table{weak__vss__ivpcm__evt__notify__v2__t}

  @detdesc
  This event is also overloaded to provide the timetick and various
  information to the client, depending on the use case (e.g., requested output
  buffer size, requested input buffer size and sampling rate, input buffer
  consumed notification, and output buffer filled notification). The
  interpretation of this notification is indicated by the notify_mask in the
  payload.
  @par
  @keep{6} If the timetick bit of notify_mask is set, this notification is
  used as a timetick, which is issued to the client every x milliseconds,
  where x is the PCM frame size on which the VDSP operates, and denotes the
  boundary upon which PCM buffer transfers between the client and CVD occur.
  @par
  If the timetick bit is not set, a notify event is used for buffer
  notifications only. For an output or input error in the buffer operation, the
  corresponding error bit is set in the notify_mask.
  @par
  Each notify event can accommodate any combination of timetick,
  output buffer filled, or input buffer consumed masks.
  @par
  After a #VSS_IVPCM_CMD_START_V2 command is issued, a buffer transfer between
  the client and CVD starts when the client receives the first
  #VSS_IVPCM_EVT_NOTIFY_V2 event (it cannot start before receiving the first
  event). The first notification is for buffer setup information only. The
  interpretation of #VSS_IVPCM_EVT_NOTIFY_V2 and the information it conveys to
  the client depends on the use case and the time when the information is
  issued. @vertspace{-3}

  @par Use case 1 -- Output only tap point
  First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2:
    - notify_mask = 0.
    - Indicates the requested size of the output buffer to be provided by 
      the client.
  @par
  Subsequent VSS_IVPCM_EVT_NOTIFY_V2 events:
    - Bit 0 of notify_mask is set.
    - Indicates the previously provided output buffer has been filled and its
      sampling rate and number of channels are as specified by sampling_rate
      and num_out_channels fields. This also indicates the size of the next
      output buffer to be provided by the client. @vertspace{-3}

  @par Use case 2 -- Input only tap point
  First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2:
    - notify_mask = 0
  @par
  Subsequent VSS_IVPCM_EVT_NOTIFY_V2 events:
    - Bit 1 of notify_mask is set.
    - Indicates that the previously provided input buffer has been consumed.
      This also indicates the size and sampling rate of the next input buffer
      to be provided by the client. @vertspace{-3}

  @par Use case 3 -- Output/input tap point
  The sampling rate of the input and output must always be the same in this
  case.
  @par
  First #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2:
    - notify_mask = 0.
    - Indicates the requested size of the output buffer to be provided by 
      the client. @newpage
  @par
  Second #VSS_IVPCM_EVT_NOTIFY_V2 event after #VSS_IVPCM_CMD_START_V2:
    - Bit 0 of notify_mask is set.
    - Indicates that the first provided output buffer has been filled and 
      its sampling rate and number of channels are as specified by the 
      sampling_rate and num_out_channels fields. This also indicates the size
      and sampling rate of the first input buffer to be provided by the client.
  @par
  Subsequent #VSS_IVPCM_EVT_NOTIFY_V2 events:
    - Bits 0 and 1 of notify_mask are set.
    - Indicates the previously provided output buffer has been filled and 
      its sampling rate and number of channels are as specified by the 
      sampling_rate and num_out_channels fields.
    - This also indicates that the previously provided input buffer has been
      consumed, and it indicates the requested size of the next output buffer
      and the requested size and sampling rate of the next input buffer.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IVPCM_EVT_NOTIFY_V2 ( 0x0001133B )

/** Notify event mask that indicates the output buffer is filled. */
#define VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER ( 1 )

/** Notify event mask that indicates the input buffer is consumed. */
#define VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER ( 2 )

/** Notify event mask that indicates a timetick. */
#define VSS_IVPCM_NOTIFY_MASK_TIMETICK ( 4 )

/** Notify event mask that indicates an error occured in the output buffer
    operation. */
#define VSS_IVPCM_NOTIFY_MASK_OUTPUT_ERROR ( 8 )

/** Notify event mask that indicates an error occured in the input buffer
    operation. */
#define VSS_IVPCM_NOTIFY_MASK_INPUT_ERROR ( 16 )

/** @} */ /* end_addtogroup cvd_vocoder_pcm_event_notify */

/* Type definition for vss_ivpcm_evt_notify_v2_t. */
typedef struct vss_ivpcm_evt_notify_v2_t vss_ivpcm_evt_notify_v2_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ivpcm_evt_notify_v2_t 
@{ */
/* Payload structure for the VSS_IVPCM_EVT_NOTIFY_V2 command. */
struct vss_ivpcm_evt_notify_v2_t
{
  uint32_t tap_point;
  /**< GUID that indicates the tap point location for which this notification
       is being issued. */

  uint32_t notify_mask;
  /**< Bitmask that indicates the notification mode.

       @subhead{Bitmask description}
       - Bit 0 -- Output buffer is filled; use
                  #VSS_IVPCM_NOTIFY_MASK_OUTPUT_BUFFER.
       - Bit 1 -- Input buffer is consumed; use
                  #VSS_IVPCM_NOTIFY_MASK_INPUT_BUFFER.
       - Bit 2 -- Event is a timetick; use #VSS_IVPCM_NOTIFY_MASK_TIMETICK.
       - Bit 3 -- Error in the output buffer operation; use
                  #VSS_IVPCM_NOTIFY_MASK_OUTPUT_ERROR. This bit is set if there
                  is an error in the output buffer operation. This bit is set
                  together with the output buffer filled bit to return the
                  output buffer with an error indication.
       - Bit 4 -- Error in the input buffer operation; use
                  #VSS_IVPCM_NOTIFY_MASK_INPUT_ERROR. This bit is set if there
                  is an error in the input buffer operation. This bit is set
                  together with the input buffer consumed bit to return the
                  input buffer with an error indication. @tablebulletend */

  uint64_t out_buf_mem_address;
  /**< If bit 0 of the notify_mask is set, specifies the shared memory address
       of the output buffer. Otherwise, this field is ignored. */

  uint64_t in_buf_mem_address;
  /**< If bit 1 of the notify_mask is set, specifies the shared memory address
       of the input buffer. Otherwise, this field is ignored. */

  uint16_t filled_out_size;
  /**< If bit 0 of the notify_mask is set, indicates the filled size of the
       output buffer located at the out_buf_mem_address.
       Otherwise, it is ignored. */

  uint16_t request_buf_size;
    /**< Request size of the input buffer. */

  uint16_t sampling_rate;
  /**< Sampling rate of the input/output buffer.

       @values
       - #VSS_IVPCM_SAMPLING_RATE_8K
       - #VSS_IVPCM_SAMPLING_RATE_16K @tablebulletend */

  uint16_t num_out_channels;
    /**< Number of output channels contained in the filled output buffer. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ivpcm_evt_notify_v2_t */

/** @ingroup cvd_vocoder_pcm_cmd_stop
  Stops an active vocoder PCM session.

  @apr_msgpayload
  None.

  @detdesc
  At the time this command is issued, any buffers still owned by the CVD module
  can be reclaimed by the client upon receiving a response to this command.
  @par
  If the CVD module session is destroyed while the vocoder PCM is running,
  the client can reclaim any buffers still owned by the CVD module when the
  client receives a response to the APRV2_IBASIC_CMD_DESTROY_SESSION command.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  None.
*/
#define VSS_IVPCM_CMD_STOP ( 0x0001100B )


#endif /*__VSS_IVPCM_PUBLIC_IF_H__ */

