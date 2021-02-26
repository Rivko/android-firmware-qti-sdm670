/* st_common_defs.h
 *
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef ST_COMMON_DEFS_H
#define ST_COMMON_DEFS_H

#include <stdbool.h>
#include <dlfcn.h>

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#define SOUND_TRIGGER_APE_BUFFER_DURATION_MS (480)
#define SOUND_TRIGGER_PCM_BUFFER_DURATION_MS (160)

#define SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS (240)
#define ST_GRAPHITE_LAB_BUF_DURATION_MS (480)

#define SOUND_TRIGGER_SAMPLING_RATE_16000 (16000)
#define SOUND_TRIGGER_SAMPLING_RATE_48000 (48000)
#define SOUND_TRIGGER_SAMPLING_RATE_384000 (384000)

#define SOUND_TRIGGER_CHANNEL_MODE_MONO (1)
#define SOUND_TRIGGER_CHANNEL_MODE_STEREO (2)
#define SOUND_TRIGGER_CHANNEL_MODE_TRI (3)
#define SOUND_TRIGGER_CHANNEL_MODE_QUAD (4)
#define SOUND_TRIGGER_CHANNEL_MODE_HEX (6)
#define SOUND_TRIGGER_CHANNEL_MODE_OCT (8)

#define SOUND_TRIGGER_BIT_WIDTH  (16)

#define ST_READ_WAIT_TIME_OUT_SEC (2)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SET_BIT(a, b) (a |= b)
#define CLEAR_BIT(a, b) (a &= ~(b))
#define CHECK_BIT(a, b) ((a) & (b))

#define SET_STATE(a, b) SET_BIT(a, b)
#define CLEAR_STATE(a, b) CLEAR_BIT(a, b)
#define CHECK_STATE(a, b) CHECK_BIT(a, b)

#define ALIGN(number, align) \
        ((number + align - 1) & ~(align - 1))
#define CALCULATE_PERIOD_SIZE(duration_ms, sample_rate, period_cnt, align) \
       (ALIGN(((sample_rate * duration_ms) /(period_cnt * 1000)), align))

#define GET_WAIT_TIMESPEC(timeout, wait_time) \
do {\
    const long ns_per_sec = 1000000000L; \
    clock_gettime(CLOCK_MONOTONIC, &timeout); \
    timeout.tv_sec += wait_time / ns_per_sec; \
    timeout.tv_nsec += wait_time % ns_per_sec; \
    if (timeout.tv_nsec >= ns_per_sec) { \
        timeout.tv_sec += 1; \
        timeout.tv_nsec -= ns_per_sec; \
    } \
} while(0)

/* fwk mode definitions */
#define SOUND_TRIGGER_EVENT_NON_TIME_STAMP_MODE (0)
#define SOUND_TRIGGER_EVENT_TIME_STAMP_MODE (1)

#define DLSYM(handle, ptr, symbol, err) \
do {\
    const char* error; \
    *(void**)&ptr = dlsym(handle, #symbol); \
    if ((error = dlerror())) {\
        ALOGE("%s: ERROR. %s", __func__, error);\
        err = -ENODEV;\
    }\
} while(0)

typedef enum {
    ST_DEVICE_HW_NONE,
    ST_DEVICE_HW_APE,
    ST_DEVICE_HW_CPE,
    ST_DEVICE_HW_ARM
}st_hw_type_t;

typedef enum st_exec_mode {
    ST_EXEC_MODE_NONE = -1,
    ST_EXEC_MODE_ADSP,
    ST_EXEC_MODE_CPE,
    ST_EXEC_MODE_ARM,
    ST_EXEC_MODE_MAX
} st_exec_mode_t;

/* defines possible configuration modes for
   execution mode that can be selecet through
   config file */
typedef enum st_exec_mode_config {
   EXEC_MODE_CFG_APE,
   EXEC_MODE_CFG_CPE,
   EXEC_MODE_CFG_DYNAMIC,
   EXEC_MODE_CFG_ARM
} st_exec_mode_config_t;

#endif /* ST_COMMON_DEFS_H */
