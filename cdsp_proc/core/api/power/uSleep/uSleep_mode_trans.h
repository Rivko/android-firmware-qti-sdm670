#ifndef _USLEEP_MODE_TRANS_H_
#define _USLEEP_MODE_TRANS_H_
/*==============================================================================
  FILE:         uSleep_mode_trans.h

  OVERVIEW:     Provides the public API functions and types to support
                normal <-> island transitions

  DEPENDENCIES: The build option for island must be enabled
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/uSleep/uSleep_mode_trans.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep.h"

/*==============================================================================
                         USLEEP ENTRY USER CLIENT NODE
 =============================================================================*/
/* User client NPA node name used to support normal <-> island transitions. */
#define USLEEP_NODE_NAME                   "/core/uSleep"

/* Valid required requests for the user client node */
#define USLEEP_CLIENT_RESTRICT_ISLAND      0
#define USLEEP_CLIENT_ALLOW_ISLAND         1

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* Enumeration for the uSleep notification callback function parameter */
typedef enum uSleep_state_notification
{
  USLEEP_STATE_ENTER = 0,  /* uSleep mode is about to be entered */
  USLEEP_STATE_EXIT        /* uSleep mode has been exited */
}uSleep_state_notification;

/* Handle to registered callback notification function */
typedef uint32 uSleep_notification_cb_handle; 

/* Callback notification function prototype */
typedef void (*uSleep_notification_cb_type)(uSleep_state_notification state);

/*==============================================================================
                      MAIN uSLEEP FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * uSleep_exit
 * 
 * @brief Explicitly exit uSleep mode. The caller will be blocked while DDR is
 *        enabled and the exit process continues at the next idle period.
 * 
 * @note This function only exits uSleep mode, it will not disable it and
 *       uSleep could be entered again on the next sleep cycle.
 *       It is only intended to be called while in uImage mode; calling it
 *       in normal operational mode has no effect.
 * 
 * @return USLEEP_SUCCESS or USLEEP_FAILURE
 */
uSleep_status uSleep_exit(void);

/** 
 * uSleep_exitOnError
 * 
 * @brief Explicitly exit uSleep mode without calling function callbacks or 
 *        performing any of the normal exit routines.  It will block the
 *        caller while the exit process occurs.
 *  
 * @note This function is intended to ONLY be called from the OS in a 
 *       FATAL ERROR case. It does not allow reentry into any type of sleep
 *       modes and will loop forever if system is allowed to enter the idle
 *       state again.
 *  
 *       It is only intended to be called while in uImage mode.
 */
void uSleep_exitOnError(void);

/** 
 * uSleep_registerNotificationCallback
 * 
 * @brief Registers a client callback function for notification when uSleep 
 *        is entered or exited 
 *  
 * @note  This function should be called only once during initialization, while 
 *        in normal operational mode.
 *  
 *        The registered function will be called while in single threaded
 *        mode and must not invoke other threads or otherwise exit STM.
 * 
 *        The registered function should be as minial as possible to keep the
 *        transtion times as fast as possible and must not assert a signal, block
 *        or otherwise allow a context switch.
 *     
 *  
 * @param enterLat: Time in ticks taken to execute the CB when entering uImage
 * @param exitLat:  Time in ticks taken to execute the CB when exiting uImage
 * @param callback: Function to be called
 *  
 * @return Handle of registered function; 0 = error
 */
uSleep_notification_cb_handle uSleep_registerNotificationCallback(uint32                      enterLat,
                                                                  uint32                      exitLat,
                                                                  uSleep_notification_cb_type callback);

/** 
 * uSleep_deregisterNotificationCallback
 * 
 * @brief Deregisters a client callback function from the transition notification that was 
 *        previously registered 
 *  
 * @param Handle of previously registered function 
 *  
 * @return function status 
 */
uSleep_status uSleep_deregisterNotificationCallback(uSleep_notification_cb_handle handle);

#endif /* _USLEEP_MODE_TRANS_H_ */

