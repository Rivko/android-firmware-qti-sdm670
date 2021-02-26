/******************************************************************************
 *   @file    FeatureState.cpp
 *   @brief   Feature State Machine implementation
 *
 *   DESCRIPTION
 *      FeatureState class provides a state machine implementation for the
 *   Features to use per app. The state machine forces some restrictions on the
 *   allowed state transitions and FeatureState can operate the states of apps
 *   within the allowed transitions.
 *      FeatureState uses LMDB database to make the app states persistent.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "FeatureState.h"
#include "DebugLog.h"

#define LOG_TAG_FS "FeatureState"

using namespace std;

FeatureState::FeatureState(string featureName) : mStateDb(nullptr) {
    mFeatureName = featureName;

    DEBUGV(LOG_TAG_FS, "createStateDb %s", mFeatureName.c_str());
    lmdbError dbError = LMDB_NO_ERROR;

    mStateDb = new AppStateDb(0, mFeatureName + "State.db", mFeatureName.c_str());
    if (mStateDb) {
        dbError = mStateDb->LMDatabaseInit();
        if (LMDB_NO_ERROR != dbError) {
            DEBUGE(LOG_TAG_FS,
                    "%s: Fatal: Error in DBinit - StateDb error=%d", mFeatureName.c_str(), (int )dbError);
            delete mStateDb;
            mStateDb = nullptr;
        }
    }
}

FeatureState::~FeatureState() {
    if(mStateDb) {
        delete mStateDb;
        mStateDb = nullptr;
    }
}

/* StateDb related private helper functions */

/*
 * TODO: Re-organize to take vector<DataRow*> and return the same
 * That would make sure the alloc/dealloc are in same place
 */
DbRowVect FeatureState::createGetUniqueDR(string appName, bool readonly) {
    DbRowVect dbRow;
    if(!mStateDb) {
        DEBUGE(LOG_TAG_FS,"%s: Fatal: StateDb is NULL", mFeatureName.c_str());
        return dbRow;
    }
    LMDBDataRowKey key(appName);

    int totalCount = mStateDb->getCountDR<AppState>(key);

    if (totalCount < 0) {
        return dbRow;
    }

    if(totalCount == 0) {
        /* If the app is not present in DB, Create it if this call is not from read operation*/
        if (!readonly) {
            DEBUGV(LOG_TAG_FS, "%s: App %s not present in db, creating", mFeatureName.c_str(), appName.c_str());
            createRow(key);
        } else {
            return dbRow;
        }
    }

    //createGetUniqueDR() does readAllocateDR() and returns it, caller needs to
    //explicitly take care of deallocateDR()
    dbRow = mStateDb->readAllocateDR<AppState>(key, 0);

    if(dbRow.size() > 1) {
        //multiple rows of same app is present - this shouldn't happen
        //make row unique and reset state
        //Pass by const ref - no copy & doesn't change inside function
        DEBUGV(LOG_TAG_FS, "%s: App %s rows > 1, resetting ", mFeatureName.c_str(), appName.c_str());
        makeRowUnique(dbRow, key);
        //makeRowUnique routine made rows unique but previous readAllocateDR already either
        //wrote into db or deleteDeallocated, so read again from unique records
        dbRow = mStateDb->readAllocateDR<AppState>(key, 0);
    }
    /*
     * By now it's guaranteed that dbRow.size() != 1 is all handled
     * Return the vector - caller needs to check only for dbRow.size() == 1 and perform
     * the operation on dbRow[0]
     * Returning local variable - move semantics
     */
    return dbRow;
}

int FeatureState::createRow(LMDBDataRowKey key) {
    if(!mStateDb) {
        DEBUGE(LOG_TAG_FS,"%s: Fatal: StateDb is NULL", mFeatureName.c_str());
        return -1;
    }
    DbRowVect dbRowCreate = mStateDb->allocateDR<AppState>(1);
    if(dbRowCreate.size() == 1) {
        dbRowCreate[0]->dataRowKey = key;
        dbRowCreate[0]->dataRowPayload = AppState::DATA_REQUIRED;
        DEBUGV(LOG_TAG_FS, "%s: DB: Creating %s app state %d", mFeatureName.c_str(), key.appNm.c_str(), AppState::DATA_REQUIRED);
        mStateDb->writeDeallocateDR<AppState>(dbRowCreate);
    } else {
        mStateDb->deallocateDR<AppState>(dbRowCreate);
    }
    return 0;
}

int FeatureState::makeRowUnique(const DbRowVect& dbRows, LMDBDataRowKey key) {
    if(!mStateDb) {
        DEBUGE(LOG_TAG_FS,"%s: Fatal: StateDb is NULL", mFeatureName.c_str());
        return -1;
    }
    DbRowVect dBRowKeep = DbRowVect(dbRows.begin(), dbRows.begin() + 1);
    if(dBRowKeep.size() != 1) {
        DEBUGE(LOG_TAG_FS,"%s: DbRowVect constructor failed", mFeatureName.c_str());
        return -1;
    }
    dBRowKeep[0]->dataRowKey = key;
    dBRowKeep[0]->dataRowPayload = AppState::DATA_REQUIRED;
    DEBUGV(LOG_TAG_FS, "%s: DB: Creating %s app state %d", mFeatureName.c_str(), key.appNm.c_str(), AppState::DATA_REQUIRED);

    mStateDb->writeDeallocateDR<AppState>(dBRowKeep);

    DbRowVect dBRowsDelete = DbRowVect(dbRows.begin() + 1, dbRows.end());
    mStateDb->deleteDeallocateDR<AppState>(dBRowsDelete);
    return 0;
}

AppState FeatureState::readState(string appName) {
    if(!mStateDb) {
        DEBUGE(LOG_TAG_FS,"%s: Fatal: StateDb is NULL, returning default State", mFeatureName.c_str());
        return AppState::DATA_REQUIRED;
    }

    if (appName.empty()) {
        DEBUGW(LOG_TAG_FS,"warning: appName is NULL, returning default State");
        return AppState::DATA_REQUIRED;
    }

    AppState state = AppState::DATA_REQUIRED;
    /*
     * createGetUniqueDR() does readAllocateDR() and returns it, caller needs to
     * explicitly take care of deallocateDR()
     */
    lock_guard<mutex> lock(mMutex);
    DbRowVect dbRow = createGetUniqueDR(appName, true);
    if(dbRow.size() == 1) {
        state = dbRow[0]->dataRowPayload;
        DEBUGV(LOG_TAG_FS, "%s: DB: %s app state = %d", mFeatureName.c_str(), appName.c_str(), state);
        mStateDb->deallocateDR<AppState>(dbRow);
    }
    return state;
}

int FeatureState::writeState(string appName, AppState target) {
    if(!mStateDb) {
        DEBUGE(LOG_TAG_FS,"%s: Fatal: StateDb is NULL, Can't writeState()!", mFeatureName.c_str());
        return -1;
    }
    if (appName.empty()) {
        DEBUGW(LOG_TAG_FS,"warning: appName is NULL, Can't writeState()");
        return -1;
    }
    lock_guard<mutex> lock(mMutex);
    DbRowVect dbRow = createGetUniqueDR(appName);
    if(dbRow.size() == 1) {
        if(mAllowedTransitions[target].count(dbRow[0]->dataRowPayload) == 1) {
            DEBUGV(LOG_TAG_FS, "%s: DB: Changing %s app state from %d to %d", mFeatureName.c_str(), appName.c_str(), dbRow[0]->dataRowPayload, target);
            dbRow[0]->dataRowPayload = target;
            mStateDb->writeDeallocateDR<AppState>(dbRow);
        } else {
            DEBUGW(LOG_TAG_FS,"%s: State transition not allowed", mFeatureName.c_str());
            mStateDb->deallocateDR<AppState>(dbRow);
            return -1;
        }
    }
    return 0;
}
