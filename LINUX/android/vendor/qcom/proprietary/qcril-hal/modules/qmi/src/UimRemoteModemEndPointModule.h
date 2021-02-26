/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qmi_client.h"
#include "user_identity_module_v01.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/uim_remote/UimRemoteSrvcQcci.h"
#include "modules/uim_remote/UimRmtQmiUimRemoteReqMsg.h"

class UimRemoteModemEndPointModule : public Module {
 public:
  UimRemoteModemEndPointModule(string name, ModemEndPoint* owner);
  ~UimRemoteModemEndPointModule();

  void onQmiUnsolIndCb(std::shared_ptr<qmi_uim_remote_ind_msg> ind_data_ptr);

  void onQmiNotifyCb(qmi_client_type user_handle,
                     qmi_idl_service_object_type service_obj,
                     qmi_client_notify_event_type service_event);

  void onQmiAsyncCb(std::shared_ptr<qmi_uim_remote_resp_msg>   rsp_data_ptr,
                    std::shared_ptr<UimRmtQmiUimRemoteReqMsg>  req_msg_ptr );

  void init();

 private:
  qmi_idl_service_object_type               mServiceObject;
  qmi_client_type                           mQmiSvcClient;
  std::vector<std::shared_ptr<Message>>     mMessageList;
  ModemEndPoint                           * mOwner;
  qmi_client_os_params                      mOsParams;
  qmi_client_type                           mQmiNotifyHandle;
  qmi_client_os_params                      mNotifierOsParams;

  void handleQmiClientSetup(std::shared_ptr<Message> msg);

  void handleQmiServiceUp(std::shared_ptr<Message> msg);

  void handleQmiServiceDown(std::shared_ptr<Message> msg);

  void handleQcrilUimRmtRequest(std::shared_ptr<UimRmtQmiUimRemoteReqMsg> msg_ptr);

  int qcril_uim_remote_client_request_event(qmi_uim_remote_event_req_msg  * event_req_ptr, void *orig_ptr);

  int qcril_uim_remote_client_request_apdu(qmi_uim_remote_apdu_req_msg  * apdu_req_ptr, void *orig_ptr);
};
