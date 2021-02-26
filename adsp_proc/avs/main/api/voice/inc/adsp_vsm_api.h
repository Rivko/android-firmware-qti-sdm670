#ifndef __ADSP_VSM_API_H__
#define __ADSP_VSM_API_H__

/**
  @file adsp_vsm_api.h
  @brief This file contains API definitions for the Voice Stream Manager (VSM).
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

  Copyright (c) 2010, 2012-2013, 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/13   sw      (Tech Pubs) Updated Doxygen for 2.0 internal draft.
02/27/12   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/14/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
===========================================================================*/

#include "mmdefs.h"
#include "adsp_vcmn_api.h"

/** @ingroup vsm_version
    Version ID of the aDSP VSM module. */
#define ADSP_VSM_VERSION    0x00070004


/*============================================================================
   VSM 8x60 ISOD commands
============================================================================*/

/** @addtogroup vsm_cmd_create_stream_session
@{ */
/** Creates a new paired Tx/Rx voice call stream.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_CREATE_SESSION \n
    Src_port -- Handle to the session (on a return acknowledgment)

  @msg_payload{vsm_create_session_t}
    @table{weak__vsm__create__session__t}

  @detdesc
    A voice stream session is a paired Tx/Rx session with postprocessing
    blocks. If the session type is set to TX_ONLY, the Rx module is not
    created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    - Includes the handle to the paired Tx and Rx
    - Is NULL if there is an error
    @par
    A handle to the session is implicitly sent via the src_port of the APR
    header in the acknowledgment.

  @dependencies
    This is the first command to use before starting a voice stream session.
  @newpage
*/
#define VSM_CMD_CREATE_SESSION ( 0x0001100C )

/* enumerator */
/** Valid session paths that can be created.
*/
typedef enum
{
    TX_ONLY = 0,              /**< Only a Tx path is to be created. */
    RX_ONLY,                  /**< Only an Rx path is to be created. */
    TX_AND_RX                 /**< Both Tx and Rx paths are to be created. */
} vsm_create_session_path_t;

/** @} */ /* end_addtogroup vsm_cmd_create_stream_session */

/* Used by the structure that creates a paired Tx/Rx stream of the voice call. */
typedef struct vsm_create_session_t vsm_create_session_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_create_session_t
@{ */
/* Payload of the VSM_CMD_CREATE_SESSION command, which creates a
    paired Tx/Rx stream of the voice call.
*/
struct vsm_create_session_t
{
   uint32_t mode;
   /**< Specifies which Tx or Rx sessions to create.

        @values
        - #TX_ONLY
        - #RX_ONLY
        - #TX_AND_RX @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_create_session_t */

/** @addtogroup vsm_cmd_set_vocoder_media_type
@{ */
/** Sets the vocoder media type on both Tx and Rx streams in a paired session.
    The operating media type of each session determines the vocoder used by the
    encoder and decoder paths.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_MEDIA_TYPE \n
    Dst_port -- Handle to the stream session

  @msg_payload(vsm_set_media_type_t)
    @table{weak__vsm__set__media__type__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
    @par
    This command is declined when it is sent in the Run state.
*/
#define VSM_CMD_SET_MEDIA_TYPE ( 0x0001100D )

/* Used by the structure that sets the vocoder media type on both Tx and Rx
    streams in a paired session. */
typedef struct vsm_set_media_type_t vsm_set_media_type_t;

/* enumerator */
/** Valid media types.
*/
typedef enum
{
   VSM_MEDIA_TYPE_NONE         = 0x00010FC0,  /**< Media type is None. */
   VSM_MEDIA_TYPE_13K_MODEM    = 0x00010FC1,  /**< Media type is 13K modem
                                                   (SO17/QCELP). */
   /** @cond OEM_only */
   VSM_MEDIA_TYPE_EVRC_MODEM   = 0x00010FC2,  /**< Media type is EVRC modem
                                                   (SO3). */
   /** @endcond */

   VSM_MEDIA_TYPE_4GV_NB_MODEM = 0x00010FC3,  /**< Media type is 4GV narrowband
                                                   modem (SO68). */
   VSM_MEDIA_TYPE_4GV_WB_MODEM = 0x00010FC4,  /**< Media type is 4GV wideband
                                                   modem (SO70). */
   VSM_MEDIA_TYPE_4GV_NW_MODEM = 0x00010FC5,  /**< Media type is 4GV
                                                   narrowband-wideband (SO73)
                                                   modem for circuit-switched. */
   /** @cond OEM_only */
   VSM_MEDIA_TYPE_AMR_NB_MODEM = 0x00010FC6,  /**< Media type is AMR narrowband
                                                   modem. */
   VSM_MEDIA_TYPE_AMR_WB_MODEM = 0x00010FC7,  /**< Media type is AMR wideband
                                                   modem. */
   /** @endcond */

   VSM_MEDIA_TYPE_EFR_MODEM    = 0x00010FC8,  /**< Media type is EFR modem. */
   VSM_MEDIA_TYPE_FR_MODEM     = 0x00010FC9,  /**< Media type is FR modem. */
   VSM_MEDIA_TYPE_HR_MODEM     = 0x00010FCA,  /**< Media type is HR modem. */

   /** @cond OEM_only */
   VSM_MEDIA_TYPE_PCM_8_KHZ    = 0x00010FCB,  /**< Media type is linear PCM
                                                   vocoder modem format; mono,
                                                   16 bits, little endian,
                                                   8 kHz. */
   VSM_MEDIA_TYPE_PCM_16_KHZ   = 0x00010FCC,  /**< Media type is linear PCM
                                                   vocoder modem format; mono,
                                                   16 bits, little endian,
                                                   16 kHz. */
   VSM_MEDIA_TYPE_G711_ALAW    = 0x00010FCD,  /**< Media type is G711 A-law. */
   VSM_MEDIA_TYPE_G711_MLAW    = 0x00010FCE,  /**< Media type is G711 mu-law. */
   VSM_MEDIA_TYPE_G711_ALAW_V2 = 0x000131A1,  /**< Media type is G711 A-law v2. */
   VSM_MEDIA_TYPE_G711_MLAW_V2 = 0x000131A2,  /**< Media type is G711 mu-law v2. */
   VSM_MEDIA_TYPE_G729AB       = 0x00010FD0,  /**< Media type is G729AB. */
   VSM_MEDIA_TYPE_G722         = 0x00010FD1,  /**< Media type is G722. */
   VSM_MEDIA_TYPE_AMR_NB_IF2   = 0x00010FD2,  /**< Media type is AMR
                                                   narrowband IF2. */
   /** @endcond */

   VSM_MEDIA_TYPE_4GV_NW       = 0x00010FD3,  /**< Media type is 4GV
                                                   narrowband-wideband (SO73)
                                                   for packet-switched. */
   /** @cond OEM_only */
   VSM_MEDIA_TYPE_EAMR         = 0x00010FD4,  /**< Media type is EAMR
                                                   wideband codec. */
   /** @endcond */

   VSM_MEDIA_TYPE_PCM_44_1_KHZ = 0x00010FD5,
   /**< Media type is linear PCM vocoder modem format; mono, 16 bits, little
        endian, 44.1 kHz. \n
        Stream pre/postprocessing blocks run at 48 kHz. */

   VSM_MEDIA_TYPE_PCM_48_KHZ   = 0x00010FD6,
   /**< Media type is linear PCM vocoder modem format; mono, 16 bits, little
        endian, 48 kHz. */

   /* Opcodes 0x00010FD5 and 0x00010FD6 are reserved for PCM_44.1 and PCM_48K
   media type for 8960 branch */

   VSM_MEDIA_TYPE_EVRC_NW_2K   = 0x00010FD7,
   /**< Media type is EVRC_NW_2K narrowband-wideband 2K (SO77) codec. */

   VSM_MEDIA_TYPE_EVS          = 0x00010FD8,
   /**< Media type is EVS vocoder. */

   VSM_MEDIA_TYPE_PCM_32_KHZ   = 0x00010FD9,
   /**< Media type is linear PCM vocoder modem format; mono, 16 bits, little
        endian, 32 kHz. */
   VSM_MEDIA_TYPE_DTMF         = 0x00013295
} vsm_media_type_t;

#include "adsp_begin_pack.h"

/** @} */ /* end_addtogroup vsm_cmd_set_vocoder_media_type */

/** @weakgroup weak_vsm_set_media_type_t
@{ */
/* Payload of the VSM_CMD_SET_MEDIA_TYPE command, which sets the
    vocoder media type on both Tx and Rx streams in a paired session.
*/
struct vsm_set_media_type_t
{
   uint32_t media_type;
   /**< Specifies the vocoder media type.

        @values All media types defined in #vsm_media_type_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_media_type_t */

/** @ingroup vsm_cmd_reconfigure_session
    Reconfigures an existing stream session with new parameters.

    A voice stream session is a paired Tx/Rx session. A session type change is
    permitted from the original session type.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_REINIT_SESSION \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_reinit_session_t}
    @table{weak__vsm__reinit__session__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
    @par
    If the parameters of the existing session change, this command is used
    to reinitialize the session. The session is expected to be stopped
    before this command is issued.
*/
#define VSM_CMD_REINIT_SESSION ( 0x0001100E )

/* Used by the structure that reconfigures an existing stream session
  with new parameters. */
typedef struct vsm_reinit_session_t vsm_reinit_session_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_reinit_session_t
@{ */
/* Payload of the VSM_CMD_REINIT_SESSION command, which reconfigures
    an existing stream session with new parameters.
*/
struct vsm_reinit_session_t
{
   uint16_t mode;
   /**< Sets the stream session mode.

        @values
        - #TX_ONLY
        - #RX_ONLY
        - #TX_AND_RX @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_reinit_session_t */

/** @ingroup vsm_cmd_start_session
    Starts the Rx and Tx paths of a new stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_START_SESSION \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
    @par
    A valid media type must be supplied.
*/
#define VSM_CMD_START_SESSION ( 0x0001100F )

/** @ingroup vsm_cmd_stop_session
    Stops the Rx and Tx paths of an existing stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_STOP_SESSION \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_STOP_SESSION ( 0x00011010 )

/** @ingroup vsm_cmd_destroy_session
    Destroys the Rx and Tx paths of a stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_DESTROY_SESSION \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
*/
#define VSM_CMD_DESTROY_SESSION ( 0x00011011 )

/** @ingroup vsm_cmd_route_stream
    Routes a stream to the appropriate voice processors by attaching a set of
    voice Tx and Rx processors to the voice streams.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_ATTACH_VOICE_PROC \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_routing_config_t}
    @table{weak__vsm__routing__config__t}

  @detdesc
    The Tx handles connect the encoder to the voice processors on the Tx path.
	The Rx handles connect the decoder to the voice processors on the Rx path.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    Valid sessions must have been created.
*/
#define VSM_CMD_ATTACH_VOICE_PROC ( 0x00011013 )

/** @ingroup vsm_cmd_detach_stream
    Detaches a set of voice Tx and Rx processors from the voice streams.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_DETACH_VOICE_PROC \n
    Dst_port -- Handle to the stream session

  @msg_payload
    This command uses the same APR message payload as
    #VSM_CMD_ATTACH_VOICE_PROC.

  @detdesc
    The Tx handles disconnect the encoder from the voice processors on the Tx
	path.
    The Rx handles disconnect the decoder from the voice processors Rx path.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created and attached using
    #VSM_CMD_ATTACH_VOICE_PROC.
*/
#define VSM_CMD_DETACH_VOICE_PROC ( 0x00011014 )

/* Used by the structure that routes the stream to the proper voice processors. */
typedef struct vsm_routing_config_t vsm_routing_config_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_routing_config_t
@{ */
/* Payload of the VSM_CMD_ATTACH_VOICE_PROC and
    VSM_CMD_DETACH_VOICE_PROC commands, which route the stream to the
    appropriate voice processors.
*/
struct vsm_routing_config_t
{
   uint16_t tx_device_count;
   /**< Number of Tx handles. */

   uint16_t rx_device_count;
   /**< Number of Rx handles. */

   uint16_t handles[1];
   /**< Array of Tx and Rx handles.

        This array has a tx_device_count + rx_device_count length, with the
        tx_device_count number of entries being the Tx handles followed by the
        Rx handles. Thus, the actual format is:

   @verbatim
   uint16_t tx_handles[tx_device_count];
   uint16_t rx_handles[rx_device_count]; @endverbatim */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_routing_config_t */

/** @ingroup vsm_evt_oob_send_pkt_to_client
    Indicates that the VSM has written a packet to the shared memory for the
    client to consume.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_OOB_ENC_BUF_READY \n
    Src_port -- Handle of the voice stream session that is sending the packet

  @msg_payload
    None

  @par Data format (vsm_oob_pkt_exchange_header_t)
    @tablens{weak__vsm__oob__pkt__exchange__header__t}

  @description
    The encoder service uses this event to periodically send a packet to the
    client with time intervals specified by the vocoder. All vocoders used in
    circuit-switched networks operate at 20 ms, whereas vocoders in
	packet-switched networks have varying periodicities of 10 ms, 20 ms, and
	30 ms.
    @par
    The encoder service sends the packet across only if the packet is
    available for the current frame. If no packet is available, the Encoder
    service does not send anything, and the client is expected to handle
    it appropriately.

  @return
    None. @newpage

  @dependencies
    A valid session must have been created, and it must be in the Run state.
	@par
	The Packet Exchange mode must have been configured for Out-of-band mode of
	transfer.
	@par
	The encoder service sends the packet across only if it has received the
	#VSM_EVT_OOB_ENC_BUF_CONSUMED event for the previous packet that was sent.
*/
#define VSM_EVT_OOB_ENC_BUF_READY ( 0x0001136C )

/** @ingroup vsm_evt_oob_pkt_consumed_by_client
    Indicates to the VSM that the client has consumed the packet from
    shared memory, and the service can now write the next packet.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_OOB_ENC_BUF_CONSUMED \n
    Dst_port -- Handle of the voice stream session whose packet was consumed

  @msg_payload
    None

  @description
    The encoder service uses this event to ensure that the packet it sent has
    been consumed by the client, and it can now write a new packet at the
	shared memory location.
	@par
    All vocoders used in circuit-switched networks operate at 20 ms, whereas
	vocoders in packet-switched networks have varying periodicities of
	10 ms, 20 ms, and 30 ms.
    @par
    The encoder service sends a packet across only if it has received this
	event for the previous packet that was sent via the
	#VSM_EVT_OOB_ENC_BUF_READY event. If this event has not arrived in
	acknowledgement of the previously sent packet, the encoder service does not
	send anything, and the client is expected to handle it appropriately.

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
	@par
    The Packet Exchange mode must have been configured for the Out-of-band mode
	of transfer.
*/
#define VSM_EVT_OOB_ENC_BUF_CONSUMED ( 0x0001136D )

/** @ingroup vsm_evt_send_enc_packet
    Sends an encoded packet from the encoder service on the aDSP to the client.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_SEND_ENC_PACKET \n
    Src_port -- Handle to the stream session

  @msg_payload{vsm_send_enc_packet_t}
    @table{weak__vsm__send__enc__packet__t}

  @detdesc
    The encoder service uses this event to periodically send a packet to the
    modem with time intervals dictated by the vocoder. All vocoders used in
    circuit-switched networks operate at 20 ms, whereas vocoders in
	packet-switched networks have varying periodicities of 10 ms, 20 ms, and
	30 ms.
    @par
    The encoder service sends the packet across only if the packet is
    available for the current frame. If no packet is available, the encoder
    service does not send anything, and the client/modem is expected to handle
    it appropriately.

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
    @par
    The packet exchange mode must have been configured for the In-band mode of
    transfer.
*/
#define VSM_EVT_SEND_ENC_PACKET ( 0x00011015 )

/* Used by the structure that sends an encoded packet from the encoder
  service on the aDSP to the modem. */
typedef struct vsm_send_enc_packet_t vsm_send_enc_packet_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_send_enc_packet_t
@{ */
/* Payload of the VSM_EVT_SEND_ENC_PACKET event, which sends an
    encoded packet from the encoder service on the aDSP to the client.
*/
struct vsm_send_enc_packet_t
{
    uint32_t media_type;
    /**< Specifies the media type of the vocoder packet.

         @values All media types defined in #vsm_media_type_t,
                 except VSM_MEDIA_TYPE_NONE, which is not supported.

         Following this media_type field is the variable-length vocoder packet
         payload. The total size (including the 4-byte media_id) is specified
         in the APR header. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_send_enc_packet_t */


/* Used by the structure that sends an encoded packet from the encoder
  service on the aDSP to the modem. */
typedef struct vsm_send_enc_packet_v2_t vsm_send_enc_packet_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_send_enc_packet_v2_t
@{ */
/* Payload of the VSM_EVT_SEND_ENC_PACKET event, which sends an
    encoded packet from the encoder service on the aDSP to the client.
*/
struct vsm_send_enc_packet_v2_t
{
    uint32_t media_type;
    /**< Specifies the media type of the vocoder packet.

         @values All media types defined in #vsm_media_type_t,
                 except VSM_MEDIA_TYPE_NONE, which is not supported.

         Following this media_type field is the variable-length vocoder packet
         payload. The total size (including the 4-byte media_id) is specified
         in the APR header. */

    uint64_t timestamp_us;
      /**< time stamp of the mic samples used for AV Sync feature.
         This is propagated by voice processing chain from Audio Front End */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup vsm_send_enc_packet_v2_t */


/** @ingroup vsm_evt_send_dec_packet
    Sends a packet from the modem to the decoder service on the aDSP.
    This command is sent by the modem in response to a decoder packet request
    event raised by the aDSP via #VSM_EVT_REQ_DEC_PACKET.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_SEND_DEC_PACKET \n
    Dst_port -- Handle to the stream session \n
    TokenID  -- A requirement to send the same TokenID as given by the aDSP in
                an event to request a packet. This maintains synchronization
                between the packet request and send packet events. The decoder
                service uses this TokenID for error correction and debugging
                purposes.

  @msg_payload{vsm_send_dec_packet_t}
    @table{weak__vsm__send__dec__packet__t}

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
    @par
    The packet exchange mode must have been configured for the In-band mode of
    transfer.
    @par
    This command must arrive at the aDSP within the time alloted by the
    decoder service based on gateway latency and other factors.
*/
#define VSM_EVT_SEND_DEC_PACKET ( 0x00011016 )

/* Used by the structure that sends a packet from the modem to the
  decoder service on the aDSP. */
typedef struct vsm_send_dec_packet_t vsm_send_dec_packet_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_send_dec_packet_t
@{ */
/* Payload of the VSM_EVT_SEND_DEC_PACKET event, which sends a packet
    from the modem to the decoder service on the aDSP.
*/
struct vsm_send_dec_packet_t
{
    uint32_t media_type;
    /**< Specifies the media type of the packet to be decoded.

         @values All media types defined in #vsm_media_type_t,
                 except VSM_MEDIA_TYPE_NONE, which is not supported.

         Following this media_type field is the variable-length payload, i.e.
         the packet to be decoded. The total size (including the 4-byte
         media_id) is specified in the APR header. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_send_dec_packet_t */

/** @ingroup vsm_evt_request_packet_from_modem
    Used by the decoder service on the aDSP to request a packet from the modem.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_REQ_DEC_PACKET \n
    Src_port -- Handle to the voice stream session

  @msg_payload
    None.

  @detdesc
@if OEM_only
    Packets are requested at time intervals dictated by the vocoder.
    All vocoders in a CS network request a packet every 20 ms, whereas vocoders
    in a PS network can vary, based on whether time warping is enabled (see
    #VSM_CMD_SET_TIMEWARP). In return, the decoder service expects
    #VSM_EVT_SEND_DEC_PACKET.
@endif
@if ISV_only
    Packets are requested at time intervals dictated by the vocoder.
    All vocoders in a CS network request a packet every 20 ms, whereas vocoders
    in a PS network can vary. In return, the decoder service expects
    #VSM_EVT_SEND_DEC_PACKET.
@endif

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
	@par
    The packet exchange mode must have been configured for in-band mode of transfer.
*/
#define VSM_EVT_REQ_DEC_PACKET ( 0x00011017 )

/** @ingroup vsm_evt_oob_request_pkt_from_client
    Used by the decoder service on the aDSP to request a packet from the client.

  @apr_hdr_fields
    Opcode -- VSM_EVT_OOB_DEC_BUF_REQUEST\n
    Src_port -- Handle of the voice stream session that is requesting the
                packet

  @msg_payload
    None.

  @detdesc
Packets are requested at time intervals specified by the vocoder. All
    vocoders in circuit-switched networks request a packet every 20 ms, whereas
    vocoders in packet-switched networks can vary, based on whether time
	warping is enabled (see #VSM_CMD_SET_TIMEWARP).
    In return, the decoder service expects #VSM_EVT_OOB_DEC_BUF_READY.

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
*/
#define VSM_EVT_OOB_DEC_BUF_REQUEST ( 0x0001136E )

/** @ingroup vsm_evt_oob_pkt_available
    This event is sent to the decoder service on the aDSP to indicate that
    the client has written a packet to the shared memory location.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_OOB_DEC_BUF_READY \n
    Dst_port -- Handle of the voice stream session that is reading the packet

  @msg_payload
    None

  @detdesc
This event is sent by the client in response to a decoder packet request
    event raised by the aDSP via #VSM_EVT_OOB_DEC_BUF_REQUEST.
    The packet contents are written to a shared memory location previously
	configured by the #VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG command.

  @return
    None.

  @dependencies
    A valid session must have been created, and it must be in the Run state.
    @par
    The packet exchange mode must have been configured for the Out-of-band mode
	of transfer.
*/
#define VSM_EVT_OOB_DEC_BUF_READY ( 0x0001136F )

/** @addtogroup vsm_cmd_set_frame_rates
@{ */
/** Sets minimum and maximum frame rates on the encoder. This command applies
    only to CDMA vocoders (QCELP-13K, EVRC, EVRC-B, EVRC-WB, EVRC-NW).

  @apr_hdr_fields
    Opcode   -- VSM_CMD_ENC_SET_MINMAX_RATE \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_enc_set_minmax_rate_t}
    @table{weak__vsm__enc__set__minmax__rate__t}

  @keep{8} @detdesc
    This command tells the encoder to encode with these rates as the minimum
    and maximum frame rates, respectively. The Multimode Voice Services (MVS)
    on the modem receives both the minimum rate and maximum rate at the same
    time from the modem, and the MVS sends both to the aDSP at the same time.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_ENC_SET_MINMAX_RATE ( 0x00011019 )

/* enumerator */
/** Valid encoder frame rates.
*/
typedef enum
{
   ENC_BLANK_RATE,       /**< Encoder rate is blank. */
   ENC_EIGHTH_RATE,      /**< Encoder rate is 1/8. */
   ENC_QUARTER_RATE,     /**< Encoder rate is 1/4. */
   ENC_HALF_RATE,        /**< Encoder rate is 1/2. */
   ENC_FULL_RATE         /**< Encoder rate is full rate. */
} vsm_enc_rate_t;

/** @} */ /* end_addtogroup vsm_cmd_set_frame_rates */

/* Used by the structure that sets the minimum rate and maximum rate
  on the encoder. */
typedef struct vsm_enc_set_minmax_rate_t vsm_enc_set_minmax_rate_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_enc_set_minmax_rate_t
@{ */
/* Payload of the VSM_CMD_ENC_SET_MINMAX_RATE command, which sets the
    minimum and maximum frame rates on the encoder.
*/
struct vsm_enc_set_minmax_rate_t
{
   uint32_t media_type;
   /**< Encoder media type for which to set the rate. This field is used for
        error checking and debugging.

        @values
        - #VSM_MEDIA_TYPE_13K_MODEM
        - #VSM_MEDIA_TYPE_EVRC_MODEM
        - #VSM_MEDIA_TYPE_4GV_NB_MODEM
        - #VSM_MEDIA_TYPE_4GV_WB_MODEM
        - #VSM_MEDIA_TYPE_4GV_NW_MODEM
        - #VSM_MEDIA_TYPE_4GV_NW
        - #VSM_MEDIA_TYPE_EVRC_NW_2K

        See also #vsm_media_type_t. */

   uint32_t min_rate;
   /**< Minimum rate to encode. This is applicable to 1X vocoders only.

        @values
        - #ENC_BLANK_RATE
        - #ENC_EIGHTH_RATE
        - #ENC_QUARTER_RATE
        - #ENC_HALF_RATE
        - #ENC_FULL_RATE @tablebulletend */

   uint32_t max_rate;
   /**< Maximum rate to encode. This is applicable to 1X vocoders only.

        @values
        - #ENC_BLANK_RATE
        - #ENC_EIGHTH_RATE
        - #ENC_QUARTER_RATE
        - #ENC_HALF_RATE
        - #ENC_FULL_RATE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_enc_set_minmax_rate_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_cmd_set_encoding_rates
    Sets the encoding rate on an encoder. This command applies to all vocoders
    that support different rates/modes.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_ENC_SET_RATE \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_enc_set_rate_t}
    @table{weak__vsm__enc__set__rate__t}

  @keep{6} @detdesc
    For QCELP-13K, the #VSM_CMD_ENC_SET_MINMAX_RATE takes precedence over
    this command.
    @par
    Consecutive start commands are not supported. To change the record
    parameters, the recording must be stopped and then restarted with new
    parameters.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_ENC_SET_RATE ( 0x0001101B )

/* Used by the structure that sets the rate on an encoder. */
typedef struct vsm_enc_set_rate_t vsm_enc_set_rate_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_enc_set_rate_t
@{ */
/* Payload of the VSM_CMD_ENC_SET_RATE command, which sets the rate on
    an encoder. The command is applicable to all vocoders that support
    different rates/modes.
*/
struct vsm_enc_set_rate_t
{
   uint32_t media_type;
   /**< Encoder media type for which to set the rate. This field is used for
        error checking and debugging.

        @values
        - #VSM_MEDIA_TYPE_13K_MODEM
        - #VSM_MEDIA_TYPE_EVRC_MODEM
        - #VSM_MEDIA_TYPE_4GV_NB_MODEM
        - #VSM_MEDIA_TYPE_4GV_WB_MODEM
        - #VSM_MEDIA_TYPE_4GV_NW_MODEM
        - #VSM_MEDIA_TYPE_AMR_NB_MODEM
        - #VSM_MEDIA_TYPE_AMR_WB_MODEM
        - #VSM_MEDIA_TYPE_AMR_NB_IF2
        - #VSM_MEDIA_TYPE_4GV_NW
        - #VSM_MEDIA_TYPE_EVRC_NW_2K

        See also #vsm_media_type_t. */

   uint32_t encoder_rate;
   /**< Specifies the encoder rate. The same command is used to serve as a
        reduced rate level for QCELP-13K, Capacity Operating Point (COP) for
        4GV-NB and 4GV-WB, and rate/mode for AMR-NB and AMR-WB.

        Supported values for QCELP-13K:
        - 0 -- 14.4 kbps
        - 1 -- 12.2 kbps
        - 2 -- 11.2 kbps
        - 3 -- 9.0 kbps
        - 4 -- 7.2 kbps

        @contcell
        Supported values for 4GV-NB:
        - 0 -- 9.3 kbps
        - 1 -- 8.5 kbps
        - 2 -- 7.5 kbps
        - 3 -- 7.0 kbps
        - 4 -- 6.6 kbps
        - 5 -- 6.2 kbps
        - 6 -- 5.8 kbps
        - 7 -- 4.8 kbps

        Supported values for 4GV-WB:
        - 0 -- 8.5 kbps
        - 4 -- 9.3 kbps
        - 7 -- 4.8 kbps

        Supported values for 4GV-NW:
        - 0 -- 8.5 kbps
        - 1 -- 9.3 kbps
        - 2 -- 7.5 kbps
        - 3 -- 7.0 kbps
        - 4 -- 6.6 kbps
        - 5 -- 6.2 kbps
        - 6 -- 5.8 kbps
        - 7 -- 4.8 kbps

        Supported values for AMR-NB and EAMR:
        - 0 -- 4.75 kbps
        - 1 -- 5.15 kbps
        - 2 -- 5.9 kbps
        - 3 -- 6.7 kbps
        - 4 -- 7.4 kbps
        - 5 -- 7.95 kbps
        - 6 -- 10.2 kbps
        - 7 -- 12.2 kbps

        Supported values for AMR-WB:
        - 0 -- 6.6 kbps
        - 1 -- 8.85 kbps
        - 2 -- 12.65 kbps
        - 3 -- 14.25 kbps
        - 4 -- 15.85 kbps
        - 5 -- 18.25 kbps
        - 6 -- 19.85 kbps
        - 7 -- 23.05 kbps
        - 8 -- 23.85 kbps

        @contcell
        Supported values for EVRC-NW-2K:
        - 0 -- 8.5 kbps
        - 1 -- 9.3 kbps
        - 2 -- 7.5 kbps
        - 3 -- 2.4 kbps
        - 4 -- 6.6 kbps
        - 5 -- 6.2 kbps
        - 6 -- 5.8 kbps
        - 7 -- 4.8 kbps @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_enc_set_rate_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_cmd_set_rate_modulation
    Sets the rate modulation parameter on the encoder. This command applies
    only to QCELP-13K and EVRC vocoders.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_ENC_SET_RATE_MODE \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_enc_set_rate_mod_t}
    @table{weak__vsm__enc__set__rate__mod__t}

  @detdesc
    For QCELP-13K, the #VSM_CMD_ENC_SET_MINMAX_RATE and #VSM_CMD_ENC_SET_RATE
    commands take priority over this command.
    @par
    For EVRC, the VSM_CMD_ENC_SET_MINMAX_RATE command takes priority over
    this command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_ENC_SET_RATE_MOD ( 0x0001101C )

/* Used by the structure that sets the rate modulation parameter on the encoder. */
typedef struct vsm_enc_set_rate_mod_t vsm_enc_set_rate_mod_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_enc_set_rate_mod_t
@{ */
/* Payload of the VSM_CMD_ENC_SET_RATE_MOD command, which sets the
    rate modulation parameter on the encoder. This command is applicable for
    QCELP-13K and EVRC vocoders only.
*/
struct vsm_enc_set_rate_mod_t
{
   uint32_t rate_modulation_param;
   /**< Encoder rate modulation command. The maximum average rate of the
        vocoder is limited to (2X + 1) / (2 * (X+1)).

        Supported values for bit 0:
        - 0 -- Disable
        - 1 -- Enable

        Supported values for bit 1:
        - 0 -- X = 1/S
        - 1 -- X = S

        Supported values for bits 9 to 2:
        - S (8-bit unsigned integer) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_enc_set_rate_mod_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_cmd_set_dtx_mode
    Used to set the DTX mode on the encoder. This command applies to all GSM
	vocoders, 4GV codecs, and VoIP codecs.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_ENC_SET_DTX_MODE \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_enc__set__dtx__mode_t}
    @table{weak__vsm__enc__set__dtx__mode__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_ENC_SET_DTX_MODE ( 0x0001101D )

/* Used by the structure that sets the DTX mode on the encoder. */
typedef struct vsm_enc_set_dtx_mode_t vsm_enc_set_dtx_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_enc_set_dtx_mode_t
@{ */
/* Payload of the VSM_CMD_ENC_SET_DTX_MODE command, whish sets the
    DTX mode on the encoder. This command is applicable for all GSM vocoders,
    4GV codecs, and VoIP codecs.
*/
struct vsm_enc_set_dtx_mode_t
{
   uint32_t dtx_mode;
   /**< Specifies whether DTX mode is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_enc_set_dtx_mode_t */
/*endcond */

/*cond OEM_only */
/** @addtogroup vsm_cmd_set_tty_mode
@{ */
/** Sets the Teletype (TTY) mode.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_TTY_MODE \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_set_tty_mode_t}
    @table{weak__vsm__set__tty__mode__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
*/
#define VSM_CMD_SET_TTY_MODE      ( 0x00011020 )

/* enumerator */
/** Valid TTY modes.
*/
typedef enum
{
    VSM_TTY_MODE_OFF = 0,    /**< TTY mode is Off. */
    VSM_TTY_MODE_HCO,        /**< TTY mode is Hearing Carry Over. */
    VSM_TTY_MODE_VCO,        /**< TTY mode is Voice Carry Over. */
    VSM_TTY_MODE_FULL        /**< TTY mode is Full. */
} vsm_tty_mode_t;

/* Used by the structure that sets the TTY mode. */
typedef struct vsm_set_tty_mode_t vsm_set_tty_mode_t;

#include "adsp_begin_pack.h"

/** @} */ /* end_addtogroup vsm_cmd_set_tty_mode */

/** @weakgroup weak_vsm_set_tty_mode_t
@{ */
/* Payload of the VSM_CMD_SET_TTY_MODE command, which sets the
    TTY mode.
*/
struct vsm_set_tty_mode_t
{
   uint32_t mode;
   /**< Specifies the TTY mode of operation.

        @values
        - #VSM_TTY_MODE_OFF
        - #VSM_TTY_MODE_HCO
        - #VSM_TTY_MODE_VCO
        - #VSM_TTY_MODE_FULL @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_tty_mode_t */
/*endcond */

/*cond OEM_only */

/** @addtogroup vsm_cmd_outofband_set_tty_mode
@{ */
/** Sets the Teletype (TTY) mode for Out Of Band method of character exchange.

  @apr_hdr_fields
    Opcode -- VSM_CMD_OUTOFBAND_SET_TTY_MODE\n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_set_outofband_tty_mode_t}
    @table{weak__vsm__outofband__set__tty__mode__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
*/
#define VSM_CMD_OUTOFBAND_SET_TTY_MODE      ( 0x00011061 )

/* Used by the structure that sets the TTY mode. */
typedef struct vsm_outofband_set_tty_mode_t vsm_outofband_set_tty_mode_t;

#include "adsp_begin_pack.h"

/** @} */ /* end_addtogroup vsm_cmd_outofband_set_tty_mode */

/** @weakgroup weak_vsm_cmd_outofband_set_tty_mode
@{ */
/* Payload of the VSM_CMD_OUTOFBAND_SET_TTY_MODE command, which sets the
    TTY mode.
*/
struct vsm_outofband_set_tty_mode_t
{
   uint32_t mode;
   /**< Specifies the TTY mode of operation.

        @values
        - #VSM_TTY_MODE_OFF
        - #VSM_TTY_MODE_HCO
        - #VSM_TTY_MODE_VCO
        - #VSM_TTY_MODE_FULL @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_outofband_set_tty_mode_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_outofband_tty_tx_char_detected
    Indicates that the character is detected on the Tx path for Out Of Band TTY.

  @apr_hdr_fields
    Opcode -- VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED\n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_evt_outofband_tty_tx_char_t}
    @table{weak__vsm__evt__outofband__tty__tx__char__detected__t}

  @description
    This event is sent by the stream to the client when a tty character is detected in the tx path.
    Once the tty character is detected, encoder input will be replaced with silence, so as to prevent
    inband transmission of the tty tone.
      @par
    This event is  generated during Out Of Band TTY processing.
    Therefore, the client must enable the Out Of Band TTY processing for receiving this event.

  @return
    None.

  @dependencies
    A valid session must have been created, and should be in Run state.
      @par
    An Out Of Band set tty command must be issued before this event can be
    generated.
*/
#define VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED  ( 0x00012E78 )

/* Used by the structure that notifies the client that a TTY character is detected */
typedef struct vsm_evt_outofband_tty_tx_char_detected_t vsm_evt_outofband_tty_tx_char_detected_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_evt_outofband_tty_tx_char_detected_t
@{ */
/* Payload of the VSM_EVT_OUTOFBAND_TTY_TX_CHAR_DETECTED event, which notifies
    the client that the tty character is detected on uplink.
*/
struct vsm_evt_outofband_tty_tx_char_detected_t
{
   uint16_t tty_char;
   /**< character detected on Tx in UTF-8 representation in int16 format

        @values #character from baudot set in UTF-8 representation */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_evt_outofband_tty_tx_char_t */

/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_outofband_tty_rx_char_push
    Indicates that a new character needs to be rendered on the Rx path for Out Of Band TTY.

  @apr_hdr_fields
    Opcode -- VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH\n
    Dst_port -- Handle to the stream

  @msg_payload{vsm_evt_outofband_tty_rx_char_push_t}
    @table{weak__vsm__evt__outofband__tty__rx__char_push__t}

  @description
    This event is sent by the client to the stream.
    The client can start pushing Rx characters once the session is set to run state.
      @par
    This event is  generated by client during Out Of Band TTY processing.
    Therefore, the client must enable the Out Of Band TTY processing before generating this event.

  @return
    None.

  @dependencies
    A valid session must have been created, and should be in Run state.
      @par
    An Out Of Band set tty command must be issued before this event can be
    serviced.
*/
#define VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH  ( 0x00012E79 )

/* Used by the structure that notifies the stream that a TTY character needs to be rendered */
typedef struct vsm_evt_outofband_tty_rx_char_push_t vsm_evt_outofband_tty_rx_char_push_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_evt_outofband_tty_rx_char_push_t
@{ */
/* Payload of the VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH event, which notifies
    the stream that the tty character is to be rendered on downlink.
*/
struct vsm_evt_outofband_tty_rx_char_push_t
{
   uint16_t tty_char;
   /**< character to be redendered on Rx in UTF-8 representation in int16 format

        @values #character from baudot set in UTF-8 representation */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_evt_outofband_tty_rx_char_push_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_outofband_tty_rx_char_done
    Indicates that the character render request is accepted on the Rx path for Out Of Band TTY.

  @apr_hdr_fields
    Opcode -- VSM_EVT_OUTOFBAND_TTY_RX_CHAR_ACCEPTED\n
    Dst_port -- Handle to the stream

  @msg_payload{None}

  @description
    This event is sent by the stream to the client. After the Rx character modulation is accepted,
    the downlink speech will be replaced with corresponding TTY tone.
      @par
    This event is  generated by stream in response to the VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH event during Out Of Band TTY processing.

  @return
    None.

  @dependencies
    A valid session must have been created, and should be in Run state.
      @par
    A VSM_EVT_OUTOFBAND_TTY_RX_CHAR_PUSH event must be issued by the client before this event can be
    generated, so that, this event will be generated in response with the same client token.
*/
#define VSM_EVT_OUTOFBAND_TTY_RX_CHAR_ACCEPTED  ( 0x0001105e )
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_outofband_tty_error
    Indicates that the recent character render request cannot be accepted on the Rx path for Out Of Band TTY.

  @apr_hdr_fields
    Opcode -- VSM_EVT_OUTOFBAND_TTY_ERROR\n
    Dst_port -- Handle to the stream

  @msg_payload{None}

  @description
    This event is sent by the stream to the client.
      @par
    This event is  generated, when the recent character render request cannot be accepted on the Rx path for Out Of Band TTY.

  @return
    None.

  @dependencies
    A valid session must have been created, and should be in Run state.
      @par
    A VSM_EVT_OUTOFBAND_TTY_RX_CHAR event must be issued by the client before this event can be
    generated in response with same client token.
*/
#define VSM_EVT_OUTOFBAND_TTY_ERROR  ( 0x0001105d )
/*endcond */
/*cond OEM_only */
/** @ingroup vsm_cmd_resync_ctm_demod
    Signals resynchronization to the Cellular Text Modem (CTM) demodulator.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_RESYNC_CTM \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_RESYNC_CTM      ( 0x00011021 )
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_cmd_stop_record
    Stops the current recording session by disconnecting the voice stream from
    the AFE ports being used, and stopping the delivery of samples.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_STOP_RECORD \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A voice stream must have been created.
    @par
    A record session must be running.

  @sa
    #VSM_CMD_START_RECORD_V2
*/
#define VSM_CMD_STOP_RECORD (0x00011029)

/** @ingroup vsm_cmd_stop_playback
    Stops in-call audio delivery (playback) on a specified stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_STOP_PLAYBACK \n
    Dst_port -- Handle to the stream session

  @msg_payload
    None.

  @detdesc
    This command destroys the topology required for in-call audio delivery, and
    it disconnects from the AFE port being used for audio samples.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A voice stream must have been created.
    @par
    A playback session must be running.

  @sa
    #VSM_CMD_START_PLAYBACK_V2
*/
#define VSM_CMD_STOP_PLAYBACK (0x00011031)
/*endcond */

/*cond OEM_only */
/** @addtogroup vsm_cmd_set_dtmf
@{ */
/** Starts and stops DTMF generation.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_DTMF_GENERATION \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_set_dtmf_generation_t}
    @table{weak__vsm__set__dtmf__generation__t}

  @detdesc
    This command is sent to the aDSP to start and stop an in-band DTMF tone.
    A new DTMF generation command replaces a previously unfinished command in
    the same direction. To generate a single tone, set the tone1 and tone2
    fields to the same value.
    @par
    @keep{6} If a stream is to be closed before the requested tone duration
    has elapsed, the client must first stop the currently playing tone to
    ensure proper ramp down and avoid audio artifacts/pops. The completion of
    DTMF generation due to a Stop command or because the requested duration
    has elapsed is indicated to the client via the
    #VSM_EVT_DTMF_GENERATION_ENDED event.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A voice stream must have been created.
*/
#define VSM_CMD_SET_DTMF_GENERATION ( 0x00011023 )

/** Valid paths for DTMF generation. Currently, only Tx is supported.
*/
typedef enum
{
   VSM_SET_DTMF_GEN_TX = 0,    /**< DTMF generation path is Tx. */
}vsm_set_dtmf_gen_path_t;

/** Valid values for specifying whether DTMF generation is mixed with voice.
*/
typedef enum
{
   VSM_DTMF_GEN_MIX_WITH_VOICE_DISABLE = 0,
   /**< Disables DTMF generation mixing. This replaces the speech with DTMF. */

   VSM_DTMF_GEN_MIX_WITH_VOICE_ENABLE
   /**< Enables DTMF generation mixing. */

}vsm_dtmf_gen_mixing_t;

/** @} */ /* end_addtogroup vsm_cmd_set_dtmf */

/* Used by the structure that starts and stops the DTMF generation. */
typedef struct vsm_set_dtmf_generation_t vsm_set_dtmf_generation_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_dtmf_generation_t
@{ */
/* Payload of the VSM_CMD_SET_DTMF_GENERATION command, which starts
    and stops the DTMF generation.
*/
struct vsm_set_dtmf_generation_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone is generated. Currently,
        only Tx is supported.

        @values #VSM_SET_DTMF_GEN_TX */

   uint16_t mixing;
   /**< Specifies whether mixing DTMF with speech is disabled.

        @values
        - #VSM_DTMF_GEN_MIX_WITH_VOICE_DISABLE
        - #VSM_DTMF_GEN_MIX_WITH_VOICE_ENABLE @tablebulletend */

   uint16_t tone1;
   /**< Frequency of tone 1 for DTMF.

        @values 100 to 4000 Hz */

   uint16_t tone2;
   /**< Frequency of tone 2 for DTMF.

        @values 100 to 4000 Hz */

   uint16_t gain;
   /**< DTMF tone linear gain. Because the level of tone generation is fixed
        at 0 dBFS, this parameter must be set to a negative value.

        @values Negative value in Q13 format */

   int16_t duration;
   /**< Duration of the tone with a 5 ms resolution. The duration includes
        ramp-up and ramp-down periods of 1 ms and 2 ms, respectively.

        @values
        - -1 -- Infinite duration
        -  0 -- Stop infinite tone
        - >0 -- Finite duration @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_dtmf_generation_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_dtmf_ended
    Indicates that the generation of DTMF tones has ended because the client
    issued a Stop command, or the duration of the tone has elapsed.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_DTMF_GENERATION_ENDED \n
    Src_port -- Handle to the stream session

  @msg_payload{vsm_evt_dtmf_generation_ended_t}
    @table{weak__vsm__evt__dtmf__generation__ended__t}

  @detdesc
    This event is sent by the stream to the client that enabled DTMF
    generation.
    @par
    This event is not generated during DTMF generation if the call is ended.
    Therefore, the client must issue the Stop command before the call ends so
    the event can be received.

  @return
    None.

  @dependencies
    A valid session must have been created.
    @par
    A DTMF generation command must be issued before this event can be
    generated.
*/
#define VSM_EVT_DTMF_GENERATION_ENDED ( 0x00011032 )

/* Used by the structure that notifies the client that generation of tones
  has ended */
typedef struct vsm_evt_dtmf_generation_ended_t vsm_evt_dtmf_generation_ended_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_evt_dtmf_generation_ended_t
@{ */
/* Payload of the VSM_EVT_DTMF_GENERATION_ENDED event, which notifies
    the client that generation of tones has ended.
*/
struct vsm_evt_dtmf_generation_ended_t
{
   uint16_t direction;
   /**< Specifies the path on which the DTMF tone was generated. Currently,
        only Tx is supported.

        @values #VSM_SET_DTMF_GEN_TX */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_evt_dtmf_generation_ended_t */
/*endcond */

/*cond OEM_only */
/** @addtogroup vsm_cmd_set_rx_dtmf
@{ */
/** Configures DTMF detection in the Rx direction.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_RX_DTMF_DETECTION \n
    Src_port -- Handle of the client that enabled DTMF detection \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_set_rx_dtmf_detection_t}
    @table{weak__vsm__set__rx__dtmf__detection__t}

  @detdesc
    The DTMF status is sent to the client that requested detection via
    #VSM_EVT_RX_DTMF_DETECTED. Only one client can request Rx DTMF detection
    on a given stream at a time. If another client requests Rx DTMF detection
    while the previous client's Rx DTMF detection is still active, the request
    fails.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_SET_RX_DTMF_DETECTION  ( 0x00011027 )

/** Valid values for configuring Rx DTMF detection.
*/
typedef enum
{
   VSM_RX_DTMF_DETECTION_DISABLE = 0, /**< DTMF detection is disabled. */
   VSM_RX_DTMF_DETECTION_ENABLE       /**< DTMF detection is enabled. */
}vsm_set_rx_dtmf_detection_mode_t;

/** @} */ /* end_addtogroup vsm_cmd_set_rx_dtmf */

/* Used by the structure that allows the client to enable or disable
  DTMF detection. */
typedef struct vsm_set_rx_dtmf_detection_t vsm_set_rx_dtmf_detection_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_rx_dtmf_detection_t
@{ */
/* Payload of the VSM_CMD_SET_RX_DTMF_DETECTION command, which allows
    the client to enable or disable DTMF detection in the Rx direction.
*/
struct vsm_set_rx_dtmf_detection_t
{
   uint32_t enable;
   /**< Specifies whether DTMF detection is disabled on the Rx path.

        @values
        - #VSM_RX_DTMF_DETECTION_DISABLE
        - #VSM_RX_DTMF_DETECTION_ENABLE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_rx_dtmf_detection_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_evt_rx_dtmf_detected
    Sends a DTMF detection tone status to the client that enabled DTMF
    detection.

  @apr_hdr_fields
    Opcode   -- VSM_EVT_RX_DTMF_DETECTED \n
    Dst_port -- Handle of the client that enabled DTMF detection

  @msg_payload{vsm_rx_dtmf_detected_t}
    @table{weak__vsm__rx__dtmf__detected__t}

  @detdesc
    This event is sent whenever DTMF has been detected in the Rx path.
    @par
    The DTMF detection feature can only be used to detect DTMF frequencies as
    listed in ITU-T Recommendation Q.23, Section 6 (refer to
    @xhyperref{S1,[S1]}) and not to arbitrary frequencies. Therefore the
    possible values for the low and high freqency parameters are as listed in
    the APR message payload table.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and Rx DTMF detection must be enabled.
*/
#define VSM_EVT_RX_DTMF_DETECTED ( 0x0001101A )

/* Used by the structure that sends a DTMF detection tone status to the
  client that enabled it. */
typedef struct vsm_rx_dtmf_detected_t vsm_rx_dtmf_detected_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_rx_dtmf_detected_t
@{ */
/* Payload of the VSM_EVT_RX_DTMF_DETECTED command, which sends a
    DTMF detection tone status to the client that enabled DTMF detection.
*/
struct vsm_rx_dtmf_detected_t
{
   uint16_t low_freq;
   /**< Low frequency detected in the DTMF tone.

        @values 697, 770, 852, 941 Hz */

   uint16_t high_freq;
   /**< High frequency detected in the DTMF tone.

        @values 1209, 1336, 1477, 1633 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_rx_dtmf_detected_t */
/*endcond */

/*cond OEM_only */
/** @addtogroup vsm_cmd_start_record_session
@{ */
/** Starts a recording session on a specified stream by writing PCM samples
    from the Rx and Tx paths to either a real Rx port given in the command
    payload or to the default (preassigned) Tx/Rx voice pseudoports.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_START_RECORD_V2 \n
    Dst_port -- Session to start recording

  @msg_payload{vsm_start_record_v2_t}
    @table{weak__vsm__start__record__v2__t}

  @detdesc
    This command offers additional options:
    - Specify the port to which samples are to be sent
    - Specify how to supply these samples to the port
    @par
    When Tx and Rx path samples are mixed, the output is mono with equal weight
    from the Tx and Rx paths to the AFE. If aud_ref_port in the command payload
    is set to 0xFFFF (invalid port), the Rx and Tx path PCM samples are
    sent to the default AFE pseudoports (AFE_PORT_ID_VOICE_RECORD_RX and
    AFE_PORT_ID_VOICE_RECORD_TX, respectively). These samples are processed,
    mixed, and encoded, if necessary, on the audio path. (For information on
    AFE, refer to @xhyperref{Q6,[Q6]}.)
    @par
    To refrain from recording a particular path, the tap point must be
    specified as #VSM_TAP_POINT_NONE.
    @par
    If this command is issued when recording is enabled in a specified
    session, it is rejected with an ADSP_EALREADY message (for more information,
    refer to @xhyperref{Q5,[Q5]}).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A voice stream must have been created.
    @par
    Pseudoports/Rx port corresponding to paths to be recorded must have been
    started.

  @sa
    #VSM_CMD_STOP_RECORD
*/
#define VSM_CMD_START_RECORD_V2				(0x00011038)

/** Indicates no tapping for a specified Tx/Rx path. */
#define VSM_TAP_POINT_NONE   (0x00010F78)

/**  Indicates that a specified path is to be tapped at the end of the stream. */
#define VSM_TAP_POINT_STREAM_END (0x00010F79)

/** Indicates recording for both Tx and Rx paths in Stereo mode: Tx in the left
    channel, and Rx in the right channel.
*/
#define VSM_RECORD_TX_RX_STEREO				(0x00010F7A)

/** Indicates recording of mixed Tx and Rx paths. The output is mono with equal
    weight from the Tx and Rx paths to the AFE.
*/
#define VSM_RECORD_TX_RX_MIXING				(0x00010F7B)

/** @} */ /* end_addtogroup vsm_cmd_start_record_session */

/* Used by the structure that starts voice recording. */
typedef struct vsm_start_record_v2_t vsm_start_record_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_start_record_v2_t
@{ */
/* Payload of the VSM_CMD_START_RECORD_V2 command, which starts a
    voice recording session.
*/
struct vsm_start_record_v2_t
{
   uint32_t rx_tap_point;
   /**< Tap point to use on the Rx path.

        @values
        - #VSM_TAP_POINT_NONE
        - #VSM_TAP_POINT_STREAM_END @tablebulletend */

   uint32_t tx_tap_point;
   /**< Tap point to use on the Tx path.

        @values
        - #VSM_TAP_POINT_NONE
        - #VSM_TAP_POINT_STREAM_END @tablebulletend */

   uint32_t record_mode;
   /**< Recording mode that can be selected by the user.

        @values
         - #VSM_RECORD_TX_RX_STEREO
         - #VSM_RECORD_TX_RX_MIXING

         This field is ignored under the following conditions:
         - The aud_ref_port in the command payload is set to 0xFFFF (invalid
           port). In this case, the PCM samples are sent from the Rx path to
           AFE_PORT_ID_VOICE_RECORD_RX, and from the Tx path to
           AFE_PORT_ID_VOICE_RECORD_TX. These samples are processed, mixed, and
           encoded, if necessary, on the audio path. (For information on AFE,
           refer to @xhyperref{Q6,[Q6]}.)
         - Both Tx and Rx, or any of the tap points, are set to
           #VSM_TAP_POINT_NONE. @tablebulletend */

   uint16_t aud_ref_port;
   /**< Port ID expected by the AFE for an sending the decoder and encoder
        samples.

        @values Refer to @xhyperref{Q6,[Q6]}.

        If this field is set to 0xFFFF, samples are routed to preassigned
        pseudoports: AFE_PORT_ID_VOICE_RECORD_TX for the Tx path, and
        AFE_PORT_ID_VOICE_RECORD_RX for the Rx path. */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_start_record_v2_t */
/*endcond */

/*cond OEM_only */
/** @ingroup vsm_cmd_start_playback_session
    Starts in-call audio delivery (playback) on a specified stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_START_PLAYBACK_V2 \n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_start_playback_t}
    @table{weak__vsm__start__playback__t}

  @detdesc
    This command creates the topology required for in-call audio delivery,
    connects to the external aud_ref_port or preassigned pseudoport for this
    use case, and starts reading samples from it. These samples are mixed with
    voice and sent over the air. The AFE_PORT_ID_VOICE_PLAYBACK_TX pseudoport
    is used. (For information on AFE, refer to @xhyperref{Q6,[Q6]}.)
    @par
    The audio samples are read from the external audio reference port or from
    the default pseudoport (AFE_PORT_ID_VOICE_PLAYBACK_TX) based on the command
    payload. If the aud_ref_port is 0xFFFF (invalid AFE port ID), the audio
    samples are received through the default pseudoport. Subsequent
    VSM_CMD_START_PLAYBACK_V2 commands that are made without issuing
    #VSM_CMD_STOP_PLAYBACK commands are rejected by the firmware for a given
    session.
    @par
    The client must make sure the audio samples from the audio path (in the
    case of a pseudoport), and from the external processor (in the case of
    aud_ref_port), are of 14-bit dynamic range. For an audio path, this can be
    done through the audio path calibration; for external audio, it can be
    calibrated through AFE port gain in the AFE_PORT_CMD_START command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @keep{8} @dependencies
    Voice streams must have been created.
    @par
    The AFE port being used for audio samples must have been started.

  @sa
    #VSM_CMD_STOP_PLAYBACK
*/
#define VSM_CMD_START_PLAYBACK_V2 (0x00011039)

/* Used by the structure that starts in call music delivery. */
typedef struct vsm_start_playback_t vsm_start_playback_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_start_playback_t
@{ */
/* Payload of the VSM_CMD_START_PLAYBACK_V2 command, which starts an
    in-call music playback session.
*/
struct vsm_start_playback_t
{
   uint16_t aud_ref_port;
   /**< ID of the AFE port from which the voice client receives audio samples.

        @values Refer to @xhyperref{Q6,[Q6]}.

        A value of 0xFFFF indicates an invalid port. In this case, the audio
        samples are read from a preassigned pseudoport. */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_start_playback_t */
/*endcond */

/** @addtogroup vsm_cmd_set_pkt_exchg_mode
@{ */
/** Sets the vocoder Packet Exchange mode on the stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_PKT_EXCHANGE_MODE \n
    Dst_port -- Handle to the stream session

  @msg_payload {vsm_set_pkt_exchange_mode_t}
    @table{weak__vsm__set__pkt__exchange__mode__t}

  @detdesc
    Upon receipt of this command, the Packet Exchange mode is set based on the
	value specified in the payload.
    @par
    For the vocoder packet exchange mode selection:
    - The default mode of packet exchange is #IN_BAND.
    - The mode can be changed only when the stream session is in the Stop state.
    - If the Packet Exchange mode is being configured for #OUT_OF_BAND,
	  shared memory configuration parameters must be provided via the
	  #VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG command (while the stream is in the
      the Stop state). If the configuration information is not provided, packet
      exchanges do not occur.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]})

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
	@par
    If this command is not received, the default mode of packet exchange,
	IN_BAND, is used.
*/
#define VSM_CMD_SET_PKT_EXCHANGE_MODE  (0x00011052)

/** Vocoder packet exchange is set to In-band mode. (Default) */
#define IN_BAND				(0x00000000)

/** Vocoder packet exchange is set to Out-of-band mode. */
#define OUT_OF_BAND         (0x00000001)

/** @} */ /* end_addtogroup vsm_cmd_set_pkt_exchg_mode */

typedef struct vsm_set_pkt_exchange_mode_t vsm_set_pkt_exchange_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_pkt_exchange_mode_t
@{ */
/* Payload of the VSM_CMD_SET_PKT_EXCHANGE_MODE command, which sets the mode
    of packet transfer for the encoder and decoder.
*/
struct vsm_set_pkt_exchange_mode_t
{
   uint32_t pkt_exchange_mode;
   /**< Sets the mode of packet exchange.

        @values
        - 0 -- #IN_BAND
        - 1 -- #OUT_OF_BAND @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_pkt_exchange_mode_t */

/** @ingroup vsm_cmd_set_stream_pp_sample_rate
    Sets the sampling rate for the stream pre/postprocessing blocks.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_STREAM_PP_SAMP_RATE \n
    Dst_port -- Handle to the stream session

  @msg_payload {vsm_set_samp_rate_t}
    @table{weak__vsm__set__samp__rate__t}

  @detdesc
    On the Rx side, all of the stream pre/postprocessing downstream of the
    decoder operates at a rate set in the rx_samp_rate field in this
    command.
    @par
    On the Tx side, all of the stream pre/postprocessing upstream of the
    encoder operates at a rate set in the tx_samp_rate field in this
    command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A session must have been created before issuing the command, and the
    session must be in the Stop state.
    @par
    The #VSM_CMD_SET_MEDIA_TYPE command must have been received before this
    command (because this command is applicable only for those vocoders that
    allow multiple sampling rates).
*/
#define VSM_CMD_SET_STREAM_PP_SAMP_RATE (0x00011045)

/* Used by the structure that sets sampling rate for the stream pp. */
typedef struct vsm_set_samp_rate_t vsm_set_samp_rate_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_samp_rate_t
@{ */
/* Payload of the VSM_CMD_SET_STREAM_PP_SAMP_RATE command, which sets the sampling rate
for the stream pp blocks and the encoder
*/
struct vsm_set_samp_rate_t
{
   uint32_t rx_samp_rate;
   /**< Sets the sampling rate for pre/postprocessing on the Rx path.

        @values  8000, 16000, 48000 Hz */

   uint32_t tx_samp_rate;
   /**< Sets the sampling rate for pre/postprocessing on the Tx path.

        @values  8000, 16000, 48000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_samp_rate_t */

/** @ingroup vsm_cmd_set_oob_pkt_exchg_config
    Configures the VSM for out-of-band Packet Exchange mode.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG \n
    Dst_port -- Handle of the voice stream session set for packet transfer

  @msg_payload{vsm_config_packet_exchange_t}
    @table{weak__vsm__config__packet__exchange__t}

  @par Payload header (vsm_oob_pkt_exchange_header_t)
    @tablens{weak__vsm__oob__pkt__exchange__header__t}

  @detdesc
    This command allows the client to configure the firmware for the maximum
    encoder and decoder packet sizes, and the addresses where the encoder and
    decoder packets will be written to and read from.
    @par
    On the Rx path, the buffer specifies a physical address in shared memory
    from where the decoder reads its packet.
    @par
    On the Tx path, the buffer specifies a physical address in shared memory
    where the encoder writes its packet.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created, and it must be in the Stop state.
    @par
	The #VSM_CMD_SET_PKT_EXCHANGE_MODE command must have come with #OUT_OF_BAND
	configuration.
    @par
	@keep{8} The #VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG command must come before
	the Run command.
	@par
    If the VSM_CMD_SET_PKT_EXCHANGE_MODE has set the service for #OUT_OF_BAND
	mode of packet transfer, and the Run command is issued before
	#VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG, the session does not switch to the
	Run state, and the Packet Exchange mode within the encoder service is reset
	to #IN_BAND.
*/
#define VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG (0x00011047)

/* Used by the structure to specify the addresses for packet exchange via shared memory. */
typedef struct vsm_config_packet_exchange_t vsm_config_packet_exchange_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_config_packet_exchange_t
@{ */
/* Payload of the VSM_CMD_SET_OOB_PKT_EXCHANGE_CONFIG event, which sets the
    addresses for out of band packet exchange.
*/
struct vsm_config_packet_exchange_t
{

   uint32_t mem_handle;
   /**< Pointer to the unique identifier for an address (physical/virtual).
        This memory map handle is returned by the aDSP through the
        #VOICE_CMD_SHARED_MEM_MAP_REGIONS command.

        @values Any 32-bit value

        A non-zero memory map handle implies the message is in the payload
        address memory (out-of-band data).

        The aDSP internally uses this handle to retrieve and use the shared
        memory regions associated with the handle. */

   uint32_t enc_buf_addr_lsw;
   /**< Lower 32 bits of the address where the encoded packet is written.
        This field is ignored if the mem_handle is NULL. */

   uint32_t enc_buf_addr_msw;
   /**< Upper 32 bits of the address where the encoded packet is written.
        This field is ignored if the mem_handle is NULL.

        The 64-bit number formed by enc_buf_addr_lsw and enc_buf_addr_msw must
        be 32-byte aligned. If the address is sent, data is in shared memory.

        @subhd{For a 32-bit shared memory address} This field must be set
        to zero.

        @subhd{For a 36-bit shared memory address} Bits 31 to 4 of this
        field must be set to zero. */

   uint32_t enc_buf_size;
   /**< Number of bytes in memory allocated for the maximum allowed packet size
        that the encoder packet transfer.

        @values Must be a multiple of 32 bytes, and must accomodate the
                largest possible vocoder packet size (refer to
                @xhyperref{Q8,[Q8]}). */
   /* Q8 = Vocoder Media Types Application Note (80-N8828-1) */

   uint32_t dec_buf_addr_lsw;
   /**< Lower 32 bits of the address from where the decoder reads the encoded
        packet. This field is ignored if the mem_handle is NULL. */

   uint32_t dec_buf_addr_msw;
   /**< Upper 32 bits of the address from where the decoder reads the encoded
        packet. This field is ignored if the mem_handle is NULL.

        The 64-bit number formed by enc_buf_addr_lsw and enc_buf_addr_msw must
        be 4-byte aligned. If the address is sent, data is in shared memory.

        @subhd{For a 32-bit shared memory address} This field must be set
        to zero.

        @subhd{For a 36-bit shared memory address} Bits 31 to 4 of this
        field must be set to zero. */


   uint32_t dec_buf_size;
   /**< Number of bytes in memory allocated for the maximum allowed packet size
        that the decoder can transfer.

        @values Must be a multiple of 4 bytes, and must accomodate the
                largest possible vocoder packet size (for sizes, refer to
                @xhyperref{Q8,[Q8]}.) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_config_packet_exchange_t */

/* Used by the structure to specify the header for packet exchange via shared memory. */
typedef struct vsm_oob_pkt_exchange_header_t vsm_oob_pkt_exchange_header_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_oob_pkt_exchange_header_t
@{ */
/* Header of the OOB packet exchange payload for out of band packet exchange.
    Used by VSM_EVT_OOB_ENC_BUF_READY and VSM_EVT_OOB_DEC_BUF_READY.*/
/** The format of the data in shared memory. Following this structure is the
    packet payload, which consists of signed bitstream data.
*/
struct vsm_oob_pkt_exchange_header_t
{
   uint32_t time_stamp;
   /**< Timestamp of the packet sent by the encoder or received by decoder. */

   uint32_t media_type;
   /**< Information about the type of payload.

        @values All media types defined in #vsm_media_type_t except
                VSM_MEDIA_TYPE_NONE, which is not supported. */

   uint32_t size;
   /**< Size of the packet payload.

        @values Any unsigned 32-bit number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_oob_pkt_exchange_header_t */

/** @ingroup vsm_cmd_set_timing_params_offsets
    Sets the VFR mode and timing offsets in a voice stream session.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_SET_TIMING_PARAMS \n
    Dst_port -- vsm_set_timing_params_t

  @msg_payload{vsm_set_timing_params_t}
    @table{weak__vsm__set__timing__params__t}

  @detdesc
    This command is used to provide timing information for a packet exchange
    to the aDSP services. All circuit-switched calls must use either the
    internal or external #VFR_HARD mode. All packet-switched calls with time
    warping enabled or disabled, and other applications, must use #VFR_NONE
    mode.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A valid session must have been created.
    @par
    This command can only be issued in the Stop state.
*/
#define VSM_CMD_SET_TIMING_PARAMS ( 0x00011059 )

/* Payload used to provide timing information for a packet exchange
  to the aDSP services. */
typedef struct vsm_set_timing_params_t vsm_set_timing_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_timing_params_t
@{ */
/* Payload of the #VSM_CMD_SET_TIMING_PARAMS command, which sets the
    VFR mode and timing offsets in a session.
*/
struct vsm_set_timing_params_t
{
   uint16_t mode;
   /**< Sets the stream VFR mode. The decoder derives timing based on its
        output length.

        Supported values:
        - #VFR_NONE -- No periodic tick is used
        - #VFR_HARD -- Internal modem generates a 20 ms periodic tick
        - #VFR_HARD_EXT -- External modem generates a 20 ms periodic tick
		@tablebulletend */

   uint16_t enc_offset;
   /**< Offset from the VFR to send the encoded packet to the modem. The
        encoder delivers a packet at this offset from the VFR tick,

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is derived based on the latency statistics of the uplink
        path: the time taken by the packet to travel from the aDSP through the
        gateway to the modem. The voice Tx processing module derives the
        synchronization offset based on this offset. */

   uint16_t dec_req_offset;
   /**< Offset from the VFR to request the vocoder packet from the modem. The
        decoder requests a packet at this offset from the VFR tick.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is derived based on the modem; by what time the packet is
        ready to be pulled from the VFR tick. */

   uint16_t dec_offset;
   /**< Offset from the VFR to check for an input vocoder packet and start
        decoding/generating erasure. The decoder starts decoding at this
        offset from the VFR tick.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is derived based on the latency statistics of the downlink
        path: the time taken by the packet to travel from the modem through
        the gateway to the aDSP. */

   uint16_t decpp_offset;
   /**< Offset from the VFR for Voice Stream downlink to begin postprocessing
        of incoming data from the decoder.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is forward calculated from the decode-begin offset,
		considering room for decoder processing times. This offset is used
		only in packet-switched calls (mode is #VFR_NONE) and when the time
		warp framework is enabled. Clock settings and system load are factors
		considered for this calculation. */

   uint16_t vptx_delivery_offset;
   /**< Offset from the VFR for Voice Device uplink to deliver processed data to Encoder.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity
        The offset is reverse calculated from the encoder offset, leaving room for
        Encoder processing times. Clock settings and system load are factors considered
        for this calculation.
        Logically, this offset belongs to device only but is needed by stream also for
        supporting in-call music playback use case where timing command needs to be sent
        from VSM to VPM. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_timing_params_t */

/** @ingroup vsm_cmd_get_kpps
    Queries the VSM for required KPPS by aggregating the KPPS of all modules
	independently on the Tx and Rx paths.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_GET_KPPS \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}) .
	@par
    An acknowledgment is sent back in response to this command.
    In the event of a failure, an acknowledgment is not sent back.

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_GET_KPPS ( 0x00011055 )

/** @ingroup vsm_resp_get_kpps_ack
    Response to the #VSM_CMD_GET_KPPS command.

  @apr_hdr_fields
    Opcode   -- VSM_RSP_GET_KPPS_ACK \n
    Dst_port -- Client handle \n
    Src_port -- Handle to the stream session

  @msg_payload{vsm_get_kpps_ack_t}
    @table{weak__vsm__get__kpps__ack__t}

  @dependencies
    A valid session must have been created.

  @return
    None.
*/
#define VSM_RSP_GET_KPPS_ACK ( 0x00011057 )

/* Used by the structure that responds to the Get KPPS command. */
typedef struct vsm_get_kpps_ack_t vsm_get_kpps_ack_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_get_kpps_ack_t
@{ */
/* Payload of the VSM_RSP_GET_KPPS_ACK event, which acknowledges
    the VSM_CMD_GET_KPPS command.
*/
struct vsm_get_kpps_ack_t
{
   uint32_t venc_kpps;
   /**< Specifies the KPPS consumed by voice encoder thread.

        @values Any uint32 value */

   uint32_t vdec_kpps;
   /**< Specifies the KPPS consumed by voice decoder thread.

        @values Any uint32 value */

   uint32_t vdecpp_kpps;
   /**< Specifies the KPPS consumed by voice decoder postprocessing thread.

        @values Any uint32 value */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_get_kpps_ack_t */

/** @addtogroup common_cmd_map_config_vfr_mode
@{ */
/** Sets the timing mode, VSID and timing offsets in a voice stream session.

    This command is used to provide timing information for a packet exchange
    to the aDSP services. Clients who intend to drive timing must set the mode
    to VFR_HARD and send appropriate VSID. Clients who doesn't drive the timing
    must set the mode to VFR_NONE where internal timing is used.

  @apr_hdr_fields
    Opcode -- VSM_CMD_SET_TIMING_PARAMS_V2\n
    Dst_port -- handle to the stream session

  @apr_msg_payload
    @structure{vsm_set_timing_params_v2_t}
    @tablespace
    @inputtable{VSM_CMD_SET_TIMING_PARAMS_V2_APR_message_payload.tex}

  @dependencies
    A valid session must have been created.
      @negparspace
    This command is allowed only in STOP state.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/

#define VSM_CMD_SET_TIMING_PARAMS_V2 ( 0x0001105B )

/* Payload used to provide timing information for a packet exchange
  to the aDSP services. */
typedef struct vsm_set_timing_params_v2_t vsm_set_timing_params_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #VSM_CMD_SET_TIMING_PARAMS_V2 command, which sets the
    timing mode, VSID and timing offsets in a session.
*/
struct vsm_set_timing_params_v2_t
{
   uint16_t mode;
   /**< Sets the timing mode.

        Supported values:
        - #VFR_NONE -- internal timing is used and vsid is ignored
        - #VFR_HARD -- timing is driven by the client, refer to vsid further*/

   uint32_t vsid;
   /**< Voice System ID has information about which HARD VFR timing should be used by the session.
        bits[21:18] represents VFR ID which is also used by AFE for direct mapping of mux input
        in LPA_IF HW. Mux input lines are hard-wired to different modems.

        Supported values:
        - Refer to agile document (80-NF711-1)*/

   uint16_t vfr_cycle;
      /**< Time duration between two consecutive vfrs

      		Supported values:
   		- 20000, 40000 and 60000us.*/

   uint16_t enc_offset;
   /**< Offset from the VFR to send the encoded packet to the client. The
        encoder delivers a packet at this offset from the VFR tick,

        Supported values: 0 to vfr cycle microseconds with 1-microsecond
                          granularity

        The offset is derived based on the latency statistics of the uplink
        path: the time taken by the packet to travel from the aDSP through the
        gateway to the client. The voice Tx processing module derives the
        synchronization offset based on this offset. */

   uint16_t dec_req_offset;
   /**< Offset from the VFR to request the vocoder packet from the client. The
        decoder requests a packet at this offset from the VFR tick.

        Supported values: 0 to vfr cycle microseconds with 1-microsecond
                          granularity

        The offset is derived based on the client; by what time the packet is
        ready to be pulled from the VFR tick.
        @newpage */

   uint16_t dec_offset;
   /**< Offset from the VFR to check for an input vocoder packet and start
        decoding/generating erasure. The decoder starts decoding at this
        offset from the VFR tick.

        Supported values: 0 to vfr cycle microseconds with 1-microsecond
                          granularity

        The offset is derived based on the latency statistics of the downlink
        path: the time taken by the packet to travel from the client through
        the gateway to the aDSP.
        @newpage */

   uint16_t decpp_offset;
   /**< Offset from the VFR for Voice Stream downlink to begin post processing
        incoming data from the decoder.

        Supported values: 0 to 20000 microseconds with 1-microsecond
                          granularity

        The offset is forward calculated from the decode begin offset, considering
        room for Decoder processing times. This offset is used only in the case of
        PS (VFR_NONE) and TW framework is enabled.Clock settings and system load
        are factors considered for this calculation.
        @newpage */

   uint16_t vptx_delivery_offset;
   /**< Offset from the VFR for Voice Device uplink to deliver processed data to Encoder.

        Supported values: 0 to vfr cycle microseconds with 1-microsecond
                          granularity
        The offset is reverse calculated from the encoder offset, leaving room for
        Encoder processing times. Clock settings and system load are factors considered
        for this calculation.
        Logically, this offset belongs to device only but is needed by stream also for
        supporting in-call music playback use case where timing command needs to be sent
        from VSM to VPM.

        @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup common_cmd_map_config_vfr_mode */
/** @ingroup vsm_cmd_set_eamr_mode_change_detection
    Enables/disables the eamr mode detection.

  @apr_hdr_fields
    Opcode -- VSM_CMD_SET_EAMR_MODE_CHANGE_DETECTION\n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_set_eamr_mode_change_detection_t}
    @table{weak__vsm__set__eamr__mode}

  @description
    Enables/disables the eamr mode detection
    This command is expected before every call because re-init will reset the internal flag in vdec structure
      @par

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}). @newpage

  @dependencies
    Voice streams must have been created.
      @par

  @sa
    #VSM_CMD_SET_EAMR_MODE_CHANGE_DETECTION
*/

#define VSM_CMD_SET_EAMR_MODE_CHANGE_DETECTION (0x00011054)

typedef enum
{
    VSM_EAMR_MODE_CHANGE_DETECTION_DISABLE = 0,  /**< Disable */
    VSM_EAMR_MODE_CHANGE_DETECTION_ENABLE           /**< Enable */
} vsm_set_eamr_mode_t;

/* Used by the structure that enables/disables the eamr mode detection. */
typedef struct vsm_set_eamr_mode_change_detection_t vsm_set_eamr_mode_change_detection_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_set_eamr_mode_change_detection_t
@{ */
/* Payload of the VSM_CMD_SET_EAMR_MODE_CHANGE_DETECTION command, which enables
    /disables eamr mode detection.
*/
struct vsm_set_eamr_mode_change_detection_t
{
    uint32_t enable;
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_eamr_mode_change_detection_t */
/** @endcond */

/** @cond OEM_only */

/** @ingroup vsm_eamr_mode_changed_t
    Event that will be sent from VSM to CVS whenever there is a sample rate change..
    Initial sampling rate is assumed to be 8k
    The first notification will be sent when decoder detects watermark in the pkt
    Sampling rate will be continued accross handovers

  @apr_hdr_fields
    Opcode -- VSM_EVT_EAMR_MODE_CHANGED\n
    Dst_port -- Handle to the stream session

  @msg_payload{vsm_eamr_mode_changed_t}
    @table{weak__vsm__eamr__mode__changed__t}

  @description
    Event that will be sent from VSM to CVS whenever there is a sample rate change

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}). @newpage

  @dependencies
    Voice streams must have been created.
      @par
  @sa
    #VSM_EVT_EAMR_MODE_CHANGED
*/

#define VSM_EVT_EAMR_MODE_CHANGED (0x00011053)

typedef enum
{
    VSM_EAMR_MODE_NARROWBAND = 0,              /**< indicates NB mode. */
    VSM_EAMR_MODE_WIDEBAND                  /**< indicates WB mode. */
} vsm_eamr_mode_t;


/* Used by the structure that notifies the eamr bw change. */
typedef struct vsm_eamr_mode_changed_t vsm_eamr_mode_changed_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_eamr_mode_changed_t
@{ */
/* Payload of the VSM_EVT_EAMR_MODE_CHANGED command, that notifies
    the eamr bw change..

*/
struct vsm_eamr_mode_changed_t
{
    uint32_t mode;
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_eamr_mode_changed_t */
/** @endcond */

/** @ingroup vsm_cmd_get_delay
    Queries the VSM for algorithmic delay by aggregating the delay
    of all modules independently on the Tx and Rx paths. This command
    should be issued only once a valid media type has been sent in
    order to ensure accurate delay values.

  @apr_hdr_fields
    Opcode   -- VSM_CMD_GET_DELAY \n
    Dst_port -- Handle to the session

  @msg_payload
    None.

  @return
    VSM_RSP_GET_DELAY_ACK.
	@par
    An acknowledgment is sent back in response to this command.
    In the event of a failure, the error is reported back.

  @dependencies
    A valid session must have been created.
*/
#define VSM_CMD_GET_DELAY ( 0x00011064 )

/** @ingroup vsm_resp_get_delay_ack
    Response to the #VSM_CMD_GET_DELAY command.

  @apr_hdr_fields
    Opcode   -- VSM_RSP_GET_DELAY_ACK \n
    Dst_port -- Client handle \n
    Src_port -- Handle to the stream session

  @msg_payload{vsm_get_delay_ack_t}
    @table{weak__vsm__get__delay__ack__t}

  @dependencies
    A valid session must have been created.

  @return
    None.
*/
#define VSM_RSP_GET_DELAY_ACK ( 0x00011065 )

/* Used by the structure that responds to the Get Delay command. */
typedef struct vsm_get_delay_ack_t vsm_get_delay_ack_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_vsm_get_delay_ack_t
@{ */
/* Payload of the VSM_RSP_GET_DELAY_ACK event, which acknowledges
    the VSM_CMD_GET_DELAY command.
*/
struct vsm_get_delay_ack_t
{
   uint32_t venc_delay;
   /**< Specifies the algorithmic delay (in microseconds)
        added by the voice encoder thread.

        @values Any uint32 value */

   uint32_t vdec_delay;
   /**< Specifies the algorithmic delay (in microseconds)
        added by the voice decoder thread.

        @values Any uint32 value */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_get_delay_ack_t */

/** Command to register for an event from VSM. Currently the following events are supported:
   #VSM_EVT_VOC_OPERATING_MODE_UPDATE
*/
#define VSM_CMD_REGISTER_EVENT ( 0x00011066 )

/** Command to unregister for a previously registered event from VSM.
*/
#define VSM_CMD_UNREGISTER_EVENT ( 0x00011067 )

/** Payload for VSM_CMD_REGISTER_EVENT and VSM_CMD_UNREGISTER_EVENT */
typedef struct vsm_cmd_register_event_t vsm_cmd_register_event_t;

#include "adsp_begin_pack.h"
struct vsm_cmd_register_event_t
{
   uint32_t event_id;
   /**< GUID representing the event */
}
#include "adsp_end_pack.h"
;

#define VSM_EVT_VOC_OPERATING_MODE_UPDATE ( 0x00011068 )

typedef enum vsm_voc_operating_mode_direction_t
{
   VSM_VOC_OPERATING_MODE_DIRECTION_TX = 0,
   /**< Mode change detection from Tx, i.e. encoder */
   VSM_VOC_OPERATING_MODE_DIRECTION_RX
   /**< Mode change detection from Rx, i.e. decoder */
}vsm_voc_operating_mode_direction_t ;

#define VSM_VOC_OPERATING_MODE_NONE   ( 0x00013085 )
/**< indicates no valid media type set yet. */
#define VSM_VOC_OPERATING_MODE_NB     ( 0x00013086 )
/**< indicates 8kHz vocoder bandwidth mode. */
#define VSM_VOC_OPERATING_MODE_WB     ( 0x00013087 )
/**< indicates 16kHz vocoder bandwidth mode. */
#define VSM_VOC_OPERATING_MODE_SWB    ( 0x00013088 )
/**< indicates 32kHz vocoder bandwidth mode. */
#define VSM_VOC_OPERATING_MODE_FB     ( 0x00013089 )
/**< indicates 48kHz vocoder bandwidth mode. */

/** Payload for VSM_EVT_VOC_OPERATING_MODE_UPDATE */
typedef struct vsm_evt_voc_operating_mode_update_t vsm_evt_voc_operating_mode_update_t;

#include "adsp_begin_pack.h"
struct vsm_evt_voc_operating_mode_update_t
{
    uint16_t direction;
    /**< Indicates direction where mode change is detected */
    uint16_t reserved;
    /**< Reserved, must be set to zero */
    uint32_t mode;
    /**< Indicates the new vocoder mode */
}
#include "adsp_end_pack.h"
;

/**
    This command sets the Enhanced Voice Services (EVS) encoder bit-rate and
    the bit-rate's associated bandwidth. All valid bandwidths for EVS bit-rates
    are described in the command’s payload. This will return error APR_EFAILED
    when EVS vocoder is not being used by VSM the session

  @apr_hdr_fields
    Opcode -- VSM_CMD_EVS_VOC_ENC_SET_RATE\n
    Dst_port -- handle to the stream session

  @apr_msg_payload
    @structure{vsm_set_evs_voc_enc_rate_t}

  @dependencies
    A valid session must have been created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define  VSM_CMD_SET_EVS_ENC_OPERATING_MODE ( 0x00013166 )

/** Payload for VSM_CMD_EVS_VOC_ENC_SET_RATE */
typedef struct vsm_set_evs_enc_operating_mode_t vsm_set_evs_enc_operating_mode_t;

struct vsm_set_evs_enc_operating_mode_t
{
   uint8_t mode;
   /**< Sets the EVS encoder rate.

        @values
        - 0x00000000 --  6.60 kbps, AMR-WB IO
        - 0x00000001 --  8.85 kbps, AMR-WB IO
        - 0x00000002 -- 12.65 kbps, AMR-WB IO
        - 0x00000003 -- 14.25 kbps, AMR-WB IO
        - 0x00000004 -- 15.85 kbps, AMR-WB IO
        - 0x00000005 -- 18.25 kbps, AMR-WB IO
        - 0x00000006 -- 19.85 kbps, AMR-WB IO
        - 0x00000007 -- 23.05 kbps, AMR-WB IO
        - 0x00000008 -- 23.85 kbps, AMR-WB IO
        - 0x00000009 -- 5.90 kbps, EVS (NB, WB)
        - 0x0000000A -- 7.20 kbps, EVS (NB, WB)
        - 0x0000000B -- 8.00 kbps, EVS (NB, WB)
        - 0x0000000C -- 9.60 kbps, EVS (NB, WB, SWB)
        - 0x0000000D -- 13.20 kbps, EVS (NB, WB, SWB)
        - 0x0000000E -- 16.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x0000000F -- 24.40 kbps, EVS (NB, WB, SWB, FB)
        - 0x00000010 -- 32.00 kbps, EVS (WB, SWB, FB)
        - 0x00000011 -- 48.00 kbps, EVS (WB, SWB, FB)
        - 0x00000012 -- 64.00 kbps, EVS (WB, SWB, FB)
        - 0x00000013 -- 96.00 kbps, EVS (WB, SWB, FB)
        - 0x00000014 -- 128.00 kbps, EVS (WB, SWB, FB)  */

   uint8_t bandwidth;
  /**< Sets EVS Vocoder current encoder bandwidth.

        @values
        - #VSM_EVS_VOC_BANDWIDTH_NB
        - #VSM_EVS_VOC_BANDWIDTH_WB
        - #VSM_EVS_VOC_BANDWIDTH_SWB
        - #VSM_EVS_VOC_BANDWIDTH_FB   */

}

/** Narrowband bandwidth for EVS. */
#define VSM_EVS_VOC_BANDWIDTH_NB ( 0 )

/** Wideband for bandwidth  EVS. */
#define VSM_EVS_VOC_BANDWIDTH_WB    ( 1 )

/** Super-wideband for bandwidth  EVS. */
#define VSM_EVS_VOC_BANDWIDTH_SWB   ( 2 )

/**Fullband for  bandwidth EVS. */
#define VSM_EVS_VOC_BANDWIDTH_FB    ( 3 )


#include "adsp_end_pack.h"
;

/**

    This command enables the encoder channel aware mode for EVS. It also provides
    the required parameters for running the channel aware mode. This command will
    have no effect and will return error APR_EFAILED for the following cases:
	  - The EVS vocoder is not being used by the stream
	  - The payload parameters are invalid



  @apr_hdr_fields
    Opcode -- VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE\n
    Dst_port -- handle to the stream session

  @apr_msg_payload
    @structure{vsm_set_evs_enc_channel_aware_mode_enable_t}

  @dependencies
    A valid session must have been created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define  VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE ( 0x00013168 )

/** Payload for VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE */
typedef struct vsm_set_evs_enc_channel_aware_mode_enable_t vsm_set_evs_enc_channel_aware_mode_enable_t;


struct vsm_set_evs_enc_channel_aware_mode_enable_t
{
   uint8_t fec_offset;
   /**< Specifies the forward-error correction offset.

       @values 2, 3, 5, 7 */

   uint8_t fer_rate;
   /**< Specifies FER rate threshold to LOW (0) or HIGH (1). */

}

#include "adsp_end_pack.h"
;
/**
    This command disables the encoder channel aware mode for EVS. This command
    will have no effect and will return error APR_EFAILED when either the EVS
    vocoder is not being used or if Channel Aware mode is already disabled.

  @apr_hdr_fields
    Opcode -- VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE\n
    Dst_port -- handle to the stream session

  @msg_payload
    None.

  @dependencies
    A valid session must have been created.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
*/
#define  VSM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE ( 0x0001316C )


#include "adsp_end_pack.h"
;

/*cond OEM_only */
/** @ingroup vocoder_param_ids */

/** Applies to voice decoders. This parameter has a fixed size of 4 bytes.

    @inputtable{Param_ID_VOICE_VOC_PARAM_PLC_MODE.tex}
*/

#define VOICE_VOC_PARAM_PLC_MODE   ( 0x00010E9B )

/* Used by the structure that sets the PLC mode on the decoder. */
typedef struct voice_set_plc_mode_t voice_set_plc_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_voice_set_plc_mode_t
@{ */
/* Payload of the VOICE_VOC_PARAM_PLC_MODE param ID, which sets the
   PLC mode on the decoder. This param ID currently applies to only 
   VSM_MEDIA_TYPE_G711_ALAW, VSM_MEDIA_TYPE_G711_MLAW, VSM_MEDIA_TYPE_G711_ALAW_V2 and
   VSM_MEDIA_TYPE_G711_MLAW_V2 media IDs 

*/
struct voice_set_plc_mode_t
{
   uint32_t plc_mode;
   /**< Specifies whether PLC mode is disabled (0) or enabled (1). Default value is 1*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_vsm_set_plc_mode_t */
/*endcond */

/** @} */ /* end_addtogroup vocoder_param_ids */

#endif /* __ADSP_VSM_API_H__ */
