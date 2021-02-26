/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "mmpm.h"
#include "adsppm.h"
#include "adsppm_utils.h"
#include "qosmgr.h"
#include "clientmgr.h"
#include "asic.h"


// Offset of QOS_THREAD_DANGER register from the base of the L2 config area
#define QOS_THREAD_DANGER_OFFSET            0x1000
// Shift and mask for 'priority' field of QOS_THREAD_DANGER
#define QOS_THREAD_DANGER_PRIORITY_SHIFT    16
#define QOS_THREAD_DANGER_PRIORITY_MASK     0xff

// All state info for the QoS Configuration Manager
typedef struct
{
    DALSYSSyncHandle lock;
    uint32 activeClientClasses;              // active use case class mix
    volatile uint32* qos_thread_danger_addr; // address of QOS_THREAD_DANGER reg
} AdsppmQosConfigManagerType;

static AdsppmQosConfigManagerType gAdsppmQosConfigManager;

///////////////////////////////////////////////////////////////////////////////
// Static functions 
///////////////////////////////////////////////////////////////////////////////

// Check for change in active client classes, and if there is a change, set the
// value of the 'priority' field of QOS_THREAD_DANGER to 1 or 0
//  1 - high/mixed priority (audio/voice active)
//  0 - low priority        (audio/voice not active)
static void qcmOnClientManagerUpdate(AdsppmClientManagerEventId eventId,
    void* param)
{

    AdsppmQosConfigManagerType* self = (AdsppmQosConfigManagerType*) param;

    adsppm_lock(self->lock);

    if((eventId == CM_ACTIVE_CLASS_CHANGE_EVENT) &&
        (self->qos_thread_danger_addr != NULL))
    {
        uint32 currentClasses = CM_GetActiveUseCaseClasses();
        if(currentClasses != self->activeClientClasses)
        {
            uint32 priority_val = 0; // indicates 'low priority'
            boolean avActive = FALSE; // true if audio/voice class active
            uint32 scratch = 0;

            self->activeClientClasses = currentClasses;

            if((self->activeClientClasses & MMPM_AUDIO_CLIENT_CLASS) ||
                (self->activeClientClasses & MMPM_VOICE_CLIENT_CLASS))
            {
                priority_val = 1; // indicates 'high or mixed priority'
                avActive = TRUE;
            }

            ADSPPM_LOG_PRINTF_2(ADSPPM_LOG_LEVEL_INFO,
                "detected %saudio/voice client class activity: "
                "setting priority field of qos_thread_danger to %d",
                avActive ? "" : "no ", priority_val);

            // Read current value and mask out priority field
            scratch = (*(self->qos_thread_danger_addr)) &
                (~(QOS_THREAD_DANGER_PRIORITY_MASK <<
                QOS_THREAD_DANGER_PRIORITY_SHIFT));
            // Modify priority field and update register
            *(self->qos_thread_danger_addr) =
                (priority_val << QOS_THREAD_DANGER_PRIORITY_SHIFT) | scratch;

        }
    }

    adsppm_unlock(self->lock);

}

///////////////////////////////////////////////////////////////////////////////
// Public functions 
///////////////////////////////////////////////////////////////////////////////

Adsppm_Status QCM_Init(void)
{

    AdsppmQosConfigManagerType* self = &gAdsppmQosConfigManager;
    AsicHwioRegRangeType* l2ConfigRange = NULL;

    memset(self, 0, sizeof(AdsppmQosConfigManagerType));

    if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
        &self->lock, NULL))
    {
        return Adsppm_Status_Failed;
    }

    l2ConfigRange = ACM_GetL2ConfigRegRange();
    self->qos_thread_danger_addr = ((l2ConfigRange != NULL) &&
        (l2ConfigRange->baseAddr != 0)) ? 
        ((uint32*) (l2ConfigRange->baseAddr + QOS_THREAD_DANGER_OFFSET)) : NULL;

    self->activeClientClasses = MMPM_UNKNOWN_CLIENT_CLASS;

    if(self->qos_thread_danger_addr != NULL)
    {
        // Client manager must already be initialized
        CM_RegisterEvent(CM_ACTIVE_CLASS_CHANGE_EVENT,
            qcmOnClientManagerUpdate, (void*) self);
    }

    return Adsppm_Status_Success;

}

