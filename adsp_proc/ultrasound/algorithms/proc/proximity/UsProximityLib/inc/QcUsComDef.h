/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2006 by Qualcomm Technologies, Inc.  All rights reserved. All data and   *]
[* information contained in or disclosed by this document is confidental and *]
[* proprietary information of Qualcomm Technologies, Inc and all rights therein are       *]
[* expressly reserved. By accepting this material the recipient agrees that  *]
[* this  material and the information contained therein is held in confidence*]
[* and in trust and will not be used, copied, reproduced in whole or in part,*]
[* nor its contents revealed in  any manner to others without the express    *]
[* written permission of Qualcomm Technologies, Inc.                                      *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   comdef.h                    TYPE: C-header file              *]
[* DESCRIPTION: Contains the typedefs of various data types that should be   *]
[*   used instead of the actual data types so as to have common sizes across *]
[*   platforms.                                                              *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   06/06/05   sdk     Initial revision                                     *]
[*   11/5/10    ren		modified for gesture demo definition                 *]
[*****************************************************************************/
#ifndef _QC_COMDEF_H_
#define _QC_COMDEF_H_

#if defined __hexagon__ && !defined US_STANDALONE_ALG_TESTS
// when compiling LPASS image, use the comdef.h provided with elite to prevent
// conflicts
#include "comdef.h"

#else
// ==============
// QcComDef.h generic definitions start here
// ==============

#define _ANDROID

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1                            /* Boolean true value.           */
#define FALSE  0                            /* Boolean false value.          */

#define  ON   1                             /* On value.                     */
#define  OFF  0                             /* Off value.                    */

#ifdef _lint
  #define NULL 0
#endif

#ifndef NULL
  #define NULL  0
#endif

/*=============================================================================
      Standard types
=============================================================================*/
#ifndef boolean
#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;        /* Boolean value type.           */
#define _BOOLEAN_DEFINED
#endif
#endif

#ifndef uint32
#ifndef _UINT32_DEFINED
typedef  unsigned int       uint32;          /* Unsigned 32 bit value         */
#define _UINT32_DEFINED
#endif
#endif

#ifndef uint16
#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;         /* Unsigned 16 bit value         */
#define _UINT16_DEFINED
#endif
#endif

#ifndef uint8
#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;          /* Unsigned 8  bit value         */
#define _UINT8_DEFINED
#endif
#endif

#ifndef int32
#ifndef _INT32_DEFINED
typedef  signed int         int32;           /* Signed 32 bit value           */
#define _INT32_DEFINED
#endif
#endif

#ifndef int16
#ifndef _INT16_DEFINED
typedef  signed short       int16;          /* Signed 16 bit value           */
#define _INT16_DEFINED
#endif
#endif

#ifndef int8
#ifndef _INT8_DEFINED
typedef  signed char        int8;           /* Signed 8  bit value           */
#define _INT8_DEFINED
#endif
#endif

#ifndef int64
#ifndef _INT64_DEFINED
typedef  signed long long   int64;          /* Signed 64 bit value           */
#define _INT64_DEFINED
#endif
#endif

#ifndef uint64
#ifndef _UINT64_DEFINED
typedef  unsigned long long uint64;         /* Unsigned 64 bit value         */
#define _UINT64_DEFINED
#endif
#endif

#ifndef Word16
#ifndef _WORD16_DEFINED
typedef int16                  Word16;
#define _WORD16_DEFINED
#endif
#endif

#ifndef Word32
#ifndef _WORD32_DEFINED
typedef int32                  Word32;
#define _WORD32_DEFINED
#endif
#endif

#ifndef UWord32
#ifndef _UWORD32_DEFINED
typedef uint32                 UWord32;
#define _UWORD32_DEFINED
#endif
#endif

#ifdef MATLAB
typedef int8                   int8_t;
typedef uint8                  uint8_t;
typedef int16                  int16_t;
typedef uint16                 uint16_t;
typedef int32                  int32_t;
typedef uint32                 uint32_t;
#endif

// ==============
// QcComDef.h generic definitions end here
// ==============

#endif // __hexagon__ && !US_STANDALONE_ALG_TESTS

// Ultrasound specific definitions
#if defined(__hexagon__) || defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
#define MEM_ALIGN(t, a)    t __attribute__((aligned(a)))
#elif defined(_MSC_VER)
#define MEM_ALIGN(t, a)    __declspec(align(a)) t
#elif defined(__ARMCC_VERSION)
#define MEM_ALIGN(t, a)    __align(a) t
#else
#error Unknown compiler
#endif

/* Align an offset to multiple of 4 (useful for aligning word offsets
   to 64-bit boundary */
#define ALIGN4(o)         (((o)+3)&(~3))
/* Align an offset to multiple of 8 (useful for aligning byte offsets
   to 64-bit boundary */
#define ALIGN8(o)         (((o)+7)&(~7))
/* Align addr so it starts at size boundary. When aligning pointers make sure to cast addr
   to an integer type */
#define ALIGN_US(addr, size) (((intptr_t)(addr) + size - 1) & ~(size - 1))

#ifndef Word64
#ifndef _WORD64_DEFINED
typedef int64                  Word64;
#define _WORD64_DEFINED
#endif
#endif

#ifndef CWord2x16
#ifndef _CWORD2_16_DEFINED
typedef int32                  CWord2x16;
#define _CWORD2_16_DEFINED
#endif
#endif

#ifndef CWord2x32
#ifndef _CWORD2_32_DEFINED
typedef int64                  CWord2x32;
#define _CWORD2_32_DEFINED
#endif
#endif



#endif /* _QC_COMDEF_H_ */
