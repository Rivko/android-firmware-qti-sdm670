/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/
 
/*
 * hal_hwio_clkctrl.c - Implementation for direct CGC control of ULTAudio clock branches
 *
 *  Created on: Dec 16, 2013
 *      Author: yrusakov
 */

#include "hal_hwio_clkctrl.h"
#include "msmhwiobase.h"
#include "adsppm.h"
#include "adsppm_utils.h"
#include "hal_clkrgm.h"
#include "asic.h"
#include "hal_hwio_clkctrl.h"
#include "HALhwio.h"
#include "DALSys.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIHWIO.h"
#include "CoreVerify.h"
#include "stdlib.h"


#define CGC_CLK_ON(mask) mask


typedef struct
{
    DALSYSSyncHandle lock;
    uint32 baseAddr; //!< Base address for the CSR address space
} Hal_HwIoCtxType;

static Hal_HwIoCtxType gHwIoClkCtx;


Adsppm_Status halHwIo_EnableCgcClock(const AsicClkDescriptorType *pClkDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;

    uint32 regValue = in_dword(gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    out_dword_masked_ns(
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset,
        pClkDesc->clkInfo.hwioInfo.enableMask,
        pClkDesc->clkInfo.hwioInfo.enableMask,
        regValue);

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Writing %X (mask %X) into addr %X",
        pClkDesc->clkInfo.hwioInfo.enableMask,
        pClkDesc->clkInfo.hwioInfo.enableMask,
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    if(pClkDesc->clkInfo.hwioInfo.stateMask != 0)
    {
        // Wait for clock enable status
        ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
            "Waiting for %X (mask %X) from addr %X",
            CGC_CLK_ON(pClkDesc->clkInfo.hwioInfo.stateMask),
            pClkDesc->clkInfo.hwioInfo.stateMask,
            gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);
        while(
            in_dword_masked(
                gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset,
                pClkDesc->clkInfo.hwioInfo.stateMask)
            != CGC_CLK_ON(pClkDesc->clkInfo.hwioInfo.stateMask));
    }

    return result;
}


Adsppm_Status halHwIo_DisableCgcClock(const AsicClkDescriptorType* pClkDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;

    uint32 regValue = in_dword(gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    out_dword_masked_ns(
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset,
        pClkDesc->clkInfo.hwioInfo.enableMask, 0, regValue);

    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Writing 0 (mask %X) into addr %X",
        pClkDesc->clkInfo.hwioInfo.enableMask,
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    return result;
}


Adsppm_Status halHwIo_EnableCgcHwCtl(const AsicClkDescriptorType *pClkDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;

    uint32 regValue = in_dword(gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    out_dword_masked_ns(
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset,
        pClkDesc->clkInfo.hwioInfo.hwctlMask,
        pClkDesc->clkInfo.hwioInfo.hwctlMask,
        regValue);

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Writing %X (mask %X) into addr %X",
        pClkDesc->clkInfo.hwioInfo.hwctlMask,
        pClkDesc->clkInfo.hwioInfo.hwctlMask,
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    return result;
}


Adsppm_Status halHwIo_DisableCgcHwCtl(const AsicClkDescriptorType *pClkDesc)
{
    Adsppm_Status result = Adsppm_Status_Success;

    uint32 regValue = in_dword(gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    out_dword_masked_ns(
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset,
        pClkDesc->clkInfo.hwioInfo.hwctlMask, 0, regValue);

    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
        "Writing 0 (mask %X) into addr %X",
        pClkDesc->clkInfo.hwioInfo.hwctlMask,
        gHwIoClkCtx.baseAddr + pClkDesc->clkInfo.hwioInfo.regOffset);

    return result;
}


Adsppm_Status HalHwIo_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    // Create mutex
    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE, (DALSYSSyncHandle *) &(gHwIoClkCtx.lock), NULL))
    {
        result = Adsppm_Status_NotInitialized;
    }
    else
    {
        uint32 paBase = ACM_GetLpassRegRange()->physAddr;
        if(paBase != 0)
        {
            // Map HWIO base and store vitual address
            DalDeviceHandle *hHwio;
            CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_HWIO, &hHwio));
            if(DAL_SUCCESS !=DalHWIO_MapRegionByAddress(
                hHwio, (uint8 *)paBase, (uint8 **) &gHwIoClkCtx.baseAddr))
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot map HWIO base");
                result = Adsppm_Status_NoMemory;
            }
            CORE_DAL_VERIFY(DAL_DeviceDetach(hHwio));
        }
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status HalHwIo_EnableClock(AdsppmClkIdType clkId)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gHwIoClkCtx.lock)
        {
            if(gHwIoClkCtx.baseAddr != 0)
            {
                const AsicClkDescriptorType *pClkDesc = ACMClk_GetClockDescriptorById(clkId);
                if(NULL != pClkDesc)
                {
                    // Lock HW layer
                    adsppm_lock(gHwIoClkCtx.lock);

                    switch(pClkDesc->clkType)
                    {
                    // Only work with internal clocks
                    case AsicClk_TypeInternalCGC:
                        result = halHwIo_EnableCgcClock(pClkDesc);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Enabled Clk Id:%u", clkId);
                        break;
                    default:
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Cannot enable non-HWIO Clk Id:%u by register writes", clkId);
                        result = Adsppm_Status_ResourceNotFound;
                        break;
                    }

                    // Unlock HW layer
                    adsppm_unlock(gHwIoClkCtx.lock);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "invalid Clock Descriptor");
                    result = Adsppm_Status_BadParm;
                }
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot perform HWIO on this chip (base addr == 0)");
                result = Adsppm_Status_UnSupported;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status HalHwIo_DisableClock(AdsppmClkIdType clkId)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gHwIoClkCtx.lock)
        {
            if(gHwIoClkCtx.baseAddr != 0)
            {
                const AsicClkDescriptorType *pClkDesc = ACMClk_GetClockDescriptorById(clkId);
                if(NULL != pClkDesc)
                {
                    // Lock HW layer
                    adsppm_lock(gHwIoClkCtx.lock);

                    switch(pClkDesc->clkType)
                    {
                    // Only work with internal clocks
                    case AsicClk_TypeInternalCGC:
                        result = halHwIo_DisableCgcClock(pClkDesc);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Disabled Clk Id:%u", clkId);
                        break;
                    default:
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Cannot disable non-HWIO Clk Id:%u by register writes", clkId);
                        result = Adsppm_Status_ResourceNotFound;
                        break;
                    }

                    // Unlock HW layer
                    adsppm_unlock(gHwIoClkCtx.lock);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "invalid Clock Descriptor");
                    result = Adsppm_Status_BadParm;
                }
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot perform HWIO on this chip (base addr == 0)");
                result = Adsppm_Status_UnSupported;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * @fn HalHwIo_EnableHWClockCntl - enables/disables HW clock control for the specified clock
 * Function will return success if the clock doesn't support DCG
 * @param clkId - clock ID
 * @param state - specifies whether or not HW control should be enabled
 * @return completion status
 */
Adsppm_Status HalHwIo_EnableHWClockCntl(AdsppmClkIdType clkId)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gHwIoClkCtx.lock)
        {
            if(gHwIoClkCtx.baseAddr != 0)
            {
                const AsicClkDescriptorType *pClkDesc = ACMClk_GetClockDescriptorById(clkId);
                if(NULL != pClkDesc)
                {
                    // Lock HW layer
                    adsppm_lock(gHwIoClkCtx.lock);

                    switch(pClkDesc->clkType)
                    {
                    // Only work with internal clocks
                    case AsicClk_TypeInternalCGC:
                        result = halHwIo_EnableCgcHwCtl(pClkDesc);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Enabled HW control for Clk Id:%u", clkId);

                        break;
                    default:
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Cannot control non-HWIO Clk Id:%u by register writes", clkId);
                        result = Adsppm_Status_ResourceNotFound;
                        break;
                    }

                    // Unlock HW layer
                    adsppm_unlock(gHwIoClkCtx.lock);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "invalid Clock Descriptor");
                    result = Adsppm_Status_BadParm;
                }
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot perform HWIO on this chip (base addr == 0)");
                result = Adsppm_Status_UnSupported;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * @fn HalHwIo_EnableHWClockCntl - enables/disables HW clock control for the specified clock
 * Function will return success if the clock doesn't support DCG
 * @param clkId - clock ID
 * @param state - specifies whether or not HW control should be enabled
 * @return completion status
 */
Adsppm_Status HalHwIo_DisableHWClockCntl(AdsppmClkIdType clkId)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;

    if((AdsppmClk_None < clkId) && (AdsppmClk_EnumMax > clkId))
    {
        if(NULL != gHwIoClkCtx.lock)
        {
            if(gHwIoClkCtx.baseAddr != 0)
            {
                const AsicClkDescriptorType *pClkDesc = ACMClk_GetClockDescriptorById(clkId);
                if(NULL != pClkDesc)
                {
                    // Lock HW layer
                    adsppm_lock(gHwIoClkCtx.lock);

                    switch(pClkDesc->clkType)
                    {
                    // Only work with internal clocks
                    case AsicClk_TypeInternalCGC:
                        result = halHwIo_DisableCgcHwCtl(pClkDesc);
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
                            "Disabled HW control for Clk Id:%u", clkId);

                        break;
                    default:
                        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                            "Cannot control non-HWIO Clk Id:%u by register writes", clkId);
                        result = Adsppm_Status_ResourceNotFound;
                        break;
                    }

                    // Unlock HW layer
                    adsppm_unlock(gHwIoClkCtx.lock);
                }
                else
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                        "invalid Clock Descriptor");
                    result = Adsppm_Status_BadParm;
                }
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                    "Cannot perform HWIO on this chip (base addr == 0)");
                result = Adsppm_Status_UnSupported;
            }
        }
        else
        {
            result = Adsppm_Status_NotInitialized;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "AdsppmClkId out of range");
        result = Adsppm_Status_BadParm;
    }

    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

