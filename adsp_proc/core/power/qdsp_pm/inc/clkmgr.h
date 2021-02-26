/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * clkmgr.h
 *
 *  Created on: Aug 19, 2011
 *      Author: Shiju
 */

#ifndef CLKMGR_H_
#define CLKMGR_H_

#include "requestmgr.h"


Adsppm_Status CLK_Init(void);

Adsppm_Status CLK_ProcessRequest(coreUtils_Q_Type *pClkReqQ);

Adsppm_Status CLKDomain_ProcessRequest(coreUtils_Q_Type *pClkDomainReqQ);

Adsppm_Status CLK_GetInfo(AdsppmInfoClkFreqType *pClockInfo);


#endif /* CLKMGR_H_ */

