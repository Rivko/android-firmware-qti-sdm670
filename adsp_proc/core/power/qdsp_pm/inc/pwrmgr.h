/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * pwrmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef PWRMGR_H_
#define PWRMGR_H_

#include "requestmgr.h"
#include "adsppm.h"
#include "adsppm_utils.h"
#include "asic.h"
#include "DALSys.h"
#include "hal_clkrgm.h"

#define XO_FREQ 19200000

typedef struct
{
    DALSYSSyncHandle            lock;
    Hal_ClkRgmEnableDisableType pwrDomainState[AsicPowerDomain_EnumMax][2];
    uint32                      activeStateIndex;
	uint32						prevAhbFreq;
    npa_client_handle           uSleepHandle; //to vote on dsp pm usleep node
} powerMgrCtxType;



Adsppm_Status PWR_Init(void);

Adsppm_Status PWR_ProcessRequest(coreUtils_Q_Type *pPwrReqQ);

void pwrMgr_targetInit(void);

Adsppm_Status pwrMgr_checkHvxLockStatus(Adsppm_Status* hvxLockCheckStatus,
												  RMHandleType* pRMHandle);

Adsppm_Status pwrMgr_preRequestPower(AsicPowerDomainType pwrDomain);


Adsppm_Status pwrMgr_postRequestPower(AsicPowerDomainType pwrDomain);

boolean pwrMgr_lpassCoreEnabled(void);

#endif /* PWRMGR_H_ */

