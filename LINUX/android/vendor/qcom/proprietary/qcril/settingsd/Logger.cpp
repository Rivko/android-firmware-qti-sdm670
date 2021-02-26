/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "RILQ-Logger"
#include <iostream>
#include <log/log.h>
#include "Logger.h"
#include "commands/Command.h"
extern "C" {
#include <stdarg.h>
#include <sys/prctl.h>
}

using namespace QcSettingsD;

/*
 * LoggerFactory
 */
std::map< std::string, LoggerFactory * > &LoggerFactory::loggerFactories()
{
  static std::map< std::string, LoggerFactory * >lf;

  return lf;
}
LoggerFactory::LoggerFactory(const std::string &name): name(name)
{
    DEBUG_LOGGER("> (\"%s\")",name.c_str());
    if (!LoggerFactory::loggerFactories()[name])
    {
        LoggerFactory::loggerFactories()[name] = this;
    }
    else
    {
        DEBUG_LOGGER("Attempting to add already existing factory");
    }
    DEBUG_LOGGER("<");
}

LoggerFactory::LoggerFactory(const LoggerFactory &o)
{
    DEBUG_LOGGER("> (\"%s\")",o.name.c_str());
    DEBUG_LOGGER("<");
}

LoggerFactory::~LoggerFactory()
{
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    LoggerFactory::loggerFactories().erase(name);
    DEBUG_LOGGER("< (\"%s\")", name.c_str());
}

LoggerFactory *LoggerFactory::getLoggerFactory(const std::string &name)
{
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    LoggerFactory *ret = loggerFactories()[name];
    DEBUG_LOGGER("< (empty? %s)", (ret != nullptr) ? "false" : "true");
    return ret;
}

std::shared_ptr<LoggerImpl> LoggerFactory::getLogger(const std::string &name)
{
    LoggerFactory *fact = LoggerFactory::getLoggerFactory(name);
    std::shared_ptr<LoggerImpl> impl = NULL;
    if (fact)
    {
        impl = fact->getLoggerImpl();
    }
    return impl;
}

/*
 * StdoutLogger
 */
StdoutLogger::StdoutLogger(): LoggerImpl(std::string("stdout"))
{
}

const char *LogLevel_names[] =
{
    [LVL_ESSENTIAL] = "ESSENTIAL ",
    [LVL_VERBOSE]   = "VERBOSE   ",
    [LVL_DEBUG]     = "DEBUG     ",
    [LVL_INFO]      = "INFO      ",
    [LVL_WARN]      = "WARN      ",
    [LVL_HIGH]      = "HIGH      ",
    [LVL_FATAL]     = "FATAL     ",
    [LVL_MAX]       = "MAX       ",
};
void StdoutLogger::vlog(LogLevel lvl, const char *tag, const char *msg)
{
    printf("[%s:%s]: %s\n", LogLevel_names[lvl], tag, msg);
}

/*
 * StdoutLoggerFactory
 */
std::shared_ptr<LoggerImpl> StdoutLoggerFactory::getLoggerImpl()
{
    static std::shared_ptr<LoggerImpl> impl = std::make_shared<StdoutLogger>();
    return impl;
}

/*
 * LoggerImpl
 */
class LoggerImpl::Param
{
    public:
    Param(const std::string &name,
          const std::string &description):
                  name(name), description(description)
    {}
    unsigned id;
    std::string name;
    std::string description;
};
LoggerImpl::LoggerImpl(const std::string &name): name(name)
{
    registerParam("level",
                  "Set the maximum level to log at.");
}

int LoggerImpl::registerParam(const std::string &name,
                              const std::string &description)
{
    /* TODO: Implement */
    QCRIL_NOTUSED(name);
    QCRIL_NOTUSED(description);
    return 0;
}

/*
 * Logger
 */
std::shared_ptr<LoggerImpl> Logger::defaultLogger;
bool Logger::debugLogger = false;
std::string Logger::prefix;
thread_local char Logger::log_buf[1024];

void Logger::log(LogLevel lvl, const char *tag, const char *fmt, ...)
{
    int pref_len = 0;
    va_list ap;

    if (defaultLogger)
    {
        va_start(ap, fmt);
        if (!prefix.empty()) {
            pref_len = prefix.size();
            snprintf(log_buf, sizeof(log_buf), "%s", prefix.c_str());
        }
        vsnprintf(log_buf+pref_len, sizeof(log_buf)-pref_len, fmt, ap);
        defaultLogger->vlog(lvl, tag, log_buf);
        va_end(ap);
    }
}

void Logger::setParams(const std::string &name, ...)
{
    QCRIL_NOTUSED(name);
}

bool Logger::setDefault(const std::string &name)
{
    bool ret = false;
    LoggerFactory *f = LoggerFactory::getLoggerFactory(name);
    std::shared_ptr<LoggerImpl> impl = NULL;
    DEBUG_LOGGER("> (\"%s\")", name.c_str());
    if (f)
    {
        impl = f->getLoggerImpl();
        if (impl)
        {
            log(LVL_INFO, TAG, "Setting logger to %s", name.c_str());
            defaultLogger = std::shared_ptr<LoggerImpl>(impl);
            ret = true;
        }
        else
        {
            log(LVL_HIGH, TAG, "Unable to instantiate logger %s", name.c_str());
        }
    }
    else
    {
        log(LVL_HIGH, TAG, "Unable to find LoggerFactory for logger %s", name.c_str());
    }
    DEBUG_LOGGER("< (ret: %s", ret ? "true" : "false");
    return ret;
}

static Logger theLogger;
void Logger::init()
{
    std::shared_ptr<StdoutLoggerFactory> f = std::make_shared<StdoutLoggerFactory>();
    if (nullptr != f)
    {
        defaultLogger = f->getLoggerImpl();
    }
    else
    {
        fprintf(stderr, "QCRIL Logger Initialization failure");
    }

    std::shared_ptr<Command> commands[] =
    {
        ActionCmd::make_cmd("debug_logger", "Enable debugging of the logger",
            "debug_logger <0|1>",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults &results) -> bool {
                bool ret = false;
                QCRIL_NOTUSED(this);
                QCRIL_NOTUSED(id);
                QCRIL_NOTUSED(command);
                QCRIL_NOTUSED(results);

                if (pars.size() >= 1)
                {
                    const bool debug = pars[0].getBool();
                    QCRIL_HAL_LOG_DEBUG("Calling setDebugLogger with %s", debug?"true":"false");

                    setDebugLogger(debug);
                    ret = true;
                }
                return ret;
            }
        ),
    };

    for(std::shared_ptr<Command> cmd: commands)
    {
        CommandRegistry::registerCommand(cmd->getName(), cmd);
    }
}

#define MAX_THREAD_NAME_SIZE 43
#define MAX_PID_SIZE 23
namespace QcSettingsD {
const char *qcril_get_thread_name()
{
    static thread_local char thread_name[MAX_THREAD_NAME_SIZE];
    prctl(PR_GET_NAME, (unsigned long)thread_name, 0, 0, 0);
    return thread_name;
}
}
