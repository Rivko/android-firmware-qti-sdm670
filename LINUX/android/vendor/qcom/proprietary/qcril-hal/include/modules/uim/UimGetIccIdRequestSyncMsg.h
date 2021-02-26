/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"
#include "telephony/ril.h"


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
class UimGetIccIdRequestSyncMsg : public SolicitedSyncMessage<RIL_Errno>,
                                  public add_message_id<UimGetIccIdRequestSyncMsg>
{
  private:
    uint8_t    mSlot;
    uint8_t   *mIccidPtr;
    uint8_t   *mIccidLenPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_iccid_sync_request";
    UimGetIccIdRequestSyncMsg() = delete;
    ~UimGetIccIdRequestSyncMsg() = default;

    UimGetIccIdRequestSyncMsg(uint8_t slot, uint8_t *iccid_ptr, uint8_t *iccid_len_ptr) :
                                SolicitedSyncMessage<RIL_Errno>(get_class_message_id())
    {
      mSlot = slot;
      mName = MESSAGE_NAME;
      mIccidPtr = iccid_ptr;
      mIccidLenPtr = iccid_len_ptr;
    }

    inline uint8_t get_slot(void)
    {
      return mSlot;
    }

    inline uint8_t *get_iccid_ptr(void)
    {
      return mIccidPtr;
    }

    inline uint8_t *get_iccid_len_ptr(void)
    {
      return mIccidLenPtr;
    }

    inline string dump()
    {
      return mName + "{ slot :" + std::to_string(mSlot) + "}";
    }
};
