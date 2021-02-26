/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestEmbmsContentDescUpdateMessage.h"

namespace rildata {

RilRequestEmbmsContentDescUpdateMessage::~RilRequestEmbmsContentDescUpdateMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
  qcril_request_params_type p = params.get_params();
  if (p.data) {
    embms_update_content_desc_req_msg_v01* lp = static_cast<embms_update_content_desc_req_msg_v01*>(p.data);
    if (lp) {
      delete lp;
    }
  }
}

string RilRequestEmbmsContentDescUpdateMessage::dump() {
  return "<TODO>";
}

}// namespace
