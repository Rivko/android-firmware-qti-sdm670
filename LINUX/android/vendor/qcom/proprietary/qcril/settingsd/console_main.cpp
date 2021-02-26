/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "MainLoop.h"
#include "settingsD.h"
#include <memory>
#include "ConsoleSource.h"
#include "ProtoSource.h"
#include "ServiceSocket.h"
#include "ClientSocket.h"
#include "AndroidLogger.h"
#include "Logger.h"
#include <google/protobuf/stubs/common.h>

using namespace QcSettingsD;

#ifdef __ANDROID__
#define CLIENT_PROTO_SOCKET ClientSocket<ProtoSource>::getUnix("/dev/socket/settingsd")
#else
#define CLIENT_PROTO_SOCKET ClientSocket<ProtoSource>::getUnix("settingsd_socket")
#endif
void myLogHandler(google::protobuf::LogLevel level, const char* filename, int line,
                        const std::string& message)
{
    QCRIL_NOTUSED(level);
    QCRIL_HAL_LOG_INFO("pb: %s @%d: %s", filename, line, message.c_str());
}

int main() //int argc, char *argv[])
{
    google::protobuf::SetLogHandler(myLogHandler);
    Logger::setDefault("android");
    QCRIL_HAL_LOG_INFO("settings console starting");

    std::unique_ptr<EventSource> cs(new ConsoleSource());

    MainLoop::addEventSource(cs);

    std::unique_ptr<ClientSocket<ProtoSource>> cps = CLIENT_PROTO_SOCKET;
    if (!cps->connect())
    {
        std::unique_ptr<EventSource> ces = std::move(cps);
        MainLoop::addEventSource(ces);
    }

    MainLoop::startLoop();
}
