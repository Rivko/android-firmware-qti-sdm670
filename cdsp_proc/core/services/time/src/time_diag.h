#ifndef TIMEDIAG_H
#define TIMEDIAG_H
/*==========================================================================

               Diagnostic Packet Definitions for Time Services

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.

  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1.c4/services/time/src/time_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/09   pc      Created.

===========================================================================*/

#include "comdef.h"

#include "diagcmd.h"
#include "diagpkt.h"
#include "diagi.h"
#include "log_codes.h"
#include "msg.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */
#if defined(T_WINNT)
#pragma pack()
#endif

#define DIAG_TIME_DATA_SIZE 7

#define TIME_SERVICES_TEST_F  1


/*==========================================================================

PACKET   TIME_SERVICES_TEST_F

PURPOSE  Sent from the DM to the DMSS to request execution of a Time Service
         command. Usage: send_data 75 69 <SSCmd code> 0

============================================================================*/

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_TIME_DATA_SIZE]; /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}TIME_SERVICES_TEST_F_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_TIME_DATA_SIZE]; /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}TIME_SERVICES_TEST_F_rsp_type;

void time_services_test_init( void );

#endif /* TIMEDIAG_H */
