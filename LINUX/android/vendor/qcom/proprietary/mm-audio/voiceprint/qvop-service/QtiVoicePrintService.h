/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef QTI_VOICEPRINT_SERVICE_H
#define QTI_VOICEPRINT_SERVICE_H

#include <vendor/qti/voiceprint/1.0/IQtiVoicePrintService.h>
#include <vendor/qti/voiceprint/1.0/IQtiVoicePrintServiceCallback.h>
#include <vendor/qti/voiceprint/1.0/types.h>
#include <hardware/hw_auth_token.h>
#include "QvopCommandService.h"

using ::vendor::qti::voiceprint::V1_0::IQtiVoicePrintService;
using ::vendor::qti::voiceprint::V1_0::IQtiVoicePrintServiceCallback;
using ::vendor::qti::voiceprint::V1_0::Status;
using ::vendor::qti::voiceprint::V1_0::ErrorResult;
using ::vendor::qti::voiceprint::V1_0::MatchRequest;
using ::vendor::qti::voiceprint::V1_0::EnrollRequest;
using ::vendor::qti::voiceprint::V1_0::DeleteRequest;
using ::vendor::qti::voiceprint::V1_0::ProcessFrameRequest;
using ::vendor::qti::voiceprint::V1_0::StartAudioCaptureRequest;
using ::vendor::qti::voiceprint::V1_0::RenameRequest;
using ::vendor::qti::voiceprint::V1_0::ThresholdRequest;

using ::android::hardware::Void;
using ::android::hardware::Return;
using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using namespace android;
using namespace qvop;

class QtiVoicePrintService : public IQtiVoicePrintService {
public:
    static sp<QtiVoicePrintService> getInstance();
    Return<Status> getVersion(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> enroll(const sp<IQtiVoicePrintServiceCallback>& cb, const EnrollRequest& request) override;
    Return<Status> match(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> matchWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> matchSecure(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> matchAny(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> matchAnyKeyPhrase(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> matchAnySecure(const sp<IQtiVoicePrintServiceCallback>& cb, const MatchRequest& request) override;
    Return<Status> cancel() override;
    Return<Status> deleteUser(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) override;
    Return<Status> deleteWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) override;
    Return<Status> deleteAll(const sp<IQtiVoicePrintServiceCallback>& cb, const DeleteRequest& request) override;
    Return<Status> processFrame(const sp<IQtiVoicePrintServiceCallback>& cb, const ProcessFrameRequest& request) override;
    Return<Status> processFrameWithId(const sp<IQtiVoicePrintServiceCallback>& cb, const ProcessFrameRequest& request) override;
    Return<Status> onStartAudioCapture(const sp<IQtiVoicePrintServiceCallback>& cb, const StartAudioCaptureRequest& request) override;
    Return<Status> onStopAudioCapture(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> enrollCaptureStart(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> enrollCaptureComplete(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> enrollCommit(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> matchCaptureComplete(const sp<IQtiVoicePrintServiceCallback>& cb) override;
    Return<Status> rename(const sp<IQtiVoicePrintServiceCallback>& cb, const RenameRequest& request) override;
    Return<Status> setThreshold(const sp<IQtiVoicePrintServiceCallback>& cb, const ThresholdRequest& request) override;
    Return<Status> isVoicePrintEnabled() override;
private:
    //private ctor, copy, & assignment
    QtiVoicePrintService() : mSampleRate(AUDIO_DEFAULT_SAMPLE_RATE),
        mNumberChannels(AUDIO_DEFAULT_NUMBER_CHANNELS),
        mAudioFormat(AUDIO_DEFAULT_FORMAT),
        mIsShutdown(false) {};
    QtiVoicePrintService(const QtiVoicePrintService&);
    QtiVoicePrintService& operator=(const QtiVoicePrintService&);

    Return<Status> execute(sp<IQtiVoicePrintServiceCallback> const& callback, QvopCommandService& target);

    static sp<QtiVoicePrintService> sInstance;

    //data members for type of audio stream
    int32_t mSampleRate;
    int32_t mNumberChannels;
    int32_t mAudioFormat;

    //flag indicating that the operation is cancelled
    bool mIsShutdown;
};

#endif //QTI_VOICEPRINT_SERVICE_H