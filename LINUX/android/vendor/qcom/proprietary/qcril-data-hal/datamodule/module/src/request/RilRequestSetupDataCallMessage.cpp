/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestSetupDataCallMessage.h"

namespace rildata {

RilRequestSetupDataCallMessage::~RilRequestSetupDataCallMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  qcril_request_params_type p = params.get_params();
  int n = p.datalen / sizeof(char *);
  for (int i = 0; i < n; i++)
  {
    if (((char **)p.data)[i] != NULL)
    {
      delete[] ((char **)p.data)[i];
      ((char **)p.data)[i] = NULL;
    }
  }
}

string RilRequestSetupDataCallMessage::dump() {
  return mName + "{}";
}

}// namespace
