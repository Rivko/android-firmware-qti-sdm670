/***************************************************************************
* Copyright (c) 2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 


/*! \file rfft32x16.h
    \brief Brief description of file
*/

/*! 
Implement N-point real-valued FFT with scaling factor 1/N

\param Input pointer to input array
\param points number of points
\param Wt1 twiddle factors for N/2-point complex FFT
\param Wt2 twiddle factors for last stage
\param Output pointer to output array

*/

void us_rfft32x16(CWord2x32 *input, int N, CWord2x16 *Wt1, CWord2x16 *Wt2, CWord2x32 *output);




