/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_rex_term.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcinit/src/rcinit_term.c#2 $
$DateTime: 2017/09/01 01:33:34 $
$Change: 14246640 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "err.h"
#include "timer.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
#include "rcevt_qurt.h"
#include "rcinit_qurt.h"
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
#include "rcevt_rex.h"
#include "rcinit_rex.h"
#else
#error NHLOS INTERFACE NOT DEFINED PROPERLY
#endif

#include "tms_rcinit.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
extern const tracer_event_id_t rcinit_swe_event_term[RCINIT_GROUP_MAX];          // internal
#endif

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/*===========================================================================

 FUNCTION rcinit_register_term_group

 DESCRIPTION
 register for termination notification and handshake back expectations

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_register_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   unsigned long task_hash;
   char task_name[RCINIT_NAME_MAX];
   rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

   if (RCINIT_NULL == rcinit_p)
   {
      task_hash = rcinit_internal_task_name(task_name, sizeof(task_name));

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Register by task %s # %x not started from rcinit grp %x", task_name, task_hash, group);
#endif
   }

   else
   {
      task_hash = rcinit_p->hash;

      strlcpy(task_name, rcinit_p->name, sizeof(task_name));
   }

   if (RCINIT_GROUP_7 >= group)
   {
      if (RCINIT_TASK_MAX == rcinit_internal_hs_list_add(rcinit_internal.hs_term, group, task_name))
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_RI, 2, group, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_ERROR_2("Register task # %x ignored grp %x", task_hash, group);
#endif
      }

      else
      {
         if (RCEVT_NULL != rcevt_register_sigex_name(rcinit_term_rcevt[group], type, sigex))
         {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
            tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_R, 2, group, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
            TMS_SHUTDOWN_MSG_HIGH_2("Register task # %x grp %x", task_hash, group);
#endif
         }
      }
   }
}

void rcinit_unregister_term_group(RCINIT_GROUP group, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   unsigned long task_hash;
   char task_name[RCINIT_NAME_MAX];
   rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

   if (RCINIT_NULL == rcinit_p)
   {
      task_hash = rcinit_internal_task_name(task_name, sizeof(task_name));

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Unregister by task %s # %x not started from rcinit grp %x", task_name, task_hash, group);
#endif
   }

   else
   {
      task_hash = rcinit_p->hash;

      strlcpy(task_name, rcinit_p->name, sizeof(task_name));
   }

   if (RCINIT_GROUP_7 >= group)
   {
      // decision: the unregister does not remove the entry from the handshake table (no hs_list_del)
      // to facilitate trace32 debugging

      if (RCEVT_NULL != rcevt_unregister_sigex_name(rcinit_term_rcevt[group], type, sigex))
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_U, 2, group, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_HIGH_2("Unregister task # %x grp %x", group, task_hash);
#endif
      }
   }
}

/*===========================================================================

 FUNCTION rcinit_register_termfn_group

 DESCRIPTION
 register for termination notification and no handshake back expectations

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_register_termfn_group(RCINIT_GROUP group, void (*fn)(void))
{
   unsigned long task_hash;
   char task_name[RCINIT_NAME_MAX];
   rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

   if (RCINIT_NULL == rcinit_p)
   {
      task_hash = rcinit_internal_task_name(task_name, sizeof(task_name));

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Register termfn %s # %x not started from rcinit grp %x", task_name, task_hash, group);
#endif
   }

   else
   {
      task_hash = rcinit_p->hash;

      strlcpy(task_name, rcinit_p->name, sizeof(task_name));
   }

   if (RCINIT_GROUP_7 >= group)
   {
      if (RCECB_NULL != rcecb_register_name(rcinit_term_rcecb[group], fn))
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_FUNC_R, 2, group, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_HIGH_2("Register termfn # %x grp %x", task_hash, group);
#endif
      }
   }
}

void rcinit_unregister_termfn_group(RCINIT_GROUP group, void (*fn)(void))
{
   unsigned long task_hash;
   char task_name[RCINIT_NAME_MAX];
   rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

   if (RCINIT_NULL == rcinit_p)
   {
      task_hash = rcinit_internal_task_name(task_name, sizeof(task_name));

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Unregister termfn %s # %x not started from rcinit grp %x", task_name, task_hash, group);
#endif
   }

   else
   {
      task_hash = rcinit_p->hash;

      strlcpy(task_name, rcinit_p->name, sizeof(task_name));
   }

   if (RCINIT_GROUP_7 >= group)
   {
      if (RCECB_NULL != rcecb_unregister_name(rcinit_term_rcecb[group], fn))
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_FUNC_U, 2, group, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_HIGH_2("Unregister termfn # %x grp %x", task_hash, group);
#endif
      }
   }
}

void rcinit_register_term(RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rcinit_register_term_group(RCINIT_GROUP_7, type, sigex);
}

void rcinit_unregister_term(RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rcinit_unregister_term_group(RCINIT_GROUP_7, type, sigex);
}

void rcinit_register_termfn(void (*fn)(void))
{
   rcinit_register_termfn_group(RCINIT_GROUP_7, fn);
}

/*===========================================================================

 FUNCTION rcinit_handshake_term

 DESCRIPTION
 client called api to handshake during termination

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none
 ===========================================================================*/
void rcinit_handshake_term(void)
{
   unsigned long task_hash;
   char task_name[RCINIT_NAME_MAX];
   rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

   if (RCINIT_NULL == rcinit_p)
   {
      task_hash = rcinit_internal_task_name(task_name, sizeof(task_name));

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Handshake by task %s %x not started from rcinit grp %x", task_name, task_hash, rcinit_internal.group_curr);
#endif
   }

   else
   {
      task_hash = rcinit_p->hash;

      strlcpy(task_name, rcinit_p->name, sizeof(task_name));
   }

   // LOGIC TO FILTER WHEN A HS IS ACCEPTED

   if (RCINIT_GROUP_7 >= rcinit_internal.group_curr &&                           // we accept a HS only in valid GROUP
       (rcinit_process_group_term_task == rcinit_internal.process_state ||       // we accept a HS during task term event delivery
        rcinit_process_group_term_blocking == rcinit_internal.process_state      // we accept a HS during blocking task term event ACK
        )
       )
   {
      // check for ignored handshake, and report that as an error (not fatal)

      if (RCINIT_TASK_MAX == rcinit_internal_hs_list_hs(rcinit_internal.hs_term, rcinit_internal.group_curr, task_name))
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_HI, 2, rcinit_internal.group_curr, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_ERROR_2("Handshake ignored task # %x grp %x", task_hash, rcinit_internal.group_curr);
#endif
      }

      else
      {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_HS, 2, rcinit_internal.group_curr, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_ERROR_2("Handshake task # %x grp %x", task_hash, rcinit_internal.group_curr);
#endif
         if (rcevt_signal_name(RCINIT_RCEVT_TERMACK)) {;}
      }
   }

   else
   {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
      tracer_event_simple_vargs(RCINIT_SWE_TERM_TASK_HI, 2, rcinit_internal.group_curr, task_hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_SPRINTF_ERROR_3("Handshake discarded task %s # %x grp %x", task_name, task_hash, rcinit_internal.group_curr);
#endif
   }
}

void rcinit_handshake_shutdown(void)
{
   rcinit_handshake_term();
}

/*===========================================================================

 FUNCTION rcinit_internal_unprocess_groups

 DESCRIPTION
 task and termfn processing

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_internal_unprocess_groups(void)
{
   RCINIT_GROUP group;
   static timer_type tmo = { 0 };
   static timer_type bail = { 0 };
   static timer_group_type timer_group = { 0 };

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_anysignal_t anysignal;
   RCEVT_SIGEX_SIGQURT rcevt_sigex;
   qurt_anysignal_init(&anysignal);
   rcevt_sigex.signal = &anysignal;
   rcevt_sigex.mask = SIG_ACK;
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   RCEVT_SIGEX_SIGREX rcevt_sigex;
   rcevt_sigex.signal = rex_self();
   rcevt_sigex.mask = SIG_ACK;
#endif

   // define the tmo & bail timer

   timer_group_set_deferrable(&timer_group, FALSE);

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   timer_def_osal(&tmo, &timer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &anysignal, SIG_TMO);
   timer_def_osal(&bail, &timer_group, TIMER_NATIVE_OS_SIGNAL_TYPE, &anysignal, SIG_BAIL);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   timer_def_osal(&tmo, &timer_group, TIMER_REX_TCB_SIGNAL_TYPE, rex_self(), SIG_TMO);
   timer_def_osal(&bail, &timer_group, TIMER_REX_TCB_SIGNAL_TYPE, rex_self(), SIG_BAIL);
#endif

   timer_group_enable(&timer_group);

   // Overall Process Bailout Timer Starts

   if (0 != rcinit_term_timeout)                                                 // condition check used to start bailout timer
   {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_HIGH_1("Bailout timer set %d", rcinit_term_timeout);
#endif

      // set the bail out timer

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
      qurt_anysignal_clear(&anysignal, SIG_BAIL);                                        // clear signals
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
      rex_clr_sigs(rex_self(), SIG_BAIL);                                        // clear signals
#endif

      timer_set_64(&bail, rcinit_term_timeout, 0, T_MSEC);
   }

   // Per Group Processing, Reverse Order from Startup [ RCINIT_GROUP_7 .. RCINIT_GROUP_0 ]

   group = RCINIT_GROUP_MAX;

   do
   {
      int count;                                                                 // registered count
      RCEVT_THRESHOLD ackcnt;                                                    // starting ack count
      RCEVT_THRESHOLD current;
      unsigned long timetick;

      group--;

      // Termination Notification to Tasks Begins
      // HS Accepting On -- Checked in rcinit_handshake_term

      rcinit_internal.group_curr = group;                                        // processing group
      rcinit_internal.process_state = rcinit_process_group_term_task;            // processing tasks

      // Group Process Group Timer Starts

      timetick = rcinit_term_tmo[rcinit_internal.group_curr];

      if (0 != timetick)
      {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_HIGH_2("Group %x timer set %d", rcinit_internal.group_curr, timetick);
#endif

         // set the timeout timer

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         qurt_anysignal_clear(&anysignal, SIG_TMO);                              // clear signals before waiting
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         rex_clr_sigs(rex_self(), SIG_TMO);                                      // clear signals before waiting
#endif

         timer_set_64(&tmo, timetick, 0, T_MSEC);
      }

      count = rcevt_getregistrants_name(rcinit_term_rcevt[rcinit_internal.group_curr]); // get registered count

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_HIGH_2("Terminate begin registrants %d grp %x", count, rcinit_internal.group_curr);
#endif

      ackcnt = rcevt_getcount_name(RCINIT_RCEVT_TERMACK);                        // get current ack count

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
      if (rcevt_register_sigex_name(RCINIT_RCEVT_TERMACK, RCEVT_SIGEX_TYPE_SIGQURT, &rcevt_sigex)) {;}
      qurt_anysignal_clear(&anysignal, SIG_ACK);                                 // clear signals before waiting
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
      if (rcevt_register_sigex_name(RCINIT_RCEVT_TERMACK, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex)) {;}
      rex_clr_sigs(rex_self(), SIG_ACK);                                         // clear signals before waiting
#endif

      if (rcevt_signal_name(rcinit_term_rcevt[rcinit_internal.group_curr])) {;}  // signal registered

      if (count > 0)                                                             // block only if count is positive
      {
         // HS Accepting On (Still), Checked in rcinit_handshake_term

         rcinit_internal.process_state = rcinit_process_group_term_blocking;     // blocking for *all* handshake

         do
         {
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
            unsigned int recv;
            recv = qurt_anysignal_wait(&anysignal, SIG_ACK | SIG_TMO | SIG_BAIL); // wait for HANDSHAKE ACK, GROUP TIMEOUT, BAILOUT TIMEOUT
            qurt_anysignal_clear(&anysignal, recv);                              // clear signals, then handle
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
            rex_sigs_type recv;
            recv = rex_wait(SIG_ACK | SIG_TMO | SIG_BAIL);                       // wait for HANDSHAKE ACK, GROUP TIMEOUT, BAILOUT TIMEOUT
            rex_clr_sigs(rex_self(), recv);                                      // clear signals, then handle
#endif
            // Bailout Check

            if (recv & SIG_BAIL)                                                 // BAIL OUT - MAXIMUM TIME
            {
               RCINIT_GROUP bail_group = rcinit_internal.group_curr;

               timer_clr_64(&bail, T_NONE);                                      // Group BAILOUT Timer Expires

               // Overall Process Bailout Timer Expires

               // BAILOUT BEHAVIOR BEGINS : ALWAYS PERFORM CALLBACKS & TASKS ASSOCIATED TO TERM GROUP ZERO
               // WITH THE KNOWLEDGE THAT THIS ACTIVITY IS CAPPED TO DURATION OF TIME ALLOCATED TO GROUP_0
               // WHICH IS NOW LOGICALLY PAST THE THE BAIL OUT TIME, HLOS IS NOT BEEN NOTIFIED OF COMPLETION
               // AT THIS POINT.

               rcinit_internal.process_state = rcinit_process_group_term_fn;     // processing functions
               rcinit_internal.group_curr = RCINIT_GROUP_0;                      // current group

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_SHUTDOWN_MSG_ERROR("Bailout timer termfn processing grp");
#endif

               timetick = rcinit_term_tmo[rcinit_internal.group_curr];

               timer_set_64(&tmo, timetick, 0, T_MSEC);

               if (rcevt_signal_name(rcinit_term_rcevt[rcinit_internal.group_curr])) {;}  // signal registered

               timetick += 250; // failsafe adds 250ms

               if (rcecb_signal_name_timetick(rcinit_term_rcecb[rcinit_internal.group_curr], &timetick)) {;}          // signal registered functions of their event

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
               recv = qurt_anysignal_wait(&anysignal, SIG_TMO);      // wait for HANDSHAKE ACK, GROUP TIMEOUT, BAILOUT TIMEOUT
               qurt_anysignal_clear(&anysignal, recv);                              // clear signals, then handle
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
               recv = rex_wait(SIG_TMO);                             // wait for HANDSHAKE ACK, GROUP TIMEOUT, BAILOUT TIMEOUT
               rex_clr_sigs(rex_self(), recv);                                      // clear signals, then handle
#endif

               if (rcevt_signal_name(RCINIT_RCEVT_TERMACK)) {;}

               rcinit_internal.process_state = rcinit_process_none;
               rcinit_internal.group_curr = RCINIT_GROUP_NONE;                   // current group

               // BAILOUT BEHAVIOR ENDS : ALWAYS PERFORM CALLBACKS ASSOCIATED TO TERM GROUP ZERO

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_TERM_BAIL, 2, bail_group, 0);
#endif

               if (0 == rcinit_term_err_fatal)
               {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
                  TMS_SHUTDOWN_MSG_ERROR_1("Bailout timer expire grp %x", bail_group);
#endif
               }

               else
               {
                  ERR_FATAL("bailout timer expire group %x", bail_group, 0, 0);
               }

               // ALL Timers must be cleared at this point before return from routine.

               timer_group_disable(&timer_group);

               timer_undef(&tmo);

               timer_undef(&bail);

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
               qurt_anysignal_destroy(&anysignal);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
               /* NULL */
#endif
               return;                                                           // exit function
            }

            // Timeout Check

            if (recv & SIG_TMO)                                                  // TIMEOUT - GROUP BASED TIMEOUT
            {
               char* task_name = NULL;
               unsigned long i;

               timer_clr_64(&tmo, T_NONE);                                       // Group Process Group Timer Expires

               for (i = 0; i < RCINIT_TASK_MAX; i++)
               {
                  if (0 == rcinit_internal.hs_term[i].hs &&
                      rcinit_internal.group_curr == rcinit_internal.hs_term[i].group)
                  {
                     task_name = &(rcinit_internal.hs_term[i].task_name[0]);     // selects FIRST (potentially more)

                     break;
                  }
               }

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_TERM_TIMEO, 2, rcinit_internal.group_curr, 0);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_SHUTDOWN_MSG_SPRINTF_ERROR_2("Task %s grp timer expire missing handshake %x", task_name, rcinit_internal.group_curr);
#endif
               break;                                                            // exit do/while
            }

            // ACK Check

            if (recv & SIG_ACK)                                                  // ACK
            {
               current = rcevt_getcount_name(RCINIT_RCEVT_TERMACK) - ackcnt;     // read current adjust for [ 0 .. count ]

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_SHUTDOWN_MSG_HIGH_3("Handshake count %d/%d grp %x", current, count, rcinit_internal.group_curr);
#endif
               if (current >= count)                                             // ACK Count
               {
                  break;                                                         // exit do/while
               }
            }

         } while (1);
      }

      // Termination Notification to Tasks Ends
      // HS Accepting Off -- Checked in rcinit_handshake_term

      rcinit_internal.process_state = rcinit_process_group_term_fn;              // processing functions

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
      if (rcevt_unregister_sigex_name(RCINIT_RCEVT_TERMACK, RCEVT_SIGEX_TYPE_SIGQURT, &rcevt_sigex)) {;}
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
      if (rcevt_unregister_sigex_name(RCINIT_RCEVT_TERMACK, RCEVT_SIGEX_TYPE_SIGREX, &rcevt_sigex)) {;}
#endif

      // BAILOUT Timer is not cleared here; BAILOUT timer is utilized across
      // the entire time taken for termination to complete.

      timetick = timer_get_64(&tmo, T_MSEC); /* REMAINDER of expiry */

      timer_clr_64(&tmo, T_NONE);                                                /* Clear TIMEOUT timer, if any */

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
      qurt_anysignal_clear(&anysignal, SIG_ACK | SIG_TMO);                       // Clear any pending TIMEOUT or ack signals
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
      rex_clr_sigs(rex_self(), SIG_ACK | SIG_TMO);                               // Clear any pending TIMEOUT or ack signals
#endif

      // Termination Functions Processing
      // HS Accepting Off -- Checked in rcinit_handshake_term

      rcinit_internal.process_state = rcinit_process_group_term_fn;              // processing functions

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_HIGH_3("termfn processing registrants %d grp %x remaining grp timeout %d", rcecb_getregistrants_name(rcinit_term_rcecb[rcinit_internal.group_curr]), rcinit_internal.group_curr, timetick);
#endif

      timetick += 250; // failsafe adds 250ms

      if (rcecb_signal_name_timetick(rcinit_term_rcecb[rcinit_internal.group_curr], &timetick)) {;}          // signal registered functions of their event

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
      tracer_event_simple(rcinit_swe_event_term[rcinit_internal.group_curr]);    // software event
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_HIGH_1("Terminate end grp %x", rcinit_internal.group_curr);
#endif

   } while (RCINIT_GROUP_0 != group);

   // Termination group processing complete

   rcinit_internal.group_curr = RCINIT_GROUP_NONE;                               // current group
   rcinit_internal.process_state = rcinit_process_none;

   // +++ BEGIN

   // Not suitable for sustained production use. This specifically circumvents the encouraged
   // policy that termination functionality must be as expedient as possible. The artificial
   // latency added here is one technique that allows more diagnostic logging messages to exit
   // from buffering in the target.

   if (0 != rcinit_term_latency_enable && 0 != rcinit_term_timeout)              // condition check used to start bailout timer
   {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_ERROR( "ARTIFICIAL LATENCY BEG -- REMOVE FROM PRODUCTION USE, IMAGE OWNER SUPPLIES DALCFG");
#endif

      do
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         unsigned int recv;
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         rex_sigs_type recv;
#endif

         // Bailout timer previously was initialized. Bailout timer is
         // currently running and has not yet expired.

         // Set the TIMEOUT timer

         timer_set_64(&tmo, 500, 0, T_MSEC);                                     // messages at 500ms intervals

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         recv = qurt_anysignal_wait(&anysignal, SIG_TMO | SIG_BAIL);             // artificial delay for bailout timer
         qurt_anysignal_clear(&anysignal, recv);                                 // clear signals
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         recv = rex_wait(SIG_TMO | SIG_BAIL);                                    // artificial delay for bailout timer
         rex_clr_sigs(rex_self(), recv);                                         // clear signals
#endif

         if (recv & SIG_TMO)                                                     // TIMEOUT
         {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
            TMS_SHUTDOWN_MSG_HIGH("ARTIFICIAL LATENCY ACTIVE -- REMOVE FROM PRODUCTION USE, IMAGE OWNER SUPPLIES DALCFG");
#endif
         }

         if (recv & SIG_BAIL)                                                    // BAILOUT
         {
            break;                                                               // exit do/while
         }

      } while (1);

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_SHUTDOWN_MSG_ERROR("ARTIFICIAL LATENCY END -- REMOVE FROM PRODUCTION USE, IMAGE OWNER SUPPLIES DALCFG");
#endif
   }

   // +++ END

   // Remove the timers before exiting this function

   timer_group_disable(&timer_group);

   timer_undef(&tmo);

   timer_undef(&bail);

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_anysignal_destroy(&anysignal);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   /* NULL */
#endif

   // ALL Timers must be cleared at this point before return from routine.
}

/*===========================================================================

 FUNCTION rcinit_term

 DESCRIPTION
 prepare internal data storage cleanup

 DEPENDENCIES
 none

 RETURN VALUE
 none

 SIDE EFFECTS
 none

 ===========================================================================*/
void rcinit_term(void)
{
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   TMS_SHUTDOWN_MSG_HIGH("Termination begins");
#endif

   rcinit_internal_unprocess_groups();                                           // sequence groups

   // DESIGN DECISION TO *NOT* CLEANUP ANY INFORMATION STRUCTURES

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
   TMS_SHUTDOWN_MSG_HIGH("Termination ends");
#endif

   if (rcevt_term()) {;}

   if (rcesn_term()) {;}

   if (rcecb_term()) {;}

   // EXPECTATION AT THIS POINT IS THAT THE SYSTEM IS RUNNING IN DEGRADED OPERATING
   // MODE. TASKS AND SERVICES ARE PREPARED FOR POWER REMOVAL AT ANY TIME NOW.
}

// Accessor Functions for Manipulating Termination Parameters

unsigned long rcinit_term_err_fatal_set(unsigned long val)
{
   unsigned long rc = rcinit_term_err_fatal;
   rcinit_term_err_fatal = val;
   return rc;
}

unsigned long rcinit_term_latency_enable_set(unsigned long val)
{
   unsigned long rc = rcinit_term_latency_enable;
   rcinit_term_latency_enable = val;
   return rc;
}

unsigned long rcinit_term_timeout_get(void)
{
   return rcinit_term_timeout;
}

unsigned long rcinit_term_timeout_set(unsigned long timeout_ms)
{
   unsigned long rc = rcinit_term_timeout;
   rcinit_term_timeout = timeout_ms;
   return rc;
}

unsigned long rcinit_term_timeout_group_get(RCINIT_GROUP group)
{
   unsigned long rc = 0;

   if (RCINIT_GROUP_7 >= group)
   {
      rc = rcinit_term_tmo[group];
   }

   return rc;
}

unsigned long rcinit_term_timeout_group_set(RCINIT_GROUP group, unsigned long timeout_ms)
{
   unsigned long rc = 0;

   if (RCINIT_GROUP_7 >= group)
   {
      rc = rcinit_term_tmo[group];

      rcinit_term_tmo[group] = timeout_ms;
   }

   return rc;
}
