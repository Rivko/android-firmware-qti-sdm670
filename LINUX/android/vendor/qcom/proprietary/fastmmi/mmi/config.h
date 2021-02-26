/*
 * Copyright (c) 2014-2016, Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#ifndef __UTILS_CONFIG__
#define __UTILS_CONFIG__

#include "common.h"
#include "layout.h"
int load_config(const char *path, unordered_map < string, module_info * >*modules,
                list < module_info * >*clients);
int load_layout(const char *docname, layout * g_layout);
#endif
