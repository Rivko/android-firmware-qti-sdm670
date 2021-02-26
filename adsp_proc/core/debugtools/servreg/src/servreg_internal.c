/*
#============================================================================
#  Name:
#    servreg_internal.c 
#
#  Description:
#     Common header file for Service Registry feature
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

#include "stringl/stringl.h"
#include "servreg_internal.h"

#ifdef SERVREG_TRACE_ENABLE
/* Declare the log structure */
static volatile srt_type servreg_trace;

void srt_event(srt_event_type event, void** data)
{
   srt_data_type *trace = (srt_data_type *)&servreg_trace.events[servreg_trace.index];
   trace->event = event;

   //currently data is binary (0 or 2) if we need more variety add a param count to the macro
   if (data != NULL)
   {
      trace->data0 = data[0];
      trace->data1 = data[1];
   }
   else
   {
      trace->data0 = 0;
      trace->data1 = 0;
   }

  /* Go to the next index */
  servreg_trace.index++;
  if (servreg_trace.index >= SRT_LENGTH)
  {
    servreg_trace.index = 0;
  }

}

/*
void *srt_q6_return_address()
{
  int return_addr;
  __asm__ ("{r1=memw(fp+#4); r2=framekey}\n\t"
           "%0=xor(r1, r2)\n"
  :"=r"(return_addr): : "r1", "r2"
  );
  return (void *)return_addr;
}
*/

#endif //SERVREG_TRACE_ENABLE

/** =====================================================================
 * Function:
 *     servreg_mutex_init_dal
 *
 * Description:
 *     Initialize a DAL mutex
 *
 * Parameters:
 *     mutex_p : mutex to be initialized
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_mutex_init_dal(servreg_mutex_p const mutex_p)
{
   if (SERVREG_NULL != mutex_p)
   {
      secure_memset((void*)mutex_p, 0, sizeof(servreg_mutex_t));
      if (DAL_SUCCESS == DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &(mutex_p->mutex), &(mutex_p->mutexObject)))
      {
      }
   }
}

/** =====================================================================
 * Function:
 *     servreg_mutex_lock_dal
 *
 * Description:
 *     Lock the mutex
 *
 * Parameters:
 *    mutex_p : mutex to be locked
 *
 * Returns:
 *    None
 * =====================================================================  */
void servreg_mutex_lock_dal(servreg_mutex_p const mutex_p)
{
   if (SERVREG_NULL != mutex_p)
   {
      DALSYS_SyncEnter(mutex_p->mutex);
   }
}

/** =====================================================================
 * Function:
 *     servreg_mutex_unlock_dal
 *
 * Description:
 *     Unlock the mutex
 *
 * Parameters:
 *     mutex_p : mutex to be unlocked
 *
 * Returns:
 *     None
 * =====================================================================  */
void servreg_mutex_unlock_dal(servreg_mutex_p const mutex_p)
{
   if (SERVREG_NULL != mutex_p)
   {
      DALSYS_SyncLeave(mutex_p->mutex);
   }
}

/** =====================================================================
 * Function:
 *     servreg_nmelen
 *
 * Description:
 *     Function to find out the string length
 *
 * Parameters:
 *     name : string
 *
 * Returns:
 *     int : string length
 * =====================================================================  */
int servreg_nmelen(SERVREG_NAME const name)
{
   SERVREG_NAME s;
   for (s = name; '\0' != *s; ++s)
   /* NULL */;
   return s - name;
}

/** =====================================================================
 * Function:
 *     servreg_nmecmp
 *
 * Description:
 *     Compares two strings
 *
 * Parameters:
 *     name_1 : string 1
 *     name_2 : string 2
 *     len    : length of the string
 *
 * Returns:
 *     -1, 0 or 1
 * =====================================================================  */
int servreg_nmecmp(SERVREG_NAME const name_1, SERVREG_NAME const name_2, int len)
{
   SERVREG_NAME s1 = name_1, s2 = name_2;

   if (0 == len)
   {
      return 0;
   }

   while (len-- > 0 && *s1 == *s2)
   {
      if (0 == len || '\0' == *s1)
      {
         return 0;
      }
      s1++, s2++;
   }
   /* lexical analysis and return result (-1, 0, 1) */
   return(*s1 < *s2) ? -1 : (*s1 > *s2);
}

/** =====================================================================
 * Function:
 *     servreg_nmehash
 *
 * Description:
 *     Function calculated the hash value given a name
 *
 * Parameters:
 *     name : name given to calculate the hash value
 *
 * Returns:
 *     hash : hash value calculated for the given name
 * =====================================================================  */
servreg_hash_t servreg_nmehash(SERVREG_NAME const name, uint32_t size)
{
   SERVREG_NAME s = name;
   servreg_hash_t hash = 0;

   while ('\0' != *s)
   {
      servreg_hash_t temp;

      hash = (hash << 4) + (servreg_hash_t)(*s);
      if (0 != (temp = hash & 0xf0000000))
      {
         hash ^= (temp >> 24);
      }
      hash &= (~temp);
      s++;
   }
   return hash % size;
}


