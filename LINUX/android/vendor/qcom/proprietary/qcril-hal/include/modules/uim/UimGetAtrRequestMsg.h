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
#include "telephony/ril.h"

typedef struct
{
  RIL_Errno     err;
  char        * atr;
  size_t        atr_len;
} UimAtrRspParam;


/*===========================================================================*/
/*!
    @brief
    Provides Atr of requested slot.

    Retruns RIL_Errno shared pointer
*/
/*==========================================================================*/
class UimGetAtrRequestMsg : public SolicitedMessage<UimAtrRspParam>,
                              public add_message_id<UimGetAtrRequestMsg>
{
  private:
    RIL_Token    mToken;
    int          mSlot;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_atr_request";
    UimGetAtrRequestMsg() = delete;
    ~UimGetAtrRequestMsg() = default;

    UimGetAtrRequestMsg(RIL_Token token, GenericCallback<UimAtrRspParam> *callback):
                          SolicitedMessage<UimAtrRspParam>(get_class_message_id())
    {
      mToken = token;
      mName = MESSAGE_NAME;
      mSlot = (int)qmi_ril_get_process_instance_id();
      setCallback(callback);
    }

    inline RIL_Token get_token(void)
    {
      return mToken;
    }

    inline int *get_slot(void)
    {
      return &mSlot;
    }

    inline string dump()
    {
      return mName;
    }
};
