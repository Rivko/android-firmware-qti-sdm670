/*============================================================================
@file CoreHeap.c

Implements CoreHeap.h

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreHeap.c#1 $
============================================================================*/
#include "CoreHeap.h"
#include "DALSys.h"

#ifdef __cplusplus
extern "C" {
#endif

void* Core_Malloc( unsigned int size )
{
  void *ptr = NULL;
  return ( DALSYS_Malloc( size, &ptr ) == DAL_SUCCESS ? ptr : NULL );
}

void Core_Free( void *ptr )
{
  if ( ptr )
  {
    DALSYS_Free( ptr );
  }
}

#ifdef __cplusplus
}
#endif
