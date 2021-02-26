/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2009 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*/

#ifndef FFT2_UTIL_H
#define FFT2_UTIL_H
/*============================================================================
  @file quartet_rfft.h

  Declare helper functions for forward and inverse real-input FFT (simulates QDSP6
  implementation).

============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/utils/audio_cmn_lib/inc/fft2_util.h#1 $ */

#include "fft2_private_comdef.h"

#if !(defined(bitrev))
Word32 bitrev( Word32 x, Word32 BITS );
#endif
void ButterflyRadix2Scaled ( CWord2x16 *x );
void ButterflyRadix4Scaled ( CWord2x16 *x );

void ifftButterflyRadix2 ( CWord2x16 *x );
void ifftButterflyRadix4 ( CWord2x16 *x );

CWord2x32 L_cmult32x16( CWord2x32 x, CWord2x16 y);
CWord2x32 L_mac_cmult32x16( CWord2x32 s, CWord2x32 x, CWord2x16 y);


void ButterflyRadix2_32x16 ( CWord2x32 *x );
void ButterflyRadix4_32x16 ( CWord2x32 *x );
void ButterflyRadix2Scaled_32x16 ( CWord2x32 *x );
void ButterflyRadix4Scaled_32x16 ( CWord2x32 *x );
void ifftButterflyRadix2_32x16 ( CWord2x32 *x );
void ifftButterflyRadix4_32x16 ( CWord2x32 *x );
void ifftButterflyRadix2Scaled_32x16 ( CWord2x32 *x );
void ifftButterflyRadix4Scaled_32x16 ( CWord2x32 *x );

#endif /* #ifndef FFT2_UTIL_H */
