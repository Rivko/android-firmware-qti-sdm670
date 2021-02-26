/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_dal.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcinit/src/rcinit_dal.c#2 $
$DateTime: 2017/09/01 01:33:34 $
$Change: 14246640 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)

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

#include "rcinit_dal.h"
#include "tms_rcinit.h"

#define DWMAXNUMEVENTS 1

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask_info(RCINIT_INFO info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle && RCINIT_TASK_DALTASK == rcinit_p->type)
   {
      return(RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p);
   }

   return RCINIT_NULL;
}

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask(RCINIT_NAME name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return rcinit_lookup_daltask_info(info);
   }

   return RCINIT_NULL;
}

static DALResult TASK_ENTRY(DALSYSEventHandle hUnused, void* argv)               // hEvent is a WORKLOOP_EVENT
{
   DALResult rc = DAL_ERROR;
   const rcinit_info_t* rcinit_p = (const rcinit_info_t*)argv;

   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      DALSYSWorkLoopExecute entry = (DALSYSWorkLoopExecute)rcinit_p->handle->entry;

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
               rc = entry(RCINIT_NULL, (void*)restart);

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

         rc = entry(RCINIT_NULL, (void*)restart);

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

   if (hUnused) {;}                                                              // unused

   return rc;
}

void rcinit_internal_start_daltask(const rcinit_info_t* rcinit_p, void* entry)
{
   if (RCINIT_NULL != rcinit_p && RCINIT_NULL != rcinit_p->handle)
   {
      if (NULL == (rcinit_p->handle->tid_p = (rcinit_tid_p)rcinit_internal_malloc(sizeof(RCINIT_TASK_DALTASK_CTX))))
      {
         ERR_FATAL("daltask context creation", 0, 0, 0);
      }
      secure_memset(rcinit_p->handle->tid_p, 0, sizeof(RCINIT_TASK_DALTASK_CTX));
      if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, &(((RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p))->hEventStart), NULL))
      {
         ERR_FATAL("daltask event creation", 0, 0, 0);
      }
      if (DAL_SUCCESS != DALSYS_RegisterWorkLoopEx((char*)rcinit_p->name, rcinit_p->stksz, RCINIT_MAP_PRIO_DAL(rcinit_p->prio), DWMAXNUMEVENTS, &(((RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p))->hWorkLoop), NULL))
      {
         ERR_FATAL("daltask work loop registration", 0, 0, 0);
      }
      rcinit_p->handle->entry = (rcinit_entry_p)entry;
      if (DAL_SUCCESS != DALSYS_AddEventToWorkLoop(((RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p))->hWorkLoop, TASK_ENTRY, (void*)rcinit_p, ((RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p))->hEventStart, NULL))
      {
         ERR_FATAL("daltask work loop event addition", 0, 0, 0);
      }
      if (DAL_SUCCESS != DALSYS_EventCtrl(((RCINIT_TASK_DALTASK_CTX*)(rcinit_p->handle->tid_p))->hEventStart, DALSYS_EVENT_CTRL_TRIGGER))
      {
         ERR_FATAL("daltask work loop start", 0, 0, 0);
      }
   }
}

#else

typedef unsigned int RCINIT_TASK_DALTASK_CTX;

#include "tms_rcinit.h"

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask_info(RCINIT_INFO info)
{
   return RCINIT_NULL;
}

RCINIT_TASK_DALTASK_CTX* rcinit_lookup_daltask(RCINIT_NAME name)
{
   return RCINIT_NULL;
}

void rcinit_internal_start_daltask(const rcinit_info_t* rcinit_p, void* entry)
{
}

#endif
