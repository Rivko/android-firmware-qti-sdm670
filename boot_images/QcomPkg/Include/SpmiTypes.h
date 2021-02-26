/**
 * @file:  SpmiTypes.h
 * @brief: Common types and definitions used by the SPMI driver
 * 
 * Copyright (c) 2013-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/SpmiTypes.h#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#ifndef SPMITYPES_H
#define	SPMITYPES_H

#include "com_dtypes.h"

/**
 * Encodes a physical slave id to a virtual slave id
 */
static inline uint8 spmiEncodeVSid(uint8 busId, uint8 slaveId) {
    return (busId << 4) | (slaveId & 0x0F);
}

/**
 * Decodes a virtual slave id to a physical slave id
 */
static inline void spmiDecodeVSid(uint8 vSlaveId, uint8* busId, uint8* slaveId) {
    if(busId) *busId = vSlaveId >> 4;
    if(slaveId) *slaveId = vSlaveId & 0x0F;
}

typedef enum
{
    SPMI_SUCCESS,

    // Transaction Errors
    SPMI_FAILURE_BUS_BUSY,
    SPMI_FAILURE_BUS_DISABLED,
    SPMI_FAILURE_COMMAND_NOT_SUPPORTED,
    SPMI_FAILURE_COMMAND_NOT_ALLOWED,
    SPMI_FAILURE_TRANSACTION_FAILED,
    SPMI_FAILURE_TRANSACTION_DENIED,
    SPMI_FAILURE_TRANSACTION_DROPPED,
    SPMI_FAILURE_TRANSACTION_TIMEOUT,
    SPMI_FAILURE_SERIAL_CLK_IS_OFF,
    SPMI_FAILURE_AHB_CLK_IS_OFF,
            
    // Initialization & Validation Errors
    SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED,
    SPMI_FAILURE_INVALID_PARAMETER,
    SPMI_FAILURE_INIT_FAILED,
    SPMI_FAILURE_NOT_INITIALIZED,
    SPMI_FAILURE_INVALID_BUFFER,
    SPMI_FAILURE_TOO_MANY_ENTRIES,
    SPMI_FAILURE_NOT_ENOUGH_RGS,
    SPMI_FAILURE_INVALID_BUS_ID,
    SPMI_FAILURE_INVALID_MASTER_ID,
    SPMI_FAILURE_INVALID_SLAVE_ID,
    SPMI_FAILURE_INVALID_ADDRESS,
    SPMI_FAILURE_INVALID_PERIPH_ID,
    SPMI_FAILURE_INVALID_PORT_ID,
    SPMI_FAILURE_INVALID_OWNER,
    SPMI_FAILURE_INVALID_CHAN_NUM,
    SPMI_FAILURE_INVALID_PTR,
    SPMI_FAILURE_CHANNEL_RESERVED,
    SPMI_FAILURE_CHANNEL_NOT_FOUND,
    SPMI_FAILURE_MEMORY_NOT_AVAILABLE,
    SPMI_FAILURE_INVALID_TOKEN,
            
    SPMI_RESULT_MAX
} Spmi_Result;

typedef enum 
{
    SPMI_ACCESS_PRIORITY_LOW = 0,
    SPMI_ACCESS_PRIORITY_HIGH = 1,
    SPMI_ACCESS_PRIORITY_COUNT = 2
} Spmi_AccessPriority;

typedef enum 
{
    SPMI_COMMAND_RESET,
    SPMI_COMMAND_SLEEP,
    SPMI_COMMAND_SHUTDOWN,
    SPMI_COMMAND_WAKEUP,
    SPMI_COMMAND_COUNT
} Spmi_Command;

/**
 * SPMI ISR function type
 * @param[in] ctx   User defined data provided when registering
 * @param[in] mask  bit(s) will be set to the corresponding interrupts
 *
 * @return The return value is ignored.  Can be NULL.
 */
typedef void* (*SpmiIsr)(void* ctx, uint32 mask);

/** Legacy - DEPRECATED - don't use in new code **/
#define SPMI_BUS_SUCCESS       SPMI_SUCCESS
#define SPMI_CFG_SUCCESS       SPMI_SUCCESS
#define SPMI_INTR_CTLR_SUCCESS SPMI_SUCCESS
#define SPMI_BUS_ACCESS_PRIORITY_LOW  SPMI_ACCESS_PRIORITY_LOW
#define SPMI_BUS_ACCESS_PRIORITY_HIGH SPMI_ACCESS_PRIORITY_HIGH

#define SPMI_BUS_SERIAL_CLK_IS_OFF SPMI_FAILURE_SERIAL_CLK_IS_OFF
#define SPMI_BUS_AHB_CLK_IS_OFF    SPMI_FAILURE_AHB_CLK_IS_OFF

#define SPMI_BUS_FAILURE_INIT_FAILED         SPMI_FAILURE_INIT_FAILED
#define SPMI_BUS_FAILURE_GENERAL_FAILURE     SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED
#define SPMI_BUS_FAILURE_INVALID_PARAMETER   SPMI_FAILURE_INVALID_PARAMETER
#define SPMI_BUS_FAILURE_TRANSACTION_FAILED  SPMI_FAILURE_TRANSACTION_FAILED
#define SPMI_BUS_FAILURE_TRANSACTION_DENIED  SPMI_FAILURE_TRANSACTION_DENIED
#define SPMI_BUS_FAILURE_TRANSACTION_DROPPED SPMI_FAILURE_TRANSACTION_DROPPED
#define SPMI_BUS_FAILURE_TRANSACTION_TIMEOUT SPMI_FAILURE_TRANSACTION_TIMEOUT

#define SPMI_BUS_COMMAND_RESET    SPMI_COMMAND_RESET
#define SPMI_BUS_COMMAND_SLEEP    SPMI_COMMAND_SLEEP
#define SPMI_BUS_COMMAND_SHUTDOWN SPMI_COMMAND_SHUTDOWN
#define SPMI_BUS_COMMAND_WAKEUP   SPMI_COMMAND_WAKEUP
#define SPMI_BUS_COMMAND_COUNT    SPMI_COMMAND_COUNT

#define SPMI_BUS_ACCESS_PRIORITY_COUNT SPMI_ACCESS_PRIORITY_COUNT

typedef Spmi_Result SpmiBus_ResultType;
typedef Spmi_Result SpmiCfg_ResultType;
typedef Spmi_Result SpmiIntrCtlr_ResultType;
typedef Spmi_Command SpmiBus_CommandType;
typedef Spmi_AccessPriority SpmiBus_AccessPriorityType;
/** Legacy - DEPRECATED - don't use in new code **/

#endif
