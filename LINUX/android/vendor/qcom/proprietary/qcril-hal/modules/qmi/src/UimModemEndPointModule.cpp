/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <cstring>

#include "common_v01.h"
#include "user_identity_module_v01.h"

#include "framework/ModuleLooper.h"
#include "framework/PolicyManager.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceDownIndMessage.h"
#include "modules/qmi/QmiUimSetupRequest.h"
#include "modules/qmi/QmiUimSetupRequestSync.h"
#include "modules/uim/UimQmiUimIndicationMsg.h"
#include "modules/uim/qcril_uim.h"
#include "UimModemEndPointModule.h"
#include "UimModemQcci.h"

#ifdef QMI_RIL_UTF
extern "C"{
#include "ril_utf_qmi_sim.h"
}
#endif

typedef struct
{
  std::shared_ptr<UimQmiUimRequestMsg> reqPtr;
} UimModemUserData;

/*---------------------------------------------------------------------------
  Internal functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qmi_uim_notify_cb - QMI Notify callback function
===========================================================================*/
static void qmi_uim_notify_cb
(
  qmi_client_type user_handle,
  qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event,
  void *notify_cb_data
)
{
  /* Wrapper for member method*/
  UimModemEndPointModule *me = (UimModemEndPointModule *)notify_cb_data;
  me->onQmiNotifyCb(user_handle, service_obj, service_event);
} /* qmi_uim_notify_cb */


/*===========================================================================
  FUNCTION  qmi_async_cb - QMI callback function
===========================================================================*/
static void qmi_async_cb
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
)
{
  UimModemUserData   * cb_data       = (UimModemUserData *)user_data;

  if (rsp_data == nullptr || cb_data == nullptr || cb_data->reqPtr == nullptr)
  {
    QCRIL_LOG_ERROR( "qmi_async_cb, Null data");

    if (cb_data != nullptr)
    {
      cb_data->reqPtr = nullptr;
      delete cb_data;
    }
    return;
  }

  /* Send response back to UIM module */
  if (cb_data->reqPtr->get_cb_ptr() != nullptr)
  {
    cb_data->reqPtr->get_cb_ptr()(rsp_data, (void *)cb_data->reqPtr->get_orig_ptr());
  }

  cb_data->reqPtr = nullptr;
  delete cb_data;
} /* qmi_uim_async_cb */


/*===========================================================================
  FUNCTION  qmi_uim_process_qcril_uim_request - Process QCRIL UIM requests
===========================================================================*/
static int qmi_uim_process_qcril_uim_request
(
  qmi_client_type                mQmiSvcClient,
  uint32_t                       request_type,
  const void                   * param_data_ptr,
  qmi_uim_user_async_cb_type     callback_function_ptr,
  void                         * user_data,
  qmi_uim_rsp_data_type        * rsp_data_ptr
)
{
  int ret = -1;

 QCRIL_LOG_INFO( "%s, request: %d", __FUNCTION__, request_type);

  switch(request_type)
  {
    case QCRIL_UIM_REQUEST_READ_TRANSPARENT:
      ret = qcril_qmi_uim_read_transparent(mQmiSvcClient,
                                          (const qmi_uim_read_transparent_params_type*)param_data_ptr,
                                          callback_function_ptr,
                                          user_data,
                                          rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_READ_RECORD:
      ret = qcril_qmi_uim_read_record(mQmiSvcClient,
                                     (const qmi_uim_read_record_params_type*)param_data_ptr,
                                     callback_function_ptr,
                                     user_data,
                                     rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_WRITE_TRANSPARENT:
      ret = qcril_qmi_uim_write_transparent(mQmiSvcClient,
                                           (const qmi_uim_write_transparent_params_type*)param_data_ptr,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_WRITE_RECORD:
      ret = qcril_qmi_uim_write_record(mQmiSvcClient,
                                      (const qmi_uim_write_record_params_type*)param_data_ptr,
                                      callback_function_ptr,
                                      user_data,
                                      rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_RESPONSE:
      ret = qcril_qmi_uim_get_file_attributes(mQmiSvcClient,
                                             (const qmi_uim_get_file_attributes_params_type*)param_data_ptr,
                                             callback_function_ptr,
                                             user_data,
                                             rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_REFRESH_REGISTER:
      ret = qcril_qmi_uim_refresh_register(mQmiSvcClient,
                                          (const qmi_uim_refresh_register_params_type*)param_data_ptr,
                                          callback_function_ptr,
                                          user_data,
                                          rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_FDN:
      ret = qcril_qmi_uim_get_service_status(mQmiSvcClient,
                                            (const qmi_uim_get_service_status_params_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data,
                                            rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_FDN:
      ret = qcril_qmi_uim_set_service_status(mQmiSvcClient,
                                            (const qmi_uim_set_service_status_params_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data,
                                            rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_VERIFY_PIN2:
    case QCRIL_UIM_REQUEST_VERIFY_PIN:
      ret = qcril_qmi_uim_verify_pin(mQmiSvcClient,
                                    (const qmi_uim_verify_pin_params_type*)param_data_ptr,
                                    callback_function_ptr,
                                    user_data,
                                    rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_UNBLOCK_PIN:
      ret = qcril_qmi_uim_unblock_pin(mQmiSvcClient,
                                     (const qmi_uim_unblock_pin_params_type*)param_data_ptr,
                                     callback_function_ptr,
                                     user_data,
                                     rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PIN:
      ret = qcril_qmi_uim_change_pin(mQmiSvcClient,
                                    (const qmi_uim_change_pin_params_type*)param_data_ptr,
                                    callback_function_ptr,
                                    user_data,
                                    rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_PIN:
      ret = qcril_qmi_uim_set_pin_protection(mQmiSvcClient,
                                            (const qmi_uim_set_pin_protection_params_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data,
                                            rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_DEPERSO:
      ret = qcril_qmi_uim_depersonalization(mQmiSvcClient,
                                           (const qmi_uim_depersonalization_params_type*)param_data_ptr,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_POWER_UP:
      ret = qcril_qmi_uim_power_up(mQmiSvcClient,
                                  (const qmi_uim_power_up_params_type*)param_data_ptr,
                                  callback_function_ptr,
                                  user_data,
                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_POWER_DOWN:
      ret = qcril_qmi_uim_power_down(mQmiSvcClient,
                                    (const qmi_uim_power_down_params_type*)param_data_ptr,
                                    callback_function_ptr,
                                    user_data,
                                    rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_CHANGE_PROV_SESSION:
      ret = qcril_qmi_uim_change_provisioning_session(mQmiSvcClient,
                                                     (const qmi_uim_change_prov_session_params_type*)param_data_ptr,
                                                     callback_function_ptr,
                                                     user_data,
                                                     rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_AUTHENTICATE:
      ret = qcril_qmi_uim_authenticate(mQmiSvcClient,
                                       (const qmi_uim_authenticate_params_type*)param_data_ptr,
                                       callback_function_ptr,
                                       user_data,
                                       rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SAP_CONNECTION:
      ret = qcril_qmi_uim_sap_connection(mQmiSvcClient,
                                         (const qmi_uim_sap_connection_params_type*)param_data_ptr,
                                         callback_function_ptr,
                                         user_data,
                                         rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SAP_REQUEST:
      ret = qcril_qmi_uim_sap_request(mQmiSvcClient,
                                      (const qmi_uim_sap_request_params_type*)param_data_ptr,
                                      callback_function_ptr,
                                      user_data,
                                      rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_LOGICAL_CHANNEL:
      ret = qcril_qmi_uim_logical_channel(mQmiSvcClient,
                                         (const qmi_uim_logical_channel_params_type*)param_data_ptr,
                                         callback_function_ptr,
                                         user_data,
                                         rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_OPEN_LOGICAL_CHANNEL:
      ret = qcril_qmi_uim_open_logical_channel(mQmiSvcClient,
                                               (const qmi_uim_open_logical_channel_params_type*)param_data_ptr,
                                               callback_function_ptr,
                                               user_data,
                                               rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SEND_APDU:
      ret = qcril_qmi_uim_send_apdu(mQmiSvcClient,
                                   (const qmi_uim_send_apdu_params_type*)param_data_ptr,
                                   callback_function_ptr,
                                   user_data,
                                   rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_ATR:
      ret = qcril_qmi_uim_get_atr(mQmiSvcClient,
                                  (const qmi_uim_get_atr_params_type*)param_data_ptr,
                                  callback_function_ptr,
                                  user_data,
                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SEND_STATUS:
      ret = qcril_qmi_uim_send_status(mQmiSvcClient,
                                      (const qmi_uim_status_cmd_params_type*)param_data_ptr,
                                      callback_function_ptr,
                                      user_data,
                                      rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_RESELECT:
      ret = qcril_qmi_uim_reselect(mQmiSvcClient,
                                   (const qmi_uim_reselect_params_type*)param_data_ptr,
                                   callback_function_ptr,
                                   user_data,
                                   rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SUPPLY_VOLTAGE:
      ret = qcril_qmi_uim_supply_voltage(mQmiSvcClient,
                                         (const qmi_uim_supply_voltage_params_type*)param_data_ptr,
                                         callback_function_ptr,
                                         user_data,
                                         rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GBA:
      ret = qcril_qmi_uim_gba_request(mQmiSvcClient,
                                      (const qmi_uim_gba_params_type*)param_data_ptr,
                                      callback_function_ptr,
                                      user_data,
                                      rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GBA_IMPI:
      ret = qcril_qmi_uim_gba_impi(mQmiSvcClient,
                                   (const qmi_uim_gba_impi_params_type*)param_data_ptr,
                                   callback_function_ptr,
                                   user_data,
                                   rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_PERSO:
      ret = qcril_qmi_uim_personalization(mQmiSvcClient,
                                         (const qmi_uim_personalization_params_type*)param_data_ptr,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_PHYSICAL_SLOT_MAPPING:
      ret = qcril_qmi_uim_get_physical_slot_mapping(mQmiSvcClient,
                                                   callback_function_ptr,
                                                   user_data,
                                                   rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SWITCH_SLOT_EXT:
      ret = qcril_qmi_uim_switch_slot_ext(mQmiSvcClient,
                                          (const qmi_uim_physical_slot_mapping_type*)param_data_ptr,
                                          callback_function_ptr,
                                          user_data,
                                          rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_APDU_BEHAVIOR:
      ret = qcril_qmi_uim_set_apdu_behavior(mQmiSvcClient,
                                            (const qmi_uim_set_apdu_behavior_params_type*)param_data_ptr,
                                            callback_function_ptr,
                                            user_data,
                                            rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_EVENT_REG:
      ret = qcril_qmi_uim_event_reg(mQmiSvcClient,
                                    (qmi_uim_event_reg_params_type *)param_data_ptr,
                                    rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_LABEL:
      ret = qcril_qmi_uim_get_label(mQmiSvcClient,
                                    (qmi_uim_get_label_params_type *)param_data_ptr,
                                    callback_function_ptr,
                                    user_data,
                                    rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_CONFIGURATION:
      ret = qcril_qmi_uim_get_configuration(mQmiSvcClient,
                                            (qmi_uim_get_configuration_params_type *)param_data_ptr,
                                            callback_function_ptr,
                                            user_data,
                                            rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_REFRESH_OK:
      ret = qcril_qmi_uim_refresh_ok(mQmiSvcClient,
                                     (qmi_uim_refresh_ok_params_type *)param_data_ptr,
                                     rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_REFRESH_COMPLETE:
      ret = qcril_qmi_uim_refresh_complete(mQmiSvcClient,
                                           (qmi_uim_refresh_complete_params_type *)param_data_ptr,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_CARD_STATUS:
      ret = qcril_qmi_uim_get_card_status(mQmiSvcClient,
                                          callback_function_ptr,
                                          user_data,
                                          rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_CLOSE_SESSION:
      ret = qcril_qmi_uim_close_session(mQmiSvcClient,
                                        (qmi_uim_close_session_params_type *)param_data_ptr,
                                        rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_TERMINAL_CAPABILITY:
      ret = qcril_qmi_uim_get_terminal_capability(mQmiSvcClient,
                                                  (qmi_uim_terminal_capability_params_type*)param_data_ptr,
                                                  callback_function_ptr,
                                                  user_data,
                                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_TERMINAL_CAPABILITY:
      ret = qcril_qmi_uim_set_terminal_capability(mQmiSvcClient,
                                                  (qmi_uim_terminal_capability_params_type*)param_data_ptr,
                                                  callback_function_ptr,
                                                  user_data,
                                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_SLOTS_STATUS:
      ret = qcril_qmi_uim_get_slots_status(mQmiSvcClient,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_REMOTE_UNLOCK:
      ret = qcril_qmi_uim_remote_unlock(mQmiSvcClient,
                                        (const qmi_uim_remote_unlock_params_type *)param_data_ptr,
                                        callback_function_ptr,
                                        user_data,
                                        rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_ADD_PROFILE:
      ret = qcril_qmi_uim_add_profile(mQmiSvcClient,
                                      (const qmi_uim_add_profile_req_type*)param_data_ptr,
                                      callback_function_ptr,
                                      user_data,
                                      rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_SET_SIM_PROFILE:
      ret = qcril_qmi_uim_set_sim_profile(mQmiSvcClient,
                                          (const qmi_uim_set_sim_profile_req_type*)param_data_ptr,
                                          callback_function_ptr,
                                          user_data,
                                          rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_SIM_PROFILES:
      ret = qcril_qmi_uim_get_sim_profiles(mQmiSvcClient,
                                           (const qmi_uim_get_sim_profiles_type*)param_data_ptr,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_DELETE_PROFILE:
      ret = qcril_qmi_uim_delete_profile(mQmiSvcClient,
                                         (const qmi_uim_delete_profile_req_type*)param_data_ptr,
                                         callback_function_ptr,
                                         user_data,
                                         rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_UPDATE_PROFILE_NICKNAME:
      ret = qcril_qmi_uim_update_profile_nickname(mQmiSvcClient,
                                                  (const qmi_uim_update_profile_nickname_req_type*)param_data_ptr,
                                                  callback_function_ptr,
                                                  user_data,
                                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_EUICC_MEMORY_RESET:
      ret = qcril_qmi_uim_euicc_memory_reset(mQmiSvcClient,
                                             (const qmi_uim_euicc_memory_reset_req_type*)param_data_ptr,
                                             callback_function_ptr,
                                             user_data,
                                             rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_USER_CONSENT:
      ret = qcril_qmi_uim_user_consent(mQmiSvcClient,
                                       (const qmi_uim_profile_user_consent_req_type*)param_data_ptr,
                                       callback_function_ptr,
                                       user_data,
                                       rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_EID:
      ret = qcril_qmi_uim_get_eid(mQmiSvcClient,
                                  (const qmi_uim_get_eid_req_type*)param_data_ptr,
                                  callback_function_ptr,
                                  user_data,
                                  rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_SET_SERVER_ADDRESS:
      ret = qcril_qmi_uim_get_set_server_address(mQmiSvcClient,
                                                 (const qmi_uim_euicc_default_server_address_req_type*)param_data_ptr,
                                                 callback_function_ptr,
                                                 user_data,
                                                 rsp_data_ptr);
      break;

    case QCRIL_UIM_REQUEST_GET_PROFILE_INFO:
      ret = qcril_qmi_uim_get_profile_info(mQmiSvcClient,
                                           (const qmi_uim_get_profile_info_req_type*)param_data_ptr,
                                           callback_function_ptr,
                                           user_data,
                                           rsp_data_ptr);
      break;

    default:
      ret = -1;
      break;
  }
  return ret;
} /* qmi_uim_process_qcril_uim_request */


/*---------------------------------------------------------------------------
  UimModemEndPointModule class functions
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  UimModemEndPointModule - Constructor
===========================================================================*/
UimModemEndPointModule::UimModemEndPointModule
(
  string name,
  ModemEndPoint *owner
)
{
  mName = name;
  mOwner = owner;
  mServiceObject = nullptr;
  mQmiSvcClient = nullptr;
  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  using std::placeholders::_1;

  mMessageHandler = {
       HANDLER(QmiUimSetupRequest, UimModemEndPointModule::handleQmiClientSetup),
       HANDLER(QmiUimSetupRequestSync, UimModemEndPointModule::handleQmiClientSetup),
       HANDLER(UimQmiUimRequestMsg, UimModemEndPointModule::handleQcrilUimRequest),
       HANDLER(UimQmiUimRequestSyncMsg, UimModemEndPointModule::handleQcrilUimSyncRequest),
       HANDLER(QmiServiceUpIndMessage, UimModemEndPointModule::handleQmiServiceUp),
       HANDLER(QmiServiceDownIndMessage, UimModemEndPointModule::handleQmiServiceDown),
  };
} /* UimModemEndPointModule::UimModemEndPointModule */


/*===========================================================================
  FUNCTION  UimModemEndPointModule - init
===========================================================================*/

void UimModemEndPointModule::init()
{
  QCRIL_LOG_INFO( "%s init\n", __FUNCTION__);

  /* Call base init before doing any other stuff.*/
  Module::init();
}


/*===========================================================================
  FUNCTION  UimModemEndPointModule - Destructor
===========================================================================*/
UimModemEndPointModule::~UimModemEndPointModule()
{
  QCRIL_LOG_INFO( "%s", __FUNCTION__);

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
  mLooper = nullptr;
} /* UimModemEndPointModule::~UimModemEndPointModule */


/*===========================================================================
  FUNCTION  onQmiNotifyCb - UimModemEndPointModule QMI client notfier
                                    callback handler
===========================================================================*/
void UimModemEndPointModule::onQmiNotifyCb
(
  qmi_client_type user_handle, qmi_idl_service_object_type service_obj,
  qmi_client_notify_event_type service_event
)
{
  (void)user_handle;
  (void)service_obj;
  Log::getInstance().d("[UimModemEndPointModule]: onQmiNotifyCb()");
  // std::shared_ptr<Message> msg =
  // std::static_pointer_cast<Message>notify_cb_data;

  switch (service_event)
  {
    case QMI_CLIENT_SERVICE_COUNT_INC:
      Log::getInstance().d(
          "[UimModemEndPointModule]: qmiNotifyCb() Service up indication");

      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceUpIndMessage(nullptr)));

      break;
    case QMI_CLIENT_SERVICE_COUNT_DEC:
      Log::getInstance().d(
          "[UimModemEndPointModule]: qmiNotifyCb() Service down indication");
      this->dispatch(
          std::shared_ptr<Message>(new QmiServiceDownIndMessage(QMI_SERVICE_ERR)));
      break;
    default:
      Log::getInstance().d(
          "[UIMModemEndPointModule]: qmiNotifyCb() unsupported service "
          "event: " +
          std::to_string(service_event));
      break;
  }
  return;
} /* UimModemEndPointModule::onQmiNotifyCb */


/*===========================================================================
  FUNCTION  onQmiUnsolIndCb - UimModemEndPointModule QMI UIM unsol
                                    message handler
===========================================================================*/
void UimModemEndPointModule::onQmiUnsolIndCb
(
  void * user_data,
  qmi_uim_indication_id_type     ind_id,
  qmi_uim_indication_data_type * ind_data_ptr
)
{
  std::shared_ptr<UimQmiUimIndicationMsg> ind_msg_ptr =
       std::make_shared<UimQmiUimIndicationMsg>(user_data,
                                                (uint32_t)ind_id,
                                                (void *)ind_data_ptr);
  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* UimModemEndPointModule::onQmiUnsolIndCb */


/*===========================================================================
  FUNCTION  handleQmiClientSetup -  UimModemEndPointModule QMI Client setup
                                    requet handler
===========================================================================*/
void UimModemEndPointModule::handleQmiClientSetup
(
  std::shared_ptr<Message> msg
)
{
  (void)msg;

  Log::getInstance().d("[UimModemEndPointModule]: handleQmiClientSetup()");

  QCRIL_LOG_INFO( "handleQmiClientSetup : %d", mOwner->getState());

  if (mServiceObject == nullptr)
  {
    mServiceObject = uim_get_service_object_v01();
    if (mServiceObject == nullptr)
    {
      Log::getInstance().d(
          "[UimModemEndPointModule]:Did not get uim_get_service_object");
      return;
    }
    else
    {
      Log::getInstance().d(
          "[UimModemEndPointModule]:Got uim_get_service_object");
    }

    qmi_client_error_type rc = qmi_client_notifier_init(
        mServiceObject, &mNotifierOsParams, &mQmiNotifyHandle);

    if (rc == QMI_NO_ERR)
    {
      mMessageList.push_back(msg);
      rc = qmi_client_register_notify_cb(mQmiNotifyHandle, qmi_uim_notify_cb, this);
      if (rc != QMI_NO_ERR)
      {
        Log::getInstance().d(
            "[UimModemEndPointModule]: qmi_client_register_notify_cb "
            "failed: " +
            std::to_string(rc));
      }
    }
    else
    {
      Log::getInstance().d(
          "[UimModemEndPointModule]: qmi_client_notifier_init failed: " +
          std::to_string(rc));
    }
    return;
  }
  else
  {
    mMessageList.push_back(msg);
    return;
  }
} /* UimModemEndPointModule::handleQmiClientSetup */


/*===========================================================================
  FUNCTION  handleQmiServiceUp -    UimModemEndPointModule QMI service up
                                    indication handler
===========================================================================*/
void UimModemEndPointModule::handleQmiServiceUp
(
  std::shared_ptr<Message> msg
)
{
  qmi_client_error_type client_err     = QMI_NO_ERR;
  qmi_client_type       qmi_svc_client = nullptr;
  qmi_service_info      info;

  (void)msg;
  Log::getInstance().d("[UimModemEndPointModule]: handleQmiServiceUp()");

  if (mQmiSvcClient != nullptr)
  {
    QCRIL_LOG_INFO( "Already Registered to service");
    return;
  }

  memset(&mOsParams, 0x00, sizeof(mOsParams));

  client_err = qmi_client_get_any_service(mServiceObject, &info);
  Log::getInstance().d("[UimModemEndPointModule]: client_info: " + std::to_string(client_err));

  client_err = qmi_client_init(&info,
                               mServiceObject,
                               (qmi_client_ind_cb)qmi_client_indication_cb,
                               this,
                               &mOsParams,
                               &qmi_svc_client);

  Log::getInstance().d("[UimModemEndPointModule]: client_err = " +
                       std::to_string(client_err));
  if (client_err != QMI_NO_ERR)
  {
    Log::getInstance().d("[UimModemEndPointModule]: Error in client init");
  }
  else
  {
    Log::getInstance().d("[UimModemEndPointModule]: No Error in client init");
    QCRIL_LOG_INFO( "Qmi service client : %p", qmi_svc_client);

    mQmiSvcClient = qmi_svc_client;
    mOwner->setState(ModemEndPoint::State::OPERATIONAL);

    // Notify clients of this end-point that  we have qmi client handle.
    for (auto msg : mMessageList) {
      Log::getInstance().d("[UimModemEndPointModule]: Notify client = " +
                           msg->dump());
      QCRIL_LOG_INFO( "handleQmiServiceUp : %d", mOwner->getState());
      std::shared_ptr<QmiUimSetupRequest> shared_msg =
          std::static_pointer_cast<QmiUimSetupRequest>(msg);
      if(shared_msg) {
        shared_msg->sendResponse(shared_msg, Message::Callback::Status::SUCCESS,
                                 nullptr);
      }
    }

    std::shared_ptr<UimQmiUimIndicationMsg> ind_msg_ptr =
         std::make_shared<UimQmiUimIndicationMsg>(nullptr,
                                                  QMI_UIM_SRVC_UP_IND_MSG,
                                                  nullptr);
    if (ind_msg_ptr != nullptr)
    {
      ind_msg_ptr->broadcast();
    }

    mMessageList.clear();
  }
} /* UimModemEndPointModule::handleQmiServiceUp */


/*===========================================================================
  FUNCTION  handleQmiServiceDown -  UimModemEndPointModule QMI service down
                                    indication handler
===========================================================================*/
void UimModemEndPointModule::handleQmiServiceDown
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

  std::shared_ptr<UimQmiUimIndicationMsg> ind_msg_ptr =
       std::make_shared<UimQmiUimIndicationMsg>(nullptr,
                                                QMI_UIM_SRVC_DOWN_IND_MSG,
                                                nullptr);
  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* UimModemEndPointModule::handleQmiServiceDown */


/*===========================================================================
  FUNCTION  handleQcrilUimRequest - UimModemEndPointModule QCRIL request
                                    handler
===========================================================================*/
void UimModemEndPointModule::handleQcrilUimRequest
(
  std::shared_ptr<UimQmiUimRequestMsg> uim_qmi_req_msg_ptr
)
{
  std::shared_ptr<int>              respPtr   = nullptr;
  UimModemUserData                * user_data    = nullptr;

  respPtr = std::make_shared<int>(-1);
  user_data = new UimModemUserData{uim_qmi_req_msg_ptr};

  if (mOwner->getState() != ModemEndPoint::State::OPERATIONAL ||
      respPtr == nullptr ||
      user_data == nullptr ||
      uim_qmi_req_msg_ptr == nullptr ||
      uim_qmi_req_msg_ptr->get_orig_ptr() == nullptr)
  {
    QCRIL_LOG_ERROR(" Failed to handle request resp: 0x%x, req: 0x%x, user_data: %p",
                    respPtr.get(), uim_qmi_req_msg_ptr.get(), user_data);

    respPtr = nullptr;

    if (uim_qmi_req_msg_ptr != nullptr)
    {
      uim_qmi_req_msg_ptr->sendResponse(uim_qmi_req_msg_ptr,
                                        Message::Callback::Status::FAILURE,
                                        respPtr);
    }

    if (user_data != nullptr)
    {
      user_data->reqPtr = nullptr;
      delete user_data;
    }
    return;
  }

  *respPtr = qmi_uim_process_qcril_uim_request(mQmiSvcClient,
                                               uim_qmi_req_msg_ptr->get_msg_id(),
                                               uim_qmi_req_msg_ptr->get_message(),
                                               qmi_async_cb,
                                               (void *)user_data,
                                               nullptr);

  uim_qmi_req_msg_ptr->sendResponse(uim_qmi_req_msg_ptr,
                                    Message::Callback::Status::SUCCESS,
                                    respPtr);
} /* UimModemEndPointModule::handleQcrilUimRequest */


/*===========================================================================
  FUNCTION  handleQcrilUimSyncRequest - UimModemEndPointModule QCRIL sync
                                        request handler
===========================================================================*/
void UimModemEndPointModule::handleQcrilUimSyncRequest
(
  std::shared_ptr<UimQmiUimRequestSyncMsg> uim_qmi_req_msg_ptr
)
{
  std::shared_ptr<int>              respPtr   = nullptr;

  QCRIL_LOG_INFO("%s, State: %d", __FUNCTION__, mOwner->getState());

  respPtr = std::make_shared<int>(-1);

  if (mOwner->getState() != ModemEndPoint::State::OPERATIONAL ||
      respPtr == nullptr ||
      uim_qmi_req_msg_ptr == nullptr)
  {
    QCRIL_LOG_ERROR(" Failed to handle request resp: 0x%x, req: 0x%x", respPtr.get(), uim_qmi_req_msg_ptr.get());

    respPtr = nullptr;
    if (uim_qmi_req_msg_ptr != nullptr)
    {
      uim_qmi_req_msg_ptr->sendResponse(uim_qmi_req_msg_ptr,
                                        Message::Callback::Status::FAILURE,
                                        respPtr);
    }
    return;
  }

  *respPtr = qmi_uim_process_qcril_uim_request(mQmiSvcClient,
                                               uim_qmi_req_msg_ptr->get_msg_id(),
                                               uim_qmi_req_msg_ptr->get_message(),
                                               uim_qmi_req_msg_ptr->get_cb_ptr(),
                                               uim_qmi_req_msg_ptr->get_user_data(),
                                               (qmi_uim_rsp_data_type *)uim_qmi_req_msg_ptr->get_rsp_data());

  uim_qmi_req_msg_ptr->sendResponse(uim_qmi_req_msg_ptr,
                                    Message::Callback::Status::SUCCESS,
                                    respPtr);
} /* UimModemEndPointModule::handleQcrilUimSyncRequest */


#ifdef QMI_RIL_UTF
void UimModemEndPointModule::cleanUpQmiSvcClient()
{
  if(nullptr != mQmiSvcClient)
  {
    qmi_client_release(mQmiSvcClient);
    mQmiSvcClient = nullptr;
    mServiceObject = nullptr;
  }
}
#endif
