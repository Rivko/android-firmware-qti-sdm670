#ifndef AEEINTERFACE_H
#define AEEINTERFACE_H /* #ifndef AEEINTERFACE_H */
/*======================================================

FILE:      AEEInterface.h

SERVICES:  Macros and typedefs necessary to declare a
           BREW-style interface

GENERAL DESCRIPTION:

  BREW's COM-style binary interface specification is
  a little cumbersome to craft in C, these macros
  and types make BREW headers a little cleaner.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    AEEINTERFACE, et al.

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright 2004-2005 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#if !defined(__cplusplus)
# undef AEEINTERFACE_CPLUSPLUS
#endif /* if !defined(__cplusplus) */

#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 200000 || defined(__APCS_ADSABI))) || \
   (!defined(__ARMCC_VERSION) && defined(__GNUC__) && defined(__arm__) && !defined(__ARM_EABI__))

# define AEEINTERFACE_ATPCS /* means compiling for ATPCS */

# define AEEINTERFACE_ATPCS_PADARG              unsigned __padarg,
# define AEEINTERFACE_ATPCS_PADARGS(n)          unsigned __padarg##n,
# define AEEINTERFACE_ATPCS_PADVOID             (void)__padarg
# define AEEINTERFACE_ATPCS_PADPARAM            0,
# define AEEINTERFACE_ATPCS_PADMEMBERS(n, s)    char __padmember##n[s];

#else 

# define AEEINTERFACE_ATPCS_PADARG
# define AEEINTERFACE_ATPCS_PADARGS(n)
# define AEEINTERFACE_ATPCS_PADPARAM
# define AEEINTERFACE_ATPCS_PADVOID    (void)0
# define AEEINTERFACE_ATPCS_PADMEMBERS(n, s)

#endif

# define AEEINTERFACE_PADMEMBERS(n, s)    char __padmember##n[s];

/* floating point values to be passed in integer registers */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 200000)
#define AEEINTERFACE_SOFTFP __softfp
#elif !defined(__ARMCC_VERSION) && defined(__GNUC__) && defined(__arm__)
#define AEEINTERFACE_SOFTFP __attribute__((pcs("aapcs"))) 
#else
#define AEEINTERFACE_SOFTFP
#endif

/* 
   Our interfaces use Microsoft's __cdecl calling convention.  This token
   provides C++ interface bindings a way to enforce it.
*/
#ifdef _MSC_VER
#define AEEINTERFACE_CDECL __cdecl
#else
#define AEEINTERFACE_CDECL
#endif /* _MSC_VER */

/* the type of an interface's method vtable */
#define AEEVTBL(iname) iname##Vtbl

/* shortcut for declaring an abstract interface */
#define AEEINTERFACE(iname) \
           typedef struct AEEVTBL(iname) AEEVTBL(iname); \
           struct AEEVTBL(iname)

/* macro for retrieving the vtable from an instance of an interface */
#define AEEGETPVTBL(p,iname)  (*((AEEVTBL(iname) **)((void *)p)))

/* Define an AEE interface vtable (Assumes INHERIT_iname exists for iname) */
#define AEEVTBL_DEFINE(iname)\
            AEEINTERFACE(iname) {\
               INHERIT_##iname(iname);\
            }

/* Define an AEE interface (Assumes INHERIT_iname exists for iname) */
#define AEEINTERFACE_DEFINE(iname)\
            typedef struct iname iname;\
            AEEVTBL_DEFINE(iname)


/*
=======================================================================
MACROS DOCUMENTATION
=======================================================================
AEEVTBL()

Description:
    helps declare the standard type of a BREW interface's method
      vtable

Definition:

   #define AEEVTBL(iname) iname##Vtbl

Parameters:
    iname: the interface name for which the vtable type name
            should be generatedl

Evaluation Value:
    the token iname##Vtbl

=======================================================================
AEEINTERFACE()

Description:
    The standard way to declare the type of a new interface. Defines
      an opaque type, and the method vtable's struct header.
      Designed to be used like so:
===pre>
      AEEINTERFACE(IFoo) {
           uint32 (*AddRef)(IFoo*);
           uint32 (*Release)(IFoo*);
           int    (*QueryInterface)(IFoo *, AEECLSID, void **);
      };
===/pre>

Definition:
   #define AEEINTERFACE(iname) \
              typedef struct AEEVTBL(iname) AEEVTBL(iname); \
              struct AEEVTBL(iname)

Parameters:
    iname: the interface name for which the method vtable
           type header should be should be generated

Comments:
   This macro is rarely used by itself, it's more commonly used
     only as part of AEEVTBL_DEFINE().

=======================================================================
AEEVTBL_DEFINE()

Description:
    Uses an INHERIT_iname() macro and AEEINTERFACE() to define a
      method vtable type for an interface.
   Designed to be used like so:
===pre>
      #define INHERIT_IFoo(iname) \
         uint32 (*AddRef)(iname*);\
         uint32 (*Release)(iname*);\
         int    (*QueryInterface)(iname *, AEECLSID, void **)

      AEEVTBL_DEFINE(IFoo);
===/pre>

Definition:
    #define AEEVTBL_DEFINE(iname)\
                AEEINTERFACE(iname) {\
                   INHERIT_##iname(iname);\
                }

Parameters:
    iname: the interface name for which the method vtable
         type should be should be defined

Comments:
  This macro is rarely used by itself.  It is more commonly used as
     part of AEEINTERFACE_DEFINE().
  It can be useful when a compiler (like gcc) generates an error
    on duplicate typedefs, and the object definition needs to be
    separate from the vtable definition.
  This macro assumes the existence of INHERIT_##iname() #define

=======================================================================
AEEINTERFACE_DEFINE()

Description:
   Uses an INHERIT_iname() macro and AEEINTERFACE() to define a
     standard BREW interface.

   Designed to be used like so:
===pre>
      #define INHERIT_IFoo(iname) \
         uint32 (*AddRef)(iname*);\
         uint32 (*Release)(iname*);\
         int    (*QueryInterface)(iname *, AEECLSID, void **)

      AEEINTERFACE_DEFINE(IFoo);
===/pre>

Definition:

   #define AEEINTERFACE_DEFINE(iname)\
               typedef struct iname iname;\
               AEEVTBL_DEFINE(iname)

Parameters:
    iname: the interface name for which the struct and method vtable
          types should be defined.

Comments:
  This macro assumes the existence of INHERIT_##iname() #define

=======================================================================

AEEINTERFACE_ATPCS
  Qualcomm Component Model Interfaces methods must be binary 
  compatible across both AAPCS and ATPCS ABI standards.

  ATPCS and AAPCS differ in 2 primary areas:

    1) default alignment of 64-bit quantities in structures
       * AAPCS - 64-bit quantities are aligned on 8-byte boundaries
       * ATPCS - 64-bit quantities are aligned on 4-byte boundaries

    2) registers used when a 64-bit quantity is passed as an argument
       * AAPCS - 64-bit quantity at arg[1] is placed in r2,r3
       * ATPCS - 64-bit quantity at arg[1] is placed in r1,r2

  See here for more information on AAPCS:
    http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042b/IHI0042B_aapcs.pdf

  See here for more information on ATPCS:

  This #define is provided to indicate when a recognized ARM 
  compiler is being used and ATPCS (legacy ABI) is in effect.

Description:

Definition: (when defined)
   # define AEEINTERFACE_ATPCS

Comments:

=======================================================================

AEEINTERFACE_ATPCS_PADARG

Description:
   This token should be placed in function declarations and definitions 
   before any ATPCS/AAPCS-incompatible 64-bit argument in an odd 
   position to force its placement in even registers.  

   For example, given a function:

    void func(int a, int64 b); 

      The above is not ATPCS/AAPCS compatible because b, at arg[1] will be
      placed in r1,r2 under ATPCS and r2,r3 under AAPCS

    void func(int a, AEEINTERFACE_ATPCS_PADARG int64 b);

       The above is now ATPCS/AAPCS _compatible_ because b, at arg[1] will be
       placed in r2,r3 under both ATPCS and AAPCS

   Ideally, functions with this kind of signature should be avoided.

   AEEINTERFACE_ATPCS_PADARG is meant to be used in conjunction with
    AEEINTERFACE_ATPCS_PADPARAM and AEEINTERFACE_ATPCS_PADVOID.

Definition:
   #if defined(AEEINTERFACE_ATPCS)
   #   define AEEINTERFACE_ATPCS_PADARG unsigned __padarg,
   #else 
   #   define AEEINTERFACE_ATPCS_PADARG
   #endif

See also: 
   AEEINTERFACE_ATPCS
   AEEINTERFACE_ATPCS_PADPARAM

   AEEINTERFACE_ATPCS_PADVOID

=======================================================================

AEEINTERFACE_ATPCS_PADARGS

Description:
   Just like AEEINTERFACE_ATPCS_PADARG, but can be used in functions
   that have multiple ATPCS/AAPCS-incompatible input arguments.

    void func(int a, AEEINTERFACE_ATPCS_PADARGS(b) int64 b,
              int c, AEEINTERFACE_ATPCS_PADARGS(d) int64 d);

See also: 
   AEEINTERFACE_ATPCS_PADARG

=======================================================================
AEEINTERFACE_SOFTFP

Description:
   This macro is used to set the calling convenction attribute for 
   functions that move floating points by value.  For example:

   float function() AEEINTERFACE_SOFTFP;
   int function(float a) AEEINTERFACE_SOFTFP;
   
   #define INHERIT_IDouble(iname) \
      INHERIT_IQI(iname);\
      double (*ReturnDouble)     (iname*  pif)             AEEINTERFACE_SOFTFP;
      int    (*GetDoubleByRef)   (iname*  pif, double* d);
      int    (*PutDoubleByValue) (iname*  pif, double d)   AEEINTERFACE_SOFTFP

   This attribute is not necessary when the floating point is moved by
   reference:
   
   int function(double* a); //SOFTFP attribute is not necessary

   This attribute is only necessary in functions that are passed across
   API boundaries.  So it MUST be present in inherit macro definitions
   and c++ class virtual tables.  It's not required in static inlines 
   which are not passed across API boundaries, and is benign if present.

=======================================================================

AEEINTERFACE_ATPCS_PADMEMBERS

Description:
   This token should be placed in structure declarations before any 
   ATPCS/AAPCS-incompatible 64-bit structure members which are not 
   alignmed at a 64 bit structure boundary.
   For example:

    struct foo {
       uint8 a;
       int64 b;
       uint16 c;
       int64 d;
       uint32 e;
       int64 f;
       int64 g;
    }

      The above is not ATPCS/AAPCS compatible because b,d and f will
      be aligned at a 32 bit boundary under ATPCS and 64 bit boundary
      under AAPCS.

    struct foo {
       uint8 a;
       AEEINTERFACE_ATPCS_PADMEMBERS(b,7) int64 b;
       uint16 c;
       AEEINTERFACE_ATPCS_PADMEMBERS(d,6) int64 d;
       uint32 e;
       AEEINTERFACE_ATPCS_PADMEMBERS(f,4) int64 f;
       int64 g;
    }

      The above is ATPCS/AAPCS compatible because b,d and f will
      be aligned at 64 bit boundaries for both ATPCS and AAPCS.

   Ideally, structures with these kinds of gaps should be avoided.

=======================================================================
AEEINTERFACE_PADMEMBERS

Description:
   This token is similar in usage to AEEINTERFACE_ATPCS_PADMEMBERS
   but its used to force compilers to generate structures that have
   the same size and alignment characteristics across all targets.


See also:
   AEEINTERFACE_ATPCS_PADMEMBERS

=======================================================================

AEEINTERFACE_ATPCS_PADPARAM

Description:
   This token is supplied for placing in the C and C++ glue bindings as
   padding for a call through a vtable function whose signature includes
   AEEINTERFACE_ATPCS_PADARG.

   For example, an interface vtable function might have the type:

    AEEResult (*Func)(IObj* pif, AEEINTERFACE_ATPCS_PADARG int64 ll);

   To avoid having callers pass an ifdef'd dummy parameter, we can supply
   C/C++ interface accessors that hide the dummy, like:

   static __inline AEEResult IObj_Func(IObj* pif, int64 ll)
   {
       return AEEGETPVTBL(IObj, pif)->Func(pif, 
                                           AEEINTERFACE_ATPCS_PADPARAM ll);
   }
   
   Similarly, for C++:

   struct IObj : public IQI
   {
      virtual AEEResult CDECL Func(AEEINTERFACE_ATPCS_PADARG int64 ll) = 0;
      
#if defined(AEEINTERFACE_ATPCS)
      AEEResult Func (int64 ll)
      {
         return Func(AEEINTERFACE_ATPCS_PADPARAM ll);
      }
#endif
   }
  
   The ifdef in the above C++ glue is necessary for the case where
    AEEINTERFACE_ATPCS_PADARG evaluates to nothing.

Definition:
   #if defined(AEEINTERFACE_ATPCS)
   #   define AEEINTERFACE_ATPCS_PADPARM 0,
   #else 
   #   define AEEINTERFACE_ATPCS_PADPARM
   #endif

See also: 
   AEEINTERFACE_ATPCS
   AEEINTERFACE_ATPCS_PADARG

=======================================================================

AEEINTERFACE_ATPCS_PADVOID

Description:
   This token is supplied for implementers of functions whose
   signature includes AEEINTERFACE_ATPCS_PADARG, to allow them to
   suppress compiler warnings about "unused parameter __padarg"

   Example use case:

   static AEEResult Func(IObj* pif, AEEINTERFACE_ATPCS_PADARG int64 ll)
   {
      Obj* me = IObjToObj(pif);
      int  nErr;

      AEEINTERFACE_ATPCS_PADVOID;
  
      // function logic

      return nErr;
   }

Definition:
   #if defined(AEEINTERFACE_ATPCS)
   #   define AEEINTERFACE_ATPCS_PADVOID (void)__padarg,
   #else 
   #   define AEEINTERFACE_ATPCS_PADPARM 0
   #endif

See also: 
   AEEINTERFACE_ATPCS_PADARG


=======================================================================

*/

#endif /* #ifndef AEEINTERFACE_H */
