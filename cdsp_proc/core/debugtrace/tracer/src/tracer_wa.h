#ifndef TRACER_WA_H
#define TRACER_WA_H
/*===========================================================================
  @file tracer_wa.h

  Constant definitions for tracer workaround to restricted bit widths HW
  bugs.

  Different possible builds:
    No flags specified is the original SW design (simplified OST BP header)
    TRACER_OST_WA_PAD is the releasable SW workaround build.
    Other options are used for testing performance of
       different designs.
       TRACER_OST_WA_RW does a read write pre-alignment w/postpad.
       TRACER_OST_BASIC does the standard OST BP format (with length field)
                        but doesn't postpad.

  Valid TRACER_WA_ALIGNMENT values are 4, 8, and 16 (bytes).
  FLAGS := -DTRACER_OST_WA_PAD -DTRACER_WA_ALIGNMENT=4 #SW Workaround
  FLAGS := -DTRACER_OST_WA_PAD -DTRACER_WA_ALIGNMENT=4 -DIMITATE_STPPKTS #SWWA Devt
  FLAGS := -DTRACER_OST_WA_RW -DTRACER_WA_ALIGNMENT=4
  FLAGS := -DTRACER_OST_BASIC

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_wa.h#1 $ */
/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/* OST Protocol Version
   Value  1 = Tracer (workaround) Base Protocol
   Value 16 = OST BP Version 1.0 documented in
              MIPI Alliance Specification for Open System Trace (OST)
              Base Protocol Version 0.80.00. April 22, 2009
*/
/* Version of the OST protocol being used */
#define TRACER_OST_VERSION_WA        1    /**< Tracer Workaround version */
#define TRACER_OST_VERSION_MIPI     16    /**< MIPI OST Base Prot Ver 1.0 */

/* Tracer token values */
#define TRACER_OST_TOKEN_STARTSIMPLE  0x10
#define TRACER_OST_TOKEN_STARTBASIC   0x30
#define TRACER_OST_TOKEN_STARTWA      0x30
#define TRACER_OST_TOKEN_END          0x40
#define TRACER_OST_TOKEN_SWEVENT      0x80


// OST BP formats
#define TRACER_OST_FMT_SIMPLE    0  // Simplified OST BP header
#define TRACER_OST_FMT_BASIC     1  // Standard OST BP header
#define TRACER_OST_FMT_WA_RW     2  // Workaround header, no pre-padding
#define TRACER_OST_FMT_WA_PREPAD 3  // Workaround header, w/pre-padding


// Validate boundary and size alignment value.
#ifndef TRACER_WA_ALIGNMENT
   #define TRACER_WA_ALIGNBYTE  4 // Default
#else
   #if ((TRACER_WA_ALIGNMENT != 4 ) && \
        (TRACER_WA_ALIGNMENT != 8 ) && \
        (TRACER_WA_ALIGNMENT != 16))
      #define TRACER_WA_ALIGNBYTE  4
   #else
      #define TRACER_WA_ALIGNBYTE  TRACER_WA_ALIGNMENT
   #endif
#endif

#ifdef TRACER_OST_SIMPLE            // Simplified OST BP format
   #define TRACER_OST_FORMAT        TRACER_OST_FMT_SIMPLE
   #define TRACER_OST_TOKEN_START   TRACER_OST_TOKEN_STARTSIMPLE
   #define TRACER_OST_VERSION       TRACER_OST_VERSION_MIPI
   #undef TRACER_WA_ALIGNBYTE
   #define TRACER_WA_ALIGNBYTE   0  // Don't care, not applicable.

#else
#ifdef TRACER_OST_BASIC             // Standard OST BP format
   #define TRACER_OST_FORMAT        TRACER_OST_FMT_BASIC
   #define TRACER_OST_TOKEN_START   TRACER_OST_TOKEN_STARTBASIC
   #define TRACER_OST_VERSION       TRACER_OST_VERSION_MIPI

#else
#ifdef TRACER_OST_WA_RW             // Tracer SW workaround (RW)
   #define TRACER_OST_FORMAT        TRACER_OST_FMT_WA_RW
   #define TRACER_OST_TOKEN_START   TRACER_OST_TOKEN_STARTWA
   #define TRACER_OST_VERSION       TRACER_OST_VERSION_WA

#else
#ifdef TRACER_OST_WA_PAD            // Tracer SW workaround (Pad)
   #define TRACER_OST_FORMAT        TRACER_OST_FMT_WA_PREPAD
   #define TRACER_OST_TOKEN_START   TRACER_OST_TOKEN_STARTWA
   #define TRACER_OST_VERSION       TRACER_OST_VERSION_WA

#else                               // Default when nothing given
   #define TRACER_OST_FORMAT        TRACER_OST_FMT_SIMPLE
   #define TRACER_OST_TOKEN_START   TRACER_OST_TOKEN_STARTSIMPLE
   #define TRACER_OST_VERSION       TRACER_OST_VERSION_MIPI
   #undef TRACER_WA_ALIGNBYTE
   #define TRACER_WA_ALIGNBYTE   0  // Don't care, not applicable.

#endif   // #Prepad
#endif   // #RW
#endif   // #Basic
#endif   // #Simple

#endif /* #ifndef TRACER_WA_H */

