#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "pwrmgr.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"
#include "qurt_hvx.h"
#include "usleepmgr.h"



static void PWR_USleepNode_RegisterCB(void *context, unsigned int event_type, void *data, unsigned int data_size);


extern powerMgrCtxType gPowerMgrCtx;


void pwrMgr_targetInit()
{

    /* Register a callback for dspPm uSleep node
     * create a client for PwrMgr to vote on dspPm uSleep node.
	 */
     npa_resource_available_cb(
							QDSPPM_USLEEP_NODE_NAME,
							PWR_USleepNode_RegisterCB,
							NULL);
							
	 return;
}

/**
 * PWR_USleepNode_RegisterCB
 * 
 * @brief Registers dspPm uSleep node
 * 
 * @param see npa_callback defintion for function parameters
 */
static void PWR_USleepNode_RegisterCB(
			void         *context,
            unsigned int  eventType,
            void         *data,
            unsigned int  dataSize)
{
    gPowerMgrCtx.uSleepHandle = npa_create_sync_client(
											QDSPPM_USLEEP_NODE_NAME, 
                                            "qdsppm_lpass_core",
											NPA_CLIENT_REQUIRED);
    return;
}



Adsppm_Status pwrMgr_checkHvxLockStatus(Adsppm_Status* hvxLockCheckStatus,
												  RMHandleType* pRMHandle)
{
		

	return Adsppm_Status_Success;


}
												  
Adsppm_Status pwrMgr_preRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;

	uint32 Freq;
	
	if((pwrDomain == AsicPowerDomain_LpassCore) && (ACM_IsPwrDomainValid(AsicPowerDomain_LpassCore)))
	{
		Hal_ClkRgmEnableDisableType lpass_core_state = gPowerMgrCtx.pwrDomainState[pwrDomain][gPowerMgrCtx.activeStateIndex];
		
		if((Hal_ClkRgmDisable == lpass_core_state) && ACM_IsClkIdValid(AdsppmClk_Ahb_Root))
		{
			result = ClkRgm_GetClockFrequency(AdsppmClk_Ahb_Root,&Freq);
				
			if(Adsppm_Status_Success == result)
			{
				gPowerMgrCtx.prevAhbFreq = Freq;
			
				ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
                                   "LPASS Core Disable : Setting the Freq of Ahb_Root from %u to XO",
                                    Freq);

				result = ClkRgm_SetClock(AdsppmClk_Ahb_Root, XO_FREQ);
			}
			
		}
		
	}

	return result;
}


Adsppm_Status pwrMgr_postRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;
	uint32 Freq;
	
	if((pwrDomain == AsicPowerDomain_LpassCore) && (ACM_IsPwrDomainValid(AsicPowerDomain_LpassCore)))
	{
		Hal_ClkRgmEnableDisableType lpass_core_state = gPowerMgrCtx.pwrDomainState[pwrDomain][gPowerMgrCtx.activeStateIndex];
		
		if((Hal_ClkRgmEnable == lpass_core_state) && ACM_IsClkIdValid(AdsppmClk_Ahb_Root))
		{
		  Freq = gPowerMgrCtx.prevAhbFreq;
		 
		  ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
                                    "LPASS Core Enable : Restoring the Freq of Ahb_Root from XO to %u",
                                    Freq);
		 
		  result = ClkRgm_SetClock(AdsppmClk_Ahb_Root, Freq);
		 
		}

		if(gPowerMgrCtx.uSleepHandle)
		{			
			boolean uSleep_vote;

			/* Allow usleep only when LPASS Core is in disabled state */
			uSleep_vote = lpass_core_state ? QDSPPM_USLEEP_DISALLOW : QDSPPM_USLEEP_ALLOW;

			ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_INFO_EXT,
				"NPA request to %s usleep", (uSleep_vote ? "Allow" : "DisAllow"));

			npa_issue_required_request(gPowerMgrCtx.uSleepHandle, uSleep_vote);
		}
	}
	
	return result;

}

