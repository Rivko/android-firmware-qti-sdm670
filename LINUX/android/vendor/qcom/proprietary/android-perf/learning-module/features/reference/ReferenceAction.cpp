/******************************************************************************
 *   @file    ReferenceAction.cpp
 *   @brief   Action Reference implementation
 *
 *   DESCRIPTION
 *      Reference action module implementing only an output Database container.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "ReferenceAction.h"
#include "DebugLog.h"

#define LOG_TAG_REF_ACTION "RefAction"

ReferenceOutputDb* ReferenceOutputDb::mDbInstance = NULL;

/*
 * ReferenceOutputDb is a singleton pattern - as the OutputDB needs to be
 * accessed by both Algo and Action
 */

ReferenceOutputDb* ReferenceOutputDb::getInstance() {
    DEBUGV(LOG_TAG_REF_ACTION,"getInstance()");
    int err;
    if (!mDbInstance) {
        mDbInstance = new ReferenceOutputDb(0, "ReferenceOutput.db", "Reference");
        if (!mDbInstance) {
            DEBUGV(LOG_TAG_REF_ACTION,"ReferenceOutputDb::getInstance failure");
            return NULL;
        }

        err=mDbInstance->LMDatabaseInit();
        if (err) {
            DEBUGV(LOG_TAG_REF_ACTION,"LMDatabaseInit failed, err=%d", err);
            delete mDbInstance;
            mDbInstance = nullptr;
            return NULL;
        }
    }
    DEBUGV(LOG_TAG_REF_ACTION,"returning DB Instance");
    return mDbInstance;
}

/*
 * Currently ReferenceAction doesn't apply any actions, if we need to apply actions
 * this function must be implemented and used in an action lib which registers
 * directly with perf-hal - check memperfd / PreKillAction for usage
 */
void ReferenceAction::applyAction(const AsyncTriggerPayload& payload) {
    DEBUGV("ReferenceAction","applyAction payload.appName is %s", payload.appName.c_str());
}
