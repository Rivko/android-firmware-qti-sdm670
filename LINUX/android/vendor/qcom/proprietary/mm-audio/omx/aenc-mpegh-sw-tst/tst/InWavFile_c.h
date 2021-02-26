/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   InWavFile_c.h            TYPE: C-header file                 *]
[*                                                                           *]
[* DESCRIPTION: This file contains the prototypes of utility functions used  *]
[*              for processing the input wave files                          *]
[*****************************************************************************/
#ifndef _IN_WAV_FILE_C_H_
#define _IN_WAV_FILE_C_H_

#include <stdio.h>
#include "wavefile.h"

#define FLOAT float

typedef struct InWaveFileStruct
{
    FILE *_fp;
    /*uint8_t *_waveBuf;*/
    uint8_t _waveBuf[196];
    int32_t _samplesLeft;
    wav_header_t _wh;
}InWaveFileStruct;

#ifdef __cplusplus
extern "C"
{
#endif
FLOAT ConvPCM16ToFloat_convert(uint8_t *p);
FLOAT ConvPCM24ToFloat_convert(uint8_t *p);
FLOAT ConvIEEE32ToFloat_convert(uint8_t *p);
int32_t InWavFile_open(char *name, InWaveFileStruct *waveFileInfo);
int32_t InWavFile_read(FLOAT inp[][1024], int32_t numToRead, InWaveFileStruct *waveFileInfo);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _IN_WAV_FILE_C_H_ */
