/*=============================================================================

FILE:         qdss_ctrl.c

DESCRIPTION:  Implements handlers for diag comamnds to configure qdss on
              a particular core
=============================================================================*/
/*=============================================================================
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*===========================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/ctrl/src/qdss_ctrl.c#1 $
===========================================================================*/

#include "qdss_ctrl_diag.h"
#include "qdss_ctrl_etm.h"
#include "qdss_ctrl_utils.h"
#include "qdss_ctrl_priv.h"
#include "ClockDefs.h"
#include "qdss_control.h"
#include "qdss_tfunnel.h"



struct qdss_ctrl_s qdss_ctrl_ctxt;

/*---------------------------------------------------------------------------
 * Static Function Definitions
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief  Handles the QDSS_CTRL_FILTER_ETM diag command

  @return 0 if successful, error code otherwise
 */

int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                                 int req_len,
                                 qdss_ctrl_filter_etm_rsp *pRsp,
                                 int rsp_len)
{
   int nErr = DAL_ERROR;

   nErr = qdss_control_set_etm(pReq->state);
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
int qdss_control_get_etm(uint8 *state)
{
   *state = qdss_ctrl_ctxt.etm_state;
   return 0;
}


/*-------------------------------------------------------------------------*/
// Debug Agent QMI QDSSC message
int qdss_control_get_etm_config(const char *pget_str,
                                char *presp_str,
                                size_t resp_max_len)
{
   return qdss_ctrl_etm_get_config(pget_str,
                                   presp_str,
                                   resp_max_len);
}

/*-------------------------------------------------------------------------*/
// Debug Agent QMI QDSSC message
int qdss_control_set_etm_config(const char *pset_str)
{
   return qdss_ctrl_etm_set_config(pset_str);
}

/*-------------------------------------------------------------------------*/
/**
  @brief  Handles the QDSS_CTRL_ETM_GET_PARAM diag command

  @return 0 if successful, error code otherwise
 */
int qdss_ctrl_etm_get_param_handler(qdss_ctrl_etm_get_param_req *pReq,
                                    int req_len,
                                    qdss_ctrl_etm_get_param_rsp *pRsp,
                                    int rsp_len)
{
   int nErr = QDSS_CONTROL_RVAL_UNKNOWN_ERR;;
   uint32 getval = 0;
   pRsp->config_id = pReq->config_id;
   pRsp->param_id = pReq->param_id;
   pRsp->resv = 0;
   if (QDSS_CTRL_ETM_CONFIG_FORMAT_ID != pReq->config_id)
   {
      nErr = QDSS_CONTROL_RVAL_INVALID_ARG;
      pRsp->val = 0;
   }
   else
   {
      nErr = qdss_ctrl_etm_get_param(pReq->param_id, &getval);
      pRsp->val = getval;
   }
   pRsp->result = nErr;
   return nErr;
}

/*-------------------------------------------------------------------------*/
/**
   @brief  Handles the QDSS_CTRL_SET_ETM_PARAM diag command

   @return 0 if successful, error code otherwise
 */
#if 1
int qdss_ctrl_etm_set_param_handler(qdss_ctrl_etm_set_param_req *pReq,
                                    int req_len,
                                    qdss_ctrl_etm_set_param_rsp *pRsp,
                                    int rsp_len)
{
   int nErr = QDSS_CONTROL_RVAL_UNKNOWN_ERR;
   pRsp->config_id = pReq->config_id;
   pRsp->param_id = pReq->param_id;
   if (QDSS_CTRL_ETM_CONFIG_FORMAT_ID != pReq->config_id)
   {
      nErr = QDSS_CONTROL_RVAL_INVALID_ARG;
   }
   else
   {
      nErr = qdss_ctrl_etm_set_param(pReq->param_id, pReq->val);
   }
   pRsp->result = nErr;
   return nErr;
}
#else
// For development and testing: Using diag IF to test the
// qdss_control_set_etm_config function.
#include <stdio.h>
int qdss_ctrl_etm_set_param_handler(qdss_ctrl_etm_set_param_req *pReq,
                                    int req_len,
                                    qdss_ctrl_etm_set_param_rsp *pRsp,
                                    int rsp_len)
{
   int nErr;
   char teststr[32];
   switch (pReq->param_id)
   {
   case 1:
      snprintf(teststr, sizeof(teststr), "mode %d", pReq->val);
      break;
   case 2:
      snprintf(teststr, sizeof(teststr), "route %d", pReq->val);
      break;
   case 3:
      snprintf(teststr, sizeof(teststr), "filter 0x%x", pReq->val);
      break;
   default:
      nErr = QDSS_CONTROL_RVAL_ACCESS_DENIED;
      break;
   }
   nErr = qdss_control_set_etm_config(teststr);
   pRsp->param_id = pReq->param_id;
   pRsp->result = nErr;
   return nErr;
}
#endif

/*-------------------------------------------------------------------------*/
void qdss_ctrl_clk_on(void)
{
   if (NULL==qdss_ctrl_ctxt.npa_client) {
      qdss_ctrl_ctxt.npa_client =
         npa_create_sync_client("/clk/qdss", "qdss", NPA_CLIENT_REQUIRED);
   }

   if (NULL==qdss_ctrl_ctxt.npa_client) {
      return;
   }
   npa_issue_required_request(qdss_ctrl_ctxt.npa_client,
                              CLOCK_QDSS_LEVEL_DEBUG);    
}
/*-------------------------------------------------------------------------*/

void qdss_ctrl_clk_off(void)
{
   if(NULL==qdss_ctrl_ctxt.npa_client) {
      return;
   }
   npa_issue_required_request(qdss_ctrl_ctxt.npa_client,
                              CLOCK_QDSS_LEVEL_OFF);
}


/*-------------------------------------------------------------------------*/

/**
   @brief Initializes the QDSS control subsystem.

   This is called from RC init.

   @return None
 */
void qdss_ctrl_init(void)
{
   int nErr = DAL_ERROR;

   qdss_ctrl_diag_init();
   qdss_ctrl_ctxt.hSync = 0;
   qdss_ctrl_ctxt.etm_port_enabled = FALSE;
   qdss_ctrl_ctxt.etm_state = 0;
   qdss_ctrl_ctxt.hTracer = 0;
   qdss_ctrl_ctxt.test_data_count = 0;

   qdss_ctrl_etm_init_config_def();

   TRY(nErr, DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                               &qdss_ctrl_ctxt.hSync, NULL));
   CATCH(nErr)   {}
   IGNORE(nErr);
}

