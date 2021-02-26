/*
@file utils.h
@brief This file contains all definitions used in ipprotector
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/apps.tz/2.0.2/securemsm/trustzone/qsapps/secdspdemo/utils.h#1 $
  $DateTime: 2018/08/09 06:10:01 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __UTILS_H__
#define __UTILS_H__

#include "qsee_log.h"
#include "stdint.h"
#include "stringl.h"

#define MAX_LOG_MSG_LEN 2048
#define SUCCESS         0
#define FAILED          1

static char msg[MAX_LOG_MSG_LEN];
static char func_name[MAX_LOG_MSG_LEN];
static char error_msg[MAX_LOG_MSG_LEN];

static inline void _get_func_name(char *full_func_def, uint32_t len)
{
    char *p;
    strlcpy(func_name, full_func_def, len);
    if ((p = strchr(func_name, '('))) {
        *p = 0;
    }
}

// 120 is limit imposed by qsee_log where 16 chars are needed for app_name and other things
#define LOG_MSG(fmt, ...)                                                                               \
    if (strlen(fmt) > (120 - 16)) {                                                                     \
        qsee_log(QSEE_LOG_MSG_ERROR,                                                                    \
                "WARNING: final fmt (len : %d) is over 120 chars, it will be truncated!",               \
                strlen(fmt) + 16);                                                                      \
    }                                                                                                   \
    qsee_log(QSEE_LOG_MSG_DEBUG, fmt, ##__VA_ARGS__);                                                   \

#define ERROR(fmt, ...)                                                     \
    snprintf(error_msg, MAX_LOG_MSG_LEN, "%s %s", "ERROR:", fmt);           \
    LOG_MSG(error_msg, ##__VA_ARGS__);                                      \
    goto end;                                                               \

#define CHECK(rv, ...)                              \
    if (rv != SUCCESS) {                            \
        ERROR(__VA_ARGS__);                         \
    }                                               \

#define CHECK_COND(cond, rv, error_code, ...)       \
    if (cond) {                                     \
        rv=error_code;                              \
        ERROR(__VA_ARGS__);                         \
    }                                               \

#define MAKE_LOG(full_func_def, sub_msg)                                \
    _get_func_name(full_func_def, MAX_LOG_MSG_LEN);                     \
    snprintf(msg, MAX_LOG_MSG_LEN, "%s %s", func_name, sub_msg);        \

#define LOG(full_func_def) {                        \
    MAKE_LOG(full_func_def, "");                    \
    LOG_MSG(msg);                                   \
}                                                   \

#define CHECK_FUNC(rv, full_func_def) {             \
    MAKE_LOG(full_func_def, "failed : %d");			\
    CHECK(rv, msg, rv)	                            \
}                                                   \

#define CHECK_N_CALL(rv, func) {                    \
    LOG(#func);				                        \
    rv=func;                                        \
    CHECK_FUNC(rv, #func);                          \
}                                                   \

#endif // __UTILS_H__
