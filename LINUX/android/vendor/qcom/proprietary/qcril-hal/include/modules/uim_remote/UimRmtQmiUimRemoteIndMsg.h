/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"

class UimRmtQmiUimRemoteIndMsg : public UnSolicitedMessage,
                                 public add_message_id<UimRmtQmiUimRemoteIndMsg>
{
  private:
    std::shared_ptr<qmi_uim_remote_ind_msg>  mIndPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.uim_remote.client.qmi_uim_remote_indication";
    UimRmtQmiUimRemoteIndMsg() = delete;
    inline ~UimRmtQmiUimRemoteIndMsg()
    {
      mIndPtr = nullptr;
    }

    inline UimRmtQmiUimRemoteIndMsg(std::shared_ptr<qmi_uim_remote_ind_msg>   ind_data_ptr):
                                                UnSolicitedMessage(get_class_message_id())
    {
      mIndPtr = ind_data_ptr;
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimRmtQmiUimRemoteIndMsg>(mIndPtr));
    }

    inline string dump()
    {
      return mName;
    }

    inline std::shared_ptr<qmi_uim_remote_ind_msg> get_message(void)
    {
      return mIndPtr;
    }
};

