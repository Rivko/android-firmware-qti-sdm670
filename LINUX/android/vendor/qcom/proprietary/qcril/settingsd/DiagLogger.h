/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef DIAG_LOGGER_H_INCLUDED
#define DIAG_LOGGER_H_INCLUDED

#include "Logger.h"

namespace QcSettingsD
{

class DiagLogger;
class DiagLoggerFactory: virtual public LoggerFactory
{
    public:
    DiagLoggerFactory();
    virtual std::shared_ptr<LoggerImpl> getLoggerImpl();
    virtual ~DiagLoggerFactory();

    protected:

    private:
    DiagLoggerFactory(const DiagLoggerFactory &other) = delete;
    DiagLoggerFactory(DiagLoggerFactory &&other) = delete;
    DiagLoggerFactory &operator= (const DiagLoggerFactory &) = delete;
    DiagLoggerFactory &operator= (DiagLoggerFactory &&) = delete;
    static std::shared_ptr<LoggerImpl> theLogger();
    static DiagLoggerFactory factory;
};

class DiagLogger: virtual public LoggerImpl
{
    friend class DiagLoggerFactory;
    public:
    virtual void vlog(LogLevel lvl, const char *tag, const char *msg);
    DiagLogger();
    ~DiagLogger();

    private:
    bool inited = false;
};

}
#endif
