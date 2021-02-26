#ifndef SMP2P_QDI_DRIVER_H
#define SMP2P_QDI_DRIVER_H
/*===========================================================================

                            SMP2P QDI Driver Header

  @file
    smp2p_qdi_driver.h

  This is the QDI Driver header for SMP2P in the ADSP Guest OS.

  Copyright (c) 2014, 2016-2017 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/smp2p/inc/smp2p_qdi_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/14   bc      Initial version of QDI implementation for SMP2P driver.
==========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "qurt_qdi.h"
#include "smp2p_os.h"
#include "dll_global_def.h"

/*===========================================================================
                   CONSTANT AND MACRO DECLARATIONS
===========================================================================*/
#ifdef DLL_API_GLOBAL
#define SMP2P_EXPORT DLL_API_GLOBAL
#else
#define SMP2P_EXPORT
#endif

/* Define SMP2P QDI method IDs */
#define SMP2P_QDI_INIT              (QDI_OPEN)
#define SMP2P_QDI_DEINIT            (QDI_CLOSE)

/* Custom QDI methods for the SMP2P driver.  QDI_PRIVATE == 256. */
#define SMP2P_QDI_CREATE            (QDI_PRIVATE + 0)
#define SMP2P_QDI_WRITE             (QDI_PRIVATE + 1)
#define SMP2P_QDI_MODIFY            (QDI_PRIVATE + 2)
#define SMP2P_QDI_MODIFY_EX         (QDI_PRIVATE + 3)
#define SMP2P_QDI_REGISTER          (QDI_PRIVATE + 4)
#define SMP2P_QDI_READ              (QDI_PRIVATE + 5)
#define SMP2P_QDI_GET_SIGNAL_LIST   (QDI_PRIVATE + 6)
/*
#define SMP2P_QDI_WAIT           (QDI_PRIVATE + 6)
#define SMP2P_QDI_WAIT_MULTIPLE  (QDI_PRIVATE + 7)
*/
#define SMP2P_QDI_GET_EVENT         (QDI_PRIVATE + 6)
#define SMP2P_QDI_GET_SIGNAL        (QDI_PRIVATE + 7)
#define SMP2P_QDI_GET_GROUP_SIGNAL  (QDI_PRIVATE + 8)

/* Error and return defines.  0 should always be success, < 0 are errors. */
#define SMP2P_QDI_STATUS_SUCCESS           (0)
#define SMP2P_QDI_STATUS_FAILURE           (-1)
#define SMP2P_QDI_STATUS_INVALID_PARAM     (-2)
#define SMP2P_QDI_STATUS_NOT_INIT          (-3)
#define SMP2P_QDI_STATUS_OUT_OF_RESOURCES  (-4)
#define SMP2P_QDI_STATUS_DUPLICATE         (-5)
#define SMP2P_QDI_STATUS_NOT_FOUND         (-6)
#define SMP2P_QDI_STATUS_PERM_ERR          (-7)
#define SMP2P_QDI_STATUS_SIG_ERR           (-8)

#define SMP2P_QDI_DEV_NAME        "/dev/smp2p"

/*==============================================================================
                       PUBLIC FUNCTION DEFINITIONS
==============================================================================*/
/*==============================================================================
  FUNCTION  smp2p_qdi_signal_send_event
==============================================================================*/
/**

  Notify userspace client with QDI signal

  param[in]   entry_name        Name of smp2p entry

  @return
  SMP2P_QDI_STATUS_SUCCESS    Success to notify
  SMP2P_QDI_STATUS_FAILURE    Failed to notify
*/
/*============================================================================*/
int smp2p_qdi_signal_send_event
(
  smp2p_qdi_context_type *qdi_context
);

#endif /* SMP2P_QDI_DRIVER_H */