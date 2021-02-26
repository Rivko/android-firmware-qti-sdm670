/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <string.h>
#include "request/RilRequestSetDataProfileMessage.h"

using namespace rildata;

RilRequestSetDataProfileMessage::RilRequestSetDataProfileMessage
(
  const qcril_request_params_type &p
) : SolicitedMessage<generic_callback_payload>(get_class_message_id()), params(p)
{
  mName = MESSAGE_NAME;
  deepCopy(params.get_params());
}

RilRequestSetDataProfileMessage::~RilRequestSetDataProfileMessage()
{
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  qcril_request_params_type p = params.get_params();
  int n = p.datalen / sizeof(RIL_DataProfileInfo *);

  for (int i = 0; i < n; i++) {
    RIL_DataProfileInfo *prof = ((RIL_DataProfileInfo **)p.data)[i];
    if (prof != nullptr) {

      if (prof->apn != nullptr)
        delete prof->apn;
      if (prof->protocol != nullptr)
        delete prof->protocol;
      if (prof->user != nullptr)
        delete prof->user;
      if (prof->password != nullptr)
        delete prof->password;

      delete prof;
      ((RIL_DataProfileInfo **)p.data)[i] = NULL;
    }
  }
}

void RilRequestSetDataProfileMessage::deepCopy
(
 qcril_request_params_type &p
)
{
  unsigned size;
  int n = p.datalen / sizeof(RIL_DataProfileInfo *);

  for (int i = 0; i < n; i++) {
    RIL_DataProfileInfo *prof = ((RIL_DataProfileInfo **)p.data)[i];
    if (prof != NULL) {
      RIL_DataProfileInfo *new_prof = new RIL_DataProfileInfo;
      if (!new_prof) continue;
      memcpy(new_prof, prof, sizeof(RIL_DataProfileInfo));
      ((RIL_DataProfileInfo **)p.data)[i] = new_prof;

      if (prof->apn != nullptr) {
        size = strlen(prof->apn) + 1;
        new_prof->apn = new char[size];
        if (new_prof->apn != nullptr) {
            strlcpy(new_prof->apn, prof->apn, size);
        }
      }
      if (prof->protocol != nullptr) {
        size = strlen(prof->protocol) + 1;
        new_prof->protocol = new char[size];
        if (new_prof->protocol != nullptr) {
            strlcpy(new_prof->protocol, prof->protocol, size);
        }
      }
      if (prof->user != nullptr) {
        size = strlen(prof->user) + 1;
        new_prof->user = new char[size];
        if (new_prof->user != nullptr) {
            strlcpy(new_prof->user, prof->user, size);
        }
      }
      if (prof->password != nullptr) {
        size = strlen(prof->password) + 1;
        new_prof->password = new char[size];
        if (new_prof->password != nullptr) {
            strlcpy(new_prof->password, prof->password, size);
        }
      }
    }
  }
}

qcril_request_params_type& RilRequestSetDataProfileMessage::get_params()
{
  return params.get_params();
}

string RilRequestSetDataProfileMessage::dump() {
  return mName;
}

#if (QCRIL_RIL_VERSION >= 15)
RilRequestSetDataProfileMessage_v15::RilRequestSetDataProfileMessage_v15
(
  const qcril_request_params_type &p
) : SolicitedMessage<generic_callback_payload>(get_class_message_id()), params(p)
{
  mName = MESSAGE_NAME;
  deepCopy(params.get_params());
}

RilRequestSetDataProfileMessage_v15::~RilRequestSetDataProfileMessage_v15()
{
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  qcril_request_params_type p = params.get_params();
  int n = p.datalen / sizeof(RIL_DataProfileInfo_v15 *);

  for (int i = 0; i < n; i++) {
    RIL_DataProfileInfo_v15 *prof = ((RIL_DataProfileInfo_v15 **)p.data)[i];
    if (prof != nullptr) {

      if (prof->apn != nullptr)
        delete prof->apn;
      if (prof->protocol != nullptr)
        delete prof->protocol;
      if (prof->roamingProtocol != nullptr)
        delete prof->roamingProtocol;
      if (prof->user != nullptr)
        delete prof->user;
      if (prof->password != nullptr)
        delete prof->password;
      if (prof->mvnoType != nullptr)
        delete prof->mvnoType;
      if (prof->mvnoMatchData != nullptr)
        delete prof->mvnoMatchData;

      delete prof;
      ((RIL_DataProfileInfo_v15 **)p.data)[i] = NULL;
    }
  }
}

void RilRequestSetDataProfileMessage_v15::deepCopy
(
 qcril_request_params_type &p
)
{
  unsigned size;
  int n = p.datalen / sizeof(RIL_DataProfileInfo_v15 *);

  for (int i = 0; i < n; i++) {
    RIL_DataProfileInfo_v15 *prof = ((RIL_DataProfileInfo_v15 **)p.data)[i];
    if (prof != NULL) {
      RIL_DataProfileInfo_v15 *new_prof = new RIL_DataProfileInfo_v15;
      if (!new_prof) continue;
      memcpy(new_prof, prof, sizeof(RIL_DataProfileInfo_v15));
      ((RIL_DataProfileInfo_v15 **)p.data)[i] = new_prof;

      if (prof->apn != nullptr) {
        size = strlen(prof->apn) + 1;
        new_prof->apn = new char[size];
        if (new_prof->apn != nullptr) {
            strlcpy(new_prof->apn, prof->apn, size);
        }
      }
      if (prof->protocol != nullptr) {
        size = strlen(prof->protocol) + 1;
        new_prof->protocol = new char[size];
        if (new_prof->protocol != nullptr) {
            strlcpy(new_prof->protocol, prof->protocol, size);
        }
      }
      if (prof->roamingProtocol != nullptr) {
        size = strlen(prof->roamingProtocol) + 1;
        new_prof->roamingProtocol = new char[size];
        if (new_prof->roamingProtocol) {
            strlcpy(new_prof->roamingProtocol, prof->roamingProtocol, size);
        }
      }
      if (prof->user != nullptr) {
        size = strlen(prof->user) + 1;
        new_prof->user = new char[size];
        if (new_prof->user != nullptr) {
            strlcpy(new_prof->user, prof->user, size);
        }
      }
      if (prof->password != nullptr) {
        size = strlen(prof->password) + 1;
        new_prof->password = new char[size];
        if (new_prof->password != nullptr) {
            strlcpy(new_prof->password, prof->password, size);
        }
      }
      if (prof->mvnoType!= nullptr) {
        size = strlen(prof->mvnoType) + 1;
        new_prof->mvnoType = new char[size];
        if (new_prof->mvnoType != nullptr) {
            strlcpy(new_prof->mvnoType, prof->mvnoType, size);
        }
      }
      if (prof->mvnoMatchData!= nullptr) {
        size = strlen(prof->mvnoMatchData) + 1;
        new_prof->mvnoMatchData = new char[size];
        if (new_prof->mvnoMatchData != nullptr) {
            strlcpy(new_prof->mvnoMatchData, prof->mvnoMatchData, size);
        }
      }
    }
  }
}

qcril_request_params_type& RilRequestSetDataProfileMessage_v15::get_params()
{
  return params.get_params();
}

string RilRequestSetDataProfileMessage_v15::dump() {
  return mName;
}
#endif /* QCRIL_RIL_VERSION >= 15 */

