/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

// Synaptics on 8994 implies Fluid
#define DEFAULT_SYNAPTICS_PANEL_WIDTH   1600
#define DEFAULT_SYNAPTICS_PANEL_HEIGHT  2704

uint32_t synaptics_drTsOpen(const uint32_t width, const uint32_t height);

uint32_t synaptics_drTsClose(void);

uint32_t synaptics_drTsProcessEvents(void);
