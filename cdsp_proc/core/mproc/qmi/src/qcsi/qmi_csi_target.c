/******************************************************************************
  @file    qmi_cci_target.c
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This source provides an OS abstraction to QCCI.
  
  The current design is to use QuRT abstraction for all interfaces except
  when the user is involved. If the user provides NULL as the rex TCB type,
  the QuRT abstraction is automatically used.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/qmi/src/qcsi/qmi_csi_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qmi_csi_target.h"
#include "qmi_common.h"
#include "qmi_idl_lib_internal.h"

#ifdef FEATURE_QMI_SMEM_LOG
#include "smem_log.h"


#define QMI_CSI_LOG_EVENT_TX            (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x00)
#define QMI_CSI_LOG_EVENT_TX_EXT        (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x04)
#define QMI_CSI_LOG_EVENT_RX            (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x01)
#define QMI_CSI_LOG_EVENT_RX_EXT        (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x05)
#define QMI_CSI_LOG_EVENT_ERROR         (SMEM_LOG_QMI_CSI_EVENT_BASE + 0x03)
#endif

size_t strlcpy(char *dst, const char *src, size_t siz);

extern void qmi_csi_os_signal_set(qmi_csi_os_params *sig)
{
  ASSERT(sig->signal);
  if(sig->signal)
  {
    qurt_anysignal_set(sig->signal, sig->sig);
  }
}

void qmi_csi_os_log_tx
(
  qmi_idl_service_object_type svc_obj,
  uint32_t cntl_flag,
  uint32_t txn_id,
  uint32_t msg_id,
  uint32_t msg_len,
  void *addr,
  size_t addr_len
)
{
#ifdef FEATURE_QMI_SMEM_LOG
  uint32_t __addr[MAX_ADDR_LEN/4] = {0};
  QMI_MEM_COPY(__addr, MAX_ADDR_LEN, addr, addr_len);
  SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_TX_EXT, (cntl_flag << 16) | txn_id, 
                  (msg_id << 16) | msg_len,    svc_obj->service_id, 
                  __addr[0], __addr[1], __addr[2]);
#endif
}

void qmi_csi_os_log_rx
(
  qmi_idl_service_object_type svc_obj,
  uint32_t cntl_flag,
  uint32_t txn_id,
  uint32_t msg_id,
  uint32_t msg_len,
  void *addr,
  size_t addr_len
)
{
#ifdef FEATURE_QMI_SMEM_LOG
  uint32_t __addr[MAX_ADDR_LEN/4] = {0};
  QMI_MEM_COPY(__addr, MAX_ADDR_LEN, addr, addr_len);
  SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_RX_EXT, (cntl_flag << 16) | txn_id, 
                    (msg_id << 16) | msg_len, svc_obj->service_id,
                    __addr[0], __addr[1], __addr[2]);
#endif
}

void qmi_csi_os_log_error(char *filename, unsigned int line)
{
#ifdef FEATURE_QMI_SMEM_LOG
  uint32 name[5];
  strlcpy((char *)name, filename, sizeof(name));
  SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
#endif

#ifdef FEATURE_QMI_MSG
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);
#endif
}

