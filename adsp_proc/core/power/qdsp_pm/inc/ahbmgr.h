/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * ahbmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef AHBMGR_H_
#define AHBMGR_H_

#include "adsppm.h"


Adsppm_Status AHBM_Init(void);

Adsppm_Status AHBM_ProcessRequest(AdsppmBusBWDataIbAbType *pCurAHBBwData, AdsppmBusBWDataIbAbType *pNonSuppressibleAhbBW, AdsppmBusBWDataIbAbType *pNocBW);

Adsppm_Status AHBM_AdjustAndSetAHBClock(void);

Adsppm_Status AHBM_SetActiveAHBClock(void);

Adsppm_Status AHBM_SetSleepAHBClock(void);

Adsppm_Status AHBM_GetInfo(AdsppmInfoAhbType* ahbInfo);

#endif /* AHBMGR_H_ */

