#ifndef __APRV2_PACKET_H__
#define __APRV2_PACKET_H__

/** @file aprv2_packet.h
*/

/*===========================================================================
Copyright (c) 2010-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/aprv2_packet.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup aprv2_constants_macros
@{ */
/****************************************************************************/
/** @name Header Field Definitions
@{ */
/****************************************************************************/

#define APRV2_PKT_VERSION_V ( 0 )
  /**< Defines the layout of the packet structure. Packet versions are assumed
       to be backwards compatible to the core format (first five header words),
       unless otherwise indicated. The major difference between the APRv2
       packet versions is how the reserved fields and the optional header
       extension field are interpreted. */

#define APRV2_PKT_MSGTYPE_EVENT_V ( 0 )
  /**< Used by #aprv2_packet_t for the event message type. The behavior and
       event message guarantees are message-specific and service-specific. */

#define APRV2_PKT_MSGTYPE_CMDRSP_V ( 1 )
  /**< Used by #aprv2_packet_t for the command response message type. Command
       response messages are guaranteed to be returned by the server to the
       client after commands have completed or have failed. Command response
       messages typically carry a return status. */

#define APRV2_PKT_MSGTYPE_SEQCMD_V ( 2 )
  /**< Used by #aprv2_packet_t for the sequential command message type. Each
       sequential command message is processed only after the previous command
       completes in FIFO order. */

#define APRV2_PKT_MSGTYPE_NSEQCMD_V ( 3 )
  /**< Used by #aprv2_packet_t for the non-sequential command message type.
       Non-sequential commands are processed by the recipient regardless of any
       on-going sequential commands.

       Non-sequential commands are reserved for special purposes. One such case
       is to avert a sequential command processing deadlock when both the
       client and the server send commands to each other. The server-to-client
       direction is defined to have a higher priority than the client-to-server
       direction.

       Therefore, server-to-client commands are restricted to non-sequential
       command messages. */

#define APRV2_PKT_INIT_ADDR_V ( ( uint16_t ) 0 )
  /**< Address intialization value. */

#define APRV2_PKT_HEADER_WORD_SIZE_V ( ( sizeof( aprv2_packet_t ) + 3 ) >> 2 )
  /**< V2.0 header size in number of 32-bit words. */

#define APRV2_PKT_HEADER_BYTE_SIZE_V ( APRV2_PKT_HEADER_WORD_SIZE_V << 2 )
  /**< V2.0 header size in number of bytes. */

/** @} */ /* end_name Header Field Definitions */

/****************************************************************************/
/** @name APRv2 Packet Bitfield Macros for Header Fields
@{ */
/****************************************************************************/

#define APRV2_PKT_VERSION_MASK ( 0x0000000F )     /**< Bitmask of the version
                                                       field. */
#define APRV2_PKT_VERSION_SHFT ( 0 )              /**< Bit shift of the version
                                                       field. */
#define APRV2_PKT_HEADER_SIZE_MASK ( 0x000000F0 ) /**< Bitmask of the header
                                                       size field. */
#define APRV2_PKT_HEADER_SIZE_SHFT ( 4 )          /**< Bit shift of the header
                                                       size field. */
#define APRV2_PKT_MSGTYPE_MASK ( 0x00000300 )     /**< Bitmask of the message
                                                       type field. */
#define APRV2_PKT_MSGTYPE_SHFT ( 8 )              /**< Bit shift of the message
                                                       type field. */
#define APRV2_PKT_RESERVED_MASK ( 0x0000FC00 )    /**< Bitmask of the reserved
                                                       field. */
#define APRV2_PKT_RESERVED_SHFT ( 10 )            /**< Bit shift of the
                                                       reserved field. */
#define APRV2_PKT_PACKET_SIZE_MASK ( 0xFFFF0000 ) /**< Bitmask of the packet
                                                       size field. */
#define APRV2_PKT_PACKET_SIZE_SHFT ( 16 )         /**< Bit shift of the packet
                                                       size field. */

/** @} */ /* end_name Packet Bitfield Macros for Header Fields */

/****************************************************************************/
/** @name Packet Bitfield Macros for Address Fields
@{ */
/****************************************************************************/

#define APRV2_PKT_SERVICE_ID_MASK ( 0x00FF )      /**< Bitmask of the service
                                                       ID field. */
#define APRV2_PKT_SERVICE_ID_SHFT ( 0 )           /**< Bit shift of the service
                                                       ID field. */
#define APRV2_PKT_DOMAIN_ID_MASK ( 0xFF00 )       /**< Bitmask of the domain ID
                                                       field. */
#define APRV2_PKT_DOMAIN_ID_SHFT ( 8 )            /**< Bit shift of the domain
                                                       ID field. */

/** @} */ /* end_name Packet Bitfield Macros for Address Fields */

/****************************************************************************/
/** @name Packet Helper Macros
@{ */
/****************************************************************************/

/** Given the packet header, returns the packet's current size in bytes.
    The current packet byte size is the sum of the base packet structure and
    the used portion of the payload.
*/
#define APRV2_PKT_GET_PACKET_BYTE_SIZE( header ) \
  ( APR_GET_FIELD( APRV2_PKT_PACKET_SIZE, header ) )

/** Given the packet header, returns the header's current size in bytes.
*/
#define APRV2_PKT_GET_HEADER_BYTE_SIZE( header ) \
  ( APR_GET_FIELD( APRV2_PKT_HEADER_SIZE, header ) << 2 )

/** Given the packet header, returns the payload's current size in bytes.
    The current payload byte size is the difference between the packet size and
    the header size.
*/
#define APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( header ) \
  ( APRV2_PKT_GET_PACKET_BYTE_SIZE( header ) - \
    APRV2_PKT_GET_HEADER_BYTE_SIZE( header ) )

/** Given the packet, returns a pointer to the beginning of the packet's
    payload.
*/
#define APRV2_PKT_GET_PAYLOAD( type, packet_ptr ) \
  ( ( type* ) APR_PTR_END_OF( packet_ptr, \
                APRV2_PKT_GET_HEADER_BYTE_SIZE( \
                  ( ( struct aprv2_packet_t* ) packet_ptr )->header ) ) )

/** @} */ /* end_name Packet Helper Macros */
/** @} */ /* end_addtogroup aprv2_constants_macros */

/** @addtogroup aprv2_data_types
@{ */
/****************************************************************************/
/** @name Packet Structure Definition
@{ */
/****************************************************************************/

/* Packet structure type definition. */
typedef struct aprv2_packet_t aprv2_packet_t;

/** Core header structure (V2.0) necessary to route a packet from a
	source to a destination.

	Members of the V2 family of packet structures may have different total
	header sizes, but all variants share the same core header structure and
	therefore share a common set of basic functionalities. For more
    information, see Section @xref{sec:APRv2PacketDetails}.

    @proto
    @code
typedef struct aprv2_packet_t aprv2_packet_t;

struct aprv2_packet_t
{
  uint32_t header;
  uint16_t src_addr;
  uint16_t src_port;
  uint16_t dst_addr;
  uint16_t dst_port;
  uint32_t token;
  uint32_t opcode;
};
    @endcode
*/
struct aprv2_packet_t
{
  uint32_t header;
  /**< Contains the following subfield information for the header field.
       (For more information, see Section @xref{sec:APRv2PacketDetailsHeader}.)

       @subfld{version}
        - Bits 3 to 0 (four bits)

       @subfld{header_size}
        - Bits 7 to 4 (four bits)
        - The header size is the number of 32-bit header words starting
          from the beginning of the packet.
        - Set this value to 5 when the optional header extension field is
          not used.

       @subfld{msg_type}
        - Bits 9 to 8 (two bits)
        - The message type selects the particular message processing
          behavior on the recipient.
        - Set this value to one of the four types of asynchronous messages,
          as defined by by the specific opcode or service:
           - #APRV2_PKT_MSGTYPE_EVENT_V
           - #APRV2_PKT_MSGTYPE_CMDRSP_V
           - #APRV2_PKT_MSGTYPE_SEQCMD_V
           - #APRV2_PKT_MSGTYPE_NSEQCMD_V

       @subfld{reserved}
        - Bits 15 to 10 (six bits)
        - Set the value to 0.

       @subfld{packet_size}
        - Bits 31 to 16 (16 bits)
        - The total packet size in bytes.
        - The total packet size includes both the header and payload data.
        @tablebulletend */

  uint16_t src_addr;
  /**< Source address from where the packet came.

       The proper return message routing path from server to client is the
       same path taken from client to server. On the return path, all
       previous source addresses become the destination addresses.

       Refer to the target configuration file to determine the actual
       address mappings.

       @subfld{domain_id}
        - Bits 15 to 8 (eight bits)
        - The domain ID refers to a process, a processor, or an off-target
          location that houses a single packet router.
        - The transport layer used may differ depending on the physical
          link between domains.
        - All identifier values are reserved by the APR. A value of zero
          can be used to refer to the local domain.

       @subfld{service_id}
        - Bits 7 to 0 (eight bits)
        - The service ID refers to a registered service within a domain.
        @tablebulletend */

  uint16_t src_port;
  /**< Source port is an arbitrary service defined value.

       The source port is used to indicate the sending end of a
       communication channel between two services. Ports are not limited to
       and can be used for identifying group sessions, independent stream
       sessions, resources, memory addresses, objects, command queues, etc.

       Initialize the source port to #APR_NULL_V when unused. */

  uint16_t dst_addr;
  /**< Destination address where the packet is to be delivered.

       The proper return message routing path from server to client is the
       same path taken from client to server. On the return path, all previous
       destination addresses become the source addresses.

       Refer to the target configuration file to determine the actual
       address mappings.

       @subfld{domain_id}
        - Bits 15 to 8 (eight bits)
        - The domain ID refers to a process, a processor, or an off-target
          location that houses a single packet router.
        - The transport layer used may differ depending on the physical
          link between domains.
        - All identifier values are reserved by the APR. A value of zero
          can be used to refer to the local domain.

       @subfld{service_id}
        - Bits 7 to 0 (eight bits)
        - The service ID refers to a registered service within a domain.
        @tablebulletend */

  uint16_t dst_port;
  /**< Arbitrary service defined value. The destination port is used to
       indicate the receiving end of a communication channel between two
       services.

       Ports are not limited to and can be used for identifying group sessions,
       independent stream sessions, resources, memory addresses, objects,
       command queues, etc.

       Initialize the destination port to #APR_NULL_V when is is not used. */

  uint32_t token;
  /**< Client transaction ID provided by the sender. This initializes the
       client token to #APR_NULL_V when none exists. */

  uint32_t opcode;
  /**< Defines both the action and the payload structure. This operation code
       is a Globally Unique ID (GUID) and must be a valid value. */
};

/** @} */ /* end_name Packet Structure Definition */
/** @} */ /* addtogroup aprv2_data_types */

#endif /* __APRV2_PACKET_H__ */

