/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]*/
#ifndef _OMX_AENC_LOGGER_H_
#define _OMX_AENC_LOGGER_H_
#include<utils/Log.h>
#ifdef USE_LOG4CXX
#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"
using namespace log4cxx;
using namespace log4cxx::helpers;

#ifndef LOG_TAG
#define LOG_TAG "default_logger."__FILE__
#endif

#endif
#define VARGS_PORTABLE(...) ,##__VA_ARGS__
#ifndef USE_LOG4CXX
#ifndef DEBUG_PRINT
 #define DEBUG_PRINT ALOGV
#endif

#ifndef ALOGE
 #define ALOGE(fmt, ...) \
             do { printf(fmt, ## __VA_ARGS__); } while (0)
#endif


#ifndef DEBUG_PRINT_ERROR
 #define DEBUG_PRINT_ERROR ALOGE
#endif

#ifndef DEBUG_DETAIL
 #define DEBUG_DETAIL ALOGD
#endif
#else
#ifndef DEBUG_PRINT
 #define DEBUG_PRINT(fmt, ...) \
             do {\
               char tmp_buffer[2048];\
                 sprintf(tmp_buffer,fmt,##__VA_ARGS__);\
                 log4cxx::LoggerPtr  tmp_logger(log4cxx::Logger::getLogger(LOG_TAG));\
                     LOG4CXX_DEBUG(tmp_logger, tmp_buffer);\
                     tmp_logger=0;} while (0)
#endif

#ifndef ALOGE
 #define ALOGE(fmt, ...) \
             do {\
               char tmp_buffer[2048];\
                 sprintf(tmp_buffer,fmt,##__VA_ARGS__);\
                 log4cxx::LoggerPtr  tmp_logger(log4cxx::Logger::getLogger(LOG_TAG));\
                     LOG4CXX_ERROR(tmp_logger, tmp_buffer); \
                     tmp_logger = 0;} while (0)
#endif


#ifndef DEBUG_PRINT_ERROR
 #define DEBUG_PRINT_ERROR(fmt, ...) \
             do { ALOGE(fmt, ## __VA_ARGS__); } while (0)
#endif

#ifndef DEBUG_DETAIL
 #define DEBUG_DETAIL(fmt, ...) \
             do {\
               char tmp_buffer[2048];\
                 sprintf(tmp_buffer,fmt,##__VA_ARGS__);\
                 log4cxx::LoggerPtr  tmp_logger(log4cxx::Logger::getLogger(LOG_TAG));\
                     LOG4CXX_TRACE(tmp_logger, tmp_buffer); \
             tmp_logger=0;} while (0)
#endif
#endif

#endif //_OMX_AENC_LOGGER_H_
