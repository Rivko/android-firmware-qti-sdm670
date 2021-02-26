/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class LpaQmiUimHttpResponseMsg : public SolicitedMessage<void>,
                                 public add_message_id<LpaQmiUimHttpResponseMsg>
{
  private:
    lpa_qmi_uim_http_rsp_data_type        mRspData;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.qmi_uim_http_response";
    LpaQmiUimHttpResponseMsg() = delete;
    ~LpaQmiUimHttpResponseMsg(){}

    LpaQmiUimHttpResponseMsg(lpa_qmi_uim_http_rsp_data_type * rsp_data_ptr) :
	  SolicitedMessage<void>(get_class_message_id())
    {
      memset(&mRspData, 0x00, sizeof(mRspData));
      if (rsp_data_ptr != nullptr)
      {
        memcpy(&mRspData, rsp_data_ptr, sizeof(mRspData));
      }
    }

    inline string dump()
    {
      return mName + "{ QMI Error: " + std::to_string(mRspData.qmi_error_code) + "}";
    }

    inline lpa_qmi_uim_http_rsp_data_type * get_message()
    {
      return &mRspData;
    }
};
