#ifndef __CVD_DIAG_I_H__
#define __CVD_DIAG_I_H__

/**
  @file  cvd_diag_i.h
  @brief This file contains private definitions of cvd diag utility.
*/

/*
  ============================================================================

   Copyright (C) 2016 , 2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_diag_i.h#2 $
  $Author: pwbldsvc $

  ============================================================================
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"

/* APR APIs. */
#include "apr_comdef.h"
#include "apr_lock.h"
#include "apr_errcodes.h"
#include "aprv2_ids_domains.h"
#include "aprv2_api_inline.h"
#include "aprv2_msg_if.h"

#include "diagbuf.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "vss_private_if.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_DIAG_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "cvd_diag:%d error[%d]", __LINE__, rc, 0 ); } }

#define CVD_DIAG_COMMAND ( 0 )
#define VDS_DIAG_COMMAND ( 1 )

#define VOID_DIAG_PACKED_PTR PACKED void* PACKED_POST

#define CVD_DIAG_ITEST_CMD_ENABLE_CSM ( 1 )
#define CVD_DIAG_ITEST_CMD_DISABLE_CSM ( 0 )
#define CVD_DIAG_ITEST_CMD_INITIAL_VALUE ( 0xFF )
/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef PACKED struct PACKED_POST cvd_diag_cmd_req_t
{
  diagpkt_subsys_header_type header;
  uint8_t cmd;
  uint8_t payload1;
  uint8_t payload2;
  uint8_t payload3;
  uint8_t payload4;
  uint8_t payload5;
}
  cvd_diag_cmd_req_t;

typedef PACKED struct PACKED_POST cvd_diag_rsp_t
{
  diagpkt_subsys_header_type header;
  uint32_t result;
}
  cvd_diag_rsp_t;

/****************************************************************************
 * FORWARD PROTOTYPES                                                       *
 ****************************************************************************/

VOID_DIAG_PACKED_PTR cvd_diag_cmd_processing (
  VOID_DIAG_PACKED_PTR req_pkt,
  uint16 pkt_len
);

#endif /* __CVD_DIAG_I_H__ */

