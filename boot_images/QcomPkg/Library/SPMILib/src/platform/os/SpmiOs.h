/**
 * @file:  SpmiOs.h
 * @brief: Implements platform specific functionality
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/platform/os/SpmiOs.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#ifndef SPMIOS_H
#define	SPMIOS_H

#include "SpmiTypes.h"
#include "SpmiUtils.h"
#include "SpmiInfo.h"

typedef enum 
{
    SPMI_CLK_OFF,
    SPMI_CLK_ON,
    SPMI_CLK_STATUS_NOT_SUPPORTED
} SpmiOs_ClkStatus;


typedef void* (*SpmiOs_IsrPtr)(void* ctx);

//******************************************************************************
// Required Functionality
//******************************************************************************

/**
 * Wait for 'us' microseconds
 */
void SpmiOs_Wait(uint32 us);

/**
 * Allows custom handling of transaction errors.
 * Return True to retry the transaction. False to stop and 
 * return the error to the client.
 */
boolean SpmiOs_HandleTransactionError(SpmiDevInfo *pDev, Spmi_Result* rslt, PmicArbCmd cmd, 
                                      uint8 slaveId, uint16 address, uint8 tries);

//******************************************************************************
// Optional / Debug Functionality
//******************************************************************************

/**
 * Initializes os support structs.
 * This function sets the block address for the bus.
 */
Spmi_Result SpmiOs_Init(SpmiInfo **ppSpmiInfo);

/**
 * Sets the pointer 'buf' to a buffer of size 'size'
 * This function must zero out the new buffer
 */
Spmi_Result SpmiOs_Malloc(uint32 size, void** buf);

/**
 * Frees the buf previously allocated by SpmiOs_Malloc
 */
Spmi_Result SpmiOs_Free(void* buf);

/**
 * Registers the given ISR to service all SPMI interrupts
 */
Spmi_Result SpmiOs_RegisterISR(SpmiDevInfo *pDev, SpmiOs_IsrPtr isr);

/**
 * Return some relative time value
 */
uint64 SpmiOs_GetTimeTick(void);

/**
 * Return the state of the SPMI serial clock
 */
SpmiOs_ClkStatus SpmiOs_GetSerialClkState(SpmiDevInfo *pDev);

/**
 * Return the state of the AHB clock
 */
SpmiOs_ClkStatus SpmiOs_GetAhbClkState(SpmiDevInfo *pDev);

#endif
