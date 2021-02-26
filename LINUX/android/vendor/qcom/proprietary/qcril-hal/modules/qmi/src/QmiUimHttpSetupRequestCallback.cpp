/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiUimHttpSetupRequestCallback.h"
#include "framework/Log.h"

#define TAG "QmiUimHttpSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiUimHttpSetupRequestCallback onResponse function
===========================================================================*/
void QmiUimHttpSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO("QmiUimHttpSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiUimHttpSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
} /* QmiUimHttpSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiUimHttpSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiUimHttpSetupRequestCallback::clone()
{
  QmiUimHttpSetupRequestCallback *clone = new QmiUimHttpSetupRequestCallback(mClientToken);
  return clone;
} /* QmiUimSetupRequestCallback::clone */
