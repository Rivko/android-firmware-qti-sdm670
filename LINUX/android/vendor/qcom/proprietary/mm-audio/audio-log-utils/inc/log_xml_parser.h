/*
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef AUDIO_LOG_XML_PARSER_H
#define AUDIO_LOG_XML_PARSER_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <cutils/log.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <poll.h>
#include <sys/inotify.h>
#include <expat.h>
#include <fcntl.h>

#define LOG_XML_PATH "/data/vendor/misc/audio/audio_dynamic_log.xml"

#define BUF_SIZE 1024

struct log_xml_data {
    bool parse_module;
    const char *mod_name;
    int *log_value;
    unsigned int size;
};

int parse_log_xml(const char *module_name, int *log_value, unsigned int size);

#define ARR_SIZE(a) (sizeof(a) / sizeof((a) [0]))

struct StringToEnum {
    const char *name;
    int value;
};

enum {
    HAL_MOD_FILE_AUDIO_HW = 0,
    HAL_MOD_FILE_AUDIO_HW_EXTN_API,
    HAL_MOD_FILE_EDID,
    HAL_MOD_FILE_PLATFORM_INFO,
    HAL_MOD_FILE_VOICE,
    /* audio_extn */
    HAL_MOD_FILE_A2DP,
    HAL_MOD_FILE_ADSP_HDLR,
    HAL_MOD_FILE_AUDIO_EXTN,
    HAL_MOD_FILE_BT_HAL,
    HAL_MOD_FILE_COMPR_CAP,
    HAL_MOD_FILE_COMPR_IN,
    HAL_MOD_FILE_DEV_ARBI,
    HAL_MOD_FILE_DOLBY,
    HAL_MOD_FILE_DTS_EAGLE,
    HAL_MOD_FILE_FM,
    HAL_MOD_FILE_GEF,
    HAL_MOD_FILE_HFP,
    HAL_MOD_FILE_KEEP_ALIVE,
    HAL_MOD_FILE_LISTEN,
    HAL_MOD_FILE_PASSTH,
    HAL_MOD_FILE_PM,
    HAL_MOD_FILE_QAF,
    HAL_MOD_FILE_SND_MONITOR,
    HAL_MOD_FILE_SND_TRIGGER,
    HAL_MOD_FILE_SRC_TRACK,
    HAL_MOD_FILE_SPKR_PROT,
    HAL_MOD_FILE_SSR,
    HAL_MOD_FILE_USB,
    HAL_MOD_FILE_UTILS,
    /* msm8916/msm8974 */
    HAL_MOD_FILE_HW_INFO,
    HAL_MOD_FILE_PLATFORM,
    /* voice_extn */
    HAL_MOD_FILE_COMPR_VOIP,
    HAL_MOD_FILE_VOICE_EXTN,
    HAL_MOD_FILE_MAX
};

int stringToValue(const struct StringToEnum *table, size_t size, const char *name);

#if defined(__cplusplus)
}  /* extern "C" */
#endif

#endif
