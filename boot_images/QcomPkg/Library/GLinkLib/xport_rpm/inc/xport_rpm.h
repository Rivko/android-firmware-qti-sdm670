#ifndef XPORT_RPM_H
#define XPORT_RPM_H

/**
 * @file xport_rpm.h
 *
 * Internal definitions for RPM Glink transport
 */

/*==============================================================================
     Copyright (c) 2014 - 2015 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/xport_rpm/inc/xport_rpm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/21/15   bc      Removed priority related macros
12/09/14   db      Added macros to set and get xport priority
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_transport_if.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/* Version request command */
#define XPORT_RPM_CMD_VERSION_REQ       0x0
/* Version ACK command */
#define XPORT_RPM_CMD_VERSION_ACK       0x1
/* Open channel command */
#define XPORT_RPM_CMD_OPEN_CHANNEL      0x2
/* Close channel command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL     0x3
/* Open channel ACK command */
#define XPORT_RPM_CMD_OPEN_CHANNEL_ACK  0x4
/* Tx data command */
#define XPORT_RPM_CMD_TX_DATA           0x9
/* Close channel ACK command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL_ACK 0xB
/* Tx signals command */
#define XPORT_RPM_CMD_TX_SIGNALS        0xF


#define XPORT_RPM_GET_CMD_ID(c)      ((c) & 0xFFFF)
#define XPORT_RPM_SET_CMD_ID(cmd_id) ((cmd_id) & 0xFFFF)

#define XPORT_RPM_GET_CHANNEL_ID(c)   (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_CHANNEL_ID(cid) (((cid) << 16) & 0xFFFF0000)

#define XPORT_RPM_GET_VERSION(c) (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_VERSION(v) (((v) << 16) & 0xFFFF0000)


/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

#endif //XPORT_RPM_H

