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

using namespace QcSettingsD;

#ifdef __ANDROID__
#define DEFAULT_ROOT "/vendor/CarrierConfig"
#define PROTO_SOCKET ServiceSocket<ProtoSource>::getAndroid("settingsd")
#else
#define DEFAULT_ROOT "."
#define PROTO_SOCKET ServiceSocket<ProtoSource>::getUnix("settingsd_socket", "settingsd_socket")
#endif
int main() //int argc, char *argv[])
{
    Logger::setDefault("android");
    QCRIL_HAL_LOG_INFO("settingsd starting");

    std::unique_ptr<settingsD> settings = std::unique_ptr<settingsD>(new settingsD(DEFAULT_ROOT));

#ifndef __ANDROID__
    std::unique_ptr<EventSource> cs(new ConsoleSource());
    MainLoop::addEventSource(cs);
#endif

    std::unique_ptr<ServiceSocket<ProtoSource>> ps = PROTO_SOCKET;
    ps->listen(5);
    std::unique_ptr<EventSource> es = std::move(ps);
    MainLoop::addEventSource(es);

    MainLoop::startLoop();
}
