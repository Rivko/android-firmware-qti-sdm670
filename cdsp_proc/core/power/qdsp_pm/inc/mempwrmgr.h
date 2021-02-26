/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * mempwrmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef MEMPWRMGR_H_
#define MEMPWRMGR_H_

#include "requestmgr.h"


Adsppm_Status MEMPWR_Init(void);

Adsppm_Status MEMPWR_ProcessRequest(coreUtils_Q_Type *pMemPwrReqQ);


#endif /* MEMPWRMGR_H_ */

