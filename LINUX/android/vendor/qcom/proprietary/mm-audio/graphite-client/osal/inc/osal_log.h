/*
 * osal_log.h
 *
 * This file contains logging helper functions for GCS.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_LOG_H__
#define __OSAL_LOG_H__

#if defined(_ANDROID_)

/* Definitions for Android logging */
#include <utils/Log.h>
#include <cutils/properties.h>

#define OSAL_LOGE   ALOGE
#define OSAL_LOGD   ALOGD
#define OSAL_LOGI   ALOGI
#define OSAL_LOGV   ALOGV

#elif defined(_QCET)

#define OSAL_LOGE(...)   fprintf(stderr, __VA_ARGS__)
#define OSAL_LOGD(...)   fprintf(stderr, __VA_ARGS__)
#define OSAL_LOGI(...)   fprintf(stdout, __VA_ARGS__)
#define OSAL_LOGV(...)   fprintf(stderr, __VA_ARGS__)

#elif defined(_WIN_OS)

#define OSAL_LOGE(...)   fprintf(stderr, __VA_ARGS__)
#define OSAL_LOGD(...)   fprintf(stderr, __VA_ARGS__)
#define OSAL_LOGI(...)   fprintf(stdout, __VA_ARGS__)
#define OSAL_LOGV(...)   fprintf(stderr, __VA_ARGS__)

#elif defined(_OPEN_WRT_)

#include <unistd.h>
#include <stdio.h>

#define OSAL_LOGE(...)   fprintf(stderr, __VA_ARGS__)
#define OSAL_LOGD(...)
#define OSAL_LOGI(...)
#define OSAL_LOGV(...)

#endif /* _ANDROID_ */

#endif /* __OSAL_LOG_H__ */
