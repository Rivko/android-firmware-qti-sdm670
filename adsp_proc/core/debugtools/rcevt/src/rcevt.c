/** vi: tw=128 ts=3 sw=3 et :
@file rcevt.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcevt/src/rcevt.c#2 $
$DateTime: 2017/08/17 11:25:19 $
$Change: 14115431 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"
#include "err.h"
#include "timer.h"

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
#include "rcevt_dal.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
#include "rcevt_posix.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
#include "rcevt_qurt.h"
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
#include "rcevt_rex.h"
#endif

#include "tms_rcevt.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

#if !defined(RCINIT_EXCLUDE_ULOG_SWEVT)
#endif

/**
API, Service initialization
@param
None
@return
None
*/
RCEVT_BOOL rcevt_init(void)
{
   rce_init();

   return RCEVT_TRUE;
}

/**
API, Service termination
@param
None
@return
None
*/
RCEVT_BOOL rcevt_term(void)
{
   return RCEVT_TRUE;
}

/**
API, Map name to opaque handle
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
static RCEVT_HANDLE internal_map_name(RCEVT_NAME const name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p;

   if (rcevt_init()) {;}

   nde_p = rce_nde_hashtab_get(rce_nmehash(name));

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len1 = rce_nmelen(nde_p->name);                                     // string compare required
         int len2 = rce_nmelen(name);
         if (len1 == len2 && 0 == rce_nmecmp(nde_p->name, name, len1))           // lexical comparision
         {
            rc = nde2rcevt(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcevt(nde_p);                                                  // return result

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
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_create_name(RCEVT_NAME const name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle;

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

            rc = nde2rcevt(nde_p);                                               // return result
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
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_BOOL -- Boolean reflecting event previously created
*/
RCEVT_BOOL rcevt_search_name(RCEVT_NAME const name)
{
   RCEVT_BOOL rc = RCEVT_FALSE;                                                  // return result

   if (RCE_NULL != internal_map_name(name))
   {
      rc = RCEVT_TRUE;                                                           // return result
   }

   return rc;                                                                    // return status or failure
}

/**
API, Count of registrants for event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
int rcevt_getregistrants_handle(RCEVT_HANDLE const handle)
{
   int rc = 0;                                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = nde_p->notify_head;                                                // locate notify registrant

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         RCEVT_SIGEX_TYPE nfy_p_type = (RCEVT_SIGEX_TYPE)nfy_p->sigex_type;

         switch (nfy_p_type)
         {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
            case RCEVT_SIGEX_TYPE_SIGDAL:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
            case RCEVT_SIGEX_TYPE_SIGPOSIX:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
            case RCEVT_SIGEX_TYPE_SIGQURT:
#endif
#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
            case RCEVT_SIGEX_TYPE_SIGREX:
#endif
            case RCEVT_SIGEX_TYPE_SIGNATIVE:
               rc++;
               break;

            default:
            case RCEVT_SIGEX_TYPE_UNKNOWN:
            case RCEVT_SIGEX_TYPE_NONE:
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
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
int rcevt_getregistrants_name(RCEVT_NAME const name)
{
   int rc = 0;                                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_getregistrants_handle(handle);
   }

   return rc;                                                                    // return status or failure
}

/**
API, Count of times event signaled, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_THRESHOLD -- Count of registrants for specific event
*/
RCEVT_THRESHOLD rcevt_getcount_handle(RCEVT_HANDLE const handle)
{
   RCEVT_THRESHOLD rc = RCE_THRESHOLD_INIT;                                      // default
   rce_nde_p nde_p = rcevt2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   rc = nde_p->threshold;

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Count of times event signaled, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_THRESHOLD -- Count of registrants for specific event
*/
RCEVT_THRESHOLD rcevt_getcount_name(RCEVT_NAME const name)
{
   RCEVT_THRESHOLD rc = RCE_THRESHOLD_INIT;                                      // default
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rce_nde_p nde_p = rcevt2nde(handle);

      rce_mutex_lock_dal(&nde_p->mutex);

      rc = nde_p->threshold;

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_sigex_update(RCEVT_HANDLE const handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nde_p nde_p;
   rce_nfy_p nfy_p;

   nde_p = rcevt2nde(handle);
   nfy_p = nde_p->notify_head;                                                   // locate notify registrant

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      RCEVT_SIGEX_TYPE nfy_p_type = (RCEVT_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCEVT_SIGEX_TYPE_SIGDAL:
         {
            RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)sigex;
            RCEVT_SIGEX_SIGDAL* nfy_p_sigex_p = (RCEVT_SIGEX_SIGDAL*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p->threshold = threshold;
               rc = nfy_p;
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCEVT_SIGEX_TYPE_SIGPOSIX:
         {
            RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)sigex;
            RCEVT_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->mutex_p == nfy_p_sigex_p->mutex_p)
            {
               nfy_p_sigex_p->cond_p = sigex_p->cond_p;
               nfy_p->threshold = threshold;
               rc = nfy_p;
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCEVT_SIGEX_TYPE_SIGQURT:
         {
            RCEVT_SIGEX_SIGQURT* sigex_p = (RCEVT_SIGEX_SIGQURT*)sigex;
            RCEVT_SIGEX_SIGQURT* nfy_p_sigex_p = (RCEVT_SIGEX_SIGQURT*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p_sigex_p->mask = sigex_p->mask;
               nfy_p->threshold = threshold;
               rc = nfy_p;
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCEVT_SIGEX_TYPE_SIGREX:
         {
            RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)sigex;
            RCEVT_SIGEX_SIGREX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGREX*)&nfy_p->sigex;
            if (type == nfy_p_type && sigex_p->signal == nfy_p_sigex_p->signal)
            {
               nfy_p_sigex_p->mask = sigex_p->mask;
               nfy_p->threshold = threshold;
               rc = nfy_p;
            }
         }
            break;
#endif

         default:
         case RCEVT_SIGEX_TYPE_SIGNATIVE:
         case RCEVT_SIGEX_TYPE_UNKNOWN:
         case RCEVT_SIGEX_TYPE_NONE:
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
static rce_nfy_p internal_register_sigex_add(RCEVT_HANDLE const handle, RCEVT_THRESHOLD threshold, RCEVT_SIGEX_TYPE type, RCEVT_SIGEX sigex)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcevt2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = (rce_sigex_t)type;                                     // update member

      switch (type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCEVT_SIGEX_TYPE_SIGDAL:
         {
            RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)sigex;
            RCEVT_SIGEX_SIGDAL* nfy_p_sigex_p = (RCEVT_SIGEX_SIGDAL*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p->threshold = threshold;
            rc = nfy_p;
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCEVT_SIGEX_TYPE_SIGPOSIX:
         {
            RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)sigex;
            RCEVT_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            nfy_p_sigex_p->mutex_p = sigex_p->mutex_p;
            nfy_p_sigex_p->cond_p = sigex_p->cond_p;
            nfy_p->threshold = threshold;
            rc = nfy_p;
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCEVT_SIGEX_TYPE_SIGQURT:
         {
            RCEVT_SIGEX_SIGQURT* sigex_p = (RCEVT_SIGEX_SIGQURT*)sigex;
            RCEVT_SIGEX_SIGQURT* nfy_p_sigex_p = (RCEVT_SIGEX_SIGQURT*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p_sigex_p->mask = sigex_p->mask;
            nfy_p->threshold = threshold;
            rc = nfy_p;
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCEVT_SIGEX_TYPE_SIGREX:
         {
            RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)sigex;
            RCEVT_SIGEX_SIGREX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGREX*)&nfy_p->sigex;
            nfy_p_sigex_p->signal = sigex_p->signal;
            nfy_p_sigex_p->mask = sigex_p->mask;
            nfy_p->threshold = threshold;
            rc = nfy_p;
         }
            break;
#endif

         default:
         case RCEVT_SIGEX_TYPE_SIGNATIVE:
         case RCEVT_SIGEX_TYPE_UNKNOWN:
         case RCEVT_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }
   }

   return rc;                                                                    // return result
}

/**
API, Register notification to event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_register_sigex_handle(RCEVT_HANDLE const handle, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_THRESHOLD threshold = RCE_THRESHOLD_DEFAULT;                            // default
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != handle)
   {
      rce_nfy_p nfy_p = RCE_NULL;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = internal_register_sigex_update(handle, threshold, type, sigex);    // update an existing

      if (RCE_NULL != nfy_p)
      {
         rc = handle;                                                            // return result
      }
      else
      {
         nfy_p = internal_register_sigex_add(handle, threshold, type, sigex);    // add a new

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
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_register_sigex_name(RCEVT_NAME const name, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_register_sigex_handle(handle, type, sigex);
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCEVT_HANDLE internal_unregister_sigex(RCEVT_HANDLE const handle, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nde_p nde_p = rcevt2nde(handle);
   rce_nfy_p nfy_p;

   rce_mutex_lock_dal(&nde_p->mutex);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      RCEVT_SIGEX_TYPE nfy_p_type = (RCEVT_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCEVT_SIGEX_TYPE_SIGDAL:
         {
            RCEVT_SIGEX_SIGDAL* sigex_p = (RCEVT_SIGEX_SIGDAL*)sigex;
            RCEVT_SIGEX_SIGDAL* nfy_p_sigex_p = (RCEVT_SIGEX_SIGDAL*)&nfy_p->sigex;
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
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcevt(nde_p);                                            // return result
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCEVT_SIGEX_TYPE_SIGPOSIX:
         {
            RCEVT_SIGEX_SIGPOSIX* sigex_p = (RCEVT_SIGEX_SIGPOSIX*)sigex;
            RCEVT_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGPOSIX*)&nfy_p->sigex;
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
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcevt(nde_p);                                            // return result
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCEVT_SIGEX_TYPE_SIGQURT:
         {
            RCEVT_SIGEX_SIGQURT* sigex_p = (RCEVT_SIGEX_SIGQURT*)sigex;
            RCEVT_SIGEX_SIGQURT* nfy_p_sigex_p = (RCEVT_SIGEX_SIGQURT*)&nfy_p->sigex;
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
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcevt(nde_p);                                            // return result
            }
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCEVT_SIGEX_TYPE_SIGREX:
         {
            RCEVT_SIGEX_SIGREX* sigex_p = (RCEVT_SIGEX_SIGREX*)sigex;
            RCEVT_SIGEX_SIGREX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGREX*)&nfy_p->sigex;
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
               rce_nfy_pool_free(nfy_p);                                         // restore to pool
               rc = nde2rcevt(nde_p);                                            // return result
            }
         }
            break;
#endif

         default:
         case RCEVT_SIGEX_TYPE_SIGNATIVE:
         case RCEVT_SIGEX_TYPE_UNKNOWN:
         case RCEVT_SIGEX_TYPE_NONE:
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
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_unregister_sigex_handle(RCEVT_HANDLE const handle, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   rc = internal_unregister_sigex(handle, type, sigex);                          // existing

   return rc;                                                                    // return result
}

/**
API, Unegister notification to event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_unregister_sigex_name(RCEVT_NAME const name, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_unregister_sigex_handle(handle, type, sigex);                   // existing
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_signal_handle(RCEVT_HANDLE const handle)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      nde_p->threshold += 1;                                                     // increment threshold

      nfy_p = nde_p->notify_head;                                                // traverse notify queue

      while (RCE_NULL != nfy_p)                                                  // iterate
      {
         rce_nfy_p nfy_p_next = nfy_p->next;                                     // memorize nfy_p->next allowing callback to unregister in its handler
         RCEVT_SIGEX_TYPE nfy_p_type = (RCEVT_SIGEX_TYPE)nfy_p->sigex_type;

         switch (nfy_p_type)
         {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
            case RCEVT_SIGEX_TYPE_SIGDAL:
            {
               RCEVT_SIGEX_SIGDAL* nfy_p_sigex_p = (RCEVT_SIGEX_SIGDAL*)&nfy_p->sigex;
               if (nde_p->threshold >= nfy_p->threshold)
               {
                  DALSYS_EventCtrl(nfy_p_sigex_p->signal, DALSYS_EVENT_CTRL_TRIGGER);
                  rc = handle;
               }
            }
               break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
            case RCEVT_SIGEX_TYPE_SIGPOSIX:
            {
               RCEVT_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGPOSIX*)&nfy_p->sigex;
               if (nde_p->threshold >= nfy_p->threshold)
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
            case RCEVT_SIGEX_TYPE_SIGQURT:
            {
               RCEVT_SIGEX_SIGQURT* nfy_p_sigex_p = (RCEVT_SIGEX_SIGQURT*)&nfy_p->sigex;
               if (nde_p->threshold >= nfy_p->threshold)
               {
                  qurt_anysignal_set(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
                  rc = handle;
               }
            }
               break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
            case RCEVT_SIGEX_TYPE_SIGREX:
            {
               RCEVT_SIGEX_SIGREX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGREX*)&nfy_p->sigex;
               if (nde_p->threshold >= nfy_p->threshold)
               {
                  rex_set_sigs(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
                  rc = handle;
               }
            }
               break;
#endif

            default:
            case RCEVT_SIGEX_TYPE_SIGNATIVE:
            case RCEVT_SIGEX_TYPE_UNKNOWN:
            case RCEVT_SIGEX_TYPE_NONE:
               /*NULL*/
               break;
         }

         nfy_p = nfy_p_next;                                                     // iterate to next using memorized value
      }

      rce_mutex_unlock_dal(&nde_p->mutex);

      rc = handle;
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_signal_name(RCEVT_NAME const name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_signal_handle(handle);                                          // signal handle
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCEVT_HANDLE internal_wait_common(RCEVT_HANDLE const handle, RCEVT_THRESHOLD const threshold, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcevt2nde(handle);
   rce_nfy_p nfy_p = RCE_NULL;

   rce_mutex_lock_dal(&nde_p->mutex);

   if (nde_p->threshold < threshold)                                             // implicit register
   {
      nfy_p = internal_register_sigex_update(handle, threshold, type, sigex);    // update an existing

      if (RCE_NULL != nfy_p)
      {
         rc = handle;                                                            // return result
      }
      else
      {
         nfy_p = internal_register_sigex_add(handle, threshold, type, sigex);    // add a new

         if (RCE_NULL != nfy_p)
         {
            rc = handle;                                                         // return result
         }
      }
   }
   else
   {
       rce_mutex_unlock_dal(&nde_p->mutex);
       return handle;
   }
   rce_mutex_unlock_dal(&nde_p->mutex);

   if (rc)
   {
      RCEVT_SIGEX_TYPE nfy_p_type = (RCEVT_SIGEX_TYPE)nfy_p->sigex_type;

      switch (nfy_p_type)
      {
#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)
         case RCEVT_SIGEX_TYPE_SIGDAL:
         {
            RCEVT_SIGEX_SIGDAL* nfy_p_sigex_p = (RCEVT_SIGEX_SIGDAL*)&nfy_p->sigex;
            DALSYS_EventWait(nfy_p_sigex_p->signal);
            DALSYS_EventCtrl(nfy_p_sigex_p->signal, DALSYS_EVENT_CTRL_RESET);
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
         case RCEVT_SIGEX_TYPE_SIGPOSIX:
         {
            RCEVT_SIGEX_SIGPOSIX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGPOSIX*)&nfy_p->sigex;
            pthread_mutex_lock(nfy_p_sigex_p->mutex_p);
            pthread_cond_wait(nfy_p_sigex_p->cond_p, nfy_p_sigex_p->mutex_p);
            pthread_mutex_unlock(nfy_p_sigex_p->mutex_p);
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)
         case RCEVT_SIGEX_TYPE_SIGQURT:
         {
            RCEVT_SIGEX_SIGQURT* nfy_p_sigex_p = (RCEVT_SIGEX_SIGQURT*)&nfy_p->sigex;
            qurt_anysignal_wait(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
            qurt_anysignal_clear(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
         }
            break;
#endif

#if !defined(RCINIT_EXCLUDE_KERNEL_REX)
         case RCEVT_SIGEX_TYPE_SIGREX:
         {
            RCEVT_SIGEX_SIGREX* nfy_p_sigex_p = (RCEVT_SIGEX_SIGREX*)&nfy_p->sigex;
            rex_wait(nfy_p_sigex_p->mask);
            rex_clr_sigs(nfy_p_sigex_p->signal, nfy_p_sigex_p->mask);
         }
            break;
#endif

         default:
         case RCEVT_SIGEX_TYPE_SIGNATIVE:
         case RCEVT_SIGEX_TYPE_UNKNOWN:
         case RCEVT_SIGEX_TYPE_NONE:
            /*NULL*/
            break;
      }
   }

   return rc;                                                                    // return result
}

/**
API, Wait for event at specific threshold, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@param[in] Notification begins with threshold and continues until unregistered
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_sigex_handle(RCEVT_HANDLE const handle, RCEVT_THRESHOLD const threshold, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   if (RCE_NULL != handle)
   {
      rc = internal_wait_common(handle, threshold, type, sigex);                 // wait
   }

   return rc;
}

/**
API, Wait for event at specific threshold, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@param[in] Notification begins with threshold and continues until unregistered
@param[in] Notification type (DAL, POSIX, QURT, REX)
@param[in] Notification sigex structure pointer, type specific values
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_sigex_name(RCEVT_NAME const name, RCEVT_THRESHOLD const threshold, RCEVT_SIGEX_TYPE const type, RCEVT_SIGEX const sigex)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = internal_wait_common(handle, threshold, type, sigex);                 // wait
   }

   return rc;
}

/**
API, Wait for event, one time per call, until threshold met, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_count_handle(RCEVT_HANDLE const handle, RCEVT_THRESHOLD const threshold)
{
   RCEVT_HANDLE rc = RCE_NULL;

#if !defined(RCINIT_EXCLUDE_KERNEL_DAL)

   DALSYSEventObj signalObject;
   RCEVT_SIGEX_SIGDAL sig;

   // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
   if (DAL_SUCCESS == DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &sig.signal, &signalObject))
   {
      // Waiting causes an implicit registration
      rc = internal_wait_common(handle, threshold, RCEVT_SIGEX_TYPE_SIGDAL, &sig);

      // Unregister the implicit addition from waiting
      if (internal_unregister_sigex(handle, RCEVT_SIGEX_TYPE_SIGDAL, &sig)) { ; }

      // DAL Event Objects Are Protected on Context Allowed to Wait (Create/Wait/Destroy)
      if (DAL_SUCCESS == DALSYS_DestroyObject(sig.signal)) { ; }
   }

#endif

   return rc;                                                                    // return result
}

/**
API, Wait for event, one time per call, with handle
See prerequsite note with rcevt_create_name
@param[in] Opaque handle of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_handle(RCEVT_HANDLE const handle)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   rc = rcevt_wait_count_handle(handle, RCE_THRESHOLD_DEFAULT);

   return rc;                                                                    // return result
}

/**
API, Wait for event, one time per call, until threshold met, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_count_name(RCEVT_NAME const name, RCEVT_THRESHOLD const threshold)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result
   RCEVT_HANDLE handle = rcevt_create_name(name);

   if (RCE_NULL != handle)
   {
      rc = rcevt_wait_count_handle(handle, threshold);
   }

   return rc;                                                                    // return result
}

/**
API, Wait for event, one time per call, with name
See prerequsite note with rcevt_create_name
@param[in] NULL terminated string, name of the event
@return
RCEVT_NULL -- Error
RCEVT_HANDLE -- Opaque handle to existing event
*/
RCEVT_HANDLE rcevt_wait_name(RCEVT_NAME const name)
{
   RCEVT_HANDLE rc = RCE_NULL;                                                   // return result

   rc = rcevt_wait_count_name(name, RCE_THRESHOLD_DEFAULT);

   return rc;                                                                    // return result
}
