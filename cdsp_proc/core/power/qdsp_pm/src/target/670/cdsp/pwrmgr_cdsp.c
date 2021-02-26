#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "pwrmgr.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"
#include "qurt_hvx.h"

extern powerMgrCtxType gPowerMgrCtx;

void pwrMgr_targetInit()
{
	
	return;
	
}



Adsppm_Status pwrMgr_checkHvxLockStatus(Adsppm_Status* hvxLockCheckStatus,
												  RMHandleType* pRMHandle)
{
	AsicPowerDomainType pwrDomain = ACMPwr_GetPowerDomain(pRMHandle->pClient->coreId);
		
	// HVX HW bug workaround for 8996 (CR 777974):
	// Return error if HVX power off is requested while the
	// client still has HVX locked.  This can only be
	// checked for synchronous requests since
	// qurt_hvx_get_lock_val() checks the status for only
	// the current thread.  Also we only check for !unknown
	// client classes, to avoid this logic during testing.
	if((pRMHandle->validFlag == RM_InValid) &&
		(pRMHandle->lifecycleState == RELEASED_REQUEST) &&
		(pwrDomain == AsicPowerDomain_Hvx) &&
		(pRMHandle->synchronousRequest == TRUE) &&
		(pRMHandle->pClient->clientClass != MMPM_UNKNOWN_CLIENT_CLASS) &&
		(qurt_hvx_get_lock_val() == QURT_HVX_LOCKED))
	{
		*hvxLockCheckStatus = Adsppm_Status_Failed;
		// Mark request as valid and keep power enabled,
		// since release will fail
		gPowerMgrCtx.pwrDomainState[pwrDomain][gPowerMgrCtx.activeStateIndex] =
			Hal_ClkRgmEnable;
		pRMHandle->validFlag = RM_Valid;
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_WARNING,
			"HVX power was requested off before HVX was unlocked");
	}	

	return Adsppm_Status_Success;


}
												  
Adsppm_Status pwrMgr_preRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;

	if((pwrDomain == AsicPowerDomain_ComputeCore) &&
		(ACM_IsPwrDomainValid(AsicPowerDomain_ComputeCore)) &&
		(ACM_IsClkIdValid (AdsppmClk_AhbI_Hclk )))
	{
		if(Adsppm_Status_Success != ClkRgm_EnableClock(AdsppmClk_AhbI_Hclk , Hal_ClkRgmDisable))
		{
		ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
		                        "AhbI disablement failed while VAPPS ops");
		}
	}

	return result;
}

Adsppm_Status pwrMgr_postRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;

	if((pwrDomain == AsicPowerDomain_ComputeCore) &&
       (ACM_IsPwrDomainValid(AsicPowerDomain_ComputeCore)) &&
       (ACM_IsClkIdValid (AdsppmClk_AhbI_Hclk)))
    {
      if(Adsppm_Status_Success != ClkRgm_EnableClock(AdsppmClk_AhbI_Hclk , Hal_ClkRgmEnable))
      {
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR,
                                "AhbI enablement failed while VAPPS ops");
      }   
    }
#ifdef  HVX_SPL_INST
    if(pwrDomain == AsicPowerDomain_Hvx &&
       (ACM_IsPwrDomainValid(AsicPowerDomain_Hvx)) &&
       (gPowerMgrCtx.pwrDomainState[pwrDomain][gPowerMgrCtx.activeStateIndex] == Hal_ClkRgmEnable))
    {
       char buf1[128];
       __asm__ volatile("vmem(%0) = v0" :: "r" (buf1));
       ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_INFO,
                                "Spl HVX instruction executed");
    }
#endif

	return result;


}

