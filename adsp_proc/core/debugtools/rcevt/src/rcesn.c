/** vi: tw=128 ts=3 sw=3 et :
@file rcesn.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcevt/src/rcesn.c#2 $
$DateTime: 2017/08/17 11:25:19 $
$Change: 14115431 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"
#include "err.h"
#include "timer.h"

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
#include "rcesn_dal.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
#include "rcesn_posix.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
#include "rcesn_qurt.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
#include "rcesn_rex.h"
#endif

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_ULOG_SWEVT)
#endif

#include "tms_rcevt.h"

/**
INTERNAL, Not Exposed for API Use
*/
static timer_group_type internal_timer_group = { 0 };

/**
API, Service initialization
@param
None
@return
None
*/
RCESN_BOOL rcesn_init(void)
{
   timer_group_set_deferrable(&internal_timer_group, TRUE);

   timer_group_enable(&internal_timer_group);

   rce_init();

   return RCESN_TRUE;
}

/**
API, Service termination
@param
None
@return
None
*/
RCESN_BOOL rcesn_term(void)
{
   return RCESN_TRUE;
}

/**
INTERNAL, Map name to opaque handle
*/
static RCESN_HANDLE internal_map_name(RCESN_NAME const name)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p;

   if (rcesn_init()) {;}

   nde_p = rce_nde_hashtab_get(rce_nmehash(name));

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len1 = rce_nmelen(nde_p->name);                                     // string compare required
         int len2 = rce_nmelen(name);

         if (len1 == len2 && 0 == rce_nmecmp(nde_p->name, name, len1))            // lexical comparision
         {
            rc = nde2rcesn(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcesn(nde_p);                                                  // return result

         break;                                                                  // break loop for return
      }

      nde_p = nde_p->next;                                                       // iterate to locate
   }

   return rc;                                                                    // return status or failure
}

/**
API, Create an event, with name

Prerequsite call before posting event -- create the event
before use to avoid a potential "race condition" between
registrants that will post the event and concurrently wait on
the event being posted.

For performance, cache the resulting handle for use within
the API. Name based use cases will take a penalty when doing
a dictionary lookup on every call.

@param[in] NULL terminated string, name of the event
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_create_name(RCESN_NAME const name)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle;

   rce_mutex_lock_dal(&rce_internal.mutex_create);

   handle = internal_map_name(name);

   if (RCE_NULL != handle)                                                       // check succes of loop for found node
   {
      rc = handle;                                                               // return result
   }

   else                                                                          // create new
   {
      rce_nde_p nde_p;

      nde_p = rce_nde_pool_alloc();                                              // allocate new node

      if (RCE_NULL != nde_p)
      {
         // potential optimization would be to perform these allocations from a pool; help prevent heap fragments
         // as events are persistent for the duration of execution, this allocation is also persistent

         unsigned long buf_s = rce_nmelen(name) + 1;                             // length

         char* buf_p = rce_malloc(buf_s);                                        // space

         if (RCE_NULL != buf_p)
         {
            strlcpy(buf_p, name, buf_s);                                         // private copy of name

            nde_p->name = buf_p;                                                 // memorize name address

            nde_p->notify_head = RCE_NULL;                                       // initialize notify queue

            rce_nde_hashtab_put(nde_p, rce_nmehash(nde_p->name));

            rc = nde2rcesn(nde_p);                                               // return result
         }
         else
         {
            rce_nde_pool_free(nde_p);                                            // prevent pool leak
         }
      }
   }

   rce_mutex_unlock_dal(&rce_internal.mutex_create);

   return rc;                                                                    // return result
}

/**
API, Search an event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@return
RCESN_BOOL -- Boolean reflecting event previously created
*/
RCESN_BOOL rcesn_search_name(RCESN_NAME const name)
{
   RCESN_BOOL rc = RCESN_FALSE;                                                  // return result

   if (RCE_NULL != internal_map_name(name))
   {
      rc = RCESN_TRUE;                                                           // return result
   }

   return rc;                                                                    // return status or failure
}

/**
API, Count of registrants for event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
int rcesn_getregistrants_handle(RCESN_HANDLE const handle)
{
   int rc = 0;                                                                   // return result
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = nde_p->notify_head;                                                // locate notify registrant

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         RCESN_SIGEX_TYPE nfy_p_type = (RCESN_SIGEX_TYPE)nfy_p->sigex_type;

         switch (nfy_p_type)
         {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
            case RCESN_SIGEX_TYPE_SIGDAL:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
            case RCESN_SIGEX_TYPE_SIGPOSIX:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
            case RCESN_SIGEX_TYPE_SIGQURT:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
            case RCESN_SIGEX_TYPE_SIGREX:
#endif
            case RCESN_SIGEX_TYPE_SIGNATIVE:
               rc++;
               break;

            default:
            case RCESN_SIGEX_TYPE_UNKNOWN:
            case RCESN_SIGEX_TYPE_NONE:
               break;
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return status or failure
}

/**
API, Count of registrants for event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
int rcesn_getregistrants_name(RCESN_NAME const name)
{
   int rc = 0;                                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcesn_getregistrants_handle(handle);
   }

   return rc;                                                                    // return status or failure
}

/**
API, Current State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_getstatecurr_handle(RCESN_HANDLE const handle)
{
   RCESN_STATE rc = RCE_STATE_INIT;                                              // default
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_curr;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }
   else
   {
      ERR_FATAL("Null RCESN handle.", 0, 0, 0);
   }

   return rc;                                                                    // return result
}

/**
API, Current State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] State to apply
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_setstatecurr_handle(RCESN_HANDLE const handle, RCESN_STATE const state)
{
   RCESN_STATE rc = RCE_STATE_INIT;                                              // default
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_curr;

      nde_p->state_curr = state;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }
   else
   {
      ERR_FATAL("Null RCESN handle.", 0, 0, 0);
   }

   return rc;                                                                    // return result
}

/**
API, Previous State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_getstateprev_handle(RCESN_HANDLE const handle)
{
   RCESN_STATE rc = RCE_STATE_INIT;                                              // default
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_prev;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }
   else
   {
      ERR_FATAL("Null RCESN handle.", 0, 0, 0);
   }

   return rc;                                                                    // return result
}

/**
API, Previous State of event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] State to apply
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_setstateprev_handle(RCESN_HANDLE const handle, RCESN_STATE const state)
{
   RCESN_STATE rc = RCE_STATE_INIT;                                              // default
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_prev;

      nde_p->state_prev = state;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }
   else
   {
      ERR_FATAL("Null RCESN handle.", 0, 0, 0);
   }

   return rc;                                                                    // return result
}

/**
API, Current State of event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_getstatecurr_name(RCESN_NAME const name)
{
   RCESN_STATE rc = RCE_STATE_INIT;
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rce_nde_p nde_p = rcesn2nde(handle);

      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_curr;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return result
}

/**
API, Previous State of event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@return
RCESN_STATE -- State of event
*/
RCESN_STATE rcesn_getstateprev_name(RCESN_NAME const name)
{
   RCESN_STATE rc = RCE_STATE_INIT;
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rce_nde_p nde_p = rcesn2nde(handle);

      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->state_prev;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_sigex_update(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nde_p nde_p;
   rce_nfy_p nfy_p;

   nde_p = rcesn2nde(handle);
   nfy_p = nde_p->notify_head;                                                   // locate notify registrant

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      RCESN_SIGEX_TYPE nfy_p_type = (RCESN_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCESN_SIGEX_TYPE_SIGDAL:
         {
            RCESN_SIGEX_SIGDAL* sigex_p = (RCESN_SIGEX_SIGDAL*)sigex;
            RCESN_SIGEX_SIGDAL* nfy_p_sigex_p = (RCESN_SIGEX_SIGDAL*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p_sigex_p->compare = sigex_p->compare;
               rc = nfy_p;
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCESN_SIGEX_TYPE_SIGPOSIX:
         {
            RCESN_SIGEX_SIGPOSIX* sigex_p = (RCESN_SIGEX_SIGPOSIX*)sigex;
            RCESN_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCESN_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->mutex_p == nfy_p_sigex_p->mutex_p)
            {
               nfy_p_sigex_p->cond_p = sigex_p->cond_p;
               nfy_p_sigex_p->compare = sigex_p->compare;
               rc = nfy_p;
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCESN_SIGEX_TYPE_SIGQURT:
         {
            RCESN_SIGEX_SIGQURT* sigex_p = (RCESN_SIGEX_SIGQURT*)sigex;
            RCESN_SIGEX_SIGQURT* nfy_p_sigex_p = (RCESN_SIGEX_SIGQURT*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p_sigex_p->mask = sigex_p->mask;
               nfy_p_sigex_p->compare = sigex_p->compare;
               rc = nfy_p;
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCESN_SIGEX_TYPE_SIGREX:
         {
            RCESN_SIGEX_SIGREX* sigex_p = (RCESN_SIGEX_SIGREX*)sigex;
            RCESN_SIGEX_SIGREX* nfy_p_sigex_p = (RCESN_SIGEX_SIGREX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p_sigex_p->mask = sigex_p->mask;
               nfy_p_sigex_p->compare = sigex_p->compare;
               rc = nfy_p;
            }
         }
         break;
#endif

         default:
         case RCESN_SIGEX_TYPE_SIGNATIVE:
         case RCESN_SIGEX_TYPE_UNKNOWN:
         case RCESN_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }

      if (RCE_NULL != rc)
      {
         break;
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_sigex_add(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcesn2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = (rce_sigex_t)type;                                     // update member

      switch (type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCESN_SIGEX_TYPE_SIGDAL:
         {
            RCESN_SIGEX_SIGDAL* sigex_p = (RCESN_SIGEX_SIGDAL*)sigex;
            RCESN_SIGEX_SIGDAL* nfy_p_sigex_p = (RCESN_SIGEX_SIGDAL*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p_sigex_p->compare = sigex_p->compare;
            rc = nfy_p;
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCESN_SIGEX_TYPE_SIGPOSIX:
         {
            RCESN_SIGEX_SIGPOSIX* sigex_p = (RCESN_SIGEX_SIGPOSIX*)sigex;
            RCESN_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCESN_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            nfy_p_sigex_p->mutex_p = sigex_p->mutex_p;
            nfy_p_sigex_p->cond_p = sigex_p->cond_p;
            nfy_p_sigex_p->compare = sigex_p->compare;
            rc = nfy_p;
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCESN_SIGEX_TYPE_SIGQURT:
         {
            RCESN_SIGEX_SIGQURT* sigex_p = (RCESN_SIGEX_SIGQURT*)sigex;
            RCESN_SIGEX_SIGQURT* nfy_p_sigex_p = (RCESN_SIGEX_SIGQURT*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p_sigex_p->mask = sigex_p->mask;
            nfy_p_sigex_p->compare = sigex_p->compare;
            rc = nfy_p;
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCESN_SIGEX_TYPE_SIGREX:
         {
            RCESN_SIGEX_SIGREX* sigex_p = (RCESN_SIGEX_SIGREX*)sigex;
            RCESN_SIGEX_SIGREX* nfy_p_sigex_p = (RCESN_SIGEX_SIGREX*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p_sigex_p->mask = sigex_p->mask;
            nfy_p_sigex_p->compare = sigex_p->compare;
            rc = nfy_p;
         }
         break;
#endif

         default:
         case RCESN_SIGEX_TYPE_SIGNATIVE:
         case RCESN_SIGEX_TYPE_UNKNOWN:
         case RCESN_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }
   }

   return rc;                                                                    // return result
}

/**
API, Register notification to event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_register_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcesn2nde(handle);

   if (RCE_NULL != handle)
   {
      rce_nfy_p nfy_p = RCE_NULL;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = internal_register_sigex_update(handle, type, sigex);               // update an existing

      if (RCE_NULL != nfy_p)
      {
         rc = handle;                                                            // return result
      }
      else
      {
         nfy_p = internal_register_sigex_add(handle, type, sigex);               // add a new

         if (RCE_NULL != nfy_p)
         {
            rc = handle;                                                         // return result
         }
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return result
}

/**
API, Register notification to event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_register_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcesn_register_sigex_handle(handle, type, sigex);
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCESN_HANDLE internal_unregister_sigex(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nde_p nde_p = rcesn2nde(handle);
   rce_nfy_p nfy_p;

   rce_mutex_lock_dal(&nde_p->mutex);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      RCESN_SIGEX_TYPE nfy_p_type = (RCESN_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCESN_SIGEX_TYPE_SIGDAL:
         {
            RCESN_SIGEX_SIGDAL* sigex_p = (RCESN_SIGEX_SIGDAL*)sigex;
            RCESN_SIGEX_SIGDAL* nfy_p_sigex_p = (RCESN_SIGEX_SIGDAL*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               if (RCE_NULL == nfy_prev)                                         // head of list check
               {
                  nde_p->notify_head = nfy_p->next;                              // remove from head
               }
               else
               {
                  nfy_prev->next = nfy_p->next;                                  // remove from list
               }
               nfy_p->next = RCE_NULL;                                           // depopulate
               nfy_p_sigex_p->signal = RCE_NULL;                                 // sigex
               nfy_p_sigex_p->compare = RCE_NULL;                                // sigex
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcesn(nde_p);                                            // return result
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCESN_SIGEX_TYPE_SIGPOSIX:
         {
            RCESN_SIGEX_SIGPOSIX* sigex_p = (RCESN_SIGEX_SIGPOSIX*)sigex;
            RCESN_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCESN_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->mutex_p == nfy_p_sigex_p->mutex_p)
            {
               if (RCE_NULL == nfy_prev)                                         // head of list check
               {
                  nde_p->notify_head = nfy_p->next;                              // remove from head
               }
               else
               {
                  nfy_prev->next = nfy_p->next;                                  // remove from list
               }
               nfy_p->next = RCE_NULL;                                           // depopulate
               nfy_p_sigex_p->mutex_p = RCE_NULL;                                // sigex
               nfy_p_sigex_p->cond_p = RCE_NULL;                                 // sigex
               nfy_p_sigex_p->compare = RCE_NULL;                                // sigex
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcesn(nde_p);                                            // return result
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCESN_SIGEX_TYPE_SIGQURT:
         {
            RCESN_SIGEX_SIGQURT* sigex_p = (RCESN_SIGEX_SIGQURT*)sigex;
            RCESN_SIGEX_SIGQURT* nfy_p_sigex_p = (RCESN_SIGEX_SIGQURT*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               if (RCE_NULL == nfy_prev)                                         // head of list check
               {
                  nde_p->notify_head = nfy_p->next;                              // remove from head
               }
               else
               {
                  nfy_prev->next = nfy_p->next;                                  // remove from list
               }
               nfy_p->next = RCE_NULL;                                           // depopulate
               nfy_p_sigex_p->signal = RCE_NULL;                                 // sigex
               nfy_p_sigex_p->mask = 0;                                          // sigex
               nfy_p_sigex_p->compare = RCE_NULL;                                // sigex
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcesn(nde_p);                                            // return result
            }
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCESN_SIGEX_TYPE_SIGREX:
         {
            RCESN_SIGEX_SIGREX* sigex_p = (RCESN_SIGEX_SIGREX*)sigex;
            RCESN_SIGEX_SIGREX* nfy_p_sigex_p = (RCESN_SIGEX_SIGREX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               if (RCE_NULL == nfy_prev)                                         // head of list check
               {
                  nde_p->notify_head = nfy_p->next;                              // remove from head
               }
               else
               {
                  nfy_prev->next = nfy_p->next;                                  // remove from list
               }
               nfy_p->next = RCE_NULL;                                           // depopulate
               nfy_p_sigex_p->signal = RCE_NULL;                                 // sigex
               nfy_p_sigex_p->mask = 0;                                          // sigex
               nfy_p_sigex_p->compare = RCE_NULL;                                // sigex
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcesn(nde_p);                                            // return result
            }
         }
         break;
#endif

         default:
         case RCESN_SIGEX_TYPE_SIGNATIVE:
         case RCESN_SIGEX_TYPE_UNKNOWN:
         case RCESN_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }

      if (RCE_NULL != rc)
      {
         break;
      }

      nfy_prev = nfy_p;

      nfy_p = nfy_p->next;                                                       // iterate to check next
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Unegister notification to event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_unregister_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result

   rc = internal_unregister_sigex(handle, type, sigex);                          // existing

   return rc;                                                                    // return result
}

/**
API, Unegister notification to event, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_unregister_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcesn_unregister_sigex_handle(handle, type, sigex);                   // existing
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static void internal_timer_fail(timer_cb_data_type data)
{
   ERR_FATAL("RCESN client callback runtime exceeded", 0, 0, 0);

   if (data) {;}
}

/**
API, Signal event, with handle and timetick
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@param[in] Maximum timer value associated to event
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_signal_handle_timetick(RCESN_HANDLE const handle, RCESN_STATE const state, unsigned long* const timetick_p)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcesn2nde(handle);
   RCESN_BOOL ret = RCESN_FALSE;

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      if (timetick_p)                                                            // uses a supplied timetick
      {
         unsigned long timetick = *timetick_p;

         timer_def_osal(&nde_p->expiry, &internal_timer_group, TIMER_FUNC1_CB_TYPE, internal_timer_fail, 0);

         timer_set_64(&nde_p->expiry, timetick, timetick, T_MSEC);
      }

      nde_p->state_prev = nde_p->state_curr;

      nde_p->state_curr = state;

      nfy_p = nde_p->notify_head;                                                // traverse notify queue

      while (RCE_NULL != nfy_p)                                                  // iterate
      {
         rce_nfy_p nfy_p_next = nfy_p->next;                                     // memorize nfy_p->next allowing callback to unregister in its handler
         RCESN_SIGEX_TYPE nfy_p_type = (RCESN_SIGEX_TYPE)nfy_p->sigex_type;

         switch (nfy_p_type)
         {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
            case RCESN_SIGEX_TYPE_SIGDAL:
            {
               RCESN_SIGEX_SIGDAL* nfy_p_sigex_p = (RCESN_SIGEX_SIGDAL*)&nfy_p->sigex;
               RCESN_FNSIG_COMPARE compare = nfy_p_sigex_p->compare;
               ret = compare(handle);
               if (RCESN_TRUE == ret)
               {
                  DALSYS_EventCtrl(nfy_p_sigex_p->signal, DALSYS_EVENT_CTRL_TRIGGER);
                  rc = handle;
               }
            }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
            case RCESN_SIGEX_TYPE_SIGPOSIX:
            {
               RCESN_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCESN_SIGEX_SIGPOSIX*)&nfy_p->sigex;
               RCESN_FNSIG_COMPARE compare = nfy_p_sigex_p->compare;
               ret = compare(handle);
               if (RCESN_TRUE == ret)
               {
                  pthread_mutex_lock(nfy_p_sigex_p->mutex_p);
                  pthread_cond_broadcast(nfy_p_sigex_p->cond_p);
                  pthread_mutex_unlock(nfy_p_sigex_p->mutex_p);
                  rc = handle;
               }
            }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
            case RCESN_SIGEX_TYPE_SIGQURT:
            {
               RCESN_SIGEX_SIGQURT* nfy_p_sigex_p = (RCESN_SIGEX_SIGQURT*)&nfy_p->sigex;
               RCESN_FNSIG_COMPARE compare = nfy_p_sigex_p->compare;
               ret = compare(handle);
               if (RCESN_TRUE == ret)
               {
                  qurt_anysignal_set(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
                  rc = handle;
               }
            }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
            case RCESN_SIGEX_TYPE_SIGREX:
            {
               RCESN_SIGEX_SIGREX* nfy_p_sigex_p = (RCESN_SIGEX_SIGREX*)&nfy_p->sigex;
               RCESN_FNSIG_COMPARE compare = nfy_p_sigex_p->compare;
               ret = compare(handle);
               if (RCESN_TRUE == ret)
               {
                  rex_set_sigs(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
                  rc = handle;
               }
            }
            break;
#endif

            default:
            case RCESN_SIGEX_TYPE_SIGNATIVE:
            case RCESN_SIGEX_TYPE_UNKNOWN:
            case RCESN_SIGEX_TYPE_NONE:
               /*NULL*/
               break;
         }

         nfy_p = nfy_p_next;                                                     // iterate to next using memorized value
      }

      if (timetick_p)                                                            // uses a supplied timetick
      {
         unsigned long timetick = timer_get_64(&nde_p->expiry, T_MSEC);

         *timetick_p = timetick;

         timer_clr_64(&nde_p->expiry, T_NONE);

         timer_undef(&nde_p->expiry);
      }

      rce_mutex_unlock_dal(&nde_p->mutex);

      rc = handle;
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_signal_handle(RCESN_HANDLE const handle, RCESN_STATE const state)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   unsigned long timetick = RCESN_CALLBACK_MAX_TIME;

   rc = rcesn_signal_handle_timetick(handle, state, &timetick);

   return rc;                                                                    // return result
}

/**
API, Signal event, with name
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_signal_name(RCESN_NAME const name, RCESN_STATE const state)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);
   unsigned long timetick = RCESN_CALLBACK_MAX_TIME;

   if (RCE_NULL != handle)
   {
      rc = rcesn_signal_handle_timetick(handle, state, &timetick);
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with name and timetick
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Current state to apply before notifications
@param[in] Maximum timer value associated to event
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
DLL_API_WARN_UNUSED_RESULT 
DLL_API_EXPORT_RCINIT RCESN_HANDLE rcesn_signal_name_timetick(RCESN_NAME const name, RCESN_STATE const state, unsigned long* const timetick_p)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcesn_signal_handle_timetick(handle, state, timetick_p);
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCESN_HANDLE internal_wait_common(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex, unsigned long* const timetick_p)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcesn2nde(handle);
   rce_nfy_p nfy_p = RCE_NULL;
   RCESN_FNSIG_COMPARE compare = RCE_NULL;
   RCESN_BOOL ret = RCESN_FALSE;

   switch (type)
   {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
      case RCESN_SIGEX_TYPE_SIGDAL:
      {
         RCESN_SIGEX_SIGDAL* sigex_p = (RCESN_SIGEX_SIGDAL*)sigex;
         compare = sigex_p->compare;
      }
      break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
      case RCESN_SIGEX_TYPE_SIGPOSIX:
      {
         RCESN_SIGEX_SIGPOSIX* sigex_p = (RCESN_SIGEX_SIGPOSIX*)sigex;
         compare = sigex_p->compare;
      }
      break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
      case RCESN_SIGEX_TYPE_SIGQURT:
      {
         RCESN_SIGEX_SIGQURT* sigex_p = (RCESN_SIGEX_SIGQURT*)sigex;
         compare = sigex_p->compare;
      }
      break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
      case RCESN_SIGEX_TYPE_SIGREX:
      {
         RCESN_SIGEX_SIGREX* sigex_p = (RCESN_SIGEX_SIGREX*)sigex;
         compare = sigex_p->compare;
      }
      break;
#endif

      default:
      case RCESN_SIGEX_TYPE_SIGNATIVE:
      case RCESN_SIGEX_TYPE_UNKNOWN:
      case RCESN_SIGEX_TYPE_NONE:
         /*NULL*/
         break;
   }

   if (RCE_NULL != compare)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      if (timetick_p)                                                            // uses a supplied timetick
      {
         timer_def_osal(&nde_p->expiry, &internal_timer_group, TIMER_FUNC1_CB_TYPE, internal_timer_fail, 0);

         timer_set_64(&nde_p->expiry, *timetick_p, 0, T_MSEC);
      }

      if (RCE_NULL != compare)
      {
         ret = compare(handle);                                                  // call compare function to determine if blocking

         if (RCESN_TRUE == ret)
         {
            rc = handle;                                                         // return result
         }

         else
         {
            nfy_p = internal_register_sigex_update(handle, type, sigex);         // update an existing

            if (RCE_NULL != nfy_p)
            {
               rc = handle;                                                      // return result
            }
            else
            {
               nfy_p = internal_register_sigex_add(handle, type, sigex);         // add a new

               if (RCE_NULL != nfy_p)
               {
                  rc = handle;                                                   // return result
               }
            }
         }
      }

      if (timetick_p)                                                            // uses a supplied timetick
      {
         *timetick_p = timer_get_64(&nde_p->expiry, T_MSEC);

         timer_clr_64(&nde_p->expiry, T_NONE);

         timer_undef(&nde_p->expiry);
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   if (RCESN_TRUE == ret || RCE_NULL == nfy_p)
   {
      /*NULL*/
   }

   else
   {
      RCESN_SIGEX_TYPE nfy_p_type = (RCESN_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCESN_SIGEX_TYPE_SIGDAL:
         {
            RCESN_SIGEX_SIGDAL* nfy_p_sigex_p = (RCESN_SIGEX_SIGDAL*)&nfy_p->sigex;
            DALSYS_EventWait(nfy_p_sigex_p->signal);
            DALSYS_EventCtrl(nfy_p_sigex_p->signal, DALSYS_EVENT_CTRL_RESET);
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCESN_SIGEX_TYPE_SIGPOSIX:
         {
            RCESN_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCESN_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            pthread_mutex_lock(nfy_p_sigex_p->mutex_p);
            pthread_cond_wait(nfy_p_sigex_p->cond_p, nfy_p_sigex_p->mutex_p);
            pthread_mutex_unlock(nfy_p_sigex_p->mutex_p);
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCESN_SIGEX_TYPE_SIGQURT:
         {
            RCESN_SIGEX_SIGQURT* nfy_p_sigex_p = (RCESN_SIGEX_SIGQURT*)&nfy_p->sigex;
            qurt_anysignal_wait(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
            qurt_anysignal_clear(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
         }
         break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCESN_SIGEX_TYPE_SIGREX:
         {
            RCESN_SIGEX_SIGREX* nfy_p_sigex_p = (RCESN_SIGEX_SIGREX*)&nfy_p->sigex;
            rex_wait(nfy_p_sigex_p->mask);
            rex_clr_sigs(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
         }
         break;
#endif

         default:
         case RCESN_SIGEX_TYPE_SIGNATIVE:
         case RCESN_SIGEX_TYPE_UNKNOWN:
         case RCESN_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }
   }

   return rc;                                                                    // return result
}

/**
API, Wait for event with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_wait_sigex_handle(RCESN_HANDLE const handle, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   unsigned long timetick = RCESN_CALLBACK_MAX_TIME;

   if (RCE_NULL != handle)
   {
      rc = internal_wait_common(handle, type, sigex, &timetick);                 // wait
   }

   return rc;
}

/**
API, Wait for event at specific state, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_wait_sigex_name(RCESN_NAME const name, RCESN_SIGEX_TYPE const type, RCESN_SIGEX const sigex)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);
   unsigned long timetick = RCESN_CALLBACK_MAX_TIME;

   if (RCE_NULL != handle)
   {
      rc = internal_wait_common(handle, type, sigex, &timetick);                 // wait
   }

   return rc;
}

/**
API, Wait for event, one time per call, until compare met, with handle
See prerequsite note with rcesn_create_name
@param[in] Opaque handle of the event
@param[in] Comparator function pointer
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_wait_handle(RCESN_HANDLE const handle, RCESN_FNSIG_COMPARE const compare)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)

   DALSYSEventObj signalObject;
   RCESN_SIGEX_SIGDAL sig;

   // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
   if (DAL_SUCCESS == DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &sig.signal, &signalObject))
   {
      sig.compare = compare;
      unsigned long timetick = RCESN_CALLBACK_MAX_TIME;

      // Waiting causes an implicit registration
      rc = internal_wait_common(handle, RCESN_SIGEX_TYPE_SIGDAL, &sig, &timetick);

      // Unregister the implicit addition from waiting
      if (internal_unregister_sigex(handle, RCESN_SIGEX_TYPE_SIGDAL, &sig)) { ; }

      // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
      if (DAL_SUCCESS == DALSYS_DestroyObject(sig.signal)) {;}
   }
#endif

   return rc;                                                                    // return result
}

/**
API, Wait for event, one time per call, until compare met, with name
See prerequsite note with rcesn_create_name
@param[in] NULL terminated string, name of the event
@param[in] Comparator function pointer
@return
RCESN_NULL -- Error
RCESN_HANDLE -- Opaque handle to existing event
*/
RCESN_HANDLE rcesn_wait_name(RCESN_NAME const name, RCESN_FNSIG_COMPARE const compare)
{
   RCESN_HANDLE rc = RCE_NULL;                                                   // return result
   RCESN_HANDLE handle = rcesn_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcesn_wait_handle(handle, compare);
   }

   return rc;                                                                    // return result
}

/**
API, Common Comparision Functions
See prerequsite note with rcesn_create_name
Clients have the ability to supply comparator callback functions for use. Primarily
these functions will capture the policy logic and determine if notification should
occur with a TRUE/FALSE return. NOTE: these callback functions are never allowed
to block by policy; to do so is considered a bug. Callbacks are exeucted and if
they do not complete within an gracious time period (~20ms), the system is halted.
@param[in] Opaque handle of the event
@return
RCESN_BOOL -- Boolean reflecting comparision status
*/
RCESN_BOOL rcesn_compare_lt_0(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       0 > (long)rcesn_getstatecurr_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_eq_0(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       0 == (long)rcesn_getstatecurr_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_ne_0(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       0 != (long)rcesn_getstatecurr_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_gt_0(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       0 < (long)rcesn_getstatecurr_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_lt_prev(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       rcesn_getstatecurr_handle(handle) < rcesn_getstateprev_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_eq_prev(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       rcesn_getstatecurr_handle(handle) == rcesn_getstateprev_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_ne_prev(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       rcesn_getstatecurr_handle(handle) != rcesn_getstateprev_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_gt_prev(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle &&
       rcesn_getstatecurr_handle(handle) > rcesn_getstateprev_handle(handle))
   {
      rc = RCESN_TRUE;
   }

   return rc;
}

RCESN_BOOL rcesn_compare_mask_t_0001(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle)
   {
      if (0 != (0x0001 & (unsigned long)rcesn_getstatecurr_handle(handle)))
      {
         rc = RCESN_TRUE;
      }
   }

   return rc;
}

RCESN_BOOL rcesn_compare_mask_f_0001(RCESN_HANDLE const handle)
{
   RCESN_BOOL rc = RCESN_FALSE;

   if (RCE_NULL != handle)
   {
      if (0 == (0x0001 & (unsigned long)rcesn_getstatecurr_handle(handle)))
      {
         rc = RCESN_TRUE;
      }
   }

   return rc;
}
