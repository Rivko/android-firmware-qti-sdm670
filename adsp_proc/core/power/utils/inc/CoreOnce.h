/*==============================================================================
@file CoreOnce.h

One time run function library. This utility enables initialization in 
a multi-threaded, multi-core environment.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreOnce.h#1 $ 
==============================================================================*/
#ifndef CORE_ONCE_H
#define CORE_ONCE_H

typedef void ( *core_once_fcn )( void );

typedef struct
{
  volatile core_once_fcn fcn;
} CoreOnceType;

/**
   @brief CoreOnce - Guarantee the init function is run only
          once.
   
   This function will call the provided init function at most
   once, and will guarantee that any other callers will be held
   off until the init function has finished running.
   
   As implemented, it will serialize all subsystems that use
   this function.
   
   @param once : The init information to execute.
   
   @return: Success = 0
*/
void CoreOnce (CoreOnceType * once);

#endif // CORE_ONCE_H
