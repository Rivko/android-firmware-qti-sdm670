/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#define TAG "RILQ_translator_utils"
#include <telephony/ril.h>
#include <framework/Log.h>
#include <framework/qcril_event.h>
#include <translators/android/utils.h>
#include "qcrili.h"
#include <map>

#define QCRIL_EVT_TO_ANDROID_ENTRY(id)   { QCRIL_EVT_##id, id },


#undef QCRIL_ANDROID_RIL_REQUEST
#define QCRIL_ANDROID_RIL_REQUEST        QCRIL_EVT_TO_ANDROID_ENTRY
static std::map<qcril_evt_e_type, int> evt_to_android_request {
#include <translators/android/android_requests.h>
};


#undef QCRIL_ANDROID_RIL_REQUEST
#define QCRIL_ANDROID_RIL_REQUEST(id) case id: return QCRIL_EVT_##id; break;
qcril_evt_e_type qcril_android_request_get_internal_event(int request)
{
  (void)request;
  // EVENT REFACTOR TODO: create a look up a table and generate the internal event.
  switch(request) {
#include <translators/android/android_requests.h>
  }
  return nullptr;
}
#undef QCRIL_ANDROID_RIL_REQUEST

int qcril_event_get_android_request(qcril_evt_e_type event)
{
  (void)event;
  int req_id = -1;
  if (event) {
    auto it = evt_to_android_request.find(event);
    if (it != evt_to_android_request.end()) {
        req_id = it->second;
        QCRIL_LOG_DEBUG("Found id %d for %s", req_id, event->name);
    } else {
        QCRIL_LOG_DEBUG("Unable to find id for %s", event->name);
    }
  } else {
      QCRIL_LOG_DEBUG("Invalid event received");
  }
  return req_id;
}
