#ifndef TRACER_MALLOC_H
#define TRACER_MALLOC_H
/**==========================================================================
  @file tracer_malloc.h

   Tracer malloc

               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_malloc.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <DALSys.h>     // malloc, free

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/**
  @brief Translator to target specific malloc function.
 */
__inline static void *_tracer_malloc(size_t size)
{
   void *pTemp;
   if (DAL_SUCCESS == DALSYS_Malloc(size, &pTemp))
   {
      return pTemp;
   }
   else
   {
      return NULL;
   }
}

/**
  @brief Translator to target specific free function.
 */
__inline static void _tracer_free(void *ptr)
{
   DALSYS_Free(ptr);
   return;
}

#endif /* #ifndef TRACER_MALLOC_H */


