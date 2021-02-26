/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "qmi_client.h"
#include "card_application_toolkit_v02.h"

#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/qmi/QmiServiceUpIndMessage.h"
#include "modules/uim/GstkQmiCatRequestMsg.h"
#include "modules/uim/GstkQmiCatRequestSyncMsg.h"
#include "modules/uim/qcril_gstk_srvc.h"

class CatModemEndPointModule : public Module {
 public:
  CatModemEndPointModule(string name, ModemEndPoint* owner);
  ~CatModemEndPointModule();

  void onQmiUnsolIndCb(qmi_cat_indication_id_type   ind_id,
                       uint32_t                     ind_len,
                       uint8_t                    * ind_data_ptr);

  void onQmiNotifyCb(qmi_client_type user_handle,
                     qmi_idl_service_object_type service_obj,
                     qmi_client_notify_event_type service_event);

  void onQmiAsyncCb(std::shared_ptr<GstkQmiCatRequestMsg>  req_msg_ptr,
                    unsigned int                           msg_id,
                    void                                 * resp_c_struct,
                    int                                    transp_err);

  void init();
#ifdef QMI_RIL_UTF
  void cleanUpQmiSvcClient();
#endif

 private:
  qmi_idl_service_object_type                          mServiceObject;
  qmi_client_type                                      mQmiSvcClient;
  std::vector<std::shared_ptr<Message>>                mMessageList;
  std::vector<std::shared_ptr<GstkQmiCatRequestMsg>>   mEnvList;
  std::shared_ptr<GstkQmiCatRequestMsg>                mTrMsgPtr;
  ModemEndPoint                                      * mOwner;
  qmi_client_os_params                                 mOsParams;
  qmi_client_os_params                                 mNotifyParams;
  qmi_client_type                                      mNotifyHandle;

  void handleQmiClientSetup(std::shared_ptr<Message> msg);

  void handleQmiServiceUp(std::shared_ptr<Message> msg);

  void handleQmiServiceDown(std::shared_ptr<Message> msg);

  void handleQcrilGstkRequest(std::shared_ptr<GstkQmiCatRequestMsg> msg_ptr);

  void handleQcrilGstkSyncRequest(std::shared_ptr<GstkQmiCatRequestSyncMsg> msg_ptr);
};

