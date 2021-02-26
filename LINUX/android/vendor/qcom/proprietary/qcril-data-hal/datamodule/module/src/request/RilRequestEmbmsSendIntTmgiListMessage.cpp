/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestEmbmsSendIntTmgiListMessage.h"

namespace rildata {

RilRequestEmbmsSendIntTmgiListMessage::~RilRequestEmbmsSendIntTmgiListMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
  qcril_request_params_type p = params.get_params();
  if (p.data) {
    embms_get_interested_tmgi_list_resp_msg_v01* lp = static_cast<embms_get_interested_tmgi_list_resp_msg_v01*>(p.data);
    if (lp) {
      delete lp;
    }
  }
}

string RilRequestEmbmsSendIntTmgiListMessage::dump() {
  return "<TODO>";
}

}// namespace
