/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/Module.h"
#include "framework/ModuleLooper.h"
#include "modules/qmi/ModemEndPoint.h"

#include "common_v01.h"
#include "qmi_client.h"

#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceDownIndMessage.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"
#include "modules/qmi/StackIdIndMessage.h"
#include "modules/qmi/QmiSetupRequest.h"

class ModemEndPointModule : public Module {
 ModemEndPointModule() = delete;
 public:
  ModemEndPointModule(string name, ModemEndPoint& owner);
  ~ModemEndPointModule();
  void init() override;
  virtual void onQmiUnsolIndCb(qmi_client_type user_handle, unsigned int msg_id,
                               void* ind_buf, unsigned int ind_buf_len);
  virtual void onQmiAsyncCb(qmi_client_type user_handle, unsigned int msg_id,
                          void *resp_buf, unsigned int resp_buf_len,
                          void *resp_cb_data, qmi_client_error_type transp_err);
  virtual void onQmiNotifyCb(qmi_client_type user_handle,
                             qmi_idl_service_object_type service_obj,
                             qmi_client_notify_event_type service_event);
#ifdef QMI_RIL_UTF
  void cleanUpQmiSvcClient();
#endif

 protected:
  virtual qmi_idl_service_object_type getServiceObject() = 0;

  virtual void handleQmiClientSetup(std::shared_ptr<QmiSetupRequest> msg);
  virtual void handleQmiServiceUp(std::shared_ptr<QmiServiceUpIndMessage> msg);
  virtual void handleQmiServiceDown(std::shared_ptr<QmiServiceDownIndMessage> msg);
  virtual bool handleQmiBinding(qcril_instance_id_e_type instanceId, int8_t stackId);

  void handleQmiRawSendSync(std::shared_ptr<QmiSendRawSyncMessage> msg);
  void handleQmiRawSendAsync(std::shared_ptr<QmiSendRawAsyncMessage> msg);
  void handleStackIdIndMessage(std::shared_ptr<StackIdIndMessage> msg);

  int8_t mStackId[QCRIL_MAX_INSTANCE_ID];
  qmi_idl_service_object_type mServiceObject;
  qmi_client_type mQmiSvcClient;
  qmi_client_type mQmiNotifyHandle;
  qmi_client_os_params mOsParams;
  qmi_client_os_params mNotifierOsParams;
  ModemEndPoint &mOwner;
  message_id_t mIndMsgId;
  message_id_t mEndPtStatusIndId;
};
