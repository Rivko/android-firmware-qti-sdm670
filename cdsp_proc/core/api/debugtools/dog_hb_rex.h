#ifndef DOG_HB_REX_H
#define DOG_HB_REX_H
/*
#============================================================================
#  Name:
#    dog_hb_rex.h
#
#  Description:
#    Dog heartbeat monitoring REX APIs
#
# Copyright (c) 2015-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
#============================================================================
*/

#include "stdint.h"
#include "rex.h"
#include "dog_hb.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** =====================================================================
* Function:
*     dog_hb_register_rex
*
* Description:
*     API for clients to register their task for watchdog heartbeat monitoring
*
* Parameters:
*     sig_m : Client supplied signal mask
*
* Returns:
*     dog_report_type : Dog heartbeat handle returned back and should be greater than 0
* =====================================================================  */
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
dog_report_type static inline dog_hb_register_rex(rex_sigs_type sig_m)
{
   rex_tcb_type* tcb_p;
   char task_name[REX_TASK_NAME_LEN + 1];

   tcb_p = rex_self();
   rex_get_task_name(tcb_p, task_name, sizeof(task_name), NULL);
   return dog_hb_register((char const*)task_name, DOG_HB_SIG_REXSIGNAL, tcb_p, sig_m);

}

#if defined(__cplusplus)
}
#endif

#endif
