/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_NDEBUG 0
#define LOG_TAG "native/OmaDrmInputStream"

#include <stdlib.h>
#include <utils/Log.h>
#include <drm/DrmManagerClient.h>
#include "OmaDrmInputStream.h"
#include "OmaDrmRightsManager.h"
#include "drm_framework_common.h"

using namespace android;

OmaDrmInputStream::OmaDrmInputStream(int uniqueId, int fd, bool consumeRights) :
        mDrmManagerClient(NULL), mDecryptHandle(NULL), mFileSize(0) {
    ALOGI("OmaDrmInputStream::init");
    mUniqueId = uniqueId;
    mFD = fd;
    mConsumeRights = consumeRights;

    mDrmManagerClient = new DrmManagerClient();
    if (mDrmManagerClient != NULL) {
        ALOGI("DrmManagerClient created");
    }

    mDecryptHandle = mDrmManagerClient->openDecryptSession(mFD, 0, 1, NULL);

    if (mDecryptHandle != NULL) {
        mStatus = mDecryptHandle->status;

        mDrmManagerClient->setPlaybackStatus(mDecryptHandle, Playback::START, 0);
        if (mStatus == RightsStatus::RIGHTS_INVALID) {
            ALOGI("Unable to create native:OmaDrmInputStream! RIGHTS_INVALID");
        } else {
            fillInfoObject(&(mDecryptHandle->extendedData));
            ALOGV(
                "decryptHandle created and status = %d, decrepeted file size = %zd",
                mDecryptHandle->status, mFileSize);
        }
    }
    ALOGI("Init exit!");
}

OmaDrmInputStream::~OmaDrmInputStream() {
    ALOGI("Destroy success!");
}
;

OmaDrmInputStream* OmaDrmInputStream::create(int uniqueId, int fd,
        bool consumeRights) {
    return new OmaDrmInputStream(uniqueId, fd, consumeRights);
}

int OmaDrmInputStream::release() {
    if (mDecryptHandle != NULL) {
        if (mConsumeRights) {
            mDrmManagerClient->setPlaybackStatus(mDecryptHandle, Playback::STOP,
                    0);
        }
        mDrmManagerClient->closeDecryptSession(mDecryptHandle); // it will delete the mDecryptHandle object
        mDecryptHandle = NULL;
    }
    if (mDrmManagerClient != NULL) {
        delete mDrmManagerClient;
        mDrmManagerClient = NULL;
    }
    if (mOmaDrmInfo != NULL) {
        delete mOmaDrmInfo->mPermission;
        mOmaDrmInfo->mPermission = NULL;
        delete mOmaDrmInfo;
        mOmaDrmInfo = NULL;
    }
    return 1;
}

ssize_t OmaDrmInputStream::read(void* buffer, ssize_t numBytes, off_t offset) {
    int length = 0;

    if (mDrmManagerClient != NULL && mDecryptHandle != NULL) {
        length = mDrmManagerClient->pread(mDecryptHandle, buffer, numBytes,
                offset);
    }

    return length;
}

int android::OmaDrmInputStream::fillInfoObject(
        KeyedVector<String8, String8> *extendedData) {
    int vectorSize = extendedData->size();
    if (vectorSize > 0) {
        for (int i = 0; i < vectorSize; i++) {
            int len = strlen(extendedData->valueAt(i).string()) + 1;
            if (strncmp(extendedData->keyAt(i).string(), KEY_CONTENT_ID, 20)
                    == 0) {
                mOmaDrmInfo->mUid = (char*) malloc(len);
                memset(mOmaDrmInfo->mUid, 0x00, len);
                strlcpy(mOmaDrmInfo->mUid, extendedData->valueAt(i).string(),
                        len);
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_MIMETYPE, 20)
                    == 0) {
                mOmaDrmInfo->mMimetype = (char*) malloc(len);
                memset(mOmaDrmInfo->mMimetype, 0x00, len);
                strlcpy(mOmaDrmInfo->mMimetype,
                        extendedData->valueAt(i).string(), len);
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_SIZE, 20) == 0) {
                mOmaDrmInfo->mFileSize = atol(extendedData->valueAt(i));
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_TYPE, 20) == 0) {
                mOmaDrmInfo->mPermission->mType = atoi(
                        extendedData->valueAt(i));
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_COUNT, 20) == 0) {
                mOmaDrmInfo->mPermission->mCount = atoi(
                        extendedData->valueAt(i));
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_START_DATE, 20)
                    == 0) {
                mOmaDrmInfo->mPermission->mStartDate = atol(
                        extendedData->valueAt(i));
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_END_DATE, 20)
                    == 0) {
                mOmaDrmInfo->mPermission->mEndDate = atol(
                        extendedData->valueAt(i));
            }
            if (strncmp(extendedData->keyAt(i).string(), KEY_INTERVAL, 20)
                    == 0) {
                mOmaDrmInfo->mPermission->mInterval = atol(
                        extendedData->valueAt(i));
            }
        }
    }
    return 0;
}
