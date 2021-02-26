/*=============================================================================

FILE:         qdss_tmc.c

DESCRIPTION:  This file has the implementation for all TMC diag commands

================================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/
#include "qdss_control_priv.h"
#include "qdss_control.h"


/*-------------------------------------------------------------------------*/

/**
  @brief Handler for QDSS_TMC_READ_START diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_start_handler(qdss_tmc_read_start_req *pReq,
                                qdss_tmc_read_start_rsp *pRsp)
{
      return DalTMC_Flush(qdss.hTMC,TMC_FLUSH_STOP);
}



/**
  @brief Handler for QDSS_TMC_READ diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_handler(qdss_tmc_read_req *pReq,
                          qdss_tmc_read_rsp *pRsp)
{
   int nErr = QDSS_CONTROL_SUCCESS;
   uint32 len = pReq->len;
   uint32 rsp_len = 0;

   if (len > QDSS_TMC_READ_BLOCK_SIZE) {
      len = QDSS_TMC_READ_BLOCK_SIZE;
   }

   TRY(nErr,DalTMC_ReadTrace(qdss.hTMC,
                             pRsp->data,
                             len,
                             &rsp_len));
   pRsp->len = rsp_len;

   CATCH(nErr) {}


   return nErr;
}


/**
  @brief Handler for QDSS_TMC_READ_END diag command

  @return 0 if successful, error code otherwise
 */
static int qdss_tmc_read_end_handler(qdss_tmc_read_end_req *pReq,
                                qdss_tmc_read_end_rsp *pRsp)
{
   int nErr1,nErr2 = 0;

   nErr1 = qdss_trace_sink_disable();
   nErr2 = qdss_trace_sink_enable();

   return (nErr1|nErr2);
}


#define QDSS_HANDLE_TMC_CMD(cmd)                                        \
   if (pkt_len < (sizeof(qdss_tmc_req) + sizeof(cmd##_req) -1 )) {      \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);            \
   }                                                                    \
   else {                                                               \
      pRsp =  diagpkt_subsys_alloc                                      \
         (DIAG_SUBSYS_QDSS,                                             \
          pReq->hdr.subsysCmdCode,                                      \
          (sizeof(qdss_tmc_rsp)+sizeof(cmd##_rsp) -1) );                \
      if (NULL != pRsp) {                                               \
         nErr =  cmd##_handler((cmd##_req *)pReq->args,                 \
                               (cmd##_rsp *)(((qdss_tmc_rsp *)pRsp)->args)); \
      }                                                                 \
   }


/*-------------------------------------------------------------------------*/

/**
  @brief Handler for all TMC diag commandds


  @return pointer to a the DIAG response packet
 */
PACK(void *)  qdss_tmc_handler(qdss_tmc_req  *pReq,
                               int pkt_len)
{
   int nErr;
   PACK(void *) pRsp = NULL;

   if (0==qdss.hSync) {
      return diagpkt_err_rsp(QDSS_CONTROL_BAD_STATE, pReq, pkt_len);
   }

   DALSYS_SyncEnter(qdss.hSync);
   TRY(nErr,qdss_ensure_hw_ready());

   switch(pReq->id) {
   case QDSS_TMC_READ_START:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read_start);
      break;

   case QDSS_TMC_READ:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read)
      break;

   case QDSS_TMC_READ_END:
      QDSS_HANDLE_TMC_CMD(qdss_tmc_read_end)
      break;

   default:
      nErr=QDSS_CONTROL_ERROR;
   }

   CATCH(nErr){ }


   if  (NULL == pRsp) {
      pRsp = diagpkt_err_rsp(nErr, pReq, pkt_len);
   }
   else {
      ((qdss_tmc_rsp*)pRsp)->id = pReq->id;
      ((qdss_tmc_rsp *)pRsp)->result = nErr;
   }
   DALSYS_SyncLeave(qdss.hSync);
   return pRsp;

}

