/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * core_internal.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yrusakov
 */

#ifndef CORE_INTERNAL_H_
#define CORE_INTERNAL_H_

#include "adsppm.h"


/**
 * @typedef
 * @struct AdsppmCBData - call back data for call back function
 */
/*typedef struct AdsppmCBData{
    int                 eventId; //callback type
    int                 adsppmId; //client id
    struct AdsppmCBData *pNext;
} AdsppmCBDataType;*/

/**
 * @define MAX_EVENT_NUM - max event num for callback
 */
//#define MAX_EVENT_NUM 32

/**
 * @typedef
 * @struct AdsppmCoreCtxType - ADSPPM Global context to hold global information for adsppm.
 */
typedef struct{
    DALSYSSyncHandle  resourceLock[Adsppm_Rsc_Id_Max - 1];
    coreUtils_Q_Type  rscReqQ[Adsppm_Rsc_Id_Max - 1];
    //AdsppmCBDataType  cbEventQue[MAX_EVENT_NUM]; //callback event queue, headEvt and tailEvt points to the head and tail in the eventQue
    //DALSYSEventHandle signalCbEvent; //signal event for callback
    uint32            periodicUseCase; //Indicates if the system running only periodic clients (1) or there are non-periodic clients as well (0)
} AdsppmCoreCtxType;


/**
 * @fn GetAdsppmCoreContext - retrieves pointer to the global ADSPPM context. Inline
 * @return pointer to the global ADSPPM context
 */
AdsppmCoreCtxType *GetAdsppmCoreContext(void);

/**
 * @fn IsPeriodicUseCase - Get periodic use case flag
 * @return 1 if only periodic clients are running
 *         0 in all other cases
 */
uint32 IsPeriodicUseCase(void);

/**
 * @fn SetPeriodicUseCase - Indicate whether the current use case is periodic
 * @param periodic - 1 if only periodic clients are running
 *                   0 in all other cases
 */
void SetPeriodicUseCase(uint32 periodic);


#endif /* CORE_INTERNAL_H_ */

