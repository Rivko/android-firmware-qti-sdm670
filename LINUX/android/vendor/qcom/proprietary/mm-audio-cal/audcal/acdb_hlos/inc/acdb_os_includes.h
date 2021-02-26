/*
 * Copyright (c) 2012, 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ACDB_OS_INCLUDES_H__
#define __ACDB_OS_INCLUDES_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringl.h>
#include <limits.h>
#include "comdef.h"


#ifdef _ANDROID_
/* definitions for Android logging */
#include <utils/Log.h>
#include "common_log.h"
#else /* _ANDROID_ */
#define LOGI(...)      fprintf(stdout,__VA_ARGS__)
#define LOGE(...)      fprintf(stderr,__VA_ARGS__)
#define LOGV(...)      fprintf(stderr,__VA_ARGS__)
#define LOGD(...)      fprintf(stderr,__VA_ARGS__)
#endif /* _ANDROID_ */


#define ACDB_DEBUG_LOG  LOGD
#define ACDB_MALLOC     malloc
#define ACDB_MEM_FREE   free
#define ACDB_MEM_CPY(dst,dst_size,src,src_size)    memcpy((uint8_t *)dst,(uint8_t *)src,src_size)

#define bool_t		bool
#define char_t		char

#endif /* __ACDB_OS_INCLUDES_H__ */
