/*==============================================================================

FILE:      uGPIOInt.c

DESCRIPTION
  This modules implements the API to utilize the Micro GPIO interrupt controller.
  This Micro GPIO Interrupt Controller module is strictly used in USER Protection
  Domain and in Micro image.The definitions in this module are implemented internally
  and should not be exposed for external client usage directly without using the
  accompanying uGPIOInt.h in the core/api/systemdrivers interface file for this module.

REFERENCES

       Copyright (c) 2014 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/user/uimage/uGPIOIntUserStub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

// #include "uTimetick.h" so far now not sure about the logging plan.

#include <atomic_ops.h>
#include <qurt.h>
#include "qurt_int.h"
#include "qurt_anysignal.h"
#include "stringl.h"
#include "uGPIOIntUser.h"



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

  return UGPIOINT_ERROR;

} /* END uGPIOInt_Init */

/*==========================================================================

   FUNCTION      uGPIOInt_RegisterInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_RegisterInterrupt( uint32 gpio,uGPIOIntTriggerType trigger,
                            uGPIOINTISR isr,uGPIOINTISRCtx param, uint32 nFlags)
{
  
  return UGPIOINT_ERROR;

} /* END uGPIOInt_RegisterInterrupt */


/*==========================================================================

   FUNCTION      uGPIOInt_DeregisterInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_DeregisterInterrupt(uint32  gpio)
{
  return UGPIOINT_ERROR;

} /* END uGPIOInt_DeregisterInterrupt */


/*==========================================================================

   FUNCTION      uGPIOInt_IsInterruptPending

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_IsInterruptPending( uint32  gpio, uint32 *  state)
{

  return UGPIOINT_ERROR;

} /* END uGPIOInt_IsInterruptPending */


/*==========================================================================

   FUNCTION      uGPIOInt_TriggerInterrupt

   DESCRIPTION   See uGPIOInt.h

==========================================================================*/
int32
uGPIOInt_TriggerInterrupt(uint32 gpio)
{
  return UGPIOINT_ERROR;

} /* END uGPIOInt_TriggerInterrupt */


int32 uGPIOInt_MapMPMInterrupt
(
  
  uint32 gpio,
  uint32 mpm_id
)
{
  return UGPIOINT_ERROR;
}

/*=============================================================================
  FUNCTION      uGPIOInt_GetInterruptID
=============================================================================*/
int32 
uGPIOInt_GetInterruptID
(
  
  uint32 gpio,
  uint32* pnInterruptID
)
{
  return UGPIOINT_ERROR;
}


/*=============================================================================
  FUNCTION      uGPIOInt_EnableInterrupt
=============================================================================*/
int32 
uGPIOInt_EnableInterrupt
(
  
  uint32 gpio
)
{
  return UGPIOINT_ERROR;
}


/*=============================================================================
  FUNCTION      uGPIOInt_DisableInterrupt
=============================================================================*/
int32 
uGPIOInt_DisableInterrupt
(
  
  uint32 gpio
)
{
  return UGPIOINT_ERROR;
}

/*=============================================================================
  FUNCTION      uGPIOInt_TimestampRead
=============================================================================*/
int32 
uGPIOInt_TimestampRead
(
  uint32                    gpio,
  uGPIOIntTimestampStatus*  pnStatus,
  uint64*                   pnTimestamp
)
  return UGPIOINT_ERROR;
{

}