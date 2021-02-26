/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <msg.h>
#include <msgcfg.h>
#include <diag_lsm.h>
#include <diag/include/log.h>
#include <iostream>
#include "Logger.h"
#include "DiagLogger.h"
#define QCRIL_MAX_LOG_MSG_SIZE                512

namespace QcSettingsD {

DiagLoggerFactory DiagLoggerFactory::factory;


DiagLoggerFactory::~DiagLoggerFactory()
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}
DiagLoggerFactory::DiagLoggerFactory(): LoggerFactory("diag")
{
    DEBUG_LOGGER(">");
    DEBUG_LOGGER("<");
}

std::shared_ptr<LoggerImpl> DiagLoggerFactory::getLoggerImpl()
{
    std::shared_ptr<LoggerImpl> ret = NULL;

    DEBUG_LOGGER(">");
    ret = theLogger();
    DEBUG_LOGGER("< (%p)", ret.get());

    return ret;
}

std::shared_ptr<LoggerImpl> DiagLoggerFactory::theLogger()
{
    DEBUG_LOGGER(">");
    static std::shared_ptr<LoggerImpl> logger = std::make_shared<DiagLogger>();

    DEBUG_LOGGER("< (%p)", logger.get());
    return logger;
}

DiagLogger::DiagLogger(): LoggerImpl("diag")
{
    DEBUG_LOGGER(">");
    boolean ret = Diag_LSM_Init(NULL);
    if (ret) {
        inited = true;
    } else {
        DEBUG_LOGGER("Error initializing DIAG");
    }
    DEBUG_LOGGER("<");
}
DiagLogger::~DiagLogger()
{
    DEBUG_LOGGER(">");
    Diag_LSM_DeInit();
    DEBUG_LOGGER("<");
}

static uint32 getDiagPrio(LogLevel lvl)
{
    DEBUG_LOGGER("> lvl: %d", (int) lvl);
    switch(lvl)
    {
        case LVL_ESSENTIAL: return MSG_LEGACY_HIGH;
        case LVL_VERBOSE: return MSG_LEGACY_LOW;
        case LVL_DEBUG  : return MSG_LEGACY_LOW;
        case LVL_INFO   : return MSG_LEGACY_LOW;
        case LVL_WARN   : return MSG_LEGACY_HIGH;
        case LVL_HIGH   : return MSG_LEGACY_HIGH;
        case LVL_FATAL  : return MSG_LEGACY_FATAL;
        default         : return MSG_LEGACY_LOW;
    }
}
void DiagLogger::vlog(LogLevel lvl, const char *tag, const char *msg)
{
    if(inited)
    {
        MSG_SPRINTF_2( MSG_SSID_ANDROID_QCRIL, getDiagPrio(lvl), "[%s] %s", tag, msg );
    }
}

}
