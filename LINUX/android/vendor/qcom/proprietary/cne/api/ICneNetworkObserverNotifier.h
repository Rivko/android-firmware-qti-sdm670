#ifndef _CNE_NETWORK_OBSERVER_NTFR_H_
#define _CNE_NETWORK_OBSERVER_NTFR_H_
/**
 * @file ICneNetworkObserverNotifier.h
 *
 * @brief
 * Connectivity Engine Network Observer (CNO) Notifier Class Header file
 *
 * This file contains the definitions of constants, data structures and
 * interfaces for the Connectivity Engine Network Observer signal
 * notifications.
 *
 * User must override signal handler methods to process signals delivered by
 * CNO by extending this class.
 *
 *                   Copyright 2010-2012, 2015-2016 Qualcomm Technologies, Inc.
 *                   All Rights Reserved.
 *                   Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <ICneObserverDefs.h>

/**
 * @brief
 * The connectivity Engine Network Observer Notifier class. The user
 * must extend this class with implementation of each signal handler.
 */
class ICneNetworkObserverNotifier
{
  public:
  ICneNetworkObserverNotifier(){};
  virtual ~ICneNetworkObserverNotifier(){};

  /**
   * @addtogroup cno_api
   * @{
   */
  /**
  @brief
  Signal handler to process Connectivity Engine Network Observer network
  configuration change signal

  This method will be called whenever the \c ICneNetworkObserver wants to notify
  regarding network configuration changes to the client, such as: <ul><li> The
  network type has changed. <li> The network subType has changed. <li> The
  network mtu has changed. <li> The network ip address has changed.</ul> The
  client shall override this virtual method to handle \c SIGNAL_NET_CONFIG_CHANGE
  signal notification.

  @param[in] netConfig A reference to a structure of type \c CnoNetConfigType

  @dependencies
  \c ICneNetworkObserver::startSignal must be called for
  \c SIGNAL_NET_CONFIG_CHANGE or \c SIGNAL_NET_ALL in order to receive this.

  @return
  None

  @sa
  \c CneFactory::CreateCneNetworkObserver,
  \c ICneNetworkObserver::startSignal,
  \c ICneNetworkObserver::stopSignal
  */
  virtual void onNetConfigChange(const CnoNetConfigType &netConfig) = 0;

  /**
  @brief
  Signal handler to process Connectivity Engine Network Observer network
  configuration change signal for a particular profile

  This method will be called whenever the \c ICneNetworkObserver wants to notify
  regarding network configuration changes to the client that has previously
  created a \c ICneNetwrorkObserver with the particular profile type, such as:
  <ul><li> The network type has changed. <li> The network subType has changed.
  <li> The network mtu has changed. <li> The network ip address has changed.
  </ul> The client shall override this virtual method to handle
   \c SIGNAL_NET_CONFIG_CHANGE signal notification.

  @param[in] netConfig A reference to a structure of type \c CnoNetConfigType
  @param[in] profile The character string that indicates which profile this
   onNetConfigChange is for.

  @dependencies
  \c ICneNetworkObserver::startSignal must be called for
  \c SIGNAL_NET_CONFIG_CHANGE or \c SIGNAL_NET_ALL in order to receive this.

  @return
  None

  @sa
  \c CneFactory::CreateCneNetworkObserver,
  \c ICneNetworkObserver::startSignal,
  \c ICneNetworkObserver::stopSignal
  */
  virtual void onNetConfigChange (const char * profile,
        const CnoNetConfigType &netConfig){
        #pragma unused (profile, netConfig)
  };
  /**
   * @}
   */
};

#endif /* _CNE_NETWORK_OBSERVER_NTFR_H_ */
