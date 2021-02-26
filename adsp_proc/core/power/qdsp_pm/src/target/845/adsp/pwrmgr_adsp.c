#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "pwrmgr.h"
#include "hal_clkrgm.h"
#include "requestmgr.h"






extern powerMgrCtxType gPowerMgrCtx;


void pwrMgr_targetInit()
{

	return;
    
}




Adsppm_Status pwrMgr_checkHvxLockStatus(Adsppm_Status* hvxLockCheckStatus,
												  RMHandleType* pRMHandle)
{
		

	return Adsppm_Status_Success;


}
												  
Adsppm_Status pwrMgr_preRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;

	

	return result;
}


Adsppm_Status pwrMgr_postRequestPower(AsicPowerDomainType pwrDomain){

	Adsppm_Status result = Adsppm_Status_Success;

	
	

	return result;

}

