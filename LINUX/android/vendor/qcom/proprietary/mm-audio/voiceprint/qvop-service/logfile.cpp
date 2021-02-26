/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <map>
#include <string>
#include <unistd.h>

#define _ftime ftime
#define _access access
#define __cdecl

static char const* accessFileName = "logfileON";
static char const* logFileName = "logfile";

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "QVOP"

using namespace std;

//trace messages that map to cmd ids
//not in use
static const pair<uint32_t, string> trace_msgs[] = {
  /*  make_pair(QVOP_CMD_GET_VERSION, "QVOP_CMD_GET_VERSION"), */
};

static const map<uint32_t, string> trace_data(trace_msgs,
    trace_msgs + sizeof trace_msgs / sizeof trace_msgs[0]);

// ========================================================

#include "logfile.h"

#include <android/log.h>
#include <cutils/properties.h>


static bool g_logToFile = false;
static bool printToScreen = true;
static bool useAndroid = true;
static uint8_t logLevel = QVOP_LOG_INFO;

void __cdecl logfile_init(bool _printToScreen) {
  FILE* fp = fopen(accessFileName, "r");
  g_logToFile = (fp != 0);
  if (fp != 0) {
    fclose(fp);
  }
  printToScreen = _printToScreen;

}

void __cdecl logfile_init_platform(bool _printToScreen, bool android) {

    logfile_init(_printToScreen);
    useAndroid = android;
}

void __cdecl logfile_set(uint8_t level) {
    if (level <= QVOP_LOG_ERROR && level >= QVOP_LOG_VERBOSE){
        logLevel = level;
    }
}

static void vlogfile_print(uint8_t priority, char const* pFormat, va_list pArg) {
    // filter out debug log level if the log level is not set
    if (priority < logLevel) {
        return;
    }

    if (g_logToFile || printToScreen) {

        // format log message
        char buf[256];
        vsnprintf(buf, sizeof buf, pFormat, pArg);

        if (printToScreen && useAndroid) {
            __android_log_print(priority, "QVOP", "%s", buf);
        }
        else {
            printf("%s\n", buf);
        }

        if (g_logToFile) {
            //tbd
        }
    }
}

void __cdecl logfile_print_e(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_ERROR, pFormat, pArg);
    va_end(pArg);
}

void __cdecl logfile_print_w(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_INFO, pFormat, pArg);
    va_end(pArg);
}

void __cdecl logfile_print_i(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_WARN, pFormat, pArg);
    va_end(pArg);
}

void __cdecl logfile_print_d(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_DEBUG, pFormat, pArg);
    va_end(pArg);
}

void __cdecl logfile_print_v(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_VERBOSE, pFormat, pArg);
    va_end(pArg);
}

// backwards compatible with existing code
void __cdecl logfile_print(char const* pFormat, ...) {
    va_list pArg;
    va_start(pArg, pFormat);
    vlogfile_print(QVOP_LOG_VERBOSE, pFormat, pArg);
    va_end(pArg);
}

// Extracts the short function name from a __func__ string
string logfile_shortfname(char const* long_fname) {
  string name(long_fname = 0 ? "unknown" : long_fname);

  // a long function name has the form of "void my_function_name(char*, ...)"
  // or "void my_class::my_function_name(char*, ...)"

  // find end of function name
  size_t pos = name.find('(');
  if (pos != string::npos) {
    name.erase(pos);
  }

  // find begining of qualified function name
  pos = name.rfind(' ');
  if (pos != string::npos) {
    name.erase(0, pos + 1);
  }

  return name;
}

void print_mem(char const* msg, void const* buf_start, size_t len) {
  char buf[50] = {};  // arbitrary size, fits on one logcat line
  size_t remaining = sizeof buf;
  uint8_t const* data = static_cast<uint8_t const*>(buf_start);
  for (size_t i = 0; i < len && remaining > 0; ++i) {
    remaining -= snprintf(buf + sizeof buf - remaining, remaining, "%02x", data[i]);
  }

  logfile_print("%s: %s", msg, buf);
}

void get_trace(const uint32_t key, string &msg) {
  map<uint32_t, string>::const_iterator it = trace_data.find(key);
  if (it != trace_data.end()) {
    msg.append(it->second);
  } else {
    char keyStr[40];
    snprintf(keyStr, sizeof keyStr, "QVOP_CMD_%d", key);
    msg.append(keyStr);
  }
}

