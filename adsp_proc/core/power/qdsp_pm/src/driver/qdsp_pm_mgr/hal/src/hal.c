/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "hal_interrupt.h"
#include "hal_hwio_clkctrl.h"
#include "hal_clkrgm.h"
#include "hal_busdrv.h"
#include "hal_sleep.h"


extern uint32 gADSPPMINITSTATE;


Adsppm_Status Hal_Init(void)
{
    Adsppm_Status result;
    ADSPPM_LOG_FUNC_PROFILESTART;
    result = Intr_Init();
    if(Adsppm_Status_Success == result)
    {
        gADSPPMINITSTATE |= Adspmm_State_HalIntrInit;
        result = HalHwIo_Init();
        if(Adsppm_Status_Success == result)
        {
            gADSPPMINITSTATE |= Adsppm_State_HalHwIoInit;
            result = ClkRgm_Init();
            if(Adsppm_Status_Success == result)
            {
                gADSPPMINITSTATE |= Adsppm_State_HalClkRgmInit;
                result = Bus_Init();
                if(Adsppm_Status_Success == result)
                {
                    gADSPPMINITSTATE |= Adsppm_State_HalBusInit;
                    result = Slp_Init(NULL, NULL);
                    if(Adsppm_Status_Success == result)
                    {
                        gADSPPMINITSTATE |= Adsppm_State_HalSlpInit;
                    }
                }
            }
        }
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

