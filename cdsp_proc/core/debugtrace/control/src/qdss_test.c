/*=============================================================================

FILE:         qdss_test.c

DESCRIPTION:  QDSS diag test commands implementation

================================================================================
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.
==============================================================================*/

#include "qdss_control_priv.h"
#include "qdss_control.h"
#include "tracer_test.h"
#include "qdss.h"
#include "dbglog.h"

/*-------------------------------------------------------------------------*/

/**
  @brief Send test tracer_data packets

  @param                pArgs [in ] : test data args

  @return 0 if successful, error code otherwise
 */
static int tracer_data_test(qdss_tracer_data_test_args *pArgs)
{
   int nErr = QDSS_CONTROL_SUCCESS;
   uint8 i = 0;

   if (0==qdss.hTracer) {
      THROW_IF(nErr,TRACER_RETURN_SUCCESS !=
               tracer_register(&qdss.hTracer,
                               TRACER_ENTITY_DEFAULT,
                               TRACER_PROTOCOL_DEFAULT),
               QDSS_CONTROL_ERROR);
   }

   for (i=0; i < pArgs->count; i++) {
      THROW_IF(nErr,TRACER_RETURN_SUCCESS !=
               tracer_data(qdss.hTracer,
                           TRACER_OPT_GUARANTEE | TRACER_OPT_TIMESTAMP,
                           0,
                           "QDSS tracer test data %d",
                           qdss.test_data_count),
               QDSS_CONTROL_ERROR);

#ifdef QDSS_UART_LOG      
      DBG_LOG("QDSS tracer test data %lu",qdss.test_data_count);
#endif
      qdss.test_data_count++;
   }

   CATCH(nErr) {}
   return nErr;
}



int qdss_drain_test(qdss_drain_test_args *pArgs) 
{
   switch (pArgs->type) {
   case 1:
      QDSS_IslandEnter();
      break;
   case 2:
      QDSS_IslandExit();
      break;
   case 3:
      qdss_lpm_enter();
      break;
   case 4:
      qdss_lpm_exit();
      break;
   default:
      return DAL_ERROR;
   }
   return DAL_SUCCESS;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Handler for QDSS diag test commands

  @return 0 if successful, error code otherwise
 */
int qdss_test_handler(qdss_test_req  *pReq,
                      int req_len,
                      qdss_test_rsp *pRsp,
                      int rsp_len)
{
   qdss_tracer_test_args *pArgs;
   int nErr = QDSS_CONTROL_SUCCESS;
   int rval;

   switch(pReq->test_num) {

   case QDSS_TRACER_DATA_TEST:
      nErr = tracer_data_test((qdss_tracer_data_test_args *)pReq->args);
      break;

   case QDSS_TRACER_TEST:
      if (req_len < (sizeof(qdss_test_req) +
                     sizeof(qdss_tracer_test_args) - 1) ) {
         nErr = QDSS_CONTROL_FORMAT_ERROR;
      }
      else {
         pArgs = (qdss_tracer_test_args *)pReq->args;
         rval = tracer_test(pArgs->type, pArgs->subtype,
                            pArgs->option, pArgs->mask,
                            pArgs->count1, pArgs->count2);

         nErr = (TRACER_TEST_RVAL_SUCCESS == rval) ? QDSS_CONTROL_SUCCESS :
                ((TRACER_TEST_RVAL_ERROR == rval) ? QDSS_CONTROL_ERROR :
                ((TRACER_TEST_RVAL_REQUEST_ERR == rval) ? QDSS_CONTROL_FORMAT_ERROR :
                ((TRACER_TEST_RVAL_UNSUPPORTED == rval) ? QDSS_CONTROL_UNSUPPORTED :
                                                            QDSS_CONTROL_ERROR)));
      }
      break;

   case QDSS_DRAIN_TEST:
      nErr=qdss_drain_test((qdss_drain_test_args *)pReq->args);
      break;

   default:
      nErr = QDSS_CONTROL_ERROR;
   }

   pRsp->result = nErr;
   return nErr;
}

