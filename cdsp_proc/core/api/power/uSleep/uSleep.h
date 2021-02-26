#ifndef _USLEEP_H_
#define _USLEEP_H_
/*==============================================================================
  FILE:         uSleep.h

  OVERVIEW:     Provides the public API interface functions and types for uSleep

  DEPENDENCIES: The build option for uImage / island must be enabled
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/uSleep/uSleep.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "uSleep_lpr.h"

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* uSleep API function error codes */
typedef enum 
{
  USLEEP_SUCCESS = 0,     /* General success */
  USLEEP_FAILURE,         /* General failure*/

  USLEEP_DYN_LPR_NO_ROOM, /* Maximum number of dynamic LPR CB's have already been registered */
}uSleep_status;

typedef enum 
{
  USLEEP_LPR_CB_ENTER = 0,  /* Callback function will run before PC entry or when selecting the mode */
  USLEEP_LPR_CB_EXIT        /* Callback function will run after PC, or when selecting a new mode */ 
}uSleep_lpr_cb_location;

typedef enum
{
  USLEEP_LPR_CB_ATTR_NONE     = 0x00, 
  USLEEP_LPR_CB_ATTR_CACHABLE = 0x01, /* Callback function can be run at active time */
}uSleep_lpr_cb_attr;

/* Routines registered to execute on sleep enter/exit must be of this type */
typedef void (*uSleep_lpr_cb) (uint64 wakeupTick, void *userData);

/*==============================================================================
                      PUBLIC uSLEEP FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * uSleepLPR_registerCallback
 * 
 * @brief Registers a function that is called as part of the power mode entry and/or exit.
 * 
 * @note The registered function will be called for every UNCACHEABLE mode supported.
 * 
 * @param cbName:     String name, used for logging purposes, of the registered callback.
 *                    This should be something useful to identify when your CB is called in the logs
 *                    If NULL is given, a default string name will be used
 * @param func:       Function to call 
 * @param mode:       Low power mode to register the callback to 
 * @param location:   Callback is called either on entry to, or exit from PC. 
 *                    If desired, the same function can be run in both cases by calling the
 *                    API twice with each location flag.
 * @param userData:   Any persistent user data given to the calback function as a parameter.
 *                    The data MUST be located in island if provided
 * @param latency:    Latency value (in ticks) of the time taken to run the callback function 
 * @param attributes: Specific attributes of the callback.  See the enum type for more information 
 *  
 * @return USLEEP_SUCCESS or USLEEP_FAILURE
 */
uSleep_status uSleepLPR_registerCallback(char                   *cbName,
                                         uSleep_lpr_cb          func, 
                                         uSleep_mode_choices    mode,
                                         uSleep_lpr_cb_location location,
                                         void                   *userData, 
                                         uint32                 latency,
                                         uSleep_lpr_cb_attr     attributes);

#ifdef __cplusplus
}
#endif

#endif /* _USLEEP_H_ */

#include "uSleep_mode_trans.h"
