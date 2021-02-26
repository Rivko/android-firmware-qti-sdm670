/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef UIM_CARD_STATUS_IND_MSG_H
#define UIM_CARD_STATUS_IND_MSG_H
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/* Card status */
typedef enum
{
  QCRIL_CARD_STATUS_UNKNOWN          = 0, /*!< Internal use only */
  QCRIL_CARD_STATUS_NOT_APPLICABLE   = 1, /*!< Indicates that NV is the only subscription source */
  QCRIL_CARD_STATUS_DOWN             = 2, /*!< Indicates that card had been power down */
  QCRIL_CARD_STATUS_UP               = 3, /*!< Indicates that card has been power up */
  QCRIL_CARD_STATUS_NOT_ACCESSIBLE   = 4, /*!< Indicates that card is not accessible */
  QCRIL_CARD_STATUS_REFRESH          = 5, /*!< Indicates that card refresh */
  QCRIL_CARD_STATUS_POWERUP_FAILED   = 6, /*!< Indicates that card powerup failed */
  QCRIL_CARD_STATUS_POWERDOWN_FAILED = 7, /*!< Indicates that card powerdown failure */
  QCRIL_CARD_STATUS_ILLEGAL          = 8, /*!< Indicates that card is illegal, for internal use */
  QCRIL_CARD_STATUS_ABSENT           = 9, /*!< Indicates that card is absent */
  QCRIL_CARD_STATUS_SIM_READY        = 10, /*!< Indicates that SIM is ready */
} qcril_card_status_e_type;

/*! @brief Card status for a Slot
*/
typedef struct
{
  int slot;                              /* Slot where the card resides */
  qcril_card_status_e_type status;       /* Status of the card */
} qcril_card_info_type;

class UimCardStatusIndMsg : public UnSolicitedMessage,
                            public add_message_id<UimCardStatusIndMsg>
{
  private:
    qcril_card_info_type       mCardInfo;

  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.card_status_update";
    UimCardStatusIndMsg() = delete;
    ~UimCardStatusIndMsg() = default;

    inline UimCardStatusIndMsg(qcril_card_info_type  *card_info):
                       UnSolicitedMessage(get_class_message_id())
    {
      mCardInfo.slot = card_info->slot;
      mCardInfo.status = card_info->status;
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage> clone()
    {
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimCardStatusIndMsg>(&mCardInfo));
    }

    inline string dump()
    {
      return mName + "{}" + "slot: " + std::to_string(mCardInfo.slot) + "Status" + std::to_string(mCardInfo.status);
    }

    inline  qcril_card_info_type *get_card_status(void)
    {
      return &mCardInfo;
    }
};
#endif
