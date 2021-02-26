/* Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __OMADRMINPUTSTREAM_H__
#define __OMADRMINPUTSTREAM_H__

#include "drm_framework_common.h"

namespace android {

class OmaDrmPermission {

public:
    int mType;
    int mCount;
    long mStartDate;
    long mEndDate;
    long mInterval;

    OmaDrmPermission() :
            mType(-1), mCount(-1), mStartDate(-1l), mEndDate(-1l), mInterval(
                    -1l) {
    }

};

class OmaDrmInfo {

public:

    char* mMimetype;
    char* mUid;
    long mFileSize;
    OmaDrmPermission *mPermission;

    OmaDrmInfo() :
            mFileSize(-1l), mMimetype(NULL), mUid(NULL) {
        mPermission = new OmaDrmPermission();
    }
};

class OmaDrmInputStream {
protected:

    OmaDrmInputStream() {
    }
    ;

public:

    OmaDrmInputStream(int uniqueId, int fd, bool consumeRights);

    int fillInfoObject(KeyedVector<String8, String8> *extendedData);

    static OmaDrmInputStream* create(int uniqueId, int fd, bool consumeRights);

    ssize_t read(void* buffer, ssize_t numBytes, off_t offset);

    int release();

    int getUniqueId() {
        return mUniqueId;
    }

    int getStatus() {
        return mStatus;
    }

    int getFileSize() {
        return mFileSize;
    }

    OmaDrmInfo* getOmaDrmInfo() {
        return mOmaDrmInfo;
    }

    ~OmaDrmInputStream();

private:
    int mUniqueId;
    int mFD;
    bool mConsumeRights;
    long mFileSize;
    int mStatus;

    DrmManagerClient* mDrmManagerClient;
    sp<DecryptHandle> mDecryptHandle;
    OmaDrmInfo *mOmaDrmInfo = new OmaDrmInfo();
    ;

};

}
;

#endif /* __OMADRMINPUTSTREAM_H__ */
