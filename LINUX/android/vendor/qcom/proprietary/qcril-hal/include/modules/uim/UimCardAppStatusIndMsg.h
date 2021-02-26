/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef UIM_CARD_APP_STATUS_IND_MSG_H
#define UIM_CARD_APP_STATUS_IND_MSG_H
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/*! @brief App status
*/
typedef struct
{
  uint32_t                slot;
  uint8                   aid_len;/* Number of bytes in aid_value */
  char                    aid_value[QMI_UIM_MAX_AID_LEN]; /* Binary AID value */
  qmi_uim_app_type        app_type; /* Type of app */
  qmi_uim_app_state_type  app_state; /* State of the app */
}qcril_card_app_info_type;


class UimCardAppStatusIndMsg : public UnSolicitedMessage,
                               public add_message_id<UimCardAppStatusIndMsg>
{
  private:
    qcril_card_app_info_type       mAppInfo;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.card_app_status_change_ind";
    UimCardAppStatusIndMsg() = delete;
    ~UimCardAppStatusIndMsg() = default;

    inline UimCardAppStatusIndMsg(qcril_card_app_info_type   *app_info_ptr) :
                                  UnSolicitedMessage(get_class_message_id())
    {
      memcpy(&mAppInfo, app_info_ptr, sizeof(mAppInfo));
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimCardAppStatusIndMsg>(&mAppInfo));
    }

    inline string dump()
    {
      return mName + "{}";
    }

    inline qcril_card_app_info_type *get_app_info(void)
    {
      return &mAppInfo;
    }
};
#endif
