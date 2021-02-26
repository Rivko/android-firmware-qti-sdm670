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

#define FLOCK_GET_SIZE 4

typedef struct
{
  RIL_Errno  ril_err;
  /* Lock_status is valid only if ril_err is RIL_E_SUCCESS */
  int        lock_status;
} UimGetFacilityLockResponseMsg;

/*   mInptr[0]: facility string code
     mInptr[1]: password
     mInptr[2]: service class bit (unused)
     mInptr[3]: AID value */
class UimGetFacilityLockRequestMsg : public SolicitedMessage<UimGetFacilityLockResponseMsg>,
                                     public add_message_id<UimGetFacilityLockRequestMsg>
                                     
{
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.get_facility_lock";
    enum facility_lock_type
    {
      FDN,
      PIN1
    };

    UimGetFacilityLockRequestMsg() = default;
    inline ~UimGetFacilityLockRequestMsg()
    {
      uint8_t i = 0;
      for ( i = 0; i < FLOCK_GET_SIZE; i++)
      {
        if (mFLockPtr[i] != nullptr)
        {
          delete[] mFLockPtr[i];
          mFLockPtr[i] = nullptr;
        }
      }
    }

    inline UimGetFacilityLockRequestMsg(char **flock_ptr, facility_lock_type flock_type,
                                        GenericCallback<UimGetFacilityLockResponseMsg> *callback) :
                                        SolicitedMessage<UimGetFacilityLockResponseMsg>(get_class_message_id())
    {
      uint8_t  i = 0;

      mName = MESSAGE_NAME;
      mFLockType = flock_type;

      for ( i = 0; i < FLOCK_GET_SIZE; i++)
      {
        mFLockPtr[i] = nullptr;
        if (flock_ptr[i] != nullptr)
        {
          uint32_t   str_len = strlen(flock_ptr[i]);

          mFLockPtr[i] = new char[str_len + 1];
          strncpy(mFLockPtr[i], flock_ptr[i], str_len);
          mFLockPtr[i][str_len] = '\0';
        }
      }
      setCallback(callback);
    }

    inline char ** get_facility_lock_data()
    {
      return mFLockPtr;
    }

    inline facility_lock_type get_facility_lock_type()
    {
      return mFLockType;
    }

    inline string dump()
    {
      return mName + "{}";
    }

  private:
    char               *mFLockPtr[FLOCK_GET_SIZE];
    facility_lock_type mFLockType;
};
