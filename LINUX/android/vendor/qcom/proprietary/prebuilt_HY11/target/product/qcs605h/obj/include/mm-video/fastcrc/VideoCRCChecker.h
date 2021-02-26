/*-------------------------------------------------------------------
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#ifndef _VIDEOCRCCHECKER_H
#define _VIDEOCRCCHECKER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <VideoComDef.h>

#define TWICE_MAX_STR_LEN (2 * MAX_STR_LEN)

typedef struct VideoCRCChecker VideoCRCChecker;

#define IS_VIDEO_SWCRC(pCRC) ((pCRC->crcMode == VIDEO_CRCMODE_SW)?TRUE:FALSE)

typedef struct VideoSWCRCInfo
{
    uint32 crc;
    uint32 crcPolynomial;
    uint32 m_CRCTable[256];
    uint32 prevCrc;
}VideoSWCRCInfo;

typedef struct VideoHWCRCInfo
{
    VideoMisrPayload prevCrc;
}VideoHWCRCInfo;

typedef enum VideoCRCFileInfoType
{
    VIDEO_CRCFILEINFO_EXT,
    VIDEO_CRCFILEINFO_TAG
}VideoCRCFileInfoType;

typedef enum VideoCRCMode
{
    VIDEO_CRCMODE_SW,
    VIDEO_CRCMODE_HW,
    VIDEO_CRCMODE_UNKNOWN
}VideoCRCMode;

struct VideoCRCChecker
{
    char sCrcFileName[MAX_STR_LEN];
    FILE* fCRCStream;
    VideoCRCMode crcMode;
    char *crcModeName;
    union
    {
        VideoSWCRCInfo swCRC;
        VideoHWCRCInfo hwCRC;
    }crcInfo;
    uint32 m_fCRCCounter;
    uint32 m_nTotalCRCCount;
    uint32 m_bIsDivxPacked;
    BOOL  m_bCheckFile; //=1, Check each CRC calculated in entire file
    BOOL m_bIsGenerate; //=1, Generates crc file on output root path with inputclipname.fcrc
    uint32 prevCrcCount;
    VidcStatus(*Configure)(VideoCRCChecker* pCRC, char* pRefPath);
    void(*Destroy)(VideoCRCChecker* pCRC);
    void(*SetCRCCheckFileMode)(VideoCRCChecker* pCRC, BOOL bFileMode);
    uint32(*GetPendingCRCCount)(VideoCRCChecker* pCRC); //Returns still how many pending CRC's left
    VidcStatus(*CheckCRCofYUVframe)(VideoCRCChecker* pCRC, uint8 * pData, VideoResolution *pRes,
        BOOL bInterleaved, BOOL* pbCRCResult);
    void(*SkipFrameCRCCheck)(VideoCRCChecker* pCRC);
    uint32(*TotalCRCCount)(VideoCRCChecker* pCRC);
    uint32(*GetPrevCRC)(VideoCRCChecker* pCRC);
};

VidcStatus VideoCRCCheckerInit
(
    VideoCRCChecker* pCRC,
    BOOL isGenerate,
    VideoCRCMode crcMode
);
VidcStatus VideoCRCGetCRCFileInfo
(
    VideoCRCChecker* pCRC,
    string *crcFileInfo, uint32 crcFileInfoType
);

#ifdef __cplusplus
}
#endif

#endif //_VIDEOCRCCHECKER_H
