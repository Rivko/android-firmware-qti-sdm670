/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "telephony/ril.h"
#include "framework/add_message_id.h"

/*===========================================================================*/
/*!
    @brief
    Provides ICCID of requested slot.

    mSlot            - In Slot on which IMSI needed
    mCardStatusPtr   - out Pointer to RIL_CardStatus_v6

    Retruns RIL_Errno shared pointer
*/
/*==========================================================================*/
class UimGetCardStatusRequestMsg : public SolicitedMessage<RIL_CardStatus_v6>,
                                   public add_message_id<UimGetCardStatusRequestMsg>
{
  private:
    uint8_t              mSlot;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_card_status_request";
    UimGetCardStatusRequestMsg() = delete;
    ~UimGetCardStatusRequestMsg() {}

    UimGetCardStatusRequestMsg(uint8_t slot,
                               GenericCallback<RIL_CardStatus_v6> *callback) :
                               SolicitedMessage<RIL_CardStatus_v6>(get_class_message_id())
    {
      mSlot = slot;
      mName = MESSAGE_NAME;
      setCallback(callback);
    }

    inline uint8_t get_slot(void)
    {
      return mSlot;
    }

    inline string dump()
    {
      return mName + "{ slot :" + std::to_string(mSlot) + "}";
    }
};
