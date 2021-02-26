/***************************************************************************
* Copyright (c) 2012-2014 Qualcomm Technologies, Inc. 
* All Rights Reserved 
* Confidential and Proprietary - Qualcomm Technologies, Inc. 
****************************************************************************/ 

/***************************************************************************
* Edit History
*
* when       who     what, where, why
* --------   ---     ------------------------------------------------------- 
* 07/01/12   IM      Initial version
* 01/01/13   IM      Replace Complex FFT to Real FFT
* 10/06/13   LD      Add support functions for fft32x16
* 02/02/14   LD      Several basic functions moved to basic_ops
* --------   ---     ------------------------------------------------------- 
****************************************************************************/ 
#ifndef __FFT_UTIL_H__
#define __FFT_UTIL_H__

#include "typedef_ultrasound.h"

// --------------------------------------
// Functions that are only used by C reference implementation
// of FFT functions (not compiled when assembler version is used)
// --------------------------------------

Word32 bitrev( Word32 x, Word32 BITS );

void ButterflyRadix2 ( CWord2x16 *x );
void ButterflyRadix4 ( CWord2x16 *x );

void ifftButterflyRadix2 ( CWord2x16 *x );
void ifftButterflyRadix4 ( CWord2x16 *x );

void us_Radix2BTFLYsc32 ( CWord2x32 *x );
void us_Radix4BTFLYsc32 ( CWord2x32 *x );
void us_Radix4BTFLYqv3sc32 ( CWord2x32 *x );



// --------------------------------------
// Functions that are also used by algorithms and 
// are always compiled
// --------------------------------------

CWord2x32 us_L_cmult32x16(CWord2x32 x, CWord2x16 y);

#endif
