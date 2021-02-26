 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
  This module contains the implementation of stack protection routines.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/common/stack_protect.c#1 $
$DateTime: 2018/02/16 04:48:18 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "PrngCL.h"
#include "PrngML.h"
#include "err.h"

/*===========================================================================

                     DATA DECLARATIONS

===========================================================================*/
unsigned int __stack_chk_guard[] = {0xDEADDEAD};

/*===========================================================================

                     MODULE FUNCTIONS

===========================================================================*/

#ifdef NO_STACK_CHECK
void __init_stack_chk_guard(void) {}
#else
void
__init_stack_chk_guard(void)
{
   unsigned int random_num = 0xDEADD00D;

   /* Get random number */
   if (PRNGML_ERROR_NONE != PrngML_getdata((uint8*)&random_num, (uint16) sizeof(random_num)))
   {
      ERR_FATAL( "Stack protect cannot get random number", 0, 0, 0 );
   }
   *__stack_chk_guard = random_num;

  return;
}
#endif

void
__stack_chk_fail(void)
{
    ERR_FATAL( "Stack Check Failed", 0, 0, 0 );
}

