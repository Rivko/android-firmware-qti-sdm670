/*-------------------------------------------------------------------
Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#ifndef _VIDEOCOMDEF_H
#define _VIDEOCOMDEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOL uint32
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MAX_TAG_LEN 100
#define MAX_STR_LEN 300
#define MAX_FRAME_NUMBER 99999

#define FCLOSE(file)  if(file){ fclose(file); (file) = NULL; }
#define FREE(ptr)     if(ptr){  free(ptr); (ptr) = NULL; }

#define FOPEN(fd, flag) fopen64(fd, flag)
#define FTELL(fd) ftello(fd)
#define FSEEK(fd, offset, flag) fseeko(fd, offset, flag)

#ifdef __ANDROID__
#define SPRINTF(fmt,...) snprintf(fmt,sizeof(fmt),##__VA_ARGS__)
#define SNPRINTF(fmt,size,...) snprintf(fmt,size,##__VA_ARGS__)
#else
#define SPRINTF(fmt,...) _snprintf(fmt,sizeof(fmt),##__VA_ARGS__)
#define SNPRINTF(fmt,size,...) _snprintf(fmt,size,##__VA_ARGS__)
#endif // __ANDROID__
#define STRCPY(dest,src) strlcpy(dest,src,sizeof(dest))
#define STRLCPY(dest,src,size) ((dest)?strlcpy(dest,src,size):0)
#define STRCAT(dest,src) strlcat(dest,src,sizeof(dest))
#define STRLCAT(dest,src,size) strlcat(dest,src,size)
#define STRCMP(str1,str2) ((str1 && str2)? strcmp(str1,str2) : 1)

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long long u64;
typedef char* string;

typedef enum{
    VidcStatusSuccess                   = 0,
    VidcStatusEOSExit                   = 1,
    VidcStatusAllocError                = 2,
    VidcStatusBadParamError             = 3,
    VidcStatusXmlCfgError               = 4,
    VidcStatusFileReadError             = 5,
    VidcStatusBufferOverFlowError       = 6,
    VidcStatusFileZeroLenError          = 7,
    VidcStatusDisplyReorderError        = 8,
    VidcStatusMaxSeekError              = 9,
    VidcStatusOutPutThreadError         = 10,
    VidcStatusCaptureThreadError        = 11,
    VidcStatusOperationError            = 12,
    VidcStatusNoIDRFound                = 13,
    VidcStatusDisplyReorderDupCRCError  = 14,
    VidcStatusMBIMismatchError          = 15,
    VidcStatusMISRMismatchError         = 16,
    VidcStatusEOSNoExit                 = 17,
    VidcStatusUnSupported               = 18,
    VidcStatusFlushTimeOutError         = 19,
    VidcStatusUnknownError = 0xFFFFF
}VidcStatus;

typedef enum
{
    VIDC_ERR_NIL,
    VIDC_ERR_SYS,
    VIDC_ERR_CRCMISMATCH,
    VIDC_ERR_POLL,
    VIDC_ERR_POLLTIMEOUT,
    VIDC_ERR_OUTPUT,
    VIDC_ERR_INPUT,
    VIDC_ERR_INPUT_UNSUPPORTED,
    VIDC_ERR_INPUT_DATA_CORRUPT,
    VIDC_ERR_INPUT_STARTCODE_NOTFOUND,
    VIDC_ERR_BUFFERSRETURN,
    VIDC_ERR_CRCCONSUME,
    VIDC_ERR_FILEOPEN,
    VIDC_ERR_BUFFERTIMEOUT,
    VIDC_ERR_FLUSHTIMEOUT,
    VIDC_ERR_STOPTIMEOUT,
    VIDC_ERR_ALLOC,
    VIDC_ERR_IOCTL,
    VIDC_ERR_DEVICEINIT,
    VIDC_ERR_BUFFERCOUNT,
    VIDC_ERR_BUFFERPROCESS,
    VIDC_ERR_BADPARAM,
    VIDC_ERR_BUFFLAG_DATACORRUPT,
    VIDC_ERR_ENC_BITMISMATCH,
    VIDC_ERR_FILE_ZERO_LEN,
    VIDC_ERR_CRC_READ_FAIL,
    VIDC_ERR_DISPLAY_REORDER,
    VIDC_ERR_PROCESS_EXTRADATA,
    VIDC_ERR_NOT_UNSUPPORTED,
    VIDC_ERR_CFG_NOT_SUPPORTED,
    VIDC_ERR_SMSTR_INSUFF_RESOURCE,
    VIDC_ERR_MAX_SEEK,
    VIDC_ERR_SEEK_NO_SYNC_FOUND,
    VIDC_ERR_RESOLUTION_CHANGE,
    VIDC_ERR_DISPLAY_REORDER_DUP_CRC,
    VIDC_ERR_MARKDATA,
    VIDC_ERR_MBIMISMATCH,
    VIDC_ERR_BITEXTACT_MISMATCH,
    VIDC_ERR_EXTRADATA_MISMATCH,
    VIDC_ERR_MULTI_SLICE_RETURNED,
    VIDC_ERR_EXDATA_REF_CONSUME,
    VIDC_ERR_MISC,  //Random Error
    VIDC_ERR_MISRMISMATCH,
    VIDC_ERR_MAX
}VideoSessionError;

typedef struct VideoMisrInfo {
    uint32 nMisrDPBLuma;
    uint32 nMisrDPBChroma;
    uint32 nMisrOPBLuma;
    uint32 nMisrOPBChroma;
} VideoMisrInfo;

typedef struct VideoMisrPayload {
    uint32 nFrameNum;
    uint32 nWidth;
    uint32 nHeight;
    uint32 nCropWidth;
    uint32 nCropHeight;
    uint32 nCropX;
    uint32 nCropY;
    uint32 nBitDepthY;
    uint32 nBitDepthC;
    VideoMisrInfo misrInfo[2];
} VideoMisrPayload;


typedef struct VideoResolution
{
    uint32 nWidth;
    uint32 nHeight;
    uint32 nCropWidth;
    uint32 nCropHeight;
    uint32 nScanlines;
    uint32 nStride;
    uint32 nStartX; // left start x coordinate
    uint32 nStartY; // top start y coordinate
    uint32 nColorFmt;
    uint32 nInterlaceFormat;
    VideoMisrPayload misrPayload;
    VideoMisrInfo misrInfo[2];
}VideoResolution;

#ifdef __cplusplus
}
#endif

#endif //_VIDEOCOMDEF_H
