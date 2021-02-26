/***************************************************************************
* Copyright (c) 2008-2013 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 
#ifndef __FFTWIDDLES_H__
#define __FFTWIDDLES_H__

#include "typedef_ultrasound.h"

/********************************************/
/* interleaved twiddles with size of        */
/* 3*4096/4                                 */
/* + support up to 4096-point FFT           */
/* + table can be shortened for smaller     */
/*   size FFT                               */
/* Change to use the version from fft32x16  */
/* sources                                  */
/********************************************/
extern CWord2x16 us_Wtwiddles[];

// Twiddle factors used for the last stage of N-point real FFT
// generated as j*W^k, k=1, 2, ... N/4

extern CWord2x16 us_Wt2_2048[2048/4];
extern CWord2x16 us_Wt2_1024[1024/4];
extern CWord2x16 us_Wt2_512[512/4];
extern CWord2x16 us_Wt2_256[256/4];

#endif
