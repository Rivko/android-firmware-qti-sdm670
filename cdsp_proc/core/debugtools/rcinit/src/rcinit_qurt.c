/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_qurt.c
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcinit/src/rcinit_qurt.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)

#define DLL_API_BUILD_RCINIT

#include "err.h"
#include "stringl/stringl.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#include "tms_utils.h"
#include "rcinit_qurt.h"
#include "tms_rcinit.h"

qurt_thread_t rcinit_lookup_qurttask_info(RCINIT_INFO info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_QURTTASK == rcinit_p->type)
   {
      return(qurt_thread_t)(rcinit_p->handle->tid_p);
   }

   return RCINIT_ZERO;
}

qurt_thread_t rcinit_lookup_qurttask(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return rcinit_lookup_qurttask_info(info);
   }

   return RCINIT_ZERO;
}

static void TASK_ENTRY(void* argv)
{
   const rcinit_info_t* rcinit_p = (const rcinit_info_t*)argv;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      void (*entry)(void*) = (void (*)(void*))rcinit_p->handle->entry;

      rcinit_internal_tls_set_specific(rcinit_internal.tls_key, (void*)rcinit_p);

      if (RCINIT_NULL != entry && RCINIT_ENTRY_NONE != entry)
      {
         unsigned long restart = 0;

         rcinit_internal_hs_list_add(rcinit_internal.hs_init, rcinit_internal.group_curr, rcinit_p->name);

#if !defined(RCINIT_EXCLUDE_RCXH_HANDLER)

         do
         {
            RCXH_TRY
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_RN, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_STARTUP_MSG_SPRINTF_HIGH_3("Task %s # %x begins grp %x", rcinit_p->name, rcinit_p->hash, rcinit_internal.group_curr);
#endif
               entry((void*)restart);

               restart = 0;
            }

            RCXH_CATCH(RCXH_RCINIT_TASK_RESTART)
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_RS, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_STARTUP_MSG_HIGH_2("Task # %x exception restart grp %x", rcinit_p->hash, rcinit_internal.group_curr);
#endif
               restart++;
            }

            RCXH_CATCH(RCXH_RCINIT_TASK_END)
            {
#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
               tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_E, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
               TMS_STARTUP_MSG_HIGH_2("Task # %x exception end grp %x", rcinit_p->hash, rcinit_internal.group_curr);
#endif
               restart = 0;
            }

            RCXH_ENDTRY;

         } while (0 < restart);

#else

         entry((void*)restart);

#endif

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
         tracer_event_simple_vargs(RCINIT_SWE_INIT_TASK_XT, 2, rcinit_internal.group_curr, rcinit_p->hash);
#endif
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("Task # %x exit grp %x", rcinit_p->hash, rcinit_internal.group_curr);
#endif
      }
   }

   else
   {
#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
      TMS_STARTUP_MSG_HIGH_1("Task exit grp %x no context", rcinit_internal.group_curr);
#endif
   }
}

void rcinit_internal_start_qurttask(const rcinit_info_t* rcinit_p, void* entry)
{
   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      qurt_thread_attr_t qurt_attr;

      if (RCINIT_NULL != rcinit_p->handle->stack)
      {
         secure_memset(rcinit_p->handle->stack, RCINIT_STACK_INIT, rcinit_p->stksz);
      }

      qurt_thread_attr_init(&qurt_attr);
      qurt_thread_attr_set_name(&qurt_attr, (char*)rcinit_p->name);
      qurt_thread_attr_set_stack_size(&qurt_attr, rcinit_p->stksz);
      qurt_thread_attr_set_stack_addr(&qurt_attr, rcinit_p->handle->stack);
      qurt_thread_attr_set_priority(&qurt_attr, RCINIT_MAP_PRIO_QURT(rcinit_p->prio)); // qurt priority is 0 = high, invert to match 0 = low

      rcinit_p->handle->entry = (rcinit_entry_p)entry;
      if ( 0 != qurt_thread_create((qurt_thread_t*)(&(rcinit_p->handle->tid_p)), &qurt_attr, TASK_ENTRY, (void*)rcinit_p))
      {
         char msg[TMS_UTILS_BUFFER_SIZE];
         (void)tms_utils_fmt(msg, TMS_UTILS_BUFFER_SIZE, "RCINIT failed to launch QuRT task %s", rcinit_p->name);
         ERR_FATAL_AUX_MSG("RCINIT failed to launch QuRT task. Check coredump.err.aux_msg", msg, TMS_UTILS_BUFFER_SIZE);
      }
   }
}

#else

typedef unsigned int qurt_thread_t;

#include "tms_rcinit.h"

qurt_thread_t rcinit_lookup_qurttask_info(RCINIT_INFO info)
{
   return RCINIT_ZERO;
}

qurt_thread_t rcinit_lookup_qurttask(RCINIT_NAME name)
{
   return RCINIT_ZERO;
}

void rcinit_internal_start_qurttask(const rcinit_info_t* rcinit_p, void* entry)
{
}

#endif
