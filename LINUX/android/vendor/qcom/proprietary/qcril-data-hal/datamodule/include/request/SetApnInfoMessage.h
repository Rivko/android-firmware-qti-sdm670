/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/GenericCallback.h"
#include "modules/data/DataCommon.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>

namespace rildata {

class SetApnInfoMessage : public SolicitedMessage<RIL_Errno>,
                          public add_message_id<SetApnInfoMessage> {
    public:

    qcril_request_params_type mParams;
    char *mApnType;
    char *mApnName;
    int32 mIsApnValid;

    static constexpr const char *MESSAGE_NAME = "QCRIL_DATA_SET_APN_INFO";
    SetApnInfoMessage() = delete;
    SetApnInfoMessage(const qcril_request_params_type &p,
                      const char *type,
                      const char *name,
                      int32 is_apn_valid)
          :SolicitedMessage<RIL_Errno>(get_class_message_id()), mParams(p) {

      unsigned int size;

      mName = MESSAGE_NAME;
      mApnType = nullptr;
      mApnName = nullptr;
      mIsApnValid = is_apn_valid;

      if (type != nullptr) {
        size = strlen(type) + 1;
        mApnType = new char[size];
        strlcpy(mApnType, type, size);
      }

      if (name != nullptr) {
        size = strlen(name) + 1;
        mApnName = new char[size];
        strlcpy(mApnName, name, size);
      }
    }
    ~SetApnInfoMessage();

    string dump();
};

}
