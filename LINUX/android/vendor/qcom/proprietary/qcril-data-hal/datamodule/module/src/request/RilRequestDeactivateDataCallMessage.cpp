/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "string.h"
#include "request/RilRequestDeactivateDataCallMessage.h"

namespace rildata {

RilRequestDeactivateDataCallMessage::~RilRequestDeactivateDataCallMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  qcril_request_params_type p = params.get_params();
  if (((char **)p.data)[0] != NULL) {
    delete[] ((char **)p.data)[0];
    ((char **)p.data)[0] = NULL;
  }
}

void RilRequestDeactivateDataCallMessage::deepCopy(qcril_request_params_type &p) {
    char *cId = ((char **)p.data)[0];
    if (cId != NULL) {
      int size = strlen(cId) + 1;
      ((char **)p.data)[0] = new char[size];
      strlcpy(((char **)p.data)[0], cId, size);
    }
}

string RilRequestDeactivateDataCallMessage::dump() {
  return mName + "{ }";
}

}// namespace
