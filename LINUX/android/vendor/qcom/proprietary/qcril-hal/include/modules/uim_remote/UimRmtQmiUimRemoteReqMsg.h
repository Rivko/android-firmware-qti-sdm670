/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"

class UimRmtQmiUimRemoteReqMsg : public SolicitedMessage<qmi_uim_remote_resp_msg>,
                                 public add_message_id<UimRmtQmiUimRemoteReqMsg>
                            
{
  private:
    std::shared_ptr<qmi_uim_remote_request_msg>  mMsgPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.uim_remote.client.qmi_uim_remote_request_msg";
    UimRmtQmiUimRemoteReqMsg() = delete;
    inline ~UimRmtQmiUimRemoteReqMsg()
    {
      mMsgPtr = nullptr;
    }

    inline UimRmtQmiUimRemoteReqMsg(std::shared_ptr<qmi_uim_remote_request_msg>  dataPtr,
                                    GenericCallback<qmi_uim_remote_resp_msg>  * callback):
                                    SolicitedMessage<qmi_uim_remote_resp_msg>(get_class_message_id())
    {
      mMsgPtr = dataPtr;
      mName = MESSAGE_NAME;
      setCallback(callback);
    }

    inline std::shared_ptr<qmi_uim_remote_request_msg> get_message()
    {
      return mMsgPtr;
    }

    inline string dump()
    {
      return mName;
    }
};

