/*========================================================================*/
/**
@file AudioComdef.h

@brief Type definitions for memory alignment attribute macros.

This file contains ASM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2008, 2010, 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/AudioComdef.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/09/10   rkc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _AUDIOCOMDEF_H_      
#define _AUDIOCOMDEF_H_
      
#include "qurt_elite_types.h"


/** @cond
*/
/* Type definition for signed 40-bit integers. */
typedef  int64_t int40;

/* Type definition for unsigned 40-bit integers. */
typedef  uint64_t uint40;
/** @endcond */


/** @addtogroup mem_alignment_macros
@{ */

#if ((defined __hexagon__) || (defined __qdsp6__)) || defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
#define MEM_ALIGN(t, a)    t __attribute__((aligned(a)))
  /**< Memory alignment macro for target compilers. The value of the MEM_ALIGN 
       macro depends on the following conditional settings:

       - If __qdsp6__ or __GNUC__ or __ARMCC_VERSION >= 220000 is TRUE, 
         MEM_ALIGN = t __attribute__((aligned(a))).
       - If _MSC_VER is TRUE, MEM_ALIGN = __declspec(align(a)) t.
       - If __ARMCC_VERSION is TRUE, MEM_ALIGN = __align(a) t.
       - If the compiler is unknown, it returns an unknown compiler error. */

#elif defined(_MSC_VER)
#define MEM_ALIGN(t, a)    __declspec(align(a)) t
  /**< Memory alignment macro for target compilers. The value of the MEM_ALIGN 
       macro depends on the following conditional settings:

       - If __qdsp6__ or __GNUC__ or __ARMCC_VERSION >= 220000 is TRUE, 
         MEM_ALIGN = t __attribute__((aligned(a))).
       - If _MSC_VER is TRUE, MEM_ALIGN = __declspec(align(a)) t.
       - If __ARMCC_VERSION is TRUE, MEM_ALIGN = __align(a) t.
       - If the compiler is unknown, it returns an unknown compiler error. */

#elif defined(__ARMCC_VERSION)
#define MEM_ALIGN(t, a)    __align(a) t
  /**< Memory alignment macro for target compilers. The value of the MEM_ALIGN 
       macro depends on the following conditional settings:

       - If __qdsp6__ or __GNUC__ or __ARMCC_VERSION >= 220000 is TRUE, 
         MEM_ALIGN = t __attribute__((aligned(a))).
       - If _MSC_VER is TRUE, MEM_ALIGN = __declspec(align(a)) t.
       - If __ARMCC_VERSION is TRUE, MEM_ALIGN = __align(a) t.
       - If the compiler is unknown, it returns an unknown compiler error. */

#else
#error Unknown compiler
#endif

/** @} */ /* end_addtogroup mem_alignment_macros */
#ifndef _COMPLEX_TYPES
#define _COMPLEX_TYPES
typedef int32                  cint2x16; 
typedef int64                  cint2x32;
#endif

#endif /* _AUDIOCOMDEF_H_ */

