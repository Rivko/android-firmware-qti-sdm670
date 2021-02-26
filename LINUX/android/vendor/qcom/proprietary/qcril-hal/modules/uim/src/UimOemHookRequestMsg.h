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
#include "framework/legacy.h"

typedef struct
{
  qcril_instance_id_e_type instance_id;
  RIL_Token                token;
  RIL_Errno                result;
  qcril_evt_e_type         request;
  void*                    rsp_ptr;
  size_t                   rsp_len;
  char*                    logstr;
} UimOemHookRspData;

class UimOemHookRequestMsg : public SolicitedMessage<UimOemHookRspData>,
                             public add_message_id<UimOemHookRequestMsg>
                            
{
  private:
    qcril_request_params_type    mParams;
    qcril_request_return_type  * mRetPtr;
    int                          mMsgId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.oem_hook_msg";
    UimOemHookRequestMsg() = delete;
    ~UimOemHookRequestMsg();

    UimOemHookRequestMsg(int msg_id,
                         const qcril_request_params_type *const params_ptr,
                         qcril_request_return_type * const ret_ptr,
                         GenericCallback<UimOemHookRspData> *callback);

    void deep_clean
    (
      int    request_type,
      void  *data_ptr
    );

    void deep_copy
    (
      int         request_type,
      void       *in_ptr,
      uint32_t    in_len,
      void      **out_ptr
    );

    inline qcril_request_params_type * get_params_ptr(void)
    {
      return &mParams;
    }

    inline qcril_request_return_type * get_ret_ptr(void)
    {
      return mRetPtr;
    }

    inline int get_msg_id(void)
    {
      return mMsgId;
    }

    inline string dump()
    {
      return mName + "{ " + std::to_string(mMsgId) + "}";
    }
};
