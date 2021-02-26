/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim/qcril_uim_srvc.h"

class UimQmiUimRequestSyncMsg : public SolicitedSyncMessage<int>,
                                public add_message_id<UimQmiUimRequestSyncMsg>
{
  private:
    uint32_t                          mMsgId;
    const void                      * mMsgPtr;
    void                            * mRspData;
    void                            * mUserData;
    qmi_uim_user_async_cb_type        mCbPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.qmi_uim_client_sync_request";
    UimQmiUimRequestSyncMsg() = delete;
    inline ~UimQmiUimRequestSyncMsg()
    {
      mMsgPtr = nullptr;
      mRspData = nullptr;
      mUserData = nullptr;
      mCbPtr = nullptr;
    }

    UimQmiUimRequestSyncMsg(uint32_t msg_id, const void *dataPtr, void  *rsp_data, void *user_data,
                            qmi_uim_user_async_cb_type cb_ptr):
                            SolicitedSyncMessage<int>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mMsgId = msg_id;
      mMsgPtr = dataPtr;
      mRspData = rsp_data;
      mUserData = user_data;
      mCbPtr = cb_ptr;
    }

    inline const void * get_message(void)
    {
      return mMsgPtr;
    }

    inline uint32_t get_msg_id(void)
    {
      return mMsgId;
    }

    inline void *get_rsp_data()
    {
      return mRspData;
    }

    inline void *get_user_data()
    {
      return mUserData;
    }

    inline qmi_uim_user_async_cb_type get_cb_ptr()
    {
      return mCbPtr;
    }

    inline string dump()
    {
      return mName + "{ " + std::to_string(mMsgId) + "}";
    }
};
