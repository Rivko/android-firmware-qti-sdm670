/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

class ImsWfcSettingsStatusInd : public UnSolicitedMessage, public add_message_id<ImsWfcSettingsStatusInd> {
  public:

    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.ims.wfc_settings_status";

    enum class Status {
      INVALID,
      WFC_NOT_SUPPORTED,
      WFC_OFF,
      WFC_ON,
    };

    enum class Preference {
      INVALID,
      WLAN_PREFERRED,
      WLAN_ONLY,
      CELLULAR_PREFERRED,
      CELLULAR_ONLY,
      NONE,
    };

    struct StatusInfo {
      Status     status;
      Preference preference;
    };

    ImsWfcSettingsStatusInd(StatusInfo statusInfo) : UnSolicitedMessage(get_class_message_id()) {
      mName = MESSAGE_NAME;
      mStatusInfo.status = statusInfo.status;
      mStatusInfo.preference = statusInfo.preference;
    }

    inline ~ImsWfcSettingsStatusInd() {}

    std::shared_ptr<UnSolicitedMessage> clone() {
      return (std::make_shared<ImsWfcSettingsStatusInd>(mStatusInfo));
    }

    string dump() {
      return ImsWfcSettingsStatusInd::MESSAGE_NAME;
    }

    inline StatusInfo &getStatusInfo() {
      return mStatusInfo;
    }

  private:
    StatusInfo mStatusInfo;
};
