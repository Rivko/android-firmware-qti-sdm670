#ifndef DOG_HB_H
#define DOG_HB_H
/*
#============================================================================
#  Name:
#    dog_hb.h
#
#  Description:
#    Watchdog heartbeat monitoring
#
# Copyright (c) 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
#============================================================================
*/

#include "stdint.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef unsigned int dog_report_type;

typedef enum
{
   DOG_HB_SIG_REXSIGNAL = 1,              /* Argument Supplied for REX API */
   DOG_HB_SIG_QURTSIGNAL = 2              /* Argument Supplied for QURT ANYSIGNAL API */

} DOG_HB_SIG;

/** =====================================================================
* Function:
*     dog_hb_register
*
* Description:
*     Registers clients for watchdog hb monitoring
*
* Parameters:
*     task_name : Client task name
*     sig_t     : Client supplied signal type information. Qurt or Rex signal type
*     sig_p     : Client supplied signal information. Qurt or Rex signal
*     sig_m     : Client supplied mask information.
*
* Returns:
*     dog_report_type : Dog heartbeat handle returned back and should be greater than 0
* =====================================================================  */
dog_report_type dog_hb_register(char const* task_name, DOG_HB_SIG sig_t, void* sig_p, unsigned int sig_m);

/** =====================================================================
* Function:
*     dog_hb_report
*
* Description:
*     API for clients to report to watchdog heartbeat.
*     Reports to the watchdog whether the monitored task is
*     functioning properly.
*
* Parameters:
*     ident : Dog handle used to tell the watchdog which task is reporting.
*
* Returns:
*     none
* =====================================================================  */
void dog_hb_report(dog_report_type ident);

/** =====================================================================
* Function:
*     dog_hb_deregister
*
* Description:
*     Dynamically de-registers tasks for watchdog heartbeat services.
*
* Parameters:
*     ident : Handle that the watchdog uses to identify the task.
*
* Returns:
*     none
* =====================================================================  */
void dog_hb_deregister(dog_report_type ident);

#if defined(__cplusplus)
}
#endif

#endif
