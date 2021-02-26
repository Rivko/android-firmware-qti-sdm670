/*==============================================================================
@file CoreCond.c

Implements the create and destroy functions for a condition variable.  All other
methods for the condition variable are implemented in os/<os>/CoreCondImpl.c

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreCond.c#1 $
==============================================================================*/
#include "CoreCond.h"
#include "CoreVerify.h"

CoreCondType* Core_CondCreate( CoreCondAttrType attr )
{
  CoreCondType *cond;

  CORE_VERIFY( (DALSYS_Malloc( sizeof(CoreCondType), (void**)&cond )
                   == DAL_SUCCESS)
               && (NULL != cond)); /* Klocworks */
  memset( cond, 0, sizeof(CoreCondType) );
  cond->attr = attr;

  if ( Core_CondInit( cond, attr ) != 0 )
  {
    DALSYS_Free( cond );
    cond = NULL;
  }
  return cond;
}

void Core_CondDestroy( CoreCondType *cond )
{
  /* Be tolerant of NULL input */
  if ( cond )
  {
    CORE_VERIFY_PTR( cond->vtable->destroy );
    cond->vtable->destroy( cond->cv );
    DALSYS_Free( cond );
  }
}
