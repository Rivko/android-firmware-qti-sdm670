/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiUimSetupRequestCallback.h"
#include "modules/uim/UimQmiUimIndicationMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "framework/Log.h"

#define TAG "QmiUimSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiUimSetupRequestCallback onResponse function
===========================================================================*/
void QmiUimSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO( "QmiUimSetupRequestCallback : %d ", status);
  Log::getInstance().d(
      "[QmiSetupRequestCallback]: Callback executed. client data = " +
      mClientToken);
} /* QmiUimSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiUimSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiUimSetupRequestCallback::clone()
{
  QmiUimSetupRequestCallback *clone = new QmiUimSetupRequestCallback(mClientToken);
  return clone;
} /* QmiUimSetupRequestCallback::clone */
