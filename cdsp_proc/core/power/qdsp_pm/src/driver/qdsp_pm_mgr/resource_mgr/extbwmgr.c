/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: extbwmgr.c
@brief: Implementation of ADSPPM External Bandwidth Manager.

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/extbwmgr.c#1 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "dcvsmgr.h"
#include "hal_busdrv.h"
#include "sysmonmgr.h"
#include "busmgr.h"


// All state info for the External BW Manager
typedef struct
{
    DALSYSSyncHandle lock;
    boolean dcvsEnabledDisabledFlag;

    AdsppmBusBWDataIbAbType dcvsVote[MAX_ADSPPM_DCVS_ROUTES];
    AdsppmBusBWDataIbAbType clientsFloorVote[MAX_ADSPPM_DCVS_ROUTES];

    AdsppmBusBWDataIbAbType clientsFinalVote[MAX_ADSPPM_DCVS_ROUTES];
    AdsppmBusBWDataIbAbType finalVoteToNpa[MAX_ADSPPM_DCVS_ROUTES];
} AdsppmExtBwManagerType;

static AdsppmExtBwManagerType gAdsppmExtBwManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions
///////////////////////////////////////////////////////////////////////////////

static AdsppmBusPortIdType extBwMgr_SetMasterPort( AdsppmDcvsManagerEventId eventId );

// Calculate new vote to NPA
// Assume lock is obtained
static void extBwMgr_CalculateNewNpaVote(
    AdsppmExtBwManagerType* self,
    Adsppm_DCVS_Monitored_Routes route,
    AdsppmBusBWDataIbAbType* pNewClientsFinalVote,
    AdsppmBusBWDataIbAbType* pNewClientsFloorVote,
    AdsppmBusBWDataIbAbType* pNewFinalVoteToNpa)
{

    AdsppmBusBWDataIbAbType previousClientsFinalVote = self->clientsFinalVote[route];
    boolean changeInClientFinalVote =
        ((pNewClientsFinalVote->Ib != previousClientsFinalVote.Ib) ||
            (pNewClientsFinalVote->Ab != previousClientsFinalVote.Ab));

    if(self->dcvsVote[route].Ib || self->dcvsVote[route].Ab)
    {
        pNewFinalVoteToNpa->Ib = changeInClientFinalVote?
            MAX(pNewClientsFinalVote->Ib, self->dcvsVote[route].Ib):
            MAX(pNewClientsFloorVote->Ib, self->finalVoteToNpa[route].Ib);
        pNewFinalVoteToNpa->Ab = changeInClientFinalVote?
            MAX(pNewClientsFinalVote->Ab, self->dcvsVote[route].Ab):
            MAX(pNewClientsFloorVote->Ab, self->finalVoteToNpa[route].Ab);
    }

}

// Print current context values
// Assume lock is obtained
static void extBwMgr_PrintContext(
    AdsppmExtBwManagerType* self,
    Adsppm_DCVS_Monitored_Routes route)
{

    if(self->dcvsEnabledDisabledFlag)
    {

        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO, "DCVS Enabled: Route = %u", route);
        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "   Current DCVS vote:          ib=%llu, ab=%llu",
            (uint32) self->dcvsVote[route].Ib,
            (uint32) (self->dcvsVote[route].Ib >> 32),
            (uint32) self->dcvsVote[route].Ab,
            (uint32) (self->dcvsVote[route].Ab >> 32));
        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Floor vote: ib=%llu, ab=%llu",
            (uint32) self->clientsFloorVote[route].Ib,
            (uint32) (self->clientsFloorVote[route].Ib >> 32),
            (uint32) self->clientsFloorVote[route].Ab,
            (uint32) (self->clientsFloorVote[route].Ab >> 32));
        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Final vote: ib=%llu, ab=%llu",
            (uint32) self->clientsFinalVote[route].Ib,
            (uint32) (self->clientsFinalVote[route].Ib >> 32),
            (uint32) self->clientsFinalVote[route].Ab,
            (uint32) (self->clientsFinalVote[route].Ab >> 32));

    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "DCVS Disabled");
    }

}

// Check changes in final vote to NPA
// Assume lock is obtained
static boolean extBwMgr_checkChangeInFinalVoteToNpa(
    AdsppmExtBwManagerType* self,
    Adsppm_DCVS_Monitored_Routes route,
    AdsppmBusBWDataIbAbType* pNewFinalVoteToNpa)
{

    boolean result = FALSE;

    if((pNewFinalVoteToNpa->Ib != self->finalVoteToNpa[route].Ib) ||
        (pNewFinalVoteToNpa->Ab != self->finalVoteToNpa[route].Ab) ||
        (pNewFinalVoteToNpa->latencyNs != self->finalVoteToNpa[route].latencyNs))
    {
        self->finalVoteToNpa[route] = *pNewFinalVoteToNpa;
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "Send new DDR BW on route %u to HAL", route);

        extBwMgr_PrintContext(self, route);

        result = TRUE;
    }
    else
    {
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "NO updates on DDR BW route %u", route);

        result = FALSE;
    }

    ADSPPM_LOG_PRINTF_5(ADSPPM_LOG_LEVEL_INFO,
        "   Current vote to NPA: ib=%llu, ab=%llu, latency=%u",
        (uint32) self->finalVoteToNpa[route].Ib,
        (uint32) (self->finalVoteToNpa[route].Ib >> 32),
        (uint32) self->finalVoteToNpa[route].Ab,
        (uint32) (self->finalVoteToNpa[route].Ab >> 32),
        self->finalVoteToNpa[route].latencyNs);

    return result;

}

// Check for changes in ADSP to DDR BW vote
// if there is a change, update local variable
static void extBwMgr_onAdspToDdrBwVoteUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmExtBwManagerType* self = (AdsppmExtBwManagerType*) param;
    AdsppmBusBWRequestValueType newFinalVoteToNpa;
    boolean changeInFinalVoteToNpa = FALSE;
    AdsppmBusBWDataIbAbType dcvsVote;

    if(eventId != DCVSMGR_ADSP_TO_DDR_BW_VOTE_CHANGE_EVENT)
        return;

    DcvsMgr_GetDcvsAdsp2DdrBwVote(&dcvsVote);

    adsppm_lock(self->lock);

    newFinalVoteToNpa.busRoute.masterPort = extBwMgr_SetMasterPort(eventId);
    newFinalVoteToNpa.busRoute.slavePort = AdsppmBusPort_Ddr_Slave;
    newFinalVoteToNpa.bwOperation = AdsppmBusBWOperation_BW;
    newFinalVoteToNpa.bwValue.busBwAbIb = self->clientsFinalVote[Adsppm_DCVS_Route_DSP_DDR];

    if(self->dcvsEnabledDisabledFlag)
    {
        // update self->dcvsVote
        self->dcvsVote[Adsppm_DCVS_Route_DSP_DDR] = dcvsVote;

        if(self->dcvsVote[Adsppm_DCVS_Route_DSP_DDR].Ib ||
           self->dcvsVote[Adsppm_DCVS_Route_DSP_DDR].Ab)
        {
            newFinalVoteToNpa.bwValue.busBwAbIb.Ib = MAX(
                self->clientsFloorVote[Adsppm_DCVS_Route_DSP_DDR].Ib,
                self->dcvsVote[Adsppm_DCVS_Route_DSP_DDR].Ib);

            newFinalVoteToNpa.bwValue.busBwAbIb.Ab = MAX(
                self->clientsFloorVote[Adsppm_DCVS_Route_DSP_DDR].Ab,
                self->dcvsVote[Adsppm_DCVS_Route_DSP_DDR].Ab);
        }
    }

    changeInFinalVoteToNpa = extBwMgr_checkChangeInFinalVoteToNpa(
        self, Adsppm_DCVS_Route_DSP_DDR, &newFinalVoteToNpa.bwValue.busBwAbIb);

    adsppm_unlock(self->lock);

    // Send final ADSP to DDR BW vote to HAL
    if(changeInFinalVoteToNpa)
        Bus_IssueBWRequest(1, &newFinalVoteToNpa);

}



// Check for changes in CORE (Compute:VAPSS) to DDR BW vote
// if there is a change, update local variable
static void extBwMgr_onCoreToDdrBwVoteUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmExtBwManagerType* self = (AdsppmExtBwManagerType*) param;
    AdsppmBusBWRequestValueType newFinalVoteToNpa;
    boolean changeInFinalVoteToNpa = FALSE;
    AdsppmBusBWDataIbAbType dcvsVote;
    Adsppm_DCVS_Monitored_Routes route;

    if(eventId != DCVSMGR_HCP_TO_DDR_BW_VOTE_CHANGE_EVENT &&
       eventId != DCVSMGR_DMA_TO_DDR_BW_VOTE_CHANGE_EVENT)
        return;

    DcvsMgr_GetDcvsCore2DdrBwVote(&dcvsVote, eventId);

    adsppm_lock(self->lock);

    newFinalVoteToNpa.busRoute.masterPort = extBwMgr_SetMasterPort(eventId);;
    newFinalVoteToNpa.busRoute.slavePort = AdsppmBusPort_Ddr_Slave;
    newFinalVoteToNpa.bwOperation = AdsppmBusBWOperation_BW;
    route = bmSelectRouteOnPort(newFinalVoteToNpa.busRoute.masterPort,
                                newFinalVoteToNpa.busRoute.slavePort);
    ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO, "Event %u, sending DCVS vote Route %u [M=%u, S=%u]",
                        eventId, route,
                        newFinalVoteToNpa.busRoute.masterPort,
                        newFinalVoteToNpa.busRoute.slavePort);
    if(route == Adsppm_DCVS_Route_Max)
    {
        return;
    }

    newFinalVoteToNpa.bwValue.busBwAbIb = self->clientsFinalVote[route];

    if(self->dcvsEnabledDisabledFlag)
    {
        //Adsppm_DCVS_Monitored_Routes route;

        //route = bmSelectRouteOnPort(newFinalVoteToNpa.busRoute.masterPort,
        //                            newFinalVoteToNpa.busRoute.slavePort);

        // update local variable self->dcvsVote
        self->dcvsVote[route] = dcvsVote;
        if(self->dcvsVote[route].Ib || self->dcvsVote[route].Ab)
        {
            newFinalVoteToNpa.bwValue.busBwAbIb.Ib = MAX(
                self->clientsFloorVote[route].Ib, self->dcvsVote[route].Ib);
            newFinalVoteToNpa.bwValue.busBwAbIb.Ab = MAX(
                self->clientsFloorVote[route].Ab, self->dcvsVote[route].Ab);
        }
    }

    changeInFinalVoteToNpa = extBwMgr_checkChangeInFinalVoteToNpa(
        self, route, &newFinalVoteToNpa.bwValue.busBwAbIb);

    adsppm_unlock(self->lock);

    // Send final ADSP to DDR BW vote to HAL
    if(changeInFinalVoteToNpa)
        Bus_IssueBWRequest(1, &newFinalVoteToNpa);

}


// Check for changes in DCVS' enabled/disabled flag
// if there is a change, update local variable
static void extBwMgr_onDcvsEnabledDisabledUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmExtBwManagerType* self = (AdsppmExtBwManagerType*) param;
    boolean dcvsEnabledDisabledFlag = DcvsMgr_GetDcvsEnabledDisabledFlag();

    adsppm_lock(self->lock);

    // update self->dcvsEnabledDisabledFlag
    self->dcvsEnabledDisabledFlag = dcvsEnabledDisabledFlag;

    if(!dcvsEnabledDisabledFlag)
    {
        int i;
        for(i = 0; i < MAX_ADSPPM_DCVS_ROUTES; i++)
        {
            self->dcvsVote[i].Ib = 0;
            self->dcvsVote[i].Ab = 0;
        }
    }

    adsppm_unlock(self->lock);

}

static AdsppmBusPortIdType extBwMgr_SetMasterPort( AdsppmDcvsManagerEventId eventId )
{
    AdsppmBusPortIdType   mPort = AdsppmBusPort_None;

    if(ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
    {
        switch(eventId)
        {
            case DCVSMGR_HCP_TO_DDR_BW_VOTE_CHANGE_EVENT:
                mPort = AdsppmBusPort_Vapss_Hcp_Master;
                break;

            case DCVSMGR_DMA_TO_DDR_BW_VOTE_CHANGE_EVENT:
                mPort = AdsppmBusPort_Vapss_Dma_Master;
                break;

            case DCVSMGR_ADSP_TO_DDR_BW_VOTE_CHANGE_EVENT:
            default:
               mPort = AdsppmBusPort_Q6DSP_Master;
               break;
        }
    }
    else
    {
         mPort = AdsppmBusPort_Q6DSP_Master;
    }
   return mPort;
}


///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status ExtBwMgr_Init(void)
{

    AdsppmExtBwManagerType* self = &gAdsppmExtBwManager;

    memset(self, 0, sizeof(AdsppmExtBwManagerType));

    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock,
        NULL))
    {
        return Adsppm_Status_Failed;
    }

    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
        AsicFeatureState_Enabled)
    {
        if(DcvsMgr_RegisterEvent(
                DCVSMGR_DCVS_ENABLED_DISABLED_CHANGE_EVENT,
                extBwMgr_onDcvsEnabledDisabledUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

        if(DcvsMgr_RegisterEvent(
                DCVSMGR_ADSP_TO_DDR_BW_VOTE_CHANGE_EVENT,
                extBwMgr_onAdspToDdrBwVoteUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

        if(DcvsMgr_RegisterEvent(
                DCVSMGR_HCP_TO_DDR_BW_VOTE_CHANGE_EVENT,
                extBwMgr_onCoreToDdrBwVoteUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

        if(DcvsMgr_RegisterEvent(
                DCVSMGR_DMA_TO_DDR_BW_VOTE_CHANGE_EVENT,
                extBwMgr_onCoreToDdrBwVoteUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

    }

    return Adsppm_Status_Success;

}

Adsppm_Status ExtBwMgr_IssueExtBwRequest(
    uint32 numUpdatedExtBwVotes,
    AdsppmBusBWRequestValueType* pUpdatedExtBusBwValues)
{

    AdsppmExtBwManagerType* self = &gAdsppmExtBwManager;
    Adsppm_Status sts = Adsppm_Status_Success;
    AdsppmBusBWDataIbAbType* pNewClientsFloorVote;



    // Find new clients final vote
    int i = 0;
    for(i = 0; i < numUpdatedExtBwVotes; i++)
    {
        Adsppm_DCVS_Monitored_Routes route;

        route = bmSelectRouteOnPort(pUpdatedExtBusBwValues[i].busRoute.masterPort,
                                    pUpdatedExtBusBwValues[i].busRoute.slavePort);

        pNewClientsFloorVote = &(self->clientsFloorVote[route]);
        ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "DCVS, clients floor value: ib=%llu, ab=%llu",
            (uint32) pNewClientsFloorVote->Ib,
            (uint32) (pNewClientsFloorVote->Ib >> 32),
            (uint32) pNewClientsFloorVote->Ab,
            (uint32) (pNewClientsFloorVote->Ab >> 32));

        if( route != Adsppm_DCVS_Route_Max)
        {
            boolean changeInFinalVoteToNpa = FALSE;
            boolean sendClientVoteToDcvs = FALSE;
            AdsppmBusBWDataIbAbType newClientsFinalVote;
            AdsppmBusBWRequestValueType newFinalVoteToNpa;

            newClientsFinalVote.Ib = 0;
            newClientsFinalVote.Ab = 0;
            newClientsFinalVote.latencyNs = 0;

            newClientsFinalVote = pUpdatedExtBusBwValues[i].bwValue.busBwAbIb;

            newFinalVoteToNpa.busRoute.masterPort = pUpdatedExtBusBwValues[i].busRoute.masterPort;
            newFinalVoteToNpa.busRoute.slavePort = pUpdatedExtBusBwValues[i].busRoute.slavePort;
            newFinalVoteToNpa.bwOperation = AdsppmBusBWOperation_BW;
            newFinalVoteToNpa.bwValue.busBwAbIb = newClientsFinalVote;

            adsppm_lock(self->lock);

            /* In RPMh Architecture, NPA node query is not supported, hence SysMon required
            ** DSPPM to notify its static vote regardless of whether client has enabled DCVS or not
            */
            if((self->dcvsEnabledDisabledFlag ||
                ACM_GetFeatureStatus(AsicFeatureId_RPMh) == AsicFeatureState_Enabled ))
            {

                /* However, we still only consider to update static vote IF there's a change
                   If no change, we don't need to update. */
                sendClientVoteToDcvs =
                    ((newClientsFinalVote.Ib != self->clientsFinalVote[route].Ib) ||
                    (newClientsFinalVote.Ab != self->clientsFinalVote[route].Ab));
			}
			
			if(self->dcvsEnabledDisabledFlag &&
               (newClientsFinalVote.Ib || newClientsFinalVote.Ab))
            {
                extBwMgr_CalculateNewNpaVote(self, route, &newClientsFinalVote,
                pNewClientsFloorVote, &newFinalVoteToNpa.bwValue.busBwAbIb);
            }

           // Store new clients final and floor vote
            self->clientsFinalVote[route] = newClientsFinalVote;


            changeInFinalVoteToNpa = extBwMgr_checkChangeInFinalVoteToNpa(
                self, route, &newFinalVoteToNpa.bwValue.busBwAbIb);

            adsppm_unlock(self->lock);

            //Part #1: Clear the DCVS Monitored Route as we're sending it here, not later in part.2
            pUpdatedExtBusBwValues[i].busRoute.masterPort = AdsppmBusPort_None;
            pUpdatedExtBusBwValues[i].busRoute.slavePort = AdsppmBusPort_None;

            // Send DCVS-monitored DDR BW to HAL
            if((sts == Adsppm_Status_Success) && changeInFinalVoteToNpa)
                sts = Bus_IssueBWRequest(1, &newFinalVoteToNpa);

            // Update SysMon DCVS with new clients final vote
            if((sts == Adsppm_Status_Success) && sendClientVoteToDcvs)
               sysMonMgr_SendNewBwClientsFinalVoteToDcvs(&newClientsFinalVote, route);
       }
    }

    //Part #2: Send non DCVS-monitored to DDR external BW votes to HAL
    if((sts == Adsppm_Status_Success) && numUpdatedExtBwVotes)
        sts = Bus_IssueBWRequest(numUpdatedExtBwVotes, pUpdatedExtBusBwValues);

    return sts;

}

Adsppm_Status ExtBwMgr_GetInfo(AdsppmInfoDcvsAdspDdrBwType* pDcvsAdspDdrBwInfo,
                               Adsppm_DCVS_Monitored_Routes route )
{

    AdsppmExtBwManagerType* self = &gAdsppmExtBwManager;

    if(pDcvsAdspDdrBwInfo == NULL)
        return Adsppm_Status_NullPointer;

    adsppm_lock(self->lock);
    pDcvsAdspDdrBwInfo->dcvsVote            = self->dcvsVote[route];
    pDcvsAdspDdrBwInfo->clientsFloorVote    = self->clientsFloorVote[route];
    pDcvsAdspDdrBwInfo->clientsFinalVote = self->clientsFinalVote[route];
    pDcvsAdspDdrBwInfo->finalVoteToNpa   = self->finalVoteToNpa[route];
    adsppm_unlock(self->lock);

    return Adsppm_Status_Success;

}
void ExtBwMgr_UpdateAdsptoDDrClientFloorVotes( AdsppmBusBWDataIbAbType* pNewClientsFloorVote,
                                                                Adsppm_DCVS_Monitored_Routes Route)
{

  AdsppmExtBwManagerType* self = &gAdsppmExtBwManager;

  if(NULL != pNewClientsFloorVote)
  {
    if(NULL != self)
    {
      adsppm_lock(self->lock);

      //Update Clients floor vote
             ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "Current Clients floor Votes : Ib=%llu, Ab=%llu",
            (uint32) self->clientsFloorVote[Route].Ib,
            (uint32) (self->clientsFloorVote[Route].Ib >> 32),
            (uint32) self->clientsFloorVote[Route].Ab,
            (uint32) (self->clientsFloorVote[Route].Ab >> 32));

      self->clientsFloorVote[Route] = *pNewClientsFloorVote;

            ADSPPM_LOG_PRINTF_4(ADSPPM_LOG_LEVEL_INFO,
            "New Clients floor Votes : Ib=%llu, Ab=%llu",
            (uint32) self->clientsFloorVote[Route].Ib,
            (uint32) (self->clientsFloorVote[Route].Ib >> 32),
            (uint32) self->clientsFloorVote[Route].Ab,
            (uint32) (self->clientsFloorVote[Route].Ab >> 32));
      adsppm_unlock(self->lock);
    }
  }
}

