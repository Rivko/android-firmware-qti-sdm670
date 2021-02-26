/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiPbmSetupRequestCallback.h"

#define TAG "QmiPbmSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiPbmSetupRequestCallback onResponse function
===========================================================================*/
void QmiPbmSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO( "QmiPbmSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
} /* QmiPbmSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiPbmSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiPbmSetupRequestCallback::clone()
{
  QmiPbmSetupRequestCallback *clone = new QmiPbmSetupRequestCallback(mClientToken);
  return clone;
} /* QmiPbmSetupRequestCallback::clone */
