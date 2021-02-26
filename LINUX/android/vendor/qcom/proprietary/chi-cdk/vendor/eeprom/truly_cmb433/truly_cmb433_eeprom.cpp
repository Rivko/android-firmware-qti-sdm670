////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "camxeepromdriverapi.h"
// NOWHINE ENTIRE FILE

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define BASE_ADDR              0x7010
#define WB_FLAG_ADDR_R2A       0x7016
#define LENS_FLAG_ADDR_R2A     0x7028

#define WB_OFFSET_R2A          (WB_FLAG_ADDR_R2A-BASE_ADDR)
#define WB_GROUP_SIZE_R2A      8
#define INFO_GROUP_SIZE_R2A    8

#define LSC_OFFSET_R2A         (LENS_FLAG_ADDR_R2A-BASE_ADDR)
#define LSC_GROUP_SIZE_R2A     241

#define LSC_REG_SIZE_R2A       240
#define AWB_REG_SIZE_R2A       3

#define RG_RATIO_TYPICAL_VALUE 0x13E
#define BG_RATIO_TYPICAL_VALUE 0x140

#define ABS(x)                 (((x) < 0) ? -(x) : (x))

static RegisterSetting         wbRegSetting[6];
static RegisterSetting         lscRegSetting[LSC_REG_SIZE_R2A + 1];
static RegisterData            wbRegData[6];
static RegisterData            lscRegData[LSC_REG_SIZE_R2A + 1];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// truly_cmb433_get_group_index
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static INT32 truly_cmb433_get_group_index(UINT8 flag)
{
    INT8 group_index = -1;

    flag = flag & 0xFC;
    if ((flag & 0xC0) == 0x40)
    {
        group_index = 0;
    }
    else if ((flag & 0x30) == 0x10)
    {
        group_index = 1;
    }
    else
    {
        group_index = -1;
    }

    return group_index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// format_wbdata
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID format_wbdata(
    UINT8*            otpdata,
    UINT32            otpdatasize,
    WBCalibration*    pWBData)
{
    UINT8  flag;
    UINT8  temp;
    UINT8  reg_val;
    INT32  addr_offset = -1;
    INT32  group_index  = -1;
    INT32  group_offset = WB_GROUP_SIZE_R2A;
    UINT16 R_gain, G_gain, B_gain;
    UINT16 G_gain_R, G_gain_B;
    UINT16 nR_G_gain, nB_G_gain, nG_G_gain;
    UINT16 nBase_gain;
    RegisterData* pRegData = NULL;

    flag = (UINT8)(otpdata[0]);

    group_index = truly_cmb433_get_group_index(flag);
    if (group_index == -1)
    {
        return;
    }

    addr_offset = WB_OFFSET_R2A + group_index * WB_GROUP_SIZE_R2A;
    if ((UINT32)(addr_offset + 2) > (otpdatasize-1))
    {
        return;
    }

    temp            = otpdata[addr_offset + 2];
    reg_val         = otpdata[addr_offset];
    pWBData->rOverG = (reg_val<<2) + ((temp>>6)&0x03);
    reg_val         = otpdata[addr_offset + 1];
    pWBData->bOverG = (reg_val<<2) + ((temp>>4)&0x03);

    nR_G_gain = (RG_RATIO_TYPICAL_VALUE*1000) / pWBData->rOverG;
    nB_G_gain = (BG_RATIO_TYPICAL_VALUE*1000) / pWBData->bOverG;
    nG_G_gain = 1000;

    if (nR_G_gain < 1000 || nB_G_gain < 1000)
    {
        if (nR_G_gain < nB_G_gain)
            nBase_gain = nR_G_gain;
        else
            nBase_gain = nB_G_gain;
    }
    else
    {
        nBase_gain = nG_G_gain;
    }

    R_gain = 0x400 * nR_G_gain / (nBase_gain);
    B_gain = 0x400 * nB_G_gain / (nBase_gain);
    G_gain = 0x400 * nG_G_gain / (nBase_gain);

    pWBData->settings.regSetting = wbRegSetting;
    pRegData                     = wbRegData;

    if (NULL != pWBData->settings.regSetting)
    {
        UINT32 index = 0;
        if ((R_gain >= 0x400) && (NULL != &pRegData[index]))
        {
            pWBData->settings.regSetting[index].registerAddr       = 0x5019;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = G_gain >> 8;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;

            pWBData->settings.regSetting[index].registerAddr       = 0x501A;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = R_gain & 0x00ff;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;
        }

        if ((G_gain >= 0x400) && (NULL != &pRegData[index]))
        {
            pWBData->settings.regSetting[index].registerAddr       = 0x501B;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = G_gain >> 8;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;

            pWBData->settings.regSetting[index].registerAddr       = 0x501C;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = G_gain & 0x00ff;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;
        }

        if ((B_gain >= 0x400) && (NULL != &pRegData[index]))
        {
            pWBData->settings.regSetting[index].registerAddr       = 0x501D;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = B_gain >> 8;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;

            pWBData->settings.regSetting[index].registerAddr       = 0x501E;
            pWBData->settings.regSetting[index].registerData       = &pRegData[index];
            pWBData->settings.regSetting[index].registerData[0]    = B_gain & 0x00ff;
            pWBData->settings.regSetting[index].regAddrType        = 2;
            pWBData->settings.regSetting[index].regDataType        = 1;
            pWBData->settings.regSetting[index].operation          = OperationType::WRITE;
            pWBData->settings.regSetting[index].registerDataCount  = 1;
            index++;
        }

        pWBData->settings.regSettingCount = index;
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// format_lensshading
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VOID format_lensshading(
    UINT8*            otpdata,
    UINT32            otpdatasize,
    LSCCalibration*   pLSCData)
{
    INT32 addr_offset = -1;
    INT32 group_index  = -1;
    INT32 j = 0;
    UINT8 flag;
    INT32 temp;
    RegisterData* pRegData = NULL;

    flag = otpdata[LSC_OFFSET_R2A];

    group_index = truly_cmb433_get_group_index(flag);
    if (group_index == -1)
    {
      return;
    }

    pLSCData->settings.regSettingCount  = LSC_GROUP_SIZE_R2A;
    pLSCData->settings.regSetting       = lscRegSetting;
    pRegData                            = lscRegData;

    if (NULL != pLSCData->settings.regSetting)
    {
        UINT32 index = 0;
        temp         = (otpdata[otpdatasize-1]);
        temp         = 0x20 | temp;

        pLSCData->settings.regSetting[index].registerAddr       = 0x5000;
        pLSCData->settings.regSetting[index].registerData       = &pRegData[0];
        pLSCData->settings.regSetting[index].registerData[0]    = temp;
        pLSCData->settings.regSetting[index].regAddrType        = 2;
        pLSCData->settings.regSetting[index].regDataType        = 1;
        pLSCData->settings.regSetting[index].operation          = OperationType::WRITE;
        pLSCData->settings.regSetting[index].registerDataCount  = 1;
        index++;

        addr_offset = LSC_OFFSET_R2A + group_index * LSC_GROUP_SIZE_R2A;

        for (j = 0; j < LSC_REG_SIZE_R2A; j++)
        {
            pLSCData->settings.regSetting[index].registerAddr = 0x5900 + j;
            pLSCData->settings.regSetting[index].registerData       = &pRegData[j+1];
            pLSCData->settings.regSetting[index].registerData[0]    = otpdata[addr_offset + j + 1];
            pLSCData->settings.regSetting[index].regAddrType        = 2;
            pLSCData->settings.regSetting[index].regDataType        = 1;
            pLSCData->settings.regSetting[index].operation          = OperationType::WRITE;
            pLSCData->settings.regSetting[index].registerDataCount  = 1;
            index++;
        }
    }

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
    pEEPROMLibraryAPI->pFormatWBSettings  = format_wbdata;
}

#ifdef __cplusplus
} // CamX Namespace
#endif // __cplusplus
