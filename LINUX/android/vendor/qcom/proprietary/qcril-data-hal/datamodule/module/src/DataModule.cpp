/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include <iostream>
#include <cstdlib>
#include <pthread.h>

#include "qmi_client.h"
#include "wireless_data_service_v01.h"

/* Framework includes */
#include "framework/Dispatcher.h"
#include "framework/Looper.h"
#include "framework/ModuleLooper.h"
#include "framework/QcrilInitMessage.h"
#include "modules/android/ril_message_factory.h"

/* Module includes */
#include "local/DataModule.h"
#include "UnSolMessages/DataInitMessage.h"
#include "request/RilRequestSetupDataCallMessage.h"

#include "request/RilRequestDataCallListMessage.h"
#include "request/RilRequestGetDataCallProfileMessage.h"
#include "request/RilRequestDeactivateDataCallMessage.h"
#include "request/RilRequestEmbmsActivateDeactivateTmgiMessage.h"
#include "request/RilRequestEmbmsActivateTmgiMessage.h"
#include "request/RilRequestEmbmsContentDescUpdateMessage.h"
#include "request/RilRequestEmbmsDeactivateTmgiMessage.h"
#include "request/EmbmsEnableDataReqMessage.h"
#include "request/RilRequestEmbmsGetActiveTmgiMessage.h"
#include "request/RilRequestEmbmsGetAvailTmgiMessage.h"
#include "request/RilRequestEmbmsSendIntTmgiListMessage.h"
#include "request/RilRequestGoDormantMessage.h"
#include "request/RilRequestLastDataCallFailCauseMessage.h"
#include "request/ProcessScreenStateChangeMessage.h"
#include "request/ProcessStackSwitchMessage.h"
#include "request/RilRequestPullLceDataMessage.h"
#include "request/SetApnInfoMessage.h"
#include "request/RilRequestSetDataProfileMessage.h"
#include "request/SetIsDataEnabledMessage.h"
#include "request/SetIsDataRoamingEnabledMessage.h"
#include "request/SetLteAttachProfileMessage.h"
#include "request/SetQualityMeasurementMessage.h"
#include "request/SetRatPrefMessage.h"
#include "request/RilRequestStartLceMessage.h"
#include "request/RilRequestStopLceMessage.h"
#include "request/ToggleDormancyIndMessage.h"
#include "request/ToggleLimitedSysIndMessage.h"
#include "request/UpdateMtuMessage.h"
#include "request/GetDdsSubIdMessage.h"
#include "request/RequestDdsSwitchMessage.h"
#include "request/RilRequestStartKeepaliveMessage.h"
#include "request/RilRequestStopKeepaliveMessage.h"

#include "sync/RilDataEmbmsActiveMessage.h"

#include "legacy/qcril_data_qmi_wds.h"
#include "legacy/qcril_data.h"

#include "modules/nas/NasRequestDataShutdownMessage.h"

static load_module<rildata::DataModule> dataModule;

namespace rildata {

DataModule::DataModule() {
  mName = "DataModule";
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;
  mMessageHandler = {
                      {QcrilInitMessage::get_class_message_id(), std::bind(&DataModule::handleQcrilInitMessage, this, _1)},
                      {RilRequestDataCallListMessage::get_class_message_id(), std::bind(&DataModule::handleDataCallListMessage, this, _1)},
                      {RilRequestGetDataCallProfileMessage::get_class_message_id(), std::bind(&DataModule::handleGetDataCallProfileMessage, this, _1)},
                      {RilRequestDeactivateDataCallMessage::get_class_message_id(), std::bind(&DataModule::handleDeactivateDataCallMessage, this, _1)},
                      {RilRequestEmbmsActivateDeactivateTmgiMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsActivateDeactivateTmgiMessage, this, _1)},
                      {RilRequestEmbmsActivateTmgiMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsActivateTmgiMessage, this, _1)},
                      {RilRequestEmbmsContentDescUpdateMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsContentDescUpdateMessage, this, _1)},
                      {RilRequestEmbmsDeactivateTmgiMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsDeactivateTmgiMessage, this, _1)},
                      {EmbmsEnableDataReqMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsEnableDataReqMessage, this, _1)},
                      {RilRequestEmbmsGetActiveTmgiMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsGetActiveTmgiMessage, this, _1)},
                      {RilRequestEmbmsGetAvailTmgiMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsGetAvailTmgiMessage, this, _1)},
                      {RilRequestEmbmsSendIntTmgiListMessage::get_class_message_id(), std::bind(&DataModule::handleEmbmsSendIntTmgiListMessage, this, _1)},
                      {RilRequestGoDormantMessage::get_class_message_id(), std::bind(&DataModule::handleGoDormantMessage, this, _1)},
                      {RilRequestLastDataCallFailCauseMessage::get_class_message_id(), std::bind(&DataModule::handleLastDataCallFailCauseMessage, this, _1)},
                      {ProcessScreenStateChangeMessage::get_class_message_id(), std::bind(&DataModule::handleProcessScreenStateChangeMessage, this, _1)},
                      {ProcessStackSwitchMessage::get_class_message_id(), std::bind(&DataModule::handleProcessStackSwitchMessage, this, _1)},
                      {RilRequestPullLceDataMessage::get_class_message_id(), std::bind(&DataModule::handlePullLceDataMessage, this, _1)},
                      {SetApnInfoMessage::get_class_message_id(), std::bind(&DataModule::handleSetApnInfoMessage, this, _1)},
                      {SetIsDataEnabledMessage::get_class_message_id(), std::bind(&DataModule::handleSetIsDataEnabledMessage, this, _1)},
                      {SetIsDataRoamingEnabledMessage::get_class_message_id(), std::bind(&DataModule::handleSetIsDataRoamingEnabledMessage, this, _1)},
                      {SetQualityMeasurementMessage::get_class_message_id(), std::bind(&DataModule::handleSetQualityMeasurementMessage, this, _1)},
                      {SetRatPrefMessage::get_class_message_id(), std::bind(&DataModule::handleSetRatPrefMessage, this, _1)},
                      {RilRequestSetupDataCallMessage::get_class_message_id(), std::bind(&DataModule::handleSetupDataCallMessage, this, _1)},
                      {RilRequestStartLceMessage::get_class_message_id(), std::bind(&DataModule::handleStartLceMessage, this, _1)},
                      {RilRequestStopLceMessage::get_class_message_id(), std::bind(&DataModule::handleStopLceMessage, this, _1)},
                      {ToggleDormancyIndMessage::get_class_message_id(), std::bind(&DataModule::handleToggleDormancyIndMessage, this, _1)},
                      {ToggleLimitedSysIndMessage::get_class_message_id(), std::bind(&DataModule::handleToggleLimitedSysIndMessage, this, _1)},
                      {UpdateMtuMessage::get_class_message_id(), std::bind(&DataModule::handleUpdateMtuMessage, this, _1)},
                      {RilDataEmbmsActiveMessage::get_class_message_id(), std::bind(&DataModule::handleDataEmbmsActiveMessage, this, _1)},
                      {GetDdsSubIdMessage::get_class_message_id(), std::bind(&DataModule::handleGetDdsSubIdMessage, this, _1)},
                      {RequestDdsSwitchMessage::get_class_message_id(), std::bind(&DataModule::handleDataRequestDDSSwitchMessage, this, _1)},
#if (QCRIL_RIL_VERSION >= 15)
                      {SetLteAttachProfileMessage_v15::get_class_message_id(), std::bind(&DataModule::handleSetLteAttachProfileMessage_v15, this, _1)},
                      {RilRequestSetDataProfileMessage_v15::get_class_message_id(), std::bind(&DataModule::handleSetDataProfileMessage_v15, this, _1)},
                      {RilRequestStartKeepaliveMessage::get_class_message_id(), std::bind(&DataModule::handleStartKeepaliveMessage, this, _1)},
                      {RilRequestStopKeepaliveMessage::get_class_message_id(), std::bind(&DataModule::handleStopKeepaliveMessage, this, _1)},
#else
                      {SetLteAttachProfileMessage::get_class_message_id(), std::bind(&DataModule::handleSetLteAttachProfileMessage, this, _1)},
                      {RilRequestSetDataProfileMessage::get_class_message_id(), std::bind(&DataModule::handleSetDataProfileMessage, this, _1)},

#endif /* QCRIL_RIL_VERSION >= 15 */
                      {NasRequestDataShutdownMessage::get_class_message_id(), std::bind(&DataModule::handleNasRequestDataShutdown, this, _1)},
                    };
}

DataModule::~DataModule() {
  mLooper = nullptr;
  //mDsdEndPoint = nullptr;
}

void DataModule::init() {
  /* Call base init before doing any other stuff.*/
  Module::init();
}

void DataModule::broadcastReady()
{
    std::shared_ptr<DataInitMessage> data_init_msg =
                       std::make_shared<DataInitMessage>(global_instance_id);
    Dispatcher::getInstance().broadcast(data_init_msg);
}

void DataModule::handleQcrilInitMessage(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());

  auto qcril_init_msg = std::static_pointer_cast<QcrilInitMessage>(msg);

  if( qcril_init_msg != NULL ) {
    global_instance_id = qcril_init_msg->get_instance_id();
    qcril_data_init();

    broadcastReady();

    Log::getInstance().d("[" + mName + "]: Done msg = " + msg->dump());
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received. =" + msg->dump() + "QCRIL DATA Init not triggered!!!");
  }
}

void DataModule::handleDataCallListMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  qcril_request_return_type ret;
  auto m = std::static_pointer_cast<RilRequestDataCallListMessage>(msg);
  if( m != NULL ) {
    qcril_data_request_data_call_list(&(m->get_params()), &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGetDataCallProfileMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestGetDataCallProfileMessage> m = std::static_pointer_cast<RilRequestGetDataCallProfileMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_omh_profile_info(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleDeactivateDataCallMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestDeactivateDataCallMessage> m = std::static_pointer_cast<RilRequestDeactivateDataCallMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_deactivate_data_call(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsActivateDeactivateTmgiMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsActivateDeactivateTmgiMessage> m = std::static_pointer_cast<RilRequestEmbmsActivateDeactivateTmgiMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_activate_deactivate_tmgi(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsActivateTmgiMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsActivateTmgiMessage> m = std::static_pointer_cast<RilRequestEmbmsActivateTmgiMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_activate_tmgi(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsContentDescUpdateMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsContentDescUpdateMessage> m = std::static_pointer_cast<RilRequestEmbmsContentDescUpdateMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_content_desc_update(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsDeactivateTmgiMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsDeactivateTmgiMessage> m = std::static_pointer_cast<RilRequestEmbmsDeactivateTmgiMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_deactivate_tmgi(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsEnableDataReqMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<EmbmsEnableDataReqMessage> m = std::static_pointer_cast<EmbmsEnableDataReqMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_enable_data_req(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsGetActiveTmgiMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsGetActiveTmgiMessage> m = std::static_pointer_cast<RilRequestEmbmsGetActiveTmgiMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_get_active_tmgi(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsGetAvailTmgiMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsGetAvailTmgiMessage> m = std::static_pointer_cast<RilRequestEmbmsGetAvailTmgiMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_get_available_tmgi(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleEmbmsSendIntTmgiListMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestEmbmsSendIntTmgiListMessage> m = std::static_pointer_cast<RilRequestEmbmsSendIntTmgiListMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_embms_send_interested_list(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGoDormantMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestGoDormantMessage> m = std::static_pointer_cast<RilRequestGoDormantMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_process_qcrilhook_go_dormant(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleLastDataCallFailCauseMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestLastDataCallFailCauseMessage> m = std::static_pointer_cast<RilRequestLastDataCallFailCauseMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_last_data_call_fail_cause(&req,&ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleProcessScreenStateChangeMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ProcessScreenStateChangeMessage> m = std::static_pointer_cast<ProcessScreenStateChangeMessage>(msg);
  if( m != NULL ) {
    int ret = qcril_data_process_screen_state_change(m->screenState);
    Message::Callback::Status status = (ret == QCRIL_DS_SUCCESS ?
           Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleProcessStackSwitchMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ProcessStackSwitchMessage> m = std::static_pointer_cast<ProcessStackSwitchMessage>(msg);
  if( m != NULL ) {
    ProcessStackSwitchMessage::StackSwitchReq info = m->getParams();
    qcril_data_process_stack_switch( info.oldStackId, info.newStackId, info.instanceId);
    auto resp = std::make_shared<int>(QCRIL_DS_SUCCESS);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handlePullLceDataMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestPullLceDataMessage> m = std::static_pointer_cast<RilRequestPullLceDataMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_lqe_get_info(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetApnInfoMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetApnInfoMessage> m = std::static_pointer_cast<SetApnInfoMessage>(msg);
  if( m != NULL ) {
    RIL_Errno ret = qcril_data_set_apn_info(&m->mParams, m->mApnType, m->mApnName, m->mIsApnValid);
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetIsDataEnabledMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetIsDataEnabledMessage> m = std::static_pointer_cast<SetIsDataEnabledMessage>(msg);
  if ( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    RIL_Errno ret = qcril_data_set_is_data_enabled( &req, m->is_data_enabled);
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetIsDataRoamingEnabledMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetIsDataRoamingEnabledMessage> m = std::static_pointer_cast<SetIsDataRoamingEnabledMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    RIL_Errno ret = qcril_data_set_is_data_roaming_enabled(&req, m->is_data_roaming_enabled);
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

#if (QCRIL_RIL_VERSION >= 15)
void DataModule::handleSetLteAttachProfileMessage_v15(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetLteAttachProfileMessage_v15> reqMsg = std::static_pointer_cast<SetLteAttachProfileMessage_v15>(msg);
  if( reqMsg != NULL ) {
    RIL_Errno ret = qcril_data_request_set_lte_attach_profile_v15 ( reqMsg->get_params() );
    auto resp = std::make_shared<RIL_Errno>(ret);
    reqMsg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetDataProfileMessage_v15(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestSetDataProfileMessage_v15> m = std::static_pointer_cast<RilRequestSetDataProfileMessage_v15>(msg);
  if ( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_set_data_profile(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}
#else
void DataModule::handleSetLteAttachProfileMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetLteAttachProfileMessage> reqMsg = std::static_pointer_cast<SetLteAttachProfileMessage>(msg);
  if( reqMsg != NULL ) {
    RIL_Errno ret = qcril_data_request_set_lte_attach_profile ( reqMsg->get_params() );
    auto resp = std::make_shared<RIL_Errno>(ret);
    reqMsg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetDataProfileMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestSetDataProfileMessage> m = std::static_pointer_cast<RilRequestSetDataProfileMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_set_data_profile(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}
#endif /* #if QCRIL_RIL_VERSION >= 15 */

void DataModule::handleSetQualityMeasurementMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetQualityMeasurementMessage> reqMsg = std::static_pointer_cast<SetQualityMeasurementMessage>(msg);
  if( reqMsg != NULL ) {
    dsd_set_quality_measurement_info_req_msg_v01 info = reqMsg->getInfo();
    qmi_response_type_v01 ret= qcril_data_set_quality_measurement(&info);
    auto resp = std::make_shared<qmi_response_type_v01>(ret);
    reqMsg->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetRatPrefMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<SetRatPrefMessage> m = std::static_pointer_cast<SetRatPrefMessage>(msg);
  if( m != NULL ) {
    RIL_Errno ret = qcril_data_set_rat_preference(m->ratPrefType);
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleSetupDataCallMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<RilRequestSetupDataCallMessage> m = std::static_pointer_cast<RilRequestSetupDataCallMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_request_setup_data_call(&req, &ret);
    // We don't return response here. It will be sent upon
    // success/failure at a later point in time.
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleStartLceMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestStartLceMessage> m = std::static_pointer_cast<RilRequestStartLceMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_lqe_start(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleStopLceMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilRequestStopLceMessage> m = std::static_pointer_cast<RilRequestStopLceMessage>(msg);
  if( m != NULL ) {
    qcril_request_params_type req = m->get_params();
    qcril_request_return_type ret;
    qcril_data_lqe_stop(&req, &ret);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleToggleDormancyIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ToggleDormancyIndMessage> m = std::static_pointer_cast<ToggleDormancyIndMessage>(msg);
  if( m != NULL ) {
    int ret = qcril_data_toggle_dormancy_indications(m->dormIndSwitch);
    Message::Callback::Status status = (ret == QCRIL_DS_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleToggleLimitedSysIndMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<ToggleLimitedSysIndMessage> m = std::static_pointer_cast<ToggleLimitedSysIndMessage>(msg);
  if( m != NULL ) {
    int ret = qcril_data_toggle_limited_sys_indications(m->sysIndSwitch);
    Message::Callback::Status status = (ret == QCRIL_DS_SUCCESS ?
             Message::Callback::Status::SUCCESS : Message::Callback::Status::FAILURE);
    auto resp = std::make_shared<int>(ret);
    m->sendResponse(msg, status, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleUpdateMtuMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<UpdateMtuMessage> m = std::static_pointer_cast<UpdateMtuMessage>(msg);
  if( m != NULL ) {
    qcril_data_update_mtu(m->Mtu);
    auto resp = std::make_shared<int>(QCRIL_DS_SUCCESS);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleDataEmbmsActiveMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RilDataEmbmsActiveMessage> m = std::static_pointer_cast<RilDataEmbmsActiveMessage>(msg);
  if( m != NULL ) {
    int is_Embms_Active = qcril_data_is_embms_active();
    auto resp = std::make_shared<bool>(is_Embms_Active);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleGetDdsSubIdMessage(std::shared_ptr<Message> msg) {
  Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
  std::shared_ptr<GetDdsSubIdMessage> m = std::static_pointer_cast<GetDdsSubIdMessage>(msg);
  if( m != NULL ) {
    DDSSubIdInfo ddsInfo = qcril_data_get_dds_sub_info();
    auto resp = std::make_shared<DDSSubIdInfo>(ddsInfo);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

void DataModule::handleDataRequestDDSSwitchMessage(std::shared_ptr<Message> msg) {
  std::shared_ptr<RequestDdsSwitchMessage> m = std::static_pointer_cast<RequestDdsSwitchMessage>(msg);
  if( m != NULL ) {
    RIL_Errno ret = qcril_data_request_dds_switch(m->dds_sub_info);
    auto resp = std::make_shared<RIL_Errno>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

#if (QCRIL_RIL_VERSION >= 15)
void DataModule::handleStartKeepaliveMessage(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<RilRequestStartKeepaliveMessage> m = std::static_pointer_cast<RilRequestStartKeepaliveMessage>(msg);
    if( m != NULL ) {
      qcril_request_params_type req = m->get_params();
      qcril_request_return_type ret;
      qcril_data_start_modem_assist_keepalive(&req, &ret);
    } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}

void DataModule::handleStopKeepaliveMessage(std::shared_ptr<Message> msg) {
    Log::getInstance().d("[" + mName + "]: Handling msg = " + msg->dump());
    std::shared_ptr<RilRequestStopKeepaliveMessage> m = std::static_pointer_cast<RilRequestStopKeepaliveMessage>(msg);
    if( m != NULL ) {
      qcril_request_params_type req = m->get_params();
      qcril_request_return_type ret;
      qcril_data_stop_modem_assist_keepalive(&req, &ret);
    } else {
      Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
    }
}
#endif

/*===========================================================================

  FUNCTION:  handleNasRequestDataShutdown

===========================================================================*/
/*!
    @brief
    Handler which gets invoked when NasRequestDataShutdownMessage is received

    @return
*/
/*=========================================================================*/
void DataModule::handleNasRequestDataShutdown(std::shared_ptr<Message> msg)
{
  Log::getInstance().d("[DataModule]: Handling msg = " + msg->dump());
  std::shared_ptr<NasRequestDataShutdownMessage> m = std::static_pointer_cast<NasRequestDataShutdownMessage>(msg);
  if (m != NULL)
  {
    NasRequestDataShutdownResponse ret = NasRequestDataShutdownResponse::FAILURE;
    if(qcril_data_device_shutdown()) {
      ret = NasRequestDataShutdownResponse::SUCCESS;
    }
    auto resp = std::make_shared<NasRequestDataShutdownResponse>(ret);
    m->sendResponse(msg, Message::Callback::Status::SUCCESS, resp);
  } else {
    Log::getInstance().d("[" + mName + "]: Improper message received = " + msg->dump());
  }
}

}//namespace
