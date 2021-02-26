/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedSyncMessage.h"
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
class UimGetCardStatusRequestSyncMsg : public SolicitedSyncMessage<RIL_Errno>,
                                       public add_message_id<UimGetCardStatusRequestSyncMsg>
{
  private:
    uint8_t              mSlot;
    RIL_CardStatus_v6   *mCardStatusPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_card_status_sync_request";
    UimGetCardStatusRequestSyncMsg() = delete;
    ~UimGetCardStatusRequestSyncMsg() {}

    UimGetCardStatusRequestSyncMsg(uint8_t slot,
                                   RIL_CardStatus_v6 *card_status_ptr) :
                                   SolicitedSyncMessage<RIL_Errno>(get_class_message_id())
    {
      mSlot = slot;
      mName = MESSAGE_NAME;
      mCardStatusPtr = card_status_ptr;
    }

    inline uint8_t get_slot(void)
    {
      return mSlot;
    }

    inline RIL_CardStatus_v6 *get_card_status_ptr(void)
    {
      return mCardStatusPtr;
    }

    inline string dump()
    {
      return mName + "{ slot :" + std::to_string(mSlot) + "}";
    }
};
