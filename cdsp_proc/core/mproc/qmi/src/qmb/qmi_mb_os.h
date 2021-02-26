#ifndef _QMI_MB_OS_H_
#define _QMI_MB_OS_H_
/******************************************************************************
  @file    qmi_mb_os.h
  @brief   OS Specific routines internal to QMB

  DESCRIPTION
  This header provides an OS abstraction to QMB
  This is a private header

  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "stdlib.h"
#include "stdint.h"
#include "qurt.h"

#define MALLOC malloc
#define CALLOC calloc
#define FREE free

typedef qurt_mutex_t qmi_mb_lock_type;
#define LOCK(ptr)        qurt_pimutex_lock(ptr)
#define UNLOCK(ptr)      qurt_pimutex_unlock(ptr)
#define LOCK_INIT(ptr)   qurt_pimutex_init(ptr)
#define LOCK_DEINIT(ptr) qurt_pimutex_destroy(ptr)

#endif
