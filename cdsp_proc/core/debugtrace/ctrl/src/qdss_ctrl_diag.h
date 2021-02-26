#ifndef QDSS_CTRL_DIAG_H
#define QDSS_CTRL_DIAG_H

/*=============================================================================

FILE:         qdss_ctrl_diag.h

DESCRIPTION:

================================================================================
              Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "diagpkt.h"
#include "diagcmd.h"

#define QDSS_CTRL_FILTER_ETM                0x02
#define QDSS_CTRL_ETM_GET_PARAM             0x39
#define QDSS_CTRL_ETM_SET_PARAM             0x3A
#define QDSS_CTRL_TEST_CMD                  0xFF


typedef PACK(struct)
{
  uint8 cmdCode;        /**< Diag Message ID */
  uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
  uint16 subsysCmdCode; /**< Subsystem command code */
} qdss_ctrl_diag_pkt_hdr;

/*-------------------------------------------------------------------------*/
/**
  @brief Response packet: Generic reponse with result.

  Common packet when only returning a result.
 */

typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
} qdss_ctrl_diag_pkt_req;


typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr; /**< Header */
  uint8 result;             /**< See QDSS_CMDRESP_... definitions */
} qdss_ctrl_diag_pkt_rsp;


/* ----------------------------------------------------------------------- */
typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint8 state;
} qdss_ctrl_filter_etm_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_filter_etm_rsp;

int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                                 int req_len,
                                 qdss_ctrl_filter_etm_rsp *pRsp,
                                 int rsp_len);

/* ----------------------------------------------------------------------- */
typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint16 config_id;         // Configuration format id
  uint16 param_id;          // QDSS_CTRL_ETM_PARAM_ID_DSP_...
} qdss_ctrl_etm_get_param_req;

typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint16 config_id;         // Configuration format id
  uint16 param_id;          // QDSS_CTRL_ETM_PARAM_ID_DSP_...
  uint16 result;            /**< See QDSS_CONTROL_... definitions */
  uint16 resv;              // Reserved.
  uint32 val;               // Value of param_id
} qdss_ctrl_etm_get_param_rsp;

int qdss_ctrl_etm_get_param_handler(qdss_ctrl_etm_get_param_req *pReq,
                                    int req_len,
                                    qdss_ctrl_etm_get_param_rsp *pRsp,
                                    int rsp_len);

/* ----------------------------------------------------------------------- */
typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint16 config_id;         // Configuration format id
  uint16 param_id;          // QDSS_CTRL_ETM_PARAM_ID_DSP_...
  uint32 val;               // Value to configure param_id to
} qdss_ctrl_etm_set_param_req;

typedef  PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint16 config_id;         // Configuration format id
  uint16 param_id;          // QDSS_CTRL_ETM_PARAM_ID_DSP_...
  uint16 result;            /**< See QDSS_CONTROL_... definitions */
} qdss_ctrl_etm_set_param_rsp;

int qdss_ctrl_etm_set_param_handler(qdss_ctrl_etm_set_param_req *pReq,
                                    int req_len,
                                    qdss_ctrl_etm_set_param_rsp *pRsp,
                                    int rsp_len);


/* ----------------------------------------------------------------------- */
typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint8  test_num;
  uint8  args[1];
} qdss_ctrl_test_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_test_rsp;

#define QDSS_TRACER_DATA_TEST 1

typedef PACK(struct)
{
  uint8 count;
} qdss_ctrl_tracer_data_test_args;

#define QDSS_TRACER_TEST    3

typedef PACK(struct)
{
  uint8 type;
  uint8 subtype;
  uint16 option;
  uint32 mask;
  uint32 count1;
  uint32 count2;
} qdss_ctrl_tracer_test_args;

int qdss_ctrl_test_handler(qdss_ctrl_test_req *pReq,
                           int req_len,
                           qdss_ctrl_test_rsp *pRsp,
                           int rsp_len);

/* ----------------------------------------------------------------------- */

void qdss_ctrl_diag_init(void);


#endif //QDSS_CTRL_DIAG_H

