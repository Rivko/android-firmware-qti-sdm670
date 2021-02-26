#ifndef _CNE_NETWORK_REQUEST_H_
#define _CNE_NETWORK_REQUEST_H_
/**
 * @file ICneNetworkRequest.h
 *
 * @brief Connectivity Engine Network Request Class
 *        (Abstract)
 *
 * This file contains the definitions of constants, data structures and
 * interfaces for the Connectivity Engine Network Request
 *
 * Only the interfaces declared shall be used by the user of Connectivity
 * Engine Network Request
 *
 *                   Copyright 2015 Qualcomm Technologies, Inc.
 *                   All Rights Reserved.
 *                   Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <ICneObserverDefs.h>

/**
 * @brief
 * Connectivity Engine Network Request interface class (abstract).
 */
class ICneNetworkRequest
{
  public:

  /**
   * @}
   */
  /**
   * @addtogroup cnr_api
   * @{
   */

  /**
  @brief
  gets active network configuration.

  This method returns to the caller the network type, subtype, ip
  address, mtu size, default network and netId indication.

  @param[out] netConfig reference to NetConfigRspStruct

  @dependencies
  \c ICneNetworkRequest must be created

  @return One of the following codes
          <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNR service error
            <li> \c CO_RET_PERM_DENIED - client does not have sufficient permission
            <li> \c CO_RET_INVALID_ARGS - invalid params
            <li> \c CO_RET_FAIL - general failure
          </ul>
  @sa
  \c CneFactory::CreateCneNetworkRequest
  */
  virtual CORetType getNetConfig (CnoNetConfigType& netConfig) = 0;

  /**
  @brief
  brings up a network

  This method brings up the network associated with the request.

  @dependencies
  \c ICneNetworkRequest must be created

  @return One of the following codes
          <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNR service error
            <li> \c CO_RET_PERM_DENIED - client does not have sufficient permission
            <li> \c CO_RET_FAIL - general failure
          </ul>
  @sa
  \c CneFactory::CreateCneNetworkRequest
  */
  virtual CORetType bringUp () = 0;

  /**
  @brief
  tears down a network

  This method tears down the network associated with the request.

  @dependencies
  \c ICneNetworkRequest must be created

  @return One of the following codes
          <ul>
            <li> \c CO_RET_SUCCESS - Success
            <li> \c CO_RET_SERVICE_UNAVAILABLE - CNR service error
            <li> \c CO_RET_PERM_DENIED - client does not have sufficient permission
            <li> \c CO_RET_FAIL - general failure
          </ul>
  @sa
  \c CneFactory::CreateCneNetworkRequest
  */
  virtual CORetType tearDown () = 0;

   /**
   @brief
   Destructor.
   */
   virtual ~ICneNetworkRequest(){};

};

#endif /* _CNE_NETWORK_REQUEST_H_ */
