/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2009 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*/

#ifndef FFT2_RFFT_H
#define FFT2_RFFT_H
/*============================================================================
  @file quartet_rfft.h

  Declare methods for forward and inverse real-input FFT (QDSP6
  implementation reference).

============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/fft2_rfft.h#1 $ */

#include "fft2_private_comdef.h"


void cmplxifft32x16_scaled(CWord2x32 *input, int N, const CWord2x16 *w, CWord2x32 *output);

/*! 
Performs scaled N-point real-valued FFT  

\param Input pointer to input buffer
\param N points number of FFT points
\param Wt1 pointer to twiddles factor array for N/2-point complex FFT
\param Wt2 pointer to twiddles factor array for last stage
\param Output pointer to output buffer

\details

\b Assembly \b Assumptions
 - \a Input must be aligned by array size
 - \a Output must be aligned by 8
 - \a twiddles must be aligned by 8
 - \a N must be larger than 16

\b Cycle-Count
 - (7N/16)*(K-1) + (47N-32)/48 + 7K + 13, for N = 4^k 
 - (7N/16)*K + (35N-16)/48 + 7K + 16 , for N= 2*(4^K) 


\b Notes
 - The FFT is scaled down by 2/N
 - Because the FFT output has Hermitian symmetry, only N/2+1 points are computed.
 - A total of 5*N/8 twiddle factors are used

*/


void Ffecns_rfft(Word16 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, int16 fc, CWord2x16 *output);

void Ffecns_rifft(CWord2x16 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, int16 fc, Word16 *output);

void Ffecns_rfft_32x16_scaled(Word32 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, int16 fc, CWord2x32 *output);

void Ffecns_rfft_32x16(Word32 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, int16 fc, CWord2x32 *output);

void Ffecns_rifft_32x16_scaled(CWord2x32 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, int16 fc, Word32 *output);

void Ffecns_rfft_32x32(Word32 *input, int N, const CWord2x32 *Wt1, const CWord2x32 *Wt2, int16 fc, CWord2x32 *output);

void Ffecns_rifft_32x32_scaled(CWord2x32 *input, int N, const CWord2x32 *Wt1, const CWord2x32 *Wt2, int16 fc, Word32 *output);

#endif /* #ifndef FFT2_RFFT_H */
