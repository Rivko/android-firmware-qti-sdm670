#ifndef QDSS_DIAG_H
#define QDSS_DIAG_H

/*=============================================================================

FILE:         qdss_diag.h

DESCRIPTION:

================================================================================
              Copyright (c) 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include "qdss_diag_cmd.h"

int qdss_query_status_handler(qdss_query_status_req *pReq, int req_len,
                              qdss_query_status_rsp *pRsp, int rsp_len);

int qdss_trace_sink_handler(qdss_trace_sink_req  *pReq, int req_len,
                            qdss_trace_sink_rsp *pRsp,  int rsp_len);

int qdss_filter_etm_handler(qdss_filter_etm_req *pReq, int req_len,
                            qdss_filter_etm_rsp *pRsp, int rsp_len);



int qdss_filter_stm_handler(qdss_filter_stm_req *pReq, int req_len,
                            qdss_filter_stm_rsp *pRsp, int rsp_len);


int qdss_filter_hwevents_handler(qdss_filter_hwevents_req *pReq, int req_len,
                                 qdss_filter_hwevents_rsp *pRsp, int rsp_len);

int qdss_filter_hwevents_configure_handler(
         qdss_filter_hwevents_configure_req *pReq, int req_len,
         qdss_filter_hwevents_configure_rsp *pRsp, int rsp_len);

int qdss_filter_hwevents_set_detect_mask_handler(
         qdss_filter_hwevents_set_detect_mask_req *pReq, int req_len,
         qdss_filter_hwevents_set_detect_mask_rsp *pRsp, int rsp_len);


int qdss_filter_hwevents_set_trigger_mask_handler(
         qdss_filter_hwevents_set_trigger_mask_req *pReq, int req_len,
         qdss_filter_hwevents_set_trigger_mask_rsp *pRsp, int rsp_len);

int qdss_map_cti_handler(qdss_map_cti_req *pReq, int req_len,
                         qdss_map_cti_rsp *pRsp, int rsp_len);

int qdss_unmap_cti_handler(qdss_unmap_cti_req *pReq, int req_len,
                           qdss_unmap_cti_rsp *pRsp, int rsp_len);

int qdss_reset_cti_handler(qdss_reset_cti_req *pReq, int req_len,
                           qdss_reset_cti_rsp *pRsp, int rsp_len);

int qdss_query_cti_handler(qdss_query_cti_req *pReq, int req_len,
                           qdss_query_cti_rsp *pRsp,  int rsp_len);

PACK(void *)  qdss_tmc_handler(qdss_tmc_req *pReq, int req_len);

PACK(void *)  qdss_tpdm_handler(qdss_tpdm_req *pReq, int req_len);

PACK(void *)  qdss_tpda_handler(qdss_tpda_req *pReq, int req_len);
int qdss_test_handler(qdss_test_req *pReq, int req_len,
                      qdss_test_rsp *pRsp, int rsp_len);

int qdss_agent_test_handler(qdss_agent_test_req *pReq, int req_len,
                            qdss_agent_test_rsp *pRsp, int rsp_len);


void qdss_diag_init(void);

#endif //QDSS_DIAG_H
