#ifndef QMI_CCI_TARGET_EXT_H
#define QMI_CCI_TARGET_EXT_H
/******************************************************************************
  @file    qmi_cci_target_ext.h
  @brief   QMI CCI OS Specific macro/types externalized to clients.

  DESCRIPTION
  QMI OS Specific types and macros required by QCCI clients.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2011 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "qurt.h"
#include "timer.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OS Specific parameters provided by User on client and 
 * Notifier initialization. 
 * NULL is a valid OS parameter for qmi_client_init() or qmi_client_init_instance().
 */
typedef struct {
  /***************************
   *     USER SET MEMBERS    *
   ***************************/

  /* (Optional) Use a provided initialized signal structure.
   * This field is useful for notifier clients (initialized
   * using qmi_client_notifier_init() and will be ignored
   * otherwise) */
  qurt_anysignal_t  *ext_signal;

  /* Event signal mask to be used for events.
   * Set to 0 to let QCCI pick one for you */
  unsigned int      sig;

  /* Timer signal mask to be used for timeout. 
   * Set to 0 to let QCCI pick one for you */
  unsigned int      timer_sig;

  /***************************
   *     PRIVATE MEMBERS     *
   ***************************/
  qurt_anysignal_t  signal;
  timer_type        timer;
  boolean           timer_inited;
  boolean           timed_out;
  boolean           inited;
} qmi_cci_os_signal_type;

typedef qmi_cci_os_signal_type qmi_client_os_params;

#define QMI_CCI_OS_SIGNAL qmi_cci_os_signal_type

extern void qmi_cci_os_signal_deinit
(
  qmi_cci_os_signal_type *sig
);

extern void qmi_cci_os_signal_clear
(
  qmi_cci_os_signal_type *sig
);

extern void qmi_cci_os_signal_wait
(
  qmi_cci_os_signal_type *sig,
  unsigned int timeout_ms
);

extern void qmi_cci_os_signal_set
(
  qmi_cci_os_signal_type *sig
);

/* This is no longer used */
#define QMI_CCI_OS_SIGNAL_INIT(ptr, os_params) \
  do { \
    (ptr)->inited = TRUE;  \
    qurt_anysignal_init(&(ptr)->signal);  \
    (ptr)->timer_inited = FALSE;  \
  }while(0)


#define QMI_CCI_OS_SIGNAL_DEINIT(ptr) qmi_cci_os_signal_deinit(ptr)

#define QMI_CCI_OS_SIGNAL_CLEAR(ptr) qmi_cci_os_signal_clear(ptr)

#define QMI_CCI_OS_SIGNAL_WAIT(ptr, timeout_ms) qmi_cci_os_signal_wait(ptr, timeout_ms)

#define QMI_CCI_OS_SIGNAL_TIMED_OUT(ptr) ((ptr)->timed_out)

#define QMI_CCI_OS_SIGNAL_SET(ptr)  qmi_cci_os_signal_set(ptr)


#ifdef __cplusplus
}
#endif

#endif
