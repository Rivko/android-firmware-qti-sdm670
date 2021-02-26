/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "sync/RilDataEmbmsActiveMessage.h"

namespace rildata {

RilDataEmbmsActiveMessage::~RilDataEmbmsActiveMessage() {
    if (mCallback) {
        delete mCallback;
        mCallback = nullptr;
    }
}

string RilDataEmbmsActiveMessage::dump() {
    return mName + "{ }";
}

}// namespace
