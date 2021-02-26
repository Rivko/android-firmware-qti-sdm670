/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * busmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef BUSMGR_H_
#define BUSMGR_H_

#include "requestmgr.h"
#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "hal_clkrgm.h"
#include "systemcachemgr.h"

// Pre-aggregation data for external bandwidth requests.
// Requests are grouped in two classes:
//   - generic: "normal" requests, that are not modified
//   - compensated - requests which are to be modified according to compensation table
typedef struct
{
    AdsppmBusBWRequestValueType *pGeneric;
    AdsppmBusBWRequestValueType *pCompensated;
    uint32 numCompensatedAdspDdrVotes;
    uint64 maxAdspToDDrBw;
} busMgrExtPreAggregateData;

// Pre-aggregation data for internal bandwidth requests.
// Requests are grouped in two classes:
//   - generic: "normal" requests, that are not modified
//   - compensated - requests which are to be modified according to compensation table
typedef struct
{
    // Generic (non-compensated) class
    struct
    {
        AdsppmBusBWDataIbAbType totalBw;	/* Used for AHBI/X Aggregation */
        AdsppmBusBWDataIbAbType nonSuppressibleBw;
        AdsppmBusBWDataIbAbType Q6LpassSlpiMasterBw; /* Used for AHBE/Noc Aggregation */
    } generic;
    // Compensated class
    struct
    {
        // BW votes for ADSP master, which are modified with compensation table
        AdsppmBusBWDataIbAbType adspMasterBw;	/* Used for AHBI/X & AHBE/Noc Aggregation */
        AdsppmBusBWDataIbAbType adspMasterSlpiBw;	/* Used for AHBE/Noc Aggregation */
        // BW votes for non-ADSP masters, which aren't modified with compensation table
        AdsppmBusBWDataIbAbType nonAdspMasterBw;	/* Used for AHBI/X Aggregation */
        AdsppmBusBWDataIbAbType nonSuppressibleBw;
        AdsppmBusBWDataIbAbType nonQ6LpassSlpiMasterBw;	/* Used for AHBE/Noc Aggregation */
    } compensated;
} busMgrIntPreAggregateData;

/**
* Struct to define system cache parameters
*/
typedef struct 
{
  uint32    AggregatedClientClass;
}busmgrSystemCacheConfig;

/**
 * @struct busMgrCtxType - define bus management context
 */
typedef struct
{
    DALSYSSyncHandle            lock;
    uint32                      extRouteNum;
    AsicBusExtRouteType         *pExtRoutes;
    uint32                      preCurrentIndex; //!< Index used in pExtBwAggregateData, clockState, and snocFlootVoteMhz arrays
    AdsppmBusBWRequestValueType *pExtBwAggregateData[2];
    busMgrExtPreAggregateData   extPreAggregateData;
    busMgrIntPreAggregateData   intPreAggregateData;
    AdsppmBusBWDataIbAbType     *dcvsExtBwFloorVote[Adsppm_DCVS_Route_Max][2];//!< Keeping track of dcvs floor votes	
    Hal_ClkRgmEnableDisableType clockState[2][AdsppmClk_EnumMax];
    AdsppmBusBWDataIbAbType     curAHBBwData; //!< Current aggregated AHB BW
    AdsppmBusBWDataIbAbType     nonSuppressibleAhbBW; //!< Non-suppressible AHB BW
    AdsppmBusBWDataIbAbType     nocAhbBW; //!< Non-suppressible AHB BW
    boolean                     bPeriodicClientBW2DDR; //!< True if at least 1 periodic client requests BW to DDR
    uint32                      snocFloorVoteMhz[2]; //!< Snoc Floor vote
    uint32                      scalingfactor;//!< flag to indicate bw scaling factor based on cache size
    boolean                     externalBwUpdatePending;//!< Flag to inform main bw call, BusMgr_ProcessRequest, to skip sending vote to ICB driver	
    uint64                      bwAfterCompensatedFFIb;
    uint64                      bwAfterCompensatedFFAb;
    busmgrSystemCacheConfig     systemCacheConfig[BUSMgr_Scid_Max][MAX_STREAM_ID]; //!< System Cache Context
    uint32                      falTolerance[2];   //!< Array to maintain first access tolerance for alc
    uint32                      idleTime[2];       //!< Array to maintain idle time for Alc
    npa_client_handle           uSleepHandle; //to vote on dsp pm usleep node
} busMgrCtxType;

/**
 * @struct BUSMgr_BwDescriptorType - contains bw info for a single request
 */
typedef struct
{
    AdsppmBusPortIdType masterPort;
    AdsppmBusPortIdType slavePort;
    AdsppmBusPortIdType accessPort;
    // Master port descriptor
    const AsicBusPortDescriptorType *pMPortDescriptor;
    // Slave port descriptor
    const AsicBusPortDescriptorType *pSPortDescriptor;
    // Access port descriptor
    const AsicBusPortDescriptorType *pAPortDescriptor;
    AdsppmBusBWOperationType bwOperation;
    AdsppmBusBWDataType bwValue;
} BUSMgr_BwDescriptorType;


/**
 * @fn BusMgr_Init - bus management init
 */
Adsppm_Status BusMgr_Init(void);

/**
 * @fn BusMgr_ProcessRequest - bus management main function
 */
Adsppm_Status BusMgr_ProcessRequest(coreUtils_Q_Type *pBWReqQ);

/**
 * @fn BusMgr_GetAggregatedBwInfo - Get function for DDR ab/ib values
 */
Adsppm_Status BusMgr_GetAggregatedBwInfo(AdsppmInfoAggregatedBwType *pBusInfo);

/**
 * @fn BusMgr_GetCompensatedBwInfo- Returns the total requested BW from compensated
 *                              request types, for the specified bus route.
 *                              The output, in bytes/second, is written to the
 *                              pointer provided in pTotalRequestedBw parameter.
 */
Adsppm_Status BusMgr_GetCompensatedBwInfo(coreUtils_Q_Type* pBwRequestQueue,
    AdsppmBusRouteType* pRoute, uint64* pTotalRequestedBw);

/**
 * @fn busMgrRegisterEvents - bus management register events with other managers
 */

Adsppm_Status busMgrRegisterEvents (void);

/**
 * @fn BusMgr_GetCompensatedFudgedBwValue- Returns compensated external bw after applying v2 fudge factor calculation. Updated Ab/Ib values are returned in pAb, pIb pointers provided as arguments
 */

Adsppm_Status BusMgr_GetCompensatedFudgedBwValue(
    coreUtils_Q_Type* pBwRequestQueue,
    uint64* pAb, 
    uint64* pIb,
    AsicCompensatedBWFudgeFactor *pcompensatedBWFudgeFactorValues
    );
	
/**
 * @fn BusMgr_GetCompensatedBW- Get compensated BW value after application of fudge factor
 */

void BusMgr_GetCompensatedBW(uint64* pAb, uint64* pIb, coreUtils_Q_Type* pBwRequestQueue,	
					AsicCompensatedBWFudgeFactor *compensatedBWFudgeFactorValues);

Adsppm_DCVS_Monitored_Routes  bmSelectRouteOnPort( AdsppmBusPortIdType master,
                                                   AdsppmBusPortIdType slave);
#endif /* BUSMGR_H_ */

