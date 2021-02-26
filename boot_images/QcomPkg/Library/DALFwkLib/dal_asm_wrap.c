/** @file dal_asm_wrap.c

  DAL assembly wrapper

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/27/11   sho     Add history

=============================================================================*/

#include "DALFramework.h"
#include "dalconfig.h"
#include <Library/BaseMemoryLib.h>

uint32 DALFW_LockedSwapW(volatile uint32 *pTarget, uint32 value)
{
   uint32 prevValue;
   prevValue = *pTarget;
   *pTarget = value;
   return prevValue;
}

uint32 DALFW_LockedGetW(volatile uint32 * pTarget)
{
   return *pTarget;
}

DALResult DALFW_LockedConditionalSetW(volatile uint32 *pTarget,  uint32 value)
{
   *pTarget = value;
   return DAL_SUCCESS;
}

uint32 DALFW_LockedExchangeW(volatile uint32 *pTarget, uint32 value)
{
   return DALFW_LockedSwapW( pTarget, value );
}

uint32 DALFW_LockedIncrementW(volatile uint32 *pTarget)
{
   uint32 newval;
   newval = ++(*pTarget);
   return newval;
}

uint32 DALFW_LockedDecrementW(volatile uint32 *pTarget)
{
   uint32 newval;
   newval = --(*pTarget);
   return newval;
}

uint32 DALFW_LockedGetModifySetW(volatile uint32 *pTarget, uint32 AND_mask, uint32 OR_value)
{
   uint32 newval;
   newval = (*pTarget & AND_mask) | OR_value;
   *pTarget = newval;
   return newval;
}

uint32 DALFW_LockedCompareExchangeW(volatile uint32 *pTarget, uint32 comparand, uint32 value)
{
   if(*pTarget == comparand)
      *pTarget = value;
   else
      value = *pTarget;   // previous value
   return value;
}

/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/
/*-- Semi-Raw Inter CPU (i.e External shared) spinlocks ----------------------------*/
/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/

/*
   Keep in mind these are not raw-spin locks and intended for very low contention
   locks as while these locks are being held the running thread may be interrupted
   which will cause the other processor to loop for an undetermined amount of time.
   For such scenarios it may be necessary that instead the "Try" implementations
   be used conjunction of OS yield/sleep primitives as when used via DALGlbCtxt.

   TEST_LOCK_COUNT controls how often the test access to the shared memory occurs.
   The value must always be a power of 2.
      Use 2 to force test each time.
   This value is especially important when shared memory is Strongly-Ordered as it
   will behave as a memory barrier.
*/
#define TEST_LOCK_COUNT    64

void DALFW_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt=0;
   for(;;)
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0) //time to test?
      {
         if(0 == *spinLock)             //is the lock currently available? 
         {
            if( DALFW_LockedExchangeW(spinLock, pid) == 0 ) // try and grab it 
               break;
         }
      }
      cnt++;
   }
}

DALResult DALFW_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   uint32 cnt;
   for(cnt=0; cnt<=TEST_LOCK_COUNT; cnt++) //always tries twice
   {
      if((cnt & (TEST_LOCK_COUNT-1))==0)   //time to test?
      {
         if(0 == *spinLock)             //is the lock currently available?  
         {
            if( DALFW_LockedExchangeW(spinLock, pid) == 0 )  // try and grab it  
               return DAL_SUCCESS;
         }
      }
   }
   return DAL_ERROR;
}

void DALFW_UnLockSpinLockExt(volatile uint32 *spinLock)
{
    *spinLock = 0;   // Use store instead of Atomic Operation
}


/* raw implementations of the above */

void _raw_LockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   while ( DALFW_LockedExchangeW(spinLock, pid) );
}

void _raw_UnLockSpinLockExt(volatile uint32 *spinLock)
{
    DALFW_LockedExchangeW(spinLock, 0);
}

DALResult _raw_TryLockSpinLockExt(volatile uint32 *spinLock, uint32 pid)
{
   return (DALFW_LockedExchangeW(spinLock, pid)==0)?DAL_SUCCESS:DAL_ERROR;
}


/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/
/*------------ Raw intra CPU (i.e NON-shared) spinlocks ----------------------------*/
/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/

void DALFW_LockSpinLock(volatile uint32 *spinLock, uint32 pid)
{
   while ( DALFW_LockedExchangeW(spinLock, pid) );
}

void DALFW_UnLockSpinLock(volatile uint32 *spinLock)
{
    DALFW_LockedExchangeW(spinLock, 0);
}

DALResult DALFW_TryLockSpinLock(volatile uint32 *spinLock, uint32 pid)
{
   return (DALFW_LockedExchangeW(spinLock, pid)==0)?DAL_SUCCESS:DAL_ERROR;
}

