/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiNasSetupRequestCallback.h"

#define TAG "QmiNasSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiNasSetupRequestCallback onResponse function
===========================================================================*/
void QmiNasSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO( "QmiNasSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
} /* QmiNasSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiNasSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiNasSetupRequestCallback::clone()
{
  QmiNasSetupRequestCallback *clone = new QmiNasSetupRequestCallback(mClientToken);
  return clone;
} /* QmiNasSetupRequestCallback::clone */
