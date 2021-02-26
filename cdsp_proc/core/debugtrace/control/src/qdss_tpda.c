/*=============================================================================

FILE:         qdss_tpda.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qdss_tpda.h"
#include "qdss_control.h"
#include "qdss_control_priv.h"
#include "DDITPDA.h"
#include "DDITPDM.h"
#include "qdss_diag_cmd.h"

int qdss_tpda_id_to_name(uint8 tpda_id,const char **pname) 
{
   int nErr=DAL_SUCCESS;
   const char *name=NULL;

   if (QDSS_TPDA_ID_LPASS == tpda_id) {    // for lpass
      name="lpass";
   }
   else {
      nErr = DAL_ERROR;
   }
   *pname = name;

   return nErr;
}

#define QDSS_TPDA_ENTER                                     \
   int nErr=DAL_ERROR;                                      \
   const char *tpda_name=NULL;                              \
   do  {                                                    \
      if (0==qdss.hSync) {                                  \
         nErr=QDSS_CONTROL_BAD_STATE;                       \
         return nErr;                                       \
      }                                                     \
      DALSYS_SyncEnter(qdss.hSync);                         \
      TRY(nErr,qdss_ensure_hw_ready());                     \
      TRY(nErr,qdss_tpda_id_to_name(tpda_id,&tpda_name));   \
   } while (0) 


#define QDSS_TPDA_LEAVE                         \
   do {                                         \
      CATCH(nErr) {}                            \
      DALSYS_SyncLeave(qdss.hSync);             \
      return nErr;                              \
   } while (0)  


int qdss_tpda_set_param(uint8 tpda_id, uint8 param_id, uint32 val)
{
  QDSS_TPDA_ENTER;

  TRY(nErr,DAL_TPDA_SetParam(qdss.hTPDA,tpda_name,param_id,val));

  QDSS_TPDA_LEAVE;
}

int qdss_tpda_set_param_handler(qdss_tpda_set_param_req *pReq,
                                qdss_tpda_set_param_rsp *pRsp)
{
   return qdss_tpda_set_param(pReq->tpda_id,
                              pReq->param_id,
                              pReq->val);
}


#define QDSS_HANDLE_TPDA_CMD(cmd)                                       \
   if (pkt_len < (sizeof(qdss_tpda_req) + sizeof(cmd##_req) -1 )) {     \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);            \
   }                                                                    \
   else {                                                               \
      pRsp =  diagpkt_subsys_alloc                                      \
         (DIAG_SUBSYS_QDSS,                                             \
          pReq->hdr.subsysCmdCode,                                      \
          (sizeof(qdss_tpda_rsp)+sizeof(cmd##_rsp) -1) );               \
      if (NULL != pRsp) {                                               \
         nErr =  cmd##_handler((cmd##_req *)pReq->args,                 \
                               (cmd##_rsp *)(((qdss_tpda_rsp *)pRsp)->args)); \
      }                                                                 \
   }




/*-------------------------------------------------------------------------*/

/**
  @brief Handler for all TPDA diag commandds


  @return pointer to a the DIAG response packet
 */
PACK(void *)  qdss_tpda_handler(qdss_tpda_req  *pReq,
                               int pkt_len)
{
   int nErr = QDSS_CONTROL_ERROR;
   PACK(void *) pRsp = NULL;

   if (0==qdss.hSync) {
      return diagpkt_err_rsp(QDSS_CONTROL_BAD_STATE, pReq, pkt_len);
   }

   switch(pReq->id) {
   case  QDSS_TPDA_SET_PARAM:
      QDSS_HANDLE_TPDA_CMD(qdss_tpda_set_param);
      break;
   default:
      nErr=QDSS_CONTROL_ERROR;
      break;
   }

   
   if  (NULL == pRsp) {
      pRsp = diagpkt_err_rsp(nErr, pReq, pkt_len);
   }
   else {
      ((qdss_tpda_rsp*)pRsp)->id = pReq->id;
      ((qdss_tpda_rsp *)pRsp)->result = nErr;
   }
   return pRsp;

}


