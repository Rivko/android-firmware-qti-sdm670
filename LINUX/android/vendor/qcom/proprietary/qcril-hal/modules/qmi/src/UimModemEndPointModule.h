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
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/UimQmiUimRequestMsg.h"
#include "modules/uim/UimQmiUimRequestSyncMsg.h"

class UimModemEndPointModule : public Module {
 public:
  UimModemEndPointModule(string name, ModemEndPoint* owner);
  ~UimModemEndPointModule();

  void onQmiUnsolIndCb(void * user_data, qmi_uim_indication_id_type  ind_id,
                       qmi_uim_indication_data_type * ind_data_ptr);

  void onQmiNotifyCb(qmi_client_type user_handle,
                     qmi_idl_service_object_type service_obj,
                     qmi_client_notify_event_type service_event);

  void init();
#ifdef QMI_RIL_UTF
  void cleanUpQmiSvcClient();
#endif

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

  void handleQcrilUimRequest(std::shared_ptr<UimQmiUimRequestMsg> msg_ptr);

  void handleQcrilUimSyncRequest(std::shared_ptr<UimQmiUimRequestSyncMsg> msg_ptr);
};
