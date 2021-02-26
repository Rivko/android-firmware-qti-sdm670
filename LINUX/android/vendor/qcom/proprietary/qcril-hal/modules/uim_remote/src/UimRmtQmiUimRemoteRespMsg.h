/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim_remote/UimRmtQmiUimRemoteReqMsg.h"

class UimRmtQmiUimRemoteRespMsg : public SolicitedMessage<void>,
                                  public add_message_id<UimRmtQmiUimRemoteRespMsg>
{
  private:
    int32_t                                   mToken;
    qmi_uim_remote_req_type                   mReqType;
    std::shared_ptr<qmi_uim_remote_resp_msg>  mRspDataPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.uim_remote.client.qmi_uim_remote_response";
    UimRmtQmiUimRemoteRespMsg() = delete;
    ~UimRmtQmiUimRemoteRespMsg(){}

    UimRmtQmiUimRemoteRespMsg(int32_t token, qmi_uim_remote_req_type req_type,
                              std::shared_ptr<qmi_uim_remote_resp_msg> rsp_data_ptr) :
	  SolicitedMessage<void>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mToken = token;
      mRspDataPtr = rsp_data_ptr;
      mReqType = req_type;

      Log::getInstance().d(
        "[UimRmtQmiUimRemoteRespMsg]: response token" +
        std::to_string(token) + "req_type: %d" + std::to_string(req_type));
    }

    inline string dump()
    {
      return mName;
    }

    inline int32_t get_token()
    {
      return mToken;
    }

    inline qmi_uim_remote_req_type get_req_type()
    {
      return mReqType;
    }

    inline std::shared_ptr<qmi_uim_remote_resp_msg> get_message()
    {
      return mRspDataPtr;
    }
};
