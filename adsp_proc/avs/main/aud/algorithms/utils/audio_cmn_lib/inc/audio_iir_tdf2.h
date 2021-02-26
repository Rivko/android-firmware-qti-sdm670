/*========================================================================*/
/**
@file audio_iir_tdf2.h

This file contains IIR TDF2 function definition.
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
Copyright (c) 2008, 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/audio_iir_tdf2.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/17/08   sg      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
06/23/11   rkolte  24-bit support
11/03/11   wjin    merged 24-bit support
========================================================================== */

#ifndef _AUDIO_IIRTDF2_H_
#define _AUDIO_IIRTDF2_H_

#include "qurt_elite_types.h"

/*=============================================================================
      Function Declarations 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GUARD_BITS_16 3

/** @addtogroup dsp_algorithms
@{ */

/**
  IIR Transposed Direct Form 2 (TDF2) function.

  @param[in]  inp        Input buffer.
  @param[out] out        Output buffer.
  @param[in]  samples    Number of samples.
  @param[in]  numcoefs   Numerator coefficients.
  @param[in]  dencoefs   Denominator coefficients.
  @param[in]  mem        Memory.
  @param[in]  shiftn     Numerator shift.
  @param[in]  shiftd     Denominator shift.

  @return
  Interger value indicating success or failure.

  @dependencies
  None.
*/
int iirTDF2( int16_t *inp,
            int16_t *out,
            uint16_t samples,
            int32_t *numcoefs,
            int32_t *dencoefs,
            int32_t *mem,
            int16_t shiftn,
            int16_t shiftd);

/** @} */ /* end_addtogroup dsp_algorithms */

void iirTDF2_32(int32 *inp,
            int32 *out,
            int32 samples,
            int32 *numcoefs,
            int32 *dencoefs,
            int64 *mem,
            int16 shiftn,
            int16 shiftd);

void iirTDF2_16(int16 *inp,
            int16 *out,
            int32 samples,
            int32 *numcoefs,
            int32 *dencoefs,
            int64 *mem,
            int16 shiftn,
            int16 shiftd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _IIRTDF2_H_ */
