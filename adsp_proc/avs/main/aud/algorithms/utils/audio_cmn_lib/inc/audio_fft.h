#ifndef __FFT_H
#define __FFT_H
/***************************************************************************
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
****************************************************************************/ 

#include "audio_fft_basic_ops.h"


/*! \file fft.h
    \brief Brief description of file
*/

/*! 
Performs (scaled) N-point complex FFT  

\param Input pointer to input buffer
\param points number of FFT points
\param twiddles pointer to twiddles factor array
\param Output pointer to output buffer

\details

\b Assembly \b Assumptions
 - \a Input must be aligned by array size
 - \a Output must be aligned by 8
 - \a twiddles must be aligned by 4

\b Cycle-Count
 - (7N/8)*K + (5N-8)/24   + 5K +  7, for N = 4^K
 - (7N/8)*k + (17N-16)/24 + 5K + 11, for N= 2*(4^K) 

\b Notes
 - Radix-4 DIF implementation
 - 3N/4 twiddle factors are interleaved and stored in bit-reverse order.
 - CONVERGENT_ROUNDING defines convergent/2's-complement rounding mode.
 - SCALED_FFT defines scaled/non-scaled version FFT.

*/

void  fft16x16( cint2x16 *Input, int points, cint2x16 *twiddles, cint2x16 *Output);

void fft32x16(CWord2x32 *input, int N, CWord2x16 *w, CWord2x32 *output);
void rfft(CWord2x32 *input, int N, CWord2x16 *Wt1, CWord2x16 *Wt2, CWord2x32 *output);
void ifft32x16(CWord2x32 *input, int N, CWord2x16 *w, CWord2x32 *output);
void rifft(CWord2x32 *input, int N, const CWord2x16 *Wt1, const CWord2x16 *Wt2, CWord2x32 *output);




/*! 
Performs N-point complex inverse FFT  

\param Input pointer to input buffer
\param points number of IFFT points
\param twiddles pointer to twiddles factor array
\param Output pointer to output buffer

\details

\b Assembly \b Assumptions
 - \a Input must be aligned by array size
 - \a Output must be aligned by 8
 - \a twiddles must be aligned by 4

\b Cycle-Count
 - (7N/8)*K + (5N-8)/24   + 5K +  7, for N = 4^K
 - (7N/8)*k + (17N-16)/24 + 5K + 11, for N= 2*(4^K) 

\b Notes
 - Radix-4 DIF implementation
 - Twiddle factors are the same as FFT
 - CONVERGENT_ROUNDING defines convergent/2's-complement rounding mode.

*/


void ifft( CWord2x16 *Input, int points, CWord2x16 *twiddles,
          CWord2x16 *Output);

		  
#endif
