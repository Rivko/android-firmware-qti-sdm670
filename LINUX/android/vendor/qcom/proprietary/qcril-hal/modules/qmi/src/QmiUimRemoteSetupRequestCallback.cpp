/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiUimRemoteSetupRequestCallback.h"
#include "framework/Log.h"

#define TAG "QmiUimRemoteSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiUimRemoteSetupRequestCallback onResponse function
===========================================================================*/
void QmiUimRemoteSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO("QmiUimRemoteSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiUimRemoteSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
} /* QmiUimRemoteSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiUimRemoteSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiUimRemoteSetupRequestCallback::clone()
{
  QmiUimRemoteSetupRequestCallback *clone = new QmiUimRemoteSetupRequestCallback(mClientToken);
  return clone;
} /* QmiUimSetupRequestCallback::clone */
