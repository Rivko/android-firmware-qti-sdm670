#ifndef ELITEAPRIF_H
#define ELITEAPRIF_H

/**
@file EliteAprIf.h

@brief This file provides an API wrapper of APR functions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013-2016 Qualcomm Technologies, Inc. All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "aprv2_api_inline.h"
#include "apr_errcodes.h"
#include "aprv2_msg_if.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup elite_apr_datatypes
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** APR packet wrapper. */
typedef aprv2_packet_t elite_apr_packet_t;

/** APR handle. */
typedef uint32_t          elite_apr_handle_t;

/** APR port. This value is the least significant 16 bits of the APR handle. */
typedef uint16_t          elite_apr_port_t;

/** APR address. This value is the most significant 16 bits of the APR handle. */
typedef uint16_t          elite_apr_addr_t;

/** APR callback function type. */
typedef int32_t ( *elite_apr_call_back_fn_t ) ( elite_apr_packet_t* packet, void* dispatch_data );

/** @} */ /* end_addtogroup elite_apr_datatypes */


/* =======================================================================
**                          Basic APR Core Services
** ======================================================================= */

/** @ingroup eliteapr_func_reg_name
  Registers a service callback by name with the APR. For example, the ASM name
  is qcom.adsp.asm.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_call_back_fn_t

  @param[out] pRetHandle   Pointer to the APR handle to be returned by the APR.
  @param[out] usRetAddr    Pointer to the address of this service as determined
                           from the name by the APR.
  @param[in]  pSvcName     Pointer to the array containing the service name.
  @param[in]  ulNameSize   Size of the array containing the service name.
  @param[in]  pFn          Callback function to be invoked by the APR when
                           messages are intended for this service.
  @param[in]  pData        Service-defined data pointer to forward on
                           invocation to the callback function.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_register_by_name(
   elite_apr_handle_t  *pRetHandle,
   elite_apr_addr_t    *usRetAddr,
   char    *pSvcName,
   uint32_t  ulNameSize,
   elite_apr_call_back_fn_t pFn,
   void                    *pData)

{
   uint16_t usDummyDesiredAddr = 0;
   return  __aprv2_cmd_register2 (pRetHandle, pSvcName, ulNameSize,
                                  usDummyDesiredAddr, pFn, pData,usRetAddr);
}

/** @ingroup eliteapr_func_register
  Registers a service callback by address with the APR.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_call_back_fn_t

  @param[out] pRetHandle      Pointer to the APR handle to be returned by the
                              APR.
  @param[in]  usDesiredAddr   Address of the service.
  @param[in]  pFn             Callback function to be invoked by the APR
                              when messages are intended for this service.
  @param[in]  pData           Service-defined data pointer to forward on
                              invocation to the callback function.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_register(
   elite_apr_handle_t  *pRetHandle,
   elite_apr_addr_t    usDesiredAddr,
   elite_apr_call_back_fn_t pFn,
   void                    *pData)

{
   return  __aprv2_cmd_register (pRetHandle, usDesiredAddr, pFn,pData);
}

/** @ingroup eliteapr_func_deregister
  Deregisters a service by handle.

  @datatypes
  #elite_apr_handle_t

  @param[in] handle   Handle of the service.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_deregister(elite_apr_handle_t handle)
{ return   __aprv2_cmd_deregister (handle); }

/** @ingroup eliteapr_func_lookup_addr_name
  Gets the APR address by name.

  @datatypes
  #elite_apr_addr_t

  @param[in]  name       Pointer to the array containing the service name.
  @param[in]  size       Size of the array containing the service name.
  @param[out] ret_addr   Pointer to the address of the specified service.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_lookup_addr_by_name( char * name,
                                               uint32_t size,
                                               elite_apr_addr_t *ret_addr)
{
   return __aprv2_cmd_local_dns_lookup( name, size, ret_addr);
}


/* =======================================================================
**                          Basic Get/SET field from apr Packet
** ======================================================================= */
/** @ingroup eliteapr_func_get_payload
  Gets the APR packet payload.

  @datatypes
  #elite_apr_packet_t

  @param[out] pPayload   Double pointer to the payload.
  @param[in]  pPacket    Pointer to the APR packet.

  @return
  None.

  @dependencies
  None.
*/
static inline void elite_apr_if_get_payload( void **pPayload, elite_apr_packet_t *pPacket)
{
   *pPayload = APRV2_PKT_GET_PAYLOAD( void, pPacket );
}


/** @ingroup eliteapr_func_get_pkt_size
  Gets the APR packet size.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPacket   Pointer to the APR packet.

  @return
  APR packet size.

  @dependencies
  None.
*/
static inline uint32_t elite_apr_if_get_pkt_size( elite_apr_packet_t *pPacket)
{
   return APRV2_PKT_GET_PACKET_BYTE_SIZE( pPacket->header );
}

/** @ingroup eliteapr_func_set_pkt_size
  Sets the APR packet size.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPacket       Pointer to the APR packet.
  @param[in] uPacketSize   Packet size.

  @return
  None.

  @dependencies
  None.
 */
static inline void elite_apr_if_set_pkt_size( elite_apr_packet_t *pPacket, uint32_t uPacketSize)
{
   // first, clear whatever was there.
   pPacket->header ^= APR_SET_FIELD (APRV2_PKT_PACKET_SIZE, APRV2_PKT_GET_PACKET_BYTE_SIZE( pPacket->header ));
   // Now, OR in the new value.
   pPacket->header |= APR_SET_FIELD( APRV2_PKT_PACKET_SIZE, uPacketSize );
}

/** @ingroup eliteapr_func_set_payld_size
  Sets the APR packet payload size.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPacket        Pointer to the APR packet.
  @param[in] uPayloadSize   Payload size.

  @return
  None.

  @dependencies
  None.
 */
static inline void elite_apr_if_set_payload_size( elite_apr_packet_t *pPacket, uint32_t uPayloadSize)
{
   elite_apr_if_set_pkt_size( pPacket, uPayloadSize + APRV2_PKT_GET_HEADER_BYTE_SIZE( pPacket->header ));
}

/** @ingroup eliteapr_func_get_payld_size
  Gets the APR packet payload size.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPacket   Pointer to the APR packet.

  @return
  APR packet payload size.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_get_payload_size( elite_apr_packet_t *pPacket)
{
   return APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( pPacket->header );
}

/** @ingroup eliteapr_func_get_opcode
  Gets the APR packet opcode.

  @datatypes
  #elite_apr_packet_t

  @param[in] packet   Pointer to the APR packet.

  @return
  APR packet opcode.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_get_opcode(elite_apr_packet_t* packet)
{
   return packet->opcode;
}

/** @ingroup eliteapr_func_get_dest_addr
  Gets the destination address of the APR packet.

  @datatypes
  #elite_apr_packet_t

  @param[in] packet   Pointer to the APR packet.

  @return
  elite_apr_addr_t -- Destination address of the packet.

  @dependencies
  None.
 */
static inline elite_apr_addr_t elite_apr_if_get_dst_addr(elite_apr_packet_t* packet)
{
   return packet->dst_addr;
}

/** @ingroup eliteapr_func_get_dest_port
  Gets the destination port of the APR packet.

  @datatypes
  #elite_apr_packet_t

  @param[in] packet   Pointer to the APR packet.

  @return
  elite_apr_port_t -- Destination port of the packet.

  @dependencies
  None.
 */
static inline elite_apr_port_t elite_apr_if_get_dst_port(elite_apr_packet_t* packet)
{
   return packet->dst_port;
}

/** @ingroup eliteapr_func_get_src_port
  Gets the source port of the APR packet.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPkt   Pointer to the APR packet.

  @return
  elite_apr_port_t -- Source port of the packet.

  @dependencies
  None.
 */
static inline elite_apr_port_t elite_apr_if_get_src_port( elite_apr_packet_t *pPkt)
{
   return pPkt->src_port;
}

/** @ingroup eliteapr_func_set_dst_port
  Sets the destination port of the APR packet.

  @datatypes
  #elite_apr_packet_t
  #elite_apr_port_t

  @param[in] pPkt      Pointer to the APR packet.
  @param[in] apr_port  Destination port of the packet.

  @return
  None

  @dependencies
  None.
 */
static inline void elite_apr_if_set_dst_port(elite_apr_packet_t *pPkt, elite_apr_port_t apr_port)
{
   pPkt->dst_port = apr_port;
}

/** @ingroup eliteapr_func_get_src_addr
  Gets the source address of the APR packet.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPkt   Pointer to the APR packet.

  @return
  elite_apr_addr_t -- Source address of the packet.

  @dependencies
  None.
 */
static inline elite_apr_addr_t elite_apr_if_get_src_addr( elite_apr_packet_t *pPkt)
{
   return pPkt->src_addr;
}

/** @ingroup eliteapr_func_get_client_tkn
  Gets the client token of the APR packet.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPkt   Pointer to the APR packet.

  @return
  Client token is returned in the packet.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_get_client_token( elite_apr_packet_t *pPkt)
{
   return pPkt->token;
}

/** @ingroup eliteapr_func_get_payld_ptr
  Gets the pointer to the APR packet payload.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPkt   Pointer to the APR packet.

  @return
  Pointer to the APR packet payload.

  @dependencies
  None.
 */
static inline void* elite_apr_if_get_payload_ptr( elite_apr_packet_t *pPkt)
{
   return APRV2_PKT_GET_PAYLOAD( void, pPkt);
}

/* =======================================================================
**                          Basic Packet Checks
** ======================================================================= */
/** @ingroup eliteapr_func_type_cmd
  Checks whether the message type of the APR packet is a command.

  @datatypes
  #elite_apr_packet_t

  @param[in] pPkt   Pointer to the APR packet.

  @return
  TRUE if the message type of the APR packet is a command. \n
  Otherwise, FALSE.

  @dependencies
  None.
 */
static inline uint8_t elite_apr_if_msg_type_is_cmd( elite_apr_packet_t *pPkt)
{
    return ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, pPkt->header ) ==
                APRV2_PKT_MSGTYPE_SEQCMD_V ) ||
              ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, pPkt->header ) ==
                APRV2_PKT_MSGTYPE_NSEQCMD_V ) );
}


/* =======================================================================
**                      Basic APR Packet Services
** ======================================================================= */
/** @ingroup eliteapr_func_alloc
  Allocates an APR packet with a specific packet size.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in]  handle       APR handle of the service that is making the request.
  @param[in]  alloc_size   Size of the packet.
  @param[out] ret_packet   Pointer to the returned APR packet.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t  elite_apr_if_alloc( elite_apr_handle_t handle, uint32_t alloc_size, elite_apr_packet_t **ret_packet)
{
   return __aprv2_cmd_alloc(  handle, APRV2_ALLOC_TYPE_RESPONSE, alloc_size, ret_packet);
}

/** @ingroup eliteapr_func_async_send
  Sends an APR packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] handle   APR handle of the service that is making the request.
  @param[in] packet   Pointer to the APR packet.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_async_send(elite_apr_handle_t handle, elite_apr_packet_t* packet)
{
   return __aprv2_cmd_async_send ( handle, packet );
}

/** @ingroup eliteapr_func_apr_if_cmd
  Forwards an allocated APR packet to a destination service.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] handle   APR handle of the service that is making the request.
  @param[in] packet   Pointer to the APR packet.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_cmd_fwd(elite_apr_handle_t handle, elite_apr_packet_t* packet)
{
   return __aprv2_cmd_forward ( handle, packet );
}

/** @ingroup eliteapr_func_free
  Frees an APR packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] handle   APR handle of the service that is making the request.
  @param[in] packet   Pointer to the APR packet.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_free( elite_apr_handle_t handle,  elite_apr_packet_t *packet)
{ return  __aprv2_cmd_free ( handle, packet ) ; }


/** @ingroup eliteapr_func_alloc_send_cmd
  Allocates and sends an APR packet with specified header information and
  payload. The packet is a sequential command type.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t

  @param[in] handle       APR handle of the service that is making the request.
  @param[in] src_addr     Source address.
  @param[in] src_port     Source port.
  @param[in] dst_addr     Destination address.
  @param[in] dst_port     Destination port.
  @param[in] token        Token to be put in the APR packet.
  @param[in] opcode       Opcode to be put in the APR packet.
  @param[in] payload      Pointer to the payload to be copied to the packet.
  @param[in] payload_size Payload size.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t  elite_apr_if_alloc_send_cmd (
                                            elite_apr_handle_t handle,
                                            elite_apr_addr_t src_addr,
                                            elite_apr_port_t src_port,
                                            elite_apr_addr_t dst_addr,
                                            elite_apr_port_t dst_port,
                                            uint32_t token,
                                            uint32_t opcode,
                                            void* payload,
                                            uint32_t payload_size
                                            )
{ return    __aprv2_cmd_alloc_send (
   handle,
   APRV2_PKT_MSGTYPE_SEQCMD_V,
   src_addr,
   src_port,
   dst_addr,
   dst_port,
   token,
   opcode,
   payload,
   payload_size
   );
}

/** @ingroup eliteapr_func_alloc_send_ack
  Allocates and sends an APR packet with specified header information and
  payload. The packet is a command response type.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t

  @param[in] handle       APR handle of the service that is making the request.
  @param[in] src_addr     Source address.
  @param[in] src_port     Source port.
  @param[in] dst_addr     Destination address.
  @param[in] dst_port     Destination port.
  @param[in] token        Token to be put in the APR packet.
  @param[in] opcode       Opcode to be put in the APR packet.
  @param[in] payload      Pointer to the payload to be copied to the packet.
  @param[in] payload_size Payload size.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t  elite_apr_if_alloc_send_ack (
                                             elite_apr_handle_t handle,
                                             elite_apr_addr_t src_addr,
                                             elite_apr_port_t src_port,
                                             elite_apr_addr_t dst_addr,
                                             elite_apr_port_t dst_port,
                                             uint32_t token,
                                             uint32_t opcode,
                                             void* payload,
                                             uint32_t payload_size
                                             )
{ return    __aprv2_cmd_alloc_send (
   handle,
   APRV2_PKT_MSGTYPE_CMDRSP_V,
   src_addr,
   src_port,
   dst_addr,
   dst_port,
   token,
   opcode,
   payload,
   payload_size
   );
}

/** @ingroup eliteapr_func_alloc_send_evt
  Allocates and sends an APR packet with specified header information and
  payload. The packet is an event type.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t

  @param[in] handle       APR handle of the service that is making the request.
  @param[in] src_addr     Source address.
  @param[in] src_port     Source port.
  @param[in] dst_addr     Destination address.
  @param[in] dst_port     Destination port.
  @param[in] token        Token to be put in the APR packet.
  @param[in] opcode       Opcode to be put in the APR packet.
  @param[in] payload      Pointer to the payload to be copied to the packet.
  @param[in] payload_size Payload size.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t  elite_apr_if_alloc_send_event (
                                                elite_apr_handle_t handle,
                                                elite_apr_addr_t src_addr,
                                                elite_apr_port_t src_port,
                                                elite_apr_addr_t dst_addr,
                                                elite_apr_port_t dst_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
)
{ return    __aprv2_cmd_alloc_send (
   handle,
   APRV2_PKT_MSGTYPE_EVENT_V,
   src_addr,
   src_port,
   dst_addr,
   dst_port,
   token,
   opcode,
   payload,
   payload_size
   );
}

/** @ingroup eliteapr_func_end_cmd
  Sends an APRV2_IBASIC_RESP_RESULT response to a specified APR packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] handle   APR handle of the service that is making the request.
  @param[in] packet   Pointer to the APR packet that is to be acknowledged.
  @param[in] status   Status of the acknowledgment.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline uint32_t elite_apr_if_end_cmd( elite_apr_handle_t handle,
                                      elite_apr_packet_t *packet,
                                      uint32_t status)
{
   return __aprv2_cmd_end_command( handle, packet, status);
}

/** @ingroup eliteapr_func_send_evt_accept
  Allocates and sends a new APR message back to a client, indicating that the
  incoming command is accepted. The incoming packet is not freed.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_packet_t

  @param[in] handle   APR handle of the service that is making the request.
  @param[in] pPkt     Pointer to the APR packet that is to be acknowledged.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_send_event_accepted( elite_apr_handle_t handle, elite_apr_packet_t *pPkt)
{
    aprv2_ibasic_evt_accepted_t  basic_rsp;

    basic_rsp.opcode = pPkt->opcode;
    return __aprv2_cmd_alloc_send( handle,
                                   APRV2_PKT_MSGTYPE_EVENT_V,
                                   pPkt->dst_addr,
                                   pPkt->dst_port,
                                   pPkt->src_addr,
                                   pPkt->src_port,
                                   pPkt->token,
                                   APRV2_IBASIC_EVT_ACCEPTED,
                                   &basic_rsp,
                                   sizeof( basic_rsp ) );
}

/** @ingroup eliteapr_func_alloc_send_ack_res
  Allocates and sends a basic response message. The token and opcode
  are from the original packet. The incoming packet is not freed.

  For more information on this message, refer to
  @xrefcond{Q11,80-N1463-2,80-NA601-5}.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t

  @param[in] handle     APR handle of the service that is making the request.
  @param[in] src_addr   Source address.
  @param[in] src_port   Source port.
  @param[in] dst_addr   Destination address.
  @param[in] dst_port   Destination port.
  @param[in] token      Token of the APR packet to be acknowledged.
  @param[in] opcode     Opcode of the APR packet to be acknowledged.
  @param[in] status     Acknowledgment status.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t  elite_apr_if_alloc_send_ack_result (
                                             elite_apr_handle_t handle,
                                             elite_apr_addr_t src_addr,
                                             elite_apr_port_t src_port,
                                             elite_apr_addr_t dst_addr,
                                             elite_apr_port_t dst_port,
                                             uint32_t token,
                                             uint32_t opcode,
                                             uint32_t status
                                             )
{

 aprv2_ibasic_rsp_result_t     result;

 result.opcode = opcode;
 result.status = status;

 return    __aprv2_cmd_alloc_send (
   handle,
   APRV2_PKT_MSGTYPE_CMDRSP_V,
   src_addr,
   src_port,
   dst_addr,
   dst_port,
   token,
   APRV2_IBASIC_RSP_RESULT,
   &result,
   sizeof(aprv2_ibasic_rsp_result_t)
   );
}

/** @ingroup eliteapr_func_alloc_evt
  Allocates and populates an APR event message packet, but does not send the
  packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t \n
  #elite_apr_packet_t

  @param[in]  handle       APR handle of the service that is making the request.
  @param[in]  src_addr     Source address.
  @param[in]  src_port     Source port.
  @param[in]  dst_addr     Destination address.
  @param[in]  dst_port     Destination port.
  @param[in]  token        Token to be put in the APR packet.
  @param[in]  opcode       Opcode to be put in the APR packet.
  @param[in]  payload_size Payload size.
  @param[out] ret_packet   Double pointer to the APR event packet.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_alloc_event(
                                             elite_apr_handle_t handle,
                                             elite_apr_addr_t src_addr,
                                             elite_apr_port_t src_port,
                                             elite_apr_addr_t dst_addr,
                                             elite_apr_port_t dst_port,
                                             uint32_t token,
                                             uint32_t opcode,
                                             uint32_t payload_size,
                                             elite_apr_packet_t** ret_packet)
{
   return __aprv2_cmd_alloc_ext( handle,
                                 APRV2_PKT_MSGTYPE_EVENT_V,
                                 src_addr,
                                 src_port,
                                 dst_addr,
                                 dst_port,
                                 token,
                                 opcode,
                                 payload_size,
                                 ret_packet);
}

/** @ingroup eliteapr_func_alloc_cmd
  Allocates and populates an APR command message packet, but does not send the
  packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t \n
  #elite_apr_packet_t

  @param[in]  handle       APR handle of the service that is making the request.
  @param[in]  src_addr     Source address.
  @param[in]  src_port     Source port.
  @param[in]  dst_addr     Destination address.
  @param[in]  dst_port     Destination port.
  @param[in]  token        Token to be put in the APR packet.
  @param[in]  opcode       Opcode to be put in the APR packet.
  @param[in]  payload_size Payload size.
  @param[out] ret_packet   Double pointer to the APR packet being returned.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_alloc_cmd(
                                             elite_apr_handle_t handle,
                                             elite_apr_addr_t src_addr,
                                             elite_apr_port_t src_port,
                                             elite_apr_addr_t dst_addr,
                                             elite_apr_port_t dst_port,
                                             uint32_t token,
                                             uint32_t opcode,
                                             uint32_t payload_size,
                                             elite_apr_packet_t** ret_packet)
{
   return __aprv2_cmd_alloc_ext( handle,
                                 APRV2_PKT_MSGTYPE_SEQCMD_V,
                                 src_addr,
                                 src_port,
                                 dst_addr,
                                 dst_port,
                                 token,
                                 opcode,
                                 payload_size,
                                 ret_packet);
}

/** @ingroup eliteapr_func_alloc_cmd_resp
  Allocates and populates an APR command response packet, but does not send the
  packet.

  @datatypes
  #elite_apr_handle_t \n
  #elite_apr_addr_t \n
  #elite_apr_port_t \n
  #elite_apr_packet_t

  @param[in]  handle       APR handle of the service that is making the request.
  @param[in]  src_addr     Source address.
  @param[in]  src_port     Source port.
  @param[in]  dst_addr     Destination address.
  @param[in]  dst_port     Destination port.
  @param[in]  token        Token to be put in the APR packet.
  @param[in]  opcode       Opcode to be put in the APR packet.
  @param[in]  payload_size Payload size.
  @param[out] ret_packet   Double pointer to the APR packet being returned.

  @return
  Success or failure code.

  @dependencies
  None.
 */
static inline int32_t elite_apr_if_alloc_cmd_rsp(
                                             elite_apr_handle_t handle,
                                             elite_apr_addr_t src_addr,
                                             elite_apr_port_t src_port,
                                             elite_apr_addr_t dst_addr,
                                             elite_apr_port_t dst_port,
                                             uint32_t token,
                                             uint32_t opcode,
                                             uint32_t payload_size,
                                             elite_apr_packet_t** ret_packet)
{
   return __aprv2_cmd_alloc_ext( handle,
                                 APRV2_PKT_MSGTYPE_CMDRSP_V,
                                 src_addr,
                                 src_port,
                                 dst_addr,
                                 dst_port,
                                 token,
                                 opcode,
                                 payload_size,
                                 ret_packet);
}

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEAPRIF_H

