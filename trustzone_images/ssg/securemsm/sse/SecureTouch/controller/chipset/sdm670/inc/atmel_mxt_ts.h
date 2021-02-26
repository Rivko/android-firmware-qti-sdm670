/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

// Atmel on 8994 implies Liquid
#define DEFAULT_ATMEL_PANEL_WIDTH   3840
#define DEFAULT_ATMEL_PANEL_HEIGHT  2160

uint32_t atmel_drTsOpen(const uint32_t width, const uint32_t height);

uint32_t atmel_drTsClose(void);

uint32_t atmel_drTsProcessEvents(void);

