/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __MMI_LOG_H_
#define __MMI_LOG_H_
#include <cutils/klog.h>

#define MMI_LOG_SIZE_MAX              (1024 * 2)
#define MMI_LOG_INFO                  "INFO"
#define MMI_LOG_WARNING               "WARNING"
#define MMI_LOG_ERROR                 "ERROR"
#define MMI_LOG_DEBUG                 "DEBUG"
#define MMI_LOG_START                 "START"
#define MMI_LOG_END                   "END"

#define MMI_RESULT(val)               (!(val) ? "success" : "fail")
#define MMI_TEST_RESULT(val)          (!(val) ? "pass" : "fail")
#define MMI_STR(addr)                 (!addr ? "\"\"" : (!addr[0] ? "\"\"" : addr))
#define MMI_CMD_NAME(val)             get_cmd_name(val)
#define MMI_PRI_TYPE(val)             get_print_type(val)

/**Logging into logcat*/
#define MMI_ALOGI(...)                mmi_log_print(MMI_LOG_TYPE_INFO,  __LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define MMI_ALOGD(...)                mmi_log_print(MMI_LOG_TYPE_DEBUG, __LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define MMI_ALOGW(...)                mmi_log_print(MMI_LOG_TYPE_WARN,  __LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)
#define MMI_ALOGE(...)                mmi_log_print(MMI_LOG_TYPE_ERROR, __LINE__, __FILE__, __FUNCTION__, __VA_ARGS__)

/**Logging into dmesg*/
#define MMI_KLOGI(x...)               KLOG_INFO(LOG_TAG,x)
#define MMI_KLOGD(x...)               KLOG_DEBUG(LOG_TAG,x)
#define MMI_KLOGW(x...)               KLOG_WARNING(LOG_TAG,x)
#define MMI_KLOGE(x...)               KLOG_ERROR(LOG_TAG,x)

#define MMI_PROC_TYPE(type)           set_cur_proc_type(type)
#define MMI_FUNC_ENTRY()              MMI_ALOGI(MMI_LOG_START)
#define MMI_FUNC_EXIT()               MMI_ALOGI(MMI_LOG_END)

typedef enum mmi_log_type {
    MMI_LOG_TYPE_MIN = 0,
    MMI_LOG_TYPE_VERBOSE,
    MMI_LOG_TYPE_DEBUG,
    MMI_LOG_TYPE_INFO,
    MMI_LOG_TYPE_WARN,
    MMI_LOG_TYPE_ERROR,
    MMI_LOG_TYPE_MAX,
} MmiLogType;

typedef enum mmi_proc_type {
    MMI_PROC_TYPE_MIN = 0,
    MMI_PROC_TYPE_MMI,
    MMI_PROC_TYPE_MMI_AGENT,
    MMI_PROC_TYPE_MMI_DIAG,
    MMI_PROC_TYPE_MMI_DEBUG,
    MMI_PROC_TYPE_MAX,
} MmiProcType;

typedef struct mmi_int_to_str {
    int integer;
    char *string;
} MmiIntToStr;

void mmi_log_print(int prio, unsigned long line, const char *path, const char *func, const char *fmt, ...);
void set_cur_proc_type(MmiProcType protype);
char *get_cmd_name(int cmd);
char *get_print_type(int msg_id);

#endif
