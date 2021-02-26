/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_lookup.c
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcinit/src/rcinit_lookup.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#define DLL_API_BUILD_RCINIT

#include "ctype.h"
#include "rcinit_dal.h"
#include "tms_rcinit.h"

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Localized Storage
/////////////////////////////////////////////////////////////////////

/**
API, Lookup API Available (Indirectly Reflects NHLOS Tasking Available)
@returns
RCINIT_BOOL
*/
RCINIT_BOOL rcinit_lookup_available(void)
{
   /** API IS NOT AVAILABLE BEFORE NHLOS TASKING, NEVER ATTEMPT TO MAKE IT AVAILABLE EARLIER
   */

   if (RCINIT_TASK_TLS_AVAILABLE == rcinit_internal_tls_available) // TLS API available
   {
      return RCINIT_TRUE;
   }

   return RCINIT_FALSE;
}

int rcinit_internal_nmelen(RCINIT_NAME const name)
{
   RCINIT_NAME ptr = name;                                                       // return result

   while ('\0' != *ptr)                                                          // iterate to locate NULL terminator
   {
      ptr++;                                                                     // setup for next comparision
   }

   return ptr - name;                                                            // return length of name (sans NULL terminator)
}

int rcinit_internal_nmecmp(RCINIT_NAME name1, RCINIT_NAME name2, int len)
{
   if (0 == len)                                                                 // no work; return
   {
      return 0;                                                                  // lexical compare identical
   }

   while (len-- > 0 && tolower(*name1) == tolower(*name2))                       // iterate comparing names
   {
      if (0 == len || '\0' == *name1)                                            // len hits zero or NULL terminator
      {
         return 0;                                                               // lexical compare identical
      }

      name1++, name2++;                                                          // setup for next comparision
   }

   return(tolower(*name1) < tolower(*name2)) ? -1 : (tolower(*name1) > tolower(*name2)); // lexical analysis and return result (-1, 0, 1)
}

unsigned long rcinit_internal_nmehash(RCINIT_NAME const name)
{
   RCINIT_NAME s = name;
   unsigned long hash = 0;

   while ('\0' != *s)
   {
      unsigned long temp;

      hash = (hash << 4) + (unsigned long)(*s);

      if (0 != (temp = hash & 0xf0000000))
      {
         hash ^= (temp >> 24);
      }

      hash &= (~temp);

      s++;
   }

   return hash;
}

int rcinit_internal_name_map_search(RCINIT_NAME const value, int len, int low, int high)
{
   while (low <= high)
   {
      int compare;
      int mid = (low + high) / 2;

      compare = rcinit_internal_nmecmp(rcinit_internal_name_map[mid].name, value, len); // compare array[mid] to value

      if (0 == compare)                                                          // array[mid] == value
      {
         return mid;
      }
      else if (compare > 0)                                                      // array[mid] > value
      {
         high = mid - 1;
      }
      else                                                                       // array[mid] < value
      {
         low = mid + 1;
      }
   }
   return -1;
}

int rcinit_internal_policy_map_search(RCINIT_NAME const value, int len, int low, int high)
{
   while (low <= high)
   {
      int compare;
      int mid = (low + high) / 2;

      compare = rcinit_internal_nmecmp(rcinit_internal_policy_map[mid].name, value, len); // compare array[mid] to value

      if (0 == compare)                                                          // array[mid] == value
      {
         return mid;
      }
      else if (compare > 0)                                                      // array[mid] > value
      {
         high = mid - 1;
      }
      else                                                                       // array[mid] < value
      {
         low = mid + 1;
      }
   }
   return RCINIT_POLICY_NONE;
}

#if 0
int rcinit_internal_priority_map_search(RCINIT_NAME const value, int len, int low, int high)
{
   while (low <= high)
   {
      int compare;
      int mid = (low + high) / 2;

      compare = rcinit_internal_nmecmp(rcinit_internal_priority_map[mid].name, value, len); // compare array[mid] to value

      if (0 == compare)                                                          // array[mid] == value
      {
         return mid;
      }
      else if (compare > 0)                                                      // array[mid] > value
      {
         high = mid - 1;
      }
      else                                                                       // array[mid] < value
      {
         low = mid + 1;
      }
   }
   return -1;
}
#endif

/**
API, name to RCINIT_INFO Opaque handle
@param[in] NULL terminated string, string identifier for task context
@returns
NULL -- Error
RCINIT_INFO -- Opaque handle representing an RCINIT_INFO object
*/
RCINIT_INFO rcinit_lookup(RCINIT_NAME const name)
{
   int offset = rcinit_internal_name_map_search(name, rcinit_internal_nmelen(name) + 1, 0, rcinit_internal_name_map_size - 2);

   if (-1 != offset)
   {
      return(RCINIT_INFO)(rcinit_internal_name_map[offset].info_p);
   }

   return RCINIT_NULL;
}

RCINIT_INFO rcinit_lookup_self(void)
{
   if (rcinit_lookup_available()) // TLS API available
   {
      const rcinit_info_t* rc = (const rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

      return(RCINIT_INFO)rc;
   }

   return RCINIT_NULL;
}

/**
API, RCINIT_INFO Opaque handle to name
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
NULL -- Error
RCINIT_NAME -- NULL terminated string, string identifier for task context
*/
RCINIT_NAME rcinit_lookup_name_info(RCINIT_INFO const info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   return rcinit_p->name;
}

/**
API, name to Policy Extension Inclusion (returns only First Occurance if Multiple)
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_POLICY -- Returns the Policy Representation
*/
RCINIT_POLICY rcinit_lookup_policy(RCINIT_NAME const name)
{
   int offset = rcinit_internal_policy_map_search(name, rcinit_internal_nmelen(name) + 1, 0, rcinit_internal_policy_map_size - 2);

   if (-1 != offset)
   {
      return rcinit_internal_policy_map[offset].index;
   }

   return RCINIT_POLICY_NONE;
}

/**
API, Current Policy Extension (eg, returns default)
@returns
RCINIT_POLICY -- Returns the Policy Representation
*/
RCINIT_POLICY rcinit_lookup_policy_current(void)
{
   return rcinit_internal.policy_curr;
}

/**
API, RCINIT_INFO Opaque handle to Pointer Provisioned at Compile Time Presense
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
NULL -- Netagive Query for Presense
void* -- Positive Query for Presense
*/
void* rcinit_lookup_entry_info(RCINIT_INFO const info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   return rcinit_p->entry;
}

/**
API, name to Pointer Provisioned at Compile Time Presense
@param[in] NULL terminated string, string identifier for task context
@returns
NULL -- Netagive Query for Presense
void* -- Positive Query for Presense
*/
void* rcinit_lookup_entry(RCINIT_NAME const name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return rcinit_lookup_entry_info(info);
   }

   return RCINIT_NULL;
}

/**
API, RCINIT_INFO Opaque handle to Task Priority Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
RCINIT_PRIO -- Returns the Priority Representation
*/
RCINIT_PRIO rcinit_lookup_prio_info(RCINIT_INFO const info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   return rcinit_p->prio;
}

/**
API, name to Task Priority Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_PRIO -- Returns the Priority Representation
*/
RCINIT_PRIO rcinit_lookup_prio(RCINIT_NAME const name)
{
   if (rcinit_lookup_available()) // TLS API available
   {
      RCINIT_INFO info = rcinit_lookup(name);

      if (RCINIT_NULL != info)
      {
         return rcinit_lookup_prio_info(info);
      }
   }
   return RCINIT_ZERO;
}

/**
API, RCINIT_INFO Opaque handle to Task Stack Size Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
unsigned long -- Returns the Stack Size Representation
*/
unsigned long rcinit_lookup_stksz_info(RCINIT_INFO const info)
{
   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;

   return rcinit_p->stksz;
}

/**
API, name to Task Stack Size Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
unsigned long -- Returns the Stack Size Representation
*/
unsigned long rcinit_lookup_stksz(RCINIT_NAME const name)
{
   RCINIT_INFO info = rcinit_lookup(name);

   if (RCINIT_NULL != info)
   {
      return rcinit_lookup_stksz_info(info);
   }

   return RCINIT_ZERO;
}

//unsigned long rcinit_lookup_cpu_affinity_info(RCINIT_INFO const info)
//{
//   rcinit_info_t* rcinit_p = (rcinit_info_t*)info;
//
//   return rcinit_p->cpu_affinity;
//}

//unsigned long rcinit_lookup_cpu_affinity(RCINIT_NAME const name)
//{
//   RCINIT_INFO info = rcinit_lookup(name);
//
//   if (RCINIT_NULL != info)
//   {
//      return rcinit_lookup_cpu_affinity_info(info);
//   }
//
//   return RCINIT_ZERO;
//}

/**
API, name to Task Group Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
RCINIT_GROUP rcinit_lookup_group(RCINIT_NAME const name)
{
   RCINIT_GROUP rc = RCINIT_GROUP_0;
   const rcinit_info_t*** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_groups[rcinit_internal.policy_curr];

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_t** rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         const rcinit_info_t* rcinit_p = *rcinit_group;

         int compare = rcinit_internal_nmecmp(rcinit_p->name, name, rcinit_internal_nmelen(rcinit_p->name) + 1);

         if (0 == compare)
         {
            return rc;
         }

         rcinit_group++;                                                         // next; this group
      }

      rc++;                                                                      // processing next group

      rcinit_sequence_group++;                                                   // next; sequence group
   }

   return RCINIT_GROUP_NONE;                                                     // no task context in framework processing
}

/**
API, entry to Task Group Provisioned at Compile Time
@param[in] Address of entry point
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
RCINIT_GROUP rcinit_lookup_group_entry(void* const entry)
{
   RCINIT_GROUP rc = RCINIT_GROUP_0;
   const rcinit_info_t*** rcinit_sequence_group;

   rcinit_sequence_group = rcinit_internal_groups[rcinit_internal.policy_curr];

   while (RCINIT_NULL != *rcinit_sequence_group)
   {
      const rcinit_info_t** rcinit_group = *rcinit_sequence_group;

      while (RCINIT_NULL != *rcinit_group)
      {
         const rcinit_info_t* rcinit_p = *rcinit_group;

         if (RCINIT_NULL != rcinit_p->entry)
         {
            if (rcinit_p->entry == entry)                                        // entry pointer associates with group
            {
               return rc;
            }
         }

         rcinit_group++;                                                         // next; this group
      }

      rc++;                                                                      // processing next group

      rcinit_sequence_group++;                                                   // next; sequence group
   }

   return RCINIT_GROUP_NONE;                                                     // no task context in framework processing
}

/**
API, Current group processing during initialization or termination
@returns
RCINIT_GROUP -- Returns the Group Representation
*/
RCINIT_GROUP rcinit_lookup_group_current(void)
{
   return rcinit_internal.group_curr;
}

/**
API, name to Pointer Provisioned at Compile Time Presense
@param[in] Opaque handle representing an RCINIT_PRIO object
@returns
RCINIT_PRIO_NONE -- Netagive Query for Presense
RCINIT_PRIO -- Positive Query for Presense (MAY NEED TO BE MAPPED TO NHLOS INTERFACES)
*/
#if 0
RCINIT_PRIO rcinit_lookup_priority(RCINIT_NAME const name)
{
   int offset = rcinit_internal_priority_map_search(name, rcinit_internal_nmelen(name) + 1, 0, rcinit_internal_priority_map_size - 2);

   if (-1 != offset)
   {
      return rcinit_internal_priority_map[offset].prio;
   }

   return RCINIT_PRIO_NONE;
}
#endif


/**
API, name to Task Heap Quota Size Provisioned at Compile Time
@param[in] NULL terminated string, string identifier for task context
@returns
RCINIT_HEAPQSZ -- Returns the Stack Size Representation
*/
RCINIT_HEAPQSZ rcinit_lookup_heapqsz(RCINIT_NAME const name, RCINIT_HEAP_PTR heap_ptr)
{
   if (rcinit_lookup_available()) // TLS API available
   {

      RCINIT_INFO info = rcinit_lookup(name);

      if (info)
      {
         return rcinit_lookup_heapqsz_info(info,heap_ptr);
      }
  }

   return RCINIT_HEAPQSZ_ZERO;
}

/**
API, name to Task Heap Quota Size Provisioned at Compile Time
@param[in] Opaque handle representing an RCINIT_INFO object
@returns
RCINIT_HEAPQSZ -- Returns the Heap Quota Size Representation
*/
RCINIT_HEAPQSZ rcinit_lookup_heapqsz_info(RCINIT_INFO const info, RCINIT_HEAP_PTR heap_ptr)
{
   rcinit_heapq_p heapq_lst;

   if (info)
   {
      rcinit_info_t* rcinit_p = (rcinit_info_t*)info;
      heapq_lst = rcinit_p->heapq_list;
      while(RCINIT_NULL != heapq_lst)
      {
         if(heap_ptr == heapq_lst->heap_p)
            return heapq_lst->heap_quota;
         heapq_lst++;
      }
   }

   return RCINIT_HEAPQSZ_ZERO;
}

/**
API, self to Heap Quota Size
@returns
RCINIT_HEAPQSZ -- Returns the Heap Quota Size Representation
*/
RCINIT_HEAPQSZ rcinit_lookup_heapqsz_self(RCINIT_HEAP_PTR heap_ptr)
{
   /** API IS NOT AVAILABLE BEFORE NHLOS TASKING, NEVER ATTEMPT TO MAKE IT AVAILABLE EARLIER
   */
   rcinit_heapq_p heapq_lst;

   if (rcinit_lookup_available()) // TLS API available
   {

      if (rcinit_internal.tls_key) // TLS API available
      {
         rcinit_info_t* rcinit_p = (rcinit_info_t*)rcinit_internal_tls_get_specific(rcinit_internal.tls_key);

         if (rcinit_p)
         {
            heapq_lst = rcinit_p->heapq_list;
            while(RCINIT_NULL != heapq_lst)
            {
               if(heap_ptr == heapq_lst->heap_p)
                  return heapq_lst->heap_quota;
               heapq_lst++;
            }
         }
     }
   }
   return RCINIT_HEAPQSZ_ZERO;
}
