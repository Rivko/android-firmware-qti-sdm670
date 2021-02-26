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

class gstk_ril_resp_payload: public generic_callback_payload
{
  private:
    void       * mRspPtr;

  public:
    RIL_Errno    mRilErr;
    size_t       mRspLen;

    gstk_ril_resp_payload(RIL_Errno  ril_err, void *rsp_ptr, size_t rsp_len)
    {
      mRilErr = ril_err;
      mRspPtr = rsp_ptr;
      mRspLen = rsp_len;
    }

    ~gstk_ril_resp_payload()
    {
      mRspPtr = nullptr;
    }

    void *get()
    {
      return mRspPtr;
    }
};


class GstkRilRequestMessage : public SolicitedMessage<generic_callback_payload>,
                              public add_message_id<GstkRilRequestMessage>
                            
{
  private:
    uint8_t                  * mParamPtr;
    int                        mMsgId;
    uint32_t                   mParamLen;
    RIL_Token                  mToken;
    qcril_instance_id_e_type   mInstance;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.gstk.ril_request_msg";
    GstkRilRequestMessage() = delete;

    inline ~GstkRilRequestMessage()
    {
      if (mParamPtr != nullptr)
      {
        delete[] mParamPtr;
        mParamPtr = nullptr;
      }
    }

    inline GstkRilRequestMessage(const ril_request_type *req_ptr) :
      SolicitedMessage<generic_callback_payload>(get_class_message_id())
    {
      mParamPtr = nullptr;
      mParamLen = 0;
      mMsgId = req_ptr->req_id_android;
      mToken = req_ptr->t;
      mInstance = req_ptr->instance_id;
      if (req_ptr->payload != nullptr &&
          req_ptr->payload_len > 0)
      {
        switch(mMsgId)
        {
          case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
          case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
          case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
            mParamLen = (uint32_t)strlen((char *)req_ptr->payload);
            mParamPtr = new uint8_t[mParamLen + 1]{0};
            if (mParamPtr != nullptr)
            {
              memcpy(mParamPtr, req_ptr->payload, mParamLen);
            }
            else
            {
              mParamLen = 0;
            }
            break;
          default:
            mParamPtr = new uint8_t[req_ptr->payload_len]{0};
            if (mParamPtr != nullptr)
            {
              mParamLen = (uint32_t)req_ptr->payload_len;
              memcpy(mParamPtr, req_ptr->payload, mParamLen);
            }
            break;
        }
      }
      mName = MESSAGE_NAME;
    }

    inline uint8_t *get_params(uint32_t *len)
    {
      *len = mParamLen;
      return mParamPtr;
    }

    inline int get_req_id()
    {
      return mMsgId;
    }

    inline qcril_instance_id_e_type get_instance_id()
    {
      return mInstance;
    }

    inline RIL_Token get_token()
    {
      return mToken;
    }

    inline string dump()
    {
      return mName + "{ msg_id: " + std::to_string(mMsgId) + " }";
    }
};

