/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "common.h"

static int g_proc_type = MMI_PROC_TYPE_MIN;

static const char *g_proc_name[] = { "", "mmi", "mmi_agent", "mmi_diag", "mmi_debug", ""
};

static const MmiIntToStr g_cmd_type[] = { {CMD_INIT, "CMD_INIT"},   // 0x01
{CMD_DEINIT, "CMD_DEINIT"},     // 0x02
{CMD_QUERY, "CMD_QUERY"},       // 0x03
{CMD_RUN, "CMD_RUN"},           // 0x04
{CMD_STOP, "CMD_STOP"},         // 0x05
{CMD_NV_READ, "CMD_NV_READ"},   // 0x06
{CMD_NV_WRITE, "CMD_NV_WRITE"}, // 0x07
{CMD_RESULT, "CMD_RESULT"},     // 0x08
{CMD_PRINT, "CMD_PRINT"},       // 0x101
{CMD_HELLO, "CMD_HELLO"},       // 0x103
{CMD_CTRL, "CMD_CTRL"},         // 0x104
{-1, NULL},                     // invalid
};

static const MmiIntToStr g_print_type[] = { {PRINT, "PRINT"},   // 0x00
{DATA, "DATA"},                 // 0x01
{PRINT_DATA, "PRINT_DATA"},     // 0x02
{-1, NULL},                     // invalid
};

static char *get_cmd_str(int cmd, const MmiIntToStr * tbl) {
    const MmiIntToStr *p = tbl;

    while(-1 != p->integer && NULL != p->string) {
        if(cmd == p->integer) {
            return p->string;
        }
        p++;
    }
    return "null";
}

char *get_cmd_name(int cmd) {
    return get_cmd_str(cmd, g_cmd_type);
}

char *get_print_type(int msg_id) {
    return get_cmd_str(msg_id, g_print_type);
}

void set_cur_proc_type(MmiProcType protype) {
    if(MMI_PROC_TYPE_MIN > protype || MMI_PROC_TYPE_MAX < protype) {
        g_proc_type = MMI_PROC_TYPE_MIN;
    } else {
        g_proc_type = protype;
    }
}


#ifdef ANDROID   // android log
void mmi_log_print(int prio, unsigned long line, const char *path, const char *func, const char *fmt, ...) {
    int len;

    va_list ap;
    char buf[MMI_LOG_SIZE_MAX] = { 0 };
    if(MMI_LOG_TYPE_INFO == prio) {
        snprintf(buf, sizeof(buf), "%s, %s(): ", g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_DEBUG == prio) {
        snprintf(buf, sizeof(buf), "%s, %s(): ", g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_WARN == prio) {
        snprintf(buf, sizeof(buf), "%s: %s,%s(): ", MMI_LOG_WARNING, g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_ERROR == prio) {
        const char *file = strrchr(path, '/');

        file = (!file ? path : (file + 1));
        snprintf(buf, sizeof(buf), "%s: %s,%s,%ld,%s(): ", MMI_LOG_ERROR, g_proc_name[g_proc_type], file, line, func);
    } else {
    }
    len = strlen(buf);
    va_start(ap, fmt);
    vsnprintf(buf + len, MMI_LOG_SIZE_MAX - len, fmt, ap);
    va_end(ap);
    switch (prio) {
    case MMI_LOG_TYPE_INFO:
        ALOGI("%s", buf);
        break;
    case MMI_LOG_TYPE_DEBUG:
        ALOGD("%s", buf);
        break;
    case MMI_LOG_TYPE_WARN:
        ALOGW("%s", buf);
        break;
    case MMI_LOG_TYPE_ERROR:
        ALOGE("%s", buf);
        break;
    default:
        break;
    }
    return;
}


#else // linux log
#include <stdio.h>
void mmi_log_print(int prio, unsigned long line, const char *path, const char *func, const char *fmt, ...) {
    int len;

    va_list ap;
    char buf[MMI_LOG_SIZE_MAX] = { 0 };
    if(MMI_LOG_TYPE_INFO == prio) {
        snprintf(buf, sizeof(buf), "%s, %s(): ", g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_DEBUG == prio) {
        snprintf(buf, sizeof(buf), "%s, %s(): ", g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_WARN == prio) {
        snprintf(buf, sizeof(buf), "%s: %s,%s(): ", MMI_LOG_WARNING, g_proc_name[g_proc_type], func);
    } else if(MMI_LOG_TYPE_ERROR == prio) {
        const char *file = strrchr(path, '/');

        file = (!file ? path : (file + 1));
        snprintf(buf, sizeof(buf), "%s: %s,%s,%ld,%s(): ", MMI_LOG_ERROR, g_proc_name[g_proc_type], file, line, func);
    } else {
    }
    len = strlen(buf);
    va_start(ap, fmt);
    vsnprintf(buf + len, MMI_LOG_SIZE_MAX - len, fmt, ap);
    va_end(ap);
    switch (prio) {
    case MMI_LOG_TYPE_INFO:
    case MMI_LOG_TYPE_DEBUG:
    case MMI_LOG_TYPE_WARN:
    case MMI_LOG_TYPE_ERROR:
        fprintf(stderr, "%s", buf);
        break;
    default:
        break;
    }
    return;
}
#endif                          /*  */
