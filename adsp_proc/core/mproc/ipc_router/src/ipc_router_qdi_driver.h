#ifndef IPC_ROUTER_QDI_DRIVER_H_
#define IPC_ROUTER_QDI_DRIVER_H_
/*===========================================================================

                      I P C    R O U T E R    Q D I
                          H E A D E R    F I L E
  This header describes the QDI interface of the IPC Router driver in the
  Guest OS. This is a QuRT specific header file.

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_qdi_driver.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   aep     First revision
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "qurt_qdi.h"
#include "stdint.h"

/*===========================================================================
                  QDI Device Name
===========================================================================*/
#define IPC_ROUTER_QDI_DEV_NAME          "/dev/ipc_router"

/*===========================================================================
                  QDI METHOD TYPES
===========================================================================*/
#define IPC_ROUTER_QDI_INIT              (QDI_OPEN)
#define IPC_ROUTER_QDI_DEINIT            (QDI_CLOSE)

#define IPC_ROUTER_QDI_OPEN              (QDI_PRIVATE + 0)
#define IPC_ROUTER_QDI_CLOSE             (QDI_PRIVATE + 1)
#define IPC_ROUTER_QDI_SEND              (QDI_PRIVATE + 2)
#define IPC_ROUTER_QDI_SEND_CONF         (QDI_PRIVATE + 3)
#define IPC_ROUTER_QDI_REGISTER_SERVER   (QDI_PRIVATE + 4)
#define IPC_ROUTER_QDI_UNREGISTER_SERVER (QDI_PRIVATE + 5)
#define IPC_ROUTER_QDI_FIND_ALL_SERVERS  (QDI_PRIVATE + 6)
#define IPC_ROUTER_QDI_READ              (QDI_PRIVATE + 7)
#define IPC_ROUTER_QDI_EVENT_READ        (QDI_PRIVATE + 8)
#define IPC_ROUTER_QDI_LOG               (QDI_PRIVATE + 9)
#define IPC_ROUTER_QDI_SEND_TO_NAME      (QDI_PRIVATE + 10)
#define IPC_ROUTER_QDI_SET_PORT_OPTIONS  (QDI_PRIVATE + 11)
#define IPC_ROUTER_QDI_CONNECT           (QDI_PRIVATE + 12)
#define IPC_ROUTER_QDI_POLL              (QDI_PRIVATE + 13)
#define IPC_ROUTER_QDI_REGISTER_NAME     (QDI_PRIVATE + 14)
#define IPC_ROUTER_QDI_FIND_NAME         (QDI_PRIVATE + 15)
#define IPC_ROUTER_QDI_SEND_TO_SNAME     (QDI_PRIVATE + 16)

/*===========================================================================
                  ERROR CODES 
    (Negative to aid differentiating from ipc router core error codes)
===========================================================================*/
#define IPC_ROUTER_QDI_BADF_ERR          (-1)
#define IPC_ROUTER_QDI_INTERNAL_ERR      (-2)
#define IPC_ROUTER_QDI_PARAM_ERR         (-3)
#define IPC_ROUTER_QDI_NO_SPACE_ERR      (-4)
#define IPC_ROUTER_QDI_PERM_ERR          (-5)
#define IPC_ROUTER_QDI_NO_MEM_ERR        (-6)
#define IPC_ROUTER_QDI_NOT_SUPP_ERR      (-7)
#define IPC_ROUTER_QDI_TIMEOUT_ERR       (-8)
#define IPC_ROUTER_QDI_HUNGUP_ERR        (-9)
#define IPC_ROUTER_QDI_TERM_ERR          (-10)
#define IPC_ROUTER_QDI_ADDR_REQ_ERR      (-11)
#define IPC_ROUTER_QDI_NO_MSG_ERR        (-12)

/*===========================================================================
                        Read flags
===========================================================================*/
/* Do not block and wait for incoming packets if the RX queue is empty */
#define IPC_ROUTER_QDI_RDFLAG_DONTWAIT  0x1
/* Just return the length of the next packet do not actually read it */
#define IPC_ROUTER_QDI_RDFLAG_MSGPEEK   0x2
/* If the provided buffer is smaller than the required packet, read
 * as much as possible and return the length of the actual buffer */
#define IPC_ROUTER_QDI_RDFLAG_TRUNC     0x4

/*===========================================================================
                        Write flags
===========================================================================*/
/* Do not block and wait on flow control */
#define IPC_ROUTER_QDI_WRFLAG_DONTWAIT  0x1

/*===========================================================================
                    Port Allowed Operation flags
===========================================================================*/
#define DATA_ALLOWED     1
#define EVENT_ALLOWED    2
#define TRANSMIT_ALLOWED 4

/*===========================================================================
                    Event types for Poll
            (Same values used as QPOLL* in qsocket.h)
===========================================================================*/
#define IPC_QDI_POLLIN   1
#define IPC_QDI_POLLOUT  2
#define IPC_QDI_POLLHUP  4
#define IPC_QDI_POLLNVAL 8

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/* Port options */
typedef struct {
  /* Operations allowed on the port */
  int      allowed_valid;
  uint32_t allowed_flag;

  /* Timeout(ms) for a blocking send */
  int      sendtimeo_valid;
  uint32_t sendtimeo;

  /* Timeout(ms) for a blocking receive */
  int      rcvtimeo_valid;
  uint32_t rcvtimeo;

  /* Flag to turn the port into a control port */
  int      ctrl_port_valid;

  /* Limit events to specified type */
  int      type_pref_valid;
  uint32_t type_pref;

} ipc_router_qdi_port_options;

/* Poll structure, same as 'struct qpollfd' in qsocket.h  */
typedef struct {
  int fd;
  int events;
  int revents;
} ipc_router_qdi_poll_fd;

#endif
