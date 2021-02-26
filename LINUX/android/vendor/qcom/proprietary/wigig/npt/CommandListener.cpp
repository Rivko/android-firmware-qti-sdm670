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

#include <android-base/logging.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <linux/if.h>

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#include <sysutils/SocketClient.h>

#include "CommandListener.h"

#include <string>
#include <vector>

class LockingFrameworkCommand : public FrameworkCommand {
public:
    LockingFrameworkCommand(FrameworkCommand *wrappedCmd, android::RWLock& lock) :
            FrameworkCommand(wrappedCmd->getCommand()),
            mWrappedCmd(wrappedCmd),
            mLock(lock) {}

    int runCommand(SocketClient *c, int argc, char **argv) {
        android::RWLock::AutoWLock lock(mLock);
        return mWrappedCmd->runCommand(c, argc, argv);
    }

private:
    FrameworkCommand *mWrappedCmd;
    android::RWLock& mLock;
};

GenericCommand::GenericCommand(const char *cmd) :
              FrameworkCommand(cmd)  {
}

void CommandListener::registerLockingCmd(FrameworkCommand *cmd, android::RWLock& lock) {
    registerCmd(new LockingFrameworkCommand(cmd, lock));
}

CommandListener::CommandListener(sp<NetworkPerformanceTuner> npt) :
                 FrameworkListener("wigignpt", true), mNpt(npt) {
    registerLockingCmd(new NptCmd(mNpt));
}

int CommandListener::runNptCommand(int argc, const char **argv,
				   char *resultStr, size_t resultLen)
{
    android::RWLock::AutoWLock lock(mCommandLock);

    return mNpt->runCommand(argc, argv, resultStr, resultLen);
}

CommandListener::NptCmd::NptCmd(sp<NetworkPerformanceTuner> npt) :
                 GenericCommand("perftuner"), mNpt(npt) {
}

int CommandListener::NptCmd::runCommand(SocketClient *cli,
                                        int argc, char **argv) {
    int ret = mNpt->runCommand(cli, argc, argv);
    if (ret)
        LOG(ERROR) << "Perf tuner command failed: " << ret;
    return ret;
}

