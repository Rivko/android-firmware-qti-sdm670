/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once

#include <memory>
#include <framework/SolicitedMessage.h>
#include <framework/UnSolicitedMessage.h>
#include <framework/message_translator.h>
#include <modules/android/ril_message_factory.h>

class ril_request_uim_translator: public android_ril_translator
{
    private:
    public:
        ril_request_uim_translator();
        std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request);
        std::shared_ptr<ril_response_type> translate_response(std::shared_ptr<SolicitedMessage<generic_callback_payload>>, Message::Callback::Status, std::shared_ptr<generic_callback_payload>);
        std::shared_ptr<ril_unsol_type> translate_unsol(std::shared_ptr<UnSolicitedMessage> msg);
        ~ril_request_uim_translator();
};
