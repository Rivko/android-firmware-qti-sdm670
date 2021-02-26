/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * main.c
 *
 *  Created on: Feb 10, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "hal.h"
#include "core.h"
#include "asic.h"
#include "api.h"
#include "npa.h"



typedef struct
{
    ULogHandle m_hLog;
    uint32     m_log_buffer_size;
    uint32     initialized;
} AdsppmGlbCtxType;

/**
 * @const busPortName - defines Bus ports name to be used by ADSPPM ULOG internally
 *                      keep updated with AdsppmBusPortIdType in adsppm.h
 */
const char *AdsppmBusPortName[AdsppmBusPort_EnumMax] =
{
    "None",
    "Adsp_Master",
    "Dml_Master",
    "Aif_Master",
    "Slimbus_Master",
    "Slimbus2_Master",
    "Midi_Master",
    "HwRsmp_Master",
    "Ext_Ahb_Master",
    "Spdif_Master",
    "Hdmirx_Master",
    "Hdmitx_Master",
    "Sif_Master",
    "DPAudio_Master",
    "Vapss_Hcp_Master",
    "Vapss_Dma_Master",
    "Sdc_Master",
    "Qup_Master",
    "Dml_Slave",
    "Aif_Slave",
    "Slimbus_Slave",
    "Slimbus2_Slave",
    "Midi_Slave",
    "HwRsmp_Slave",
    "AvSync_Slave",
    "Lpm_Slave",
    "Sram_Slave",
    "Ext_Ahb_Slave",
    "Ddr_Slave",
    "Ocmem_Slave",
    "PerifNoc_Slave",
    "Snoc_Slave",
    "Spdif_Slave",
    "Hdmirx_Slave",
    "Hdmitx_Slave",
    "Sif_Slave",
    "Bstc_Slave",
    "Dcodec_Slave",
    "DPAudio_Slave",	
    "Cdsp_Tcm_Slave",
    "Slpi_Sram_Slave",
    "Adsp_Slave",
    "Core",
	"Snoc_Master",
    "Snoc_Ahb_Master",

};

static AdsppmGlbCtxType gAdsppmCtx;

uint32 gADSPPMINITSTATE = 0;

#ifdef CDSP_HVX_LPR
   extern npa_node_definition g_CdspHvxNode;
#endif
ULogHandle GetUlogHandle(void)
{
    return gAdsppmCtx.m_hLog;
}


/**
 * @fn adsppm_lock
 * @brief Lock the critical section using the dalsys API
 * @param DALSYSSyncHandle lock
 */
void adsppm_lock(DALSYSSyncHandle lock)
{
    DALSYS_SyncEnter(lock);
}


/**
 * @fn adsppm_unlock
 * @brief Unlock the critical section using the dalsys API
 * @param DALSYSSyncHandle lock
 */
void adsppm_unlock(DALSYSSyncHandle lock)
{
    DALSYS_SyncLeave(lock);
}


Adsppm_Status ADSPPM_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;

    // Initialize ADSPPM ULOG
    gAdsppmCtx.m_log_buffer_size = ADSPPM_ULOG_BUFFER_SIZE;
    gAdsppmCtx.initialized = 0;
    result = ULogFront_RealTimeInit(
        &gAdsppmCtx.m_hLog,
        "ADSPPM Log",
        gAdsppmCtx.m_log_buffer_size,
        ULOG_MEMORY_LOCAL,
        ULOG_LOCK_OS);

    if(DAL_SUCCESS == result)
    {
        // Create ASIC Config Manager
        result = ACM_Init();

        // Create OS abstraction components
        if(Adsppm_Status_Success == result)
        {
            gADSPPMINITSTATE |= Adsppm_State_ACMInit;

            
#ifdef  CDSP_HVX_LPR  
                npa_resource_state intial_state = 0;    
                npa_define_node(&g_CdspHvxNode, &intial_state, NULL); 
#endif 

            // Create all CoreBSP wrapper components
            result = Hal_Init();

            if(Adsppm_Status_Success == result)
            {
                // Create ADSPPM core
                result = Core_Init();

                if(Adsppm_Status_Success == result)
                {
                    // Create API components
                    result = API_Init();
                }
            }
        }
    }
    else
    {
        result = Adsppm_Status_CoreResourceNotAvailable;
    }
    if(Adsppm_Status_Success == result)
    {
        gAdsppmCtx.initialized = 1;
    }
   
    return result;
}


uint32 ADSPPM_IsInitialized(void)
{
    return gAdsppmCtx.initialized;
}


void ADSPPM_EnterSleep(void)
{
    /*Adsppm_Status result = Core_EnterSleep();
    if(Adsppm_Status_Success != result)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed with code %u", result);
    }*/
}


void ADSPPM_ExitSleep(void)
{
    /*Adsppm_Status result = Core_ExitSleep();
    if(Adsppm_Status_Success != result)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Failed with code %u", result);
    }*/
}


const char *adsppm_getBusPortName(AdsppmBusPortIdType busPort)
{
    if(AdsppmBusPort_EnumMax > busPort)
    {
        return AdsppmBusPortName[busPort];
    }
    return AdsppmBusPortName[AdsppmBusPort_None];
}

