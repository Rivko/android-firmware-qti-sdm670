/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <framework/ModuleLooper.h>
#include <framework/QcrilInitMessage.h>
#include <framework/Log.h>

#include "modules/lpa/LpaInitMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/UimHttpModemEndPoint.h"
#include "modules/qmi/QmiUimHttpSetupRequestCallback.h"
#include "modules/lpa/LpaUimHttpRequestMsg.h"
#include "modules/lpa/LpaQmiUimHttpIndicationMsg.h"
#include "modules/lpa/lpa_service_types.h"
#include "modules/uim/UimLpaReqMessage.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "LpaQmiUimHttpRequestCallback.h"
#include "LpaUimReqMessageCallback.h"
#include "LpaModule.h"

#ifdef QMI_RIL_UTF
#include "modules/qmi/UimHttpModemEndPointModule.h"
#endif

#define TAG "LPA_Module"

using namespace vendor::qti::hardware::radio::lpa::V1_0::implementation;

static load_module<LpaModule> lpa_module;

LpaModule::LpaModule()
{
  mName  = "LpaModule";
  mReady = false;
  mRestart = false;
  uimLpa = nullptr;
  //mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
      HANDLER(QcrilInitMessage, LpaModule::handleQcrilInitMessage),
      HANDLER(LpaQmiUimHttpResponseMsg, LpaModule::handleQmiUimHttpResponse),
      HANDLER(LpaQmiUimHttpIndicationMsg, LpaModule::handleQmiUimHttpIndication),
      HANDLER(UimLpaRespMessage, LpaModule::handleUimLpaResponseMessage),
      HANDLER(UimLpaIndicationMsg, LpaModule::handleUimLpaIndicationMessage)
  };
} /* LpaModule::LpaModule */


LpaModule::~LpaModule()
{
  mReady = false;
  mRestart = false;
} /* LpaModule::~LpaModule */


void LpaModule::init()
{
  Module::init();

  // Initializations complete.
  mReady = false;
  mRestart = false;

  ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint();
} /* LpaModule::init */


bool LpaModule::isReady()
{
  return mReady;
} /* LpaModule::isReady */


void LpaModule::broadcastReady()
{
  std::shared_ptr<LpaInitMessage> lpa_init_msg = std::make_shared<LpaInitMessage>();
  Dispatcher::getInstance().broadcast(lpa_init_msg);
} /* LpaModule::broadcastReady */


void LpaModule::handleQcrilInitMessage
(
  std::shared_ptr<QcrilInitMessage> msg
)
{
  android::status_t              ret         = android::OK;
  int                            instanceId  = qmi_ril_get_process_instance_id();
  std::string                    serviceName = "UimLpa";
  QmiUimHttpSetupRequestCallback qmiSetupCallback("Uim-Module-Token");

  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  uimLpa = new UimLpaImpl(this);

  if (uimLpa != nullptr)
  {
    uimLpa->setInstanceId(instanceId);
    ret = uimLpa->registerAsService(serviceName + std::to_string(instanceId));
  }
  QCRIL_LOG_INFO("qcril_uim_lpa_register_service instanceId=%d status=%d", instanceId, ret);

  ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint()->requestSetup("Client-Uim-http-Serv-Token", &qmiSetupCallback);

  mReady = true;
  broadcastReady();
} /* LpaModule::handleQcrilInitMessage */


void LpaModule::handleUimLpaUserRequest
(
  int token,
  lpa_service_user_req_type *user_req_ptr
)
{
  std::shared_ptr<UimLpaReqMessage>   lpa_req_ptr      = nullptr;
  LpaUimReqMessageCallback            lpaUimReqcb("UIM LPA REQUEST");
  lpa_service_user_resp_type          resp_data;

  memset(&resp_data, 0x00, sizeof(resp_data));
  resp_data.result = LPA_SERVICE_RESULT_FAILURE;

  if (user_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid null data");
    if (uimLpa != nullptr)
    {
      uimLpa->uimLpaUserResponse(token, &resp_data);
    }
    return;
  }

  QCRIL_LOG_INFO("UimLpaUserRequest: token_id: 0x%x", token);

  lpa_req_ptr = std::make_shared<UimLpaReqMessage>(UIM_LPA_USER_REQUEST_ID,
                                                   &lpaUimReqcb,
                                                   user_req_ptr,
                                                   token);

  if (lpa_req_ptr != nullptr)
  {
    lpa_req_ptr->dispatch();
  }
  else
  {
    if (uimLpa != nullptr)
    {
      resp_data.event = user_req_ptr->event;
      uimLpa->uimLpaUserResponse(token, &resp_data);
    }
  }
} /* LpaModule::handleUimLpaUserRequest */


void LpaModule::handleUimLpaHttpTxnCompletedRequest
(
  int token,
  lpa_service_http_transaction_req_type * http_req_ptr
)
{
  std::shared_ptr<LpaUimHttpRequestMsg>   lpa_req_ptr      = nullptr;
  LpaQmiUimHttpRequestCallback            lpaHttpReqcb("QMI UIM HTTP REQUEST");

  if (http_req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid null data");
    if (uimLpa != nullptr)
    {
      uimLpa->uimLpaHttpTxnCompletedResponse(token, LPA_SERVICE_RESULT_FAILURE);
    }
    return;
  }

  QCRIL_LOG_INFO("handleUimLpaHttpTxnCompletedRequest: token_id: 0x%x", token);

  lpa_req_ptr = std::make_shared<LpaUimHttpRequestMsg>(http_req_ptr,
                                                       token,
                                                       LPA_QMI_UIM_HTTP_REQUEST_HTTP_TXN_COMPLETED,
                                                       &lpaHttpReqcb);

  if (lpa_req_ptr != nullptr)
  {
    lpa_req_ptr->dispatch();
  }
  else
  {
    if (uimLpa != nullptr)
    {
      uimLpa->uimLpaHttpTxnCompletedResponse(token, LPA_SERVICE_RESULT_FAILURE);
    }
  }
} /* LpaModule::handleUimLpaHttpTxnCompletedRequest */


void LpaModule::handleQmiUimHttpResponse
(
  std::shared_ptr<LpaQmiUimHttpResponseMsg> msg_ptr
)
{
  lpa_qmi_uim_http_rsp_data_type * rsp_data_ptr = nullptr;
  lpa_service_result_type          http_result  = LPA_SERVICE_RESULT_SUCCESS;

  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  rsp_data_ptr = msg_ptr->get_message();
  if (rsp_data_ptr == nullptr || rsp_data_ptr->rsp_id == LPA_QMI_UIM_HTTP_INVALID_RSP)
  {
    QCRIL_LOG_ERROR("Invalid null response data");
    return;
  }

  if (rsp_data_ptr->qmi_error_code != 0 ||
      rsp_data_ptr->transp_err != 0)
  {
    http_result = LPA_SERVICE_RESULT_FAILURE;
  }
  if (uimLpa != nullptr)
  {
    uimLpa->uimLpaHttpTxnCompletedResponse(rsp_data_ptr->token, http_result);
  }
} /* LpaModule::handleQmiUimHttpResponse */


void LpaModule::handleQmiUimHttpIndication
(
  std::shared_ptr<LpaQmiUimHttpIndicationMsg> msg_ptr
)
{
  if (msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid null msg_ptr");
    return;
  }

  QCRIL_LOG_INFO("QMI UIM HTTP Indication : %d", msg_ptr->get_ind_id());

  switch (msg_ptr->get_ind_id())
  {
    case QMI_UIM_HTTP_SRVC_UP_IND_MSG:
      break;
    case QMI_UIM_HTTP_SRVC_DOWN_IND_MSG:
      break;
    case QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG:
      {
        uim_http_transaction_ind_msg          * ind_ptr     = (uim_http_transaction_ind_msg *)msg_ptr->get_message();
        lpa_service_http_transaction_ind_type * svc_ind_ptr = nullptr;

        if (ind_ptr == nullptr)
        {
          QCRIL_LOG_ERROR("Invalid null ind_ptr");
          break;
        }

        svc_ind_ptr = new lpa_service_http_transaction_ind_type;

        if (svc_ind_ptr == nullptr)
        {
          QCRIL_LOG_ERROR("Invalid memory failure");
          break;
        }
        memset(svc_ind_ptr, 0x00, sizeof(lpa_service_http_transaction_ind_type));

        svc_ind_ptr->tokenId = ind_ptr->token_id;
        svc_ind_ptr->payload_len = ind_ptr->payload_len;
        svc_ind_ptr->payload = ind_ptr->payload_ptr;
        if (ind_ptr->url_valid)
        {
          svc_ind_ptr->url = ind_ptr->url;
        }
        if (ind_ptr->headers_valid)
        {
          uint32_t i = 0;

          svc_ind_ptr->contentType = ind_ptr->headers.content_type;
          svc_ind_ptr->no_of_headers = (ind_ptr->headers.custom_header_len < UIM_HTTP_CUST_HEADER_MAX_COUNT) ?
                                         ind_ptr->headers.custom_header_len : UIM_HTTP_CUST_HEADER_MAX_COUNT;

          svc_ind_ptr->customHeaders = new lpa_service_http_custom_header_type[svc_ind_ptr->no_of_headers];

          if (svc_ind_ptr->customHeaders == nullptr)
          {
            svc_ind_ptr->no_of_headers = 0;
          }
          else
          {
            for (i = 0; i < svc_ind_ptr->no_of_headers; i++)
            {
              svc_ind_ptr->customHeaders[i].headerName = ind_ptr->headers.custom_header[i].name;
              svc_ind_ptr->customHeaders[i].headerValue = ind_ptr->headers.custom_header[i].value;
            }
          }
        }

        if (uimLpa != nullptr)
        {
          uimLpa->uimLpaHttpTxnIndication(svc_ind_ptr);
        }

        if (svc_ind_ptr->customHeaders != nullptr)
        {
          delete[] svc_ind_ptr->customHeaders;
          svc_ind_ptr->customHeaders = nullptr;
        }
      }
      break;
    default:
      break;
  }
} /* LpaModule::handleQmiUimHttpIndication */


void LpaModule::handleUimLpaResponseMessage
(
  std::shared_ptr<UimLpaRespMessage> msg_ptr
)
{
  if (msg_ptr == nullptr)
  {
    return;
  }

  switch(msg_ptr->get_response_id())
  {
    case UIM_LPA_USER_RESP_ID:
      {
        lpa_service_user_resp_type    resp_data;

        memset(&resp_data, 0x00, sizeof(lpa_service_http_transaction_ind_type));
        if (msg_ptr->get_message_ptr() != nullptr)
        {
          qcril_uim_lpa_user_resp_type *resp_ptr =
            (qcril_uim_lpa_user_resp_type *)msg_ptr->get_message_ptr();

          resp_data.event = (lpa_service_user_event_type)resp_ptr->event;
          resp_data.result = (lpa_service_result_type)resp_ptr->result;
          resp_data.eid = resp_ptr->eid;
          resp_data.eid_len = resp_ptr->eid_len;
          resp_data.profiles = (lpa_service_profile_info_type *)resp_ptr->profiles;
          resp_data.no_of_profiles = resp_ptr->no_of_profiles;
          resp_data.srvAddr.smdpAddress = resp_ptr->srvAddr.smdpAddress;
          resp_data.srvAddr.smdsAddress = resp_ptr->srvAddr.smdsAddress;
        }
        else
        {
          resp_data.result = LPA_SERVICE_RESULT_FAILURE;
        }

        if (uimLpa != nullptr)
        {
          uimLpa->uimLpaUserResponse(msg_ptr->get_token(), &resp_data);
        }
      }
      break;
    default:
      break;
  }
} /* LpaModule::handleUimLpaResponseMessage */


void LpaModule::handleUimLpaIndicationMessage
(
  std::shared_ptr<UimLpaIndicationMsg> msg_ptr
)
{
  if (msg_ptr == nullptr)
  {
    return;
  }

  switch(msg_ptr->get_ind_id())
  {
    case UIM_LPA_PROFILE_PROGRESS_IND_ID:
      if (msg_ptr->get_data_ptr() != nullptr)
      {
        qcril_uim_lpa_add_profile_progress_ind_type *ind_data_ptr =
           (qcril_uim_lpa_add_profile_progress_ind_type *) msg_ptr->get_data_ptr();
        lpa_service_add_profile_progress_ind_type  lpa_ind;

        memset(&lpa_ind, 0x00, sizeof(lpa_ind));
        lpa_ind.status = (lpa_service_add_profile_status_type)ind_data_ptr->status;
        lpa_ind.cause  = (lpa_service_add_profile_failure_cause)ind_data_ptr->cause;
        lpa_ind.progress = ind_data_ptr->progress;
        lpa_ind.policyMask = (lpa_service_profile_policy_mask)ind_data_ptr->policyMask;
        lpa_ind.userConsentRequired = ind_data_ptr->userConsentRequired;

        if (uimLpa != nullptr)
        {
          uimLpa->uimLpaAddProfileProgressInd(&lpa_ind);
        }
      }
      break;
    default:
      break;
  }
} /* LpaModule::handleUimLpaIndicationMessage */

LpaModule *getLpaModule() {
    return &(lpa_module.get_module());
}

#ifdef QMI_RIL_UTF
void LpaModule::qcrilHalLpaModuleCleanup()
{
    std::shared_ptr<UimHttpModemEndPoint> mUimHttpModemEndPoint =
                    ModemEndPointFactory<UimHttpModemEndPoint>::getInstance().buildEndPoint();
    UimHttpModemEndPointModule* mUimHttpModemEndPointModule =
                   (UimHttpModemEndPointModule*)mUimHttpModemEndPoint->mModule;
    mUimHttpModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
    mRestart = false;
}
#endif
