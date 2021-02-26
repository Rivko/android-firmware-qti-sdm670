/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_sleep.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_SLEEP_H_
#define HAL_SLEEP_H_

#include "adsppm.h"


#define HAL_SLEEP_LATENCY_DONT_CARE (-1)

AdsppmStatusType Slp_Init(void (*sleep_enter_func)(uint32), void (*sleep_exit_func)(void));

AdsppmStatusType Slp_RequestLatency(uint32 latency);

AdsppmStatusType Slp_RequestRpmLpm(uint32 status);

AdsppmStatusType Slp_Release(void);


#endif /* HAL_SLEEP_H_ */

