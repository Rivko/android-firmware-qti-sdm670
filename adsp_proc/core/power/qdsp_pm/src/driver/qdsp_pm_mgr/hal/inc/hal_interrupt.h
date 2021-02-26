/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_interrupt.h
 *
 *  Created on: Jul 4, 2012
 *      Author: yrusakov
 */

#ifndef HAL_INTERRUPT_H_
#define HAL_INTERRUPT_H_

#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"


Adsppm_Status Intr_Init(void);

Adsppm_Status Intr_RegisterIsr(uint32 irq, void* (*pIsr)(void*), void* pCtx);

Adsppm_Status Intr_RegisterEvent(uint32 irq, DALSYSEventHandle  event);

Adsppm_Status Intr_TriggerInterrupt(DalIPCIntInterruptType irq);

Adsppm_Status Intr_AckInterrupt(uint32 irq);

Adsppm_Status Intr_EnableInterrupt(uint32 irq, uint32 enable);


#endif /* HAL_INTERRUPT_H_ */

