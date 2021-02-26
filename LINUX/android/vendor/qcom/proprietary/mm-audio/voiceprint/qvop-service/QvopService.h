/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QVOPSERVICE_H
#define QVOPSERVICE_H

#include "QvopCommandService.h"

namespace qvop {

    class QvopService {
public:

    QvopService();
    virtual ~QvopService();
    // Interface
    virtual QvopStatus getVersion(IQvopCallback& callback);
    virtual QvopStatus enroll(IQvopCallback& callback,
        const char* user_id,
        const char* identifier,
        const char* phrase);
    virtual QvopStatus match(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus matchWithId(IQvopCallback& callback,
        const char* user_id,
        const char* identifier);
    virtual QvopStatus matchSecure(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus matchAny(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus matchAnyKeyPhrase(IQvopCallback& callback,
        const char* user_id,
        const char* phrase);
    virtual QvopStatus matchAnySecure(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus cancel();
    virtual QvopStatus deleteUser(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus deleteWithId(IQvopCallback& callback,
        const char* user_id, const char* identifier);
    virtual QvopStatus deleteAll(IQvopCallback& callback,
        const char* user_id);
    virtual QvopStatus processFrame(IQvopCallback& callback,
        uint64_t timestamp,
        size_t bufferlen,
        int8_t const * buffer);
    virtual QvopStatus processFrameWithId(IQvopCallback& callback,
        uint64_t timestamp,
        uint64_t frameId,
        size_t bufferlen,
        int8_t const * buffer);
    virtual QvopStatus onStartAudioCapture(IQvopCallback& callback,
        int32_t sampleRate,
        int32_t numberOfChannels,
        int32_t audioFormat);
    virtual QvopStatus onStopAudioCapture(IQvopCallback& callback);
    virtual QvopStatus enrollCaptureStart(IQvopCallback& callback);
    virtual QvopStatus enrollCaptureComplete(IQvopCallback& callback);
    virtual QvopStatus enrollCommit(IQvopCallback& callback);
    virtual QvopStatus matchCaptureComplete(IQvopCallback& callback);
    virtual QvopStatus rename(IQvopCallback& callback,
        const char* user_id,
        const char* old_id,
        const char* new_id);
    virtual QvopStatus setThreshold(IQvopCallback& callback,
        const char* user_id,
        const char* identifier,
        int32_t threshold);

private:
    int32_t mSampleRate;
    int32_t mNumberChannels;
    int32_t mAudioFormat;

    // true if shut down - meaning that the system is shut down.
    bool mIsShutdown;

    // Attempt to init session with secure environment
    QvopStatus tryInitSession();

    // Call to the target command - will call secure environment
    QvopStatus execute(IQvopCallback& callback, QvopCommandService& target);
};

}; // namespace qvop

#endif // QVOPSERVICE_H
