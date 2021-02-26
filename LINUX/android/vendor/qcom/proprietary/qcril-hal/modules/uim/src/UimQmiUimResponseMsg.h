/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "modules/uim/qcril_uim_srvc.h"

class UimQmiUimResponseMsg : public SolicitedMessage<void>,
                             public add_message_id<UimQmiUimResponseMsg>
{
  private:
    void                           *mRspData;
    uint32_t                        mRspSize;
    void                           *mOrigPtr;
    qmi_uim_rsp_id_type             mRspId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.qmi_command_callback";
    UimQmiUimResponseMsg() = delete;
    ~UimQmiUimResponseMsg();

    UimQmiUimResponseMsg(qmi_uim_rsp_data_type * rsp_data,
                         void                  * orig_ptr);

    inline string dump()
    {
      return mName + "{}";
    }

    inline void * get_message(uint32_t *out_msg_len)
    {
      *out_msg_len = mRspSize;
      return mRspData;
    }

    inline void * get_user_data()
    {
      return mOrigPtr;
    }

    inline qmi_uim_rsp_id_type get_message_id()
    {
      return mRspId;
    }
};
