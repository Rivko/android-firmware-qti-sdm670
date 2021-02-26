/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIzatManager

  Copyright  (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIZATMANAGER_H__
#define __IZAT_MANAGER_IIZATMANAGER_H__

#include <comdef.h>
#include "IzatTypes.h"
#include "IzatRequest.h"

namespace izat_manager {

class IOSFramework;
class IOSListener;
class IIzatManager;

/**
 * @brief Gets an instance of type IIzatManager
 * @details This method is invoked to initialize OS Independent Izat subsystem
 *
 * @param osFramework Pointer to an instance of IOSFramework
 * @return Pointer to an instance of IIzatManager
 */
IIzatManager * getIzatManager (IOSFramework * osFramework);

/**
 * @brief Destroy instance of type IIzatManager
 * @details This method is invoked to destroy instance of type IIzatManager and to de-initialize the subsystem
 * @return Zero for success or non zero for failure
 */
int destroyIzatManager ();


/**
 * @brief IIzatManager interface
 * @details IIzatManager interface allows the OS/framework to subscribe/unsubscribe listeners,
 *          add/remove requests and enable/disable providers.
 *          The order of invocation of the methods in this interface is as follows:
 *          subscribeListener, enableProvider, addRequest, removeRequest, disableProvider, unsubscribeListeners
 */
class IIzatManager {

public:

    typedef IzatStreamType IzatProviderType;

    /**
     * @brief Subscribe a listener of type IOSListener
     * @details Subscribe a listener of type IOSListener
     *
     * @param osLocationListener Pointer to an instance of IOSListener
     * @return Zero for success or non zero for failure
     */
    virtual int32 subscribeListener (IOSListener * osLocationListener) = 0;

    /**
     * @brief Unsubscribe a listener of type IOSListener
     * @details Unsubscribe a listener of type IOSListener
     *
     * @param osLocationListener Pointer to an instance of IOSListener
     * @return Zero for success or non zero for failure
     */
    virtual int32 unsubscribeListener (IOSListener * osLocationListener) = 0;

    /**
     * @brief Add an IzatRequest
     * @details Add an IzatRequest;
     *          A subscribed OS dependent instance of IOSListener may start receiving location updates
     *          after a call to this method. The added request could be either provider based or criteria based.
     *
     * @param request Pointer to an instance of IzatRequest
     * @return Zero for success or non zero for failure
     */
    virtual int32 addRequest (const IzatRequest * request) = 0;

    /**
     * @brief Remove an IzatRequest
     * @details Remove an IzatRequest;
     *          A subscribed framework provider may stop receiving location updates after a call to this method.
     *          The removed request could be either provider based or criteria based.
     *
     * @param request Pointer to an instance of IzatRequest
     * @return Zero for sucess and non zero for failure
     */
    virtual int32 removeRequest (const IzatRequest * request) = 0;

    /**
     * @brief Enable location provider
     * @details Enable location provider
     *
     * @param provider Instance of IzatProviderType
     * @return Zero for sucess and non zero for failure
     */
    virtual int32 enableProvider (const IzatProviderType provider) = 0;

    /**
     * @brief Disable location provider
     * @details Disable location provider
     *
     * @param provider Instance of IzatProviderType
     * @return Zero for sucess and non zero for failure
     */
    virtual int32 disableProvider (const IzatProviderType provider) = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IIzatManager () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIZATMANAGER_H__
