#ifndef __APRV2_API_H__
#define __APRV2_API_H__

/** @file aprv2_api.h
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

  $Header: //components/rel/apr.common/2.0/api/aprv2_api.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

#include "apr_comdef.h"
#include "apr_api.h"
#include "aprv2_packet.h"


/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Allocates an uninitialized raw client-to-server command packet. */
#define APRV2_ALLOC_TYPE_COMMAND_RAW ( 0x000110E0 )

/* Allocates an uninitialized raw server-to-client response packet. */
#define APRV2_ALLOC_TYPE_RESPONSE_RAW ( 0x000110E5 )

/** @addtogroup aprv2_data_types
@{ */

/** Allocates a packet from the service's client-to-server command queue. The
    alloc_size field (in #aprv2_cmd_alloc_t) must contain only the payload
    size.
*/
#define APRV2_ALLOC_TYPE_COMMAND ( 0x000100C7 )

/** Allocates a packet from the service's server-to-client response queue. The
    alloc_size field (in aprv2_cmd_alloc_t) must contain only the payload
    size. Clients must allocate from this queue for sending command 
    responses.
*/
#define APRV2_ALLOC_TYPE_RESPONSE ( 0x000100C8 )


/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

/** Message dispatch function prototype.

  @proto
  @code
typedef int32_t ( *aprv2_dispatch_fn_t ) ( aprv2_packet_t* packet,
                                           void* dispatch_data );
  @endcode

  @param[in] packet        Pointer to the incoming message (packet) to receive.
                           The packet is guaranteed to have a non-NULL value.
  @param[in] dispatch_data Client-supplied data pointer that the service
                           provided at registration time.

  @return
  When successful, #APR_EOK indicates that the callee has taken ownership of
  the packet.
    @par
  When not successful (there is an error), the packet ownership is returned to
  the caller.
*/
typedef int32_t ( *aprv2_dispatch_fn_t ) ( aprv2_packet_t* packet,
                                           void* dispatch_data );

/** @} */ /* end_addtogroup aprv2_data_types */

/*****************************************************************************
 * Fundamental Controls                                                      *
 ****************************************************************************/

/** @addtogroup aprv2_reg_service
@{ */
/** Registers a service, by address, to the APRv2 service.
  
  This function exists for historical reasons. Use #APRV2_CMDID_REGISTER2.

  @proto
  @code
static inline int32_t __aprv2_cmd_register (
  uint32_t* ret_handle,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data
);
  @endcode

  @param[out] ret_handle    Returns the registered service handle.
  @param[in] addr           Service address to register.
  @param[in] dispatch_fn    ISR dispatch callback function to register. 
  @param[in] dispatch_data  Client-supplied data pointer for the ISR dispatch
                            callback function.

  @detdesc
  This function is called by services once during system bring-up or during
  runtime to register its presence to the APRv2 service. The client and server
  services must be registered with the APRv2 service before any messages can be
  passed between them.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_ids_domains.h"
#include "aprv2_ids_services.h"
#include "aprv2_api_inline.h"

static uint32_t my_handle;

static int32_t my_isr_dispatch_cb ( aprv2_packet_t* packet,
                                    void* dispatch_data );

int main ( void )
{
  int32_t rc;

  rc = __aprv2_cmd_register(
         &my_handle,
         ( ( APRV2_IDS_DOMAIN_ID_MODEM_V << 8 ) |
           APRV2_IDS_SERVICE_ID_TESTCLIENT_V ),
         my_isr_dispatch_cb,
         NULL
        );
   if ( rc )
   {
     printf( "Could not register with the APRv2 service.\n" );
   }
   return 0;
} 

static int32_t my_isr_dispatch_cb ( aprv2_packet_t* packet,
                                    void* dispatch_data )
{
   switch ( packet->opcode )
   {
   case SUBSYSTEM_IINTERFACE_CMD_FUNCTION:
     // Do ISR work otherwise queue to task.
     __aprv2_cmd_end_command( my_handle, packet, APR_EOK );
     break;

   case SUBSYSTEM_IINTERFACE_RSP_FUNCTION:
     // Notify command completion; response may contain payload.
     __aprv2_cmd_free( my_handle, packet );
     break;

   case APRV2_IBASIC_RSP_RESULT:
     // Notify command completion; response contains the status.
     __aprv2_cmd_free( my_handle, packet );
     break;

   default:
     if ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
            APRV2_PKT_MSGTYPE_SEQCMD_V ) ||
          ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
            APRV2_PKT_MSGTYPE_NSEQCMD_V ) )
     { // Reply unsupported command.
       __aprv2_cmd_end_command ( my_handle, packet, APR_EUNSUPPORTED );
     }
     else
     { // Drop unsupported events and command responses.
       __aprv2_cmd_free( my_handle, packet );
     }
     break;
   }
   return APR_EOK;
     // APR_EOK tells the caller that the packet was consumed (freed).
}
  @endcode  @newpage
*/
#define APRV2_CMDID_REGISTER ( 0x000110D5 )

typedef struct aprv2_cmd_register_t aprv2_cmd_register_t;

/** Payload of the #APRV2_CMDID_REGISTER command. */
struct aprv2_cmd_register_t
{
  uint32_t* ret_handle;
  /**< Returns the registered service handle. */

  uint16_t addr;
  /**< Service address to register. */

  aprv2_dispatch_fn_t dispatch_fn;
  /**< ISR dispatch callback function to register. */

  void* dispatch_data;
  /**< Client-supplied data pointer for the ISR dispatch callback function. */
};
/** @} */ /* end_addtogroup aprv2_reg_service */


/** @addtogroup aprv2_reg_service2
@{ */
/** Registers a service, by name or address, to the APRv2 service.

  This command is preferred over the #APRV2_CMDID_REGISTER command, which
  exists for historical reasons.

  @proto
  @code
static inline int32_t __aprv2_cmd_register2 (
  uint32_t* ret_handle,
  char_t* name,
  uint32_t name_size,
  uint16_t addr,
  aprv2_dispatch_fn_t dispatch_fn,
  void* dispatch_data,
  uint16_t* ret_addr
);
  @endcode

  @param[out] ret_handle     Returns the registered service handle.
  @param[in] rname           Service name string to register. A NULL name means
                             the service does not have a name.
  @param[in] rname_size      Service name string size in bytes.
  @param[in] raddr           Address to register. A zero value means to
                             autogenerate a free dynamic address. A nonzero
                             value means to directly use the statically
                             assigned address.
  @param[in] rdispatch_fn    Dispatch function to register.
  @param[in] rdispatch_data  Client-supplied data pointer for the dispatch
                             function.
  @param[out] rret_addr      Returns the actual registered address.

  @detdesc
  This function is called by services once during system bring-up or during
  runtime to register its presence to the APRv2 service. The client and server
  services must be registered with the APRv2 service before any messages can
  be passed between them.
    @par
  Dynamic services can register with the APRv2 service. The dynamic services
  set the addr field to zero, and the ret_addr field returns the assigned
  service address.
    @par
  This function allows registration by service name. Dynamic services
  must set the addr field to zero, and the ret_addr field returns the assigned
  service address. Static services can register with the addr field set to
  zero to automatically retrieve the address, or they can register with the
  addr field set to nonzero to override the default service name.
    @par
  @keep{6} Registration by service names gives customers the flexibility to
  remap addresses according to their deployment and debugging requirements.
  Future enhancements may include remote DNS query support so that a process
  domain's Service ID space can be more efficiently allocated at runtime rather
  than statically (permanently) assigned.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

static char_t my_dns[] = "qcom.domain.myaddress";
static uint16_t my_addr;
static uint32_t my_handle;

static int32_t my_isr_dispatch_cb ( aprv2_packet_t* packet,
                                    void* dispatch_data );

int main ( void )
{
   int32_t rc;

   rc = __aprv2_cmd_register2( &my_handle, my_dns, sizeof( my_dns ),
                               0, my_isr_dispatch_cb, NULL, &my_addr );
   if ( rc )
   {
     printf( "Could not register with the APRv2 service.\n" );
   }
   return 0;
}

static int32_t my_isr_dispatch_cb ( aprv2_packet_t* packet,
                                    void* dispatch_data )
{ 
  // Accept command; the usage is optional.
  __aprv2_cmd_accept_command( my_handle, packet );
 
  switch ( packet->opcode )
  {
  case SUBSYSTEM_IINTERFACE_CMD_FUNCTION:
    // Do ISR work otherwise queue to task.
    __aprv2_cmd_end_command( my_handle, packet, APR_EOK );
    break;

  case SUBSYSTEM_IINTERFACE_RSP_FUNCTION:
    // Notify command completion; response may contain payload.
    __aprv2_cmd_free( my_handle, packet );
    break;

  case APRV2_IBASIC_RSP_RESULT:
    // Notify command completion; response contains the status.
    __aprv2_cmd_free( my_handle, packet );
    break;

  default:
    if ( ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
           APRV2_PKT_MSGTYPE_SEQCMD_V ) ||
         ( APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header ) ==
           APRV2_PKT_MSGTYPE_NSEQCMD_V ) )
    { // Reply unsupported command.
      __aprv2_cmd_end_command ( my_handle, packet, APR_EUNSUPPORTED );
    }
    else
    { // Drop unsupported events and command responses.
      __aprv2_cmd_free( my_handle, packet );
    }
    break;
  }
  return APR_EOK;
    // APR_EOK tells the caller that the packet was consumed (freed).
}
  @endcode
*/
#define APRV2_CMDID_REGISTER2 ( 0x000100BD )

typedef struct aprv2_cmd_register2_t aprv2_cmd_register2_t;

/** Payload of the #APRV2_CMDID_REGISTER2 command. */
struct aprv2_cmd_register2_t
{
  uint32_t* ret_handle;
  /**< Returns the registered service handle. */

  char_t* name;
  /**< Service name string to register. A NULL name means the service does not
       have a name. */

  uint32_t name_size;
  /**< Service name string size in bytes. */

  uint16_t addr;
  /**< Address to register. A zero value means to auto-generate a free dynamic
       address. A nonzero value means to directly use the statically assigned
       address. */

  aprv2_dispatch_fn_t dispatch_fn;
  /**< Dispatch function to register. */

  void* dispatch_data;
  /**< Client-supplied data pointer for the dispatch function. */

  uint16_t* ret_addr;
  /**< Returns the actual registered address. */
};
/** @} */ /* end_addtogroup aprv2_reg_service2 */


/** @addtogroup aprv2_dereg_service
@{ */
/** Deregisters a service from the APRv2 service.

  @proto
  @code
static inline int32_t __aprv2_cmd_deregister (
  uint32_t handle
);
  @endcode

  @param[in] handle  Registered service handle.

  @detdesc
  This function is called by services once during system teardown or during
  runtime to deregister its presence from the APRv2 service.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

static char_t my_dns[] = "qcom.domain.myaddress";
static uint16_t my_addr;
static uint32_t my_handle;

static int32_t my_isr_dispatch_cb ( aprv2_packet_t* packet,
                                    void* dispatch_data );

int main ( void )
{
   int32_t rc;

   rc = __aprv2_cmd_register2( &my_handle, my_dns, sizeof( my_dns ),
                               0, my_isr_dispatch_cb, NULL, &my_addr );
   if ( rc )
   {
     printf( "Could not register with the APRv2 service.\n" );
   }
   ...
   rc = __aprv2_cmd_deregister( my_handle );
   if ( rc )
   {
     printf( "Could not de-register from the APRv2 service.\n" );
   }
   return 0;
}
  @endcode
*/
#define APRV2_CMDID_DEREGISTER ( 0x000110D6 )

typedef struct aprv2_cmd_deregister_t aprv2_cmd_deregister_t;

/** Payload of the #APRV2_CMDID_DEREGISTER command. */
struct aprv2_cmd_deregister_t
{
  uint32_t handle;    /**< Registered service handle. */
};
/** @} */ /* end_addtogroup aprv2_dereg_service */


/** @addtogroup aprv2_send_async_msg
@{ */
/** Sends an asynchronous message to other services.

  @proto
  @code
static inline int32_t __aprv2_cmd_async_send (
  uint32_t handle,
  aprv2_packet_t* packet
);
  @endcode

  @param[in] handle  Registered service handle.
  @param[in] packet  Packet (message) to send.

  @detdesc
  Calling #APRV2_CMDID_ALLOC or #APRV2_CMDID_ALLOC_EXT allocates free messages
  for sending. The caller can try to resend messages, or abort and free
  messages when delivery fails.
    @par
  This APRV2_CMDID_ASYNC_SEND function provides the caller with low-level
  control over the sending process. For general use, consider using a
  simplified helper function, such as #APRV2_CMDID_ALLOC_SEND.

  @consid
  The sender must always anticipate failures, even when this
  APRV2_CMDID_ASYNC_SEND function returns no errors. At a minimum, the
  #APRV2_IBASIC_RSP_RESULT response messages are to be checked for error
  statuses returned.
    @par
  The sender can locally abort remotely pending operations by implementing
  timeouts. The sender must still expect and handle receipt of response
  messages for those aborted operations.

  @return
  #APR_EOK when successful.

  @dependencies
  None. @newpage

  @par Code example
  @code
#include "aprv2_api_inline.h"

int32_t rc;
aprv2_packet_t* packet;
uint32_t payload_size;
subsystem_iinterface_cmd_function_t payload;

payload_size = sizeof( subsystem_iinterface_cmd_function_t );

// Allocate a free packet.
rc = __aprv2_cmd_alloc( my_handle, APRV2_ALLOC_TYPE_COMMAND,
                        payload_size, &packet );
if ( rc )
{
   return APR_ENORESOURCE;
}

// Fill in the packet details.
packet->header &=
   ~( APRV2_PKT_MSGTYPE_MASK | APRV2_PKT_PAYLOAD_SIZE_MASK );
packet->header |=
   APR_SET_FIELD( APRV2_PKT_MSGTYPE, APRV2_PKT_MSGTYPE_SEQCMD_V ) |
   APR_SET_FIELD( APRV2_PKT_PAYLOAD_SIZE, payload_size );
packet->src_addr = my_addr;
packet->src_port = my_port;
packet->dst_addr = some_destination_address;
packet->dst_port = some_destination_port;
packet->token = 0x12345678;
packet->opcode = SUBSYSTEM_IINTERFACE_CMD_FUNCTION;

// Fill in the payload.
payload.param1 = 1;
payload.param2 = 2;
memcpy( APRV2_PKT_GET_PAYLOAD( void, packet ), payload, payload_size );

// Send the packet.
rc = __aprv2_cmd_async_send( packet );
if ( rc )
{ // Free the packet when delivery fails.
  ( void ) __aprv2_cmd_free( packet );
  return rc;
}
  @endcode
*/
#define APRV2_CMDID_ASYNC_SEND ( 0x000110D7 )

typedef struct aprv2_cmd_async_send_t aprv2_cmd_async_send_t;

/** Payload of the #APRV2_CMDID_ASYNC_SEND command. */
struct aprv2_cmd_async_send_t
{
  uint32_t handle;           /**< Registered service handle. */
  aprv2_packet_t* packet;    /**< Packet to send. */
};
/** @} */ /* end_addtogroup aprv2_cmd_async_send_t */

/** @addtogroup aprv2_alloc_free_msg
@{ */
/** Allocates a free message for delivery.

  @proto
  @code
static inline int32_t __aprv2_cmd_alloc (
  uint32_t handle,
  uint32_t alloc_type,
  uint32_t alloc_size,
  aprv2_packet_t** ret_packet
);
  @endcode

  @param[in]  handle      Registered service handle.
  @param[in]  alloc_type  Packet type to allocate: #APRV2_ALLOC_TYPE_COMMAND
                          or #APRV2_ALLOC_TYPE_RESPONSE.
  @param[in]  alloc_size  Amount of memory required for allocation, in bytes.
  @param[out] ret_packet  Returns the pointer to the allocated packet.

  @detdesc
  This function allocates a packet from the indicated service's free packet
  queue, and it provides the caller with low-level control over the allocation
  process. For general use, consider using a simplified helper function, such
  as #APRV2_CMDID_ALLOC_EXT.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  See #APRV2_CMDID_ASYNC_SEND. @newpage
*/
#define APRV2_CMDID_ALLOC ( 0x000110D8 )

typedef struct aprv2_cmd_alloc_t aprv2_cmd_alloc_t;

/** Payload of the #APRV2_CMDID_ALLOC command.
*/
struct aprv2_cmd_alloc_t
{
  uint32_t handle;                /**< Registered service handle. */
  uint32_t alloc_type;            /**< Packet memory allocation type:
                                       #APRV2_ALLOC_TYPE_COMMAND or
                                       #APRV2_ALLOC_TYPE_RESPONSE. */
  uint32_t alloc_size;            /**< Amount of memory required, in bytes. */
  aprv2_packet_t** ret_packet;    /**< Return pointer to the allocated packet. */
};
/** @} */ /* end_addtogroup aprv2_alloc_free_msg */

/** @addtogroup aprv2_free_msg_from_queue
@{ */
/** Frees a packet from the indicated service's free packet queue.

  @proto
  @code
static inline int32_t __aprv2_cmd_free (
  uint32_t handle,
  aprv2_packet_t* packet
);
  @endcode

  @param[in] handle  Registered service handle.
  @param[in] packet  Packet to return.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  See #APRV2_CMDID_ASYNC_SEND.
*/
#define APRV2_CMDID_FREE ( 0x000110D9 )

typedef struct aprv2_cmd_free_t aprv2_cmd_free_t;

/** Payload of the APRV2_CMDID_FREE command.
*/
struct aprv2_cmd_free_t
{
  uint32_t handle;           /**< Registered service handle. */
  aprv2_packet_t* packet;    /**< Packet to return. */
};
/** @} */ /* end_addtogroup aprv2_free_msg_from_queue */

/*****************************************************************************
 * Service Controls                                                          *
 ****************************************************************************/

/** @addtogroup aprv2_query_address
@{ */
/** Returns the service address of an indicated service handle.

  @proto
  @code
static inline int32_t __aprv2_cmd_query_address (
  uint32_t handle,
  uint16_t* ret_addr
);
  @endcode

  @param[in]  handle    Registered service handle.
  @param[out] ret_addr  Return pointer to the service address.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

int32_t rc;
uint16_t my_addr;

rc = __aprv2_cmd_query_address( my_handle, &my_addr );
if ( rc )
{
   printf( "Could not retrieve the address from the handle.\n" );
}
  @endcode @newpage
*/
#define APRV2_CMDID_QUERY_ADDRESS ( 0x000110E9 )

typedef struct aprv2_cmd_query_address_t aprv2_cmd_query_address_t;

/** Payload of the #APRV2_CMDID_QUERY_ADDRESS command. */
struct aprv2_cmd_query_address_t
{
  uint32_t handle;       /**< Registered service handle. */
  uint16_t* ret_addr;    /**< Returns the service address. */
};
/** @} */ /* end_addtogroup aprv2_query_address */


/** @addtogroup aprv2_query_address_dns_lookup
@{ */
/** Returns the service address of an indicated service name string.

  @proto
  @code
static inline int32_t __aprv2_cmd_local_dns_lookup (
  char_t* name,
  uint32_t name_size,
  uint16_t* ret_addr
);
  @endcode

  @param[in] name         Name string of the service to look up.
  @param[in] name_size;   Size of the name string in bytes, including the
                          NULL character.
  @param[out] ret_addr;   Return pointer to the service address.

  @detdesc
  The service address is retrieved from an internal routing table from the
  local APRv2 service. For more information, #APRV2_CMDID_ASYNC_SEND.


  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

int32_t rc;
static char_t my_dns[] = "qcom.domain.myaddress";
static uint16_t my_addr;

rc = __aprv2_cmd_local_dns_lookup ( &my_dns, sizeof( my_dns ), &my_addr );
if ( rc )
{
   printf( "Could not retrieve the address from the given name.\n" );
}
  @endcode*/
#define APRV2_CMDID_LOCAL_DNS_LOOKUP ( 0x000110EA )

typedef struct aprv2_cmd_local_dns_lookup_t aprv2_cmd_local_dns_lookup_t;

/** Payload of the #APRV2_CMDID_LOCAL_DNS_LOOKUP command. */
struct aprv2_cmd_local_dns_lookup_t
{
  char_t* name;         /**< Name string of the service to look up. */
  uint32_t name_size;   /**< Size of the name string in bytes, including the
                             NULL character. */
  uint16_t* ret_addr;   /**< Return pointer to the service address. */
};
/**@} */ /* end_addtogroup aprv2_query_address_dns_lookup */

/*****************************************************************************
 * Utility Controls                                                          *
 ****************************************************************************/

/** @addtogroup aprv2_forward_async_msg
@{ */
/** Forwards an asynchronous message.

  @proto
  @code
static inline int32_t __aprv2_cmd_forward (
  uint32_t handle,
  aprv2_packet_t* packet
);
  @endcode

  @param[in] handle  Registered service handle.
  @param[in] packet  Packet to send.

  @detdesc
  This helper function provides a simple message forwarding mechanism.
  Undelivered messages are dropped (freed) and an error is returned. The
  caller is expected to handle return errors. See the example in
  #APRV2_CMDID_REGISTER for comparison.

  @consid
  The ownership of the packet is returned to the caller when
  APRV2_CMDID_FORWARD returns an #APR_EBADPARAM error.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

int32_t rc;
aprv2_packet_t* packet;
uint32_t payload_size;
subsystem_iinterface_cmd_function_t* payload;

payload_size = sizeof( subsystem_iinterface_cmd_function_t );

// Create a new packet.
rc = __aprv2_cmd_alloc_ext(
       my_handle,
       APRV2_PKT_MSGTYPE_SEQCMD_V,
       my_addr, my_port,
       some_destination_addr, some_destination_port,
       0x12345678,
       SUBSYSTEM_IINTERFACE_CMD_FUNCTION,
       payload_size,
       &packet
     );
if ( rc )
{
   return APR_EFAILED;
}

// Fill in the payload.
payload = APRV2_PKT_GET_PAYLOAD( subsystem_iinterface_cmd_function_t, packet );
payload->param1 = 1;
payload->param2 = 2;

// Send the packet.
rc = __aprv2_cmd_forward( my_handle, packet );
if ( rc )
{
   printf( "Could not forward the packet.\n" );
}
  @endcode
*/
#define APRV2_CMDID_FORWARD ( 0x000110DA )

typedef struct aprv2_cmd_forward_t aprv2_cmd_forward_t;

/** Payload of the #APRV2_CMDID_FORWARD command. */
struct aprv2_cmd_forward_t
{
  uint32_t handle;           /**< Registered service handle. */
  aprv2_packet_t* packet;    /**< Packet to send. */
};
/** @} */ /* end_addtogroup aprv2_forward_async_msg */


/** @addtogroup aprv2_alloc_preformatted_msg
@{ */
/** Allocates a preformatted free message for delivery.

  @proto
  @code
static inline int32_t __aprv2_cmd_alloc_ext (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  uint32_t payload_size,
  aprv2_packet_t** ret_packet
);
  @endcode

  @param[in]  handle       Registered service handle.
  @param[in]  msg_type     One of the following message types:
                           - #APRV2_PKT_MSGTYPE_EVENT_V
                           - #APRV2_PKT_MSGTYPE_CMDRSP_V
                           - #APRV2_PKT_MSGTYPE_SEQCMD_V
                           - #APRV2_PKT_MSGTYPE_NSEQCMD_V @tablebulletend
  @param[in]  src_addr     Source address of the sender.
  @param[in]  src_port     Source port of the sender.
  @param[in]  dst_addr     Destination address of the receiver.
  @param[in]  dst_port     Destination port of the receiver.
  @param[in]  token        Value attached by the sender to determine when
                           command messages have been processed by the receiver
                           after having received response messages.
  @param[in]  opcode       Defines both the action and the payload structure to
                           the receiver.
  @param[in]  payload_size Actual number of bytes needed for the payload.
  @param[out] ret_packet   Return pointer to the allocated packet.

  @detdesc
  This helper function partially creates a message for delivery. The message
  allocation and initialization is performed by this function, but the message
  payload is dependent on the caller to fill in. The two-step process allows
  the caller to avoid multiple memcpy() operations on the message payload.

  @consid
  The helper function allocates from the indicated service's free packet pool
  based on the msg_type.
    @par
  - For CMDRSP messages, #APRV2_ALLOC_TYPE_RESPONSE is used.
  - For EVENT, SEQCMD, and NSEQCMD messages, #APRV2_ALLOC_TYPE_COMMAND is used.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  See #APRV2_CMDID_FORWARD.
*/
#define APRV2_CMDID_ALLOC_EXT ( 0x000110DB )

typedef struct aprv2_cmd_alloc_ext_t aprv2_cmd_alloc_ext_t;

/** Payload of the #APRV2_CMDID_ALLOC_EXT command. */
struct aprv2_cmd_alloc_ext_t
{
  uint32_t handle;
  /**< Registered service handle. */

  uint8_t msg_type;
  /**< One of the following message types:

       - #APRV2_PKT_MSGTYPE_EVENT_V
       - #APRV2_PKT_MSGTYPE_CMDRSP_V
       - #APRV2_PKT_MSGTYPE_SEQCMD_V
       - #APRV2_PKT_MSGTYPE_NSEQCMD_V @tablebulletend */

  uint16_t src_addr;
  /**< Source address of the sender. */

  uint16_t src_port;
  /**< Source port of the sender. */

  uint16_t dst_addr;
  /**< Destination address of the receiver. */

  uint16_t dst_port;
  /**< Destination port of the receiver. */

  uint32_t token;
  /**< Value attached by the sender to determine when command messages have
       been processed by the receiver after having received response
       messages. */

  uint32_t opcode;
  /**< Defines both the action and the payload structure to the receiver. */

  uint32_t payload_size;
  /**< Actual number of bytes needed for the payload. */

  aprv2_packet_t** ret_packet;
  /**< Returns a formatted free packet. */
};
/** @} */ /* end_addtogroup aprv2_alloc_preformatted_msg */


/** @addtogroup aprv2_alloc_send_msg
@{ */
/** Allocates and sends a formatted message.

  @proto
  @code
static inline int32_t __aprv2_cmd_alloc_send (
  uint32_t handle,
  uint8_t msg_type,
  uint16_t src_addr,
  uint16_t src_port,
  uint16_t dst_addr,
  uint16_t dst_port,
  uint32_t token,
  uint32_t opcode,
  void* payload,
  uint32_t payload_size
);
  @endcode

  @param[in] handle        Registered service handle.
  @param[in] msg_type      One of the following message types:
                           - #APRV2_PKT_MSGTYPE_EVENT_V
                           - #APRV2_PKT_MSGTYPE_CMDRSP_V
                           - #APRV2_PKT_MSGTYPE_SEQCMD_V
                           - #APRV2_PKT_MSGTYPE_NSEQCMD_V @tablebulletend
  @param[in] src_addr      Source address of the sender.
  @param[in] src_port      Source port of the sender.
  @param[in] dst_addr      Destination address of the receiver.
  @param[in] dst_port      Destination port of the receiver.
  @param[in] token         Value attached by the sender to determine when
                           command messages have been processed by the
                           receiver after having received response messages.
  @param[in] opcode        Defines both the action and the payload structure
                           to the receiver.
  @param[in] payload       Payload to send.
  @param[in] payload_size  Actual number of bytes needed for the payload.

  @detdesc
  This helper function fully creates and sends a message. The message
  allocation and initialization is performed by this function, and the message
  payload is supplied up front by the caller. See the examples in
  #APRV2_CMDID_ASYNC_SEND and #APRV2_CMDID_FORWARD for comparison.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  @code
#include "aprv2_api_inline.h"

int32_t rc;
uint32_t payload_size;
subsystem_iinterface_cmd_function_t payload;

payload_size = sizeof( subsystem_iinterface_cmd_function_t );

// Fill in the payload.
payload.param1 = 1;
payload.param2 = 2;

// Create and send a packet.
rc = __aprv2_cmd_alloc_send(
       my_handle,
       APRV2_PKT_MSGTYPE_SEQCMD_V,
       my_addr, my_port,
       some_destination_address, some_destination_port,
       0x12345678,
       SUBSYSTEM_IINTERFACE_CMD_FUNCTION,
       &payload,
       payload_size
     );
if ( rc )
{
   return APR_EFAILED;
}
  @endcode
*/
#define APRV2_CMDID_ALLOC_SEND ( 0x000110DC )

typedef struct aprv2_cmd_alloc_send_t aprv2_cmd_alloc_send_t;

/** Payload of the #APRV2_CMDID_ALLOC_SEND command. */
struct aprv2_cmd_alloc_send_t
{
  uint32_t handle;
  /**< Registered service handle. */

  uint8_t msg_type;
  /**< One of the following message types:

       - #APRV2_PKT_MSGTYPE_EVENT_V
       - #APRV2_PKT_MSGTYPE_CMDRSP_V
       - #APRV2_PKT_MSGTYPE_SEQCMD_V
       - #APRV2_PKT_MSGTYPE_NSEQCMD_V @tablebulletend */

  uint16_t src_addr;
  /**< Source address of the sender. */

  uint16_t src_port;
  /**< Source port of the sender. */

  uint16_t dst_addr;
  /**< Destination address of the receiver. */

  uint16_t dst_port;
  /**< Destination address of the receiver. */

  uint32_t token;
  /**< Value attached by the sender to determine when command messages have
       been processed by the receiver after having received response
       messages. */

  uint32_t opcode;
  /**< Operation code defines both the action and the payload structure to the
       receiver. */

  void* payload;
  /**< Payload to send. */

  uint32_t payload_size;
  /**< Actual number of bytes needed for the payload. */
};
/** @} */ /* end_addtogroup aprv2_alloc_send_msg */


/** @addtogroup aprv2_accept_cmd
@{ */
/** Accepts a command message by replying with an #APRV2_IBASIC_EVT_ACCEPTED
    message to the sender.

  @proto
  @code
static inline int32_t __aprv2_cmd_accept_command (
  uint32_t handle,
  aprv2_packet_t* packet
);
  @endcode

  @param[in] handle  Registered service handle.
  @param[in] packet  Command packet to accept.

  @detdesc  
  The same procedure can be performed manually by swapping the source and
  destination fields, and then inserting an #APRV2_IBASIC_EVT_ACCEPTED payload.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  See #APRV2_CMDID_REGISTER2.
*/
#define APRV2_CMDID_ACCEPT_COMMAND ( 0x000112C7 )

typedef struct aprv2_cmd_accept_command_t aprv2_cmd_accept_command_t;

/** Payload of the #APRV2_CMDID_ACCEPT_COMMAND command. */
struct aprv2_cmd_accept_command_t
{
  uint32_t handle;          /**< Registered service handle. */
  aprv2_packet_t* packet;   /**< Command packet to accept. */
};
/** @} */ /* end_addtogroup aprv2_accept_cmd */

/** @addtogroup aprv2_send_cmd_end_resp
@{ */
/** Completes a command message by replying with an #APRV2_IBASIC_RSP_RESULT
    command response message to the sender. The indicated packet is then freed.

  @proto
  @code
static inline int32_t __aprv2_cmd_end_command (
  uint32_t handle,
  aprv2_packet_t* packet,
  uint32_t status
);
  @endcode

  @param[in] handle  Registered service handle.
  @param[in] packet  Command message to complete.
  @param[in] status  Completion/error status (see Section
                     @xref{dox:AprCoreStatusErrorcodes} for available values).
  
  @detdesc
  The same procedure can be done manually by swapping the source and
  destination fields and then inserting an #APRV2_IBASIC_RSP_RESULT command
  response payload.

  @return
  #APR_EOK when successful.

  @dependencies
  None.

  @par Code example
  See #APRV2_CMDID_REGISTER2. @newpage
*/
#define APRV2_CMDID_END_COMMAND ( 0x000110DD )

typedef struct aprv2_cmd_end_command_t aprv2_cmd_end_command_t;

/** Payload of the #APRV2_CMDID_END_COMMAND command. */
struct aprv2_cmd_end_command_t
{
  uint32_t handle;         /**< Registered service handle. */
  aprv2_packet_t* packet;  /**< Command packet to end. */
  uint32_t status;         /**< Completion status to respond to the client.
                                See Section @xref{dox:AprCoreStatusErrorcodes}
                                for a list of available values. */
};
/** @} */ /* end_addtogroup aprv2_send_cmd_end_resp */

#define APRV2_CMDID_HACK_INIT_IPC ( 0x12345678 )

#endif /* __APRV2_API_H__ */

