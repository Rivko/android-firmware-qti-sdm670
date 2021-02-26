/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef LPA_INIT_MESSAGE_H_INCLUDED
#define LPA_INIT_MESSAGE_H_INCLUDED

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
  * LpaInitMessage: Sent by the LPA module.
  * Indicates that LPA module is ready to receive requests.
  *
  */

class LpaInitMessage: public UnSolicitedMessage, public add_message_id<LpaInitMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.lpa.init";
    public:
        string dump() {
            return LpaInitMessage::MESSAGE_NAME;
        }
        LpaInitMessage():
            UnSolicitedMessage(get_class_message_id())
        {
            mName = MESSAGE_NAME;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            return (std::make_shared<LpaInitMessage>());
        }

        inline ~LpaInitMessage() {}
};

#endif
