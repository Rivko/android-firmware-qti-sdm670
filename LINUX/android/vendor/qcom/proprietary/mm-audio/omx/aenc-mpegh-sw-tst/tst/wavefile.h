/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   wavefile.h                  TYPE: C-header file              *]
[*                                                                           *]
[* DESCRIPTION: function prototypes for reading & writing .wav files         *]
[*****************************************************************************/
#ifndef _WAVEFILE_H_
#define _WAVEFILE_H_

#include <stdint.h>

typedef struct wav_header {
   int16_t coding;
   int16_t numChannels;
   int32_t sampleRate;
   int32_t bytesPerSecond;
   int16_t bytesPerSample;
   int16_t bitsPerSample;
   uint32_t dataLength;
   int32_t dataStartPosition;
   // use fseek(fp,dataStartPosition,SEEK_SET) to start reading 1st sample
} wav_header_t;

// Windows WAVE format
#define WAVE_FMT_PCM   (0x0001)
// IEEE floating-point format
#define WAVE_FMT_IEEE  (0x0003)
// Wave Extensible format
#define WaveFormatExtensible (65534)

#define WAV_SUCCESS                 (0)    // Operation succeeded
#define WAV_ERR_INVALID_PARAM       (-1)   // Invalid parameters passed to function
#define WAV_ERR_BAD_FORMAT          (-2)   // File not in WAVE format
#define WAV_ERR_WRITE_FAILED        (-3)   // Failed to write to WAVE file

int32_t read_wav_header(FILE *fp, wav_header_t *wh);

#endif /* #ifdef _WAVEFILE_H_ */
