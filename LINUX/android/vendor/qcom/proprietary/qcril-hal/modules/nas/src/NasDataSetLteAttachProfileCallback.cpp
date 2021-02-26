/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_qmi_nas.h"
#include "framework/Log.h"
#include "framework/legacy.h"
#include "modules/nas/NasDataSetLteAttachProfileCallback.h"
#include "modules/data/RequestDdsSwitchMessage.h"

//TODO: move to a common file


//NasDataSetLteAttachProfileCallback
void NasDataSetLteAttachProfileCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<RIL_Errno> responseDataPtr) {

  if (status == Message::Callback::Status::SUCCESS) {
    Log::getInstance().d("[NasDataSetLteAttachProfileCallback]: Callback[msg = " +
    solicitedMsg->dump() + "] executed. client data = " +
    mClientToken + " status = Message::Callback::Status::SUCCESS");
    qcril_qmi_nas_set_initial_attach_apn_part2(*(responseDataPtr.get()), &params.get_params());
} else {
    Log::getInstance().d("[NasDataSetLteAttachProfileCallback]: Callback[msg = " +
    solicitedMsg->dump() + "] executed. client data = " +
    mClientToken + " FAILURE !!");
  }
}

Message::Callback *NasDataSetLteAttachProfileCallback::clone() {
  NasDataSetLteAttachProfileCallback *clone = new NasDataSetLteAttachProfileCallback(mClientToken, params.get_params());
  return clone;
}

//NasDataGetDdsSubIdCallback
void NasDataGetDdsSubIdCallback::onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
        std::shared_ptr<DDSSubIdInfo> responseDataPtr) {
   RIL_Errno res = RIL_E_INTERNAL_ERR;
   boolean send_resp = FALSE;
   DDSSubIdInfo *dds_sub;
   DDSSubIdInfo new_dds_sub;
   qcril_request_params_type params_data;
   qcril_request_resp_params_type resp;

   if(status == Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("[NasDataGetDdsSubIdCallback]: Callback[msg = " +
        solicitedMsg->dump() + "] executed. client data = " +
        mClientToken + " status = Message::Callback::Status::SUCCESS");

      dds_sub = (responseDataPtr.get());
      res =  qcril_qmi_nas_update_dds_sub_id(dds_sub);

      if(from_allow_data) {
        //request is made from allow data
        Log::getInstance().d("[NasDataGetDdsSubIdCallback]: request is made from allow data");

        if(qcril_qmi_nas_is_dds_change()) {
#ifndef QMI_RIL_UTF
          //send DDS switch request
          qcril_qmi_nas_dds_to_be_updated(&new_dds_sub);
          std::shared_ptr<rildata::RequestDdsSwitchMessage> msg =
              std::make_shared<rildata::RequestDdsSwitchMessage>(new_dds_sub);
          if(msg) {
              NasDataRequestDdsSwitchCallback cb("set-cb-token", params.get_params());
              msg->setCallback(&cb);
              msg->dispatch();

              //response will be sent in the callback.
              res = RIL_E_SUCCESS;
          } else {
              res = RIL_E_NO_MEMORY;
              send_resp = TRUE;
          }
#endif
        } else {
          send_resp = TRUE;
          res = RIL_E_SUCCESS;
          Log::getInstance().d("[NasDataGetDdsSubIdCallback] Already it is DDS sub, no need to update to modem");
      }
    }
    else if(from_set_initial_attch_apn) {
        //request is made from set_initial_attach_apn
        Log::getInstance().d("[NasDataGetDdsSubIdCallback]: request is made from set_initial_attach_apn");
        qcril_qmi_nas_set_initial_attach_apn_part1(&params.get_params());
        res = RIL_E_SUCCESS;
    }
  } else {
    Log::getInstance().d("[NasDataGetDdsSubIdCallback]: Callback[msg = " +
      solicitedMsg->dump() + "] executed. client data = " +
      mClientToken + " FAILURE !!");
  }

  if( (from_allow_data || from_set_initial_attch_apn) && (send_resp || res != RIL_E_SUCCESS)) {
    // send resp
    Log::getInstance().d("[NasDataGetDdsSubIdCallback]: PS attach/detach response");
    params_data = params.get_params();
    qcril_qmi_nas_set_init_attch_state(QMI_RIL_NAS_INIT_APN_ATTCH_NONE);
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       params_data.t,
                                       params_data.event_id,
                                       res,
                                       &resp );
    qcril_send_request_response( &resp );
  }
}

Message::Callback *NasDataGetDdsSubIdCallback::clone() {
  NasDataGetDdsSubIdCallback *clone = new NasDataGetDdsSubIdCallback(mClientToken, params.get_params(), from_allow_data, from_set_initial_attch_apn);
  return clone;
}

//NasDataRequestDdsSwitchCallback
void NasDataRequestDdsSwitchCallback::onResponse(
    std::shared_ptr<Message> solicitedMsg, Status status,
    std::shared_ptr<RIL_Errno> responseDataPtr) {

    RIL_Errno   res = RIL_E_INTERNAL_ERR;
    qcril_request_params_type params_data = params.get_params();
    qcril_request_resp_params_type resp;
  
    if (status == Message::Callback::Status::SUCCESS) {
      Log::getInstance().d("[NasDataRequestDdsSwitchCallback]: Callback[msg = " +
      solicitedMsg->dump() + "] executed. client data = " +
      mClientToken + " status = Message::Callback::Status::SUCCESS");
      res = *(responseDataPtr.get());

      if(res != RIL_E_SUCCESS)
      {
        Log::getInstance().d("[NasDataRequestDdsSwitchCallback]: set dds through data layer FAILED");
      }
      else
      {
         res = qcril_qmi_nas_set_dds_start_timer();
      }
  } else {
    Log::getInstance().d("[NasDataRequestDdsSwitchCallback]: Callback[msg = " +
    solicitedMsg->dump() + "] executed. client data = " +
    mClientToken + " FAILURE !!");
  }

  if(res != RIL_E_SUCCESS) {
     Log::getInstance().d("[NasDataGetDdsSubIdCallback]: PS attach/detach response");
     qcril_qmi_nas_set_init_attch_state(QMI_RIL_NAS_INIT_APN_ATTCH_NONE);
     qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                            params_data.t,
                                            params_data.event_id,
                                            res,
                                            &resp );
     qcril_send_request_response( &resp );
   }
}

Message::Callback *NasDataRequestDdsSwitchCallback::clone() {
  NasDataRequestDdsSwitchCallback *clone = new NasDataRequestDdsSwitchCallback(mClientToken, params.get_params());
  return clone;
}

