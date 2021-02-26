/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

typedef enum
{
  UIM_LPA_UNKOWN_IND_ID,
  UIM_LPA_PROFILE_PROGRESS_IND_ID
} uimLpaIndicationType;

typedef enum
{
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_NONE                  = 0,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_ERROR                 = 1,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS     = 2,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS = 3,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE = 4,
  QCRIL_UIM_LPA_ADD_PROFILE_STATUS_GET_USER_CONSENT      = 5
} qcril_uim_lpa_add_profile_status_type;

typedef enum
{
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE    = 0,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC = 1,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM     = 2,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK = 3,
  QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY  = 4
} qcril_uim_lpa_add_profile_failure_cause;

typedef enum
{
  QCRIL_UIM_LPA_PROFILE_TYPE_IND_DISABLE_NOT_ALLOWED = 0x0000000000000001,
  QCRIL_UIM_LPA_PROFILE_TYPE_IND_DELETE_NOT_ALLOWED  = 0x0000000000000002,
  QCRIL_UIM_LPA_PROFILE_TYPE_IND_DELETE_ON_DISABLE   = 0x0000000000000004
} qcril_uim_lpa_profile_ind_policy_mask;

typedef struct
{
  qcril_uim_lpa_add_profile_status_type    status;
  qcril_uim_lpa_add_profile_failure_cause  cause;
  int32_t                                  progress;
  qcril_uim_lpa_profile_ind_policy_mask    policyMask;
  bool                                     userConsentRequired;
} qcril_uim_lpa_add_profile_progress_ind_type;


class UimLpaIndicationMsg : public UnSolicitedMessage,
                            public add_message_id<UimLpaIndicationMsg>
{
  private:
    void                 * mDataPtr;
    uimLpaIndicationType   mIndId;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.lpa_indication";

    UimLpaIndicationMsg() = delete;

    inline ~UimLpaIndicationMsg()
    {
      if (mDataPtr != nullptr)
      {
        delete (uint8_t *)mDataPtr;
        mDataPtr = nullptr;
      }
    }

    inline UimLpaIndicationMsg(uimLpaIndicationType ind_id, void * data_ptr):
                       UnSolicitedMessage(get_class_message_id())
    {
      mDataPtr = nullptr;
      mIndId = ind_id;

      switch (ind_id)
      {
        case UIM_LPA_PROFILE_PROGRESS_IND_ID:
          if (data_ptr != nullptr)
          {
            qcril_uim_lpa_add_profile_progress_ind_type *ind_data_ptr =
                        new qcril_uim_lpa_add_profile_progress_ind_type;

            if (ind_data_ptr != nullptr)
            {
              memcpy(ind_data_ptr, data_ptr, sizeof(qcril_uim_lpa_add_profile_progress_ind_type));
            }
            mDataPtr = ind_data_ptr;
          }
          break;
        default:
          break;
      }
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimLpaIndicationMsg>(mIndId, mDataPtr));
    }

    inline string dump()
    {
      return mName + "{}" + "Ind Id: " + std::to_string(mIndId);
    }

    inline void *get_data_ptr()
    {
      return mDataPtr;
    }

    inline uimLpaIndicationType get_ind_id()
    {
      return mIndId;
    }
};
