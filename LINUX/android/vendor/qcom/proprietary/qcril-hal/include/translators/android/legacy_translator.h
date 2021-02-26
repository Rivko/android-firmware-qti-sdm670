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
#define TAG "RILQ-legacy_translator"
#include <framework/Log.h>
#include <modules/android/ril_message_factory.h>
#include <modules/android/RilRequestMessage.h>

#define RIL_REQUEST_DIAL_PAYLOAD RIL_Dial *
#define RIL_REQUEST_DIAL_RSP_PAYLOAD void *

template <int REQ_ID, class RSP_TYPE, class UNSL_TYPE>
class legacy_translator_base: public android_ril_translator
{
    static_assert(std::is_base_of<legacy_response_payload, RSP_TYPE>::value, "Response type has to be a legacy_response_payload");
    private:
    public:
        legacy_translator_base() {};
        virtual std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request) = 0;
        virtual std::shared_ptr<ril_response_type> translate_response(std::shared_ptr<SolicitedMessage<generic_callback_payload>>, Message::Callback::Status, std::shared_ptr<generic_callback_payload>) = 0;
        virtual std::shared_ptr<ril_unsol_type> translate_unsol(std::shared_ptr<UnSolicitedMessage> msg) = 0;
        virtual ~legacy_translator_base() {};
};

template <int REQ_ID>
class legacy_translator: public legacy_translator_base<REQ_ID, legacy_response_payload, legacy_unsol_payload>
{
    private:
    public:
        legacy_translator();
        void register_for_request() {
            QCRIL_ANDROID_REGISTER_REQUEST(REQ_ID, unused);
        }
        //virtual std::shared_ptr<SolicitedMessage<generic_callback_payload>> translate_request(const ril_request_type &request) override;
        virtual std::shared_ptr<ril_response_type> translate_response(std::shared_ptr<SolicitedMessage<generic_callback_payload>>, Message::Callback::Status, std::shared_ptr<generic_callback_payload>) override;
        virtual std::shared_ptr<ril_unsol_type> translate_unsol(std::shared_ptr<UnSolicitedMessage> msg) override
        {
            QCRIL_NOTUSED(msg);
            return std::shared_ptr<ril_unsol_type>();
        };
};

template <int REQ_ID>
legacy_translator<REQ_ID>::legacy_translator()
{
    register_for_request();
}

template <int REQ_ID>
std::shared_ptr<ril_response_type> legacy_translator<REQ_ID>::translate_response(
        std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg,
        Message::Callback::Status status,
        std::shared_ptr<generic_callback_payload> payload)
{
    std::shared_ptr<ril_response_type> ret;
    RIL_Errno ril_errno = RIL_E_SUCCESS;
    QCRIL_NOTUSED(msg);
    if ( status != Message::Callback::Status::SUCCESS) {
        ril_errno = RIL_E_INTERNAL_ERR;
    }

    std::shared_ptr<legacy_response_payload> rsp = std::static_pointer_cast<legacy_response_payload>(payload);
    if (rsp) {
        const qcril_request_resp_params_type &params = rsp->get_params();
        if (status != Message::Callback::Status::SUCCESS) {
            ril_errno = RIL_E_INTERNAL_ERR;
        }
        else
        {
            ril_errno = params.ril_err_no;
        }
        ret = std::make_shared<ril_response_type>(ril_response_type{params.instance_id,
                params.request_id_android,
                params.t,
                ril_errno,
                params.resp_pkt,
                params.resp_len});
    }
    return ret;
}
