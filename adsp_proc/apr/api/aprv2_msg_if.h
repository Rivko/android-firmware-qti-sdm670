#ifndef __APRV2_MSG_IF_H__
#define __APRV2_MSG_IF_H__

/** @file aprv2_msg_if.h
*/

/*===========================================================================
Copyright (c) 2009-2012 QUALCOMM Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - QUALCOMM Technologies, Inc.
======================================================================== */

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/apr.common/2.0/api/aprv2_msg_if.h#1 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2012/04/16 sw      (Tech Pubs) Updated for new Doxygen version; reorganized
                   organization of PDF.
2011/11/18 sw      (Tech Pubs) Added Doxygen comments for Interface Spec.
===========================================================================*/

#include "apr_comdef.h"

/*****************************************************************************
 *  Core Messages                                                            *
 ****************************************************************************/

/** @addtogroup msg_cmdrsp_result
@{ */
/** Mandatory command completion response message that all services (clients
    and servers) must handle in addition to any custom command completion
    response messages expected.

    @proto
    @code
struct aprv2_ibasic_rsp_result_t
{
  uint32_t opcode;
  uint32_t status;
};
    @endcode
*/
#define APRV2_IBASIC_RSP_RESULT ( 0x000110E8 )

typedef struct aprv2_ibasic_rsp_result_t aprv2_ibasic_rsp_result_t;

#include "apr_pack_begin.h"

/** Payload of the APRV2_IBASIC_RSP_RESULT command response message.
*/
struct aprv2_ibasic_rsp_result_t
{
  uint32_t opcode;    /**< Command operation code that completed. */
  uint32_t status;    /**< Completion status (see the error codes in Section
                           @xref{dox:AprCoreStatusErrorcodes}). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_addtogroup msg_cmdrsp_result */


/** @addtogroup msg_event_accepted
@{ */
/** Standard event message that indicates a command was accepted. The
    generation and processing of this event is optional. Clients that do not
    understand this event must drop it by freeing the received packet.

    @proto
    @code
struct aprv2_ibasic_evt_accepted_t
{
  uint32_t opcode;
};
    @endcode
*/
#define APRV2_IBASIC_EVT_ACCEPTED ( 0x000100BE )

typedef struct aprv2_ibasic_evt_accepted_t aprv2_ibasic_evt_accepted_t;

#include "apr_pack_begin.h"

/** Payload of the APRV2_IBASIC_EVT_ACCEPTED message.
*/
struct aprv2_ibasic_evt_accepted_t
{
  uint32_t opcode;    /**< Command operation code that was accepted. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_addtogroup msg_event_accepted */


/** @ingroup msg_cmdrsp_create_session
  Reference session creation command for servers to develop their own variant.
  The handshaking using the src_addr, src_port, dst_addr, and dst_port fields
  is described here.

  This command does not expect a payload.
  
  After this command is issued, the client must wait for the server to
  respond with an #APRV2_IBASIC_RSP_RESULT response. The client is required
  to do the following:
     
  - Put the client's address and port into the src_addr and src_port fields of
    the command message.
  - Put the server's address into the dst_addr field.
  - Put #APR_NULL_V in the dst_port field.
 
  The server's response message puts the server's address in the src_addr
  field and the server's nonzero session value into the src_port field. The
  client's address and port values are echoed back in the dst_addr and
  dst_port fields. The src_port remains as #APR_NULL_V when the server does
  not support sessions or fails to open a new session.
*/
#define APRV2_IBASIC_CMD_CREATE_SESSION ( 0x0001003B )

/** @ingroup msg_cmdrsp_destroy_session
  Standard command used to destroy an active session. The session to destroy
  is specified in the dst_port field of the packet header.
    
  There is no payload. An #APRV2_IBASIC_RSP_RESULT command response is
  expected to be returned by the server after the destroy session is complete.
*/
#define APRV2_IBASIC_CMD_DESTROY_SESSION ( 0x0001003C )

#endif /* __APRV2_MSG_IF_H__ */

