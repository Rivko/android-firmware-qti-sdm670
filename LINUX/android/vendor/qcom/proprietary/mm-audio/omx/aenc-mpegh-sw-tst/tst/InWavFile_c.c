/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   InWavFile_c.c            TYPE: C-source file                 *]
[*                                                                           *]
[* DESCRIPTION: This file contains the utility functions used for processing *]
[*              the input wave files                                         *]
[*****************************************************************************/

#include "InWavFile_c.h"

#define MAX_16 0x00007fffL
#define MIN_16 (-MAX_16-1)

#define MAX_24 0x007fffffL
#define MIN_24 (-MAX_24-1)
#ifdef PCM16_SUPPORT
FLOAT ConvPCM16ToFloat_convert(uint8_t *p)
{
    int32_t samp = p[0];
    samp += (p[1]) << 8;
    if (samp > MAX_16) samp -= (MAX_16+1)*2;
    FLOAT val;
    val = ((FLOAT)samp) / MAX_16;
    if (val != val) val = 0;
    return val;// ((FLOAT)samp) / MAX_16;
}
#endif //PCM16_SUPPORT
#ifdef PCM24_SUPPORT
FLOAT ConvPCM24ToFloat_convert(uint8_t *p)
{
    int32_t samp = p[0];
    samp += (p[1]) << 8;
    samp += (p[2]) << 16;
    if (samp > MAX_24) samp -= (MAX_24+1)*2;
    FLOAT val;
    val = ((FLOAT)samp) / MAX_24;
    if (val != val) val = 0;
    return val;
}
#endif // PCM24_SUPPORT
FLOAT ConvIEEE32ToFloat_convert(uint8_t *p)
{
    FLOAT val = (FLOAT)(*((float *)p));
    if (val != val) val = 0;
    return val;
}

int32_t InWavFile_open(char *name, InWaveFileStruct *waveFileInfo)
{
    int32_t ret;
    waveFileInfo->_fp = fopen(name, "rb");
    if (waveFileInfo->_fp == NULL) {
        return -1;
    }
    ret = read_wav_header(waveFileInfo->_fp, &waveFileInfo->_wh);
    if (ret != WAV_SUCCESS) {
        return ret;
    }

    waveFileInfo->_samplesLeft = waveFileInfo->_wh.dataLength / waveFileInfo->_wh.bytesPerSample;
    return 0;
}

int32_t InWavFile_read(FLOAT inp[][1024], int32_t numToRead, InWaveFileStruct *waveFileInfo)
{
    int32_t bytesPerSamp = waveFileInfo->_wh.bitsPerSample / 8;

    if (waveFileInfo->_fp == NULL || inp == NULL) {
        return -1;
    }

    if (waveFileInfo->_samplesLeft < numToRead) {
        numToRead = waveFileInfo->_samplesLeft;
    }
    for (int32_t i=0; i < numToRead; i++) {
        int32_t ret = fread(waveFileInfo->_waveBuf, 1, waveFileInfo->_wh.bytesPerSample, waveFileInfo->_fp);
        if (ret < waveFileInfo->_wh.bytesPerSample || feof(waveFileInfo->_fp)) {
            return i;
        }

        /* Calling appropriate conversion function based on sample data type and no of bits */
#ifdef PCM24_SUPPORT
        if ((waveFileInfo->_wh.coding == WAVE_FMT_PCM || waveFileInfo->_wh.coding == (int16_t)WaveFormatExtensible) && waveFileInfo->_wh.bitsPerSample == 24) {

            for (int32_t ch = 0; ch < waveFileInfo->_wh.numChannels; ch++) {
                inp[ch][i] = ConvPCM24ToFloat_convert(&waveFileInfo->_waveBuf[ch*bytesPerSamp]);
            }
        }
#endif // PCM24_SUPPORT
#ifdef PCM16_SUPPORT
       if ((waveFileInfo->_wh.coding == WAVE_FMT_PCM || waveFileInfo->_wh.coding == (int16_t)WaveFormatExtensible)&& waveFileInfo->_wh.bitsPerSample == 16) {

           for (int32_t ch = 0; ch < waveFileInfo->_wh.numChannels; ch++) {
               inp[ch][i] = ConvPCM16ToFloat_convert(&waveFileInfo->_waveBuf[ch*bytesPerSamp]);
           }
       }
#endif // PCM16_SUPPORT
        if ((waveFileInfo->_wh.coding == WAVE_FMT_IEEE || waveFileInfo->_wh.coding == (int16_t)WaveFormatExtensible || waveFileInfo->_wh.coding == WAVE_FMT_PCM) && waveFileInfo->_wh.bitsPerSample == 32) {
            for (int32_t ch = 0; ch < waveFileInfo->_wh.numChannels; ch++) {
                inp[ch][i] = ConvIEEE32ToFloat_convert(&waveFileInfo->_waveBuf[ch*bytesPerSamp]);
            }
        }
    }
    waveFileInfo->_samplesLeft -= numToRead;
    return numToRead;
}
