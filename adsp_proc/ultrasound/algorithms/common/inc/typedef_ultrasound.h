/*-----------------------------------------------------------------------
 Copyright (c) 2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 01/01/13   IM      Added limits of exact-width integer types
 04/28/13   IM      Add support for proximity motion detection
 05/21/13   IM      Enable handling warnings as errors
==========================================================================*/

#ifndef _TYPEDEF_ULTRASOUND_
#define _TYPEDEF_ULTRASOUND_

// guard against known re-definer
#ifdef COM_DTYPES_H
#define _WORD8_DEFINED
#define _UWORD8_DEFINED
#define _WORD16_DEFINED
#define _UWORD16_DEFINED
#define _WORD32_DEFINED
#define _UWORD32_DEFINED
#endif
/*=============================================================================
      Standard types
  They are defined only if not defined elsewhere (like in elite 
  headers) 
=============================================================================*/
#ifndef boolean
#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;        /* Boolean value type.           */
#define _BOOLEAN_DEFINED
#endif
#endif

#ifndef uint32
#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;         /* Unsigned 32 bit value         */
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
typedef  signed long int    int32;          /* Signed 32 bit value           */
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

#ifdef __hexagon__

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

#endif // __hexagon__

/**
  @brief Define Basic Data Types 
*/

#ifndef Word8
#ifndef _WORD8_DEFINED
typedef int8                 Word8;
#define _WORD8_DEFINED
#endif
#endif

#ifndef UWord8
#ifndef _UWORD8_DEFINED
typedef uint8                UWord8;
#define _UWORD8_DEFINED
#endif
#endif

#ifndef Word16
#ifndef _WORD16_DEFINED
typedef int16                  Word16;
#define _WORD16_DEFINED
#endif
#endif

#ifndef UWord16
#ifndef _UWORD16_DEFINED
typedef uint16               UWord16;
#define _UWORD16_DEFINED
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

#ifndef Word64
#ifndef _WORD64_DEFINED
typedef int64                  Word64;
#define _WORD64_DEFINED
#endif
#endif

#ifndef UWord64
#ifndef _UWORD64_DEFINED
typedef uint64               UWord64;
#define _UWORD64_DEFINED
#endif
#endif

typedef unsigned char		 BOOL;    

/**
  @brief Define Complex Number
*/
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

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif 



