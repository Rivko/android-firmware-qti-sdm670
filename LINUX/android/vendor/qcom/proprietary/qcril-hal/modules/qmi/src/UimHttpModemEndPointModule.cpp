/******************************************************************************
#  @file    UimHttpModemEndPointModule.cpp
#  @brief   Registers with QMI UIM HTTP and sends receives messages to and from
#           modem using QMI UIM interface
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include <cstring>

#include "common_v01.h"
#include "user_identity_module_http_v01.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceDownIndMessage.h"
#include "modules/qmi/QmiUimHttpSetupRequest.h"
#include "modules/lpa/LpaQmiUimHttpIndicationMsg.h"
#include "modules/lpa/LpaUimHttpTransactionIndMsg.h"
#include "modules/lpa/LpaUimHttpTransactionRspMsg.h"
#include "modules/lpa/LpaUimHttpSrvc.h"
#include "framework/ModuleLooper.h"

#include "UimHttpModemEndPointModule.h"

#ifdef QMI_RIL_UTF
extern "C"{
#include "ril_utf_qmi_sim.h"
}
#endif

#define TAG "UimHttpModemEndPointModule"

#define QCRIL_QMI_UIM_HTTP_FREE_PTR(ptr)                                    \
  if (ptr != nullptr)                                                       \
  {                                                                         \
    delete ptr;                                                             \
    ptr = nullptr;                                                          \
  }                                                                         \


/*=========================================================================

FUNCTION:  qmiNotifyCb

Description : Callback for qmi service notification

===========================================================================*/
static void qmiNotifyCb
(
  qmi_client_type user_handle,
  qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event,
  void *notify_cb_data
)
{
  /* Wrapper for member method*/
  UimHttpModemEndPointModule *me = (UimHttpModemEndPointModule *)notify_cb_data;
  me->onQmiNotifyCb(user_handle, service_obj, service_event);
}


/*=========================================================================

FUNCTION:  qcril_uim_http_generic_response_callback

Description : Callback for two things
1. the error transaction request send to MODEM
2. reset request send to MODEM

===========================================================================*/
static void qcril_uim_http_generic_response_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_http_rsp_ptr,
  unsigned int                   qmi_http_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  (void)user_handle;
  (void)qmi_http_rsp_len;
  (void)resp_cb_data_ptr;

  Log::getInstance().d("qcril_uim_http_generic_response_callback: msg_id = "
    + std::to_string(msg_id) + "with transp err =  " + std::to_string(transp_err));

  delete (uim_http_transaction_resp_msg_v01 *)qmi_http_rsp_ptr;
  qmi_http_rsp_ptr = nullptr;
} /* qcril_uim_http_generic_response_callback */


/*=========================================================================

FUNCTION:  qmiUnsolIndCb

Description : Callback for qmi service notification

===========================================================================*/
static void qmiUnsolIndCb
(
  qmi_client_type user_handle,
  unsigned long msg_id,
  unsigned char *ind_buf,
  int ind_buf_len,
  void *ind_cb_data
)
{
  if (ind_cb_data == nullptr)
  {
    return;
  }

  UimHttpModemEndPointModule *me = (UimHttpModemEndPointModule *)ind_cb_data;
  me->onQmiUnsolIndCb(user_handle, msg_id, ind_buf, ind_buf_len);
}


/*=========================================================================

FUNCTION:  qmi_uim_http_request_cb

Description : Callback for qmi uim http request

===========================================================================*/
static void qmi_uim_http_request_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_http_rsp_ptr,
  unsigned int                   qmi_http_rsp_len,
  void                         * resp_cb_data_ptr,
  qmi_client_error_type          transp_err
)
{
  if (resp_cb_data_ptr == nullptr)
  {
    if (qmi_http_rsp_ptr != nullptr)
    {
      delete (uim_http_reset_resp_msg_v01 *)qmi_http_rsp_ptr;
      qmi_http_rsp_ptr = nullptr;
    }
    return;
  }

  /* Wrapper for member method*/
  UimHttpModemEndPointModule *me = (UimHttpModemEndPointModule *)resp_cb_data_ptr;
  me->qcril_uim_http_transaction_completed_callback(user_handle, msg_id, qmi_http_rsp_ptr, qmi_http_rsp_len, transp_err);
}


/*=========================================================================

FUNCTION:  UimHttpModemEndPointModule

Description : UIM HTTP qmi module constructor

===========================================================================*/
UimHttpModemEndPointModule::UimHttpModemEndPointModule
(
  string name,
  ModemEndPoint *owner
)
{
  mName = name;
  mOwner = owner;
  mServiceObject = nullptr;
  mQmiSvcClient = nullptr;
  http_trasaction_ind_ptr = nullptr;
  http_transaction_req_ptr = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
       HANDLER(QmiUimHttpSetupRequest, UimHttpModemEndPointModule::handleQmiClientSetup),
       HANDLER(LpaUimHttpRequestMsg, UimHttpModemEndPointModule::qcril_uim_http_client_transaction_request),
       HANDLER(LpaUimHttpTransactionIndMsg, UimHttpModemEndPointModule::qcril_uim_http_handle_transaction_ind),
       HANDLER(LpaUimHttpTransactionRspMsg, UimHttpModemEndPointModule::qcril_uim_http_process_transaction_completed_qmi_callback),
       HANDLER(QmiServiceUpIndMessage, UimHttpModemEndPointModule::handleQmiServiceUp),
       HANDLER(QmiServiceDownIndMessage, UimHttpModemEndPointModule::handleQmiServiceDown),
  };
}


/*=========================================================================

FUNCTION:  UimHttpModemEndPointModule

Description : UIM HTTP qmi module destructor

===========================================================================*/
UimHttpModemEndPointModule::~UimHttpModemEndPointModule()
{
  mLooper = nullptr;

  if (mQmiNotifyHandle != nullptr)
  {
    (void)qmi_client_release(mQmiNotifyHandle);
    mQmiNotifyHandle = nullptr;
  }

  if (mQmiSvcClient != nullptr)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }
  mServiceObject = nullptr;
}

/*=========================================================================

  FUNCTION:  qcril_uim_http_copy_indication

  Description : Makes a copy of the indication received from QMI UIM HTTP.

===========================================================================*/
uint8_t  * UimHttpModemEndPointModule::qcril_uim_http_copy_indication
(
  qmi_client_type                user_handle_ptr,
  unsigned int                   msg_id,
  unsigned char                * qmi_http_ind_ptr,
  unsigned int                   qmi_http_ind_len,
  uint32_t                     * out_len_ptr
)
{
  uint8_t                          * decoded_payload_ptr  = nullptr;
  uint32_t                           decoded_payload_len  = 0;
  qmi_client_error_type              qmi_err              = QMI_INTERNAL_ERR;

  if ((user_handle_ptr == nullptr) ||
      (qmi_http_ind_ptr == nullptr) ||
      (out_len_ptr     == nullptr))
  {
    return nullptr;
  }

  /* First decode the message payload from QCCI */
  qmi_idl_get_message_c_struct_len(uim_http_get_service_object_v01(),
                                   QMI_IDL_INDICATION,
                                   msg_id,
                                   &decoded_payload_len);

  if (decoded_payload_len == 0)
  {
    return nullptr;
  }

  /* Allocate decoded payload buffer */
  decoded_payload_ptr = new uint8_t[decoded_payload_len];
  if (decoded_payload_ptr == nullptr)
  {
    return nullptr;
  }
  memset(decoded_payload_ptr, 0x00, decoded_payload_len);

  /* Decode the Indication payload */
  qmi_err = qmi_client_message_decode(user_handle_ptr,
                                      QMI_IDL_INDICATION,
                                      msg_id,
                                      qmi_http_ind_ptr,
                                      qmi_http_ind_len,
                                      (void *)decoded_payload_ptr,
                                      decoded_payload_len);
  if (qmi_err != QMI_NO_ERR)
  {
    if (decoded_payload_ptr != nullptr)
    {
      delete[] decoded_payload_ptr;
      decoded_payload_ptr = nullptr;
    }
    return nullptr;
  }

  /* Initialize the payload data & assign the message data pointer */
  *out_len_ptr = decoded_payload_len;

  return decoded_payload_ptr;
} /* qcril_uim_http_copy_indication */


/*=========================================================================

FUNCTION:  onQmiUnsolIndCb

Description : UIM HTTP qmi module unsol indication callback

===========================================================================*/
void UimHttpModemEndPointModule::onQmiUnsolIndCb
(
  qmi_client_type user_handle,
  unsigned long msg_id,
  unsigned char *ind_buf_ptr,
  int ind_buf_len
  )
{
  uint8_t   *msg_ptr = nullptr;
  uint32_t   msg_len = 0;
  Log::getInstance().d("[UimHttpModemEndPointModule]: onQmiUnsolIndCb()");

  if ((ind_buf_ptr == nullptr) || (ind_buf_len == 0))
  {
    return;
  }

  /* Process only the supported IND messages */
  switch (msg_id)
  {
    case QMI_UIM_HTTP_TRANSACTION_IND_V01:
      msg_ptr = qcril_uim_http_copy_indication(user_handle,
                                               msg_id,
                                               ind_buf_ptr,
                                               ind_buf_len,
                                               &msg_len);
      break;

    default:
      Log::getInstance().d("Unsupported QMI UIM HTTP indication: 0x%x"+ std::to_string(msg_id));
      break;
  }

  if (msg_ptr != nullptr && msg_len > 0)
  {
    this->dispatch(
        std::shared_ptr<Message>(new LpaUimHttpTransactionIndMsg(msg_len, msg_ptr, msg_id, 0)));
    delete[] msg_ptr;
  }
} /* UimHttpModemEndPointModule::onQmiUnsolIndCb */


/*=========================================================================

FUNCTION:  onQmiNotifyCb

Description : UIM HTTP qmi module unsol notification callback

===========================================================================*/
void UimHttpModemEndPointModule::onQmiNotifyCb
(
  qmi_client_type user_handle, qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event
)
{
  (void)user_handle;
  (void)service_obj;
  Log::getInstance().d("[UimHttpModemEndPointModule]: onQmiNotifyCb()");

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]: qmiNotifyCb() Service up indication");

      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceUpIndMessage(nullptr)));

      break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]: qmiNotifyCb() Service down indication");
      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceDownIndMessage(QMI_SERVICE_ERR)));
      break;
    default:
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]: qmiNotifyCb() unsupported service "
          "event: " +
          std::to_string(service_event));
      break;
  }
  return;
}


/*=========================================================================

FUNCTION:  handleQmiClientSetup

Description : UIM HTTP qmi module client setup

===========================================================================*/
void UimHttpModemEndPointModule::handleQmiClientSetup
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;
  Log::getInstance().d("[UimHttpModemEndPointModule]: handleQmiClientSetup()");

  if (mServiceObject == nullptr)
  {
    mServiceObject = uim_http_get_service_object_v01();
    if (mServiceObject == nullptr)
    {
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]:Did not get uim_get_service_object");
      return;
    }
    else
    {
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]:Got uim_get_service_object");
    }

    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);

    if (rc == QMI_NO_ERR)
    {
      mMessageList.push_back(msg);
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmiNotifyCb, this);
      if (rc != QMI_NO_ERR) {
        Log::getInstance().d(
            "[UimHttpModemEndPointModule]: qmi_client_register_notify_cb "
            "failed: " +
            std::to_string(rc));
      }
    mOwner->setState(ModemEndPoint::State::WAITING_SERVICE_UP);
    }
    else
    {
      Log::getInstance().d(
          "[UimHttpModemEndPointModule]: qmi_client_notifier_init failed: " +
          std::to_string(rc));
    }
    return;
  }
  else
  {
    mMessageList.push_back(msg);
    return;
  }
}


/*=========================================================================

FUNCTION:  handleQmiServiceUp

Description : QMI service up handler

===========================================================================*/
void UimHttpModemEndPointModule::handleQmiServiceUp
(
  std::shared_ptr<Message> msg_ptr
)
{
  qmi_client_type                 qmi_svc_client   = nullptr;
  qmi_client_error_type           client_err       = QMI_NO_ERR;
  uim_http_reset_req_msg_v01      qmi_http_req;
  uim_http_reset_resp_msg_v01   * qmi_http_rsp_ptr = nullptr;
  qmi_txn_handle                  txn_handle;
  qmi_service_info                info;

  (void) msg_ptr;
  Log::getInstance().d("[UimHttpModemEndPointModule]: handleQmiServiceUp()");

  if (mQmiSvcClient != nullptr)
  {
    QCRIL_LOG_INFO( "Already Registered to service");
    return;
  }

  memset(&mOsParams, 0x00, sizeof(mOsParams));

  client_err = qmi_client_get_any_service(mServiceObject, &info);
  Log::getInstance().d("[UimHttpModemEndPointModule]: client_info: " + std::to_string(client_err));

  client_err = qmi_client_init(&info,
                               mServiceObject,
                               (qmi_client_ind_cb)qmiUnsolIndCb,
                               this,
                               &mOsParams,
                               &qmi_svc_client);

  mOwner->setState(ModemEndPoint::State::CLIENT_INIT_REQUESTED);

  Log::getInstance().d("[UimHttpModemEndPointModule]: client_err = " +
                       std::to_string(client_err));
  if (client_err != QMI_NO_ERR)
  {
    Log::getInstance().d("[UimHttpModemEndPointModule]: Error in client init");
  }
  else
  {
    Log::getInstance().d("[UimHttpModemEndPointModule]: No Error in client init");
    mQmiSvcClient = qmi_svc_client;
    mOwner->setState(ModemEndPoint::State::OPERATIONAL);

    // Notify clients of this end-point that  we have qmi client handle.
    for (auto msg : mMessageList)
    {
      std::shared_ptr<QmiUimHttpSetupRequest> shared_msg =
          std::static_pointer_cast<QmiUimHttpSetupRequest>(msg);

      Log::getInstance().d("[UimHttpModemEndPointModule]: Notify client = " +
                           msg->dump());

      if(shared_msg)
      {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
                                 nullptr);
      }
    }

    std::shared_ptr<LpaQmiUimHttpIndicationMsg> ind_msg_ptr =
         std::make_shared<LpaQmiUimHttpIndicationMsg>(QMI_UIM_HTTP_SRVC_UP_IND_MSG,
                                                      nullptr);
    if (ind_msg_ptr != nullptr)
    {
      ind_msg_ptr->broadcast();
    }

    mMessageList.clear();

    /* UTF is not supported for HTTP, don't send event during initalization which will fail
       bootup test cases */
    Log::getInstance().d("QMI UIM HTTP Client init complete, send reset request to MODEM");

    qmi_http_rsp_ptr = new uim_http_reset_resp_msg_v01;

    if (qmi_http_rsp_ptr == nullptr)
    {
      Log::getInstance().d("error allocating memory for rsp pointers");
      return;
    }

    memset(&qmi_http_req, 0x00, sizeof(uim_http_reset_req_msg_v01));
    memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_reset_resp_msg_v01));

    client_err = qmi_client_send_msg_async( mQmiSvcClient,
                                            QMI_UIM_HTTP_RESET_REQ_V01,
                                            (void *)&qmi_http_req,
                                            sizeof(uim_http_reset_req_msg_v01),
                                            (void *)qmi_http_rsp_ptr,
                                            sizeof(uim_http_reset_resp_msg_v01),
                                            qcril_uim_http_generic_response_callback,
                                            nullptr,
                                            &txn_handle);

    if (client_err != 0)
    {
      Log::getInstance().d("error in sending http reset req");
      delete qmi_http_rsp_ptr;
      qmi_http_rsp_ptr = nullptr;
    }
  }
}


/*=========================================================================

FUNCTION:  handleQmiServiceDown

Description : QMI service down handler

===========================================================================*/
void UimHttpModemEndPointModule::handleQmiServiceDown
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;

  if (mQmiSvcClient != NULL)
  {
    (void)qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
  }

  QCRIL_LOG_INFO( "handleQmiServiceDown : %d", mOwner->getState());

  mOwner->setState(ModemEndPoint::State::NON_OPERATIONAL);

  std::shared_ptr<LpaQmiUimHttpIndicationMsg> ind_msg_ptr =
       std::make_shared<LpaQmiUimHttpIndicationMsg>(QMI_UIM_HTTP_SRVC_DOWN_IND_MSG,
                                                    nullptr);
  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* UimHttpModemEndPointModule::handleQmiServiceDown */


/*=========================================================================

  FUNCTION:  qcril_uim_http_transaction_completed_callback

  Description : Callback implementation for the QMI UIM HTTP commands.

===========================================================================*/
void UimHttpModemEndPointModule::qcril_uim_http_transaction_completed_callback
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                         * qmi_http_rsp_ptr,
  unsigned int                   qmi_http_rsp_len,
  qmi_client_error_type          transp_err
)
{
  (void) user_handle;

  if (http_transaction_req_ptr == nullptr)
  {
    return;
  }

  /* Process only the supported RESP messages */
  switch (msg_id)
  {
    case QMI_UIM_HTTP_TRANSACTION_RESP_V01:
      this->dispatch(std::shared_ptr<Message>(
                         new LpaUimHttpTransactionRspMsg(qmi_http_rsp_len,
                                                         qmi_http_rsp_ptr,
                                                         msg_id, transp_err)));
      break;
    default:
      break;
  }
} /* qcril_uim_http_transaction_completed_callback */


/*===========================================================================

FUNCTION:  qcril_uim_http_send_error_response

Description : This function sends response to the Modem for any failure
in the transaction indication

===========================================================================*/
void UimHttpModemEndPointModule::qcril_uim_http_send_error_response
(
  uint32_t token_id
)
{
  int                                                  qmi_err_code = 0;
  uim_http_transaction_req_msg_v01                   * qmi_http_req_ptr = nullptr;
  uim_http_transaction_resp_msg_v01                  * qmi_http_rsp_ptr = nullptr;
  qmi_txn_handle                                       txn_handle;

  if (token_id == 0)
  {
    /* do nothing if the token ID is invalid */
    return;
  }

  qmi_http_req_ptr = new uim_http_transaction_req_msg_v01;
  qmi_http_rsp_ptr = new uim_http_transaction_resp_msg_v01;

  if ((qmi_http_rsp_ptr == nullptr) || (qmi_http_req_ptr == nullptr))
  {
    if (qmi_http_req_ptr != nullptr)
    {
      delete qmi_http_req_ptr;
      qmi_http_req_ptr = nullptr;
    }

    if (qmi_http_rsp_ptr != nullptr)
    {
      delete qmi_http_rsp_ptr;
      qmi_http_rsp_ptr = nullptr;
    }
    Log::getInstance().d("error allocating memory for req or rsp pointers");
    return;
  }

  memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_transaction_resp_msg_v01));

  /* Fill the qmi_http_req_ptr with token ID and result as error */
  memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));
  qmi_http_req_ptr->token_id = token_id;
  qmi_http_req_ptr->result = UIM_HTTP_UNKNOWN_ERROR_V01;

  qmi_err_code = qmi_client_send_msg_async(
    mQmiSvcClient,
    QMI_UIM_HTTP_TRANSACTION_REQ_V01,
    (void *)qmi_http_req_ptr,
    sizeof(uim_http_transaction_req_msg_v01),
    (void *)qmi_http_rsp_ptr,
    sizeof(uim_http_transaction_resp_msg_v01),
    qcril_uim_http_generic_response_callback,
    nullptr,
    &txn_handle);

  if (qmi_err_code != 0)
  {
    Log::getInstance().d("Failure in sending the error notification to modem");
    delete qmi_http_rsp_ptr;
    qmi_http_rsp_ptr = nullptr;
  }
  delete qmi_http_req_ptr;
  qmi_http_req_ptr = nullptr;
} /* qcril_uim_http_send_error_response */


/*===========================================================================

  FUNCTION:  qcril_uim_http_handle_transaction_ind

  Description : Handles the HTTP indication

===========================================================================*/
void UimHttpModemEndPointModule::qcril_uim_http_handle_transaction_ind
(
  std::shared_ptr<Message> msg_ptr
)
{
  uint8_t                              index            = 0;
  uim_http_transaction_ind_msg_v01   * qmi_http_ind_ptr = nullptr;
  uint32_t                             msg_len          = 0;
  uint32_t                             segment_offset   = 0;

  std::shared_ptr<LpaUimHttpTransactionIndMsg> http_transaction_ind_msg_ptr(
    std::static_pointer_cast<LpaUimHttpTransactionIndMsg>(msg_ptr));

  if (http_transaction_ind_msg_ptr == nullptr)
  {
    return;
  }

  qmi_http_ind_ptr = (uim_http_transaction_ind_msg_v01 *)http_transaction_ind_msg_ptr->get_message(&msg_len);

  if (qmi_http_ind_ptr == nullptr)
  {
    return;
  }

  Log::getInstance().d("token_id: " + std::to_string(qmi_http_ind_ptr->token_id));

  /* Token ID should be same for all the chunks of a http command payload */
  if (http_trasaction_ind_ptr != nullptr && http_trasaction_ind_ptr->token_id != qmi_http_ind_ptr->token_id)
  {
    Log::getInstance().d("Token ID received is different than expected, send error to previous token and process the new indication");
    goto report_error_to_modem;
  }

  /* http_transaction_ind_ptr will be nullptr for the first chunk
     of a http request payload from the MODEM */
  if (http_trasaction_ind_ptr == nullptr)
  {
    http_trasaction_ind_ptr = new uim_http_transaction_ind_msg;
    if (http_trasaction_ind_ptr == nullptr)
    {
      Log::getInstance().d("error allocating memory for http_trasaction_payload_ptr");
      goto report_error_to_modem;
    }
    memset(http_trasaction_ind_ptr, 0x00, sizeof(uim_http_transaction_msg));
    http_trasaction_ind_ptr->token_id = qmi_http_ind_ptr->token_id;

    if (qmi_http_ind_ptr->payload_body_valid)
    {
      uint32_t   payload_len = 0;

      if (qmi_http_ind_ptr->segment_info_valid)
      {
        payload_len = qmi_http_ind_ptr->segment_info.total_size;
      }
      else
      {
        payload_len = qmi_http_ind_ptr->payload_body_len;
      }

      http_trasaction_ind_ptr->payload_ptr = new uint8_t[payload_len]{};
      if (http_trasaction_ind_ptr->payload_ptr == nullptr)
      {
        Log::getInstance().d("error allocating memory for http_trasaction_payload_ptr");
        goto report_error_to_modem;
      }
      http_trasaction_ind_ptr->payload_len = payload_len;
      memset(http_trasaction_ind_ptr->payload_ptr, 0x00, sizeof(uint8_t) * http_trasaction_ind_ptr->payload_len);
    }
  }

  /* Copying URL */
  if (qmi_http_ind_ptr->url_valid)
  {
    http_trasaction_ind_ptr->url_valid = true;

    strlcpy(http_trasaction_ind_ptr->url,
            qmi_http_ind_ptr->url,
            UIM_HTTP_URL_MAX_LEN);
    http_trasaction_ind_ptr->url[UIM_HTTP_URL_MAX_LEN] = '\0';
  }

  /* Copying headers */
  if (qmi_http_ind_ptr->headers_valid)
  {
    http_trasaction_ind_ptr->headers_valid = true;

    strlcpy(http_trasaction_ind_ptr->headers.content_type,
            qmi_http_ind_ptr->headers.content_type,
            UIM_HTTP_HEADER_VALUE_MAX_LEN);
    http_trasaction_ind_ptr->headers.content_type[UIM_HTTP_HEADER_VALUE_MAX_LEN] = '\0';

    for(index = 0;
        index < qmi_http_ind_ptr->headers.custom_header_len &&
        index < UIM_HTTP_CUST_HEADER_MAX_COUNT_V01 &&
        index < UIM_HTTP_CUST_HEADER_MAX_COUNT;
        index++)
    {
      strlcpy(http_trasaction_ind_ptr->headers.custom_header[index].name,
              qmi_http_ind_ptr->headers.custom_header[index].name,
              UIM_HTTP_HEADER_NAME_MAX_LEN);
      http_trasaction_ind_ptr->headers.custom_header[index].name[UIM_HTTP_HEADER_NAME_MAX_LEN] = '\0';

      strlcpy(http_trasaction_ind_ptr->headers.custom_header[index].value,
              qmi_http_ind_ptr->headers.custom_header[index].value,
              UIM_HTTP_HEADER_VALUE_MAX_LEN);
      http_trasaction_ind_ptr->headers.custom_header[index].value[UIM_HTTP_HEADER_VALUE_MAX_LEN] = '\0';
    }
  }

  /* Copying Payload */
  if ((qmi_http_ind_ptr->payload_body_valid != 0) &&
      (qmi_http_ind_ptr->payload_body_len > 0) &&
      (http_trasaction_ind_ptr->payload_ptr != nullptr))
  {
    if (qmi_http_ind_ptr->segment_info_valid)
    {
      segment_offset = qmi_http_ind_ptr->segment_info.segment_offset;
    }

    if(segment_offset + qmi_http_ind_ptr->payload_body_len <= http_trasaction_ind_ptr->payload_len)
    {
      memcpy(http_trasaction_ind_ptr->payload_ptr + segment_offset,
             qmi_http_ind_ptr->payload_body,
             qmi_http_ind_ptr->payload_body_len);
    }
    else
    {
      goto report_error_to_modem;
    }

    Log::getInstance().d("Received the http transaction request payload. Received bytes: " +
                           std::to_string(qmi_http_ind_ptr->payload_body_len + segment_offset) +
                           "total payload size: " + std::to_string(http_trasaction_ind_ptr->payload_len));
  }

  /* Send the entire http payload to the client after receiving the total payload from multiple chunk indications */
  if(segment_offset + qmi_http_ind_ptr->payload_body_len == http_trasaction_ind_ptr->payload_len)
  {
    std::shared_ptr<LpaQmiUimHttpIndicationMsg> qcril_uim_http_ind_ptr =
           std::make_shared<LpaQmiUimHttpIndicationMsg>(QMI_UIM_HTTP_SRVC_TRANSACTION_IND_MSG,
                                                        http_trasaction_ind_ptr);

    if (qcril_uim_http_ind_ptr != nullptr)
    {
      qcril_uim_http_ind_ptr->broadcast();
    }

    goto cleanup_memory;
  }

  return;

report_error_to_modem:
  qcril_uim_http_send_error_response(qmi_http_ind_ptr->token_id);

cleanup_memory:
  if (http_trasaction_ind_ptr != nullptr &&
      http_trasaction_ind_ptr->payload_ptr != nullptr)
  {
    delete[] http_trasaction_ind_ptr->payload_ptr;
    http_trasaction_ind_ptr->payload_ptr = nullptr;
  }
  QCRIL_QMI_UIM_HTTP_FREE_PTR(http_trasaction_ind_ptr);
  http_transaction_ind_msg_ptr = nullptr;
} /* qcril_uim_http_handle_transaction_ind */


/*=========================================================================

  FUNCTION:  qcril_uim_http_process_transaction_completed_qmi_callback

  Description : This function sends the http response payload to the MODEM in chunks

===========================================================================*/
void UimHttpModemEndPointModule::qcril_uim_http_process_transaction_completed_qmi_callback
(
  std::shared_ptr<Message> msg_ptr
)
{
  qmi_txn_handle                           txn_handle;
  int                                      qmi_err_code        = 0;
  uint32_t                                       msg_len             = 0;
  uim_http_transaction_req_msg_v01       * qmi_http_req_ptr    = nullptr;
  uim_http_transaction_resp_msg_v01      * qmi_http_rsp_ptr    = nullptr;
  uim_http_transaction_msg               * http_req_ptr        = nullptr;
  std::shared_ptr<LpaUimHttpTransactionRspMsg>   http_transaction_rsp_msg_ptr(
    std::static_pointer_cast<LpaUimHttpTransactionRspMsg>(msg_ptr));
  std::shared_ptr<lpa_qmi_uim_http_rsp_data_type> resp_data_ptr = nullptr;

  if (http_transaction_req_ptr == nullptr || http_transaction_rsp_msg_ptr == nullptr)
  {
    return;
  }

  http_req_ptr = (uim_http_transaction_msg *)http_transaction_req_ptr->get_message();

  if (http_req_ptr == nullptr)
  {
    return;
  }

  if (http_transaction_rsp_msg_ptr->get_transport_err() != 0)
  {
    goto clean_up;
  }

  http_req_ptr->offset += UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;

  QCRIL_LOG_INFO("Token: 0x%x, result: %d, payload_len: %d, offset:%d",
    http_req_ptr->token_id, http_req_ptr->result, http_req_ptr->payload_len,
    http_req_ptr->offset);

  /* Check to see if there are still chunks left that needs to be sent to MODEM */
  if(http_req_ptr->offset < http_req_ptr->payload_len &&
     http_req_ptr->payload_ptr != nullptr)
  {
    /* Allocate memory for the new QMI HTTP transaction request where the next payload chunk will be sent */
    qmi_http_req_ptr = new uim_http_transaction_req_msg_v01;
    if (qmi_http_req_ptr == nullptr)
    {
      goto clean_up;
    }

    memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));
    qmi_http_req_ptr->token_id = http_req_ptr->token_id;
    qmi_http_req_ptr->result   = (uim_http_result_type_enum_v01)http_req_ptr->result;
    qmi_http_req_ptr->segment_info_valid = 0x01;
    qmi_http_req_ptr->payload_body_valid = 0x01;
    qmi_http_req_ptr->segment_info.total_size = http_req_ptr->payload_len;
    qmi_http_req_ptr->segment_info.segment_offset = http_req_ptr->offset;

    if ((http_req_ptr->payload_len - http_req_ptr->offset) > UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 )
    {
      qmi_http_req_ptr->payload_body_len = UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;
    }
    else
    {
      qmi_http_req_ptr->payload_body_len = (http_req_ptr->payload_len - http_req_ptr->offset);
    }
    memcpy(qmi_http_req_ptr->payload_body,
           http_req_ptr->payload_ptr + http_req_ptr->offset,
           qmi_http_req_ptr->payload_body_len);

    QCRIL_LOG_INFO("payload_len: %d, total: %d, offset: %d",
      qmi_http_req_ptr->payload_body_len, qmi_http_req_ptr->segment_info.total_size,
      qmi_http_req_ptr->segment_info.segment_offset);

    /* Allocate response pointer since it is an async command */
    qmi_http_rsp_ptr = new uim_http_transaction_resp_msg_v01;
    if (qmi_http_rsp_ptr == nullptr)
    {
      goto clean_up;
    }
    memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_transaction_resp_msg_v01));
    /* Proceed with transaction request */
    qmi_err_code = qmi_client_send_msg_async(
                     mQmiSvcClient,
                     QMI_UIM_HTTP_TRANSACTION_REQ_V01,
                     (void *) qmi_http_req_ptr,
                     sizeof(uim_http_transaction_req_msg_v01),
                     (void *) qmi_http_rsp_ptr,
                     sizeof(uim_http_transaction_resp_msg_v01),
                     qmi_uim_http_request_cb,
                     this,
                     &txn_handle);

    /* On successful API call, free only the request */
    if (qmi_err_code != 0)
    {
      goto clean_up;
    }

    QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
    return;
  }

  resp_data_ptr = std::shared_ptr<lpa_qmi_uim_http_rsp_data_type>(new lpa_qmi_uim_http_rsp_data_type{});

  if (resp_data_ptr == nullptr)
  {
    goto clean_up;
  }

  resp_data_ptr->transp_err = -1;
  resp_data_ptr->qmi_error_code = -1;
  
  /* When the entire response payload is sent, the client is notified using qcril_uim_http_transaction_resp */
  switch(http_transaction_rsp_msg_ptr->get_msg_id())
  {
    case QMI_UIM_HTTP_TRANSACTION_RESP_V01:
      qmi_http_rsp_ptr = (uim_http_transaction_resp_msg_v01 *)http_transaction_rsp_msg_ptr->get_message(&msg_len);

      resp_data_ptr->rsp_id = LPA_QMI_UIM_HTTP_TRANSACTION_RSP;
      resp_data_ptr->transp_err = http_transaction_rsp_msg_ptr->get_transport_err();

      if (qmi_http_rsp_ptr != nullptr)
      {
        resp_data_ptr->qmi_error_code = qmi_http_rsp_ptr->resp.error;
      }
      break;

    default:
      /* This shouldn't happen since we never post for these msg ids */
      Log::getInstance().d("Unsupported QMI UIM HTTP response: " + std::to_string(http_transaction_req_ptr->get_msg_id()));
      break;
   }

clean_up:
  http_transaction_req_ptr->sendResponse((std::shared_ptr<Message>)http_transaction_req_ptr,
                                         Message::Callback::Status::SUCCESS,
                                         resp_data_ptr);
  /* Free memory allocated previously.*/
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
  http_transaction_req_ptr = nullptr;
} /* qcril_uim_http_process_transaction_completed_qmi_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_http_client_transaction_request

  Description : Handles UIM_HTTP_TRANSACTION request from the client.

===========================================================================*/
void UimHttpModemEndPointModule::qcril_uim_http_client_transaction_request
(
  std::shared_ptr<Message> msg_ptr
)
{
  qmi_txn_handle                                     txn_handle;
  uint8_t                                            header_index     = 0;
  int                                                qmi_err_code     = -1;
  uim_http_transaction_req_msg_v01                 * qmi_http_req_ptr = nullptr;
  uim_http_transaction_resp_msg_v01                * qmi_http_rsp_ptr = nullptr;
  uim_http_transaction_msg                         * http_req_ptr     = nullptr;
  std::shared_ptr<lpa_qmi_uim_http_rsp_data_type>    resp_data_ptr    = nullptr;

  std::shared_ptr<LpaUimHttpRequestMsg> http_transaction_req_msg_ptr(
    std::static_pointer_cast<LpaUimHttpRequestMsg>(msg_ptr));

  if (http_transaction_req_msg_ptr == nullptr)
  {
    return;
  }

  http_req_ptr = (uim_http_transaction_msg *)http_transaction_req_msg_ptr->get_message();

  if (http_req_ptr == nullptr)
  {
    return;
  }

  /* Allocate QMI HTTP transaction request for the first response payload chunk to be sent */
  qmi_http_req_ptr = new uim_http_transaction_req_msg_v01;
  if (qmi_http_req_ptr == nullptr)
  {
    goto report_error_to_modem;
  }

  if (http_req_ptr->payload_ptr == nullptr && http_req_ptr->payload_len != 0)
  {
     goto report_error_to_modem;
  }


  Log::getInstance().d("Sending payload of length " + std::to_string(http_req_ptr->payload_len));

  /* Filling the qmi_http_req_ptr */
  memset(qmi_http_req_ptr, 0, sizeof(uim_http_transaction_req_msg_v01));

  QCRIL_LOG_INFO("Token: 0x%x, result: %d, payload_len :%d, offset: %d",
    http_req_ptr->token_id, http_req_ptr->result, http_req_ptr->payload_len,
    http_req_ptr->offset);

  qmi_http_req_ptr->token_id = http_req_ptr->token_id;
  qmi_http_req_ptr->result   = (uim_http_result_type_enum_v01)http_req_ptr->result;


  if (http_req_ptr->payload_len > 0)
  {
    qmi_http_req_ptr->segment_info_valid = 0x01;
    qmi_http_req_ptr->payload_body_valid = 0x01;
    qmi_http_req_ptr->segment_info.total_size = http_req_ptr->payload_len;
    qmi_http_req_ptr->segment_info.segment_offset = http_req_ptr->offset;

    if (http_req_ptr->payload_len  > UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01 )
    {
      qmi_http_req_ptr->payload_body_len = UIM_HTTP_PAYLOAD_BODY_CHUNK_MAX_LEN_V01;
    }
    else
    {
      qmi_http_req_ptr->payload_body_len = http_req_ptr->payload_len;
    }
    memcpy(qmi_http_req_ptr->payload_body,
           http_req_ptr->payload_ptr,
           qmi_http_req_ptr->payload_body_len);

  }

  QCRIL_LOG_INFO("payload_len: %d, total: %d, offset: %d",
    qmi_http_req_ptr->payload_body_len, qmi_http_req_ptr->segment_info.total_size,
    qmi_http_req_ptr->segment_info.segment_offset);

  if(http_req_ptr->headers_valid)
  {
    qmi_http_req_ptr->headers_valid = true;

    if (http_req_ptr->headers_len > UIM_HTTP_CUST_HEADER_MAX_COUNT_V01)
    {
      qmi_http_req_ptr->headers_len = UIM_HTTP_CUST_HEADER_MAX_COUNT_V01;
    }
    else
    {
      qmi_http_req_ptr->headers_len = http_req_ptr->headers_len;
    }

    for(header_index = 0; header_index < qmi_http_req_ptr->headers_len &&
                          header_index < UIM_HTTP_CUST_HEADER_MAX_COUNT; header_index++)
    {

      (void)strlcpy(qmi_http_req_ptr->headers[header_index].name,
                    http_req_ptr->headers[header_index].name,
                    UIM_HTTP_HEADER_NAME_MAX_LEN_V01);
      qmi_http_req_ptr->headers[header_index].name[UIM_HTTP_HEADER_NAME_MAX_LEN_V01] = '\0';

      (void)strlcpy(qmi_http_req_ptr->headers[header_index].value,
                    http_req_ptr->headers[header_index].value,
                    UIM_HTTP_HEADER_VALUE_MAX_LEN_V01);
      qmi_http_req_ptr->headers[header_index].value[UIM_HTTP_HEADER_VALUE_MAX_LEN_V01] = '\0';
    }
  }

  /* Allocate response pointer since it is an async command */
  qmi_http_rsp_ptr = new uim_http_transaction_resp_msg_v01;
  if (qmi_http_rsp_ptr == nullptr)
  {
    goto report_error_to_modem;
  }

  memset(qmi_http_rsp_ptr, 0x00, sizeof(uim_http_transaction_resp_msg_v01));

  http_transaction_req_ptr = http_transaction_req_msg_ptr;

  qmi_err_code = qmi_client_send_msg_async(
                 mQmiSvcClient,
                 QMI_UIM_HTTP_TRANSACTION_REQ_V01,
                 (void *) qmi_http_req_ptr,
                 sizeof(uim_http_transaction_req_msg_v01),
                 (void *) qmi_http_rsp_ptr,
                 sizeof(uim_http_transaction_resp_msg_v01),
                 qmi_uim_http_request_cb,
                 (void *)this,
                 &txn_handle);

  /* On successful API call, free only the request */
  if (qmi_err_code != 0)
  {
    goto report_error;
  }

  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  return;

report_error_to_modem:
  qcril_uim_http_send_error_response(http_req_ptr->token_id);

report_error:
  resp_data_ptr = std::shared_ptr<lpa_qmi_uim_http_rsp_data_type>(new lpa_qmi_uim_http_rsp_data_type{});

  if (resp_data_ptr != nullptr)
  {
    resp_data_ptr->qmi_error_code = -1;
    resp_data_ptr->rsp_id = LPA_QMI_UIM_HTTP_TRANSACTION_RSP;
    resp_data_ptr->transp_err = -1;
  }

  /* In any case of error, check & free all the allocated pointers */
  http_transaction_req_msg_ptr->sendResponse((std::shared_ptr<Message>)http_transaction_req_msg_ptr,
                                             Message::Callback::Status::SUCCESS,
                                             resp_data_ptr);
  http_transaction_req_ptr = nullptr;
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_req_ptr);
  QCRIL_QMI_UIM_HTTP_FREE_PTR(qmi_http_rsp_ptr);
} /* qcril_uim_http_client_transaction_request */

#ifdef QMI_RIL_UTF
void UimHttpModemEndPointModule::cleanUpQmiSvcClient()
{
    if(nullptr != mQmiSvcClient)
    {
        qmi_client_release(mQmiSvcClient);
        mQmiSvcClient = nullptr;
        mServiceObject = nullptr;
    }
}
#endif
