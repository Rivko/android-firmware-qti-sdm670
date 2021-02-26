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
#include "modules/uim/qcril_gstk_srvc.h"

class GstkQmiCatRequestMsg : public SolicitedMessage<qmi_cat_rsp_data_type>,
                             public add_message_id<GstkQmiCatRequestMsg>
                            
{
  private:
    qcril_gstk_request_type                              mMsgId;
    std::shared_ptr<qcril_gstk_request_data_type>        mMsgPtr;
    std::shared_ptr<qcril_gstk_original_request_type>    mOrigPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.gstk.qmi_cat_request_msg";
    GstkQmiCatRequestMsg() = delete;
    inline ~GstkQmiCatRequestMsg()
    {
      mMsgPtr = nullptr;
      mOrigPtr = nullptr;
    }

    inline GstkQmiCatRequestMsg(qcril_gstk_request_type                              msg_id,
                                std::shared_ptr<qcril_gstk_request_data_type>        dataPtr,
                                std::shared_ptr<qcril_gstk_original_request_type>    orig_ptr,
                                GenericCallback<qmi_cat_rsp_data_type>             * callback):
                                SolicitedMessage<qmi_cat_rsp_data_type>(get_class_message_id())
    {
      mMsgId = msg_id;
      mMsgPtr = dataPtr;
      mOrigPtr = orig_ptr;
      mName = MESSAGE_NAME;
      setCallback(callback);
    }

    inline qcril_gstk_request_data_type * get_message()
    {
      return mMsgPtr.get();
    }

    inline std::shared_ptr<qcril_gstk_original_request_type> get_orig_ptr(void)
    {
      return mOrigPtr;
    }

    inline qcril_gstk_request_type get_msg_id(void)
    {
      return mMsgId;
    }

    inline string dump()
    {
      return mName + "{ msg_id: " + std::to_string(mMsgId) + "}";
    }
};

