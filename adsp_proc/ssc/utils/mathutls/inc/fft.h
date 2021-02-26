/*=============================================================================
 Qualcomm Motion Classifier (CMC) FFT header

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 =============================================================================*/

#pragma once
typedef __complex__ float  cfloat;



void sfpFFT( cfloat *Input, int points, cfloat *twiddles, cfloat *Output);

