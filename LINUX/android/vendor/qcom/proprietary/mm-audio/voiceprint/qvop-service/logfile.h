/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANDROID_IQVOPLOGFILE_H
#define ANDROID_IQVOPLOGFILE_H

#include <string>

#ifdef QVOP_DEBUG
 #define ATRACE_TAG ATRACE_TAG_ALWAYS

 #include "cutils/trace.h"

 #define QVOPTRACE_BEGIN(str) ATRACE_BEGIN(str)
 #define QVOPTRACE_END() ATRACE_END()
#else  // !QVOP_DEBUG
 #define QVOPTRACE_BEGIN(str)
 #define QVOPTRACE_END()
#endif // QVOP_DEBUG

#define QVOP_LOG_ERROR 6
#define QVOP_LOG_WARN 5
#define QVOP_LOG_INFO 4
#define QVOP_LOG_DEBUG 3
#define QVOP_LOG_VERBOSE 2

#define QVOP_FN logfile_shortfname(__func__).c_str()

extern void logfile_init(bool _printToScreen = true);
extern void logfile_init_platform(bool _printToScreen, bool android);
extern void logfile_print(char const* pFormat, ...);
extern void logfile_print_e(char const* pFormat, ...);
extern void logfile_print_w(char const* pFormat, ...);
extern void logfile_print_i(char const* pFormat, ...);
extern void logfile_print_d(char const* pFormat, ...);
extern void logfile_print_v(char const* pFormat, ...);
extern void logfile_set(uint8_t level);
extern std::string logfile_shortfname(char const* long_fname);
void print_mem(char const* msg, void const* buf_start, size_t len);
void get_trace(const uint32_t key, std::string &msg);

#endif // ANDROID_IQVOPLOGFILE_H
