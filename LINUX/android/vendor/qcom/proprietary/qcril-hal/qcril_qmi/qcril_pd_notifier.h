/******************************************************************************
#  Copyright (c) 2016, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_pd_notifier.h

  @brief
  Get notifications of PD status

*/

#ifndef QCRIL_PD_NOTIFIER_H_INCLUDED
#define QCRIL_PD_NOTIFIER_H_INCLUDED

#include <stdint.h>

struct qcril_qmi_pd_handle;
typedef struct qcril_qmi_pd_handle qcril_qmi_pd_handle;
struct qcril_qmi_pd_entry;
typedef struct qcril_qmi_pd_entry qcril_qmi_pd_entry;

typedef enum qcril_qmi_pd_state
{
    QCRIL_QMI_PD_UNKNOWN,
    QCRIL_QMI_PD_DOWN,
    QCRIL_QMI_PD_UP,
    QCRIL_QMI_PD_UNUSED
} qcril_qmi_pd_state;

typedef void (*qcril_qmi_pd_notifier_cb)(qcril_qmi_pd_entry *, qcril_qmi_pd_state state, void *userdata);

#ifdef __cplusplus
extern "C" {
#endif

qcril_qmi_pd_handle * qcril_qmi_pd_notifier_new
(
    const char *service,
    const char *client_name
);

void qcril_pd_notifier_register_for_all
(
    qcril_qmi_pd_handle *handle,
    qcril_qmi_pd_notifier_cb cb,
    void *userdata
);

void qcril_pd_notifier_start_listening_all(qcril_qmi_pd_handle *handle);
void qcril_pd_notifier_stop_listening_all(qcril_qmi_pd_handle *handle);

uint32_t qcril_pd_notifier_get_n_entries(qcril_qmi_pd_handle *handle);

qcril_qmi_pd_entry *qcril_pd_notifier_get_entry(qcril_qmi_pd_handle *handle, int32_t i);
void qcril_pd_notifier_release_entry(qcril_qmi_pd_handle *handle, int32_t i);
void qcril_pd_notifier_put_entry(qcril_qmi_pd_entry *entry);
void qcril_pd_notifier_register_for
(
    qcril_qmi_pd_handle *handle,
    const char *domain,
    qcril_qmi_pd_notifier_cb cb,
    void *userdata
);
void qcril_pd_notifier_register_for_entry
(
    qcril_qmi_pd_entry *entry,
    qcril_qmi_pd_notifier_cb cb,
    void *userdata
);
void qcril_pd_notifier_unregister_for_entry(qcril_qmi_pd_entry *entry);

void qcril_qmi_pd_entry_get_data
(
    qcril_qmi_pd_entry *entry,
    const char **name,
    const char **notifier_string,
    int *instance,
    int *svc_data_valid,
    int *svc_data,
    void **userdata
);

void qcril_pd_notifier_start_listening
(
    qcril_qmi_pd_handle *handle,
    const char *domain
);
void qcril_pd_notifier_stop_listening
(
    qcril_qmi_pd_handle *handle,
    const char *domain
);

uint8_t qcril_pd_notifier_available
(
    qcril_qmi_pd_handle *handle
);

#ifdef __cplusplus
}
#endif
#endif
