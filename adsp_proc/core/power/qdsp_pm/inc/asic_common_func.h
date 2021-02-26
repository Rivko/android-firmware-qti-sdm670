/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * asic_common_func.h - contains ASIC configuration definitions required for COMMON_FUNC
 *      
 */

#ifndef ASIC_COMMON_FUNC_H_
#define ASIC_COMMON_FUNC_H_

#include "adsppm.h"
#include "asic_internal.h"


Adsppm_Status getBWFromMips(AdsppmMIPSToBWAggregateType *pMipsAggregateData);

Adsppm_Status getClockFromMips(AdsppmMIPSToClockAggregateType *pMipsAggregateData);

Adsppm_Status getClockFromBW(uint32 *pClock, AdsppmBusBWDataIbAbType *pAHBBwData);

Adsppm_Status aggregateBusBW(AdsppmBusBWDataIbAbType *pAggregateBwIbAbValue, AdsppmBusBWDataType *pBwValue);


#endif /* ASIC_COMMON_FUNC_H_ */

