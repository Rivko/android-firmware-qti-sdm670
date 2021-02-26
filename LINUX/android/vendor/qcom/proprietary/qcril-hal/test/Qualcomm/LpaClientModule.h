/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef QMI_RIL_UTF
#pragma once
#include "framework/Module.h"
#include "modules/lpa/LpaMessageTypes.h"

extern "C" {
#include <framework/qcrili.h>
}

class LpaClientModule : public Module {
    public:
        LpaClientModule();
        ~LpaClientModule();

        void init();

        void sendUserReqMsg(qcril_instance_id_e_type instance_id,
            qcril_modem_id_e_type modem_id, qcril_evt_e_type uim_req,
            void *data, size_t datalen, RIL_Token token);

        inline lpa_MessageType getLastMsgTypeHandled() { return m_last_msg_type_handled; }

    private:
        qcril_evt_e_type m_uim_req;
        lpa_MessageType m_last_msg_type_handled;

        void handleQcrilInitMessage(std::shared_ptr<Message> msg);
        void handleLpaResponseMessage(std::shared_ptr<Message> msg);
};




class UimStubModule : public Module {
    public:
        UimStubModule();
        ~UimStubModule();

        void init();

        void sendLpaResponseMessage(std::shared_ptr<Message> msg,
                                    bool has_token, RIL_Token token,
                                    lpa_MessageType type,
                                    lpa_MessageId message_id,
                                    bool has_error,
                                    lpa_Error error,
                                    const void* msgbuf, int msg_len);
        inline lpa_MessageType getLastMsgTypeHandled() { return m_last_msg_type_handled; }

    private:
        lpa_MessageType m_last_msg_type_handled;
        void handleLpaUserReqMessage(std::shared_ptr<Message> msg);
};
#endif
