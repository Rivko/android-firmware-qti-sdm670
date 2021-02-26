/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef UIM_PROVISION_STATUS_IND_MSG_H
#define UIM_PROVISION_STATUS_IND_MSG_H
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

typedef enum
{
  UIM_DEACTIVATE_PROVISION_STATUS,
  UIM_ACTIVATE_PROVISION_STATUS,
} uim_provision_status_type;

typedef enum
{
  UIM_SESSION_TYPE_PRI_GW_PROV          = 0,
  UIM_SESSION_TYPE_PRI_1X_PROV          = 1,
  UIM_SESSION_TYPE_SEC_GW_PROV          = 2,
  UIM_SESSION_TYPE_SEC_1X_PROV          = 3,
  UIM_SESSION_TYPE_NON_PROV_SLOT_1      = 4,
  UIM_SESSION_TYPE_NON_PROV_SLOT_2      = 5,
  UIM_SESSION_TYPE_CARD_SLOT_1          = 6,
  UIM_SESSION_TYPE_CARD_SLOT_2          = 7,
  UIM_SESSION_TYPE_TER_GW_PROV          = 10,
  UIM_SESSION_TYPE_TER_1X_PROV          = 11,
  UIM_SESSION_TYPE_NON_PROV_SLOT_3      = 16,
  UIM_SESSION_TYPE_CARD_SLOT_3          = 19
} uim_session_type;

/* Provision status */
typedef enum
{
  QCRIL_PROVISION_STATUS_FAILURE     = 0, /*!< Provision success */
  QCRIL_PROVISION_STATUS_SUCCESS     = 1, /*!< Provision failure */
  QCRIL_PROVISION_STATUS_IN_PROGRESS = 2  /*!< Provision in progress */
} qcril_provision_status_e_type;

/*! @brief Provision status
*/
typedef struct
{
  qcril_provision_status_e_type  status;       /* Status of the provision */
  uim_session_type               session_type; /* UIM session type on which this subscription is currently provisioned */
  uint32_t                       err_code;     /* RIL_Errno */
} qcril_provision_info_type;

class UimProvisionStatusIndMsg : public UnSolicitedMessage,
                                 public add_message_id<UimProvisionStatusIndMsg>
{
  private:
    qcril_provision_info_type       mProvInfo;
    uim_provision_status_type       mProvType;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.app_provisioning_status";
    UimProvisionStatusIndMsg() = delete;
    ~UimProvisionStatusIndMsg() = default;

    UimProvisionStatusIndMsg(uim_provision_status_type   prov_type,
                             qcril_provision_info_type  *prov_info_ptr) :
                                UnSolicitedMessage(get_class_message_id())
    {
      mProvInfo.status = prov_info_ptr->status;
      mProvInfo.session_type = prov_info_ptr->session_type;
      mProvInfo.err_code = prov_info_ptr->err_code;
      mProvType = prov_type;
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {    
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimProvisionStatusIndMsg>(mProvType, &mProvInfo));
    }

    inline string dump()
    {
      return mName + "{}" + "Provision status type: " + std::to_string(mProvType);
    }

    inline qcril_provision_info_type *get_provision_status_info(void)
    {
      return &mProvInfo;
    }

    inline uim_provision_status_type get_provision_type(void)
    {
      return mProvType;
    }
};
#endif
