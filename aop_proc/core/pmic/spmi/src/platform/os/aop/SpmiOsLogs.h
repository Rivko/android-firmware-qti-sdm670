/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/spmi/src/platform/os/aop/SpmiOsLogs.h#1 $
 * $Change: 15485400 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 6/19/14  Abort on fatal log
 * 10/1/13  Initial Version
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include <stdbool.h>
//#include "ULog.h"
#include "CoreVerify.h"
#include "SpmiGenericConsts.h"

#define SPMI_LOG_NAME "Spmi Log"
#define SPMI_LOG_SIZE 128

#ifndef SPMI_IGNORE_FATAL_FLAG
  #define SPMI_IGNORE_FATAL_FLAG 0
#endif
//extern ULogHandle spmiLogHandle;

#define SPMI_OS_LOG_INIT() CORE_VERIFY(1)
/*#define SPMI_OS_LOG_INIT() ULogFront_RealTimeInit(&spmiLogHandle, \
                                                  SPMI_LOG_NAME, \
                                                  SPMI_LOG_SIZE, \
                                                  ULOG_MEMORY_LOCAL, \
                                                  ULOG_LOCK_OS)*/
                                                  
#define GET_COUNT_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,COUNT,...) COUNT
//#define DO_ULOG(...) ULogFront_RealTimePrintf(spmiLogHandle, GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), __VA_ARGS__)
#define DO_ULOG(...) CORE_VERIFY(1)
                                                  
#define SPMI_OS_LOG_FATAL(msg, args...)   do { DO_ULOG("Fatal: "msg, ##args); CORE_VERIFY(SPMI_IGNORE_FATAL_FLAG); } while(FALSE)
#define SPMI_OS_LOG_ERROR(msg, args...)   DO_ULOG("Error: "msg, ##args)
#define SPMI_OS_LOG_WARNING(msg, args...) DO_ULOG("Warn: "msg, ##args)
#define SPMI_OS_LOG_INFO(msg, args...)    DO_ULOG("Info: "msg, ##args)
#define SPMI_OS_LOG_VERBOSE(msg, args...) DO_ULOG("Verbose: "msg, ##args)

#endif
