/**
 * @file:  SpmiBusCfg.c
 * 
 * Copyright (c) 2013-2017 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/hal/hoya/SpmiBusCfg.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 9/2/14   Enabled security features when channel info is set
 * 10/1/13  Initial Version
*/

#include "SpmiBusCfg.h"
#include "SpmiBusInternal.h"
#include "SpmiUtils.h"
#include "SpmiLogs.h"
#include "SpmiHal.h"
#include "SpmiMaster.h"
#include "PmicArb.h"
#include "SpmiOs.h"
#include "SpmiInfo.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define DEFAULT_MASTER_ID 0
#define INIT_CHECK() do { if(!initialized) { return SPMI_FAILURE_NOT_INITIALIZED; } } while(FALSE)

//******************************************************************************
// Global Data
//******************************************************************************

static boolean initialized = FALSE;
static SpmiInfo *pSpmiInfo;

//******************************************************************************
// Local Helper Functions
//******************************************************************************
static Spmi_Result assignChannel(SpmiDevInfo *pDev, SpmiBusCfg_InternalChnlCfg* chanCfg, boolean autoAssignChan)
{
    boolean chanReserved;
    
    do
    {
        chanReserved = FALSE;
        
        if(autoAssignChan) {
            chanCfg->channel = pDev->nextChanIdx;
        }

        if(chanCfg->channel > (SPMI_NUM_CHANNELS_SUPPORTED(pDev) - 1))
        {
            if(autoAssignChan && pDev->dynamicChannelMode)
            {
                SPMI_LOG_VERBOSE( "Exhausted channels. Rolling over." );
                chanCfg->channel = 0;
            }
            else {
                return SPMI_FAILURE_INVALID_CHAN_NUM;
            }
        }
     
        pDev->nextChanIdx = chanCfg->channel + 1;
        
        if(pDev->reservedChanEn) 
        {
            if(chanCfg->channel == pDev->reservedChan) {
                chanReserved = TRUE;
            }
        }
        
        if(!autoAssignChan && chanReserved) {
            return SPMI_FAILURE_CHANNEL_RESERVED;
        }
        
    } while(autoAssignChan && chanReserved);
    
    return SPMI_SUCCESS;
}


static Spmi_Result configChannels(SpmiDevInfo *pDev, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries, 
                                  boolean autoAssignChan)
{
    uint32 i;
    Spmi_Result rslt;
    SpmiBusCfg_ChannelCfg* entry = NULL;
    SpmiBusCfg_InternalChnlCfg chanCfg;
    uint8 *base_addr = pDev->baseAddrs;
    uint16 periphOwnerMask = 0;
    uint16 irqOwnerMask = 0;
    uint8 ownerIndex = 0;
    
    INIT_CHECK();
    
    if(entries == NULL || numEntries == 0) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    if(numEntries > SPMI_NUM_CHANNELS_SUPPORTED(pDev)) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }

    if(autoAssignChan) {
        pDev->nextChanIdx = 0;
    }

    // Zero table
    for (i = 0; i < SPMI_NUM_CHANNELS_SUPPORTED(pDev); i++) {
        SPMI_HWIO_OUT( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( base_addr, i ), 0 );
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, i ), 0 );
    }

    if (SPMI_SUCCESS != (rslt = PmicArb_InvalidateCache(pSpmiInfo))) {
        return rslt;
    }
    
    for (i = 0; i < numEntries; i++) {

        entry = &entries[i];

        if(entry->slaveId > SPMI_MAX_SLAVE_ID) {
            SPMI_LOG_ERROR( "Invalid slaveId: %d", entry->slaveId );
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        if(entry->irqOwnerMask >= SPMI_OWNER_INVALID) {
            SPMI_LOG_ERROR( "Invalid irqOwnerMask: %d", entry->irqOwnerMask );
            return SPMI_FAILURE_INVALID_OWNER;
        }

        if((entry->periphOwnerMask == SPMI_OWNER_NONE) || (entry->periphOwnerMask >= SPMI_OWNER_INVALID)) {
            SPMI_LOG_ERROR( "Invalid periphOwnerMask: %d", entry->periphOwnerMask );
            return SPMI_FAILURE_INVALID_OWNER;
        }

        chanCfg.slaveId = entry->slaveId;
        chanCfg.periphId = entry->periphId;

        periphOwnerMask = entry->periphOwnerMask;
        irqOwnerMask = entry->irqOwnerMask;

        for (ownerIndex = 0; ownerIndex < SPMI_NUM_MASTERS(pDev); ownerIndex++) {

            if((periphOwnerMask & pDev->ownerMaskMap[ownerIndex]) == 0) {
                continue;
            }

            chanCfg.periphOwner = ownerIndex;
            chanCfg.irqOwnerFlag = FALSE;

            if((irqOwnerMask & pDev->ownerMaskMap[ownerIndex]) != 0) {
                chanCfg.irqOwnerFlag = TRUE;
            }

            rslt = SpmiBusCfg_ConfigureChannel( pDev, &chanCfg, autoAssignChan );

            if(SPMI_SUCCESS == rslt) {
                rslt = PmicArb_AddCacheEntry(pDev, chanCfg.channel, entry->slaveId, entry->periphId);
            }

            if(rslt != SPMI_SUCCESS) {
                return rslt;
            }
        }
    }
    
    if(!pDev->dynamicChannelMode) {

        // Enable security now that the tables are configured
        //Set SPMI_CMPR_EN_REG. CMPR_ENABLE (this step is not required for Nazgul onwards).
        //SPMI_HWIO_OUT_SET( HWIO_SPMI_CMPR_EN_REG_ADDR(base_addr), HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK );
        SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_SEC_DISABLE_REG_ADDR(base_addr), HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );

        // Enable interrupt processing
        SPMI_HWIO_OUT_SET( HWIO_SPMI_MWB_ENABLE_REG_ADDR(base_addr), HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK );
    }
    
    return SPMI_SUCCESS;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiBusCfg_Init(boolean initHw)
{
    uint32 i, k;
    Spmi_Result rslt;
    SpmiDevInfo *pDev;
    uint8 *base_addr;
    
    if (!initialized) 
    {
        SPMI_LOG_INIT();

        if ((rslt = SpmiOs_Init(&pSpmiInfo)) != SPMI_SUCCESS) {
            return rslt;
        }
        
        initialized = TRUE;
    }
    
    for(i = 0; i < pSpmiInfo->uNumDevices; i++) 
    {
        if(initHw) 
        {
            pDev = SPMI_PDEV(pSpmiInfo, i);  
            base_addr = pDev->baseAddrs;
              
            if((rslt = SpmiMaster_ConfigHW( pDev, DEFAULT_MASTER_ID, SPMI_CFG_FULL )) != SPMI_SUCCESS) {
                return rslt;
            }

            for(k = 0; k < SPMI_NUM_PORT_PRIORITIES(pDev); k++) 
            {
                SPMI_HWIO_OUT( HWIO_PMIC_ARB_PRIORITIESn_ADDR( base_addr, k ), k );
            }
        }
    }

    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_DeInit()
{
    initialized = FALSE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_GetChannelInfo(uint8 busId, uint16 channel, SpmiBusCfg_ChannelCfg* chanCfg, SpmiBusCfg_RGConfig* rgCfg)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    SpmiDevInfo *pDev;
    uint8 * base_addr;
    uint16 irqOwnerMask = 0, periphOwnerMask = 0;
    uint8 slaveId = 0, periphId = 0;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;
         
    if((chanCfg == NULL) && (rgCfg == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    rslt = PmicArb_GetChannelInfo(pDev, channel, &slaveId, &periphId, &irqOwnerMask, &periphOwnerMask);

    if(rslt != SPMI_SUCCESS) {
        return rslt;
    }

    if(chanCfg != NULL) {

        chanCfg->slaveId = slaveId;
        chanCfg->periphId = periphId;
        chanCfg->periphOwnerMask = periphOwnerMask;
        chanCfg->irqOwnerMask = irqOwnerMask;
    }

    if(rgCfg != NULL) {

        rgCfg->ownerMask = (SpmiBusCfg_OwnerMask)periphOwnerMask;
        rgCfg->startAddr = (uint32) HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel );
        rgCfg->size = (uint32) HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel+1 ) - rgCfg->startAddr;
        rgCfg->startIdx = channel;
        rgCfg->endIdx = channel;
    }
    
    return rslt;
}

Spmi_Result SpmiBusCfg_GetPeripherialInfo(uint8 busId, uint8 slaveId, uint8 periphId, SpmiBusCfg_ChannelCfg *chanCfg)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    SpmiDevInfo *pDev;
    uint16 irqOwnerMask = 0, periphOwnerMask = 0;
    
    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
         
    if(chanCfg == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    if(slaveId > SPMI_MAX_SLAVE_ID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
    
    if(periphId == 0) {
        return SPMI_FAILURE_INVALID_PERIPH_ID;
    }

    // Get the interrupt owner mask and periph owner mask
    rslt = PmicArb_GetPeriphInfo(pDev, slaveId, periphId, &irqOwnerMask, &periphOwnerMask);

    if(rslt == SPMI_SUCCESS) {
        chanCfg->slaveId = slaveId;
        chanCfg->periphId = periphId;
        chanCfg->irqOwnerMask = irqOwnerMask;
        chanCfg->periphOwnerMask = periphOwnerMask;
    }

    return rslt;
}

Spmi_Result SpmiBusCfg_SetDynamicChannelMode(uint8 busId, boolean enabled)
{
    SpmiDevInfo *pDev;
    uint8 * base_addr;
    
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;    
    // If we are changing the channel -> ppid mapping, the security 
    // and interrupt tables wont be in sync (assuming they were populated)
    // and we dont have the information to update them; so interrupts and security aren't supported.
    if(enabled) 
    {
        SPMI_LOG_WARNING( "Disabling security and interrupts" );
        //Set SPMI_CMPR_EN_REG. CMPR_ENABLE (this step is not required for Nazgul onwards).
        //SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_CMPR_EN_REG_ADDR( base_addr ), HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK );
        SPMI_HWIO_OUT_SET( HWIO_SPMI_SEC_DISABLE_REG_ADDR( base_addr ), HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );
        SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_MWB_ENABLE_REG_ADDR( base_addr ), HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK );
    }
    
    pDev->dynamicChannelMode = enabled;
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_ConfigureChannel(SpmiDevInfo *pDev, SpmiBusCfg_InternalChnlCfg* chanCfg, boolean autoAssignChan)
{
    Spmi_Result rslt;
    uint32 reg;
    uint8* base_addr = pDev->baseAddrs;
    
    if(chanCfg == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    if(chanCfg->slaveId > SPMI_MAX_SLAVE_ID) {
        SPMI_LOG_ERROR( "Invalid slaveId: %d", chanCfg->slaveId );
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }

    if(chanCfg->periphOwner >= SPMI_NUM_MASTERS(pDev)) {
        SPMI_LOG_ERROR( "Invalid periphOwner: %d", chanCfg->periphOwner );
        return SPMI_FAILURE_INVALID_OWNER;
    }

    if((rslt = assignChannel( pDev, chanCfg, autoAssignChan )) != SPMI_SUCCESS) {
        return rslt;
    }

    // Update peripheral ownership table
    reg = SPMI_HWIO_SET_FIELD_VALUE( chanCfg->periphOwner,
                                     HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );

    SPMI_HWIO_OUT( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( base_addr, chanCfg->channel ), reg );

    // Update peripheral interrupt ownership if not in dynamic mode
    if((!pDev->dynamicChannelMode) && (chanCfg->irqOwnerFlag == TRUE)) {
        reg = SPMI_HWIO_SET_FIELD_VALUE( 0x1, HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN );
    }
    else {
        reg = SPMI_HWIO_SET_FIELD_VALUE( 0x0, HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN );
    }

    // Update the channel address translation table - used by owner and observer channels
    reg |= SPMI_HWIO_SET_FIELD_VALUE( chanCfg->slaveId, HWIO_PMIC_ARB_REG_ADDRp_SID );
    reg |= SPMI_HWIO_SET_FIELD_VALUE( chanCfg->periphId, HWIO_PMIC_ARB_REG_ADDRp_ADDRESS );

    SPMI_HWIO_OUT( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, chanCfg->channel ), reg );

    SPMI_LOG_VERBOSE( "%sBus Addr:0x%x, PPID %01X %02X (irqOwnerFlag %d, chanOwner %d) @ chan %d",
                      pDev->dynamicChannelMode ? "*" : "", base_addr, chanCfg->slaveId, 
                      chanCfg->periphId, chanCfg->irqOwnerFlag, 
                      chanCfg->periphOwner, chanCfg->channel );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_ConfigureChannels(uint8 busId, SpmiBusCfg_ChannelCfg* entries, uint32 numEntries)
{
    return configChannels( SPMI_PDEV(pSpmiInfo, busId), entries, numEntries, TRUE );
}

Spmi_Result SpmiBusCfg_CalculateRGConfig(uint8 busId, SpmiBusCfg_RGConfig* rgCfg, uint32* rgCount)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    SpmiDevInfo *pDev;
    uint8 * base_addr;
    SpmiBusCfg_RGConfig channelRG;
    uint32 rgIdx = 0;
    uint16 irqOwnerMask = 0, periphOwnerMask = 0;
    uint16 channel = 0;
    uint8 slaveId = 0, periphId = 0;
    boolean rgStartFlag = FALSE;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;

    if((rgCfg == NULL) || (rgCount == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    channelRG.ownerMask = 0;


    for(channel = 0; channel < SPMI_NUM_CHANNELS_SUPPORTED(pDev); channel++) {

        rslt = PmicArb_GetChannelInfo(pDev, channel, &slaveId, &periphId, &irqOwnerMask, &periphOwnerMask);

        if(rslt != SPMI_SUCCESS) {
            continue;
    }

        // check if this channel's owner is a rg owner
        periphOwnerMask = (pSpmiInfo->rgOwnerMask & periphOwnerMask);

        if(periphOwnerMask == 0) {
    
            // if we started filling rg info and the current channel doesn't have the same rg owner,
            // then finish the previous rg info by filling up the size and endIdx info
            if(rgStartFlag == TRUE) {

                if(rgIdx == *rgCount) {
                    return SPMI_FAILURE_NOT_ENOUGH_RGS;
                }
                rgStartFlag = FALSE;
                channelRG.endIdx = channel - 1;
                channelRG.size = (uint32) HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel ) - channelRG.startAddr;
                rgCfg[rgIdx++] = channelRG;
            }
            continue;
    }
    
        if((periphOwnerMask != channelRG.ownerMask) || (rgStartFlag == FALSE)) {

            // if we started filling rg info and the current channel doesn't have the same rg owner,
            // then finish the previous rg info by filling up the size and endIdx info
            if(rgStartFlag == TRUE) {
        
                if(rgIdx == *rgCount) {
                    return SPMI_FAILURE_NOT_ENOUGH_RGS;
                }
                rgStartFlag = FALSE;
                channelRG.endIdx = channel - 1;
                channelRG.size = (uint32) HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel ) - channelRG.startAddr;
                rgCfg[rgIdx++] = channelRG;
    }

            // start filling up new RG and set the rgStartFlag
            channelRG.ownerMask = (SpmiBusCfg_OwnerMask)periphOwnerMask;
            channelRG.startIdx = channel;
            channelRG.startAddr = (uint32) HWIO_PMIC_ARB_CHNLn_CMD_ADDR( base_addr, channel );
            rgStartFlag = TRUE;
        }
    }

    // don't return rslt here since rslt is just used to ignore invalid channels
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_GetNumChannels(uint8 busId, uint32 *numChan)
{
    Spmi_Result rslt = SPMI_SUCCESS;
    SpmiDevInfo *pDev;
         
    if(numChan == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
}

    INIT_CHECK();

    pDev = SPMI_PDEV(pSpmiInfo, busId);

    *numChan = SPMI_NUM_CHANNELS_SUPPORTED(pDev);
    
    return rslt;
}


Spmi_Result SpmiBusCfg_ConfigurePvcPort(uint8 busId, const SpmiBusCfg_PvcPortCfg* portCfg)
{
    uint32 i;
    uint8 * base_addr;
    SpmiDevInfo *pDev;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;

    if((portCfg == NULL) || (portCfg->ppids == NULL)) 
    {
        return SPMI_FAILURE_INVALID_BUFFER;
    }

    if(portCfg->pvcPortId >= SPMI_NUM_PVC_PORTS(pDev) ) 
    {
        return SPMI_FAILURE_INVALID_PORT_ID;
    }

    if(portCfg->priority >= SPMI_ACCESS_PRIORITY_COUNT) 
    {
        return SPMI_FAILURE_INVALID_TOKEN;
    }
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR( base_addr, portCfg->pvcPortId ), 
                         HWIO_PMIC_ARB_PVC_PORTn_CTL_SPMI_PRIORITY,
                         portCfg->priority );

    if(portCfg->numPpids > SPMI_NUM_PVC_PPIDS(pDev) ) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }
    
    for(i = 0; i < portCfg->numPpids; i++) 
    {
        if(portCfg->ppids[i].slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVCn_ADDRm_ADDR( base_addr, portCfg->pvcPortId, i ), 
                             HWIO_PMIC_ARB_PVCn_ADDRm_SID, 
                             portCfg->ppids[i].slaveId );
        
        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVCn_ADDRm_ADDR( base_addr, portCfg->pvcPortId, i ), 
                             HWIO_PMIC_ARB_PVCn_ADDRm_ADDRESS, 
                             portCfg->ppids[i].address );
    }

    // Enable the individual port
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR( base_addr , portCfg->pvcPortId ),
                       HWIO_PMIC_ARB_PVC_PORTn_CTL_PVC_PORT_EN_BMSK );

    return SPMI_SUCCESS;
}



Spmi_Result SpmiBusCfg_SetPVCPortsEnabled(uint8 busId, boolean enable)
{
    uint8 * base_addr;

    INIT_CHECK();
    base_addr = SPMI_BID2ADDR(pSpmiInfo, busId);
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR( base_addr ), 
                         HWIO_PMIC_ARB_PVC_INTF_CTL_PVC_INTF_EN, 
                         enable );
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_ConfigureVrmPort(uint8 busId, const SpmiBusCfg_VrmPortCfg* portCfg)
{
    uint32 i;
    uint8 * base_addr;
    SpmiDevInfo *pDev;
    uint8 ppid = 0;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;

    if((portCfg == NULL) || (portCfg->ppids == NULL))
    {
        return SPMI_FAILURE_INVALID_BUFFER;
    }

    if(portCfg->priority >= SPMI_ACCESS_PRIORITY_COUNT) 
    {
        return SPMI_FAILURE_INVALID_TOKEN;
    }
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR( base_addr ), 
                         HWIO_PMIC_ARB_VRM_S_PORT_CTL_SPMI_PRIORITY,
                         portCfg->priority );

    if(portCfg->numPpids > SPMI_NUM_VRM_IDS_SUPPORTED(pDev) ) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }
    
    for(i = 0; i < portCfg->numPpids; i++) 
    {
        if(portCfg->ppids[i].slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        ppid = REG_ADDR_TO_PERIPH_ID(portCfg->ppids[i].address);

        if(ppid == 0) {
            return SPMI_FAILURE_INVALID_PERIPH_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR( base_addr, i ), 
                             HWIO_PMIC_ARB_VRM_S_ADDRw_SID, 
                             portCfg->ppids[i].slaveId );
        
        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR( base_addr, i ), 
                             HWIO_PMIC_ARB_VRM_S_ADDRw_PPID, 
                             ppid );
    }

    // Enable the individual port
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR( base_addr ),
                       HWIO_PMIC_ARB_VRM_S_PORT_CTL_VRM_S_PORT_EN_BMSK );

    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_SetVrmPortsEnabled(uint8 busId, boolean enable)
{
    uint8 * base_addr;

    INIT_CHECK();
    base_addr = SPMI_BID2ADDR(pSpmiInfo, busId);
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR( base_addr ), 
                         HWIO_PMIC_ARB_VRM_S_INTF_CTL_VRM_S_INTF_EN, 
                         enable );
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_ConfigureMgpiPort(uint8 busId, const SpmiBusCfg_MgpiPortCfg* portCfg)
{
    uint8 * base_addr;
    SpmiDevInfo *pDev;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;

    if(portCfg == NULL)
    {
        return SPMI_FAILURE_INVALID_BUFFER;
    }

    if((portCfg->mgpiPortId >= SPMI_NUM_MGPI_PORTS(pDev) ) ||
       (portCfg->pvcPortId >= SPMI_NUM_PVC_PORTS(pDev) ))
    {
        return SPMI_FAILURE_INVALID_PORT_ID;
    }

    if(portCfg->posEdgeEn) 
    {
        if(portCfg->posEdgePvcIdx >= SPMI_NUM_PVC_PPIDS(pDev) )
        {
            return SPMI_FAILURE_INVALID_PORT_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR( base_addr, portCfg->mgpiPortId ), 
                             HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_POS, 
                             portCfg->posEdgePvcIdx );

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR( base_addr, portCfg->mgpiPortId ), 
                             HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_POS, 
                             portCfg->posEdgeData );
    }

    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR( base_addr, portCfg->mgpiPortId ), 
                         HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_POS_EN, 
                         portCfg->posEdgeEn );

    if(portCfg->negEdgeEn)
    {
        if(portCfg->negEdgePvcIdx >= SPMI_NUM_PVC_PPIDS(pDev) )
        {
            return SPMI_FAILURE_INVALID_PORT_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR( base_addr, portCfg->mgpiPortId ), 
                             HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_NEG, 
                             portCfg->negEdgePvcIdx );

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR( base_addr, portCfg->mgpiPortId ), 
                             HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_NEG, 
                             portCfg->negEdgeData );
    }

    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR( base_addr, portCfg->mgpiPortId ), 
                         HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_NEG_EN, 
                         portCfg->negEdgeEn );

    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_SetMgpiPortsEnabled(uint8 busId, boolean enable)
{
    uint8* base_addr;

    INIT_CHECK();
    base_addr = SPMI_BID2ADDR(pSpmiInfo, busId);
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_MGPI_MASTER_CSR_ADDR( base_addr ), 
                         HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ENABLE, 
                         enable );
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBusCfg_ConfigureVioctl(uint8 busId, const SpmiBusCfg_VioctlCfg* vioctlCfg)
{
    uint8 * base_addr;
    SpmiDevInfo *pDev;
    uint8 ppid = 0;

    INIT_CHECK();
    pDev = SPMI_PDEV(pSpmiInfo, busId);
    base_addr = pDev->baseAddrs;

    if(vioctlCfg == NULL)
    {
        return SPMI_FAILURE_INVALID_BUFFER;
    }

    if(vioctlCfg->posEdgeEn) 
    {
        if(vioctlCfg->posEdgePpid.slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        ppid = REG_ADDR_TO_PERIPH_ID(vioctlCfg->posEdgePpid.address);

        if(ppid == 0) {
            return SPMI_FAILURE_INVALID_PERIPH_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_SID, 
                             vioctlCfg->posEdgePpid.slaveId );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_PID, 
                             ppid );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_MSG_MASK, 
                             vioctlCfg->posEdgeIrqMask );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_RCS_MSG, 
                             vioctlCfg->posEdgeIrqMask );
    }

    SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_ADDR( base_addr ), 
                         HWIO_SPMI_PIC_RCS_VIOCTL_VIOCTL_EN, 
                         vioctlCfg->posEdgeEn );

    if(vioctlCfg->negEdgeEn) 
    {
        if(vioctlCfg->negEdgePpid.slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        ppid = REG_ADDR_TO_PERIPH_ID(vioctlCfg->negEdgePpid.address);

        if(ppid == 0) {
            return SPMI_FAILURE_INVALID_PERIPH_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_N_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_N_SID, 
                             vioctlCfg->negEdgePpid.slaveId );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_N_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_N_PID, 
                             ppid );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_N_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_N_MSG_MASK, 
                             vioctlCfg->negEdgeIrqMask );

        SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_N_ADDR( base_addr ), 
                             HWIO_SPMI_PIC_RCS_VIOCTL_N_RCS_MSG, 
                             vioctlCfg->negEdgeIrqMask );
    }

    SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_RCS_VIOCTL_N_ADDR( base_addr ), 
                         HWIO_SPMI_PIC_RCS_VIOCTL_N_VIOCTL_EN, 
                         vioctlCfg->negEdgeEn );

    return SPMI_SUCCESS;
}
