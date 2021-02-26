/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_hs_list.c
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
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcinit/src/rcinit_hs_list.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
#include "rcinit_dal.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
#include "rcinit_posix.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
#include "rcinit_qurt.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
#include "rcinit_rex.h"
#endif

#include "tms_rcinit.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
DALSYSSyncHandle hs_mutex;
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
static qurt_mutex_t hs_mutex;
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
static rex_crit_sect_type hs_mutex;
#endif

/////////////////////////////////////////////////////////////////////
// Init Function Support
/////////////////////////////////////////////////////////////////////

unsigned long rcinit_internal_task_name(char* task_name, int buflen)
{
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   qurt_thread_get_name(task_name, buflen);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_thread_get_name(task_name, buflen);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_get_task_name(rex_self(), task_name, buflen, NULL);
#endif

   return rcinit_internal_nmehash(task_name);
}

void rcinit_internal_hs_list_init(void)
{
   unsigned long cnt = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &hs_mutex, NULL)) { ; }
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_init(&hs_mutex);
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_init_crit_sect(&hs_mutex);
   rex_enter_crit_sect(&hs_mutex);
#endif

   for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)
   {
      rcinit_internal.hs_init[cnt].group = RCINIT_GROUP_NONE;
      rcinit_internal.hs_term[cnt].group = RCINIT_GROUP_NONE;

      strlcpy(rcinit_internal.hs_init[cnt].task_name, "", sizeof(rcinit_internal.hs_init[cnt].task_name));
      strlcpy(rcinit_internal.hs_term[cnt].task_name, "", sizeof(rcinit_internal.hs_term[cnt].task_name));

      rcinit_internal.hs_init[cnt].hs = 0;
      rcinit_internal.hs_term[cnt].hs = 0;

      rcinit_internal.hs_init[cnt].hs_dup = 0;
      rcinit_internal.hs_term[cnt].hs_dup = 0;

      rcinit_internal.hs_init[cnt].hs_oob = 0;
      rcinit_internal.hs_term[cnt].hs_oob = 0;

      rcinit_internal.hs_init[cnt].tid = RCINIT_NULL;
      rcinit_internal.hs_term[cnt].tid = RCINIT_NULL;
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif
}

unsigned long rcinit_internal_hs_list_add(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name)
{
   unsigned long cnt = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_enter_crit_sect(&hs_mutex);
#endif

   if (RCINIT_GROUP_7 >= group)
   {
      for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                                // find existing
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         void* tid = (void*)rex_self();
#endif

         if (tid == (void*)array[cnt].tid)
         {
            if (group == array[cnt].group)
            {
               break;
            }
         }
      }

      if (cnt < RCINIT_TASK_MAX)                                                 // update existing
      {
         // already set array[cnt].group = group;
         // already set strlcpy(array[cnt].task_name, task_name, sizeof(array[cnt].task_name));
         // already set array[cnt].tid = GET_SELF();
         array[cnt].hs = 0;
      }

      else
      {
         for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                             // find empty
         {
            if (array[cnt].group == RCINIT_GROUP_NONE)
            {
               break;
            }
         }

         if (cnt < RCINIT_TASK_MAX)                                              // update empty
         {
            array[cnt].group = group;
            strlcpy(array[cnt].task_name, task_name, sizeof(array[cnt].task_name));
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
            array[cnt].tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
            array[cnt].tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
            array[cnt].tid = (void*)rex_self();
#endif

            array[cnt].hs = 0;
         }
         else
         {
            cnt = RCINIT_TASK_MAX;
         }
      }
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif

   return cnt;                                                                   // RCINIT_TASK_MAX == Not Present, !RCINIT_TASK_MAX = Present & Added
}

unsigned long rcinit_internal_hs_list_del(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name)
{
   unsigned long cnt = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_enter_crit_sect(&hs_mutex);
#endif

   if (RCINIT_GROUP_7 >= group)
   {
      for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                                // find existing
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         void* tid = (void*)rex_self();
#endif

         if (tid == (void*)array[cnt].tid)
         {
            if (group == array[cnt].group)
            {
               break;
            }
         }
      }

      if (cnt < RCINIT_TASK_MAX)                                                 // update existing
      {
         array[cnt].group = RCINIT_GROUP_NONE;
         //strlcpy(array[cnt].task_name, "", sizeof(array[cnt].task_name));
         //array[cnt].tid = RCINIT_NULL;
         //array[cnt].hs = 0;
      }
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif

   return cnt;                                                                   // RCINIT_TASK_MAX == Not Present, !RCINIT_TASK_MAX = Present & Removed
}

unsigned long rcinit_internal_hs_list_query(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name)
{
   unsigned long cnt = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_enter_crit_sect(&hs_mutex);
#endif

   if (RCINIT_GROUP_7 >= group)
   {
      for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                                // find existing
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         void* tid = (void*)rex_self();
#endif

         if (tid == (void*)array[cnt].tid)
         {
            if (group == array[cnt].group)
            {
               break;
            }
         }
      }
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif

   return cnt;                                                                   // RCINIT_TASK_MAX == Not Present, !RCINIT_TASK_MAX = Present
}

unsigned long rcinit_internal_hs_list_query_hs(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name)
{
   unsigned long cnt = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_enter_crit_sect(&hs_mutex);
#endif

   if (RCINIT_GROUP_7 >= group)
   {
      for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                                // find existing
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         void* tid = (void*)rex_self();
#endif

         if (tid == (void*)array[cnt].tid)
         {
            if (group == array[cnt].group)
            {
               if (1 == array[cnt].hs)
               {
                  break;
               }
            }
         }
      }
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif

   return cnt;                                                                   // RCINIT_TASK_MAX == Not Present or No HS, !RCINIT_TASK_MAX = HS
}

unsigned long rcinit_internal_hs_list_hs(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name)
{
   unsigned long cnt;
   unsigned long rc = RCINIT_TASK_MAX;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncEnter(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_lock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_enter_crit_sect(&hs_mutex);
#endif

   if (RCINIT_GROUP_7 >= group)
   {
      for (cnt = 0; cnt < RCINIT_TASK_MAX; cnt++)                                // find existing
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         void* tid = (void*)qurt_thread_get_id();
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
         void* tid = (void*)rex_self();
#endif

         if (tid == (void*)array[cnt].tid)
         {
            if (group == array[cnt].group)                                       // check matching group
            {
               if (0 == array[cnt].hs)                                           // check first handshake
               {
                  // accept the handshake, it matches the group and is the first one
                  array[cnt].hs = 1;
                  rc = cnt;
               }
               else
               {
                  // statistics indicates this is duplicate; a task is not allowed to
                  // duplicate its handshake within the same group. this is not fatal,
                  // but it must not be counted in the total HS required for the group.
                  array[cnt].hs_dup++;
               }
            }
            else
            {
               // statistics indicates this is out of band; a task that registers for multiple
               // groups will have the OOB counters incremented for any group not current, the
               // behavior is not a problem, it is not fatal, but it must not be counted in the
               // total HS required for the group.
               array[cnt].hs_oob++;
            }
         }
      }
   }

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYS_SyncLeave(hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_unlock(&hs_mutex);
#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)
   rex_leave_crit_sect(&hs_mutex);
#endif

   return rc;                                                                    // RCINIT_TASK_MAX == Not Present or Spurious HS, !RCINIT_TASK_MAX = HS
}
