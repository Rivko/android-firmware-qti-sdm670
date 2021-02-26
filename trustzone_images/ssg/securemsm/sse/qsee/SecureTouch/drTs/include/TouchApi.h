/*
 * Copyright (c) 2013,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include <stdint.h>
#include "TouchApiDefs.h"

uint32_t touchOpen(uint32_t *sid, const uint32_t width, const uint32_t height, uint32_t touchController);

uint32_t touchClose(const uint32_t sid);

uint32_t touchGetData(const uint32_t sid, struct tsFingerData *fingers);
