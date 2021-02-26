////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxeepromdriverapi.h"
#include "camxtypes.h"
#include <android/log.h>
// NOWHINE ENTIRE FILE
#define IMX577_DBG(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "IMX577",fmt, ##args)
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

UINT32 g_shadingData = 29;
UINT32 g_shadingTotalSize = (13 * 17 * 2 * 4 * 3 + 6);
UINT32 g_shading = (1024 * 1024);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// format_lensshading
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID format_lensshading(
    UINT8*            otpdata,
    UINT32            otpdatasize,
    LSCCalibration*   pLSCData)
{
    UINT16* pMptr = 0;
    UINT16 tempdata = 0;
    UINT16 changedata = 0;
    UINT32 i = 0;

    IMX577_DBG("QLOG:1. otpdatasize = %d, pLSCData= %p", otpdatasize, pLSCData);
    otpdata += g_shadingData;
    pMptr = reinterpret_cast<UINT16*>(otpdata);
    IMX577_DBG("1. pmptr = %p", pMptr);

    for (i = 0; i<g_shadingTotalSize; i += 2, pMptr++)
    {
        if (pMptr != NULL)
        {
            IMX577_DBG("1. pmptr = %p, *pMptr=%u", pMptr, *pMptr);
            tempdata = (*pMptr);
            if (tempdata != 0)
            {
                changedata = (g_shading / tempdata);
            }
            *pMptr = changedata;
        }
    }

   IMX577_DBG("2 . pmptr = %p", pMptr);
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GetEEPROMLibraryAPIs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID GetEEPROMLibraryAPIs(
    EEPROMLibraryAPI* pEEPROMLibraryAPI)
{
    pEEPROMLibraryAPI->majorVersion       = 1;
    pEEPROMLibraryAPI->minorVersion       = 0;
    pEEPROMLibraryAPI->pFormatLSCSettings = format_lensshading;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
