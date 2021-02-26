/******************************************************************************
#  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <telephony/ril.h>
#include "framework/ModuleLooper.h"
#include "framework/QcrilInitMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/QmiUimSetupRequestCallback.h"
#include "modules/uim/Uimi.h"
#include "modules/uim/UimQmiUimRequestSyncMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "UimModule.h"
#include "qcril_uim_card.h"
#include "qcril_uim_file.h"
#include "qcril_uim_util.h"
#include "qcril_uim_restart.h"
#include "qcril_uim_security.h"
#include "qcril_uim_slot_mapping.h"
#ifdef QMI_RIL_UTF
#include "modules/qmi/UimModemEndPointModule.h"
#endif

#define TAG "UimModule"
#define MAX_QMI_UIM_ASYNC_MSG 10

static load_module<UimModule> uim_module;

UimModule::UimModule()
{
  mName = "UimModule";
  mReady = false;
  mRestart = false;

  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  QCRIL_LOG_INFO( "%s - Constructor\n", __FUNCTION__);

  using std::placeholders::_1;
  mMessageHandler = {
       HANDLER(QcrilInitMessage, UimModule::UimProcessRilInit),
       HANDLER(UimQmiUimResponseMsg, UimModule::UimProcessQmiCommandCallback),
       HANDLER(UimQmiUimIndicationMsg, UimModule::UimProcessQmiIndiaction),
       HANDLER(UimCardPowerReqMsg, UimModule::UimProcessCardPowerReq),
       HANDLER(UimChangeSubscriptionReqMsg, UimModule::UimProcessChangeSubscription),
       HANDLER(UimGetImsiRequestMsg, UimModule::UimProcessGetImsiReq),
       HANDLER(UimGetCardStatusRequestMsg, UimModule::UimProcessGetCardStatusReq),
       HANDLER(UimGetIccIdRequestMsg, UimModule::UimProcessGetIccIdReq),
       HANDLER(UimGetImsiRequestSyncMsg, UimModule::UimProcessGetImsiSyncReq),
       HANDLER(UimGetCardStatusRequestSyncMsg, UimModule::UimProcessGetCardStatusSyncReq),
       HANDLER(UimGetIccIdRequestSyncMsg, UimModule::UimProcessGetIccIdSycnReq),
       HANDLER(UimGetMccMncRequestMsg, UimModule::UimProcessGetMccMncReq),
       HANDLER(UimGetFacilityLockRequestMsg, UimModule::UimProcessGetFacilityLockStatusReq),
       HANDLER(UimSetFacilityLockRequestMsg, UimModule::UimProcessSetFacilityLockReq),
       HANDLER(UimRilRequestMessage, UimModule::uim_process_ril_request),
       HANDLER(UimOemHookRequestMsg, UimModule::uim_process_oem_hook_request),
       HANDLER(UimLpaReqMessage, UimModule::uim_process_lpa_request),
       HANDLER(UimGetAtrRequestMsg, UimModule::UimProcessGetAtrReq)
                    };
}

UimModule::~UimModule()
{
  QCRIL_LOG_INFO( "%s - Destructor \n", __FUNCTION__);
  mReady = false;
  mRestart = false;
  mLooper = nullptr;
  mQmiAsyncMsgCount = 0;
}


void UimModule::init()
{
  QCRIL_LOG_INFO( "%s init\n", __FUNCTION__);

  /* Call base init before doing any other stuff.*/
  Module::init();

  /* Reset qcril_uim global structure*/
  qcril_uim_reset_state();

  ModemEndPointFactory<UimModemEndPoint>::getInstance().buildEndPoint();
}


void UimModule::UimProcessRilInit(std::shared_ptr<Message> msg_ptr)
{
  (void) msg_ptr;
  /* Init QMI services.*/
  QmiUimSetupRequestCallback qmiUimSetupCallback("Uim-Module-Token");
  ModemEndPointFactory<UimModemEndPoint>::getInstance().buildEndPoint()->requestSetup("Client-Uim-Serv-Token", &qmiUimSetupCallback);
}


void UimModule::uim_process_ril_request(std::shared_ptr<UimRilRequestMessage> req_ptr)
{
  qcril_request_params_type               params;
  qcril_request_return_type               ret_params;
  std::shared_ptr<uim_ril_resp_payload>   rsp_ptr = nullptr;
  UimRequestMsgData                     * msg_data_ptr = nullptr;
  RIL_Errno                               ril_err      = RIL_E_GENERIC_FAILURE;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_RIL_EVENT, nullptr};

  QCRIL_LOG_INFO( "mReady :%d", mReady);

  if (mReady == false || msg_data_ptr == nullptr)
  {
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }

    if (mReady == false)
    {
      ril_err = RIL_E_RADIO_NOT_AVAILABLE;
    }
    rsp_ptr = std::make_shared<uim_ril_resp_payload>(ril_err, nullptr, 0);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, rsp_ptr);
    return;
  }

  /* Copy request parameters - Shallow copy, Deep copy already done in request message */
  memcpy(&params, req_ptr->get_params(), sizeof(params));

  /* Request message data to RIL_Token(void *) will be used when sending response */
  msg_data_ptr->ril_token = params.t;
  params.t  = (RIL_Token)msg_data_ptr;

  QCRIL_LOG_INFO( "Token :%d", qcril_log_get_uim_token_id(params.t));

  switch(req_ptr->get_req_id())
  {
    case RIL_REQUEST_GET_SIM_STATUS:
      qcril_uim_request_get_sim_status(&params, &ret_params);
      break;
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PIN2:
      qcril_uim_request_enter_pin(&params, &ret_params);
      break;
    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PUK2:
      qcril_uim_request_enter_puk(&params, &ret_params);
      break;
    case RIL_REQUEST_CHANGE_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
      qcril_uim_request_change_pin(&params, &ret_params);
      break;
    case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION:
      qcril_uim_request_enter_perso_key(&params, &ret_params);
      break;
    case RIL_REQUEST_GET_IMSI:
      qcril_uim_request_get_imsi(&params, &ret_params);
      break;
    case RIL_REQUEST_SIM_IO:
      qcril_uim_request_sim_io(&params, &ret_params);
      break;
    case RIL_REQUEST_ISIM_AUTHENTICATION:
      qcril_uim_request_isim_authenticate(&params, &ret_params);
      break;
    case RIL_REQUEST_SIM_OPEN_CHANNEL:
    case RIL_REQUEST_SIM_CLOSE_CHANNEL:
      qcril_uim_request_logical_channel(&params, &ret_params);
      break;
    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
      qcril_uim_request_send_apdu(&params, &ret_params);
      break;
    case RIL_REQUEST_SIM_GET_ATR:
      qcril_uim_request_get_atr(&params, &ret_params);
      break;
    case RIL_REQUEST_SIM_AUTHENTICATION:
      qcril_uim_request_sim_authenticate(&params, &ret_params);
      break;
    case RIL_REQUEST_SET_SIM_CARD_POWER:
      qcril_uim_request_uicc_power(&params, &ret_params);
      break;
    default:
      /* Send NOT SUPPORTED response */
      if (msg_data_ptr != nullptr)
      {
        delete msg_data_ptr;
        msg_data_ptr = nullptr;
      }

      rsp_ptr = std::make_shared<uim_ril_resp_payload>(RIL_E_REQUEST_NOT_SUPPORTED, nullptr, 0);
      req_ptr->sendResponse(req_ptr, Message::Callback::Status::NO_HANDLER_FOUND, rsp_ptr);
      break;
  }
}


void UimModule::uim_process_oem_hook_request(std::shared_ptr<UimOemHookRequestMsg> req_ptr)
{
  UimRequestMsgData                     * msg_data_ptr = nullptr;
  RIL_Errno                               ril_err      = RIL_E_GENERIC_FAILURE;
  qcril_request_params_type             * params_ptr   = nullptr;
  qcril_request_return_type             * ret_ptr      = nullptr;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  params_ptr = req_ptr->get_params_ptr();
  ret_ptr    = req_ptr->get_ret_ptr();

  if (params_ptr == nullptr || ret_ptr == nullptr)
  {
    return;
  }

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_OEM_HOOK_EVENT, nullptr};

  QCRIL_LOG_INFO( "mReady :%d", mReady);

  if (mReady == false || msg_data_ptr == nullptr)
  {
    std::shared_ptr<UimOemHookRspData>   rsp_ptr = nullptr;
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }

    if (mReady == false)
    {
      ril_err = RIL_E_RADIO_NOT_AVAILABLE;
    }
    rsp_ptr = std::shared_ptr<UimOemHookRspData>(new UimOemHookRspData{params_ptr->instance_id,
                                                                       params_ptr->t,
                                                                       ril_err,
                                                                       params_ptr->event_id,
                                                                       nullptr,
                                                                       0,
                                                                       (char *)"Modem not ready"});
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);
    return;
  }

  /* Request message data to RIL_Token(void *) will be used when sending response */
  msg_data_ptr->ril_token = params_ptr->t;
  params_ptr->t  = (RIL_Token)msg_data_ptr;

  switch (req_ptr->get_msg_id())
  {
    case UIM_OEM_HOOK_UICC_VOLTAGE_STATUS_REQ:
      qcril_uim_request_voltage_status(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_UICC_POWER_REQ:
      qcril_uim_request_uicc_power(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_CARD_POWER_REQ:
      qcril_uim_request_uicc_power(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_PERSONALIZATION_REACTIVATE_REQ:
      qcril_uim_request_perso_reactivate(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_PERSONALIZATION_STATUS_REQ:
      qcril_uim_request_perso_status(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_ENTER_DEPERSONALIZATION_REQ:
      qcril_uim_request_enter_perso_key(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_GET_CARD_STATE_REQ:
      qcril_uim_request_get_sim_status(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_GET_ATR_REQ:
      qcril_uim_request_get_atr(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_SEND_APDU_REQ:
      qcril_uim_request_send_apdu(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SWITCH_SLOT_REQ:
      qcril_uim_request_switch_slot(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_GET_PHYSICAL_SLOT_MAPPING_REQ:
      qcril_uim_request_get_physical_slot_mapping(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ:
      qcril_uim_request_set_apdu_behavior(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      qcril_uim_request_get_terminal_capability(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ:
      qcril_uim_request_set_terminal_capability(params_ptr, ret_ptr);
      break;
    case UIM_OEM_HOOK_GET_SLOTS_STATUS:
      qcril_uim_request_get_slots_status(params_ptr, ret_ptr);
      break;
    default:
      break;
  }
}

void UimModule::UimProcessQmiCommandCallback(std::shared_ptr<UimQmiUimResponseMsg> uim_qmi_resp_msg_ptr)
{
  qcril_request_params_type             params;
  qcril_request_return_type             ret_params;
  qcril_uim_original_request_type     * orig_ptr;

  memset(&ret_params, 0x00, sizeof(ret_params));
  memset(&params, 0x00, sizeof(params));

  if (mQmiAsyncMsgCount > 0)
  {
    mQmiAsyncMsgCount--;
  }

  /* Received one response, check the queue for pending messages */
  while (!mQmiPendingList.empty())
  {
    std::shared_ptr<int>                 respPtr    = nullptr;
    std::shared_ptr<UimQmiUimRequestMsg> pendingMsg = mQmiPendingList.front();

    mQmiPendingList.pop();
    QCRIL_LOG_INFO("Processing the message in the QMI Queue");

    if (pendingMsg != nullptr)
    {
      Message::Callback::Status status = pendingMsg->dispatchSync(respPtr);

      if (status == Message::Callback::Status::SUCCESS &&
          respPtr != nullptr && *respPtr == 0)
      {
        /* Queuing async request to modem success */
        mQmiAsyncMsgCount++;
        break;
      }
      else
      {
        /* QMI UIM request failed, send error response back */
        qmi_uim_rsp_data_type   rsp_data = {};

        rsp_data.sys_err_code = (respPtr != nullptr) ? *respPtr : QMI_UIM_SERVICE_ERR;
        rsp_data.qmi_err_code = QMI_UIM_SERVICE_ERR_INTERNAL;
        rsp_data.rsp_id = qmi_uim_convert_req_type_to_rsp_id(pendingMsg->get_msg_id());
        pendingMsg->get_cb_ptr()(&rsp_data, (void *)pendingMsg->get_orig_ptr());
      }
    }
  }

  if (uim_qmi_resp_msg_ptr != NULL)
  {
    params.instance_id = qmi_ril_get_process_instance_id();
    params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
    params.data        = uim_qmi_resp_msg_ptr->get_message((uint32_t *)&params.datalen);

    switch(uim_qmi_resp_msg_ptr->get_message_id())
    {
      case QMI_UIM_SRVC_ADD_PROFILE_RSP_MSG:
      case QMI_UIM_SRVC_SET_SIM_PROFILE_RSP_MSG:
      case QMI_UIM_SRVC_GET_SIM_PROFILES_RSP_MSG:
      case QMI_UIM_SRVC_DELETE_PROFILE_RSP_MSG:
      case QMI_UIM_SRVC_UPDATE_PROFILE_NICKNAME_RSP_MSG:
      case QMI_UIM_SRVC_EUICC_MEMORY_RESET_RSP_MSG:
      case QMI_UIM_SRVC_USER_CONSENT_RSP_MSG:
      case QMI_UIM_SRVC_GET_EID_RSP_MSG:
      case QMI_UIM_SRVC_GET_PROFILE_INFO_RSP_MSG:
      case QMI_UIM_SRVC_GET_SET_SERVER_ADDRESS_RSP_MSG:
        qcril_uim_lpa_process_qmi_callback(uim_qmi_resp_msg_ptr->get_message_id(),
                                           (qcril_uim_callback_params_type *)params.data,
                                           uim_qmi_resp_msg_ptr->get_user_data());
        break;

      default:
        if (params.data  == NULL)
        {
          orig_ptr = (qcril_uim_original_request_type *)uim_qmi_resp_msg_ptr->get_user_data();

          if (orig_ptr != nullptr)
          {
            if (orig_ptr->is_sap)
            {
              qcril_uim_sap_send_err_response((qcril_uim_sap_clients_type)orig_ptr->data.sap_info.client_id,
                                              orig_ptr->request_id,
                                              (void *)orig_ptr->token,
                                              QCRIL_UIM_RIL_E_MODEM_ERR);
            }
            else
            {
              qcril_uim_response(orig_ptr->instance_id,
                                 orig_ptr->token,
                                 QCRIL_UIM_RIL_E_MODEM_ERR,
                                 NULL,
                                 0,
                                 FALSE,
                                 NULL);
            }
          }
        }
        else
        {
          qcril_uim_process_qmi_callback(&params, &ret_params);
        }
        break;
    }
  }
}


void UimModule::UimProcessQmiIndiaction(std::shared_ptr<UimQmiUimIndicationMsg> uim_qmi_ind_msg_ptr)
{
  qcril_request_params_type               params;
  qcril_request_return_type               ret_params;
  std::shared_ptr<UimInitMessage>         uim_init_msg_ptr = nullptr;

  memset(&ret_params, 0x00, sizeof(ret_params));
  memset(&params, 0x00, sizeof(params));

  if (uim_qmi_ind_msg_ptr != NULL)
  {
    params.instance_id = qmi_ril_get_process_instance_id();
    params.modem_id    = QCRIL_DEFAULT_MODEM_ID;

    switch(uim_qmi_ind_msg_ptr->get_ind_id())
    {
      case QMI_UIM_SRVC_UP_IND_MSG:
        if (mRestart == true)
        {
          qcril_uim_process_modem_restart_complete(&params, &ret_params);
        }
        if (qcril_uim_init_state(&mReady) != 0)
        {
          QCRIL_LOG_INFO("Init failed");
          break;
        }

        /* Broadcast UIM ready message */
        uim_init_msg_ptr = std::make_shared<UimInitMessage>();
        if (uim_init_msg_ptr)
        {
          uim_init_msg_ptr->broadcast();
        }
        break;
      case QMI_UIM_SRVC_DOWN_IND_MSG:
        mRestart = true;
        mReady = false;
        qcril_uim_process_modem_restart_start(&params, &ret_params);
        break;
      default:
        params.data        = uim_qmi_ind_msg_ptr->get_message((uint32_t *)&params.datalen);
        qcril_uim_process_qmi_indication(&params, &ret_params);
        break;
    }
  }
}


void UimModule::UimProcessCardPowerReq(std::shared_ptr<UimCardPowerReqMsg> card_power_req_ptr)
{
  if (card_power_req_ptr == nullptr || mReady == false)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(card_power_req_ptr);

  if (card_power_req_ptr->get_card_power_req() == UIM_UICC_POWER_DOWN)
  {
    qcril_uim_process_power_down(qmi_ril_get_process_instance_id(),
                                 QCRIL_DEFAULT_MODEM_ID,
                                 (int)card_power_req_ptr->get_slot());
  }
  else
  {
    qcril_uim_process_power_up(qmi_ril_get_process_instance_id(),
                               QCRIL_DEFAULT_MODEM_ID,
                               (int)card_power_req_ptr->get_slot());
  }
}

void UimModule::UimProcessChangeSubscription(std::shared_ptr<UimChangeSubscriptionReqMsg> sub_info_ptr)
{
  qcril_request_params_type                    params;
  qcril_request_return_type                    ret_params;

  memset(&ret_params, 0x00, sizeof(ret_params));
  memset(&params, 0x00, sizeof(params));

  if (sub_info_ptr == nullptr || mReady == false)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(sub_info_ptr);

  params.instance_id = qmi_ril_get_process_instance_id();
  params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
  params.data        = (void *)sub_info_ptr->get_sub_info();
  params.datalen     = sizeof(qcril_uicc_subs_info_type);
  params.event_id_android = UIM_INTERNAL_REQUEST_CHANGE_SUBSCRIPTION;
  qcril_uim_process_change_subscription(&params, &ret_params);
}


void UimModule::UimProcessGetImsiSyncReq(std::shared_ptr<UimGetImsiRequestSyncMsg> imsi_req_ptr)
{
  std::shared_ptr<RIL_Errno>                respPtr       = nullptr;
  Message::Callback::Status                 status        = Message::Callback::Status::FAILURE;
  RIL_Errno                                 ret           = RIL_E_RADIO_NOT_AVAILABLE;

  if (imsi_req_ptr == nullptr)
  {
    return;
  }

  if (mReady)
  {
    ret = qcril_uim_direct_get_imsi(imsi_req_ptr->get_slot(),
                                    imsi_req_ptr->get_aid(),
                                    imsi_req_ptr->get_imsi_ptr_address());

    respPtr = std::make_shared<RIL_Errno>(ret);
    if (respPtr != nullptr)
    {
      status = Message::Callback::Status::SUCCESS;
    }
  }
  imsi_req_ptr->sendResponse(imsi_req_ptr, status, respPtr);
}


void UimModule::UimProcessGetCardStatusSyncReq(std::shared_ptr<UimGetCardStatusRequestSyncMsg> req_ptr)
{
  std::shared_ptr<RIL_Errno>                      respPtr         = nullptr;
  Message::Callback::Status                       status          = Message::Callback::Status::FAILURE;
  RIL_Errno                                       ret             = RIL_E_RADIO_NOT_AVAILABLE;

  if (req_ptr == nullptr)
  {
    return;
  }

  if (mReady)
  {
    ret = qcril_uim_direct_get_card_status(req_ptr->get_slot(),
                                           req_ptr->get_card_status_ptr());

    respPtr = std::make_shared<RIL_Errno>(ret);
    if (respPtr != nullptr)
    {
      status = Message::Callback::Status::SUCCESS;
    }
  }

  req_ptr->sendResponse(req_ptr, status, respPtr);
}


void UimModule::UimProcessGetIccIdSycnReq(std::shared_ptr<UimGetIccIdRequestSyncMsg> req_ptr)
{
  std::shared_ptr<RIL_Errno>                 respPtr                    = nullptr;
  Message::Callback::Status                  status                     = Message::Callback::Status::FAILURE;
  RIL_Errno                                  ret                        = RIL_E_RADIO_NOT_AVAILABLE;

  if (req_ptr == nullptr)
  {
    return;
  }

  if (mReady)
  {
    ret = qcril_uim_read_iccid(req_ptr->get_slot(),
                               req_ptr->get_iccid_ptr(),
                               req_ptr->get_iccid_len_ptr());

    respPtr = std::make_shared<RIL_Errno>(ret);
    if (respPtr != nullptr)
    {
      status = Message::Callback::Status::SUCCESS;
    }
  }
  req_ptr->sendResponse(req_ptr, status, respPtr);
}



void UimModule::UimProcessGetImsiReq(std::shared_ptr<UimGetImsiRequestMsg> imsi_req_ptr)
{
  Message::Callback::Status                 status        = Message::Callback::Status::FAILURE;
  RIL_Errno                                 ret           = RIL_E_RADIO_NOT_AVAILABLE;
  char                                    * imsi_ptr      = nullptr;
  std::shared_ptr<char>                     imsi_rsp_ptr  = nullptr;

  if (imsi_req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(imsi_req_ptr);

  if (mReady)
  {
    ret = qcril_uim_direct_get_imsi(imsi_req_ptr->get_slot(),
                                    imsi_req_ptr->get_aid(),
                                    &imsi_ptr);

    if (ret == RIL_E_SUCCESS)
    {
      status = Message::Callback::Status::SUCCESS;
    }

    if (imsi_ptr != nullptr)
    {
      imsi_rsp_ptr = std::shared_ptr<char>(new char[strlen(imsi_ptr) + 1]{0}, [](char *p){delete[] p;});
      if (imsi_rsp_ptr != nullptr)
      {
        memcpy(imsi_rsp_ptr.get(), imsi_ptr, strlen(imsi_ptr));
      }
    }
  }
  imsi_req_ptr->sendResponse(imsi_req_ptr, status, imsi_rsp_ptr);
  
  /* qcril_uim_direct_get_imsi allocated memory using qcril_malloc */
  if (imsi_ptr != nullptr)
  {
    qcril_free(imsi_ptr);
  }
}


void UimModule::UimProcessGetCardStatusReq(std::shared_ptr<UimGetCardStatusRequestMsg> req_ptr)
{
  Message::Callback::Status                       status          = Message::Callback::Status::FAILURE;
  RIL_Errno                                       ret             = RIL_E_RADIO_NOT_AVAILABLE;
  std::shared_ptr<RIL_CardStatus_v6>              respPtr         = nullptr;

  if (req_ptr == nullptr)
  {
    return;
  }

  if (mReady)
  {
    respPtr = std::shared_ptr<RIL_CardStatus_v6>(new RIL_CardStatus_v6);

    if (respPtr != nullptr)
    {
      memset(respPtr.get(), 0x00, sizeof(RIL_CardStatus_v6));

      ret = qcril_uim_direct_get_card_status(req_ptr->get_slot(),
                                             (RIL_CardStatus_v6 *)respPtr.get());

      if (ret == RIL_E_SUCCESS)
      {
        status = Message::Callback::Status::SUCCESS;
      }
    }
  }

  req_ptr->sendResponse(req_ptr, status, respPtr);

  if (respPtr != nullptr)
  {
    /* Cleanup AID and Label info */
    qcril_uim_free_aid_and_label_info((RIL_CardStatus_v6 *)respPtr.get());
  }
}


void UimModule::UimProcessGetIccIdReq(std::shared_ptr<UimGetIccIdRequestMsg> req_ptr)
{
  Message::Callback::Status                  status                     = Message::Callback::Status::FAILURE;
  RIL_Errno                                  ret                        = RIL_E_RADIO_NOT_AVAILABLE;
  std::shared_ptr<UimIccidRspParam>          respPtr                    = nullptr;

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  if (mReady)
  {
    respPtr = std::shared_ptr<UimIccidRspParam>(new UimIccidRspParam{{0},0});

    if (respPtr != nullptr)
    {
      ret = qcril_uim_read_iccid(req_ptr->get_slot(),
                                 respPtr->iccid,
                                 &respPtr->iccid_len);

      if (ret == RIL_E_SUCCESS)
      {
        status = Message::Callback::Status::SUCCESS;
      }
    }
  }
  req_ptr->sendResponse(req_ptr, status, respPtr);
}


void UimModule::UimProcessGetMccMncReq(std::shared_ptr<UimGetMccMncRequestMsg> req_ptr)
{
  qcril_request_params_type                 params       = {};
  qcril_request_return_type                 ret_params   = {};
  UimRequestMsgData                       * msg_data_ptr = nullptr;
  qcril_uim_card_app_type                   app_type     = {};

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_GET_MCC_MNC, nullptr};

  if (mReady == false || msg_data_ptr == nullptr)
  {
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, nullptr);
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }
    return;
  }

  app_type.aid = req_ptr->get_aid();
  switch (req_ptr->get_app_type())
  {
    case QCRIL_UIM_APP_SIM:
      app_type.app_type = QMI_UIM_APP_SIM;
      break;
    case QCRIL_UIM_APP_USIM:
      app_type.app_type = QMI_UIM_APP_USIM;
      break;
    case QCRIL_UIM_APP_RUIM:
      app_type.app_type = QMI_UIM_APP_RUIM;
      break;
    case QCRIL_UIM_APP_CSIM:
      app_type.app_type = QMI_UIM_APP_CSIM;
      break;
    default:
      app_type.app_type = QMI_UIM_APP_UNKNOWN;
      break;
  }

  params.instance_id = qmi_ril_get_process_instance_id();
  params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
  params.data        = &app_type;
  params.datalen     = sizeof(app_type);
  params.t           = (RIL_Token)msg_data_ptr;
  params.event_id_android = UIM_INTERNAL_REQUEST_GET_MCC_MNC;
  qcril_uim_request_get_mcc_mnc(&params, &ret_params);
}


void UimModule::UimProcessGetFacilityLockStatusReq(std::shared_ptr<UimGetFacilityLockRequestMsg> req_ptr)
{
  qcril_request_params_type                       params;
  qcril_request_return_type                       ret_params;
  UimRequestMsgData                             * msg_data_ptr = nullptr;

  memset(&ret_params, 0x00, sizeof(ret_params));
  memset(&params, 0x00, sizeof(params));

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_GET_FACILITY_LOCK, nullptr};

  if (mReady == false || msg_data_ptr == nullptr)
  {
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, nullptr);
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }
    return;
  }

  params.instance_id = qmi_ril_get_process_instance_id();
  params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
  params.data        = (void *)req_ptr->get_facility_lock_data();
  params.t           = (RIL_Token)msg_data_ptr;
  if (req_ptr->get_facility_lock_type() == UimGetFacilityLockRequestMsg::FDN)
  {
    params.event_id_android = UIM_INTERNAL_REQUEST_GET_FDN_STATUS;
    qcril_uim_request_get_fdn_status(&params, &ret_params);
  }
  else
  {
    params.event_id_android = UIM_INTERNAL_REQUEST_GET_PIN_STATUS;
    qcril_uim_request_get_pin_status(&params, &ret_params);
  }
}


void UimModule::UimProcessSetFacilityLockReq(std::shared_ptr<UimSetFacilityLockRequestMsg> req_ptr)
{
  qcril_request_params_type                       params;
  qcril_request_return_type                       ret_params;
  UimRequestMsgData                             * msg_data_ptr = nullptr;

  memset(&ret_params, 0x00, sizeof(ret_params));
  memset(&params, 0x00, sizeof(params));

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_SET_FACILITY_LOCK, nullptr};

  if (mReady == false || msg_data_ptr == nullptr)
  {
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, nullptr);
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }
    return;
  }

  params.instance_id = qmi_ril_get_process_instance_id();
  params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
  params.data        = (void *)req_ptr->get_facility_lock_data();
  params.t           = (RIL_Token)msg_data_ptr;
  if (req_ptr->get_facility_lock_type() == UimSetFacilityLockRequestMsg::FDN)
  {
    params.event_id_android = UIM_INTERNAL_REQUEST_SET_FDN_STATUS;
    qcril_uim_request_set_fdn_status(&params, &ret_params);
  }
  else
  {
    params.event_id_android = UIM_INTERNAL_REQUEST_SET_PIN_STATUS;
    qcril_uim_request_set_pin_status(&params, &ret_params);
  }
}


void UimModule::uim_process_lpa_request(std::shared_ptr<UimLpaReqMessage> req_ptr)
{
  boolean  ret = FALSE;

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  switch(req_ptr->get_req_id())
  {
    case UIM_LPA_USER_REQUEST_ID:
      ret = qcril_uim_lpa_user_request(req_ptr);
      break;
    default:
      break;
  }

  if (ret == FALSE)
  {
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, nullptr);
  }
}


void UimModule::UimProcessGetAtrReq(std::shared_ptr<UimGetAtrRequestMsg> req_ptr)
{
  UimRequestMsgData                     * msg_data_ptr = nullptr;
  RIL_Errno                               ril_err      = RIL_E_GENERIC_FAILURE;
  qcril_request_params_type               params{};
  qcril_request_return_type               ret_params{};

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (req_ptr == nullptr)
  {
    return;
  }

  /* Cancel timeout slow SIM card can take more time */
  Dispatcher::getInstance().clearTimeoutForMessage(req_ptr);

  msg_data_ptr = new UimRequestMsgData{req_ptr, UimRequestMsgData::UIM_REQUEST_GET_ATR, nullptr};

  QCRIL_LOG_INFO( "mReady :%d", mReady);

  if (mReady == false || msg_data_ptr == nullptr)
  {
    std::shared_ptr<UimAtrRspParam>   rsp_ptr = nullptr;
    if (msg_data_ptr != nullptr)
    {
      delete msg_data_ptr;
      msg_data_ptr = nullptr;
    }

    if (mReady == false)
    {
      ril_err = RIL_E_RADIO_NOT_AVAILABLE;
    }
    rsp_ptr = std::shared_ptr<UimAtrRspParam>(new UimAtrRspParam{ril_err, nullptr, 0});
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, rsp_ptr);
    return;
  }

  /* Request message data to RIL_Token(void *) will be used when sending response */
  params.instance_id = qmi_ril_get_process_instance_id();
  params.modem_id    = QCRIL_DEFAULT_MODEM_ID;
  params.data        = (void *)req_ptr->get_slot();
  params.t           = (RIL_Token)msg_data_ptr;
  params.event_id_android = UIM_INTERNAL_REQUEST_GET_ATR;
  qcril_uim_request_get_atr(&params, &ret_params);
} /* UimProcessGetAtrReq */


UimModule *getUimModule()
{
  return &(uim_module.get_module());
}


int UimModule::UimSendQmiUimRequest
(
  uint32                        msg_id,
  const void                  * params,
  void                        * rsp_data_ptr,
  void                        * original_request_ptr,
  qmi_uim_user_async_cb_type    cb_ptr
)
{
  /* Sync request or QMI async message count less than MAX_QMI_UIM_ASYNC_MSG */
  if (mQmiAsyncMsgCount < MAX_QMI_UIM_ASYNC_MSG ||
      rsp_data_ptr != nullptr)
  {
    std::shared_ptr<UimQmiUimRequestSyncMsg> req_msg_ptr    = nullptr;
    std::shared_ptr<int>                     respPtr        = nullptr;

    req_msg_ptr = std::make_shared<UimQmiUimRequestSyncMsg>(msg_id,
                                                            params,
                                                            rsp_data_ptr,
                                                            original_request_ptr,
                                                            cb_ptr);
    if (req_msg_ptr == nullptr)
    {
      return -1;
    }

    if (req_msg_ptr->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS)
    {
      if (respPtr == nullptr)
      {
        return -1;
      }
      /* Request is qmi async increase QMI msg count */
      if (*respPtr == 0 && original_request_ptr != nullptr)
      {
        mQmiAsyncMsgCount++;
      }
      else
      {
        return *respPtr;
      }
    }
    else
    {
      return -1;
    }
  }
  else
  {
    std::shared_ptr<UimQmiUimRequestMsg> req_msg_ptr    = nullptr;

    req_msg_ptr = std::make_shared<UimQmiUimRequestMsg>(msg_id,
                                                        params,
                                                        original_request_ptr,
                                                        cb_ptr);

    if (req_msg_ptr == nullptr)
    {
      return -1;
    }

    /* Async message and waiting for 10 QMI async responses */
    QCRIL_LOG_INFO("Waiting for response from QMI UIM putting message in the QMI Queue");
    mQmiPendingList.push(req_msg_ptr);
  }

  return 0;
}


#ifdef QMI_RIL_UTF
void UimModule::qcrilHalUimModuleCleanup()
{
  std::shared_ptr<UimModemEndPoint> mUimModemEndPoint =
          ModemEndPointFactory<UimModemEndPoint>::getInstance().buildEndPoint();
  UimModemEndPointModule* mUimModemEndPointModule =
          (UimModemEndPointModule*)mUimModemEndPoint->mModule;
  mUimModemEndPointModule->cleanUpQmiSvcClient();
  mReady = false;
  mRestart = false;
}
#endif
