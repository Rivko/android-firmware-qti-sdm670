/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef QTI_VOICEPRINT_CALLBACK_H
#define QTI_VOICEPRINT_CALLBACK_H

#include <vendor/qti/voiceprint/1.0/IQtiVoicePrintService.h>
#include <vendor/qti/voiceprint/1.0/types.h>
#include <hardware/hw_auth_token.h>
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>

#include "IQvopCallback.h"

using ::vendor::qti::voiceprint::V1_0::IQtiVoicePrintService;
using ::vendor::qti::voiceprint::V1_0::IQtiVoicePrintServiceCallback;
using ::vendor::qti::voiceprint::V1_0::Status;
using ::vendor::qti::voiceprint::V1_0::ErrorResult;
using ::vendor::qti::voiceprint::V1_0::MatchResult;
using ::vendor::qti::voiceprint::V1_0::EnrollResult;
using ::vendor::qti::voiceprint::V1_0::DeleteResult;
using ::vendor::qti::voiceprint::V1_0::VersionInfoResult;
using ::vendor::qti::voiceprint::V1_0::ConfigResult;
using ::vendor::qti::voiceprint::V1_0::RenameResult;

using ::android::hardware::hidl_vec;
using ::android::hardware::hidl_string;
using namespace android;
using namespace qvop;

class QtiVoicePrintServiceCallback: public IQvopCallback, public RefBase {
public:
    QtiVoicePrintServiceCallback(sp<IQtiVoicePrintServiceCallback> callback);
    ~QtiVoicePrintServiceCallback();

    // On any error
    virtual void onError(int32_t error);

    // On a match or match any successful search
    virtual void onMatchFound(int32_t error, const char* user_id, const char* identifier, int32_t index, double sentence_score, double ver_score, float spoofScore);

    // On a match or match any failed
    virtual void onMatchNotFound(int32_t error, int32_t index, double sentence_score, double ver_score, float spoofScore);

    // Enrollment was successful
    virtual void onEnrollStatus(int32_t status);

    // Delete status - user/identifier, depends on request
    virtual void onDeleteStatus(int32_t status, const char* user_id, const char* identifier);

    // Delete all voiceprint templates status
    virtual void onDeleteAllStatus(int32_t status);

    // Version info
    virtual void onVersionInfo(long major, long minor);

    // Config setting success
    virtual void onConfigStatus(int32_t status);

    // Rename successful
    virtual void onRenameStatus(int32_t status);

private:
    sp<IQtiVoicePrintServiceCallback> mQtiVoicePrintCallback;
};

#endif  // QTI_VOICEPRINT_CALLBACK_H
