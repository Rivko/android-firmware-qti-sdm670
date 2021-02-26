/*-----------------------------------------------------------------------
Copyright (c) 2008, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 -----------------------------------------------------------------------*/

#ifndef _RESAMPLE_BY_2_API_H
#define _RESAMPLE_BY_2_API_H

short GetMemSizeOfUpBy2Filter(void);

short GetMemSizeOfDownBy2Filter(void);

void DownSampleBy2(short* pIn, short* pOut, unsigned short numSamples,
                   short* nDownBy2FilterMem);

void UpSampleBy2(short* pIn, short* pOut, unsigned short numSamples,
                   short* nUpBy2FilterMem);

#endif


