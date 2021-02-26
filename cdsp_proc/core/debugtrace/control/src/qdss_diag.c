/*=============================================================================

FILE:         qdss_diag.c

DESCRIPTION:

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_diag.h"

#define QDSS_HANDLE_DIAG_CMD(cmd)                              \
   if (pkt_len < sizeof(cmd##_req)) {                          \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);   \
   }                                                           \
   else {                                                      \
      pRsp =  diagpkt_subsys_alloc(DIAG_SUBSYS_QDSS,           \
                                   pHdr->subsysCmdCode,        \
                                   sizeof(cmd##_rsp));         \
      if (NULL != pRsp) {                                      \
         cmd##_handler((cmd##_req *)pReq,                      \
                       pkt_len,                                \
                       (cmd##_rsp *)pRsp,                      \
                       sizeof(cmd##_rsp));                     \
      }                                                        \
   }


PACK(void *) qdss_diag_pkt_handler(PACK(void *) pReq, uint16 pkt_len)
{
   qdss_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;

   if (NULL != pReq)    {
      pHdr = (qdss_diag_pkt_hdr *)pReq;

      switch (pHdr->subsysCmdCode & 0x0FF) {

      case QDSS_QUERY_STATUS:
         QDSS_HANDLE_DIAG_CMD(qdss_query_status);
         break;

      case QDSS_TRACE_SINK:
         QDSS_HANDLE_DIAG_CMD(qdss_trace_sink);
         break;

      case QDSS_FILTER_STM:
         QDSS_HANDLE_DIAG_CMD(qdss_filter_stm);
         break;

      case QDSS_FILTER_HWEVENT_ENABLE:
         QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents);
         break;

      default:
         pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
         break;
      }

      if (NULL != pRsp)
      {
         diagpkt_commit(pRsp);
         pRsp = NULL;
      }
   }
   return (pRsp);
}


PACK(void *) qdss_diag_pkt_handler_ext(PACK(void *) pReq, uint16 pkt_len)
{
   qdss_diag_pkt_hdr *pHdr;
   PACK(void *)pRsp = NULL;

   if (NULL != pReq)    {
      pHdr = (qdss_diag_pkt_hdr *)pReq;

      switch (pHdr->subsysCmdCode & 0x0FF) {

      case QDSS_TMC_CMD:
         pRsp=qdss_tmc_handler((qdss_tmc_req *)pReq,pkt_len);
         break;

      case QDSS_TPDM_CMD:
         pRsp=qdss_tpdm_handler((qdss_tpdm_req *)pReq,pkt_len);
         break;

      case QDSS_TPDA_CMD:
         pRsp=qdss_tpda_handler((qdss_tpda_req *)pReq,pkt_len);
         break;		

      case QDSS_FILTER_HWEVENT_CONFIGURE:
         QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents_configure);
         break;

      case QDSS_HWEVENT_SET_DETECT_MASK:
         QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents_set_detect_mask);
         break;

      case QDSS_HWEVENT_SET_TRIGGER_MASK:
         QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents_set_trigger_mask);
         break;

      case QDSS_TEST_CMD:
         QDSS_HANDLE_DIAG_CMD(qdss_test);
         break;

      default:
         pRsp = diagpkt_err_rsp(DIAG_BAD_CMD_F, pReq, pkt_len);
         break;
      }

      if (NULL != pRsp)
      {
         diagpkt_commit(pRsp);
         pRsp = NULL;
      }
   }
   return (pRsp);
}


// Diag packet service - callback tables.
static const diagpkt_user_table_entry_type qdss_diag_pkt_tbl[] =
   {
      { QDSS_DIAG_PROC_ID | QDSS_QUERY_STATUS,          // range: from
        QDSS_DIAG_PROC_ID | QDSS_TRACE_SINK,            // range: to
        qdss_diag_pkt_handler },                        // handler fcn

      { QDSS_DIAG_PROC_ID | QDSS_FILTER_STM,             // range: from
        QDSS_DIAG_PROC_ID | QDSS_FILTER_HWEVENT_ENABLE,  // range: to
        qdss_diag_pkt_handler }                          // handler fcn
   };


static const diagpkt_user_table_entry_type qdss_diag_ext_tbl[] =
{
   { QDSS_DIAG_PROC_ID | QDSS_TMC_CMD,       // range: from
     QDSS_DIAG_PROC_ID | QDSS_QUERY_CTI,     // range: to
     qdss_diag_pkt_handler_ext },            // handler fcn
     
   { QDSS_DIAG_PROC_ID | QDSS_TPDM_CMD,      // range: from
     QDSS_DIAG_PROC_ID | QDSS_TPDM_CMD,      // range: to
     qdss_diag_pkt_handler_ext },            // handler fcn

   { QDSS_DIAG_PROC_ID | QDSS_TPDA_CMD,      // range: from
     QDSS_DIAG_PROC_ID | QDSS_TPDA_CMD,      // range: to
     qdss_diag_pkt_handler_ext },            // handler fcn   

   { QDSS_DIAG_PROC_ID | QDSS_TEST_CMD,     // range: from
     QDSS_DIAG_PROC_ID | QDSS_AGENT_TEST,   // range: to
     qdss_diag_pkt_handler_ext }            // handler fcn

};


// Initialize interface with diag packet service
void qdss_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (
     (diagpkt_subsys_id_type) DIAG_SUBSYS_QDSS, qdss_diag_pkt_tbl);

  DIAGPKT_DISPATCH_TABLE_REGISTER (
     (diagpkt_subsys_id_type) DIAG_SUBSYS_QDSS, qdss_diag_ext_tbl);

  return;
}

