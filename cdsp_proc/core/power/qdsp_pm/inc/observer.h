/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/


#ifndef _OBSERVER_H_ 
#define _OBSERVER_H_ 

#include "adsppm.h"
#include "coreUtils.h"

// Observer notification handler type
typedef void (*AdsppmEventHandler)(uint32 eventId, void* param);

// All state info for observers
typedef struct
{
    DALSYSSyncHandle lock;
    coreUtils_Q_Type observers;
} AdsppmObserverStateType;

// Initialized observer state
Adsppm_Status observer_init(AdsppmObserverStateType* observerState);

// Add a new observer to the list.  The observer is described by the event ID,
// the handler function to be called for notifications, and observer's
// parameter to be passed to the handler function.
Adsppm_Status observer_add(AdsppmObserverStateType* observerState,
    uint32 eventId, AdsppmEventHandler handler, void* param);

// Remove a previously added observer from the list
Adsppm_Status observer_remove(AdsppmObserverStateType* observerState,
    uint32 eventId, AdsppmEventHandler handler);

// Notify all observers of the event specified by eventId
Adsppm_Status observer_notify(AdsppmObserverStateType* observerState,
    uint32 eventId);

#endif /* _OBSERVER_H_ */

