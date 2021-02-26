/*
@file dsc_utils.h
@brief DSC -> DSP Shared Channel
*/
/*===========================================================================
  Copyright (c) 2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //components/rel/core.qdsp6/2.1.c4/securemsm/secdsplib/inc/dsc_utils.h#2 $
  $DateTime: 2018/08/30 07:36:57 $
  $Author: pwbldsvc $

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
===========================================================================*/

#ifndef __DSC_UTILS_H__
#define __DSC_UTILS_H__

#include "stdint.h"
#include "stringl.h"

#define MAX_LOG_MSG_LEN 2048

static char dsc_msg[MAX_LOG_MSG_LEN];
static char dsc_func_name[MAX_LOG_MSG_LEN];
static char dsc_error_msg[MAX_LOG_MSG_LEN];

static inline void _get_func_name(char *full_func_def, uint32_t len)
{
    char *p;
    strlcpy(dsc_func_name, full_func_def, len);
    if ((p = strchr(dsc_func_name, '('))) {
        *p = 0;
    }
}

//TO-DO moving from secure channel specific logging to qurt_printf logging
#define LOG_MSG(fmt, ...)

#define ERROR(fmt, ...)                                                     \
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

#define MAKE_LOG(full_func_def, sub_msg)

#define LOG(full_func_def) {                        \
    MAKE_LOG(full_func_def, "");                    \
    LOG_MSG(msg);                                   \
}                                                   \

#define CHECK_FUNC(rv, full_func_def) {             \
    MAKE_LOG(full_func_def, "failed : %d");			\
    CHECK(rv, dsc_msg, rv)	                            \
}                                                   \

#define CHECK_N_CALL(rv, func) {                    \
    LOG(#func);				                        \
    rv=func;                                        \
    CHECK_FUNC(rv, #func);                          \
}                                                   \

#endif // __DSC_UTILS_H__
