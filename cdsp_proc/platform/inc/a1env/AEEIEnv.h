#ifndef AEEIENV_H
#define AEEIENV_H
/*=============================================================================
        Copyright 2006-2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
          QUALCOMM Confidential and Proprietary
=============================================================================*/
#include "AEEIRealloc.h"

#define AEEIID_IEnv 0x0102346d

#if !defined(_HEAP_DEBUG)
#if defined(_DEBUG)
#define _HEAP_DEBUG 1
#else /* _DEBUG */
#define _HEAP_DEBUG 0
#endif /* _DEBUG */
#endif /* _HEAP_DEBUG */

#if defined(AEEINTERFACE_CPLUSPLUS)
struct IEnv : public IRealloc
{
   virtual int AEEINTERFACE_CDECL CreateInstance(AEECLSID cls, void** ppif) = 0;
   virtual int AEEINTERFACE_CDECL AtExit(void (*pfnAtExit)(void* pCtx), void* pCtx) = 0;
};

#define IENV_FREEIF(pif, p) \
   do { if (p) { (void)(pif)->ErrRealloc(0,(void**)&(p)); } } while (0)

#define IENV_ERRMALLOCRECEX(pif, t, nSize, pp) \
   (*(pp)=(t*)0,(pif)->ErrMalloc((int)sizeof(t)+(nSize), (void**)(pp)))

#define IENV_ERRMALLOCREC(pif, t, pp) \
   (*(pp)=(t*)0,(pif)->ErrMalloc((int)sizeof(t), (void**)(pp)))

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IEnv(iname) \
   INHERIT_IRealloc(iname); \
   int   (*CreateInstance)(iname* pif, AEECLSID cls, void** ppif); \
   int   (*AtExit)(iname* pif, void (*pfnAtExit)(void* pCtx), void* pCtx)

AEEINTERFACE_DEFINE(IEnv);

static __inline uint32 IEnv_AddRef(IEnv* pif)
{
   return AEEGETPVTBL(pif,IEnv)->AddRef(pif);
}

static __inline uint32 IEnv_Release(IEnv* pif)
{
   return AEEGETPVTBL(pif,IEnv)->Release(pif);
}

static __inline int IEnv_QueryInterface(IEnv* pif, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(pif,IEnv)->QueryInterface(pif, cls, ppo);
}

static __inline int IEnv_CreateInstance(IEnv* pif, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(pif,IEnv)->CreateInstance(pif, cls, ppo);
}

static __inline int IEnv_AtExit(IEnv* pif, 
                                void (*pfnAtExit)(void* pCtx), void* pCtx)
{
   return AEEGETPVTBL(pif,IEnv)->AtExit(pif, pfnAtExit, pCtx);
}

static __inline int IEnv_ErrReallocName(IEnv* pif,  
                                            int nSize, void** pp, 
                                            const char* cpszName)
{
   return AEEGETPVTBL(pif,IEnv)->ErrReallocName(pif, nSize, 
                                                   pp, cpszName);
}

static __inline int IEnv_ErrMallocName(IEnv* pif, int nSize, 
                                           void** pp, 
                                           const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IEnv)->ErrReallocName(pif, nSize, 
                                                   pp, cpszName);
}

static __inline int IEnv_ErrReallocNameNoZI(IEnv* pif,
                                                int nSize, void** pp, 
                                                const char* cpszName)
{
   return AEEGETPVTBL(pif,IEnv)->ErrReallocNameNoZI(pif, nSize, 
                                                       pp, cpszName);
}

static __inline int IEnv_ErrMallocNameNoZI(IEnv* pif, int nSize, 
                                               void** pp, 
                                               const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IEnv)->ErrReallocNameNoZI(pif, nSize, 
                                                       pp, cpszName);
}

static __inline int IEnv_Free(IEnv* pif, void* p)
{
   return AEEGETPVTBL(pif,IEnv)->ErrRealloc(pif, 0, &p);
}

#if _HEAP_DEBUG == 0

static __inline int IEnv_ErrRealloc(IEnv* pif, int nSize, void** pp)
{
   return AEEGETPVTBL(pif,IEnv)->ErrRealloc(pif, nSize, pp);
}

static __inline int IEnv_ErrMalloc(IEnv* pif, int nSize, void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IEnv)->ErrRealloc(pif, nSize, pp);
}

static __inline int IEnv_ErrReallocNoZI(IEnv* pif, 
                                            int nSize, void** pp)
{
   return AEEGETPVTBL(pif,IEnv)->ErrReallocNoZI(pif, nSize, pp);
}

static __inline int IEnv_ErrMallocNoZI(IEnv* pif, int nSize, 
                                           void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(pif,IEnv)->ErrReallocNoZI(pif, nSize, pp);
}

#else /* #if _HEAP_DEBUG == 0 */

#define IEnv_ErrRealloc(pif, nSize, pp)  \
    IEnv_ErrReallocName((pif), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrMalloc(pif, nSize, pp)  \
    IEnv_ErrMallocName((pif), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrReallocNoZI(pif, nSize, pp)  \
    IEnv_ErrReallocNameNoZI((pif), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrMallocNoZI(pif, nSize, pp)  \
    IEnv_ErrMallocNameNoZI((pif), (nSize), (pp), __FILE_LINE__)

#endif /* #if _HEAP_DEBUG == 0 */

#define IENV_FREEIF(pif, p) \
   do { if (p) { (void)IEnv_ErrRealloc((pif), 0,(void**)&(p)); } } while (0)

#define IENV_ERRMALLOCRECEX(pif, t, nSize, pp) \
    (*(pp)=(t*)0,IEnv_ErrMalloc((pif), (int)sizeof(t)+(nSize), (void**)(pp)))

#define IENV_ERRMALLOCREC(pif, t, pp) \
    (*(pp)=(t*)0,IEnv_ErrMalloc((pif), (int)sizeof(t), (void**)(pp)))

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
=====================================================================
MACROS DOCUMENTATION
=====================================================================

IENV_ERRMALLOCREC()

Description:
    This macro is inherited from IREALLOC_ERRMALLOCREC()

=====================================================================

IENV_ERRMALLOCRECEX()

Description:
    This macro is inherited from IREALLOC_ERRMALLOCRECEX()

=====================================================================

IENV_FREEIF()

Description:
    This macro is inherited from IREALLOC_FREEIF()

=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IEnv Interface

Description:

  This interface provides the basic set of services a component typically
  requires to be instantiated and initialized: memory allocation and
  creation of other objects.

  There are two types of objects supporting IEnv that are commonly
  encountered in the system: "Env" objects, and "ModEnv" objects.

  An Env, or environment, is the component infrastructure surrounding the
  object, and establishes a context within which objects exist.  There is
  one Env created for each each process or applet when it is started.  At
  any one time, a thread is considered to be "inside" a particular Env.

  Objects that reside within the same Env are "local" to each other: they
  can invoke each other without passing through remote invocation gateways,
  they may directly share objects and pointers, and they may trade off
  responsibility for freeing allocated memory.  Objects that reside in
  different Envs will typically communicate only through remote invocation
  (via remotable APIs), although this is strictly enforced only when the Env
  is associated with a process.  Each Env has a set of privileges that
  dictates which remote services is can obtained.

  A ModEnv represents an instance of a module (an executable code and data
  image that implements a set of classes) within an Env.  When a module is
  loaded within an Env, a ModEnv is created.  The ModEnv is mainly used to
  track references to the module, but it can also be used to store
  module-local data.  For convenience it re-exposes the functionality of the
  surrounding Env (memory allocation and object creation).

  When an object is created by the component infrastructure, an IEnv
  reference is passed to its CreateInstance() function.  This IEnv is an
  interface to the ModEnv object, not to the Env.  Each new object is
  expected to AddRef this IEnv and hold that reference until it is deleted
  (when its reference count goes to zero).  After the last object
  implemented in the module releases its IEnv, the module code and data may
  be removed from memory.

  The env_CreateInstance() function, on the other hand, utilizes the current
  Env.  Since the ModEnv re-exposes the functionality of its surrounding
  Env, object creation through env_CreateInstance() or IEnv_CreateInstance()
  should behave equivalently (although using IEnv will avoid context
  recovery and may be faster as a result).

See Also:
   AEECLSID_Env

=======================================================================

IEnv_AddRef()

Description:
    This function is inherited from IQI_AddRef. 

See Also:
    IEnv
	IEnv_Release

=======================================================================

IEnv_Release()

Description:
    This function is inherited from IQI_Release. 

See Also:
    IEnv
	IEnv_AddRef

=======================================================================

IEnv_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface. 

See Also:
   IEnv

=======================================================================
IEnv_CreateInstance()

Description:

   This method creates an instance of a specified class.

   Creation of a class may entail loading a module into memory and
   initializing the module, or it may re-use and already loaded module.

   Failure can result from failure to locate an implementation of the class,
   failure to load a module, failure to allocate memory for the module or
   for the instance, or failure to construct the instance.

   Creation may also fail due to a lack of privileges held by the
   environment.  Refer to the documentation of each class for privilege
   requirements.

Prototype:

   int IEnv_CreateInstance(IEnv* pif, AEECLSID cls, void** ppif);

Parameters:
   pif :      Pointer to the IEnv interface.
   cls :      The AEECLSID of the class to instantiate.
   ppif [out]: Pointer to be filled with a new instance of the class requested.

Return value:

   AEE_SUCCESS: An instance was created.  *ppif holds a pointer to the default interface for that class.
   An error code, perhaps of diagnostic use : otherwise.

Comments:  
   None

See Also:
    Error Codes
	IEnv
	Privileges

=============================================================================

IEnv_AtExit()

Description: 

   This method allows a caller to register or de-register a function to be
   called during Env (environment) cleanup.  For environments associated
   with applets or processes, cleanup occurs when the applet or process is
   shut down.

   This facility allows objects to "AddRef themselves", if necessary,
   without incurring leaks when the containing Env is cleaned up.

   The IEnv_AtExit callbacks are called in reverse order of subscription.

Prototype:
   int IEnv_AtExit(IEnv *pif, void (*pfnAtExit)(void* pCtx), void* pCtx);

Parameters:
   pif :       Pointer to the IEnv interface.
   pfnAtExit: Function to call on system exit. A value of NULL cancels all functions previously registered with the same pCtx value.
   pCtx:      Function context to be passed to pfnAtExit.

Return value:
   AEE_SUCCESS: callback has been successfully registered.
   An error message : otherwise.

Comments:  
   None.

See Also:
   Error Codes
   IEnv

=======================================================================

IEnv_ErrRealloc()

Description:
  This function is inherited from IRealloc_ErrRealloc.

See Also:
   IEnv

=======================================================================

IEnv_Free()

Description:
  This function is inherited from IRealloc_Free.

See Also:
   IEnv

=======================================================================

IEnv_ErrMalloc()

Description:
  This function is inherited from IRealloc_ErrMalloc.

See Also:
   IEnv

=======================================================================

IEnv_ErrReallocNoZI()

Description:
  This function is inherited from IRealloc_ErrReallocNoZI.

See Also:
   IEnv

=======================================================================

IEnv_ErrMallocNoZI()

Description:
  This function is inherited from IRealloc_ErrMallocNoZI.

See Also:
   IEnv

=======================================================================

IEnv_ErrReallocName()

Description:
  This function is inherited from IRealloc_ErrReallocName.

See Also:
   IEnv

=======================================================================

IEnv_ErrMallocName()

Description:
  This function is inherited from IRealloc_ErrMallocName.

See Also:
   IEnv

=======================================================================

IEnv_ErrReallocNameNoZI()

Description:
  This function is inherited from IRealloc_ErrReallocNameNoZI.

See Also:
   IEnv

=======================================================================

IEnv_ErrMallocNameNoZI()

Description:
  This function is inherited from IRealloc_ErrMallocNameNoZI.

See Also:
   IEnv

=======================================================================
*/

#endif /* #ifndef AEEIENV_H */

