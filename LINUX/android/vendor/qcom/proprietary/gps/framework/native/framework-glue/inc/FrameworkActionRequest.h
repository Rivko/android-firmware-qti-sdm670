/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Framework action request class definition.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __FRAMEWORKACTIONREQUEST_H__
#define __FRAMEWORKACTIONREQUEST_H__

#include <DataItemId.h>
#include <IFrameworkActionReq.h>
#include <loc_pla.h>

using loc_core::IFrameworkActionReq;

class FrameworkActionRequest : public IFrameworkActionReq {

public:
    static FrameworkActionRequest* getFrameworkActionReqObj();
    static void destroyInstance();

    // IFrameworkActionReq Overrides
    void turnOn (DataItemId dit, int timeOut = 0);
    void turnOff (DataItemId dit);
#ifdef USE_GLIB
    virtual bool connectBackhaul();
    virtual bool disconnectBackhaul();
#endif

protected:

private:
    static FrameworkActionRequest *mFrameworkActionReqObj;

    FrameworkActionRequest() {}
    ~FrameworkActionRequest() {}
};

#endif // #ifndef __FRAMEWORKACTIONREQUEST_H__
