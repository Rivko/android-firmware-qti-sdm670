/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#pragma once

#include <cstdio>
#include <cutils/log.h>

namespace sensors_log
{

enum log_level
{
    SILENT = 0,
    ERROR = 1,
    INFO = 2,
    DEBUG = 3,
    VERBOSE = 4,
};

/**
 * @brief sets global log level for current process
 * @param level
 */
void set_level(log_level level);

/**
 * @brief get current log tag
 * @return const char*
 */
const char *get_tag();

/**
 * @brief set log tag
 * @param tag
 */
void set_tag(const char *tag);

/**
 * @brief get current log level value
 * @return log_level
 */
log_level get_level();

/**
 * @brief enable/disable logging over stderr
 * @param enable
 */
void set_stderr_logging(bool val);

/**
 * @brief get current status of stderr logging
 * @return bool
 */
bool get_stderr_logging();

}

#define __sns_log_message(prio, level, fmt, args...) do { \
    if (sensors_log::get_level() >= level) { \
        __android_log_print(prio, sensors_log::get_tag(), "%s:%d, " fmt, \
                            __FUNCTION__, __LINE__, ##args); \
        if (sensors_log::get_stderr_logging() == true) { \
            fprintf(stderr, "%s:%d, " fmt "\n", __FUNCTION__, __LINE__, \
                    ##args); \
        } \
    } \
} while (0)

#define sns_logv(fmt, args...) do { \
    __sns_log_message(ANDROID_LOG_VERBOSE, sensors_log::VERBOSE, fmt, ##args); \
} while (0)

#define sns_logd(fmt, args...) do { \
    __sns_log_message(ANDROID_LOG_DEBUG, sensors_log::DEBUG, fmt, ##args); \
} while (0)

#define sns_logi(fmt, args...) do { \
    __sns_log_message(ANDROID_LOG_INFO, sensors_log::INFO, fmt, ##args); \
} while (0)

#define sns_loge(fmt, args...) do { \
    __sns_log_message(ANDROID_LOG_ERROR, sensors_log::ERROR, fmt, ##args); \
} while (0)
