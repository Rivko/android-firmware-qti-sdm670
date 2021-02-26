/***************************************************************************
* Copyright (c) 2008-2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file fft32x16.h
    \brief Brief description of file
*/

/*! 
Performs N-point complex FFT  

\param Input pointer to input buffer
\param points number of FFT points
\param twiddles pointer to twiddles factor array
\param Output pointer to output buffer

\details

\b Assembly \b Assumptions
 - \a Input must be aligned by array size
 - \a Output must be aligned by 8-bytes
 - \a twiddles must be aligned by 4-bytes

\b Cycle-Count
 - (9N/4+5)*K - (11N+4)/12 +  8, for N = 4^K
 - (9N/4+5)*K + (13N-8)/12 + 14, for N= 2*(4^K) 

\b Notes
 - Radix-4 DIF implementation
 - Real and imaignary parts of input and output samples are in 32-bit.
 - Real and imaignary parts of twiddle factors are in 16-bit.
 - 3N/4 twiddle factors are interleaved and stored in bit-reverse order.

*/

void us_fft32x16( CWord2x32 *Input, int points, CWord2x16 *twiddles,
               CWord2x32 *Output);




