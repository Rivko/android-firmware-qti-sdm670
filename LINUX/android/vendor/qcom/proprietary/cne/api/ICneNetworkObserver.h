#ifndef _CNE_NETWORK_OBSERVER_H_
#define _CNE_NETWORK_OBSERVER_H_
/**
 * @file ICneNetworkObserver.h
 *
 * @brief Connectivity Engine Network Observer Class (Abstract)
 *
 * This file contains the definitions of constants, data structures and
 * interfaces for the Connectivity Engine Network Observer
 *
 * Only the interfaces declared shall be used by the user of Connectivity
 * Engine Network Observer
 *
 *                   Copyright 2010-2012, 2015-2016 Qualcomm Technologies, Inc.
 *                   All Rights Reserved.
 *                   Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <ICneObserverDefs.h>

/**
 * @brief
 * Connectivity Engine Network Observer interface class (abstract).
 */
class ICneNetworkObserver
{
  public:

  /**
   * @}
   */
  /**
   * @addtogroup cno_api
   * @{
   */

  /**
  @brief
  gets active network configuration.

  This method returns to the caller the active network type, subtype, ip
  address, mtu size, and default network indication. If no network is active
  then the call will fail. ip address 0 will signify that the network is not
  connected (i.e not ready for data.)

  @param[out] netConfig reference to NetConfigRspStruct

  @dependencies
  \c ICneNetworkObserver must be created

  @return One of the following codes
          <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNO service error
            <li> \c CO_RET_PERM_DENIED - client does not have sufficient permission
            <li> \c CO_RET_INVALID_ARGS - invalid params
            <li> \c CO_RET_NET_INACTIVE - no active default network
            <li> \c CO_RET_FAIL - general failure
          </ul>
  @sa
  \c CneFactory::CreateCneNetworkObserver
  */
  virtual CORetType getNetConfig (CnoNetConfigType& netConfig) = 0;

 /**
  @brief
  Gets supplemental information for wlan interface

   This method is used to retrieve additional wlan related information.
   It supplements \c getNetConfig. If clients attempt to call
   this method in the absense of wlan,\c CO_RET_SERVICE_UNAVAILABLE
   will be returned.

  @param[out] netConfig reference to CnoWlanSuppNetConfigType

  @dependencies
  \c ICneNetworkObserver must be created and interface must be
     available

  @return One of the following codes
          <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNO service error
            <li> \c CO_RET_INVALID_ARGS - invalid params
            <li> \c CO_RET_PERM_DENIED - permission denied
            <li> \c CO_RET_NET_INACTIVE - wlan network is unavailable
          </ul>
  @sa
  \c CneFactory::CreateCneNetworkObserver
  */
   virtual CORetType getWlanSuppNetConfig(CnoWlanSuppNetConfigType& netConfig) = 0;

   /**
   @brief
   Starts signal notification for the given signal identifier

   Starts aynchronous notification of signals as denoted by \c startSig. The
   client can enable all signals by setting \c startSig to \c SIGNAL_NET_ALL.

   @param[in] startSig signal identifier of the form \c CnoSignalType

   @dependencies
   A \c ICneNetworkObserverNotifier must be passed during creation of this \c
   ICneNetworkObserver for startSignal to succeed.

   @return One of the following codes
           <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNO service error
            <li> \c CO_RET_INVALID_ARGS - invalid params
            <li> \c CO_RET_NOT_ALLOWED - operation not allowed at the moment
           </ul>

   @sa
   \c ICneNetworkObserverNotifier,
   \c CneFactory::CreateCneNetworkObserver,
   \c ICneNetworkObserver::stopSignal
   */
   virtual CORetType startSignal (CnoSignalType startSig) = 0;

   /**
   @brief
   Stops sending signals for the given signal identifier

   Stops asynchronous notification of signals as denoted by \c cnoSignalType. The
   client can disable all signals by setting \c stopSig to \c SIGNAL_NET_ALL.

   @param[in] stopSig identifier of the form CnoSignalType

   @dependencies
   A \c ICneNetworkObserverNotifier must be passed during creation of this \c
   ICneNetworkObserver for stopSignal to succeed.

   @return One of the following codes
       <ul>
         <li> \c CO_RET_SUCCESS - Success
         <li> \c CO_RET_SERVICE_UNAVAILABLE - CNO service error
         <li> \c CO_RET_INVALID_ARGS - invalid params
         <li> \c CO_RET_NOT_ALLOWED - operation not allowed at the moment
       </ul>

   @sa
   \c ICneNetworkObserverNotifier,
   \c CneFactory::CreateCneNetworkObserver,
   \c ICneNetworkObserver::startSignal
   */
   virtual CORetType stopSignal (CnoSignalType stopSig) = 0;

   /**
   @brief
     request for enabling precise sampling

   @return void

   @sa
   \c ICneNetworkObserverNotifier,
   \c CneFactory::CreateCneNetworkObserver
   */
   virtual void enablePreciseSampling() = 0;
   /**
   @brief
     request for disabling precise sampling

   @return void

   @sa
   \c ICneNetworkObserverNotifier,
   \c CneFactory::CreateCneNetworkObserver
   */
   virtual void disablePreciseSampling() = 0;
   /**
    * @}
    */

   /**
   @brief
   Destructor.
   */
   virtual ~ICneNetworkObserver(){};

  };

#endif /* _CNE_NETWORK_OBSERVER_H_ */
