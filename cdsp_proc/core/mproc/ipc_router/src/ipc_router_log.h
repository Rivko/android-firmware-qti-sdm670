#ifndef IPC_ROUTER_LOG_H
#define IPC_ROUTER_LOG_H
/*===========================================================================

                      I P C    R O U T E R    L O G
                          H E A D E R    F I L E

   This file describes the logging interface

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_log.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_database.h"
#include "ipc_router_protocol.h"
#include "ipc_router_packet.h"

typedef enum
{
  IPC_ROUTER_LOG_TX = 1,
  IPC_ROUTER_LOG_RX = 2
} ipc_router_log_dir;

/*===========================================================================
FUNCTION      ipc_router_log_packet

DESCRIPTION   Log the outgoing packet

ARGUMENTS     dir - direction
              xport - pointer to xport
              header - pointer to the header

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_packet
(
 ipc_router_log_dir dir,
 ipc_router_xport_type *xport,
 ipc_router_header_type *header
);

/*===========================================================================
FUNCTION      ipc_router_log_ctrl

DESCRIPTION   Log control packets

ARGUMENTS     dir - direction
              xport - pointer to xport
              ctrl - pointer to the control message  body

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_ctrl
(
  ipc_router_log_dir dir,
  ipc_router_xport_type *xport,
  ipc_router_control_msg_body *ctrl
);

/*===========================================================================
FUNCTION      ipc_router_log_error

DESCRIPTION   Log runtime error

ARGUMENTS     filename - filename
              line - line number

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_error
(
 char *filename,
 unsigned int line
 );

/*===========================================================================
FUNCTION      ipc_router_log_init

DESCRIPTION   Initialize ULOG for IPC Router.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_log_init(void);

#endif
