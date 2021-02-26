#ifndef DIAG_LSM_PKT_I_H
#define DIAG_LSM_PKT_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                Internal Header File for Event Packet Req/Res Service Mapping

GENERAL DESCRIPTION
  
Copyright (c) 2007-2008 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/Diag_LSM_Pkt_i.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/02/08   mad    Moved declaration for diagpkt_LSM_process_request() here.
                  This is now an internal function.
02/27/08   JV     Created File
===========================================================================*/

/* Initializes legacy service mapping for Diag packet Req/Res service */
boolean Diag_LSM_Pkt_Init(void);

/* clean up packet Req/Res service before exiting legacy service mapping layer.
*/
boolean Diag_LSM_Pkt_DeInit(void);


void diagpkt_LSM_process_request (void *req_pkt, uint16 pkt_len,
								  diag_cmd_rsp rsp_func, void *rsp_func_param);

/*===========================================================================

FUNCTION DIAGPKT_FREE

DESCRIPTION
  This function free the packet allocated by diagpkt_alloc(), which doesn't
  need to 'commit' for sending as a response because it is merely a temporary
  processing packet.

===========================================================================*/
  void diagpkt_free(PACKED void *pkt);

#endif /* DIAG_LSM_EVENT_I_H */
