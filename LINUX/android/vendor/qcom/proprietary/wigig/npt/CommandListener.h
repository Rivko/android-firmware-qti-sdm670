/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _COMMANDLISTENER_H__
#define _COMMANDLISTENER_H__

#include <sysutils/FrameworkListener.h>
#include <sysutils/FrameworkCommand.h>
#include <utils/StrongPointer.h>
#include <utils/RWLock.h>
#include "NetworkPerformanceTuner.h"

using namespace android;

class GenericCommand : public FrameworkCommand {
public:
    explicit GenericCommand(const char *cmd);
    virtual ~GenericCommand() {}
};

class CommandListener : public FrameworkListener {
public:
    CommandListener(sp<NetworkPerformanceTuner> npt);
    virtual ~CommandListener() {}

    int runNptCommand(int argc, const char **argv, char *resultStr, size_t resultLen);

private:
    android::RWLock mCommandLock;
    sp<NetworkPerformanceTuner> mNpt;
private:
    void registerLockingCmd(FrameworkCommand *cmd, android::RWLock& lock);
    void registerLockingCmd(FrameworkCommand *cmd) {
        registerLockingCmd(cmd, mCommandLock);
    }

    class NptCmd : public GenericCommand {
    public:
        NptCmd(sp<NetworkPerformanceTuner> npt);
        virtual ~NptCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    private:
        sp<NetworkPerformanceTuner> mNpt;
    };
};

#endif /* _COMMANDLISTENER_H__ */
