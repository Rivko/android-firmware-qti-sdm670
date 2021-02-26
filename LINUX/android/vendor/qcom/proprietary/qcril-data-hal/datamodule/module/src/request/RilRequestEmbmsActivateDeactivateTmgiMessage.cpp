/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestEmbmsActivateDeactivateTmgiMessage.h"

namespace rildata {

RilRequestEmbmsActivateDeactivateTmgiMessage::~RilRequestEmbmsActivateDeactivateTmgiMessage() {
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }
  qcril_request_params_type p = params.get_params();
  if (p.data) {
    embms_activate_deactivate_tmgi_req_msg_v01* lp = static_cast<embms_activate_deactivate_tmgi_req_msg_v01*>(p.data);
    if (lp) {
      delete lp;
    }
  }
}

string RilRequestEmbmsActivateDeactivateTmgiMessage::dump() {
  return "<TODO>";
}

}// namespace
