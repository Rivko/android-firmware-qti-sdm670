/**
 * @file:  SpmiBus.c
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/SpmiBus.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 6/19/14  Log Fatal if bus is busy
 * 05/8/14  To check the clock status for every transaction failure reason
 * 10/1/13  Initial Version
 */

#include "SpmiBus.h"
#include "SpmiOs.h"
#include "SpmiLogs.h"
#include "PmicArb.h"
#include "SpmiInfo.h"

//******************************************************************************
// Macros / Definitions
//******************************************************************************

#define NUM_DEBUG_TRANSACTIONS 10
#define NUM_DEBUG_FAILURES 5

#define INIT_CHECK() if(!spmiDrv.initialized) { return SPMI_FAILURE_NOT_INITIALIZED; }

typedef enum {
    NOT_SET,
    STARTED,
    FINISHED,
} BusDbgState;

typedef struct {
    uint8 vsid;
    uint16 pid;
    uint8 data0;
    Spmi_Result result;
    PmicArbCmd cmd;
    uint8 dataLen;
    uint64 startTime;
    uint32 duration;
    uint8 tries;
    BusDbgState state;
} BusDbgInfo;

typedef struct {
    BusDbgInfo info;
    PmicArbHwStatus hwStatus;
} BusErrDbgInfo;

typedef struct
{
    uint8 busIdx;
    uint8 errorIdx;
    uint8 isrIdx;
    BusDbgInfo busTrans[NUM_DEBUG_TRANSACTIONS];
    BusErrDbgInfo errorTrans[NUM_DEBUG_FAILURES];
    uint16 rsltCnt[SPMI_RESULT_MAX];
    boolean syncFlag;
} SpmiBusDebugInfo;

typedef struct 
{
    boolean initialized;
    uint16  uid;
    SpmiBusDebugInfo debug;
    SpmiInfo        *pSpmiInfo;
} SpmiDrv;

//******************************************************************************
// Global Data
//******************************************************************************

SpmiDrv spmiDrv = {0};
boolean freeIsrNodes = FALSE;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static inline void debugTransStart(PmicArbCmd cmd, uint8 vSlaveId, uint16 address, uint32 len)
{
    SPMI_LOG_VERBOSE( "cmd %d / vsid: 0x%02x / addr: %x / len: %d", cmd, vSlaveId, address, len );
    
    spmiDrv.debug.busIdx = (spmiDrv.debug.busIdx + 1) % NUM_DEBUG_TRANSACTIONS;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].state = STARTED;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].cmd = cmd;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].vsid = vSlaveId;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].pid = address;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].dataLen = len;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].tries = 0;
    spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].startTime = SpmiOs_GetTimeTick();
}

static inline void debugTransEnd(PmicArbCmd cmd, uint8 vSlaveId, uint16 address, uint32 len, 
                                 uint8* data, uint8 tries, Spmi_Result rslt)
{
    if(spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].cmd == cmd &&
       spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].vsid == vSlaveId &&
       spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].pid == address &&
       spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].dataLen == len) 
    {
        spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].state = FINISHED;
        spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].result = rslt;
        spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].tries = tries;
        spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].duration = 
        SpmiOs_GetTimeTick() - spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].startTime;
        
        if(data != NULL) {
            spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].data0 = *data;
        }

        if(rslt < SPMI_RESULT_MAX) {
            spmiDrv.debug.rsltCnt[rslt]++;
        }
    }
    else {
        spmiDrv.debug.syncFlag = TRUE;
    }
    
    if(rslt != SPMI_SUCCESS)
    {
        uint8 bid;
        spmiDecodeVSid( vSlaveId, &bid, NULL );
                
        spmiDrv.debug.errorIdx = (spmiDrv.debug.errorIdx + 1) % NUM_DEBUG_FAILURES;
        spmiDrv.debug.errorTrans[spmiDrv.debug.errorIdx].info = spmiDrv.debug.busTrans[spmiDrv.debug.busIdx];
        
        PmicArb_GetHwStatus( SPMI_PDEV(spmiDrv.pSpmiInfo, bid), &spmiDrv.debug.errorTrans[spmiDrv.debug.errorIdx].hwStatus );
        SPMI_LOG_WARNING( "cmd: %d / vsid: 0x%02x / addr: 0x%04x / len: %d / rslt: %d / tries: %d", 
                          cmd, vSlaveId, address, len, rslt,
                          spmiDrv.debug.busTrans[spmiDrv.debug.busIdx].tries);
    }
}

static Spmi_Result debugExecute(PmicArbCmd cmd,
                                Spmi_AccessPriority priority,
                                uint8 vSlaveId,
                                uint16 address,
                                uint8* data,
                                uint32 len,
                                uint32* bytesTransacted)
{
    Spmi_Result rslt;
    boolean retry;
    uint8 slaveId;
    uint8 busId;
    uint8 tries = 0;
    SpmiDevInfo *pDev;
    uint16 channel;
    

    INIT_CHECK();
    
    spmiDecodeVSid( vSlaveId, &busId, &slaveId );
    if(busId >= SPMI_NUM_BUSES_SUPPORTED(spmiDrv.pSpmiInfo)) {
       return SPMI_FAILURE_INVALID_BUS_ID;
    }
    debugTransStart( cmd, vSlaveId, address, len );
    
    pDev = SPMI_PDEV(spmiDrv.pSpmiInfo,busId);
    do 
    {
        tries++;
        retry = FALSE;
        rslt = PmicArb_FindChannel(pDev, slaveId, REG_ADDR_TO_PERIPH_ID( address ), spmiDrv.pSpmiInfo->owner, &channel);
        if (SPMI_SUCCESS == rslt) {
           rslt = PmicArb_ExecuteCommand( pDev, cmd, priority,
                                          spmiDrv.pSpmiInfo->owner, channel,
                                          REG_ADDR_TO_REG_OFFSET( address ),
                                          data, len, bytesTransacted );     
        }

        // Adjust result if a more specific root cause can be found
        if(rslt != SPMI_SUCCESS) 
        {
            if(SpmiOs_GetAhbClkState( pDev ) == SPMI_CLK_OFF) {
                rslt = SPMI_FAILURE_AHB_CLK_IS_OFF;
            }
            else if(SpmiOs_GetSerialClkState( pDev ) == SPMI_CLK_OFF) {
                rslt = SPMI_FAILURE_SERIAL_CLK_IS_OFF;
            }

            retry = SpmiOs_HandleTransactionError( pDev, &rslt, cmd, slaveId, address, tries );
        }

    } while(retry);

    debugTransEnd( cmd, vSlaveId, address, len, data, tries, rslt );
    
    // Fail-fast by catching some irrecoverable errors
    if(rslt == SPMI_FAILURE_TRANSACTION_TIMEOUT) {
        SPMI_LOG_FATAL( "Timeout while executing SPMI transaction" );
    }
    else if(rslt == SPMI_FAILURE_BUS_BUSY) {
        SPMI_LOG_FATAL( "SPMI bus busy" );
    }
    
    return rslt;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiBus_Init()
{
    Spmi_Result rslt;
    
    if(spmiDrv.initialized) {
        return PmicArb_UpdateCache(spmiDrv.pSpmiInfo);
    }
    
    SPMI_LOG_INIT();

    if((rslt = SpmiOs_Init(&spmiDrv.pSpmiInfo)) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if((rslt = PmicArb_Init( spmiDrv.pSpmiInfo)) != SPMI_SUCCESS) {
        return rslt;
    }

    spmiDrv.pSpmiInfo->hasInterruptSupport = FALSE;
    
    spmiDrv.initialized = TRUE;
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_DeInit()
{
    spmiDrv.initialized = FALSE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_ReadLong(uint32 vSlaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_READ_LONG, priority, vSlaveId, address, data, len, bytesRead );
}

Spmi_Result SpmiBus_WriteLong(uint32 vSlaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG, priority, vSlaveId, address, data, len, NULL );
}

Spmi_Result SpmiBus_Command(uint32 vSlaveId, Spmi_AccessPriority priority, Spmi_Command cmd)
{
    PmicArbCmd pmicArbCmd;
    
    switch(cmd)
    {
        case SPMI_COMMAND_RESET:    pmicArbCmd = PMIC_ARB_CMD_RESET; break;
        case SPMI_COMMAND_SLEEP:    pmicArbCmd = PMIC_ARB_CMD_SLEEP; break;
        case SPMI_COMMAND_SHUTDOWN: pmicArbCmd = PMIC_ARB_CMD_SHUTDOWN; break;
        case SPMI_COMMAND_WAKEUP:   pmicArbCmd = PMIC_ARB_CMD_WAKEUP; break;
        default:
            return SPMI_FAILURE_COMMAND_NOT_SUPPORTED;
    }
    
    return debugExecute( pmicArbCmd, priority, vSlaveId, 0, NULL, 0, NULL );
}

Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 vSlaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten)
{
    Spmi_Result rslt;
    uint8 readData;

    if((rslt = SpmiBus_ReadLong( vSlaveId, priority, address, &readData, 1, NULL )) != SPMI_SUCCESS) {
        return rslt;
    }

    readData &= ~mask;
    readData |= data & mask;

    if((rslt = SpmiBus_WriteLong( vSlaveId, priority, address, &readData, 1 )) != SPMI_SUCCESS) {
        return rslt;
    }

    *byteWritten = readData;

    return SPMI_SUCCESS;
}

SpmiDevInfo *_SpmiBus_GetDevInfo(uint8 vSlaveId)
{
    uint8 busId;

    spmiDecodeVSid( vSlaveId, &busId, NULL );

    if( !spmiDrv.initialized ||
        busId >= SPMI_NUM_BUSES_SUPPORTED(spmiDrv.pSpmiInfo) )
    {
        return NULL;
    }
    
    return SPMI_PDEV(spmiDrv.pSpmiInfo,busId);
}

/************DebugChannel**********/
Spmi_Result SpmiBus_ReadLongDebug ( uint8  slaveId, Spmi_AccessPriority priority, uint16 address, uint8* data, uint32 len )
{
    Spmi_Result  rslt;    
    PmicArbCmd   cmd = PMIC_ARB_CMD_EXTENDED_REG_READ_LONG;
    uint8*       dbgBaseAddrs = NULL;
    uint32       bytesTransacted = 0;
    INIT_CHECK();
    
    dbgBaseAddrs = spmiDrv.pSpmiInfo->dbgInfo->dbgBaseAddrs;
    
    if(dbgBaseAddrs == NULL)
    {
      return SPMI_FAILURE_INVALID_PTR;
    }
    
    rslt = PmicArb_ExecuteCommandDebug ( dbgBaseAddrs, cmd, 
                                         priority, slaveId,
                                         address, data, len, 
                                         &bytesTransacted );
    if( bytesTransacted != len )
    {
      rslt = SPMI_FAILURE_TRANSACTION_FAILED;
    }
    
    return rslt;
}

Spmi_Result SpmiBus_WriteLongDebug ( uint8  slaveId, Spmi_AccessPriority priority, uint16 address, uint8* data,  uint32 len )
{
    Spmi_Result  rslt;    
    PmicArbCmd   cmd = PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG;
    uint8*       dbgBaseAddrs = NULL;
    uint32       bytesTransacted = 0;
    INIT_CHECK();
    
    dbgBaseAddrs = spmiDrv.pSpmiInfo->dbgInfo->dbgBaseAddrs;
    
    if(dbgBaseAddrs == NULL)
    {
        return SPMI_FAILURE_INVALID_PTR;
    }
    
    rslt = PmicArb_ExecuteCommandDebug ( dbgBaseAddrs, cmd, 
                                         priority, slaveId,
                                         address, data, len, 
                                         &bytesTransacted );
    if( bytesTransacted != len )
    {
        rslt = SPMI_FAILURE_TRANSACTION_FAILED;
    }
    return rslt;
}
