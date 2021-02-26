#ifndef __APRV2P1_PACKET_H__
#define __APRV2P1_PACKET_H__

/*
  Copyright (C) 2010 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/core/inc/aprv2p1_packet.h#1 $
  $Author: pwbldsvc $
*/

#include "apr_comdef.h"

/****************************************************************************
 * The definitions.                                                         *
 ****************************************************************************/

#define APRV2P1_PKT_VERSION_V ( 1 )

#define APRV2P1_PKT_MSGTYPE_EVENT_V ( 0 )
#define APRV2P1_PKT_MSGTYPE_CMDRSP_V ( 1 )
#define APRV2P1_PKT_MSGTYPE_SEQCMD_V ( 2 )
#define APRV2P1_PKT_MSGTYPE_NSEQCMD_V ( 3 )

#define APRV2P1_PKT_INIT_ADDR_V ( ( uint16_t ) 0 )

#define APRV2P1_PKT_HEADER_WORD_SIZE_V ( ( sizeof( aprv2p1_packet_t ) + 3 ) >> 2 )
  /* V2.1 header size in number of 32-bit words. */
#define APRV2P1_PKT_HEADER_BYTE_SIZE_V ( APRV2P1_PKT_HEADER_WORD_SIZE_V << 2 )
  /* V2.1 header size in number of bytes. */

/****************************************************************************
 * The packet bit-field macros.                                             *
 ****************************************************************************/

#define APRV2P1_PKT_VERSION_MASK ( 0x0000000F )
#define APRV2P1_PKT_VERSION_SHFT ( 0 )
#define APRV2P1_PKT_HEADER_SIZE_MASK ( 0x000000F0 )
#define APRV2P1_PKT_HEADER_SIZE_SHFT ( 4 )
#define APRV2P1_PKT_MSGTYPE_MASK ( 0x00000300 )
#define APRV2P1_PKT_MSGTYPE_SHFT ( 8 )
#define APRV2P1_PKT_RESERVED_MASK ( 0x0000FC00 )
#define APRV2P1_PKT_RESERVED_SHFT ( 10 )
#define APRV2P1_PKT_PACKET_SIZE_MASK ( 0xFFFF0000 )
#define APRV2P1_PKT_PACKET_SIZE_SHFT ( 16 )

#define APRV2P1_PKT_SERVICE_ID_MASK ( 0x00FF )
#define APRV2P1_PKT_SERVICE_ID_SHFT ( 0 )
#define APRV2P1_PKT_DOMAIN_ID_MASK ( 0xFF00 )
#define APRV2P1_PKT_DOMAIN_ID_SHFT ( 8 )

#define APRV2P1_PKT_DEBUG_TRACE_HOP_COUNT_MASK ( 0x000000FF )
#define APRV2P1_PKT_DEBUG_TRACE_HOP_COUNT_SHFT ( 0 )
#define APRV2P1_PKT_DEBUG_TRACE_ORG_SEQ_NUM_MASK ( 0x00FFFF00 )
#define APRV2P1_PKT_DEBUG_TRACE_ORG_SEQ_NUM_SHFT ( 8 )
#define APRV2P1_PKT_DEBUG_TRACE_ORG_DOMAIN_ID_MASK ( 0xFF000000 )
#define APRV2P1_PKT_DEBUG_TRACE_ORG_DOMAIN_ID_SHFT ( 24 )

/****************************************************************************
 * The packet helper macros.                                                *
 ****************************************************************************/

/**
 * Return the packet's current size in bytes given the packet header.
 *
 * The current packet byte size is the sum of the base packet structure and
 * the used portion of the payload.
 */
#define APRV2P1_PKT_GET_PACKET_BYTE_SIZE( header ) \
  ( APR_GET_FIELD( APRV2P1_PKT_PACKET_SIZE, header ) )

/**
 * Return the header's current size in bytes given the packet header.
 */
#define APRV2P1_PKT_GET_HEADER_BYTE_SIZE( header ) \
  ( APR_GET_FIELD( APRV2P1_PKT_HEADER_SIZE, header ) << 2 )

/**
 * Return the payload's current size in bytes given the packet header.
 *
 * The current payload byte size is the difference between the packet size and
 * the header size.
 */
#define APRV2P1_PKT_GET_PAYLOAD_BYTE_SIZE( header ) \
  ( APRV2P1_PKT_GET_PACKET_BYTE_SIZE( header ) - \
    APRV2P1_PKT_GET_HEADER_BYTE_SIZE( header ) )

/**
 * Return a pointer to the beginning of the packet's payload.
 */
#define APRV2P1_PKT_GET_PAYLOAD( type, packet_ptr ) \
  ( ( type* ) APR_PTR_END_OF( packet_ptr, \
                APRV2P1_PKT_GET_HEADER_BYTE_SIZE( \
                  ( ( struct aprv2p1_packet_t* ) packet_ptr )->header ) ) )

/****************************************************************************
 * The packet structure definition.                                         *
 ****************************************************************************/

/**
 * The packet structure contains all information necessary to route a packet
 * from a source to a destination.
 */
typedef struct aprv2p1_packet_t aprv2p1_packet_t;

struct aprv2p1_packet_t
{
  uint32_t header;
    /**<
     * \par version: bit 3..0 (4 bits).
     *
     * The packet version.
     *
     * Packet versions are assumed to be incompatible unless otherwise
     * indicated.
     *
     * This value should be set to APRV2P1_PKT_VERSION_V (2).
     *
     * \par header_size: bit 7..4 (4 bits).
     *
     * The header size.
     *
     * The header size is the number of 32-bit header words starting from
     * the beginning of the packet.
     *
     * \par msg_type: bit 9..8 (2 bits).
     *
     * The message type selects the particular message processing behavior
     * on the recipient.
     *
     * The Eagle protocol defines four types of asynchronous messages each
     * having a different intent.
     *
     * APRV2P1_PKT_MSGTYPE_EVENT_V (0) selects the event message type. The
     * behavior and guarantees of event messages are message-specific and
     * service-specific.
     *
     * APRV2P1_PKT_MSGTYPE_SEQCMD_V (1) selects the sequential command message
     * type. Each sequential command message is processed only after the
     * previous command completes in FIFO order.
     *
     * APRV2P1_PKT_MSGTYPE_NSEQCMD_V (2) selects the non-sequential command
     * message type. Non-sequential commands are processed by the recipient
     * irrespective of any on-going sequential commands. Non-sequential
     * commands are reserved for special purposes. One such case is to avert
     * a sequential command processing deadlock when both the client and the
     * server send commands to each other. The server-to-client direction is
     * defined to have a higher priority than the client-to-server
     * direction. Thus server-to-client commands is restricted to
     * non-sequential command messages.
     *
     * APRV2P1_PKT_MSGTYPE_CMDRSP_V (3) selects the command response message
     * type. Command response messages are guaranteed to be returned by the
     * server to the client after commands have completed or have failed.
     * Command response messages typically carry return statuses.
     *
     * This value should be set to one of the four message types as defined
     * by the specific opcode and/or by the specific service.
     *
     * \par reserved: bit 15..10 (6 bits).
     *
     * Reserved; set the value to 0.
     *
     * \par packet_size: bit 31..16 (16 bits).
     *
     * The total packet size in terms of bytes.
     *
     * The total packet size includes both the header and payload data.
     */
  uint16_t src_addr;
    /**<
     * This is the source address where the packet came from.
     *
     * The proper return message routing path from server to client is the
     * same path taken from client to server. On the return path all
     * previous source addresses become the destination addresses.
     *
     * \par domain_id: bit 15..8 (8 bits).
     *
     * The domain ID refers to a process, a processor, or an off-target
     * location which houses a single Packet Router.
     *
     * The transport layer used may differ depending on the physical link
     * between domains.
     *
     * All identifier values are reserved by the Audio Packet Router.
     *
     * A value of 0 may be used to refer to the local domain.
     *
     * \par service_id: bit 7..0 (8 bits).
     *
     * The service ID refers to a registered service within a domain.
     *
     * Refer to the target configuration file to determine the actual
     * address mappings.
     */
  uint16_t src_port;
    /**<
     * The source port is an arbitrary service defined value.
     *
     * The source port is used to indicate the sending end of a
     * communication channel between two services. Ports are not limited to
     * and can be used for identifying group sessions, independent stream
     * sessions, resources, memory addresses, objects, command queues, and
     * etc.
     *
     * The source port should be initialized to APR_NULL_V (0) when
     * unused.
     */
  uint16_t dst_addr;
    /**<
     * This is the destination address where the packet needs to be
     * delivered.
     *
     * The proper return message routing path from server to client is the
     * same path taken from client to server. On the return path all
     * previous destination addresses become the source addresses.
     *
     * \par domain_id: bit 15..8 (8 bits).
     *
     * The domain ID refers to a process, a processor, or an off-target
     * location which houses a single Packet Router.
     *
     * The transport layer used may differ depending on the physical link
     * between domains.
     *
     * All identifier values are reserved by the Audio Packet Router.
     *
     * A value of 0 may be used to refer to the local domain.
     *
     * \par service_id: bit 7..0 (8 bits).
     *
     * The service ID refers to a registered service within a domain.
     *
     * Refer to the target configuration file to determine the actual
     * address mappings.
     */
  uint16_t dst_port;
    /**<
     * The destination port is an arbitrary service defined value.
     *
     * The destination port is used to indicate the receiving end of a
     * communication channel between two services. Ports are not limited to
     * and can be used for identifying group sessions, independent stream
     * sessions, resources, memory addresses, objects, command queues, and
     * etc.
     *
     * The destination port should be initialized to APR_NULL_V (0)
     * when unused.
     */
  uint32_t token;
    /**<
     * The token is a client transaction ID provided by the sender.
     *
     * The client token should be initialized to APR_NULL_V (0) when
     * none exists.
     */
  uint32_t opcode;
    /**<
     * The operation code defines both the action and the payload structure.
     *
     * The opcode is a GUID (Globally Unique ID).
     *
     * The opcode must be a valid value.
     */
  uint32_t trace;
    /**<
     * The trace field is used for debugging timing, stability, and
     * performance issues. The field can also be used to track messages
     * flowing across subsystems.
     *
     * The trace field should be initialized to zero to request the local
     * router to stamp a trace value on the message.
     *
     * \par hop_count: bit 7..0 (8 bits).
     *
     * The hop count.
     *
     * The hop count increments by one each time a packet is sent or
     * forwarded. The hop count starts from zero when first sent. The hop
     * count wraps around to zero after 255 hops.
     *
     * \par org_seq_num: bit 23..8 (16 bits).
     *
     * The originating sequence number.
     *
     * The originating sequence number is a (relatively) unique value
     * generated from a packet's originating source domain. This value allows
     * tracking specific messages from the originating source to the final
     * destination as well as tracking specific problem messages from any
     * particular hop count back to the originating source.
     *
     * The originating sequence number should not be modified for the
     * lifetime of the message.
     *
     * \par org_domain_id: bit 31..24 (8 bits).
     *
     * The originating source domain ID.
     *
     * The originating source domain ID indicates where the originating
     * sequence number was generated.
     *
     * The originating source domain ID should not be modified for the
     * lifetime of the message.
     */
  uint64_t timestamp;
#ifdef APR_PROFILING
  uint32_t timediff;
#endif
};

#endif /* __APRV2P1_PACKET_H__ */

