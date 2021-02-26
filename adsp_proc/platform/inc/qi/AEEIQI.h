#ifndef AEEIQI_H
#define AEEIQI_H /* #ifndef AEEIQI_H */
/*======================================================
        Copyright 2004-2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEStdDef.h"
#include "AEEInterface.h"

#define AEEIID_IQI 0x01000001

#if defined(AEEINTERFACE_CPLUSPLUS)
struct IQI
{
   virtual uint32 AEEINTERFACE_CDECL AddRef() = 0;
   virtual uint32 AEEINTERFACE_CDECL Release() = 0;
   virtual int AEEINTERFACE_CDECL QueryInterface(AEEIID idReq, void **ppo) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IQI(iname) \
   uint32 (*AddRef)(iname*);\
   uint32 (*Release)(iname*);\
   int    (*QueryInterface)(iname *, AEEIID, void **)

AEEINTERFACE_DEFINE(IQI);

static __inline uint32 IQI_AddRef(IQI *pif)
{
   return AEEGETPVTBL(pif,IQI)->AddRef(pif);
}

static __inline uint32 IQI_Release(IQI *pif)
{
   return AEEGETPVTBL(pif,IQI)->Release(pif);
}

static __inline int IQI_QueryInterface(IQI *pif, AEEIID iid, void** ppo)
{
   return AEEGETPVTBL(pif,IQI)->QueryInterface(pif, iid, ppo);
}

#define IQI_ADDREFIF(p) do { if (p) (void)IQI_AddRef((IQI*)(void*)(p)); } while (0)

#define IQI_RELEASEIF(p) do { if (p) { IQI* __releaseif__piqi = (IQI*)(p); (p) = 0; (void)IQI_Release(__releaseif__piqi); } } while (0)

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
=======================================================================
 INTERFACES DOCUMENTATION
=======================================================================

IQI Interface

Description:

   This interface is the base interface for component objects.  It provides
   for reference counting and runtime discovery of functionality.  Other
   component interfaces are expected to derive from this interface.

=======================================================================

IQI_AddRef()

Description:

   This method increments the reference count on an object.  This
   allows the object to be shared by multiple callers.  The object 
   can be trusted to remain valid until a matching call to Release()
   is made.  AddRef() should be called whenever a reference to an object 
   is copied (e.g., when written to the output parameter to 
   QueryInterface())

Prototype:

   uint32 IQI_AddRef(IQI * pif);

Parameters:
   pif: Pointer to the interface

Return Value:

   Returns the incremented reference count for the object.  A valid
   object returns a positive reference count.

Comments:
   None

Side Effects:
   None

See Also:
   IQI_Release()
=======================================================================
IQI_Release()

Description:

   This function decrements the reference count of an object.  This function
   should be called whenever a reference to an object is destroyed
   (e.g. when a non-null interface pointer is set to null).

Prototype:

   uint32 IQI_Release(IQI * pif);

Parameters:

   pif: Pointer to the interface

Return Value:

   Returns the decremented reference count for the object.

Comments:
   None

Side Effects:

    The object may be deleted during this call.  In any case, the 
    caller should assume that the object _has_ been freed.

See Also:
   IQI_AddRef()

=======================================================================
IQI_QueryInterface()

Description:

   This method asks an object for another API contract from the object in
   question.

Prototype:

   int IQI_QueryInterface(IQI *pif, AEEIID idReq, void **ppo)

Parameters:

   pif: Pointer to the interface
   idReq:  Requested interface ID exposed by the object
   ppo: (out) Returned object.  Filled by this method

Return Value:
   AEE_SUCCESS - Interface found
   AEE_ENOMEMORY - Insufficient memory
   AEE_ECLASSNOTSUPPORT - Requested interface is unsupported
   Another AEEError.h, if appropriate

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
IQI_ADDREFIF()

Description:
   This is a helper macro for those conditionally adding a reference to
   interfaces derived from IQI.

Definition:
   IQI_ADDREFIF(p) do { if (p) (void)IQI_AddRef((IQI*)(p)); } while (0)

Parameters:
  p: the object to addref, it *must* implement IQI_AddRef(), as this macro 
      does an unsafe cast

Evaluation Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   IQI_AddRef()

=======================================================================
IQI_RELEASEIF()

Description:
   This is a helper macro for those conditionally releasing interfaces
     derived from IQI.

Definition:
   IQI_RELEASEIF(p) do { if (p) { IQI_Release((IQI*)(p)); p = 0; } } while (0)

Parameters:
  p: the object to release, it *must* implement IQI_Release(), as this macro 
      does an unsafe cast

Evaluation Value:
   None

Comments:
   None

Side Effects:
   p is set to 0

See Also:
   IQI_Release()

=======================================================================

*/
#endif /* #ifndef AEEIQI_H */
