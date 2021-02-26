/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#include<string>

#include <modules/ims/ImsServiceStatusInd.h>

constexpr const char *ImsServiceStatusInd::MESSAGE_NAME;

ImsServiceStatusInd::ImsServiceStatusInd(
   ImsServiceStatusInd::ServiceStatusInfo serviceStatusInfo) :
  UnSolicitedMessage(get_class_message_id()) {
  mName = MESSAGE_NAME;
  // Registration status
  mServiceStatusInfo.reg_status = serviceStatusInfo.reg_status;
  // voip service status
  mServiceStatusInfo.voip_service_status = serviceStatusInfo.voip_service_status;
  mServiceStatusInfo.voip_service_rat = serviceStatusInfo.voip_service_rat;
  // vt service status
  mServiceStatusInfo.vt_service_status = serviceStatusInfo.vt_service_status;
  mServiceStatusInfo.vt_service_rat = serviceStatusInfo.vt_service_rat;
}
