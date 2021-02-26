/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <functional>
#include <memory>
#include <utility>

#include <cutils/properties.h>

#include <framework/Module.h>
#include <modules/android/AndroidRequestRegisterMessage.h>
#include <framework/Resource.h>
#include <framework/legacy.h>
#include <framework/Dispatcher.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Log.h>
#include "android_ril_module.h"
#include <modules/android/version.h>
#include <modules/android/ril_api.h>
#include <modules/android/qcril_android_event_context.h>

#define TAG "RILQ_android_ril"

void android_module_on_request_complete(RIL_Token t, RIL_Errno e, void *response, size_t responselen);
void android_module_on_request_complete(std::shared_ptr<ril_response_type> rsp);
void android_module_on_request_ack(RIL_Token t);

RIL_Env android_ril_module::local_env;

extern "C" const RIL_RadioFunctions *legacy_RIL_Init
(
  const struct RIL_Env *env,
  qcril_instance_id_e_type instance_id,
  int argc,
  char **argv
);

static load_module<android_ril_module> the_module;

struct factory_info_t
{
    ril_factory_method getMessage;
    unsigned request_id;
};

android_ril_module &get_android_ril_module()
{
    return the_module.get_module();
}

#define QCRIL_LOG_ADB_ON  "persist.vendor.radio.adb_log_on"

bool is_adb_logging_enabled () {
  bool adb_log_on = false;
  char args[PROPERTY_VALUE_MAX];

  memset(args, 0, sizeof(args));

  property_get(QCRIL_LOG_ADB_ON, args, "0");

  if (!strcmp(args, "1")) {
    adb_log_on = true;
  }

  return adb_log_on;
}

android_ril_module::android_ril_module()
{
    mName = "AndroidRilModule";

#ifndef QMI_RIL_UTF
    if (is_adb_logging_enabled()) {
      Logger::setDefault("android");
    } else {
      Logger::setDefault("diag") || Logger::setDefault("android");
    }
#endif
    using std::placeholders::_1;
    mMessageHandler = {{AndroidRequestRegisterMessage::get_class_message_id(),
                        std::bind(&android_ril_module::handleRegisterRequest, this, _1)}};
}

bool android_ril_module::dispatch_request(qcril_instance_id_e_type instance_id, int req_id, void* data, size_t size, RIL_Token t)
{
    QCRIL_LOG_FUNC_ENTRY();
    bool ret = false;
    std::shared_ptr<factory_info_t> finfo;
    qcril_event_context *ctx = new qcril_event_context(instance_id, req_id, t);

    if(!is_dispatch_allowed(instance_id, req_id))
    {
        QCRIL_LOG_DEBUG("Cannot dispatch request: %d as radio is unavailable", req_id);
        send_response(instance_id,
                      static_cast<RIL_Token>(ctx),
                      RIL_E_RADIO_NOT_AVAILABLE,
                      nullptr, 0);
    }
    else
    {
        QCRIL_LOG_DEBUG("Looking for factory for request %d", req_id);
        auto it = factories.find(req_id);
        if (it != factories.end())
        {
            QCRIL_LOG_DEBUG("Entry found. factory: %p", it->second.get());
            finfo = it->second;
        }
        if (finfo)
        {
            QCRIL_LOG_DEBUG("Found a factory. Attempting to obtain message");
            std::shared_ptr<SolicitedMessage<generic_callback_payload>>msg =
                        finfo->getMessage(instance_id, data, size, static_cast<RIL_Token>(ctx));
            ctx->msg = msg;
            if (msg)
            {
                QCRIL_LOG_DEBUG("Message obtained. Dispatching");
                msg->dispatch();
                ret = true;
            }
            else
            {
                QCRIL_LOG_ERROR("Message not obtained using the corresponding ctx");
                delete ctx;
                ret = false;
            }
        }
        else
        {
            QCRIL_LOG_DEBUG("Unable to find factory. Attempting legacy");
            if (qcril_legacy_functions[instance_id] && qcril_legacy_functions[instance_id]->onRequest)
            {
                QCRIL_LOG_DEBUG("Dispatching legacy");
                qcril_legacy_functions[instance_id]->onRequest(req_id, data, size, ctx);
                ret = true;
            }
            else
            {
                QCRIL_LOG_ERROR("Qcril legacy onRequest() is not available/qcril_legacy_functions is null for instance_id:%d",
                        instance_id);
                delete ctx;
                ret = false;
            }

        }
    }

    QCRIL_LOG_FUNC_RETURN("%s", ret?"true":"false");
    return ret;
}

void android_ril_module::handleRegisterRequest(std::shared_ptr< Message > msg)
{
    std::shared_ptr<factory_info_t> f = std::make_shared<factory_info_t>();
    std::shared_ptr<AndroidRequestRegisterMessage> amsg = std::static_pointer_cast<AndroidRequestRegisterMessage>(msg);
    if(f != NULL && amsg != NULL)
    {
      f->request_id = amsg->request_id;
      f->getMessage = amsg->factory;
      factories[f->request_id] = f;

      QCRIL_LOG_DEBUG("registered factory %p for android request id %d", &f->getMessage, f->request_id);

      std::shared_ptr<bool> shared_response = std::make_shared<bool>(true);
      amsg->sendResponse(amsg, Message::Callback::Status::SUCCESS, shared_response);
    }
    else if(amsg)
    {
      std::shared_ptr<bool> shared_response = std::make_shared<bool>(false);
      amsg->sendResponse(amsg, Message::Callback::Status::FAILURE, shared_response);
    }
}

int android_ril_module::is_request_supported(qcril_instance_id_e_type instance_id, int req_id)
{
    int ret = 0;
    (void) instance_id;
    auto it = factories[req_id];
    if (it)
    {
    }
    return ret;
}

bool android_ril_module::is_dispatch_allowed(qcril_instance_id_e_type instance_id, int req_id)
{
    bool allowed = true;
    static std::vector<int> alwaysDispatch = {
                       RIL_REQUEST_RADIO_POWER,
                       RIL_REQUEST_SHUTDOWN,
                       RIL_REQUEST_SCREEN_STATE,
                       RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER,
                       RIL_REQUEST_SEND_DEVICE_STATE,
                       RIL_REQUEST_SET_RADIO_CAPABILITY,
                       RIL_REQUEST_GET_SIM_STATUS,
                       RIL_REQUEST_GET_RADIO_CAPABILITY,
                       RIL_REQUEST_DIAL,
                       RIL_REQUEST_SIM_IO,
                       RIL_REQUEST_GET_IMSI,
                       RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING
                     };

    if(RADIO_STATE_UNAVAILABLE == currentState(instance_id))
    {
        allowed = false;
        if(std::find(alwaysDispatch.begin(), alwaysDispatch.end(), req_id)
                                                                != alwaysDispatch.end())
        {
            allowed = true;
        }
    }

    return allowed;
}

void android_ril_module::ril_init
(
    qcril_instance_id_e_type instance_id,
    const struct RIL_Env *env,
    int argc,
    char **argv
)
{
    QCRIL_LOG_FUNC_ENTRY();

    local_env = *env;
    local_env.OnRequestComplete = ::android_module_on_request_complete;
    local_env.OnRequestAck = ::android_module_on_request_ack;

    qcril_legacy_functions[instance_id] = legacy_RIL_Init(&local_env, instance_id, argc, argv);

    std::shared_ptr<QcrilInitMessage> qcril_init_msg = std::make_shared<QcrilInitMessage>(instance_id);
    if(qcril_init_msg) {
      qcril_init_msg->broadcast();
    }

    QCRIL_LOG_FUNC_RETURN();
}

RIL_RadioState android_ril_module::currentState
(
  qcril_instance_id_e_type instance_id
)
{
  char label[ 300 ];
  const char *radio_state_name;
  RIL_RadioState radio_state;

  /*-----------------------------------------------------------------------*/

  QCRIL_ASSERT( instance_id < QCRIL_MAX_INSTANCE_ID );

  /*-----------------------------------------------------------------------*/

  radio_state = qcril_get_current_radio_state();

  radio_state_name = qcril_log_ril_radio_state_to_str(radio_state);

  QCRIL_LOG_DEBUG( "currentState() -> %s(%d)", radio_state_name, radio_state );

  QCRIL_SNPRINTF( label, sizeof( label ), "currentState() - %s", radio_state_name );
  QCRIL_LOG_CF_PKT_RIL_FN( instance_id, label );

  return radio_state;

}

void android_ril_module::send_response(qcril_instance_id_e_type instance_id, RIL_Token token_id, RIL_Errno e, void *response, size_t response_len)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_event_context *ctx = static_cast<qcril_event_context *>(token_id);
    if (ctx)
    {
        if (!ctx->is_internal)
        {
            android_ril_on_request_complete(instance_id, ctx->t, e, response, response_len);
        }
        else
        {
            QCRIL_LOG_INFO("Attempting to send response for internal event. Ignored.");
        }
        delete ctx;
    }
    else
    {
        QCRIL_LOG_ERROR("send response with NULL context");
    }
    QCRIL_LOG_FUNC_RETURN();
}
void android_ril_module::send_response(std::shared_ptr<ril_response_type> rsp)
{
    if (rsp)
    {
        send_response(rsp->instance_id,
                rsp->t,
                rsp->e,
                rsp->response,
                rsp->response_len);
    }
    else
    {
        QCRIL_LOG_ERROR("Requested to send response but no valid ril_response_type given");
    }
}

void android_ril_module::setup_timed_callback
(
 qcril_instance_id_e_type instance_id,
 RIL_TimedCallback callback,
 void *param,
 const struct timeval *relativeTime
)
{
    android_ril_setup_timed_callback(
            instance_id,
            callback,
            param,
            relativeTime);
}

void android_ril_module::send_request_ack(RIL_Token t)
{
    QCRIL_LOG_FUNC_ENTRY();
    qcril_event_context *ctx = static_cast<qcril_event_context *>(t);
    if (ctx)
    {
        if (!ctx->is_internal)
        {
            RIL_Token original_token = ctx->t;
            android_ril_on_request_ack(qmi_ril_get_process_instance_id(), original_token);
        }
        else
        {
            QCRIL_LOG_INFO("Attempting to send ack to internal event. Ignored.");
        }
    }
    else
    {
        QCRIL_LOG_ERROR("NULL token received");
    }
    QCRIL_LOG_FUNC_RETURN();
}

void android_module_on_request_complete(qcril_instance_id_e_type instance_id, RIL_Token t, RIL_Errno e, void *response, size_t response_len)
{
    get_android_ril_module().send_response(instance_id, t, e, response, response_len);
}


void android_module_on_request_complete(RIL_Token t, RIL_Errno e, void *response, size_t response_len)
{
    get_android_ril_module().send_response(qmi_ril_get_process_instance_id(), t, e, response, response_len);
}


void android_module_on_request_complete(std::shared_ptr<ril_response_type> rsp)
{
    get_android_ril_module().send_response(rsp);
}


void android_module_on_request_ack(RIL_Token t)
{
    get_android_ril_module().send_request_ack(t);
}

boolean android_ril_try_respond
(
 qcril_request_resp_params_type *rsp
)
{
    QCRIL_LOG_FUNC_ENTRY();
    boolean ret = FALSE;
    if (rsp) {
        qcril_event_context *ctx = static_cast<qcril_event_context *>(rsp->t);
        if (ctx) {
            if (ctx->is_internal) {
                QCRIL_LOG_ERROR("Attempted sending response for internal token");
            } else {
                rsp->t = ctx;
                auto msg = ctx->msg;
                if (msg) {
                    std::shared_ptr<generic_callback_payload> response = std::make_shared<legacy_response_payload>(*rsp);
                    msg->sendResponse(msg, Message::Callback::Status::SUCCESS, response);
                    ret = TRUE;
                } else {
                    QCRIL_LOG_DEBUG("Received a null message. Not sending response");
                }
            }
        }
    }
    QCRIL_LOG_FUNC_RETURN("ret: %s", ret ? "true" : "false");
    return ret;
}

