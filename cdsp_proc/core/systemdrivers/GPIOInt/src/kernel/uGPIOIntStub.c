/*==============================================================================

FILE:      uGPIOInt.c

DESCRIPTION
  This modules implements the API to utilize the Micro GPIO interrupt controller.
  This Micro GPIO Interrupt Controller module is strictly used in USER Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uGPIOInt.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2017 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/uGPIOIntStub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "uGPIOIntInternal.h"



/*------------------------------------------------------------------------------
 GPIOInt Data Declarations.
------------------------------------------------------------------------------*/ 

/*------------------------------------------------------------------------------
 GPIOInt Macro Declarations.
------------------------------------------------------------------------------*/ 

/*------------------------------------------------------------------------------
Following functions are internal functions.
------------------------------------------------------------------------------*/

/*==========================================================================

   FUNCTION      uGPIOInt_Init

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32 uGPIOInt_Init(void)
{

  return UGPIOINT_SUCCESS;

} /* END uGPIOInt_Init */


