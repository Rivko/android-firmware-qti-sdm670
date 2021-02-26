/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef FRAMEWORK_QCRIL_INIT_MESSAGE_H_INCLUDED
#define FRAMEWORK_QCRIL_INIT_MESSAGE_H_INCLUDED

#include <framework/legacy.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/add_message_id.h>

/**
  * QcrilInitMessage: Sent by the main interface module (e.g. Android)
  * to request the core to perform initialization.
  * This will trigger invocation of the legacy qcril_init function.
  *
  */

class QcrilInitMessage: public UnSolicitedMessage,
                        public add_message_id<QcrilInitMessage>
{
    public:
        static constexpr const char *MESSAGE_NAME = "com.qualcomm.qti.qcril.core.qcril_init";

    private:
        qcril_instance_id_e_type instance_id;
    public:
        string dump() {
            return mName + "{}";
        }
        QcrilInitMessage(qcril_instance_id_e_type instance_id):
            UnSolicitedMessage(get_class_message_id()),
            instance_id(instance_id)
        {
            mName = MESSAGE_NAME;
        }

        std::shared_ptr<UnSolicitedMessage> clone() {
            return (std::make_shared<QcrilInitMessage>(instance_id));
        }

        qcril_instance_id_e_type get_instance_id()
        {
            return instance_id;
        }

        inline ~QcrilInitMessage() {}
};

#endif
