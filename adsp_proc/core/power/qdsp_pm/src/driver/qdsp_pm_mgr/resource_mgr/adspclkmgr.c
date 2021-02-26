/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adspclkmgr.c
@brief: Implementation of ADSPPM ADSP Clock Manager.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/adspclkmgr.c#5 $
*/

#include "adsppm.h"
#include "adsppm_utils.h"
#include "adspclkmgr.h"
#include "dcvsmgr.h"
#include "hal_clkrgm.h"
#include "observer.h"
#include "sysmonmgr.h"
#include "usleepmgr.h"

// All state info for the ADSP Clock Manager
typedef struct
{
    DALSYSSyncHandle lock;
    boolean dcvsEnabledDisabledFlag;
    uint32 dcvsVote;
	uint32 clientLimitMaxVote;
    uint32 clientsFloorVote;
    uint32 clientsFinalVote;
    uint32 finalVoteToNpa;
    AdsppmObserverStateType observerState;
    npa_client_handle uSleepHandle; //to vote on dsp pm usleep node
} AdsppmAdspClkManagerType;

static AdsppmAdspClkManagerType gAdsppmAdspClkManager;

#define QDSPPM_ISLAND_Q6_MAX_CLK 600000000

///////////////////////////////////////////////////////////////////////////////
// Static functions
///////////////////////////////////////////////////////////////////////////////

// Set the ADSP core clock and notify any observers of the event
static Adsppm_Status adspClkMgr_SetAdspClock(AdsppmAdspClkManagerType* self,
    uint32 freqHz)
{
    Adsppm_Status status;
    // Send final ADSP clock vote to HAL
    status = ClkRgm_SetClock(AdsppmClk_Adsp_Core, freqHz);
    if(status == Adsppm_Status_Success)
     {
        status = observer_notify(&self->observerState, ADSPCLKMGR_CLOCK_CHANGE_EVENT);

		if (gAdsppmAdspClkManager.uSleepHandle)
		{
			boolean uSleep_vote;
			
			adsppm_lock(self->lock);
            freqHz = self->finalVoteToNpa;
            
    		/* Q6 clock should be less than 250Mhz for entry into Island mode */
	    	uSleep_vote = freqHz > QDSPPM_ISLAND_Q6_MAX_CLK ? QDSPPM_USLEEP_DISALLOW : QDSPPM_USLEEP_ALLOW;
            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO_EXT,
                                    "NPA request to %s usleep for %d Hz", (uSleep_vote ? "Allow" : "DisAllow"), freqHz);
            npa_issue_required_request(gAdsppmAdspClkManager.uSleepHandle, uSleep_vote);
            
            adsppm_unlock(self->lock);
		}
     }
    return status;
}

// Set the ADSP Max core clock 
static Adsppm_Status adspClkMgr_SetAdspMaxClock(AdsppmAdspClkManagerType* self, 
	uint32 freqHz)
{
    Adsppm_Status status;
    // Send final ADSP Max clock vote to HAL
    status = ClkRgm_SetMaxClock(AdsppmClk_Adsp_Core, freqHz);
    
    return status;
}

// Calculate new vote to NPA
// Assume lock is obtained
static void adspClkMgr_CalculateNewNpaVote(
    AdsppmAdspClkManagerType* self,
    uint32 newClientsFinalVote,
    uint32 newClientsFloorVote,
    uint32* pNewFinalVoteToNpa)
{

    uint32 previousClientsFinalVote = self->clientsFinalVote;
    boolean changeInClientFinalVote =
        (newClientsFinalVote != previousClientsFinalVote);

    if(self->dcvsVote)
    {
        *pNewFinalVoteToNpa = changeInClientFinalVote?
            MAX(newClientsFinalVote, self->dcvsVote):
            MAX(newClientsFloorVote, self->finalVoteToNpa);
    }

}

// Print current context values
// Assume lock is obtained
static void adspClkMgr_PrintContext(
    AdsppmAdspClkManagerType* self)
{

    if(self->dcvsEnabledDisabledFlag)
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "DCVS Enabled:");
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current DCVS vote:          clk=%u",
            self->dcvsVote);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Floor vote: clk=%u",
            self->clientsFloorVote);
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
            "   Current Clients Final vote: clk=%u",
            self->clientsFinalVote);
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO, "DCVS Disabled");
    }

}

// Check changes in final vote to NPA
// Assume lock is obtained
static boolean adspClkMgr_checkChangeInFinalVoteToNpa(
    AdsppmAdspClkManagerType* self,
    uint32 newFinalVoteToNpa)
{

    boolean result = FALSE;

    if(newFinalVoteToNpa != self->finalVoteToNpa)
    {
        self->finalVoteToNpa = newFinalVoteToNpa;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
            "Send new ADSP Clock to HAL");

        adspClkMgr_PrintContext(self);

        result = TRUE;
    }
    else
    {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
            "NO updates on ADSP Clock");

        result = FALSE;
    }

    ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO,
        "   Current vote to NPA: clk=%u",
        self->finalVoteToNpa);

    return result;

}

// Check for changes in ADSP clock vote
// if there is a change, update local variable
static void adspClkMgr_onAdspClockVoteUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmAdspClkManagerType* self = (AdsppmAdspClkManagerType*) param;
    uint32 newFinalVoteToNpa;
    boolean changeInFinalVoteToNpa = FALSE;
    uint32 dcvsVote = 0;

    if(eventId != DCVSMGR_ADSP_CLOCK_VOTE_CHANGE_EVENT)
        return;

    DcvsMgr_GetDcvsAdspClockVote(&dcvsVote);

    adsppm_lock(self->lock);

    newFinalVoteToNpa = self->clientsFinalVote;

    if(self->dcvsEnabledDisabledFlag)
    {
        // update self->dcvsVote
        self->dcvsVote = dcvsVote;

        if(self->dcvsVote)
        {
            newFinalVoteToNpa = MAX(self->clientsFloorVote, self->dcvsVote);
        }
    }

    changeInFinalVoteToNpa = adspClkMgr_checkChangeInFinalVoteToNpa(
        self, newFinalVoteToNpa);

    adsppm_unlock(self->lock);

    // Set final ADSP clock vote
    if(changeInFinalVoteToNpa)
        adspClkMgr_SetAdspClock(self, newFinalVoteToNpa);

}

// Check for changes in ADSP Max clock vote
// if there is a change, update local variable
void adspClkMgr_AdspMaxClockVoteUpdate(
    uint32 clientLimitMaxVote)
{
	AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
	boolean changeInMaxVoteToNpa = FALSE;

    adsppm_lock(self->lock);
	
	if(self->clientLimitMaxVote != clientLimitMaxVote)
	{
		// update self->clientLimitMaxVote
		self->clientLimitMaxVote = clientLimitMaxVote;
		changeInMaxVoteToNpa = TRUE;
	}
    adsppm_unlock(self->lock);

    // Set final ADSP clock vote
    if(changeInMaxVoteToNpa)
        adspClkMgr_SetAdspMaxClock(self, clientLimitMaxVote);

}

// Check for changes in DCVS' enabled/disabled flag
// if there is a change, update local variable
static void adspClkMgr_onDcvsEnabledDisabledUpdate(
    AdsppmDcvsManagerEventId eventId,
    void* param)
{

    AdsppmAdspClkManagerType* self = (AdsppmAdspClkManagerType*) param;
    boolean dcvsEnabledDisabledFlag = DcvsMgr_GetDcvsEnabledDisabledFlag();

    adsppm_lock(self->lock);

    // update self->dcvsEnabledDisabledFlag
    self->dcvsEnabledDisabledFlag = dcvsEnabledDisabledFlag;

    if(!dcvsEnabledDisabledFlag)
    {
        self->dcvsVote = 0;
    }

    adsppm_unlock(self->lock);

}


/**
 * AdspClkMgr_USleepNode_RegisterCB
 * 
 * @brief Registers qdspPm uSleep node
 * 
 * @param see npa_callback defintion for function parameters
 */
static void AdspClkMgr_USleepNode_RegisterCB(
				void         *context,
                unsigned int  eventType,
                void         *data,
                unsigned int  dataSize)
{
    gAdsppmAdspClkManager.uSleepHandle = npa_create_sync_client(
										QDSPPM_USLEEP_NODE_NAME, 
                                        "qdsppm_q6_clkmgr",
										NPA_CLIENT_REQUIRED);
    return;
}
///////////////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status AdspClkMgr_Init(void)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;

    memset(self, 0, sizeof(AdsppmAdspClkManagerType));

    if(DAL_SUCCESS != DALSYS_SyncCreate(
        DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock,
        NULL))
    {
        return Adsppm_Status_Failed;
    }

    if(observer_init(&self->observerState) != Adsppm_Status_Success)
        return Adsppm_Status_Failed;

    if(ACM_GetFeatureStatus(AsicFeatureId_DcvsControl) ==
        AsicFeatureState_Enabled)
    {
        if(DcvsMgr_RegisterEvent(
                DCVSMGR_DCVS_ENABLED_DISABLED_CHANGE_EVENT,
                adspClkMgr_onDcvsEnabledDisabledUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }

        if(DcvsMgr_RegisterEvent(
                DCVSMGR_ADSP_CLOCK_VOTE_CHANGE_EVENT,
                adspClkMgr_onAdspClockVoteUpdate,
                (void*) self) !=
            Adsppm_Status_Success)
        {
            return Adsppm_Status_Failed;
        }
    }

    /* Register a callback for qdspPm uSleep node
     * create a client for adsp clk to vote on qdspPm uSleep node.
	 */
    npa_resource_available_cb(QDSPPM_USLEEP_NODE_NAME,
                              AdspClkMgr_USleepNode_RegisterCB, NULL);

    return Adsppm_Status_Success;

}

Adsppm_Status AdspClkMgr_IssueAdspClkRequest(
    uint32 newClientsFinalVote,
    uint32 newClientsFloorVote)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    Adsppm_Status sts = Adsppm_Status_Success;
    uint32 newFinalVoteToNpa = newClientsFinalVote;
    boolean changeInFinalVoteToNpa = FALSE;
    boolean sendClientVoteToDcvs = FALSE;

     ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
        "DCVS, clients floor value: clk=%u, clients final value=%u", newClientsFloorVote, newClientsFinalVote);


    adsppm_lock(self->lock);

    /* In RPMh Architecture, NPA node query is not supported, hence SysMon require
    ** DSPPM to notify its static vote regardless of whether client has enabled DCVS or not
    */
    if(self->dcvsEnabledDisabledFlag ||
       ACM_GetFeatureStatus(AsicFeatureId_RPMh) == AsicFeatureState_Enabled)
    {
        /* Only send if there's a CHANGE in previous client vote */
        sendClientVoteToDcvs = (newClientsFinalVote != self->clientsFinalVote);

        adspClkMgr_CalculateNewNpaVote(self, newClientsFinalVote,
            newClientsFloorVote, &newFinalVoteToNpa);
    }

    // Store new clients final and floor vote
    self->clientsFinalVote = newClientsFinalVote;
    self->clientsFloorVote = newClientsFloorVote;

    changeInFinalVoteToNpa = adspClkMgr_checkChangeInFinalVoteToNpa(
        self, newFinalVoteToNpa);

    adsppm_unlock(self->lock);

    // Set final ADSP clock vote
    if((sts == Adsppm_Status_Success) && changeInFinalVoteToNpa)
        sts = adspClkMgr_SetAdspClock(self, newFinalVoteToNpa);

    // Update SysMon DCVS with new clients final vote
    if((sts == Adsppm_Status_Success) && sendClientVoteToDcvs)
        sysMonMgr_SendNewQ6ClientsFinalVoteToDcvs(newClientsFinalVote);

    return sts;

}

Adsppm_Status AdspClkMgr_GetAdspClockFreqVote(uint32* adspClockFreqVote)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    Adsppm_Status status = Adsppm_Status_Failed;
    if(adspClockFreqVote != NULL)
    {
        adsppm_lock(self->lock);
        *adspClockFreqVote = self->finalVoteToNpa;
        adsppm_unlock(self->lock);
        status = Adsppm_Status_Success;
    }
    return status;
}

Adsppm_Status AdspClkMgr_RegisterEvent(AdsppmAdspClkManagerEventId event,
    AdsppmAdspClkEventHandler handler, void* param)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    // Assuming AdsppmAdspClkEventHandler can be cast to AdsppmEventHandler
    return observer_add(&self->observerState, event,
        (AdsppmEventHandler) handler, param);

}

Adsppm_Status AdspClkMgr_UnregisterEvent(AdsppmAdspClkManagerEventId event,
    AdsppmAdspClkEventHandler handler)
{
    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;
    // Assuming AdsppmAdspClkEventHandler can be cast to AdsppmEventHandler
    return observer_remove(&self->observerState, event,
        (AdsppmEventHandler) handler);

}

Adsppm_Status AdspClkMgr_GetInfo(
    AdsppmInfoDcvsAdspClockType* pDcvsAdspClockInfo)
{

    AdsppmAdspClkManagerType* self = &gAdsppmAdspClkManager;

    if(pDcvsAdspClockInfo == NULL)
        return Adsppm_Status_NullPointer;

    adsppm_lock(self->lock);
    pDcvsAdspClockInfo->dcvsVote         = self->dcvsVote;
    pDcvsAdspClockInfo->clientsFloorVote = self->clientsFloorVote;
    pDcvsAdspClockInfo->clientsFinalVote = self->clientsFinalVote;
    pDcvsAdspClockInfo->finalVoteToNpa   = self->finalVoteToNpa;
    adsppm_unlock(self->lock);

    return Adsppm_Status_Success;

}

