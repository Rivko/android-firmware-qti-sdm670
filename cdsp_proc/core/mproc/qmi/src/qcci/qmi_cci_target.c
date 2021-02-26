/******************************************************************************
  @file    qmi_cci_target.c
  @brief   OS Specific routines internal to QCCI.

  DESCRIPTION
  This source provides an OS abstraction to QCCI.
  
  The QuRT abstraction for QCCI signaling/logging

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/qmi/src/qcci/qmi_cci_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qmi_cci_target.h"

#ifdef FEATURE_QMI_SMEM_LOG
#include "smem_log.h"

#define QMI_CCI_LOG_EVENT_TX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x00)
#define QMI_CCI_LOG_EVENT_TX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x04)
#define QMI_CCI_LOG_EVENT_RX            (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x01)
#define QMI_CCI_LOG_EVENT_RX_EXT        (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x05)
#define QMI_CCI_LOG_EVENT_ERROR         (SMEM_LOG_QMI_CCI_EVENT_BASE + 0x03)
#endif

#define IS_POW_2(val) (((val) & ((val) - 1)) == 0 ? TRUE : FALSE)

size_t strlcpy(char *dst, const char *src, size_t siz);

/*===========================================================================
  FUNCTION  qmi_cci_os_ext_signal_init
===========================================================================*/
/*!
@brief

  Initializes an external signal object (For a notifier)

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_ext_signal_init
(
  qmi_cci_os_signal_type *ptr
)
{
  unsigned int timer_sig;
  if(!ptr || !IS_POW_2(ptr->sig))
  {
    return;
  }
  if(!IS_POW_2(ptr->timer_sig))
  {
    ptr->timer_sig = 0;
  }
  timer_sig = ptr->timer_sig;

  ptr->inited = FALSE;
  ptr->timer_inited = FALSE;
  ptr->timed_out = FALSE;

  if(ptr->ext_signal)
  {
    if(timer_sig)
    {
      if(TE_SUCCESS == timer_def_osal(&ptr->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, ptr->ext_signal, timer_sig))
      {
        ptr->timer_inited = TRUE;
      }
    }
    ptr->inited = TRUE;
    return;
  }
  qurt_anysignal_init(&ptr->signal);
  if(timer_sig)
  {
    if(TE_SUCCESS == timer_def_osal(&ptr->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, &ptr->signal, timer_sig))
    {
      ptr->timer_inited = TRUE;
    }
  }
  ptr->inited = TRUE;
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_init_self
===========================================================================*/
/*!
@brief

  Initialize a signal used internally by QCCI

@return
  None

@note
  Since this is used internally by QCCI we shall use QuRT signaling 
  and ignore any REX specific bits.
*/
/*=========================================================================*/
void qmi_cci_os_signal_init_self
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
)
{
  if(!sig)
  {
    return;
  }
  sig->inited = FALSE;
  sig->timer_inited = FALSE;
  sig->timed_out = FALSE;
  sig->sig = 1;
  sig->timer_sig = 2;
  sig->ext_signal = NULL;
  qurt_anysignal_init(&sig->signal);

  if(TE_SUCCESS == timer_def_osal(&sig->timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, &sig->signal, sig->timer_sig))
  {
    sig->timer_inited = TRUE;
  }
  sig->inited = TRUE;
}

/*===========================================================================
  FUNCTION  qmi_cci_os_copy_os_params
===========================================================================*/
/*!
@brief

  Copy the OS Params provided by the user

@return
  None

@note
  Since this is also for a signal we are creating internal to QCCI, we 
  can go ahead and ignore any REX specific parameters in the OS Params
  and initiate the creation of a pure QuRT signal
*/
/*=========================================================================*/
void qmi_cci_os_copy_os_params
(
  qmi_cci_os_signal_type *sig,
  qmi_cci_os_signal_type *os_params
)
{
  if(sig)
  {
    sig->ext_signal = NULL;
    sig->sig = 1;
    sig->timer_sig = 2;
  }
}


/*===========================================================================
  FUNCTION  qmi_cci_os_signal_deinit
===========================================================================*/
/*!
@brief

  De-initialize a previously initialized signal

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_deinit
(
  qmi_cci_os_signal_type *ptr
)
{
  if(!ptr || !ptr->inited)
  {
    return;
  }

  if(ptr->timer_inited)
  {
    timer_clr_64(&ptr->timer, T_MSEC);
    timer_undef(&ptr->timer);
    ptr->timer_inited = FALSE;
  }
  ptr->inited = FALSE;
  if(!ptr->ext_signal)
  {
    qurt_anysignal_destroy(&ptr->signal);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_clear
===========================================================================*/
/*!
@brief

  Clears a set event

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_clear
(
  qmi_cci_os_signal_type *ptr
)
{
  unsigned int sig_mask;
  if(!ptr || !ptr->inited)
  {
    return;
  }
  sig_mask = ptr->sig | ptr->timer_sig;

  if(ptr->ext_signal)
  {
    qurt_anysignal_clear(ptr->ext_signal, sig_mask);
  }
  else
  {
    qurt_anysignal_clear(&ptr->signal, sig_mask);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_os_signal_set
===========================================================================*/
/*!
@brief

  Sets an event on a signal

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_signal_set
(
  qmi_cci_os_signal_type *ptr
)
{
  if(!ptr || !ptr->inited)
  {
    return;
  }

  if(ptr->ext_signal)
  {
    qurt_anysignal_set(ptr->ext_signal, ptr->sig);
  }
  else
  {
    qurt_anysignal_set(&ptr->signal, ptr->sig);
  }
}


/*===========================================================================
  FUNCTION  qmi_cci_os_signal_wait
===========================================================================*/
/*!
@brief

  Waits for a signal for the specified amount of time

@return
  None

@note
  REX will be used only if supported
*/
/*=========================================================================*/
void qmi_cci_os_signal_wait
(
  qmi_cci_os_signal_type *ptr,
  unsigned int timeout_ms
)
{
  boolean timer_req = FALSE;
  unsigned int wait_mask;

  if(!ptr || !ptr->inited)
  {
    return;
  }

  wait_mask = ptr->sig;

  if(timeout_ms && ptr->timer_inited)
  {
    timer_req = TRUE;
    wait_mask |= ptr->timer_sig;

    /* Start the timer if it is currently inactive */
    if(timer_is_active(&ptr->timer) == FALSE)
    {
      timer_set_64(&ptr->timer, timeout_ms, 0, T_MSEC);
    }
  }

  qurt_anysignal_wait(ptr->ext_signal ? ptr->ext_signal : &ptr->signal, wait_mask);
  if(timer_req && timer_get_64(&ptr->timer, T_MSEC) == 0)
  {
    ptr->timed_out = TRUE;
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_os_log_tx
===========================================================================*/
/*!
@brief

  Logs an outgoing packet

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_log_tx
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
  /* Assume that addr_len is MAX_ADDR_LEN (This needs to change if
   * the common's behavior changes to make this assumption invalid) */
  uint32_t *int_addr = (uint32_t *)(addr);
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_TX_EXT, (cntl_flag << 16) | txn_id, 
                    (msg_id << 16) | msg_len, svc_obj->service_id, 
                    int_addr[0], int_addr[1], int_addr[2]);
#endif
}

/*===========================================================================
  FUNCTION  qmi_cci_os_log_rx
===========================================================================*/
/*!
@brief

  Logs an incoming packet

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_log_rx
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
  /* Assume that addr_len is MAX_ADDR_LEN (This needs to change if
   * the common's behavior changes to make this assumption invalid) */
  uint32_t *int_addr = (uint32_t *)(addr);
    SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_RX_EXT, (cntl_flag << 16) | txn_id, 
                    (msg_id << 16) | msg_len, svc_obj->service_id, 
                    int_addr[0], int_addr[1], int_addr[2]);
#endif
}

/*===========================================================================
  FUNCTION  qmi_cci_os_log_error
===========================================================================*/
/*!
@brief

  Logs an error

@return
  None

@note
  None
*/
/*=========================================================================*/
void qmi_cci_os_log_error
(
  char *filename, 
  unsigned int line
)
{
  uint32 name[5];
  strlcpy((char *)name, filename, sizeof(name));

#ifdef FEATURE_QMI_SMEM_LOG
  SMEM_LOG_EVENT6(QMI_CCI_LOG_EVENT_ERROR, name[0], name[1], name[2], name[3], 
      name[4], line);
#endif
#ifdef FEATURE_QMI_MSG
  MSG_2(MSG_SSID_ONCRPC, MSG_LEGACY_ERROR,
        "Runtime error. File 0x%s, Line: %d", filename, line);
#endif
}

