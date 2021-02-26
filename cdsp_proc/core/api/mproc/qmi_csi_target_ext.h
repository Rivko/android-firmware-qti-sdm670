#ifndef QMI_CSI_TARGET_EXT_H
#define QMI_CSI_TARGET_EXT_H
/******************************************************************************
  @file    qmi_csi_target_ext.h
  @brief   The QMI Common Service Interface (CSI) QuRT specific Header file.

  DESCRIPTION
  Describes the QuRT specific OS parameters to QCSI
  
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/
#include "comdef.h"
#include "qurt.h"

/** OS Parameters provided by the
 * service to the QCSI framework 
 */
typedef struct
{
  /***************************
   *     USER SET MEMBERS    *
   ***************************/

  /** signal structure which the 
   * service thread shall wait on
   * using qurt_anysignal_wait() */
  qurt_anysignal_t *signal;

  /** Signal mask which should be 
   * set by the QCSI framework 
   * when there is an event to be
   * handled by the service. The 
   * service shall clear the signal 
   * using qurt_anysignal_clear() before
   * handling the event by calling
   * qmi_csi_handle_event() */
  unsigned int     sig;
} qmi_csi_os_params;

#endif
