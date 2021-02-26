/******************************************************************************
 ----------------------------------------------------------------------------
 Copyright (c) 2011-2015,2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ----------------------------------------------------------------------------
*******************************************************************************/

#ifndef QMI_FRAMEWORK_ERR_H_
#define QMI_FRAMEWORK_ERR_H_

#include <stdlib.h>
#include <stdio.h>
#include "qmi_common.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_idl_lib_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(QMI_CCI_SMEM_LOG_DISABLE) || defined(QMI_CSI_SMEM_LOG_DISABLE)
#define SMEM_LOG_EVENT( event, data1, data2, data3 ) \
do { } while(0)

#define SMEM_LOG_EVENT6(identifier, data1, data2, data3, data4, data5, data6) \
do { } while(0)
#else
#include "smem_log.h"
#endif

#ifdef QMI_FW_ADB_LOG
#define LOG_TAG "QMI_FW"
#include <utils/Log.h>
#include "common_log.h"

#ifdef QMI_CCI_ANDROID
extern unsigned int qmi_cci_debug_level;
#define QMI_CCI_LOGI(x...) do { \
  if(qmi_cci_debug_level <= ANDROID_LOG_INFO) \
    SLOGI("QCCI: "x); \
} while(0)
#define QMI_CCI_LOGD(x...) do { \
  if(qmi_cci_debug_level <= ANDROID_LOG_DEBUG) \
    SLOGD("QCCI: "x); \
} while(0)
#else
#define QMI_CCI_LOGI(x...)
#define QMI_CCI_LOGD(x...)
#endif

#ifdef QMI_CSI_ANDROID
extern unsigned int qmi_csi_debug_level;
#define QMI_CSI_LOGI(x...) do { \
  if(qmi_csi_debug_level <= ANDROID_LOG_INFO) \
    SLOGI("QCSI: "x); \
} while(0)
#define QMI_CSI_LOGD(x...) do { \
  if(qmi_csi_debug_level <= ANDROID_LOG_DEBUG) \
    SLOGD("QCSI: "x); \
} while(0)
#else
#define QMI_CSI_LOGI(x...)
#define QMI_CSI_LOGD(x...)
#endif

#define QMI_FW_LOGE(x...) LOGE(x);

#elif defined(QMI_FW_SYSLOG)
#include <syslog.h>

extern unsigned int qmi_cci_debug_level;
extern unsigned int qmi_csi_debug_level;
#define QMI_CCI_LOGI(x...) do { \
  if(qmi_cci_debug_level >= LOG_INFO) \
    syslog(LOG_INFO, "QMI_FW: QCCI: "x); \
  } while(0)
#define QMI_CSI_LOGI(x...) do { \
  if(qmi_csi_debug_level >= LOG_INFO) \
    syslog(LOG_INFO, "QMI_FW: QCSI: "x); \
  } while(0)

#define QMI_CCI_LOGD(x...) do { \
  if(qmi_cci_debug_level >= LOG_DEBUG) \
    syslog(LOG_DEBUG, "QMI_FW: QCCI: "x); \
  } while(0)
#define QMI_CSI_LOGD(x...) do { \
  if(qmi_csi_debug_level >= LOG_DEBUG) \
    syslog(LOG_DEBUG, "QMI_FW: QCSI: "x); \
  } while(0)

#define QMI_FW_LOGE(x...)  syslog(LOG_ERR, x)

#else
#define QMI_CCI_LOGI(x...) do { \
        fprintf(stdout, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stdout, ##x);                               \
    } while(0)

#define QMI_CCI_LOGD(x...) do { \
        fprintf(stdout, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stdout, ##x);                               \
    } while(0)

#define QMI_CSI_LOGI(x...) do { \
        fprintf(stdout, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stdout, ##x);                               \
    } while(0)

#define QMI_CSI_LOGD(x...) do { \
        fprintf(stdout, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stdout, ##x);                               \
    } while(0)

#define QMI_FW_LOGE(x...) do { \
        fprintf(stderr, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stderr, ##x);                               \
    } while(0)
#endif

#define ASSERT( xx_exp ) \
    if( !(xx_exp) ) \
    { \
        QMI_FW_LOGE("Assertion "  #xx_exp " failed"); \
        exit(1); \
    } \

#define QMI_CCI_LOG_EVENT_TX            (SMEM_LOG_QCCI_EVENT_BASE + 0x00)
#define QMI_CCI_LOG_EVENT_RX            (SMEM_LOG_QCCI_EVENT_BASE + 0x01)
#define QMI_CCI_LOG_EVENT_ERROR         (SMEM_LOG_QCCI_EVENT_BASE + 0x03)
#define QMI_CCI_LOG_EVENT_TX_EXT        (SMEM_LOG_QCCI_EVENT_BASE + 0x04)
#define QMI_CCI_LOG_EVENT_RX_EXT        (SMEM_LOG_QCCI_EVENT_BASE + 0x05)

/* Assumes addr_len is MAX_ADDR_LEN. Change needed if qmi_cci_common.c changes
 * to invalidate this assumption */
#define QMI_CCI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do { \
    uint32_t *int_addr = (uint32_t *)(addr); \
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_TX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len), \
                    (svc_obj)->service_id, int_addr[0], int_addr[1], int_addr[2]); \
    QMI_CCI_LOGD("QMI_CCI_TX: cntl_flag - %02x, txn_id - %04x, msg_id - %04x, msg_len - %04x, svc_id - %08x\n", \
                 cntl_flag, txn_id, msg_id, msg_len, (svc_obj)->service_id); \
  } while(0)

/* Assumes addr_len is MAX_ADDR_LEN. Change needed if qmi_cci_common.c changes
 * to invalidate this assumption */
#define QMI_CCI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do { \
    uint32_t *int_addr = (uint32_t *)(addr); \
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_RX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len), \
                    (svc_obj)->service_id, int_addr[0], int_addr[1], int_addr[2]); \
    QMI_CCI_LOGD("QMI_CCI_RX: cntl_flag - %02x, txn_id - %04x, msg_id - %04x, msg_len - %04x, svc_id - %08x\n", \
                 cntl_flag, txn_id, msg_id, msg_len, (svc_obj)->service_id); \
  } while(0)

#define QMI_CCI_LOG_ERR(x...) QMI_FW_LOGE(x)

#define QMI_CSI_LOG_EVENT_TX            (SMEM_LOG_QCSI_EVENT_BASE + 0x00)
#define QMI_CSI_LOG_EVENT_RX            (SMEM_LOG_QCSI_EVENT_BASE + 0x01)
#define QMI_CSI_LOG_EVENT_ERROR         (SMEM_LOG_QCSI_EVENT_BASE + 0x03)
#define QMI_CSI_LOG_EVENT_TX_EXT        (SMEM_LOG_QCSI_EVENT_BASE + 0x04)
#define QMI_CSI_LOG_EVENT_RX_EXT        (SMEM_LOG_QCSI_EVENT_BASE + 0x05)

#define QMI_CSI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint32_t *int_addr = (uint32_t *)(addr); \
    SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_TX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len), \
                    (svc_obj)->service_id, int_addr[0], int_addr[1], 0); \
    QMI_CSI_LOGD("QMI_CSI_TX: cntl_flag - %02x, txn_id - %04x, msg_id - %04x, msg_len - %04x, " \
                 "svc_id - %08x, clnt_addr - %08x:%08x:%08x\n", \
                 cntl_flag, txn_id, msg_id, msg_len, (svc_obj)->service_id, int_addr[0], int_addr[1], 0); \
  } while(0)

#define QMI_CSI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do { \
    uint32_t *int_addr = (uint32_t *)(addr); \
    SMEM_LOG_EVENT6(QMI_CSI_LOG_EVENT_RX_EXT, (cntl_flag) << 16 | (txn_id), (msg_id) << 16 | (msg_len), \
                    (svc_obj)->service_id, int_addr[0], int_addr[1], 0); \
    QMI_CSI_LOGD("QMI_CSI_RX: cntl_flag - %02x, txn_id - %04x, msg_id - %04x, msg_len - %04x, " \
                 "svc_id - %08x, clnt_addr - %08x:%08x:%08x\n", \
                 cntl_flag, txn_id, msg_id, msg_len, (svc_obj)->service_id, int_addr[0], int_addr[1], 0); \
  } while(0)

#ifdef __cplusplus
}
#endif

#endif
