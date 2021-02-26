/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "network_access_service_v01.h"
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class NasSetVoiceDomainPreferenceRequest : public SolicitedMessage<RIL_Errno>,
  public add_message_id<NasSetVoiceDomainPreferenceRequest> {
  private:
    nas_voice_domain_pref_enum_type_v01 mVoiceDomainPref;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.nas.set_voice_domain_preference";

    NasSetVoiceDomainPreferenceRequest() = delete;

    ~NasSetVoiceDomainPreferenceRequest() {}

    inline NasSetVoiceDomainPreferenceRequest(const nas_voice_domain_pref_enum_type_v01 &pref,
        GenericCallback<RIL_Errno> *cb) : SolicitedMessage<RIL_Errno>(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mVoiceDomainPref = pref;
      setCallback(cb);
    }

    inline nas_voice_domain_pref_enum_type_v01 getVoiceDomainPreference() {
      return mVoiceDomainPref;
    }

    string dump() {
      return mName + "{}";
    }
};
