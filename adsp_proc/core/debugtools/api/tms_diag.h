#ifndef TMS_DIAGPKT_H
#define TMS_DIAGPKT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Defnitions for diagpkts used by various TMS modules

GENERAL DESCRIPTION
  Common internal defniitions for diag command handling by TMS 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2014-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1/debugtools/api/tms_diag.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/04/14   dav     File created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "diagpkt.h"
#include "diagcmd.h"
#include <stdint.h>
#include "DALSys.h"
#include "DALPropDef.h"
#include "tms_utils_msg.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define TMS_DIAG_TBLSIZE 3

// DO NOT ARBITRARILY RESIZE THIS ENUMERATION : LEGACY OPERATION REQUIRES THAT
// UNTIL DIAG_F3_TRACE IS IT'S OWN DISTINCT SUBSYSTEM, ITS COMMANDS ARE INTERLEAVED
// WITH THOSE OF ERR.
typedef enum
{
  TMS_DIAGPKT_DEPRECATED_0, 
  TMS_DIAGPKT_DEPRECATED_1,
  TMS_DIAGPKT_DEPRECATED_2,   /* Delete type trace file          */
  TMS_DIAGPKT_ERR_CORE_DUMP,
  TMS_DIAGPKT_DEPRECATED_3,   /* Get number of type saved        */
  TMS_DIAGPKT_DEPRECATED_4,   /* Get next type saved             */
  TMS_DIAGPKT_DEPRECATED_5,   /* Get names of all trace files       */
  TMS_DIAGPKT_DEPRECATED_6,   /* Get number of type saved from file */
  TMS_DIAGPKT_DEPRECATED_7,   /* Get next type saved from file      */
  TMS_DIAGPKT_DEPRECATED_8,   /* Delete specified trace file        */
  TMS_DIAGPKT_DEPRECATED_9,
  TMS_DIAGPKT_SYS_M_SHUTDOWN_DELAY, /* Delay shutdown completion to allow diag additional time to flush data*/
  TMS_DIAGPKT_SYS_M_RESTART,
}CMD;

diagpkt_user_table_entry_type tms_diag_build_table_entry(CMD cmd, void* cb);
diagpkt_user_table_entry_type tms_diag_build_table_entry_uimg(CMD cmd, void* cb);

void tms_diag_send_generic_response(void *req);
void tms_diag_send_error_response(void * req, uint16 pkt_len);

#endif //TMS_DIAGPKT_H
