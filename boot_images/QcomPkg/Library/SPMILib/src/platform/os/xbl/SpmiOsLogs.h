/**
 * @file: SpmiOsLogs.h
 * 
 * @brief: This module implements logging functionality for the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $Change: 14402555 $
 */

#ifndef __SPMI_OS_LOGS_H_
#define __SPMI_OS_LOGS_H_

#include <Library/DebugLib.h>

#define SPMI_LOG_COMPILE_LEVEL SPMI_LOG_LEVEL_VERBOSE

#define SPMI_OS_LOG_FATAL(msg, args...)     DEBUG((EFI_D_ERROR, msg"\n", ##args));
#define SPMI_OS_LOG_ERROR(msg, args...)     DEBUG((EFI_D_INFO, msg"\n", ##args));  
#define SPMI_OS_LOG_WARNING(msg, args...)   DEBUG((EFI_D_INFO, msg"\n", ##args));   
#define SPMI_OS_LOG_INFO(msg, args...)      DEBUG((EFI_D_INFO, msg"\n", ##args));      
#define SPMI_OS_LOG_VERBOSE(msg, args...)   DEBUG((EFI_D_INFO, msg"\n", ##args));   
#define SPMI_OS_LOG_TRACE(msg, args...)     DEBUG((EFI_D_INFO, msg"\n", ##args));   
#define SPMI_OS_LOG_TRACE_RAW(msg, args...) DEBUG((EFI_D_INFO, msg, ##args));   

#endif
