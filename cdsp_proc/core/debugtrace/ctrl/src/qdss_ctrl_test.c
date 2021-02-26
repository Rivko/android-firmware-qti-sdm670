/*=============================================================================

FILE:         qdss_ctrl_test.c

DESCRIPTION:  QDSS diag test commands implementation

================================================================================
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/

#include "qdss_ctrl_diag.h"
#include "qdss_ctrl_utils.h"
#include "tracer_entity.h"
#include "qdss_ctrl_priv.h"


extern int tracer_test(uint8 type, uint8 subtype,
                       uint16 option, uint32 mask,
                       uint32 count1, uint32 count2);

/*-------------------------------------------------------------------------*/

/**
  @brief Send test tracer_data packets

  @param                pArgs [in ] : test data args

  @return 0 if successful, error code otherwise
 */
static int tracer_ctrl_data_test(qdss_ctrl_tracer_data_test_args *pArgs)
{
   int nErr = TRACER_RETURN_ERROR;
   uint8 i = 0;

   if (0 == qdss_ctrl_ctxt.hTracer)
   {
      TRY(nErr,tracer_register(&qdss_ctrl_ctxt.hTracer,
                               TRACER_ENTITY_DEFAULT,
                               TRACER_PROTOCOL_DEFAULT));
   }

   for (i = 0; i < pArgs->count; i++)
   {
      TRY(nErr,tracer_data(qdss_ctrl_ctxt.hTracer,
                           TRACER_OPT_GUARANTEE | TRACER_OPT_TIMESTAMP,
                           0,
                           "QDSS tracer test data %d",
                           qdss_ctrl_ctxt.test_data_count));

      qdss_ctrl_ctxt.test_data_count++;
   }

   CATCH(nErr) {}
   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handler for QDSS diag test commands

  @return 0 if successful, error code otherwise
         -1 = unavailable
         -2 = test failed
 */
int qdss_ctrl_test_handler(qdss_ctrl_test_req  *pReq,
                           int req_len,
                           qdss_ctrl_test_rsp *pRsp,
                           int rsp_len)
{
   qdss_ctrl_tracer_test_args *pArgs;
   int nErr = 0;

   switch(pReq->test_num)
   {
      case QDSS_TRACER_DATA_TEST:
         if (TRACER_RETURN_SUCCESS != tracer_ctrl_data_test(
            (qdss_ctrl_tracer_data_test_args *)pReq->args))
         {
            nErr = -2;
         }
         break;
      case QDSS_TRACER_TEST:
         if (req_len < (sizeof(qdss_ctrl_test_req) +
                        sizeof(qdss_ctrl_tracer_test_args) - 1) ) {
            nErr = -2;
         }
         else {
            pArgs = (qdss_ctrl_tracer_test_args *)pReq->args;
            nErr = tracer_test(pArgs->type, pArgs->subtype,
                               pArgs->option, pArgs->mask,
                               pArgs->count1, pArgs->count2);
         }
         break;

      default:
         nErr = -1;
   }

   pRsp->result = nErr;
   return nErr;
}

