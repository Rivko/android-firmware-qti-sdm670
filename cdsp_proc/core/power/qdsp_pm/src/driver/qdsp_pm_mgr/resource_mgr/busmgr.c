/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: busmgr.c
@brief: ADSPPM bus manager implementation.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/busmgr.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "busmgr.h"
#include "hal_clkrgm.h"
#include "ahbmgr.h"
#include "hal_busdrv.h"
#include "clientmgr.h"
#include "stdlib.h"
#include "extbwmgr.h"
#include "cachemgr.h"
#include "systemcachemgr.h"
#include "usleepmgr.h"

#define   DEFAULT_STREAM_ID    (0)
#define   ONE_BIT              (0x1)


busMgrCtxType gBusMgrCtx;

// Internal function to process bandwidth
static Adsppm_Status busMgrAggregateRequest(
    AdsppmBusBWRequestValueType *pBwRequest,
    AdsppmBwRequestClass reqClass);

static void busMgrAggregateExternalBW(coreUtils_Q_Type *pBWReqQ);
static void busMgrAggregateInternalBW(void);

/**
*@fn bmOnCPManagerUpdate - Internal function to act on cache partition change request
*/

static void bmOnCPManagerUpdate(AdsppmCachePartitionManagerEventId eventId,void * param);

/**
 * @enum BUSMgrBwTypeType - define bus bw type
 */
typedef enum
{
    BUSMgr_Bw_None              = 0,
    BUSMgr_Bw_Internal_Only     = 1,
    BUSMgr_Bw_External_Only     = 2,
    BUSMgr_Bw_Internal_External = 3,
    BUSMgr_Bw_ForceTo8bits      = 0x7f
} BUSMgrBwTypeType;

/**
 * BuMgr_USleepNode_RegisterCB
 * 
 * @brief Registers dspPm uSleep node
 * 
 * @param see npa_callback defintion for function parameters
 */
static void BuMgr_USleepNode_RegisterCB(
			void         *context,
            unsigned int  eventType,
            void         *data,
            unsigned int  dataSize)
{
    gBusMgrCtx.uSleepHandle = npa_create_sync_client(
									QDSPPM_USLEEP_NODE_NAME, 
                                    "qdsppm_busmgr",
									NPA_CLIENT_REQUIRED);
    return;
}

/**
 * @fn BusMgr_Init - bus management init, init mutex and global variables
 */
Adsppm_Status BusMgr_Init(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    DALResult dalResult = DAL_SUCCESS;
    uint32 index = 0;
    boolean dcvsfloorVoteMalloc = TRUE;

    memset(&gBusMgrCtx, 0, sizeof(busMgrCtxType));
    dalResult = DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        (DALSYSSyncHandle *) &gBusMgrCtx.lock,
        NULL);

    if(DAL_SUCCESS != dalResult)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Failed to create lock");
        sts = Adsppm_Status_Failed;
    }
    else
    {
        /* Register a callback for dspPm uSleep node
         * create a client for BusMgr to vote on dspPm uSleep node.
		 */
        npa_resource_available_cb(QDSPPM_USLEEP_NODE_NAME,
                                  BuMgr_USleepNode_RegisterCB, NULL);
	    
        // Get support route pair number from asic manager
        gBusMgrCtx.extRouteNum = ACMBus_GetNumberOfExtRoutes();
        gBusMgrCtx.pExtRoutes =
            malloc(gBusMgrCtx.extRouteNum * sizeof(AsicBusExtRouteType));
        gBusMgrCtx.pExtBwAggregateData[0] =
            malloc(gBusMgrCtx.extRouteNum * sizeof(AdsppmBusBWRequestValueType));
        gBusMgrCtx.pExtBwAggregateData[1] =
            malloc(gBusMgrCtx.extRouteNum * sizeof(AdsppmBusBWRequestValueType));
        gBusMgrCtx.extPreAggregateData.pGeneric =
            malloc(gBusMgrCtx.extRouteNum * sizeof(AdsppmBusBWRequestValueType));
        gBusMgrCtx.extPreAggregateData.pCompensated =
            malloc(gBusMgrCtx.extRouteNum * sizeof(AdsppmBusBWRequestValueType));
        for(int32 index = 0; index < Adsppm_DCVS_Route_Max; index++)
        {
            gBusMgrCtx.dcvsExtBwFloorVote[index][0] =
               malloc(sizeof(AdsppmBusBWDataIbAbType));
            gBusMgrCtx.dcvsExtBwFloorVote[index][1] =
               malloc(sizeof(AdsppmBusBWDataIbAbType));
            if( gBusMgrCtx.dcvsExtBwFloorVote[index][0] == NULL ||
                gBusMgrCtx.dcvsExtBwFloorVote[index][1] == NULL )
            {
                int32 cntAllocated = (index);
                while(cntAllocated >= 0)
                {
                   if(gBusMgrCtx.dcvsExtBwFloorVote[cntAllocated][0])
                      free (gBusMgrCtx.dcvsExtBwFloorVote[cntAllocated][0]);
                   
                   if(gBusMgrCtx.dcvsExtBwFloorVote[cntAllocated][1])
                      free (gBusMgrCtx.dcvsExtBwFloorVote[cntAllocated][1]);
                   
                   cntAllocated --;
                }
                dcvsfloorVoteMalloc = FALSE;
                break;
            }

        }

        gBusMgrCtx.scalingfactor  = 100;
        // Check for failed malloc
        if((NULL == gBusMgrCtx.pExtRoutes) ||
            (NULL == gBusMgrCtx.pExtBwAggregateData[0]) ||
            (NULL == gBusMgrCtx.pExtBwAggregateData[1]) ||
            (NULL == gBusMgrCtx.extPreAggregateData.pGeneric) ||
            (NULL == gBusMgrCtx.extPreAggregateData.pCompensated) || 
            (dcvsfloorVoteMalloc == FALSE)
            )
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                "Failed to allocate aggregate buffer");

            if(NULL != gBusMgrCtx.pExtRoutes)
            {
                free(gBusMgrCtx.pExtRoutes);
            }
            if(NULL != gBusMgrCtx.pExtBwAggregateData[0])
            {
                free(gBusMgrCtx.pExtBwAggregateData[0]);
            }
            if(NULL != gBusMgrCtx.pExtBwAggregateData[1])
            {
                free(gBusMgrCtx.pExtBwAggregateData[1]);
            }
            if(NULL != gBusMgrCtx.extPreAggregateData.pGeneric)
            {
                free(gBusMgrCtx.extPreAggregateData.pGeneric);
            }
            if(NULL != gBusMgrCtx.extPreAggregateData.pCompensated)
            {
                free(gBusMgrCtx.extPreAggregateData.pCompensated);
            }

            sts = Adsppm_Status_NoMemory;
        }
        else
        {
            // Get supported route pairs from asic manager and save
            ACMBus_GetExtRoutes(gBusMgrCtx.pExtRoutes);
            for(index = 0; index < gBusMgrCtx.extRouteNum; index++)
            {
                gBusMgrCtx.pExtBwAggregateData[0][index].busRoute.masterPort =
                    gBusMgrCtx.pExtRoutes[index].masterPort.adsppmMaster;
                gBusMgrCtx.pExtBwAggregateData[0][index].busRoute.slavePort =
                    gBusMgrCtx.pExtRoutes[index].slavePort.adsppmSlave;
                gBusMgrCtx.pExtBwAggregateData[0][index].bwOperation =
                    AdsppmBusBWOperation_BW;

                gBusMgrCtx.pExtBwAggregateData[1][index].busRoute.masterPort =
                    gBusMgrCtx.pExtRoutes[index].masterPort.adsppmMaster;
                gBusMgrCtx.pExtBwAggregateData[1][index].busRoute.slavePort =
                    gBusMgrCtx.pExtRoutes[index].slavePort.adsppmSlave;
                gBusMgrCtx.pExtBwAggregateData[1][index].bwOperation =
                    AdsppmBusBWOperation_BW;
            }

        }

    }
    return sts;
}
/**
 * @fn busMgrRegisterEvents - bus management register events with other managers
 */

Adsppm_Status busMgrRegisterEvents (void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    busMgrCtxType *self = &gBusMgrCtx;
    uint32 cachepartitionsize = 0;

    //Register an event with Cache Partition Manager for notification regarding
    //change in Cache Partition size
    if(ACM_GetFeatureStatus(AsicFeatureId_CacheSizeBWScaling) ==
                                     AsicFeatureState_Enabled)
     {
         if (CPM_RegisterEvent(CPM_CACHE_CONFIGURATION_CHANGE, bmOnCPManagerUpdate,
                              (void*) self) != Adsppm_Status_Success)
           {
                sts = Adsppm_Status_Failed;
           }
         else
          {
                cachepartitionsize = FULL_SIZE;
                ACM_GetBWScalingFactorFromCacheSize(cachepartitionsize, &gBusMgrCtx.scalingfactor);
          }
      }
  return sts;
}
static Adsppm_Status busMgrProcessClocks(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    const AsicClkDescriptorType *pClkDesc = NULL;
    uint32 index = 0;

    for(index = AdsppmClk_AhbI_Hclk;
        (index < AdsppmClk_EnumMax) && (Adsppm_Status_Success == sts);
        index++)
    {
        if(gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex^1][index] !=
            gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][index])
        {
            pClkDesc = ACMClk_GetClockDescriptorById(index);
            if(NULL == pClkDesc)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "Clock descriptor is NULL, clkId=%u", index);
                sts = Adsppm_Status_Failed;
            }
            else
            {
                switch(pClkDesc->clkCntlType)
                {
                case AsicClk_CntlSW_DCG:
                case AsicClk_CntlAlwaysON_DCG:
                case AsicClk_CntlSW:
                    sts = ClkRgm_EnableClock(
                        (AdsppmClkIdType) index,
                        gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][index]);
                    break;
                default:
                    // Ignore AON and AOFF clocks
                    break;
                }
            }
        }
    }

    return sts;
}


static uint32 busMgrProcessExternalBW(AdsppmBusBWRequestValueType *pUpdateExtBusBwValue)
{
    uint32 index = 0, numOfBW = 0;
    AdsppmBusBWRequestValueType* pCurrentBwVote = NULL;
    AdsppmBusBWRequestValueType* pPreviousBwVote = NULL;

    for(index = 0; index < gBusMgrCtx.extRouteNum; index++)
    {
        pCurrentBwVote = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex][index]);
        pPreviousBwVote = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex^1][index]);

        if((pPreviousBwVote->bwValue.busBwAbIb.Ab != pCurrentBwVote->bwValue.busBwAbIb.Ab) ||
            (pPreviousBwVote->bwValue.busBwAbIb.Ib != pCurrentBwVote->bwValue.busBwAbIb.Ib))
        {
            // Apply BW to DDR optimization if:
            // 1. use case is periodic
            // 2. and at least 1 periodic client requests BW > 0 to DDR from Q6
            if((AdsppmBusPort_Adsp_Master == pCurrentBwVote->busRoute.masterPort) &&
                (AdsppmBusPort_Ddr_Slave == pCurrentBwVote->busRoute.slavePort) &&
                (AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_TimelineOptimisationBw)))
            {
                if(IsPeriodicUseCase() && gBusMgrCtx.bPeriodicClientBW2DDR &&
                    (0 < pCurrentBwVote->bwValue.busBwAbIb.Ib))
                {
                    pCurrentBwVote->bwValue.busBwAbIb.Ib = ACM_AdjustParamValue(
                        AsicFeatureId_TimelineOptimisationBw,
                        pCurrentBwVote->bwValue.busBwAbIb.Ib);
                }
            }
            // Copy updated bus vote to buffer
            memscpy(&(pUpdateExtBusBwValue[numOfBW]),
                sizeof(AdsppmBusBWRequestValueType),
                pCurrentBwVote,
                sizeof(AdsppmBusBWRequestValueType));
            ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_INFO_EXT,
                "ext BW %u updated: Ib=%llu, Ab=%llu Latency=%u",
                numOfBW,
                (uint32) pUpdateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ib,
                (uint32) (pUpdateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ib >> 32),
                (uint32) pUpdateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ab,
                (uint32) (pUpdateExtBusBwValue[numOfBW].bwValue.busBwAbIb.Ab >> 32),
                (uint32) pUpdateExtBusBwValue[numOfBW].bwValue.busBwAbIb.latencyNs);
            numOfBW++;
        }
    }

    return numOfBW;
}

// Aggregate DCVS ADSP to DDR BW Floor vote
static Adsppm_Status busMgrAggregateDcvsFloorVote(
    AdsppmBusBWRequestValueType* pBwRequest,
    AdsppmBusBWDataIbAbType* pDcvsFloorVote)
{
    Adsppm_Status sts = Adsppm_Status_Success;

    sts = ACM_BusBWAggregate(pDcvsFloorVote, &pBwRequest->bwValue);

    return sts;
}

static Adsppm_Status busMgrUpdateUSleepMgr(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 index = 0;
    AdsppmBusBWRequestValueType* pCurrentBwVote = NULL;
    uint64 ddrAb = 0;

    for(index = 0; index < gBusMgrCtx.extRouteNum; index++)
    {
        pCurrentBwVote = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex][index]);
        if(pCurrentBwVote->busRoute.slavePort == AdsppmBusPort_Ddr_Slave)
        {
            ddrAb += pCurrentBwVote->bwValue.busBwAbIb.Ab;
        }
    }

	if (gBusMgrCtx.uSleepHandle)
	{
		boolean uSleep_vote;

   		/* Allow sleep only then aggr DDR bw = 0 */ 
    	uSleep_vote = ddrAb ? QDSPPM_USLEEP_DISALLOW : QDSPPM_USLEEP_ALLOW;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
                       "NPA request to %s usleep", (uSleep_vote ? "Allow" : "DisAllow"));

        npa_issue_required_request(gBusMgrCtx.uSleepHandle, uSleep_vote);
	}

    return sts;
}

static Adsppm_Status busMgrProgramScid (uint32 index, BUSMgrScidType scidType)
{
   busMgrCtxType *self = &gBusMgrCtx;
   uint32 usecaseId = 0;
   uint32 clientClass = 0;
   Adsppm_Status status = Adsppm_Status_Success;

   if(index >= MAX_STREAM_ID)
   {
     status = Adsppm_Status_BadParm;
     ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR, "Index is incorrect %u", index);
   }

   if (status == Adsppm_Status_Success)
   {
     clientClass = self->systemCacheConfig[scidType][index].AggregatedClientClass;

     ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"clientClass %u",clientClass);

        if((status = ACM_SystemCacheUsecaseIdfromClientClass(clientClass,
                                                             &usecaseId)) == Adsppm_Status_Success)
        {
           ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"usecase Id from SC Driver; usecaseid %u",usecaseId );
           status =  sysCacheMgrProgramScid (usecaseId, index, scidType );
        }
        else
        {
            ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,"Didn't Receive valid usecase Id from SC Driver; usecaseid %u",usecaseId );
        }


   }

  return status;
}

/**
*@Fn busMgr_SendExternalBWInfo -- Sned final bw info vote to ICB
*@param input void
*@return Adspppm_Status
*/
Adsppm_Status busMgr_SendExternalBWInfo (uint32 numUpdatedExtBwVotes,AdsppmBusBWRequestValueType *updateExtBusBwValue, boolean snocFloorChanged, uint32 snocFloorVoteMhz)
{
    Adsppm_Status sts = Adsppm_Status_Success;

    if(sts == Adsppm_Status_Success )
    {
        if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
                                 AsicFeatureState_Enabled)
         {
            sts = ExtBwMgr_IssueExtBwRequest(
                numUpdatedExtBwVotes,
                &updateExtBusBwValue[0]);
         }
        else
        {
            if(numUpdatedExtBwVotes > 0)
            {
              sts = Bus_IssueBWRequest(
                     numUpdatedExtBwVotes, &updateExtBusBwValue[0]);
            }
        }
        if((Adsppm_Status_Success == sts) && snocFloorChanged)
           sts = Bus_IssueSnocRequest(snocFloorVoteMhz);
    }

   return sts;
}

/**
*@fn busMgr_ProgramExternalBW -- Process final external bw information
*@param input void
*@return Adsppm_Status
*/
uint32  busMgr_ProcessExternalBWInfo (AdsppmBusBWRequestValueType *updateExtBusBwValue, boolean *snocFloorChanged, uint32 *snocFloorVoteMhz)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    // Locals to track change in SNOC floor vote
    uint32 numUpdatedExtBwVotes=0 ;

    // Process External Bandwidth
    if(Adsppm_Status_Success == sts)
    {
        numUpdatedExtBwVotes = busMgrProcessExternalBW(&updateExtBusBwValue[0]);
    }

    // Check for change in required SNOC floor vote
    if(gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex] !=
        gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex ^ 1])
    {
        *snocFloorVoteMhz = gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex];
        *snocFloorChanged = TRUE;
    }

   return numUpdatedExtBwVotes;
}

void  busMgr_ProcessAlcInfo (boolean *alcInfoChanged, uint32 *fal_tolerance,uint32 *idle_time )
{
    
    // Check for change in required fal tolerance
    if((gBusMgrCtx.falTolerance [gBusMgrCtx.preCurrentIndex] !=
        gBusMgrCtx.falTolerance [gBusMgrCtx.preCurrentIndex ^ 1]) ||
        (gBusMgrCtx.idleTime [gBusMgrCtx.preCurrentIndex] !=
         gBusMgrCtx.idleTime [gBusMgrCtx.preCurrentIndex ^ 1]))
    {
        *fal_tolerance = gBusMgrCtx.falTolerance [gBusMgrCtx.preCurrentIndex];
        *idle_time = gBusMgrCtx.idleTime [gBusMgrCtx.preCurrentIndex];
        *alcInfoChanged = TRUE;
    }

   ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR, "Process- Status %u, Idle time %u, Fal tol %u",*alcInfoChanged, *idle_time, *fal_tolerance );
}

void busMgr_ProcessSysCache (void )
{

  for(uint32 scidType = 0; scidType < BUSMgr_Scid_Max; scidType++)
  {
    for(uint32 index = 0;index < MAX_STREAM_ID; index++)
    {
       if(gBusMgrCtx.systemCacheConfig[scidType][index].AggregatedClientClass == MMPM_UNKNOWN_CLIENT_CLASS)
       {
          //Reset SCID
          ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                 "Resetting Scid on Index %u of scidType %u", index, scidType);
          if(Adsppm_Status_Success != sysCacheMgrResetScid (index, scidType))
          {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Reseting Scid failed");
          }
        }
        else
        {
           ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                 "Programming Scid on Index %u of scidType %u", index, scidType);
           if (Adsppm_Status_Success != busMgrProgramScid (index, scidType))
           {
                   ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,"Programming of Scid failed");
           }
        }
     }
   }

}



Adsppm_Status busMgr_AggregateSysCache (uint32 streamId, BUSMgrScidType scidType, MmpmClientClassType clientClass)
{

   Adsppm_Status sts = Adsppm_Status_Success;
   if(streamId < MAX_STREAM_ID &&
      scidType < BUSMgr_Scid_Max)
   {
     gBusMgrCtx.systemCacheConfig[scidType][streamId].AggregatedClientClass |= clientClass;

     ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_ERROR,"Path %u BW aggregated Client class %u, stream id %u",
                                                     scidType,
                                                     gBusMgrCtx.systemCacheConfig[scidType][streamId].AggregatedClientClass,
                                                     streamId );
   }
   else
   {
      ADSPPM_LOG_PRINTF_0 (ADSPPM_LOG_LEVEL_ERROR,"Aggregation of System Cache Params failed");
      sts = Adsppm_Status_BadParm;
   }

   return sts;
}
/**
 * @fn BusMgr_ProcessRequest - bus management main function,
 *                             separate bw to external and internal bw and do external bw aggregate
 * @param input: coreUtils_Q_Type *pBWReqQ
 * @return Adsppm_Status
 */
Adsppm_Status BusMgr_ProcessRequest(coreUtils_Q_Type *pBWReqQ)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 index = 0, bwNum = 0;
    RMHandleType *pRMHandle = NULL;
    AdsppmBusBWRequestValueType updateExtBusBwValue[gBusMgrCtx.extRouteNum];
    //AdsppmBusBWDataIbAbType dcvsExtBwFloorVote;
    uint32 numUpdatedExtBwVotes=0;
    // Locals to track change in SNOC floor vote
    boolean snocFloorChanged = FALSE, bSendICBRequest=FALSE;
    boolean SysCacheStatus = TRUE;
    uint32 snocFloorVoteMhz = 0;
    //Locals to track Alc values
    uint32 fal_tolerance = 0;
    uint32 idle_time = 0;
    boolean alcInfoChanged = FALSE;

    if(NULL == pBWReqQ)
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Pointer is NULL");
    }
    else
    {
        if(NULL == gBusMgrCtx.lock)
        {
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
            sts = Adsppm_Status_NotInitialized;
        }
        else
        {
            uint32 newRequestClientId = 0;
            uint32 newAdspDdrReqClientId = 0;
            uint32 releasedRequestClientId = 0;
            uint32 releaseAdspDdrReqClientId = 0;
            // Number of requests for the current client that are valid and
            // have a non-zero bandwidth.
            uint32 numNonZeroRequests = 0;
            // Number of requests for the current client that are valid and
            // have a non-zero ADSP to DDR bandwidth.
            uint32 numNonZeroAdspDdrRequests = 0;
            uint32 numNonZeroDdrRequests = 0;


            // Lock for bus manager global data update
            adsppm_lock(gBusMgrCtx.lock);
            ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_BW);

            // Init variables
            memset(updateExtBusBwValue, 0, gBusMgrCtx.extRouteNum*sizeof(AdsppmBusBWRequestValueType));
            gBusMgrCtx.curAHBBwData.Ib = gBusMgrCtx.nonSuppressibleAhbBW.Ib = MIN_AHB_BW;
            gBusMgrCtx.curAHBBwData.Ab = gBusMgrCtx.nonSuppressibleAhbBW.Ab = gBusMgrCtx.nocAhbBW.Ab = 0;
            gBusMgrCtx.bwAfterCompensatedFFAb = gBusMgrCtx.bwAfterCompensatedFFIb = 0;
            gBusMgrCtx.bPeriodicClientBW2DDR = FALSE;
            gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes = 0;
            gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw = 0;
            gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex] = 0;
            gBusMgrCtx.falTolerance[gBusMgrCtx.preCurrentIndex]     = 0;
            gBusMgrCtx.idleTime[gBusMgrCtx.preCurrentIndex]         = 0;

            for(uint32 index = 0; index < Adsppm_DCVS_Route_Max; index++)
            {
              gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex]->Ab = 0;
              gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex]->Ib = 0;
              gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex]->latencyNs= 0;
            }

            if (ACM_GetFeatureStatus( AsicFeatureId_AhbE_VoteScaling) == AsicFeatureState_Enabled)
            {
              gBusMgrCtx.nocAhbBW.Ib = MIN_AHB_BW;
            }
            else
            {
              gBusMgrCtx.nocAhbBW.Ib = 0;
            }
            for (index = 0; index < BUSMgr_Scid_Max; index++)
            {
               for (uint32 streamid = 0; streamid < MAX_STREAM_ID; streamid++)
               {
                  gBusMgrCtx.systemCacheConfig[index][streamid].AggregatedClientClass = MMPM_UNKNOWN_CLIENT_CLASS;
               }
            }

            for(index = 0; index < gBusMgrCtx.extRouteNum; index++)
            {
                gBusMgrCtx.extPreAggregateData.pGeneric[index].bwValue.busBwAbIb.Ab = 0;
                gBusMgrCtx.extPreAggregateData.pGeneric[index].bwValue.busBwAbIb.Ib = 0;
                gBusMgrCtx.extPreAggregateData.pCompensated[index].bwValue.busBwAbIb.Ab = 0;
                gBusMgrCtx.extPreAggregateData.pCompensated[index].bwValue.busBwAbIb.Ib = 0;
                gBusMgrCtx.extPreAggregateData.pCompensated[index].bwValue.busBwAbIb.latencyNs = 0;
            }

            memset(&gBusMgrCtx.intPreAggregateData, 0, sizeof(busMgrIntPreAggregateData));

            for(index = AdsppmClk_AhbI_Hclk; index < AdsppmClk_Audio_Core_AON; index++)
            {
              if(ACM_IsClkIdValid(index))
              {
                gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][index] =
                 (AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Ahb_Sw_CG))?
                 Hal_ClkRgmEnable:Hal_ClkRgmDisable;
              }
            }

            for(index = AdsppmClk_HwRsp_Core; index < AdsppmClk_EnumMax; index++)
            {
              if(ACM_IsClkIdValid(index))
              {
                 gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][index] =
                    (AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_CoreClk_Scaling))?
                 Hal_ClkRgmEnable:Hal_ClkRgmDisable;
              }
            }

            // Go through each client's bw request
            pRMHandle = (RMHandleType *)coreUtils_Q_Check(pBWReqQ);

            for(index = 0; (index < pBWReqQ->nCnt) && (NULL != pRMHandle); index++)
            {
                numNonZeroRequests = 0;

                // Generic BW Request
                if(RM_Valid == pRMHandle->validFlag)
                {
                    ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT, "Generic BW Request. Index : %u. Client ID : %u",index,pRMHandle->clientId);
                    if((NULL == (AdsppmBwReqType *)(pRMHandle->pRequestData)) ||
                        (NULL == pRMHandle->pClient))
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                            "pRequestData(%u) or pClient(%u) is NULL",
                            (AdsppmBwReqType *) (pRMHandle->pRequestData), pRMHandle->pClient);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        AdsppmBwReqType *pBwReqData = (AdsppmBwReqType *) (pRMHandle->pRequestData);
                        // Go through each bw request for this client
                        for(bwNum = 0; (bwNum < pBwReqData->numOfBw) && (Adsppm_Status_Success == sts); bwNum++)
                        {
                            AdsppmBusBWRequestValueType *pBwRequest = &pBwReqData->pBwArray[bwNum];

                            // Get bus port IDs corresponding to this core instance if applicable
                            pBwRequest->busRoute.masterPort = ACMBus_GetInstanceMasterBusPortId(
                                pRMHandle->pClient->coreId,
                                pRMHandle->pClient->instanceId,
                                pBwRequest->busRoute.masterPort);
                            pBwRequest->busRoute.slavePort = ACMBus_GetInstanceSlaveBusPortId(
                                pRMHandle->pClient->coreId,
                                pRMHandle->pClient->instanceId,
                                pBwRequest->busRoute.slavePort);

                            // Filter out requests with bus ports that have no connections
                            // meaning they do not exit in current chip set
                            AsicBusPortConnectionType masterPortConnection =
                                ACMBus_GetPortConnections(pBwRequest->busRoute.masterPort);
                            AsicBusPortConnectionType slavePortConnection =
                                ACMBus_GetPortConnections(pBwRequest->busRoute.slavePort);

                            if((masterPortConnection == AsicBusPort_None) ||
                                (slavePortConnection == AsicBusPort_None))
                            {
                                ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
                                    "BW request with non-existing ports: from %s(%u) to %s(%u)",
                                    adsppm_getBusPortName(pBwRequest->busRoute.masterPort),
                                    pBwRequest->busRoute.masterPort,
                                    adsppm_getBusPortName(pBwRequest->busRoute.slavePort),
                                    pBwRequest->busRoute.slavePort);
                                sts = Adsppm_Status_UnSupported;
                                break;
                            }
                            else
                            {
                                // Filter out BW values of Zero
                                if(0 == pBwRequest->bwValue.busBwValue.bwBytePerSec)
                                {
                                    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
                                        "BW request of Zero from %s(%u) to %s(%u), exclude from aggregation",
                                        adsppm_getBusPortName(pBwRequest->busRoute.masterPort),
                                        pBwRequest->busRoute.masterPort,
                                        adsppm_getBusPortName(pBwRequest->busRoute.slavePort),
                                        pBwRequest->busRoute.slavePort);
                                }
                                else
                                {
                                    const AsicBusPortDescriptorType *masterPortDesc = NULL;
                                    masterPortDesc = ACMBus_GetPortDescriptor(pBwRequest->busRoute.masterPort);
									
									if(masterPortDesc == NULL){
										ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                                        "masterPortDesc is NULL for request from Client ID: %u ",pRMHandle->clientId);
										sts = Adsppm_Status_Failed;
										break;
									}
									
                                    // Special considerations for BW requests from ADSP to DDR
                                    if((pBwRequest->busRoute.masterPort == AdsppmBusPort_Q6DSP_Master) &&
                                        (pBwRequest->busRoute.slavePort == AdsppmBusPort_Ddr_Slave))
                                    {
                                        // Check if at least 1 periodic client requests BW to DDR
                                        if((!gBusMgrCtx.bPeriodicClientBW2DDR) &&
                                            (Adsppm_ClientType_Periodic ==
                                                pRMHandle->pClient->periodicityType))
                                        {
                                            gBusMgrCtx.bPeriodicClientBW2DDR = TRUE;
                                        }

                                        numNonZeroAdspDdrRequests += 1;

                                        //Check if System Cache is available
                                        if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_SystemCacheSupport))
                                        {
                                           
                                           SysCacheIsImplemented(&SysCacheStatus);
                                           if(SysCacheStatus)
                                           {
                                               //Aggregate External Active Client Class
                                               if(pRMHandle->pClient->clientClass > MMPM_UNKNOWN_CLIENT_CLASS)
                                               {
                                                   //Traverse System Cache Stream Id 32-bit mask
                                                   // to set client class for every stream id
                                                   uint32 count = 0;
                                                   uint32 clientStreamIdMask = 0;
                                                   clientStreamIdMask = pRMHandle->pClient->streamIdMask;
                                                   do
                                                   {
                                                     if(count == 0 || 
                                                        clientStreamIdMask & ONE_BIT)
                                                     {
                                                       busMgr_AggregateSysCache (count,
                                                                                 BUSMgr_Scid_External,
                                                                                 pRMHandle->pClient->clientClass);
                                                     }
                                                     count++;
                                                     clientStreamIdMask >>= ONE_BIT;

                                                     if(clientStreamIdMask == 0)
                                                       break;
                                                    
                                                   }while (count < MAX_STREAM_ID);
                                               }
                                           }
                                       }
                                    }
                                    else if (masterPortDesc->portConnection != NULL &&
                                             (0 != (masterPortDesc->portConnection & AsicBusPort_AhbI_M)) &&
                                             (pBwRequest->busRoute.slavePort == AdsppmBusPort_Ddr_Slave))
                                    {
                                       if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_SystemCacheSupport))
                                       {
                                          SysCacheIsImplemented(&SysCacheStatus);
                                          if(SysCacheStatus)
                                          {
                                             //Aggregate Internal Active Client Class; Add new member in bus mgr ctxt
                                             if(pRMHandle->pClient->clientClass > MMPM_UNKNOWN_CLIENT_CLASS)
                                             {
                                                busMgr_AggregateSysCache (DEFAULT_STREAM_ID,
                                                                          BUSMgr_Scid_Internal,
                                                                          pRMHandle->pClient->clientClass);
                                             }
                                          }
                                        }
                                    }

                                    /* If this is one of the DCVS route, update DCVS Floor vote */
                                    if( bmSelectRouteOnPort(pBwRequest->busRoute.masterPort,
                                                 pBwRequest->busRoute.slavePort) != Adsppm_DCVS_Route_Max)
                                    {
                                        numNonZeroDdrRequests += 1;

                                        if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
                                            AsicFeatureState_Enabled)
                                        {
                                            // Aggregate DCVS Floor vote depending on client's
                                            // participation in DCVS
                                            // Two type of clients counts towards DCVS Floor vote:
                                            // 1. Clients that voted for DCVS disabled
                                            // 2. Clients that voted for DCVS enabled and
                                            //    chose MMPM_DCVS_ADJUST_ONLY_UP as enable option
                                            if((!pRMHandle->pClient->dcvsParticipation.enable) ||
                                                (pRMHandle->pClient->dcvsParticipation.enable &&
                                                (pBwReqData->requestClass == Adsppm_BwReqClass_Generic) &&
                                                    (pRMHandle->pClient->dcvsParticipation.enableOpt ==
                                                        MMPM_DCVS_ADJUST_ONLY_UP)))
                                            {
                                                Adsppm_DCVS_Monitored_Routes route = bmSelectRouteOnPort(pBwRequest->busRoute.masterPort,
                                                                                                         pBwRequest->busRoute.slavePort);
                                                sts = busMgrAggregateDcvsFloorVote(
                                                    pBwRequest, gBusMgrCtx.dcvsExtBwFloorVote[route][gBusMgrCtx.preCurrentIndex]);
                                                if(sts != Adsppm_Status_Success)
                                                {
                                                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                                        "Aggregation of DCVS floor vote failed");
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    numNonZeroRequests += 1;
                                    sts = busMgrAggregateRequest(pBwRequest, pBwReqData->requestClass);
                                }
                            }
                        }
                    }
                }

                // Register Programming
                if(RM_Valid == pRMHandle->validFlag_Ext)
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO_EXT, "Register Programming");
                    if((NULL == (AdsppmBusBWRequestValueType *) (pRMHandle->pRequestData_Ext)) ||
                        (NULL == pRMHandle->pClient))
                    {
                        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                            "pRequestData_Ext(%u) or pClient(%u) is NULL",
                            (AdsppmBusBWRequestValueType *) (pRMHandle->pRequestData_Ext),
                            pRMHandle->pClient);
                        sts = Adsppm_Status_BadParm;
                        break;
                    }
                    else
                    {
                        AdsppmBusBWRequestValueType *pRegBwReqData =
                            (AdsppmBusBWRequestValueType *) (pRMHandle->pRequestData_Ext);

                        // Get slave port for this core instance
                        pRegBwReqData->busRoute.slavePort = ACMBus_GetCoreSlavePort(
                            pRMHandle->pClient->coreId,
                            pRMHandle->pClient->instanceId);

                        ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO_EXT,
                            "RegProg: M=%s(%u), S=%s(%u), bw=%u",
                            adsppm_getBusPortName(pRegBwReqData->busRoute.masterPort),
                            pRegBwReqData->busRoute.masterPort,
                            adsppm_getBusPortName(pRegBwReqData->busRoute.slavePort),
                            pRegBwReqData->busRoute.slavePort,
                            (uint32) pRegBwReqData->bwValue.busBwValue.bwBytePerSec);

                        // Filter out requests with bus ports that have no connections
                        // meaning they do not exit in current chip set
                        // and requests with AdsppmBusPort_None
                        AsicBusPortConnectionType slavePortConnection =
                            ACMBus_GetPortConnections(pRegBwReqData->busRoute.slavePort);

                        if(slavePortConnection == AsicBusPort_None)
                        {
                            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                                "Reg prog for coreID %u instanceID %u not supported",
                                pRMHandle->pClient->coreId,
                                pRMHandle->pClient->instanceId);
                            sts = Adsppm_Status_UnSupported;
                            break;
                        }

                        numNonZeroRequests += 1;
                        sts = busMgrAggregateRequest(pRegBwReqData, Adsppm_BwReqClass_Generic);
                    }
                }

                if(Adsppm_Status_Success != sts)
                {
                    ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "BW aggregation failed");
                    break;
                }

                // Keep track of new and released requests;
                // Assuming there is no more than one of each for each
                // time this function is called
                if(pRMHandle->lifecycleState == NEW_REQUEST)
                {
                    if(numNonZeroRequests > 0)
                    {
                        // This is a normal new/modified request with non-zero
                        // BW
                        if(newRequestClientId)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                                "more than one new request received");
                        }
                        newRequestClientId = pRMHandle->clientId;
                    }
                    else
                    {
                        // This is a new/modified request with zero BW, which
                        // we treat similar to a release request
                        if(releasedRequestClientId)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                                "more than one request released");
                        }
                        releasedRequestClientId = pRMHandle->clientId;
                    }

                    if(numNonZeroAdspDdrRequests > 0 ||
                       numNonZeroDdrRequests > 0)
                    {
                        // This is a normal new/modified request with
                        // non-zero ADSP to DDR BW
                        if(newAdspDdrReqClientId)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                                "more than one new ADSP to DDR request received");
                        }
                        newAdspDdrReqClientId = pRMHandle->clientId;
                    }
                    else
                    {
                        // This is a new/modified request with zero BW, which
                        // we treat similar to a release request
                        if(releaseAdspDdrReqClientId)
                        {
                            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                                "more than one ADSP to DDR request released");
                        }
                        releaseAdspDdrReqClientId = pRMHandle->clientId;
                    }
                }
                else if((pRMHandle->lifecycleState == RELEASED_REQUEST) &&
                    (pRMHandle->validFlag != RM_Valid) &&
                    (pRMHandle->validFlag_Ext != RM_Valid))
                {
                    // This is a fully released request since both validFlag
                    // and validFlag_Ext are false
                    if(releasedRequestClientId)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                            "more than one request released");
                    }
                    releasedRequestClientId = pRMHandle->clientId;

                    if(releaseAdspDdrReqClientId)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
                            "more than one ADSP to DDR request released");
                    }
                    releaseAdspDdrReqClientId = pRMHandle->clientId;
                }

                pRMHandle = (RMHandleType *) coreUtils_Q_Next(pBWReqQ, &pRMHandle->link);
            }

            if(Adsppm_Status_Success == sts)
            {
                // Previous processing only applied pre-aggregation for
                // external BW.  Now do final aggregation for external BW
                // requests.
                busMgrAggregateExternalBW(pBWReqQ);
            }

            if(Adsppm_Status_Success == sts)
            {
                // Previous processing only applied pre-aggregation for
                // internal (AHB) BW.  Now do final aggregation for internal BW
                // requests.
                busMgrAggregateInternalBW();
            }
#ifdef BUSMGR_AHB_BW_SUPPORT
            // Process Internal AHB bandwidth
            if(Adsppm_Status_Success == sts)
            {
                sts = AHBM_ProcessRequest(&gBusMgrCtx.curAHBBwData, &gBusMgrCtx.nonSuppressibleAhbBW, &gBusMgrCtx.nocAhbBW);
            }
#endif
            // Process clocks
            if(Adsppm_Status_Success == sts)
            {
                sts = busMgrProcessClocks();
            }

            //Prepare final external bw results
            numUpdatedExtBwVotes = busMgr_ProcessExternalBWInfo(&updateExtBusBwValue[0], &snocFloorChanged, &snocFloorVoteMhz);
            
            if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Alc))
            {
              if (numUpdatedExtBwVotes > 0)
                busMgr_ProcessAlcInfo(&alcInfoChanged, &fal_tolerance, &idle_time);
            }

            
            //Update adsp/non-adsp to ddr floor votes if changed
            for(uint32 index = 0; index < Adsppm_DCVS_Route_Max; index++)
            {
              if((gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex]->Ab != 
                  gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex^1]->Ab) ||
                  (gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex]->Ib != 
                  gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex^1]->Ib))
              {
                 ExtBwMgr_UpdateAdsptoDDrClientFloorVotes(gBusMgrCtx.dcvsExtBwFloorVote[index][gBusMgrCtx.preCurrentIndex],
                                                          index);
              }
            }

		    // Update uSleep manager
            if(Adsppm_Status_Success == sts)
            {
                sts = busMgrUpdateUSleepMgr();
            }
            // Update the index for next iteration
            gBusMgrCtx.preCurrentIndex ^= 1;

            //ReSet flag to make sure that BW process request sends request to ICB
            if(numUpdatedExtBwVotes > 0)
                gBusMgrCtx.externalBwUpdatePending = TRUE;

            ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_BW);
            adsppm_unlock(gBusMgrCtx.lock);

            // Update client properties
            if(newRequestClientId)
            {
                // BW request is now active for client making the new request
                CM_SetProperty(newRequestClientId, CM_BW_REQ_ACTIVE_PROPERTY, TRUE);
            }
            if(releasedRequestClientId)
            {
                // BW request is no longer active for client making the released request
                // clientId should still be valid for released requests
                CM_SetProperty(releasedRequestClientId, CM_BW_REQ_ACTIVE_PROPERTY, FALSE);
            }
            if(newAdspDdrReqClientId)
            {
                // ADSP to DDR BW request is now active for client making the new request
                CM_SetProperty(newAdspDdrReqClientId,
                    CM_ADSP_DDR_BW_REQ_ACTIVE_PROPERTY, TRUE);
            }
            if(releaseAdspDdrReqClientId)
            {
                // ADSP to DDR BW request is no longer active for client making the
                // released request clientId should still be valid for released requests
                CM_SetProperty(releaseAdspDdrReqClientId,
                    CM_ADSP_DDR_BW_REQ_ACTIVE_PROPERTY, FALSE);
            }
            //Send Final bw infor to ICB
            adsppm_lock(gBusMgrCtx.lock);
            bSendICBRequest = gBusMgrCtx.externalBwUpdatePending;
            gBusMgrCtx.externalBwUpdatePending = FALSE;
            adsppm_unlock(gBusMgrCtx.lock);

            if (bSendICBRequest == TRUE )
            {
              if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_SystemCacheSupport))
              {
                SysCacheIsImplemented(&SysCacheStatus);
                if(SysCacheStatus)
                {
                   busMgr_ProcessSysCache ();
                }
              }
              if( Adsppm_Status_Success == busMgr_SendExternalBWInfo(numUpdatedExtBwVotes,&updateExtBusBwValue[0], snocFloorChanged,snocFloorVoteMhz ))
              {
                 if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Alc))
                 {
                    if(alcInfoChanged == TRUE)
                      Bus_IssueAlcRequest (fal_tolerance, idle_time);
                 }
              }
            }
          }
        }
    return sts;
}


BUSMgrBwTypeType busMgrGetBwType(AsicBusPortConnectionType masterPortConnection, AsicBusPortConnectionType slavePortConnection)
{
    BUSMgrBwTypeType bwType = BUSMgr_Bw_None;

    if((0 != (masterPortConnection & AsicBusPort_Ext_Any)) &&
        (0 != (slavePortConnection & AsicBusPort_Ext_Any)))
    {
        bwType = BUSMgr_Bw_External_Only;
    }
    else if((0 != (masterPortConnection & AsicBusPort_Ahb_Any)) &&
        (0 != (slavePortConnection & AsicBusPort_Ahb_Any)))
    {
        bwType = BUSMgr_Bw_Internal_Only;
    }
    else if(((0 != (masterPortConnection & AsicBusPort_Ext_Any)) ||
            (0 != (masterPortConnection & AsicBusPort_Ahb_Any))) &&
        ((0 != (slavePortConnection & AsicBusPort_Ext_Any)) ||
            (0 != (slavePortConnection & AsicBusPort_Ahb_Any))))
    {
        bwType = BUSMgr_Bw_Internal_External;
    }

    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "connectM=0x%x, connectS=0x%x, bwType=%u",
        masterPortConnection, slavePortConnection, bwType);

    return bwType;
}


static Adsppm_Status busMgrPreAggregateInternalBW(
    BUSMgr_BwDescriptorType *pBwDesc,
    AdsppmBwRequestClass requestClass)
{
    Adsppm_Status sts = Adsppm_Status_Success;

    boolean isByPassAggregation = FALSE;

    //For CDSP, the route going from HCP/DMA to DDR wouldn't impact the internal
    //AHB-I clock, because AHB-I and AHB-X are sourced from different source,
    //and AHB-X is always ON
    if (( pBwDesc->masterPort == AdsppmBusPort_Vapss_Hcp_Master ||
          pBwDesc->masterPort == AdsppmBusPort_Vapss_Dma_Master ) &&
          pBwDesc->slavePort  == AdsppmBusPort_Ddr_Slave )
    {
        isByPassAggregation = TRUE;
    }

    if(requestClass == Adsppm_BwReqClass_Compensated)
    {
        AdsppmBusBWDataIbAbType* pAdspMasterBw;
        AdsppmBusBWDataIbAbType* pAdspMasterSlpiBw;
        AdsppmBusBWDataIbAbType* pNonAdspMasterBw;
        AdsppmBusBWDataIbAbType* pNonSuppressibleBw;
        AdsppmBusBWDataIbAbType* pNonQ6LpassSlpiMasterBw;

        pAdspMasterBw =
            &gBusMgrCtx.intPreAggregateData.compensated.adspMasterBw;
        pAdspMasterSlpiBw = 
            &gBusMgrCtx.intPreAggregateData.compensated.adspMasterSlpiBw;
        pNonAdspMasterBw =
            &gBusMgrCtx.intPreAggregateData.compensated.nonAdspMasterBw;
        pNonSuppressibleBw =
            &gBusMgrCtx.intPreAggregateData.compensated.nonSuppressibleBw;
        pNonQ6LpassSlpiMasterBw =
            &gBusMgrCtx.intPreAggregateData.compensated.nonQ6LpassSlpiMasterBw;
        
        if(AdsppmBusPort_Q6DSP_Master == pBwDesc->masterPort)
        {

            if(AdsppmBusPort_Slpi_Sram_Slave == pBwDesc->slavePort)
            {
				/* Q6 -> SRAM :: C2 */
               sts = ACM_BusBWAggregate(pAdspMasterSlpiBw, &pBwDesc->bwValue);
               if(Adsppm_Status_Success != sts)
               {
                  ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal BW aggregation Slpi failed: %u", sts);
               }               
            }
            else
			{
				/* Q6 -> Others(except SRAM) :: C1 (eg: Q6 -> LPM/LPASS)*/
				sts = ACM_BusBWAggregate(pAdspMasterBw, &pBwDesc->bwValue);
				if(Adsppm_Status_Success != sts)
				{
					ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
						"internal BW aggregation failed: %u", sts);
				}
			}
        }
        else if (AdsppmBusPort_Sdc_Master == pBwDesc->masterPort || 
                 AdsppmBusPort_Qup_Master == pBwDesc->masterPort )
        {
			/* SDC/QUP -> Others(Slpi clients) :: C4 (sdc->ddr, sdc->sram, qup->ddr, qup->sram) */
            sts = ACM_BusBWAggregate(pNonQ6LpassSlpiMasterBw, &pBwDesc->bwValue);  
            if(Adsppm_Status_Success != sts)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal BW aggregation for nonQ6LpassSlpi failed: %u", sts);
            }
            
        }
        else
        {
            if ((ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled) &&
                  AdsppmBusPort_Ddr_Slave == pBwDesc->slavePort)
            {
				/* NonQ6 -> DDR :: LPASS->DDR : nonQ6LpassSlpiM + Lpass->ddr: C4 */
                sts = ACM_BusBWAggregate(pNonQ6LpassSlpiMasterBw, &pBwDesc->bwValue);  
            }
            if(!isByPassAggregation)
            {
				/* NonQ6 -> Others :: C3 (eg. LPASS Core internal, LPASS->DDR) */
				/* Includes LPASS->DDR as well since it is required for AHBI/X scaling */
                sts = ACM_BusBWAggregate(pNonAdspMasterBw, &pBwDesc->bwValue);
            }

            if(Adsppm_Status_Success != sts)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal BW aggregation failed: %u", sts);
            }
        }
        if (AdsppmBusPort_Q6DSP_Master != pBwDesc->masterPort &&
            Adsppm_Status_Success == sts)
            {
                // Aggregate all non-suppressible BW requests
                // All BW requirements originating from ADSP are considered
                // suppressible and should be excluded
				/* NonQ6 :: C5 (all apart from Q6) */
                sts = ACM_BusBWAggregate(pNonSuppressibleBw, &pBwDesc->bwValue);
                if(Adsppm_Status_Success != sts)
                {
                    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                        "internal BW aggregation failed: %u", sts);
                }


        }

    }
    else // if(requestClass == Adsppm_BwReqClass_Generic)
    {
        AdsppmBusBWDataIbAbType* pTotalBw;
        AdsppmBusBWDataIbAbType* pNonSuppressibleBw;
        AdsppmBusBWDataIbAbType* pQ6LpassSlpiMasterBw;
        
        pTotalBw =
            &gBusMgrCtx.intPreAggregateData.generic.totalBw;
        pNonSuppressibleBw =
            &gBusMgrCtx.intPreAggregateData.generic.nonSuppressibleBw;
        pQ6LpassSlpiMasterBw =
            &gBusMgrCtx.intPreAggregateData.generic.Q6LpassSlpiMasterBw;

		if(AdsppmBusPort_Q6DSP_Master == pBwDesc->masterPort)
        {
            if(AdsppmBusPort_Slpi_Sram_Slave == pBwDesc->slavePort)
            {  
            	/* Q6 -> SRAM :: G2 */
               sts = ACM_BusBWAggregate(pQ6LpassSlpiMasterBw, &pBwDesc->bwValue);
               if(Adsppm_Status_Success != sts)
               {
                  ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal genBW aggregation Slpi failed: %u", sts);
               }               
            }
            else
        	{
        		/* Q6 -> Others(except SRAM) :: G2 (eg: Q6 -> LPM/LPASS)*/
            	sts = ACM_BusBWAggregate(pQ6LpassSlpiMasterBw, &pBwDesc->bwValue);
            	if(Adsppm_Status_Success != sts)
            	{
                	ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    	"internal genBW aggregation failed: %u", sts);
            	}
				
				/* Q6 -> Others(except SRAM) :: G1 (eg: Q6 -> LPM/LPASS)*/
               sts = ACM_BusBWAggregate(pTotalBw, &pBwDesc->bwValue);
               if(Adsppm_Status_Success != sts)
               {
                  ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal genBW g1 aggregation Slpi failed: %u", sts);
               }  
        	}
        }
        else if ( (AdsppmBusPort_Sdc_Master == pBwDesc->masterPort) || 
             (AdsppmBusPort_Qup_Master == pBwDesc->masterPort))
        {
			/* QUP/SDC -> Others :: G2 (eg. SDC->DDR, SDC->SRAM, QUP->DDR, QUP->SRAM) */
           sts = ACM_BusBWAggregate(pQ6LpassSlpiMasterBw, &pBwDesc->bwValue);
        }
        else
        {
		   if(!isByPassAggregation)
           {
				if ((ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled) &&
					AdsppmBusPort_Ddr_Slave   == pBwDesc->slavePort)
				{
					/* NonQ6 -> DDR :: LPASS->DDR : G2 */
					sts = ACM_BusBWAggregate(pQ6LpassSlpiMasterBw, &pBwDesc->bwValue);  
				}
		   	
				/* NonQ6 -> Others :: G1 (eg. LPASS Core internal, LPASS->DDR) */
				/* Includes LPASS->DDR as well since it is required for AHBI/X scaling */
				sts = ACM_BusBWAggregate(pTotalBw, &pBwDesc->bwValue);
				
				if(Adsppm_Status_Success != sts)
				{
					ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
						"internal BW aggregation failed: %u", sts);
				}
		   }
		   
            
		}
        // Aggregate all non-suppressible BW requests
        // All BW requirements originating from ADSP are considered
        // suppressible and should be excluded
        if((Adsppm_Status_Success == sts) &&
           (AdsppmBusPort_Q6DSP_Master != pBwDesc->masterPort))
        {
            if(!isByPassAggregation)
            {
				/* NonQ6 :: G3 (all apart from Q6) */
                sts = ACM_BusBWAggregate(pNonSuppressibleBw, &pBwDesc->bwValue);
            }
            if(Adsppm_Status_Success != sts)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "internal non-supressible BW aggregation failed: %u", sts);
            }
        }
    }

    return sts;
}

static void busMgrAggregateInternalBW(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmBusBWDataIbAbType compTotalBw;
    AdsppmBusBWDataIbAbType compTotalNocBw;

    // Pointers for generic BW votes
    AdsppmBusBWDataIbAbType* pGenTotalBw =
        &gBusMgrCtx.intPreAggregateData.generic.totalBw;
    AdsppmBusBWDataIbAbType* pGenNSBw =
        &gBusMgrCtx.intPreAggregateData.generic.nonSuppressibleBw;
    AdsppmBusBWDataIbAbType* pGenQ6LpassSlpiBw =
        &gBusMgrCtx.intPreAggregateData.generic.Q6LpassSlpiMasterBw;
    
    // Pointers for compensated BW votes
    AdsppmBusBWDataIbAbType* pCompNSBw =
        &gBusMgrCtx.intPreAggregateData.compensated.nonSuppressibleBw;
    AdsppmBusBWDataIbAbType* pCompAdspBw =
        &gBusMgrCtx.intPreAggregateData.compensated.adspMasterBw;
    AdsppmBusBWDataIbAbType* pCompAdspSlpiBw =
        &gBusMgrCtx.intPreAggregateData.compensated.adspMasterSlpiBw;
    AdsppmBusBWDataIbAbType* pCompNonAdspBw =
        &gBusMgrCtx.intPreAggregateData.compensated.nonAdspMasterBw;
    AdsppmBusBWDataIbAbType* pCompNonQ6LpassSlpiBw =
        &gBusMgrCtx.intPreAggregateData.compensated.nonQ6LpassSlpiMasterBw;
    
    //
    // Intermediate aggregation for internal BW:
    // Aggregate BW for -compensated- BW class.
    // - inputs
    //   (a) pre-aggregated votes for ADSP master
    //   (b) pre-aggregated votes for !ADSP master
    // - compensation table is applied to (a)
    // - output to compTotalBw
    //

    compTotalBw.Ab = compTotalBw.Ib = 0;
    compTotalNocBw.Ab = compTotalNocBw.Ib = 0;
    // Ab: compensated(a) C1
    sts = ACM_ApplyAhbBwCompensation(pCompAdspBw->Ab, AsicBusType_AhbIx, &compTotalBw.Ab);
    if(sts == Adsppm_Status_Success)
    {
        if (pCompAdspSlpiBw->Ab > 0)
        {
			/* C1 + C2 */
           sts = ACM_ApplyAhbBwCompensation((pCompAdspSlpiBw->Ab + pCompAdspBw->Ab), AsicBusType_AhbE, &compTotalNocBw.Ab);
        }
        // Ab: compensated(a) + b  
		/* (C1 + C2) + C4(contains LPASS->DDR)*/
        compTotalNocBw.Ab += pCompNonQ6LpassSlpiBw->Ab;
		/* (C1) + C3 */
        compTotalBw.Ab += pCompNonAdspBw->Ab;
    }
    if(sts == Adsppm_Status_Success)
    {
        // Ib: compensated(a)
        sts = ACM_ApplyAhbBwCompensation(pCompAdspBw->Ib, AsicBusType_AhbIx, &compTotalBw.Ib);
        if(sts == Adsppm_Status_Success)
        {
           if (pCompAdspSlpiBw->Ib > 0)
           {
              sts = ACM_ApplyAhbBwCompensation(MAX(pCompAdspSlpiBw->Ib,pCompAdspBw->Ib), AsicBusType_AhbE, &compTotalNocBw.Ib);
           }
            // Ib: max(compensated(a), b)
            compTotalNocBw.Ib += MAX(compTotalNocBw.Ib, pCompNonQ6LpassSlpiBw->Ib);
            compTotalBw.Ib = MAX(compTotalBw.Ib, pCompNonAdspBw->Ib);
        }
    }

    //
    // Final aggregation for internal BW:
    //   Separately for total and non-suppressible BW:
    //     Ab = sum(generic vote, compensated vote)
    //     Ib = max(generic vote, compensated vote)
    //

    // Total ABH BW
    //
    // curAHBBwData may have initial values, so preserving 
	/* ((C1) + C3)+ G1 : AHBI/X -> C1 + C3 + G1 */
    gBusMgrCtx.curAHBBwData.Ab += compTotalBw.Ab + pGenTotalBw->Ab;
    gBusMgrCtx.curAHBBwData.Ib = MAX(gBusMgrCtx.curAHBBwData.Ib, compTotalBw.Ib);
    gBusMgrCtx.curAHBBwData.Ib = MAX(gBusMgrCtx.curAHBBwData.Ib, pGenTotalBw->Ib);

    // Non-suppressible BW.
    //
    // No compensation is applied, assuming non-suppressible BW never
    // has ADSP as master.
    // nonSuppressibleAhbBW may have initial values, so preserving
	/* C5 + G3 */
    gBusMgrCtx.nonSuppressibleAhbBW.Ab += pCompNSBw->Ab + pGenNSBw->Ab;
    gBusMgrCtx.nonSuppressibleAhbBW.Ib = MAX(gBusMgrCtx.nonSuppressibleAhbBW.Ib, pCompNSBw->Ib);
    gBusMgrCtx.nonSuppressibleAhbBW.Ib = MAX(gBusMgrCtx.nonSuppressibleAhbBW.Ib, pGenNSBw->Ib);

	/* ((C1 + C2) + C4)+ G2 :: AHBE:-> C1 + C2 + C4 + G2 */
    gBusMgrCtx.nocAhbBW.Ab += pGenQ6LpassSlpiBw->Ab + compTotalNocBw.Ab;
    gBusMgrCtx.nocAhbBW.Ib = MAX(gBusMgrCtx.nocAhbBW.Ib, compTotalNocBw.Ib);
    gBusMgrCtx.nocAhbBW.Ib = MAX(gBusMgrCtx.nocAhbBW.Ib, pGenQ6LpassSlpiBw->Ib);
	
	ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
                    "curr ib=%llu, ab=%llu",
                    (uint32) gBusMgrCtx.curAHBBwData.Ib,
                    (uint32) (gBusMgrCtx.curAHBBwData.Ib >> 32),
                    (uint32) gBusMgrCtx.curAHBBwData.Ab,
                    (uint32) (gBusMgrCtx.curAHBBwData.Ab >> 32));
	ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
                    "non ib=%llu, ab=%llu",
                    (uint32) gBusMgrCtx.nonSuppressibleAhbBW.Ib,
                    (uint32) (gBusMgrCtx.nonSuppressibleAhbBW.Ib >> 32),
                    (uint32) gBusMgrCtx.nonSuppressibleAhbBW.Ab,
                    (uint32) (gBusMgrCtx.nonSuppressibleAhbBW.Ab >> 32));
	ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
                    "noc ib=%llu, ab=%llu",
                    (uint32) gBusMgrCtx.nocAhbBW.Ib,
                    (uint32) (gBusMgrCtx.nocAhbBW.Ib >> 32),
                    (uint32) gBusMgrCtx.nocAhbBW.Ab,
                    (uint32) (gBusMgrCtx.nocAhbBW.Ab >> 32));
    
}

static Adsppm_Status busMgrPreAggregateExternalBW(
    BUSMgr_BwDescriptorType *pBwDesc,
    AdsppmBwRequestClass requestClass)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 extRouteIndex = 0;

    // Search corresponding external route
    for(extRouteIndex = 0; extRouteIndex < gBusMgrCtx.extRouteNum; extRouteIndex++)
    {
        if((gBusMgrCtx.pExtRoutes[extRouteIndex].masterPort.adsppmMaster == pBwDesc->masterPort) &&
           (gBusMgrCtx.pExtRoutes[extRouteIndex].slavePort.adsppmSlave == pBwDesc->slavePort))
        {
            if(requestClass == Adsppm_BwReqClass_Compensated)
            {
                sts = ACM_BusBWAggregate(
                    &(gBusMgrCtx.extPreAggregateData.pCompensated[extRouteIndex].bwValue.busBwAbIb),
                    &(pBwDesc->bwValue));

                if((sts == Adsppm_Status_Success) &&
                    (pBwDesc->masterPort == AdsppmBusPort_Q6DSP_Master ) &&
                    (pBwDesc->slavePort == AdsppmBusPort_Ddr_Slave))
                {
                    gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes += 1;

                    // Get max ADSP to DDR BW requested
                    gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw =
                        MAX(gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw,
                        pBwDesc->bwValue.busBwValue.bwBytePerSec);
                }
            }
            else // if(requestClass == Adsppm_BwReqClass_Generic)
            {
                sts = ACM_BusBWAggregate(
                    &(gBusMgrCtx.extPreAggregateData.pGeneric[extRouteIndex].bwValue.busBwAbIb),
                    &(pBwDesc->bwValue));
            }
            if(Adsppm_Status_Success != sts)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "External BW pre-aggregation failed: %u", sts);
            }
            break;
        }
    }

    if(gBusMgrCtx.extRouteNum == extRouteIndex)
    {
        ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
            "Invalid external route, M=%u, S=%u",
            pBwDesc->masterPort, pBwDesc->slavePort);
        sts = Adsppm_Status_BadParm;
    }

    return sts;
}

void busMgrUpdateMemNocVote(AdsppmBusBWRequestValueType *pAggregate,uint64 AgQ6ClientVote,
                                      uint64 FinalQ6ClientVoteAb, uint64 FinalQ6ClientVoteIb)
{
  AsicSnocVoteTableEntryType snocTableEntry;

  //Get Entry
  ACM_GetSnocVoteTableEntry(AgQ6ClientVote,
                            &snocTableEntry);

  if(snocTableEntry.honestFlag == 0)
  {
  pAggregate->bwValue.busBwAbIb.Ab        = snocTableEntry.snocVoteAb;
  }
  else
    pAggregate->bwValue.busBwAbIb.Ab  = FinalQ6ClientVoteAb;

  pAggregate->bwValue.busBwAbIb.Ib        = snocTableEntry.snocVoteIb;
  pAggregate->bwValue.busBwAbIb.latencyNs = snocTableEntry.latencyVote;

  
  gBusMgrCtx.falTolerance[gBusMgrCtx.preCurrentIndex] =
      snocTableEntry.falTolerance;
  
  gBusMgrCtx.idleTime[gBusMgrCtx.preCurrentIndex] =
      snocTableEntry.idleTime;

  
  ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
        "BW Values for SNOC Slave path : ib=%llu, ab=%llu",
        (uint32) pAggregate->bwValue.busBwAbIb.Ib,
        (uint32) (pAggregate->bwValue.busBwAbIb.Ib >> 32),
        (uint32) pAggregate->bwValue.busBwAbIb.Ab,
        (uint32) (pAggregate->bwValue.busBwAbIb.Ab >> 32));

}

void busMgrCopyAhbVote(AdsppmBusBWRequestValueType *pAggregate, AdsppmBusBWRequestValueType *pAggregateAhbMaster)
{
  //Make of copy of Ext Ahb master vote to provide split vote
  pAggregateAhbMaster->bwValue.busBwAbIb.Ab = pAggregate->bwValue.busBwAbIb.Ab;
  pAggregateAhbMaster->bwValue.busBwAbIb.Ib = pAggregate->bwValue.busBwAbIb.Ib;
  pAggregateAhbMaster->bwValue.busBwAbIb.latencyNs = pAggregate->bwValue.busBwAbIb.latencyNs;

       
}


void busMgrUpdateFloorVote(uint64 Ab, uint64 Ib)
{
   
   gBusMgrCtx.dcvsExtBwFloorVote[Adsppm_DCVS_Route_DSP_DDR][gBusMgrCtx.preCurrentIndex]->Ab += Ab;
 
   gBusMgrCtx.dcvsExtBwFloorVote[Adsppm_DCVS_Route_DSP_DDR][gBusMgrCtx.preCurrentIndex]->Ib =  
                                MAX (Ib, gBusMgrCtx.dcvsExtBwFloorVote[Adsppm_DCVS_Route_DSP_DDR][gBusMgrCtx.preCurrentIndex]->Ib);
}


static void busMgrAggregateExternalBW_CallBack(coreUtils_Q_Type *pBWReqQ)
{
    uint32 extRouteIndex = 0, scalingfactor=0;
    AdsppmBusBWRequestValueType *pAggregate;
    AdsppmBusBWRequestValueType *pPreAggregateGeneric;
    AdsppmBusBWRequestValueType *pPreAggregateCompensated;
    AsicCompensatedDdrBwTableEntryType tableEntry = {0,0,0,0,0,0,};
    uint64 tempPreAggregateCompensatedAb = 0,tempPreAggregateCompensatedIb = 0;
    uint32 tempPreAggregateCompensatedLatency = 0;
    uint64 GenericQ6DdrBwVoteAb = 0, GenericQ6DdrBwVoteIb = 0, bwVoteForSnocAb = 0, bwVoteForSnocIb = 0;
    uint64 GenericAhbDdrBwVoteAb = 0, GenericAhbDdrBwVoteIb = 0;
    AsicCompensatedBWFudgeFactor compensatedBWFudgeFactorValues;
    AdsppmBusBWRequestValueType AggregateAhbMaster;

    // For each route:
    // Ab = sum(generic vote, compensated vote)
    // Ib = max(generic vote, compensated vote)
    for(extRouteIndex = 0; extRouteIndex < gBusMgrCtx.extRouteNum; extRouteIndex++)
    {

        pAggregate = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex][extRouteIndex]);
        pPreAggregateGeneric = &(gBusMgrCtx.extPreAggregateData.pGeneric[extRouteIndex]);
        pPreAggregateCompensated = &(gBusMgrCtx.extPreAggregateData.pCompensated[extRouteIndex]);

        // Compensated BW vote class: apply compensated vote table
        // Compensation only applies if Master port is ADSP and
        // slave port is DDR; otherwise treat as uncompensated
		/* Should be applied to only audio voice clients */
        if( (pAggregate->busRoute.masterPort == AdsppmBusPort_Q6DSP_Master) &&
            (pAggregate->busRoute.slavePort == AdsppmBusPort_Ddr_Slave) && 
            (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Disabled) && 
            (ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Disabled) &&
			(ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled))
        {
            //Fudge Factor should only be applied--if there is a new request
            //If fudge factor is called for cache size change initiated by new BW request, this part should be ignored
            if(gBusMgrCtx.externalBwUpdatePending == FALSE)
            {
                    // Apply BW concurrency factor before compensated vote table
                    ACM_ApplyAdspDdrBwConcurrencyFactor(
                        gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes,
                        gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw,
                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ab,
                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ib,
                                        &compensatedBWFudgeFactorValues);

                    if(compensatedBWFudgeFactorValues.floorFudgeFactor && compensatedBWFudgeFactorValues.ceilingFudgeFactor)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                        "Ab value is more than threshold\n");
                        pPreAggregateCompensated->bwValue.busBwAbIb.Ab = gBusMgrCtx.bwAfterCompensatedFFAb;
                        pPreAggregateCompensated->bwValue.busBwAbIb.Ib = gBusMgrCtx.bwAfterCompensatedFFIb;
                    }
                
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                    "Concurrency Factor Calculation Skipped\n");
            }
            tempPreAggregateCompensatedAb = pPreAggregateCompensated->bwValue.busBwAbIb.Ab;
            tempPreAggregateCompensatedIb = pPreAggregateCompensated->bwValue.busBwAbIb.Ib;
            scalingfactor = gBusMgrCtx.scalingfactor;
            tempPreAggregateCompensatedAb = ((tempPreAggregateCompensatedAb * (uint64) scalingfactor)/100ull);
            tempPreAggregateCompensatedIb = ((tempPreAggregateCompensatedIb * (uint64) scalingfactor)/100ull);
            ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                    "BW Values after scaling factor: ib=%llu, ab=%llu",
                    (uint32) tempPreAggregateCompensatedIb,
                    (uint32) (tempPreAggregateCompensatedIb >> 32),
                    (uint32) tempPreAggregateCompensatedAb,
                    (uint32) (tempPreAggregateCompensatedAb >> 32));
					
			bwVoteForSnocAb = tempPreAggregateCompensatedAb;
            bwVoteForSnocIb = tempPreAggregateCompensatedIb;

            ACM_GetCompensatedDdrBwTableEntry(
                MAX(tempPreAggregateCompensatedIb,
                tempPreAggregateCompensatedAb),
                &tableEntry);


            gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex] =
                tableEntry.snocFloorVoteMhz;

            tempPreAggregateCompensatedLatency = tableEntry.latencyVote;

            // Dishonest ib vote selected
            if(tableEntry.honestFlag == 0)
            {
                // Take table's Ab vote as is
                tempPreAggregateCompensatedAb = tableEntry.bwAbVote;
            }
     
            tempPreAggregateCompensatedIb = tableEntry.bwIbVote;


            pAggregate->bwValue.busBwAbIb.Ab        = tempPreAggregateCompensatedAb;
            pAggregate->bwValue.busBwAbIb.Ib        = tempPreAggregateCompensatedIb;
            pAggregate->bwValue.busBwAbIb.latencyNs = tempPreAggregateCompensatedLatency;
            
            //Update floor vote as per agreement with dcvs/arch team
            //Compensated votes would be only UP_ONLY clients; hence, Ab floor vote is: Generic floor Up_only + compensated final vote(all votes are UP_ONLY)
            //Similarly, Ib is MAX(generic up_only Ib, compensated up_only Ib)
              
            busMgrUpdateFloorVote(pAggregate->bwValue.busBwAbIb.Ab, pAggregate->bwValue.busBwAbIb.Ib);

            
			//Make a copy of Q6 master generic vote for Snoc master
            GenericQ6DdrBwVoteAb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
            GenericQ6DdrBwVoteIb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ib;
        }
        else
        {
            pAggregate->bwValue.busBwAbIb.Ab        = pPreAggregateCompensated->bwValue.busBwAbIb.Ab;
            pAggregate->bwValue.busBwAbIb.Ib        = pPreAggregateCompensated->bwValue.busBwAbIb.Ib;
            pAggregate->bwValue.busBwAbIb.latencyNs = pPreAggregateCompensated->bwValue.busBwAbIb.latencyNs;
        }


        //This is a dummy bus port, AdsppmBusPort_Snoc_Master, idea is: Q6 Master/DDr Slave would be divided into two
        //votes: Q6 Master/SNoc SLave and Snoc Master/DDr Slave to do split voting

        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Snoc_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Snoc_Slave)
        {

          busMgrUpdateMemNocVote(pAggregate,MAX(bwVoteForSnocAb,bwVoteForSnocIb),
                                             bwVoteForSnocAb,
                                             bwVoteForSnocIb);
          //Update snoc master generic vote
          pPreAggregateGeneric->bwValue.busBwAbIb.Ab = GenericQ6DdrBwVoteAb;
          pPreAggregateGeneric->bwValue.busBwAbIb.Ib = GenericQ6DdrBwVoteIb;
  
        }
        //This is a dummy bus port, AdsppmBusPort_Snoc_Ahb_Master, 
        // idea is: Q6 Master/DDr Slave would be divided into two
        // votes: Ahb Master/SNoc SLave and Snoc Master/DDr Slave to do split voting
        //Since Ahb master vote and Snoc vote vote would be same, Ahb master vote is saved and copied for Snoc Slave
        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Ext_Ahb_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Ddr_Slave)
        {
           busMgrCopyAhbVote(pAggregate, &AggregateAhbMaster);
           //Update Ahb vote for higher Ahb master to Snoc vote
           ACM_GetCompensatedDdrBwTableEntry(MAX(pAggregate->bwValue.busBwAbIb.Ab,
                                                 pAggregate->bwValue.busBwAbIb.Ib),
                                                 &tableEntry);
           // Dishonest ib vote selected
           if(tableEntry.honestFlag == 0)
           {
               // Map ib to dishonest vote from table
               pAggregate->bwValue.busBwAbIb.Ab = tableEntry.bwAbVote;
           }
    
           pAggregate->bwValue.busBwAbIb.Ib        = tableEntry.bwIbVote;
           pAggregate->bwValue.busBwAbIb.latencyNs = tableEntry.latencyVote; 
		   //Copy Ahb master generic vote
           GenericAhbDdrBwVoteAb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
           GenericAhbDdrBwVoteIb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ib;
        }
    
        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Snoc_Ahb_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Snoc_Slave)
        {

           busMgrUpdateMemNocVote(pAggregate,MAX(AggregateAhbMaster.bwValue.busBwAbIb.Ab,
                                                 AggregateAhbMaster.bwValue.busBwAbIb.Ib),
                                             AggregateAhbMaster.bwValue.busBwAbIb.Ab,
                                             AggregateAhbMaster.bwValue.busBwAbIb.Ib);
		   //Update snoc ahb master generic vote
           pPreAggregateGeneric->bwValue.busBwAbIb.Ab = GenericAhbDdrBwVoteAb;
           pPreAggregateGeneric->bwValue.busBwAbIb.Ib = GenericAhbDdrBwVoteIb;								 
											
        }
		
		ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
                                   "Generic BW Values: ib=%llu, ab=%llu for master port %s",
                                   (uint32) pPreAggregateGeneric->bwValue.busBwAbIb.Ib,
                                   (uint32) (pPreAggregateGeneric->bwValue.busBwAbIb.Ib >> 32),
                                   (uint32) pPreAggregateGeneric->bwValue.busBwAbIb.Ab,
                                   (uint32) (pPreAggregateGeneric->bwValue.busBwAbIb.Ab >> 32),
                                            adsppm_getBusPortName(pAggregate->busRoute.masterPort));
        // Generic BW vote class (no compensation)
        pAggregate->bwValue.busBwAbIb.Ab += pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
        pAggregate->bwValue.busBwAbIb.Ib = MAX(
            pPreAggregateGeneric->bwValue.busBwAbIb.Ib,
            pAggregate->bwValue.busBwAbIb.Ib);



    }
}

static void busMgrAggregateExternalBW(coreUtils_Q_Type *pBWReqQ)
{
    uint32 extRouteIndex = 0, scalingfactor=0;
    AdsppmBusBWRequestValueType *pAggregate;
    AdsppmBusBWRequestValueType *pPreAggregateGeneric;
    AdsppmBusBWRequestValueType *pPreAggregateCompensated;
    AsicCompensatedDdrBwTableEntryType tableEntry;
    uint64 tempPreAggregateCompensatedAb = 0,tempPreAggregateCompensatedIb = 0;
    uint32 tempPreAggregateCompensatedLatency = 0;
    uint64 GenericQ6DdrBwVoteAb = 0, GenericQ6DdrBwVoteIb = 0, bwVoteForSnocAb = 0, bwVoteForSnocIb = 0;
    uint64 GenericAhbDdrBwVoteAb = 0, GenericAhbDdrBwVoteIb = 0;
    AsicCompensatedBWFudgeFactor compensatedBWFudgeFactorValues;
    AdsppmBusBWRequestValueType AggregateAhbMaster;

    // For each route:
    // Ab = sum(generic vote, compensated vote)
    // Ib = max(generic vote, compensated vote)
    for(extRouteIndex = 0; extRouteIndex < gBusMgrCtx.extRouteNum; extRouteIndex++)
    {

        pAggregate = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex][extRouteIndex]);
        pPreAggregateGeneric = &(gBusMgrCtx.extPreAggregateData.pGeneric[extRouteIndex]);
        pPreAggregateCompensated = &(gBusMgrCtx.extPreAggregateData.pCompensated[extRouteIndex]);

        // Compensated BW vote class: apply compensated vote table
        // Compensation only applies if Master port is ADSP and
        // slave port is DDR; otherwise treat as uncompensated
		/* Should be applied to only audio voice clients */
        if( (pAggregate->busRoute.masterPort == AdsppmBusPort_Q6DSP_Master) &&
            (pAggregate->busRoute.slavePort == AdsppmBusPort_Ddr_Slave) && 
            (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Disabled) && 
            (ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Disabled) &&
			(ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled))
        {
            //Fudge Factor should only be applied--if there is a new request
            //If fudge factor is called for cache size change initiated by new BW request, this part should be ignored
            if(gBusMgrCtx.externalBwUpdatePending == FALSE)
            {
                    // Apply BW concurrency factor before compensated vote table
                    ACM_ApplyAdspDdrBwConcurrencyFactor(
                        gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes,
                        gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw,
                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ab,
                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ib,
                                        &compensatedBWFudgeFactorValues);

                    if(compensatedBWFudgeFactorValues.floorFudgeFactor && compensatedBWFudgeFactorValues.ceilingFudgeFactor)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                        "Ab value is more than threshold\n");
                        BusMgr_GetCompensatedFudgedBwValue (pBWReqQ, 
                                                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ab,
                                                        &pPreAggregateCompensated->bwValue.busBwAbIb.Ib,
                                                        &compensatedBWFudgeFactorValues);
                    }
                
            }
            else
            {
                ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                    "Concurrency Factor Calculation Skipped\n");
            }
            tempPreAggregateCompensatedAb = pPreAggregateCompensated->bwValue.busBwAbIb.Ab;
            tempPreAggregateCompensatedIb = pPreAggregateCompensated->bwValue.busBwAbIb.Ib;
            scalingfactor = gBusMgrCtx.scalingfactor;
            tempPreAggregateCompensatedAb = ((tempPreAggregateCompensatedAb * (uint64) scalingfactor)/100ull);
            tempPreAggregateCompensatedIb = ((tempPreAggregateCompensatedIb * (uint64) scalingfactor)/100ull);
            ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                    "BW Values after scaling factor: ib=%llu, ab=%llu",
                    (uint32) tempPreAggregateCompensatedIb,
                    (uint32) (tempPreAggregateCompensatedIb >> 32),
                    (uint32) tempPreAggregateCompensatedAb,
                    (uint32) (tempPreAggregateCompensatedAb >> 32));
					
			bwVoteForSnocAb = tempPreAggregateCompensatedAb;
            bwVoteForSnocIb = tempPreAggregateCompensatedIb;

            ACM_GetCompensatedDdrBwTableEntry(
                MAX(tempPreAggregateCompensatedIb,
                tempPreAggregateCompensatedAb),
                &tableEntry);


            gBusMgrCtx.snocFloorVoteMhz[gBusMgrCtx.preCurrentIndex] =
                tableEntry.snocFloorVoteMhz;

            tempPreAggregateCompensatedLatency = tableEntry.latencyVote;

            // Dishonest ib vote selected
            if(tableEntry.honestFlag == 0)
            {
                // Map ib to dishonest vote from table
                tempPreAggregateCompensatedAb = tableEntry.bwAbVote;
            }
     
            tempPreAggregateCompensatedIb = tableEntry.bwIbVote;


            pAggregate->bwValue.busBwAbIb.Ab        = tempPreAggregateCompensatedAb;
            pAggregate->bwValue.busBwAbIb.Ib        = tempPreAggregateCompensatedIb;
            pAggregate->bwValue.busBwAbIb.latencyNs = tempPreAggregateCompensatedLatency;
            
            //Update floor vote as per agreement with dcvs/arch team
            //Compensated votes would be only UP_ONLY clients; hence, Ab floor vote is: Generic floor Up_only + compensated final vote(all votes are UP_ONLY)
            //Similarly, Ib is MAX(generic up_only Ib, compensated up_only Ib)
              
            busMgrUpdateFloorVote(pAggregate->bwValue.busBwAbIb.Ab, pAggregate->bwValue.busBwAbIb.Ib);

            
			//Make a copy of Q6 master generic vote
            GenericQ6DdrBwVoteAb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
            GenericQ6DdrBwVoteIb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ib;
        }
        else
        {
            pAggregate->bwValue.busBwAbIb.Ab        = pPreAggregateCompensated->bwValue.busBwAbIb.Ab;
            pAggregate->bwValue.busBwAbIb.Ib        = pPreAggregateCompensated->bwValue.busBwAbIb.Ib;
            pAggregate->bwValue.busBwAbIb.latencyNs = pPreAggregateCompensated->bwValue.busBwAbIb.latencyNs;
        }


        //This is a dummy bus port, AdsppmBusPort_Snoc_Master, idea is: Q6 Master/DDr Slave would be divided into two
        //votes: Q6 Master/SNoc SLave and Snoc Master/DDr Slave to do split voting

        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Snoc_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Snoc_Slave)
        {

           busMgrUpdateMemNocVote(pAggregate,MAX(bwVoteForSnocAb,bwVoteForSnocIb),
                                             bwVoteForSnocAb,
                                             bwVoteForSnocIb);
           //Update generic vote for snoc master
           pPreAggregateGeneric->bwValue.busBwAbIb.Ab = GenericQ6DdrBwVoteAb;
           pPreAggregateGeneric->bwValue.busBwAbIb.Ib = GenericQ6DdrBwVoteIb;
        }

        //This is a dummy bus port, AdsppmBusPort_Snoc_Ahb_Master, 
        // idea is: Q6 Master/DDr Slave would be divided into two
        // votes: Ahb Master/SNoc SLave and Snoc Master/DDr Slave to do split voting
        //Since Ahb master vote and Snoc vote vote would be same, Ahb master vote is saved and copied for Snoc Slave

        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Ext_Ahb_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Ddr_Slave)
        {
           //Make a copy of Ahb Master vote to copy in snoc vote
           busMgrCopyAhbVote(pAggregate, &AggregateAhbMaster);
           ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                                   "BW Values for Ahb Master-SNOC path : ib=%llu, ab=%llu",
                                   (uint32) AggregateAhbMaster.bwValue.busBwAbIb.Ib,
                                   (uint32) (AggregateAhbMaster.bwValue.busBwAbIb.Ib >> 32),
                                   (uint32) AggregateAhbMaster.bwValue.busBwAbIb.Ab,
                                   (uint32) (AggregateAhbMaster.bwValue.busBwAbIb.Ab >> 32)); 
           //Update Ahb vote for higher Ahb master to Snoc vote
           ACM_GetCompensatedDdrBwTableEntry(MAX(pAggregate->bwValue.busBwAbIb.Ab,
                                                 pAggregate->bwValue.busBwAbIb.Ib),
                                                 &tableEntry);
           // Dishonest ib vote selected
           if(tableEntry.honestFlag == 0)
           {
               // Map ib to dishonest vote from table
               pAggregate->bwValue.busBwAbIb.Ab = tableEntry.bwAbVote;
           }
           
           pAggregate->bwValue.busBwAbIb.Ib        = tableEntry.bwIbVote;
           pAggregate->bwValue.busBwAbIb.latencyNs = tableEntry.latencyVote;
		   
		   //Make a copy of generic Ahb master vote
           GenericAhbDdrBwVoteAb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
           GenericAhbDdrBwVoteIb                    = pPreAggregateGeneric->bwValue.busBwAbIb.Ib;
        }
        
        if(pAggregate->busRoute.masterPort == AdsppmBusPort_Snoc_Ahb_Master &&
           pAggregate->busRoute.slavePort == AdsppmBusPort_Snoc_Slave)
        {
           //Update Snoc Master vote
           busMgrUpdateMemNocVote(pAggregate,MAX(AggregateAhbMaster.bwValue.busBwAbIb.Ab,
                                                 AggregateAhbMaster.bwValue.busBwAbIb.Ib),
                                             AggregateAhbMaster.bwValue.busBwAbIb.Ab,
                                             AggregateAhbMaster.bwValue.busBwAbIb.Ib);
           ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
                                   "BW Values for Snoc Master-SNOC path : ib=%llu, ab=%llu",
                                   (uint32) pAggregate->bwValue.busBwAbIb.Ib,
                                   (uint32) (pAggregate->bwValue.busBwAbIb.Ib >> 32),
                                   (uint32) pAggregate->bwValue.busBwAbIb.Ab,
                                   (uint32) (pAggregate->bwValue.busBwAbIb.Ab >> 32));
		   //Update snoc ahb master vote
           pPreAggregateGeneric->bwValue.busBwAbIb.Ab = GenericAhbDdrBwVoteAb;
           pPreAggregateGeneric->bwValue.busBwAbIb.Ib = GenericAhbDdrBwVoteIb;

        }
		ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
									"BW Ports: MasterPort %u, SlavePort %u", pAggregate->busRoute.masterPort,pAggregate->busRoute.slavePort);
		ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_INFO,
                                   "Generic BW Values: ib=%llu, ab=%llu for master port %s slave port %s",
                                   (uint32) pPreAggregateGeneric->bwValue.busBwAbIb.Ib,
                                   (uint32) (pPreAggregateGeneric->bwValue.busBwAbIb.Ib >> 32),
                                   (uint32) pPreAggregateGeneric->bwValue.busBwAbIb.Ab,
                                   (uint32) (pPreAggregateGeneric->bwValue.busBwAbIb.Ab >> 32),
                                            adsppm_getBusPortName(pAggregate->busRoute.masterPort),
                                            adsppm_getBusPortName(pAggregate->busRoute.slavePort));
        // Generic BW vote class (no compensation)
        pAggregate->bwValue.busBwAbIb.Ab += pPreAggregateGeneric->bwValue.busBwAbIb.Ab;
        pAggregate->bwValue.busBwAbIb.Ib = MAX( pPreAggregateGeneric->bwValue.busBwAbIb.Ib,
                                                pAggregate->bwValue.busBwAbIb.Ib);

    }
}

static void busMgrVoteForAhbBusClocks(AsicBusPortConnectionType portConnection)
{
    if((portConnection & (AsicBusPort_AhbE_M | AsicBusPort_AhbE_S)) && ACM_IsClkIdValid(AdsppmClk_AhbE_Hclk))
    {
        gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][AdsppmClk_AhbE_Hclk] = Hal_ClkRgmEnable;
    }
    if((portConnection & (AsicBusPort_AhbI_M | AsicBusPort_AhbI_S)) && ACM_IsClkIdValid(AdsppmClk_AhbI_Hclk))
    {
        gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][AdsppmClk_AhbI_Hclk] = Hal_ClkRgmEnable;
    }
    if((portConnection & (AsicBusPort_AhbX_M | AsicBusPort_AhbX_S)) && ACM_IsClkIdValid(AdsppmClk_AhbX_Hclk))
    {
        gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][AdsppmClk_AhbX_Hclk] = Hal_ClkRgmEnable;
    }
}


static Adsppm_Status busMgrVoteForRegProgClock(AdsppmClkIdType clkId)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    const AsicClkDescriptorType *pClkDesc = ACMClk_GetClockDescriptorById(clkId);

    if(NULL == pClkDesc)
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
            "Clock descriptor is NULL, clkId=%u", clkId);
        sts = Adsppm_Status_Failed;
    }
    else
    {
        switch(pClkDesc->clkCntlType)
        {
        case AsicClk_CntlSW:
            gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][clkId] = Hal_ClkRgmEnable;
            break;

        case AsicClk_CntlSW_DCG:
            if(AsicFeatureState_Enabled != ACM_GetFeatureStatus(AsicFeatureId_Ahb_DCG))
            {
                gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][clkId] = Hal_ClkRgmEnable;
            }
            break;

        default:
            break;
        }
    }

    return sts;
}


static Adsppm_Status busMgrAggregateClocks(BUSMgr_BwDescriptorType *pBwDesc)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    ADSPPM_LOG_PRINTF_3(ADSPPM_LOG_LEVEL_INFO_EXT,
        "clkM=%u, clkS=%u, clkA=%u",
        pBwDesc->pMPortDescriptor->busClk,
        pBwDesc->pSPortDescriptor->busClk,
        pBwDesc->pAPortDescriptor->busClk);

    if(AdsppmBusBWOperation_RegAccess == pBwDesc->bwOperation)
    {
        uint32 index = 0;
        sts = busMgrVoteForRegProgClock(pBwDesc->pMPortDescriptor->busClk);

        // Aggregate register programming clocks voting from Slave Port
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
            "num of regProgClocks=%u",
            pBwDesc->pSPortDescriptor->regProgClocks.numClocks);

        for(index = 0;
            (Adsppm_Status_Success == sts) &&
                (index < pBwDesc->pSPortDescriptor->regProgClocks.numClocks);
            index++)
        {
            sts = busMgrVoteForRegProgClock(pBwDesc->pSPortDescriptor->regProgClocks.pClocks[index]);
        }
    }
    else // pBwDesc->bwOperation == AdsppmBusBWOperation_BW
    {
    if (ACM_IsClkIdValid(pBwDesc->pMPortDescriptor->busClk))
      gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][pBwDesc->pMPortDescriptor->busClk] = Hal_ClkRgmEnable;
    if (ACM_IsClkIdValid(pBwDesc->pSPortDescriptor->busClk))
      gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][pBwDesc->pSPortDescriptor->busClk] = Hal_ClkRgmEnable;
    if (ACM_IsClkIdValid(pBwDesc->pAPortDescriptor->busClk))
      gBusMgrCtx.clockState[gBusMgrCtx.preCurrentIndex][pBwDesc->pAPortDescriptor->busClk] = Hal_ClkRgmEnable;
    }

    /////////// CATL --- This section below may not be applicable for CDSPPM.
    //// It depends on which clocks drive the internal/external bus.
    //// However - we need to remove it otherwise CDSPPM will CRASH because these functions enable clocks
    /// AdsppmClk_AhbE_Hclk, AdsppmClk_AhbI_Hclk, which are not available in CDSPPM
    if(Adsppm_Status_Success == sts)
    {
        busMgrVoteForAhbBusClocks(pBwDesc->pMPortDescriptor->portConnection);
        busMgrVoteForAhbBusClocks(pBwDesc->pSPortDescriptor->portConnection);
        busMgrVoteForAhbBusClocks(pBwDesc->pAPortDescriptor->portConnection);
    }

    return sts;
}


static Adsppm_Status busMgrAggregateRequest(
    AdsppmBusBWRequestValueType *pBwRequest,
    AdsppmBwRequestClass requestClass)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    BUSMgr_BwDescriptorType bwDesc;
    BUSMgrBwTypeType busMgrBwType = BUSMgr_Bw_None;

    bwDesc.masterPort = pBwRequest->busRoute.masterPort;
    bwDesc.slavePort = pBwRequest->busRoute.slavePort;
    bwDesc.accessPort = AdsppmBusPort_None;
    bwDesc.pMPortDescriptor = ACMBus_GetPortDescriptor(bwDesc.masterPort);
    bwDesc.pSPortDescriptor = ACMBus_GetPortDescriptor(bwDesc.slavePort);
    bwDesc.pAPortDescriptor = ACMBus_GetPortDescriptor(bwDesc.accessPort);
    bwDesc.bwOperation = pBwRequest->bwOperation;
    bwDesc.bwValue = pBwRequest->bwValue;

    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO_EXT,
        "M=%s(%u), S=%s(%u)",
        adsppm_getBusPortName(bwDesc.masterPort), bwDesc.masterPort,
        adsppm_getBusPortName(bwDesc.slavePort), bwDesc.slavePort);

    if((NULL == bwDesc.pMPortDescriptor) ||
        (NULL == bwDesc.pSPortDescriptor) ||
        (NULL == bwDesc.pAPortDescriptor))
    {
        ADSPPM_LOG_PRINTF_6(ADSPPM_LOG_LEVEL_ERROR,
            "Bus route descriptor is NULL, M=%s(%u), S=%s(%u), A=%s(%u)",
            adsppm_getBusPortName(bwDesc.masterPort), bwDesc.masterPort,
            adsppm_getBusPortName(bwDesc.slavePort), bwDesc.slavePort,
            adsppm_getBusPortName(bwDesc.accessPort), bwDesc.accessPort);
        sts = Adsppm_Status_BadParm;
    }
    else
    {
        busMgrBwType = busMgrGetBwType(
            bwDesc.pMPortDescriptor->portConnection,
            bwDesc.pSPortDescriptor->portConnection);

        switch(busMgrBwType)
        {
        case BUSMgr_Bw_None:
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_ERROR,
                "None BW type, connectM=%u, connectS=%u",
                bwDesc.pMPortDescriptor->portConnection,
                bwDesc.pSPortDescriptor->portConnection);
            sts = Adsppm_Status_Failed;
            break;

        case BUSMgr_Bw_Internal_External:
		    if(Adsppm_Status_Success == sts)
            {
                sts = busMgrPreAggregateInternalBW(&bwDesc, requestClass);
            }

            if(0 != (AsicBusPort_Ext_M & bwDesc.pMPortDescriptor->portConnection))
            {
                bwDesc.slavePort = bwDesc.pSPortDescriptor->accessPort;
                bwDesc.accessPort = bwDesc.slavePort;
            }
            else if(0 != (AsicBusPort_Ext_S & bwDesc.pSPortDescriptor->portConnection))
            {
                bwDesc.masterPort = bwDesc.pMPortDescriptor->accessPort;
                bwDesc.accessPort = bwDesc.masterPort;
            }
            bwDesc.pAPortDescriptor = ACMBus_GetPortDescriptor(bwDesc.accessPort);
			
			ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
					"updated M=%s(%u), S=%s(%u)",
					adsppm_getBusPortName(bwDesc.masterPort), bwDesc.masterPort,
					adsppm_getBusPortName(bwDesc.slavePort), bwDesc.slavePort);

            if(NULL == bwDesc.pAPortDescriptor)
            {
                ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_ERROR,
                    "Access port descriptor is NULL, accessPort:%u",
                    bwDesc.accessPort);
                sts = Adsppm_Status_Failed;
            }
            if(Adsppm_Status_Success == sts)
            {
                sts = busMgrAggregateClocks(&bwDesc);
            }
            if(Adsppm_Status_Success == sts)
            {
                sts = busMgrPreAggregateExternalBW(&bwDesc, requestClass);
            }
            break;

        case BUSMgr_Bw_Internal_Only:
            sts = busMgrPreAggregateInternalBW(&bwDesc, requestClass);
            if(Adsppm_Status_Success == sts)
            {
                sts = busMgrAggregateClocks(&bwDesc);
            }
            break;

        case BUSMgr_Bw_External_Only:
            sts = busMgrPreAggregateExternalBW(&bwDesc, requestClass);
            break;

        default:
             break;
        }
    }

    return sts;
}

Adsppm_Status BusMgr_GetAggregatedBwInfo(AdsppmInfoAggregatedBwType *pBusInfo)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    int index = 0;
    AdsppmBusBWRequestValueType* pBwVote = NULL;

    if(pBusInfo == NULL)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pBusInfo is NULL");
        return Adsppm_Status_BadParm;
    }

    if(gBusMgrCtx.lock == NULL)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Not initialized");
        return Adsppm_Status_NotInitialized;
    }

    // Lock for bus manager global data update
    adsppm_lock(gBusMgrCtx.lock);

    for(index = 0; index < gBusMgrCtx.extRouteNum; index++)
    {
        pBwVote = &(gBusMgrCtx.pExtBwAggregateData[gBusMgrCtx.preCurrentIndex^1][index]);

        if((AdsppmBusPort_Adsp_Master == pBwVote->busRoute.masterPort) &&
            (AdsppmBusPort_Ddr_Slave == pBwVote->busRoute.slavePort))
        {
            pBusInfo->adspDdrBwAb = pBwVote->bwValue.busBwAbIb.Ab;
            pBusInfo->adspDdrBwIb = pBwVote->bwValue.busBwAbIb.Ib;
        }

        if((AdsppmBusPort_Ext_Ahb_Master == pBwVote->busRoute.masterPort) &&
            (AdsppmBusPort_Ddr_Slave == pBwVote->busRoute.slavePort))
        {
            pBusInfo->extAhbDdrBwAb = pBwVote->bwValue.busBwAbIb.Ab;
            pBusInfo->extAhbDdrBwIb = pBwVote->bwValue.busBwAbIb.Ib;
        }
    }

    // Internal AHB BW
    pBusInfo->intAhbBwAb = gBusMgrCtx.curAHBBwData.Ab;
    pBusInfo->intAhbBwIb = gBusMgrCtx.curAHBBwData.Ib;

    adsppm_unlock(gBusMgrCtx.lock);

    ADSPPM_LOG_PRINTF_8(ADSPPM_LOG_LEVEL_INFO,
        "ADSP-DDR BW: Ab=%llu, Ib=%llu; AHB BW: Ab=%llu, Ib=%llu",
        (uint32) pBusInfo->adspDdrBwAb,
        (uint32) (pBusInfo->adspDdrBwAb >> 32),
        (uint32) pBusInfo->adspDdrBwIb,
        (uint32) (pBusInfo->adspDdrBwIb >> 32),
        (uint32) pBusInfo->intAhbBwAb,
        (uint32) (pBusInfo->intAhbBwAb >> 32),
        (uint32) pBusInfo->intAhbBwIb,
        (uint32) (pBusInfo->intAhbBwIb >> 32));

    return sts;
}

Adsppm_Status BusMgr_GetCompensatedBwInfo(
    coreUtils_Q_Type* pBwRequestQueue,
    AdsppmBusRouteType* pRoute,
    uint64* pTotalRequestedBw)
{
    busMgrCtxType* self = &gBusMgrCtx;
    RMHandleType* pRequest = NULL;
    AdsppmBusBWDataIbAbType aggResult;
    uint32 failCount = 0;
    AsicCompensatedBWFudgeFactor compensatedBWFudgeFactorValues;

    if((pBwRequestQueue == NULL) ||
        (pTotalRequestedBw == NULL) ||
        (pRoute == NULL))
    {
        return Adsppm_Status_BadParm;
    }

    if(self->lock == NULL)
    {
        return Adsppm_Status_NotInitialized;
    }

    memset(&aggResult, 0, sizeof(AdsppmBusBWDataIbAbType));

    ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_BW);

    // Go through each client's bw request; aggregate the BW requests only for
    // compensated request types
    pRequest = (RMHandleType*) coreUtils_Q_Check(pBwRequestQueue);
    while(pRequest != NULL)
    {
        AdsppmBwReqType* pBwReqData = (AdsppmBwReqType*) pRequest->pRequestData;
        if((pRequest->validFlag == RM_Valid) &&
            (pBwReqData != NULL) &&
            (pBwReqData->requestClass == Adsppm_BwReqClass_Compensated))
        {
            uint32 bwIndex;
            // For each sub-request within current client's request
            for(bwIndex = 0; bwIndex < pBwReqData->numOfBw; bwIndex++)
            {
                AdsppmBusBWRequestValueType* pBwReq =
                    &pBwReqData->pBwArray[bwIndex];
                if((pBwReq->busRoute.masterPort == pRoute->masterPort) &&
                    (pBwReq->busRoute.slavePort == pRoute->slavePort))
                {
                    if(ACM_BusBWAggregate(&aggResult, &pBwReq->bwValue) !=
                        Adsppm_Status_Success)
                    {
                        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                            "BW aggregation failed");
                        failCount += 1;
                    }
                }
            }
        }
        pRequest = (RMHandleType*) coreUtils_Q_Next(pBwRequestQueue,
            &pRequest->link);
    }

    if((AdsppmBusPort_Adsp_Master == pRoute->masterPort) &&
        (AdsppmBusPort_Ddr_Slave == pRoute->slavePort) && 
        ((ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Disabled) ||
         (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Disabled)))
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                "BW Aggregation for ADSP-to-DDR, Get fudged BW val");
        ACM_ApplyAdspDdrBwConcurrencyFactor(
                gBusMgrCtx.extPreAggregateData.numCompensatedAdspDdrVotes,
                gBusMgrCtx.extPreAggregateData.maxAdspToDDrBw,
                &aggResult.Ab, &aggResult.Ib,
                &compensatedBWFudgeFactorValues);

        if(compensatedBWFudgeFactorValues.floorFudgeFactor && compensatedBWFudgeFactorValues.ceilingFudgeFactor)
                {
                    BusMgr_GetCompensatedFudgedBwValue (pBwRequestQueue, 
                                                        &aggResult.Ab,
                                                        &aggResult.Ib,
                                                        &compensatedBWFudgeFactorValues);
                }        
    }

    ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_BW);

    // Query result set to Max of Ab, Ib.
    // This result is used as input for the lookup table.
    *pTotalRequestedBw = MAX(aggResult.Ab, aggResult.Ib);

    return (failCount == 0) ? Adsppm_Status_Success : Adsppm_Status_Failed;
}
/**
*@fn - BusMgrApplyCacheSizeExtCompBWVote - Function used to update ext compensated bw vote
*/
static Adsppm_Status BusMgrApplyCacheSizeExtCompBWVote (uint32 cacheSize)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 numUpdatedExtBwVotes=0;
    AdsppmBusBWRequestValueType updateExtBusBwValue[gBusMgrCtx.extRouteNum];
    // Locals to track change in SNOC floor vote
    uint32 snocFloorVoteMhz = 0;
    //Locals to track change in alc values
    uint32 fal_tolerance = 0;
    uint32 idle_time = 0;
    boolean snocFloorChanged = FALSE;
    boolean alcInfoChanged = FALSE;

    memset(updateExtBusBwValue, 0, gBusMgrCtx.extRouteNum*sizeof(AdsppmBusBWRequestValueType));
    adsppm_lock(gBusMgrCtx.lock); 
    
    //Go back to old index
    gBusMgrCtx.preCurrentIndex ^= 1;

    ACM_GetBWScalingFactorFromCacheSize(cacheSize, &gBusMgrCtx.scalingfactor);

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "Scaling Factor is %u",gBusMgrCtx.scalingfactor);
    AdsppmCoreCtxType* pAdsppmCoreCtx = GetAdsppmCoreContext();
    
    busMgrAggregateExternalBW_CallBack(&pAdsppmCoreCtx->rscReqQ[Adsppm_Rsc_Id_BW-1]);
    
    
    gBusMgrCtx.externalBwUpdatePending = FALSE;

    numUpdatedExtBwVotes = busMgr_ProcessExternalBWInfo(&updateExtBusBwValue[0], &snocFloorChanged, &snocFloorVoteMhz);

    if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Alc))
    {
      if(numUpdatedExtBwVotes > 0)
        busMgr_ProcessAlcInfo(&alcInfoChanged, &fal_tolerance, &idle_time);
    }
    // Update uSleep manager
    if(Adsppm_Status_Success == sts)
    {
        sts = busMgrUpdateUSleepMgr();
    }

    //Go back on previously set buffer
    gBusMgrCtx.preCurrentIndex ^= 1;
    adsppm_unlock(gBusMgrCtx.lock);

    //Send Final bw infor to ICB
    if( Adsppm_Status_Success == busMgr_SendExternalBWInfo(numUpdatedExtBwVotes,&updateExtBusBwValue[0], snocFloorChanged,snocFloorVoteMhz ))
    {
       if(AsicFeatureState_Enabled == ACM_GetFeatureStatus(AsicFeatureId_Alc))
       {
         if(alcInfoChanged == TRUE)
           Bus_IssueAlcRequest (fal_tolerance, idle_time);
       }
    }

    return sts;
}
/**
* @fn - bmOnCPManagerUpdate - Check for change in active client classes, and if there is a change, select
* a new cache partitioning configuration based on the new mix of client
* classes
*/
static void bmOnCPManagerUpdate(AdsppmCachePartitionManagerEventId eventId,void * param)
{

    busMgrCtxType *self =
        (busMgrCtxType *) param;
    uint32 cachepartitionsize = 0;

    if(self && eventId == CPM_CACHE_CONFIGURATION_CHANGE)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "BW request due to change in cache size");
        // Get the current partition size
        if (Adsppm_Status_Success == CPM_GetCachePartitionSize(&cachepartitionsize))
            BusMgrApplyCacheSizeExtCompBWVote(cachepartitionsize);
        else
            ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "CPM cache size returned invalid value; skipping bw scaling");
     }
}


/**
* @fn - Adsppm_DCVS_Monitored_Routes - Check for a route given by master/slave
* ports are monitored by DCVS
*/
Adsppm_DCVS_Monitored_Routes  bmSelectRouteOnPort( AdsppmBusPortIdType master,
                                                   AdsppmBusPortIdType slave)
{
    Adsppm_DCVS_Monitored_Routes route = Adsppm_DCVS_Route_Max;

    if( (master == AdsppmBusPort_Q6DSP_Master) &&
         slave  == AdsppmBusPort_Ddr_Slave)
    {
        route = Adsppm_DCVS_Route_DSP_DDR;
    }
    else if(master == AdsppmBusPort_Vapss_Hcp_Master &&
            slave  == AdsppmBusPort_Ddr_Slave)
    {
        route = Adsppm_DCVS_Route_HCP_DDR;
    }
    else if(master == AdsppmBusPort_Vapss_Dma_Master &&
            slave  == AdsppmBusPort_Ddr_Slave)
    {
        route = Adsppm_DCVS_Route_DMA_DDR;
    }

    return route;
}
