/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <functional>
#include <memory.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Module.h>

class CoreModule: public Module {
    private:
    public:
        CoreModule() {
            mName = "com.qualcomm.qti.qcril.core";
            mLooper = NULL;
            using std::placeholders::_1;
            mMessageHandler = {
                HANDLER(QcrilInitMessage, CoreModule::handleQcrilInit),
            };
        }
        void handleQcrilInit(std::shared_ptr<QcrilInitMessage> msg)
        {
            (void) msg;
            std::shared_ptr<QcrilInitMessage> themsg = std::static_pointer_cast<QcrilInitMessage>(msg);
            //qcril_init(themsg->get_instance_id());
        }

};
