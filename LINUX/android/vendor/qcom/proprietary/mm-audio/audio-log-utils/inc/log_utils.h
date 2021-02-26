/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef AUDIO_LOG_UTILS_H
#define AUDIO_LOG_UTILS_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef DYNAMIC_LOG_ENABLED
#ifdef ALOGV
#undef ALOGV
#endif
#define ALOGV(format, args...) ALOGD_IF(hal_log_mask[LOG_MASK], format, ##args)
#endif

#include <log_xml_parser.h>
#include <stdbool.h>
#include <cutils/log.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#include <sys/inotify.h>

#define MAX_MODULE_BUF_SIZE 128
#define MAX_MODULE_NAME_LEN  64
#define MAX_MODULES 5

void log_utils_init(void);
void log_utils_deinit(void);

extern int hal_log_mask[];
void register_for_dynamic_logging(char *mod);

#if defined(__cplusplus)
}  /* extern "C" */
#endif

#endif
