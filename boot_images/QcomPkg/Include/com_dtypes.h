#ifndef COM_DTYPES_H
#define COM_DTYPES_H
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

DESCRIPTION
  This header file contains general data types that are of use to all modules.
  The values or definitions are dependent on the specified
  target.  T_WINNT specifies Windows NT based targets, otherwise the
  default is for ARM targets.

  T_WINNT  Software is hosted on an NT platforn, triggers macro and
           type definitions, unlike definition above which triggers
           actual OS calls

Copyright (c) 2009-2010, 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/15   bh      Remove the typedefs now covered in stdlib
09/02/14   seansw  Add guards for size_t and uintptr_t. 
06/20/14   kedara  Add defines for PACK.
05/22/14   kedara  Add defines for ALIGN, uintptr_t.
09/15/09   pc      Created file.
===========================================================================*/

#include <bits/alltypes.h>

/*===========================================================================

                            Data Declarations

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* For NT apps we want to use the Win32 definitions and/or those
** supplied by the Win32 compiler for things like NULL, MAX, MIN
** abs, labs, etc.
*/
#ifdef T_WINNT
   #ifndef WIN32
      #define WIN32
   #endif
   #include <stdlib.h>
#endif

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1   /* Boolean true value. */
#define FALSE  0   /* Boolean false value. */

#define  ON   1    /* On value. */
#define  OFF  0    /* Off value. */

#ifndef NULL
  #define NULL  0
#endif

/* -----------------------------------------------------------------------
** Standard Types
** ----------------------------------------------------------------------- */

/* The following definitions are the same accross platforms.  This first
** group are the sanctioned types.
*/
#ifndef _ARM_ASM_
#ifndef _BOOLEAN_DEFINED
typedef  BOOLEAN            boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif




#ifndef _UINT32_DEFINED
typedef  UINT32             uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  UINT16             uint16;      /* Unsigned 16 bit value */
#define _UINT16_DEFINED
#endif

#ifndef _UINT8_DEFINED
typedef  UINT8              uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  INT32              int32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  INT16              int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _INT8_DEFINED
typedef  INT8               int8;        /* Signed 8  bit value */
#define _INT8_DEFINED
#endif

/* This group are the deprecated types.  Their use should be
** discontinued and new code should use the types above
*/
typedef  CHAR16             word;         /* Unsinged 16 bit value type. */
typedef  UINT32             dword;        /* Unsigned 32 bit value type. */

typedef  UINT8              uint1;        /* Unsigned 8  bit value type. */
typedef  UINT16             uint2;        /* Unsigned 16 bit value type. */
typedef  UINT32             uint4;        /* Unsigned 32 bit value type. */

typedef  INT8               int1;         /* Signed 8  bit value type. */
typedef  INT16              int2;         /* Signed 16 bit value type. */
typedef  INT32              int4;         /* Signed 32 bit value type. */

typedef  INT32              sint31;       /* Signed 32 bit value */
typedef  INT16              sint15;       /* Signed 16 bit value */
typedef  INT8               sint7;        /* Signed 8  bit value */

typedef uint16 UWord16 ;
typedef uint32 UWord32 ;
typedef int32  Word32 ;
typedef int16  Word16 ;
typedef uint8  UWord8 ;
typedef int8   Word8 ;
typedef int32  Vect32 ;

#if (! defined T_WINNT) && (! defined __GNUC__)
  /* Non WinNT Targets */
  #ifndef _INT64_DEFINED
    typedef INT64      int64;       /* Signed 64 bit value */
    #define _INT64_DEFINED
  #endif
  #ifndef _UINT64_DEFINED
    typedef  UINT64    uint64;      /* Unsigned 64 bit value */
    #define _UINT64_DEFINED
  #endif
#else /* T_WINNT || TARGET_OS_SOLARIS || __GNUC__ */
  /* WINNT or SOLARIS based targets */
  #if (defined __GNUC__)
    #ifndef _INT64_DEFINED
      typedef INT64              int64;
      #define _INT64_DEFINED
    #endif
    #ifndef _UINT64_DEFINED
      typedef UINT64             uint64;
      #define _UINT64_DEFINED
    #endif
  #else
    typedef  __int64              int64;       /* Signed 64 bit value */
    #ifndef _UINT64_DEFINED
      typedef  unsigned __int64   uint64;      /* Unsigned 64 bit value */
      #define _UINT64_DEFINED
    #endif
  #endif
#endif /* T_WINNT */

#ifndef _UINTNT_DEFINED
typedef  unsigned long      uintnt;      /* Unsigned Native Integer
                                            Aarch32: 32 bits
					    Aarch64: 64 bits        */
#define _UINTNT_DEFINED
#endif


#if (! defined T_WINNT) && (! defined __GNUC__)
    /* Non WinNT Targets */

    #if defined(__ARMCC_VERSION)
      #define ALIGN(__value) __align(__value)
    #else
      #ifndef FEATURE_WINCE
        #define ALIGN(__value) __attribute__((__aligned__(__value)))
      #else /* FEATURE_WINCE */
        #define ALIGN(__value)
      #endif /* FEATURE_WINCE */
    #endif /* defined (__GNUC__) */	  
#else /* T_WINNT || TARGET_OS_SOLARIS || __GNUC__ */

    /* WINNT or SOLARIS based targets*/
    #if defined (__GNUC__) || defined (__GNUG__)
      #define ALIGN(__value) __attribute__((aligned(__value)))
    #else
      #define ALIGN(__value)
    #endif	  
#endif /* T_WINNT */

/*********************** BEGIN PACK() Definition ***************************/

#if defined __GNUC__
  #define PACK(x)       __attribute__((packed)) x
#elif defined __GNUG__
  #define PACK(x)       __attribute__((packed)) x
#elif defined __arm
  #define PACK(x)       __attribute__((packed)) x
#elif defined _WIN32
  #define PACK(x)       x /* Microsoft uses #pragma pack() prologue/epilogue */
#else
  #define PACK(x)       __attribute__((packed)) x
#endif

/********************** END PACK() Definition *****************************/

#endif /* _ARM_ASM_ */

#ifdef __cplusplus
}
#endif

#endif  /* COM_DTYPES_H */
