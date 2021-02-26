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

class UimQmiUimRequestMsg : public SolicitedSyncMessage<int>,
                            public add_message_id<UimQmiUimRequestMsg>
                            
{
  private:
    uint32_t                          mMsgId;
    void                            * mMsgPtr;
    const void                      * mOrigPtr;
    qmi_uim_user_async_cb_type        mCbPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.qmi_uim_request_msg";
    static constexpr int MESSAGE_EXPIRY_TIMEOUT{ 1000 * 125 };
    UimQmiUimRequestMsg() = delete;
    ~UimQmiUimRequestMsg();

    UimQmiUimRequestMsg(uint32_t msg_id, const void *dataPtr, const void *orig_ptr,
                        qmi_uim_user_async_cb_type cb_ptr);

    inline void * get_message()
    {
      return mMsgPtr;
    }

    inline const void * get_orig_ptr(void)
    {
      return mOrigPtr;
    }

    inline uint32_t get_msg_id(void)
    {
      return mMsgId;
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
