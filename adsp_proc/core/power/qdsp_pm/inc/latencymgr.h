/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * latencymgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef LATENCYMGR_H_
#define LATENCYMGR_H_

#include "requestmgr.h"


Adsppm_Status SLEEP_Init(void);

Adsppm_Status SLEEP_ProcessRequest(coreUtils_Q_Type *pSlpReqQ);


#endif /* LATENCYMGR_H_ */

