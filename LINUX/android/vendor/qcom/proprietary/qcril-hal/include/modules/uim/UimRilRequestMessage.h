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
#include "framework/message_translator.h"
#include "framework/legacy.h"
#include "modules/android/ril_request_info.h"

class uim_ril_resp_payload: public generic_callback_payload
{
  private:
    void       * mRspPtr;

  public:
    RIL_Errno    mRilErr;
    size_t       mRspLen;

    uim_ril_resp_payload(RIL_Errno  ril_err, void *rsp_ptr, size_t rsp_len)
    {
      mRilErr = ril_err;
      mRspPtr = rsp_ptr;
      mRspLen = rsp_len;
    }

    ~uim_ril_resp_payload()
    {
      mRspPtr = nullptr;
    }

    void *get()
    {
      return mRspPtr;
    }
};


class UimRilRequestMessage : public SolicitedMessage<generic_callback_payload>,
                             public add_message_id<UimRilRequestMessage>
                            
{
  private:
    qcril_request_params_type         mParams;
    int                               mMsgId;
    void                             *mRilDataPtr;
    RIL_Token                         mToken;

  public:
    bool is_valid;
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.ril_request_msg";
    static constexpr int MESSAGE_EXPIRY_TIMEOUT{ 1000 * 125 };
    UimRilRequestMessage() = delete;
    ~UimRilRequestMessage();

    UimRilRequestMessage(const ril_request_type *req_ptr);

    inline qcril_request_params_type *get_params()
    {
      return &mParams;
    }

    inline int get_req_id()
    {
      return mMsgId;
    }

    inline RIL_Token get_token()
    {
      return mToken;
    }

    string dump();

    bool deep_copy
    (
      int req_id,
      const void *payload,
      size_t payload_len,
      void **data
    );

    void deep_clean
    (
      int msg_id
    );
};
