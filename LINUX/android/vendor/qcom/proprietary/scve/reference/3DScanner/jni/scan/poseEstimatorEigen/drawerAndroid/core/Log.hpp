/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CORE_LOG_HPP
#define CORE_LOG_HPP

#include <memory>

namespace core {

class Log {
public:
    static int32_t initialize(const char *log_path);
    static int32_t terminate();

    Log();
    Log(uint32_t channel);
    ~Log();

    int32_t record(const char *msg_fmt, ...);

private:
    Log(const Log &) = delete;
    Log &operator=(const Log &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
