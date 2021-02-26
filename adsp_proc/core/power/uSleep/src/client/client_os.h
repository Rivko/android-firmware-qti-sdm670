#ifndef _CLIENT_OS_H_
#define _CLIENT_OS_H_
/*==============================================================================
  FILE:         client_os.h

  OVERVIEW:     uSleep guest client types

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/client/client_os.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleep_registerUserNotificationCallback
 * 
 * @brief Function that is called once during user PD callback registration
 *        that will add a guest callback to signal the user callbacks to
 *        execute
 * 
 * @param signals: Handshake signals used to coordinate the guest-user callback
 *                 system. These signals are specific to each seperate user PD.
 * 
 * @return USLEEP_SUCCESS or USLEEP_FAILURE 
 */
uSleep_status uSleep_registerUserNotificationCallback(uSleep_signals *signals);

/**
 * uSleep_deregisterUserNotificationCallback
 * 
 * @brief Deregisters the user callback signal on PD restart
 * 
 * @return USLEEP_SUCCESS or USLEEP_FAILURE 
 */
uSleep_status uSleep_deregisterUserNotificationCallback(uSleep_signals  *signals,
                                                        uint32          enterLat,
                                                        uint32          exitLat);

/**
 * uSleep_addUserCallbackLatency 
 *  
 * @brief Helper function that adds the enter/exit latencies for a callback 
 *        function
 *  
 * @note  This function should not be called directly, it will be called 
 *        appropriately via the registration API
 * 
 * @param enterLat: Enter latency (in ticks) of the callback being registered 
 * @param exitLat:  Exit latency (in ticks) of the callback being registered 
 *  
 * @return USLEEP_SUCCESS or USLEEP_FAILURE 
 */
uSleep_status uSleep_addUserCallbackLatency(int32 enterLat, int32 exitLat);

/** 
 * uSleep_guestClientInit
 * 
 * @brief Initializes the guest PD client data
 */
void uSleep_guestClientInit(void);

#endif /* _CLIENT_OS_H_*/

