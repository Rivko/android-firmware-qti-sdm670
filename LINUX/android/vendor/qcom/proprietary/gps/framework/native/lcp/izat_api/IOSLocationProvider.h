/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IOSLocationProvider

  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IOSLOCATIONPROVIDER_H__
#define __IZAT_MANAGER_IOSLOCATIONPROVIDER_H__

#include <comdef.h>

namespace izat_manager {

class IzatRequest;
class IIzatListener;

/**
 * @brief IOSLocationProvider interface
 * @details IOSLocationProvider interface;
 *          OS dependent code may provide an implementation of this interface
 *          in designation of its network location provider.
 *
 */
class IOSLocationProvider {

public:

    /**
     * @brief subscribe an instance of IIzatListener
     * @details subscribe an instance of IIzatListener;
     *          Calling this method multiple times each time with
     *          a different izatListener pointer will subscribe
     *          multiple listeners with the provider.
     *
     * @param izatListener Pointer to an instance of IIzatListener
     * @return Zero for success or non zero for failure
     */
    virtual int32 subscribe (IIzatListener * izatListener) = 0;

    /**
     * @brief Sets Location Request
     * @details Sets Location Request;
     *          Multiple calls to this method will
     *          update the request. subscribed
     *          listeners may get location updates
     *          after a call to this method.
     *
     * @param request Pointer to an instance of IzatRequest
     * @return Zero for success or non zero for failure
     */
    virtual int32 setRequest (const IzatRequest * request) = 0;

    /**
     * @brief Stops Location Requests
     * @details Stops Location Requests;
     *          Call to this method will stop provider
     *          activity and listeners will not receive
     *          location updates anymore but they will
     *          remain subscribed. Calling this method
     *          without any active ongoing location requests
     *          will result in a no-op.
     *
     */
    virtual void stopRequest () = 0;

    /**
     * @brief Unsubscribe listener
     * @details Unsubscribe listener;
     *          unsubscribe this listener from location updates.
     *          After call to this method Listener cannot receive
     *          location updates even if there are active requests
     *          with the provider.
     * @param izatListener Pointer to an instance of IIzatListener
     * @return Zero for success or non zero for failure
     */
    virtual int32 unsubscribe (IIzatListener * izatListener) = 0;

    virtual ~IOSLocationProvider () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSLOCATIONPROVIDER_H__
