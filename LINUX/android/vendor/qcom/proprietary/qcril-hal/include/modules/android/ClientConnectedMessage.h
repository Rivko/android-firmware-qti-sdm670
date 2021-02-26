/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef MODULES_ANDROID_CLIENT_CONNECTED_MESSAGE_H
#define MODULES_ANDROID_CLIENT_CONNECTED_MESSAGE_H

#include <framework/UnSolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/Dispatcher.h>
#include <framework/add_message_id.h>

/**
  * ClientConnectedMessage: Sent when an android client connects.
  * Modules can register for this message to perform actions
  * on connect (e.g. sending unsols)
  */
class ClientConnectedMessage: public UnSolicitedMessage, public add_message_id<ClientConnectedMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.android.client_connected";
    private:
        qcril_instance_id_e_type instance_id;
    public:
        ClientConnectedMessage(qcril_instance_id_e_type instance):
            UnSolicitedMessage(get_class_message_id())
        {
            this->instance_id = instance;
        }
         ~ClientConnectedMessage() { }
        virtual string dump() {
            return MESSAGE_NAME;
        }
        std::shared_ptr<UnSolicitedMessage> clone() override {
            return (std::make_shared<ClientConnectedMessage>(instance_id));
        }
};

#endif
