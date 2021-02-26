/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GARDEN_UTIL_H
#define GARDEN_UTIL_H

#include <inttypes.h>
#include <iostream>
#include <unistd.h>
#include <loc_pla.h>
#include <log_util.h>
#include "IGardenCase.h"

namespace garden {

#define GARDEN_HEAD(fmt) "%s:%d] " fmt

#define gardenError(fmt, ...) \
    fprintf(stderr, GARDEN_HEAD(fmt), __FILE__, __LINE__, ##__VA_ARGS__); \
    LOC_LOGE(GARDEN_HEAD(fmt), __FILE__, __LINE__, ##__VA_ARGS__); \

#define gardenPrint(fmt, ...) \
    fprintf(stdout, fmt "\n", ##__VA_ARGS__); \
    LOC_LOGV(fmt, ##__VA_ARGS__)

#define menuPrint(fmt, ...) \
    fprintf(stdout, fmt, ##__VA_ARGS__)

#define GARDEN_ERROR(fmt, ...) \
{ \
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ASSERT_EQ(val, exp, fmt, ...) \
if (val != exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ASSERT_NEQ(val, exp, fmt, ...) \
if (val == exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_FAILED; \
}

#define GARDEN_ABORT_EQ(val, exp, fmt, ...) \
if (val != exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_ABORT; \
}

#define GARDEN_ABORT_NEQ(val, exp, fmt, ...) \
if (val == exp) {\
    gardenError(fmt, ##__VA_ARGS__); \
    return ::garden::GARDEN_RESULT_ABORT; \
}

uint64_t getRealTimeNsec();
uint64_t getUpTimeSec();
} // namespace garden
#endif // GARDEN_UTIL_H
