/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>


class ImsServiceStatusInd : public UnSolicitedMessage, public add_message_id<ImsServiceStatusInd> {
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.ims.serivce_status_ind";

    enum class ServiceStatus {
      INVALID,
      NO_SERVICE,
      LIMITED_SERVICE,
      FULL_SERVICE,
    };

    enum class ServiceRat {
      INVALID,
      WLAN,
      WWAN,
      IWLAN,
    };

    enum class RegistrationStatus {
      INVALID,
      NOT_REGISTERED,
      REGISTERING,
      REGISTERED,
      LIMITED_REGISTERED,
    };

    struct ServiceStatusInfo {
      RegistrationStatus reg_status;

      ServiceStatus voip_service_status;
      ServiceRat voip_service_rat;

      ServiceStatus vt_service_status;
      ServiceRat vt_service_rat;
    };

    ImsServiceStatusInd(ServiceStatusInfo serviceStatusInfo);

    inline ~ImsServiceStatusInd() {}

    std::shared_ptr<UnSolicitedMessage> clone() {
      return (std::make_shared<ImsServiceStatusInd>(mServiceStatusInfo));
    }

    string dump() {
      return ImsServiceStatusInd::MESSAGE_NAME;
    }

    inline ServiceStatusInfo &getServiceStatusInfo() {
      return mServiceStatusInfo;
    }

  private:
    ServiceStatusInfo mServiceStatusInfo;
};
