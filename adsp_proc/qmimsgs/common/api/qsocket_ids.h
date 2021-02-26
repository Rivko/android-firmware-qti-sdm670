#ifndef QSOCKET_IDS_H
#define QSOCKET_IDS_H
/******************************************************************************
  @file    qsocket_ids.h
  
  @brief   List of approved QSocket IDs

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  $Header: //components/rel/qmimsgs.adsp/2.6/common/api/qsocket_ids.h#5 $
*******************************************************************************/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum {
    QSOCKET_MIN_ID = 0x1000,
    QSOCKET_TFTP_ID = QSOCKET_MIN_ID,
    QSOCKET_DIAG_ID,
    QSOCKET_MDM_CXM_ID,
    QSOCKET_WCN_CXM_ID,
    QSOCKET_SNS_CLIENT_SVC_ID,
    QSOCKET_APR_ADSP_MDSP_ID,
    QSOCKET_LOCATION_HAL_DAEMON_ID,
    QSOCKET_LOCATION_REMOTE_CLIENT_ID,
    QSOCKET_MAX_ID,
    QSOCKET_VENDOR_MIN_ID = 0x1800,
    QSOCKET_VENDOR_MAX_ID = 0x2000
  }qsocket_ids_enum;

#ifdef __cplusplus
}
#endif
#endif

