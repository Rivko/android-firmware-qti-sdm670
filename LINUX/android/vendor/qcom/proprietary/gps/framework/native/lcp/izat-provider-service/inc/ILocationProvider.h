/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  LocationReport

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_ILOCATIONPROVIDER_H__
#define __IZAT_MANAGER_ILOCATIONPROVIDER_H__
#ifdef __ANDROID__
#include <sys/types.h>
#endif
#include "ILocationResponse.h"
#include <loc_pla.h>
namespace izat_manager
{

class ILocationProvider {

public:

    typedef enum LocationRequestAction {
        LocationRequestAction_SingleShot,
        LocationRequestAction_Start,
        LocationRequestAction_Stop
    } LocationRequestAction;

    typedef struct {
        LocationRequestAction  action;
        uint32_t               intervalInMsec;
        bool                   emergencyRequest;
    } LocationRequest;

    virtual int setRequest(const LocationRequest *request) = 0;
    virtual void subscribe(const ILocationResponse * respObject) = 0;
    virtual void unsubscribe(const ILocationResponse * respObject) = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual ~ILocationProvider() {}
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_ILOCATIONPROVIDER_H__
