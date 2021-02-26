#ifndef AEEIREALLOC_H
#define AEEIREALLOC_H
/*=============================================================================
        Copyright 2004-2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
=============================================================================*/
#include "AEEIQI.h"

#define AEEIID_IRealloc 0x0102345e

#if !defined(_HEAP_DEBUG)
#if defined(_DEBUG)
#define _HEAP_DEBUG 1
#else /* _DEBUG */
#define _HEAP_DEBUG 0
#endif /* _DEBUG */
#endif /* _HEAP_DEBUG */

#if defined(AEEINTERFACE_CPLUSPLUS)
struct IRealloc : public IQI
{
   virtual int AEEINTERFACE_CDECL ErrRealloc(int nSize, void** ppBlock) = 0;
   virtual int AEEINTERFACE_CDECL ErrReallocName(int nSize, void** ppBlock, const char* cpszName) = 0;
   virtual int AEEINTERFACE_CDECL ErrReallocNoZI(int nSize, void** ppBlock) = 0;
   virtual int AEEINTERFACE_CDECL ErrReallocNameNoZI(int nSize, void** ppBlock, const char* cpszName) = 0;
   int ErrMallocName(int nSize, void** pp, const char* cpszName)
   {
      *pp = 0;
      return ErrReallocName (nSize, pp, cpszName);
   }
   int ErrMallocNameNoZI(int nSize, void** pp, const char* cpszName)
   {
      *pp = 0;
      return ErrReallocNameNoZI (nSize, pp, cpszName);
   }
   int Free(void* p)
   {
      return ErrRealloc (0, &p);
   }
   int ErrMalloc(int nSize, void** pp)
   {
      *pp = 0;
      return ErrRealloc (nSize, pp);
   }
   int ErrMallocNoZI(int nSize, void** pp)
   {
      *pp = 0;
      return ErrReallocNoZI (nSize, pp);
   }
};

#define IREALLOC_FREEIF(pif, p) \
   do { if (p) { (void)(pif)->ErrRealloc(0,(void**)&(p)); } } while (0)

#define IREALLOC_ERRMALLOCRECEX(pif, t, nSize, pp) \
   (*(pp)=(t*)0,(pif)->ErrMalloc((int)sizeof(t)+(nSize), (void**)(pp)))

#define IREALLOC_ERRMALLOCREC(pif, t, pp) \
   (*(pp)=(t*)0,(pif)->ErrMalloc((int)sizeof(t), (void**)(pp)))

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IRealloc(iname) \
   INHERIT_IQI(iname); \
   int (*ErrRealloc)(iname* pif, int nSize, void** ppOut); \
   int (*ErrReallocName)(iname* pif, int nSize, void** ppOut, \
                         const char* cpszName);\
   int (*ErrReallocNoZI)(iname* pif, int nSize, void** ppOut); \
   int (*ErrReallocNameNoZI)(iname* pif, int nSize, void** ppOut, \
                             const char* cpszName)

AEEINTERFACE_DEFINE(IRealloc);

static __inline uint32 IRealloc_AddRef(IRealloc* pif)
{
   return AEEGETPVTBL(pif,IRealloc)->AddRef(pif);
}

static __inline uint32 IRealloc_Release(IRealloc* pif)
{
   return AEEGETPVTBL(pif,IRealloc)->Release(pif);
}

static __inline int IRealloc_QueryInterface(IRealloc* pif, AEECLSID cls, 
                                            void** ppo)
{
   return AEEGETPVTBL(pif,IRealloc)->QueryInterface(pif, cls, ppo);
}

static __inline int IRealloc_ErrReallocName(IRealloc* pif,  
                                            int nSize, void** pp, 
                                            const char* cpszName)
{
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocName(pif, nSize, 
                                                   pp, cpszName);
}

static __inline int IRealloc_ErrMallocName(IRealloc* pif, int nSize, 
                                           void** pp, 
                                           const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocName(pif, nSize, 
                                                   pp, cpszName);
}

static __inline int IRealloc_ErrReallocNameNoZI(IRealloc* pif,
                                                int nSize, void** pp, 
                                                const char* cpszName)
{
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocNameNoZI(pif, nSize, 
                                                       pp, cpszName);
}

static __inline int IRealloc_ErrMallocNameNoZI(IRealloc* pif, int nSize, 
                                               void** pp, 
                                               const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocNameNoZI(pif, nSize, 
                                                       pp, cpszName);
}

static __inline int IRealloc_Free(IRealloc* pif, void* p)
{
   return AEEGETPVTBL(pif,IRealloc)->ErrRealloc(pif, 0, &p);
}

#if _HEAP_DEBUG == 0

static __inline int IRealloc_ErrRealloc(IRealloc* pif, int nSize, void** pp)
{
   return AEEGETPVTBL(pif,IRealloc)->ErrRealloc(pif, nSize, pp);
}

static __inline int IRealloc_ErrMalloc(IRealloc* pif, int nSize, void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IRealloc)->ErrRealloc(pif, nSize, pp);
}

static __inline int IRealloc_ErrReallocNoZI(IRealloc* pif, 
                                            int nSize, void** pp)
{
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocNoZI(pif, nSize, pp);
}

static __inline int IRealloc_ErrMallocNoZI(IRealloc* pif, int nSize, 
                                           void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IRealloc)->ErrReallocNoZI(pif, nSize, pp);
}

#else /* #if _HEAP_DEBUG == 0 */

#define IRealloc_ErrRealloc(me, nSize, pp)  \
    IRealloc_ErrReallocName((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrMalloc(me, nSize, pp)  \
    IRealloc_ErrMallocName((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrReallocNoZI(me, nSize, pp)  \
    IRealloc_ErrReallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrMallocNoZI(me, nSize, pp)  \
    IRealloc_ErrMallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#endif /* #if _HEAP_DEBUG == 0 */

#define IREALLOC_FREEIF(me, p) \
    do { if (p) { (void)IRealloc_ErrRealloc((me), 0,(void**)&(p)); } } while (0)

#define IREALLOC_ERRMALLOCRECEX(me, t, nSize, pp) \
    (*(pp)=(t*)0,IRealloc_ErrMalloc((me), (int)sizeof(t)+(nSize), (void**)(pp)))

#define IREALLOC_ERRMALLOCREC(me, t, pp) \
    (*(pp)=(t*)0,IRealloc_ErrMalloc((me), (int)sizeof(t), (void**)(pp)))

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */


/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IRealloc Interface

Description:
   IRealloc is an interface to a pool of memory available for dynamic
   allocation (a "heap").

Interface ID:
   AEEIID_IRealloc

Usage:
   IRealloc implements four methods; all have the same basic functionality,
   but differ in whether they associate a "debug name" with the requested
   memory block and in whether they guarantee newly allocated memory to be
   zero-filled.

   The basic functionality is similar to the standard C library realloc()
   function.  A new allocation is done by reallocating a NULL pointer to the
   desired size.  Freeing an allocated block is done by reallocating the
   block to size zero (0).  Convenience methods are provided for these simple
   operations which are roughly equivalent to malloc() and free().

   Note that reallocating a NULL pointer to size zero (0) is explicitly
   allowed and causes no action to be taken.

   For those methods which associate a "debug name" with the requested
   memory block, the name is a zero-terminated character string.  The name
   will be copied by the implementation, so the caller may free or re-use
   the memory immediately after the IRealloc method finishes.  Heap
   implementations may silently drop, truncate, or otherwise modify debug
   names, but for those heap implementations which are capable of providing
   debug output, including the accurate debug name in the output provides
   valuable context for the developer.

See also:
   IEnv interface (derived from IRealloc)
============================================================================ */

/*=============================================================================
METHOD DOCUMENTATION
===============================================================================
IRealloc_AddRef()

Description:
   Increments the reference count of the object.

Prototype:
   uint32 IRealloc_AddRef(IRealloc *piRealloc);

Parameters:
   piRealloc: pointer to the IRealloc interface

Return Value:
   The incremented reference count of the object.

Comments:
   Inherited from IQI interface.

Side Effects:
   None.

See Also:
   IQI_AddRef()

===============================================================================
IRealloc_Release()

Description:
   Decrements the reference count of the object.

Prototype:
   uint32 IRealloc_Release(IRealloc *piRealloc);

Parameters:
   piRealloc: pointer to the IRealloc interface

Return Value:
   The decremented reference count of the object.

Comments:
   Inherited from IQI interface.

Side Effects:
   If the reference count is decreased to zero, the object is deleted
   and unavailable for further operations; the interface pointer should
   be discarded as it is no longer valid.

See Also:
   IQI_Release()

===============================================================================
IRealloc_QueryInterface()

Description:
   Makes a request to the underlying object for a specific interface.

Prototype:
   int IRealloc_QueryInterface(IRealloc *piRealloc,
                               AEEIID idReq,
                               void **ppo);

Parameters:
   piRealloc: pointer to the IRealloc interface
   idReq: an interface ID describing the requested interface
   ppo: (out) Returned interface pointer.  Filled in on success.

Return Value:
   AEE_SUCCESS - Interface found
   AEE_ECLASSNOTSUPPORT - Requested interface is unsupported
   Another error from AEEStdErr.h, if appropriate

Comments:
   Inherited from IQI interface.

Side Effects:
   On success, increments the object's reference count.

See Also:
   IQI_QueryInterface()

===============================================================================
IRealloc_ErrRealloc()

Description:
   Makes an allocation request to the heap object with zero-initialization.

Prototype:
   int IRealloc_ErrRealloc(IRealloc *piRealloc,
                           int nSize,
                           void **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested new size of the block, or zero (0) to request that
          the block be freed.
   ppOut: (in/out) on entry, *ppOut should have the existing pointer to
          the block; on exit, *ppOut will have the new pointer to the
          block, or NULL if a block was freed.
          On failure, *ppOut is unchanged.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   AEE_EMEMPTR - An invalid pointer was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   Any newly allocated space is guaranteed to be zero-filled.
   When zero-filled allocation is not required, the non-zero-filling
    alternative IRealloc_ErrReallocNoZI() may be more efficient.
   If _HEAP_DEBUG is defined to a non-zero value, IRealloc_ErrRealloc()
    is silently converted to IRealloc_ErrReallocName() with a debug name
    containing the source file name and line number where the method is used.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrReallocNoZI()
   _HEAP_DEBUG
   IRealloc_ErrReallocName()

===============================================================================
IRealloc_ErrReallocNoZI()

Description:
   Makes an allocation request to the heap object without zero-initialization.

Prototype:
   int IRealloc_ErrReallocNoZI(IRealloc *piRealloc,
                               int nSize,
                               void **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested new size of the block, or zero (0) to request that
          the block be freed.
   ppOut: (in/out) on entry, *ppOut should have the existing pointer to
          the block; on exit, *ppOut will have the new pointer to the
          block, or NULL if a block was freed.
          On failure, *ppOut is unchanged.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   AEE_EMEMPTR - An invalid pointer was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   Any newly allocated space is not guaranteed to be zero-filled.
   When zero-filled allocation is not required, this method may be
    more efficient than IRealloc_ErrRealloc().
   If _HEAP_DEBUG is defined to a non-zero value, IRealloc_ErrReallocNoZI()
    is silently converted to IRealloc_ErrReallocNameNoZI() with a debug name
    containing the source file name and line number where the method is used.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrRealloc()
   _HEAP_DEBUG
   IRealloc_ErrReallocNameNoZI()

===============================================================================
IRealloc_ErrReallocName()

Description:
   Makes an allocation request to the heap object with zero-initialization,
    and provides a debug name for the allocated block.

Prototype:
   int IRealloc_ErrReallocName(IRealloc *piRealloc,
                               int nSize,
                               void **ppOut,
                               const char *cpszName);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested new size of the block, or zero (0) to request that
          the block be freed.
   ppOut: (in/out) on entry, *ppOut should have the existing pointer to
          the block; on exit, *ppOut will have the new pointer to the
          block, or NULL if a block was freed.
          On failure, *ppOut is unchanged.
   cpszName: pointer to a zero-terminated character string containing
             the debug name for this block, or NULL for no name.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   AEE_EMEMPTR - An invalid pointer was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   Any newly allocated space is guaranteed to be zero-filled.
   When zero-filled allocation is not required, the non-zero-filling
    alternative IRealloc_ErrReallocNameNoZI() may be more efficient.
   The string pointed to by cpszName may be re-used or otherwise
    discarded immediately upon return from this method; the caller
    need not preserve the string.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrReallocNameNoZI()

===============================================================================
IRealloc_ErrReallocNameNoZI()

Description:
   Makes an allocation request to the heap object without zero-initialization,
    and provides a debug name for the allocated block.

Prototype:
   int IRealloc_ErrReallocNameNoZI(IRealloc *piRealloc,
                                   int nSize,
                                   void **ppOut,
                                   const char *cpszName);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested new size of the block, or zero (0) to request that
          the block be freed.
   ppOut: (in/out) on entry, *ppOut should have the existing pointer to
          the block; on exit, *ppOut will have the new pointer to the
          block, or NULL if a block was freed.
          On failure, *ppOut is unchanged.
   cpszName: pointer to a zero-terminated character string containing
             the debug name for this block, or NULL for no name.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   AEE_EMEMPTR - An invalid pointer was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   Any newly allocated space is not guaranteed to be zero-filled.
   When zero-filled allocation is not required, this method may be
    more efficient than IRealloc_ErrRealloc().
   The string pointed to by cpszName may be re-used or otherwise
    discarded immediately upon return from this method; the caller
    need not preserve the string.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrReallocName()

============================================================================ */

/*=============================================================================
HELPER DOCUMENTATION
===============================================================================
_HEAP_DEBUG

Description:
   Configurable macro to turn on automatic debug names on heap operations

Value:
   _HEAP_DEBUG may be explicitly configured to be either 0 or 1, or it
    may be implicitly set based on the presence of the _DEBUG macro.

Comments:
   If _HEAP_DEBUG isn't explicitly set, then it will be set to either 0 if
    the macro _DEBUG is not defined, or it will be set to 1 if the macro
    _DEBUG is defined.

Side Effects:
   If _HEAP_DEBUG is set to a non-zero value, then all of the IRealloc methods
    and helpers which don't explicitly provide a debug name will construct a
    default debug name based on the source file name and line number where the
    method is referenced.
   If _HEAP_DEBUG is set to a non-zero value, it is still possible to make
    allocation requests which explicitly avoid providing a debug name, by
    using one of the methods or helpers which does explicitly provide a
    debug name, and by passing in NULL as the debug name.

See Also:
   IRealloc_ErrRealloc()
   IRealloc_ErrReallocNoZI()
   IRealloc_ErrMalloc()
   IRealloc_ErrMallocNoZI()

===============================================================================
IRealloc_ErrMalloc()

Description:
   Helper function for doing allocations.

Prototype:
   int IRealloc_ErrMalloc(IRealloc *piRealloc,
                          int nSize,
                          void **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested size of the block
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper sets *ppOut to NULL to indicate that a new allocation is
    requested, and then chains to IRealloc_ErrRealloc().
   Any newly allocated space is guaranteed to be zero-filled.
   When zero-filled allocation is not required, the non-zero-filling
    alternative IRealloc_ErrMallocNoZI() may be more efficient.
   If _HEAP_DEBUG is defined to a non-zero value, IRealloc_ErrMalloc()
    is silently converted to IRealloc_ErrMallocName() with a debug name
    containing the source file name and line number where the method is used.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrRealloc()
   IRealloc_ErrMallocNoZI()
   _HEAP_DEBUG
   IRealloc_ErrMallocName()

===============================================================================
IRealloc_ErrMallocNoZI()

Description:
   Helper function for doing allocations.

Prototype:
   int IRealloc_ErrMallocNoZI(IRealloc *piRealloc,
                              int nSize,
                              void **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested size of the block
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper sets *ppOut to NULL to indicate that a new allocation is
    requested, and then chains to IRealloc_ErrReallocNoZI().
   Any newly allocated space is not guaranteed to be zero-filled.
   When zero-filled allocation is not required, this method may be
    more efficient than IRealloc_ErrMalloc().
   If _HEAP_DEBUG is defined to a non-zero value, IRealloc_ErrMallocNoZI()
    is silently converted to IRealloc_ErrMallocNameNoZI() with a debug name
    containing the source file name and line number where the method is used.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrRealloc()
   IRealloc_ErrMalloc()
   _HEAP_DEBUG
   IRealloc_ErrMallocNameNoZI()

===============================================================================
IRealloc_ErrMallocName()

Description:
   Helper function for doing allocations.

Prototype:
   int IRealloc_ErrMallocName(IRealloc *piRealloc,
                              int nSize,
                              void **ppOut,
                              const char *cpszName);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested size of the block
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.
   cpszName: pointer to a zero-terminated character string containing
             the debug name for this block, or NULL for no name.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper sets *ppOut to NULL to indicate that a new allocation is
    requested, and then chains to IRealloc_ErrReallocName().
   Any newly allocated space is guaranteed to be zero-filled.
   When zero-filled allocation is not required, the non-zero-filling
    alternative IRealloc_ErrMallocNameNoZI() may be more efficient.
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrReallocName()
   IRealloc_ErrMallocNameNoZI()

===============================================================================
IRealloc_ErrMallocNameNoZI()

Description:
   Helper function for doing allocations.

Prototype:
   int IRealloc_ErrMallocNameNoZI(IRealloc *piRealloc,
                                  int nSize,
                                  void **ppOut,
                                  const char *cpszName);

Parameters:
   piRealloc: pointer to the IRealloc interface
   nSize: the requested size of the block
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.
   cpszName: pointer to a zero-terminated character string containing
             the debug name for this block, or NULL for no name.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper sets *ppOut to NULL to indicate that a new allocation is
    requested, and then chains to IRealloc_ErrReallocNameNoZI().
   Any newly allocated space is not guaranteed to be zero-filled.
   When zero-filled allocation is not required, this method may be
    more efficient than IRealloc_ErrMallocName().
   Negative values for nSize should be avoided.

Side Effects:
   None

See Also:
   IRealloc_ErrReallocNameNoZI()
   IRealloc_ErrMallocName()

===============================================================================
IRealloc_Free()

Description:
   Helper function for freeing allocated blocks.

Prototype:
   int IRealloc_Free(IRealloc *piRealloc, void *pBlock);

Parameters:
   piRealloc: pointer to the IRealloc interface
   pBlock: pointer to a block previously allocated from this interface

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_EHEAP - An internal heap error was detected
   AEE_EMEMPTR - An invalid pointer was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper function uses the IRealloc_ErrRealloc() method with a size
    value of zero (0) to free the provided block.
   Freeing a NULL pointer is explicitly allowed and does nothing.

Side Effects:
   None

See Also:
   IRealloc_ErrRealloc()

===============================================================================
IREALLOC_FREEIF()

Description:
   Helper macro for freeing allocated blocks.

Prototype:
   void IREALLOC_FREEIF(IRealloc *piRealloc, void *pBlock);

Parameters:
   piRealloc: pointer to the IRealloc interface
   pBlock: pointer to a block previously allocated from this interface

Return Value:
   None

Comments:
   This helper macro behaves much like IRealloc_Free(), except that
    it also explicitly sets pBlock to NULL upon successful completion.
   The second parameter to this macro must be a pointer expression suitable
    for the left side of an assignment ("=") operator.

Side Effects:
   Sets pBlock to NULL upon successful completion.

See Also:
   IRealloc_Free()

===============================================================================
IREALLOC_ERRMALLOCREC()

Description:
   Helper macro for allocating blocks capable of holding a particular
    data type.

Prototype:
   int IREALLOC_ERRMALLOCREC(IRealloc *piRealloc,
                             <type>,
                             <type> **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   <type>: the type name of a valid C type
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper macro may be used to request a zero-initialized block of
    memory capable of holding a particular data type.

   Example:

     struct foo *pFoo;
     nErr = IREALLOC_ERRMALLOCREC(piRealloc, struct foo, &pFoo);
     if (AEE_SUCCESS == nErr) {
       ... Fill in the empty structure pointed to by pFoo ...
     }

Side Effects:
   None

See Also:
   IRealloc_ErrMalloc()

===============================================================================
IREALLOC_ERRMALLOCRECEX()

Description:
   Helper macro for allocating blocks capable of holding a particular
    data type, plus a specified amount of extra space.

Prototype:
   int IREALLOC_ERRMALLOCRECEX(IRealloc *piRealloc,
                               <type>,
                               int nExtra,
                               <type> **ppOut);

Parameters:
   piRealloc: pointer to the IRealloc interface
   <type>: the type name of a valid C type
   nExtra: the number of extra bytes to allocate
   ppOut: (out) on exit, *ppOut will have the new pointer to the
          block, or NULL if no block was allocated.

Return Value:
   AEE_SUCCESS - Allocation (or free) was successful
   AEE_ENOMEMORY - Not enough memory to fulfill the request
   AEE_EHEAP - An internal heap error was detected
   Another error from AEEStdErr.h, if appropriate

Comments:
   This helper macro may be used to request a zero-initialized block of
    memory capable of holding a particular data type, plus some extra
    space.

   Example:

     struct foo *pFoo;
     nErr = IREALLOC_ERRMALLOCRECEX(piRealloc, struct foo, 16, &pFoo);
     if (AEE_SUCCESS == nErr) {
       ... Fill in the empty structure pointed to by pFoo ...
       ... Fill in up to 16 bytes immediately after the structure ...
     }

   There is no range checking for nExtra; if a negative value is passed
    for nExtra, or if nExtra is so large as to cause an arithmetic
    overflow, this macro will likely fail to work correctly.

Side Effects:
   None

See Also:
   IRealloc_ErrMalloc()

============================================================================ */

#endif /* #ifndef AEEIREALLOC_H */
