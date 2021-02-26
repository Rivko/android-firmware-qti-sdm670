/*-------------------------------------------------------------------
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
--------------------------------------------------------------------*/

#ifndef _VTEST_XML_COMDEF_H
#define _VTEST_XML_COMDEF_H

#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "OMX_Core.h"
#include "vtest_ComDef.h"
#include <libxml/tree.h>

namespace vtest {

//Maximum string length
#define MAX_STR_LEN 300
#define MAX_NUM_SESSIONS 10
#define MAX_FILENAME_LENGTH 128
#define MAX_TAG_LEN 100


// String Macros
#define SPRINTF(fmt,...) snprintf(fmt,sizeof(fmt),##__VA_ARGS__)
#define SNPRINTF(fmt,size,...) snprintf(fmt,size,##__VA_ARGS__)
#define STRCPY(dest,src) strlcpy(dest,src,sizeof(dest))
#define STRLCPY(dest,src,size) ((dest)?strlcpy(dest,src,size):0)
#define STRCAT(dest,src) strlcat(dest,src,sizeof(dest))
#define STRLCAT(dest,src,size) strlcat(dest,src,size)
#define STRCMP(str1,str2) ((str1 && str2)? strcmp(str1,str2) : 1)

typedef struct VideoStaticProperties
{
    char sCRCRoot[MAX_STR_LEN];
    char sMISRRoot[MAX_STR_LEN];
    char sInputRoot[MAX_STR_LEN];
    char sOutRoot[MAX_STR_LEN];
    char sPqRoot[MAX_STR_LEN];
    char sEncFlowXml[MAX_STR_LEN];
    char sDecFlowXml[MAX_STR_LEN];
    OMX_U32 logMask;
    OMX_U32 inputDump;
    OMX_U32 outputDump;
    OMX_U32 nDisplayBufCnt;
    OMX_U32 bDynamicBufMode;
    OMX_U32 bSinkType;
    OMX_U32 nDefaultHeight;
    OMX_U32 nDefaultWidth;
    OMX_U32 nMinFrameDelay;
    OMX_U32 nErrorDelay; // Error delay milli seconds
    OMX_U32 nPollTimeout;  // In mill seconds
    OMX_U32 nThreadTimeout;// In mill seconds
    OMX_U32 bGenerateCrc;
    OMX_U32 bGenerateMisr;
    OMX_U32 bSkipCrcCheck;
    OMX_U32 nDecOutPortBufCnt;
    OMX_U32 nEncCapPortBufCnt;
    OMX_U32 nEncOutPortBufCnt;
    OMX_U32 nOffsetDump;
    OMX_U32 bExtraDataDump;
    OMX_U32 nFwLog;
    OMX_U32 bIsConcurrency;
    FILE *fResult;
}VideoStaticProperties;

typedef struct VideoSessionInfo
{
    char SessionType[MAX_STR_LEN];
    char TestId[MAX_STR_LEN];
    xmlNodePtr pXmlNode;
    OMX_BOOL bActiveSession;

} VideoSessionInfo;

}
extern vtest::VideoStaticProperties sGlobalStaticVideoProp;

#endif // #ifndef _VTEST_XML_COMDEF_H
