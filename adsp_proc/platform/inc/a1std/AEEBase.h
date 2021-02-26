#ifndef AEEBASE_H
#define AEEBASE_H
/*======================================================
FILE:  AEEBase.h

SERVICES:  AEEBase structure; related functions and macros

GENERAL DESCRIPTION:

   Class for supporting multiple inheritance from abstract base classes.

        Copyright 1999-2005 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary and Confidential
=====================================================*/

#include "AEEIQI.h"


// AEEBASE_INHERIT: declare the structure members required for an interface to
//    be compatible with AEEBase (vtable + object pointer).   AEEBASE_INHERIT()
//    should appear first in the structure definition.
//
#define AEEBASE_INHERIT(abc, derived)      const AEEVTBL(abc) *pvt; \
                                           derived       *pMe


// AEEBASE_INIT: initialize an inherited interface
//
//   Note that 'me' (the base interface) should derive from IQI
//   if the AEEBase member functions are to be used.
//
#define AEEBASE_INIT(me,fld,v)          ((void)((me)->fld.pvt = (v), \
                                                (me)->fld.pMe  = (me) ))


// AEEBase: Generic ABC interface.  All interfaces declared with the above
// macros should be compatible with AEEBase.

typedef struct AEEBase {
   AEEBASE_INHERIT(IQI, IQI);
} AEEBase;


// AEEBase member functions

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

uint32 AEEBase_AddRef(AEEBase *po);
uint32 AEEBase_Release(AEEBase *po);
int    AEEBase_QueryInterface(AEEBase *pMe, AEECLSID clsid, void **ppNew);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


// These macros cast the utility functions to a type appropriate for the
// particular vtable being defined.

#define AEEBASE_AddRef(abc)          ((uint32 (*)(abc *)) AEEBase_AddRef)
#define AEEBASE_Release(abc)         ((uint32 (*)(abc *)) AEEBase_Release)
#define AEEBASE_QueryInterface(abc)  ((int (*)(abc *po, AEECLSID,void**)) AEEBase_QueryInterface)



// AEEBase
//
//   AEEBase is a class that simplifies inheriting from multiple abstract base
//   classes (ABCs).  Interfaces can be defined as structures that inherit
//   from AEEBase.  These interfaces can then be included as members of an
//   object and serve as interfaces exported by that object.
//
//   Member functions of each interface can use the 'pMe' member of AEEBase to
//   access the containing object.  Since every AEEBase interface will contain
//   a 'pMe' pointing to the containing object, member functions can be
//   written without dependencies on the interface in which they reside
//   (except for typechecking issues.)
//
// AEEBase_AddRef/Release/QueryInterface
//
//   In statically-linked code, AEEBase-derived interfaces can use the AEEBase
//   versions of AddRef, Release, and QueryInterface.  These functions simply
//   defer to the corresponding methods on the base object, and rely on the
//   object's primary vtable being a superset of IQI.
//
//
// Usage Example:
//
//
//   // define ITransform as an AEEBase-derived interface
//   struct ITransform {
//      AEEBASE_INHERIT(ITransform, MyClass);
//   };
//
//   struct MyClass {
//      const AEEVTBL(IBitmap) pvt;          // this class is primarily an IBitmap
//      ...
//      ITransform   t;                      // this class also implements ITransform
//      ...
//   };
//
//
//   static const VTBL(ITransform) gMyClassTransformFuncs = {
//      AEEBASE_AddRef(ITransform),            // provided by AEEBase
//      AEEBASE_Release(ITransform),           // provided by AEEBase
//      AEEBASE_QueryInterface(ITransform),    // provided by AEEBase
//      MyClass_TransformBltSimple,
//      MyClass_TransformBltComplex
//   };
//
//   void MyClass_Constructor(MyClass *pMe)
//   {
//       me->pvt = &gMyClassFuncs;    // normal vtable initialization
//       ...
//       // we also have to initialize the AEEBase interface
//       AEEBASE_INIT(pMe, t, &gMyClassTransformFuncs);
//       ...
//   }
// 
//
//   int MyClass_QueryInterface(MyClass *pMe, AEECLSID id, void **ppif)
//   {
//      if (id == AEECLSID_LOADSTREAM) {
//         *ppif = &pMe->t;
//         MyClass_AddRef(pMe);
//         return AEE_SUCCESS;
//      }
//      ...
//   }
//
//   // Method of the new ITransform interface...
//   //
//   int MyClass_TransformBltSimple(ITransform *po, IAStream *pStream)
//   {
//       //  "po" points to the ITransform interface
//       //  "po->pMe" points to MyClass
//       ...
//   }
//
//
// Notes:
//
// - AEEBase could be used for the primary interface (i.e. the vtable at
//   offset 0).  This could be useful if there is a function that needs to be
//   shared between the primary interface and another interface.  One
//   implementation of that function could be user (it would use the po->pMe
//   pointer to reference the object).  This would avoid the need for two
//   functions (a stub function + the 'real' version).
//
//
// - This is designed to work with interfaces that use the newer AEEINTERFACE
//   conventions:
//
//     AEEVTBL(iname) = type name for vtable for interface 'iname'
//     AEEINTERFACE(iname) { member functions } : Used to declare a vtable
//


#endif // AEEBASE_H
