/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiSmsSetupRequestCallback.h"


#define TAG "QmiSmsSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiSmsSetupRequestCallback onResponse function
===========================================================================*/
void QmiSmsSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO( "QmiSmsSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);

} /* QmiSmsSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiSmsSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiSmsSetupRequestCallback::clone()
{
  QmiSmsSetupRequestCallback *clone = new QmiSmsSetupRequestCallback(mClientToken);
  return clone;
} /* QmiSmsSetupRequestCallback::clone */
