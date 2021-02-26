/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_clkrgm.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_CLKRGM_H_
#define HAL_CLKRGM_H_

#include "adsppm.h"
#include "asic.h"


typedef enum
{
    Hal_ClkRgmDisable = 0,
    Hal_ClkRgmEnable
} Hal_ClkRgmEnableDisableType;

typedef enum
{
    Hal_ClkRgmMemCollapse = 0,
    Hal_ClkRgmMemRetain
} Hal_ClkRgmMemStateType;

typedef enum
{
    Hal_ClkRgmClkSrc_Primary = 1,
    Hal_ClkRgmClkSrc_Secondary = 2,
    Hal_ClkRgmClkSrc_Ternery = 3,
} Hal_ClkRgmClkSrcType;


Adsppm_Status ClkRgm_Init(void);

Adsppm_Status ClkRgm_SetClock(AdsppmClkIdType clk, uint32 freq);

Adsppm_Status ClkRgm_SetMaxClock(AdsppmClkIdType clk, uint32 freq);

Adsppm_Status ClkRgm_EnableClock(AdsppmClkIdType clk, Hal_ClkRgmEnableDisableType state);

Adsppm_Status ClkRgm_RequestPower(AsicPowerDomainType blk, Hal_ClkRgmEnableDisableType state);

Adsppm_Status ClkRgm_MemoryControl(AdsppmMemIdType memCore, Hal_ClkRgmMemStateType pwrState);

Adsppm_Status ClkRgm_GetClockFrequency(AdsppmClkIdType clk, uint32 *pFreq);

Adsppm_Status ClkRgm_CalcClockFrequency(AdsppmClkIdType clk, uint32 *pFreq);

Adsppm_Status ClkRgm_SelectClkSource(AdsppmClkIdType clk, Hal_ClkRgmClkSrcType clkSrc);


#endif /* HAL_CLKRGM_H_ */

