/******************************************************************************
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

class UimGstkCardStatusMsg : public SolicitedMessage<void>,
                             public add_message_id<UimGstkCardStatusMsg>
{
  private:
    int     mSlot;
    boolean isRefresh;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.gstk_card_status_msg";
    UimGstkCardStatusMsg() = delete;
    inline ~UimGstkCardStatusMsg(){}

    UimGstkCardStatusMsg(int slot, boolean is_refresh):
                         SolicitedMessage<void>(get_class_message_id())
    {
      mName = MESSAGE_NAME;
      mSlot = slot;
      isRefresh = is_refresh;
    }

    inline int get_slot_id(void)
    {
      return mSlot;
    }

    inline boolean is_refresh(void)
    {
      return isRefresh;
    }

    inline string dump()
    {
      return mName + "{ slot: " + std::to_string(mSlot) + ", Is refresh:" + std::to_string(isRefresh) + "}";
    }
};

