/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
        
                                DIAG TEST SERVICES                  

General Description
This file contains function handlers for loopback and diag test services which 
is used to test events, logs, F3s and command request response in user space. 

Copyright (c) 2010, 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History 

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/platform/adsp/src/diagq6test.c#1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/22/13   sr      Removed PACK from pointer references 
04/09/10   sg      Created Initial Version
===========================================================================*/

#if defined CUST_H
#include "customer.h"
#endif
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "memheap.h"
#include "diag.h"
#include "log_codes.h"
#include "event.h"
#include "msg.h"
#include "log.h"
#include "qube.h"

 #define DIAG_Q6_LOOPBACK_F 0
 #define DIAG_Q6_TEST_SERVICES 1
 #define DIAG_SUBSYS_Q6_TEST 68

 typedef struct
 {
   log_hdr_type hdr;    /* Log header (length, code, timestamp) */
   uint32 log_payload;
 } q6_log;


 
 /*===========================================================================

                    DIAG COMMAND FUNCTION DEFINITIONS

 ===========================================================================*/

 
 /*===========================================================================

 FUNCTION diagq6_loopback
 DESCRIPTION

 ===========================================================================*/
 void * diagq6_loopback (
   void * req_pkt,
   uint16 pkt_len
 )
 {
   void * rsp = NULL;

   /* Allocate the same length as the request. */
   rsp = diagpkt_alloc (DIAG_Q6_LOOPBACK_F, pkt_len);

   if (rsp != NULL) {
     memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
   }

   return (rsp);

 } /* diagq6_loopback */

 typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint32 iter_count;
} diagq6_test_services_req_type;


typedef PACK(struct)
{
  diagpkt_subsys_header_type header;
  uint32 iter_count;
} diagq6_test_services_rsp_type;

 /*===========================================================================

 FUNCTION diagq6_test_services
 DESCRIPTION

 ===========================================================================*/
 void * diagq6_test_services (
   void * req_pkt,
   uint16 pkt_len
 )
 {
   int i = 0;
   q6_log test_log;

   diagq6_test_services_req_type *req =
    (diagq6_test_services_req_type *) req_pkt;
  diagq6_test_services_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof( diagq6_test_services_rsp_type );

   char event_payload[32] = "DiagTestApp Event with Payload1";
   test_log.log_payload = 0x230987;
   q6_log* pLog_Test = NULL;
   uint32 iter = 0;

   /* Allocate the same length as the request. */
   rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_Q6_TEST, DIAG_Q6_TEST_SERVICES, rsp_len);

   if (rsp != NULL)
   {
//      memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
      rsp->iter_count = req->iter_count;
        iter = req->iter_count;
      diagpkt_commit(rsp);

 	for(i = 0; i < iter; i++)
 	{
       MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"****QDSP6 MSG_HIGH Iter %d \n****",i,0,0);
 	  MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW,"****QDSP6 MSG_LOW Iter %d \n****",i,0,0);
        event_report(EVENT_DIAG_STRESS_TEST_NO_PAYLOAD);
        event_report_payload (EVENT_DIAG_STRESS_TEST_WITH_PAYLOAD,sizeof(event_payload), event_payload);
        log_set_code(&test_log, LOG_DIAG_STRESS_TEST_C);
  	  log_set_length(&test_log, sizeof(q6_log));
  	  log_set_timestamp(&test_log);
  	  log_submit(&test_log);
  	  pLog_Test = (q6_log*)log_alloc(LOG_WMS_READ_C,sizeof(q6_log));
  	  log_commit(pLog_Test);
 	  //qtimer_sleep(10000);
 	}
     //memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
   }
   return (NULL);
 } /* diagq6_test_services */

 
 /*===========================================================================

                   DIAG COMMAND FUNCTION DISPATCH TABLE

 ===========================================================================*/

 static const diagpkt_user_table_entry_type diagq6test_tbl[] =
 {
   {DIAG_Q6_LOOPBACK_F, DIAG_Q6_LOOPBACK_F, diagq6_loopback},
   {DIAG_Q6_TEST_SERVICES,DIAG_Q6_TEST_SERVICES,diagq6_test_services}
 };


 
 /*===========================================================================

 FUNCTION PARAMSDIAG_INIT

 DESCRIPTION
   Register the diag packet function dispatch table.

 ===========================================================================*/

   void diagq6test_init (void)
   {
 	/* Could use the following registration without specifying the processor */
    // DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_Q6_TEST, diagq6test_tbl);

 	/* Could use the following registration to specify the correct processor */
 	DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_QDSP6_PROC, DIAG_SUBSYS_Q6_TEST, diagq6test_tbl);
   }

