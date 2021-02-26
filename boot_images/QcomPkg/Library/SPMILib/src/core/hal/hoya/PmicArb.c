/**
 * @file:  PmicArb.c
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/hal/hoya/PmicArb.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 8/06/15  More than 256 chan support.
 * 1/20/15  Multiple bus support
 * 12/5/14  Added check to limit the spmi channels equal to spmi interrupts
 * 11/3/14  Automatic channel assignment
 * 05/8/14  Moved the clock status check to other file.
 * 10/1/13  Initial Version
 */

#include "PmicArb.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"
#include "SpmiLogs.h"
#include "SpmiOs.h"
#include "SpmiInfo.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define INVALID_SID ((uint8)0x10)
#define INVALID_OWNER ((uint8)0x10)
#define INVALID_DATA ((uint16)0x200)
#define INVALID_VRM_DATA ((uint16)0x100)
#define INVALID_PERIPH_ID 0

// Max number of bytes we can read/write at a time.
#define MAX_TRANSACTION_LEN 8

//******************************************************************************
// Global Data
//******************************************************************************


//******************************************************************************
// Local Helper Functions
//******************************************************************************


static void writeSpmiData(uint8 *base_addr, uint16 channel, uint8* data, uint32 bytesToWrite)
{
    uint32 i;
    uint32 tmp = 0;
    
    for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
        ((uint8*)&tmp)[i] = data[i];
    }
    
    SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR( base_addr, channel ), tmp );
    
    if(bytesToWrite > 0)
    {
        tmp = 0;
        for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
            ((uint8*)&tmp)[i] = data[i + sizeof(uint32)];
        }
        
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR( base_addr, channel ), tmp );
    }
}

static void readSpmiData(uint8 *base_addr, uint8 owner, uint16 channel, uint8* data, uint32 bytesToRead)
{
    uint32 i;

    uint32 spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR( base_addr, owner, channel ) );
        
    for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
        data[i] = ((uint8*)&spmiData)[i];
    }

    if(bytesToRead > 0)
    {
        spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR( base_addr, owner, channel ) );

        for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
            data[i + sizeof(uint32)] = ((uint8*)&spmiData)[i];
        }
    }
}

static Spmi_Result waitForStatusDone(uint8 *base_addr, uint8 owner, boolean isObserver, uint16 channel)
{
    uint32 status = 0;
    uint32 timeout = SPMI_TIMEOUT_USEC;
    Spmi_Result result = SPMI_SUCCESS;
    
    while(status == 0 && timeout-- > 0)
    {
        SpmiOs_Wait( 1 );
        status = isObserver ? SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( base_addr, owner, channel ) ) :
                              SPMI_HWIO_IN( HWIO_PMIC_ARB_CHNLn_STATUS_ADDR( base_addr, channel ) );
    }
    
    if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DROPPED_BMSK) 
    {
        result = SPMI_FAILURE_TRANSACTION_DROPPED;
    }
    else if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DONE_BMSK)
    {
        if(status & HWIO_PMIC_ARB_CHNLn_STATUS_FAILURE_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_FAILED;
        }
        else if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DENIED_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_DENIED;
        }
    }
    else if(SPMI_HWIO_IN_FIELD(HWIO_SPMI_GENI_CTRL_ADDR( base_addr ), HWIO_SPMI_GENI_CTRL_M_GENI_DISABLE) ) {
        result = SPMI_FAILURE_BUS_DISABLED;
    }
    else {
        result = SPMI_FAILURE_TRANSACTION_TIMEOUT;
    }

    return result;
}

static __inline boolean isTransactionDone(uint8 *base_addr, uint8 owner, boolean isObserver, uint16 channel)
{
    if(isObserver) {
        return SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( base_addr, owner, channel ),
                                   HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DONE );
    } else {
        return SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_CHNLn_STATUS_ADDR( base_addr, channel ),
                                   HWIO_PMIC_ARB_CHNLn_STATUS_DONE );
    }
}

static __inline void writeCommand(uint8 *base_addr,
                                  uint8 owner, 
                                  boolean isObserver,
                                  PmicArbCmd cmd,
                                  uint16 channel,
                                  Spmi_AccessPriority priority,
                                  uint8 regOffset,
                                  uint32 len)
{
    if(isObserver) 
    {
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR( base_addr, owner, channel ),
                      (cmd                      << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OPCODE_SHFT)   |
                      (SPMI_ACCESS_PRIORITY_LOW << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_PRIORITY_SHFT) |
                      (regOffset                << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDRESS_SHFT)  |
                      (len                      << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_BYTE_CNT_SHFT) );
    }
    else 
    {
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel ),
                      (cmd       << HWIO_PMIC_ARB_CHNLn_CMD_OPCODE_SHFT)   |
                      (priority  << HWIO_PMIC_ARB_CHNLn_CMD_PRIORITY_SHFT) |
                      (regOffset << HWIO_PMIC_ARB_CHNLn_CMD_ADDRESS_OFFSET_SHFT)  |
                      (len       << HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_SHFT) );
    }
}


//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result PmicArb_Init(SpmiInfo *pSpmiInfo)
{
    Spmi_Result rslt;

    if((rslt = SpmiHal_Init()) != SPMI_SUCCESS) {
        return rslt;
    }
        
    // Cache all ppid -> channel mappings
    PmicArb_UpdateCache(pSpmiInfo);
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_InvalidateCache(SpmiInfo *pSpmiInfo)
{
    uint32 i, k;
    SpmiDevInfo *pDev;
    
    for(i = 0; i < pSpmiInfo->uNumDevices; i++) 
    {
        pDev = &pSpmiInfo->devices[i];    

        for(k = 0; k <= SPMI_MAX_PERIPH_ID; k++) {
            pDev->periphMap[k] = INVALID_DATA;
        }

        for(k = 0; k < SPMI_NUM_CHANNELS_SUPPORTED(pDev); k++) {
            pDev->channelMap[k].sid = INVALID_SID;
            pDev->channelMap[k].owner = INVALID_OWNER;
            pDev->channelMap[k].irqOwnerFlag = 0;
            pDev->channelMap[k].nextIdx = INVALID_DATA;
            pDev->channelMap[k].vrmId = INVALID_VRM_DATA;
        }
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_UpdateCache(SpmiInfo *pSpmiInfo)
{
    uint32 bid, reg;
    uint8 sid, periph;
    SpmiDevInfo *pDev;
    uint16 chan, vrmIdx, idx;
    
    PmicArb_InvalidateCache(pSpmiInfo);
    
    for(bid = 0; bid < pSpmiInfo->uNumDevices; bid++) 
    {
        pDev = SPMI_PDEV(pSpmiInfo,bid);
        uint8 * base_addr = pDev->baseAddrs; 
           
        for(chan = 0; chan < SPMI_NUM_CHANNELS_SUPPORTED(pDev); chan++) 
        {
            reg = SPMI_HWIO_IN( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, chan ) );
            sid = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_REG_ADDRp_SID );
            periph = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_REG_ADDRp_ADDRESS );

            if(periph == INVALID_PERIPH_ID) 
            {
                continue;
            }

            PmicArb_AddCacheEntry( pDev, chan, sid, periph );
        }  
   
        // update the VRM id info in the channel map entry      
        for(vrmIdx = 0; vrmIdx < SPMI_NUM_VRM_IDS_SUPPORTED(pDev); vrmIdx++) 
        {
            reg = SPMI_HWIO_IN( HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR( base_addr, vrmIdx ) );
            sid = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_VRM_S_ADDRw_SID );
            periph = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_VRM_S_ADDRw_PPID );

            if(periph == INVALID_PERIPH_ID) 
            {
                continue;
            }

            idx = pDev->periphMap[periph];

            while ((idx & INVALID_DATA) == 0)
            {
                ChannelMapEntry* entry = &pDev->channelMap[idx];

                if (entry->sid == sid) 
                {
                    entry->vrmId = vrmIdx;
                }

                idx = entry->nextIdx;
            }
        }
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_RemoveCacheEntry(SpmiDevInfo *pDev, uint16 chan)
{
   ChannelMapEntry* entry;
    uint16* pMap = pDev->periphMap;
    ChannelMapEntry* cMap = pDev->channelMap;

    if(chan >= SPMI_NUM_CHANNELS_SUPPORTED(pDev)) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }

    entry = &cMap[chan];

    if(entry->sid != INVALID_SID) 
    {
        uint16 prevIdx = chan;
        uint8 periph;

        SPMI_LOG_TRACE( "  removing chan %d (out of chain)", chan);

        // Go around the world to locate the previous entry in the chain        
        while ((cMap[prevIdx].nextIdx & INVALID_DATA) == 0)
        {
            prevIdx = cMap[prevIdx].nextIdx;
        }

        periph = (uint8)(cMap[prevIdx].nextIdx & ~INVALID_DATA);
        prevIdx = pMap[periph];

        while( (prevIdx & INVALID_DATA) == 0 &&
               prevIdx != chan && 
               cMap[prevIdx].nextIdx != chan )
        {
            prevIdx = cMap[prevIdx].nextIdx;
        }

        if(prevIdx & INVALID_DATA)
        {
            return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
        }
        
        // pull newEntry out of its chain
        if(prevIdx != chan)
        {
            cMap[prevIdx].nextIdx = entry->nextIdx;
        }
        else
        {
            // This was the start of a chain -- update periphMap.
            pMap[periph] = entry->nextIdx;
            SPMI_LOG_TRACE( "  New head ptr - periph: %x, idx: %x", periph, pMap[periph]);
        }
        
        entry->sid = INVALID_SID;
        entry->owner = INVALID_OWNER;
        entry->irqOwnerFlag = 0;
        entry->nextIdx = INVALID_DATA;
        entry->vrmId = INVALID_VRM_DATA;
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_AddCacheEntry(SpmiDevInfo *pDev, uint16 chan, uint8 sid, uint8 periph)
{
    uint32 reg;
    uint8 owner, irqOwner;
    uint16 chanIdx;
    boolean newRoot = FALSE;
    ChannelMapEntry* entry;
    ChannelMapEntry* newEntry;

    uint8* base_addr = pDev->baseAddrs; 
    uint16* pMap = pDev->periphMap;
    ChannelMapEntry* cMap = pDev->channelMap;
        
    if(chan >= SPMI_NUM_CHANNELS_SUPPORTED(pDev)) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    if(pDev->reservedChanEn && (chan == pDev->reservedChan)) {
       SPMI_LOG_VERBOSE( "Not caching reserved channel: %d", chan);
       return SPMI_FAILURE_CHANNEL_RESERVED;
    }
    
    newEntry = &cMap[chan];
    
    SPMI_LOG_TRACE( "Add to cache - base_addr: 0x%p, chan %d: sid: %d, periph: %d (0x%x)",
                    pDev->baseAddrs, chan, sid, periph, periph );

    if(pMap[periph] == INVALID_DATA)
    {
        SPMI_LOG_TRACE("  first hit empty");
        newRoot = TRUE;
        pMap[periph] = chan;
    }
    
    // 1st entry
    chanIdx = pMap[periph];
    entry = &cMap[chanIdx];

    // If this isn't the start of a new chain, follow the existing one
    if(!newRoot)
    {
        uint16 nextIdx = chanIdx;

        // go till the end of the chain or we hit the replacement
        do
        {
            chanIdx = nextIdx;
            entry = &cMap[chanIdx];
            nextIdx = entry->nextIdx;

            if(chanIdx == chan) {
                SPMI_LOG_TRACE( "  overwriting chan %d (in chain)", chan );
                newEntry->sid = sid;

                // add owner information to the new entry
                reg = SPMI_HWIO_IN( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( base_addr, chan ) );
                owner = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );

                reg = SPMI_HWIO_IN( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, chan ) );
                irqOwner = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN );

                newEntry->owner = owner;

                if(irqOwner != 0) {
                    newEntry->irqOwnerFlag = 1;
                }
                newEntry->vrmId = INVALID_VRM_DATA;
                return SPMI_SUCCESS;
            }

        } while((entry->nextIdx & INVALID_DATA) == 0);
    }
        
    if(newEntry->sid != INVALID_SID) 
    {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }

    // Extend the chain to new node
    entry->nextIdx = chan;

    newEntry->sid = sid;
    newEntry->nextIdx = INVALID_DATA | periph;

    // add owner information to the new entry
    reg = SPMI_HWIO_IN( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( base_addr, chan ) );
    owner = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );

    reg = SPMI_HWIO_IN( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, chan ) );
    irqOwner = (uint8)SPMI_HWIO_GET_FIELD_VALUE( reg, HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN );

    newEntry->owner = owner;

    if(irqOwner != 0) {
        newEntry->irqOwnerFlag = 1;
    }

    newEntry->vrmId = INVALID_VRM_DATA;

    return SPMI_SUCCESS;
}

void PmicArb_GetHwStatus(SpmiDevInfo *pDev, PmicArbHwStatus* status)
{
    uint8 * base_addr = pDev->baseAddrs;    
    if(status == NULL) {
        return;
    }
    
    status->irqStatus = SPMI_HWIO_IN( HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR( base_addr ) );
    status->geniStatus = SPMI_HWIO_IN( HWIO_SPMI_GENI_STATUS_ADDR( base_addr ) );
    status->geniCtrl = SPMI_HWIO_IN( HWIO_SPMI_GENI_CTRL_ADDR( base_addr ) );
}


Spmi_Result PmicArb_FindChannel(SpmiDevInfo *pDev, uint8 slaveId, uint8 periph, uint8 owner, uint16* channel)
{
    Spmi_Result rslt = SPMI_FAILURE_CHANNEL_NOT_FOUND;
    uint16 idx;
    
    idx = pDev->periphMap[periph];
    
    while ((idx & INVALID_DATA) == 0)
    {
        ChannelMapEntry* entry = &pDev->channelMap[idx];
        
        // If only sid matches, save the channel id and continue looping for owner match
        if (entry->sid == slaveId) {

            rslt = SPMI_SUCCESS;
            *channel = idx;

            // If both owner and sid match, break and return the channel id
            if(entry->owner == owner) {
                break;
            }
        }

        idx = entry->nextIdx;
    }

    // This peripheral is not supported
    if(rslt != SPMI_SUCCESS) {
        SPMI_LOG_TRACE( "Chan not found: bid: %d, sid %d, periph: 0x%x", bid, slaveId, periph );
    } else {
        SPMI_LOG_TRACE( "Found chan: bid: %d, sid %d, periph: 0x%x = %d", bid, slaveId, periph, *channel );
    }

    return rslt;
}


Spmi_Result PmicArb_FindPeriph(SpmiDevInfo *pDev, uint16 channel, uint8* slaveId, uint8* periph)
{
    //uint8 idx = 1;
    ChannelMapEntry* cMap;
    int16 lastIdx = channel;
         
    if((pDev == NULL) || (slaveId == NULL) || (periph == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    if (channel >= SPMI_NUM_CHANNELS_SUPPORTED(pDev)) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }

    cMap = pDev->channelMap;

    if (cMap[channel].sid == INVALID_SID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }

    *slaveId = cMap[channel].sid;

    // Go around the world to locate the last entry in the chain        
    while ((cMap[lastIdx].nextIdx & INVALID_DATA) == 0)
    {
        lastIdx = cMap[lastIdx].nextIdx;
    }

    *periph = (uint8)(cMap[lastIdx].nextIdx & ~INVALID_DATA);
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_ExecuteCommand(SpmiDevInfo *pDev,
                                   PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 owner,
                                   uint16 channel,
                                   uint16 offset,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted)
{
    uint32 tmp;
    boolean doRead = FALSE;
    boolean doWrite = FALSE;
    uint8 * base_addr = pDev->baseAddrs;
    
         
    if(data == NULL || dataLen == 0) {
        return SPMI_FAILURE_INVALID_BUFFER;
    }
    
    if(bytesTransacted == NULL) {
        bytesTransacted = &tmp;
    }

    *bytesTransacted = 0;    
    
    switch(cmd)
    {
        case PMIC_ARB_CMD_EXTENDED_REG_READ_LONG:
            doRead = TRUE;
            break;
            
        case PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG:
            doWrite = TRUE;
            break;

        default:
            return SPMI_FAILURE_COMMAND_NOT_SUPPORTED;
    }
    
    while(dataLen > 0)
    {
        Spmi_Result result;
        
        // Check if the Done bit is set. If not, the previous transaction is stuck
        if(!isTransactionDone( base_addr, owner, doRead, channel )) {
            return SPMI_FAILURE_BUS_BUSY;
        }
    
        uint32 len = dataLen > MAX_TRANSACTION_LEN ? MAX_TRANSACTION_LEN : dataLen;
        
        if(doWrite) {
            writeSpmiData( base_addr, channel, data + *bytesTransacted, len );
        }
        
        // This is the signal to start the transaction
        // len - 1 because value of 0 means 1 byte and increments from there
        writeCommand( base_addr, owner, doRead, cmd, channel, priority, (uint8)(offset + *bytesTransacted), len - 1 );
        
        if((result = waitForStatusDone( base_addr, owner, doRead, channel )) != SPMI_SUCCESS) {
            return result;
        }
        
        if(doRead) {
            readSpmiData( base_addr, owner, channel, data + *bytesTransacted, len );
        }
        
        dataLen -= len;
        *bytesTransacted += len;
    }
    
    return SPMI_SUCCESS;
}


Spmi_Result PmicArb_LogAccessViolation(SpmiDevInfo *pDev, uint64 faultAddress)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    uint16 numChannels = SPMI_NUM_CHANNELS_SUPPORTED(pDev);
    uint64 pmicArbCmdBaseAddr = (uint64)HWIO_PMIC_ARB_CHNLn_CMD_ADDR(pDev->baseAddrs, 0);
    uint64 pmicArbCmdMaxAddr = (uint64)HWIO_PMIC_ARB_CHNLn_CMD_ADDR(pDev->baseAddrs, numChannels);
    uint64 pmicArbCmdOffset = (uint64)HWIO_PMIC_ARB_CHNLn_CMD_ADDR(pDev->baseAddrs, 1) - pmicArbCmdBaseAddr;
    uint16 channel = 0;
    uint8 slaveId = 0;
    uint8 periph = 0;

    // Check if fault address is within the PMIC Arbiter core address range
    if( (faultAddress >= pmicArbCmdBaseAddr) && (faultAddress < pmicArbCmdMaxAddr) ) {

        channel = (uint16)((faultAddress - pmicArbCmdBaseAddr)/pmicArbCmdOffset);

        channel = (channel & (numChannels - 1));

        // find the peripheral info corresponding to the channel number
        rslt = PmicArb_FindPeriph(pDev, channel, &slaveId, &periph);

        SPMI_LOG_FATAL( "SPMI Fault address 0x%x maps to channel %u, slaveId %u, periph %u, rslt %d", 
                        faultAddress, channel, slaveId, periph, rslt );
    }

    return rslt;
}

Spmi_Result PmicArb_GetPeriphInfo(SpmiDevInfo *pDev, uint8 slaveId, uint8 periphId, uint16* irqOwnerMask, uint16* periphOwnerMask)
{
    Spmi_Result rslt = SPMI_FAILURE_CHANNEL_NOT_FOUND;
    uint16 idx;
         
    if((pDev == NULL) || (irqOwnerMask == NULL) || (periphOwnerMask == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    if(slaveId > SPMI_MAX_SLAVE_ID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
    
    if(periphId == INVALID_PERIPH_ID) {
        return SPMI_FAILURE_INVALID_PERIPH_ID;
    }
    
    idx = pDev->periphMap[periphId];
    
    while ((idx & INVALID_DATA) == 0)
    {
        ChannelMapEntry* entry = &pDev->channelMap[idx];
        
        // If only sid matches, save the owner info and continue looping
        if (entry->sid == slaveId) {

            if(entry->owner >= SPMI_NUM_MASTERS(pDev)) {
                rslt = SPMI_FAILURE_INVALID_OWNER;
                break;
            }

            rslt = SPMI_SUCCESS;
            *irqOwnerMask = (*irqOwnerMask|pDev->ownerMaskMap[entry->owner]);

            // Save the irq owner info if the owner is interrupt owner as well
            if(entry->irqOwnerFlag) {
                *irqOwnerMask = pDev->ownerMaskMap[entry->owner];
            }
        }

        idx = entry->nextIdx;
    }

    return rslt;
}

Spmi_Result PmicArb_GetChannelInfo(SpmiDevInfo *pDev, uint16 channel, uint8* slaveId, uint8* periphId, 
                                   uint16* irqOwnerMask, uint16* periphOwnerMask)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    ChannelMapEntry* cMap;
         
    if(pDev == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
         
    if((irqOwnerMask == NULL) || (periphOwnerMask == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    rslt = PmicArb_FindPeriph(pDev, channel, slaveId, periphId);

    if(rslt == SPMI_SUCCESS) {

        cMap = pDev->channelMap;

        if(cMap[channel].owner >= SPMI_NUM_MASTERS(pDev)) {
            return SPMI_FAILURE_INVALID_OWNER;
        }

        *periphOwnerMask = pDev->ownerMaskMap[cMap[channel].owner];
        *irqOwnerMask = 0;

        if(cMap[channel].irqOwnerFlag) {
            *irqOwnerMask = *periphOwnerMask;
        }
    }

    return rslt;
}


static void 
writeSpmiDataDebug(uint8 *base_addr, uint8* data, uint32 bytesToWrite,
                   Spmi_AccessPriority priority, uint16 address)
{
    uint32 i;
    
    //Write Data to be written
    for(i = 0; i < bytesToWrite ; i++) 
    {
        SPMI_HWIO_OUT( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base_addr,i), (uint32)(data[i]) );
    }
    return;    
}


static void 
writeCommandDebug( uint8 *base_addr, PmicArbCmd cmd, 
                   Spmi_AccessPriority priority,
                   uint8 slaveId, uint8 ppId,
                   uint8 regOffset, uint32 len)
{
    
    SPMI_HWIO_OUT( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR( base_addr ),
                  (len       << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_BYTE_CNT_SHFT) |
                  (slaveId   << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_SID_SHFT) );
                  
    SPMI_HWIO_OUT( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR( base_addr ),
                  (ppId      << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_PPID_SHFT) );
                  
    SPMI_HWIO_OUT( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR( base_addr ),
                  (regOffset << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDRESS_SHFT) );
                  
    SPMI_HWIO_OUT( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR( base_addr ),
                  (cmd       << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OPCODE_SHFT) |
                  (priority  << HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_PRIORITY_SHFT) );

    return;
}


static void readSpmiDataDebug(uint8 *base_addr, uint8* data, uint32 bytesToRead)
{
    uint32 i;

    for(i=0; i < bytesToRead; i++)
    {
        data[i] = (uint8)( SPMI_HWIO_IN( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_ADDR(base_addr, i) ) );
    }
    return;
}


static Spmi_Result waitForStatusDoneDebug(uint8 *base_addr)
{
    uint32 status = 0;
    uint32 timeout = SPMI_TIMEOUT_USEC;
    Spmi_Result result = SPMI_SUCCESS;
    
    while(status == 0 && timeout-- > 0)
    {
        SpmiOs_Wait( 1 );
        status = SPMI_HWIO_IN( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_ADDR( base_addr ) ); 
    }
    
    if(status & HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DROPPED_BMSK) 
    {
        result = SPMI_FAILURE_TRANSACTION_DROPPED;
    }
    else if(status & HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DONE_BMSK)
    {
        if(status & HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_FAILURE_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_FAILED;
        }
        else if(status & HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DENIED_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_DENIED;
        }
    }
    else {
        result = SPMI_FAILURE_TRANSACTION_TIMEOUT;
    }

    return result;
}


Spmi_Result 
PmicArb_ExecuteCommandDebug ( uint8*              dbgBaseAddrs,
                              PmicArbCmd          cmd,
                              Spmi_AccessPriority priority, 
                              uint8               slaveId, 
                              uint16              address,
                              uint8*              data, 
                              uint32              dataLen, 
                              uint32*             bytesTransacted )
{
   //add variable if the variable is for reading or writing. 
    
    uint32      tmp=0;
    uint8       ppId    = (uint8)(address >> 8);
    uint8       offset  = (uint8)(address & 0xFF);
    uint32      status  = 0;
    Spmi_Result result;
    
    if(data == NULL || dataLen == 0) {
        return SPMI_FAILURE_INVALID_BUFFER;
    }
    
    if(bytesTransacted == NULL) {
        bytesTransacted = &tmp;
    }

    *bytesTransacted = 0;    
    
    
    while(dataLen > 0)
    {
            
        uint32 len = dataLen > MAX_TRANSACTION_LEN ? MAX_TRANSACTION_LEN : dataLen;
        
        // //Check for previous done status here
        status = SPMI_HWIO_IN( HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_ADDR( dbgBaseAddrs ) );
        
        if( !(status & HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DONE_BMSK) )
        {
            return SPMI_FAILURE_BUS_BUSY;
        }
        
        if( cmd == PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG ) 
        {
            writeSpmiDataDebug( dbgBaseAddrs, data + *bytesTransacted, len, priority,  address);
        }
        
        writeCommandDebug( dbgBaseAddrs, cmd, priority, slaveId, ppId, (uint8)(offset + *bytesTransacted), len - 1 );

        
        if((result = waitForStatusDoneDebug( dbgBaseAddrs )) != SPMI_SUCCESS) 
        {
             return result;
        }
        
        if(cmd == PMIC_ARB_CMD_EXTENDED_REG_READ_LONG) {
            readSpmiDataDebug( dbgBaseAddrs, data + *bytesTransacted, len );
        }
        
        dataLen -= len;
        *bytesTransacted += len;
    }
    
    return SPMI_SUCCESS;

}
