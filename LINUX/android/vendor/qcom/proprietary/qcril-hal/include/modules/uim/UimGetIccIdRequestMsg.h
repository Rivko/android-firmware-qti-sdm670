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
#include "telephony/ril.h"

#define QCRIL_UIM_ICCID_LEN   10

typedef struct
{
  uint8_t   iccid[QCRIL_UIM_ICCID_LEN];
  uint8_t   iccid_len;
} UimIccidRspParam;


/*===========================================================================*/
/*!
    @brief
    Provides ICCID of requested slot.

    mSlot       - In Slot on which IMSI needed
    mIccidPtr   - out Pointer to ICCID
    mIccidLen   - Out ICCID len

    Retruns RIL_Errno shared pointer
*/
/*==========================================================================*/
class UimGetIccIdRequestMsg : public SolicitedMessage<UimIccidRspParam>,
                              public add_message_id<UimGetIccIdRequestMsg>
{
  private:
    uint8_t    mSlot;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_iccid_request";
    UimGetIccIdRequestMsg() = delete;
    ~UimGetIccIdRequestMsg() = default;

    UimGetIccIdRequestMsg(uint8_t slot, GenericCallback<UimIccidRspParam> *callback):
                          SolicitedMessage<UimIccidRspParam>(get_class_message_id())
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
