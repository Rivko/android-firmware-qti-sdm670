/******************************************************************************
#  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

#include <telephony/ril.h>
#include <cutils/properties.h>
#include "framework/ModuleLooper.h"
#include "framework/QcrilInitMessage.h"
#include "modules/qmi/ModemEndPointFactory.h"
#include "modules/qmi/CatModemEndPoint.h"
#include "modules/qmi/QmiCatSetupRequestCallback.h"
#include "GstkModule.h"
#include "qcril_gstk.h"

#ifdef QMI_RIL_UTF
#include "modules/qmi/CatModemEndPointModule.h"
#endif

#define TAG "GstkModule"

/* Android property to disable for certain test scenarios */
#define QCRIL_GSTK_PROPERTY_DISABLED               "persist.vendor.radio.qcril_gstk.disable"
#define QCRIL_GSTK_PROP_DISABLED_VALUE             "1"

static load_module<GstkModule> gstk_module;

GstkModule::GstkModule()
{
  mName = "GstkModule";
  mReady = false;

  mLooper = std::unique_ptr<ModuleLooper>(new ModuleLooper);

  QCRIL_LOG_INFO( "%s - Constructor\n", __FUNCTION__);

  using std::placeholders::_1;
  mMessageHandler = {
       HANDLER(QcrilInitMessage, GstkModule::GstkProcessRilInit),
       HANDLER(GstkRilRequestMessage, GstkModule::GstkProcessRilRequest),
       HANDLER(GstkQmiCatResponseMsg, GstkModule::GstkProcessQmiCatCallback),
       HANDLER(GstkQmiCatIndicationMsg, GstkModule::GstkProcessQmiCatInd),
       HANDLER(UimGstkCardStatusMsg, GstkModule::GstkProcessCardStatusMsg),
                    };
} /* GstkModule::GstkModule */


GstkModule::~GstkModule()
{
  QCRIL_LOG_INFO( "%s - Destructor \n", __FUNCTION__);
  mReady = false;
} /* GstkModule::~GstkModule */


void GstkModule::init()
{
  QCRIL_LOG_INFO( "%s \n", __FUNCTION__);

  /* Call base init before doing any other stuff.*/
  Module::init();

  /* Reset qcril_gstk */
  qcril_gstk_reset_state(FALSE);

  ModemEndPointFactory<CatModemEndPoint>::getInstance().buildEndPoint();
} /* GstkModule::init */


void GstkModule::GstkProcessCardStatusMsg(std::shared_ptr<UimGstkCardStatusMsg> msg_ptr)
{
  QCRIL_LOG_INFO( "%s \n", __FUNCTION__);

  if (msg_ptr == nullptr)
  {
    return;
  }
   
  QCRIL_LOG_DEBUG( "Card_status on slot : %d\n", msg_ptr->get_slot_id());
  if (msg_ptr->get_slot_id() == qmi_ril_get_process_instance_id())
  {
    qcril_gstk_reset_state(msg_ptr->is_refresh());
  }

  msg_ptr->sendResponse(msg_ptr,
                        Message::Callback::Status::SUCCESS,
                        nullptr);
} /* GstkModule::GstkProcessCardStatusMsg */


void GstkModule::GstkProcessRilInit(std::shared_ptr<QcrilInitMessage> msg_ptr)
{
  char  prop_value_disable[PROPERTY_VALUE_MAX];

  (void) msg_ptr;

  /* If QCRIL_GSTK needs to be disabled, just return at this point */
  memset(prop_value_disable, 0x00, sizeof(prop_value_disable));
  property_get(QCRIL_GSTK_PROPERTY_DISABLED, prop_value_disable, "");
  if (strcmp(prop_value_disable, QCRIL_GSTK_PROP_DISABLED_VALUE)  == 0)
  {
    QCRIL_LOG_INFO("%s", "QCRIL GSTK is being disabled");
    return;
  }

  /* Init QMI services.*/
  QmiCatSetupRequestCallback qmiCatSetupCallback("GSTK-Module-Token");
  ModemEndPointFactory<CatModemEndPoint>::getInstance().buildEndPoint()->requestSetup("Client-CAT-Serv-Token", &qmiCatSetupCallback);
} /* GstkModule::GstkProcessRilInit */


void GstkModule::GstkProcessRilRequest(std::shared_ptr<GstkRilRequestMessage> req_ptr)
{
  std::shared_ptr<gstk_ril_resp_payload>    rsp_ptr    = nullptr;
  RIL_Errno                                 ril_err    = RIL_E_SUCCESS;
  int                                       msg_id     = 0;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (req_ptr == nullptr)
  {
    return;
  }

  msg_id = req_ptr->get_req_id();
  QCRIL_LOG_INFO( "mReady :%d, request: %d", mReady, msg_id);

  /* ATEL may ready before GSTK module is ready */
  if ( msg_id == RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING)
  {
    ril_err = qcril_gstk_qmi_request_stk_service_is_running(req_ptr, mReady);
    if (ril_err != RIL_E_SUCCESS)
    {
      rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err, nullptr, 0);
      req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);
    }
    return;
  }

  if (mReady == false)
  {
    ril_err = RIL_E_RADIO_NOT_AVAILABLE;
    rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err, nullptr, 0);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, rsp_ptr);
    return;
  }

  switch(msg_id)
  {
    case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
    case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
      ril_err = qcril_gstk_qmi_request_stk_send_envelope_command(req_ptr);
      break;
    case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
      ril_err = qcril_gstk_qmi_request_stk_send_terminal_response(req_ptr);
      break;
    case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
      ril_err = qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim(req_ptr);
      break;
    default:
      rsp_ptr = std::make_shared<gstk_ril_resp_payload>(RIL_E_REQUEST_NOT_SUPPORTED, nullptr, 0);
      req_ptr->sendResponse(req_ptr, Message::Callback::Status::NO_HANDLER_FOUND, rsp_ptr);
      return;
  }

  if (ril_err != RIL_E_SUCCESS)
  {
    rsp_ptr = std::make_shared<gstk_ril_resp_payload>(ril_err, nullptr, 0);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, rsp_ptr);
  }
} /* GstkModule::GstkProcessRilRequest */


void GstkModule::GstkProcessQmiCatCallback(std::shared_ptr<GstkQmiCatResponseMsg> msg_ptr)
{
  if (msg_ptr != nullptr)
  {
    qcril_gstk_process_qmi_cat_cb(msg_ptr->get_message());
  }
} /* GstkModule::GstkProcessQmiCatCallback */


void GstkModule::GstkProcessQmiCatInd(std::shared_ptr<GstkQmiCatIndicationMsg> msg_ptr)
{
  if (msg_ptr != nullptr)
  {
    QCRIL_LOG_DEBUG(" GstkProcessQmiCatInd : 0x%x", msg_ptr->get_ind_id());
    switch(msg_ptr->get_ind_id())
    {
      case QMI_CAT_SRVC_UP_IND_MSG:
        if (qcril_gstk_init_state() != 0)
        {
          QCRIL_LOG_INFO("Init failed");
          break;
        }
        mReady = true;
        break;
      case QMI_CAT_SRVC_DOWN_IND_MSG:
        mReady = false;
        qcril_gstk_reset_state(FALSE);
        break;
      default:
        qcril_gstk_process_qmi_cat_ind(msg_ptr->get_ind_id(), msg_ptr->get_message());
        break;
    }
  }
} /* GstkModule::GstkProcessQmiCatInd */

GstkModule *getGstkModule() {
    return &(gstk_module.get_module());
}

#ifdef QMI_RIL_UTF
void GstkModule::qcrilHalGstkModuleCleanup()
{
    std::shared_ptr<CatModemEndPoint> mCatModemEndPoint =
                    ModemEndPointFactory<CatModemEndPoint>::getInstance().buildEndPoint();
    CatModemEndPointModule* mCatModemEndPointModule =
                   (CatModemEndPointModule*)mCatModemEndPoint->mModule;
    mCatModemEndPointModule->cleanUpQmiSvcClient();
    mReady = false;
}
#endif
