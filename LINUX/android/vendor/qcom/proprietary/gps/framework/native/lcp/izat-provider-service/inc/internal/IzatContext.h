/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  loc service module

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATCONTEXT_H__
#define __IZAT_MANAGER_IZATCONTEXT_H__

#include <MsgTask.h>
#include <mq_client/IPCMessagingProxy.h>
#include <IDataItemSubscription.h>
#include <IFrameworkActionReq.h>
#include <IOsObserver.h>

using namespace qc_loc_fw;
using loc_core::IFrameworkActionReq;
using loc_core::IDataItemSubscription;
using loc_core::IOsObserver;

namespace izat_manager
{
class IOSFramework;
class OSObserver;

struct s_IzatContext {
    IOSFramework *mOSFrameworkObj;
    IDataItemSubscription *mSubscriptionObj;
    IFrameworkActionReq *mFrameworkActionReqObj;
    IPCMessagingProxy *mIPCMessagingProxyObj;
    IOsObserver *mSystemStatusOsObsrvr;
    MsgTask *mMsgTask;

   inline s_IzatContext() : mOSFrameworkObj(NULL), mSubscriptionObj(NULL),
        mIPCMessagingProxyObj(NULL), mSystemStatusOsObsrvr(NULL), mMsgTask(NULL) {
    }
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATCONTEXT_H__

