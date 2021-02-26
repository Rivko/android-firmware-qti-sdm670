/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include <stdint.h>

uint32_t loopback_drTsOpen(const uint32_t width, const uint32_t height);

uint32_t loopback_drTsClose(void);

uint32_t loopback_drTsProcessEvents(void);

