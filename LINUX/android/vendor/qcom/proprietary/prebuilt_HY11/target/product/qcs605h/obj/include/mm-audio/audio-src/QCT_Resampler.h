#ifndef ANDROID_QCT_RESAMPLER_API_H
#define ANDROID_QCT_RESAMPLER_API_H
/*============================================================================
* Copyright (c) 2012, 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#include <stdint.h>
#include <sys/types.h>

namespace android {

class QCT_Resampler {

public:
	static size_t	MemAlloc(int bitDepth, int inChannelCount, int32_t inSampleRate, int32_t sampleRate);
	static void		Init(int16_t *pState, int32_t inChannelCount, int32_t inSampleRate, int32_t mSampleRate, int32_t is32BitIn=0,int32_t is32BitOut=1, int32_t dynamicEnable=1);
	static void		Resample90dB(int16_t* pState, void* in, void* out, size_t inFrameCount, size_t outFrameCount);
	static size_t	GetNumInSamp(int16_t* pState, size_t outFrameCount);
	static void     ReInitDynamicResamp(int16_t *pState, int32_t inSampleRate, int32_t mSampleRate);
};
};
#endif  //ANDROID_QCT_RESAMPLER_API_H
