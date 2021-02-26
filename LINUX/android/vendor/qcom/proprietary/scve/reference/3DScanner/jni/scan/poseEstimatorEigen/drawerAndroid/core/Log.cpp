/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "core/Log.hpp"

#include <cstdarg>
#include <vector>
#include <string>

#include <pil/File.hpp>
#include <pil/Timer.hpp>

static std::string *gLogPath;

namespace core {

int32_t Log::initialize(const char *log_path) {
    if(nullptr == log_path)
        return 1;

    if(nullptr == gLogPath)
        gLogPath = new std::string;

    *gLogPath = log_path;
    return 0;
}

int32_t Log::terminate() {
    delete gLogPath;
    gLogPath = nullptr;
    return 0;
}

struct Log::Impl {
    uint32_t mChannel;
};

Log::Log() : Log(0) {
}

Log::Log(uint32_t channel) : mImpl(new Impl) {
    mImpl->mChannel = channel;
}

Log::~Log() {
}

int32_t Log::record(const char *msg_fmt, ...) {
    if(nullptr == msg_fmt)
        return 1;

    va_list arguments;
    va_start(arguments, msg_fmt);

    std::vector<char> buffer(BUFSIZ);

    int32_t space_needed = vsnprintf(&buffer[0], BUFSIZ, msg_fmt, arguments);
    if(space_needed < 0)
        return 2;

    if(space_needed >= BUFSIZ) {
        //include '\0' at the end
        buffer.resize(static_cast<uint32_t>(space_needed + 1));
        space_needed = vsnprintf(&buffer[0], BUFSIZ, msg_fmt, arguments);
        if(space_needed < 0)
            return 3;
    }
    va_end(arguments);

    pil::File file(((*gLogPath) + "log.txt").c_str(), true);
    char date_time[BUFSIZ];
    auto size_date_time = snprintf(date_time,
        BUFSIZ, "%.9f %d $ ", pil::time(), mImpl->mChannel);
    file.append(size_date_time, reinterpret_cast<const uint8_t *>(date_time));

    buffer.push_back('\n');
    auto data = reinterpret_cast<const uint8_t *>(buffer.data());
    file.append(static_cast<int32_t>(buffer.size()), data);

    return 0;
}

}
