/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   MPEGHEncFileUtil_c.h          TYPE: C-header file            *]
[*                                                                           *]
[* DESCRIPTION: Header file for MPEG-H file utilities                        *]
[*****************************************************************************/

#ifndef _MPEG_H_ENC_FILE_UTIL_C_H_
#define _MPEG_H_ENC_FILE_UTIL_C_H_

#include <stdio.h>
#include "InWavFile_c.h"
#include "wavIO.h"

#define USAC_FRAME_SIZE         1024
#define MAX_PACKET_BITBUF_SIZE     8192 /* in bytes */

/* ***********************************************************************/
/**
* @Description Safer free
*
* @Param ptr
*/
/* ***********************************************************************/
#define FREE(ptr)\
{\
  if(ptr)\
  {\
    free(ptr);\
  }\
}\

/* ***********************************************************************/
/**
* @Description free a 2d memory
*
* @Param pptr 2 dimensional pointer
* @Param size size of the inner dimension
*
*/
/* ***********************************************************************/
#define FREE_2D(pptr,size)\
{\
  int32_t i;\
  for(i=0;i<size;i++)\
  FREE(pptr[i]);\
  FREE(pptr);\
}

/**
* Enumeration representing the two types of channel sequence for HOA:
*
* 3DMPEG, or SID: (0, 0), (1, +1), (1, -1), (1, 0), (2, +2), (2, -2), (2, +1), (2, -1), ...
* 3D, or ACN:     (0, 0), (1, -1), (1, 0), (1, +1), (2, -2), (2, -1), (2, 0), (2, +1), ...
*/
typedef enum {
    ORDER_3DMPEG, /* "3DMPEG" ordering, or "SID" */
    ORDER_3D      /* "3D" ordering, or "ACN" */
} HOAChannelOrder;

typedef struct MPEGHEncFileUtilStruct {

  int32_t _rendererID;
  int32_t _nfcFlag;
  FLOAT _nfcRefDist;
  int32_t _hoaOrder;
  FLOAT **dump_amb_buf;
  int32_t _numInputChannels;
  InWaveFileStruct _inputWavFiles[50];
  WAVIO_HANDLE hWavIO_N3D;
}MPEGHEncFileUtilStruct;

#ifdef __cplusplus
extern "C"
{
#endif

int32_t MPEGHEncFileUtil_openInputWavFiles(MPEGHEncFileUtilStruct *fileData, char *itemDir, char *itemName, HOAChannelOrder hoaChannelOrder, int32_t hoaOrder);
int32_t MPEGHEncFileUtil_openMultiChannelInputWavFile(MPEGHEncFileUtilStruct *fileData, char *filePath, int32_t hoaOrder);
const wav_header_t* MPEGHEncFileUtil_getInputWavHeader(MPEGHEncFileUtilStruct *fileData);
int32_t MPEGHEncFileUtil_readInput(MPEGHEncFileUtilStruct *fileData, FLOAT inPCM[][USAC_FRAME_SIZE], int32_t numToRead);

FILE* mpegh_file_write_open(char* outfilename);
int32_t mpegh_file_out_write(FILE *fp, uint8_t* buf, uint32_t frame_samples);
int32_t mpegh_util_file_close(FILE* fp);

int32_t mpegh_file_open_n3d_wav_file(MPEGHEncFileUtilStruct *fileData, int32_t channels, int32_t sample_rate);
int32_t mpegh_file_write_n3d_wav_file(MPEGHEncFileUtilStruct *fileData, float **pp_OutputBuffer);
int32_t mpegh_file_close_n3d_wav_file(MPEGHEncFileUtilStruct *fileData);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MPEG_H_ENC_FILE_UTIL_C_H_ */
