/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim/qcril_gstk_srvc.h"

class GstkQmiCatRequestSyncMsg : public SolicitedSyncMessage<int>,
                                 public add_message_id<GstkQmiCatRequestSyncMsg>
{
  private:
    qcril_gstk_request_type                         mMsgId;
    std::shared_ptr<qcril_gstk_request_data_type>   mMsgPtr;
    std::shared_ptr<qmi_cat_sync_rsp_data_type>     mRspDataPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.gstk.qmi_cat_client_sync_request";
    GstkQmiCatRequestSyncMsg() = delete;
    inline ~GstkQmiCatRequestSyncMsg()
    {
      mMsgPtr = nullptr;
      mRspDataPtr = nullptr;
    }

    GstkQmiCatRequestSyncMsg(qcril_gstk_request_type                         msg_id,
                             std::shared_ptr<qcril_gstk_request_data_type>   dataPtr,
                             std::shared_ptr<qmi_cat_sync_rsp_data_type>     rsp_data_ptr):
                             SolicitedSyncMessage<int>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mMsgId = msg_id;
      mMsgPtr = dataPtr;
      mRspDataPtr = rsp_data_ptr;
    }

    inline const qcril_gstk_request_data_type * get_message(void)
    {
      if (mMsgPtr != nullptr)
      {
        return mMsgPtr.get();
      }
      return nullptr;
    }

    inline qcril_gstk_request_type get_msg_id(void)
    {
      return mMsgId;
    }

    inline qmi_cat_sync_rsp_data_type *get_rsp_data()
    {
      return mRspDataPtr.get();
    }

    inline string dump()
    {
      return mName + "{ " + std::to_string(mMsgId) + "}";
    }
};

