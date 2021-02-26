#pragma once
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <memory>
#include <telephony/ril.h>
#include <framework/SolicitedMessage.h>
#include <framework/legacy.h>
#include <framework/message_translator.h>
#include <modules/android/ril_request_info.h>
#include <modules/android/qcril_android_event_context.h>

struct legacy_request_payload
{
    private:
        qcril_request_params_type params;
        void copy_data_from(const void *data, size_t datalen)
        {
            if (data) {
                this->params.data = calloc(1, datalen);
                memcpy(this->params.data, data, datalen);
                this->params.datalen = params.datalen;
            }
        }

        void copy_data_from(const qcril_request_params_type &params)
        {
            copy_data_from(params.data, params.datalen);
        }

    public:
        qcril_request_params_type &get_params() {
            return params;
        }
        legacy_request_payload() {
        }

        legacy_request_payload(
                qcril_instance_id_e_type instance_id,
                qcril_modem_id_e_type    modem_id,
                int req_id,
                const void *data,
                size_t datalen,
                RIL_Token token):
            params {
                instance_id,
                modem_id,
                req_id,
                NULL,
                NULL,
                datalen,
                token,
                NULL
                }
        {
            copy_data_from(data, datalen);
        }
        legacy_request_payload(
                const qcril_request_params_type &params):
            params(params)
        {
            copy_data_from(params);
        }

        legacy_request_payload(
                qcril_request_params_type &&params):
            params(std::move(params))
        {
        }

        legacy_request_payload &operator= (const qcril_request_params_type &params)
        {
            this->params = params;
            copy_data_from(params);
            return *this;
        }

        legacy_request_payload &operator= (qcril_request_params_type &&params)
        {
            this->params = params;
            std::swap(this->params, params);
            return *this;
        }

        legacy_request_payload &operator= (const legacy_request_payload &o)
        {
            *this = o.params;
            return *this;
        }

        legacy_request_payload &operator= (legacy_request_payload &&o)
        {
            *this = std::move(o.params);
            return *this;
        }

        ~legacy_request_payload() {
            free(params.data);
            params.data = NULL;
        }

};

struct legacy_response_payload: public generic_callback_payload
{
    private:
        qcril_request_resp_params_type params;
        void copy_data_from(const void *data, size_t datalen)
        {
            if (data) {
                this->params.resp_pkt = calloc(1, datalen);
                memcpy(this->params.resp_pkt, data, datalen);
                this->params.resp_len = datalen;
            }
        }

    public:
        legacy_response_payload() {
        }
        legacy_response_payload(qcril_instance_id_e_type instance_id,
                RIL_Token token,
                int req_id,
                RIL_Errno ril_errno,
                void *data,
                size_t datalen):
            params {
                instance_id,
                token,
                req_id,
                NULL,
                ril_errno,
                data,
                datalen,
                "<|||||>",
                0,
                0 }
        {}
        legacy_response_payload(const qcril_request_resp_params_type &params):
            params(params)
        {
            copy_data_from(params.resp_pkt, params.resp_len);
        }

        qcril_request_resp_params_type &get_params() {
            return params;
        }
        ~legacy_response_payload() {
            free(params.resp_pkt);
            params.resp_pkt = NULL;
            params.logstr = NULL;
        }

};

struct legacy_unsol_payload
{
};

class RilRequestMessage: public SolicitedMessage<generic_callback_payload>
{
    legacy_request_payload params;
    public:

        RilRequestMessage(message_id_t msg_id, const qcril_request_params_type &params):
            SolicitedMessage<generic_callback_payload>(msg_id),
            params(params)
        {
        }

        RilRequestMessage( message_id_t msg_id, qcril_request_params_type &&params):
            SolicitedMessage<generic_callback_payload>(msg_id),
            params(params)
        {
        }

        std::shared_ptr<generic_callback_payload> duplicateResponseData(
            std::shared_ptr<generic_callback_payload>) override;
        bool isSameMessage(std::shared_ptr<Message> shared_msg) override;

        string dump() override;
        ~RilRequestMessage() {
        }
        qcril_request_params_type &get_params() {
            return params.get_params();
        }
};

inline std::shared_ptr<generic_callback_payload> RilRequestMessage::duplicateResponseData(
    std::shared_ptr<generic_callback_payload> respData)
{
    std::shared_ptr<legacy_response_payload> legacyResp =
            std::static_pointer_cast<legacy_response_payload>(respData);
    if (!legacyResp) return nullptr;

    /* get the context in order to get the token */
    qcril_request_params_type& reqParam = get_params();
    qcril_event_context *ctx = static_cast<qcril_event_context *>(reqParam.t);
    if (!ctx) return nullptr;

    /* copy and replace the token */
    auto res = std::make_shared<legacy_response_payload>(legacyResp->get_params());
    if (!res) return nullptr;
    res->get_params().t = ctx;

    return res;
}

inline bool RilRequestMessage::isSameMessage(std::shared_ptr<Message> shared_msg)
{
    if (!shared_msg) return false;
    if (get_message_id() != shared_msg->get_message_id()) return false;

    /* type cast when message id matches */
    std::shared_ptr<RilRequestMessage> ril_msg =
            std::static_pointer_cast<RilRequestMessage>(shared_msg);
    qcril_request_params_type& src = get_params();
    qcril_request_params_type& dst = ril_msg->get_params();

    if (src.event_id_android != dst.event_id_android) return false;
    if (!src.data && !dst.data) return true;
    if (!src.data || !dst.data) return false;
    if (src.datalen != dst.datalen) return false;
    return (!memcmp(src.data, dst.data, src.datalen));
}

inline std::string RilRequestMessage::dump()
{
    return std::string("RilRequest(") + get_message_name() + ")";
}

