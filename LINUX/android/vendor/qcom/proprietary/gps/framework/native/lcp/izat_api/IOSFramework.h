/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IOSFramework

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IOSFRAMEWORK_H__
#define __IZAT_MANAGER_IOSFRAMEWORK_H__

#include <MsgTask.h>
#include <IFrameworkActionReq.h>
#include <IDataItemObserver.h>
#include <IDataItemSubscription.h>

namespace loc_core
{
class IFrameworkActionReq;
}

using loc_core::IFrameworkActionReq;
using loc_core::IDataItemObserver;
using loc_core::IDataItemSubscription;

namespace izat_manager {

class IOSLocationProvider;
class IOSFrameworkCleaner;

/**
 * @brief IOSFramework interface
 * @details Definition of an interface to be implemented by OS dependent code.
 *          An instance of an implementation of this interface is supplied to the Izat Subsystem
 *          at its initialization time.
 */
class IOSFramework {

public:

    /**
     * @brief Gets an instance of IDataItemSubscription
     * @details Gets an instance of IDataItemSubscription
     * @return Pointer to an instance of IDataItemSubscription
     */
    virtual IDataItemSubscription * getDataItemSubscriptionObj () = 0;

    /**
     * @brief Gets an instance of IFrameworkActionReq
     * @details Gets an instance of IFrameworkActionReq
     * @return Pointer to an instance of IFrameworkActionReq
     */
    virtual IFrameworkActionReq * getFrameworkActionReqObj () = 0;

    /**
     * @brief Gets an instance of IOSLocationProvider
     * @details Gets an instance of IOSLocationProvider;
     *          This instance is the OS dependent instance of a network location provider
     * @return Pointer to an instance of IOSLocationProvider or NULL if OS network location provider does not exist.
     */
    virtual IOSLocationProvider * getOSNP () = 0;


    /**
     * @brief Gets an instance of IOSFrameworkCleaner
     * @details Gets an instance of IOSFrameworkCleaner;
     *          This instance of IOSFrameworkCleaner is responsible for freeing up memory in OS dependent code.
     * @return Pointer to an instance of IOSFrameworkcleaner
     */
    virtual IOSFrameworkCleaner * getOSFrameworkCleaner () = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IOSFramework   () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSFRAMEWORK_H__
