/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include "modules/qmi/QmiCatSetupRequestCallback.h"
#include "modules/uim/GstkQmiCatIndicationMsg.h"
#include "modules/uim/qcril_gstk_srvc.h"
#include "framework/Log.h"

#define TAG "QmiCatSetupRequestCallback"

/*===========================================================================
  FUNCTION  onResponse  -  QmiCatSetupRequestCallback onResponse function
===========================================================================*/
void QmiCatSetupRequestCallback::onResponse
(
  std::shared_ptr<Message> solicitedMsg,
  Message::Callback::Status status,
  std::shared_ptr<string> responseDataPtr
)
{
  (void)solicitedMsg;
  (void)status;
  (void)responseDataPtr;

  QCRIL_LOG_INFO( "QmiCatSetupRequestCallback : %d ", status);
} /* QmiCatSetupRequestCallback::onResponse */


/*===========================================================================
  FUNCTION  clone  -  QmiCatSetupRequestCallback clone function
===========================================================================*/
Message::Callback *QmiCatSetupRequestCallback::clone()
{
  QmiCatSetupRequestCallback *clone = new QmiCatSetupRequestCallback(mClientToken);
  return clone;
} /* QmiCatSetupRequestCallback::clone */

