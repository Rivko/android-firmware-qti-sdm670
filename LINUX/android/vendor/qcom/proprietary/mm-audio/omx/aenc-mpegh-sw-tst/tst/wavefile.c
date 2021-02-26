/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   wavefile.c                  TYPE: C-source file              *]
[*                                                                           *]
[* DESCRIPTION: Implement utility functions for reading & writing .wav files *]
[*****************************************************************************/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "wavefile.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/


//
// read_wav_header - reads a WAVE file header from the given file
//
// fp - The file to read (must be seeked to beginning of file)
// wh - A pointer to WavHeader structure, which will contain
//      wave header on return.
//
int32_t read_wav_header(FILE *fp, wav_header_t *wh)
{
    char magic[4];
    uint32_t len = 0;
    int32_t lng;
    size_t cnt;

    if (fp == NULL || wh == NULL) {
        return WAV_ERR_INVALID_PARAM;
    }

    // Start from the beginning of the file
    rewind(fp);

    wh->coding = -1;
    wh->numChannels = -1;
    wh->sampleRate = -1;
    wh->bytesPerSecond = -1;
    wh->bytesPerSample = -1;
    wh->bitsPerSample = -1;
    wh->dataLength = -1;
    wh->dataStartPosition = -1;

    cnt = fread(magic, 4, 1, fp);
    if (cnt < 1 || strncmp("RIFF", magic, 4)) {
        return WAV_ERR_BAD_FORMAT;
    }
    cnt = fread(&lng, 4, 1, fp);
    if (cnt < 1) {
        return WAV_ERR_BAD_FORMAT;
    }
    cnt = fread(magic, 4, 1, fp);
    if (cnt < 1 || strncmp("WAVE", magic, 4)) {
        return WAV_ERR_BAD_FORMAT;
    }

    // Look for "fmt " before end of file
    while (1) {
        char c;
        if (feof(fp)) {
            return WAV_ERR_BAD_FORMAT;
        }
        cnt = fread(magic, 4, 1, fp);
        if (cnt < 1) {
            return WAV_ERR_BAD_FORMAT;
        }
        cnt = fread(&len, 4, 1, fp);
        if (cnt < 1) {
            return WAV_ERR_BAD_FORMAT;
        }

        // Check for data chunk
        if (strncmp("data", magic, 4)==0) {
            break;
        }

        if (strncmp("fmt ", magic, 4)==0) {
            cnt = fread(&(wh->coding), 2, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            cnt = fread(&(wh->numChannels), 2, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            cnt = fread(&(wh->sampleRate), 4, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            cnt = fread(&(wh->bytesPerSecond), 4, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            cnt = fread(&(wh->bytesPerSample), 2, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            cnt = fread(&(wh->bitsPerSample), 2, 1, fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
            len -= 16;
        }
        // Skip chunk
        for (; len>0; len--) {
            cnt = fread(&c,1,1,fp);
            if (cnt < 1) {
                return WAV_ERR_BAD_FORMAT;
            }
        }
    }
    wh->dataLength = len;
    wh->dataStartPosition = ftell(fp);   // save the start position of data samples
    return WAV_SUCCESS;
}
