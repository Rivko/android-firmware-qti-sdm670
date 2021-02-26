/** vi: tw=128 ts=3 sw=3 et :
@file rcevt_common.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcevt/src/rcevt_common.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "stringl/stringl.h"

#include "rcevt.h"
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

rce_hash_t rce_nmehash(RCEVT_NAME const name)
{
   RCEVT_NAME s = name;
   rce_hash_t hash = 0;

   while ('\0' != *s)
   {
      rce_hash_t temp;

      hash = (hash << 4) + (rce_hash_t)(*s);

      if (0 != (temp = hash & 0xf0000000))
      {
         hash ^= (temp >> 24);
      }

      hash &= (~temp);

      s++;
   }

   return hash % (sizeof(rce_hashtab) / sizeof(rce_nde_p));
}

int rce_nmelen(RCEVT_NAME const name)
{
   RCEVT_NAME s;

   for (s = name; '\0' != *s; ++s)
   /* NULL */;

   return s - name;
}

int rce_nmecmp(RCEVT_NAME const name_1, RCEVT_NAME const name_2, int len)
{
   RCEVT_NAME s1 = name_1, s2 = name_2;

   if (0 == len)                                                                 // no work; return
   {
      return 0;                                                                  // lexical compare identical
   }

   while (len-- > 0 && *s1 == *s2)                                               // iterate comparing name strings
   {
      if (0 == len || '\0' == *s1)                                               // len hits zero or NULL terminator
      {
         return 0;                                                               // lexical compare identical
      }

      s1++, s2++;                                                                // setup for next comparision
   }

   return(*s1 < *s2) ? -1 : (*s1 > *s2);                                         // lexical analysis and return result (-1, 0, 1)
}

rce_nfy_p rce_nfy_pool_init(void)
{
   rce_nfy_pool_p next_pool = RCE_NULL;                                          // return result

   if (RCE_NULL == rce_internal.nfy_pool_head_p)                                 // first call use static pool
   {
      next_pool = &rce_nfy_pool_static;                                          // first pool allocation static
   }

   else if (RCEVT_TRUE == rce_internal.dynamic_use)                              // else dynamic allocation allowed; attempt pool expansion
   {
      next_pool = (rce_nfy_pool_p)rce_malloc(sizeof(rce_nfy_pool_t));            // allocate
   }

   if (RCE_NULL != next_pool)                                                    // populate
   {
      int i;

      for (i = 0; i < RCEVT_NOTIFY_POOL_SIZ; i++)                                // initialize pool internal
      {
         if (i != (RCEVT_NOTIFY_POOL_SIZ - 1))
         {
            next_pool->nfy_pool[i].next = &(next_pool->nfy_pool[i + 1]);         // next entry
         }

         else
         {
            next_pool->nfy_pool[i].next = rce_internal.nfy_pool_free_p;          // last entry of pool list
         }

         secure_memset(&(next_pool->nfy_pool[i].sigex), 0, sizeof(next_pool->nfy_pool[i].sigex));

         next_pool->nfy_pool[i].sigex_type = RCEVT_SIGEX_TYPE_NONE;              // initialize member

         next_pool->nfy_pool[i].threshold = 0;                                   // initialize member

         next_pool->nfy_pool[i].nfy_cookie = RCE_NFY_COOKIE;                     // initialize member (set cookie last)
      }

      rce_internal.nfy_pool_free_p = &(next_pool->nfy_pool[0]);                  // initialize free list

      next_pool->next = rce_internal.nfy_pool_head_p;                            // link this pool to the head of the chain

      rce_internal.nfy_pool_head_p = next_pool;                                  // point to the new pool as the head
   }

   else                                                                          // failure for pool expansion
   {
      ERR_FATAL("no resource, pool expansion exhausted", 0, 0, 0);
   }

   return rce_internal.nfy_pool_free_p;                                          // return result
}

rce_nfy_p rce_nfy_pool_alloc(void)
{
   rce_nfy_p rc;
   rce_nfy_p nfy_p;

   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   if (RCE_NULL == rce_internal.nfy_pool_free_p)
   {
      nfy_p = rce_nfy_pool_init();                                               // obtain reference from pool, possible pool expansion
   }

   else
   {
      nfy_p = rce_internal.nfy_pool_free_p;
   }

   if (RCE_NULL != nfy_p)
   {
      rce_internal.nfy_pool_free_p = nfy_p->next;                                // unlink from free list

      nfy_p->next = RCE_NULL;                                                    // initialize member

      nfy_p->sigex_type = RCEVT_SIGEX_TYPE_NONE;                                 // initialize member

      rc = nfy_p;                                                                // return result

   }

   else
   {
      rc = RCE_NULL;
   }

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex

   return rc;                                                                    // return result
}

rce_nfy_p rce_nfy_pool_free(rce_nfy_p nfy_p)
{
   rce_nfy_p rc = RCE_NULL;                                                      // return result

   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   nfy_p->next = rce_internal.nfy_pool_free_p;                                   // return to head of free list

   rce_internal.nfy_pool_free_p = nfy_p;

   rc = nfy_p;                                                                   // return result

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex

   return rc;                                                                    // return result
}

rce_nde_p rce_nde_pool_init(void)
{
   rce_nde_pool_p next_pool = RCE_NULL;                                          // return result

   if (RCE_NULL == rce_internal.nde_pool_head_p)                                 // first call use static pool
   {
      next_pool = &rce_nde_pool_static;                                          // first pool allocation static
   }

   else if (RCEVT_TRUE == rce_internal.dynamic_use)                              // else dynamic allocation allowed; attempt pool expansion
   {
      next_pool = (rce_nde_pool_p)rce_malloc(sizeof(rce_nde_pool_t));            // allocate
   }

   if (RCE_NULL != next_pool)                                                    // populate
   {
      int i;

      for (i = 0; i < RCEVT_NODE_POOL_SIZ; i++)                                  // initialize pool internal
      {
         if (i != (RCEVT_NODE_POOL_SIZ - 1))
         {
            next_pool->nde_pool[i].next = &(next_pool->nde_pool[i + 1]);         // next entry
         }

         else
         {
            next_pool->nde_pool[i].next = rce_internal.nde_pool_free_p;          // last entry of pool list
         }

         secure_memset(&(next_pool->nde_pool[i].mutex), 0, sizeof(next_pool->nde_pool[i].mutex));

         rce_mutex_init_dal(&(next_pool->nde_pool[i].mutex));                    // initialize member

         next_pool->nde_pool[i].notify_head = RCE_NULL;                          // initialize member

         next_pool->nde_pool[i].name = RCE_NULL;                                 // initialize member

         next_pool->nde_pool[i].threshold = 0;                                   // initialize member

         next_pool->nde_pool[i].nde_cookie = RCE_NDE_COOKIE;                     // initialize member (set cookie last)
      }

      rce_internal.nde_pool_free_p = &(next_pool->nde_pool[0]);                  // initialize free list

      next_pool->next = rce_internal.nde_pool_head_p;                            // link this pool to the head of the chain

      rce_internal.nde_pool_head_p = next_pool;                                  // point to the new pool as the head
   }

   else                                                                          // failure for pool expansion
   {
      ERR_FATAL("no resource, pool expansion exhausted", 0, 0, 0);
   }

   return rce_internal.nde_pool_free_p;                                          // return result
}

rce_nde_p rce_nde_pool_alloc(void)
{
   rce_nde_p rc;
   rce_nde_p nde_p;

   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   if (RCE_NULL == rce_internal.nde_pool_free_p)
   {
      nde_p = rce_nde_pool_init();                                               // obtain reference from pool, possible pool expansion
   }
   else
   {
      nde_p = rce_internal.nde_pool_free_p;
   }

   if (RCE_NULL != nde_p)
   {
      rce_internal.nde_pool_free_p = nde_p->next;                                // unlink from free list

      nde_p->next = RCE_NULL;                                                    // initialize member

      nde_p->notify_head = RCE_NULL;                                             // initialize member

      rc = nde_p;                                                                // return result
   }

   else
   {
      rc = RCE_NULL;
   }

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex

   return rc;                                                                    // return result
}

rce_nde_p rce_nde_pool_free(rce_nde_p nde_p)
{
   rce_nde_p rc = RCE_NULL;                                                      // return result

   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   nde_p->next = rce_internal.nde_pool_free_p;                                   // return to head of free list

   rce_internal.nde_pool_free_p = nde_p;

   rc = nde_p;                                                                   // return result

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex

   return rc;                                                                    // return result
}

void rce_hashtab_init(void)
{
   secure_memset(&rce_hashtab, 0, sizeof(rce_hashtab));
}

rce_nde_p rce_nde_hashtab_get(rce_hash_t hash)
{
   rce_nde_p rc = RCE_NULL;

   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   rc = rce_hashtab[hash];                                                       // hashtable offset

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex

   return rc;
}

void rce_nde_hashtab_put(rce_nde_p nde_p, rce_hash_t hash)
{
   rce_mutex_lock_dal(&rce_internal.mutex);                                      // lock mutex

   nde_p->next = rce_hashtab[hash];                                              // initalize next node pointer to current bucket list head

   rce_hashtab[hash] = nde_p;                                                    // insert node pointer to hash bucket list

   rce_mutex_unlock_dal(&rce_internal.mutex);                                    // unlock mutex
}

void rce_init(void)
{
   if (RCE_NDE_COOKIE != rce_internal.init_flag)                                 // first call to insure service init
   {
      DALSYS_InitMod(NULL);                                                      // prerequsite dependency

      rce_mutex_init_dal(&rce_internal.mutex);                                   // initialize mutex

      rce_mutex_init_dal(&rce_internal.mutex_create);

      rce_mutex_lock_dal(&rce_internal.mutex);                                   // lock mutex

      rce_internal.dynamic_use = TRUE;                                           // allow pools to grow at runtime

      rce_hashtab_init();                                                        // allocate initial hash table buckets

      rce_nde_pool_init();                                                       // allocate initial pool storage

      rce_nfy_pool_init();                                                       // allocate initial pool storage

      rce_mutex_unlock_dal(&rce_internal.mutex);                                 // unlock mutex

      rce_internal.init_flag = RCE_NDE_COOKIE;                                   // mark service init
   }
}

void rce_term(void)
{
}
