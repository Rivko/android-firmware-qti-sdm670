#pragma once
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#ifndef RIL_MESSAGE_FACTORY_H
#define RIL_MESSAGE_FACTORY_H

#include <memory>
#include <functional>
#include <telephony/ril.h>
#include <framework/Log.h>
#include <framework/Message.h>
#include <framework/SolicitedMessage.h>
#include <framework/legacy.h>
#include <modules/android/qcril_android_event_context.h>
#include <modules/android/RilRequestMessage.h>
#include <modules/android/ril_request_info.h>
#include <modules/android/ril_api.h>
#include "qcrili.h"

#ifndef TAG
#define TAG "RILQ-msg-factory"
#define RILQ_MSG_FACTORY_DEFINED_LOCAL
#endif

extern void android_module_on_request_complete(qcril_instance_id_e_type instance_id, RIL_Token t, RIL_Errno e, void *response, size_t responselen);
extern void android_module_on_request_complete(std::shared_ptr<ril_response_type> rsp);
extern void android_module_on_request_ack(RIL_Token t);

using
ril_factory_method = std::function<std::shared_ptr<SolicitedMessage<generic_callback_payload>>(qcril_instance_id_e_type instance_id, const void *payload, size_t, RIL_Token)>;

#include <modules/android/AndroidRequestRegisterMessage.h>


template <typename P>
using request_conv_fn_t = std::function<std::shared_ptr<Message>(int, // Request ID
                                                   P *, // Payload
                                                   size_t, // Payload length
                                                   RIL_Token // Token ID
                                                  )>;

template <typename RP, typename CP>
using response_conv_fn_t = std::function<RP *(std::shared_ptr<Message>, // The message that is being responded
                                            CP & // The result provided by the callback
                                           )>;

using android_ril_translator = message_translator_base<ril_request_type, ril_response_type, ril_unsol_type>;

template <int REQ_ID>
class  android_converting_callback: public GenericCallback<generic_callback_payload>
{
    private:
        const ril_request_info<REQ_ID> &req_info;
    public:
        android_converting_callback(const android_converting_callback &cb):
            GenericCallback<generic_callback_payload>(cb.mClientToken),
            req_info(cb.req_info)
        {
        }
        android_converting_callback(const ril_request_info<REQ_ID> &req_info):
            GenericCallback<generic_callback_payload>(""),
            req_info(req_info) {}

        Message::Callback *clone() {
            android_converting_callback *clone = new android_converting_callback<REQ_ID>(req_info);
            return clone;
        }

        void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                std::shared_ptr<generic_callback_payload> responseDataPtr) {

            std::shared_ptr<SolicitedMessage<generic_callback_payload>> msg =
                std::static_pointer_cast<SolicitedMessage<generic_callback_payload>>(solicitedMsg);

            std::shared_ptr<ril_response_type> rsp = req_info.translator.translate_response(msg, status, responseDataPtr);
            android_module_on_request_complete(rsp);
        }
};

template <int REQ_ID>
class  android_non_converting_callback: public GenericCallback<generic_callback_payload>
{
    private:
        const ril_request_info<REQ_ID> &req_info;
    public:
        android_non_converting_callback(const android_non_converting_callback &cb):
            GenericCallback<generic_callback_payload>(cb.mClientToken),
            req_info(cb.req_info)
        {
        }
        android_non_converting_callback(const ril_request_info<REQ_ID> &req_info):
            GenericCallback<generic_callback_payload>(""),
            req_info(req_info) {}

        Message::Callback *clone() {
            android_non_converting_callback *clone = new android_non_converting_callback<REQ_ID>(req_info);
            return clone;
        }

        void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                std::shared_ptr<generic_callback_payload> responseDataPtr) {

            std::shared_ptr<RilRequestMessage> msg =
                std::static_pointer_cast<RilRequestMessage>(solicitedMsg);

            QCRIL_LOG_DEBUG("Sending Response to %s", req_info.name);
            std::shared_ptr<legacy_response_payload> rsp = std::static_pointer_cast<legacy_response_payload>(responseDataPtr);
            if (status == Status::SUCCESS) {
                if (rsp)
                {
                    qcril_request_resp_params_type &params = rsp->get_params();
                    android_module_on_request_complete(params.instance_id,
                            params.t,
                            params.ril_err_no,
                            params.resp_pkt,
                            params.resp_len
                            );
                }
            } else {
                RIL_Errno ril_err = RIL_E_INTERNAL_ERR;
                if (status == Status::CANCELLED) {
                  ril_err = RIL_E_CANCELLED;
                }
                qcril_request_params_type &params = msg->get_params();
                android_module_on_request_complete(params.instance_id,
                        params.t,
                        ril_err,
                        NULL,
                        0);
            }
        }
};

template <int REQ_ID, typename req_type, typename rsp_type>
class declare_ril_request
{
    private:
        using payload_type = const req_type;
        using response_type = const rsp_type;
        static const char *const name;
        message_id_t msg_id;
        int req_id;

    public:
        declare_ril_request(string name):
            msg_id(REG_MSG(name)),
            req_id(REQ_ID) {}

};

template <int REQ_ID, typename CP>
class register_ril_request
{
    public:
        register_ril_request(struct ril_request_info<REQ_ID>) {
        }
};

#define QCRIL_MAKE_ANDROID_REQ_MSG_NAME_EX(request_name) QCRIL_MAKE_ANDROID_REQ_MSG_NAME(#request_name)

#define QCRIL_MAKE_ANDROID_REQ_MSG_NAME(request_name) "com.qualcomm.qti.qcril.android." request_name

#define QCRIL_ANDROID_DECLARE_LEGACY_EVENT(legacy_event_id) \
        qcril_evt_e_type legacy_event_req_id = legacy_event_id; \

#define QCRIL_ANDROID_DECLARE_REQUEST_BASE(request_id, ril_req_info_type, request_id_string, legacy_event_id, req_type, rsp_type, req_inf_extn, other_extn) \
    template <> \
    struct ril_request_info<request_id> { \
        using payload_type = const req_type; \
        using response_type = const rsp_type; \
        static constexpr const char * const name = QCRIL_MAKE_ANDROID_REQ_MSG_NAME(request_id_string); \
        int req_id = request_id; \
        QCRIL_ANDROID_DECLARE_LEGACY_EVENT(legacy_event_id) \
        req_inf_extn \
    }; \
    constexpr const char * const ril_request_info<request_id>::name; \
    using ril_req_info_type = struct qcril_tag_struct(req_type) ril_request_info<request_id>; \
    QCRIL_ANDROID_DECLARE_GET_MESSAGE(request_id) \
    other_extn \


#define QCRIL_ANDROID_DEFINE_REQUEST_BASE(request_id, ril_req_info_type, ril_req_info_name, get_msg_impl_body) \
    static const ril_req_info_type  ril_req_info_name{}; \
    template <> \
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> ril_message_factory::getMessage<request_id>( qcril_instance_id_e_type instance_id, \
            const void *payload, size_t size, RIL_Token t) \
    { \
        get_msg_impl_body \
    } \


#define QCRIL_ANDROID_DECLARE_GET_MESSAGE(request_id) \
    template <> \
    std::shared_ptr<SolicitedMessage<generic_callback_payload>> ril_message_factory::getMessage<request_id>( qcril_instance_id_e_type instance_id, \
            const void *payload, size_t size, RIL_Token t); \

#define QCRIL_ANDROID_DECLARE_EXTERN_LEGACY_EVT(legacy_evt_name) \
    extern qcril_evt_e_type legacy_evt_name; \



#define QCRIL_ANDROID_DECLARE_REQINFO_EXTENSION(request_id_string, legacy_event_id) \
        message_id_t msg_id = REG_MSG(QCRIL_MAKE_ANDROID_REQ_MSG_NAME(request_id_string)); \

#define QCRIL_ANDROID_DECLARE_REQUEST(request_id, req_type, rsp_type) \
    QCRIL_ANDROID_DECLARE_REQUEST_BASE(request_id, \
            ril_request_info_##request_id##_t, #request_id, QCRIL_EVT_##request_id, req_type, rsp_type, \
            message_id_t msg_id = REG_MSG(QCRIL_MAKE_ANDROID_REQ_MSG_NAME(#request_id));, \
            QCRIL_ANDROID_DECLARE_EXTERN_LEGACY_EVT(QCRIL_EVT_##request_id)) \


#define QCRIL_ANDROID_DEFINE_REQUEST_IMPL_BODY(request_id, ril_req_info_name) \
        return getMessage<request_id>( ril_req_info_name, instance_id, payload, size, std::is_same<ril_request_info<request_id>::payload_type, const char *>::value, t); \

/**
 * QCRIL_ANDROID_DEFINE_REQUEST
 * Use this macro to define a request factory that has been declared with QCRIL_ANDROID_DECLARE_REQUEST()
 * Please note that these two macros don't perform deep copy of the payload received from RIL. As such,
 * they can only be used with payload that doesn't contain pointers.
 */
#define QCRIL_ANDROID_DEFINE_REQUEST(request_id) \
    QCRIL_ANDROID_DEFINE_REQUEST_BASE(request_id, ril_request_info_##request_id##_t, ril_request_info_##request_id, \
            QCRIL_ANDROID_DEFINE_REQUEST_IMPL_BODY(request_id, ril_request_info_##request_id)) \


#define QCRIL_ANDROID_GET_MSG_ID(request_id) (ril_request_info_##request_id.msg_id)

#define QCRIL_ANDROID_REGISTER_REQUEST(request_id, callback) (({ \
    using namespace std::placeholders; \
    std::shared_ptr<AndroidRequestRegisterMessage> msg = \
            std::make_shared<AndroidRequestRegisterMessage>( \
                    request_id, \
                    std::bind<std::shared_ptr<SolicitedMessage<generic_callback_payload>>(qcril_instance_id_e_type, const void *, size_t, RIL_Token)>(ril_message_factory::getMessage<request_id>, _1, _2, _3, _4)); \
    if(msg) {\
      msg->dispatch(); \
    }\
    }))


#define QCRIL_ANDROID_REGISTER_REQUEST_WITH_RESTRICTION(request_id, callback, restrictionObj) (({ \
    using namespace std::placeholders; \
    std::shared_ptr<AndroidRequestRegisterMessage> msg = \
            std::make_shared<AndroidRequestRegisterMessage>( \
                    request_id, \
                    std::bind<std::shared_ptr<SolicitedMessage<generic_callback_payload>>(qcril_instance_id_e_type, const void *, size_t, RIL_Token)>(ril_message_factory::getMessage<request_id>, _1, _2, _3, _4)); \
    if(msg) {\
      msg->dispatch(); \
    }\
    std::vector<std::string> restMsg{QCRIL_MAKE_ANDROID_REQ_MSG_NAME(#request_id)}; \
    PolicyManager::getInstance().setMessageRestriction(restMsg, restrictionObj); \
    }))



#define QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV(request_id, req_type, rsp_type, the_translator) \
    QCRIL_ANDROID_DECLARE_REQUEST_BASE(request_id, \
            ril_request_info_##request_id##_t, #request_id, QCRIL_EVT_##request_id, req_type, rsp_type, \
            android_ril_translator &translator = the_translator;, \
            QCRIL_ANDROID_DECLARE_EXTERN_LEGACY_EVT(QCRIL_EVT_##request_id) \
            ) \

#define QCRIL_ANDROID_DEFINE_REQUEST_IMPL_BODY_WITH_CONV(request_id, ril_req_info_name) \
        ril_request_type rdata = {instance_id, request_id, ril_req_info_name.legacy_event_req_id, payload, size, t};\
        android_converting_callback<request_id> cb(ril_req_info_name); \
        auto msg = ril_req_info_name.translator.translate_request(rdata); \
        if(msg) {\
          msg->setCallback(&cb); \
        }\
        return msg; \

#define QCRIL_ANDROID_DEFINE_REQUEST_WITH_CONV(request_id) \
    QCRIL_ANDROID_DEFINE_REQUEST_BASE(request_id, ril_request_info_##request_id##_t, ril_request_info_##request_id, \
            QCRIL_ANDROID_DEFINE_REQUEST_IMPL_BODY_WITH_CONV(request_id, ril_request_info_##request_id)) \

#define QCRIL_ANDROID_DECLARE_REQUEST_WITH_CONV_NEW(request_id, req_type, rsp_type, the_translator) \
    QCRIL_ANDROID_DECLARE_REQUEST_BASE(request_id, \
            ril_request_info_##request_id##_t, #request_id, nullptr, req_type, rsp_type, \
            android_ril_translator &translator = the_translator;,) \

#define QCRIL_ANDROID_SET_MESSAGE_RESTRICTION(message_name, restrictionObj) (({ \
   std::vector<std::string> restMsg{message_name}; \
   PolicyManager::getInstance().setMessageRestriction(restMsg, restrictionObj); \
   }))

class ril_message_factory
{
    public:

        template <int REQ_ID>
        static std::shared_ptr<SolicitedMessage<generic_callback_payload>> getMessage(
                const ril_request_info<REQ_ID> &req_info,
                qcril_instance_id_e_type instance_id,
                const void *payload,
                size_t size,
                bool is_char_star,
                RIL_Token t)
                    qcril_tag_func(3,1)
        {
            std::shared_ptr<RilRequestMessage> request;
            QCRIL_NOTUSED(req_info);

            do
            {
                qcril_request_params_type params;

                params.instance_id = instance_id;
                params.event_id = req_info.legacy_event_req_id;
                params.event_id_android = REQ_ID;
                params.t = t;
                params.datalen = 0;
                params.data = nullptr;

                if (!is_char_star) {
                    if (payload) {
                        params.datalen = size;
                        params.data = calloc(1, params.datalen);
                        if (!params.data) {
                            break;
                        }
                        memcpy(params.data, payload, size);
                    }
                } else {
                    if (payload) {
                        params.datalen = strlen(static_cast<const char *>(payload)) + 1;
                        params.data = calloc(1, params.datalen);
                        if (!params.data) {
                            break;
                        }
                        strlcpy(static_cast<char *>(params.data), static_cast<const char *>(payload), params.datalen);
                    }
                }

                request = std::make_shared<RilRequestMessage>(
                        req_info.msg_id,
                        params);

                if (request) {
                    android_non_converting_callback<REQ_ID> callback(req_info);
                    request->setCallback(&callback);
                }

            }while(false);

            return std::static_pointer_cast<SolicitedMessage<generic_callback_payload>>(request);
        }

        template <int REQ_ID>
        static std::shared_ptr<SolicitedMessage<generic_callback_payload>> getMessage( qcril_instance_id_e_type instance_id,
                const void *payload, size_t size, RIL_Token t);

};

typedef std::shared_ptr<SolicitedMessage<generic_callback_payload>> (*android_request_factory_method_t)(
        qcril_instance_id_e_type instance_id,
        const void *payload,
        size_t size, RIL_Token t);


//static register_ril_message_factory<ril_request_info<RIL_REQUEST_DIAL>> reg_1(ril_request_info_RIL_REQUEST_DIAL);


#ifdef RILQ_MSG_FACTORY_DEFINED_LOCAL
#undef TAG
#endif
#endif // RIL_MESSAGE_FACTORY_H
