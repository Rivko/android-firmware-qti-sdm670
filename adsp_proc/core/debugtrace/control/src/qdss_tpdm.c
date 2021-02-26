/*=============================================================================

FILE:         qdss_tpdm.c

DESCRIPTION:  

================================================================================
            Copyright (c) 2016 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "qdss_tpdm.h"
#include "qdss_control.h"
#include "qdss_control_priv.h"
#include "qdss_diag.h"
#include "DDITPDA.h"
#include "DDITPDM.h"
#include "qdss_diag_cmd.h"

int qdss_tpdm_id_to_name(uint8 tpdm_id,const char **pname) 
{
   int nErr=DAL_SUCCESS;
   const char *name=NULL;

   if (QDSS_TPDM_ID_LPASS == tpdm_id) {  // for LPASS
      name="lpass";
   }
   else {
      nErr = DAL_ERROR;
   }
   *pname = name;

   return nErr;
}

#define QDSS_TPDM_ENTER                                     \
   int nErr=DAL_SUCCESS;                                    \
   const char *tpdm_name=NULL;                              \
   do  {                                                    \
      if (0==qdss.hSync) {                                  \
         nErr=QDSS_CONTROL_BAD_STATE;                       \
         return nErr;                                       \
      }                                                     \
      DALSYS_SyncEnter(qdss.hSync);                         \
      TRY(nErr,qdss_ensure_hw_ready());                     \
      TRY(nErr,qdss_tpdm_id_to_name(tpdm_id,&tpdm_name));   \
   } while (0) 


#define QDSS_TPDM_LEAVE                         \
   do {                                         \
      CATCH(nErr) {}                            \
      DALSYS_SyncLeave(qdss.hSync);             \
      return nErr;                              \
   } while (0)  


int qdss_tpdm_set(uint8 tpdm_id,uint8 state) 
{
  const char *tpda_name;
  
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_GetTPDA(qdss.hTPDM,tpdm_name,(const char **)&tpda_name));
  TRY(nErr,DAL_TPDA_EnableDisable(qdss.hTPDA,tpdm_name,tpda_name,state));

  QDSS_TPDM_LEAVE;
}

int qdss_tpdm_set_handler(qdss_tpdm_set_req *pReq,
                          qdss_tpdm_set_rsp *pRsp)
{
   return qdss_tpdm_set(pReq->tpdm_id,
                        pReq->state);
}


int qdss_tpdm_dataset_set(uint8 tpdm_id,uint8 enable_mask,uint8 state) 
{
   QDSS_TPDM_ENTER;
   
   TRY(nErr,DAL_TPDM_EnableDisable(qdss.hTPDM,tpdm_name,enable_mask,state));

   if (state) {
      //When enabling the data set enable the TPDM as well.
      TRY(nErr,qdss_tpdm_set(tpdm_id,state));
   }

   QDSS_TPDM_LEAVE;
}


int qdss_tpdm_dataset_set_handler(qdss_tpdm_dataset_set_req *pReq,
                                  qdss_tpdm_dataset_set_rsp *pRsp)
{
   return qdss_tpdm_dataset_set(pReq->tpdm_id,
                                pReq->enable_mask,
                                pReq->state);
}

int qdss_tpdm_dsb_set_edcmr(uint8 tpdm_id,uint8 start, uint8 end, uint8 state) 
{
   QDSS_TPDM_ENTER;

   TRY(nErr,DAL_TPDM_DSB_SetEDCMR(qdss.hTPDM,tpdm_name,start,end,state));

   QDSS_TPDM_LEAVE;
}

int qdss_tpdm_dsb_set_edcmr_handler(qdss_tpdm_dsb_set_edcmr_req *pReq,
                                    qdss_tpdm_dsb_set_edcmr_rsp *pRsp) 
{
   return qdss_tpdm_dsb_set_edcmr(pReq->tpdm_id,
                                  pReq->start,
                                  pReq->end,
                                  pReq->state);
}
                                    


int qdss_tpdm_get_gpr(uint8 tpdm_id, uint8 reg_num, uint32 *pval)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_Get_GPR(qdss.hTPDM,tpdm_name,reg_num,pval));

  QDSS_TPDM_LEAVE;
}

int qdss_tpdm_get_gpr_handler(qdss_tpdm_get_gpr_req *pReq,
                              qdss_tpdm_get_gpr_rsp *pRsp) 
{
   return qdss_tpdm_get_gpr(pReq->tpdm_id,
                            pReq->reg_num,
                            &pRsp->val);

}


int qdss_tpdm_set_gpr(uint8 tpdm_id, uint8 reg_num, uint32 val)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_Set_GPR(qdss.hTPDM,tpdm_name,reg_num,val));

  QDSS_TPDM_LEAVE;
}

int qdss_tpdm_set_gpr_handler(qdss_tpdm_set_gpr_req *pReq,
                              qdss_tpdm_set_gpr_rsp *pRsp)
{
   return qdss_tpdm_set_gpr(pReq->tpdm_id,
                            pReq->reg_num,
                            pReq->val);
   
}

int qdss_tpdm_dsb_get_param(uint8 tpdm_id, uint8 param_id, uint32 *pval)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_DSB_GetParam(qdss.hTPDM,tpdm_name,param_id,pval));

  QDSS_TPDM_LEAVE;
}

int qdss_tpdm_dsb_get_param_handler(qdss_tpdm_dsb_get_param_req *pReq,
                                    qdss_tpdm_dsb_get_param_rsp *pRsp)
{
   return qdss_tpdm_dsb_get_param(pReq->tpdm_id,
                                  pReq->param_id,
                                  &pRsp->val);
}


int qdss_tpdm_dsb_set_param(uint8 tpdm_id, uint8 param_id, uint32 val)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_DSB_SetParam(qdss.hTPDM,tpdm_name,param_id,val));

  QDSS_TPDM_LEAVE;
}


int qdss_tpdm_dsb_set_param_handler(qdss_tpdm_dsb_set_param_req *pReq,
                                    qdss_tpdm_dsb_set_param_rsp *pRsp)
{
   return qdss_tpdm_dsb_set_param(pReq->tpdm_id,
                                  pReq->param_id,
                                  pReq->val);
}




int qdss_tpdm_cmb_get_param(uint8 tpdm_id, uint8 param_id, uint32 *pval)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_CMB_GetParam(qdss.hTPDM,tpdm_name,param_id,pval));

  QDSS_TPDM_LEAVE;
}



int qdss_tpdm_cmb_get_param_handler(qdss_tpdm_cmb_get_param_req *pReq,
                                    qdss_tpdm_cmb_get_param_rsp *pRsp)
{
   return qdss_tpdm_cmb_get_param(pReq->tpdm_id,
                                  pReq->param_id,
                                  &pRsp->val);
}



int qdss_tpdm_cmb_set_param(uint8 tpdm_id, uint8 param_id, uint32 val)
{
  QDSS_TPDM_ENTER;

  TRY(nErr,DAL_TPDM_CMB_SetParam(qdss.hTPDM,tpdm_name,param_id,val));

  QDSS_TPDM_LEAVE;
}



int qdss_tpdm_cmb_set_param_handler(qdss_tpdm_cmb_set_param_req *pReq,
                                    qdss_tpdm_cmb_set_param_rsp *pRsp)
{
   return qdss_tpdm_cmb_set_param(pReq->tpdm_id,
                                  pReq->param_id,
                                  pReq->val);
}






#define QDSS_HANDLE_TPDM_CMD(cmd)                                        \
   if (pkt_len < (sizeof(qdss_tpdm_req) + sizeof(cmd##_req) -1 )) {      \
      pRsp = diagpkt_err_rsp(DIAG_BAD_LEN_F, pReq, pkt_len);            \
   }                                                                    \
   else {                                                               \
      pRsp =  diagpkt_subsys_alloc                                      \
         (DIAG_SUBSYS_QDSS,                                             \
          pReq->hdr.subsysCmdCode,                                      \
          (sizeof(qdss_tpdm_rsp)+sizeof(cmd##_rsp) -1) );                \
      if (NULL != pRsp) {                                               \
         nErr =  cmd##_handler((cmd##_req *)pReq->args,                 \
                               (cmd##_rsp *)(((qdss_tpdm_rsp *)pRsp)->args)); \
      }                                                                 \
   }


/*-------------------------------------------------------------------------*/

/**
  @brief Handler for all TPDM diag commandds


  @return pointer to a the DIAG response packet
 */
PACK(void *)  qdss_tpdm_handler(qdss_tpdm_req  *pReq,
                               int pkt_len)
{
   int nErr = QDSS_CONTROL_ERROR;
   PACK(void *) pRsp = NULL;

   if (0==qdss.hSync) {
      return diagpkt_err_rsp(QDSS_CONTROL_BAD_STATE, pReq, pkt_len);
   }

   switch(pReq->id) {
   case QDSS_TPDM_SET:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_set);
      break;

   case QDSS_TPDM_DATASET_SET:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_dataset_set);
      break;

   case QDSS_TPDM_GET_GPR:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_get_gpr);
      break;

   case QDSS_TPDM_SET_GPR:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_set_gpr);
      break;

   case QDSS_TPDM_DSB_SET_EDCMR:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_dsb_set_edcmr);
      break;

   case  QDSS_TPDM_DSB_GET_PARAM:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_dsb_get_param);
      break;

   case  QDSS_TPDM_DSB_SET_PARAM:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_dsb_set_param);
      break;

   case  QDSS_TPDM_CMB_GET_PARAM:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_cmb_get_param);
      break;

   case  QDSS_TPDM_CMB_SET_PARAM:
      QDSS_HANDLE_TPDM_CMD(qdss_tpdm_cmb_set_param);
      break;

   default:
      nErr=QDSS_CONTROL_ERROR;
   }


   if  (NULL == pRsp) {
      pRsp = diagpkt_err_rsp(nErr, pReq, pkt_len);
   }
   else {
      ((qdss_tpdm_rsp*)pRsp)->id = pReq->id;
      ((qdss_tpdm_rsp *)pRsp)->result = nErr;
   }
   return pRsp;

}


