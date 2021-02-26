/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include "QtiVoicePrintServiceCallback.h"

QtiVoicePrintServiceCallback::QtiVoicePrintServiceCallback(sp<IQtiVoicePrintServiceCallback> callback) {
    mQtiVoicePrintCallback = callback;
}

QtiVoicePrintServiceCallback::~QtiVoicePrintServiceCallback() {
    mQtiVoicePrintCallback = NULL;
}

// On any error
void QtiVoicePrintServiceCallback::onError(int32_t error) {
    ErrorResult result;
    result.error = error;
    mQtiVoicePrintCallback->onError(result);
}

// On a match or match any successful search
void QtiVoicePrintServiceCallback::onMatchFound(int32_t error, const char* user_id, const char* identifier, int32_t index, double sentence_score, double ver_score, float spoofScore) {
    MatchResult result;
    result.status = Status::SUCCESS;
    result.error = error;
    result.user_id = hidl_string(user_id);
    result.identifier = hidl_string(identifier);
    result.index = index;
    result.sentence_score = sentence_score;
    result.ver_score = ver_score;
    result.spoofScore = spoofScore;
    mQtiVoicePrintCallback->onMatch(result);
}

// On a match or match any failed
void QtiVoicePrintServiceCallback::onMatchNotFound(int32_t error, int32_t index, double sentence_score, double ver_score, float spoofScore) {
    onMatchFound(error, "", "", index, sentence_score, ver_score, spoofScore);
}

// Enrollment was successful
void QtiVoicePrintServiceCallback::onEnrollStatus(int32_t status) {
    EnrollResult result;
    result.status = status == 0 ? Status::SUCCESS : Status::FAILURE;
    mQtiVoicePrintCallback->onEnroll(result);
}

// Delete status - user/identifier, depends on request
void QtiVoicePrintServiceCallback::onDeleteStatus(int32_t status, const char* user_id, const char* identifier) {
    DeleteResult result;
    result.status = status == 0 ? Status::SUCCESS : Status::FAILURE;
    result.user_id = hidl_string(user_id);
    result.identifier = hidl_string(identifier);
    mQtiVoicePrintCallback->onDelete(result);
}

// Delete all voiceprint templates status
void QtiVoicePrintServiceCallback::onDeleteAllStatus(int32_t status) {
    onDeleteStatus(status, "", "");
}

// Version info
void QtiVoicePrintServiceCallback::onVersionInfo(long major, long minor) {
    VersionInfoResult result;
    result.status = Status::SUCCESS;
    result.major = major;
    result.minor = minor;
    mQtiVoicePrintCallback->onVersionInfo(result);
}

// Config setting success
void QtiVoicePrintServiceCallback::onConfigStatus(int32_t status) {
    ConfigResult result;
    result.status = status == 0 ? Status::SUCCESS : Status::FAILURE;
    mQtiVoicePrintCallback->onConfigStatus(result);
}

// Rename successful
void QtiVoicePrintServiceCallback::onRenameStatus(int32_t status) {
    RenameResult result;
    result.status = status == 0 ? Status::SUCCESS : Status::FAILURE;
    mQtiVoicePrintCallback->onRenameStatus(result);
}