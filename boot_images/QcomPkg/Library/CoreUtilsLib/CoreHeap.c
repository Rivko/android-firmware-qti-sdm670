/*============================================================================
@file CoreHeap.c

Implements CoreHeap.h

Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
11/25/14   rli     Replace banned DALSYS_memcpy() with safe core_memcpy() 
========================================================================*/

#include "CoreHeap.h"
#include "CoreString.h"
#include "DALSys.h"

#ifndef MIN
  #define MIN( a, b ) ((a)<(b)) ? (a) : (b)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* Core_Malloc( unsigned int size )
{
    unsigned int* pTemp = NULL;
    unsigned int original = size;
    size += sizeof(unsigned int);
    if (DALSYS_Malloc( size, (void **)&pTemp ) == DAL_SUCCESS)
    {
        *pTemp = original;
        pTemp++;
    }
    else
    {
        pTemp = NULL;
    }
    return (void*)(pTemp);
}

void Core_Free( void *ptr )
{
  if ( ptr )
  {
    unsigned int* pTemp = (unsigned int*)ptr;
    pTemp--;
    DALSYS_Free( pTemp );
  }
}

void* Core_Realloc(void* in_ptr,  unsigned int in_size)
{
    void *pNewBuffer = NULL;
    unsigned int *tmp = (unsigned int*)in_ptr;

    if (!in_ptr && in_size)
    {
        return Core_Malloc(in_size);
    }

    if (in_size)
    {
        tmp--;
        unsigned int origSize = *tmp;

        pNewBuffer = Core_Malloc(in_size);
		if(pNewBuffer == NULL)
			return NULL;
        core_memcpy(pNewBuffer, in_size, in_ptr, origSize);
    }

    Core_Free(in_ptr);

    return(pNewBuffer);
}

#ifdef __cplusplus
}
#endif
