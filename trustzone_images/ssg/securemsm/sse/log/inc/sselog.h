/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>

#ifndef min
#define min(a,b) (a)>(b)?(b):(a)
#endif
#ifndef max
#define max(a,b) (a)>(b)?(a):(b)
#endif
#ifndef LENGTH
#define LENGTH(X) (sizeof(X)/sizeof((X)[0]))
#endif

#if defined(SSE_LOG_GLOBAL)
#if defined(SSE_LOG_FILE)
#define SSE_LOG
#endif
#endif

#ifdef MAIN_C
#ifdef LOG_TAG
#define QUOTE(x) #x
#define EXPAND_AND_QUOTE(str) QUOTE(str)
#define SSE_LOG_TAG EXPAND_AND_QUOTE(LOG_TAG)
#endif
const char *sse_log_tag=SSE_LOG_TAG;
#else
extern char *sse_log_tag;
#endif

void _log_x(unsigned int nl, const char* tag0, const char* tag1, const char* fmt, ... );
void dumpHex(const char *tag, const uint8_t *buffer, unsigned int len);

#ifdef SSE_LOG

#define DUMPHEX(tag, buffer, len)     dumpHex(tag, buffer, len)
#define PRINTLOG(buffer, len)         printLog(buffer, len)
#define DUMPBUFFER(tag, buffer, len)  dumpBuffer(tag, buffer, len)


#else /* SSE_LOG not defined */

#define _LOG_x(_x_,...)               do {} while(0)
#define DUMPHEX(tag, buffer, len)     do {} while(0)
#define PRINTLOG(buffer, len)         do {} while(0)
#define DUMPBUFFER(tag, buffer, len)  do {} while(0)

#endif /* SSE_LOG */


#include <qsee_log.h>
#ifdef SSE_LOG
  #define LOG_V(...)  qsee_log(QSEE_LOG_MSG_LOW,  ##__VA_ARGS__);
  #define LOG_D(...)  qsee_log(QSEE_LOG_MSG_DEBUG,##__VA_ARGS__);
  #define LOG_I(...)  qsee_log(QSEE_LOG_MSG_MED,  ##__VA_ARGS__);
  #define LOG_W(...)  qsee_log(QSEE_LOG_MSG_HIGH, ##__VA_ARGS__);
  #ifdef SSE_PROFILING
    #include <qsee_timer.h>
    #define TIME_START  unsigned long long stop, start = qsee_get_uptime()
    #define TIME_STOP   stop = qsee_get_uptime(); delay = stop - start
  #else
    #define TIME_START  do {} while(0)
    #define TIME_STOP   do {} while(0)
  #endif
#else
  #define LOG_V(...) do {} while(0)
  #define LOG_D(...) do {} while(0)
  #define LOG_I(...) do {} while(0)
  #define LOG_W(...) do {} while(0)
  #define TIME_START  do {} while(0)
  #define TIME_STOP   do {} while(0)
#endif
#define LOG_E(...)  qsee_log(QSEE_LOG_MSG_ERROR,__VA_ARGS__);

// helpers
#define ENTER   LOG_D("%s+",__FUNCTION__);TIME_START
#define EXIT    {volatile unsigned long long delay = 0; TIME_STOP; LOG_D("%s- (%llu ms)",__FUNCTION__, delay);} return

#define EXITV(x)   {volatile unsigned long long delay = 0; TIME_STOP; \
                    LOG_D("%s-: %08X (%llu ms)",__FUNCTION__,(x), delay);} return x;

#define ENTERRV   \
                  ENTER; \
                  tciReturnCode_t rv = 0

#define EXITRV    EXITV(rv)

