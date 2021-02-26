/* Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <binder/IBinder.h>
#include <binder/ProcessState.h>

using namespace android;

class death_notifier : public IBinder::DeathRecipient
{
    public:
        death_notifier();
        // DeathRecipient
        virtual void binderDied(const android::wp<IBinder>& who);
};

