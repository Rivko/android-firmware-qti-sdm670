/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * core.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef CORE_H_
#define CORE_H_

#include "adsppm.h"


/**
 * @fn Core_EnterSleep - Prepare LPASS resources for power collapse.
 * Currently it will just ramp down AHB clock to the lowest possible frequency while still satisfying
 * non-suppressible requests.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_EnterSleep(void);

/**
 * @fn Core_ExitSleep - Restore LPASS resources after power collapse.
 * Currently it will just ramp up AHB clock to the pre-PC state.
 * TODO: Handle the clock gates as well
 * @return completion status
 */
Adsppm_Status Core_ExitSleep(void);

/**
 * @fn Core_Init - Initialize all components within ADSPPM Core
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status Core_Init(void);

/**
 * Get register programming speed
 * @param Register Programming Match
 * @return Speed in bytes per sec
 */
uint64 ACMBus_GetRegProgSpeed(AdsppmRegProgMatchType);


#endif /* CORE_H_ */

