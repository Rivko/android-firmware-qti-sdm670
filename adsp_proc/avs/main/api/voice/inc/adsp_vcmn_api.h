#ifndef __ADSP_VCMN_API_H__
#define __ADSP_VCMN_API_H__

/**
  @file adsp_vcmn_api.h
  @brief This file contains API definitions common to VPM and VSM.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Voice_mainpage.dox file contains all file/group
      descriptions that are in the output PDF generated using Doxygen and
      Latex. To edit or update any of the file/group text in the PDF, edit
      the Voice_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================

  Copyright (c) 2010, 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/13   sw      (Tech Pubs) Updated Doxygen for 2.0 internal draft.
02/27/12   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/11/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
===========================================================================*/

#include "mmdefs.h"


/** @ingroup voice_common_cmd_config_param
    Configures/calibrates various settings in the voice processing and voice
    stream session. 

  @apr_hdr_fields
    Opcode -- VOICE_CMD_SET_PARAM_V2\n
    Dst_port -- Handle to the stream session

  @msg_payload{voice_set_param_v2_t}
    @tablens{weak__voice__set__param__v2__t} @newpage

  @par Parameter data variable payload (voice_param_data_t)
    @tablens{weak__voice__param__data__t}

  @description
    This command allows one or more parameters to be set on a stream. It
    is typically used to apply calibration data to the modules.
    @par
    @keep{6} Messages can be in-band or out-of-band via shared memory.
    - In-band set parameter -- In this mode, the memory map handle is set to
      NULL. The rest of the payload contains the parameters as shown in the
      variable payload structure (parameter data).
    - Shared memory set parameter -- In this mode, the physical/virtual address
      to the parameter data and the data payload size are sent. The rest of the
      message is inside the shared memory in the variable payload format.
      The memory content is the same as the in-band payload parameter data.
    - For shared memory, Voice services invalidate the cache line before
      reading the content of the memory. The memory map handle determines the
      type of address, i.e., virtual, physical, etc. A physical address is
      translated to a virtual address before the service invalidates the cache. 
      @tablebulletend

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
/* Q3 = Async Packet Router API (for MDM9x15 and Newer Devices) Interface Spec
        (80-N1463-2) */
#define VOICE_CMD_SET_PARAM_V2 ( 0x0001103C )

/* Used by the structure that contains voice parameter data. */
typedef struct voice_param_data_t voice_param_data_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_param_data_t
@{ */
/* Payload of the voice parameter data used by voice_set_param_v2_t. */
/** Immediately following this structure are param_size bytes of parameter
    data.
*/
struct voice_param_data_t
{
   uint32_t module_id;
   /**< Unique ID of the module to be configured.

        @values See Section @xref{hdr:ModuleIDs} */

   uint32_t param_id;
   /**< Unique ID of the parameter to be configured.

        @values See Section @xref{hdr:ParameterIDs} */

   uint16_t param_size;
   /**< Actual data size in bytes of the param_id/module_id combination. This is
        a multiple of four bytes.

        @values > 0 */

   uint16_t reserved;
   /**< Clients must set this field to zero. */
   /*
   int8_t param_values[param_size]

   More mod id/param id structures as above can continue below
   uint32_t next_module_id;...
   */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_param_data_t */


/* Used by the structure that contains voice parameter data associated with ELITEMSG_PARAM_ID_CAL_V2. */
typedef struct voice_param_data_v2_t voice_param_data_v2_t;

/** @weakgroup weak_voice_param_data_v2_t
@{ */
/* Payload of the voice parameter data used by voice_set_param_v2_t. */
/** Immediately following this structure are param_size bytes of parameter
    data.
*/
struct voice_param_data_v2_t
{
   uint32_t module_id;
   /**< Unique ID of the module to be configured.

        @values See Section @xref{hdr:ModuleIDs} */

   uint16_t instance_id;
   /**< Unique instance ID of a particular module to be configured.

        @values See Section @xref{hdr:InstanceIDs} */

   uint16_t reserved;
   /**< Clients must set this field to zero  */

   uint32_t param_id;
   /**< Unique ID of the parameter to be configured.

        @values See Section @xref{hdr:ParameterIDs} */

   uint32_t param_size;
   /**< Actual data size in bytes of the param_id/module_id combination. This is
        a multiple of four bytes.

        @values > 0 */

   /*
   int8_t param_values[param_size]

   More mod id/param id structures as above can continue below
   uint32_t next_module_id;...
   */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_param_data_v2_t */


/* Used by the structure that configures/calibrates various settings in
  the voice processing and voice stream session. */
typedef struct voice_set_param_v2_t voice_set_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_set_param_v2_t
@{ */
/* Payload of the VOICE_CMD_SET_PARAM_V2 command, which
    configures/calibrates settings in the voice processing and voice stream
    session. */
/** If mem_map_handle=NULL (parameter data is in-band), a series of
    voice_param_data_t structures immediately follow the mem_map_handle field.
*/
struct voice_set_param_v2_t
{
   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the parameter data payload address.
        This field is ignored if the mem_map_handle is NULL. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the parameter data payload address.
        This field is ignored if the mem_map_handle is NULL. 

        The 64-bit number formed by payload_address_lsw and
        payload_address_msw must be set 4-byte aligned, and it must be in
        contiguous memory. If the address is sent, data is in shared memory

        @subhd{For a 32-bit shared memory address} This field must be set
        to zero.

        @subhd{For a 36-bit shared memory address} Bits 31 to 4 of this
        field must be set to zero.  */

   uint32_t payload_size;
   /**< Actual size in bytes of the variable payload accompanying the message,
        or in shared memory. This field is used for parsing the parameter
        payload.

        @values > 0, and must be a multiple of 4 */

   uint32_t mem_map_handle;
   /**< Unique identifier for an address. This handle is returned by the aDSP
        through the #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.
     
        @values
        - NULL -- Parameter data payloads are within the message payload
          (in-band).
        - Non-NULL -- Parameter data payloads begin at the address specified
          in the data_payload_addr_lsw and data_payload_addr_msw fields
          (out-of-band) (see <b>Parameter data variable payload</b> below).
          The aDSP uses this handle to get the virtual address. @tablebulletend */
   /*
   voice_param_data_t param_data[...];
   Optional field if param data is in band
   */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_set_param_v2_t */

/** @ingroup voice_common_cmd_get_param
    Gets the current value of a specified parameter.
    The expected size of the parameter must match the module ID/parameter ID
    pair. Otherwise, an error is returned.

  @apr_hdr_fields
    Opcode   -- VOICE_CMD_GET_PARAM_V2 \n
    Dst_port -- Handle to the stream session

  @msg_payload{voice_get_param_v2_t}
    @tablens{weak__voice__get__param__v2__t}
    
  @par Parameter data variable payload (voice_param_data_t)
    @tablens{weak__voice__param__data__t}

  @return
    #VOICE_RSP_GET_PARAM_ACK.

  @dependencies
    A valid session must have been created.
*/
#define VOICE_CMD_GET_PARAM_V2 ( 0x0001103D )

/* Used by the structure that gets a parameter. */
typedef struct voice_get_param_v2_t voice_get_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_get_param_v2_t
@{ */
/* Payload of the VOICE_CMD_GET_PARAM_V2 command, which gets the current
    value of a parameter. */
/** If parameter data is in-band, sufficient memory to hold the parameter data
    plus the voice_param_data_t header must be present in the payload following
	the mem_map_handle field.
*/
struct voice_get_param_v2_t
{
   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the parameter data payload address.
        This field is ignored if mem_map_handle is NULL. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the parameter data payload address.
        This field is ignored if mem_map_handle is NULL.

        The 64-bit number formed by payload_address_lsw and payload_address_msw
        must be 32-byte aligned, and it must be in contiguous memory.

        If the address is sent, data is in shared memory.

        @subhd{For a 32-bit shared memory address} This field must be set
        to zero.

        @subhd{For a 36-bit shared memory address} Bits 31 to 4 of this
        field must be set to zero. */

   uint32_t module_id;
   /**< Valid ID of the module to be queried.

        @values See Section @xref{hdr:ModuleIDs} */

   uint32_t param_id;
   /**< Valid ID of the parameter to be queried.

        @values See Section @xref{hdr:ParameterIDs} */

   uint16_t param_max_size;
   /**< Maximum data size of the param_id/module_id combination. This is
        a multiple of 4 bytes. Allocated memory must be at least the size of
        this parameter plus the parameter data header size (see <b>Parameter
        data variable payload</b> below).

        @values	> 0 */

   uint16_t reserved;
   /**< Clients must set this field to zero. */

   uint32_t mem_map_handle;
   /**< Unique identifier for an address.
        This memory map handle is returned by the aDSP through the
        #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.

        @values Any
        - NULL -- Parameter data payloads in the acknowledgment are within
          the message payload (in-band).
        - Non-NULL -- The <b>Parameter data variable payloads</b> in the
          acknowledgment begin at the address specified in the
          data_payload_addr_lsw and data_payload_addr_msw fields
          (out-of-band). @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_get_param_t */

/** @ingroup voice_common_evt_get_param_ack
    Response to the #VOICE_CMD_GET_PARAM_V2 command.

  @apr_hdr_fields
    Opcode -- VOICE_RSP_GET_PARAM_ACK\n
    Dst_port -- Client handle\n
    Src_port -- Handle to the stream session

  @msg_payload{voice_get_param_ack_t}
    @tablens{weak__voice__get__param__ack__t}

  @par Parameter data variable payload (voice_param_data_t)
    @tablens{weak__voice__param__data__t}

  @description
    This event supports shared memory if an address is sent with
    #VOICE_CMD_GET_PARAM_V2. Some parameters can be sent only via shared memory 
    because of the transport layer size limitation.

  @return
    None.

  @dependencies
    A valid session must have been created.
*/
#define VOICE_RSP_GET_PARAM_ACK  ( 0x00011008 )

/* Used by the structure that responds to the Get Parameter command. */
typedef struct voice_get_param_ack_t voice_get_param_ack_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_get_param_ack_t
@{ */
/* Payload of the VOICE_RSP_GET_PARAM_ACK event, which acknowledges
    the VOICE_CMD_GET_PARAM_V2 command. */
/** In an in-band scenario, immediately following this structure is the
	parameter data variable payload, whose size depends on the size of the
	parameter. For an address/shared memory-based get parameter, this variable
	size payload is not present.
*/
struct voice_get_param_ack_t
{
   uint32_t status;
   /**< Status message (error code). For more information, refer to
        @xhyperref{Q3,[Q3]}.
   
        @values
        - APR_EOK -- Success
        - APR_EBADPARAM -- Failure @tablebulletend */
   /*
   voice_param_data_t param_data[...];
   */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_get_param_ack_t */

/*cond OEM_only */
/** @addtogroup voice_common_cmd_set_vfr_and_timing
@{ */
/** Sets the VFR mode and timing offsets in a session.

  @apr_hdr_fields
    Opcode   -- VOICE_CMD_SET_TIMING_PARAMS \n
    Dst_port -- %voice_set_timing_params_t

  @msg_payload{voice_set_timing_params_t}
    @table{weak__voice__set__timing__params__t}

  @description
    This command is used to provide timing information for a packet exchange
    to the aDSP services. All circuit-switched calls must use either the
    internal or external VFR_HARD mode. All packet-switched calls with time
    warping enabled or disabled, and other applications, must use VFR_NONE
    mode.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
    @par
    VFR mode must not be changed in the Run state.
*/
#define VOICE_CMD_SET_TIMING_PARAMS ( 0x00011018 )

/* enumerator */
/** Valid VFR mode values.
*/
typedef enum
{
   VFR_NONE = 0,       /**< VFR mode is None. */
   VFR_HARD,           /**< VFR mode is Hard and internal. */
   VFR_HARD_EXT,        /**< VFR mode is Hard and external. */
   VFR_MODE_END        /**< Maximum value; indicates the end of the valid modes. */
} voice_vfr_mode_t;

/** @} */ /* end_addtogroup voice_common_cmd_set_vfr_and_timing */

/* Payload used to provide timing information for a packet exchange
  to the aDSP services. */
typedef struct voice_set_timing_params_t voice_set_timing_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_set_timing_params_t
@{ */
/* Payload of the VOICE_CMD_SET_TIMING_PARAMS command, which sets the
    VFR mode and timing offsets in a session.
*/
struct voice_set_timing_params_t
{
   uint16_t mode;
   /**< Sets the stream VFR mode. The decoder derives timing based on its
        output length.
    
        @values
        - #VFR_NONE -- No periodic tick is used
        - #VFR_HARD -- Internal modem generates a 20 ms periodic tick
        - #VFR_HARD_EXT -- External modem generates a 20 ms periodic tick
        @tablebulletend */

   uint16_t enc_offset;
   /**< Offset from the VFR to send the encoded packet to the modem. The
        encoder delivers a packet at this offset from the hard VFR tick,

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is derived based on the latency statistics of the uplink
        path: the time taken by the packet to travel from the aDSP through the
        gateway to the modem. The voice Tx processing module derives the
        synchronization offset based on this offset. */

   uint16_t dec_req_offset;
   /**< Offset from the VFR to request the vocoder packet from the modem. The
        decoder requests a packet at this offset from the hard VFR tick.

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is derived based on the modem; by what time the packet is
        ready to be pulled from the hard VFR tick. */

   uint16_t dec_offset;
   /**< Offset from the VFR to check for an input vocoder packet and start
        decoding/generating erasure. The decoder starts decoding at this
        offset from the hard VFR tick.

        @values 0 to 20000 microseconds with 1-microsecond granularity

        The offset is derived based on the latency statistics of the downlink
        path: the time taken by the packet to travel from the modem through
        the gateway to the aDSP. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_set_timing_params_t */
/*endcond */

/*cond OEM_only */
/** @addtogroup voice_common_cmd_start_host_pcm
@{ */
/** Starts a host PCM session on a set of tap points for a specified device or
    stream session. For voice, the host PCM session is always mono.

  @apr_hdr_fields
    Opcode -- VOICE_CMD_START_HOST_PCM_V2\n
    Dst_port -- Handle to the device/stream session

  @msg_payload{voice_start_host_pcm_v2_t}
    @table{weak__voice__start__host__pcm__v2__t}

  @par Tap point payload (voice_tap_point_v2_t)
    @table{weak__voice__tap__point__v2__t}

  @description
    The command payload is of variable length, depending on the number of tap
    points configured. Each tap point to be enabled can have a unique
    configuration (client sampling rate, direction, etc.)
    @par
    A host PCM session cannot be started on a device/stream session that
    already has a host PCM session active. The client must first stop the
    active host PCM session via the #VOICE_CMD_STOP_HOST_PCM command before
    issuing another start command on the device/stream session.
    @par
    Once a host PCM session is established on a device/stream session, the
    buffer exchange is handled via the #VOICE_EVT_PUSH_HOST_BUF_V2 and
    #VOICE_EVT_HOST_BUF_AVAILABLE_V2 events. The host PCM operates seamlessly
    across handovers as a client sampling rate is specified that is
    independent of the operating rate of the tap point.
    @par
    Each buffer must be large enough to hold 20 ms at the host sampling rate.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid device/stream session must be created.
    @par
    No active host PCM session can exist on the specified device/stream
    session. @newpage
*/
/* The dynamic range of host PCM samples is expected to be the 
  same as stream PCM samples. For host PCM on device, dynamic 
  range of stream is 16-bit and for host PCM on stream, it is 14-bit. 
*/
#define VOICE_CMD_START_HOST_PCM_V2 ( 0x0001103E )

/** Host PCM session mode is Read. */
#define VOICE_HOST_PCM_READ  0x0001

/** Host PCM session mode is Write. */
#define VOICE_HOST_PCM_WRITE 0x0002

/** @} */ /* end_addtogroup voice_common_cmd_start_host_pcm */

/* Payload for specifying a tap point */
typedef struct voice_tap_point_v2_t voice_tap_point_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_tap_point_v2_t
@{ */
/* Payload of the VOICE_CMD_START_HOST_PCM_V2 command, which specifies a tap
    point.
*/
struct voice_tap_point_v2_t
{
  uint32_t tap_point;
  /**< GUID of the configured tap point.

       @values
       - #VOICEPROC_MODULE_TX
       - #VOICEPROC_MODULE_RX
       - #VOICESTREAM_MODULE_TX
       - #VOICESTREAM_MODULE_RX @tablebulletend */

  uint16_t direction;
  /**< Specifies the operating mode of the tap point.
    
       @values
       - #VOICE_HOST_PCM_READ
       - #VOICE_HOST_PCM_WRITE

       The values can be ORed to define a simultaneous read/write
       session. The direction is with respect to the client:
       - A tap point with a read session enabled delivers data to the client.
       - A tap point with write enabled injects data from the client.
       - A read + write session first reads data from the tap point to the
         client, and then injects data from the client for downstream
         processing. @tablebulletend */

  uint16_t sampling_rate;
  /**< Client sampling rate for the configured tap point. The aDSP resamples
       between the client sampling rate and the operating rate of the tap point.
    
       @values 8000, 16000, 48000 Hz */

  uint16_t duration_ms;
  /**< Duration of the buffer in milliseconds.

       Unsupported. Clients must set this field to zero. */

  uint16_t reserved;
  /**< Clients must set this field to zero. */

   uint32_t mem_map_handle;
   /**< Memory map handle returned by the aDSP for the
        #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.

        This handle defines the property of the shared memory used for HPCM
        data buffer transfer. This handle is expected to be valid for all the
        HPCM buffers until #VOICE_CMD_STOP_HOST_PCM is issued. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_tap_point_v2_t */

/* Payload for starting a host PCM session. */
typedef struct voice_start_host_pcm_v2_t voice_start_host_pcm_v2_t;


#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_start_host_pcm_v2_t
@{ */
/* Payload of the VOICE_CMD_START_HOST_PCM_V2 command, which starts a
    host PCM session.
*/
struct voice_start_host_pcm_v2_t
{

  uint32_t num_tap_points;
  /**< Number of tap points requested for the host PCM.

       This command has a variable length payload. The payload length is
       4+12*N bytes, where N=num_tap_points. */

  voice_tap_point_v2_t tap_points[ 1 ];
  /**< Dummy array of tap points (see voice_tap_point_v2_t).

       The size of this array is defined in num_tap_points. Thus, the actual
       size is %voice_tap_point_v2_t tap_points[num_tap_points]. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_start_host_pcm_v2_t */
/*endcond */

/*cond OEM_only */
/** @ingroup voice_common_evt_push_host_buffers
    Pushes buffers from the host to the firmware for a specified tap point.

  @apr_hdr_fields
    Opcode -- VOICE_EVT_PUSH_HOST_BUF_V2\n
    Dst_port -- Handle of the voice device/stream session

  @msg_payload{voice_evt_push_host_pcm_buf_v2_t}
    @table{weak__voice__evt__push__host__pcm__buf__v2__t}

  @description
    This command allows the client to push buffers in the specified direction
    to be used by the firmware in the host PCM processing timeline. In the read
    direction, the buffer specifies a physical address in shared memory to be
    filled. In the write direction, the buffer specifies a physical address in
    shared memory of the data to inject into the tap point for downstream
    processing. Currently, the host PCM operates on a default 20&nbsp;ms basis
    for each tap point.
    @par
    Read buffers must be aligned to 32-byte boundaries for cache purposes,
    while write buffers must be aligned to 4-byte boundaries. Read and write
    buffers must have been mapped previously via 
    #VOICE_CMD_SHARED_MEM_MAP_REGIONS.
    @par
    The firmware maintains independent read and write buffer queues for each
    tap point. When the firmware receives this VOICE_EVT_PUSH_HOST_BUF_V2 event,
	it attempts to push the corresponding buffer as indicated in the mask field.
    @par
    @keep{8} If there is an internal queue overflow, the client is notified
	immediately that the buffer can be reclaimed via
	#VOICE_EVT_HOST_BUF_AVAILABLE_V2. Otherwise, the buffer is pushed to the
	proper queue. Currently, the firmware supports only one buffer at a time
	on each path to ensure minimum delay.
    @par
    Pushed buffers are dequeued at the operating timeline of the specified tap
    point, and then they are processed. A notification is sent to the client
    via #VOICE_EVT_HOST_BUF_AVAILABLE_V2 so the buffers can be reclaimed by the
    client. Buffers must be consistent with the host PCM configuration and
    size/duration requirements.
    - If the buffer is consistent, it is filled in the read direction or
      consumed in the write direction.
    - If the buffer is inconsistent, data is not filled in the read
      direction, and data is dropped in the write direction.
    @par
    In either case, a notify event must still be generated with the
    corresponding error masks set.   

  @return
    None.

  @dependencies
    A valid session must have been created.
    @par
    A valid host PCM session must be started on the tap point selected to
    push buffers.
*/
#define VOICE_EVT_PUSH_HOST_BUF_V2         ( 0x0001103F )
/*endcond */

/*cond OEM_only */
/** @addtogroup voice_common_evt_host_buffers_available
@{ */
/** Indicates that the firmware has finished processing a buffer previously
    pushed via #VOICE_EVT_PUSH_HOST_BUF_V2, and the buffer can be reclaimed by
    the client.

  @apr_hdr_fields
    Opcode -- VOICE_EVT_HOST_BUF_AVAILABLE_V2\n
    Src_port -- Handle to the voice processing session

  @msg_payload{voice_evt_notify_host_pcm_buf_v2_t}
    @table{weak__voice__evt__notify__host__pcm__buf__t}

  @description
    Based on the mask field, the client can reclaim the buffers (used by the
    firmware) for its buffer pool. The memory map handle defines this
    property for the shared memory buffer, e.g., virtual/physical/local.  
    @par
    Any mask field that contains bits other than a timing tick indicates that
    a corresponding buffer has been used and can be reclaimed. The client can
    read out the corresponding fields.
    @par
    - If the read mask is set, the read-related fields represent information
      of the filled buffer at the physical address specified, and the buffer
      can be reclaimed.
    - If the write mask is set, the buffer specified at the physical address
      can be reclaimed in the client's buffer pool. The write-related fields
      represent buffer size and format information requested by firmware for
      the next write buffer pushed.
    @par
    @keep{8} If the mask field only contains the timing tick, the firmware did
    not use any buffers and there is nothing to be reclaimed by the client.
    However, if the write direction at the specified tap point is active, the
    write-related fields represent buffer size and format information
    requested for the next write buffer pushed.
    @par
    Notify events are typically sent at the same processing timeline as the
    selected tap point, so they can be used as a trigger for the client
    buffer exchange. When initially starting a host PCM session, the
    firmware does not have buffers available, and it sends a notify event
    with only the timing tick set in the mask to initiate client buffer
    transfer and to specify any applicable write buffer requirements.
    @par
    - If a pushed read buffer is inconsistent with the host PCM configuration
    and size requirements, the notify event indicates the buffer can be
    reclaimed and that zero bytes were read out. Also, the read error bit is
    set.
    - @keep{4} If a pushed write buffer is inconsistent with the host PCM
      configuration and size requirements, the notify event still indicates
      the requested configuration and size requirements for the next write
      buffer pushed. In this case, the write error bit is set.
    @par
    Notify events are not given for any queued buffer that the firmware is
    holding if either the host PCM is stopped or the device/stream session
    is destroyed successfully. In these cases, the successful response to
    either the stop host PCM command or the destroy command is to be used as
    an indication that all client buffers can be reclaimed.
    @par
    If the client attempts to push buffers to the firmware and it overflows
    the firmware's internal buffer queue, the notify event comes back
    immediately to indicate the buffer can be reclaimed.

  @return
    None.

  @dependencies
    A valid session must have been created.
*/
#define VOICE_EVT_HOST_BUF_AVAILABLE_V2    ( 0x00011040 )

/** Indicates a timing tick. */
#define VOICE_HOST_PCM_TIMETICK    0x0004

/** Indicates an error occurred during read processing. */
#define VOICE_HOST_PCM_READ_ERROR  0x0008

/** Indicates an error occurred during write processing. */
#define VOICE_HOST_PCM_WRITE_ERROR 0x0010

/** @} */ /* end_addtogroup voice_common_evt_host_buffers_available */

/* Used by the structure to send buffers into the host. */
typedef struct voice_evt_push_host_pcm_buf_v2_t voice_evt_push_host_pcm_buf_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_evt_push_host_pcm_buf_v2_t
@{ */
/* Payload of the VOICE_EVT_PUSH_HOST_BUF_V2 event, which sends or
    receives the PCM samples to or from the host PCM.
*/
struct voice_evt_push_host_pcm_buf_v2_t
{

   uint32_t tap_point;
   /**< GUID of the tap point where the buffers are to be used.

        @values
        - #VOICEPROC_MODULE_TX
        - #VOICEPROC_MODULE_RX
        - #VOICESTREAM_MODULE_TX
        - #VOICESTREAM_MODULE_RX @tablebulletend */

   uint32_t mask;
   /**< Buffer types that are available to be pushed.

        @values
        - Bit 0 -- Read buffer is available
        - Bit 1 -- Write buffer is available

        Multiple paths can be set by ORing the respective masks. How the
        firmware looks at the buffer read/write fields depends on the mask
        setting. */

   uint32_t rd_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address (physical or virtual) of
        the read buffer to be filled with data from the specified tap point.
        This address is valid if the mask field indicates that the read buffer
        is available.*/

   uint32_t rd_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address (physical or virtual) of
        the read buffer to be filled with data from the specified tap point.
        This address is valid if the mask field indicates that the read buffer
        is available.

        The 64-bit number formed by rd_buff_addr_lsw and rd_buff_addr_msw must
        be 32-byte aligned and must have been previously mapped. */

   uint32_t wr_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address (physical or virtual) of
        the write buffer that contains the data to inject at the tap point for
        downstream processing. This address is valid if the mask field
        indicates that the write buffer is available.*/

   uint32_t wr_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address (physical or virtual) of
        the write buffer that contains the data to inject at the tap point for
        downstream processing. This address is valid if the mask field
        indicates that the write buffer is available.

        The 64-bit number formed by wr_buff_addr_lsw and wr_buff_addr_msw must
        be 4-byte aligned and must have been previously mapped. */

   uint16_t rd_buff_size;
   /**< Maximum size of the read buffer to be filled. This is valid if the mask
        field indicates that the read buffer is to be filled. 

        This field should be multiple of 32. */

   uint16_t wr_buff_size;
   /**< Size of the write buffer to be consumed. This is valid if the mask
        field indicates that the write buffer is to be consumed. */

   uint16_t sampling_rate;
   /**< Sampling rate of the buffer.
   
        @values 8000, 16000, 48000 Hz
        
        This rate must match the client-configured host sampling rate. It is
        provided for informational purposes in the VOICE_EVT_PUSH_HOST_BUF_V2
        event and can be used in the calculation of duration. */

   uint16_t wr_num_chan;
   /**< Number of channels at the selected tap point for the write buffer.
        This is valid if the mask field indicates that the write buffer is to
        be consumed.

        The client can use this parameter when calculating the duration. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_evt_push_host_pcm_buf_v2_t */

/* Used by the structure to notify the host of buffers used. */
typedef struct voice_evt_notify_host_pcm_buf_v2_t voice_evt_notify_host_pcm_buf_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_evt_notify_host_pcm_buf_t
@{ */
/* Used by the VOICE_EVT_HOST_BUF_AVAILABLE_V2 event to notify the host
    of buffers filled/consumed.
*/

struct voice_evt_notify_host_pcm_buf_v2_t
{

   uint32_t tap_point;
   /**< GUID of the tap point at which the buffer is operated.

        @values
        - #VOICEPROC_MODULE_TX
        - #VOICEPROC_MODULE_RX
        - #VOICESTREAM_MODULE_TX
        - #VOICESTREAM_MODULE_RX @tablebulletend */

   uint32_t mask;
   /**< Notification mode.

        @values
        - Bit 0 -- Read buffer is filled
        - Bit 1 -- Write buffer is consumed
        - Bit 2 -- Timing tick that provides buffer sizing/duration information
        - Bit 3 -- Error occurred during read processing
        - Bit 4 -- Error occurred during write processing

        Multiple notifications can be set by ORing the respective masks. */

   uint32_t rd_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address of the read buffer that has
        been filled with data from the specified tap point.
        This address is valid if the mask field indicates that the read buffer
        is filled. */

   uint32_t rd_buff_addr_msw;
   /**< Upper bits of the shared memory address of the read buffer that has
        been filled with data from the specified tap point.
        This address is valid if the mask field indicates that the read buffer
        is filled.

        Data was resampled, if necessary, to the client-configured host
        PCM sampling rate.

        This shared memory buffer was provided by client via the
        #VOICE_EVT_PUSH_HOST_BUF_V2 command. */

   uint32_t wr_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address of the consumed write buffer.
        This address is valid if the mask field indicates that the write buffer
        is consumed. */
        
   uint32_t wr_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address of the consumed write buffer.
        This address is valid if the mask field indicates that the write buffer
        is consumed.

        Consumed data was resampled, if necessary, from the client-configured
        host PCM sampling rate to the operating rate of the tap point.

        This shared memory buffer was provided by client via the
        #VOICE_EVT_PUSH_HOST_BUF_V2 command. */

   uint16_t rd_buff_size;
   /**< Actual size of the filled read buffer.
        This is valid if the mask field indicates that the read buffer is
        filled. If the mask field has only a timing tick set, this value
        represents the required buffer size for the next push event.

        If an error occurred in processing, the size may be given as 0, and
        the buffer can be reclaimed. */

   uint16_t wr_buff_size;
   /**< Requested size of the next write buffer to be pushed.
        This is valid if the mask field indicates that the write buffer is
        consumed. 

        The client can use this field to determine buffer size requirements
        for subsequent write buffers, and to calculate the requested duration
        of data required by the firmware at the selected tap point. */

   uint16_t sampling_rate;
   /**< Sampling rate of the buffers.

        @values 8000, 16000, 48000 Hz

        This rate must match the client-configured host sampling rate. It is
        provided for informational purposes in the notification event, and it
        can be used when calculating duration.*/

   uint16_t rd_num_chan;
   /**< Actual number of channels associated with the read buffer
        (non-interleaved).

        This is valid if the mask field indicates that the read buffer is
        filled. */

   uint16_t wr_num_chan;
   /**< Number of channels at the selected tap point for the next pushed write
        buffer (non-interleaved). The client can use this when calculating
        duration. */

   uint16_t reserved;
   /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_evt_notify_host_pcm_buf_t */
/*endcond */

/*cond OEM_only */
/** @ingroup voice_common_cmd_stop_host_pcm
    Stops an active host PCM session (any active read/write host PCM sessions
    for the selected device/stream handle).

  @apr_hdr_fields
    Opcode   -- VOICE_CMD_STOP_HOST_PCM \n
    Dst_port -- Handle to the device/stream session

  @msg_payload
    None.

  @description
    If this command is not issued, but a voice session is destroyed, any active
    host PCM sessions are destroyed without explicit notice to the client.
    @par
    If this command completes successfully, all buffers pushed by the client to
    the firmware can be reclaimed by the client. In addition, if the destroy
    command is issued without first stopping the host PCM, and the destroy
    command completes successfully, all client buffers pushed can also be
    reclaimed.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid host PCM session must have been created.
 */
#define VOICE_CMD_STOP_HOST_PCM ( 0x0001100B )
/*endcond */

/*cond OEM_only */
/** @addtogroup voice_common_cmd_set_mute
@{ */
/** Allows a client to set Mute/Unmute mode on a specified paired stream/device
    session by decreasing/increaseing the gain over a specified duration.

  @note1hang CNG is not available in device sessions.
  
  @apr_hdr_fields
    Opcode -- VOICE_CMD_SET_SOFT_MUTE_V2\n
    Dst_port -- Handle to the stream/device session

  @msg_payload{voice_set_soft_mute_v2_t}
    @table{weak__voice__set__soft__mute__v2__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid device/stream session must have been created.
*/
#define VOICE_CMD_SET_SOFT_MUTE_V2 ( 0x0001103B )

/** Valid paths to mute.
*/
typedef enum
{
   VOICE_SET_MUTE_TX_ONLY = 0,    /**< Only the Tx path is muted. */
   VOICE_SET_MUTE_RX_ONLY,        /**< Only the Rx path is muted. */
   VOICE_SET_MUTE_TX_AND_RX       /**< Both Tx and Rx paths are muted. */
} voice_mute_path_t;

/** Valid values for configuring Mute/CNG mode.
*/
typedef enum
{
   VOICE_UNMUTE = 0,              /**< Unmute is in effect. */
   VOICE_MUTE                     /**< Mute is in effect. */
} voice_mute_status_t;

/** @} */ /* end_addtogroup voice_common_cmd_set_mute */

/* Used by the structure that mutes Tx, Rx, or both in a paired-device/stream
  session. */
typedef struct voice_set_soft_mute_v2_t voice_set_soft_mute_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_set_soft_mute_v2_t
@{ */
/* Payload of the VOICE_CMD_SET_SOFT_MUTE_V2 command, which sets Mute
    mode in a specified paired stream/device session.
*/
struct voice_set_soft_mute_v2_t
{
   uint16_t direction;
   /**< Specifies the path on which to set Mute mode.

        @values
        - 0 -- #VOICE_SET_MUTE_TX_ONLY
        - 1 -- #VOICE_SET_MUTE_RX_ONLY
        - 2 -- #VOICE_SET_MUTE_TX_AND_RX @tablebulletend */

   uint16_t mute;
   /**< Enables or disables muting.

        @values
        - 0 -- #VOICE_UNMUTE
        - 1 -- #VOICE_MUTE
        
        If #VOICE_MODULE_ENC_CNG is enabled on the encoder path, comfort noise
        is generated instead of muting. */

   uint16_t ramp_duration;
   /**< Specifies the duration over which the volume is muted or unmuted. For
        an immediate effect of mute/unmute without ramping, set this parameter
        to zero.

        @values 0 to 5000 ms */

   uint16_t reserved;
   /**< Clients set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_set_soft_mute_v2_t */
/*endcond */

/** @addtogroup voice_common_cmd_map_mem_region
@{ */
/** Maps multiple shared memory regions. All regions must come from the same
    memory pool.

  @apr_hdr_fields
      Opcode -- VOICE_CMD_SHARED_MEM_MAP_REGIONS \n
    Dst_port -- Handle to the stream session

  @msg_payload{voice_cmd_shared_mem_map_regions_t}
    @tablens{weak__voice__cmd__shared__mem__map__regions__t}

  @par Map payload (voice_shared_map_region_payload_t)
    @table{weak__voice__shared__map__region__payload__t}

    @return
    #VOICE_RSP_SHARED_MEM_MAP_REGIONS.

  @dependencies
    None.
*/
#define VOICE_CMD_SHARED_MEM_MAP_REGIONS  (0x00011041)

/** Enumeration for the SHMEM8_4K pool ID. */
#define VOICE_MAP_MEMORY_SHMEM8_4K_POOL 3

/* enumerator */
/** Valid enumeration values for property_flag field of 
    the #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.
*/
typedef enum
{
   VOICE_PHYSICAL_SHARED_MEM_ADDR =0,
   /**< Shared memory address provided in
        voice_shared_map_region_payload_t is a physical address. */

   VOICE_VIRTUAL_SHARED_MEM_ADDR,
   /**< Shared memory address provided in
        voice_shared_map_region_payload_tis a virtual address. */

} voice_mem_map_property_flag_t;
/** @} */ /* end_addtogroup voice_common_cmd_map_mem_region */

/* VOICE memory map regions structure  */
typedef struct  voice_cmd_shared_mem_map_regions_t voice_cmd_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_cmd_shared_mem_map_regions_t
@{ */
/* Payload of the VOICE_CMD_SHARED_MEM_MAP_REGIONS, which allows clients to
    map multiple shared memory regions in a single command. */
/** Following this structure are num_regions of
    voice_shared_map_region_payload_t.
*/
struct voice_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         @values
         - #VOICE_MAP_MEMORY_SHMEM8_4K_POOL
         - Other values are reserved
         
         The memory ID implicitly defines the characteristics of the memory.
         Characteristics might include alignment type, permissions, etc.

         SHMEM8_4K is shared memory, byte addressable, and 4 KB aligned. */
    
    uint16_t                  num_regions;
    /**< Number of regions to map.*/

    uint32_t                  property_flag;
    /**< Configures one common property for all the regions in the payload. 
         Only one memory region can be mapped per command. No two regions in
         the same #VOICE_CMD_SHARED_MEM_MAP_REGIONS command can have a
         different property.
    
         @values 0x00000000 to 0x00000001 (see #voice_mem_map_property_flag_t)
         - 0 (#VOICE_PHYSICAL_SHARED_MEM_ADDR) -- The shared memory must be
           mapped (hardware TLB entry), and a software entry must be added for
           internal bookkeeping.
         - 1 (#VOICE_VIRTUAL_SHARED_MEM_ADDR) -- The shared memory must not be
           mapped (because the hardware TLB entry is already available), but a
           software entry must be added for internal bookkeeping. This can be
           useful if two services within the aDSP are communicating via APR.
           They can directly communicate via the virtual address instead of a
           physical address. The virtual regions must be contiguous.
         @tablebulletend*/

     /* Following this structure are num_regions of
        voice_shared_map_region_payload_t */
} 
#include "adsp_end_pack.h"
;
/** @} */ /* weakgroup voice_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/* VOICE memory map regions payload structure  */
typedef struct voice_shared_map_region_payload_t voice_shared_map_region_payload_t;

/** @weakgroup weak_voice_shared_map_region_payload_t
@{ */
/* Payload of the map region used by the voice_cmd_shared_mem_map_regions_t
    structure.
*/
struct voice_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map.

         This address must be contiguous memory, and it must be 4 KB aligned. */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw must
         be 4 KB aligned, and it must be contiguous memory.

         @subhd{For a 32-bit shared memory address} This field must be set
         to zero.

         @subhd{For a 36-bit shared memory address} Bits 31 to 4 of this
         field must be set to zero. */
         
    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_shared_map_region_payload_t */

/** @ingroup voice_common_resp_mem_map
    Returns the memory map handle in response to a
    #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.

  @apr_hdr_fields
      Opcode -- VOICE_RSP_SHARED_MEM_MAP_REGIONS \n
    Dst_port -- Ignored

  @msg_payload{voice_rsp_shared_mem_map_regions_t}
    @table{weak__voice__rsp__shared__mem__map__regions__t}

  @return
    None.

  @dependencies
    A #VOICE_CMD_SHARED_MEM_MAP_REGIONS command must have been issued.
*/
#define VOICE_RSP_SHARED_MEM_MAP_REGIONS (0x00011042)

/* VOICE memory map regions response structure  */
typedef struct  voice_rsp_shared_mem_map_regions_t voice_rsp_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_rsp_shared_mem_map_regions_t
@{ */
/* Memory map command response payload used by the
    VOICE_RSP_SHARED_MEM_MAP_REGIONS command. This structure is an AVS
    response to the memory maping regions command. mem_map_handle can later be
    used by the client to unmap regions that are previously mapped.
*/
struct voice_rsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address.
         This memory map handle is returned by the aDSP through the
         #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.

         This handle can be used as an abstract representation of the shared
         memory regions that are being mapped. The aDSP uses this handle to
         retrieve the shared memory attributes. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_rsp_shared_mem_map_regions_t */

/** @ingroup voice_common_cmd_map_unmem_region
    Unmaps multiple shared memory regions that were previously mapped via
    #VOICE_CMD_SHARED_MEM_MAP_REGIONS.

  @apr_hdr_fields
      Opcode -- VOICE_CMD_SHARED_MEM_UNMAP_REGIONS \n
    Dst_port -- Handle to the stream session

  @msg_payload{voice_cmd_shared_mem_unmap_regions_t}
    @table{weak__voice__cmd__shared__mem__unmap__regions__t}

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A corresponding #VOICE_CMD_SHARED_MEM_MAP_REGIONS command must have been
    sent before this unmap command is sent.
*/
#define VOICE_CMD_SHARED_MEM_UNMAP_REGIONS (0x00011043)

/* VOICE memory unmap regions command structure */
typedef struct voice_cmd_shared_mem_unmap_regions_t voice_cmd_shared_mem_unmap_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_cmd_shared_mem_unmap_regions_t
@{ */
/* Memory unmap regions command payload used by the
    VOICE_CMD_SHARED_MEM_UNMAP_REGIONS command.
    This structure allows clients to unmap multiple shared memory regions in
    a single command. 
*/
struct voice_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Memory map handle returned by the #VOICE_CMD_SHARED_MEM_MAP_REGIONS
	     command. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* @end_weakgroup weak_voice_cmd_shared_mem_unmap_regions_t */

/** Command to send calibration data using the compressed data format representation in ACDB */
#define VOICE_CMD_SET_PARAM_V3 (0x00011069)

/** Payload for VOICE_CMD_SET_PARAM_V3 */
typedef struct voice_cmd_set_param_v3_t voice_cmd_set_param_v3_t;

#include "adsp_begin_pack.h"
struct voice_cmd_set_param_v3_t
{
    uint32_t cal_handle;
    /**< Handle to use to query CVD calibration utility for calibration entries */
}
#include "adsp_end_pack.h"
;

/* Used by the structure that contains voice parameter data. */
typedef struct voice_voc_param_data_t  voice_voc_param_data_t ;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_voc_param_data_t
@{ */
/* Payload of the voice param data used by elite_msg_custom_set_voc_param_type. */
/** Immediately following this structure are param_size bytes of parameter
    data.
*/
struct voice_voc_param_data_t
{
   uint32_t media_id;
    /**< Unique ID of the parameter. */
   uint32_t param_id;
    /**< Unique ID of the parameter. */
   uint32_t direction_id;
    /**< Unique ID denoting the direction. */
   uint32_t param_size;
    /**< Size of the parameter in bytes. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_voice_voc_param_data_t*/




#endif
