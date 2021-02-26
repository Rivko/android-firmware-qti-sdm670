/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/* -----------------------------------------------------------------------------
   ENUM:      RIL_UiccPower

   DESCRIPTION:
     Used to convey respective power command to the SIM card
-------------------------------------------------------------------------------*/
typedef enum
{
  UIM_UICC_POWER_DOWN = 0,
  UIM_UICC_POWER_UP,
  UIM_UICC_POWER_UP_PASS_THRU
} UIM_UiccPower;

/*===========================================================================

  CLASS:  UimCardPowerReqMsg

===========================================================================*/
/*!
    @brief
    UIM Message request for card power up and power down.

    0 - card power down
    1 - card power up

    @return
    None.
*/
/*=========================================================================*/
class UimCardPowerReqMsg : public SolicitedMessage<void>,
                           public add_message_id<UimCardPowerReqMsg>
{
  private:
    uint8_t             mSlot;
    UIM_UiccPower       mCardPower;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.card_power_request";
    UimCardPowerReqMsg() = delete;
    ~UimCardPowerReqMsg() = default;

    inline UimCardPowerReqMsg(UIM_UiccPower  card_power,
                              uint8_t        slot) :
                              SolicitedMessage<void>(get_class_message_id())
    {
      mCardPower = card_power;
      mSlot = slot;
      mName = MESSAGE_NAME;
    }

    inline string dump()
    {
      return mName + "{}" + "slot: " + std::to_string(mSlot) + " Power Req:" + std::to_string(mCardPower);
    }

    inline UIM_UiccPower get_card_power_req(void)
    {
      return mCardPower;
    }

    inline uint8_t get_slot(void)
    {
      return mSlot;
    }
};
