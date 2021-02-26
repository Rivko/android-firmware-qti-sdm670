/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "pil/Timer.hpp"

#include <ctime>

namespace pil {

double time() {
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return res.tv_sec + res.tv_nsec * 1.0e-9;
}

}
