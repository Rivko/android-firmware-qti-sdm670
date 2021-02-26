/******************************************************************************
 *      CPU-VOLTAGE-MATCHER.H
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/

#ifndef CPU_VOLTAGE_MATCHER_H
#define CPU_VOLTAGE_MATCHER_H

#ifdef USE_ANDROID_LOG
#define LOG_TAG "CPU-Voltage-Matcher"
#include "cutils/log.h"
#ifdef LOGE
#define msg(format, ...)   LOGE(format, ## __VA_ARGS__)
#define info(format, ...)   LOGI(format, ## __VA_ARGS__)
#else
#define msg(format, ...)   ALOGE(format, ## __VA_ARGS__)
#define info(format, ...)   ALOGI(format, ## __VA_ARGS__)
#endif /* ifdef LOGE */
#else
#define msg(format, ...)   printf(format, ## __VA_ARGS__)
#define info(format, ...)   printf(format, ## __VA_ARGS__)
#endif

#endif
