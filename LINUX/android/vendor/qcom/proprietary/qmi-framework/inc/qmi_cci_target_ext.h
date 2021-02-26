#ifndef QMI_CCI_TARGET_EXT_H
#define QMI_CCI_TARGET_EXT_H
/******************************************************************************
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2013, 2016, 2019 Qualcomm Technologies, Inc. All Rights 
  Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include <pthread.h>
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t sig_set;
    uint32_t timed_out;
    uint32_t clock;
    pthread_cond_t cond;
    pthread_condattr_t attr;
    pthread_mutex_t mutex;
} qmi_cci_os_signal_type;

typedef qmi_cci_os_signal_type qmi_client_os_params;

#define QMI_CCI_OS_SIGNAL qmi_cci_os_signal_type

#define QMI_CCI_OS_SIGNAL_INIT(ptr, os_params) \
  do { \
    int rc; \
    (ptr)->sig_set = 0; \
    (ptr)->timed_out = 0; \
    pthread_condattr_init(&(ptr)->attr); \
    rc = pthread_condattr_setclock(&(ptr)->attr, CLOCK_MONOTONIC); \
    if(!rc) \
    { \
      rc = pthread_cond_init(&(ptr)->cond, &(ptr)->attr); \
      if(!rc) \
      { \
        (ptr)->clock = CLOCK_MONOTONIC; \
      } \
      else \
      { \
        pthread_cond_init(&(ptr)->cond, NULL); \
        (ptr)->clock = CLOCK_REALTIME; \
      } \
    } \
    else \
    { \
      pthread_cond_init(&(ptr)->cond, NULL); \
      (ptr)->clock = CLOCK_REALTIME; \
    } \
    pthread_mutex_init(&(ptr)->mutex, NULL); \
  } while(0)

#define QMI_CCI_OS_SIGNAL_DEINIT(ptr) \
  do {  \
    (ptr)->sig_set = 0; \
    (ptr)->timed_out = 0; \
    pthread_condattr_destroy(&(ptr)->attr); \
    pthread_cond_destroy(&(ptr)->cond); \
    pthread_mutex_destroy(&(ptr)->mutex); \
  } while(0)

#define QMI_CCI_OS_EXT_SIGNAL_INIT(ptr, os_params) \
  do { \
    ptr = os_params; \
    QMI_CCI_OS_SIGNAL_INIT(ptr, NULL); \
  } while(0)

#define QMI_CCI_OS_SIGNAL_CLEAR(ptr) (ptr)->sig_set = 0

void qmi_cci_os_signal_wait(QMI_CCI_OS_SIGNAL *ptr, unsigned int timeout_ms);
#define QMI_CCI_OS_SIGNAL_WAIT qmi_cci_os_signal_wait

#define QMI_CCI_OS_SIGNAL_TIMED_OUT(ptr) (ptr)->timed_out
#define QMI_CCI_OS_SIGNAL_SET(ptr)  \
  do { \
    pthread_mutex_lock(&(ptr)->mutex); \
    (ptr)->sig_set = 1; \
    pthread_cond_signal(&(ptr)->cond); \
    pthread_mutex_unlock(&(ptr)->mutex); \
  } while(0)

#ifdef __cplusplus
}
#endif
#endif
