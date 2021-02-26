/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __XMLPARSE__
#define __XMLPARSE__
#include "common.h"

#define KEY_MMI_AGENT "mmi_agent"
#define KEY_MMI_AGENT32 "mmi_agent32"
#define KEY_MMI_AGENT64 "mmi_agent64"
#define KEY_MMI_DIAG "mmi_diag"
#define KEY_MMI_LIB_BASE "mmi_lib_base"
#define KEY_MMI_LIB64_BASE "mmi_lib64_base"
#define KEY_MMI_SOCKET "mmi_socket"
#define KEY_FTM_AP_DIR "mmi_ftm_ap_dir"
#define KEY_MMI_LOG_DAEMON "mmi_log_daemon"

int parse_strings(char *filepath, unordered_map < string, string > &strings_map);
int parse_strings(char *filepath);
const char *get_value(const char *key);
const char *get_value(string key);
#endif
