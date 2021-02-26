/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: mipsmgr.h
@brief: Header for ADSPPM MIPS/MPPS Manager.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/inc/mipsmgr.h#1 $
*/

#ifndef MIPSMGR_H_
#define MIPSMGR_H_

#include "requestmgr.h"
#include "adsppm.h"

#define AUDIO_INDEX 0
#define VOICE_INDEX 1
#define COMPUTE_INDEX 2
#define SLPI_INDEX 3



typedef struct
{
    uint32 mppsTotal;           //!< Total MPPS value
    uint32 numDominantThreads;  //!< Thread Loading Info
    uint32 adspFloorClock;      //!< ADSP Floor clock value in MHz
    uint32 maxMppsVote;         //!< Current Max MPPS vote
    uint32 secondMaxMppsVote;   //!< Current Second Max MPPS vote
} mipsMgrMppsClassDataType;

typedef struct
{
    uint32 mpps;
    MmpmClientClassType class;
} mipsMgrMppsReqListType;

typedef struct
{
    // Total MIPS value calculated from MPPS
    uint32 mipsTotal;
    // Audio and voice MPPS data.  Array is indexed using AUDIO_INDEX or
    // VOICE_INDEX.
    mipsMgrMppsClassDataType classData[ADSPPM_NUM_OF_CLIENT_CLASSES];
    uint32 mppsPerClientClass[ADSPPM_NUM_OF_CLIENT_CLASSES];
    // Factor 'k' determined during MPPS vote aggregation by the following
    // expression:
    //      kFactor = [CPP] * [thread loading] * 10000
    // This is a multiplier used to determine DSP frequency from MPPS.
    uint64 kFactor;

    // MPPSv3
    uint32 numReq;
    // List of MPPS requests.
    uint32 mppsReqList[30];
} mipsMgrMppsDataType;

typedef struct
{
    AdsppmMIPSToClockAggregateType mipsToClockData;
    AdsppmMIPSToBWAggregateType mipsToBWAggregateDataDdr;
    AdsppmMIPSToBWAggregateType mipsToBWAggregateDataOcmem;
} mipsMgrAggregateDataType;

typedef struct
{
    DALSYSSyncHandle         lock;
    uint32                   numOfClients;
    uint32                   numOfPeriodicClients;
    AdsppmMipsDataType       mipsAggregateData;
    mipsMgrMppsDataType      mppsAggregateData;
    mipsMgrAggregateDataType previousAggregateData;
    mipsMgrAggregateDataType currentAggregateData;
    npa_client_handle        uSleepHandle;
} mipsMgrCtxType;

Adsppm_Status MIPS_Init(void);

Adsppm_Status MIPS_ProcessRequest(coreUtils_Q_Type *pMipsReqQ);

uint32 MIPS_GetMppsAggregationInfo(uint32 clientClasses);

Adsppm_Status mipsmgr_GetMppsPerClientClass( MmpmClientClassType clientClass, uint32 *mppsvalue);

void mipsMgr_CalcMips(mipsMgrCtxType *self);


#endif /* MIPSMGR_H_ */

