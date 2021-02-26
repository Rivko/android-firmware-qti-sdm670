#ifndef AEEIPRIVSET_H
#define AEEIPRIVSET_H
/*=============================================================================
        Copyright 2005 - 2007 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
=============================================================================*/
#include "AEEIQI.h"

#define AEEIID_IPrivSet 0x0102f959

#if defined(AEEINTERFACE_CPLUSPLUS)
struct IPrivSet : public IQI
{
   virtual int AEEINTERFACE_CDECL CheckPrivileges(AEEPRIVID *aprivs, int nPrivsLen) = 0;
   virtual int AEEINTERFACE_CDECL GetPrivileges(AEEPRIVID *aprivs, int nPrivsLen,
                                   int* pnPrivsLenReq) = 0;
   virtual int AEEINTERFACE_CDECL CreateSubset(const AEEPRIVID* aprivSubset,
                                  int nNumPrivs, IPrivSet** ppipiSubset) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IPrivSet(iname) \
   INHERIT_IQI(iname); \
   int   (*CheckPrivileges)(iname *pif, const AEEPRIVID *aprivs,\
                            int nPrivsLen); \
   int   (*GetPrivileges)(iname *pif, AEEPRIVID *aprivs, int nPrivsLen, \
                          int* pnPrivsLenReq); \
   int   (*CreateSubset)(iname *pif, const AEEPRIVID *aprivSubset, \
                         int nNumPrivs, IPrivSet** ppipiSubset)


/* declare the actual interface */
AEEINTERFACE_DEFINE(IPrivSet);

static __inline uint32 IPrivSet_AddRef(IPrivSet* pif)
{
   return AEEGETPVTBL(pif,IPrivSet)->AddRef(pif);
}

static __inline uint32 IPrivSet_Release(IPrivSet* pif)
{
   return AEEGETPVTBL(pif,IPrivSet)->Release(pif);
}

static __inline int IPrivSet_QueryInterface(IPrivSet* pif, 
                                             AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(pif,IPrivSet)->QueryInterface(pif, 
                                                      cls, ppo);
}

static __inline int IPrivSet_CheckPrivilege(IPrivSet* pif, AEEPRIVID priv)
{
   return AEEGETPVTBL(pif,IPrivSet)->CheckPrivileges(pif, &priv, 1);
}

static __inline int IPrivSet_CheckPrivileges(IPrivSet* pif, 
                                             const AEEPRIVID *aprivs, 
                                             int nPrivsLen)
{
   return AEEGETPVTBL(pif,IPrivSet)->CheckPrivileges(pif, aprivs, nPrivsLen);
}

static __inline int IPrivSet_GetPrivileges(IPrivSet* pif, 
                                           AEEPRIVID* aprivs, 
                                           int nPrivsLen,
                                           int* pnPrivsLenReq)
{
   return AEEGETPVTBL(pif,IPrivSet)->GetPrivileges(pif, aprivs, nPrivsLen,
                                                  pnPrivsLenReq);
}

static __inline int IPrivSet_CreateSubset(IPrivSet* pif, 
                                          const AEEPRIVID* aprivSubset, 
                                           int nNumPrivs, 
                                           IPrivSet** ppipiSubset)
{
   return AEEGETPVTBL(pif,IPrivSet)->CreateSubset(pif, aprivSubset, 
                                                 nNumPrivs, 
                                                 ppipiSubset);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IPrivSet Interface

  IPrivSet is an interface the describes a set of privileges, each identified
  by a unique ID.
  
=============================================================================

IPrivSet_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 


   

See Also:
    IPrivSet_Release()

=============================================================================

IPrivSet_Release()

Description:
    This function is inherited from IQI_Release(). 


   

See Also:
    IPrivSet_AddRef()

=============================================================================

IPrivSet_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 


   

See Also:
   IQI_QueryInterface()

=============================================================================

IPrivSet_CheckPrivilege()

Description:
   Tests whether a privilege is included in the privilege set.

Prototype:

   int IPRIVSET_CheckPrivilege(IPrivSet* pif, AEEPRIVID priv);

Parameters:
   pif: Pointer to an IPrivSet interface
   priv: the privilege to check

Return Value:
  AEE_SUCCESS if privilege is in the set,
  AEE_EPRIVLEVEL if privilege not present in this set,
  other more specific error if operation fails

Comments:
   None

Side Effects:
   None

See Also:
   IProcess_Start()

=============================================================================

IPrivSet_CheckPrivileges()

Description:
   Checks a list of privileges against the set contained in this instance of 
      IPrivSet.

Prototype:

   int IPRIVSET_CheckPrivileges(IPrivSet* pif, const AEEPRIVID* aprivs,
                                int nPrivsLen);

Parameters:
   pif: Pointer to an IPrivSet interface
   aprivs: the list of privileges to check
   nPrivsLen: the number of privileges in aprivs

Return Value:
  AEE_SUCCESS if one of the privileges in aprivs is held by this set
  AEE_EPRIVLEVEL if none of the privileges in aprivs is held
  other more specific error if operation fails

Comments:
   None

Side Effects:
   None

See Also:
   IProcess_Start()

=======================================================================

IPrivSet_CreateSubset()

Description:
   Creates a new object that consists of a subset of the privileges in
   this set.

Prototype:

   int IPrivSet_CreateSubset(IPrivSet* me, const AEEPRIVID* aprivSubset, 
                             int nNumPrivs, IPrivSet** ppipiSubset)

Parameters:
   pif: Pointer to an IPrivSet interface
   aprivSubset: array of privileges to use to create the new IPrivSet
   nNumPrivs: number of privileges in aprivSubset
   ppipiSubset: pointer to be filled with new IPrivSet instance

Return Value:
  AEE_SUCCESS if privilege granted
  AEE_EPRIVLEVEL if a privilege given not present in this instance
  other more specific error if operation fails

Comments:
   Each privilege in aprivSubset must be contained by the given IPrivSet,
    i.e. IPrivSet_CheckPrivilege() must succeed for each privilege given.

Side Effects:
   None

See Also:
   None

=======================================================================

IPrivSet_GetPrivileges()

Description:
   Gets an array of IDs for the privileges in this set.

Prototype:

   int IPrivSet_GetPrivileges(IPrivSet* pif, AEEPRIVID* apriv, int nPrivsLen, 
                              int* pnPrivsLenReq)

Parameters:
   pif: Pointer to an IPrivSet interface
   apriv: [out] array of IDs to be filled
   nPrivsLen: [in] on input, is the size of apriv, in privileges
   pnPrivsLenReq: [out] contains the number of privileges held by this set

Return Value:
   AEE_SUCCESS if all goes well
   other more specific error if operation fails

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================= */

#endif /* #ifndef AEEIPRIVSET_H */

