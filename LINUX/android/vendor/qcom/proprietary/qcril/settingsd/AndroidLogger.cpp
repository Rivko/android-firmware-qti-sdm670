/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <iostream>
#include "Logger.h"
#include "AndroidLogger.h"
#include "log/log.h"
#define LOG_BUF_SIZE 1024

namespace QcSettingsD {

AndroidLoggerFactory AndroidLoggerFactory::factory;


AndroidLoggerFactory::~AndroidLoggerFactory()
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}
AndroidLoggerFactory::AndroidLoggerFactory(): LoggerFactory("android")
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}

std::shared_ptr<LoggerImpl> AndroidLoggerFactory::getLoggerImpl()
{
    std::shared_ptr<LoggerImpl> ret = NULL;

    DEBUG_LOGGER(">");
    ret = theLogger();
    DEBUG_LOGGER("< (%p)", ret.get());

    return ret;
}

std::shared_ptr<LoggerImpl> AndroidLoggerFactory::theLogger()
{
    DEBUG_LOGGER(">");
    static std::shared_ptr<LoggerImpl> logger = std::make_shared<AndroidLogger>();

    DEBUG_LOGGER("< (%p)", logger.get());
    return logger;
}

AndroidLogger::AndroidLogger(): LoggerImpl(std::string("android"))
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}

static android_LogPriority getAndroidPrio(LogLevel lvl)
{
    DEBUG_LOGGER("> lvl: %d", (int) lvl);
    switch(lvl)
    {
        case LVL_VERBOSE: return ANDROID_LOG_VERBOSE;
        case LVL_DEBUG  : return ANDROID_LOG_DEBUG;
        case LVL_INFO   : return ANDROID_LOG_INFO;
        case LVL_WARN   : return ANDROID_LOG_WARN;
        case LVL_HIGH   : return ANDROID_LOG_ERROR;
        case LVL_FATAL  : return ANDROID_LOG_FATAL;
        case LVL_MAX    : return ANDROID_LOG_SILENT;
        default         : return ANDROID_LOG_DEFAULT;
    }
}
void AndroidLogger::vlog(LogLevel lvl, const char *tag, const char *msg)
{
    DEBUG_LOGGER(">");
    __android_log_buf_write(LOG_ID_RADIO,getAndroidPrio(lvl), tag, msg);
    DEBUG_LOGGER("<");
}

}
