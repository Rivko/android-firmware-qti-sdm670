/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_dal_task.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcinit/src/rcinit_task.c#3 $
$DateTime: 2017/09/27 08:08:43 $
$Change: 14490220 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "err.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
#include "rcinit_dal.h"
#include "rcinit_rex.h"
#elif !defined(RCINIT_EXCLUDE_KERNEL_DAL)
#include "rcinit_dal.h"
#else
#error NHLOS INTERFACE NOT DEFINED PROPERLY
#endif

#include "tms_rcinit.h"
#include "sys_m_internal.h"

/*===========================================================================

 FUNCTION rcinit_task

 DESCRIPTION
 hlos task thread to process sequnece groups

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
void rcinit_task(unsigned long arg_p)
#elif !defined(RCINIT_EXCLUDE_KERNEL_DAL)
DALResult rcinit_task(DALSYSEventHandle hEvent, void* arg_p)
#endif
{
   RCEVT_SIGEX_SIGDAL sig;
   DALSYSEventHandle hEventShutdownTask;

   rcinit_internal.process_state = rcinit_process_init;

   DALSYS_InitMod(NULL);                                                         // prerequsite dependency

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEventShutdownTask, NULL))
   {
      ERR_FATAL("create event shutdown", 0, 0, 0);
   }

   // rcinit init
   rcinit_init();

   // register as client to system monitor
   sig.signal = hEventShutdownTask;
   if (rcevt_register_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGDAL, &sig)) {;}
   if (rcecb_signal_handle(rcecb_create_name(RCINIT_RCECB_INITGROUPS))) {;}
   if (rcesn_signal_handle(rcesn_create_name(RCINIT_RCESN_INITGROUPS), 1)) {;}
   if (rcevt_signal_handle(rcevt_create_name(RCINIT_RCEVT_INITGROUPS))) {;}

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
   tracer_event_simple(RCINIT_SWE_INITGROUPS);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   TMS_STARTUP_MSG_HIGH("Group initializations end");
#endif

   rcinit_internal.process_state = rcinit_process_blocking;

   // block for system monitor notification
   if (NULL == hEventShutdownTask || DAL_SUCCESS != DALSYS_EventWait(hEventShutdownTask))
   {
      ERR_FATAL("task wait event shutdown", 0, 0, 0);
   }

   rcinit_internal.process_state = rcinit_process_running;

   // unregister further system monitor notifications
   if (rcevt_unregister_sigex_name(SYS_M_SHUTDOWN, RCEVT_SIGEX_TYPE_SIGDAL, &sig)) {;}

   if (NULL == hEventShutdownTask || DAL_SUCCESS != DALSYS_EventCtrl(hEventShutdownTask, DALSYS_EVENT_CTRL_RESET))
   {
      ERR_FATAL("task wait event shutdown", 0, 0, 0);
   }

   if (NULL == hEventShutdownTask || DAL_SUCCESS != DALSYS_DestroyObject(hEventShutdownTask))
   {
      ERR_FATAL("create event shutdown", 0, 0, 0);
   }

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
   tracer_event_simple(RCINIT_SWE_TERMGROUPS);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   TMS_SHUTDOWN_MSG_HIGH("Group terminations begin");
#endif

   if (rcevt_signal_handle(rcevt_create_name(RCINIT_RCEVT_TERMGROUPS))) {;}
   if (rcesn_signal_handle(rcesn_create_name(RCINIT_RCESN_TERMGROUPS), 1)) {;}
   if (rcecb_signal_handle(rcecb_create_name(RCINIT_RCECB_TERMGROUPS))) {;}

   // rcinit termination
   rcinit_term();

   // client ack to system monitor
   if (rcevt_signal_name(SYS_M_SHUTDOWN_ACK)) {;}

   // termination
   rcinit_internal.process_state = rcinit_process_term;

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
   if (arg_p) {;}
#elif !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   if (arg_p) {;}
   if (hEvent) {;}
   return DAL_SUCCESS;
#endif
}

void rcinit_fail(void)
{
   static unsigned long unused = 0;

   for (;;)                                                                      // punt, no exception handler
   {
   }

   /* NOTREACHED */

   if (unused) {;}
}

#define dwMaxNumEvents        1
#define wlPriority            rcinit_lookup_prio("rcinit")
#define wlStackSize           rcinit_lookup_stksz("rcinit")

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)

void bootstrap_rcinit_task(void)
{
   static rex_tcb_type tcb;                                                      // static allocation for boot strapping
   static rex_stack_word_type stack[6144 / sizeof(rex_stack_word_type)];         // static allocation for boot strapping

   rcinit_internal_tls_create_key(&rcinit_internal.tls_key, RCINIT_NULL);
   
   // Initialize NHLOS Layers Needed

   // Must initialize the kernel REX API Layer; its not done by kernel
   // even on PL where the interface is available, and we are already
   // multi-tasking at this point by the NHLOS.

#if !defined(RCINIT_EXCLUDE_KERNEL_REX_Q6)

   rex_init(REX_ANY_CPU_AFFINITY_MASK, NULL, 0, &tcb,
            stack, sizeof(stack) / sizeof(rex_stack_word_type), wlPriority,
            rcinit_task, 0);

#elif !defined(RCINIT_EXCLUDE_KERNEL_REX_ARM)

   rex_init(NULL, 0, &tcb,
            stack, sizeof(stack) / sizeof(rex_stack_word_type), wlPriority,
            rcinit_task, 0);

#endif

   // ALWAYS RETURNS
}

#elif !defined(RCINIT_EXCLUDE_KERNEL_DAL)

void bootstrap_rcinit_task(void)
{
   DALSYSEventHandle hEvent;                                                     // context "lost" on scope exit, reclaimed at task
   DALSYSWorkLoopHandle hWorkLoop;                                               // context "lost" on scope exit, but remains active, not reclaimed

   // Initialize NHLOS Layers Needed

   DALSYS_InitMod(NULL);                                                         // prerequsite dependency

   if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &hEvent, NULL) ||
       DAL_SUCCESS != DALSYS_RegisterWorkLoopEx("rcinit", wlStackSize, wlPriority, dwMaxNumEvents, &hWorkLoop, NULL) ||
       NULL == hWorkLoop || NULL == hEvent ||
       DAL_SUCCESS != DALSYS_AddEventToWorkLoop(hWorkLoop, rcinit_task, NULL, hEvent, NULL) ||
       DAL_SUCCESS != DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_TRIGGER))
   {
      rcinit_fail();
   }

   // ALWAYS RETURNS
}

#endif
