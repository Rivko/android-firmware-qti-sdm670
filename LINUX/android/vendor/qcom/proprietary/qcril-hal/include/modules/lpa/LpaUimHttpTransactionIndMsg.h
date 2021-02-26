/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"


class LpaUimHttpTransactionIndMsg : public UnSolicitedMessage,
                                    public add_message_id<LpaUimHttpTransactionIndMsg>
{
  private:
    uint32_t                mMsgId;
    uint32_t                mMsgLen;
    uint8_t               * mMsgPtr;
    int                     mTranspErr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.http_transaction_ind_msg";
    LpaUimHttpTransactionIndMsg() = delete;
    ~LpaUimHttpTransactionIndMsg();

    LpaUimHttpTransactionIndMsg(uint32_t dataSize, void *dataPtr, unsigned long msg_id,
                                int tansp_err);

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
        return  std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<LpaUimHttpTransactionIndMsg>(mMsgLen, (void *)mMsgPtr, mMsgId, mTranspErr));
    }

    inline string dump()
    {
      return mName + "{}";
    }

    inline void * get_message(uint32_t *out_msg_len)
    {
      if (out_msg_len != nullptr)
      {
        *out_msg_len = mMsgLen;
      }
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
