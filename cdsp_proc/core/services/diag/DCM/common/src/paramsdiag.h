#ifndef PARAMSDIAG_H
#define PARAMSDIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Diagnostics Packet Processing Routines
                  for debug parameters

General Description
  Diagnostic packet processing routines for debug parameters

Copyright (c) 2005,2007,2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                             Edit History

  $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/paramsdiag.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/13   xy      Removed code under FEATURE_WINCE  
08/20/09   sg      Added support for command PARAMSDIAG_DIAG_STATUS_F
02/06/09   ps	     Changes to support wrapping of response Id count after 0xFFFF & Return wrap count.
04/23/07   as      Enabled pragma pack support for WINCE targets
01/07/05   as      Created file.
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "qw.h"




/*==========================================================================
  
                  SUBSYSTEM PACKET CODE DEFINITIONS
   
============================================================================*/

#define PARAMSDIAG_UPTIME_F                         0
#define PARAMSDIAG_WRAPTID_STATUS_F					1  /* Wrap Flag staus   */
#define PARAMSDIAG_WRAP_RSPID_F						2  /* Wrap response Id count after 0xFFFF & Return wrap count   */
#define PARAMSDIAG_DIAG_STATUS_F					3


/*===========================================================================

FUNCTION PARAMSDIAG_CLK_UPTIME
DESCRIPTION
  This packet gives the time elapsed in milliseconds, since the last reset.

===========================================================================*/
typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
}  paramsdiag_clk_uptime_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  qword uptime;
} paramsdiag_clk_uptime_rsp_type;

/*===========================================================================

PACKET   PARAMSDIAG_WRAP_RSPID_F
PURPOSE  Sent by DM to request Wrap and return delayed response id count after DIAGPKT_MAX_DELAYED_RSP (0xFFFF)

===========================================================================*/

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
}  paramsdiag_wrap_rspid_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 wrapcount;            /* Rsp Id wrap count */
} paramsdiag_wrap_rspid_rsp_type;

/*===========================================================================

PACKET   PARAMSDIAG_WRAPTID_STATUS_F
PURPOSE  Sent by DM to query Wrap Flag status 

===========================================================================*/

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
}  paramsdiag_wraptid_status_req_type;

typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint16 wrapflag;
  uint16 pad;
} paramsdiag_wraptid_status_rsp_type;


/*===========================================================================

PACKET    PARAMSDIAG_DIAG_STATUS_F
DESCRIPTION
  Sent by DM to query whether the RF mode is in diag mode.

===========================================================================*/
typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 proc_id;                    /* Processor ID To identify which processor has sent the request*/
  uint32 reserved;                   /* Reserved for future Use*/
}  paramsdiag_diag_status_req_type;


typedef struct {
  diagpkt_subsys_header_type header; /* Sub System header */
  uint32 proc_id;                    /* Processor ID To identify which processor has sent the response*/
  uint32 status;                     
  uint32 reserved;                   /*Reserved For Future Use*/
} paramsdiag_diag_status_rsp_type;

/*===========================================================================

FUNCTION PARAMSDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/

void paramsdiag_init (void);



#endif /* PARAMSDIAG_H */
