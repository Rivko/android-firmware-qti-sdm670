/** vi: tw=128 ts=3 sw=3 et :
@file rcecb.c
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
Copyright (c) 2015-2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcevt/src/rcecb.c#3 $
$DateTime: 2019/03/22 05:56:01 $
$Change: 18666992 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"
#include "err.h"
#include "assert.h"
#include "timer.h"

#include "rcecb.h"
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
RCECB_BOOL rcecb_init(void)
{
   timer_group_set_deferrable(&internal_timer_group, TRUE);

   timer_group_enable(&internal_timer_group);

   rce_init();

   return RCECB_TRUE;
}

/**
API, Service termination
@param
None
@return
None
*/
RCECB_BOOL rcecb_term(void)
{
   return RCECB_TRUE;
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCECB_HANDLE internal_map_name(RCECB_NAME const name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p;

   if (rcecb_init()) {;}

   nde_p = rce_nde_hashtab_get(rce_nmehash(name));

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len1 = rce_nmelen(nde_p->name);                                     // string compare required
         int len2 = rce_nmelen(name);
         if (len1 == len2 && 0 == rce_nmecmp(nde_p->name, name, len1))           // lexical comparision
         {
            rc = nde2rcecb(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // break loop for return
      }

      nde_p = nde_p->next;                                                       // iterate to locate
   }

   return rc;                                                                    // return status or failure
}

/**
INTERNAL, Not Exposed for API Use, Specialized, no mutex protections, single thread only STM
NOTE Limitation includes possiblity that callbacks are unmanaged and have incompatibility with STM
with no ability to detect stalling by utilization of a timer.
*/
static RCECB_HANDLE internal_map_name_nolocks(RCECB_NAME const name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_hash_t hash = rce_nmehash(name);                                          // map name to hash
   rce_nde_p nde_p;

   nde_p = rce_hashtab[hash];                                                    // hashtable offset

   while (RCE_NULL != nde_p)                                                     // search for existing node
   {
      if (nde_p->name != name)                                                   // same address compare
      {
         int len1 = rce_nmelen(nde_p->name);                                     // string compare required
         int len2 = rce_nmelen(name);
         if (len1 == len2 && 0 == rce_nmecmp(nde_p->name, name, len1))           // lexical comparision
         {
            rc = nde2rcecb(nde_p);                                               // return result

            break;                                                               // break loop for return
         }
      }

      else                                                                       // same address
      {
         rc = nde2rcecb(nde_p);                                                  // return result

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
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_create_name(RCECB_NAME const name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle;

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

         unsigned long buf_s = rce_nmelen(name) + 1;                             // length of RCECB_NAME
         char* buf_p = rce_malloc(buf_s);                                        // space for RCECB_NAME

         if (RCE_NULL != buf_p)
         {
            strlcpy(buf_p, name, buf_s);                                         // private copy of name

            nde_p->name = buf_p;                                                 // memorize name address

            nde_p->notify_head = RCE_NULL;                                       // initialize notify queue

            rce_nde_hashtab_put(nde_p, rce_nmehash(nde_p->name));

            rc = nde2rcecb(nde_p);                                               // return result
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
API, Search an event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_BOOL -- Boolean reflecting event previously created
*/
RCECB_BOOL rcecb_search_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = nde_p->notify_head;                                                // locate notify registrant

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         if (RCECB_SIGEX_TYPE_CALLBACK_P0 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
             fnsig == (RCECB_CALLBACK_FNSIG)nfy_p->sigex.parm[0])
         {
            rc = RCECB_TRUE;                                                     // return result

            break;                                                               // break loop
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return status or failure
}

/**
API, Search an event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_BOOL -- Boolean reflecting event previously created
*/
RCECB_BOOL rcecb_search_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig)
{
   RCECB_BOOL rc = RCECB_FALSE;                                                  // return result
   RCECB_HANDLE handle = internal_map_name(name);

   rc = rcecb_search_handle(handle, fnsig);

   return rc;                                                                    // return status or failure
}

/**
API, Count of registrants for event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@return
int -- Count of registrants for specific event
*/
int rcecb_getregistrants_handle(RCECB_HANDLE const handle)
{
   int rc = 0;                                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   if (RCE_NULL != nde_p)
   {
      rce_nfy_p nfy_p;

      rce_mutex_lock_dal(&nde_p->mutex);

      nfy_p = nde_p->notify_head;                                                // locate notify registrant

      while (RCE_NULL != nfy_p)                                                  // loop for check
      {
         if (RCECB_SIGEX_TYPE_CALLBACK_P0 <= (RCECB_SIGEX_TYPE)nfy_p->sigex_type && RCECB_SIGEX_TYPE_CALLBACK_P2 >= (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
         {
            rc++;                                                                // return result
         }

         nfy_p = nfy_p->next;                                                    // iterate to next
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }

   return rc;                                                                    // return status or failure
}

/**
API, Count of registrants for event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@return
int -- Count of registrants for specific event
*/
int rcecb_getregistrants_name(RCECB_NAME const name)
{
   int rc = 0;                                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);

   rc = rcecb_getregistrants_handle(handle);

   return rc;                                                                    // return status or failure
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_upd_parm0(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rcecb2nde(handle)->notify_head;                                       // locate notify registrant

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P0 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P0)nfy_p->sigex.parm[0])
      {
         rc = nfy_p;                                                             // return result

         break;                                                                  // break loop
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_upd_parm1(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM p1)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rcecb2nde(handle)->notify_head;                                       // locate notify registrant

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P1 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P1)nfy_p->sigex.parm[0] &&
          p1 == (RCECB_PARM)nfy_p->sigex.parm[1])
      {
         rc = nfy_p;                                                             // return result

         break;                                                                  // break loop
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_upd_parm2(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM p1, RCECB_PARM p2)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rcecb2nde(handle)->notify_head;                                       // locate notify registrant

   while (RCE_NULL != nfy_p)                                                     // loop for check
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P2 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P2)nfy_p->sigex.parm[0] &&
          p1 == (RCECB_PARM)nfy_p->sigex.parm[1] &&
          p2 == (RCECB_PARM)nfy_p->sigex.parm[2])
      {
         rc = nfy_p;                                                             // return result

         break;                                                                  // break loop
      }

      nfy_p = nfy_p->next;                                                       // iterate to next
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_add_parm0(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcecb2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = (rce_sigex_t)RCECB_SIGEX_TYPE_CALLBACK_P0;

      secure_memset(nfy_p->sigex.parm, 0, sizeof(nfy_p->sigex.parm));

      nfy_p->sigex.parm[0] = fnsig;

      rc = nfy_p;                                                                // return result
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_add_parm1(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM p1)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcecb2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = (rce_sigex_t)RCECB_SIGEX_TYPE_CALLBACK_P1;

      secure_memset(nfy_p->sigex.parm, 0, sizeof(nfy_p->sigex.parm));

      nfy_p->sigex.parm[0] = fnsig;
      nfy_p->sigex.parm[1] = (void*)p1;

      rc = nfy_p;                                                                // return result
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nfy_p internal_register_add_parm2(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM p1, RCECB_PARM p2)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = rce_nfy_pool_alloc();

   if (RCE_NULL != nfy_p)                                                        // populate
   {
      rce_nde_p nde_p = rcecb2nde(handle);

      nfy_p->next = nde_p->notify_head;                                          // insert to head of list

      nde_p->notify_head = nfy_p;                                                // update head

      nfy_p->sigex_type = (rce_sigex_t)RCECB_SIGEX_TYPE_CALLBACK_P2;

      secure_memset(nfy_p->sigex.parm, 0, sizeof(nfy_p->sigex.parm));

      nfy_p->sigex.parm[0] = fnsig;
      nfy_p->sigex.parm[1] = (void*)p1;
      nfy_p->sigex.parm[2] = (void*)p2;

      rc = nfy_p;                                                                // return result
   }

   return rc;                                                                    // return result
}

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig)
{
   return rcecb_register_parm0_handle(handle, (RCECB_CALLBACK_FNSIG_P0)fnsig);
}

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm0_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   if (RCE_NULL != internal_register_upd_parm0(handle, fnsig) ||
       RCE_NULL != internal_register_add_parm0(handle, fnsig))
   {
      rc = handle;                                                               // return result
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm1_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   if (RCE_NULL != internal_register_upd_parm1(handle, fnsig, p1) ||
       RCE_NULL != internal_register_add_parm1(handle, fnsig, p1))
   {
      rc = handle;                                                               // return result
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Register callback to event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm2_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   if (RCE_NULL != internal_register_upd_parm2(handle, fnsig, p1, p2) ||
       RCE_NULL != internal_register_add_parm2(handle, fnsig, p1, p2))
   {
      rc = handle;                                                               // return result
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig)
{
   return rcecb_register_parm0_name(name, (RCECB_CALLBACK_FNSIG_P0)fnsig);
}

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm0_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = (RCECB_HANDLE)internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_register_parm0_handle(handle, fnsig);
   }

   return rc;
}

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm1_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = (RCECB_HANDLE)internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_register_parm1_handle(handle, fnsig, p1);
   }

   return rc;
}

/**
API, Register callback to event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_register_parm2_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = (RCECB_HANDLE)internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_register_parm2_handle(handle, fnsig, p1, p2);
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCECB_HANDLE internal_unregister_parm0(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nfy_p nfy_p;
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P0 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P0)nfy_p->sigex.parm[0])
      {
         if (RCE_NULL == nfy_prev)                                               // head of list check
         {
            nde_p->notify_head = nfy_p->next;                                    // remove from head
         }
         else
         {
            nfy_prev->next = nfy_p->next;                                        // remove from list
         }

         nfy_p->next = RCE_NULL;                                                 // depopulate

         rce_nfy_pool_free(nfy_p);                                               // restore to pool

         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // exit loop
      }

      nfy_prev = nfy_p;

      nfy_p = nfy_p->next;                                                       // iterate to check next
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCECB_HANDLE internal_unregister_parm1(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM p1)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nfy_p nfy_p;
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P1 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P1)nfy_p->sigex.parm[0] &&
          p1 == (RCECB_PARM)nfy_p->sigex.parm[1])
      {
         if (RCE_NULL == nfy_prev)                                               // head of list check
         {
            nde_p->notify_head = nfy_p->next;                                    // remove from head
         }
         else
         {
            nfy_prev->next = nfy_p->next;                                        // remove from list
         }

         nfy_p->next = RCE_NULL;                                                 // depopulate

         rce_nfy_pool_free(nfy_p);                                               // restore to pool

         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // exit loop
      }

      nfy_prev = nfy_p;

      nfy_p = nfy_p->next;                                                       // iterate to check next
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use
*/
static RCECB_HANDLE internal_unregister_parm2(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM p1, RCECB_PARM p2)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   rce_nfy_p nfy_prev = RCE_NULL;
   rce_nfy_p nfy_p;
   rce_nde_p nde_p = rcecb2nde(handle);

   rce_mutex_lock_dal(&nde_p->mutex);

   nfy_p = nde_p->notify_head;                                                   // search out on notify queue

   while (RCE_NULL != nfy_p)                                                     // loop for next
   {
      if (RCECB_SIGEX_TYPE_CALLBACK_P2 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type &&
          fnsig == (RCECB_CALLBACK_FNSIG_P2)nfy_p->sigex.parm[0] &&
          p1 == (RCECB_PARM)nfy_p->sigex.parm[1] &&
          p2 == (RCECB_PARM)nfy_p->sigex.parm[2])
      {
         if (RCE_NULL == nfy_prev)                                               // head of list check
         {
            nde_p->notify_head = nfy_p->next;                                    // remove from head
         }
         else
         {
            nfy_prev->next = nfy_p->next;                                        // remove from list
         }

         nfy_p->next = RCE_NULL;                                                 // depopulate

         rce_nfy_pool_free(nfy_p);                                               // restore to pool

         rc = nde2rcecb(nde_p);                                                  // return result

         break;                                                                  // exit loop
      }

      nfy_prev = nfy_p;

      nfy_p = nfy_p->next;                                                       // iterate to check next
   }

   rce_mutex_unlock_dal(&nde_p->mutex);

   return rc;                                                                    // return result
}

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG const fnsig)
{
   return rcecb_unregister_parm0_handle(handle, (RCECB_CALLBACK_FNSIG_P0)fnsig);
}

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm0_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = internal_unregister_parm0(handle, fnsig);

   return rc;                                                                    // return result
}

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm1_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = internal_unregister_parm1(handle, fnsig, p1);

   return rc;                                                                    // return result
}

/**
API, Unegister callback to event, with handle
@param[in] Opaque handle of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm2_handle(RCECB_HANDLE const handle, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = internal_unregister_parm2(handle, fnsig, p1, p2);

   return rc;                                                                    // return result
}

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG const fnsig)
{
   return rcecb_unregister_parm0_name(name, (RCECB_CALLBACK_FNSIG_P0)fnsig);
}

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm0_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P0 const fnsig)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // use of handle, or return failure upwards
   {
      rc = internal_unregister_parm0(handle, fnsig);
   }

   return rc;                                                                    // return result
}

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm1_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P1 const fnsig, RCECB_PARM const p1)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // use of handle, or return failure upwards
   {
      rc = internal_unregister_parm1(handle, fnsig, p1);
   }

   return rc;
}

/**
API, Unegister callback to event, with name
@param[in] NULL terminated string, name of the event
@param[in] Callback function pointer
@param[in] Data Parameter
@param[in] Data Parameter
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_unregister_parm2_name(RCECB_NAME const name, RCECB_CALLBACK_FNSIG_P2 const fnsig, RCECB_PARM const p1, RCECB_PARM const p2)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // use of handle, or return failure upwards
   {
      rc = internal_unregister_parm2(handle, fnsig, p1, p2);
   }

   return rc;
}

/**
INTERNAL, Not Exposed for API Use
*/
static void internal_timer_fail(timer_cb_data_type data)
{
   ERR_FATAL("RCECB client callback runtime exceeded, data = %d", data, 0, 0);

   if (data) {;}
}

/**
INTERNAL, Not Exposed for API Use
*/
static rce_nde_p internal_signal_common(rce_nde_p nde_p, unsigned long* const timetick_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;
   if (RCE_NULL != nde_p)
   {
      rce_mutex_lock_dal(&nde_p->mutex);

      if (timetick_p)                                                               // uses a supplied timetick
      {
         unsigned long timetick = *timetick_p;

         if (timetick != 0) /*if client provided timeout as 0 then we dont want to set the timeout timer */
         {
            timer_def_osal(&nde_p->expiry, &internal_timer_group, TIMER_FUNC1_CB_TYPE, internal_timer_fail, (timer_cb_data_type)(nde_p->notify_head));

            timer_set_64(&nde_p->expiry, timetick, timetick, T_MSEC);
         }
         else
         {
            /* if client provided timeout as 0 then ASSERT */
            ASSERT(0);
         }
      }

      nfy_p = nde_p->notify_head;                                                   // traverse notify queue

      while (RCE_NULL != nfy_p)                                                     // iterate
      {
         rce_nfy_p nfy_p_next = nfy_p->next;                                        // memorize nfy_p->next allowing callback to unregister in its handler

         if (RCECB_SIGEX_TYPE_CALLBACK_P0 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
         {
            RCECB_CALLBACK_FNSIG_P0 callback = (RCECB_CALLBACK_FNSIG_P0)nfy_p->sigex.parm[0];

            callback();                                                             // <<- other tech team callbacks running this registrant

            rc = nde_p;                                                             // return result
         }

         else if (RCECB_SIGEX_TYPE_CALLBACK_P1 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
         {
            RCECB_CALLBACK_FNSIG_P1 callback = (RCECB_CALLBACK_FNSIG_P1)nfy_p->sigex.parm[0];
            RCECB_PARM p1 = (RCECB_PARM)nfy_p->sigex.parm[1];

            callback(p1);                                                           // <<- other tech team callbacks running this registrant

            rc = nde_p;                                                             // return result
         }

         else if (RCECB_SIGEX_TYPE_CALLBACK_P2 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
         {
            RCECB_CALLBACK_FNSIG_P2 callback = (RCECB_CALLBACK_FNSIG_P2)nfy_p->sigex.parm[0];
            RCECB_PARM p1 = (RCECB_PARM)nfy_p->sigex.parm[1];
            RCECB_PARM p2 = (RCECB_PARM)nfy_p->sigex.parm[2];

            callback(p1, p2);                                                       // <<- other tech team callbacks running this registrant

            rc = nde_p;                                                             // return result
         }

         nfy_p = nfy_p_next;                                                        // iterate to next using memorized value
      }

      if (timetick_p)                                                               // uses a supplied timetick
      {
         unsigned long timetick = timer_get_64(&nde_p->expiry, T_MSEC);

         *timetick_p = timetick;

         timer_clr_64(&nde_p->expiry, T_NONE);

         timer_undef(&nde_p->expiry);
      }

      rce_mutex_unlock_dal(&nde_p->mutex);
   }
   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use, Specialized, no mutex protections, single thread only STM
NOTE Limitation includes possiblity that callbacks are unmanaged and have incompatibility with STM
with no ability to detect stalling by utilization of a timer.
*/
static rce_nde_p internal_signal_common_nolocks(rce_nde_p nde_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result
   rce_nfy_p nfy_p;

   nfy_p = nde_p->notify_head;                                                   // traverse notify queue

   while (RCE_NULL != nfy_p)                                                     // iterate
   {
      rce_nfy_p nfy_p_next = nfy_p->next;                                        // memorize nfy_p->next allowing callback to unregister in its handler

      if (RCECB_SIGEX_TYPE_CALLBACK_P0 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
      {
         RCECB_CALLBACK_FNSIG_P0 callback = (RCECB_CALLBACK_FNSIG_P0)nfy_p->sigex.parm[0];

         callback();                                                             // <<- other tech team callbacks running this registrant

         rc = nde_p;                                                             // return result
      }

      else if (RCECB_SIGEX_TYPE_CALLBACK_P1 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
      {
         RCECB_CALLBACK_FNSIG_P1 callback = (RCECB_CALLBACK_FNSIG_P1)nfy_p->sigex.parm[0];
         RCECB_PARM p1 = (RCECB_PARM)nfy_p->sigex.parm[1];

         callback(p1);                                                           // <<- other tech team callbacks running this registrant

         rc = nde_p;                                                             // return result
      }

      else if (RCECB_SIGEX_TYPE_CALLBACK_P2 == (RCECB_SIGEX_TYPE)nfy_p->sigex_type)
      {
         RCECB_CALLBACK_FNSIG_P2 callback = (RCECB_CALLBACK_FNSIG_P2)nfy_p->sigex.parm[0];
         RCECB_PARM p1 = (RCECB_PARM)nfy_p->sigex.parm[1];
         RCECB_PARM p2 = (RCECB_PARM)nfy_p->sigex.parm[2];

         callback(p1, p2);                                                       // <<- other tech team callbacks running this registrant

         rc = nde_p;                                                             // return result
      }

      nfy_p = nfy_p_next;                                                        // iterate to next using memorized value
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_signal_handle(RCECB_HANDLE const handle)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   unsigned long timetick = RCECB_CALLBACK_MAX_TIME;

   rc = nde2rcecb(internal_signal_common(rcecb2nde(handle), &timetick));         // signal handle

   return rc;                                                                    // return result
}

/**
API, Signal event, with handle
See prerequsite note with rcecb_create_name
@param[in] Opaque handle of the event
@param[in] Maximum timer value associated to event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_signal_handle_timetick(RCECB_HANDLE const handle, unsigned long* const timetick_p)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = nde2rcecb(internal_signal_common(rcecb2nde(handle), timetick_p));        // signal handle

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use, Specialized, no mutex protections, single thread only STM
NOTE Limitation includes possiblity that callbacks are unmanaged and have incompatibility with STM
with no ability to detect stalling by utilization of a timer.
*/
RCECB_HANDLE rcecb_signal_handle_nolocks(RCECB_HANDLE const handle)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result

   rc = nde2rcecb(internal_signal_common_nolocks(rcecb2nde(handle)));            // signal handle

   return rc;                                                                    // return result
}

/**
API, Signal event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_signal_name(RCECB_NAME const name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);
   unsigned long timetick = RCECB_CALLBACK_MAX_TIME;

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_signal_handle_timetick(handle, &timetick);                      // signal handle
   }

   return rc;                                                                    // return result
}

/**
API, Signal event, with name
See prerequsite note with rcecb_create_name
@param[in] NULL terminated string, name of the event
@return
RCECB_NULL -- Error
RCECB_HANDLE -- Opaque handle to existing event
*/
RCECB_HANDLE rcecb_signal_name_timetick(RCECB_NAME const name, unsigned long* const timetick_p)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name(name);

   if (RCE_NULL == handle)                                                       // check for existing
   {
      handle = rcecb_create_name(name);                                          // implicit creation
   }

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_signal_handle_timetick(handle, timetick_p);                     // signal handle
   }

   return rc;                                                                    // return result
}

/**
INTERNAL, Not Exposed for API Use, Specialized, no mutex protections, single thread only
NOTE Limitation includes possiblity that callbacks are unmanaged and have incompatibility with STM
with no ability to detect stalling by utilization of a timer.
*/
RCECB_HANDLE rcecb_signal_name_nolocks(RCECB_NAME const name)
{
   RCECB_HANDLE rc = RCE_NULL;                                                   // return result
   RCECB_HANDLE handle = internal_map_name_nolocks(name);

   if (RCE_NULL != handle)                                                       // check for existing
   {
      rc = rcecb_signal_handle_nolocks(handle);                                  // signal handle
   }

   return rc;                                                                    // return result
}
