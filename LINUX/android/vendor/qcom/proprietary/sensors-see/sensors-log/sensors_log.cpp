/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "sensors_log.h"

namespace sensors_log
{

struct log_config
{
    log_level level = INFO;
    const char *tag = "sensors";
    bool stderr_logging = false;
};

static log_config config;

log_level get_level()
{
    return config.level;
}

void set_level(log_level level)
{
    config.level = level;
}

const char *get_tag()
{
    return config.tag;
}

void set_tag(char const *tag)
{
    config.tag = tag;
}

bool get_stderr_logging()
{
    return config.stderr_logging;
}

void set_stderr_logging(bool val)
{
    config.stderr_logging = val;
}

}



