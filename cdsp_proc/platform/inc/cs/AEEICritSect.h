#ifndef AEEICRITSECT_H
#define AEEICRITSECT_H
/*
=======================================================================

FILE:         AEEICritSect.h

SERVICES:     ICritSect

DESCRIPTION:  ICritSect represents a critical section object that can
                be used to synchronize threads' access to data.

=======================================================================
        Copyright 2004-2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
=======================================================================
*/
#include "AEEIQI.h"

/* interface */
#define AEEIID_ICritSect   0x0101f685

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ICritSect : public IQI
{
   virtual void AEEINTERFACE_CDECL Enter() = 0;
   virtual void AEEINTERFACE_CDECL Leave() = 0;
   virtual int AEEINTERFACE_CDECL TryEnter() = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_ICritSect(iname) \
   INHERIT_IQI(iname); \
   void (*Enter)(iname *p); \
   void (*Leave)(iname *p); \
   int (*TryEnter)(iname *p)

// declare the actual Create interface
AEEINTERFACE_DEFINE(ICritSect);

static __inline uint32 ICritSect_AddRef(ICritSect *pif)
{
   return AEEGETPVTBL(pif,ICritSect)->AddRef(pif);
}

static __inline uint32 ICritSect_Release(ICritSect *pif)
{
   return AEEGETPVTBL(pif,ICritSect)->Release(pif);
}

static __inline int ICritSect_QueryInterface(ICritSect *pif, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(pif,ICritSect)->QueryInterface(pif, cls, ppo);
}

static __inline void ICritSect_Enter(ICritSect *pif)
{
   AEEGETPVTBL(pif,ICritSect)->Enter(pif);
}

static __inline void ICritSect_Leave(ICritSect *pif)
{
   AEEGETPVTBL(pif,ICritSect)->Leave(pif);
}

static __inline int ICritSect_TryEnter(ICritSect* pif)
{
   return AEEGETPVTBL(pif, ICritSect)->TryEnter(pif);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=======================================================================
INTERFACES DOCUMENTATION
=======================================================================

ICritSect Interface

Description: 

  ICritSect is an interface for using "critical section" objects (also known
  as "locks" or "mutexes").  It allows multiple threads to coordinate their
  access to shared data in order to avoid concurrency problems, by ensuring
  that only one thread at a time is executing "inside" the critical section.

  Typically, one critical section object will be used to guard each data set
  that is manipulated by multiple threads.  Any code sequences that modify
  the data or rely on it being in a known state are guarded with Enter and
  Exit calls, as shown below:
===pre>
      ICritSect_Enter(me->piCS);
      me->nCount = me->nCount + 1;
      ICritSect_Leave(me->piCS);
===/pre>

  Without this guard, two threads attempting to increment nCount at the same
  time could end up both reading the same value and then writing the same
  value.

  When a thread terminates while holding a critical section, the state of
  the critical section object is undefined.  Threads must release all of
  their critical sections before terminating.

  Ads with other interfaces, ICritSect is an interface that can be used with
  many different implementations.  Refer to the documentation on the classes
  being used, such as AEECLSID_CritSect, for a more complete description of
  the behavior of the lock objects.

See Also:
   AEECLSID_CritSect
   AEECLSID_InterProcessCritSelect

=======================================================================

ICritSect_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    ICritSect
	ICritSect_Release()

=======================================================================

ICritSect_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    ICritSect
	ICritSect_AddRef()

=======================================================================

ICritSect_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

See Also:
   ICritSect

=======================================================================

ICritSect_Enter()

Description:

  Enter a critical section (grabs the lock).

Prototype:

   void ICritSect_Enter(ICritSect* pif);

Parameters:
   pif: Pointer to an ICritSect interface.

Return Value:
   None

Comments:
   ICritSect_Enter() acquires a lock.  It will block until the lock can be acquired.

Side Effects:
   None

See Also:
   ICritSect

=======================================================================

ICritSect_Leave()

Description:

  Leave a critical section (releases the lock).

Prototype:

   void ICritSect_Leave(ICritSect* pif);

Parameters:
   pif: Pointer to an ICritSect interface

Return Value:
   None

Comments:
   None

Side Effects:
   Leaving critical section will allow another thread waiting 
   on ICritSect_Enter() to acquire the lock.

See Also:
   ICritSect

=======================================================================

ICritSect_TryEnter()

Description:

   Tries to acquire a critical section.

   While ICritSect_Enter() blocks until the lock is acquired, 
   ICritSect_TryEnter() returns immediately if the lock cannot be 
   immediately acquired.

   If ICritSect_TryEnter() returns AEE_SUCCESS, the lock has been acquired 
   and the thread must subsequently release the lock by calling 
   ICritSect_Leave().

Prototype:

   int ICritSect_TryEnter(ICritSect* pif);

Parameters:
   pif: Pointer to an ICritSect interface.

Return Value:
   AEE_SUCCESS : Critical section is acquired.
   AEE_EFAILED : Critical section is not acquired.

Side Effects:
   none.

See Also:
  Error Codes
  ICritSect

=======================================================================*/

#endif /* #ifndef AEEICRITSECT_H */

