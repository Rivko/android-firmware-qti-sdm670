#ifndef QMI_CSI_TARGET_H
#define QMI_CSI_TARGET_H
/******************************************************************************
  @file    qmi_csi_target.h
  @brief   QuRT OS Specific routines internal to QCSI.

  DESCRIPTION
  This header provides an OS (QuRT) abstraction to QCSI.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2014,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "comdef.h"
#include "stringl.h"
#include "qurt.h"
#include "err.h"
#include "assert.h"
#include "qmi_common.h"
#include "qmi_csi_target_ext.h"
#include "qmi_idl_lib_internal.h"

typedef  qurt_mutex_t qmi_csi_lock_type;

#define LOCK(ptr)        qurt_pimutex_lock(ptr)
#define UNLOCK(ptr)      qurt_pimutex_unlock(ptr)
#define LOCK_INIT(ptr)   qurt_pimutex_init(ptr)
#define LOCK_DEINIT(ptr) qurt_pimutex_destroy(ptr)

#define MALLOC(size)      malloc(size)
#define CALLOC(num, size) calloc(num, size)
#define FREE(ptr)         free(ptr)
#define REALLOC(ptr,size) realloc(ptr, size)

extern void qmi_csi_os_signal_set(qmi_csi_os_params *sig);

extern void qmi_csi_os_log_error(char *filename, unsigned int line);

extern void qmi_csi_os_log_tx
(
  qmi_idl_service_object_type svc_obj,
  uint32_t cntl_flag,
  uint32_t txn_id,
  uint32_t msg_id,
  uint32_t msg_len,
  void *addr,
  size_t addr_len
);

extern void qmi_csi_os_log_rx
(
  qmi_idl_service_object_type svc_obj,
  uint32_t cntl_flag,
  uint32_t txn_id,
  uint32_t msg_id,
  uint32_t msg_len,
  void *addr,
  size_t addr_len
);

#define QMI_CSI_OS_SIGNAL_SET(s) qmi_csi_os_signal_set(s)

#define QMI_CSI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  qmi_csi_os_log_tx(svc_obj, (uint32_t)(cntl_flag), (uint32_t)(txn_id), (uint32_t)(msg_id), (uint32_t)(msg_len), addr, (size_t)(addr_len))

#define QMI_CSI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  qmi_csi_os_log_rx(svc_obj, (uint32_t)(cntl_flag), (uint32_t)(txn_id), (uint32_t)(msg_id), (uint32_t)(msg_len), addr, (size_t)(addr_len))

#define QMI_CSI_OS_LOG_ERROR() qmi_csi_os_log_error(__FILENAME__,__LINE__)

#endif
