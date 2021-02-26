/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef VQZIP_H_
#define VQZIP_H_

#include "sys/types.h"
#include <OMX_IVCommon.h>

#define VQZIP_MAX_STAT_COUNT 16

typedef struct VQZipConfig
{
    uint32_t colorFormat;
    void*    pSEIPayload; // pointer to SEI payload
    uint16_t nWidth;
    uint16_t nHeight;
}VQZipConfig;

typedef struct VQZipStats
{
    uint32_t  nCount;
    uint32_t  stats[VQZIP_MAX_STAT_COUNT];
}VQZipStats;


typedef enum VQZipStatus
{
    VQZipStatusSuccess = 0,
    VQZipStatusFail = 1,
    VQZipStatusMax = 0xFFFFFFFF
}VQZipStatus;

/*!
   @detail  Creates a VQZip Handle.

   @param[in]  none

   @return    Pointer to VQZip instance.

   @note    It is expected that except pOputStream nothing should be NULL.
*/
void* VQZipInit();

/*!
   @detail  Calculates the stats on 'pBuf' buffer for 'hVz' VQZip instance based on
            input VQZip configuration 'pConfig'.

   @param[in]  hVz  - handle to VQZip Instance
   @param[in]  pBuf - pointer to input YUV buffer, on which YUV stats need to be calculated
   @param[in]  pConfig - pointer to VQZipConfig structure, should be valid for first frame
   @param[out] pStats  - pointer to VQZipStats structure

   @return    Pointer to VQZip instance.

   @note    It is expected that except pOputStream nothing should be NULL.
*/
VQZipStatus VQZipComputeStats(void* const hVz, const void * const pBuf, const VQZipConfig *pConfig, VQZipStats *pStats);

/*!
   @detail  Destroys VQZip Handle and clears internal memory.

   @param[in]  hVz  - handle to VQZip Instance

   @return    none

   @note    It is expected that except pOputStream nothing should be NULL.
*/
void VQZipDeInit(void* hVQ);

#endif //VQZIP_H_
