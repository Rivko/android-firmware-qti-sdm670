/*
 * osal_errorcodes.h
 *
 * This header file defines error numbers to be returned from Graphite
 * Client Service (GCS) to caller. The error numbers are also used among
 * different libraries in GCS.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_ERRORCODES_h__
#define __OSAL_ERRORCODES_h__

/*
 * Errors returned among different modules and also to caller.
 * The values returned are OS specific values, so that caller
 * of GCS would be able to get right error type.
 */

#if defined(_ANDROID_) || defined(LINUX_ENABLED) || defined(_OPEN_WRT_)

/* Error definitions for Android */
#include <errno.h>

#define OSAL_SUCCESS            0
#define OSAL_ENOMEMORY          -ENOMEM
#define OSAL_EUNSUPPORTED       -EOPNOTSUPP
#define OSAL_EBUSY              -EBUSY
#define OSAL_ENOTRECOVERABLE    -ENOTRECOVERABLE
#define OSAL_ENOSYS             -ENOSYS
#define OSAL_EBADPARAM          -EINVAL
#define OSAL_EALREADY           -EALREADY
#define OSAL_EPERM              -EPERM
#define OSAL_ETIMEDOUT          -ETIMEDOUT

#elif defined(_QCET)

#define OSAL_SUCCESS            0
#define OSAL_ENOMEMORY          -1
#define OSAL_EUNSUPPORTED       -2
#define OSAL_EBUSY              -3
#define OSAL_ENOTRECOVERABLE    -4
#define OSAL_ENOSYS             -5
#define OSAL_EBADPARAM          -6
#define OSAL_EALREADY           -7
#define OSAL_EPERM              -8
#define OSAL_ETIMEDOUT          -9

#elif defined(_WIN_OS)

#include <ntstatus.h>

#define OSAL_SUCCESS            STATUS_SUCCESS
#define OSAL_ENOMEMORY          STATUS_MEMORY_NOT_ALLOCATED
#define OSAL_EUNSUPPORTED       STATUS_NOT_SUPPORTED
#define OSAL_EBUSY              STATUS_DEVICE_BUSY
#define OSAL_ENOTRECOVERABLE    STATUS_INTERNAL_ERROR
#define OSAL_ENOSYS             STATUS_NOT_IMPLEMENTED
#define OSAL_EBADPARAM          STATUS_INVALID_PARAMETER
#define OSAL_EALREADY           STATUS_ALREADY_REGISTERED
#define OSAL_EPERM              STATUS_DEVICE_NOT_READY
#define OSAL_ETIMEDOUT          STATUS_TRANSACTION_TIMED_OUT

#endif /* _ANDROID_ */

#endif /* __OSAL_ERRORCODES_h__ */
