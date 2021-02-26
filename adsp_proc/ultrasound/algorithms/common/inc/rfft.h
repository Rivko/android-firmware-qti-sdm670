/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:26:00 CST 2008 Qualcomm Technologies INCORPORATED 
* All Rights Reserved 
* Modified by Qualcomm Technologies INCORPORATED on Mon Nov 24 16:26:00 CST 2008 
****************************************************************************/ 


/*! \file rfft.h
    \brief Brief description of file
*/

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
 - \a Output must be aligned by 8-bytes
 - \a twiddles must be aligned by 8-bytes
 - \a N must be larger than 8

\b Cycle-Count
 - (7N/16 + 5)*K + (13N-16)/24 + 19, for N = 4^k 
 - (7N/16 + 5)*K + (29N-16)/48 + 20, for N = 2*(4^K) 


\b Notes
 - The FFT is scaled down by 1/N
 - Because the FFT output has Hermitian symmetry, only N/2+1 points are computed.
 - A total of 5*N/8 twiddle factors are used

*/


void us_rfft(Word16 *input, int N, CWord2x16 *Wt1, CWord2x16 *Wt2, CWord2x16 *output);


