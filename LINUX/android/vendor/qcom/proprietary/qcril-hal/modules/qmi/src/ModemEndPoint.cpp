/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/ModemEndPoint.h"
#include "modules/qmi/QmiSendRawSyncMessage.h"
#include "modules/qmi/QmiSendRawAsyncMessage.h"

qmi_client_error_type ModemEndPoint::sendRawSync
(
  unsigned int                      msgId,
  void                              *reqData,
  unsigned int                      reqSize,
  void                              *respData,
  unsigned int                      respSize,
  int                               timeout
)
{
  if (mState == State::OPERATIONAL) {
    QmiSyncStruct qmiInfo;
    qmiInfo.msgId = msgId;
    qmiInfo.reqData = reqData;
    qmiInfo.reqSize = reqSize;
    qmiInfo.respData = respData;
    qmiInfo.respSize = respSize;
    qmiInfo.timerValue = timeout;

    std::shared_ptr<QmiSendRawSyncMessage> qmiSendRawSyncMsg =
            std::make_shared<QmiSendRawSyncMessage>(&qmiInfo);
    if(qmiSendRawSyncMsg){
      mModule->dispatchSync(qmiSendRawSyncMsg);

      return qmiSendRawSyncMsg->getSendResult();
    } else {
      return QMI_INTERNAL_ERR;
    }
  } else {
    return QMI_PORT_NOT_OPEN_ERR;
  }
}

qmi_client_error_type ModemEndPoint::sendRawAsync
(
  unsigned int              msgId,
  void                      *reqData,
  unsigned int              reqSize,
  unsigned int              respSize,
  qmiAsyncCbType            cb,
  void                      *cbData,
  Module                    *clientModule
)
{
  qmi_client_error_type ret = QMI_INTERNAL_ERR;

  if (mState == State::OPERATIONAL) {
    std::shared_ptr<QmiReqAsyncStruct> shared_qmiReq =
            std::make_shared<QmiReqAsyncStruct>();
    if(shared_qmiReq) {
      shared_qmiReq->msgId = msgId;
      shared_qmiReq->reqData = reqData;
      shared_qmiReq->reqSize = reqSize;
      shared_qmiReq->respSize = respSize;
      shared_qmiReq->cb = cb;
      shared_qmiReq->cbData = cbData;

      std::shared_ptr<QmiSendRawAsyncMessage> qmiSendRawAsyncMsg =
              std::make_shared<QmiSendRawAsyncMessage>(shared_qmiReq, clientModule);
      if(qmiSendRawAsyncMsg) {
        mModule->dispatchSync(qmiSendRawAsyncMsg);
        ret = qmiSendRawAsyncMsg->getSendResult();
      }
    }
  } else {
    ret = QMI_PORT_NOT_OPEN_ERR;
  }
  return ret;
}
