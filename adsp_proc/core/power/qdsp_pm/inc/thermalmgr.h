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

#ifndef THERMALMGR_H_
#define THERMALMGR_H_

#include "requestmgr.h"


Adsppm_Status THERMAL_Init(void);

Adsppm_Status THERMAL_ProcessRequest(coreUtils_Q_Type *pThermalReqQ);


#endif /* THERMALMGR_H_ */

