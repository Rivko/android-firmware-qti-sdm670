/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//////////////////////////////////////////////////////////////////////////////////////
//

#include <stdio.h>

#include <map>

#include "QvopService.h"
#include "QvopCommandGetVersion.h"
#include "QvopCommandProcessFrame.h"
#include "QvopCommandMatch.h"
#include "QvopCommandMatchGetResult.h"
#include "QvopCommandEnroll.h"
#include "QvopCommandEnrollTrainStart.h"
#include "QvopCommandEnrollTrainComplete.h"
#include "QvopCommandEnrollCommit.h"
#include "QvopCommandMatchAny.h"
#include "QvopCommandDelete.h"
#include "QvopCommandRename.h"
#include "QvopCommandSetThreshold.h"
#include "QvopNullCallback.h"

namespace qvop {

    QvopService::QvopService() : mSampleRate(AUDIO_DEFAULT_SAMPLE_RATE),
        mNumberChannels(AUDIO_DEFAULT_NUMBER_CHANNELS),
        mAudioFormat(AUDIO_DEFAULT_FORMAT),
        mIsShutdown(false)
    {
        logfile_print_v("%s:QvopService constructed", QVOP_FN);
    }

    QvopService::~QvopService() {
        logfile_print_v("%s:QvopService destructed", QVOP_FN);
    }

    QvopStatus QvopService::getVersion(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandGetVersion cmd;
        return execute(callback, cmd);
    }

    QvopStatus QvopService::enroll(IQvopCallback& callback,
        const char* user_id,
        const char* identifier,
        const char* keyphrase) {

        logfile_print_d("%s: entry user=%s, id=%s, phrase=%s.", QVOP_FN, user_id, identifier, keyphrase);
        QvopCommandEnroll cmd(user_id, identifier, keyphrase);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::setThreshold(IQvopCallback& callback,
                                            const char* user_id,
                                            const char* identifier,
                                            int32_t setThreshold) {
        logfile_print_d("%s: entry user=%s, id=%s, setThreshold=%d.", QVOP_FN, user_id, identifier, setThreshold);
        QvopCommandSetThreshold cmd(user_id, identifier, setThreshold);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::rename(IQvopCallback& callback,
                                    const char* user_id,
                                    const char* old_id,
                                    const char* new_id) {
        logfile_print_d("%s: entry user=%s, old_id=%s, new_id=%s.", QVOP_FN, user_id, old_id, new_id);
        QvopCommandRename cmd(user_id, old_id, new_id);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::enrollCaptureStart(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        return QVOP_SUCCESS;
    }

    QvopStatus QvopService::enrollCaptureComplete(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandEnrollTrainComplete cmd;
        return execute(callback, cmd);
    }

    QvopStatus QvopService::enrollCommit(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandEnrollCommit cmd;
        return execute(callback, cmd);
    }

    QvopStatus QvopService::matchCaptureComplete(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatchGetResult cmd;
        return execute(callback, cmd);
    }

    QvopStatus QvopService::match(IQvopCallback& callback, const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        return matchWithId(callback, user_id, 0);
    }

    QvopStatus QvopService::matchWithId(IQvopCallback& callback,
                                            const char* user_id,
                                            const char* identifier) {

        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatch cmd(user_id, identifier);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::matchSecure(IQvopCallback& callback,
                                            const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatch cmd(user_id, 0);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::matchAny(IQvopCallback& callback,
                                        const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatchAny cmd(user_id, 0);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::matchAnyKeyPhrase(IQvopCallback& callback,
                                                const char* user_id,
                                                const char* phrase) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatchAny cmd(user_id, 0);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::matchAnySecure(IQvopCallback& callback,
                                            const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandMatchAny cmd(user_id, 0);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::deleteUser(IQvopCallback& callback,
                                        const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandDelete cmd(user_id);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::deleteWithId(IQvopCallback& callback,
                                            const char* user_id,
                                            const char* identifier) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandDelete cmd(user_id, identifier, false);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::deleteAll(IQvopCallback& callback,
                                        const char* user_id) {
        logfile_print_v("%s: entry", QVOP_FN);
        QvopCommandDelete cmd(user_id, 0, true);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::processFrame(IQvopCallback& callback,
            uint64_t timestamp,
            size_t bufferlen,
            int8_t const * buffer) {

        //arbitrary frameId and call overloaded method
        uint64_t frameId = 0;
        return processFrameWithId(callback, timestamp, frameId, bufferlen, buffer);
    }

    QvopStatus QvopService::processFrameWithId(IQvopCallback& callback,
            uint64_t timestamp,
            uint64_t frameId,
            size_t bufferlen,
            int8_t const * buffer) {

        logfile_print_v("%s: entry ts: %ul, frameId: %ul, bufferlen=%d", QVOP_FN, timestamp, frameId, bufferlen);
        QvopCommandProcessFrame cmd(timestamp, frameId, mSampleRate, mNumberChannels, mAudioFormat, bufferlen, buffer);
        return execute(callback, cmd);
    }

    QvopStatus QvopService::onStartAudioCapture(IQvopCallback& callback,
                                                    int32_t sampleRate,
                                                    int32_t numberOfChannels,
                                                    int32_t audioFormat) {
        logfile_print_v("%s: entry sampleRate=%d, numberOfChannels=%d, audioFormat=%d", QVOP_FN, sampleRate, numberOfChannels, audioFormat);

        mSampleRate = sampleRate;
        mNumberChannels = numberOfChannels;
        mAudioFormat = audioFormat;

        return QVOP_SUCCESS;
    }

    QvopStatus QvopService::onStopAudioCapture(IQvopCallback& callback) {
        logfile_print_v("%s: entry", QVOP_FN);
        return QVOP_SUCCESS;
    }

    QvopStatus QvopService::execute(IQvopCallback& callback,
                                    QvopCommandService& target) {
        logfile_print_v("%s: entry", QVOP_FN);
        if (mIsShutdown) {
            logfile_print_w("%s: called when shutdown", QVOP_FN);
            return QVOP_FAILURE;
        }

        /*target is one of the derived classes, e.g. QvopCommandMatch*/
        target.executeCmd(callback);

        logfile_print_v("%s: exit.", QVOP_FN);
        return QVOP_SUCCESS;
    }

    QvopStatus QvopService::cancel() {
        logfile_print_v("%s: entry", QVOP_FN);

        QvopStatus res = QvopSessionEnd();
        if (QVOP_SUCCESS != res) {
            logfile_print_v("QvopSessionEnd failed");
        }
        else {
            logfile_print_d("QvopSessionEnd success");
        }
        logfile_print_v("%s: ret=%d", QVOP_FN, res);

        return QVOP_SUCCESS;
    }


}; // namespace qvop
