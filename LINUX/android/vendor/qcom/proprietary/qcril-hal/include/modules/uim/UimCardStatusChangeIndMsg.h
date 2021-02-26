/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once
#include "framework/UnSolicitedMessage.h"
#include "framework/Message.h"
#include "framework/add_message_id.h"

/*===========================================================================*/
/*!
    @brief
    Modem Card status Indication message.

    Need to call UimGetCardStatusRequestSyncMsg after getting this indication
    to know about card status.
*/
/*==========================================================================*/
class UimCardStatusChangeIndMsg : public UnSolicitedMessage,
                                  public add_message_id<UimCardStatusChangeIndMsg>
{
  public:
    static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.uim.card_status_change_ind";
    ~UimCardStatusChangeIndMsg() = default;

    UimCardStatusChangeIndMsg(void) : 
           UnSolicitedMessage(get_class_message_id())
    {
      mName = MESSAGE_NAME;
    }

    inline std::shared_ptr<UnSolicitedMessage>  clone()
    {
      
      return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimCardStatusChangeIndMsg>());
    }

    inline string dump()
    {
      return mName + "{}";
    }
};
