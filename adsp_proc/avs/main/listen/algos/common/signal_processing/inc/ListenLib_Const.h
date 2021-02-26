/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2012-2016,2017 QUALCOMM Technologies, Incorporated.         *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   ListenLib_Const.h        TYPE: C-header file                *]
[* DESCRIPTION: Listen global constant definition                            *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2012-01-27 minsubl Initial revision                                     *]
[*****************************************************************************/


#ifndef LISTENLIB_CONST_H_
#define LISTENLIB_CONST_H_

enum {
	// fft related
	N_FFT = 512,       // fft size (assuming 512pt real-fft done by 256pt complex fft)
	N_H_FFT = 256,     // half fft size
	N_Q_FFT = 128,     // quad fft size
	// filter bank analysis related
	KLO = 1,
	KHI = 256,
	N_CHANS = 22,
	// cepstrum dimension
	NCEPS_SIZE = 12,		
	DCEPS_SIZE = 13,	
};
#endif /* LISTENLIB_CONST_H_ */
