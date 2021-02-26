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


class LpaUimHttpTransactionRspMsg : public SolicitedMessage<void>,
                                    public add_message_id<LpaUimHttpTransactionRspMsg>
{
  private:
    uint32_t                mMsgId;
    uint32_t                mMsgLen;
    uint8_t               * mMsgPtr;
    int                     mTranspErr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.http_transaction_rsp_msg";
    LpaUimHttpTransactionRspMsg() = delete;
    ~LpaUimHttpTransactionRspMsg();

    LpaUimHttpTransactionRspMsg(uint32_t dataSize, void *dataPtr, unsigned long msg_id,
                                int tansp_err);

    inline string dump()
    {
      return mName + "{}";
    }

    inline void * get_message(uint32_t *out_msg_len)
    {
      *out_msg_len = mMsgLen;
      return mMsgPtr;
    }

    inline uint32_t get_msg_id(void)
    {
      return mMsgId;
    }

    inline int get_transport_err(void)
    {
      return mTranspErr;
    }
};
