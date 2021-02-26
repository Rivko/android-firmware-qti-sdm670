/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "telephony/ril.h"
#include "framework/GenericCallback.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"


/*===========================================================================*/
/*!
    @brief
    Provides IMSI of requested aid of a slot.

    mSlot     - In Slot on which IMSI needed
    mAidPtr   - In Pointer to AID on which IMSI is needed
    mImsiPtr  - Out IMSI ptr address, UIM allocates and frees the memory

    Retruns RIL_Errno shared pointer
*/
/*==========================================================================*/
class UimGetImsiRequestSyncMsg : public SolicitedSyncMessage<RIL_Errno>,
                                 public add_message_id<UimGetImsiRequestSyncMsg>
{
  private:
    uint8_t   mSlot;
    char     *mAidPtr;
    char     **mImsiPtr;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_imsi_sync_request";
    UimGetImsiRequestSyncMsg() = delete;
    inline ~UimGetImsiRequestSyncMsg()
    {
      mAidPtr = nullptr;
      mImsiPtr = nullptr;
    }

    UimGetImsiRequestSyncMsg(uint8_t slot, char *aid_ptr, char **imsi_ptr) :
                      SolicitedSyncMessage<RIL_Errno>(get_class_message_id())
    {
      mSlot = slot;
      mAidPtr = aid_ptr;
      mName = MESSAGE_NAME;
      mImsiPtr = imsi_ptr;
    }

    inline uint8_t get_slot(void)
    {
      return mSlot;
    }

    inline char *get_aid(void)
    {
      return mAidPtr;
    }

    inline char **get_imsi_ptr_address(void)
    {
      return mImsiPtr;
    }

    inline string dump()
    {
      if (mAidPtr != nullptr)
      {
        return mName + "{ slot :" + std::to_string(mSlot) + "} AidPtr: " + mAidPtr;
      }
      else
      {
        return mName + "{ slot :" + std::to_string(mSlot) + "} AidPtr: NULL";
      }
    }
};
