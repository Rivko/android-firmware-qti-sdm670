/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: clientmgr.h
@brief: Header for ADSPPM Client Manager.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/clientmgr.h#1 $
*/

#ifndef _CLIENTMGR_H_
#define _CLIENTMGR_H_

#include "mmpm.h"
#include "adsppm.h"


// Number of different client classes
// Except MMPM_UNKNOWN_CLIENT_CLASS
#define ADSPPM_NUM_OF_CLIENT_CLASSES 5
#define VALID_CLIENT_CLASS(x)     ((x) < MMPM_MAX_CLIENT_CLASS) ? 1:0


// Client periodicity type
typedef enum
{
    Adsppm_ClientType_Normal, // non-periodic
    Adsppm_ClientType_Periodic
} AdsppmClientPeriodicityType;

// Structure for a request to create a new client
typedef struct
{
    AdsppmCoreIdType coreId;
    AdsppmInstanceIdType instanceId;
    const char* clientName;
    uint32 callBackFlag;
    uint32 (*callBack)(AdsppmCallbackParamType *pCbParam);
    AdsppmClientPeriodicityType periodicityType;
    MmpmClientClassType clientClass;
    MmpmDcvsParticipationType dcvsParticipation;
} AdsppmNewClientInfoType;

// Structure for all state related to a client
typedef struct
{
    uint32 clientId;
    AdsppmCoreIdType coreId;
    AdsppmInstanceIdType instanceId;
    char clientName[MAX_ADSPPM_CLIENT_NAME];
    uint32 callBackFlag;
    uint32 (*callBack)(AdsppmCallbackParamType *pCbParam);
    void *pSetParamConfig;
    AdsppmClientPeriodicityType periodicityType;
    MmpmClientClassType clientClass;
    MmpmDcvsParticipationType dcvsParticipation;
    boolean bwActiveFlag;
    boolean adspDdrBwActiveFlag;
    //boolean coreDdrBwActiveFlag;
    boolean mipsActiveFlag;
    boolean useCaseActiveFlag;
    MmpmL2CacheLineLockParameterType cacheLockParam;
    uint32  streamIdMask;
} AdsppmClientType;

// Enumeration for observable events generated by the Client Manager
typedef enum
{
    CM_ACTIVE_CLASS_CHANGE_EVENT = 0,
    CM_DCVS_PARTICIPATION_CHANGE_EVENT
} AdsppmClientManagerEventId;

typedef enum
{
    CM_BW_REQ_ACTIVE_PROPERTY = 0,       // BW request active - boolean value
    CM_ADSP_DDR_BW_REQ_ACTIVE_PROPERTY,  // ADSP to DDR BW request active -
                                         // boolean value
    CM_MIPS_REQ_ACTIVE_PROPERTY,         // MIPS request active - boolean value
    CM_CLASS_PROPERTY,                   // MmpmClientClassType value
    CM_CACHE_LINE_LOCK_PROPERTY,         // MmpmL2CacheLineLockParameterType*
                                         // value
    CM_DCVS_PARTICIPATION_PROPERTY,      // MmpmDcvsParticipationType value
    //CM_CORE_DDR_BW_REQ_ACTIVE_PROPERTY,  // Indicating there's active bandwidth
                                         // request from Core -> DDR.
    CM_STREAM_ID_PROPERTY,               //request for 32-bit System Cache Stream Id Mask
} AdsppmClientPropertyId;

typedef void (*AdsppmClientEventHandler)(AdsppmClientManagerEventId event,
    void* param);

// Structure for summarized ADSPPM clients'
// DCVS participation info
typedef struct
{
    uint32 numOfAdspDdrBwActiveDcvsClients;  // # of clients opt in DCVS
                                             // with active ADSP to DDR BW vote
    uint32 numOfMipsActiveDcvsClients;       // # of clients opt in DCVS
                                             // with active MIPS vote
    //uint32 numOfCoreDdrBwActiveDcvsClients;  // # of client opt in DCVS with Core to DDR vote
} AdsppmClientsDcvsParticipationType;

// Initialize client manager module
Adsppm_Status CM_Init(void);

// Add a new client using the given AdsppmNewClientInfoType
const AdsppmClientType* CM_AddClient(AdsppmNewClientInfoType* clientInfo);

// Get the AdsppmClientType corresponding to the given client ID
const AdsppmClientType* CM_GetClient(uint32 clientId);

// Remove the client specified by the given client ID
Adsppm_Status CM_RemoveClient(uint32 clientId);

// Set a parameter for the client specified by the given client ID
Adsppm_Status CM_SetParameter(uint32 clientId,
    AdsppmParameterConfigType* pParamConfigData);

// Set a property for the client specified by the given client ID
Adsppm_Status CM_SetProperty(uint32 clientId, AdsppmClientPropertyId prop,
    uint32 value);

// Get the currently active mix of client use case classes.  The returned value
// is a bitwise-or of values from MmpmClientClassType corresponding to the
// active client classes.
uint32 CM_GetActiveUseCaseClasses(void);

// Get the current summary of clients' DCVS participation
AdsppmClientsDcvsParticipationType CM_GetClientsDcvsParticipation(void);

// Register as an observer for an event
Adsppm_Status CM_RegisterEvent(AdsppmClientManagerEventId event,
    AdsppmClientEventHandler handler, void* param);

// Unregister as an observer for a previously registered event
Adsppm_Status CM_UnregisterEvent(AdsppmClientManagerEventId event,
    AdsppmClientEventHandler handler);

#endif /* _CLIENTMGR_H_ */
