/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * @file hal_interrupt.c
 * This is a wrapper on top of DALInterruptController functionality.
 *
 *  Created on: Jul 4, 2012
 *      Author: yrusakov
 */

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "pwrmgr.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIIPCInt.h"


typedef struct
{
    DALSYSSyncHandle lock;
    DalDeviceHandle  *hIntrInDev; //Handle for input interrupt device
    DalDeviceHandle  *hIntrOutDev; //Handle for outgoing interrupt device
} IntMgrContextType;

// Global context for the power manager
static IntMgrContextType gIntMgrCtx;


Adsppm_Status Intr_Init(void)
{
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &gIntMgrCtx.lock, NULL))
    {
        result = Adsppm_Status_Failed;
    }
    if((Adsppm_Status_Success == result) &&
        ((DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &gIntMgrCtx.hIntrInDev)) ||
        (NULL == gIntMgrCtx.hIntrInDev)))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error attaching to the interrupt controller");
        result = Adsppm_Status_Failed;
    }

    // Create DAL Interrupt Handler
    if((Adsppm_Status_Success == result) &&
        ((DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_IPCINT, &gIntMgrCtx.hIntrOutDev)) ||
        (NULL == gIntMgrCtx.hIntrOutDev)))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error attaching to the IPC interrupt controller");
        result = Adsppm_Status_Failed;
    }

    if((Adsppm_Status_Success == result) &&
        (DAL_SUCCESS != (result = DalDevice_Open(gIntMgrCtx.hIntrInDev, DAL_OPEN_SHARED))))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error Device Open");
        result = Adsppm_Status_Failed;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status Intr_RegisterIsr(uint32 irq, void* (*pIsr)(void*), void* pCtx)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    uint32 intTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    adsppm_lock(gIntMgrCtx.lock);
    // Register the Q6_L1_AVS_SW_DONE_IRQ  interrupt isr routine
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Registering ISR for irq %d", irq);
    if(DAL_SUCCESS != DalInterruptController_RegisterISR(
        gIntMgrCtx.hIntrInDev,
        irq,
        pIsr,
        pCtx,
        DALINTRCTRL_ENABLE_CONFIG(intTrigger, 1)))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error Registering interrupt handler");
        result = Adsppm_Status_Failed;
    }
    adsppm_unlock(gIntMgrCtx.lock);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


/**
 * Interrupt event signalling ISR
 * @param ctx - pointer to the event to signal
 */
void* intr_EvtCallbackIsr(DALISRCtx ctx)
{
    DALSYSEventHandle hEvt = (DALSYSEventHandle)ctx;
    Adsppm_Status result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    if(NULL != hEvt)
    {
        // Trigger event
        if(DAL_SUCCESS != DALSYS_EventCtrl(hEvt, DALSYS_EVENT_CTRL_TRIGGER))
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "Failed to trigger event");
            result = Adsppm_Status_Failed;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Event pointer is NULL");
        result = Adsppm_Status_NullPointer;
    }
    ADSPPM_LOG_FUNC_PROFILEEND;
    return (void*)result;
}


Adsppm_Status Intr_RegisterEvent(uint32 irq, DALSYSEventHandle  event)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    //uint32 intTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
    ADSPPM_LOG_FUNC_PROFILESTART;
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Registering event for irq%d", irq);
    result = Intr_RegisterIsr(irq, intr_EvtCallbackIsr, (DALISRCtx)event);
    /*
    //TODO: Use DalInterruptController_RegisterEvent instead of registering ISR once
    //DalInterruptController_RegisterEvent becomes available
    adsppm_lock(gIntMgrCtx.lock);
    //Register the Q6_L1_AVS_SW_DONE_IRQ interrupt isr routine
    if(DAL_SUCCESS != DalInterruptController_RegisterEvent(gIntMgrCtx.hIntrInDev, irq, event,
            DALINTRCTRL_ENABLE_CONFIG(intTrigger, 1)))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Error Registering interrupt event");
        result = Adsppm_Status_Failed;
    }
    adsppm_unlock(gIntMgrCtx.lock);
     */
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status Intr_TriggerInterrupt(DalIPCIntInterruptType irq)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    adsppm_lock(gIntMgrCtx.lock);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Triggering interrupt for irq %d", irq);
    if(DAL_SUCCESS !=
        DalIPCInt_Trigger(gIntMgrCtx.hIntrOutDev, DALIPCINT_PROC_ACPU, irq))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error triggering interrupt");
        result = Adsppm_Status_Failed;
    }
    adsppm_unlock(gIntMgrCtx.lock);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status Intr_AckInterrupt(uint32 irq)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    adsppm_lock(gIntMgrCtx.lock);
    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "Acknowledging irq %d", irq);
    if(DAL_SUCCESS !=
        DalInterruptController_InterruptDone(gIntMgrCtx.hIntrInDev, irq))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
            "Error acknowledging interrupt");
        result = Adsppm_Status_Failed;
    }
    adsppm_unlock(gIntMgrCtx.lock);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}


Adsppm_Status Intr_EnableInterrupt(uint32 irq, uint32 enable)
{
    AdsppmStatusType result = Adsppm_Status_Success;
    ADSPPM_LOG_FUNC_PROFILESTART;
    adsppm_lock(gIntMgrCtx.lock);
    if(0 != enable)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Enabling interrupts for irq %d", irq);
        if(DAL_SUCCESS !=
            DalInterruptController_InterruptEnable(gIntMgrCtx.hIntrInDev, irq))
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Error enabling interrupt %d", irq);
            result = Adsppm_Status_Failed;
        }
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Disabling interrupts for irq %d", irq);
        if(DAL_SUCCESS !=
            DalInterruptController_InterruptDisable(gIntMgrCtx.hIntrInDev, irq))
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                "Error disabling interrupt", irq);
            result = Adsppm_Status_Failed;
        }
    }
    adsppm_unlock(gIntMgrCtx.lock);
    ADSPPM_LOG_FUNC_PROFILEEND;
    return result;
}

