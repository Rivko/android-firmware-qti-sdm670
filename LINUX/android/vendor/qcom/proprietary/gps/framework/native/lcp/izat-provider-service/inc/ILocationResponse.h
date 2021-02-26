/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  LocationReport

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_ILOCATIONRESPONSE_H__
#define __IZAT_MANAGER_ILOCATIONRESPONSE_H__

#include <comdef.h>

namespace izat_manager
{

class LocationReport;
class LocationError;
class ILocationProvider;

class ILocationResponse {

public:
    virtual void reportLocation(const LocationReport *location_report,
        const ILocationProvider* providerSrc = NULL) = 0;
    virtual void reportError(const LocationError *location_error,
        const ILocationProvider* providerSrc = NULL) = 0;

    virtual ~ILocationResponse() {}
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_ILOCATIONRESPONSE_H__
