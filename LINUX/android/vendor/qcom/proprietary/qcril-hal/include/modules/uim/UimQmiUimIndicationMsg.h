/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"


class UimQmiUimIndicationMsg : public UnSolicitedMessage,
                               public add_message_id<UimQmiUimIndicationMsg>
{
  private:
    void       *mIndPtr;
    uint32_t    mIndSize;
    uint32_t    mIndId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.qmi_indication";
    UimQmiUimIndicationMsg() = delete;
    ~UimQmiUimIndicationMsg();

    UimQmiUimIndicationMsg(void                         * user_data,
                           uint32_t                       ind_id,
                           void                         * ind_data_ptr);

    std::shared_ptr<UnSolicitedMessage> clone();

    inline string dump()
    {
      return mName + "{}";
    }

    inline void* get_message(uint32_t *out_msg_len)
    {
      *out_msg_len = mIndSize;
      return mIndPtr;
    }

    inline uint32_t get_ind_id(void)
    {
      return mIndId;
    }
};
