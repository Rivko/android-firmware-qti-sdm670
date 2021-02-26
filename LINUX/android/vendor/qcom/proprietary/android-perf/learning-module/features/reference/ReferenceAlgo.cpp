/******************************************************************************
 *   @file    ReferenceAlgo.cpp
 *   @brief   Algorithm Reference class implementation
 *
 *   DESCRIPTION
 *      Reference algo class implementing a reference algorithm.
 *
 *---------------------------------------------------------------------------
 *    Copyright (c) 2017 Qualcomm Technologies, Inc.
 *    All Rights Reserved.
 *    Confidential and Proprietary - Qualcomm Technologies, Inc.
 *---------------------------------------------------------------------------
 *******************************************************************************/

#include <thread>
#include "DebugLog.h"
#include "ReferenceAlgo.h"

#define LOG_TAG_REF_ALGO "RefAlgo"

using namespace std;

void ReferenceAlgo::runAlgo(const atomic_bool& interrupt) {
    DEBUGV(LOG_TAG_REF_ALGO, "ReferenceAlgo::runAlgo");

    if(!mReferenceInputDb || !mActionOutputDb) {
        DEBUGE(LOG_TAG_REF_ALGO, "DB handle is NULL, returning");
        return;
    }

    /* Get all the app names */
    auto appNameList = mReferenceInputDb->getTxtColUniqList<time_t>(TXTCOL_APPNAME);

    /* For each app, check if the state is valid and run the algo */
    for(string currAppName : appNameList) {
        if(interrupt) {
            /*
             * Check interrupt from main thread before running algo on each app
             * It's recommended to check the interrupt at least once every second
             * to avoid degradation of UX on user interaction.
             */

            break;
        }
        if(isValidAlgoState(currAppName)) {
            DEBUGV(LOG_TAG_REF_ALGO, "Running algo for %s ", currAppName.c_str());

            LMDBDataRowKey key(currAppName);
            /*
             * getCountDR gets the count of rows matching this key
             * useful if you need to take some decision based on count
             */
            int totalCount = mReferenceInputDb->getCountDR<time_t>(key);

            /*
             * readAllocateDR creates objects for dbRows and returns a vector of
             * pointers to objects populated with rows matching key
             */
            vector<LMDBDataRow<time_t>*> dbRows = mReferenceInputDb->readAllocateDR<time_t>(key, 0);
            int readCount = dbRows.size();

            DEBUGV(LOG_TAG_REF_ALGO,
                    "ReferenceAlgo::runAlgo (totalCount , readCount) = (%d, %d) ",
                    totalCount, readCount);

            time_t prevMeterData = 0;
            vector<double> timeBetweenLaunches;
            for (int iter = 0; iter < readCount; iter++) {
                time_t currMeterData = dbRows[iter]->dataRowPayload;
                DEBUGV(LOG_TAG_REF_ALGO, "runAlgo, meterData[%d]=%ld", iter, currMeterData);
                if(prevMeterData != 0) {
                    double currTimeBetweenLaunches = difftime(currMeterData, prevMeterData);
                    timeBetweenLaunches.emplace_back(currTimeBetweenLaunches);
                    DEBUGV(LOG_TAG_REF_ALGO, "time between launches=%f", currTimeBetweenLaunches);
                }
                prevMeterData = currMeterData;
            }

            double sumTimes = 0;
            for(auto currTime : timeBetweenLaunches) {
                sumTimes += currTime;
            }
            double averageTimeBetweenLaunches = sumTimes / timeBetweenLaunches.size();
            DEBUGV(LOG_TAG_REF_ALGO, "Writing to O/p DB, %s, %f", currAppName.c_str(), averageTimeBetweenLaunches);
            writeToOutputDb(currAppName, averageTimeBetweenLaunches);

            /*
             * deallocateDR frees the objects created for dbRows without doing anything
             * to the Database.
             * Alternately, deleteDeallocateDR can be used to delete some rows by manipulating
             * the vector and passing the vector to deleteDeallocateDR
             */
            mReferenceInputDb->deallocateDR<time_t>(dbRows);

            /* Algo has done inital learning, change the state to evaluation required */
            evalRequired(currAppName);
            /*
             * Debug addition to simulate time taken by the algo so that interrupting
             * the algo can be tested */
            this_thread::sleep_for(1s);
//            evalDone(appNameKey);
        }
    }
}

void ReferenceAlgo::writeToOutputDb(string appName, double timeBetweenLaunches) {
    if(!mActionOutputDb) {
        DEBUGE(LOG_TAG_REF_ALGO, "DB handle is NULL, returning");
        return;
    }
    /* Allocate memory to keep database rows */
    vector<LMDBDataRow<ReferenceOutput>*> dbRows = mActionOutputDb->allocateDR<ReferenceOutput>(1);
    if(dbRows.size() == 1) {
        /* Populate the values in the database row */
        LMDBDataRowKey* dataRowKeyPtr = &dbRows[0]->dataRowKey;
        ReferenceOutput* payload = &dbRows[0]->dataRowPayload;

        /* Set the app name as key */
        DEBUGV(LOG_TAG_REF_ALGO,"Async app name=%s", appName.c_str());
        dataRowKeyPtr->appNm = appName;
        DEBUGV(LOG_TAG_REF_ALGO,"DB Row app name=%s", dataRowKeyPtr->appNm.c_str());

        /* Set the output data */
        payload->timeBetweenLaunches = timeBetweenLaunches;
        DEBUGV(LOG_TAG_REF_ALGO, "writing timeBetweenLaunches %f", payload->timeBetweenLaunches);

        /* Write to database and de-allocate memory of database rows */
        mActionOutputDb->writeDeallocateDR<ReferenceOutput>(dbRows);
        DEBUGV(LOG_TAG_REF_ALGO,"Successfully written output to DB  ");

    } else {
        /* Database allocation was not successful for the size we requested, deallocate */
        DEBUGV(LOG_TAG_REF_ALGO,"writeToDataBase() end. Failed, allocateDR not successful");
        mActionOutputDb->deallocateDR<ReferenceOutput>(dbRows);
    }
}
