#ifndef DOG_HB_QURT_H
#define DOG_HB_QURT_H
/*
#============================================================================
#  Name:
#    dog_hb_qurt.h
#
#  Description:
#    Dog heartbeat monitoring QURT APIs
#
# Copyright (c) 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
#============================================================================
*/

#include "stdint.h"
#include "qurt.h"
#include "dog_hb.h"
#include "tms_dll_api.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** =====================================================================
* Function:
*     dog_hb_register_qurt
*
* Description:
*     API for clients to register their task for watchdog heartbeat monitoring
*
* Parameters:
*     sig_p : Client supplied Qurt signal
*     sig_m : Client supplied signal mask
*
* Returns:
*     dog_report_type : Dog heartbeat handle returned back and should be greater than 0
* =====================================================================  */
DLL_API_WARN_UNUSED_RESULT
dog_report_type static inline dog_hb_register_qurt(qurt_anysignal_t* sig_p, unsigned int sig_m)
{
   char task_name[QURT_MAX_NAME_LEN];

   qurt_thread_get_name(task_name, sizeof(task_name));

   return(dog_report_type)dog_hb_register((char const*)task_name, DOG_HB_SIG_QURTSIGNAL, sig_p, sig_m);
}

#if defined(__cplusplus)
}
#endif

#endif
