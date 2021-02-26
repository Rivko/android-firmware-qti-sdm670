/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#include <cutils/properties.h>
#include "QtiVoicePrintService.h"
#include "QtiVoicePrintServiceCallback.h"
#include "QvopCommandGetVersion.h"
#include "QvopCommandDelete.h"
#include "QvopCommandEnrollCommit.h"
#include "QvopCommandEnroll.h"
#include "QvopCommandEnrollTrainComplete.h"
#include "QvopCommandEnrollTrainStart.h"
#include "QvopCommandGetVersion.h"
#include "QvopCommandMatchAny.h"
#include "QvopCommandMatchGetResult.h"
#include "QvopCommandMatch.h"
#include "QvopCommandProcessFrame.h"
#include "QvopCommandRename.h"
#include "QvopCommandSetThreshold.h"
#include "QvopNullCallback.h"
#include "qvop_qsee_interface.h"

using namespace qvop;

#define VALIDATE_VOICEPRINT_ENABLED \
    logfile_print_i("%s: entry", QVOP_FN); \
    if (isVoicePrintEnabled() == Status::FAILURE) { \
        logfile_print_w("%s: voiceprint not enabled, suspending", QVOP_FN); \
        return Status::FAILURE; \
    } \

sp<QtiVoicePrintService> QtiVoicePrintService::sInstance = NULL;
sp<QtiVoicePrintService> QtiVoicePrintService::getInstance() {
    if (sInstance == NULL) {
        sInstance = sp<QtiVoicePrintService>(new QtiVoicePrintService());
    }
    return sInstance;
}

Return<Status> QtiVoicePrintService::getVersion(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandGetVersion cmd;
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::enroll(const sp<IQtiVoicePrintServiceCallback>& cb, const EnrollRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandEnroll cmd(request.user_id.c_str(), request.identifier.c_str(), request.phrase.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::match(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    return matchWithId(cb, request);
}

Return<Status> QtiVoicePrintService::matchWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandMatch cmd(request.user_id.c_str(), request.identifier.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::matchSecure(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    return matchWithId(cb, request);
}

Return<Status> QtiVoicePrintService::matchAny(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    return matchAnyKeyPhrase(cb, request);
}

Return<Status> QtiVoicePrintService::matchAnyKeyPhrase(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandMatchAny cmd(request.user_id.c_str(), request.identifier.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::matchAnySecure(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    return matchAnyKeyPhrase(cb, request);
}

Return<Status> QtiVoicePrintService::cancel() {
    VALIDATE_VOICEPRINT_ENABLED;
    if (QVOP_SUCCESS == QvopSessionEnd()) {
        return Status::SUCCESS;
    }
    else {
        return Status::FAILURE;
    }
}

Return<Status> QtiVoicePrintService::deleteUser(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandDelete cmd(request.user_id.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::deleteWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandDelete cmd(request.user_id.c_str(), request.identifier.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::deleteAll(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandDelete cmd(request.user_id.c_str(), 0, true);
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::processFrame(const sp<IQtiVoicePrintServiceCallback>& cb, const ProcessFrameRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    //request should already have 0 as the frame_id
    return processFrameWithId(cb, request);
}

Return<Status> QtiVoicePrintService::processFrameWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const ProcessFrameRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    size_t modBufLen = request.len;
    QvopCommandProcessFrame cmd(request.timestamp, request.frameId, mSampleRate, mNumberChannels, mAudioFormat, modBufLen, reinterpret_cast<const int8_t *>(request.buffer.data()));
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::onStartAudioCapture(const sp<IQtiVoicePrintServiceCallback>& cb, const StartAudioCaptureRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    mSampleRate = request.sampleRate;
    mNumberChannels = request.numberOfChannels;
    mAudioFormat = request.audioFormat;
    return Status::SUCCESS;
}

Return<Status> QtiVoicePrintService::onStopAudioCapture(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    return Status::SUCCESS;
}

Return<Status> QtiVoicePrintService::enrollCaptureStart(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    return Status::SUCCESS;
}

Return<Status> QtiVoicePrintService::enrollCaptureComplete(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandEnrollTrainComplete cmd;
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::enrollCommit(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandEnrollCommit cmd;
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::matchCaptureComplete(const sp<IQtiVoicePrintServiceCallback>& cb) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandMatchGetResult cmd;
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::rename(const sp<IQtiVoicePrintServiceCallback>& cb, const RenameRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    QvopCommandRename cmd(request.user_id.c_str(), request.old_id.c_str(), request.new_id.c_str());
    return execute(cb, cmd);
}

Return<Status> QtiVoicePrintService::setThreshold(const sp<IQtiVoicePrintServiceCallback>& cb, const ThresholdRequest& request) {
    VALIDATE_VOICEPRINT_ENABLED;
    //not supported?
    return Status::SUCCESS;
}

// verify qvop is enabled
Return<Status> QtiVoicePrintService::isVoicePrintEnabled() {
    logfile_print_v("%s: entry", QVOP_FN);
    char buf[PROPERTY_VALUE_MAX] = {};
    if (property_get("persist.vendor.qvop", buf, "false") >= 0) {
        if (strncmp(buf, "true", sizeof buf) == 0) {
            return Status::SUCCESS;
        }
    }
    return Status::FAILURE;
}

Return<Status> QtiVoicePrintService::execute(sp<IQtiVoicePrintServiceCallback> const& cb, QvopCommandService& target) {
    logfile_print_v("%s: entry", QVOP_FN);
    if (mIsShutdown) {
        logfile_print_w("%s: called when shutdown", QVOP_FN);
        return Status::FAILURE;
    }

    QtiVoicePrintServiceCallback callback(cb);

    //NULL pattern without the factory.
    //qvop::IQvopCallback* iqcb = static_cast<qvop::IQvopCallback*>(callback.get());
    //if (iqcb == NULL) {
    //    QvopNullCallback qcb;
    //    iqcb = &qcb;
    //}

    target.executeCmd(callback);

    //TODO: review the need for the call to clear
    //need to clear the callback to decrement ref count in sp<>
    //param is a const, so const_cast is one way to defeat this problem.
    //
    //sp<IQvopCallback> cb = const_cast<sp<IQvopCallback>&>(callback);
    //cb.clear();

    logfile_print_v("%s: exit", QVOP_FN);
    return Status::SUCCESS;
}
