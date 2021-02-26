/******************************************************************************
 *   @file    ReferenceMetaMeter.cpp
 *   @brief   MetaMeter Implementation for Reference
 *
 *   DESCRIPTION
 *      Reference Metameter implementation with only mandatory database writing
 *   funtion implemented. Everything else is used from the Base Metameter.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include "ReferenceMetaMeter.h"
#include "DebugLog.h"
#include "ReferenceMeter.h"

#define LOG_TAG_REF_MM "RefMetaMeter"

using namespace std;

bool ReferenceMetaMeter::writeToDataBase(vector<Meter*> meterList) {
    DEBUGV(LOG_TAG_REF_MM,"writeToDataBase() start");
    if(!mReferenceInputDb) {
        DEBUGE(LOG_TAG_REF_MM,"InputDB is NULL, returning");
        return false;
    }
    /* We've only one meter and only one async -
     * if we've multiple meters or multiple asyncs of same meter
     * then the logic needs to be changed accordingly
     */
    if(meterList.size() != 1) {
        DEBUGE(LOG_TAG_REF_MM,"unexpected meterList size %zu, returning", meterList.size());
        return false;
    }

    ReferenceMeter* currMeter = static_cast<ReferenceMeter*>(meterList.front());
    if(!currMeter) {
        DEBUGE(LOG_TAG_REF_MM,"meter object is NULL, returning");
        return false;
    }

    vector<AsyncTriggerPayload> asyncDataList = currMeter->getAsyncUpdates();
    if(asyncDataList.size() != 1) {
        DEBUGE(LOG_TAG_REF_MM,"unexpected asyncDataList size %zu, returning", asyncDataList.size());
        return false;
    }

    DEBUGV(LOG_TAG_REF_MM,"meterList size=%zu asyncDataList size=%zu", meterList.size(), asyncDataList.size());

    /* Allocate memory to keep database rows */
    vector<LMDBDataRow<time_t>*> dbRows = mReferenceInputDb->allocateDR<time_t>(mRefMeterSamples);
    if(dbRows.size() == mRefMeterSamples) {
        /* Populate the values in the database row.
         * Assumes mRefMeterSamples is 1.*/

        /* Set the app name as key in the dbRow */
        string currAppName = asyncDataList.front().appName;
        DEBUGV(LOG_TAG_REF_MM,"Async app name=%s", currAppName.c_str());
        dbRows[0]->dataRowKey.appNm = currAppName;

        /* Set the meter data as payload in the dbRow*/
        dbRows[0]->dataRowPayload = currMeter->getTime();
        DEBUGV(LOG_TAG_REF_MM, "meter->getTime() returned %ld", dbRows[0]->dataRowPayload);

        /* Write to database and de-allocate memory of database rows */
        mReferenceInputDb->writeDeallocateDR<time_t>(dbRows);
        DEBUGV(LOG_TAG_REF_MM,"writeToDataBase() end. Successfully written to DB  ");

        /* Get the count of all the rows */
        LMDBDataRowKey key(currAppName);

        /* getCountDR gets the count of rows matching this key */
        int appSampleCount = mReferenceInputDb->getCountDR<time_t>(key);
        DEBUGV(LOG_TAG_REF_MM,"Checking for DATA_COMPLETE, app = %s, count = %d", key.appNm.c_str(), appSampleCount);
        /* Change state if required samples are available */
        if(appSampleCount >= mFeatureInfo.samplesRequired) {
            DEBUGV(LOG_TAG_REF_MM,"Moving state to DATA_COMPLETE, count = %d", appSampleCount);
            dataComplete(currAppName);
        }
    } else {
        /* Database allocation was not successful for the size we requested, deallocate */
        DEBUGV(LOG_TAG_REF_MM,"writeToDataBase() end. Failed @  dbRows.size() != mRefMeterSamples  ");
        mReferenceInputDb->deallocateDR<time_t>(dbRows);
    }

    return true;
}
