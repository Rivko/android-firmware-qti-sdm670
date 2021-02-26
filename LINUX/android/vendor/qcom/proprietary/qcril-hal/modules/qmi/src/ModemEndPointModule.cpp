/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include <cstring>
#include "modules/qmi/ModemEndPointModule.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"
#include "modules/qmi/QmiIndMessage.h"
#include "modules/qmi/QmiSetupRequestCallback.h"
#include "modules/qmi/EndpointStatusIndMessage.h"
#include "modules/qmi/StackIdIndMessage.h"
#include "modules/qmi/QmiAsyncResponseMessage.h"

#define MESSAGE_HANDLER(MSG_TYPE, FUNC) ([this](auto msg) { Module::handler<MSG_TYPE>(std::bind(&FUNC, this, std::placeholders::_1), msg); })

static void qmiUnsolIndCb(qmi_client_type user_handle, unsigned int msg_id,
                          void *ind_buf, unsigned int ind_buf_len,
                          void *ind_cb_data) {
  Log::getInstance().d("[ModemEndPointModule]: qmiUnsolIndCb");
  if (ind_cb_data != nullptr) {
    ModemEndPointModule* pModule = static_cast<ModemEndPointModule*>(ind_cb_data);
    pModule->onQmiUnsolIndCb(user_handle, msg_id, ind_buf, ind_buf_len);
  } else {
    Log::getInstance().d("Unexpected: no right CB data for QMI ind");
  }
}

static void qmiNotifyCb(qmi_client_type user_handle,
                        qmi_idl_service_object_type service_obj,
                        qmi_client_notify_event_type service_event,
                        void *notify_cb_data) {
  Log::getInstance().d("[ModemEndPointModule]: qmiNotifyCb");
  if (notify_cb_data != nullptr) {
    ModemEndPointModule *pModule = static_cast<ModemEndPointModule*>(notify_cb_data);
    pModule->onQmiNotifyCb(user_handle, service_obj, service_event);
  } else {
    Log::getInstance().d("Unexpected: no right CB data for notification");
  }
}

static void qmiNotifyErrorCb(qmi_client_type user_handle,
                             qmi_client_error_type error,
                             void* error_cb_data) {
  (void)user_handle;

  Log::getInstance().d("[ModemEndPointModule]: qmiNotifyErrorCb");

  if (error_cb_data != nullptr) {
    ModemEndPointModule *pModule = static_cast<ModemEndPointModule*>(error_cb_data);
    Log::getInstance().d("qmiNotifyErrorCb: module : " + pModule->to_string());
    auto shared_srvDownMsg = std::make_shared<QmiServiceDownIndMessage>(error);
    pModule->dispatch(shared_srvDownMsg);
  }
}

static void qmiAsyncCb(qmi_client_type user_handle, unsigned int msg_id,
                          void *resp_buf, unsigned int resp_buf_len,
                          void *resp_cb_data, qmi_client_error_type transp_err) {
  (void)user_handle;
  (void)msg_id;
  (void)resp_buf;
  Log::getInstance().d("[ModemEndPointModule]: qmiAsyncCb");

  if (resp_cb_data != nullptr) {
    QmiAsyncCbDataStruct* asyncCbData = static_cast<QmiAsyncCbDataStruct*>(resp_cb_data);
    if (asyncCbData->pModule != nullptr) {
      asyncCbData->pModule->onQmiAsyncCb(user_handle, msg_id, resp_buf, resp_buf_len,
          resp_cb_data, transp_err);
    }
  } else {
    Log::getInstance().d("Unexpected: no right CB data for QMI Async callback");
  }
}

ModemEndPointModule::ModemEndPointModule(string name,
                                         ModemEndPoint &owner):
    mOwner(owner),
    mIndMsgId(REG_MSG(owner.mName + "_QMI_IND")),
    mEndPtStatusIndId(REG_MSG(owner.mName + "_ENDPOINT_STATUS_IND"))
{
  mName = name;
  mOwner = owner;
  mServiceObject = nullptr;
  mQmiSvcClient = nullptr;
  mQmiNotifyHandle = nullptr;
}

ModemEndPointModule::~ModemEndPointModule() {
}

void ModemEndPointModule::onQmiAsyncCb(qmi_client_type user_handle, unsigned int msg_id,
                          void *resp_buf, unsigned int resp_buf_len,
                          void *resp_cb_data, qmi_client_error_type transp_err) {
  (void)user_handle;
  (void)msg_id;
  (void)resp_buf;
  Log::getInstance().d("[" + mName + "]: onQmiAsyncCb");

  if (resp_cb_data != nullptr) {
    std::shared_ptr<QmiAsyncCbDataStruct> asyncCbData(static_cast<QmiAsyncCbDataStruct*>(resp_cb_data));
    std::shared_ptr<QmiSendRawAsyncMessage> shared_qmiMsg(
        std::static_pointer_cast<QmiSendRawAsyncMessage>(asyncCbData->msg));
    Module *clientModule = shared_qmiMsg->getClientModule();
    if (clientModule != nullptr) {
      auto shared_req = shared_qmiMsg->getData();
      QmiAsyncRespData data = {};
      data.msgId = shared_req->msgId;
      data.respData = asyncCbData->respData;
      data.respDataSize = resp_buf_len;
      data.traspErr = transp_err;
      data.cb = shared_req->cb;
      data.cbData = shared_req->cbData;
      auto shared_msg = std::make_shared<QmiAsyncResponseMessage>(&data);
      clientModule->dispatch(shared_msg);
    } else {
      Log::getInstance().d("[" + mName + "]: clientModule is null!!");
    }
  } else {
    Log::getInstance().d("Unexpected: no right CB data for QMI Async callback");
  }
}

void ModemEndPointModule::onQmiUnsolIndCb(qmi_client_type user_handle, unsigned int msg_id,
                                     void* ind_buf, unsigned int ind_buf_len) {
  uint32_t decoded_payload_len = 0;
  unsigned char* decoded_payload = nullptr;
  qmi_client_error_type qmi_err = QMI_CLIENT_PARAM_ERR;

  (void)user_handle;

  Log::getInstance().d("[" + mName + "]: indication");

  // decode QMI payload
  qmi_idl_get_message_c_struct_len(
                    getServiceObject(),
                    QMI_IDL_INDICATION,
                    msg_id,
                    &decoded_payload_len);
  if (decoded_payload_len) {
    decoded_payload = new unsigned char[decoded_payload_len];
    if (decoded_payload != nullptr) {
      qmi_err = qmi_client_message_decode(mQmiSvcClient, QMI_IDL_INDICATION,
                        msg_id, ind_buf, ind_buf_len, decoded_payload,
                        (int)decoded_payload_len);
    }
  }

  if (((decoded_payload != nullptr) && (qmi_err == QMI_NO_ERR))
        || (decoded_payload_len == 0)) {
    QmiIndMsgDataStruct data;
    data.msgId = msg_id;
    data.indData = decoded_payload;
    data.indSize = decoded_payload_len;
    data.cbData = (void*)this;
    // generate the message id
    auto shared_msg = std::make_shared<QmiIndMessage>(&data, mIndMsgId);
    Dispatcher::getInstance().dispatch(shared_msg);
  }

  if (decoded_payload != nullptr)
    delete []decoded_payload;
}

void ModemEndPointModule::onQmiNotifyCb(
    qmi_client_type user_handle, qmi_idl_service_object_type service_obj,
    qmi_client_notify_event_type service_event) {
  (void)user_handle;
  (void)service_obj;
  Log::getInstance().d("[" + mName + "]: onQmiNotifyCb()");

  switch (service_event) {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      Log::getInstance().d("[" + mName + "] : onQmiNotifyCb() Service up indication");
      this->dispatch(std::make_shared<QmiServiceUpIndMessage>(nullptr));
      break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
      Log::getInstance().d("[" + mName + "] : onQmiNotifyCb() Service down indication");
      break;
    default:
      Log::getInstance().d("[" + mName + "] : onQmiNotifyCb() unsupported service event: " +
          std::to_string(service_event));
      break;
  }
  return;
}


void ModemEndPointModule::init() {
  using std::placeholders::_1;
  // QMI setup related:  derived classes could implement their own version
  if (mMessageHandler.find(QmiSetupRequest::get_class_message_id()) == mMessageHandler.end())
    mMessageHandler[QmiSetupRequest::get_class_message_id()] =
                MESSAGE_HANDLER(QmiSetupRequest, ModemEndPointModule::handleQmiClientSetup);
  if (mMessageHandler.find(QmiServiceUpIndMessage::get_class_message_id()) == mMessageHandler.end())
    mMessageHandler[QmiServiceUpIndMessage::get_class_message_id()] =
                MESSAGE_HANDLER(QmiServiceUpIndMessage, ModemEndPointModule::handleQmiServiceUp);
  if (mMessageHandler.find(QmiServiceDownIndMessage::get_class_message_id()) == mMessageHandler.end())
    mMessageHandler[QmiServiceDownIndMessage::get_class_message_id()] =
                MESSAGE_HANDLER(QmiServiceDownIndMessage, ModemEndPointModule::handleQmiServiceDown);
  // QMI send related: will ignore if derived classes also implement
  mMessageHandler[QmiSendRawSyncMessage::get_class_message_id()] =
                MESSAGE_HANDLER(QmiSendRawSyncMessage, ModemEndPointModule::handleQmiRawSendSync);
  mMessageHandler[QmiSendRawAsyncMessage::get_class_message_id()] =
                MESSAGE_HANDLER(QmiSendRawAsyncMessage, ModemEndPointModule::handleQmiRawSendAsync);
  mMessageHandler[StackIdIndMessage::get_class_message_id()] =
                MESSAGE_HANDLER(StackIdIndMessage, ModemEndPointModule::handleStackIdIndMessage);

  // Initialize the stack id
  memset(mStackId, -1, sizeof(mStackId));

  // register handlers
  Module::init();
}

void ModemEndPointModule::handleStackIdIndMessage(
    std::shared_ptr<StackIdIndMessage> stackIndMsg)
{
  qcril_instance_id_e_type instanceId = stackIndMsg->getInstanceId();
  int8_t stackId = stackIndMsg->getStackId();

  if (instanceId >= QCRIL_DEFAULT_INSTANCE_ID &&
        instanceId < QCRIL_MAX_INSTANCE_ID && stackId != -1) {
    bool failed = false;
    // if QMI client is ready, do the QMI binding
    // otherwise after QMI client gets ready, it will also do the QMI binding
    if (mQmiSvcClient)
      failed = handleQmiBinding(instanceId, stackId);
    if (!failed)
      mStackId[instanceId] = stackIndMsg->getStackId();
  }
}

// Those which need to handle the QMI bindings, must overwrite this function
bool ModemEndPointModule::handleQmiBinding(
    qcril_instance_id_e_type instanceId, int8_t stackId) {

    (void)instanceId;
    (void)stackId;

    Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName +
        " Non-ops for QMI Binding");
    return true;
}

void ModemEndPointModule::handleQmiClientSetup(
    std::shared_ptr<QmiSetupRequest> shared_setupMsg) {

  Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName + " : handleQmiClientSetup()");

  // get the service obj if needed
  if (mServiceObject == nullptr) {
    mServiceObject = getServiceObject();
    if (mServiceObject == nullptr) {
      Log::getInstance().d(
          "[ModemEndPointModule]:" + mOwner.mName + " : Did not get get_service_object");
      return;
    } else {
      Log::getInstance().d(
          "[ModemEndPointModule]:" + mOwner.mName + " : Got get_service_object");
    }
  }

  // initialize the QMI client
  qmi_client_type qmi_svc_client;
  qmi_client_error_type client_err = qmi_client_init_instance(
    mServiceObject, QMI_CLIENT_INSTANCE_ANY, (qmi_client_ind_cb)qmiUnsolIndCb,
    this, &mOsParams, 4, &qmi_svc_client);
  // send response immediately
  shared_setupMsg->sendResponse(shared_setupMsg,
      (client_err == QMI_NO_ERR) ? Message::Callback::Status::SUCCESS
      : Message::Callback::Status::FAILURE, nullptr);
  if (client_err != QMI_NO_ERR) {
    Log::getInstance().d("[ModemEndPointModule]: Error in client init:" + mOwner.mName);
    mQmiSvcClient = nullptr;
    // register for UP event in case of client init failure
    if (mQmiNotifyHandle) {
      Log::getInstance().d("[ModemEndPointModule]: Relase QMI notify handle for"
            + mOwner.mName);
      qmi_client_release(mQmiNotifyHandle);
      mQmiNotifyHandle = nullptr;
    }
    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);
    if (rc == QMI_NO_ERR) {
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmiNotifyCb, this);

      Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName +
          " : qmi_client_register_notify_cb return : " + std::to_string(rc));

      if (rc != QMI_NO_ERR) {
        Log::getInstance().d(
            "[ModemEndPointModule]: " + mOwner.mName + " : qmi_client_register_notify_cb failed: " +
            std::to_string(rc));
      }
    } else {
      Log::getInstance().d(
        "[ModemEndPointModule]: " + mOwner.mName + " : qmi_client_notifier_init failed: " +
        std::to_string(rc));
    }
  } else {
    Log::getInstance().d("[ModemEndPointModule]: No Error in client init : " + mOwner.mName);
    mQmiSvcClient = qmi_svc_client;
    // for single SIM, the stackId is always -1, since no StackIdIndMessage received
    qcril_instance_id_e_type instanceId = qmi_ril_get_process_instance_id();
    if (instanceId >= QCRIL_DEFAULT_INSTANCE_ID &&
        instanceId < QCRIL_MAX_INSTANCE_ID) {
      if (mStackId[instanceId] != -1)
        (void)handleQmiBinding(instanceId, mStackId[instanceId]);
    }
    mOwner.setState(ModemEndPoint::State::OPERATIONAL);
    // Send ModemEndPoint state changed
    auto shared_msg = std::make_shared<EndpointStatusIndMessage>
                        (ModemEndPoint::State::OPERATIONAL, mEndPtStatusIndId);
    Dispatcher::getInstance().dispatch(shared_msg);
    // Register for service_down event in case of client init success
    client_err = qmi_client_register_error_cb(
                        mQmiSvcClient, qmiNotifyErrorCb, this);
  }
}

void ModemEndPointModule::handleQmiServiceUp(std::shared_ptr<QmiServiceUpIndMessage> msg) {
  (void)msg;
  Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName + " : handleQmiServiceUp()");

  if (mOwner.getState() != ModemEndPoint::State::OPERATIONAL) {
     qmi_client_error_type rc;
     qmi_service_info info;
     rc = qmi_client_get_service_instance(mServiceObject,
         QMI_CLIENT_INSTANCE_ANY, &info);
     Log::getInstance().d(
         "[ModemEndPointModule]: " + mOwner.mName + " : qmi_client_get_service_instance returns : "
         + std::to_string(rc));
     if (rc == QMI_NO_ERR) {
       QmiSetupRequestCallback cb("Srv-up-token");
       auto shared_setupMsg = std::make_shared<QmiSetupRequest>(
            "Srv-up-token", 0, nullptr, &cb);
       this->dispatch(shared_setupMsg);
     }
  }
}

void ModemEndPointModule::handleQmiServiceDown(
    std::shared_ptr<QmiServiceDownIndMessage> shared_srvDownMsg) {

  qmi_client_error_type error = shared_srvDownMsg->getErrorCode();

  Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName
      + " Service is down. Error code is " + std::to_string(error));

  if (mOwner.getState() == ModemEndPoint::State::OPERATIONAL) {
    mOwner.setState(ModemEndPoint::State::NON_OPERATIONAL);
    // Send ModemEndPoint state changed
    auto shared_msg = std::make_shared<EndpointStatusIndMessage>
                        (ModemEndPoint::State::NON_OPERATIONAL, mEndPtStatusIndId);
    Dispatcher::getInstance().dispatch(shared_msg);

    // release client
    if (mQmiSvcClient) {
      qmi_client_error_type rc = qmi_client_release(mQmiSvcClient);
      if (rc != QMI_NO_ERR) {
        Log::getInstance().d("[ModemEndPointModule]: "+ mOwner.mName +" :Failed to release client");
      } else {
        mQmiSvcClient = nullptr;
        Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName + " : Release client");
      }
    } else {
      Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName + " : unexpected for client");
    }

    // register for up event
    qmi_client_error_type rc = QMI_NO_ERR;
    if (!mQmiNotifyHandle) {
      rc = qmi_client_notifier_init(mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);
      Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName +
          " : qmi_client_notifier_init return" + std::to_string(rc));
    }

    if (!rc) {
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmiNotifyCb, this);
      Log::getInstance().d("[ModemEndPointModule]: " + mOwner.mName +
          " : qmi_client_register_notify_cb return: " + std::to_string(rc));
    }

    if (rc) {
      //TODO: if there is no possbile to register service up event, then try to
      // delay the setup request
    }
  }
}

void ModemEndPointModule::handleQmiRawSendSync(
    std::shared_ptr<QmiSendRawSyncMessage> qmiSendRawSyncMsg) {

    QmiSyncStruct* qmiInfo = qmiSendRawSyncMsg->getData();

    if ((mQmiSvcClient == nullptr) || (qmiInfo == nullptr)) {
        Log::getInstance().d("[" + mName + " : null pointer unexpected");
        qmiSendRawSyncMsg->sendResponse(qmiSendRawSyncMsg, Message::Callback::Status::FAILURE,
            std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{
            QMI_CLIENT_PARAM_ERR}));
        return;
    }

    Log::getInstance().d("[" + mName + " : handleQmiRawSendSync msgId = " +
        std::to_string(qmiInfo->msgId));

    // call QMI framework
    qmi_client_error_type rc = qmi_client_send_msg_sync(
                                            mQmiSvcClient,
                                            qmiInfo->msgId,
                                            qmiInfo->reqData,
                                            qmiInfo->reqSize,
                                            qmiInfo->respData,
                                            qmiInfo->respSize,
                                            qmiInfo->timerValue);
    Log::getInstance().d("[" + mName +
        " : qmi_client_send_msg_sync returns : " + std::to_string(rc));
    qmiSendRawSyncMsg->sendResponse(qmiSendRawSyncMsg, Message::Callback::Status::FAILURE,
        std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{rc}));
}

void ModemEndPointModule::handleQmiRawSendAsync(
    std::shared_ptr<QmiSendRawAsyncMessage> qmiSendRawAsyncMsg) {

    std::shared_ptr<QmiReqAsyncStruct> qmiReq = qmiSendRawAsyncMsg->getData();

    if ((mQmiSvcClient == nullptr) || (qmiReq == nullptr)) {
        Log::getInstance().d("[" + mName + " : null pointer unexpected");
        qmiSendRawAsyncMsg->sendResponse(qmiSendRawAsyncMsg, Message::Callback::Status::FAILURE,
            std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{
            QMI_CLIENT_PARAM_ERR}));
        return;
    }

    Log::getInstance().d("[" + mName + "]: handleQmiRawSendAsync: msgId = " +
        std::to_string(qmiReq->msgId));

    // allocate shared resp buf for legacy API
    // XXX: how to pass deleter for make_shared?
    unsigned char* resp = new unsigned char[qmiReq->respSize];
    if (resp == nullptr) {
        Log::getInstance().d("[" + mName + "]: resp: ALLOC FAILURE");
        qmiSendRawAsyncMsg->sendResponse(qmiSendRawAsyncMsg, Message::Callback::Status::FAILURE,
            std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{
            QMI_CLIENT_ALLOC_FAILURE}));
        return;
    }
    std::shared_ptr<unsigned char> shared_respData(resp, std::default_delete<unsigned char[]>());

    // allocate cb data
    QmiAsyncCbDataStruct *cbData = new QmiAsyncCbDataStruct;
    if (cbData == nullptr) {
        Log::getInstance().d("[" + mName + "]: cbData: ALLOC FAILURE");
        qmiSendRawAsyncMsg->sendResponse(qmiSendRawAsyncMsg, Message::Callback::Status::FAILURE,
            std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{
            QMI_CLIENT_ALLOC_FAILURE}));
        return;
    }
    cbData->msg = qmiSendRawAsyncMsg;
    cbData->pModule = this;
    // keep reference for resp buffer
    cbData->respData = shared_respData;

    // call legacy QMI async framework
    qmi_txn_handle txn_handle;
    qmi_client_error_type rc = qmi_client_send_msg_async(
                                            mQmiSvcClient,
                                            qmiReq->msgId,
                                            qmiReq->reqData,
                                            qmiReq->reqSize,
                                            resp,
                                            qmiReq->respSize,
                                            qmiAsyncCb,
                                            cbData,
                                            &txn_handle);

    Log::getInstance().d("[" + mName + "]: qmi_client_send_msg_async returns : " +
         std::to_string(rc));

    qmiSendRawAsyncMsg->sendResponse(qmiSendRawAsyncMsg, Message::Callback::Status::SUCCESS,
        std::shared_ptr<qmi_client_error_type>(new qmi_client_error_type{rc}));

    if (rc != QMI_NO_ERR) {
      delete cbData;
    }
}

#ifdef QMI_RIL_UTF
void ModemEndPointModule::cleanUpQmiSvcClient()
{
    if(nullptr != mQmiSvcClient)
    {
        qmi_client_release(mQmiSvcClient);
        mQmiSvcClient = nullptr;
    }
}
#endif
