/*******************************************************************************
 *  Copyright (c) 2018 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *   CHANGE HISTORY
 *   dd/mm/yy   Code Ver     Description                        Author
 *   --------   -------      -----------                        ------
 ******************************************************************************/
/***********************************************************************************

 This software module was originally developed by

 Fraunhofer IIS

 in the course of development of the ISO/IEC 23008-3 for reference purposes and its
 performance may not have been optimized. This software module is an implementation
 of one or more tools as specified by the ISO/IEC 23008-3 standard. ISO/IEC gives
 you a royalty-free, worldwide, non-exclusive, copyright license to copy, distribute,
 and make derivative works of this software module or modifications  thereof for use
 in implementations or products claiming conformance to the ISO/IEC 23008-3 standard
 and which satisfy any specified conformance criteria. Those intending to use this
 software module in products are advised that its use may infringe existing patents.
 ISO/IEC have no liability for use of this software module or modifications thereof.
 Copyright is not released for products that do not conform to the ISO/IEC 23008-3
 standard.

 Fraunhofer IIS retains full right to modify and use the code for its own purpose,
 assign or donate the code to a third party and to inhibit third parties from using
 the code for products that do not conform to MPEG-related ITU Recommendations and/or
 ISO/IEC International Standards.

 This copyright notice must be included in all copies or derivative works.

 Copyright (c) ISO/IEC 2013.

 ***********************************************************************************/

#ifndef _WAVIO_H
#define _WAVIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef struct _WAVIO* WAVIO_HANDLE;

int32_t wavIO_init(WAVIO_HANDLE *hWavIO, const uint32_t framesize, const uint32_t fillLastIncompleteInputFrame, int32_t delay);
int32_t wavIO_setDelay(WAVIO_HANDLE hWavIO, int32_t delay);
int32_t wavIO_openRead(WAVIO_HANDLE hWavIO, FILE *pInFileName, uint32_t *nInChannels, uint32_t *InSampleRate, uint32_t * InBytedepth, uint64_t *nTotalSamplesPerChannel, int32_t *nSamplesPerChannelFilled);
int32_t wavIO_readFrame(WAVIO_HANDLE hWavIO, float **inBuffer, uint32_t *nSamplesReadPerChannel, uint32_t *isLastFrame, uint32_t * nZerosPaddedBeginning,  uint32_t * nZerosPaddedEnd);
int32_t wavIO_openWrite(WAVIO_HANDLE hWavIO, FILE *pOutFileName, uint32_t nOutChannels, uint32_t OutSampleRate, uint32_t bytesPerSample);
int32_t wavIO_writeFrame(WAVIO_HANDLE hWavIO, float **outBuffer, uint32_t nSamplesToWritePerChannel, uint32_t *nSamplesWrittenPerChannel);
int32_t wavIO_writeFrame_withOffset(WAVIO_HANDLE hWavIO, float **outBuffer, uint32_t nSamplesToWritePerChannel, uint32_t *nSamplesWrittenPerChannel, uint32_t offset);
int32_t wavIO_updateWavHeader(WAVIO_HANDLE hWavIO, uint64_t * nTotalSamplesWrittenPerChannel);
int32_t wavIO_close(WAVIO_HANDLE hWavIO);

#ifdef __cplusplus
}
#endif

#endif
